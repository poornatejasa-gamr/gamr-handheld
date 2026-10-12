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
} handheld_feedback_t;

esp_err_t handheld_link_init(handheld_side_t side);
esp_err_t handheld_link_send_report(const handheld_report_t *report);
bool handheld_link_is_connected(void);
/* ACK means the mat accepted the report into gamepad state, not that the TV received it. */
bool handheld_link_pop_ack(uint16_t *sequence, bool *accepted);
bool handheld_link_pop_feedback(handheld_feedback_t *feedback);
