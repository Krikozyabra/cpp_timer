#include "SLEDFSEG.h"

template <class Pin1, class Pin2, class Pin3, class Pin4, // A, B, C, D
			class Pin5, class Pin6, class Pin7, class Pin8, // E, F, G, DP
				class Pin9, class Pin10, class Pin11, class Pin12> // D1, D2, D3, D4
class SSEGFDIGTimer : public SLEDFSEG_Driver<Pin1, Pin2, Pin3, Pin4,
													Pin5, Pin6, Pin7, Pin8,
														Pin9, Pin10, Pin11, Pin12>
{
	private: 
		using Base = SLEDFSEG_Driver<Pin1, Pin2, Pin3, Pin4,
													Pin5, Pin6, Pin7, Pin8,
														Pin9, Pin10, Pin11, Pin12>;
	
	public:
		static void initTimer(){
			RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Подключение TIM2 к генератору
			RCC->CFGR &= ~RCC_CFGR_HPRE; // Обнуление предделителя на шине AHB
			RCC->CFGR |= RCC_CFGR_HPRE_DIV8; // предделитель шины AHB на 8 (итого частота 9МГц)
			TIM2->PSC = 8999; // предделитель таймера на 9000(999+1) (итого частота 1КГц или же 1000 мс)
			TIM2->ARR = 999; // установка авторезета на 1000(999+1) (раз в секунду)
			TIM2->DIER |= TIM_DIER_UIE; // Включение прерываний на обновление ARR
			NVIC_SetPriority(TIM2_IRQn, 0); // Постановка приоритета для прерывания (чем меньше, тем выше приоритет)
			NVIC_EnableIRQ(TIM2_IRQn); // Включение обработки прерываний по обновлению таймера
			TIM2->EGR |= TIM_EGR_UG; // Генерация прерывания на обновление, дабы запустить загрузку конфигурационных данных
			TIM2->SR &= ~TIM_SR_UIF; // Обнуление вызванного прерывания
			TIM2->CR1 |= TIM_CR1_CEN;
		}

		static void drawTimer(uint16_t timer){
			Base::showOnPos(timer/1000%10,1);
			Base::showOnPos(timer/100%10,2);
			Base::showOnPos(timer/10%10,3);
			Base::showOnPos(timer%10,4);
		}
};