#ifndef MEASURE_PWM_CAPT_H_
#define MEASURE_PWM_CAPT_H_

#include ".\BIT_CTRL.cpp"
#include ".\TIMER_CTRL.cpp"
#include <avr/interrupt.h>


class MeasureRc{
	public:
	float bitToUsec;
	uint16_t upTcnt, downTcnt;
	uint16_t upCount, downCount;
	uint32_t highBit, lowBit;
	bool highUpdateFlag, lowUpdateFlag;
	Pin pin;

	static uint8_t pinNum;
	static uint16_t nowCount;
	
	MeasureRc(Pin pin);
	void begin(uint16_t division);
	void end();
	float getHighUsec();
	float getLowUsec();
	bool isHighUpdate();
	bool isLowUpdate();
	inline void __setUpMode();
	inline void __setDownMode();
	void __pulseUpAction(uint16_t _upTcnt);
	void __pulseDownAction(uint16_t _downTcnt);
	
	static void __countUp();
	static bool __isUpMode();
	static bool __isDownMode();
};
uint8_t MeasureRc::pinNum=0;
uint16_t MeasureRc::nowCount=0;
MeasureRc *objPtr;

MeasureRc::MeasureRc(Pin _pin){	//コンストラクタ(変数の初期化とタイマー設定)
	objPtr = this;
	pin = _pin;
	pinNum++;
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
void MeasureRc::begin(uint16_t division=256){
	Tm1Ctrl::setDivision(division);	//精度12.8us　計測可能時間約55ks=15h
	division = Tm1Ctrl::getDivision();
	bitToUsec = division/(F_CPU/1000000.0);	//1bit=12.8us
	Tm1Ctrl::enableCaptIrpt();
	Tm1Ctrl::enableOvfIrpt();//ovf割り込み許可
	sei();
}
void MeasureRc::end(){
	Tm1Ctrl::disableCaptIrpt();
}
float MeasureRc::getHighUsec(){
	highUpdateFlag=false;
	return highBit * bitToUsec;
}
float MeasureRc::getLowUsec(){
	lowUpdateFlag=false;
	return lowBit * bitToUsec;
}
bool MeasureRc::isHighUpdate(){
	return highUpdateFlag;
}
bool MeasureRc::isLowUpdate(){
	return lowUpdateFlag;
}
void MeasureRc::__countUp(){
	MeasureRc::nowCount++;
}
inline void MeasureRc::__setUpMode(){
	sbi(TCCR1B,ICES1);
}
inline void MeasureRc::__setDownMode(){
	cbi(TCCR1B,ICES1);
}
bool MeasureRc::__isUpMode(){
	return checkBit(TCCR1B, ICES1);
}
bool MeasureRc::__isDownMode(){
	return !checkBit(TCCR1B, ICES1);
}
void MeasureRc::__pulseUpAction(uint16_t _upTcnt){
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
void MeasureRc::__pulseDownAction(uint16_t _downTcnt){
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




ISR(TIMER1_OVF_vect){
	MeasureRc::__countUp();
}
ISR(TIMER1_CAPT_vect){
	sbi(PORTD,7);
	uint16_t _icr1 = ICR1;	
	
	for(uint8_t i=0; i<MeasureRc::pinNum; i++){
		if( checkBit(objPtr->pin.port, objPtr->pin.pinNum)==1 ){
			
			objPtr->__pulseUpAction(_icr1);
		}
	}
/*
	if(MeasureRc::__isUpMode()){
		
		MeasurePwmCapt.__pulseUpAction(hoge);
		MeasurePwmCapt.__setDownMode();
	}
	else{
		MeasurePwmCapt.__pulseDownAction(hoge);
		MeasurePwmCapt.__setUpMode();
	}
*/
	cbi(PORTD,7);
}
#endif