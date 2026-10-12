#include "handheld_config.h"

#include "battery.h"
#include "haptics.h"
#include "input.h"
#include "led.h"
#include "ota.h"
#include "transport.h"

#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "HANDHELD";

void app_main(void)
{
    ESP_LOGI(TAG, "GAMR %s grip scaffold",
             HANDHELD_SIDE == HANDHELD_SIDE_LEFT ? "left" : "right");

    /* Each module advertises its API, but hardware support is not yet present. */
    ESP_LOGW(TAG, "Component implementations are pending; controller is inactive");
}
