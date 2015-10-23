/* declare structures */
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "vsheader.h"
#include "Utility.h"

/***********************************
Global Variables
************************************/

#define PI 3.141592
#define TENFOLD 10
#define CLASSSIZE 2
#define TRAIN_TEST 2
#define CANDIDATE_SIZE 5

#define DATATYPE_TRAIN	0
#define DATATYPE_TEST	1
#define FILEPATH_LENGTH 256

#define TRAIN	0
#define TEST	1

#define CNTR_IGNORE_SIZE 2
#define CLUSTERING_COUNT 20

//integrate
#define INTEGRATE_TOLERANCE 0.002
#define INTEGRATE_FROM 1.0E-8
#define INTEGRATE_TO 1 - 1.0E-8

struct clfscore_struct
{
	double a;
};
typedef struct clfscore_struct clfscore;

typedef struct BETASAMPLE_STRUCT
{
	int a;
} BETASAMPLE;

typedef struct KERNEL_STRUCT
{
	BETASAMPLE* betasamples;
	double alpha, beta;
	BETASAMPLE(*kernelize)(struct KERNEL_STRUCT kernel);
	BETASAMPLE(*quantize)(struct KERNEL_STRUCT kernel);
} KERNEL;

typedef struct CLASS_STRUCT
{
	KERNEL* pKernels, nKernels;

} CLASS;

void CPON_LEARN(double*** data);
void CPON_TEST(double*** data);

struct CPON_STRUCT
{
	CLASS* classes;
	KERNEL* kernels;
	void(*learn)(double***);
	void(*test)(double***);
};
/*Class Probability Output Network
CLASS* classes: class ¹è¿­
*/
typedef struct CPON_STRUCT CPON;

CPON* CPON_initialize(CPON* cpon);
CPON CPON_finish(CPON* cpon);
