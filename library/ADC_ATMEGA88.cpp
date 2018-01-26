#ifndef ADC_ATMEGA88_H_
#define ADC_ATMEGA88_H_

namespace Adc{
	void EnableInterrupt(){
		sbi(ADCSRA,3);	//A/D変換完了割り込み許可
	}
	inline void setREFS(char refs){
		changebyte(ADMUX, refs<<6, 0b11000000);
	}
	inline void setMUX(char mux){
		changebyte(ADMUX, mux, 0b00001111);
	}
	inline void setADPS(char adps){
		changebyte(ADCSRA, adps, 0b00000111);
	}
	unsigned int __doAdc(){
		if( checkbit(ADCSRA,7)==0 ){	//A/D許可がされていない、
			sbi(ADCSRA,7);		//A/D許可
			__doAdc();	//初回起動のADC初期化のための空回し
		}
		sbi(ADCSRA,6);	//A/D変換開始
		while( checkbit(ADCSRA,4)==0 );	//変換完了まで待機
		sbi(ADCSRA,4);	//A/D変換完了割り込み要求ﾌﾗｸのクリア（1の書き込み）
		return ADCW;
	}

	unsigned int getBit(unsigned char channel){	//おおよそ88処理時間必要(8MHzなら11us)
		unsigned int result;
		setMUX(channel);
		setREFS(0b01);
		setADPS(0b111);
		ADCSRB=0x00;
		changebyte(DIDR0, 1<<channel, 0b00111111);	//ﾃﾞｼﾞﾀﾙ入力禁止(ADC5D～0D)
		result = __doAdc();
		DIDR0 = 0x00;
		return result;
	}
	float getVolt(unsigned char channel, float refVolt=5.0){
		return getBit(channel) * refVolt / 1024;
	}

	float getVolt1v1(unsigned char channel, unsigned char tryNum=100){
		int before=-1, now=0;
		setMUX(0b1110);
		setREFS(0b01);
		setADPS(0b111);
		ADCSRB=0x00;
		for(int i=0; i<tryNum; i++){	//結果が安定するまでループ
			now = __doAdc();
			if(before == now){
				break;
			}
			else{
				before = now;
			}
		}
		return getBit(channel) * 1.1 / now;
	}

	//ADCﾎﾟｰﾄ:channel(0~7),割り込み分周比:division(2,4,8,16,32,64,128)
	//指定のﾎﾟｰﾄに入力されたｱﾅﾛｸﾞ値を読む設定
	void ADC_single_init(char channel, int division){
		//分周比設定
		switch(division){
			case 2:
				division=0b000;
			break;
			case 4:
				division = 0b010;
			break;
			case 8:
				division = 0b011;
			break;
			case 16:
				division = 0b100;
			break;
			case 32:
				division = 0b101;
			break;
			case 64:
				division = 0b110;
			break;
			case 128:
			default:
				division = 0b111;
			break;
		}

		ADMUX = 0b01000000 | channel;
	//	          ||||++++--A/Dﾁｬﾈﾙ選択(MUX3～0)
	//	          |||+------予約
	//	          ||+-------0:AD変換結果を右揃え選択--1:左
	//	          ++--------基準電圧選択(REFS1～0)

		/*
		A/Dﾁｬﾈﾙ選択(AMUX3～0)
		-----------------------------------------
		|0000|0001|0010|0011|0100|0101|0110|0111|
		|----|----|----|----|----|----|----|----|
		| PC0| PC1| PC2| PC3| PC4| PC5| PC6| PC7|
		|----|----|----|----|----|----|----|----|
		|ADC0|ADC1|ADC2|ADC3|ADC4|ADC5|ADC6|ADC7|
		-----------------------------------------
		*/

		ADCSRA = 0b11100000 | division;
	//	           |||||+++-- A/D変換ｸﾛｯｸ選択(ADPS2～0)
	//	           ||||+----- A/D変換完了割り込み許可
	//	           |||+------ A/D変換完了割り込み要求ﾌﾗｸﾞ
	//	           ||+------- A/D変換自動起動許可
	//	           |+-------- A/D変換開始
	//	           +--------- A/D許可

		/*
		A/D変換ｸﾛｯｸ選択(ADPS2～0)
		--------------------------------------------------
		| 000 | 001 | 010 | 011 | 100 | 101 | 110 |  111 |
		|-----|-----|-----|-----|-----|-----|-----|------|
		| CK/2| CK/2| CK/4| CK/8|CK/16|CK/32|CK/64|CK/128|
		--------------------------------------------------
		*/

		ADCSRB = 0b0000000;
	//	           ||||+++--A/D変換自動起動要因選択(ADTS2～0)
	//	           ||++-----予約
	//	           |+-------ｱﾅﾛｸﾞ比較器多重器許可
	//	           +--------予約

		/*
		A/D変換自動起動要因選択(ADTS2～0)
		---------------------------------
		| 02| 01| 00|                   |
		|---|---|---|-------------------|
		| 0 | 0 | 0 |連続変換動作       |
		| 0 | 0 | 1 |ｱﾅﾛｸﾞ比較器        |
		| 0 | 1 | 0 |外部割り込み要求0  |
		| 0 | 1 | 1 |ﾀｲﾏ/ｶｳﾝﾀ0比較A一致 |
		| 1 | 0 | 0 |ﾀｲﾏ/ｶｳﾝﾀ0溢れ      |
		| 1 | 0 | 1 |ﾀｲﾏ/ｶｳﾝﾀ1比較B一致 |
		| 1 | 1 | 0 |ﾀｲﾏ/ｶｳﾝﾀ1溢れ      |
		| 1 | 1 | 1 |ﾀｲﾏ/ｶｳﾝﾀ1捕獲要求  |
		---------------------------------
		*/
		DIDR0 = 0b00111111;
	//	          ||++++++--1:ﾃﾞｼﾞﾀﾙ入力禁止(ADC5D～0D)
	//	          ++--------予約
		ADCW = 0;
		//ADCH,ADCL(ADCW)
		//ADCW=AD変換結果の値が入る0～1024(10bit)
	}
}
#endif