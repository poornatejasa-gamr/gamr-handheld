#pragma once

#include <stdint.h>

#include "esp_err.h"

typedef struct {
    uint8_t strength;
    uint16_t duration_ms;
} haptics_effect_t;

esp_err_t haptics_init(void);
esp_err_t haptics_play(const haptics_effect_t *effect);
void haptics_update(void);
