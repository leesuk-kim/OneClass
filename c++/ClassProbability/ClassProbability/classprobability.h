#include <iostream>
#include <vector>
#include <map>
#include <numeric>
#include <algorithm>

#include <boost\math\distributions\beta.hpp>
#include <boost\accumulators\accumulators.hpp>
#include <boost\accumulators\statistics\mean.hpp>
#include <boost\accumulators\statistics\variance.hpp>

/**
\namespace kil
\brief 본 프로젝트의 모든 내용을 담고 있는 namespace입니다.

\author Leesuk kim, lktime@skku.edu
*/
namespace kil {
	/**
	\brief struct pattern
	\details pattern 관련 정보를 저장합니다.

	\author Leesuk kim, lktime@skku.edu
	*/
	struct pattern_t {
		std::vector<double> data;
		std::vector<double> ecdf;
		unsigned int imin = 0, imax = 100;
		double iratio = 1.;
		double mean, var;
	};

	/**
	\namespace beta
	\brief beta function에 대한 namespace입니다.

	\author Leesuk kim, lktime@skku.edu
	*/
	namespace beta {
		/**
		\brief struct for contatining parameters of beta function.
		\details Boost Library의 beta 함수는 random variable과 paramter를 동시에 입력해야 beta output이 출력됩니다.
		random variable은 사용할 때마다 달라지니, 결국 특정 class의 shape of beta function을 결정하는 것은 beta parameter입니다.
		따라서 beta parameter인 alpha와 beta값을 하나의 struct로 묶어서 관리합니다.

		\author Leesuk kim, lktime@skku.edu
		*/
		struct betaparam_t {
			double alpha;
			double beta;
		};

		/**
		\brief kstest 결과를 저장합니다.

		\author Leesuk kim, lktime@skku.edu
		*/
		struct ksresult_t {
			/**Kolmogorov-Smirnov statistic*/
			double d;
			/**p-value*/
			double pval;
		};

		/**
		\brief kstest할 때 기본적으로 필요한 data들을 저장합니다.

		\author Leesuk kim, lktime@skku.edu
		*/
		struct kstest_t {
			const std::vector<double>& ecdf;
			const unsigned int size;
			const double en_sqrt;
			unsigned int front, rear;
			struct ksresult_t result;
		};

		/**
		\brief fragmentated beta shape를 그립니다.
		\details 
		fragmentated beta shape란 random varialble의 크기에 따라 해당하는 beta function을 가지고 있는 struct입니다.
		이것은 beta fitting algorithm으로 fitting할 수 없었던 극단적인 케이스도 fitting할 수 있도록
		여러 개의 beta function을 가지고 있으며, 각각의 beta function이 cover하는 random variable을 저장하는 xaxis가 있습니다.

		\author Leesuk kim, lktime@skku.edu
		*/
		struct betasketch_t{
			std::vector<double> ecdf, beta, xaxis;
			struct ksresult_t ksr;
		};

		/**
		\brief histrogram을 그립니다.

		\details
		Computes the histogram of the parameter 'data' with the parameter 'bins'.
		The 'bins' is larger than 30, proper size is 100.
		It returns a vector<int> which indicates the size of equally divided bounds.
		before use this method, please std::sort first.

		\param data the type of this parameter is 'std::vector<int>'.
		\param bins the type of this parameter is 'unsigned int'.

		\return a histogram. the type of this parameter is 'std::vector<int>'.

		\author Leesuk Kim, lktime@skku.edu
		*/
		std::vector<int> histogram(const std::vector<double>& data, unsigned int bins);

		/*
		\brief cumulative histogram.
		\details
		Computes the data of cumulative histogram. It depends on classprobability::histogram.
		It cumulates The return of classprobability::histogram and nomalizes from 0 to 1.

		\param data: The target data of pointer of vector<double> instance.
		\param bins: bin size
		
		\return empirical CDF

		\author Leesuk Kim, lktime@skku.edu
		*/
		std::vector<double> cumulahisto(std::vector<int>& hist_ui, unsigned int bins);
		/**
		*/
		inline std::vector<double> cumulahisto(std::vector<double>& data, unsigned int bins) { return cumulahisto(histogram(data, bins), bins); }

		/**
		\brief beta function의 알파와 베타의 초기값을 생성합니다.

		\details
		기본적으로, beta function은 알파와 베타에 의해 shape가 정의됩니다.
		하지만 알파와 베타는 델타함수를 바탕으로 정의되기 때문에 계산하기 쉽지 않습니다.
		따라서 이에 대한 대안으로 보다 생략된 계산법을 통해 추정합니다.
		이렇게 추정된 알파와 베타는 추후에 최적화합니다.
		\param mean 평균
		\param var 분산

		\return classprobability::beta::betaparam_t

		\author Leesuk Kim, lktime@skku.edu
		*/
		struct betaparam_t moment_match(const double& mean, const double& var);

		/*
		\brief quantile k-sample
		\details
		Calculates p-value of hypothesis test of between ecdf and beta.
		\param alam

		\author Cited from Numerical Recipes in C, 2nd edition, p.626
		*/
		double qks(const double& alam);
		/*
		\brief Kolmogorov-Smirnov Test on two sample.
		\details This test tests two sample for learning phase.

		\author Cited from numerical recipies ASC, 3rd edithion, p.737-738
		*/
		struct ksresult_t kstest(std::vector<double>& sample1, std::vector<double>& sample2);
		/**
		\brief partial kstest
		\details
		일반적인 KS Test지만 parameter로 struct betaparam_t를 사용하며,
		주어진 구간까지만 KS test한다는 점이 다릅니다. (참고: 본래 KS test는 0부터 1까지 모두 확인합니다.)
		특히 이 함수는 0부터가 아닌 front부터, 1까지가 아닌 rear까지만 test합니다.
		이를 통해 fitting이 불가능한 불연속점에서는 새로운 beta를 찾을 수 있는 기준을 제공합니다.

		\author Leesuk Kim, lktime@skku.edu
		*/
		void partial_kstest(struct kstest_t& kst, struct betaparam_t& bp);
		/**
		\details
		beta parameters를 searching으로 찾습니다.

		\author Leesuk Kim, lktime@skku.edu
		*/
		void search_beta(struct kstest_t& kst, struct betaparam_t& bp);

		/**
		\brief
		beta 를 pivot하며 search(=fitting) 해서 mapping 합니다.

		\author Leesuk Kim, lktime@skku.edu
		*/
		struct ksresult_t search_betamap(std::map<double, struct betaparam_t>& betamap, const std::vector<double>& ecdf, struct pattern_t& ptn);
	}

	/**
	\brief class probability
	\details class probability는 각 class가 갖는 '일치할 확률'입니다. 
	이 클래스는 class probability를 계산, 검증, 그리고 이를 위한 각종 계산에 필요한 함수를 포함합니다.

	\author Leesuk kim, lktime@skku.edu
	*/
	class probaclass {
	private:
		unsigned int m_bins = 100;
		struct pattern_t m_pattern;
		struct beta::ksresult_t m_ksr;
		struct beta::betasketch_t m_betasketch;

		std::vector<struct beta::betaparam_t> bp_neg;

		//rv의 limit에 따라서 서로 다른 beta를 적용합니다.
		std::map<double, struct beta::betaparam_t> m_betamap;

		//ECDF의 scale을 보정합니다.
		double scale(double rv);

	public:
		std::string m_name;
		probaclass(std::string name);
		probaclass(std::string name, std::vector<double> data);
		inline void set_bins(unsigned int bins) { m_bins = bins; }

		struct beta::betasketch_t get_betasketch();
		

		/**
		\brief maps Beta Shape from samples.

		\author Leesuk Kim, lktime@skku.edu
		*/
		void map_beta();

		void insert(std::vector<double> data);

		void update(const std::vector<double>& data);

		/**
		\brief positive와 negative의 class probability를 계산합니다.

		\details
		class probability는 positive class probability와 negative class probability의 산술평균으로 얻습니다.
		이를 위해 이 함수는 positive class probability또는 negative class probability를 구합니다.
		\param betamap
		\param rv
		\return class output

		\author Leesuk Kim, lktime@skku.edu
		*/
		double cls_prob_signed(std::map<double, beta::betaparam_t>& betamap, const double& rv);

		/**
		\details
		이 식은 엄밀히 말해서 잘못되었습니다.
		원래는 postive beta와 negitive beta distribution을 구한 후
		각각의 class probability를 계산한 뒤 산술평균을 내야 합니다.
		하지만 kernel을 알 수 없는 상황에서 negative beta를 알 수 없으므로 정확도가 떨어지지만
		어느정도 기능을 하므로 작성하였습니다.

		\author Leesuk Kim, lktime@skku.edu
		*/
		double cls_prob(const double& rv);
	};

	typedef std::map<std::string, probaclass> cpnmap;
	typedef std::pair<std::string, probaclass> cpnmap_pair;
	typedef std::map<std::string, probaclass>::iterator cpnmap_iter;
	typedef std::map<std::string, std::vector<double>> datamap;
	typedef std::map<std::string, std::vector<double>>::iterator datamap_iter;
	typedef std::pair<std::string, std::vector<double>> datamap_pair;

	/**
	\brief cpnetwork
	\details class probability (output) network를 구성하는 class입니다.
	
	\author Leesuk Kim, lktime@skku.edu
	*/
	class cpnetwork {
	private:
		cpnmap* m_cpmap;
	public:
		cpnetwork();
		~cpnetwork();
		cpnetwork(cpnmap* cpmap);
		cpnmap* getCpmap();

		void insert(std::string key, std::vector<double> value);
		void insert(std::map<std::string, std::vector<double>>* kvmap);
		void insert(probaclass cp);

		void update(std::string key, std::vector<double> value);
		void update(std::map<std::string, std::vector<double>>* kvmap);

		/**
		\brief cpon을 구성합니다.
		\details Probaclass를 묶습니다.

		\author Leesuk Kim, lktime@skku.edu
		*/
		void build_network();
	};
}