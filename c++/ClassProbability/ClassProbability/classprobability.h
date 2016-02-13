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
\namespace classprobability
\brief namespace classprobability
*/
namespace classprobability {
	/**
	\brief struct pattern
	\details pattern 관련 정보를 저장합니다.
	\author Leesuk kim, lktime\skku.edu
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
	\brief namespace beta
	*/
	namespace beta {
		/**
		\brief struct for contatining parameters of beta function.
		\details Boost Library의 beta 함수는 random variable과 paramter를 동시에 입력해야 beta output이 출력됩니다.
		random variable은 사용할 때마다 달라지니, 결국 특정 class의 shape of beta function을 결정하는 것은 beta parameter입니다.
		따라서 beta parameter인 alpha와 beta값을 하나의 struct로 묶어서 관리합니다.
		*/
		struct betaparam_t {
			/**the beta parameter 'alpha'*/
			double alpha;
			/**the beta parameter 'beta'*/
			double beta;
		};

		struct ksresult_t {
			/**Kolmogorov-Smirnov statistic*/
			double d;
			/**p-value*/
			double pval;
		};

		//kstest할 때 기본적으로 필요한 data들
		struct kstest_t {
			const std::vector<double>& ecdf;
			const unsigned int size;
			const double en_sqrt;
			unsigned int front, rear;
			struct ksresult_t result;
		};

		struct betasketch_t{
			std::vector<double> ecdf, beta, xaxis;
			struct ksresult_t ksr;
		};

		std::vector<int> histogram(const std::vector<double>& data, unsigned int bins);

		std::vector<double> cumulahisto(std::vector<int>& hist_ui, unsigned int bins);
		inline std::vector<double> cumulahisto(std::vector<double>& data, unsigned int bins) { return cumulahisto(histogram(data, bins), bins); }

		struct betaparam_t moment_match(const double& mean, const double& var);

		double qks(const double& alam);
		struct ksresult_t kstest(std::vector<double>& sample1, std::vector<double>& sample2);
		void partial_kstest(struct kstest_t& kst, struct betaparam_t& bp);

		void search_beta(struct kstest_t& kst, struct betaparam_t& bp);
		struct ksresult_t search_betamap(std::map<double, struct betaparam_t>& betamap, const std::vector<double>& ecdf, struct pattern_t& ptn);
	}

	class Probaclass {
	private:
		unsigned int m_bins = 100;
		struct pattern_t m_pattern;
		struct beta::betaparam_t m_bp;
		struct beta::ksresult_t m_ksr;
		struct beta::betasketch_t m_betasketch;

		std::vector<struct beta::betaparam_t> bp_neg;

		//rv의 limit에 따라서 서로 다른 beta를 적용합니다.
		std::map<double, struct beta::betaparam_t> m_betamap;

		//rv를 담당하는 beta를 찾아서 그 output을 리턴합니다.
		double find_betarv(double rv);
		//ECDF의 scale을 보정합니다.
		double scale(double rv);

	public:
		std::string m_name;
		Probaclass(std::string name);
		Probaclass(std::string name, std::vector<double> data);

		inline void set_bins(unsigned int bins) { m_bins = bins; }
		inline const beta::betaparam_t& get_bp() { return m_bp; }

		struct beta::betasketch_t get_betasketch();
		
		/*CPON에 입력해야하는 data를 넣으면 beta를 추정합니다.
		*/
		void map_beta();

		void insert(std::vector<double> data);

		void update(const std::vector<double>& data);
		
		double cls_prob_signed(std::map<double, beta::betaparam_t>& betamap, const double& rv);
		double cls_prob(const double& rv);

		struct beta::betasketch_t ecdf_inv();
	};

	typedef std::map<std::string, Probaclass> cpon_map;
	typedef std::pair<std::string, Probaclass> cpon_map_pair;
	typedef std::map<std::string, Probaclass>::iterator cpon_map_iter;
	typedef std::map<std::string, std::vector<double>> datamap;
	typedef std::map<std::string, std::vector<double>>::iterator datamap_iter;
	typedef std::pair<std::string, std::vector<double>> data_map_pair;

	/////////////////////////////////
	/*주의! 이 클래스는 예시입니다.*/
	/////////////////////////////////
	class CPON {
	private:
		cpon_map* m_cpmap;
	public:
		CPON();
		~CPON();
		CPON(cpon_map* cpmap);
		cpon_map* getCpmap();

		void insert(std::string key, std::vector<double> value);
		void insert(std::map<std::string, std::vector<double>>* kvmap);
		void insert(Probaclass cp);
		void insert(cpon_map_pair pair);

		void update(std::string key, std::vector<double> value);
		void update(std::map<std::string, std::vector<double>>* kvmap);

		void build_network();
	};
}