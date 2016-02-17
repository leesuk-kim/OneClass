#include "classprobability.h"

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


void beta::partial_kstest(struct beta::kstest_t& kst, struct beta::betaparam_t& bp) {
	double d = 0., dt, ff, fn, fo = 0., rv;
	
	for (unsigned int j = kst.front; j < kst.rear; j++)
	{
		fn = kst.ecdf[j];
		rv = (double)(j + 1) / (double)kst.size;
		ff = boost::math::ibeta(bp.alpha, bp.beta, rv);
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


struct beta::ksresult_t beta::search_betamap(std::map<double, struct beta::betaparam_t>& betamap, const std::vector<double>& ecdf, struct pattern_t& ptn) {
	unsigned int ecdf_size = ecdf.size();
	double en_sqrt = sqrt((double)ecdf_size);
	struct beta::kstest_t kst = {ecdf, ecdf_size, en_sqrt, 0, 1};
	struct beta::kstest_t prevkst = kst;
	struct ksresult_t ksr = {0., 1.};

	struct beta::betaparam_t prevbp = beta::moment_match(ptn.mean, ptn.var);
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


probaclass::probaclass(std::string name) {
	m_name = name;
}

probaclass::probaclass(std::string name, std::vector<double> data) {
	m_name = name;
	insert(data);
}


//rawdata를 입력합니다. 기존의 데이터는 모두 사라집니다.
void probaclass::insert(std::vector<double> data) {
	m_pattern.data = std::vector<double>(data);
}


//기존 데이터에 새 데이터를 뒤에 추가합니다.
void probaclass::update(const std::vector<double>& data) {
	for (unsigned int i = 0, size = data.size(); i < size; i++)
		m_pattern.data.push_back(data[i]);
}


double probaclass::scale(double rv) {
	double r = rv < (double)m_pattern.imin ? 0. : rv >(double)m_pattern.imax ? 99. : (rv - m_pattern.imin + 1) * m_pattern.iratio;
	return r;
}


void probaclass::map_beta() {
	struct beta::ksresult_t ksr = { 0., 0. };

	//statistics 계산
	std::sort(m_pattern.data.begin(), m_pattern.data.end());		
	//m_pattern.mean = (double)(std::accumulate(m_pattern.data.begin(), m_pattern.data.end(), 0.) / ((double)m_pattern.data.size()));

	//double std = 0.;
	//for (unsigned int i = 0; i < m_pattern.data.size(); i++) {
	//	std += pow((m_pattern.data[i] - m_pattern.mean), 2);
	//}
	//m_pattern.var = std / ((double)(m_pattern.data.size() - 1));
	

	//ecdf를 만들고
	m_pattern.ecdf = beta::cumulahisto(m_pattern.data, 100);
	//ecdf에 대한 featurescaling을 합니다.
	
	for (unsigned int i = 0; i < m_pattern.ecdf.size(); i++)
		if (m_pattern.ecdf[i] > 0.) {
			m_pattern.imin = i;
			break;
		}
	for (int i = (int)m_pattern.ecdf.size() - 1; i >= 0; i--)
		if (m_pattern.ecdf[i] < 1.) {
			m_pattern.imax= i;
			break;
		}
	//1은 서수 총 개수를 구하기 위해서, 1은 맨 뒤 1.을 추가하기 위해서
	m_pattern.iratio = 99. / (double)(m_pattern.imax - m_pattern.imin + 2);

	std::vector<double> scaledecdf;
	double srv;
	unsigned int size = m_pattern.ecdf.size();
	scaledecdf.assign(size, 0.);
	int k;
	for (unsigned int i = m_pattern.imin; i <= m_pattern.imax; i++) {
		srv = scale((double)i);
		k = (int) srv;
		for (int j = (int)size - 1; j >= k; j--)
			scaledecdf[j] = m_pattern.ecdf[i];
	}
	for (unsigned int i = m_pattern.imax; i < m_pattern.ecdf.size(); i++)
		scaledecdf[i] = 1.;
	for (unsigned int i = 0; i < size; i++)
		m_pattern.ecdf[i] = scaledecdf[i];

	for (unsigned int i = 0; i < m_pattern.ecdf.size(); m_pattern.mean += m_pattern.ecdf[i++]);
	m_pattern.mean /= (double)m_pattern.ecdf.size();
	m_pattern.var = 0;
	for (unsigned int i = 0; i < m_pattern.ecdf.size(); i++)
		m_pattern.var += pow((m_pattern.ecdf[i] - m_pattern.mean), 2.);
	m_pattern.var /= (double)(m_pattern.ecdf.size() - 1.);
	
	/////////////////////////////////////////////////////
	//최적의 beta를 찾습니다. 거의 만든다는 느낌입니다.//
	/////////////////////////////////////////////////////
	m_ksr = beta::search_betamap(m_betamap, m_pattern.ecdf, m_pattern);

	m_betasketch.ecdf = m_pattern.ecdf;
	double bins = m_pattern.ecdf.size();
	for (unsigned int i = 1; i <= bins; i++)
	{
		double rv = (double)i / (double)bins;
		m_betasketch.beta.push_back(cls_prob_signed(m_betamap, rv));
		m_betasketch.xaxis.push_back(rv);
	}

	m_betasketch.ksr = beta::kstest(m_betasketch.ecdf, m_betasketch.beta);

}


struct beta::betasketch_t probaclass::get_betasketch() {
	return m_betasketch;
}


/*
\brief 하나의 class에 대한 class probability를 계산합니다.
\details positive 또는 negative class의 class probability를 계산합니다.
probability of positive class밖에 구할 수가 없어서 Probaclass에는 probability of negative class를 저장하는 변수가 없습니다만,
만약 만들 경우 이 함수를 사용하여 각 negative class별 probability를 구할 수 있습니다.

\author Leesuk Kim, lktime@skku.edu
*/
double probaclass::cls_prob_signed(std::map<double, beta::betaparam_t>& betamap, const double& rv) {
	double prob = 0.;
	std::map<double, beta::betaparam_t>::iterator iter;
	bool isend = true;
	for (iter = betamap.begin(); rv > iter->first && iter != betamap.end(); iter++);

	beta::betaparam_t bp = iter->second;
	prob = boost::math::ibeta(bp.alpha, bp.beta, rv);

	return prob;
}


double probaclass::cls_prob(const double& rv) {
	return cls_prob_signed(m_betamap, rv);
}


//////////////////////////////////////
//////////////////////////////////////
//EXAMPLE CLASS MEMBERS DECLARATIONS//
//////////////////////////////////////
//////////////////////////////////////
cpnetwork::cpnetwork() {
	m_cpmap = new std::map<std::string, probaclass>;
}

cpnetwork::~cpnetwork() {
}

cpnetwork::cpnetwork(cpnmap* cpmap) {
	m_cpmap = cpmap;
}

cpnetwork::cpnetwork(datamap* dm) {
	for (datamap_iter mapiter = dm->begin(); mapiter != dm->end(); ++mapiter)
		insert(mapiter->first, mapiter->second);
}


cpnmap* cpnetwork::getCpmap() {
	return m_cpmap;
}


void cpnetwork::insert(std::string key, std::vector<double> value) {
	//If learning data is to Large, this methods prunes 100 the data from front.
	//std::vector<double> vec(&value[0], &value[100]);
	//Probaclass cp(key, vec);

	probaclass cp(key, value);
	insert(cp);
}


void cpnetwork::insert(probaclass cp) {	//if the element of the key exists
	if (m_cpmap->find(cp.m_name) != m_cpmap->end()) m_cpmap->erase(cp.m_name);
	m_cpmap->emplace(cp.m_name, cp);
}


void cpnetwork::update(std::string key, std::vector<double> value) {
	cpnmap_iter mi = m_cpmap->find(key);
	if (mi == m_cpmap->end())
		throw("The learning class %s does not exist. please use the method \'insert()\'.", key);
	(*mi).second.update(value);
}


/*Class Network를 구성합니다.
Designer: Leesuk Kim (lktime@skku.edu)
*/
void cpnetwork::build_network() {
	for (cpnmap_iter mapiter = m_cpmap->begin(); mapiter != m_cpmap->end(); ++mapiter)
	{
		std::cout << mapiter->first << std::endl;
		mapiter->second.map_beta();
	}
}
