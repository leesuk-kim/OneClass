#include "ctcpon.h"
#include <stdlib.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;
static const char* testdatapath = "0096test.txt";//테스트용
static const unsigned int r = 1000, c = 100;
static char* modelpath = "cpon/cponmodel.csv";//cpon 모델 경로입니다.
/**
\brief 예시 인식 데이터를 불러옵니다.
\details
이 함수는 예시 인식 데이터를 읽고 한 줄씩 리턴합니다.
마지막줄을 읽은 후엔 NULL을 리턴합니다.
\author Leesuk Kim, lktime@skku.edu
*/
static double* getTestData();

int main(int argc, char* argv[]) {
	tcpon_setmodelpath(modelpath);//만약 기본 cpon 모델의 경로가 아니라면 새로 설정합니다.
	tcpnet* testcpon= tcpon_initialize();
	
	double* testdata; //인식시킬 데이터를 가리키는 포인터입니다..
	double* cpoutput = (double*)calloc(c, sizeof(double));//인식 결과를 가리키는 포인터입니다.

	while((testdata = getTestData()) != NULL)//예시 데이터를 받습니다.
		tcpon_test(cpoutput, testdata);	//100개짜리 인식시킬 데이터(testdata)를 받으면 
										//100개짜리 인식 결과(cpoutput)를 반환합니다.

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