#include "ctcpon.h"
#include "cpontest.h"

extern "C"{
	namespace kil{
		tcpnet* tcpon_initialize(char* modelpath){
			tcpnet::getInstance()->setModelPath(modelpath);
			tcpnet::getInstance()->importModel(tcpnet::getInstance()->getModelPath());
			return tcpnet::getInstance();
		}

		void tcpon_test(double* result, double* fwoutput){
			tcpnet::getInstance()->test(result, fwoutput);
		}

		void tcpon_release(){
			delete tcpnet::getInstance();
		}
	}
}