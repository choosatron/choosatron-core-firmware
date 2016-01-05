#include "cdam_button.h"
#include "cdam_constants.h"

namespace cdam
{

Button::Button() {}

void Button::initialize(uint8_t aButtonPin, int16_t aButtonValue) {
	initialize(aButtonPin, LOW, false, aButtonValue);
}

void Button::initialize(uint8_t aButtonPin, bool aActiveType, int16_t aButtonValue) {
	initialize(aButtonPin, aActiveType, false, aButtonValue);
}

void Button::initialize(uint8_t aButtonPin, bool aActiveType, bool aInternalPullup, int16_t aButtonValue) {
	_pin              = aButtonPin;
	value             = aButtonValue;
	_activeHigh       = aActiveType;
	_buttonState      = !_activeHigh;
	_lastState        = _buttonState;
	_pressCount       = 0;
	presses           = 0;
	depressed         = false;
	held              = false;
	_lastDebounceTime = 0;
	debounceTime      = 20; // Milliseconds to debounce.
	multiPressTime    = 250; // Milliseconds for multiple presses.
	longPressTime     = 1000; // Milliseconds for long press.
	active = true;

	if (_activeHigh == LOW && aInternalPullup == HIGH) {
		pinMode(_pin, INPUT_PULLUP);
	} else {
		pinMode(_pin, INPUT_PULLDOWN);
	}
}

void Button::updateState() {
	if (this->active) {
		updateButton();
	}
}

int16_t Button::pressValue() {
	return value * presses;
}

void Button::setPresses(int16_t aPresses) {
	presses = aPresses;
	_pressCount = 0;
	_buttonState = !_activeHigh;
	_lastState = _buttonState;
	_lastDebounceTime = 0;
}

/* Private Methods */

void Button::updateButton() {
	uint32_t now = millis();
	held = false;
	_buttonState = digitalRead(_pin); // Current apparent pin state.

	// Make the button logic active-high in code
	if (!_activeHigh) {
		_buttonState = !_buttonState;
	}

	// If pin state has changed (noise or press), reset debounce timer.
	if (_buttonState != _lastState) {
		_lastDebounceTime = now;
	}

	// If debounce time has passed without state change, count as a press.
	if ((now - _lastDebounceTime > debounceTime) && (_buttonState != depressed)) {
		depressed = _buttonState;
		if (depressed) {
			_pressCount++;
		}
	}

	// Check for completion of press counting.
	if (!depressed && ((now - _lastDebounceTime) > multiPressTime)) {
		// Total presses.
		presses = _pressCount;
		_pressCount = 0;
	}

	// Check for a long press.
	if (depressed && ((now - _lastDebounceTime) > longPressTime)) {
		// Count negative to track long presses.
		held = true;
		presses = 0 - _pressCount;
		_pressCount = 0;
	}

	_lastState = _buttonState;
}

}