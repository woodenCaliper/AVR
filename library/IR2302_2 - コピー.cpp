
#ifndef IR2302_H_
#define IR2302_H_
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>	//割り込みヘッダ

#include ".\BIT_CTRL.cpp"
#include ".\TIMER_CTRL.cpp"
#include ".\TIMER_INTERRUPT.cpp"
#include ".\TIMER0N_COUNTER.cpp"

/*
IN1	SD1	HO	LO
0	0	0	0	OFF
0	1	0	1	LOW_SIDE_ON
1	0	0	0×	使用しない(OFF)
1	1	1	0	HIGH_SIDE_ON

IN1	SD1 IN2	SD2	|	HO1	LO1	HO2	LO2 |	M1	M2
1	1	0	1	|	1	0	0	1	|	1	0	doTurn
0	1	0	0	|	0	1	0	0	|	0	N	doTurnCharge
0	1	1	1	|	0	1	1	0	|	0	1	doReturn
0	0	0	1	|	0	0	0	1	|	N	0	doReturnCharge
0	0	0	0	|	0	0	0	0	|	N	N	doNeutral
0	1	0	1	|	0	1	0	1	|	0	0	doBrake
*/


#define NEUTRAL	0
#define TURN	1
#define RETURN	2
#define BRAKE	3


class Ir2302Tm0{
	public:
	volatile int8_t outState;
	avrPin in1, in2, sd;
	volatile float maxDuty;
	volatile uint8_t maxOCR;
	volatile float currentDuty, slope, goalDuty, startTime;

	Ir2302Tm0(){
		outState = NEUTRAL;
		maxOCR = 254;
		maxDuty = maxOCR/255.0*100.0;	//99.6078%
		currentDuty = 0;
		slope = 0.0;
		goalDuty = 0;
	}
//virtural>>>>>
	virtual void setOCR(uint8_t num){
		//OCRxB = num;
	}
	virtual uint8_t getOCR(){
		//return OCRxB;
		return 0;
	}
//<<<<<virtural
	void setTimer(uint16_t division){
		Tm0Ctrl::setDivision(division);
		Tm0Ctrl::setWGM(0b000);	//標準動作
		Tm0Cntr::begin(division);
		sei();
	}
	void setPins(avrPin _in1, avrPin _in2, avrPin _sd){
		in1 = _in1;
		in2 = _in2;
		sd = _sd;
		sbi( *(in1.ddr),in1.pinNum );
		sbi( *(in2.ddr),in2.pinNum );
		sbi( *(sd.ddr),sd.pinNum );
		doNeutral();
	}
	void doNeutral(){
		cbi( *(in1.port), in1.pinNum );
		cbi( *(in2.port), in2.pinNum );
		cbi( *(sd.port),  sd.pinNum );
	}
	void doTurn(){
		sbi( *(in1.port), in1.pinNum );
		cbi( *(in2.port), in2.pinNum );
		sbi( *(sd.port),  sd.pinNum );
	}
	void doReturn(){
		cbi( *(in1.port), in1.pinNum );
		sbi( *(in2.port), in2.pinNum );
		sbi( *(sd.port),  sd.pinNum );
	}
	void doBrake(){
		cbi( *(in1.port), in1.pinNum );
		cbi( *(in2.port), in2.pinNum );
		sbi( *(sd.port),  sd.pinNum );
	}

	inline void __onAction(){
		switch(outState){
			case TURN:
				doTurn();
			break;
			case RETURN:
				doReturn();
			break;
			case BRAKE:
				doBrake();
			break;
			default:	//case NEUTRAL:
				doNeutral();
			break;
		}
	}
	inline void __offAction(){
		switch(outState){
			case TURN:
			case RETURN:
				doBrake();
			break;
			case BRAKE:
				if(getOCR() >= maxOCR){	//OCR0Bがmaxのときだけ
					doBrake();
				}
				else{	//ふつうはこっち
					doNeutral();
				}
			break;
			default:	//case NEUTRAL:
				doNeutral();
			break;
		}
	}
	void __setDutyByte(int16_t dutyByte, bool brakeFlag){
		if(dutyByte == 0){
			outState = NEUTRAL;
		}
		else if(brakeFlag){
			outState = BRAKE;
			dutyByte = dutyByte;
		}
		else{
			dutyByte = dutyByte;
			if(dutyByte < 0){
				outState = RETURN;
				dutyByte *= -1;
			}
			else if(0 < dutyByte){
				outState = TURN;
			}
		}
		setOCR( (uint8_t)dutyByte );
	}
	void setDuty(float duty, bool brakeFlag=false){
		if(brakeFlag){
			duty = margeNum(duty, 0.0, 100.0);
			__setDutyByte(duty/100.0*255.0, true);
		}
		else{
			duty = margeNum(duty, -maxDuty, maxDuty);
			__setDutyByte(duty/100.0*255.0, false);
			currentDuty=duty;
		}
	}
	void setDutyGoal(float _goalDuty, uint16_t timeMs=0){
		//goalDuty==slope * startTimeTime + currentDuty
		if(timeMs==0){
			timeMs=1;
		}
		slope=(_goalDuty - currentDuty)/timeMs;
		startTime = Tm0Cntr::getMsec();
		goalDuty = _goalDuty;
	}
	void __updateNextDuty(){
		if(currentDuty < goalDuty){	//up
			float nextDuty = currentDuty + slope*(Tm0Cntr::getMsec()-startTime);
			if(goalDuty < nextDuty){
				nextDuty = goalDuty;
			}
			setDuty(nextDuty);
		}
		else if(goalDuty < currentDuty){	//down
			float nextDuty = currentDuty - slope*(Tm0Cntr::getMsec()-startTime);
			if(nextDuty < goalDuty){
				nextDuty = goalDuty;
			}
			setDuty(nextDuty);
		}
	}
};

class GateDriveA : public Ir2302Tm0{
	public:
	GateDriveA(){
		setTimer(64);
	}
	void begin(){
		sbi(TIMSK0, TOIE0);		//ovf割り込み
		sbi(TIMSK0, OCIE0A);	//ﾀｲﾏ/ｶｳﾝﾀ0比較B割り込み
		setDuty(0);
		sei();
	}
	void stop(){
		cbi(TIMSK0, TOIE0);		//ovfA割り込み
		cbi(TIMSK0, OCIE0A);	//ﾀｲﾏ/ｶｳﾝﾀ0比較B割り込み
	}
	void setOCR(uint8_t num){
		OCR0A = num;
	}
	uint8_t getOCR(){
		return OCR0A;
	}
}gateDriveA;

class GateDriveB : public Ir2302Tm0{
	public:
	GateDriveB(){
		setTimer(64);
	}
	void begin(){
		sbi(TIMSK0, TOIE0);		//ovf割り込み
		sbi(TIMSK0, OCIE0B);	//ﾀｲﾏ/ｶｳﾝﾀ0比較B割り込み
		setDuty(0);
		sei();
	}
	void stop(){
		cbi(TIMSK0, TOIE0);		//ovf割り込み
		cbi(TIMSK0, OCIE0B);	//ﾀｲﾏ/ｶｳﾝﾀ0比較B割り込み
	}
	void setOCR(uint8_t num){
		OCR0B = num;
	}
	uint8_t getOCR(){
		return OCR0B;
	}
}gateDriveB;


ISR(TIMER0_COMPA_vect){//OFFに当たる動作
	gateDriveA.__offAction();
}
ISR(TIMER0_COMPB_vect){//OFFに当たる動作
	gateDriveB.__offAction();
}
ISR(TIMER0_OVF_vect){//ON(回転)動作
	Tm0Cntr::upCount();
	gateDriveA.__onAction();
	gateDriveB.__onAction();
// 	gateDriveA.__updateNextDuty();
// 	gateDriveB.__updateNextDuty();
}

#endif

