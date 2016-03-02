#pragma once
#pragma warning(disable:4996)
#include <iostream>
#include <vector>
#include <map>
#include <numeric>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>


namespace std{
	/**
	\brief delete�� �Լ����Դϴ�.
	\details
	�� �Լ��� �ܼ��� typename T�� delete�մϴ�. ������ delete�� �����⶧���� function pointer�� �Ѱ��� �� �����ϴ�.
	���� function pointer�� �Ѱ��ֱ� ���� templete function�� �߰������� ��������ϴ�.
	��� �� ����� ���� template�� �𸥴ٰ� ģ���� ����� �� �� ����� �ý��ϴ�...
	*/
	template <typename T> void del(typename T t){
		delete t;
	}

	/**
	\brief std::map�� ���� �ݺ����� �۾��� ó���մϴ�.
	\details
	typename T�� ���ǵ� std::map�� �ִٸ�, std::map�� ���� ������ element�� ���� �۾��� ó���� �� ����մϴ�.
	��� �� ����� ���� template�� �𸥴ٰ� ģ���� ����� �� �� ����� �ý��ϴ�...
	*/
	template <typename T1, typename T2> void map_fptr_iter(std::map<typename T1, typename T2>* mappist, void(*fptr)(typename T2 t2)){
		typename std::map<T1, T2>::iterator iter;
		for(iter = mappist->begin(); iter!= mappist->end(); iter++){
			(*fptr)(iter->second);
		}
	}

	/**
	\brief double to std::string
	\details double �� ������ std::string���� ��ȯ�մϴ�.
	\author Leesuk kim, lktime@skku.edu
	*/
	std::string dtos(double d);
}

namespace kil{
	/**
	\brief double type�� std::vector�� mean�� ����մϴ�.
	\details
	����� ����մϴ�. �����Դϴ�!(�̱� ����)

	\author Leesuk Kim, lktime@skku.edu
	*/
	double mean(std::vector<double> doublevec);

	/**
	\brief double type�� std:;vector�� variance�� ����մϴ�.
	\details
	kil::mean�� ����Ǿ�� �մϴ�. parameter�� mean�� �ޱ� �����Դϴ�.
	\param doublevec ���� double type�� std::vector�Դϴ�.
	\param mean doublevec�� ����Դϴ�.

	\author Leesuk Kim, lktime@skku.edu
	*/
	double var(std::vector<double> doublevec, double mean);
	
	/**
	\brief normalize�մϴ�.
	\details
	normalization �߿����� feature-scaling�� ����Ͽ� normalize�մϴ�.
	feature-scaling�� ���� min�� max�� �ʿ�� �մϴ�.

	\author Leesuk Kim, lktime@skku.edu
	*/
	class featurescaler{
	private:
		double mMin;
		double mMax;
		double mDiffNx;
	public:
		/**
		\brief private field�� mMin�� getter�Դϴ�.
		\details GoF�� objective oriented programming design concept�� ���� getter�Դϴ�.

		\author Leesuk Kim, lktime@skku.edu
		*/
		double getMin(){
			return mMin;
		};
		/**
		\brief private field�� mMax�� getter�Դϴ�.
		\details GoF�� objective oriented programming design concept�� ���� getter�Դϴ�.

		\author Leesuk Kim, lktime@skku.edu
		*/
		double getMax(){
			return mMax;
		};
		featurescaler(double min, double max);
		/**featurescaler�� ������ִ� factory �Լ��Դϴ�.*/
		static featurescaler* factory(std::vector<double> doublevec);
		/**
		\brief normalize�մϴ�.
		\details
		���⼭�� feature-scaling���� normalization�մϴ�.
		https://en.wikipedia.org/wiki/Feature_scaling �� �������ֽñ� �ٶ��ϴ�.

		\author Leesuk Kim, lktime@skku.edu
		*/
		double output(const double& randomvariable);
	};

	/**
	\brief kernelize�մϴ�.
	\details
	kernelize�մϴ�. �̸� ���� mean�� var�� �������� parameter�� �޽��ϴ�.
	*/
	class kernelizer{
	private:
		double mMean;
		double mVar;
	public:
		/**
		\brief private field�� mean�� getter�Դϴ�.
		\details GoF�� objective oriented programming design concept�� ���� getter�Դϴ�.

		\author Leesuk Kim, lktime@skku.edu
		*/
		double getMean(){
			return mMean;
		};

		/**
		\brief private field�� var�� getter�Դϴ�.
		\details  ���⼭ var�� variance(�л�)�� �ǹ��մϴ�.
		GoF�� objective oriented programming design concept�� ���� getter�Դϴ�.
		
		\author Leesuk Kim, lktime@skku.edu
		*/
		double getVar(){
			return mVar;
		};
		
		/**
		\brief kernelizer�� �������Դϴ�.
		\details
		�� ����� ���� �ʽ��ϴ�. ���� argument�� ���� mean�� var�� �����մϴ�.

		\author Leesuk kim, lktime@skku.edu
		*/
		kernelizer(double mean, double var);

		/**
		\brief �������� �ǹ��� kernel function�Դϴ�.
		\details
		https://en.wikipedia.org/wiki/Kernel_(statistics)#In_non-parametric_statistics �� ��޵� kernel function�� �ǹ��մϴ�. 
		���⼭�� Gaussian kernel function�� �����Ͽ����ϴ�.

		\author Leesuk kim, lktime@skku.edu
		*/
		double _kernel(double& randomvariable);

		/**
		\brief kernelize�մϴ�.
		\details
		kernel�� ���� �⺻���� ��ǥ�� random variable�� ������ Ư�� ���� ������ �����ϴ� ���Դϴ�. 
		�̸� ���� Ư�� ������ �߾ӿ� �������� kernel�� ��°��� ��������, �ּ��� �������ϴ�. 
		�� �Լ��� gaussian distribution�� ������ gaussian kernel functionó�� �۵��մϴ�.		
		random variable�� � class�� kernel�� input�Ǵ��Ŀ� ���� ����� ��� �ٸ��ϴ�.
		������ class�� ���� kernel�� ������ �ٸ��� �����Դϴ�.
		�ڼ��� ������ https://en.wikipedia.org/wiki/Kernel_(statistics) �� �����Ͻʽÿ�.
		
		\author Leesuk Kim, lktime@skku.edu
		*/
		double output(double& randomvariable);
	};

	/**
	\namespace beta
	\brief beta function�� ���õ� �Լ� �Ǵ� ��ü�� �ֽ��ϴ�.
	Ư��, CPON�� model�� beta function�� ���� �����Ǳ� ������ �ſ� �߿��մϴ�.

	\author Leesuk Kim, lktime@skku.wdu
	*/
	namespace beta {
		/**
		\brief struct for contatining parameters of beta function.
		\details Boost Library�� beta �Լ��� random variable�� paramter�� ���ÿ� �Է��ؾ� beta output�� ��µ˴ϴ�.
		random variable�� ����� ������ �޶�����, �ᱹ Ư�� class�� shape of beta function�� �����ϴ� ���� beta parameter�Դϴ�.
		���� beta parameter�� alpha�� beta���� �ϳ��� struct�� ��� �����մϴ�.

		\author Leesuk kim, lktime@skku.edu
		*/
		struct betaparam_t {
			double alpha;
			double beta;
		};

		/**
		\brief kstest ����� �����մϴ�.

		\author Leesuk kim, lktime@skku.edu
		*/
		struct ksresult_t {
			/**Kolmogorov-Smirnov statistic*/
			double d;
			/**p-value*/
			double pval;
		};

		/**
		\brief kstest�� �� �⺻������ �ʿ��� data���� �����մϴ�.

		\author Leesuk kim, lktime@skku.edu
		*/
		struct kstest_t {
			const std::vector<double>& ecdf;
			const unsigned int size;
			const double en_sqrt;
			unsigned int front, rear;
			struct ksresult_t result;
		};

		/**
		\brief fragmentated beta shape�� �׸��ϴ�.
		\details 
		fragmentated beta shape�� random varialble�� ũ�⿡ ���� �ش��ϴ� beta function�� ������ �ִ� struct�Դϴ�.
		�̰��� beta fitting algorithm���� fitting�� �� ������ �ش����� ���̽��� fitting�� �� �ֵ���
		���� ���� beta function�� ������ ������, ������ beta function�� cover�ϴ� random variable�� �����ϴ� xaxis�� �ֽ��ϴ�.

		\author Leesuk kim, lktime@skku.edu
		*/
		struct betasketch_t{
			std::vector<double> ecdf, beta, xaxis;
			struct ksresult_t ksr;
		};
		/*
		\brief quantile k-sample
		\details
		Calculates p-value of hypothesis test of between ecdf and beta.
		\param alam

		\author Cited from Numerical Recipes in C, 2nd edition, p.626
		*/
		double qks(const double& alam);
		/*
		\brief Kolmogorov-Smirnov Test on two sample.
		\details This test tests two sample for learning phase.

		\author Cited from numerical recipies ASC, 3rd edithion, p.737-738
		*/
		struct ksresult_t kstest(std::vector<double>& sample1, std::vector<double>& sample2);
	}

	/**
	\brief CPON�� super class�Դϴ�.
	\details �н���/�з��� cpon�� ������ �ʿ��� ����� �߰������� �����ϴ�. �� Ŭ������ �������� ����� ������ �ִ� class�Դϴ�. 

	Leesuk kim, lktime@skku.edu
	*/
	class probaclass {
	protected:
		std::string mName;
	public:
		/**
		\brief Constructor�Դϴ�. 
		\details
		�� ��ü�� ������ �̸��� �����ϴ�. �ش� �̸��� subclass���� overload�� constructor���� ��� ó���� �� ������ �� �ֽ��ϴ�.

		Leesuk kim, lktime@skku.edu
		*/
		probaclass(std::string name);

		/**
		\brief �̸��� �����ɴϴ�.b
		\details GoF�� objective oriented programming design concept�� ���� getter�Դϴ�.
		Leesuk kim, lktime@skku.edu
		*/
		std::string getName();
	};
}