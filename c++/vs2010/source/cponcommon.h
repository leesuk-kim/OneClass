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
	\brief delete의 함수형입니다.
	\details
	이 함수는 단순히 typename T를 delete합니다. 하지만 delete는 예약어기때문에 function pointer로 넘겨줄 수 없습니다.
	따라서 function pointer로 넘겨주기 위해 templete function를 추가적으로 만들었습니다.
	사실 별 쓸모는 없고 template를 모른다고 친구가 놀려서 한 번 만들어 봤습니다...
	*/
	template <typename T> void del(typename T t){
		delete t;
	}

	/**
	\brief std::map에 대한 반복적인 작업을 처리합니다.
	\details
	typename T로 정의된 std::map가 있다면, std::map에 속한 각각의 element에 대한 작업을 처리할 때 사용합니다.
	사실 별 쓸모는 없고 template를 모른다고 친구가 놀려서 한 번 만들어 봤습니다...
	*/
	template <typename T1, typename T2> void map_fptr_iter(std::map<typename T1, typename T2>* mappist, void(*fptr)(typename T2 t2)){
		typename std::map<T1, T2>::iterator iter;
		for(iter = mappist->begin(); iter!= mappist->end(); iter++){
			(*fptr)(iter->second);
		}
	}

	/**
	\brief double to string
	*/
	std::string dtos(double d);
}

namespace kil{
	/**
	\brief double type의 std::vector의 mean을 계산합니다.
	*/
	double mean(std::vector<double> doublevec);
	/**
	\brief double type의 std:;vector의 variance를 계산합니다.
	\details
	kil::mean이 선행되어야 합니다. parameter로 mean을 받기 때문입니다.
	\param doublevec 계산될 double type의 std::vector입니다.
	\param mean doublevec의 평균입니다.
	*/
	double var(std::vector<double> doublevec, double mean);
	
	/**
	\brief normalize합니다.
	\details
	normalization 중에서도 feature-scaling을 사용하여 normalize합니다.
	feature-scaling을 위해 min과 max를 필요로 합니다.
	*/
	class featurescaler{
	private:
		double mMin;
		double mMax;
		double mDiffNx;
	public:
		double getMin(){
			return mMin;
		};
		double getMax(){
			return mMax;
		};
		featurescaler(double min, double max);
		/**featurescaler를 만들어주는 factory 함수입니다.*/
		static featurescaler* factory(std::vector<double> doublevec);
		/**
		\brief normalize합니다.
		\details
		여기서는 feature-scaling으로 normalization합니다.
		*/
		double output(const double& randomvariable);
	};

	/**
	\brief kernelize합니다.
	\details
	kernelize합니다. 이를 위해 mean과 var를 생성자의 parameter로 받습니다.
	*/
	class kernelizer{
	private:
		double mMean;
		double mVar;
	public:
		double getMean(){
			return mMean;
		};
		double getVar(){
			return mVar;
		};
		kernelizer(double mean, double var);
		/**
		\brief 수학적인 의미의 kernel function입니다.
		\details
		https://en.wikipedia.org/wiki/Kernel_(statistics)#In_non-parametric_statistics 에 언급된 kernel function을 의미합니다. 
		여기서는 Gaussian kernel function을 구현하였습니다.
		*/
		double _kernel(double& randomvariable);
		/**
		\brief kernelize합니다.
		\details
		data가 특정 범위 내에서 통계적인 값을 가지며, 그 값 중 평균이 통계적으로 가장 높은 확률을 가진다고 가정해보겠습니다.
		이 때 data의 확률값을 표현할 수 있는 함수가 kernel입니다.
		자세한 내용은 https://en.wikipedia.org/wiki/Kernel_(statistics) 을 참조하십시오.
		*/
		double output(double& randomvariable);
	};

	namespace beta {
		/**
		\brief struct for contatining parameters of beta function.
		\details Boost Library의 beta 함수는 random variable과 paramter를 동시에 입력해야 beta output이 출력됩니다.
		random variable은 사용할 때마다 달라지니, 결국 특정 class의 shape of beta function을 결정하는 것은 beta parameter입니다.
		따라서 beta parameter인 alpha와 beta값을 하나의 struct로 묶어서 관리합니다.

		\author Leesuk kim, lktime@skku.edu
		*/
		struct betaparam_t {
			double alpha;
			double beta;
		};

		/**
		\brief kstest 결과를 저장합니다.

		\author Leesuk kim, lktime@skku.edu
		*/
		struct ksresult_t {
			/**Kolmogorov-Smirnov statistic*/
			double d;
			/**p-value*/
			double pval;
		};

		/**
		\brief kstest할 때 기본적으로 필요한 data들을 저장합니다.

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
		\brief fragmentated beta shape를 그립니다.
		\details 
		fragmentated beta shape란 random varialble의 크기에 따라 해당하는 beta function을 가지고 있는 struct입니다.
		이것은 beta fitting algorithm으로 fitting할 수 없었던 극단적인 케이스도 fitting할 수 있도록
		여러 개의 beta function을 가지고 있으며, 각각의 beta function이 cover하는 random variable을 저장하는 xaxis가 있습니다.

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

	class probaclass {
	protected:
		std::string mName;
	public:
		probaclass(std::string name);
		std::string getName();
	};
}