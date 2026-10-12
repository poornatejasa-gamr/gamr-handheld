#include "handheld_link.h"
#include "handheld_wire.h"

#include <string.h>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#define LINK_TIMEOUT_MS 1000

static const char *TAG = "HANDHELD_LINK";
static const uint8_t broadcast_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static QueueHandle_t ack_queue;
static handheld_side_t grip_side;
static uint16_t next_sequence;
static TickType_t last_ack_tick;
static bool initialized;

static void receive_cb(const esp_now_recv_info_t *info, const uint8_t *data, int length)
{
    if (info == NULL || data == NULL || length != sizeof(handheld_wire_ack_t)) return;
    handheld_wire_ack_t ack;
    memcpy(&ack, data, sizeof(ack));
    if (ack.magic != HANDHELD_WIRE_MAGIC || ack.version != HANDHELD_WIRE_VERSION ||
        ack.type != HANDHELD_WIRE_ACK || ack.side != grip_side) return;
    (void)xQueueSend(ack_queue, &ack, 0);
}

esp_err_t handheld_link_init(handheld_side_t side)
{
    if (side != HANDHELD_SIDE_LEFT && side != HANDHELD_SIDE_RIGHT) return ESP_ERR_INVALID_ARG;
    if (initialized) return ESP_OK;
    grip_side = side;
    ack_queue = xQueueCreate(8, sizeof(handheld_wire_ack_t));
    if (ack_queue == NULL) return ESP_ERR_NO_MEM;

    esp_err_t err = nvs_flash_init();
    if (err != ESP_OK) return err;
    ESP_ERROR_CHECK(esp_netif_init());
    err = esp_event_loop_create_default();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) return err;
    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_channel(HANDHELD_WIRE_CHANNEL, WIFI_SECOND_CHAN_NONE));
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_recv_cb(receive_cb));

    esp_now_peer_info_t peer = {0};
    memcpy(peer.peer_addr, broadcast_mac, sizeof(broadcast_mac));
    peer.channel = HANDHELD_WIRE_CHANNEL;
    peer.ifidx = WIFI_IF_STA;
    ESP_ERROR_CHECK(esp_now_add_peer(&peer));
    initialized = true;
    ESP_LOGI(TAG, "%s grip ESP-NOW on channel %u",
             side == HANDHELD_SIDE_LEFT ? "Left" : "Right", HANDHELD_WIRE_CHANNEL);
    return ESP_OK;
}

esp_err_t handheld_link_send_report(const handheld_report_t *report)
{
    if (report == NULL) return ESP_ERR_INVALID_ARG;
    if (!initialized) return ESP_ERR_INVALID_STATE;
    const handheld_wire_report_t packet = {
        .magic = HANDHELD_WIRE_MAGIC,
        .version = HANDHELD_WIRE_VERSION,
        .type = HANDHELD_WIRE_REPORT,
        .side = (uint8_t)grip_side,
        .sequence = next_sequence++,
        .stick_x = report->stick_x,
        .stick_y = report->stick_y,
        .trigger = report->trigger,
        .buttons = (report->bumper ? HANDHELD_BUTTON_BUMPER : 0) |
                   (report->stick_click ? HANDHELD_BUTTON_STICK_CLICK : 0),
    };
    return esp_now_send(broadcast_mac, (const uint8_t *)&packet, sizeof(packet));
}

bool handheld_link_pop_ack(uint16_t *sequence, bool *accepted)
{
    handheld_wire_ack_t ack;
    if (ack_queue == NULL || xQueueReceive(ack_queue, &ack, 0) != pdTRUE) return false;
    last_ack_tick = xTaskGetTickCount();
    if (sequence != NULL) *sequence = ack.sequence;
    if (accepted != NULL) *accepted = ack.accepted != 0;
    return true;
}

bool handheld_link_is_connected(void)
{
    return initialized && last_ack_tick != 0 &&
           xTaskGetTickCount() - last_ack_tick < pdMS_TO_TICKS(LINK_TIMEOUT_MS);
}

bool handheld_link_pop_feedback(handheld_feedback_t *feedback)
{
    (void)feedback;
    return false;
}
