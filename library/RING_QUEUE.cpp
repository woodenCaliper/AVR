#ifndef RING_QUEUE_H_
#define RING_QUEUE_H_

#include <stdlib.h>

class Queue{
	protected:
	volatile char* buffer;
	volatile uint16_t head, tail;
	volatile uint16_t bufferNum;

	public:
	//コンストラクタ・デトラクタ>>>>>
	Queue(){
		head=0;
		tail=0;
		bufferNum=0;
	}
	Queue(uint16_t _bufferNum){
		head=0;
		tail=0;
		setBufferNum(_bufferNum);
	}
	~Queue(){
		free((void*)buffer);
	}
	//<<<<<コンストラクタ・デトラクタ

	void setBufferNum(uint16_t _bufferNum){
		bufferNum = _bufferNum+1;
	//	buffer = new char[bufferNum];	//動的メモリ確保
		buffer = (char*)malloc(sizeof(char)*bufferNum);	//動的メモリ確保
	}
	void init(uint16_t _bufferNum){
		setBufferNum(_bufferNum);
	}

	//抜き出したデータ、空なら-1
	int16_t out(){
		char data;
		if( !isEmpty() ){	//bufferが空でない
			data = buffer[head];
			head = (head+1) % bufferNum;
			return data;
		}
		return -1;
	}
	//return=成否
	bool in(char data){	//追加
		if(isFull()){	//bufferがfull
			// out();	//古いデータを捨てる
			// buffer[tail]=data;
			// tail = (tail+1) % bufferNum;
			return false;
		}
		else{
			buffer[tail]=data;
			tail = (tail+1) % bufferNum;
			return true;
		}
	}
	bool isEmpty(){
		return head == tail;
	}
	bool isFull(){
		return head == (tail+1)%bufferNum;
	}
	void doEmpty(){
		head=tail;
	}
	uint16_t getBufferLen(){//所有データ数
		return (tail-head+bufferNum)%bufferNum;
	}
	//値がないときは0
	uint16_t untilValueLen(char value){
		for(uint16_t i=0; i<getBufferLen(); i++){
			if(buffer[(head+i)%bufferNum]==value){
				return i+1;
			}
		}
		return 0;
	}

	uint16_t copyBuffer(char* array){
		uint16_t len = getBufferLen();
		for(uint16_t i=0; i<len; i++){
			*(array+i) = buffer[(head+i) % bufferNum];
		}
		return len;
	}
};
#endif