#pragma once
#include "stm32f10x.h"
#include "PinInterface.h"

#define MAKE_PORT(PORT, ClassName, ID) \
	class ClassName{\
		public:\
			using DataT = uint32_t;\
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
	
	using Pb0 = TPin<PortB, 0>;
	using Pb1 = TPin<PortB, 1>;
	using Pb2 = TPin<PortB, 2>;
	using Pb3 = TPin<PortB, 3>;
	using Pb4 = TPin<PortB, 4>;
	using Pb5 = TPin<PortB, 5>;
	using Pb6 = TPin<PortB, 6>;
	using Pb7 = TPin<PortB, 7>;
	using Pb8 = TPin<PortB, 8>;
	using Pb9 = TPin<PortB, 9>;
	using Pb10 = TPin<PortB, 10>;
	using Pb11 = TPin<PortB, 11>;
	using Pb12 = TPin<PortB, 12>;
	using Pb13 = TPin<PortB, 13>;
	using Pb14 = TPin<PortB, 14>;
	using Pb15 = TPin<PortB, 15>;
#endif
#ifdef PORTC
	MAKE_PORT(GPIOC, PortC, 'C')
	
	using Pc13 = TPin<PortC, 13>;
#endif