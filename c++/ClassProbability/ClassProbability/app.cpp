#include "classprobability.h"
#include <stdlib.h>
#include <fstream>

using namespace std;


struct sqlres_t
{
	std::string name;
	std::vector<double> data;
};


void fpval(classprobability::CPON cpon);


int main()
{
	classprobability::CPON mycpon;

	classprobability::datamap* src_map = new classprobability::datamap;

	mycpon.insert(src_map);
	mycpon.build_network();

	fpval(mycpon);
	//predict_result(mycpon, src_map);
	delete src_map;
}


/*KS-test의 결과값인 D와 p-value, 그리고 tolerance를 출력
ECDF와 BCDF를 출력
*/
void fpval(classprobability::CPON cpon)
{
	classprobability::cpon_map_iter iter;
	for (iter = cpon.getCpmap()->begin(); iter != cpon.getCpmap()->end(); ++iter)
	{
		classprobability::Probaclass cp = iter->second;
		ofstream of(string("scaled_pvalue\\") + string(cp.m_name) + string("_pvalue.csv"));

		//cpon::betasketch_t abpr = cp.map_beta();
		classprobability::beta::betasketch_t abpr = cp.get_betasketch();
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