#include "classprobability.h"
#include <stdlib.h>
#include <fstream>
#include <sstream>

using namespace std;

std::vector<std::string> tokelizer(std::istream& str);
void load_data(kil::datamap* dm);
void fpval(kil::cpnetwork cpon);

int main()
{
	kil::cpnetwork mycpon;

	kil::datamap* src_map = new kil::datamap;
	load_data(src_map);
	mycpon.insert(src_map);
	mycpon.build_network();

	fpval(mycpon);
	//predict_result(mycpon, src_map);
	delete src_map;
}


void load_data(kil::datamap* dm)
{
	ifstream load("output.csv", ios::in);
	string str;

	while (getline(load, str, ','))
	{
		/*
		이제 각 줄마다 각 클래스로 보내줘야 하는데 이거 너무 거지같은듯...
		*/
	}
}


std::vector<std::string> tokelizer(std::istream& str)
{
	std::vector<std::string> result;
	std::string line;
	std::getline(str, line);
	std::stringstream lineStream(line);
	std::string cell;

	while (std::getline(lineStream, cell, ','))
	{
		result.push_back(cell);
	}
	return result;
}


/*KS-test의 결과값인 D와 p-value, 그리고 tolerance를 출력
ECDF와 BCDF를 출력
*/
void fpval(kil::cpnetwork cpon)
{
	kil::cpnmap_iter iter;
	for (iter = cpon.getCpmap()->begin(); iter != cpon.getCpmap()->end(); ++iter)
	{
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