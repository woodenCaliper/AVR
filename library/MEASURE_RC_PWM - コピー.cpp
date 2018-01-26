#ifndef MEASURE_PWM_CAPT_H_
#define MEASURE_PWM_CAPT_H_

#include ".\BIT_CTRL.cpp"
#include ".\TIMER_CTRL.cpp"
#include <avr/interrupt.h>

class MeasureRc{
	protected:
	float bitToUsec;
	uint16_t upTcnt, downTcnt;
	uint16_t upCount, downCount, nowCount;
	uint32_t highBit, lowBit;
	bool highUpdateFlag, lowUpdateFlag;
	static Pin measurePins[32];
	static uint8_t pinNum;
	public:

	MeasureRc(){	//コンストラクタ(変数の初期化とタイマー設定)
		nowCount=0;
		upTcnt=0;
		upCount=0;
		downTcnt=0;
		downCount=0;
		highBit=0;
		highUpdateFlag=false;
		lowBit=0;
		lowUpdateFlag=false;
	}
	void begin(uint16_t division=256){
		Tm1Ctrl::setDivision(division);	//精度12.8us　計測可能時間約55ks=15h
		division = Tm1Ctrl::getDivision();
		bitToUsec = division/(F_CPU/1000000.0);	//1bit=12.8us
		Tm1Ctrl::enableCaptIrpt();
		Tm1Ctrl::enableOvfIrpt();//ovf割り込み許可
		sei();
	}
	void end(){
		Tm1Ctrl::disableCaptIrpt();
	}
	float getHighUsec(){
		highUpdateFlag=false;
		return highBit * bitToUsec;
	}
	float getLowUsec(){
		lowUpdateFlag=false;
		return lowBit * bitToUsec;
	}
	bool isHighUpdate(){
		return highUpdateFlag;
	}
	bool isLowUpdate(){
		return lowUpdateFlag;
	}

	inline void __countUp(){
		nowCount++;
	}
	inline void __setUpMode(){
		sbi(TCCR1B,ICES1);
	}
	inline void __setDownMode(){
		cbi(TCCR1B,ICES1);
	}
	inline bool __isUpMode(){
		return checkBit(TCCR1B, ICES1);
	}
	inline bool __isDownMode(){
		return !checkBit(TCCR1B, ICES1);
	}
	void __pulseUpAction(uint16_t _upTcnt){
		upCount = nowCount;
		uint16_t upTcnt  = _upTcnt;
		if(downTcnt <= upTcnt){
			lowBit = (uint32_t)(upCount-downCount  )*65536 + upTcnt      -downTcnt;
		}
		else{
			lowBit = (uint32_t)(upCount-downCount-1)*65536 + upTcnt+65536-downTcnt;
		}
		lowUpdateFlag = true;
	}
	void __pulseDownAction(uint16_t _downTcnt){
		downCount = nowCount;
		downTcnt  = _downTcnt;
		if(upTcnt <= downTcnt){
			highBit = (uint32_t)(downCount-upCount  )*65536 + downTcnt      -upTcnt;
		}
		else{
			highBit = (uint32_t)(downCount-upCount-1)*65536 + downTcnt+65536-upTcnt;
		}
		highUpdateFlag = true;
	}
}MeasurePwmCapt;


ISR(TIMER1_OVF_vect){
	MeasurePwmCapt.__countUp();
}

ISR(TIMER1_CAPT_vect){
	sbi(PORTD,7);
	uint16_t hoge = ICR1;

	if(MeasurePwmCapt.__isUpMode()){
		MeasurePwmCapt.__pulseUpAction(hoge);
		MeasurePwmCapt.__setDownMode();
	}
	else{
		MeasurePwmCapt.__pulseDownAction(hoge);
		MeasurePwmCapt.__setUpMode();
	}
	cbi(PORTD,7);
}
#endif