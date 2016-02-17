#include "classprobability.h"
#include <stdlib.h>
#include <fstream>
#include <sstream>

using namespace std;
const char* path_data = "output.csv";

/**
\brief
\author Leesuk kim, lktime@skku.edu
*/
std::vector<std::string> tokelizer(std::istream& str);
/**
\brief
\author Leesuk kim, lktime@skku.edu
*/
void load_data(const char* path, kil::datamap* dm);
/**
\brief
\author Leesuk kim, lktime@skku.edu
*/
void fpval(kil::cpnetwork cpon);


int main() {
	kil::datamap* src_map = new kil::datamap;
	load_data(path_data, src_map);
	kil::cpnetwork mycpon(src_map);
	mycpon.build_network();

	fpval(mycpon);
	delete src_map;
}


void load_data(const char* path, kil::datamap* dm) {
	ifstream load(path_data, ios::in);
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


/*KS-test의 결과값인 D와 p-value, 그리고 tolerance를 출력
ECDF와 BCDF를 출력
*/
void fpval(kil::cpnetwork cpon) {
	kil::cpnmap_iter iter;
	for (iter = cpon.getCpmap()->begin(); iter != cpon.getCpmap()->end(); ++iter) 	{
		kil::probaclass cp = iter->second;
		ofstream of(string("scaled_pvalue\\") + string(cp.m_name) + string("_pvalue.csv"));

		//cpon::betasketch_t abpr = cp.map_beta();
		kil::beta::betasketch_t abpr = cp.get_betasketch();
		string comma = string(","), endline = string("\n");
		
		of << cp.m_name << comma << abpr.ksr.d << comma << abpr.ksr.pval << endl;
		string str = string(""); 
		for (unsigned int i = 0; i < abpr.ecdf.size(); str += ++i == abpr.ecdf.size() ? endline : comma)
			str += boost::lexical_cast<std::string>(abpr.ecdf[i]);
		of << str;

		str = string("");
		for (unsigned int i = 0; i < abpr.beta.size(); str += ++i == abpr.ecdf.size() ? endline : comma)
			str += boost::lexical_cast<std::string>(abpr.beta[i]);
		of << str;

		str = string("");
		for (unsigned int i = 0; i < abpr.xaxis.size(); str += ++i == abpr.xaxis.size() ? endline : comma)
			str += boost::lexical_cast<std::string>(abpr.xaxis[i]);
		of << str;
		of.close();
	}
}