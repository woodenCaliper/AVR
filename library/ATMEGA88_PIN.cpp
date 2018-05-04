#ifndef ATMEGA88_PDIP
//ATMEGA88_PDIPのピン配置と名前
const uint8_t PB0_PIN=14;
const uint8_t PB1_PIN=15;
const uint8_t PB2_PIN=16;
const uint8_t PB3_PIN=17;
const uint8_t PB4_PIN=18;
const uint8_t PB5_PIN=19;
const uint8_t PB6_PIN=9;
const uint8_t PB7_PIN=10;

const uint8_t PC0_PIN=23;
const uint8_t PC1_PIN=24;
const uint8_t PC2_PIN=25;
const uint8_t PC3_PIN=26;
const uint8_t PC4_PIN=27;
const uint8_t PC5_PIN=28;
const uint8_t PC6_PIN=1;

const uint8_t PD0_PIN=2;
const uint8_t PD1_PIN=3;
const uint8_t PD2_PIN=4;
const uint8_t PD3_PIN=5;
const uint8_t PD4_PIN=6;
const uint8_t PD5_PIN=11;
const uint8_t PD6_PIN=12;
const uint8_t PD7_PIN=13;
//ATMEGA88_PDIPのピン配置と名前
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//pin interrupt
const uint8_t PCINT0_PIN=14;
const uint8_t PCINT1_PIN=15;
const uint8_t PCINT2_PIN=16;
const uint8_t PCINT3_PIN=17;
const uint8_t PCINT4_PIN=18;
const uint8_t PCINT5_PIN=19;
const uint8_t PCINT6_PIN=9;
const uint8_t PCINT7_PIN=10;
const uint8_t PCINT8_PIN=23;
const uint8_t PCINT9_PIN=24;
const uint8_t PCINT10_PIN=25;
const uint8_t PCINT11_PIN=26;
const uint8_t PCINT12_PIN=27;
const uint8_t PCINT13_PIN=28;
const uint8_t PCINT14_PIN=1;
const uint8_t PCINT16_PIN=2;
const uint8_t PCINT17_PIN=3;
const uint8_t PCINT18_PIN=4;
const uint8_t PCINT19_PIN=5;
const uint8_t PCINT20_PIN=6;
const uint8_t PCINT21_PIN=11;
const uint8_t PCINT22_PIN=12;
const uint8_t PCINT23_PIN=13;
//pininterrupt
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//ADC
const uint8_t ADC0_PIN=23;
const uint8_t ADC1_PIN=24;
const uint8_t ADC2_PIN=25;
const uint8_t ADC3_PIN=26;
const uint8_t ADC4_PIN=27;
const uint8_t ADC5_PIN=28;
//ADC
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

const uint8_t AIN0_PIN=12;
const uint8_t AIN1_PIN=13;

const uint8_t TXD_PIN=3;
const uint8_t RXD_PIN=2;

const uint8_t SCK_PIN=19;
const uint8_t MISO_PIN=18;
const uint8_t MOSI_PIN=17;

const uint8_t OC0A_PIN=12;
const uint8_t OC0B_PIN=11;
const uint8_t OC1A_PIN=15;
const uint8_t OC1B_PIN=16;
const uint8_t OC2A_PIN=17;
const uint8_t OC2B_PIN=5;

const uint8_t ICP1_PIN=14;

const uint8_t INT0_PIN=4;
const uint8_t INT1_PIN=5;
//<<<<<ATMEGA88_PDIPのピン配置と名前

/**
 * @brief 入出力レジスタ(DDR, PORT, PIN, bitMask)をまとめた構造体
 */
struct avrPin{
	volatile uint8_t *ddr;
	volatile uint8_t *port;
	volatile uint8_t *pin;
	uint8_t pinMask;
};

const avrPin avrPinB0 = {&DDRB, &PORTB, &PINB, 0b00000001};
const avrPin avrPinB1 = {&DDRB, &PORTB, &PINB, 0b00000010};
const avrPin avrPinB2 = {&DDRB, &PORTB, &PINB, 0b00000100};
const avrPin avrPinB3 = {&DDRB, &PORTB, &PINB, 0b00001000};
const avrPin avrPinB4 = {&DDRB, &PORTB, &PINB, 0b00010000};
const avrPin avrPinB5 = {&DDRB, &PORTB, &PINB, 0b00100000};
const avrPin avrPinB6 = {&DDRB, &PORTB, &PINB, 0b01000000};
const avrPin avrPinB7 = {&DDRB, &PORTB, &PINB, 0b10000000};

const avrPin avrPinC0 = {&DDRC, &PORTC, &PINC, 0b00000001};
const avrPin avrPinC1 = {&DDRC, &PORTC, &PINC, 0b00000010};
const avrPin avrPinC2 = {&DDRC, &PORTC, &PINC, 0b00000100};
const avrPin avrPinC3 = {&DDRC, &PORTC, &PINC, 0b00001000};
const avrPin avrPinC4 = {&DDRC, &PORTC, &PINC, 0b00010000};
const avrPin avrPinC5 = {&DDRC, &PORTC, &PINC, 0b00100000};
const avrPin avrPinC6 = {&DDRC, &PORTC, &PINC, 0b01000000};

const avrPin avrPinD0 = {&DDRD, &PORTD, &PIND, 0b00000001};
const avrPin avrPinD1 = {&DDRD, &PORTD, &PIND, 0b00000010};
const avrPin avrPinD2 = {&DDRD, &PORTD, &PIND, 0b00000100};
const avrPin avrPinD3 = {&DDRD, &PORTD, &PIND, 0b00001000};
const avrPin avrPinD4 = {&DDRD, &PORTD, &PIND, 0b00010000};
const avrPin avrPinD5 = {&DDRD, &PORTD, &PIND, 0b00100000};
const avrPin avrPinD6 = {&DDRD, &PORTD, &PIND, 0b01000000};
const avrPin avrPinD7 = {&DDRD, &PORTD, &PIND, 0b10000000};

const avrPin avrPinNull = {0,0,0,0};	//入出力が割り当てられていないピン用

/**
 * @brief 実ピン番号順に並べられた配列、ピン番号とポート情報を関連付けている
*/
const avrPin avrPins[28] = {
	avrPinC6,
	avrPinD0,
	avrPinD1,
	avrPinD2,
	avrPinD3,
	avrPinD4,
	avrPinNull,	//!VCC
	avrPinNull,	//!GND
	avrPinB6,
	avrPinB7,
	avrPinD5,
	avrPinD6,
	avrPinD7,
	avrPinB0,	//!14
	avrPinB1,
	avrPinB2,
	avrPinB3,
	avrPinB4,
	avrPinB5,
	avrPinNull,	//!AVCC
	avrPinNull,	//!AREF
	avrPinNull,	//!GND
	avrPinC0,
	avrPinC1,
	avrPinC2,
	avrPinC3,
	avrPinC4,
	avrPinC5	//!28
};
#endif
