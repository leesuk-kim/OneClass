#include "cponcommon.h"
#include <fstream>
#include <sstream>

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
		pctest(std::string name, double fmin, double fmax, double kmean, double kvar, std::vector<double> betagauge);
		~pctest();
		
		/**
		pattern�� kerneloutput�� ��ģ �� beta output�� ���մϴ�.
		*/
		double output(double& randomvariable);
	};

	typedef std::map<std::string, pctest*> cptmap;
	typedef std::pair<std::string, pctest*> cptmap_pair;
	typedef std::map<std::string, pctest*>::iterator cptmap_iter;
	
	/**\brief string-double map*/
	typedef std::map<std::string, double> sdmap;
	typedef std::map<std::string, double>::iterator sdmap_iter;
	typedef std::pair<std::string, double> sdmap_pair;

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
		char* mModelPath;
		static tcpnet* m_instance;
	public:
		tcpnet();
		~tcpnet();
		void insert(pctest* pct);
		
		inline void setModelPath(char* modelpath){mModelPath = modelpath;};
		inline char* getModelPath(){return mModelPath;};

		inline unsigned int getCPTmapSize(){return mCPTmap->size();};

		static tcpnet* getInstance(){
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
}