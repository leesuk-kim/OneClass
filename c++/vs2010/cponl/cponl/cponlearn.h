#include "cponcommon.h"

#include <boost\math\distributions\beta.hpp>

namespace kil {
	typedef std::map<std::string, std::vector<double>> datamap;
	typedef std::map<std::string, std::vector<double>>::iterator datamap_iter;
	typedef std::pair<std::string, std::vector<double>> datamap_pair;

	/**
	\brief pattern�� �����ϰ� �����մϴ�.

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
		- data, feature-scaled data, kernel data ����
		- data�� min, max, mean, var ���
		\author Leesuk Kim, lktime@skku.edu
		*/
		pattern(std::vector<double> data);
		~pattern();
	};

	namespace beta{
		/**
		\brief histrogram�� �׸��ϴ�.

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
		\brief beta function�� ���Ŀ� ��Ÿ�� �ʱⰪ�� �����մϴ�.

		\details
		�⺻������, beta function�� ���Ŀ� ��Ÿ�� ���� shape�� ���ǵ˴ϴ�.
		������ ���Ŀ� ��Ÿ�� ��Ÿ�Լ��� �������� ���ǵǱ� ������ ����ϱ� ���� �ʽ��ϴ�.
		���� �̿� ���� ������� ���� ������ ������ ���� �����մϴ�.
		�̷��� ������ ���Ŀ� ��Ÿ�� ���Ŀ� ����ȭ�մϴ�.
		\param mean ���
		\param var �л�

		\return classprobability::beta::betaparam_t

		\author Leesuk Kim, lktime@skku.edu
		*/
		struct betaparam_t moment_match(const double& mean, const double& var);
		/**
		\brief partial kstest
		\details
		�Ϲ����� KS Test���� parameter�� struct betaparam_t�� ����ϸ�,
		�־��� ���������� KS test�Ѵٴ� ���� �ٸ��ϴ�. (����: ���� KS test�� 0���� 1���� ��� Ȯ���մϴ�.)
		Ư�� �� �Լ��� 0���Ͱ� �ƴ� front����, 1������ �ƴ� rear������ test�մϴ�.
		�̸� ���� fitting�� �Ұ����� �ҿ����������� ���ο� beta�� ã�� �� �ִ� ������ �����մϴ�.

		\author Leesuk Kim, lktime@skku.edu
		*/
		void partial_kstest(struct kstest_t& kst, struct betaparam_t& bp);
		/**
		\details
		beta parameters�� searching���� ã���ϴ�.

		\author Leesuk Kim, lktime@skku.edu
		*/
		void search_beta(struct kstest_t& kst, struct betaparam_t& bp);

		/**
		\brief
		beta �� pivot�ϸ� search(=fitting) �ؼ� mapping �մϴ�.

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
		\brief beta function�� ����ϴ�.
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
	�������� CPON�Դϴ�. Probaclass�� network�� �����ϰ�, �����մϴ�.
	�ٸ� �� cpon�� learning�� �����մϴ�.
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
		\details NN�� ��¹�(output.csv)�� �о CPON�� build�� �� �ִ� ������ ������ �����ɴϴ�.
		\author Leesuk Kim, lktime@skku.edu
		*/
		lcpnet();

		static lcpnet* m_instance;
	public:
		cplmap* getcplmap(){//TODO ���� ���
			return mCPLmap;
		};

		/**
		\brief lcpnet�� instance�� �����ɴϴ�.
		\details
		lcpnet�� �ϳ��� instance�� �����Ǿ�� �մϴ�. ���� �ν��Ͻ��� �ʿ��� ���� �������� ���� �� �� �ֵ���, �׸��� �ߺ��� instance�� �������� �ʵ��� �մϴ�.
		*/
		inline static lcpnet* getInstance(){
			if(lcpnet::m_instance == NULL) lcpnet::m_instance = new lcpnet();
			return lcpnet::m_instance;
		}

		~lcpnet();
		/**
		\brief �־��� datamap���� cpn�� �����մϴ�.
		\details
		�� �Լ��� cpn(Class Probability Network)�� �����ϰ�, �н��� �ʿ��� �����͸� �����մϴ�.
		�н��� �ʿ��� �����͸� �����ϱ� ���� ������ ������ �����մϴ�.
		- pattern statistics ���
		- lcgk ���
		- lcgk statistics ���
		*/
		void buildnetwork();
		
		/**
		\brief �־��� pattern���� �н��մϴ�.
		\details
		learning�� �����ϰ� ����� ����մϴ�.
		learning�� ������ �۾� ������� �����մϴ�.
		- beta approximation
		- beta fitting
		��� ������ datamap*�Դϴ�.
		*/
		datamap* learn();

		void exportcpnet(const char* path);

		/**
		\brief ��� �۾��� �� ���� �����ϴ� �Լ��Դϴ�.
		\details lcpnet(Learning ClassProbibility output NETwork)�� ������ ������ �Լ����� ȣ��Ǿ�� �մϴ�.
		1. constructor
		2. buildnetwork
		3. exportdata
		�� �Լ��� ���� �Լ����� default�� ������ ����� ó���մϴ�.
		*/
		static void kil::lcpnet::factory(){
			lcpnet cpon;
			cpon.buildnetwork();
			cpon.exportcpnet("cpon/learning_result.csv");
		};
	};

}