#include "clcpon.h"

#ifdef __cplusplus
extern "C"{
#endif
	void lcpon_initialize(struct lcpnet* cpon){
	}
	void lcpon_learn(int row, int col, double** data, int* index){
	}
	void lcpon_test(int row, int col, double** testdata){
	}
	void lcpon_release(struct lcpnet* cpon){
	}
	double lcpon_measure(int row, int col, double** data, int* index){
		double measure = 0.;

		return measure;
	}

#ifdef __cplusplus
}
#endif