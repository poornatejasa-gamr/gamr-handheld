idf_component_register(SRCS "handheld_link.c" INCLUDE_DIRS "."
    REQUIRES input PRIV_REQUIRES esp_wifi esp_netif esp_event nvs_flash freertos log)
