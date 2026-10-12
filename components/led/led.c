#include "led.h"

esp_err_t led_init(void)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t led_set_state(led_state_t state)
{
    if (state < LED_STATE_OFF || state > LED_STATE_ERROR) {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_ERR_NOT_SUPPORTED;
}

void led_update(void)
{
}
