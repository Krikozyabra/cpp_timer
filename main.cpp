#define PORTA
#define PORTB
#define PORTC
#include "PortInterface.h"

#include "stm32f10x.h" 
#include "7seg_3_dig_timer.cpp"

using MyTimer = SSEGFDIGTimer<Pb10, Pb14, Pb7, Pb5, Pb15, Pb1, Pb8, Pb6, Pb11, Pb0, Pb13, Pb9>;

extern "C" void TIM2_IRQHandler(void);
extern "C" void EXTI9_5_IRQHandler(void);
extern "C" void EXTI4_IRQHandler(void);

uint16_t ResetTimer = 60; // стартовое значение таймера
uint16_t volatile Timer = ResetTimer;

uint8_t volatile state = 0; // состояние 0b(settings_status)(settings_status)(blink_status)(settings_state)(blink_state)(ss_state)
#define STARTSTOP_STATE 0x1
#define BLINK_STATE 0x2
#define SETTINGS_STATE 0x4
#define BLINK_STATUS 0x8
#define SETTINGS_STATUS 0x30
#define SETTINGS_INCREMENTOR 0x10

int main(void){
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN; // 0x10 clock enabled for port C
	
	Pb4::clearOdr();
	
	Pc13::setMode(MODE_O2);
	Pc13::setCnf(CNF_OPP);
	Pc13::setOdr()
	
	Pa4::setMode(MODE_I);
	Pa4::setCnf(CNF_IPP);
	GPIOA->ODR |= GPIO_ODR_ODR4;
	
	Pa5::setMode(MODE_I);
	Pa5::setCnf(CNF_IPP);
	GPIOA->ODR |= GPIO_ODR_ODR5; // включение pull-up mode
	
	AFIO->EXTICR[1] &= ~AFIO_EXTICR2_EXTI4;
	AFIO->EXTICR[1] &= ~AFIO_EXTICR2_EXTI5; // подключаем прерывания для пятой линии по порту A
	
	EXTI->IMR |= EXTI_IMR_MR4;
	EXTI->FTSR |= EXTI_FTSR_TR4;
	EXTI->RTSR &= ~EXTI_RTSR_TR4;
	
	EXTI->IMR |= EXTI_IMR_MR5; // разрешаем прерывания на 5 линии
	EXTI->FTSR |= EXTI_FTSR_TR5; // включение прерываний по нисходящему краю на 5 линии
	EXTI->RTSR |= EXTI_RTSR_TR5; // включение прерываний по восходящему краю на 5 линии
	
	NVIC_SetPriority(EXTI4_IRQn, 1);
	NVIC_EnableIRQ(EXTI4_IRQn);
	
	NVIC_SetPriority(EXTI9_5_IRQn, 2); // Ставим приоритет для прерывания линий 5-9 - 1
	NVIC_EnableIRQ(EXTI9_5_IRQn); // включаем обработку прерываний на векторе линий 5-9
	
	MyTimer::initPins();
	MyTimer::initTimer();
	while(1){
		if((state&BLINK_STATUS) != 0){ // Если состояние мигания изменилось, то выключаем, иначе отрисовываем таймер
			MyTimer::allLedOff();
			continue;
		}
		MyTimer::drawTimer(Timer);
	}
}

void EXTI9_5_IRQHandler(void){
	if(EXTI->PR & EXTI_PR_PR5){
		EXTI->PR = EXTI_PR_PR5;
		if(Pa5::readIdr() == 0){ // Если это восходящий край (кнопка нажата)
			Pc13::clearOdr();
			state |= STARTSTOP_STATE;
			state &= ~BLINK_STATE & ~BLINK_STATUS;
		}else{
			
		}
	}
}

void EXTI4_IRQHandler(void){
	EXTI->PR = EXTI_PR_PR4;
	Pc13::setOdr();
	state |= BLINK_STATE;
	state &= ~STARTSTOP_STATE;
}


void TIM2_IRQHandler(void){		
		TIM2->SR &= ~TIM_SR_UIF; // сбрасываем значение, чтобы закончить выполнение прерывания
		if((state&BLINK_STATE) != 0) state ^= BLINK_STATUS; // Если начали мигать, то меняем флаг в состоянии мигания
		if((state&STARTSTOP_STATE) != 0) Timer--; // Если у нас состояние рабочего таймера, то уменьшаем значение
		if(Timer == 0) state |= BLINK_STATE; // Если закончился таймер, то начинаем мигать 
}