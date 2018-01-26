//動作未確認
#ifndef ANY_FUNC_H_
#define ANY_FUNC_H_


template <class freeType>
freeType margeNum(freeType num, freeType min, freeType max){
	if(num<=min)
		return min;
	else if(max<=num)
		return max;
	return num;
}

void byteToStr(char byte, char* str){
	*(str+0)='0';
	*(str+1)='b';
	for(int i=7; 0<=i; i--){
		*(str+(9-i)) = ((byte>>i)&1)+'0';
	}
	*(str+10) = '\0';
}
//数値vs文字列変換
//itoa(255, str, 16) ==> ff
//itoa(int num, char* str, int radix);

#endif
