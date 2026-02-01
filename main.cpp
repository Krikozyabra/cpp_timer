#define PORTB
#define PORTC
#include "PortInterface.h"

#include "stm32f10x.h" 
#include "7seg_3_dig_timer.cpp"

using MyTimer = SSEGFDIGTimer<Pb10, Pb14, Pb7, Pb5, Pb15, Pb1, Pb8, Pb6, Pb11, Pb0, Pb13, Pb9>;

extern "C" void TIM2_IRQHandler(void);

uint16_t ResetTimer = 5; // стартовое значение таймера
uint16_t volatile Timer = ResetTimer; // значение таймера

uint8_t volatile state = 0; // состояние 0b(blink_status)(settings_state)(blink_state)
#define BLINK_STATE 0x1
#define SETTINGS_STATE 0x2
#define BLINK_STATUS 0x4

int main(void){
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPBEN; // 0x10 clock enabled for port C
	
	Pb4::clearOdr();
	
	Pc13::setMode(MODE_O2);
	Pc13::setCnf(CNF_OPP);
	
	MyTimer::initPins();
	MyTimer::initTimer();
	
	Pc13::clearOdr();
	while(1){
		if(state&BLINK_STATUS){ // Если состояние мигания изменилось, то выключаем, иначе отрисовываем таймер
			MyTimer::allLedOff();
			continue;
		}
		MyTimer::drawTimer(Timer);
	}
}

void TIM2_IRQHandler(void){
        // !!! САМОЕ ВАЖНОЕ: Сбрасываем флаг !!!
        TIM2->SR &= ~TIM_SR_UIF;
		
		if((state&BLINK_STATE) == 1){ // Если начали мигать, то меняем флаг в состоянии мигания
			state ^= BLINK_STATUS;
			return;
		}
	
        Timer--;
		if(Timer == 0) state = 1; // Если закончился таймер, то начинаем мигать 
}