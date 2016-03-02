#include "cponlearn.h"
#include <stdlib.h>

using namespace std;
char* path_import = "cpon/output.csv";
char* path_export = "cpon/learning_result.csv";
/*
해야될 일
1. double**로 learning data 받기
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
	//여기까지는 output.csv를 forward network의 output처럼 만들기 위해 작성한 코드입니다.

	kil::lcpnet* cpon_learn = kil::lcpnet::getInstance();

}
