#include <Arduino.h>
#include <Wire.h>
#include "Modules/RTC1307.h"

DS1307 rtc;

void setup() {
	Wire.begin();
	Serial.begin(9600);

	if (!rtc.begin()) {
		Serial.println("Couldn't find RTC");
		while (1);
	}
	pinMode(LED_BUILTIN, INPUT);
	rtc.setTime({ 0, 0, 12, 1, 1, 1, 2026 }); // Set time to 12:00:00 on Monday, January 1, 2026
	rtc.setSquareWave(SQW_1HZ);
}
char buffer[33];
uint8_t counter = 0;
void loop() {
	DateTime dt = rtc.getTime();
	buffer[0] = '\0'; // Clear the buffer
	snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d %02d/%02d / %04d", dt.hour, dt.minute, dt.second, dt.day, dt.month, dt.year);
	Serial.println(buffer);
	rtc.writeRAM(0, counter++);
	uint8_t data = rtc.readRAM(0);
	Serial.print("Data read from RAM: ");
	Serial.println(data, DEC);
	delay(1000);
}
