/*    ClickButton

 A library that decodes multiple clicks on one button.
 Also copes with long clicks and click-and-hold.

 Usage: ClickButton buttonObject(pin [LOW/HIGH, [CLICKBTN_PULLUP]]);

  where LOW/HIGH denotes active LOW or HIGH button (default is LOW)
  CLICKBTN_PULLUP is only possible with active low buttons.

 Returned click counts:

   A positive number denotes the number of (short) clicks after a released button
   A negative number denotes the number of "long" clicks

NOTE!
 This is the OPPOSITE/negative of click codes from the last pre-2013 versions!
 (this seemed more logical and simpler, so I finally changed it)

 Based on the Debounce example at arduino playground site

 Copyright (C) 2010,2012, 2013 raron

 GNU GPLv3 license

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 Contact: raronzen@gmail.com

 History:
 2013.08.29 - Some small clean-up of code, more sensible variable names etc.
				Added another example code for multiple buttons in an object array
 2013.04.23 - A "minor" debugging: active-high buttons now work (wops)!
				Thanks goes to John F. H. for pointing that out!
 2013.02.17 - Some improvements, simplified click codes.
				Added a LED fader example. Thanks to Tom K. for the idea.
 2012.01.31 - Tiny update for Arduino 1.0
 2010.06.15 - First version. Basically just a small OOP programming exercise.
*/

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
	depressed         = 0;
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
		presses = 0 - _pressCount;
		_pressCount = 0;
	}

	_lastState = _buttonState;
}

}