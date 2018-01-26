#ifndef MEASURE_PWM_INT_H_
#define MEASURE_PWM_INT_H_

#include ".\BIT_CTRL.cpp"
#include ".\TIMER_CTRL.cpp"
#include <avr/interrupt.h>

class MeasurePwmInt{
	protected:
	uint16_t upTcnt, downTcnt;
	uint16_t upCount, downCount;
	float bitUsec;
	uint16_t nowCount;
	uint16_t lowCount, lowTcnt;
	uint16_t highCount, highTcnt;
	bool highTimeUpdateFlag, lowTimeUpdateFlag;
	public:
	float highUsec, lowUsec;

	MeasurePwmInt(){	//コンストラクタ(変数の初期化とタイマー設定)
		bitUsec=0.0;
		nowCount=0;
		upTcnt=0;
		upCount=0;
		downTcnt=0;
		downCount=0;
		highUsec=0;
		highCount=0;
		highTcnt=0;
		highTimeUpdateFlag=false;
		lowUsec=0;
		lowCount=0;
		lowTcnt=0;
		lowTimeUpdateFlag=false;
		setTimer(256);
	}
	void setTimer(uint16_t division){
		Tm1Ctrl::setDivision(division);	//精度12.8us　計測可能時間約55ks=15h
		Tm1Ctrl::setWGM(0b000);
		bitUsec = division/(F_CPU/1000000.0);	//1bit=12.8us
		sbi(TIMSK1, TOIE1);	//ovf割り込み許可
		sei();
	}
	inline void __countUp(){
		nowCount++;
	}
	float getHighUsec(){
		highTimeUpdateFlag=false;
		return calcTime(highCount, highTcnt);
	}
	float getLowUsec(){
		lowTimeUpdateFlag=false;
		return calcTime(lowCount, lowTcnt);
	}
	void __pulseUpAction(uint16_t tcnt){
		lowTcnt  = tcnt -downTcnt;
		lowCount = nowCount-downCount;
		if(tcnt<downTcnt){
			lowCount--;
		}
		lowTimeUpdateFlag = true;
		upTcnt  = tcnt;
		upCount = nowCount;

	//	lowUsec = ( (((int32_t)nowCount-(int32_t)downCount+65536)%65536)*65536.0 + (((int32_t)upTcnt-(int32_t)downTcnt+65536)%65536) )*bitUsec;
	//	lowUsec = ( (uint16_t)(nowCount-downCount)*65536.0 + (uint16_t)(upTcnt-downTcnt) )*bitUsec;
	}
	void __pulseDownAction(uint16_t tcnt){
		highTcnt  = tcnt-upTcnt;
		highCount = nowCount-upCount;
		if(tcnt < upTcnt){
			highCount--;
		}
		highTimeUpdateFlag = true;
		downTcnt  = tcnt;
		downCount = nowCount;

	//	highUsec = ( (((int32_t)nowCount-(int32_t)upCount+65536)%65536)*65536.0 + (((int32_t)downTcnt-(int32_t)upTcnt+65536)%65536) )*bitUsec;
	//	highUsec = ( (uint16_t)(nowCount-upCount)*65536.0 + (uint16_t)(downTcnt-upTcnt) )*bitUsec;
	}
	bool isHighUpdate(){
		return highTimeUpdateFlag;
	}
	bool isLowUpdate(){
		return lowTimeUpdateFlag;
	}
	float calcTime(uint16_t count, uint16_t tcnt){
		return (count*65536.0 + (float)tcnt)*bitUsec;
	}
};


class measureInt0 : public MeasurePwmInt{
	public:
	measureInt0(){
	}
	//lowMax、highMaxは計測の最大値、0ならば設定しない
	void begin(){
		cbi(DDRD, 2);
		sbi(PORTD,2);
		sbi(EICRA, ISC00);
		sbi(EICRA, ISC01);
		sbi(EIMSK, INT0);
		sei();
	}
	void stop(){
		cbi(EIMSK, INT0);
	}
	inline bool isPulseUp(){
		return (EICRA&0b00000011) == 0b00000011;
		//return (checkBit(EICRA,ISC01)==1 && checkBit(EICRA,ISC00)==1);
	}
	inline bool isPulseDown(){
		return (EICRA&0b00000011) == 0b00000010;
		//return (checkBit(EICRA,ISC01)==1 && checkBit(EICRA,ISC00)==0);
	}
}measureInt0;

class measureInt1 : public MeasurePwmInt{
	public:
	measureInt1(){
	}
	void begin(){
		cbi(DDRD, 3);
		sbi(PORTD,3);
		sbi(EICRA, ISC10);
		sbi(EICRA, ISC11);
		sbi(EIMSK, INT1);
		sei();
	}
	void stop(){
		cbi(EIMSK, INT1);
	}
	inline bool isPulseUp(){
		return (EICRA&0b00001100) == 0b00001100;
	//	return (checkBit(EICRA,ISC11)==1 && checkBit(EICRA,ISC10)==1);
	}
	inline bool isPulseDown(){
		return (EICRA&0b00001100) == 0b00001000;
	//	return (checkBit(EICRA,ISC11)==1 && checkBit(EICRA,ISC10)==0);
	}
}measureInt1;

ISR(INT0_vect){
	uint16_t _tcnt = TCNT1;
	if(measureInt0.isPulseUp()){	//立ち上がり
		measureInt0.__pulseUpAction(_tcnt);
		cbi(EICRA, ISC00);	//立ち下りで割り込み設定
	}
	else if(measureInt0.isPulseDown()){	//立ち下がり
		measureInt0.__pulseDownAction(_tcnt);
		sbi(EICRA, ISC00);
	}

}
ISR(INT1_vect){
	uint16_t _tcnt = TCNT1;
	if(measureInt1.isPulseUp()){	//立ち上がり
		measureInt1.__pulseUpAction(_tcnt);
		cbi(EICRA, ISC10);	//立ち下りで割り込み設定
	}
	else if(measureInt1.isPulseDown()){	//立ち下がり
		measureInt1.__pulseDownAction(_tcnt);
		sbi(EICRA, ISC10);
	}
}

ISR(TIMER1_OVF_vect){
	sei();
	measureInt0.__countUp();
	measureInt1.__countUp();
}
#endif