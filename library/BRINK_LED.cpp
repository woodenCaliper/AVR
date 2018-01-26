
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
uint8_t funcNum=0;

class BrinkLed{
	private:
	volatile uint8_t *DDRx, *PORTx;
	uint8_t pinNum;
	uint16_t periodCount, brinkCount;
	uint16_t brinkActionCount, periodActionCount;
	uint8_t brinkNum, brinkActionNum;
	static uint16_t count;

	public:
	BrinkLed(volatile uint8_t *_DDR, volatile uint8_t *_PORT, uint8_t _pinNum){
		DDRx = _DDR;
		PORTx = _PORT;
		pinNum = _pinNum;

		Tm2Ctrl::setDivision(1024);
		Tm2Ctrl::setWGM(0b000);	//標準動作
		sbi(TIMSK2, TOIE2);
		blPtr[funcNum]=this;
		funcNum++;
	}

	void begin(uint8_t _brinkNum, uint32_t brinkPeriodMsec=200, uint32_t ExecutionPeriodMsec=2000){
		float msecToCount = (F_CPU/1000)/(1024.0*256);
		sbi(*DDRx, pinNum);
		cbi(*PORTx,pinNum);

		if(brinkPeriodMsec*_brinkNum*2 > ExecutionPeriodMsec){
			ExecutionPeriodMsec = brinkPeriodMsec*_brinkNum*2;
		}

		brinkNum = _brinkNum*2;
		periodCount = msecToCount * ExecutionPeriodMsec;
		brinkCount = msecToCount * brinkPeriodMsec;
		brinkActionNum   = 0;
		brinkActionCount = count+brinkCount;
		periodActionCount= count+periodCount;
		sei();
	}
	void stop(){

	}
	static void countUp(){
		count++;
	}
	void brink(){
		if(brinkNum > brinkActionNum){
			if(count == brinkActionCount){
				toggleBit(*PORTx, pinNum);
				brinkActionCount += brinkCount;
				brinkActionNum++;
			}
		}
		else if(count == periodActionCount){
			cbi(*PORTx, pinNum);	//出力1
			brinkActionCount  = count + brinkCount;
			periodActionCount += periodCount;
			brinkActionNum    = 0;
		}
	}
};
uint16_t BrinkLed::count=0;

ISR(TIMER2_OVF_vect){
	BrinkLed::countUp();
	for(int i=0; i<funcNum; i++){
		blPtr[i]->brink();
	}
}
#endif