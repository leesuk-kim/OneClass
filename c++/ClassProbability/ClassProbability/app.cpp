#pragma warning(disable : 4251)

#include "classprobability.h"
#include "mysql_connection.h"
#include <cppconn\driver.h>
#include <cppconn\exception.h>
#include <cppconn\resultset.h>
#include <cppconn\statement.h>
#include <stdlib.h>
#include <fstream>

using namespace std;


struct sqlres_t
{
	std::string name;
	std::vector<double> data;
};


void loadData(std::map<std::string, std::vector<double>>* data_map);
void loadData(std::map<std::string, std::vector<double>>* data_map, std::string key);
void fpval(classprobability::CPON cpon);


int main()
{
	classprobability::CPON mycpon;

	classprobability::datamap* src_map = new classprobability::datamap;
	loadData(src_map);//전체 데이터
	//loadData(src_map, "E0061");//특정 데이터만

	mycpon.insert(src_map);
	mycpon.build_network();

	fpval(mycpon);
	//predict_result(mycpon, src_map);
	delete src_map;
}


void loadData(std::map<std::string, std::vector<double>>* data_map)
{
	try {
		sql::Driver *driver;
		sql::Connection *con;
		sql::Statement *stmt;
		sql::ResultSet *res;

		/* Create a connection */
		driver = get_driver_instance();
		con = driver->connect("221.153.219.220:8806", "niplab", "qwqw`12");
		/* Connect to the MySQL test database */
		con->setSchema("LIG");

		stmt = con->createStatement();
		res = stmt->executeQuery("SELECT * from `naeultech-151208`");
		while (res->next()) {
			std::string str = res->getString(2).c_str();
			double d = res->getDouble(3);
			std::map<std::string, std::vector<double>>::iterator fres = data_map->find(str);
			if (fres == data_map->end())
			{
				std::vector<double> vec;
				vec.push_back(d);
				data_map->insert(std::map<std::string, std::vector<double>>::value_type(str, vec));
			}
			else
			{
				fres->second.push_back(d);
			}
			/* Access column data by alias or column name */
		}
		delete res;
		delete stmt;
		delete con;

	}catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
}


/*원하는 key의 data만 받음
*/
void loadData(std::map<std::string, std::vector<double>>* data_map, std::string key)
{
	loadData(data_map);
	std::vector<double> sub;
	std::map<std::string, std::vector<double>>::iterator iter;
	iter = data_map->find(key);
	sub = std::vector<double>(iter->second);
	data_map->clear();
	(*data_map)[key] = sub;
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