
#ifndef IR2302_H_
#define IR2302_H_
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>	//割り込みヘッダ

#include ".\BIT_CTRL.cpp"
#include ".\TIMER_CTRL.cpp"
#include ".\TIMER_INTERRUPT.cpp"
#include ".\TIMER0_COUNTER.cpp"

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
	int8_t outState;
	volatile uint8_t	*IN1_DDR,  *SD1_DDR,  *IN2_DDR,  *SD2_DDR;
	volatile uint8_t	*IN1_PORT, *SD1_PORT, *IN2_PORT, *SD2_PORT;
	uint8_t				 IN1_PIN,   SD1_PIN,   IN2_PIN,   SD2_PIN;
	uint8_t maxDuty;
	float currentDuty, slope, goalDuty, startTime;

	Ir2302Tm0(){
		outState = NEUTRAL;
		maxDuty=254.0/255.0*100.0;	//99.6078%
		currentDuty=0;
		slope = 0.0;
		goalDuty=0;

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
	static void setTimer(uint16_t division){
		Tm0Ctrl::setDivision(division);
		Tm0Ctrl::setWGM(0b000);	//標準動作
		Tm0Cntr::start(division);
	}

	void setIn1(volatile uint8_t *DDR, volatile uint8_t *PORT,  uint8_t PinNum){
		sbi(*DDR, PinNum);
		cbi(*PORT, PinNum);
		IN1_PORT = PORT;
		IN1_PIN  = PinNum;
	}
	void setSd1(volatile uint8_t *DDR, volatile uint8_t *PORT,  uint8_t PinNum){
		sbi(*DDR, PinNum);
		cbi(*PORT, PinNum);
		SD1_PORT = PORT;
		SD1_PIN  = PinNum;
	}
	void setIn2(volatile uint8_t *DDR, volatile uint8_t *PORT,  uint8_t PinNum){
		sbi(*DDR, PinNum);
		cbi(*PORT, PinNum);
		IN2_PORT = PORT;
		IN2_PIN  = PinNum;
	}
	void setSd2(volatile uint8_t *DDR, volatile uint8_t *PORT,  uint8_t PinNum){
		sbi(*DDR, PinNum);
		cbi(*PORT, PinNum);
		SD2_PORT = PORT;
		SD2_PIN  = PinNum;
	}
	void doNeutral(){
		cbi(*IN1_PORT, IN1_PIN);
		cbi(*SD1_PORT, SD1_PIN);
		cbi(*IN2_PORT, IN2_PIN);
		cbi(*SD2_PORT, SD2_PIN);
	}
	void doTurn(){
		sbi(*IN1_PORT, IN1_PIN);
		sbi(*SD1_PORT, SD1_PIN);
		cbi(*IN2_PORT, IN2_PIN);
		sbi(*SD2_PORT, SD2_PIN);
	}
	void doTurnCharge(){
		cbi(*IN1_PORT, IN1_PIN);
		sbi(*SD1_PORT, SD1_PIN);
		cbi(*IN2_PORT, IN2_PIN);
		cbi(*SD2_PORT, SD2_PIN);
	}
	void doReturn(){
		cbi(*IN1_PORT, IN1_PIN);
		sbi(*SD1_PORT, SD1_PIN);
		sbi(*IN2_PORT, IN2_PIN);
		sbi(*SD2_PORT, SD2_PIN);
	}
	void doReturnCharge(){
		cbi(*IN1_PORT, IN1_PIN);
		cbi(*SD1_PORT, SD1_PIN);
		cbi(*IN2_PORT, IN2_PIN);
		sbi(*SD2_PORT, SD2_PIN);
	}
	void doBrake(){
		cbi(*IN1_PORT, IN1_PIN);
		sbi(*SD1_PORT, SD1_PIN);
		cbi(*IN2_PORT, IN2_PIN);
		sbi(*SD2_PORT, SD2_PIN);
	}

	void __onAction(){
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
	void __offAction(){
		switch(outState){
			case TURN:
			case RETURN:
				doBrake();
			break;
			case BRAKE:
				if(getOCR() >= maxDuty){	//OCR0Bがmaxのときだけ
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
			currentDuty = nextDuty;
		}
		else if(goalDuty < currentDuty){	//down
			float nextDuty = currentDuty - slope*(Tm0Cntr::getMsec()-startTime);
			if(nextDuty < goalDuty){
				nextDuty = goalDuty;
			}
			setDuty(nextDuty);
			currentDuty = nextDuty;
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
	gateDriveA.__updateNextDuty();
	gateDriveB.__updateNextDuty();
}



#endif

