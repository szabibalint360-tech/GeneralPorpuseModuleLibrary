#include <Arduino.h>
#include "Modules/TM1637Display.h"

// Define digital pins connected to the module
const uint8_t CLK_PIN = D9;
const uint8_t DIO_PIN = D8;

// Instantiate display object (pinClk, pinDIO, bitDelay in microseconds)
TM1637Display display(CLK_PIN, DIO_PIN, 100);

uint16_t counter = 0;

void setup() {
    // Configures pins as open-drain (INPUT floating HIGH, OUTPUT sinking LOW)
    display.begin();

    // Set maximum brightness (0 = min, 7 = max)
    display.setBrightness(7, true);

    // Clear display buffer
    display.clear();
    delay(200);
}

void loop() {
    // Print counter (num, leading_zero = false)
    display.showNumberDec(counter, false);

    counter++;
    if (counter > 9999) {
        counter = 0;
    }

    delay(500);
}