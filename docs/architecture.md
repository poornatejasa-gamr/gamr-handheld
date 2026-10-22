# GAMR handheld firmware architecture

Both grips run the same ESP32-S3 firmware. Set `HANDHELD_SIDE` in
`main/handheld_config.h` before building so a unit identifies itself as the
left or right grip.

## Current data path

```text
Joystick + trigger + bumper + stick click
                |
              input
                |
       handheld_link (ESP-NOW, channel 1)
                |
              GAMR MAT
                |
      merged Gamepad HID report over BLE
                |
          TV / computer / phone
```

`main/main.c` reads input and sends a report every 20 ms. Each report contains
the grip side, sequence number, signed stick X/Y values, trigger value, bumper,
and stick-click state. The MAT responds with an acknowledgement containing the
same sequence number.

An acknowledgement confirms that the MAT accepted the report into its local
Gamepad HID state. It does not confirm that the downstream BLE host received
the report. If acknowledgements stop for one second, the handheld changes from
connected to ready state; the MAT independently releases stale grip controls
after 500 ms.

The current ESP-NOW transport uses channel 1 and broadcast reports so either
grip can find the MAT without stored peer addresses. It is not an encrypted or
authenticated transport yet; pairing, encryption, haptic feedback, battery,
IMU, and OTA remain later work.

## Components

| Component | Current responsibility |
| --- | --- |
| `input` | Reads/calibrates joystick axes and reads trigger, bumper, and stick click. |
| `transport` | Implements `handheld_link`: ESP-NOW report transmission and MAT acknowledgements. |
| `led` | Shows ready, connected, and error status. |
| `battery` | Interface stub for later battery and charging support. |
| `haptics` | Interface stub for feedback requested by the MAT. |
| `ota` | Interface stub for inactive-slot update support. |

The input component calibrates the joystick centre at startup, applies
averaging and a dead zone, and sends signed axes. The tested joystick wiring
uses GPIO2 for horizontal movement and GPIO1 for vertical movement; horizontal
sign is inverted so left is negative and right is positive.

```
