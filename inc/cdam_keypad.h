#ifndef CDAM_KEYPAD_H
#define CDAM_KEYPAD_H

#include "spark_wiring_usbserial.h"

namespace cdam
{

#define TICK_MS 5
#define NUM_BUTTONS 4
#define BTN_FILTER_DOWN_COUNT (25/TICK_MS)
#define BTN_FILTER_UP_COUNT (50/TICK_MS)
#define BTN_HOLD_COUNT (600/TICK_MS)

#define KEYPAD_FILTER_DOWN_COUNT (400/TICK_MS)
#define KEYPAD_FILTER_UP_COUNT (200/TICK_MS)

typedef enum {
	BTN_IDLE_STATE = 1,
	BTN_FILTER_DOWN_STATE,
	BTN_FILTER_HOLD_STATE,
	BTN_FILTER_UP_STATE
} ButtonState;

typedef enum {
	BTN_NO_EVENT = 0,
	BTN_DOWN_EVENT,
	BTN_HELD_EVENT,
	BTN_UP_EVENT
} ButtonEvent;

typedef enum {
	KEYPAD_IDLE_STATE = 1,
	KEYPAD_FILTER_MULTI_DOWN_STATE,
	KEYPAD_FILTER_MULTI_UP_STATE,
} KeypadState;

typedef enum {
	KEYPAD_NO_EVENT = 0,
	KEYPAD_MULTI_DOWN_EVENT,
	KEYPAD_MULTI_UP_EVENT
} KeypadEvent;

typedef struct {
	volatile uint8_t pin;   // the pin number of the button
	volatile uint8_t num;
  	volatile uint8_t count;
  	volatile ButtonState state; // The current button state
  	volatile ButtonEvent event;
  	volatile uint8_t value:1; // the last digital read value
  	volatile uint8_t active:1; // the boolean button (active or not)
  	volatile uint8_t heldFlag:1;
} ButtonData;


class Keypad {

public:
	/* Public Methods */
	Keypad();
	void initialize();
	void updateState();
	bool buttonsDown();
	bool buttonDown(uint8_t aBtnNum);
	// Returns value of button if the event type has occurred, otherwise 0.
	uint8_t buttonEvent(ButtonEvent aEvent, uint8_t aRange = 0);
	uint8_t buttonEventValue(ButtonEvent aEvent, uint8_t aBtnNum);
	//char buttonEventChar(ButtonEvent aEvent, uint8_t aBtnNum);
	uint8_t buttonValue(ButtonState aState, uint8_t aBtnNum);
	//char buttonChar(uint8_t aBtnNum);
	// Returns value of the buttons together, otherwise 0.
	uint8_t keypadEvent(KeypadEvent aEvent, uint8_t aRange = 0);
	uint8_t keypadEventValue(KeypadEvent aEvent);
	//char keypadEventChar(KeypadEvent aEvent);
	void clearEvents();

	/* Public Variables */

	// Should the keypad respond to update requests.
	bool active;
private:
	/* Private Methods */
	ButtonEvent filterButton(ButtonData *aBtnData);
	void getFilteredButtons(void);
	uint8_t keypadTotal(uint8_t aButtons);
	void printBinary(uint8_t aValue);

	ButtonData buttonData[NUM_BUTTONS];

	/* Private Variables */

	KeypadState state;
	KeypadEvent event;
	KeypadEvent lastEvent;

	volatile uint8_t buttons; // Current active buttons.
	volatile uint8_t keypadValue; // The value of all currently active buttons.
	volatile uint8_t lastButtons; // The last valid active buttons.
	volatile uint8_t lastValue; // The last value of all currently active buttons.

	volatile uint8_t multiCount; // Holds the debounce count for multi-down & multi-up events.
	volatile uint8_t multiUp; // Temporary holds multi-up value.
	volatile uint8_t multiUpValue;

	volatile uint8_t pressedEvents;  // Holds which buttons have gone active.
	volatile uint8_t releasedEvents; // Holds which buttons have been pressed and released.
    volatile uint8_t heldEvents; // Holds which buttons have been held for more than HOLD_COUNT.
    volatile uint8_t multiDownEvent; // Holds which buttons were active on a valid multi-down event.
    volatile uint8_t multiUpEvent; // Holds which buttons were active on a valid multi-up event.
};

}

#endif