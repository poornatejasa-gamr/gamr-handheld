#include "battery.h"

#include <stddef.h>

esp_err_t battery_init(void){
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t battery_read_status(battery_status_t *status){
    if (status == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_ERR_NOT_SUPPORTED;
}
