
#ifndef BRINK_LED_H_
#define BRINK_LED_H_

#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>	//割り込みヘッダ

#include ".\BIT_CTRL.cpp"
#include ".\TIMER_CTRL.cpp"
#include ".\TIMER_INTERRUPT.cpp"


class BrinkLed;
BrinkLed *blPtr[20];
uint8_t gFuncNum=0;

class BrinkLed{
	private:
	uint8_t pinNum;
	uint16_t periodCount, brinkCount;
	uint16_t brinkActionCount, periodActionCount;
	uint8_t brinkNum, brinkActionNum;
	static uint16_t count;
	uint8_t funcNum;

	public:
	BrinkLed(uint8_t _pinNum){
		pinNum = _pinNum;

		Tm2Ctrl::setDivision(1024);
		Tm2Ctrl::setWGM(0b000);	//標準動作
		sbi(TIMSK2, TOIE2);
		funcNum = gFuncNum;
		++gFuncNum;
		blPtr[funcNum]=this;
		sei();
	}
	~BrinkLed(){
		for(uint8_t i=funcNum; i<gFuncNum; i++){
			blPtr[i] = blPtr[i+1];
		}
		--gFuncNum;
	}

	void begin(uint8_t _brinkNum, uint32_t brinkPeriodMsec=200, uint32_t ExecutionPeriodMsec=2000){
		float msecToCount = (F_CPU/1000)/(1024.0*256);
		pinMode(pinNum, MODE_OUTPUT);
		pinOutput(pinNum, OUTPUT_LOW);

		if(brinkPeriodMsec*_brinkNum*2 > ExecutionPeriodMsec){
			ExecutionPeriodMsec = brinkPeriodMsec*_brinkNum*2;
		}

		brinkNum = _brinkNum*2;
		periodCount = msecToCount * ExecutionPeriodMsec;
		brinkCount = msecToCount * brinkPeriodMsec;
		brinkActionNum   = 0;
		brinkActionCount = count+brinkCount;
		periodActionCount= count+periodCount;
	}
	void stop(){

	}
	static void countUp(){
		count++;
	}
	void brink(){
		if(brinkNum > brinkActionNum){
			if(count == brinkActionCount){
				pinOutput(pinNum, !readOutput(pinNum));
				brinkActionCount += brinkCount;
				brinkActionNum++;
			}
		}
		else if(count == periodActionCount){
			pinOutput(pinNum, OUTPUT_LOW);
			brinkActionCount  = count + brinkCount;
			periodActionCount += periodCount;
			brinkActionNum    = 0;
		}
	}
};
uint16_t BrinkLed::count=0;

ISR(TIMER2_OVF_vect){
	BrinkLed::countUp();
	for(int i=0; i<gFuncNum; i++){
		blPtr[i]->brink();
	}
}
#endif