#include "transport.h"

#include <stddef.h>

esp_err_t transport_init(handheld_side_t side)
{
    if (side != HANDHELD_SIDE_LEFT && side != HANDHELD_SIDE_RIGHT) {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t transport_send_report(const handheld_report_t *report)
{
    if (report == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_ERR_NOT_SUPPORTED;
}

bool transport_is_connected(void)
{
    return false;
}

bool transport_pop_feedback(transport_feedback_t *feedback)
{
    (void)feedback;
    return false;
}
