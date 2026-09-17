#pragma once
#include "../Modules/RTC1307.h"
#include "../Modules/TM1637Display.h"

static char buffer[33];//make it global (we are on an mcu)

volatile uint8_t cmdCounter = 0;
volatile uint32_t lastInterruptTime = 0;
//button interrupt service routine (no cpu polling)
void buttonISR() {
	uint32_t currentTime = millis();
	//software debounce
	if (currentTime - lastInterruptTime > 150) {
		lastInterruptTime = currentTime;
		cmdCounter++;
	}
}

class DisplayClock
{
private:
	using CommandHandler = void (DisplayClock::*)();


	uint8_t clkPin;
	uint8_t dataPin;
	uint8_t buttonPin;

	TwoWire* wire;
	DS1307 rtc;
	TM1637Display display;

	DateTime dt;

public:
	DisplayClock(uint8_t clkPin, uint8_t dataPin, uint8_t buttonPin, TwoWire* wire = &Wire)
		: clkPin(clkPin), dataPin(dataPin), buttonPin(buttonPin), wire(wire), rtc(), display(clkPin, dataPin) {}
	
	//start Serial before calling this function
	void begin() {
		wire->begin();
		if (!rtc.begin()) {
			Serial.println("Couldn't find RTC");
			while (1);
		}
		if (!display.begin()) {
			Serial.println("Couldn't find Display");
			while (1);
		}
		display.setBrightness(7, true);

		pinMode(buttonPin, INPUT_PULLUP);
		attachInterrupt(digitalPinToInterrupt(buttonPin), buttonISR, FALLING);
	}
	
	void process() {
		dt = rtc.getTime();
		buffer[0] = '\0'; // Clear the buffer
		snprintf(buffer, sizeof(buffer), "BSz:P %02d:%02d:%02d %02d/%02d / %04d", dt.hour, dt.minute, dt.second, dt.day, dt.month, dt.year);
		Serial.println(buffer);
	}
	void showYear() {
		display.showNumberDec(dt.year, false, false);//2026
	}
	void showMonthDay() {
		display.showNumberDec(dt.month * 100 + dt.day, true, true);// 09:16
	}
	void showHourMinute() {
		display.showNumberDec(dt.hour * 100 + dt.minute, true, true);// 16:22
	}
	void showSecond() {
		display.showNumberDec(dt.minute * 100 + dt.second, true, true); // 22:59
	}

	void scroll(int delayTime) {
		process();
		showYear();
		delay(delayTime);
		showMonthDay();
		delay(delayTime);
		showHourMinute();
		delay(delayTime);
		showSecond();
		delay(delayTime);
	}
	void scrollOnPress() {
		if (digitalRead(buttonPin) == LOW) {
			cmdCounter ++;
		}
		process();
		runCommand(cmdCounter);
	}
	void runCommand(uint8_t cmd) {
		// Defined inside the function to eliminate the missing out-of-class symbol definition
		static constexpr CommandHandler handlers[] = {
			&DisplayClock::showYear,
			&DisplayClock::showMonthDay,
			&DisplayClock::showHourMinute,
			&DisplayClock::showSecond
		};

		constexpr uint8_t count = sizeof(handlers) / sizeof(handlers[0]);
		(this->*handlers[cmd % count])();
	}
	void setBrightness(uint8_t brightness, bool on = true) {
		display.setBrightness(brightness, on);
	}
	//7 if external power supply, 3 if battery powered
	void setTime(const DateTime& dt) {
		rtc.setTime(dt);
	}
};
