# GAMR handheld firmware

One ESP32-S3 firmware runs on both grips. Set `HANDHELD_SIDE` in
`main/handheld_config.h` to build for the left or right grip.

The stick, three switches, and two indicator LEDs are implemented for hardware
bring-up. ESP-NOW, battery, haptics, and OTA remain interface stubs, so this is
not yet a working wireless controller.

## Fresh-clone build

Open an ESP-IDF v5.4.x PowerShell terminal in the repository root, select the
ESP32-S3 target, and build:

```powershell
$env:IDF_TARGET = 'esp32s3'
idf.py build
```

Set `IDF_TARGET` after activating ESP-IDF. Some PowerShell profiles set it to
`esp32`, which conflicts with this project's ESP32-S3 configuration. In VS
Code, you can instead select **ESP32-S3** from the ESP-IDF target control in
the bottom status bar or the ESP-IDF Explorer on the left, then run **Build**.
A fresh clone does not need `fullclean` or `reconfigure`.

The tracked `sdkconfig`, defaults, and partition table are in `idf-env/`. The
project targets an ESP32-S3 with 8 MB flash and two OTA slots.

## Input and LED connections

Use the board's **3V3** output for the joystick. Its axis outputs must not
exceed 3.3 V at the ESP32-S3 pins. Connect all grounds together.

| Part | Part pin or terminal | ESP32-S3 connection |
| --- | --- | --- |
| Joystick | VCC | 3V3 |
| Joystick | GND | GND |
| Joystick | RX / VRx | GPIO1 (ADC1 channel 0) |
| Joystick | RY / VRy | GPIO2 (ADC1 channel 1) |
| Joystick | SW | GPIO9 (stick press, L3/R3) |
| Bumper push button | One terminal | GPIO7 (LB/RB) |
| Trigger push button | One terminal | GPIO8 (LT/RT for now) |
| All three switches | Other terminal | GND |
| Green LED | Anode (+) through 220 Ω | GPIO16 |
| Red LED | Anode (+) through 220 Ω | GPIO17 |
| Both LEDs | Cathode (short leg, −) | GND |

The three switch inputs use internal pull-ups, so an open switch reads released
and a switch connected to GND reads pressed. If the joystick module's SW pin
is already wired to its GND internally when pressed, it needs no extra wire to
GND beyond the joystick GND connection. Each LED needs its own 220 Ω resistor.
GPIO5 and GPIO6 remain free for the planned I²C IMU.

After flashing, `idf.py monitor` prints the signed stick values every 500 ms,
plus bumper, stick click, and trigger states. The
digital trigger reports `0` or `32767`; the report type remains `int16_t` for a
future analog trigger. The green LED blinks while this input test is running;
the red LED turns on if an input read fails. Leave the joystick centered during
startup: firmware waits two seconds, averages 32 readings to calibrate its
resting position, and prints the measured center. Each update averages 8
readings per axis, and a 160-count dead zone holds small idle fluctuations at
zero. Reset with the stick
released to recalibrate. This calibrates the center only; full travel can be
calibrated later. Switches are read directly without debounce, and a short press
between updates may be missed. If a button never shows
pressed, check that its GPIO and GND wires reach opposite sides of the switch;
two pins on the same side of a four-leg tactile switch are usually connected.
If an axis still flickers while the stick is untouched, place a 100 nF ceramic
capacitor from that ADC input (GPIO1 or GPIO2) to GND, close to the ESP32-S3.

## Repository layout

- `components/` — isolated firmware modules with public APIs.
- `main/` — application entry point and left/right grip setting.
- `idf-env/` — ESP-IDF configuration and partition table.

## Components

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
