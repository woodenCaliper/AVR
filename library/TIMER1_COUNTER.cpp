//動作未確認
#ifndef TIMER1_COUNTER_H_
#define TIMER1_COUNTER_H_

#include <avr/interrupt.h>	//割り込みヘッダ
#include ".\BIT_CTRL.cpp"
#include ".\TIMER_CTRL.cpp"

namespace Tm1Cntr{
	volatile uint32_t count=0;
	void begin(int division){
		Tm1Ctrl::setDivision(division);
		Tm1Ctrl::enableOvfIrpt();
		count=0;
		TCNT1=0;
		sei();
	}
	void stop(){

	}
	uint32_t getUsec(){
		return (count*65536+TCNT1)*Tm1Ctrl::getDivision() / (F_CPU/1000000);
	}
	uint32_t getMsec(){
		return (count*65536+TCNT1)*Tm1Ctrl::getDivision() / (F_CPU/1000);
	}
	uint32_t getSec(){
		return (count*65536+TCNT1)*Tm1Ctrl::getDivision() / (F_CPU);
	}
	inline void upCount(){
		count++;
	}
}
ISR(TIMER1_OVF_vect){
	sei();
	Tm1Cntr::upCount();
}
#endif