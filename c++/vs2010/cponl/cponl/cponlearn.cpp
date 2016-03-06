#include "cponlearn.h"

using namespace kil;

std::vector<int> beta::histogram(const std::vector<double>& data, unsigned int bins) {
	std::vector<int> hist(0);
	double step = 1 / ((double)bins);
	double area = step, x;
	int hvar = 0;

	for (unsigned int i = 0; i < data.size(); i++)
	{
		x = data[i];
		while (x > area)
		{
			hist.push_back(hvar);
			hvar = 0;
			area += step;
		}
		hvar++;
	}
	if (hist.size() == bins)
		hist.back()++;
	else
		while (hist.size() < bins)
		{
			hist.push_back(hvar);
			hvar = 0;
		}

	return hist;
}

std::vector<double> beta::cumulahisto(std::vector<int>& hist_ui, unsigned int bins) {
	std::vector<double> hist;
	std::sort(hist_ui.begin(), hist_ui.end());
	int size = std::accumulate(hist_ui.begin(), hist_ui.end(), 0);
	int cum = 0;

	for (unsigned int i = 0; i < hist_ui.size(); i++)
	{
		cum += hist_ui[i];
		hist.push_back((double)cum / (double)size);
	}

	return hist;
}

struct beta::betaparam_t beta::moment_match(const double& mean, const double& var) {
	static const double upper = 1., lower = 0.;
	struct beta::betaparam_t bp;
	double ml = mean - lower;
	double um = upper - mean;
	bp.alpha = fabs(mean * (((ml * um) / var) - 1));
	bp.beta = bp.alpha * (um / ml);
	
	return bp;
}

void beta::partial_kstest(struct beta::kstest_t& kst, struct beta::betaparam_t& bp) {
	double d = 0., dt, ff, fn, fo = 0., rv;
	
	for (unsigned int j = kst.front; j < kst.rear; j++)
	{
		fn = kst.ecdf[j];
		rv = (double)(j + 1) / (double)kst.size;
		ff = Beta::betai(bp.alpha, bp.beta, rv);
		dt = fabs(fn - ff);
		if (dt > d) d = dt;
	}
	kst.result.d = d;
	kst.result.pval = beta::qks((kst.en_sqrt + 0.23 / kst.en_sqrt) * d);
}

void beta::search_beta(struct beta::kstest_t& kst, struct beta::betaparam_t& bp) {
	double asign = 1., bsign = 1.;
	double pd = kst.result.d;

	for (unsigned int i = 0; i < 100 && kst.result.pval < 0.05; i++)
	{
		double astep = bp.alpha / 2., bstep = bp.beta / 2.;
		for (unsigned int j = 0; j < 100 && kst.result.pval < 0.05; j++)
		{
			if (bp.alpha + asign * astep < 0.)
				break;
			bp.alpha += asign * astep;

			partial_kstest(kst, bp);

			if (kst.result.d > pd) asign *= -1., astep *= .1;
			else pd = kst.result.d;
		}


		for (unsigned int j = 0; j < 100 && kst.result.pval < 0.05; j++)
		{
			if (bp.beta + bsign * bstep < 0.)
				break;
			bp.beta += bsign * bstep;

			partial_kstest(kst, bp);

			if (kst.result.d > pd) bsign *= -1., bstep *= .1;
			else pd = kst.result.d;
		}
		asign *= -1., bsign *= -1.;
	}
}


struct beta::ksresult_t beta::search_betamap(std::map<double, struct beta::betaparam_t>& betamap, const std::vector<double>& ecdf, struct beta::betaparam_t initbp) {
	unsigned int ecdf_size = ecdf.size();
	double en_sqrt = sqrt((double)ecdf_size);
	struct beta::kstest_t kst = {ecdf, ecdf_size, en_sqrt, 0, 1};
	struct beta::kstest_t prevkst = kst;
	struct ksresult_t ksr = {0., 1.};

	struct beta::betaparam_t prevbp = initbp;
	struct beta::betaparam_t presbp = prevbp;

	for (unsigned int i = 1 ; i <= ecdf_size; kst.rear = ++i)
	{
		partial_kstest(kst, prevbp);
		if (kst.result.pval < 0.05)
		{
			presbp = prevbp;
			//여기서 fitting을 해 본다.
			search_beta(kst, presbp);
			if (kst.result.pval < 0.05)
			{
				//앞부분 betaparam 저장
				betamap.insert(std::pair<double, struct betaparam_t>((double) --i / 100, prevbp));
				kst.front = i;
			}
			prevbp = presbp;
		}
		prevkst.result.pval = kst.result.pval;
		prevkst.rear = kst.rear;
	}
	betamap.insert(std::pair<double, struct betaparam_t>(1., prevbp));

	return ksr;
}

kil::pattern::pattern(std::vector<double> data){
	mData = data;
	
	mFeaturescaler = featurescaler::factory(mData);
	for(std::vector<double>::iterator vi = mData.begin(); vi != mData.end(); vi++)
		mFdata.push_back(mFeaturescaler->output(*vi));

	mFmean = mean(mFdata);
	mFvar = var(mFdata, mFmean);

	mKernelizer = new kernelizer(mFmean, mFvar);
	for(std::vector<double>::iterator vi = mFdata.begin(); vi != mFdata.end(); vi++)
		mKdata.push_back(mKernelizer->output(*vi));
}

kil::pattern::~pattern(){
	//mData.clear(), mFdata.clear(), mKdata.clear();
	delete mKernelizer;
	delete mFeaturescaler;
}


kil::pclearn::pclearn(int index, std::vector<double> pcptn): kil::probaclass(index){
	mPattern = new kil::pattern(pcptn);
}

kil::pclearn::~pclearn(){
	ecdf.clear(), m_betasketch.ecdf.clear();
	mBetamap.clear();
	delete mPattern;
}

std::vector<double> kil::pclearn::mapBeta(){
	struct beta::ksresult_t ksr = { 0., 0. };

	ecdf = beta::cumulahisto(mPattern->mKdata, 100);

	mMean = mean(ecdf);
	mVar = var(ecdf, mMean);

	m_ksr = beta::search_betamap(mBetamap, ecdf, beta::moment_match(mMean, mVar));

	m_betasketch.ecdf = ecdf;
	double bins = ecdf.size();
	for (unsigned int i = 1; i <= bins; i++)
	{
		double rv = (double)i / (double)bins;
		m_betasketch.beta.push_back(output(rv));
		m_betasketch.xaxis.push_back(rv);
	}

	m_betasketch.ksr = beta::kstest(m_betasketch.ecdf, m_betasketch.beta);
	return m_betasketch.beta;
}

double kil::pclearn::output(double& randomvariable){
	double prob = 0.;
	std::map<double, struct beta::betaparam_t>::iterator iter;
	bool isend = true;
	for (iter = mBetamap.begin(); randomvariable > iter->first && iter != mBetamap.end(); iter++);

	beta::betaparam_t bp = iter->second;
	prob = Beta::betai(bp.alpha, bp.beta, randomvariable);

	return prob;
}


kil::lcpnet::lcpnet(){
	mCPLmap = new cplmap;
	mTCPnet = new tcpnet;
}

kil::lcpnet::~lcpnet(){
	delete mTCPnet;
	for(cplmap_iter cplmi = mCPLmap->begin(); cplmi != mCPLmap->end();){
		pclearn* pcl = cplmi->second;
		cplmi++;
		delete pcl;
	}

	mCPLmap->clear();
	m_instance = NULL;
	delete mCPLmap;
}

kil::lcpnet* lcpnet::m_instance = NULL;
kil::cplmap* lcpnet::mCPLmap = NULL;
std::string lcpnet::mModelPath;
kil::tcpnet* lcpnet::mTCPnet = NULL;

void kil::lcpnet::fit(unsigned int row, unsigned int col, double** data){
	std::string colindex;

	for(unsigned int i = 0 ; i < col; i++){
		std::vector<double> coldata;
		for(unsigned int j = 0; j < row; j++)
			coldata.push_back(data[j][i]);

		insert(i, coldata);
	}

}

void kil::lcpnet::insert(int key, std::vector<double> values){
	mCPLmap->insert(cplmap_pair(key, new pclearn(key, values)));
}

double kil::lcpnet::measure(unsigned int row, unsigned int col, double** testdata, int* index){
	double m = 0.;
	unsigned int correct = 0, total = 0;
	double* res = (double*)calloc(col, sizeof(double));
	int* mapkeys = (int*)malloc(col * sizeof(int));
	unsigned int i = 0, predict;
	for(cplmap_iter cplmi = mCPLmap->begin(); cplmi != mCPLmap->end(); cplmi++, i++)
		mapkeys[i] = cplmi->first;

	for(unsigned int i = 0 ; i < row ; i++){
		mTCPnet->test(res, testdata[i]);
		if((predict = getMaxID(col, mapkeys, res)) == index[i]) correct++;
	}
	m = ((double)correct) / ((double)i);
	
	free(res);
	free(mapkeys);
	return m;
}

void kil::lcpnet::learn(){
	for(cplmap_iter cpmi = mCPLmap->begin(); cpmi != mCPLmap->end(); cpmi++)
		cpmi->second->mapBeta();

	exportModel(mModelPath.c_str());
	mTCPnet->importModel(mModelPath.c_str());
}

void kil::lcpnet::exportModel(std::string path){
	std::ofstream ofs(path, std::ios::out);
	std::string str = "";
	str = "name,fmin,fmax,kmean,kvar,";
	char betagauge[13] = "betagauge000";
	for(int i = 0 ; i < 100; i++, str += i == 100 ? '\n' : ','){
		sprintf(betagauge, "betagauge%03d", i);
		str += betagauge;
	}

	for(kil::cplmap_iter cpmi = mCPLmap->begin(); cpmi != mCPLmap->end(); cpmi++){
		str += std::ito8s(cpmi->first) + ',';
		str += std::dtos(cpmi->second->getPattern()->getFeaturescaler()->getMin()) + ',';
		str += std::dtos(cpmi->second->getPattern()->getFeaturescaler()->getMax()) + ',';
		str += std::dtos(cpmi->second->getPattern()->getKernelizer()->getMean()) + ',';
		str += std::dtos(cpmi->second->getPattern()->getKernelizer()->getVar()) + ',';
		std::vector<double> beta = cpmi->second->getBetasketch().beta;

		for(std::vector<double>::iterator vi = beta.begin(); vi != beta.end(); str += ++vi == beta.end() ? '\n' : ',')
			str += std::dtos(*vi);
	}
	ofs << str;
}

