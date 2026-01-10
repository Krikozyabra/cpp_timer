#pragma once
#include "PortInterface.h"
#define TEST 0x1
template <class PORT, unsigned char PIN>
class TPin{
	public:
		typedef PORT Port;
		enum{Number=PIN};
		static const bool isHigh = (PIN>7);
		static void setMode(uint8_t value){
			if(isHigh) PORT::DirHClearAndSet(0x3<<((PIN-8)*4), \
				value<<((PIN-8)*4));
			else PORT::DirLClearAndSet(0x3<<((PIN)*4), \
				value<<(PIN*4));
		}
		static void setCnf(uint8_t value){
			if(isHigh) PORT::DirHClearAndSet(\
				0x3<<(((PIN-8)*4)+2), value<<(((PIN-8)*4)+2));
			else PORT::DirLClearAndSet(\
				0x3<<(((PIN)*4)+2), value<<(((PIN)*4)+2));
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