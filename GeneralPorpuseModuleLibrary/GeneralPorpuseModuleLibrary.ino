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

	//should only be set once then comment out
	displayClock.setTime({ 0, 25, 16, 4, 17, 9, 2026 }); // Set time to 12:00:00 on Monday, 1 January, 2026
	
}

void loop() {
	displayClock.scrollOnPress();
	delay(1000);
}
