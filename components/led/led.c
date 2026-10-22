#include "led.h"

#include <stdbool.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LED_GREEN_PIN GPIO_NUM_16
#define LED_RED_PIN GPIO_NUM_17
#define LED_BLINK_MS 500

static const char *TAG = "LED";
static bool led_initialized;
static led_state_t led_state = LED_STATE_OFF;

static void led_write(bool green, bool red)
{
    gpio_set_level(LED_GREEN_PIN, green ? 1 : 0);
    gpio_set_level(LED_RED_PIN, red ? 1 : 0);
}

esp_err_t led_init(void)
{
    if (led_initialized) {
        return ESP_OK;
    }
    const gpio_config_t config = {
        .pin_bit_mask = (1ULL << LED_GREEN_PIN) | (1ULL << LED_RED_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    esp_err_t err = gpio_config(&config);
    if (err != ESP_OK) {
        return err;
    }
    led_write(false, false);
    led_initialized = true;
    ESP_LOGI(TAG, "Green LED=GPIO16, red LED=GPIO17 (active high)");
    return ESP_OK;
}

esp_err_t led_set_state(led_state_t state)
{
    if (state < LED_STATE_OFF || state > LED_STATE_ERROR) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!led_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    led_state = state;
    led_update();
    return ESP_OK;
}

void led_update(void)
{
    if (!led_initialized) {
        return;
    }
    const bool blink_on = ((xTaskGetTickCount() / pdMS_TO_TICKS(LED_BLINK_MS)) & 1U) == 0;
    switch (led_state) {
        case LED_STATE_OFF:
            led_write(false, false);
            break;
        case LED_STATE_READY:
        case LED_STATE_CHARGING:
            led_write(blink_on, false);
            break;
        case LED_STATE_PAIRING:
        case LED_STATE_LOW_BATTERY:
            led_write(false, blink_on);
            break;
        case LED_STATE_CONNECTED:
            led_write(true, false);
            break;
        case LED_STATE_OTA:
            led_write(blink_on, !blink_on);
            break;
        case LED_STATE_ERROR:
            led_write(false, true);
            break;
    }
}
