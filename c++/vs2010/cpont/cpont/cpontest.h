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
		pattern의 kerneloutput을 거친 후 beta output을 구합니다.
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
	실질적인 CPON입니다. Probaclass의 network를 구성하고, 관리합니다.
	다만 이 cpon은 test만 수행합니다.
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
		\brief test합니다.
		\details forward network의 output으로 test합니다. 
		forward network output의 i번째 output은 cpon의 i번째 class에 배정되어야 합니다.
		하지만 fowrard network는 unordered이기 때문에 cpon이 내부적으로 형성한 map의 key에 맞춰서 map을 형성합니다.
		
		\author Leesuk Kim, lktime@skku.edu
		*/
		void test(double* res, double* fwoutput);
		/**
		\brief learning result를 읽습니다.
		\details
		lcpnet을 통해 학습된 결과를 담고 있는 learning result를 읽습니다.
		learning result는 lcpnet의 \see{exportResult}를 통해 출력됩니다.

		\param path 경로입니다. (const char*)
		\param tcpon test cpon입니다.
		*/
		void importModel(const char* modelpath);
	};
}