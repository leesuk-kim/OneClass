#include "cponcommon.h"

namespace kil{
	class pctest: public kil::probaclass{
	private:
		kernelizer* mKernelizer;
		featurescaler* mFeaturescaler;
		std::vector<double> mBetagauge;
	public:
		kernelizer* getKernelizer(){
			return mKernelizer;
		};

		featurescaler* getFeaturescaler(){
			return mFeaturescaler;
		};

		pctest();
		pctest(int index, double fmin, double fmax, double kmean, double kvar, std::vector<double> betagauge);
		~pctest();
		
		/**
		pattern�� kerneloutput�� ��ģ �� beta output�� ���մϴ�.
		*/
		double output(double& randomvariable);
	};

	typedef std::map<int, pctest*> cptmap;
	typedef std::pair<int, pctest*> cptmap_pair;
	typedef std::map<int, pctest*>::iterator cptmap_iter;
	
	/**\brief string-double map*/
	typedef std::map<int, double> sdmap;
	typedef std::map<int, double>::iterator sdmap_iter;
	typedef std::pair<int, double> sdmap_pair;

	/**
	\brief class probability output network
	\details
	�������� CPON�Դϴ�. Probaclass�� network�� �����ϰ�, �����մϴ�.
	�ٸ� �� cpon�� test�� �����մϴ�.
	\author Leesuk Kim, lktime@skku.edu
	*/
	class tcpnet {
	private:
		cptmap* mCPTmap;
		static char* mModelPath;
		static tcpnet* m_instance;
	public:
		tcpnet();
		~tcpnet();
		void insert(pctest* pct);
		
		inline static void setModelPath(char* modelpath){mModelPath = modelpath;};
		inline static char* getModelPath(){return mModelPath;};

		inline unsigned int getCPTmapSize(){return mCPTmap->size();};

		inline static tcpnet* getInstance(){
			if(m_instance == NULL) tcpnet::m_instance = new tcpnet();
			return tcpnet::m_instance;
		};
		/**
		\brief test�մϴ�.
		\details forward network�� output���� test�մϴ�. 
		forward network output�� i��° output�� cpon�� i��° class�� �����Ǿ�� �մϴ�.
		������ fowrard network�� unordered�̱� ������ cpon�� ���������� ������ map�� key�� ���缭 map�� �����մϴ�.
		
		\author Leesuk Kim, lktime@skku.edu
		*/
		void test(double* res, double* fwoutput);
		/**
		\brief learning result�� �н��ϴ�.
		\details
		lcpnet�� ���� �н��� ����� ��� �ִ� learning result�� �н��ϴ�.
		learning result�� lcpnet�� \see{exportResult}�� ���� ��µ˴ϴ�.

		\param path ����Դϴ�. (const char*)
		\param tcpon test cpon�Դϴ�.
		*/
		void importModel(const char* modelpath);
	};

	/**
	\brief ���� ū index�� ã���ϴ�.
	\details ũ�Ⱑ ���� index�� double �迭�� �迭 �ε����� ���߼� ���ϰ�, double �迭���� ���� ū index���� ��ȯ�մϴ�.
	*/
	inline unsigned int getMaxID(unsigned int size, int* index, double* values){
		unsigned int m = 0;
		for(unsigned int i = 0 ; i < size ; i++)
			if(values[m] < values[i]) m = i;
		return index[m];
	};
}