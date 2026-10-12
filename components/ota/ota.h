#pragma once

#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"

#define OTA_SHA256_SIZE 32

typedef enum {
    OTA_STATE_IDLE,
    OTA_STATE_RECEIVING,
    OTA_STATE_VERIFYING,
    OTA_STATE_READY,
    OTA_STATE_FAILED,
} ota_state_t;

esp_err_t ota_init(void);
esp_err_t ota_begin(size_t image_size, const uint8_t sha256[OTA_SHA256_SIZE]);
esp_err_t ota_write(const uint8_t *data, size_t length);
esp_err_t ota_finish(void);
void ota_abort(void);
ota_state_t ota_get_state(void);
