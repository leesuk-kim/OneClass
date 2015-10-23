#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "vsheader.h"
#include "Utility.h"
#include "cpon.c"

//LBG Algorithm�� clustering Ƚ���Դϴ�. �������� 20�Դϴ�. 10�� �� ���� ���� �ֽ��ϴ�.
//const static int CLUSTERING_COUNT = 20;

int main(int argc, char *args[])
{
	int i;
	CPON cpon;
	clfscore clfresult[TENFOLD];

	for (i = 0; i < TENFOLD; i++)
	{
		//cpon�� �ʱ�ȭ�մϴ�.
		//�Ʒ� �Լ��� ������ �����ϴ�:
		//cpon = ch.initialize(&cpon, 0.05);
		cpon_initialize(&cpon);
		
		//cpon�� data�� upload�մϴ�.
		cpon.upload(data_learn);

		//classification�� ���� �н��� �մϴ�.
		//�� �Լ��� ���ɿ� ���� ��ǥ�� returen�� �� �����ϴ�(?).
		cpon.learn();

		//testdata�� classfication�մϴ�.
		clfresult[i] = cpon.test(data_test);

		//����� ����մϴ�.
		ch.printscore(clfresult[i]);
		
		//CPON�� �����մϴ�.
		cpon_finish(&cpon);
	}

	return 0;
}