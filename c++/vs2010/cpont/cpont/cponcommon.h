#pragma once
#pragma warning(disable:4996)
#include <iostream>
#include <vector>
#include <map>
#include <numeric>
#include <string>
#include <fstream>
#include <sstream>
#include "incgammabeta.h"


namespace std{

	/**
	\brief double to std::string
	\details double 형 변수를 std::string으로 변환합니다.
	\author Leesuk kim, lktime@skku.edu
	*/
	std::string dtos(double d);

	/**
	\brief int to 8 character string
	\details int형 변수를 8글자짜리 std::string으로 변환합니다.
	\author Leesuk kim, lktime@skku.edu
	*/
	std::string ito8s(int i);
}

namespace kil{
	/**
	\brief double type의 std::vector의 mean을 계산합니다.
	\details
	평균을 계산합니다. 정말입니다!(미국 개그)

	\author Leesuk Kim, lktime@skku.edu
	*/
	double mean(std::vector<double> doublevec);

	/**
	\brief double type의 std:;vector의 variance를 계산합니다.
	\details
	kil::mean이 선행되어야 합니다. parameter로 mean을 받기 때문입니다.
	\param doublevec 계산될 double type의 std::vector입니다.
	\param mean doublevec의 평균입니다.

	\author Leesuk Kim, lktime@skku.edu
	*/
	double var(std::vector<double> doublevec, double mean);
	
	/**
	\brief normalize합니다.
	\details
	normalization 중에서도 feature-scaling을 사용하여 normalize합니다.
	feature-scaling을 위해 min과 max를 필요로 합니다.

	\author Leesuk Kim, lktime@skku.edu
	*/
	class featurescaler{
	private:
		double mMin;
		double mMax;
		double mDiffNx;
	public:
		/**
		\brief private field인 mMin의 getter입니다.
		\details GoF의 objective oriented programming design concept에 맞춘 getter입니다.

		\author Leesuk Kim, lktime@skku.edu
		*/
		double getMin(){
			return mMin;
		};
		/**
		\brief private field인 mMax의 getter입니다.
		\details GoF의 objective oriented programming design concept에 맞춘 getter입니다.

		\author Leesuk Kim, lktime@skku.edu
		*/
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
		https://en.wikipedia.org/wiki/Feature_scaling 을 참고해주시기 바랍니다.

		\author Leesuk Kim, lktime@skku.edu
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
		/**
		\brief private field인 mean의 getter입니다.
		\details GoF의 objective oriented programming design concept에 맞춘 getter입니다.

		\author Leesuk Kim, lktime@skku.edu
		*/
		double getMean(){
			return mMean;
		};

		/**
		\brief private field인 var의 getter입니다.
		\details  여기서 var는 variance(분산)을 의미합니다.
		GoF의 objective oriented programming design concept에 맞춘 getter입니다.
		
		\author Leesuk Kim, lktime@skku.edu
		*/
		double getVar(){
			return mVar;
		};
		
		/**
		\brief kernelizer의 생성자입니다.
		\details
		그 기능은 많지 않습니다. 그저 argument로 받은 mean과 var를 저장합니다.

		\author Leesuk kim, lktime@skku.edu
		*/
		kernelizer(double mean, double var);
		/**
		\brief 수학적인 의미의 kernel function입니다.
		\details
		https://en.wikipedia.org/wiki/Kernel_(statistics)#In_non-parametric_statistics 에 언급된 kernel function을 의미합니다. 
		여기서는 Gaussian kernel function을 구현하였습니다.

		\author Leesuk kim, lktime@skku.edu
		*/
		double _kernel(double& randomvariable);

		/**
		\brief kernelize합니다.
		\details
		kernel의 가장 기본적인 목표는 random variable의 공간을 특정 범위 한으로 제한하는 것입니다. 
		이를 통해 특정 범위의 중앙에 가까울수록 kernel의 출력값은 높아지고, 멀수록 낮아집니다. 
		이 함수는 gaussian distribution을 따르는 gaussian kernel function처럼 작동합니다.		
		random variable이 어떤 class의 kernel에 input되느냐에 따라 결과는 모두 다릅니다.
		각각의 class가 갖는 kernel의 범위가 다르기 때문입니다.
		자세한 내용은 https://en.wikipedia.org/wiki/Kernel_(statistics) 을 참조하십시오.
		
		\author Leesuk Kim, lktime@skku.edu
		*/
		double output(double& randomvariable);
	};

	typedef std::map<int, std::vector<double>> datamap;
	typedef std::map<int, std::vector<double>>::iterator datamap_iter;
	typedef std::pair<int, std::vector<double>> datamap_pair;

	/**
	\namespace beta
	\brief beta function과 관련된 함수 또는 객체가 있습니다.
	특히, CPON의 model은 beta function에 의해 결정되기 때문에 매우 중요합니다.

	\author Leesuk Kim, lktime@skku.wdu
	*/
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
		Cited from Numerical Recipes in C, 2nd edition, p.626
		\param alam
		
		\author Leesuk kim, lktime@skku.edu
		*/
		double qks(const double& alam);
		/*
		\brief Kolmogorov-Smirnov Test on two sample.
		\details 
		This test tests two sample for learning phase.
		Cited from numerical recipies ASC, 3rd edithion, p.737-738

		\author Leesuk kim, lktime@skku.edu
		*/
		struct ksresult_t kstest(std::vector<double>& sample1, std::vector<double>& sample2);
	}

	/**
	\brief CPON의 super class입니다.
	\details 학습용/분류용 cpon은 각각의 필요한 기능을 추가적으로 갖습니다. 이 클래스는 공통적인 기능을 가지고 있는 class입니다. 

	\author Leesuk kim, lktime@skku.edu
	*/
	class probaclass {
	protected:
		int index;
	public:
		/**
		\brief Constructor입니다. 
		\details
		각 객체는 고유의 이름을 가집니다. 해당 이름은 subclass에서 overload할 constructor에서 어떻게 처리할 지 결정할 수 있습니다.

		\author Leesuk kim, lktime@skku.edu
		*/
		inline probaclass(int idx){
			index = idx;
		};

		/**
		\brief 이름을 가져옵니다.
		\details GoF의 objective oriented programming design concept에 맞춘 getter입니다.

		\author Leesuk kim, lktime@skku.edu
		*/
		inline int getIndex(){
			return index;
		}
	};
}