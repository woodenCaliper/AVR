#ifndef INCREMENTAL_ENCODER_H_
#define INCREMENTAL_ENCODER_H_

#ifndef PI
#define PI 3.14159265359
#endif

//get radius from incremental encoder
class IncrementalEncoder{
	protected:
	float radParPulse;
	int32_t count;
	uint8_t oldEncState;

	public:
	/**
	* @brief エンコーダの分解能と現在の状態を設定
	* @param[in] pulseParRotation エンコーダの分解能
	* @param[in] phaseA A相の状態
	* @param[in] phaseA B相の状態
	*/
	IncrementalEncoder(uint16_t pulseParRotation, bool phaseA, bool phaseB){
		radParPulse =  (2.0*PI) / (pulseParRotation*4);
		count=0;
		oldEncState = ((uint8_t)phaseA<<1) | (phaseA^phaseB);
	}
	/**
	* @brief 現在のエンコーダの角度を設定
	* @param[in] rad エンコーダの角度[rad]
	*/
	void setCurrentRad(float rad){
		count = rad / radParPulse;
	}
	/**
	* @brief 現在のエンコーダの角度の取得
	* @return bool 現在の角度[rad]
	*/
	float getCurrentRad(){
		return count * radParPulse;
	}

	void changedPulse(bool phaseA, bool phaseB){
		uint8_t currentEncState = ((uint8_t)phaseA<<1) | (phaseA^phaseB);	//00->01->10->11->00
		if( ((oldEncState+1)&0b0011) == currentEncState){
			count++;
		}
		else if( ((oldEncState-1)&0b0011) == currentEncState){
			count--;
		}
		else{	//error
			return;
		}
		oldEncState = currentEncState;
	}
	inline void changedPulseA(bool phaseA){
		changedPulse(phaseA, (bool)(oldEncState&0b0001));
	}
	inline void changedPulseB(bool phaseB){
		changedPulse((bool)(oldEncState&0b0010), phaseB);
	}
};
#endif