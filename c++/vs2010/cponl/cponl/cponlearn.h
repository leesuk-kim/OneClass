#include "cponcommon.h"
#include "cpontest.h"

namespace kil {

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
		//std::vector<double> ecdf;
		//std::vector<double> mBetamap;
		//struct beta::ksresult_t m_ksr;
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
		pclearn(int index, std::vector<double> pcptn);
		~pclearn();
		/**
		\brief beta function�� ����ϴ�.
		\details 
		\author Leesuk Kim, lktime@skku.edu
		*/
		std::vector<double> mapBeta();
		double output(double& randomvariable);
	};


	typedef std::map<int, pclearn*> cplmap;
	typedef std::pair<int, pclearn*> cplmap_pair;
	typedef std::map<int, pclearn*>::iterator cplmap_iter;
	

	/**
	\brief class probability output network
	\details
	�������� CPON�Դϴ�. Probaclass�� network�� �����ϰ�, �����մϴ�.
	�ٸ� �� cpon�� learning�� �����մϴ�.
	\author Leesuk Kim, lktime@skku.edu
	*/
	class lcpnet {
	private:
		static cplmap* mCPLmap;
		void static insert(int key, std::vector<double> values);
		static std::string mModelPath;
		static tcpnet* mTCPnet;
		/**
		\brief 
		\details NN�� ��¹�(output.csv)�� �о CPON�� build�� �� �ִ� ������ ������ �����ɴϴ�.
		\author Leesuk Kim, lktime@skku.edu
		*/
		lcpnet();

		static lcpnet* m_instance;
	public:
		inline cplmap* getCPLmap(){return mCPLmap;};
		/**
		\brief lcpnet�� instance�� �����ɴϴ�.
		\details
		lcpnet�� �ϳ��� instance�� �����Ǿ�� �մϴ�. ���� �ν��Ͻ��� �ʿ��� ���� �������� ���� �� �� �ֵ���, �׸��� �ߺ��� instance�� �������� �ʵ��� �մϴ�.
		*/
		inline static lcpnet* getInstance(){
			if(lcpnet::m_instance == NULL) lcpnet::m_instance = new lcpnet();
			return lcpnet::m_instance;
		}

		/**
		\brief CPON ���� ����� ��θ� �����մϴ�.
		*/
		inline static void setModelPath(const char* modelpath){mModelPath = modelpath;};

		/**
		\brief ������ ����մϴ�.
		\details
		���÷� ���� forward netowkr�� ����� �������� �����Ͽ����ϴ�.
		���� = �ùٸ��� �ν��� ���� ����/��ü ���� ����

		\author Leesuk Kim, lktime@skku.edu
		*/
		double measure(unsigned int row, unsigned int col, double** testdata, int* index);
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
		void learn();
		
		/**
		\brief �־��� pattern���� �н��մϴ�.
		\details
		learning�� �����ϰ� ����� ����մϴ�.
		learning�� ������ �۾� ������� �����մϴ�.
		- beta approximation
		- beta fitting
		��� ������ datamap*�Դϴ�.
		*/
		//void static fit(datamap* dm);
		void fit(unsigned int row, unsigned int col, double** data);

		/**
		\brief tcpnet�� ������ �� �ִ� cpon model�� ����մϴ�.
		*/
		void exportModel(std::string path);
	};

}

