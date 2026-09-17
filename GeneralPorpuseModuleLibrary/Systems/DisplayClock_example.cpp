#include <Arduino.h>
#include <Wire.h>
#include "Systems/DisplayClock.h"

constexpr uint8_t CLK_PIN = 9;
constexpr uint8_t DIO_PIN = 8;
constexpr uint8_t BUTTON_PIN = 7;

DisplayClock displayClock(CLK_PIN, DIO_PIN, BUTTON_PIN, &Wire);

void setup() {
	Serial.begin(9600);
	displayClock.begin();

	pinMode(LED_BUILTIN, INPUT);
	displayClock.setBrightness(1, true);

	//displayClock.setTime({ 0, 35, 20, 3, 9, 16, 2026 }); // Set time to 12:00:00 on Monday, January 1, 2026

}

void loop() {
	displayClock.scrollOnPress();
	delay(1000);
}
