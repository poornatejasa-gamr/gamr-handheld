#include "handheld_config.h"

#include "input.h"
#include "led.h"
#include "transport.h"

#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "HANDHELD";

#define INPUT_LOG_MS 500

void app_main(void)
{
    ESP_LOGI(TAG, "GAMR %s grip input bring-up",
             HANDHELD_SIDE == HANDHELD_SIDE_LEFT ? "left" : "right");

    ESP_ERROR_CHECK(led_init());
    ESP_ERROR_CHECK(input_init());
    ESP_ERROR_CHECK(led_set_state(LED_STATE_READY));

    const bool left = HANDHELD_SIDE == HANDHELD_SIDE_LEFT;

    while (true) {
        handheld_report_t report = {0};
        const esp_err_t err = input_read(&report);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Input read failed: %s", esp_err_to_name(err));
            ESP_ERROR_CHECK(led_set_state(LED_STATE_ERROR));
        } else {
            ESP_LOGI(TAG, "%c stick=(%d,%d) %s=%d %s=%d %s=%d",
                     left ? 'L' : 'R', (int)report.stick_x, (int)report.stick_y,
                     left ? "LT" : "RT", (int)report.trigger,
                     left ? "LB" : "RB", (int)report.bumper,
                     left ? "L3" : "R3", (int)report.stick_click);
        }
        led_update();
        vTaskDelay(pdMS_TO_TICKS(INPUT_LOG_MS));
    }
}
