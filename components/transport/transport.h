#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"
#include "input.h"

typedef enum {
    HANDHELD_SIDE_LEFT = 0,
    HANDHELD_SIDE_RIGHT = 1,
} handheld_side_t;

/* A command from the mat; payload semantics belong to the receiving component. */
typedef struct {
    uint8_t effect;
    uint8_t strength;
    uint16_t duration_ms;
} transport_feedback_t;

esp_err_t transport_init(handheld_side_t side);
esp_err_t transport_send_report(const handheld_report_t *report);
bool transport_is_connected(void);
bool transport_pop_feedback(transport_feedback_t *feedback);
