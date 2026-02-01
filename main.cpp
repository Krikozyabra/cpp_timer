#define PORTB
#define PORTC
#include "PortInterface.h"

#include "stm32f10x.h" 
#include "SLEDFSEG.h"

using MyLed = SLEDFSEG<Pb10, Pb14, Pb7, Pb5, Pb15, Pb1, Pb8, Pb6, Pb11, Pb0, Pb13, Pb9>;

extern "C" void TIM2_IRQHandler(void);

int main(void){
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPBEN; // 0x10 clock enabled for port C
	
	//RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Подключение TIM2 к внутреннему таймеру
	//RCC->CFGR |= 0xA0; // pre division by 8 
	
	Pb4::clearOdr();
	
	Pc13::setMode(MODE_O2);
	Pc13::setCnf(CNF_OPP);
	
	MyLed::init();
	
	//TIM2->PSC = 999; // pre scaler by 4500 (4499 + 1)
	//TIM2->ARR = 999; // 250 мс
	//TIM2->DIER |= TIM_DIER_UIE; // Включение прерываний на обновление ARR
	//NVIC_EnableIRQ(TIM2_IRQn); // Включение обработки прерываний по обновлению таймера
	//NVIC_SetPriority(TIM2_IRQn, 0); // Постановка приоритета для прерывания (чем меньше, тем выше приоритет)
	//TIM2->CR1 |= TIM_CR1_CEN; // включение таймера
	Pc13::clearOdr();
	while(1){
		for(uint8_t i = 0; i<4; i++) MyLed::showOnPos(i, i+1);
	}
}

//void TIM2_IRQHandler(void){
        // !!! САМОЕ ВАЖНОЕ: Сбрасываем флаг !!!
        //TIM2->SR &= ~TIM_SR_UIF;
        
        //Pc13::toggleOdr(); 
//}