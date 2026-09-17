#pragma once
#include <Arduino.h>
#include <inttypes.h>

// Wiring (should be obvious)
// TM1637 -> Arduino
//  VCC -> 5V
//  GND -> GND
//  CLK -> Digital Pin (e.g., D9)
//  DIO -> Digital Pin (e.g., D8)

#define TM1637_COMM_DATA      0x40  // COMM1
#define TM1637_COMM_ADDR      0xC0  // COMM2
#define TM1637_COMM_DISPLAY   0x80  // COMM3

class TM1637Display {
private:
    uint8_t m_pinClk;
    uint8_t m_pinDIO;
    uint8_t m_brightness;
    unsigned int m_bitDelay;

    const uint8_t digitToSegment[16] = {
        0b00111111, // 0 | Dec: 63  | Hex: 0x3F
        0b00000110, // 1 | Dec: 6   | Hex: 0x06
        0b01011011, // 2 | Dec: 91  | Hex: 0x5B
        0b01001111, // 3 | Dec: 79  | Hex: 0x4F
        0b01100110, // 4 | Dec: 102 | Hex: 0x66
        0b01101101, // 5 | Dec: 109 | Hex: 0x6D
        0b01111101, // 6 | Dec: 125 | Hex: 0x7D
        0b00000111, // 7 | Dec: 7   | Hex: 0x07
        0b01111111, // 8 | Dec: 127 | Hex: 0x7F
        0b01101111, // 9 | Dec: 111 | Hex: 0x6F
        0b01110111, // A | Dec: 119 | Hex: 0x77
        0b01111100, // b | Dec: 124 | Hex: 0x7C
        0b00111001, // C | Dec: 57  | Hex: 0x39
        0b01011110, // d | Dec: 94  | Hex: 0x5E
        0b01111001, // E | Dec: 121 | Hex: 0x79
        0b01110001  // F | Dec: 113 | Hex: 0x71
    };

    void bitDelay() {
        delayMicroseconds(m_bitDelay);
    }

    // Open-Drain START condition
    void start() {
        pinMode(m_pinDIO, OUTPUT); // Pull DIO LOW
        bitDelay();
    }

    // Open-Drain STOP condition
    void stop() {
        pinMode(m_pinDIO, OUTPUT); // Ensure DIO LOW
        bitDelay();
        pinMode(m_pinClk, INPUT);  // Float CLK HIGH
        bitDelay();
        pinMode(m_pinDIO, INPUT);  // Float DIO HIGH
        bitDelay();
    }

    //open drain logic
    bool writeByte(uint8_t b) {
        uint8_t data = b;

        // Shift out 8 bits (LSB First)
        for (uint8_t i = 0; i < 8; i++) {
            // CLK LOW
            pinMode(m_pinClk, OUTPUT);
            bitDelay();

            // Set DIO state via Open-Drain logic
            if (data & 0x01){
                pinMode(m_pinDIO, INPUT);  // Float HIGH
            } else {
                pinMode(m_pinDIO, OUTPUT); // Drive LOW
            }
            bitDelay();

            // CLK HIGH
            pinMode(m_pinClk, INPUT);
            bitDelay();
            data >>= 1;
        }

        // ACK Cycle
        pinMode(m_pinClk, OUTPUT); // CLK LOW
        pinMode(m_pinDIO, INPUT);  // Release DIO for ACK
        bitDelay();

        pinMode(m_pinClk, INPUT);  // CLK HIGH
        bitDelay();
        uint8_t ack = digitalRead(m_pinDIO);

        if (ack == 0) {
            pinMode(m_pinDIO, OUTPUT); // Drive LOW if acknowledged
        }
        bitDelay();
        pinMode(m_pinClk, OUTPUT); // CLK LOW
        bitDelay();

        return (ack == 0);
    }

public:
    TM1637Display(uint8_t pinClk, uint8_t pinDIO, unsigned int bitDelay = 100)
        : m_pinClk(pinClk), m_pinDIO(pinDIO), m_bitDelay(bitDelay), m_brightness(0x0F) {}

    int begin() {
        digitalWrite(m_pinClk, LOW);
        digitalWrite(m_pinDIO, LOW);

        pinMode(m_pinClk, INPUT);
        pinMode(m_pinDIO, INPUT);

        delayMicroseconds(10);

        // Verify
        if (digitalRead(m_pinClk) == LOW || digitalRead(m_pinDIO) == LOW) {
            return 0; 
        }

        return 1;
    }

    void setBrightness(uint8_t brightness, bool on = true) {
        m_brightness = (brightness & 0x07) | (on ? 0x08 : 0x00);
    }

    void setSegments(const uint8_t segments[], uint8_t length = 4, uint8_t pos = 0, bool showColon = false) {
        // Transaction 1: COMM1 (Write Data Command)
        start();
        writeByte(TM1637_COMM_DATA);
        stop();

        // Transaction 2: COMM2 + Address + Data Bytes
        start();
        writeByte(TM1637_COMM_ADDR + (pos & 0x03));

        for (uint8_t k = 0; k < length; k++) {
            uint8_t data = segments[k];

            if (showColon && ((pos + k) == 1)) {
                data |= 0x80;
            }

            writeByte(data);
        }
        stop();

        // Transaction 3: COMM3 + Brightness Latch Command
        start();
        writeByte(TM1637_COMM_DISPLAY + (m_brightness & 0x0F));
        stop();
    }

    void showNumberDec(int num, bool leading_zero = false, bool showColon = false, uint8_t length = 4, uint8_t pos = 0) {
        uint8_t digits[4] = { 0 };

        for (int i = length - 1; i >= 0; --i) {
            uint8_t digit = num % 10;
            if (digit == 0 && num == 0 && !leading_zero && i < (length - 1)) {
                digits[i] = 0; // Blank leading zeros
            }
            else {
                digits[i] = digitToSegment[digit];
            }
            num /= 10;
        }
        setSegments(digits, length, pos, showColon);
    }

    void clear() {
        uint8_t blank[4] = { 0, 0, 0, 0 };
        setSegments(blank);
    }
};