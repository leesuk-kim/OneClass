#include "ctcpon.h"
#include <stdlib.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;
static const char* testdatapath = "0096test.txt";//�׽�Ʈ��
static const unsigned int r = 1000, c = 100;
static char* modelpath = "cpon/cponmodel.csv";//cpon �� ����Դϴ�.
/**
\brief ���� �ν� �����͸� �ҷ��ɴϴ�.
\details
�� �Լ��� ���� �ν� �����͸� �а� �� �پ� �����մϴ�.
���������� ���� �Ŀ� NULL�� �����մϴ�.
\author Leesuk Kim, lktime@skku.edu
*/
static double* getTestData();

int main(int argc, char* argv[]) {
	tcpon_setmodelpath(modelpath);//���� �⺻ cpon ���� ��ΰ� �ƴ϶�� ���� �����մϴ�.
	tcpnet* testcpon= tcpon_initialize();
	
	double* testdata; //�νĽ�ų �����͸� ����Ű�� �������Դϴ�..
	double* cpoutput = (double*)calloc(c, sizeof(double));//�ν� ����� ����Ű�� �������Դϴ�.

	while((testdata = getTestData()) != NULL)//���� �����͸� �޽��ϴ�.
		tcpon_test(cpoutput, testdata);	//100��¥�� �νĽ�ų ������(testdata)�� ������ 
										//100��¥�� �ν� ���(cpoutput)�� ��ȯ�մϴ�.

	free(cpoutput);
}

static double* getTestData(){
	static std::ifstream ttstream(testdatapath, ios::in);
	static string line, cell;
	static double* fwoutput = (double*)calloc(c, sizeof(double));
	static vector<double> row;
	row.clear();

	if(getline(ttstream, line)){
		line.erase(0, 1);
		stringstream lineStream(line);
		while(getline(lineStream, cell, ' ')) row.push_back(stod(cell));
		fwoutput = &row[0];
	}else{
		free(fwoutput);
		fwoutput = NULL;
	}
	return fwoutput;
}