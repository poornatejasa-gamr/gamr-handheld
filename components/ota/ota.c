#include "ota.h"

esp_err_t ota_init(void)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t ota_begin(size_t image_size, const uint8_t sha256[OTA_SHA256_SIZE])
{
    if (image_size == 0 || sha256 == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t ota_write(const uint8_t *data, size_t length)
{
    if (data == NULL || length == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t ota_finish(void)
{
    return ESP_ERR_NOT_SUPPORTED;
}

void ota_abort(void)
{
}

ota_state_t ota_get_state(void)
{
    return OTA_STATE_IDLE;
}
