idf_component_register(
    SRCS "input.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES esp_adc esp_driver_gpio log freertos
)
