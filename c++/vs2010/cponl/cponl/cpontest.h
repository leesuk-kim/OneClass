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
		pattern�� kerneloutput�� ��ģ �� beta output�� ���մϴ�.
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
	//�������� CPON�Դϴ�. Probaclass�� network�� �����ϰ�, �����մϴ�.
	//�ٸ� �� cpon�� test�� �����մϴ�.
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
		class output�� ���մϴ�.
		�޾Ҵ� vector �״�� ��ȯ�մϴ�.
		*/
		void test(double* res, double tdata);
		/**
		\brief learning result�� �н��ϴ�.
		\details
		lcpnet�� ���� �н��� ����� ��� �ִ� learning result�� �н��ϴ�.
		learning result�� lcpnet�� \see{exportResult}�� ���� ��µ˴ϴ�.

		\param path ����Դϴ�. (const char*)
		\param tcpon test cpon�Դϴ�.
		*/
		void importdata(const char* path);
		static void factory(){};
	};
}