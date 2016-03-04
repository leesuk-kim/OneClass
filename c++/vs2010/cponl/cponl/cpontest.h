#pragma once
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
	
	/**\brief class output map*/
	typedef std::map<std::string, double> comap;
	typedef std::map<std::string, double>::iterator comap_iter;
	typedef std::pair<std::string, double> comap_pair;

	///**
	//\brief class probability output network
	//\details
	//실질적인 CPON입니다. Probaclass의 network를 구성하고, 관리합니다.
	//다만 이 cpon은 test만 수행합니다.
	//\author Leesuk Kim, lktime@skku.edu
	//*/
	class tcpnet {
	private:
		cptmap* mCPTmap;
		const char* path_import;
	public:
		tcpnet();
		~tcpnet();
		void insert(pctest* pct);
		/**
		class output을 구합니다.
		받았던 vector 그대로 반환합니다.
		*/
		void test(double* res, double tdata);
		/**
		\brief learning result를 읽습니다.
		\details
		lcpnet을 통해 학습된 결과를 담고 있는 learning result를 읽습니다.
		learning result는 lcpnet의 \see{exportResult}를 통해 출력됩니다.

		\param path 경로입니다. (const char*)
		\param tcpon test cpon입니다.
		*/
		void importdata(const char* path);
		static void factory(){};
	};
}