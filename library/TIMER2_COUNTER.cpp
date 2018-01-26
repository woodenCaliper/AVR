//動作未確認
#ifndef TIMER2_COUNTER_H_
#define TIMER2_COUNTER_H_

#include <avr/interrupt.h>	//割り込みヘッダ
#include ".\BIT_CTRL.cpp"
#include ".\TIMER_CTRL.cpp"

namespace Tm2Cntr{
	volatile uint32_t count=0;
	void begin(int division){
		Tm2Ctrl::setDivision(division);
		Tm2Ctrl::enableOvfIrpt();
		count=0;
		TCNT2=0;
		sei();
	}
	void stop(){

	}
	uint32_t getUsec(){
		return (count*256+TCNT2)*Tm2Ctrl::getDivision() / (F_CPU/1000000);
	}
	uint32_t getMsec(){
		return (count*256+TCNT2)*Tm2Ctrl::getDivision() / (F_CPU/1000);
	}
	uint32_t getSec(){
		return (count*256+TCNT2)*Tm2Ctrl::getDivision() / (F_CPU);
	}
	inline void upCount(){
		count++;
	}
}
ISR(TIMER2_OVF_vect){
	sei();
	Tm2Cntr::upCount();
}
#endif