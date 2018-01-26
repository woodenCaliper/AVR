
#ifndef IR2302_H_
#define IR2302_H_
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>	//割り込みヘッダ

#include ".\BIT_CTRL.cpp"
#include ".\TIMER_CTRL.cpp"
#include ".\TIMER_INTERRUPT.cpp"
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


	Ir2302Tm0(){
		outState = NEUTRAL;
		maxDuty=254;
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

	void onAction(){
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
	void offAction(){
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

	void setDutyByte(int16_t dutyByte, bool brakeFlag=false){
		if(dutyByte == 0){
			outState = NEUTRAL;
		}
		else if(brakeFlag){
			outState = BRAKE;
			dutyByte = margeNum(dutyByte, 1, 255);
		}
		else{
			dutyByte = margeNum(dutyByte, -maxDuty, maxDuty);
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
		setDutyByte(duty*255/100, brakeFlag);
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
		setDutyByte(0);
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
		setDutyByte(0);
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
	gateDriveA.offAction();
}
ISR(TIMER0_COMPB_vect){//OFFに当たる動作
	gateDriveB.offAction();
}

ISR(TIMER0_OVF_vect){//ON(回転)動作
	gateDriveA.onAction();
	gateDriveB.onAction();
}


#endif

