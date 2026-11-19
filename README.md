# Arduino Nano LED Strip Controller

Addressable RGB LED strip driven by an Arduino Nano using PlatformIO and FastLED.
This is the first stage of a small embedded project: a motion- and touch-controlled night light.

> **Status:** work in progress. Touch-controlled LED modes work; motion sensor is next.

![WS2812B strip controlled by Arduino Nano](docs/demo1.jpg)

## Hardware

| Component | Notes |
|---|---|
| Arduino Nano (ATmega328P clone, CH340) | 16 MHz, 2 KB RAM |
| Addressable RGB LED strip | WS2812B, 60 LEDs, 5 V |
| 5 V power supply | USB charger |
| Touch button | TTP223B |
| Sensor | HC-SR501 |

## Wiring

| Strip | Connects to |
|---|---|
| 5V | External 5 V supply (+) |
| GND | External supply (-) **and** Arduino GND |
| DIN | Arduino pin D4 |

| Touch button (TTP223B) | Connects to |
|---|---|
| VCC | External 5 V supply (+) |
| GND | Common GND |
| SIG | Arduino pin D2 |

| Sensor HC-SR501 | Connects to |
|---|---|
| VCC | External 5 V supply (+) |
| GND | Common GND |
| SIG | Arduino pin D3 |

Notes:

- The strip must be powered from an external 5 V source, not from the Arduino 5V pin. One LED can draw up to ~60 mA at full white.
- Brightness is capped in code because 60 LEDs at full white can exceed what a USB charger supplies.
- GND of the Arduino, strip and button must be common.
- Data flows in the direction of the arrows printed on the strip.

## Software

- [PlatformIO](https://platformio.org/) (VS Code extension)
- Board: `nanoatmega328`
- Framework: Arduino
- Library: [FastLED](https://github.com/FastLED/FastLED) 3.10.9 (pinned in `platformio.ini`)

## Build and upload

```bash
pio run                # build
pio run -t upload      # flash the board
```

On Linux you may need serial port permissions (e.g. add your user to the `uucp` group on Arch-based systems, or `dialout` on Debian/Ubuntu) and the PlatformIO udev rules.

## Configuration

Edit the constants at the top of `src/main.cpp`:

```cpp
constexpr uint8_t  LED_PIN     = 4;
constexpr uint16_t NUM_LEDS    = 60;
constexpr uint8_t  BRIGHTNESS  = 50;   // 0-255, keep low while testing
constexpr uint8_t  BUTTON_PIN  = 2;
```

If colors look wrong (e.g. red shows as green), change the color order in `addLeds` (`GRB`, `RGB`, `BRG`).

## Demo

[![Demo video](docs/demo2.jpg)](https://youtu.be/RCGIvZtT0HU)

## What works now

- [x] Strip control with FastLED
- [x] Three animation modes: breathing, running light, rainbow
- [x] Non-blocking animations with `millis()`
- [x] Mode switching with a state machine
- [x] Touch button (TTP223B): switch modes

## Roadmap

- [ ] Motion sensor (HC-SR501): light on when someone approaches
- [ ] Phone control (Bluetooth or Wi-Fi): color and brightness
- [ ] Battery power (TP4056 + MT3608)

