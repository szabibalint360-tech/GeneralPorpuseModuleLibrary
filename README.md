# Embedded Hardware Modules & Driver Library

A structured collection of lightweight, modular C++ drivers and integrated hardware system abstractions for microcontrollers. This repository serves as a personal code bank and driver reference for common integrated circuits, display controllers, and bus protocols.

---

## Architecture & Project Structure

The code is split into two primary layers: standalone chip drivers and composed hardware systems.

```text
├── Modules/
│   ├── RTC1307.h            # I2C Real-Time Clock driver (DS1307)
│   └── TM1637Display.h      # 4-Digit 7-Segment LED controller driver
├── Systems/
│   └── DisplayClock.h       # Integrated interrupt-driven digital clock system
└── Examples/
    ├── RTC1307_example.cpp
    ├── TM1637Display_example.cpp
    └── DisplayClock_example.cpp

```

* **Modules/**: Zero-dependency, class-based drivers handling protocol communication (I2C, custom open-drain two-wire bit-banging), register manipulation, and state control.
* **Systems/**: Composite implementations that combine multiple hardware drivers, external interrupts, and debouncing logic into clean high-level state machines.

---

## Development Environment & Toolchain

* **Primary IDE**: Microsoft Visual Studio with the **Visual Micro** extension.
* **Compatibility**: Native C++/Arduino-compatible.

### Building in Visual Studio (Visual Micro)

1. Open the `.sln` file in Visual Studio.
2. Ensure the **Visual Micro** extension is enabled.
3. Select your target board (e.g., Arduino Uno / ATmega328P) and active COM port.
4. Build and upload directly from Visual Studio using `Ctrl + Alt + U`.

### Standard Arduino / PlatformIO / Bare-Metal Usage

The source files in `Modules/` and `Systems/` are standard C++ classes. If you are not using Visual Studio, copy the `.h` files directly into your project's `src` or `include` folder.

---

## Modules Overview

### DS1307 Real-Time Clock (`Modules/RTC1307.h`)

Class-based I2C driver for the Maxim DS1307 RTC.

* **Features**:
* Binary-Coded Decimal (BCD) bit-shift encoding and decoding.
* Time write/read routines returning structured `DateTime` objects.
* Square Wave Generator configuration (`1Hz`, `4.096kHz`, `8.192kHz`, `32.768kHz`).
* 56-byte internal battery-backed NVRAM byte read and write routines.



### TM1637 7-Segment Display (`Modules/TM1637Display.h`)

Bit-banging display driver utilizing custom open-drain timing cycles.

* **Features**:
* Direct port/pin toggling mimicking open-drain hardware logic (`INPUT` for floating HIGH, `OUTPUT` for active LOW driving).
* Auto-incrementing register writing for 4-digit numerical output and decimal/colon control.
* 8-level display brightness control and power sleep toggling.



### Integrated Display Clock (`Systems/DisplayClock.h`)

High-level application class wrapping the `DS1307` and `TM1637Display` together with a hardware interrupt button handler.

* **Features**:
* Non-blocking button cycling via `FALLING` edge hardware interrupts (`buttonISR`) with software time-delta debouncing.
* Internal function-pointer lookup table (`CommandHandler handlers[]`) for mapping display modes (Year, Month/Day, Hours/Minutes, Seconds).
* Formatted serial debug logging alongside display updates.



---

## Hardware Pinout Reference

| Module / Component | Pin Name | Microcontroller Pin | Notes |
| --- | --- | --- | --- |
| **DS1307 RTC** | SDA | A4 / SDA | I2C Data (Requires Pull-ups) |
|  | SCL | A5 / SCL | I2C Clock (Requires Pull-ups) |
| **TM1637 Display** | CLK | Digital Pin 9 | Custom 2-wire serial clock |
|  | DIO | Digital Pin 8 | Custom 2-wire serial data |
| **User Pushbutton** | OUT | Digital Pin 7 | Configured with internal `INPUT_PULLUP` |

---
