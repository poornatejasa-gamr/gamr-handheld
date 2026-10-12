#include "handheld_config.h"

#include "input.h"
#include "led.h"
#include "handheld_link.h"

#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "HANDHELD";

#define INPUT_POLL_MS 20
#define INPUT_LOG_MS 500

void app_main(void)
{
    ESP_LOGI(TAG, "GAMR %s grip input bring-up",
             HANDHELD_SIDE == HANDHELD_SIDE_LEFT ? "left" : "right");

    ESP_ERROR_CHECK(led_init());
    ESP_ERROR_CHECK(handheld_link_init(HANDHELD_SIDE));
    ESP_ERROR_CHECK(input_init());
    ESP_ERROR_CHECK(led_set_state(LED_STATE_READY));

    const bool left = HANDHELD_SIDE == HANDHELD_SIDE_LEFT;
    TickType_t last_log = xTaskGetTickCount();
    uint16_t last_ack_sequence = 0;
    bool last_ack_accepted = false;
    bool have_ack = false;

    while (true) {
        handheld_report_t report = {0};
        const esp_err_t err = input_read(&report);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Input read failed: %s", esp_err_to_name(err));
            ESP_ERROR_CHECK(led_set_state(LED_STATE_ERROR));
        } else {
            const esp_err_t send_err = handheld_link_send_report(&report);
            if (send_err != ESP_OK) {
                ESP_LOGW(TAG, "ESP-NOW send failed: %s", esp_err_to_name(send_err));
            }
        }
        uint16_t sequence;
        bool accepted;
        while (handheld_link_pop_ack(&sequence, &accepted)) {
            last_ack_sequence = sequence;
            last_ack_accepted = accepted;
            have_ack = true;
        }
        if (!handheld_link_is_connected()) have_ack = false;
        if (err == ESP_OK) {
            ESP_ERROR_CHECK(led_set_state(handheld_link_is_connected() ?
                                          LED_STATE_CONNECTED : LED_STATE_READY));
        }
        if (xTaskGetTickCount() - last_log >= pdMS_TO_TICKS(INPUT_LOG_MS)) {
            ESP_LOGI(TAG, "%c stick=(%d,%d) %s=%d %s=%d %s=%d link=%s ack=%u %s",
                     left ? 'L' : 'R', (int)report.stick_x, (int)report.stick_y,
                     left ? "LT" : "RT", (int)report.trigger,
                     left ? "LB" : "RB", (int)report.bumper,
                     left ? "L3" : "R3", (int)report.stick_click,
                     handheld_link_is_connected() ? "up" : "down",
                     (unsigned)last_ack_sequence,
                     !have_ack ? "none" : last_ack_accepted ? "accepted" : "ignored");
            last_log = xTaskGetTickCount();
        }
        led_update();
        vTaskDelay(pdMS_TO_TICKS(INPUT_POLL_MS));
    }
}
