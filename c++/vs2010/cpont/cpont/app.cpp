#include "cpontest.h"
#include <stdlib.h>

using namespace std;
char* path_import = "cpon/output.csv";//테스트용

int main(int argc, char* argv[]) {
	kil::tcpnet* testcpon= new kil::tcpnet();

	ifstream ttstream(path_import, ios::in);
	string line, cell;
	double* cpoutput = (double*)calloc(sizeof(double), 110);

	vector<double> row;
	while(getline(ttstream, line)){
		std::stringstream lineStream(line);
		
		while(getline(lineStream, cell, ',')) row.push_back(stod(cell));

		for(unsigned int i = 0 ; i < row.size(); i++){
			//한 번의 test마다 이 함수를 한번만 호출하면 됩니다.
			testcpon->test(cpoutput, row[i]);
			for(int i = 0 ; i < 110; cout << i << cpoutput[i++] << endl);
		}
		row.clear();
		for(int i = 0 ; i < 110; cpoutput[i++] = 0.);
	}

	delete cpoutput;
	delete testcpon;
}