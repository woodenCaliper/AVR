#ifndef TRANSCEIVER_QUEUE_H_
#define TRANSCEIVER_QUEUE_H_

#include <stdlib.h>
#include ".\RING_ARRAY.cpp"
#include ".\STRING_CTRL.cpp"

class TransceiverQueue{
	public:
	RingArray transmitter, receiver;
	TransceiverQueue(){
	}
	TransceiverQueue(uint16_t txBufferNum, uint16_t rxBufferNum){
		init(txBufferNum, rxBufferNum);
	}
	void init(uint16_t txBufferNum, uint16_t rxBufferNum){
		transmitter.setBufferNum(txBufferNum);
		receiver.setBufferNum(rxBufferNum);
	}


//オーバーライド用関数>>>>>
	virtual	int16_t txIn(char data){
		return transmitter.lastIn(data);
	}
	virtual int16_t txOut(){
		return transmitter.firstOut();
	}
	virtual int16_t rxIn(char data){
		return receiver.lastIn(data);
	}
	virtual int16_t rxOut(){
		return receiver.firstOut();
	}
//<<<<<オーバーライド用関数

//>>>>>bufferCtrl
	//成功時=1、無行動時=0、失敗時=-1
	//cmdが届ききっていない場合、何もしない。一致したならキューを取り出す。不一致なら削除
	//MD???{+-}{01}???
	int8_t cmdCheckQueue(const char* trueCmd, char* resultStrage){
		uint16_t len = receiver.index(*trueCmd)+1;
		if(len==0){	//存在しないなら空に
			receiver.doEmpty();
			return -1;
		}
		else{	//先頭文字が一致するところまで削除
			for(uint8_t i=0;i<len-1;i++){
				rxOut();
			}
		}
		char bufferCopy[20];
		uint16_t copyNum = receiver.copy(bufferCopy);
		int16_t	cmdError = cmdCheck(trueCmd, bufferCopy, copyNum);

		if(cmdError>0){	//全一致ならキューを取り出して渡す
			for(int16_t i=0; i<cmdError; i++){
				*(resultStrage+i) = rxOut();
			}
			return 1;
		}
		else if(cmdError<0){	//不一致を確認したら間違っているところまでキューを削除
			for(int16_t i=0; i>cmdError; i--){
				rxOut();
			}
			return -1;
		}
		else{	//cmdがそろっていないため何もしない
			return 0;
		}
	}
//<<<<<bufferCtrl
	uint16_t len(){
		return receiver.len();
	}


//read>>>>>
	int16_t read(){	//空なら-1
		return rxOut();
	}
	template <class freeType>
	int16_t read(char* dataPath, freeType len){	//指定数を取り出す
		if(receiver.len() < (uint16_t)len){
			return -1;
		}
		for(freeType i=0; i<(freeType)len; i++){
			*(dataPath+i) = read();
		}
		return len;
	}

	//stopDataまで(stopDataを含め)を取り出す、なければ-1
	int16_t read(char* dataPath, char stopData){
		return read(dataPath, receiver.index(stopData)+1);
	}

	template <class freeType>
	int16_t read(int16_t zero, freeType len){	//指定数を取り出す
		if(receiver.len() < (uint16_t)len){
			return -1;
		}
		for(uint16_t i=0; i<(uint16_t)len; i++){
			read();	
		}
		return len;
	}


	int16_t readStr(char* strPath){	//\0がなければ-1
		return read(strPath, '\0');
	}
	int32_t readStrInt(char stopData='\0'){	//戻り値がintのため、floatを返せない
		uint16_t strLen = receiver.index(stopData)+1;
		char str[12];
		if(strLen <= 1){
			if(strLen == 1){
				read();
			}
			return -1;
		}
		read(str, strLen);
		*(str+strLen-1) = '\0';	//stopDataの場所を\0で上書き
		return atol(str);
	}
	float readStrFloat(char stopData='\0'){
		uint16_t strLen = receiver.index(stopData)+1;
		uint16_t dotLen = receiver.index('.')+1;
		char str[20];
		if(strLen <= 1){
			if(strLen == 1){
				read();
			}
			return -1;
		}
		read(str, strLen);
		*(str+strLen-1) = '\0';

		if((dotLen != 0)&&(dotLen < strLen)){
			return strtod(str, 0);
		}
	}
//read<<<<<

//write>>>>>
	int16_t write(char data){	//キューに追加した文字数
		return txIn(data);
	}
	int16_t write(char* buf, uint16_t size){
		int16_t success;
		uint16_t i;
		for(i=0; i<size; i++){
			success = write(*(buf+i));
			if(success==0){
				break;
			}
		}
		return i;
	}
	int16_t write(char* str){	//0も含めて送る
		int16_t n;
		n = write(str, getStringLen(str));
		n += write('\0');
		return n;
	}
	int16_t write(const char* data)				{return write((char*)data);}
//<<<<<write

//print>>>>>
	int16_t print(unsigned long n, int base=10){	//数値を文字列として送信(整数)
		return printNumber(n, base);
	}
	int16_t print(long n, int base=10){	//数値を文字列として送信(整数)
		if((base==10) && (n<0)){	//負の場合
			int t = write('-');
			return print((unsigned long)(-n), 10) + t;
		}
		else{
			return print((unsigned long)n, base);
		}
	}
	int16_t print(double num, int digits=8){	//数値を文字列として送信(浮動小数点)
		return printFloat(num, digits);
	}
	int16_t print(char data){	//ASCコードを送信
		return write(data);
	}
	int16_t print(char* data, int len){	//配列と長さ
		return write(data, len);
	}
	int16_t print(char* str){	//文字列（\0は送らない）
		int n=0;
		int16_t success;
		while(*str != '\0'){
			success = write(*str);
			if(success==0){
				break;
			}
			str++;
			n++;
		}
		return n;
	}
	int16_t print(bool data){
		if(data){
			return print("true");
		}
		else{
			return print("false");
		}
	}

	int16_t print(int data, int base=10)          {return print((long)data, base);}
	int16_t print(unsigned int data, int base=10) {return print((long)data, base);}
	int16_t print(unsigned char data, int base=10){return print((long)data, base);}
	int16_t print(float data, int digit=8)		{return print((double)data, digit);}
	int16_t print(const char* data)				{return print((char*)data);}
//<<<<<print


//println>>>>>
	int16_t println(){
		char data[2]={'\r', '\n'};
		return print(data, 2);
	}
	template <class freeType>
	int16_t println(freeType data){
		return print(data) + println();
	}
	template <class freeType1, class freeType2>
	int16_t println(freeType1 data1, freeType2 data2){
		return print(data1, data2) + println();
	}
//<<<<<println

	private:
	int16_t printNumber(unsigned long n, uint8_t base){
		char buf[8 * sizeof(long) + 1];		// Assumes 8-bit chars plus zero byte.
		char *str = &buf[sizeof(buf) - 1];
		*str = '\0';
		if (base < 2){
			return -1;
		}
		do{
			char c = n % base;
			n /= base;
			*--str = c < 10 ? c + '0' : c + 'A' - 10;
		}while(n);
		return print(str);
	}


	int16_t printFloat(double number, uint8_t digits){
		size_t n = 0;
		if (isnan(number)) return print("nan");
		if (isinf(number)) return print("inf");
		if (number > 4294967040.0) return print ("ovf");  // constant determined empirically
		if (number <-4294967040.0) return print ("ovf");  // constant determined empirically

		// Handle negative numbers
		if (number < 0.0){
			n += print('-');
			number = -number;
		}

		// Round correctly so that print(1.999, 2) prints as "2.00"
		double rounding = 0.5;
		for (uint8_t i=0; i<digits; ++i){
			rounding /= 10.0;
		}
		number += rounding;

		// Extract the integer part of the number and print it
		unsigned long int_part = (unsigned long)number;
		double remainder = number - (double)int_part;
		n += print(int_part);

		// Print the decimal point, but only if there are digits beyond
		if (digits > 0){
			n += print('.');
		}

		// Extract digits from the remainder one at a time
		while (digits-- > 0){
			remainder *= 10.0;
			unsigned int toPrint = (unsigned int)(remainder);
			n += print(toPrint);
			remainder -= toPrint;
		}
		return n;
	}
};
#endif