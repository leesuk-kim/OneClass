#include "clcpon.h"
#include "cponlearn.h"

extern "C"{
	namespace kil{
		lcpnet* lcpon_initialize(const char* modelpath){
			lcpnet::getInstance()->setModelPath(modelpath);
			return lcpnet::getInstance();
		}

		void lcpon_learn(unsigned int row, unsigned int col, double** data){
			lcpnet::getInstance()->fit(row, col, data);
			lcpnet::getInstance()->learn();
		}

		void lcpon_release(){
			delete lcpnet::getInstance();
		}

		double lcpon_measure(int row, int col, double** data, int* index){
			double measure = 0.;
			lcpnet::getInstance()->measure(row, col, data, index);
			return measure;
		}
	}
}