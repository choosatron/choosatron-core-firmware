#include "cdam_keypad.h"
#include "cdam_constants.h"

// The plan: events and state, can always check the state of a given
// button, or the value of the current pressed keys, also get events
// to be informed of button down, up, and all keys up.

namespace cdam
{

/* Public Methods */

Keypad::Keypad() {}

void Keypad::initialize(uint8_t aPinBtnOne, uint8_t aPinBtnTwo,
                        uint8_t aPinBtnThree, uint8_t aPinBtnFour) {
	_buttons[0].initialize(aPinBtnOne, LOW, HIGH, 1);
	_buttons[1].initialize(aPinBtnTwo, LOW, HIGH, 2);
	_buttons[2].initialize(aPinBtnThree, LOW, HIGH, 3);
	_buttons[3].initialize(aPinBtnFour, LOW, HIGH, 4);

	this->active = true;
}

void Keypad::updateState() {
	if (this->active) {
		for (uint8_t i = 0; i < BTN_TOTAL; ++i) {
			_buttons[i].updateState();
		}
	}
}

int16_t Keypad::downValue() {
	int16_t total = 0;

	for (uint8_t i = 0; i < BTN_TOTAL; ++i) {
		if (_buttons[i].depressed) {
			DEBUG("BTN-%d down, %d presses", _buttons[i].value, _buttons[i].presses);
			total += (_buttons[i].presses * _buttons[i].value);
		}
	}

	/*if (btnOne->depressed) {
		DEBUG("BTN-1 Down, %d presses", btnOne->presses);
		total += btnOne->value;
	}
	if (btnTwo->depressed) {
		DEBUG("BTN-2 Down, %d presses", btnTwo->presses);
		total += btnTwo->value;
	}
	if (btnThree->depressed) {
		DEBUG("BTN-3 Down, %d presses", btnThree->presses);
		total += btnThree->value;
	}
	if (btnFour->depressed) {
		DEBUG("BTN-4 Down, %d presses", btnFour->presses);
		total += btnFour->value;
	}*/
	if (total > 0) {
		DEBUG("Down Val: %d\n---", total);
	}
	return total;
}

int16_t Keypad::downValInRange(bool &aValid, int16_t aLow, int16_t aHigh) {
	int16_t total = downValue();
	aValid = false;
	if ((aLow <= total) && (total <= aHigh)) {
		aValid = true;
	}
	return total;
}

int16_t Keypad::pressedValue() {
	int16_t total = 0;

	for (uint8_t i = 0; i < BTN_TOTAL; ++i) {
		if (_buttons[i].presses) {
			DEBUG("BTN-%d, %d presses", _buttons[i].value, _buttons[i].presses);
			total += (_buttons[i].presses * _buttons[i].value);
		}
	}

	/*if (btnOne->presses) {
		DEBUG("BTN-1, %d presses", btnOne->presses);
		total += (btnOne->presses * btnOne->value);
	}
	if (btnTwo->presses) {
		DEBUG("BTN-2, %d presses", btnTwo->presses);
		total += (btnTwo->presses * btnTwo->value);
	}
	if (btnThree->presses) {
		DEBUG("BTN-3, %d presses", btnThree->presses);
		total += (btnThree->presses * btnThree->value);
	}
	if (btnFour->presses) {
		DEBUG("BTN-4, %d presses", btnFour->presses);
		total += (btnFour->presses * btnFour->value);
	}*/
	if (total > 0) {
		DEBUG("Pressed Val: %d\n---", total);
	}
	return total;
}

int16_t Keypad::pressedValInRange(bool &aValid, int16_t aLow, int16_t aHigh) {
	int16_t total = pressedValue();
	aValid = false;
	if ((aLow <= total) && (total <= aHigh)) {
		aValid = true;
	}
	return total;
}

void Keypad::setDownButton(uint8_t aButtonNum) {
	_buttons[aButtonNum - 1].depressed = true;
}

void Keypad::setPressedButton(uint8_t aButtonNum, int16_t aPresses) {
	_buttons[aButtonNum - 1].setPresses(aPresses);
}

void Keypad::setDownValue(int16_t aValue) {
	for (int8_t i = BTN_TOTAL - 1; i >= 0; --i) {
		if (aValue >= _buttons[i].value) {
			aValue -= _buttons[i].value;
			_buttons[i].depressed = true;
		} else {
			_buttons[i].depressed = false;
		}
	}
}

void Keypad::setPressedValue(int16_t aValue, bool aLongPresses) {
	int16_t presses = (aLongPresses) ? -1 : 1;
	for (int8_t i = BTN_TOTAL - 1; i >= 0; --i) {
		if (aValue >= _buttons[i].value) {
			aValue -= _buttons[i].value;
			_buttons[i].setPresses(presses);
		} else {
			_buttons[i].setPresses(0);
		}
	}

	/*if (aValue >= btnFour.value) {
		aValue -= btnFour.value;
		btnFour.pressed = 1;
	} else {
		btnFour.pressed = 0;
	}
	if (aValue >= btnThree.value) {
		aValue -= btnThree.value;
		btnThree.pressed = 1;
	} else {
		btnThree.pressed = 0;
	}
	if (aValue >= btnTwo.value) {
		aValue -= btnTwo.value;
		btnTwo.pressed = 1;
	} else {
		btnTwo.pressed = 0;
	}
	if (aValue >= btnOne.value) {
		aValue -= btnOne.value;
		btnOne.pressed = 1;
	} else {
		btnOne.pressed = 0;
	}*/
}

/* Accessors */

Button* Keypad::btnOne() {
	return &_buttons[0];
}

Button* Keypad::btnTwo() {
	return &_buttons[1];
}

Button* Keypad::btnThree() {
	return &_buttons[2];
}

Button* Keypad::btnFour() {
	return &_buttons[3];
}

}