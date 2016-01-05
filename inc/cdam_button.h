#ifndef CDAM_BUTTON_H
#define CDAM_BUTTON_H

#include "spark_wiring_usbserial.h"

namespace cdam
{

class Button {

public:
	/* Public Methods */
	Button();
	void initialize(uint8_t aButtonPin, int16_t aButtonValue = 0);
	void initialize(uint8_t aButtonPin, bool aActiveType, int16_t aButtonValue = 0);
	void initialize(uint8_t aButtonPin, bool aActiveType, bool aInternalPullup, int16_t aButtonValue = 0);
	void updateState();
	int16_t pressValue();
	void setPresses(int16_t aPresses);

	/* Public Variables */
	bool active; // Should the keypad respond to update requests.
	int16_t value; // Value of the button, zero if not set.
	int8_t presses; // Total button presses.
	bool depressed; // Current press state of button.
	bool held; // Button has been long pressed.
	uint16_t debounceTime;
	uint16_t longPressTime;
	uint16_t multiPressTime;
private:
	/* Private Methods */
	void updateButton();

	/* Private Variables */
	uint8_t _pin;
	bool _activeHigh;
	bool _buttonState;
	bool _lastState;
	int _pressCount; // Number of button presses within multiPressTime.
	uint32_t _lastDebounceTime; // Last input pin toggle (noise or press).
};

}

#endif