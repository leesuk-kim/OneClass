#include "../../source/cpontest.h"

using namespace std;
char* path_import = "output.csv";
char* path_export = "learning_result.csv";

string dtos(double d);


int main(int argc, char* argv[]) {
	if(argc > 2){
		for(int i = 1 ; i < argc; i += 2){
			if(argv[i] == "-i") 
				path_import = argv[i + 1];
			else if(argv[i] == "-o")
				path_export = argv[i + 1];
		}
	} else if(argc == 2){
		if(!std::strcmp(argv[1],"-h"))
			printf("set input path: -i\nset output path: -o\nhelp: -h\n");
		else
			throw("wrong option!");
		return 0;
	}


	kil::tcpnet* testcpon= new kil::tcpnet();
	testcpon->importdata(path_export, testcpon);
	ifstream ttstream(path_import, ios::in);
	string line, cell;
	kil::comap* cpoutput = new kil::comap;
	vector<double> row;
	while(getline(ttstream, line)){
		std::stringstream lineStream(line);
		
		while(getline(lineStream, cell, ',')) row.push_back(stod(cell));

		for(unsigned int i = 0 ; i < row.size(); i++){
			//한 번의 test마다 이 함수를 한번만 호출하면 됩니다.
			testcpon->test(cpoutput, row[i]);
			for(kil::comap_iter comi = cpoutput->begin(); comi != cpoutput->end(); cout<<comi->second<<endl, comi++);
		}
		row.clear();
		cpoutput->clear();
	}
	delete cpoutput;
	delete testcpon;
}