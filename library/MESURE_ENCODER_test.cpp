/*
this class function: get angle and angular velocity from encoder
use interrupt: pin_interrupt, timer2
	pin interrupt event: for read changed encoder pulse
	timer2: for calculation angular velocity.
	 if you don't want to use timer2, you define USE_COUNTER_TIMER.
	 for example, if you want to use timer0
	 #define USE_COUNTER_TIMER 0
*/

#ifndef MESURE_ENCODER_TM2_H_
#define MESURE_ENCODER_TM2_H_

#include <avr/interrupt.h>	//割り込みヘッダ
#include ".\BIT_CTRL.cpp"

#define USE_COUNTER_TIMER 2
#include ".\COUNTER.cpp"


#ifndef PI
#define PI 3.14159265359
#endif


class Encoder;
class MeasureEncode{
	// changedPulseA(bool input){

	// }
	// changedPulseB(bool input){

	// }
	MeasureEncoder(phaseAPin, phaseBPin, uint16_t pulseParRotation){
		PinInterrupt::setInterrupt(phaseAPin, changedPulseA);
		PinInterrupt::setInterrupt(phaseBPin, changedPulseB);

		begin(pulseParRotation, readInput(phaseAPin), readInput(phaseBPin));
	}
}

namespace MeasureEncoder{
	float radParPulse;

	int32_t counts;
	uint8_t oldEncState;

	void begin(uint16_t pulseParRotation, bool Aphase, bool Bphase){
		radParPulse =  (2*PI) / (pulseParRotation*4);
		counts=0;
		oldEncState = ((uint8_t)Aphase<<1) | Bphase;
	}

	void changedPulse(bool Aphase, bool Bphase){
		uint8_t currentEncState = (Aphase<<1) | (Aphase^Bphase);	//00->01->10->11->00
		if( (oldEncState+1)&0b0011 == currentEncState){
			counts[0]++;
		}
		else if( (oldEncState-1)&0b0011 == currentEncState){
			counts[0]--;
		}
		else{	//error
			return;
		}
		oldEncState = currentEncState;
		return;
	}

	inline void changedPulseA(bool phaseA){
		changedPulse(phaseA, (bool)(oldEncState&0b0001));
	}
	inline void changedPulseB(bool phaseB){
		changedPulse((bool)(oldEncState&0b0010), phaseB);
	}
	void setCurrentRad(float rad){
		if(counts[1]==counts[0]-1){
			counts = rad / radParPulse;
		}
		else if(counts[1]==counts[0]+1){
			counts = rad / radParPulse;
		}
	}
	float getCurrentRad(){
		return counts * radParPulse;
	}
}
#endif