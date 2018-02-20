//動作未確認
#ifndef MESURE_ENCODER_TM2_H_
#define MESURE_ENCODER_TM2_H_

#include <avr/interrupt.h>	//割り込みヘッダ
#include ".\BIT_CTRL.cpp"
#include ".\COUNTER.cpp"


#ifndef PI
#define PI 3.14159265359
#endif


namespace MeasureEncoder{
	float radParPulse;

	float times[2];
	int32_t counts[2];
	uint8_t oldEncState;

	void begin(uint16_t pulseNum, bool Aphase, bool Bphase, float differentialTime=100){
		uint16_t timerDivision=64;
		radParPulse =  (2*PI) / (pulseNum*4);

		for(uint8_t i=0;i<2;i++){
			times[i]=Counter::getUsec();
			counts[i]=i;
		}
		oldEncState = ((uint8_t)Aphase<<1) | Bphase;
		Counter::begin(timerDivision);
	}
	void changedPulse(bool Aphase, bool Bphase){
		float time = Counter::getUsec();

		uint8_t encRotationState = (((uint8_t)Bphase<<1) | Aphase) ^ oldEncState;	//00:前と同じ、01:正転、10:逆転、11:値が飛んでいる
		if(encRotationState  == 0b01 || encRotationState == 0b10){
			times[1]  = times[0];
			times[0]  = time;
			counts[1] = counts[0];
			if(encRotationState == 0b01){
				counts[0]++;
			}
			else if(encRotationState == 0b10){
				counts[0]--;
			}
			oldEncState = (Aphase<<1) | Bphase;
		}
	}
	inline void changedPulseA(bool phaseA){
		changedPulse(phaseA, (bool)(oldEncState&0b00000001));
	}
	inline void changedPulseB(bool phaseB){
		changedPulse((bool)(oldEncState&0b00000010), phaseB);
	}
	void setCurrentRad(float rad){
		if(counts[1]==counts[0]-1){
			counts[0] = rad / radParPulse;
			counts[1] = counts[0]-1;
		}
		else if(counts[1]==counts[0]+1){
			counts[0] = rad / radParPulse;
			counts[1] = counts[0]+1;
		}
	}
	float getCurrentRad(){
		return counts[0] * radParPulse;
	}
	float getCurrentVel(){
		return (counts[0]-counts[1]) / (Counter::getUsec()-times[1]);
	}
}

#endif