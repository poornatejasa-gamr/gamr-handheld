#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

typedef struct {
    uint16_t voltage_mv;
    uint8_t percentage;
    bool charging;
    bool full;
} battery_status_t;

esp_err_t battery_init(void);
esp_err_t battery_read_status(battery_status_t *status);
