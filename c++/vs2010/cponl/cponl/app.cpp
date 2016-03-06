#include "clcpon.h"
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
//#include <stdio.h>
#include <crtdbg.h>

char* modelpath = "cpon/cponmodel.csv";
static const char* testdatapath = "0096test.txt";//테스트용
static const unsigned int r = 1000, c = 100;

/**
\brief 학습할 데이터를 가져옵니다.
*/
void loadLearningData(unsigned int &row, unsigned int& col, double** &learningdata);

/**
\brief 예시 인식 데이터를 불러옵니다.
*/
static void loadTestData(unsigned int &row, unsigned int &col, double** &testdata);

int main(int argc, char* argv[]) {
	double **tdata = NULL;
	unsigned int row, col, trow, tcol;
	double **ldata = NULL;
	loadLearningData(row, col, ldata);
	
	//여기까지는 output.csv를 forward network의 output처럼 만들기 위해 작성한 코드입니다.
	struct lcpnet* cpon = lcpon_initialize(modelpath);//cpon model의 출력 경로를 설정함과 동시에 초기화합니다.
	lcpon_learn(row, col, ldata);//학습합니다.
	for(unsigned int i = 0 ; i < row ; free(ldata[i++]));
	free(ldata);
	//double learnratio = lcpon_measure(row, col, ldata, index);//학습율을 계산합니다.
	loadTestData(trow, tcol, tdata);

	//index를 생성합니다. 이 index는 measure에 사용됩니다.
	int* index = (int*)calloc(trow, sizeof(int));
	for(unsigned int i = 0 ; i < row ; index[i++] = 96);
	
	double clfratio = lcpon_measure(trow, tcol, tdata, index);//인식율을 계산합니다.
	lcpon_release();//메모리를 해제합니다.

	for(unsigned int i = 0 ; i < trow ; free(tdata[i++]));
	free(tdata);
	free(index);
}

void loadLearningData(unsigned int &row, unsigned int& col, double** &learningdata){
	std::ifstream dataStream("cpon/output.csv", std::ios::in);
	std::vector<std::vector<double>> data;
	std::string line, cell;

	while(std::getline(dataStream, line)){
		std::stringstream lineStream(line);
		std::vector<double> row;
		while(std::getline(lineStream, cell, ',')) row.push_back(std::stod(cell));
		data.push_back(row);
	}

	row = data.size(), col = data[0].size();
	learningdata = (double**)calloc(row, sizeof(double*));
	for(unsigned int i = 0 ; i < row ; i++){
		learningdata[i] = (double*)calloc(col, sizeof(double));
		for(unsigned int j = 0 ; j < col ; j++)
			learningdata[i][j] = data[i][j];
	}
}

static void loadTestData(unsigned int &row, unsigned int &col, double** &testdata){
	std::ifstream dataStream("0096test.txt", std::ios::in);
	std::vector<std::vector<double>> data;
	std::string line, cell;

	while(std::getline(dataStream, line)){
		line.erase(0, 1);
		std::stringstream lineStream(line);
		std::vector<double> row;
		while(std::getline(lineStream, cell, ' ')) row.push_back(std::stod(cell));
		data.push_back(row);
	}

	row = data.size(), col = data[0].size();
	testdata = (double**)calloc(row, sizeof(double*));
	for(unsigned int i = 0 ; i < row ; i++){
		testdata[i] = (double*)calloc(col, sizeof(double));
		for(unsigned int j = 0 ; j < col ; j++)
			testdata[i][j] = data[i][j];
	}
}