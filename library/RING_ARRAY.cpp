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
	volatile uint16_t bufferSize;

	public:
	//コンストラクタ・デトラクタ>>>>>
	RingArray(){
		head=0;
		tail=0;
		bufferSize=0;
	}
	RingArray(uint16_t _bufferSize){
		head=0;
		tail=0;
		resizeBuffer(_bufferSize);
	}
	~RingArray(){
		free((void*)buffer);
	}
	//<<<<<コンストラクタ・デトラクタ

	/**
	* @brief 配列の最大サイズの変更
	* @param[in] _bufferSize 配列の最大サイズ
	**/
	void resizeBuffer(uint16_t _bufferSize){
		bufferSize = _bufferSize+1;
		buffer = (freeType*)malloc(sizeof(freeType)*bufferSize);	//動的メモリ確保
	}


	/**
	* @brief 先頭に挿入
	* @param[in] data 挿入データ
	* @param[out] *error エラーコード[0:正常, -1:エラー]
	**/
	void inHead(freeType data, int8_t *error=0){
		if(isFull()){	//bufferがfull
			// out();	//古いデータを捨てる
			// buffer[tail]=data;
			// tail = (tail+1) % bufferSize;
			*error = -1;
		}
		else{
			head = (head+(bufferSize-1)) % bufferSize;
			buffer[head]=data;
			*error = 0;
		}
	}
	/**
	* @brief 末尾に挿入
	* @param[in] data 挿入データ
	* @param[out] *error エラーコード[0:正常, -1:エラー]
	**/
	void inTail(freeType data, int8_t *error=0){	//追加
		if(isFull()){	//bufferがfull
			// out();	//古いデータを捨てる
			// buffer[tail]=data;
			// tail = (tail+1) % bufferSize;
			*error = -1;
		}
		else{
			buffer[tail]=data;
			tail = (tail+1) % bufferSize;
			*error = 0;
		}
	}

	/**
	* @brief 先頭の要素を取り出し
	* @param[out] *error エラーコード[0:正常, -1:エラー]
	* @return 要素
	**/
	freeType outHead(int8_t *error=0){
		freeType data;
		if( !isEmpty() ){	//bufferが空でない
			data = buffer[head];
			head = (head+1) % bufferSize;
			*error = 0;
			return data;
		}
		*error = -1;
		return 0;
	}

	/**
	* @brief 末尾の要素を取り出し
	* @param[out] *error エラーコード[0:正常, -1:エラー]
	* @return 要素
	**/
	freeType outTail(int8_t *error=0){
		freeType data;
		if( !isEmpty() ){	//bufferが空でない
			tail = (tail+(bufferSize-1)) % bufferSize;
			data = buffer[tail];
			*error = 0;
			return data;
		}
		*error = -1;
		return 0;
	}

	bool isEmpty(){
		return head == tail;
	}
	bool isFull(){
		return head == (tail+1) % bufferSize;
	}

	/**
	* @brief 配列の初期化
	**/
	void doEmpty(){
		head = tail;
	}

	/**
	* @brief 配列の長さの取得
	* @return 配列長さ
	**/
	uint16_t len(){//所有データ数
		return (tail+(bufferSize-head)) % bufferSize;
	}

	/**
	* @brief 配列の要素を上書き
	* @param[in] data 上書きする値
	* @param[in] adrs 要素番号
	* @param[out] *error エラーコード[0:正常, -1:エラー]
	**/
	void overWrite(freeType data, uint16_t adrs, int8_t *error=0){
		if(adrs>=0){
			if(adrs>=len()){
				*error = -1;
				return;
			}
			buffer[(head+adrs) % bufferSize] = data;
			*error = 0;
				return;
		}
		else{
			if (-adrs>len()){
				*error = -1;
				return;
			}
			buffer[(tail+(bufferSize+adrs)) % bufferSize] = data;
			*error = 0;
			return;
		}
	}
	/**
	* @brief 配列の要素を取得
	* @param[in] adrs 要素番号
	* @param[out] *error エラーコード[0:正常, -1:エラー]
	* @return 要素
	**/
	freeType look(int32_t adrs, int8_t *error=0){
		if(adrs>=0){
			if(adrs>=len()){
				*error = -1;
				return 0;
			}

			*error = 0;
			return buffer[(head+adrs) % bufferSize];
		}
		else{
			if (-adrs>len()){
				*error = -1;
				return 0;
			}
			*error = 0;
			return buffer[(tail+(bufferSize+adrs)) % bufferSize];
		}
	}

	/**
	* @brief 配列から値の探索
	* @param[in] value 探索する値
	* @param[out] *error エラーコード[0:正常, -1:エラー]
	* @return 配列番号
	**/
	uint16_t index(freeType value, int8_t *error=0){
		for(uint16_t i=0; i<len(); i++){
			if(look(i)==value){
				*error=0;
				return i;
			}
		}
		*error=-1;
		return 0;
	}
	/**
	* @brief 配列をコピー
	* @param[out] *array コピー先
	* @return 配列長さ
	**/
	uint16_t copy(freeType* array){
		uint16_t length = len();
		for(uint16_t i=0; i<length; i++){
			*(array+i) = buffer[(head+i) % bufferSize];
		}
		return length;
	}
};
#endif