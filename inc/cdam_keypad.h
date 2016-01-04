#ifndef CDAM_KEYPAD_H
#define CDAM_KEYPAD_H

//#include "spark_wiring_usbserial.h"
#include "cdam_button.h"

namespace cdam
{

#define NUM_BUTTONS 4
#define KEYPAD_FILTER_COUNT 200

class Keypad {

public:
	/* Public Methods */
	Keypad();
	void initialize(uint8_t aPinBtnOne, uint8_t aPinBtnTwo,
	                uint8_t aPinBtnThree, uint8_t aPinBtnFour);
	void updateState();
	int16_t downValue();
	int16_t downValInRange(bool &aValid, int16_t aLow, int16_t aHigh);
	int16_t pressedValue();
	int16_t pressedValInRange(bool &aValid, int16_t aLow, int16_t aHigh);
	void setDownButton(uint8_t aButtonNum);
	void setPressedButton(uint8_t aButtonNum, int16_t aPresses = 1);
	void setDownValue(int16_t aValue);
	void setPressedValue(int16_t aValue, bool aLongPresses = false);

	Button* btnOne();
	Button* btnTwo();
	Button* btnThree();
	Button* btnFour();

	/* Public Variables */
	bool active; // Should the keypad respond to update requests.
	uint16_t multiUpTime;
	/*Button btnOne;
	Button btnTwo;
	Button btnThree;
	Button btnFour;*/
private:
	/* Private Methods */

	/* Private Variables */
	Button _buttons[NUM_BUTTONS];
	/*bool _multiDebounce;
	int16_t _multiValue;
	uint32_t _debounceTime;*/

};

}

#endif