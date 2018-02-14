//動作未確認
#ifndef PIN_INTERRUPT_H_
#define PIN_INTERRUPT_H_

#include <avr/interrupt.h>	//割り込みヘッダ
#include ".\BIT_CTRL.cpp"

namespace PinInterrupt{
	uint8_t pins[24];
	void (*func[24])();
	uint8_t pastPinState[24];
	uint8_t arrayPos = 0;
	void setInterrupt(uint8_t pinNum, void (*f)()){
		pins[arrayPos] = pinNum;
		func[arrayPos] = f;

		pinMode(pinNum, MODE_INPUT);
		pinPullup(pinNum, PULLUP_ON);
		pastPinState[arrayPos] = readInput(pinNum);
		arrayPos++;

		switch(pinNum){
			case PCINT0_PIN:
				sbi(PCMSK0, 0);
				break;
			case PCINT1_PIN:
				sbi(PCMSK0, 1);
				break;
			case PCINT2_PIN:
				sbi(PCMSK0, 2);
				break;
			case PCINT3_PIN:
				sbi(PCMSK0, 3);
				break;
			case PCINT4_PIN:
				sbi(PCMSK0, 4);
				break;
			case PCINT5_PIN:
				sbi(PCMSK0, 5);
				break;
			case PCINT6_PIN:
				sbi(PCMSK0, 6);
				break;
			case PCINT7_PIN:
				sbi(PCMSK0, 7);
				break;

			case PCINT8_PIN:
				sbi(PCMSK1, 0);
				break;
			case PCINT9_PIN:
				sbi(PCMSK1, 1);
				break;
			case PCINT10_PIN:
				sbi(PCMSK1, 2);
				break;
			case PCINT11_PIN:
				sbi(PCMSK1, 3);
				break;
			case PCINT12_PIN:
				sbi(PCMSK1, 4);
				break;
			case PCINT13_PIN:
				sbi(PCMSK1, 5);
				break;
			case PCINT14_PIN:
				sbi(PCMSK1, 6);
				break;

			case PCINT16_PIN:
				sbi(PCMSK2, 0);
				break;
			case PCINT17_PIN:
				sbi(PCMSK2, 1);
				break;
			case PCINT18_PIN:
				sbi(PCMSK2, 2);
				break;
			case PCINT19_PIN:
				sbi(PCMSK2, 3);
				break;
			case PCINT20_PIN:
				sbi(PCMSK2, 4);
				break;
			case PCINT21_PIN:
				sbi(PCMSK2, 5);
				break;
			case PCINT22_PIN:
				sbi(PCMSK2, 6);
				break;
			case PCINT23_PIN:
				sbi(PCMSK2, 7);
				break;
		}
	}
	void fireFunc(){
		for(int i=0; i<arrayPos;i++){
			if(readInput(pins[i]) != pastPinState[i]){
				(*func[i])();
			}
		}
	}
}

ISR(PCINT0_vect){
	PinInterrupt::fireFunc();
}
ISR(PCINT1_vect){
	PinInterrupt::fireFunc();
}
ISR(PCINT2_vect){
	PinInterrupt::fireFunc();
}

#endif