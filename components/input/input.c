#include "input.h"

#include <stddef.h>

esp_err_t input_init(void)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t input_read(handheld_report_t *report)
{
    if (report == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_ERR_NOT_SUPPORTED;
}
