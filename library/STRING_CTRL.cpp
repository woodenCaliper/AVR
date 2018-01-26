//動作未確認
#ifndef STRING_CTRL_H_
#define STRING_CTRL_H_

#include <stdio.h>

void byteToStr(char byte, char* str){
	*(str+0)='0';
	*(str+1)='b';
	for(int i=7; 0<=i; i--){
		*(str+(9-i)) = ((byte>>i)&1)+'0';
	}
	*(str+10) = '\0';
}

uint16_t getStringLen(const char* str){
	uint16_t len=0;
	while(*(str+len)!='\0'){
		len++;
	}
	return len;
}
uint16_t getCharaNum(char* str, char chara){
	uint16_t num=0;
	while(1){
		if(*(str)=='\0'){
			break;
		}
		else if(*(str)==chara){
			num++;
		}
		str++;
	}
	return num;
}
//sprintf
//bool
//long double
//
void numToStr(unsigned int num, char *str, char format){
	switch(format){
		case 1:
			if(num==0){
				*(str+0)='F';
				*(str+1)='a';
				*(str+2)='l';
				*(str+3)='s';
				*(str+4)='e';
			}
			else{
				*(str+0)='T';
				*(str+1)='r';
				*(str+2)='u';
				*(str+3)='e';
			}
		break;
		case 2:
			*(str+0)='0';
			*(str+1)='b';
			for(int i=7; 0<=i; i--){
				*(str+(9-i)) = ((num>>i)&1)+'0';
			}
			*(str+10) = '\0';
		break;
		case 10:

		break;
		case 16:
			*(str+0)='0';
			*(str+1)='x';
			sprintf((str+2), "%x", num);	//1500byteくらい食う
		break;
	}
}

int16_t strToNum(const char* str, uint16_t len){
	int16_t num=0;
	int16_t signedNum=1;
	if(*str=='+'){
		len--;
	}
	else if(*str=='-'){
		signedNum=-1;
		len--;
	}
	for(uint16_t i=0; i<len; i++){
		num *= 10;
		if( '0'<=*(str+i) && *(str+i)<='9'){
			num += *(str+i)-'0';
		}
		else{
			return 0;
		}
	}
	return num*signedNum;

}

//戻り値
//正 : cmd文字数
//０ : checkCmdがtrueCmdに対して少ない（trueCmd文字数よりcheckCmdNumのほうが少ない）
//負 : -戻り値文字目が間違っている、
//trueCmdはNULL文字が必要
int16_t cmdCheck(const char* trueCmd, const char* checkCmd, uint16_t checkCmdNum){
	uint16_t checkingTrueCmdNum=0, checkingCheckCmdNum=0;
	char checkChara;
	while(1){
		if(*(trueCmd+checkingTrueCmdNum) == '\0'){	//正しいcmdとの比較が最後(NULL文字)まで成功したとき
			return checkingCheckCmdNum;
		}
		if( checkCmdNum==checkingCheckCmdNum ){	//完全に比較データが揃いきっているかどうか
			return 0;	//完全に比較データ（）が揃いきっていない
		}

		checkChara = *(checkCmd+checkingCheckCmdNum);	//合致しているか調べる文字の代入

		if(*(trueCmd+checkingTrueCmdNum) == checkChara){	//一文字のときに合致しているか調査
			checkingTrueCmdNum++;
		}
		else if(*(trueCmd+checkingTrueCmdNum) == '?'){	//数値文字に合致しているか調査
			if(checkChara<'0' || '9'<checkChara){
				return -checkingCheckCmdNum-1;
			}
			else{
				checkingTrueCmdNum++;
			}
		}
		else if(*(trueCmd+checkingTrueCmdNum) == '{'){	//複数文字開始文字( { )のときに合致しているか調査
			bool flag=false;
			while(1){
				checkingTrueCmdNum++;
				if(*(trueCmd+checkingTrueCmdNum) == checkChara){
					flag=true;
				}
				else if(*(trueCmd+checkingTrueCmdNum) == '}'){	//複数文字終了文字( } )のときに合致しているか調査
					if(flag){	//既にcmdに合致していたら
						checkingTrueCmdNum++;
						break;
					}
					else{
						return -checkingCheckCmdNum-1;
					}
				}
			}
		}
		else{	//どれにも合致しなかったとき
			return -checkingCheckCmdNum-1;
		}
		checkingCheckCmdNum++;
	}
}

#endif
