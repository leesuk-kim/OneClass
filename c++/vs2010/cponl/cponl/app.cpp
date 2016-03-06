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
static const char* testdatapath = "0096test.txt";//�׽�Ʈ��
static const unsigned int r = 1000, c = 100;

/**
\brief �н��� �����͸� �����ɴϴ�.
*/
void loadLearningData(unsigned int &row, unsigned int& col, double** &learningdata);

/**
\brief ���� �ν� �����͸� �ҷ��ɴϴ�.
*/
static void loadTestData(unsigned int &row, unsigned int &col, double** &testdata);

int main(int argc, char* argv[]) {
	double **tdata = NULL;
	unsigned int row, col, trow, tcol;
	double **ldata = NULL;
	loadLearningData(row, col, ldata);
	
	//��������� output.csv�� forward network�� outputó�� ����� ���� �ۼ��� �ڵ��Դϴ�.
	struct lcpnet* cpon = lcpon_initialize(modelpath);//cpon model�� ��� ��θ� �����԰� ���ÿ� �ʱ�ȭ�մϴ�.
	lcpon_learn(row, col, ldata);//�н��մϴ�.
	for(unsigned int i = 0 ; i < row ; free(ldata[i++]));
	free(ldata);
	//double learnratio = lcpon_measure(row, col, ldata, index);//�н����� ����մϴ�.
	loadTestData(trow, tcol, tdata);

	//index�� �����մϴ�. �� index�� measure�� ���˴ϴ�.
	int* index = (int*)calloc(trow, sizeof(int));
	for(unsigned int i = 0 ; i < row ; index[i++] = 96);
	
	double clfratio = lcpon_measure(trow, tcol, tdata, index);//�ν����� ����մϴ�.
	lcpon_release();//�޸𸮸� �����մϴ�.

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