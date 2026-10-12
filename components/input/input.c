#include "input.h"

#include <stddef.h>

#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define STICK_X_CHANNEL ADC_CHANNEL_0 /* GPIO1 */
#define STICK_Y_CHANNEL ADC_CHANNEL_1 /* GPIO2 */
#define BUMPER_PIN GPIO_NUM_7
#define TRIGGER_PIN GPIO_NUM_8
#define STICK_CLICK_PIN GPIO_NUM_9
#define ADC_MAX 4095
#define CENTER_SAMPLES 32
#define READ_SAMPLES 8
#define DEAD_ZONE 160 /* ADC counts, about 4% of stick travel */

static adc_oneshot_unit_handle_t adc;
static int center_x;
static int center_y;

static esp_err_t read_axis(adc_channel_t channel, int samples, int *value)
{
    int sum = 0;
    for (int i = 0; i < samples; i++) {
        int raw;
        esp_err_t err = adc_oneshot_read(adc, channel, &raw);
        if (err != ESP_OK) {
            return err;
        }
        sum += raw;
    }
    *value = sum / samples;
    return ESP_OK;
}

static int16_t map_axis(int raw, int center)
{
    int offset = raw - center;
    int distance = offset < 0 ? -offset : offset;
    if (distance <= DEAD_ZONE) {
        return 0;
    }
    int range = offset < 0 ? center : ADC_MAX - center;
    int value = (distance - DEAD_ZONE) * INT16_MAX / (range - DEAD_ZONE);
    if (value > INT16_MAX) value = INT16_MAX;
    return (int16_t)(offset < 0 ? -value : value);
}

esp_err_t input_init(void){
    if (adc != NULL) {
        return ESP_OK;
    }

    const gpio_config_t button_config = {
        .pin_bit_mask = (1ULL << BUMPER_PIN) | (1ULL << TRIGGER_PIN) |
                        (1ULL << STICK_CLICK_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    esp_err_t err = gpio_config(&button_config);
    if (err != ESP_OK) {
        return err;
    }

    const adc_oneshot_unit_init_cfg_t unit_config = {
        .unit_id = ADC_UNIT_1,
    };
    err = adc_oneshot_new_unit(&unit_config, &adc);
    if (err != ESP_OK) {
        return err;
    }

    const adc_oneshot_chan_cfg_t channel_config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };
    err = adc_oneshot_config_channel(adc, STICK_X_CHANNEL, &channel_config);
    if (err == ESP_OK) {
        err = adc_oneshot_config_channel(adc, STICK_Y_CHANNEL, &channel_config);
    }
    if (err != ESP_OK) {
        adc_oneshot_del_unit(adc);
        adc = NULL;
        return err;
    }

    /* Let the joystick supply settle, then calibrate with the stick untouched. */
    vTaskDelay(pdMS_TO_TICKS(2000));
    int sum_x = 0, sum_y = 0;
    for (int i = 0; i < CENTER_SAMPLES; i++) {
        int x, y;
        err = adc_oneshot_read(adc, STICK_X_CHANNEL, &x);
        if (err == ESP_OK) err = adc_oneshot_read(adc, STICK_Y_CHANNEL, &y);
        if (err != ESP_OK) {
            adc_oneshot_del_unit(adc);
            adc = NULL;
            return err;
        }
        sum_x += x;
        sum_y += y;
        vTaskDelay(1);
    }
    center_x = sum_x / CENTER_SAMPLES;
    center_y = sum_y / CENTER_SAMPLES;
    if (center_x < 256 || center_x > ADC_MAX - 256 ||
        center_y < 256 || center_y > ADC_MAX - 256) {
        ESP_LOGE("INPUT", "Stick center out of range (%d, %d); check wiring", center_x, center_y);
        adc_oneshot_del_unit(adc);
        adc = NULL;
        return ESP_ERR_INVALID_STATE;
    }
    ESP_LOGI("INPUT", "Stick center calibrated: X=%d Y=%d", center_x, center_y);
    return ESP_OK;
}

esp_err_t input_read(handheld_report_t *report){
    if (report == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (adc == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    int x, y;
    esp_err_t err = read_axis(STICK_X_CHANNEL, READ_SAMPLES, &x);
    if (err != ESP_OK) {
        return err;
    }
    err = read_axis(STICK_Y_CHANNEL, READ_SAMPLES, &y);
    if (err != ESP_OK) {
        return err;
    }

    report->stick_x = map_axis(x, center_x);
    report->stick_y = map_axis(y, center_y);
    report->trigger = gpio_get_level(TRIGGER_PIN) == 0 ? INT16_MAX : 0;
    report->bumper = gpio_get_level(BUMPER_PIN) == 0;
    report->stick_click = gpio_get_level(STICK_CLICK_PIN) == 0;
    return ESP_OK;
}
