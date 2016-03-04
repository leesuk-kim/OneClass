#ifdef __cplusplus
extern "C" {
#endif
	/**
	\brief 학습용 cpon을 초기화합니다.
	\details
	\param modelpath 학습된 모델을 파일로 생성합니다. 이 모델파일은 인식용 cpon을 생성하는 데에 사용됩니다.
	다시 말하자면, 인식용 cpon은 이 모델 파일을 읽어서 신경망을 구축합니다.
	model의 출력 경로를 수정하고 싶다면 이 함수를 호출해야 됩니다.
	모델은 그대로 유지하고 경로만 바꾸고 싶을 때에도 이 함수를 호출해야 합니다.
	이 함수를 호출한다고 해서 기존에 생성한 모델이 사라지진 않습니다.
	cpon은 releasei되지 않는 이상 모델이 삭제되지 않습니다.

	\author Leesuk Kim, lktime@skku.edu
	*/
	struct lcpnet* lcpon_initialize(const char* modelpath);
	
	/**
	\brief 학습합니다.
	\details 파라미터로 받은 데이터를 토대로 학습합니다.
	data는 double**로, row x col입니다. 즉, data[0]는 첫번째 row이며, data[0][0]은 첫번째 row의 첫번째 col입니다.
	row는 한 개의 패턴을 나타내며, col은 한 개의 클래스를 나타냅니다.

	\author Leesuk Kim, lktime@skku.edu
	*/
	void lcpon_learn(unsigned int row, unsigned int col, double** data);
	
	/**
	\brief cpon에 할당된 메모리를 해제합니다.
	\details cpon에 할당된 메모리를 해제합니다.

	\author Leesuk Kim, lktime@skku.edu
	*/
	void lcpon_release();
	
	/**
	\brief cpon의 성능을 측정합니다.
	\details 파라미터로 받은 데이터로 분류를 수행하고, 분류율을 계산합니다.
	분류율은 "올바르게 분류된 패턴의 개수 / 총 패턴의 개수" 입니다.
	index는 해당 패턴의 정답 클래스가 무엇인지를 나타냅니다. 

	\author Leesuk Kim, lktime@skku.edu
	*/
	double lcpon_measure(int row, int col, double** data, int* index);

#ifdef __cplusplus
}
#endif