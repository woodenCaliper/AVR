//動作未確認
#ifndef TIMER_INTERRUPT_H_
#define TIMER_INTERRUPT_H_

#include <avr/interrupt.h>	//割り込みヘッダ
#include ".\BIT_CTRL.cpp"
#include ".\TIMER_CTRL.cpp"

//ISR TIMER0_COMPA_vect()
namespace Tm0Irpt{
	//irpt_bit(OCR0A)*分周比/CLKごとにタイマー割り込みを発生させる設定
	void setOvf(uint16_t division=0){
		if(division!=0){
			Tm0Ctrl::setDivision(division);
		}
		Tm0Ctrl::enableOvfIrpt();	//ﾀｲﾏ/ｶｳﾝﾀ0溢れ割り込み許可
		sei();
	}
	void setBitA(uint8_t irpt_bit, uint16_t division=0){
		if(division!=0){
			Tm0Ctrl::setDivision(division);
		}
		Tm0Ctrl::setWGM(0b010);
		Tm0Ctrl::enableCompaIrpt();	//ﾀｲﾏ/ｶｳﾝﾀ0比較A割り込み許可
		TCNT0 = 0;	//カウント値(マイコンが数えている数が入る)
		OCR0A = irpt_bit;	//irpt_bit値
		sei();
		//割り込み間隔 = irpt_bit(OCR0A) * 分周比 / CLK
	}
	void setUsecA(uint32_t usec, uint16_t division=0){
		uint8_t irpt_bit;
		if(division==0){
			division = Tm0Ctrl::getDivision();
			Tm0Ctrl::getTop(usec, &irpt_bit, division);
		}
		else{
			Tm0Ctrl::getTopDivision(usec, &irpt_bit, &division);
		}
		Tm0Irpt::setBitA(irpt_bit, division);
		sei();
	}
}

//ISR TIMER1_COMPA_vect()
namespace Tm1Irpt{
	void setOvf(uint16_t division=0){
		if(division!=0){
			Tm1Ctrl::setDivision(division);
		}
		Tm1Ctrl::enableOvfIrpt();	//ﾀｲﾏ/ｶｳﾝﾀ1溢れ割り込み許可
		sei();
	}
	void enableCapt(uint16_t division=0){
		if(division!=0){
			Tm1Ctrl::setDivision(division);
		}
		Tm1Ctrl::enableCaptIrpt();
		sei();
	}
	void setBitA(uint8_t irpt_bit, uint16_t division=0){
		if(division!=0){
			Tm1Ctrl::setDivision(division);
		}
		Tm1Ctrl::setWGM(0b0100);
		Tm1Ctrl::enableCompaIrpt();		//ﾀｲﾏ/ｶｳﾝﾀ1比較A割り込み許可
		TCNT1 = 0;	//カウント値(マイコンが数えている数が入る)
		OCR1A = irpt_bit;	//irpt_bit値
		sei();
		//割り込み間隔 = irpt_bit(OCR0A) * 分周比 / CLK
	}
	void setUsecA(uint32_t use, uint16_t division=0){
		uint16_t irpt_bit;
		if(division==0){
			division = Tm1Ctrl::getDivision();
			Tm1Ctrl::getTop(usec, &irpt_bit, division);
		}
		else{
			Tm1Ctrl::getTopDivision(usec, &irpt_bit, &division);
		}
		Tm1Irpt::setBitA(irpt_bit, division);
		sei();
	}
}

namespace Tm2Irpt{
	void setOvf(uint16_t division=0){
		if(division!=0){
			Tm2Ctrl::setDivision(division);
		}
		Tm2Ctrl::enableOvfIrpt();	//ﾀｲﾏ/ｶｳﾝﾀ0溢れ割り込み許可
		sei();
	}
	void setBitA(uint8_t irpt_bit, uint16_t division=0){
		if(division!=0){
			Tm2Ctrl::setDivision(division);
		}
		Tm2Ctrl::setWGM(0b010);
		Tm2Ctrl::enableCompaIrpt();	//ﾀｲﾏ/ｶｳﾝﾀ0比較A割り込み許可
		TCNT2 = 0;	//カウント値(マイコンが数えている数が入る)
		OCR2A = irpt_bit;	//irpt_bit値
		//割り込み間隔 = irpt_bit(OCR0A) * 分周比 / CLK
		sei();
	}
	void setUsecA(uint32_t usec, uint16_t division=0){
		uint8_t irpt_bit;

		if(division==0){
			division = Tm2Ctrl::getDivision();
			Tm2Ctrl::getTop(usec, &irpt_bit, division);
		}
		else{
			Tm2Ctrl::getTopDivision(usec, &irpt_bit, &division);
		}
		Tm2Irpt::setBitA(irpt_bit, division);
		sei();
	}
}

#endif