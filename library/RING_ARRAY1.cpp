#ifndef RING_ARRAY_H_
#define RING_ARRAY_H_

#include <stdlib.h>

template <class freeType>
class RingArray{
	protected:
	volatile freeType* buffer;
	//head=データが入っている先頭
	//tail=データが入ってない末尾
	volatile uint16_t head, tail;
	volatile uint16_t bufferNum;

	public:
	//コンストラクタ・デトラクタ>>>>>
	RingArray(){
		head=0;
		tail=0;
		bufferNum=0;
	}
	RingArray(uint16_t _bufferNum){
		head=0;
		tail=0;
		setBufferNum(_bufferNum);
	}
	~RingArray(){
		free((void*)buffer);
	}
	//<<<<<コンストラクタ・デトラクタ

	void setBufferNum(uint16_t _bufferNum){
		bufferNum = _bufferNum+1;
	//	buffer = new char[bufferNum];	//動的メモリ確保
		buffer = (freeType*)malloc(sizeof(freeType)*bufferNum);	//動的メモリ確保
	}
	void init(uint16_t _bufferNum){
		setBufferNum(_bufferNum);
	}

	//先頭に挿入
	bool firstIn(freeType data){
		if(isFull()){	//bufferがfull
			// out();	//古いデータを捨てる
			// buffer[tail]=data;
			// tail = (tail+1) % bufferNum;
			return false;
		}
		else{
			head = (head+(bufferNum-1)) % bufferNum;
			buffer[head]=data;
			return true;
		}
	}
	//return=成否
	bool lastIn(freeType data){	//追加
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

	//抜き出したデータ、空なら-1
	int16_t firstOut(){
		freeType data;
		if( !isEmpty() ){	//bufferが空でない
			data = buffer[head];
			head = (head+1) % bufferNum;
			return data;
		}
		return -1;
	}
	//末尾から取り出し、空なら-1
	int16_t lastOut(){
		freeType data;
		if( !isEmpty() ){	//bufferが空でない
			tail = (tail+(bufferNum-1)) % bufferNum;
			data = buffer[tail];
			return data;
		}
		return -1;
	}

	bool isEmpty(){
		return head == tail;
	}
	bool isFull(){
		return head == (tail+1) % bufferNum;
	}
	void doEmpty(){
		head = tail;
	}
	uint16_t len(){//所有データ数
		return (tail+(bufferNum-head)) % bufferNum;
	}
	bool overWrite(freeType data, uint16_t adrs){
		if(adrs>=0){
			if(adrs>=len()){
				return false;
			}
			buffer[(head+adrs) % bufferNum] = data;
			return true;
		}
		else{
			if (-adrs>len()){
				return false;
			}
			buffer[(tail+(bufferNum+adrs)) % bufferNum] = data;
			return true;
		}
	}
	freeType look(int32_t adrs){
		if(adrs>=0){
			if(adrs>=len()){
				return 0;
			}
			return buffer[(head+adrs) % bufferNum];
		}
		else{
			if (-adrs>len()){
				return 0;
			}
			return buffer[(tail+(bufferNum+adrs)) % bufferNum];
		}
	}
	//値がないときは-1
	int32_t index(freeType value){
		for(uint16_t i=0; i<len(); i++){
			if(look(i)==value){
				return i;
			}
		}
		return -1;
	}
	uint16_t copy(freeType* array){
		uint16_t length = len();
		for(uint16_t i=0; i<length; i++){
			*(array+i) = buffer[(head+i) % bufferNum];
		}
		return length;
	}
};
#endif