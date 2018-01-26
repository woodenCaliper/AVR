
#ifndef IR2302_H_
#define IR2302_H_
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>	//Š„‚èž‚Ýƒwƒbƒ_

#include ".\BIT_CTRL.cpp"
#include ".\TIMER_CTRL.cpp"
#include ".\TIMER_INTERRUPT.cpp"
#include ".\TIMER0N_COUNTER.cpp"

/*
IN1	SD1	HO	LO
0	0	0	0	OFF
0	1	0	1	LOW_SIDE_ON
1	0	0	0~	Žg—p‚µ‚È‚¢(OFF)
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
	struct motorState{
		float duty;
		uint8_t state;
	};
	
	avrPin in1, in2, sd;
	volatile float maxDuty;
	volatile uint8_t MAX_OCR;
	volatile float slope, startTime, startDuty;
	
	motorState current, goal;
	

	Ir2302Tm0(){
		current.duty = 0.0;
		current.state = NEUTRAL;
		goal.duty = 0.0;
		goal.state = NEUTRAL;
		
		maxDuty = MAX_OCR/255.0*100.0;	//99.6078%			
		slope = 0.0;
		MAX_OCR = 254;
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
		Tm0Ctrl::setWGM(0b000);	//•W€“®ì
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
		__doNeutral();
	}
	void __doNeutral(){
		cbi( *(in1.port), in1.pinNum );		
		cbi( *(in2.port), in2.pinNum );		
		cbi( *(sd.port),  sd.pinNum );
	}
	void __doTurn(){
		sbi( *(in1.port), in1.pinNum );
		cbi( *(in2.port), in2.pinNum );
		sbi( *(sd.port),  sd.pinNum );
	}
	void __doReturn(){
		cbi( *(in1.port), in1.pinNum );
		sbi( *(in2.port), in2.pinNum );
		sbi( *(sd.port),  sd.pinNum );
	}
	void __doBrake(){
		cbi( *(in1.port), in1.pinNum );
		cbi( *(in2.port), in2.pinNum );
		sbi( *(sd.port),  sd.pinNum );
	}

	inline void __onAction(){
		switch(current.state){
			case TURN:
				__doTurn();
			break;
			case RETURN:
				__doReturn();
			break;
			case BRAKE:
				__doBrake();
			break;
			default:	//case NEUTRAL:
				__doNeutral();
			break;
		}
	}
	inline void __offAction(){
		switch(current.state){
			case TURN:
			case RETURN:
				__doBrake();
			break;
			case BRAKE:
				if(getOCR() >= MAX_OCR){	//OCR0B‚ªmax‚Ì‚Æ‚«‚¾‚¯
					__doBrake();
				}
				else{	//‚Ó‚Â‚¤‚Í‚±‚Á‚¿
					__doNeutral();
				}
			break;
			default:	//case NEUTRAL:
				__doNeutral();
			break;
		}
	}
	
	inline uint8_t dutyToOCR(float duty){
		return (uint8_t)(duty/100.0*255.0);
	}
	inline float OCRToDuty(uint8_t ocr){
		return (ocr*100.0/255.0);
	}
	inline motorState toMotorState(float duty, bool brakeFlag){
		motorState ms;
		ms.duty = duty;
		if(duty == 0.0){
			ms.state = NEUTRAL;
		}
		else if(brakeFlag){
			ms.state = BRAKE;
		}
		else if(duty >= 0){
			ms.state = TURN;
		}
		else{	//else if(0 < dutyByte){
			ms.state = RETURN;
		}
		return ms;
	}

	inline void __updateOCR(motorState next){
		current = next;
		setOCR( dutyToOCR( abs(next.duty) ) );
	}

	inline motorState __calcNextFromGoal(){	
		float duty = slope*(Tm0Cntr::getMsec()-startTime) + startDuty;		
		if(slope>=0 && duty>=goal.duty){
			duty = goal.duty;
		}
		else if(slope<=0 && duty<=goal.duty){
			duty = goal.duty;
		}
		motorState next;
		next.duty  = duty;
		next.state = goal.state;
		return next;
	}

	//ƒ†[ƒU[
	void setDuty(float goalDuty, uint16_t timeMs=0, bool brakeFlag=false){
		goal = toMotorState(goalDuty, brakeFlag);
		if(timeMs<=1){
			timeMs=1;
		}
		startDuty = OCRToDuty( getOCR() );
		slope = (goalDuty - startDuty)/timeMs;
		startTime = Tm0Cntr::getMsec();
	}
	void setDuty(float goalDuty, bool brakeFlag=false){
		setDuty(goalDuty, 0, brakeFlag);
	}
};

class GateDriveA : public Ir2302Tm0{
	public:
	GateDriveA(){
		setTimer(64);
	}
	void begin(){
		sbi(TIMSK0, TOIE0);		//ovfŠ„‚èž‚Ý
		sbi(TIMSK0, OCIE0A);	//À²Ï/¶³ÝÀ0”äŠrBŠ„‚èž‚Ý
		setDuty(0,0,false);
		sei();
	}
	void stop(){
		cbi(TIMSK0, TOIE0);		//ovfAŠ„‚èž‚Ý
		cbi(TIMSK0, OCIE0A);	//À²Ï/¶³ÝÀ0”äŠrBŠ„‚èž‚Ý
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
		sbi(TIMSK0, TOIE0);		//ovfŠ„‚èž‚Ý
		sbi(TIMSK0, OCIE0B);	//À²Ï/¶³ÝÀ0”äŠrBŠ„‚èž‚Ý
		setDuty(0,0,false);
		sei();
	}
	void stop(){
		cbi(TIMSK0, TOIE0);		//ovfŠ„‚èž‚Ý
		cbi(TIMSK0, OCIE0B);	//À²Ï/¶³ÝÀ0”äŠrBŠ„‚èž‚Ý
	}
	void setOCR(uint8_t num){
		OCR0B = num;
	}
	uint8_t getOCR(){
		return OCR0B;
	}
}gateDriveB;
ISR(TIMER0_COMPA_vect){//OFF‚É“–‚½‚é“®ì
	gateDriveA.__offAction();
}
ISR(TIMER0_COMPB_vect){//OFF‚É“–‚½‚é“®ì
	gateDriveB.__offAction();	
}
ISR(TIMER0_OVF_vect){//ON(‰ñ“])“®ì
	Tm0Cntr::upCount();
	gateDriveA.__onAction();
	gateDriveB.__onAction();	
	
	Ir2302Tm0::motorState next;
	next = gateDriveA.__calcNextFromGoal();
	gateDriveA.__updateOCR(next);
	next = gateDriveB.__calcNextFromGoal();
	gateDriveB.__updateOCR(next);
}

#endif

