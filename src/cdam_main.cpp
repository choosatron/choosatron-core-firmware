#include "cdam_main.h"
#include "cdam_data_manager.h"
#include "cdam_keypad.h"

namespace cdam
{


// Extern C seems to mess up declaring this in the header.
Keypad *_keypad;

void Choosatron::enableTimerInterrupt() {
	// Timer Interrupt Setup
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseInitTypeDef timerInitStructure;
	timerInitStructure.TIM_Prescaler = 5; // Timed to 5ms
	timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStructure.TIM_Period = 59999; // Times to 5ms
	timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	timerInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &timerInitStructure);
	TIM_Cmd(TIM2, ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	//TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE); //TIM CCR2 IT enable

	NVIC_InitTypeDef nvicStructure;
	nvicStructure.NVIC_IRQChannel = TIM2_IRQn;
	nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
	nvicStructure.NVIC_IRQChannelSubPriority = 1;
	nvicStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicStructure);
}

extern "C" {
	void TIM2_IRQHandler() {
		if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
			TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
			_keypad->updateKeypad();
		}
	}
}

Choosatron::Choosatron() {
}

bool Choosatron::setup() {
	DataManager.initilize();

	// Setup the timer interrupts for keypad updates.
	enableTimerInterrupt();

	_keypad = new Keypad();
	_keypad->initialize();
	_keypad->active = false;

	return true;
}

int Choosatron::loop() {
	return 0;
}


}