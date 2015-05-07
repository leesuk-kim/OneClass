#include <vector>
#include <limits>
#include <algorithm>
#include <functional>
#include <iostream>
#include <fstream>
#include "simplex.h"
#include <boost\math\distributions\beta.hpp>

using namespace std;
using namespace boost::math;
/*fitting beta distribution
find fitting beta distribution using empirical beta.
*/
class fitBeta{
public:
	int sampleSize = 0;
	double* sampleData = NULL;
	double initParams[4], resultParams[4];
	double dn= 0.0f, pValue = 0.0f;
	double sampleMean = 0.0f, sampleVariance = 0.0f;
	vector<double*> vectorResult;
	
	void checkParameter(const char* filePath, vector<double> params);
	void doFitting(const char* filePath);
	double(*f)(vector<double> params);

	fitBeta(double(*targetFunction)(vector<double> params));
	double getBetaCDF(double x, vector<double> params);

private:
	bool getSampleData(const char* filePath);
	void initParameters();
	void fitBetaDist();
	double getKSStatistic(vector<double> params);
	double getKSPValue(double dn);

	ofstream fout;
};