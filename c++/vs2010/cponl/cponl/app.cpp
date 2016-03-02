#include "cponlearn.h"
#include <stdlib.h>

using namespace std;
char* path_import = "cpon/output.csv";
char* path_export = "cpon/learning_result.csv";
/*
�ؾߵ� ��
1. double**�� learning data �ޱ�
2. 
*/
int main(int argc, char* argv[]) {
	std::ifstream load("cpon/output.csv", std::ios::in);
	std::vector<std::vector<double>> result;
	std::string line, cell;

	while (std::getline(load, line)) {
		std::stringstream lineStream(line);
		std::vector<double> row;

		while(std::getline(lineStream, cell, ',')) row.push_back(stod(cell));
		result.push_back(row);
	}

	unsigned int row = result.size(), col = result[0].size();
	double** dresult = (double**)calloc(col, sizeof(double*));
	for(unsigned int i = 0 ; i < row ; i++)
		dresult[i] = (double*) calloc(result[i].size(), sizeof(double));
	//��������� output.csv�� forward network�� outputó�� ����� ���� �ۼ��� �ڵ��Դϴ�.

	kil::lcpnet* cpon_learn = kil::lcpnet::getInstance();

}
