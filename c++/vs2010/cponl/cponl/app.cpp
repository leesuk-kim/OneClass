#include "../../source/cponlearn.h"

using namespace std;
char* path_import = "cpon/output.csv";
char* path_export = "cpon/learning_result.csv";

//string dtos(double d);
///**
//\brief
//\author Leesuk kim, lktime@skku.edu
//*/
////ÀÌ°Ç learning
//void importdata(const char* path, kil::datamap* dm);
///***/
////void exportdata(const char* path, kil::lcpnet& cpoutput);

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

	kil::lcpnet cpon;
	cpon.buildnetwork();
	cpon.exportcpnet(path_export);
}


void importdata(const char* path, kil::datamap* dm) {
	ifstream load(path_import, ios::in);
	std::vector<std::vector<double>> result;
	std::string line, cell;

	while (std::getline(load, line)) {
		std::stringstream lineStream(line);
		std::vector<double> row;

		while(std::getline(lineStream, cell, ',')) row.push_back(stod(cell));
		result.push_back(row);
	}

	unsigned int clssize = result[0].size();
	std::vector<std::vector<double>>::iterator vvditer;

	for (unsigned int i = 0; i < clssize; i++) {
		std::vector<double> col;
		for (vvditer = result.begin(); vvditer != result.end(); vvditer++) col.push_back((*vvditer)[i]);

		char buf[9] = {'0' , };
		std::sprintf(buf, "%08d", i + 1);
		cell = std::string(buf);
		dm->insert(kil::datamap_pair(cell , col));
	}
}