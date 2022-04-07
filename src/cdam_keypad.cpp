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

   _waitForUp = false;
   _multiDebounce = false;
   _lastBtnStates = 0;
   _lastBtnHeldStates = 0;
   _maxMultiDown = 0;
   _lastValue = 0;
   _storedValue = 0;
   _lastDebounceTime = 0;
   //_buttonStates = 0;
   //_keypadValue = 0;

   this->multiUpTime = 200;
   this->active = true;
}

void Keypad::updateState() {
   if (this->active) {
      for (uint8_t i = 0; i < NUM_BUTTONS; ++i) {
         _buttons[i].updateState();
      }
      updateKeypad();
   }
}

void Keypad::waitForUp() {
   _waitForUp = true;
}

bool Keypad::buttonDepressed(uint8_t aButtonNum) {
   if (_waitForUp) { return false; }
   if (aButtonNum > NUM_BUTTONS) { return false; }
   // Don't allow any new events until all buttons are up.
   bool depressed = _buttons[aButtonNum - 1].depressed;
   if (depressed) {
      _waitForUp = true;
   }
   return depressed;
}

bool Keypad::buttonHeld(uint8_t aButtonNum) {
   if (_waitForUp) { return false; }
   if (aButtonNum > NUM_BUTTONS) { return false; }
   if (_maxMultiDown > 1) { return false; }
   // Don't allow any new events until all buttons are up.
   bool held = _buttons[aButtonNum - 1].held;
   if (held) {
      _waitForUp = true;
   }
   return held;
}

int16_t Keypad::downValue() {
   if (_waitForUp) { return 0; }

   int16_t total = 0;
   for (uint8_t i = 0; i < NUM_BUTTONS; ++i) {
      if (_buttons[i].depressed) {
         //DEBUG("BTN-%d down, %d presses", _buttons[i].value, _buttons[i].presses);
         total += (_buttons[i].presses * _buttons[i].value);
      }
   }
   // Don't allow any new events until all buttons are up.
   if (total > 0) {
      _waitForUp = true;
   }
   /*if (total > 0) {
      DEBUG("Down Val: %d\n---", total);
   }*/
   return total;
}

int16_t Keypad::downValInRange(bool &aValid, int16_t aLow, int16_t aHigh) {
   aValid = false;
   if (_waitForUp) { return 0; }

   int16_t total = downValue();
   if ((aLow <= total) && (total <= aHigh)) {
      aValid = true;
   }
   // Don't allow any new events until all buttons are up.
   if (aValid) {
      _waitForUp = true;
   }
   return total;
}

int16_t Keypad::multiUpValue() {
   if (_waitForUp) { return 0; }

   // Don't allow any new events until all buttons are up.
   if (_multiUpValue > 0) {
      _waitForUp = true;
   }
   return _multiUpValue;
}

int16_t Keypad::multiUpValInRange(bool &aValid, int16_t aLow, int16_t aHigh) {
   aValid = false;
   if (_waitForUp) { return 0; }
   
   if ((aLow <= _multiUpValue) && (_multiUpValue <= aHigh)) {
      aValid = true;
   }
   // Don't allow any new events until all buttons are up.
   if (aValid) {
      _waitForUp = true;
   }
   return _multiUpValue;
}

/*int16_t Keypad::pressedValue() {
   int16_t total = 0;

   for (uint8_t i = 0; i < NUM_BUTTONS; ++i) {
      if (_buttons[i].presses) {
         DEBUG("BTN-%d, %d presses", _buttons[i].value, _buttons[i].presses);
         total += (_buttons[i].presses * _buttons[i].value);
      }
   }

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
}*/

void Keypad::setDownButton(uint8_t aButtonNum) {
   _waitForUp = false;
   _buttons[aButtonNum - 1].depressed = true;
}

void Keypad::setPressedButton(uint8_t aButtonNum, int16_t aPresses) {
   _waitForUp = false;
   _buttons[aButtonNum - 1].setPresses(aPresses);
}

void Keypad::setDownValue(int16_t aValue) {
   _waitForUp = false;
   for (int8_t i = NUM_BUTTONS - 1; i >= 0; --i) {
      if (aValue >= _buttons[i].value) {
         aValue -= _buttons[i].value;
         _buttons[i].depressed = true;
      } else {
         _buttons[i].depressed = false;
      }
   }
}

void Keypad::setPressedValue(int16_t aValue, bool aLongPresses) {
   _waitForUp = false;
   _multiUpValue = aValue;
   _multiDebounce = false;
   _storedValue = 0;
   int16_t presses = (aLongPresses) ? -1 : 1;
   for (int8_t i = NUM_BUTTONS - 1; i >= 0; --i) {
      if (aValue >= _buttons[i].value) {
         aValue -= _buttons[i].value;
         _buttons[i].setPresses(presses);
      } else {
         _buttons[i].setPresses(0);
      }
   }
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

/* Private Methods */

void Keypad::updateKeypad() {
   _multiUpValue = 0;
   uint32_t now = millis();
   uint8_t buttonStates = 0;
   uint8_t buttonHeldStates = 0;
   int16_t keypadValue = 0;
   // Notice reverse count order, to accomodate left shift.
   for (int8_t i = NUM_BUTTONS - 1; i >= 0; --i) {
      if (_buttons[i].depressed) {
         if (_buttons[i].held) {
            buttonHeldStates |= (_buttons[i].held << i);
         }
         buttonStates |= (_buttons[i].depressed << i);
         keypadValue += _buttons[i].value;
      }
   }

   if (_waitForUp) {
      if (buttonStates == 0) {
         _lastBtnStates = 0;
         _lastBtnHeldStates = 0;
         _maxMultiDown = 0;
         _waitForUp = false;
         _multiDebounce = false;
         _storedValue = 0;
         _multiUpValue = 0;
      }
      return;
   }

   if (_multiDebounce && (now - _lastDebounceTime > this->multiUpTime)) {
      if (buttonStates == 0) {
         //DEBUG("Multi up event: %d", _storedValue);
         _multiUpValue = _storedValue;
      }/* else {
         //DEBUG("Failed multi up.");
      }*/
      _multiDebounce = false;
      _storedValue = 0;
      _maxMultiDown = 0;
   }

   // We want to make sure new buttons haven't gone down, only up.
   // The OR reveals new down buttons.

   if ((buttonStates | _lastBtnStates) == _lastBtnStates) {
      //LOG("BS: %d, LBHS: %d", buttonStates, _lastBtnHeldStates);
      // XOR (Exclusive-Or) reveals differences. We already checked
      // that no new buttons went down, so all differences are newly upped.
      uint8_t newUpBtns = buttonStates ^ _lastBtnStates;
      if (newUpBtns > 0) {
         if (!_multiDebounce) {
            // Handling for special case where one button goes down, is held, then
            // released. We don't want that to count as a multi up.
            /*if ((_maxMultiDown == 1) && (buttonStates == 0) && _lastBtnHeldStates) {
               _multiDebounce = false;
            } else {*/
            if ((_maxMultiDown > 1) || (buttonStates > 0) || !_lastBtnHeldStates) {
               _multiDebounce = true;
               _storedValue = _lastValue;
            }
         }
         _lastDebounceTime = millis();
      }
   } else {
      _multiDebounce = false;
      _storedValue = 0;

      // Count how many buttons are down.
      uint8_t testBits = buttonStates;
      for (_maxMultiDown = 0; testBits; _maxMultiDown++) {
         testBits &= testBits - 1; // clear the least significant bit set
      }
   }

   _lastBtnStates = buttonStates;
   _lastBtnHeldStates = buttonHeldStates;
   _lastValue = keypadValue;
}

}