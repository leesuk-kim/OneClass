#include "cponcommon.h"

#include <boost\math\distributions\beta.hpp>

namespace kil {
	typedef std::map<std::string, std::vector<double>> datamap;
	typedef std::map<std::string, std::vector<double>>::iterator datamap_iter;
	typedef std::pair<std::string, std::vector<double>> datamap_pair;

	/**
	\brief pattern을 저장하고 관리합니다.

	\author Leesuk Kim, lktime@skku.edu
	*/
	class pattern {
	private:
		std::vector<double> mData;

		featurescaler* mFeaturescaler;
		kernelizer* mKernelizer;

	public:
		featurescaler* getFeaturescaler(){
			return mFeaturescaler;
		};
		kernelizer* getKernelizer(){
			return mKernelizer;
		}
		double mFmean;
		double mFvar;
		std::vector<double> mFdata;
		std::vector<double> mKdata;
		std::vector<double> ecdf;
		std::vector<double> mBetamap;
		struct beta::ksresult_t m_ksr;
		/**
		- data, feature-scaled data, kernel data 저장
		- data의 min, max, mean, var 계산
		\author Leesuk Kim, lktime@skku.edu
		*/
		pattern(std::vector<double> data);
		~pattern();
	};

	namespace beta{
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
		struct ksresult_t search_betamap(std::map<double, struct betaparam_t>& betamap, const std::vector<double>& ecdf, struct beta::betaparam_t initbp);
	}

	class pclearn: public kil::probaclass {
	private:
		pattern* mPattern;
		std::map<double, struct beta::betaparam_t> mBetamap;
		struct beta::betasketch_t m_betasketch;
	public:
		struct beta::betasketch_t getBetasketch(){
			return m_betasketch;
		}
		pattern* getPattern(){
			return mPattern;
		}
		double mMean;
		double mVar;
		std::vector<double> ecdf;
		struct beta::ksresult_t m_ksr;
		pclearn(std::string name, std::vector<double> pcptn);
		~pclearn();
		/**
		\brief beta function을 만듭니다.
		\details 
		\author Leesuk Kim, lktime@skku.edu
		*/
		std::vector<double> mapBeta();
		double output(double& randomvariable);
	};


	typedef std::map<std::string, pclearn*> cplmap;
	typedef std::pair<std::string, pclearn*> cplmap_pair;
	typedef std::map<std::string, pclearn*>::iterator cplmap_iter;
	

	/**
	\brief class probability output network
	\details
	실질적인 CPON입니다. Probaclass의 network를 구성하고, 관리합니다.
	다만 이 cpon은 learning만 수행합니다.
	\author Leesuk Kim, lktime@skku.edu
	*/
	class lcpnet {
	private:
		cplmap* mCPLmap;
		void insert(std::string key, std::vector<double> values);
		static char* path_load;
		static char* path_export;
		
		/**
		\brief 
		\details NN의 출력물(output.csv)을 읽어서 CPON을 build할 수 있는 데이터 구조로 가져옵니다.
		\author Leesuk Kim, lktime@skku.edu
		*/
		lcpnet();

		static lcpnet* m_instance;
	public:
		cplmap* getcplmap(){//TODO 삭제 요망
			return mCPLmap;
		};

		/**
		\brief lcpnet의 instance를 가져옵니다.
		\details
		lcpnet은 하나의 instance만 생성되어야 합니다. 따라서 인스턴스가 필요할 때는 언제든지 가져 올 수 있도록, 그리고 중복된 instance가 생성되지 않도록 합니다.
		*/
		inline static lcpnet* getInstance(){
			if(lcpnet::m_instance == NULL) lcpnet::m_instance = new lcpnet();
			return lcpnet::m_instance;
		}

		~lcpnet();
		/**
		\brief 주어진 datamap으로 cpn을 생성합니다.
		\details
		이 함수는 cpn(Class Probability Network)를 생성하고, 학습에 필요한 데이터를 생성합니다.
		학습에 필요한 데이터를 생성하기 위해 다음의 연산을 수행합니다.
		- pattern statistics 계산
		- lcgk 계산
		- lcgk statistics 계산
		*/
		void buildnetwork();
		
		/**
		\brief 주어진 pattern으로 학습합니다.
		\details
		learning을 수행하고 결과를 출력합니다.
		learning은 다음의 작업 내용들을 포함합니다.
		- beta approximation
		- beta fitting
		출력 포멧은 datamap*입니다.
		*/
		datamap* learn();

		void exportcpnet(const char* path);

		/**
		\brief 모든 작업을 한 번에 수행하는 함수입니다.
		\details lcpnet(Learning ClassProbibility output NETwork)은 다음의 순서로 함수들이 호출되어야 합니다.
		1. constructor
		2. buildnetwork
		3. exportdata
		이 함수는 상기된 함수들을 default로 설정된 값대로 처리합니다.
		*/
		static void kil::lcpnet::factory(){
			lcpnet cpon;
			cpon.buildnetwork();
			cpon.exportcpnet("cpon/learning_result.csv");
		};
	};

}