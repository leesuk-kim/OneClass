#include "clcpon.h"
#include "cponlearn.h"

extern "C"{
	namespace kil{
		lcpnet* lcpon_initialize(const char* modelpath){
			lcpnet::getInstance()->setModelPath(modelpath);
			return lcpnet::getInstance();
		}

		void lcpon_learn(unsigned int row, unsigned int col, double** data){
			datamap* dm = new datamap();
			lcpnet::getInstance()->fit(row, col, data);
			lcpnet::getInstance()->learn();
		}

		void lcpon_release(){
			delete lcpnet::getInstance();
		}

		double lcpon_measure(kil::lcpnet* cpon, int row, int col, double** data){
			double measure = 0.;
			//lcpnet::getInstance()->measure(row, col, data);
			return measure;
		}
	}
}