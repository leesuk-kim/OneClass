#ifdef __cplusplus
extern "C" {
#endif
	/**
	\brief �н��� cpon�� �ʱ�ȭ�մϴ�.
	\details
	\param modelpath �н��� ���� ���Ϸ� �����մϴ�. �� �������� �νĿ� cpon�� �����ϴ� ���� ���˴ϴ�.
	�ٽ� �����ڸ�, �νĿ� cpon�� �� �� ������ �о �Ű���� �����մϴ�.
	model�� ��� ��θ� �����ϰ� �ʹٸ� �� �Լ��� ȣ���ؾ� �˴ϴ�.
	���� �״�� �����ϰ� ��θ� �ٲٰ� ���� ������ �� �Լ��� ȣ���ؾ� �մϴ�.
	�� �Լ��� ȣ���Ѵٰ� �ؼ� ������ ������ ���� ������� �ʽ��ϴ�.
	cpon�� releasei���� �ʴ� �̻� ���� �������� �ʽ��ϴ�.

	\author Leesuk Kim, lktime@skku.edu
	*/
	struct lcpnet* lcpon_initialize(const char* modelpath);
	
	/**
	\brief �н��մϴ�.
	\details �Ķ���ͷ� ���� �����͸� ���� �н��մϴ�.
	data�� double**��, row x col�Դϴ�. ��, data[0]�� ù��° row�̸�, data[0][0]�� ù��° row�� ù��° col�Դϴ�.
	row�� �� ���� ������ ��Ÿ����, col�� �� ���� Ŭ������ ��Ÿ���ϴ�.

	\author Leesuk Kim, lktime@skku.edu
	*/
	void lcpon_learn(unsigned int row, unsigned int col, double** data);
	
	/**
	\brief cpon�� �Ҵ�� �޸𸮸� �����մϴ�.
	\details cpon�� �Ҵ�� �޸𸮸� �����մϴ�.

	\author Leesuk Kim, lktime@skku.edu
	*/
	void lcpon_release();
	
	/**
	\brief cpon�� ������ �����մϴ�.
	\details �Ķ���ͷ� ���� �����ͷ� �з��� �����ϰ�, �з����� ����մϴ�.
	�з����� "�ùٸ��� �з��� ������ ���� / �� ������ ����" �Դϴ�.
	index�� �ش� ������ ���� Ŭ������ ���������� ��Ÿ���ϴ�. 

	\author Leesuk Kim, lktime@skku.edu
	*/
	double lcpon_measure(int row, int col, double** data, int* index);

#ifdef __cplusplus
}
#endif