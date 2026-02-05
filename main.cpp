#define PORTA
#define PORTB
#define PORTC
#include "PortInterface.h"

#include "stm32f10x.h" 
#include "SLEDFSEG.h"

using MyTimer = SLEDFSEG_Driver<Pb10, Pb14, Pb7, Pb5, Pb15, Pb1, Pb8, Pb6, Pb11, Pb0, Pb13, Pb9>;

extern "C" void TIM2_IRQHandler(void);
extern "C" void EXTI9_5_IRQHandler(void);
extern "C" void EXTI4_IRQHandler(void);

uint16_t ResetTimer = 60; // стартовое значение таймера
uint16_t volatile Timer = ResetTimer;

uint8_t volatile state = 0; // состояние 0b(setseconds_up)(settings_prestate)(settings_status)(settings_status)(blink_status)(settings_state)(blink_state)(ss_state)
#define STARTSTOP_STATE 0x1
#define BLINK_STATE 0x2
#define SETTINGS_STATE 0x4
#define BLINK_STATUS 0x8
#define SETTINGS_STATUS 0x30
#define SETTINGS_PRESTATE 0x40
#define SETTINGS_INCREMENTOR 0x10
#define SETSECONDS_UP 0x80

int main(void){
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN; // 0x10 clock enabled for port C
	
	Pb4::clearOdr();
	
	Pc13::setMode(MODE_O2);
	Pc13::setCnf(CNF_OPP);
	Pc13::setOdr();
	
	Pa3::setMode(MODE_O2);
	Pa3::setCnf(CNF_OPP);
	Pa3::clearOdr();
	
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
		MyTimer::drawNumber(Timer);
	}
}

void EXTI9_5_IRQHandler(void){
	if(EXTI->PR & EXTI_PR_PR5){
		EXTI->PR = EXTI_PR_PR5;
		if(Pa5::readIdr() == 0){ // Если это восходящий край (кнопка нажата)
			if((state&SETTINGS_STATE) == 0){ // Если не в состоянии настроек
				if((state&STARTSTOP_STATE) == 0) Timer=ResetTimer; // Если таймер уже на паузе, то обнуляем состояние таймера
				Pc13::setOdr(); // Помечаем, что таймер остановлен (своего рода дебаг тул :) )
				state |= BLINK_STATE; // Включаем мигание
				state &= ~STARTSTOP_STATE; // Останавливаем таймер
			}else{
				state ^= SETSECONDS_UP;
			}
			state |= SETTINGS_PRESTATE; // Помечаем, что кнопка зажата
		}else{ // Если это нисходящий край (кнопка отжата)
			state &= ~SETTINGS_STATUS & ~SETTINGS_PRESTATE; // Обнуляем таймер до входа в\из настройки и помечаем, что кнопка отжата
		}
	}
}

void EXTI4_IRQHandler(void){
	EXTI->PR = EXTI_PR_PR4;
	if((state&SETTINGS_STATE) == 0){ // если не в состоянии настроек
		Pc13::clearOdr(); // помечаем, что таймер запустился
		state |= STARTSTOP_STATE; // меняем состояние на "таймер запущен"
		state &= ~BLINK_STATE & ~BLINK_STATUS; // сбрасываем состояние мигания
	}else{ // Если в состоянии настроек
		if((state&SETSECONDS_UP) == 0) ResetTimer--;
		else ResetTimer++;
		Timer = ResetTimer;
	}
}


void TIM2_IRQHandler(void){		
		TIM2->SR &= ~TIM_SR_UIF; // сбрасываем значение, чтобы закончить выполнение прерывания
		if((state&BLINK_STATE) != 0) { // Если начали мигать
			state ^= BLINK_STATUS;  // меняем флаг в состоянии мигания
		}
		if((state&SETTINGS_PRESTATE) != 0) state += SETTINGS_INCREMENTOR;
		if((state&STARTSTOP_STATE) != 0) Timer--; // Если у нас состояние рабочего таймера, то уменьшаем значение
		if(Timer == 0) {
			state |= BLINK_STATE; // Если закончился таймер, то начинаем мигать 
			state &= ~STARTSTOP_STATE;
		}
		if((state&SETTINGS_STATUS) == 0x30) { // Если 3 секунды прошло
			if((state&SETTINGS_STATE) == 0){ // Если не в состоянии настроек
				state &= ~SETTINGS_PRESTATE & ~SETTINGS_STATUS & ~BLINK_STATE & ~BLINK_STATUS; // сбрасываем пресостояние и секунды удержания
				state |= SETTINGS_STATE; // переходим в состояние настройки
				Timer = ResetTimer;
				Pa3::setOdr();
			}else{ // Если в состоянии настроек
				state &= ~SETTINGS_PRESTATE & ~SETTINGS_STATUS & ~SETTINGS_STATE; // сбрасываем пресостояние, таймер до смены состояния настроек, сбрасываем состояние настроек
				state |= BLINK_STATE; // переходим в режим мигания
				Pa3::clearOdr();
			}
		}
}