#ifdef __cplusplus
extern "C" {
#endif
	/**
	\brief 학습용 cpon을 초기화합니다.
	\details
	\param modelpath 학습된 모델을 파일로 생성합니다. 이 모델파일은 인식용 cpon을 생성하는 데에 사용됩니다.
	다시 말하자면, 인식용 cpon은 이 모델 파일을 읽어서 신경망을 구축합니다.

	\author Leesuk Kim, lktime@skku.edu
	*/
	struct tcpnet* tcpon_initialize(char* modelpath);
	
	/**
	\brief 인식합니다.
	\details 파라미터로 받은 데이터를 토대로 인식합니다.

	\author Leesuk Kim, lktime@skku.edu
	*/
	void tcpon_test(double* result, double* fwoutput);
	
	/**
	\brief cpon에 할당된 메모리를 해제합니다.
	\details cpon에 할당된 메모리를 해제합니다.

	\author Leesuk Kim, lktime@skku.edu
	*/
	void tcpon_release();

#ifdef __cplusplus
}
#endif