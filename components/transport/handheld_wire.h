#pragma once

#include <stdint.h>

/* Shared on-air layout. Keep identical in both firmware repositories. */
#define HANDHELD_WIRE_MAGIC 0xA7
#define HANDHELD_WIRE_VERSION 1
#define HANDHELD_WIRE_REPORT 1
#define HANDHELD_WIRE_ACK 2
#define HANDHELD_WIRE_CHANNEL 1

#define HANDHELD_BUTTON_BUMPER (1U << 0)
#define HANDHELD_BUTTON_STICK_CLICK (1U << 1)

typedef struct __attribute__((packed)) {
    uint8_t magic;
    uint8_t version;
    uint8_t type;
    uint8_t side; /* 0 left, 1 right */
    uint16_t sequence;
    int16_t stick_x;
    int16_t stick_y;
    int16_t trigger;
    uint8_t buttons;
} handheld_wire_report_t;

typedef struct __attribute__((packed)) {
    uint8_t magic;
    uint8_t version;
    uint8_t type;
    uint8_t side;
    uint16_t sequence;
    uint8_t accepted; /* Mat applied the report to its gamepad state. */
} handheld_wire_ack_t;

_Static_assert(sizeof(handheld_wire_report_t) == 13, "Handheld report wire size changed");
_Static_assert(sizeof(handheld_wire_ack_t) == 7, "Handheld ACK wire size changed");
