//動作未確認
#ifndef BIT_CTRL_H_
#define BIT_CTRL_H_

#ifndef sbi
#define sbi(PORT,BIT) (PORT |= (1<<BIT))	//ビット操作のマクロ定義
#endif
#ifndef cbi
#define cbi(PORT,BIT) (PORT &=~(1<<BIT))
#endif
#ifndef changeBit
#define changeBit(PORT,BIT,result) (PORT = (PORT & ~(1<<BIT)) | (result<<BIT) )
#endif
#ifndef checkBit
#define checkBit(PIN,BIT) ((PIN >> BIT) & 1)
#endif
#ifndef toggleBit
#define toggleBit(PORT, BIT) (PORT ^= (1<<BIT))
#endif
#ifndef changebyte
#define changebyte(fromByte, toByte, mask_bits) fromByte=(((toByte&mask_bits)|fromByte)&(~mask_bits|toByte))
#endif

struct avrPin{
	volatile uint8_t *ddr;
	volatile uint8_t *port;
	volatile uint8_t *pin;
	uint8_t pinNum;
};

const avrPin avrPinB0 = {&DDRB, &PORTB, &PINB, 0};
const avrPin avrPinB1 = {&DDRB, &PORTB, &PINB, 1};
const avrPin avrPinB2 = {&DDRB, &PORTB, &PINB, 2};
const avrPin avrPinB3 = {&DDRB, &PORTB, &PINB, 3};
const avrPin avrPinB4 = {&DDRB, &PORTB, &PINB, 4};
const avrPin avrPinB5 = {&DDRB, &PORTB, &PINB, 5};
const avrPin avrPinB6 = {&DDRB, &PORTB, &PINB, 6};
const avrPin avrPinB7 = {&DDRB, &PORTB, &PINB, 7};

const avrPin avrPinC0 = {&DDRC, &PORTC, &PINC, 0};
const avrPin avrPinC1 = {&DDRC, &PORTC, &PINC, 1};
const avrPin avrPinC2 = {&DDRC, &PORTC, &PINC, 2};
const avrPin avrPinC3 = {&DDRC, &PORTC, &PINC, 3};
const avrPin avrPinC4 = {&DDRC, &PORTC, &PINC, 4};
const avrPin avrPinC5 = {&DDRC, &PORTC, &PINC, 5};
const avrPin avrPinC6 = {&DDRC, &PORTC, &PINC, 6};
const avrPin avrPinC7 = {&DDRC, &PORTC, &PINC, 7};

const avrPin avrPinD0 = {&DDRD, &PORTD, &PIND, 0};
const avrPin avrPinD1 = {&DDRD, &PORTD, &PIND, 1};
const avrPin avrPinD2 = {&DDRD, &PORTD, &PIND, 2};
const avrPin avrPinD3 = {&DDRD, &PORTD, &PIND, 3};
const avrPin avrPinD4 = {&DDRD, &PORTD, &PIND, 4};
const avrPin avrPinD5 = {&DDRD, &PORTD, &PIND, 5};
const avrPin avrPinD6 = {&DDRD, &PORTD, &PIND, 6};
const avrPin avrPinD7 = {&DDRD, &PORTD, &PIND, 7};


template <class freeType1, class freeType2, class freeType3>
freeType1 margeNum(freeType1 num, freeType2 min, freeType3 max){
	if((freeType1)num <= (freeType1)min)
		return min;
	else if((freeType1)max <= (freeType1)num)
		return max;
	return num;
}
#endif
