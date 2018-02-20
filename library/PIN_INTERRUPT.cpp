//動作未確認
#ifndef PIN_INTERRUPT_H_
#define PIN_INTERRUPT_H_

#include <avr/interrupt.h>	//割り込みヘッダ
#include ".\BIT_CTRL.cpp"

struct pinInterrupt{
	volatile uint8_t pcicrMask;
	volatile uint8_t *pcmskRegister;
	volatile uint8_t pcmskMask;
};
pinInterrupt PC0_INTERRUPT  = {0b00000001, &PCMSK0, 0b00000001};
pinInterrupt PC1_INTERRUPT  = {0b00000001, &PCMSK0, 0b00000010};
pinInterrupt PC2_INTERRUPT  = {0b00000001, &PCMSK0, 0b00000100};
pinInterrupt PC3_INTERRUPT  = {0b00000001, &PCMSK0, 0b00001000};
pinInterrupt PC4_INTERRUPT  = {0b00000001, &PCMSK0, 0b00010000};
pinInterrupt PC5_INTERRUPT  = {0b00000001, &PCMSK0, 0b00100000};
pinInterrupt PC6_INTERRUPT  = {0b00000001, &PCMSK0, 0b01000000};
pinInterrupt PC7_INTERRUPT  = {0b00000001, &PCMSK0, 0b10000000};

pinInterrupt PC8_INTERRUPT  = {0b00000010, &PCMSK1, 0b00000001};
pinInterrupt PC9_INTERRUPT  = {0b00000010, &PCMSK1, 0b00000010};
pinInterrupt PC10_INTERRUPT = {0b00000010, &PCMSK1, 0b00000100};
pinInterrupt PC11_INTERRUPT = {0b00000010, &PCMSK1, 0b00001000};
pinInterrupt PC12_INTERRUPT = {0b00000010, &PCMSK1, 0b00010000};
pinInterrupt PC13_INTERRUPT = {0b00000010, &PCMSK1, 0b00100000};
pinInterrupt PC14_INTERRUPT = {0b00000010, &PCMSK1, 0b01000000};

pinInterrupt PC16_INTERRUPT = {0b00000100, &PCMSK2, 0b00000001};
pinInterrupt PC17_INTERRUPT = {0b00000100, &PCMSK2, 0b00000010};
pinInterrupt PC18_INTERRUPT = {0b00000100, &PCMSK2, 0b00000100};
pinInterrupt PC19_INTERRUPT = {0b00000100, &PCMSK2, 0b00001000};
pinInterrupt PC20_INTERRUPT = {0b00000100, &PCMSK2, 0b00010000};
pinInterrupt PC21_INTERRUPT = {0b00000100, &PCMSK2, 0b00100000};
pinInterrupt PC22_INTERRUPT = {0b00000100, &PCMSK2, 0b01000000};
pinInterrupt PC23_INTERRUPT = {0b00000100, &PCMSK2, 0b10000000};

pinInterrupt PC_INTERRUPT_NULL = {0, 0, 0};

pinInterrupt pinInterrupts[28] = {
	PC14_INTERRUPT,
	PC16_INTERRUPT,
	PC17_INTERRUPT,
	PC18_INTERRUPT,
	PC19_INTERRUPT,
	PC20_INTERRUPT,
	PC_INTERRUPT_NULL,	//VCC
	PC_INTERRUPT_NULL,	//GND
	PC6_INTERRUPT,
	PC7_INTERRUPT,
	PC21_INTERRUPT,
	PC22_INTERRUPT,
	PC23_INTERRUPT,
	PC0_INTERRUPT,	//14
	PC1_INTERRUPT,
	PC2_INTERRUPT,
	PC3_INTERRUPT,
	PC4_INTERRUPT,
	PC5_INTERRUPT,
	PC_INTERRUPT_NULL,	//AVCC
	PC_INTERRUPT_NULL,	//AREF
	PC_INTERRUPT_NULL,	//GND
	PC8_INTERRUPT,
	PC9_INTERRUPT,
	PC10_INTERRUPT,
	PC11_INTERRUPT,
	PC12_INTERRUPT,
	PC13_INTERRUPT
};



namespace PinInterrupt{
	struct pinInterruptState{
		uint8_t pins[8];
		void (*func[8])(bool);
		bool pastPinState[8];
		uint8_t arrayPos;
	};
	
	pinInterruptState pcint0={{0,0,0,0,0,0,0,0}, {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}, {false, false, false, false, false, false, false, false}, 0};
	pinInterruptState pcint1={{0,0,0,0,0,0,0,0}, {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}, {false, false, false, false, false, false, false, false}, 0};
	pinInterruptState pcint2={{0,0,0,0,0,0,0,0}, {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}, {false, false, false, false, false, false, false, false}, 0};

	void setInterrupt(uint8_t pinNum, void (*f)(bool)){
		pinMode(pinNum, MODE_INPUT);
		pinPullup(pinNum, PULLUP_ON);
		switch(pinInterrupts[pinNum-1].pcicrMask){
			case 0b00000001:
				pcint0.pins[pcint0.arrayPos] = pinNum;
				pcint0.func[pcint0.arrayPos] = f;
				pcint0.pastPinState[pcint0.arrayPos] = readInput(pinNum);
				pcint0.arrayPos++;
			break;
			case 0b00000010:
				pcint1.pins[pcint1.arrayPos] = pinNum;
				pcint1.func[pcint1.arrayPos] = f;
				pcint1.pastPinState[pcint1.arrayPos] = readInput(pinNum);
				pcint1.arrayPos++;
			break;
			case 0b00000100:
				pcint2.pins[pcint2.arrayPos] = pinNum;
				pcint2.func[pcint2.arrayPos] = f;
				pcint2.pastPinState[pcint2.arrayPos] = readInput(pinNum);
				pcint2.arrayPos++;
			break;
		}
		PCICR |= pinInterrupts[pinNum-1].pcicrMask;	//ピン変化割り込み許可
		*(pinInterrupts[pinNum-1].pcmskRegister) |= pinInterrupts[pinNum-1].pcmskMask;	//ピン変化割り込みマスク
		sei();
	}
	inline void pcint0Fire(){
		uint8_t inputByte = PINB;
		for(uint8_t i=0; i<pcint0.arrayPos;i++){
			bool currentInput = inputByte & avrPins[pcint0.pins[i]-1].pinMask;
			if(currentInput != pcint0.pastPinState[i]){
				(*(pcint0.func[i]))(currentInput);
				pcint0.pastPinState[i] = currentInput;
			}
		}
	}
	inline void pcint1Fire(){
		uint8_t inputByte = PINC;
		for(uint8_t i=0; i<pcint1.arrayPos;i++){
			bool currentInput = inputByte & avrPins[pcint1.pins[i]-1].pinMask;
			if(currentInput != pcint1.pastPinState[i]){
				(*(pcint1.func[i]))(currentInput);
				pcint1.pastPinState[i] = currentInput;
			}
		}
	}
	inline void pcint2Fire(){
		uint8_t inputByte = PIND;
		for(uint8_t i=0; i<pcint2.arrayPos;i++){
			bool currentInput = inputByte & avrPins[pcint2.pins[i]-1].pinMask;
			if(currentInput != pcint2.pastPinState[i]){
				(*(pcint2.func[i]))(currentInput);
				pcint2.pastPinState[i] = currentInput;
			}
		}
	}
}

ISR(PCINT0_vect){
	PinInterrupt::pcint0Fire();
}
ISR(PCINT1_vect){
	PinInterrupt::pcint1Fire();
}
ISR(PCINT2_vect){
	PinInterrupt::pcint2Fire();
}
#endif