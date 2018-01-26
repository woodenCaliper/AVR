#ifndef I2C_H_
#define I2C_H_


#include ".\BIT_CTRL.cpp"
#include ".\TRANSCEIVER_QUEUE.cpp"
#include ".\RING_ARRAY.cpp"
#include <avr/interrupt.h>	//割り込みヘッダ


class I2c{
	public:
	class I2cTransceiver : public TransceiverQueue{
		public:
		volatile bool comuniFlag;
		RingArray snycLen;

		I2cTransceiver() : TransceiverQueue(){
			comuniFlag=false;
			snycLen.setBufferNum(20);
		}
		int16_t rxOut(){//overRide
			int16_t data = receiver.firstOut();
			if(data != -1){
				snycLen.overWrite( snycLen.look(0)-1 ,0);
				if(snycLen.look(0)==0){
					snycLen.firstOut();
				}
			}
			return data;
		}
		int16_t readSnycData(char* dataAdrs){	//戻り値は1通信データ数
			int16_t lastLen = snycLen.firstOut();
			if(lastLen != -1){
				for(int16_t i=0; i<lastLen; i++){
					*(dataAdrs+i) = receiver.firstOut();
				}
			}
			return lastLen;
		}
	};
	I2cTransceiver master, slave;

	volatile uint8_t opponetAddress;
	volatile uint16_t masterGetDataLen;		//master要求データ数
	volatile uint16_t masterSnycDataNum;	//masterの累計通信データ数

	I2c(){	//コンストラクタ
		slave.init(50,50);
		master.init(50,50);
	}
	void begin(uint8_t myAddress, uint32_t speed=400000){
		setMyAddress(myAddress);
		setSpeed(speed);
		//	sbi(TWAR, TWGCE);   //一斉呼び出し検出許可
		sbi(TWCR, TWIE);	//割り込み許可
		sbi(TWCR, TWEN);	//TWI動作許可
		setTWCR(0,0,0,1);
		TWAMR=0x00;	//マスク
		sei();
	}
	void stop(){
		cbi(TWCR, TWEN);
		cbi(TWCR, TWIE);
	}
	void setSpeed(uint32_t speed){
		int bunsyu=1;
		setDivision(1);
		TWBR = uint8_t( (F_CPU/speed-16)/(2*bunsyu) );
	}
	void setDivision(uint8_t division){
		switch(division){
			case 1:
			changebyte(TWSR, 0x00, 0x03);
			break;
			case 4:
			changebyte(TWSR, 0x01, 0x03);
			break;
			case 16:
			changebyte(TWSR, 0x02, 0x03);
			break;
			default:	//case 64:
			changebyte(TWSR, 0x03, 0x03);
			break;
		}
	}
	void setMyAddress(uint8_t address){
		TWAR = (address&0x7F)<<1;
	}
	char getTWDR(){
		return TWDR;
	}
	//TWSTA=開始条件生成許可	TWSTO=停止条件生成許可
	//TWINT=TWI割り込み要求フラグ	TWEA =TWI動作許可
	void setTWCR(char twsta, char twsto, char twint, char twea){
		char twcr = (twint<<TWINT)| (twea<<TWEA)| (twsta<<TWSTA)| (twsto<<TWSTO);//| (1<< TWEN);
		changebyte(TWCR, twcr, 0xF0);
	}
	uint8_t getTWSR(){
		return (TWSR&0xF8);
	}

//MASTER+WRITE(データ送信側)
	void beginTransmission(uint8_t address){
		opponetAddress = (address<<1) | 0x00;	//LSB=0がWrite設定
		master.transmitter.doEmpty();
	}
//MASTER+READ(データ要求側)
	void requestFrom(uint8_t address, uint16_t count){
		opponetAddress = (address<<1) | 0x01;	//LSB=0がRead設定
		masterGetDataLen = count;
	}
//MASTER+(WRITE_READ)送信開始
	uint16_t endTransmission(bool wait=true){
		while( master.comuniFlag || slave.comuniFlag );
		master.comuniFlag = true;
		setTWCR(1,0,1,1);	//開始条件送信
		if(wait){
			while(master.comuniFlag);	//通信終了までループ
			return masterSnycDataNum;
		}
		return 0;
	}
//<<<<<MASTER
};
I2c i2c;



ISR(TWI_vect){
	static uint8_t slaveLenNum=0;
	char twsr=i2c.getTWSR();

	switch(twsr){
//MASTER>>>>>
		case 0x08:	//開始条件送信した
		case 0x10:	//再送開始条件送信した
			TWDR = i2c.opponetAddress;	//スレーブアドレスセットSLA+W
			i2c.setTWCR(0,0,1,1);
			i2c.masterSnycDataNum=0;
		break;

//MASTER+WRITE(データ送信側)>>>>>
		case 0x18:	//アドレス送信→ACK受信
		case 0x28:	//データ送信→ACK送受信
			if( !(i2c.master.transmitter.isEmpty()) ){	//空でないなら
				TWDR = i2c.master.txOut();
				i2c.masterSnycDataNum++;
				i2c.setTWCR(0,0,1,1);
			}
			else{
				i2c.setTWCR(0,1,1,1);	//STOP送信
				i2c.master.comuniFlag = false;
			}
		break;
		case 0x20:	//アドレス送信→NACK受信
		case 0x30:	//データ送信→NACK送信
			i2c.setTWCR(0,1,1,1);	//STOP送信
			i2c.master.comuniFlag = false;
		break;
//<<<<<MASTER+WRITE(データ送信側)

//MASTER+READ(データ要求側)>>>>>
		case 0x40:	//アドレス送信+ACK受信
			i2c.setTWCR(0,0,1,1);	//ACKで応答予定
		break;
		case 0x50:	//データ受信→ACK応答した
			i2c.master.rxIn(TWDR);
			i2c.masterSnycDataNum++;
			if(i2c.masterSnycDataNum < i2c.masterGetDataLen){
				i2c.setTWCR(0,0,1,1);	//ACK応答
			}
			else{
				i2c.setTWCR(0,0,1,0);	//NACK応答
			}
		break;
		case 0x58:	//NACK応答した
		case 0x48:	//アドレス送信→応答なし
			i2c.setTWCR(0,1,1,1);	//STOP送信
			i2c.master.comuniFlag = false;
		break;
//<<<<<MASTER+READ(データ要求側)

//SLAVE+READ(データを受け取る側)>>>>>
		case 0x60:	//SLA+Wを受信→ACK応答した
		case 0x70:	//一斉呼び出し受信→ACK応答
			i2c.slave.comuniFlag = true;
			slaveLenNum=0;
			i2c.setTWCR(0,0,1,1);	//ACK応答予定
		break;
		case 0x80:	//データ受信→ACK応答
		case 0x90:	//一斉呼び出しでデータ受信→ACK応答
			i2c.slave.rxIn(i2c.getTWDR());
			slaveLenNum++;
		 	i2c.setTWCR(0,0,1,1);	//ACK受信予定
		break;
		case 0x88:	//データ受信→NACK応答
		case 0x98:	//一斉呼び出しでデータ受信→NACK応答
			i2c.slave.rxIn(i2c.getTWDR());
			slaveLenNum++;
			i2c.setTWCR(0,0,1,1);	//通信終了
			i2c.slave.comuniFlag = false;
		break;
		case 0xA0:	//停止条件or再送条件検出
			i2c.setTWCR(0,0,1,1);	//通信終了
			i2c.slave.snycLen.lastIn(slaveLenNum);
			i2c.slave.comuniFlag = false;
		break;
		case 0x68:	//自アドレス受信時に調停敗退→ACK応答
		case 0x78:	//一斉呼び出しで調停敗退→ACK応答
			i2c.setTWCR(0,0,1,0);	//NACK応答予定
		break;
//<<<<<SLAVE+READ(データを受け取る側)

//SLAVE+WRITE(データを読まれる側)>>>>>
		case 0xA8:	//SLA+Rを受信→ACK応答
		case 0xB0:	//自アドレス受信(SLA+R)時に調停敗退→ACK応答
			i2c.slave.comuniFlag = true;
		case 0xB8:	//データ送信→ACK応答
			if( !(i2c.slave.transmitter.isEmpty()) ){	//最終データを送信かの判断
				TWDR = i2c.slave.txOut();
				i2c.setTWCR(0,0,1,1);	//ACK受信予定
			}
			else{
				i2c.setTWCR(0,0,1,0);	//NACK受信予定
			}
		break;
		case 0xC0:	//データ送信→NACK受信
		case 0xC8:	//最終データバイト送信したのに→ACK受信
			i2c.setTWCR(0,0,1,1);	//通信終了
			i2c.slave.comuniFlag = false;
		break;
//<<<<<SLAVE+WRITE(データを読まれる側)

//例外>>>>>
		case 0x38:	//アドレス送信orデータ送信→パス競合
			i2c.setTWCR(1,0,1,1);	//可能になり次第開始
		break;
		case 0x00:	//不正な開始条件/停止条件でのﾊﾞｽ異常
		default:
			i2c.setTWCR(0,1,1,1);	//STOP送信
		break;
//<<<<<例外
	}
}
#endif