//動作未確認
#ifndef COUNTER_H_
#define COUNTER_H_

#include <avr/interrupt.h>	//割り込みヘッダ
#include ".\BIT_CTRL.cpp"
#include ".\TIMER_CTRL.cpp"

#ifndef USE_COUNTER_TIMER
#define USE_COUNTER_TIMER 2
#endif

namespace Counter{
	bool isStarted = false;

	#if USE_COUNTER_TIMER==0
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	volatile uint32_t count=0;
	void begin(int division=64){
		Tm0Ctrl::setDivision(division);
		Tm0Ctrl::enableOvfIrpt();
		count=0;
		TCNT0=0;
		isStarted=true;
		sei();
	}
	void stop(){
		isStarted=false;

	}
	float getUsec(){
		return (count*256.0+TCNT0)*Tm0Ctrl::getDivision() / (F_CPU/1000000.0);
	}
	float getMsec(){
		return (count*256.0+TCNT0)*Tm0Ctrl::getDivision() / (F_CPU/1000.0);
	}
	float getSec(){
		return (count*256.0+TCNT0)*Tm0Ctrl::getDivision() / (F_CPU);
	}
	inline void countUp(){
		count++;
	}
	ISR(TIMER0_OVF_vect){
		countUp();
	}
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	#elif USE_COUNTER_TIMER==1
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	volatile uint32_t count=0;
	void begin(int division=64){
		Tm1Ctrl::setDivision(division);
		Tm1Ctrl::enableOvfIrpt();
		count=0;
		TCNT1=0;
		isStarted=true;
		sei();
	}
	void stop(){
		isStarted=false;
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
	inline void countUp(){
		count++;
	}
	ISR(TIMER1_OVF_vect){
		countUp();
	}
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	#else
	//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	volatile uint32_t count=0;
	void begin(int division=64){
		Tm2Ctrl::setDivision(division);
		Tm2Ctrl::enableOvfIrpt();
		count=0;
		TCNT2=0;
		isStarted=true;
		sei();
	}
	void stop(){
		isStarted=false;
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
	inline void countUp(){
		count++;
	}
	ISR(TIMER2_OVF_vect){
		countUp();
	}
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	#endif
}
#endif	//ifndef COUNTER_H_