#include "cpontest.h"


kil::pctest::pctest(std::string name, double fmin, double fmax, double kmean, double kvar, std::vector<double> betagauge): kil::probaclass(name){
	mFeaturescaler = new featurescaler(fmin, fmax);
	mKernelizer = new kernelizer(kmean, kvar);
	mBetagauge = betagauge;
}

double kil::pctest::output(double& randomvariable){
	randomvariable = mFeaturescaler->output(randomvariable);
	randomvariable = mKernelizer->output(randomvariable);
	unsigned int i = (unsigned int) (randomvariable * 100.);
	return mBetagauge[i];
}

kil::pctest::~pctest(){
	delete mFeaturescaler;
	delete mKernelizer;
}

kil::tcpnet::tcpnet(){
	mCPTmap = new cptmap;
}

kil::tcpnet* kil::tcpnet::m_instance = NULL;

kil::tcpnet::~tcpnet(){
	for(cptmap_iter cpti = mCPTmap->begin(); cpti != mCPTmap->end(); delete cpti->second, cpti++);
	delete mCPTmap;
}

void kil::tcpnet::insert(kil::pctest* pct){
	mCPTmap->insert(cptmap_pair(pct->getName(), pct));
}

void kil::tcpnet::test(double* res, double* fwoutput){
	unsigned int i = 0;
	for(cptmap_iter cpmi = mCPTmap->begin(); cpmi != mCPTmap->end(); cpmi++, i++)
		res[i] = cpmi->second->output(fwoutput[i]);
}

void kil::tcpnet::importModel(const char* modelpath){
	std::ifstream load(modelpath, std::ios::in);
	std::string line, cell, name;
	double fmin, fmax, kmean, kvar;
	std::getline(load, line);

	while (std::getline(load, line)) {
		std::stringstream lineStream(line);
		std::vector<double> betagauge;

		std::getline(lineStream, name, ',');
		std::getline(lineStream, cell, ',');
		fmin = stod(cell);
		std::getline(lineStream, cell, ',');
		fmax = stod(cell);
		std::getline(lineStream, cell, ',');
		kmean = stod(cell);
		std::getline(lineStream, cell, ',');
		kvar = stod(cell);
		while(std::getline(lineStream, cell, ',')) betagauge.push_back(stod(cell));

		kil::pctest* pct = new kil::pctest(name, fmin, fmax, kmean, kvar, betagauge);
		insert(pct);
	}
}