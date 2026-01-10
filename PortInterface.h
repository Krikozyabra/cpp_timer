#pragma once
#include "stm32f10x.h"

#define MAKE_PORT(PORT, ClassName, ID) \
	class ClassName{\
		public:\
			typedef uint32_t DataT;\
		public: \
			static void DirHWrite(DataT value){\
				PORT->CRH = value;\
			}\
			static void DirLWrite(DataT value){\
				PORT->CRL = value;\
			}\
			static DataT DirHRead(){\
				return PORT->CRH;\
			}\
			static DataT DirLRead(){\
				return PORT->CRL;\
			}\
			static void DirHToggle(DataT value){\
				PORT->CRH ^= value;\
			}\
			static void DirLToggle(DataT value){\
				PORT->CRL ^= value;\
			}\
			static void DirHClear(DataT value){\
				PORT->CRH &= ~value; \
			}\
			static void DirLClear(DataT value){\
				PORT->CRL &= ~value; \
			}\
			static void DirHSet(DataT value){\
				PORT->CRH |= value; \
			}\
			static void DirLSet(DataT value){\
				PORT->CRL |= value; \
			}\
			static void DirHClearAndSet(DataT clearMask, DataT setMask){\
				PORT->CRH = (PORT->CRH & ~clearMask) | setMask; \
			}\
			static void DirLClearAndSet(DataT clearMask, DataT setMask){\
				PORT->CRL = (PORT->CRL & ~clearMask) | setMask; \
			}\
			static void Write(DataT value){\
				PORT->ODR = value;\
			}\
			static DataT Read(){\
				return PORT->ODR;\
			}\
			static void Toggle(DataT value){\
				PORT->ODR ^= value;\
			}\
			static void Clear(DataT value){\
				PORT->ODR &= ~value;\
			}\
			static void ClearAndSet(DataT clearMask, DataT setMask){\
				PORT->ODR = (PORT->ODR & ~clearMask) | setMask;\
			}\
			static void Set(DataT value){\
				PORT->ODR |= value;\
			}\
			static DataT PinRead(){\
				return PORT->IDR;\
			}\
			enum{Id=ID};\
			enum{Width=32};\
	};

#ifdef PORTA
	MAKE_PORT(GPIOA, PortA, 'A')
#endif
#ifdef PORTB
	MAKE_PORT(GPIOB, PortB, 'B')
#endif
#ifdef PORTC
	MAKE_PORT(GPIOC, PortC, 'C')
#endif