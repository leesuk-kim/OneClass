#include "cponcommon.h"

using namespace kil;

std::string std::dtos(double d){
	std::ostringstream oss;
	oss<<d;
	return oss.str();
}

std::string std::ito8s(int i){
	char buf[9] = {'0', };
	std::sprintf(buf, "%08d", i);
	std::string s = buf;//이거 되나? api보니까 된다던데...
	return s;
}


double kil::mean(std::vector<double> doublevec){
	double m = 0.;
	for(std::vector<double>::iterator vi = doublevec.begin(); vi != doublevec.end() ; vi++)
		m += *vi;
	m /= doublevec.size();

	return m;
}
double kil::var(std::vector<double> doublevec, double mean){
	double v = 0.;
	for(std::vector<double>::iterator vi = doublevec.begin(); vi != doublevec.end() ; vi++)
		v += pow(mean - *vi, 2);
	v /= (doublevec.size() - 1);

	return v;
}


double beta::qks(const double& alam) {
	//Return complementary cumulative distribution function.
	static const double EPS1 = 0.001;
	static const double EPS2 = 1.0e-8;
	unsigned int j;
	double a2, fac = 2., sum = 0., term, termbf = 0.;
	a2 = -2. * alam * alam;
	for (j = 1; j <= 100; j++)
	{
		term = fac * exp(a2 * j * j);
		sum += term;
		if (fabs(term) <= EPS1 * termbf || fabs(term) <= EPS2 * sum)
			return sum;
		fac = -fac;
		termbf = fabs(term);
	}
	return 1.;

}

struct beta::ksresult_t beta::kstest(std::vector<double>& sample1, std::vector<double>& sample2) {
	struct beta::ksresult_t ksr = { 0., 0.};
	unsigned int j1 = 0, j2 = 0, n1 = sample1.size(), n2 = sample2.size();
	double d = 0., v1, v2, en = sqrt((double)(n1 * n2) / (double)(n1 + n2)), dt;
	if (n1 != n2) throw("size of each sample must be same.");

	for (unsigned int i = 0; i < n1; i++)
	{
		v1 = sample1[i], v2 = sample2[i];
		if ((dt = fabs(v1 - v2)) > d)
			d = dt;
	}
	ksr.d = d;
	ksr.pval = beta::qks((en + 0.23 / en) * d);

	return ksr;
}

kil::featurescaler::featurescaler(double min, double max){
	mMin = min, mMax = max, mDiffNx = max - min;
}

kil::featurescaler* kil::featurescaler::factory(std::vector<double> doublevec){
	std::sort(doublevec.begin(), doublevec.end());
	double min = doublevec.front(), max = doublevec.back();

	return new featurescaler(min, max);
}

double kil::featurescaler::output(const double& randomvariable){
	return (randomvariable - mMin) / mDiffNx;
}


kil::kernelizer::kernelizer(double mean, double var){
	mMean = mean, mVar = var;
}

double kil::kernelizer::_kernel(double& randomvariable){
	return pow(randomvariable - mMean, 2) / mVar;
}

double kil::kernelizer::output(double& randomvariable){
	double k = 0.;
	k = exp(-0.5 * _kernel(randomvariable));
	return k;
}