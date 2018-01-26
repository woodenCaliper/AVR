//動作未確認
#ifndef TIMER_PWM_H_
#define TIMER_PWM_H_

#include <avr/io.h>

#include ".\BIT_CTRL.cpp"
#include ".\TIMER_CTRL.cpp"
namespace Tm0Pwm{
	//OCR0B(PD5)で出力
	void OneOutBit(uint8_t on_bit, uint8_t period_bit, uint16_t division){
		sbi(DDRD,5);	//OC0B
		Tm0Ctrl::setDivision(division);
		Tm0Ctrl::setWGM(0b111);
		Tm0Ctrl::setCOMB(0b10);
		TCNT0 = 0;	//カウント値(マイコンが数えている数が入る)
		OCR0A = period_bit;	//period
		OCR0B = on_bit;	//on_time
	}
	void OneOutUsec(uint32_t on_usec, uint32_t period_usec){
		uint8_t on_bit, period_bit;
		uint16_t division;
		Tm0Ctrl::getTopDivision(period_usec, &period_bit, &division);
		Tm0Ctrl::getTop(on_usec, &on_bit, division);
		Tm0Pwm::OneOutBit(on_bit, period_bit, division);
	}
	void OneOutDuty(float duty, uint32_t period_usec){
		uint8_t on_bit, period_bit;
		uint16_t division;
		Tm0Ctrl::getTopDivision(period_usec, &period_bit, &division);
		on_bit = (uint8_t)(period_bit*duty/100);
		Tm0Pwm::OneOutBit(on_bit, period_bit, division);
	}
	//OCR0A(PD6)、OCR0B(PD5)で出力
	void TwoOutBit(uint8_t on_bit_a, uint8_t on_bit_b, uint16_t division){
		sbi(DDRD,6);	//OC01
		sbi(DDRD,5);	//OC0B
		Tm0Ctrl::setDivision(division);
		Tm0Ctrl::setWGM(0b011);
		Tm0Ctrl::setCOMA(0b10);
		Tm0Ctrl::setCOMB(0b10);
		TCNT0 = 0;	//カウント値(マイコンが数えている数が入る)
		OCR0A = on_bit_a;
		OCR0B = on_bit_b;
	}
	void TwoOutUsec(uint32_t on_usec_a, uint32_t on_usec_b, uint16_t division=1024){
		sbi(DDRD,6);	//OC01
		sbi(DDRD,5);	//OC0B
		uint8_t on_bit_a, on_bit_b;
		Tm0Ctrl::getTop(on_usec_a, &on_bit_a, division);
		Tm0Ctrl::getTop(on_usec_b, &on_bit_b, division);
		Tm0Pwm::TwoOutBit(on_bit_a, on_bit_b, division);
	}
	void TwoOutDuty(float duty_a, float duty_b, uint16_t division=1024){
		uint8_t on_bit_a, on_bit_b;
		on_bit_a = (uint8_t)( (float)(0xff+1)*1024*1000000/F_CPU *duty_a/100);
		on_bit_b = (uint8_t)( (float)(0xff+1)*1024*1000000/F_CPU *duty_b/100);
		Tm0Pwm::TwoOutBit(on_bit_a, on_bit_b, division);
	}
}

namespace Tm1Pwm{
	//OCR1B(PB2)で出力
	void OneOutBit(uint16_t on_bit, uint16_t period_bit, uint16_t division){
		sbi(DDRB,2);	//OC1B
		Tm1Ctrl::setDivision(division);
		Tm1Ctrl::setWGM(0b1111);
		Tm1Ctrl::setCOMB(0b10);
		TCNT1 = 0;	//カウント値(マイコンが数えている数が入る)
		OCR1A = period_bit;	//period
		OCR1B = on_bit;	//on_time
	}
	void OneOutUsec(uint32_t on_usec_b, uint32_t period_usec){
		uint16_t on_bit, period_bit, division;
		Tm1Ctrl::getTopDivision(period_usec, &period_bit, &division);
		Tm1Ctrl::getTop(on_usec_b, &on_bit, division);
		Tm1Pwm::OneOutBit(on_bit, period_bit, division);
	}
	void OneOutDuty(float duty, uint32_t period_usec){
		uint16_t on_bit, period_bit, division;
		Tm1Ctrl::getTopDivision(period_usec, &period_bit, &division);
		on_bit = (uint16_t)(period_bit*duty/100);
		Tm1Pwm::OneOutBit(on_bit, period_bit, division);
	}
	//OCR1A(PB1)、OCR1B(PB2)で出力
	void TwoOutBit(uint16_t on_bit_a, uint16_t on_bit_b, uint16_t period_bit, uint16_t division){
		sbi(DDRB,1);	//OC1A
		sbi(DDRB,2);	//OC1B
		Tm1Ctrl::setDivision(division);
		Tm1Ctrl::setWGM(0b1110);
		Tm1Ctrl::setCOMA(0b10);
		Tm1Ctrl::setCOMB(0b10);
		TCNT1 = 0;	//カウント値(マイコンが数えている数が入る)
		OCR1A = on_bit_a;
		OCR1B = on_bit_b;
		ICR1 = period_bit;	//周期(top値)
	}
	void TwoOutUsec(uint32_t on_usec_a, uint32_t on_usec_b, uint32_t period_usec){
		uint16_t on_bit_a, on_bit_b, period_bit, division;
		Tm1Ctrl::getTopDivision(period_usec, &period_bit, &division);
		Tm1Ctrl::getTop(on_usec_a, &on_bit_a, division);
		Tm1Ctrl::getTop(on_usec_b, &on_bit_b, division);
		Tm1Pwm::TwoOutBit(on_bit_a, on_bit_b, period_bit, division);
	}
	void TwoOutDuty(float duty_a, float duty_b, uint32_t period_usec){
		uint16_t on_bit_a, on_bit_b, period_bit, division;
		Tm1Ctrl::getTopDivision(period_usec, &period_bit, &division);
		on_bit_a = (uint16_t)(period_bit*duty_a/100.0);
		on_bit_b = (uint16_t)(period_bit*duty_b/100.0);
		Tm1Pwm::TwoOutBit(on_bit_a, on_bit_b, period_bit, division);
	}
}

namespace Tm2Pwm{
	void OneOutBit(uint16_t on_bit, uint16_t period_bit, uint16_t division){
		sbi(DDRD,3);	//OC2B
		Tm2Ctrl::setDivision(division);
		Tm2Ctrl::setWGM(0b111);
		Tm2Ctrl::setCOMB(0b10);
		TCNT2 = 0;	//カウント値(マイコンが数えている数が入る)
		OCR2A = period_bit;	//period
		OCR2B = on_bit;	//on_time
	}
	void OneOutUsec(uint32_t on_usec, uint32_t period_usec){
		uint8_t on_bit, period_bit;
		uint16_t division;
		Tm2Ctrl::getTopDivision(period_usec, &period_bit, &division);
		Tm2Ctrl::getTop(on_usec, &on_bit, division);
		Tm2Pwm::OneOutBit(on_bit, period_bit, division);
	}
	void OneOutDuty(float duty, uint32_t period_usec){
		uint8_t on_bit, period_bit;
		uint16_t division;
		Tm2Ctrl::getTopDivision(period_usec, &period_bit, &division);
		on_bit = (uint8_t)(period_bit*duty/100);
		Tm2Pwm::OneOutBit(on_bit, period_bit, division);
	}
	void TwoOutBit(uint16_t on_bit_a, uint16_t on_bit_b, uint16_t division){
		sbi(DDRB,3);	//OC2A
		sbi(DDRD,3);	//OC2B
		Tm2Ctrl::setDivision(division);
		Tm2Ctrl::setWGM(0b011);
		Tm2Ctrl::setCOMA(0b10);
		Tm2Ctrl::setCOMB(0b10);
		TCNT2 = 0;	//カウント値(マイコンが数えている数が入る)
		OCR2A = on_bit_a;
		OCR2B = on_bit_b;
	}
	void TwoOutUsec(uint32_t on_usec_a, uint32_t on_usec_b, uint16_t division=1024){
		uint8_t on_bit_a, on_bit_b;
		Tm2Ctrl::setDivision(division);
		Tm2Ctrl::getTop(on_usec_a, &on_bit_a, division);
		Tm2Ctrl::getTop(on_usec_b, &on_bit_b, division);
		Tm2Pwm::TwoOutBit(on_bit_a, on_bit_b, division);
	}
	void TwoOutDuty(float duty_a, float duty_b, uint16_t division=1024){
		uint8_t on_bit_a, on_bit_b;
		on_bit_a = (uint8_t)( (float)(0xff+1)*1024*1000000/F_CPU *duty_a/100);
		on_bit_b = (uint8_t)( (float)(0xff+1)*1024*1000000/F_CPU *duty_b/100);
		Tm2Pwm::TwoOutBit(on_bit_a, on_bit_b, division);
	}
}

#endif