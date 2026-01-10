#define PORTC
#include "PortInterface.h"
#include "PinInterface.h"

typedef TPin<PortC, 13> Pc13;

int main(void){
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // 0x10 clock enabled for port C
	
	Pc13::setMode(0b10); // MODE = 0b10 - OUTPUT 2MhZ 	CNF = 0b00 - General purpose output Push-pull
	
	while(1){
		Pc13::toggleOdr(); // set 13 port in logical 1
		for (int i=0; i<1000000; i++){
			__asm("nop");
		} // synthetic timeout
		Pc13::toggleOdr();
		for (int i=0; i<1000000; i++){
			__asm("nop");
		}
	}
}