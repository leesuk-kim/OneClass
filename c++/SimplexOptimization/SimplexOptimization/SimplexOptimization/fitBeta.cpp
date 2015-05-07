#include "fitBeta.h"

bool fitBeta::getSampleData(const char* filePath){
	int i,n = 1;
	ifstream fin;
	fin.open(filePath);

	if (!fin.is_open())
		return false;

	sampleSize = 0;
	sampleMean = 0.0f;
	sampleVariance = 0.0f;
	
	
	while (!fin.eof()){
		fin >> sampleData[sampleSize];
		sampleMean += sampleData[sampleSize];

		sampleSize++;
	}

	sampleSize--;

	sort(sampleData, sampleData + sampleSize);

	sampleMean /= (double)sampleSize;
	
	for (i = 0; i < sampleSize; i++)
		sampleVariance += (sampleData[i] - sampleMean)*(sampleData[i] - sampleMean);

	sampleVariance /= (double)(sampleSize - 1);

	return true;
}

void fitBeta::initParameters(){
	double m = sampleMean, s_2 = sampleVariance, L = sampleData[0], U = sampleData[sampleSize - 1];

	initParams[0] = ((m - L)*(U - m) / s_2 - 1)*(m - L) / (U - L);
	initParams[1] = initParams[0] * (U - m) / (m - L);
	//initParams[2] = L;
	//initParams[3] = U;
}

void fitBeta::fitBetaDist(){
	vector<double> init(initParams, initParams + 2);

	using BT::Simplex;
	vector<double> estimateParams = Simplex(f, init, 1e-35);

	resultParams[0] = estimateParams[0];
	resultParams[1] = estimateParams[1];
//	resultParams[2] = estimateParams[2];
//	resultParams[3] = estimateParams[3];

	dn = getKSStatistic(estimateParams);
	pValue = getKSPValue(dn);
}


double fitBeta::getBetaCDF(double x, vector<double> params)
{
	//double a = params[0], b = params[1], L = params[2], U = params[3];
	double a = params[0], b = params[1], L = sampleData[0], U = sampleData[sampleSize - 1];
	double normalizedX = (x - L) / (U - L);

	if (normalizedX < 0)
		return 0;
	else if (normalizedX > 1.0)
		return 1;

	try {
		return ibeta(a, b, normalizedX);
	}
	catch (exception e){
		cout << "a or b is 0" << endl;
		throw e;
	}
}

double fitBeta::getKSStatistic(vector<double> params){
	int i;
	//double a = params[0], b = params[1], L = params[2], U = params[3];
	double a = params[0], b = params[1], L = sampleData[0], U = sampleData[sampleSize-1];
	double cdf = getBetaCDF(sampleData[0], params);
	double D = 1 / (double)sampleSize - cdf, d;

	for (i = 0; i < sampleSize; i++){
		if (i != sampleSize - 1 && sampleData[i] == sampleData[i + 1])
			continue;

		double cdf = getBetaCDF(sampleData[i], params);

		d = abs(cdf - (i+1) / (double)sampleSize);

		if (D < d)
			D = d;
	}

	return D;
}

double fitBeta::getKSPValue(double dn){
	int i = 1;
	double t = sqrt(sampleSize) * dn;
	double cur, sum = exp(-2 * t*t);
	double diff = 1e-100;
	bool tf = true;

	do{
		i++;
		cur = exp(-2 * i * i * t * t);

		if (diff >= cur)
			tf = false;

		if ((i & 0x1) == 0)
			cur *= -1;
		sum += cur;
	} while (tf);

	return 1 - (1 - 2 * sum);
}


fitBeta::fitBeta(double(*targetFunction)(vector<double> params)){
	fout.open("result.txt");
	sampleData = (double*)calloc(1000000, sizeof(double));
	f = targetFunction;
}

void fitBeta::checkParameter(const char* filePath, vector<double> params){
	int i;

	if (!getSampleData(filePath))
		return;
	dn = getKSStatistic(params);
	pValue = getKSPValue(dn);

	fout << filePath << "(a, b, L, U, Dn, pValue)" << endl;

	for (i = 0; i < 4; i++)
		fout << setw(10) << params[i];

	fout << setw(10) << dn;
	fout << setw(10) << pValue;
	fout << endl;

//	free(sampleData);
}

/*DEPRECATED?*/
void fitBeta::doFitting(const char* filePath){
	int i;
	double* elementResult = (double*)calloc(6, sizeof(double));
	ofstream fout;

	double *data;

	srand(time(NULL));

	if (!getSampleData(filePath))
		return;

	if (sampleSize > 1000){

		data = (double*)calloc(1000, sizeof(double));

		for (i = 0; i < 1000; i++){
			int j = (int)(rand() / (double)RAND_MAX * sampleSize);
			data[i] = sampleData[j];
		}

		sort(data, data + 1000);
		sampleData = data;
		sampleSize = 1000;
	}

	initParameters();
	fitBetaDist();
	
	elementResult[0] = resultParams[0];
	elementResult[1] = resultParams[1];
	elementResult[2] = resultParams[2];
	elementResult[3] = resultParams[3];
	elementResult[4] = dn;
	elementResult[5] = pValue;
	vectorResult.push_back(elementResult);

	fout << filePath << "(a, b, L, U, Dn, pValue)" << endl;

	for (i = 0; i < 4; i++)
		fout << setw(8) << resultParams[i];

	fout << setw(8) << dn;
	fout << setw(8) << pValue;
	fout << endl;
}
