#include "haptics.h"

#include <stddef.h>

esp_err_t haptics_init(void)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t haptics_play(const haptics_effect_t *effect)
{
    if (effect == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_ERR_NOT_SUPPORTED;
}

void haptics_update(void)
{
}
