# GAMR handheld firmware

One ESP32-S3 firmware runs on both grips. Set `HANDHELD_SIDE` in
`main/handheld_config.h` to build for the left or right grip.

The ESP-IDF project is currently an interface scaffold. Its hardware functions
return `ESP_ERR_NOT_SUPPORTED` until implemented.
Do not flash it as a working controller yet.

| Component | Responsibility |
| --- | --- |
| `input` | Read one stick, trigger, bumper, and stick click into a grip report. |
| `transport` | Send the report to the mat over ESP-NOW and receive feedback commands. |
| `battery` | Measure battery level and charging state. |
| `led` | Display connection, battery, and update states. |
| `haptics` | Play feedback requested by the mat. |
| `ota` | Validate and install updates in the inactive firmware slot. |

`main/main.c` coordinates the components. IMU gestures and power management can
be added as separate components when their behavior is defined.

The ESP-IDF configuration and partition table live in `idf-env/`. The baseline
targets an ESP32-S3 with 8 MB flash and two OTA slots. Configure and build with
ESP-IDF v5.4.4 using `idf.py set-target esp32s3` and `idf.py build`.
