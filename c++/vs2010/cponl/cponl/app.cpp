#include "clcpon.h"
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

char* modelpath = "cpon/learning_result.csv";

void loadTestData();

int main(int argc, char* argv[]) {
	std::ifstream load("cpon/output.csv", std::ios::in);
	std::vector<std::vector<double>> vdata;
	std::string line, cell;

	while (std::getline(load, line)) {
		std::stringstream lineStream(line);
		std::vector<double> row;

		while(std::getline(lineStream, cell, ',')) row.push_back(stod(cell));
		vdata.push_back(row);
	}

	unsigned int row = vdata.size(), col = vdata[0].size();
	double** data = (double**)calloc(row, sizeof(double*));
	for(unsigned int i = 0 ; i < row ; i++){
		data[i] = (double*) calloc(col, sizeof(double));
		for(unsigned int j = 0 ; j < col ; j++)
			data[i][j] = vdata[i][j];
	}
	int* index = (int*)calloc(col, sizeof(int));
	for(unsigned int i = 0 ; i < col ; index[i++] = i);
	//��������� output.csv�� forward network�� outputó�� ����� ���� �ۼ��� �ڵ��Դϴ�.
	struct lcpnet* cpon = lcpon_initialize(modelpath);
	lcpon_learn(row, col, data);
	lcpon_release();
//	cpon_learn->fit(row, col, data, index); // importdata, buildnetwork, exportmodel, mesure�� �����Ѵ�. �̰� clcpon���� learn�� ��.
//	cpon_learn->learn();
//	delete cpon_learn; // �̰� clcpon���� release�� ��.
}

void loadTestData(){
	const char* path = "0096test.txt";

	std::ifstream load(path, std::ios::in);
	std::vector<std::vector<double>> tdata;
	std::string line, cell;

	while(std::getline(load, line)){
		std::stringstream lineStream(line);
		std::vector<double> row;

		while(std::getline(lineStream, cell, ' ')) row.push_back(stod(cell));
		tdata.push_back(row);
	}
}