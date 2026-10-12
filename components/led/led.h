#pragma once

#include "esp_err.h"

typedef enum {
    LED_STATE_OFF,
    LED_STATE_PAIRING,
    LED_STATE_CONNECTED,
    LED_STATE_LOW_BATTERY,
    LED_STATE_CHARGING,
    LED_STATE_OTA,
    LED_STATE_ERROR,
} led_state_t;

esp_err_t led_init(void);
esp_err_t led_set_state(led_state_t state);
void led_update(void);
