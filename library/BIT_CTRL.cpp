//動作未確認
#ifndef BIT_CTRL_H_
#define BIT_CTRL_H_

#include <avr/io.h>

//ビット操作のマクロ定義>>>>>
#ifndef sbi
#define sbi(PORT,BIT) (PORT |= (1<<BIT))
#endif
#ifndef cbi
#define cbi(PORT,BIT) (PORT &=~(1<<BIT))
#endif
#ifndef sbiMask
#define sbiMask(PORT,BYTE) (PORT |= BYTE)
#endif
#ifndef cbiMask
#define cbiMask(PORT,BYTE) (PORT &=~BYTE)
#endif
#ifndef changeBit
#define changeBit(PORT,BIT,result) (PORT = (PORT & ~(1<<BIT)) | (result<<BIT) )
#endif
#ifndef checkBit
#define checkBit(PIN,BIT) ((PIN >> BIT) & 0b1)
#endif
#ifndef toggleBit
#define toggleBit(PORT, BIT) (PORT ^= (1<<BIT))
#endif
#ifndef changebyte
#define changebyte(fromByte, toByte, mask_bits) fromByte=(((toByte&mask_bits)|fromByte)&(~mask_bits|toByte))
#endif
//<<<<<ビット操作のマクロ定義

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>
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

/**
 * @brief 入出力設定のパラメータ
*/
const uint8_t MODE_INPUT=0, MODE_OUTPUT=1, MODE_INPUT_PULLUP=2, MODE_OUTPUT_LOW=3;

/**
 * @brief 出力状態のパラメータ
 */
const uint8_t OUTPUT_LOW=0, OUTPUT_HIGH=1;

/**
 * @brief プルアップ状態のパラメータ
*/
const uint8_t PULLUP_OFF=0, PULLUP_ON=1;

void pinMode(uint8_t pinNum, uint8_t mode);
void pinOutput(uint8_t pinNum, uint8_t output);
inline void pinPullup(uint8_t pinNum, uint8_t pullup);
bool readInput(uint8_t pinNum);


/**
* @brief 入出力の設定(DDR操作)を行う
* @param[in] pinNum 実ピン番号
* @param[in] mode	入出力の設定<br>
MODE_INPUT:入力設定<br>
MODE_OUTPUT:出力設定<br>
MODE_INPUT_PULLUP:入力設定&プルアップ<br>
MODE_OUTPUT_LOW:出力設定&出力LOW
*/
void pinMode(uint8_t pinNum, uint8_t mode){
	switch(mode){
		case MODE_OUTPUT:
			*(avrPins[pinNum-1].ddr) |= (avrPins[pinNum-1].pinMask);
			break;
		case MODE_INPUT:
			*(avrPins[pinNum-1].ddr) &= ~(avrPins[pinNum-1].pinMask);
			break;
		case MODE_INPUT_PULLUP:
			*(avrPins[pinNum-1].ddr) &= ~(avrPins[pinNum-1].pinMask);
			pinPullup(pinNum,PULLUP_ON);
			break;
		case MODE_OUTPUT_LOW:
			*(avrPins[pinNum-1].ddr) |= (avrPins[pinNum-1].pinMask);
			pinOutput(pinNum, OUTPUT_LOW);
			break;
		default:
			break;
	}
}

/**
* @brief 出力状態の設定(PORT操作)を行う
* @param[in] pinNum	実ピン番号
* @param[in] output	出力の状態<br>
OUTPUT_LOW:Lowを出力<br>
OUTPUT_HIGH:Highを出力<br>
*/
void pinOutput(uint8_t pinNum, uint8_t output){
	if(output==OUTPUT_HIGH){
		*(avrPins[pinNum-1].port) |= (avrPins[pinNum-1].pinMask);
	}
	else{
		*(avrPins[pinNum-1].port) &= ~(avrPins[pinNum-1].pinMask);
	}
}

/**
* @brief プルアップ状態の設定(PORT操作)を行う
* @param[in] pinNum	実ピン番号
* @param[in] pullup	プルアップの状態<br>
PULLUP_ON:プルアップ有効<br>
PULLUP_OFF:プルアップ無効<br>
*/
inline void pinPullup(uint8_t pinNum, uint8_t pullup){
	pinOutput(pinNum, pullup);
}

/**
* @brief ピンの入力の読み込み(PIN読み込み)を行う
* @param[in] pinNum	実ピン番号
* @return bool ピンの状態<br>
* @retval true HIGH<br>
* @retval false Low<br>
*/
bool readInput(uint8_t pinNum){
	//return 	(bool)( *(avrPins[pinNum].pin) & (avrPins[pinNum].pinMask) );
	return (bool)(*(avrPins[pinNum-1].pin) & (avrPins[pinNum-1].pinMask) );
}

/**
* @brief ピンの出力状態を読み取る
* @param[in] pinNum	実ピン番号
* @return bool ピンの状態<br>
* @retval true HIGH<br>
* @retval false Low<br>
**/
bool readOutput(uint8_t pinNum){
	return (bool)(*(avrPins[pinNum-1].port) & (avrPins[pinNum-1].pinMask));
}
#endif
