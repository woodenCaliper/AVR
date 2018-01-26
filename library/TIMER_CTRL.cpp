//動作未確認
#ifndef TIMER_CTRL_H_
#define TIMER_CTRL_H_

#include <avr/interrupt.h>	//割り込みヘッダ
#include ".\BIT_CTRL.cpp"

namespace Tm0Ctrl{
	inline void setCOMA(int8_t coma){
		changebyte(TCCR0A, coma<<6, 0b11000000);
	}
	inline void setCOMB(int8_t comb){
		changebyte(TCCR0A, comb<<4, 0b00110000);
	}
	inline void setWGM(int8_t wgm){	//機能設定
		changebyte(TCCR0A, wgm, 0b00000011);
		changebyte(TCCR0B, wgm<<1, 0b00001000);
	}
	inline void setCSR(int8_t csr){	//分周比
		changebyte(TCCR0B, csr, 0b00000111);
	}
	inline void enableOvfIrpt(){	sbi(TIMSK0, TOIE0);		}
	inline void disableOvfIrpt(){	cbi(TIMSK0, TOIE0);		}
	inline void enableCompaIrpt(){	sbi(TIMSK0, OCIE0A);	}
	inline void disableCompaIrpt(){	cbi(TIMSK0, OCIE0A);	}
	inline void enableCompbIrpt(){	sbi(TIMSK0, OCIE0B);	}
	inline void disableCompbIrpt(){	cbi(TIMSK0, OCIE0B);	}

	uint16_t setDivision(uint16_t division){
		if(division==1){
			setCSR(0b001);
			return 1;
		}
		else if(division==2 || division==8){
			setCSR(0b010);
			return 8;
		}
		else if(division==3 || division==64){
			setCSR(0b011);
			return 64;
		}
		else if(division==4 || division==256){
			setCSR(0b100);
			return 256;
		}
		else{	//else if(division==5 || division==1024){
			setCSR(0b101);
			return 1024;
		}
	}
	uint16_t getDivision(){
		switch(TCCR0B & 0b00000111){
			case 0b001:
				return 1;
			break;
			case 0b010:
				return 8;
			break;
			case 0b011:
				return 64;
			break;
			case 0b100:
				return 256;
			break;
			default: //case: 0b101
				return 1024;
			break;
		}
	}
	void getTopDivision(uint32_t usec, uint8_t* top_path, uint16_t* division_path){
		float top_float[5];
		uint8_t top=255;
		int16_t division[5]={1, 8, 64, 256, 1024};
		int16_t last_division=1024;
		float remainder=10000.0;
		float aaa=0;

		if( (float)(255+1)*1024*1000000/F_CPU < usec){
			*top_path = 255;
			*division_path = 1024;
		}
		else{
			for(int16_t i=0;i<5;i++){
				top_float[i] = F_CPU/1000000.0*usec/division[i]-1.0;
			}
			for(int16_t i=4;i>=0;i--){
				if(top_float[i]<=255){
					aaa=((int32_t)(top_float[i]+0.5)-top_float[i])*division[i];
					if(aaa<0){
						aaa*=-1;
					}
					if(aaa <= remainder){
						remainder=aaa;
						top=(uint8_t)(top_float[i]+0.5);
						last_division = division[i];
					}
				}
			}
			*top_path = top;
			*division_path = last_division;
		}
	}
	void getTop(uint32_t usec, uint8_t* top_path, uint16_t division){
		if( (float)(255+1)*division*1000000/F_CPU  < usec){
			*top_path = 255;
		}
		else{
			*top_path = (uint8_t)(F_CPU/1000000.0*usec/division-1.0+0.5);
		}
	}
}


namespace Tm1Ctrl{
	inline void setCOMA(int8_t coma){
		changebyte(TCCR1A, coma<<6, 0b11000000);
	}
	inline void setCOMB(int8_t comb){
		changebyte(TCCR1A, comb<<4, 0b00110000);
	}
	inline void setWGM(int8_t wgm){	//機能設定
		changebyte(TCCR1A, wgm,    0b00000011);
		changebyte(TCCR1B, wgm<<1, 0b00011000);
	}
	inline void setCSR(int8_t csr){	//分周比
		changebyte(TCCR1B, csr, 0b00000111);
	}
	inline void enableOvfIrpt(){	sbi(TIMSK1, TOIE1);		}
	inline void disableOvfIrpt(){	cbi(TIMSK1, TOIE1);		}
	inline void enableCompaIrpt(){	sbi(TIMSK1, OCIE1A);	}
	inline void disableCompaIrpt(){	cbi(TIMSK1, OCIE1A);	}
	inline void enableCompbIrpt(){	sbi(TIMSK1, OCIE1B);	}
	inline void disableCompbIrpt(){	cbi(TIMSK1, OCIE1B);	}
	inline void enableCaptIrpt(){	sbi(TIMSK1, ICIE1);		}
	inline void disableCaptIrpt(){	cbi(TIMSK1, ICIE1);		}

	uint16_t setDivision(uint16_t division){
		if(division==1){
			setCSR(0b001);
			return 1;
		}
		else if(division==2 || division==8){
			setCSR(0b010);
			return 8;
		}
		else if(division==3 || division==64){
			setCSR(0b011);
			return 64;
		}
		else if(division==4 || division==256){
			setCSR(0b100);
			return 256;
		}
		else{	//else if(division==5 || division==1024){
			setCSR(0b101);
			return 1024;
		}
	}
	uint16_t getDivision(){
		switch(TCCR1B & 0b00000111){
			case 0b001:
				return 1;
			break;
			case 0b010:
				return 8;
			break;
			case 0b011:
				return 64;
			break;
			case 0b100:
				return 256;
			break;
			default: //case: 0b101
			   	return 1024;
			break;
		}
	}
	void getTopDivision(uint32_t usec, uint16_t* top_path, uint16_t* division_path){
		float top_float[5];
		uint16_t top=65535;
		int16_t division[5]={1, 8, 64, 256, 1024};
		int16_t last_division=1024;
		float remainder=10000.0;
		float aaa=0;

		if( (float)(65535+1)*1024*1000000/F_CPU < usec){
			*top_path = 65535;
			*division_path = 1024;
		}
		else{
			for(int16_t i=0;i<5;i++){
				top_float[i] = F_CPU/1000000.0*usec/division[i]-1.0;
			}
			for(int16_t i=4;i>=0;i--){
				if(top_float[i] <= 65535){
					aaa=((int32_t)(top_float[i]+0.5)-top_float[i])*division[i];
					if(aaa<0){
						aaa*=-1;
					}
					if(aaa <= remainder){
						remainder=aaa;
						top=(uint16_t)(top_float[i]+0.5);
						last_division = division[i];
					}
				}
			}
			*top_path = top;
			*division_path = last_division;
		}
	}
	void getTop(uint32_t usec, uint16_t* top_path, uint16_t division){
		if( (float)(65535+1)*division*1000000/F_CPU  < usec){
			*top_path = 65535;
		}
		else{
			*top_path = (uint16_t)(F_CPU/1000000.0*usec/division-1.0+0.5);
		}
	}
}

namespace Tm2Ctrl{
	inline void setCOMA(int8_t coma){
		changebyte(TCCR2A, coma<<6, 0b11000000);
	}
	inline void setCOMB(int8_t comb){
		changebyte(TCCR2A, comb<<4, 0b00110000);
	}
	inline void setWGM(int8_t wgm){	//機能設定
		changebyte(TCCR2A, wgm, 0b00000011);
		changebyte(TCCR2B, wgm<<1, 0b00001000);
	}
	inline void setCSR(int8_t csr){	//分周比
		changebyte(TCCR2B, csr, 0b00000111);
	}
	inline void enableOvfIrpt(){	sbi(TIMSK2, TOIE2);		}
	inline void disableOvfIrpt(){	cbi(TIMSK2, TOIE2);		}
	inline void enableCompaIrpt(){	sbi(TIMSK2, OCIE2A);	}
	inline void disableCompaIrpt(){	cbi(TIMSK2, OCIE2A);	}
	inline void enableCompbIrpt(){	sbi(TIMSK2, OCIE2B);	}
	inline void disableCompbIrpt(){	cbi(TIMSK2, OCIE2B);	}

	uint16_t setDivision(uint16_t division){
		if(division==1){
			setCSR(0b001);
			return 1;
		}
		else if(division==2 || division==8){
			setCSR(0b010);
			return 8;
		}
		else if(division==3 || division==32){
			setCSR(0b011);
			return 32;
		}
		else if(division==4 || division==64){
			setCSR(0b100);
			return 64;
		}
		else if(division==5 || division==128){
			setCSR(0b101);
			return 128;
		}
		else if(division==6 || division==256){
			setCSR(0b110);
			return 256;
		}
		else{	//else if(division==7 || division==1024){
			setCSR(0b111);
			return 1024;
		}
	}
	uint16_t getDivision(){
		switch(TCCR2B & 0b00000111){
			case 0b001:
				return 1;
			break;
			case 0b010:
				return 8;
			break;
			case 0b011:
				return 32;
			break;
			case 0b100:
				return 64;
			break;
			case 0b101:
				return 128;
			break;
			case 0b110:
				return 256;
			break;
			default: //case: 0b111
				return 1024;
			break;
		}
	}
	void getTopDivision(uint32_t usec, uint8_t* top_path, uint16_t* division_path){
		float top_float[7];
		uint8_t top = 255;
		int16_t division[7] = {1, 8, 32, 64, 128, 256, 1024};
		int16_t last_division = 1024;
		float remainder = 10000.0;
		float aaa = 0;

		if( (float)(255+1)*1024*1000000/F_CPU < usec){
			*top_path = 255;
			*division_path = 1024;
		}
		else{
			for(int16_t i=0; i<7; i++){
				top_float[i] = F_CPU/1000000.0*usec/division[i]-1.0;
			}
			for(int16_t i=6; i>=0; i--){
				if(top_float[i] <= 255){
					aaa = ((int32_t)(top_float[i]+0.5)-top_float[i])*division[i];
					if(aaa<0){
						aaa*=-1;
					}
					if(aaa <= remainder){
						remainder=aaa;
						top=(uint8_t)(top_float[i]+0.5);
						last_division = division[i];
					}
				}
			}
			*top_path = top;
			*division_path = last_division;
		}
	}
	void getTop(uint32_t usec, uint8_t* top_path, uint16_t division){
		if( (float)(255+1)*division*1000000/F_CPU  < usec){
			*top_path = 255;
		}
		else{
			*top_path = (uint8_t)(F_CPU/1000000.0*usec/division-1.0+0.5);
		}
	}
}

#endif