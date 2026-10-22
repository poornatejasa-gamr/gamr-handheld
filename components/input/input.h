#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

/* One grip's controls. The side setting determines L/R names on the mat. */
typedef struct {
    int16_t stick_x;
    int16_t stick_y;
    int16_t trigger; /* Digital hardware: 0 or INT16_MAX; analog later. */
    bool bumper;
    bool stick_click;
} handheld_report_t;

esp_err_t input_init(void);
esp_err_t input_read(handheld_report_t *report);
