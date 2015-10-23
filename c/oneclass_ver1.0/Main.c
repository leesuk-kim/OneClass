#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "vsheader.h"
#include "Utility.h"
#include "cpon.c"

//LBG Algorithm의 clustering 횟수입니다. 기존값은 20입니다. 10이 더 나을 수도 있습니다.
//const static int CLUSTERING_COUNT = 20;

int main(int argc, char *args[])
{
	int i;
	CPON cpon;
	clfscore clfresult[TENFOLD];

	for (i = 0; i < TENFOLD; i++)
	{
		//cpon을 초기화합니다.
		//아래 함수는 다음과 같습니다:
		//cpon = ch.initialize(&cpon, 0.05);
		cpon_initialize(&cpon);
		
		//cpon에 data를 upload합니다.
		cpon.upload(data_learn);

		//classification을 위한 학습을 합니다.
		//이 함수는 성능에 대한 지표를 returen할 것 같습니다(?).
		cpon.learn();

		//testdata로 classfication합니다.
		clfresult[i] = cpon.test(data_test);

		//결과를 출력합니다.
		ch.printscore(clfresult[i]);
		
		//CPON을 제거합니다.
		cpon_finish(&cpon);
	}

	return 0;
}