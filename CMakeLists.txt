cmake_minimum_required(VERSION 3.16)

# Both grips use ESP32-S3; the side is selected in main/handheld_config.h.
set(IDF_TARGET esp32s3)
set(SDKCONFIG "${CMAKE_CURRENT_LIST_DIR}/idf-env/sdkconfig")
set(SDKCONFIG_DEFAULTS "${CMAKE_CURRENT_LIST_DIR}/idf-env/sdkconfig.defaults")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(gamr_handheld)
