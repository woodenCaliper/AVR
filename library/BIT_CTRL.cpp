//動作未確認
#ifndef BIT_CTRL_H_
#define BIT_CTRL_H_

#include <avr/io.h>
#include ".\ATMEGA88_PIN.cpp"

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
