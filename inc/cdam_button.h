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
	void setPresses(int16_t aPresses);

	/* Public Variables */
	bool active; // Should the keypad respond to update requests.
	int16_t value; // Value of the button, zero if not set.
	int8_t presses; // Total button presses.
	bool depressed; // Current press state of button.
	uint32_t debounceTime;
	uint32_t longPressTime;
	uint32_t multiPressTime;
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