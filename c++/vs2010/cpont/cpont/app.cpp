#include "cpontest.h"
#include <stdlib.h>

using namespace std;
char* testdatapath = "0096test.txt";//테스트용

int main(int argc, char* argv[]) {
	kil::tcpnet* testcpon= new kil::tcpnet();

	ifstream ttstream(testdatapath, ios::in);
	string line, cell;
	double* cpoutput = (double*)calloc(sizeof(double), 110);

	vector<double> row;
	unsigned int r = 1000, c = 100;
	
	double* fwoutput = (double*)calloc(c, sizeof(double));

	while(getline(ttstream, line)){
		line.erase(0, 1);
		std::stringstream lineStream(line);
		
		while(getline(lineStream, cell, ' ')) row.push_back(stod(cell));
		fwoutput = &row[0];
		for(unsigned int i = 0 ; i < row.size(); i++){
			//한 번의 test마다 이 함수를 한번만 호출하면 됩니다.
			testcpon->test(cpoutput, fwoutput);
			for(unsigned int i = 0 ; i < c; cout << i << ':' << cpoutput[i++] << ends);
			cout<<endl;
		}

		row.clear();
		for(int i = 0 ; i < c; cpoutput[i++] = 0.);
	}
	free(fwoutput);
	delete cpoutput;
	delete testcpon;
}