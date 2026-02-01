#pragma once
#include "stm32f10x.h"

#define MODE_I 0x0
#define MODE_O10 0x1
#define MODE_O2 0x2
#define MODE_50 0x3

#define CNF_OPP 0x0
#define CNF_OOD 0x1

template <class PORT, uint8_t PIN>
class TPin{
public:
	using Port = PORT;
	static constexpr uint8_t Number = PIN;
	static constexpr bool isHigh = (PIN>7);
	static constexpr uint8_t Shift = (PIN % 8) * 4; 
	static void setMode(uint8_t value){
		if(isHigh) {
			PORT::DirHClearAndSet(0x3 << Shift, value << Shift);
		} else {
			PORT::DirLClearAndSet(0x3 << Shift, value << Shift);
		}
	}
	static void setCnf(uint8_t value){
		if(isHigh) PORT::DirHClearAndSet(\
			0x3<<(Shift+2), value<<(Shift+2));
		else PORT::DirLClearAndSet(\
			0x3<<(Shift+2), value<<(Shift+2));
	}
	static void toggleOdr(){
		PORT::Toggle(1<<PIN);
	}
	static void setOdr(){
		PORT::Set(1<<PIN);
	}
	static void clearOdr(){
		PORT::Clear(1<<PIN);
	}
};