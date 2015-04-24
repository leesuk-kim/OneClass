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

FILE *randRawbak;

/***********************************
Structure and structure-based methods
************************************/

struct struct_record {
	int    index;
	double value;
};
//for pivot
typedef struct struct_record Record_t;

struct struct_beta
{
	double shape;
	double var;
	double bias;
};
/*struct for beta distribution
shape: beta shape parameter
var : variance
vias: vias with p-value
*/
typedef struct struct_beta Beta_t;

struct struct_data
{
	double *raw;//raw data. each dimension.
	double feature;
};
/*
raw: raw data
feature: featured by normalization method with SpaceDim_t.
*/
typedef struct struct_data Data_t;

struct struct_spacedim{
	double mean;
	double variance;
};
/*각 saplespace의 dimention별 mean과 variance입니다.
매 fold마다, 매 class마다 새로 계산됩니다.
*/
typedef struct struct_spacedim SpaceDim_t;

struct struct_betaspace{
	int	 no_centroids;
	double sigma;
	double weight;
	struct struct_beta beta_a;
	struct struct_beta beta_b;
	double p_value_train;
	double p_value_trts;
};
/*centroid의 parameter입니다.
sigma:5개의 candidated sigma 중 선택된 sigma
weight:
beta_a: 베타분포의 a값
beta_a_variance: 베타분포의 a의 분산
beta_a_bias: 베타분포의 a값의 bias
beta_b: 베타분포의 b값
beta_b_variance: 베타분포의 b값의 분산
beta_b_bias:베타분포의 b값의 bias
p_value_train: 현재 읽은 training data의 p-value
p_value_trts: 현재 읽은 test data의 p-value
*/
typedef struct struct_betaspace BetaSpace_t;

struct struct_centroid
{
	int id_fold;
	int id_cls;
	int id_ctrd;
	FILE **logSample;
	int *dimSize;
	double* betaA;
	double* betaB;
	double **betaDataA;
	double **betaDataB;
	double *varMax;
	int delegatorsize;
	Data_t delegate;
	SpaceDim_t *vSpaceDim;
	double *vDimWeight;
	double *pdfweight;
	int dele_beta_idx;
	BetaSpace_t *vBetaSpace;
	double *seedRaw;

};
typedef struct struct_centroid Centroid_t;
double* getBetaA(Centroid_t *this);
double* getBetaB(Centroid_t *this);
double* getVarMax(Centroid_t *this);

struct struct_testspace
{
	int id_fold;
	int id_cls;
	FILE **logSample;
	int* dimSize;
	int datasize;
	Data_t *data;
	double *pdf;
	Record_t *pdf_fs;
};
typedef struct struct_testspace TestSpace_t;

struct struct_trainspace
{
	int id_fold;
	int id_cls;
	FILE **logSample;
	int* isSeed;
	int *dimSize;
	int *ctrdSize;
	Centroid_t *vCentroid;
	int datasize;
	Data_t *data;
	double *pdf;
	Record_t *pdf_fs;
	double *cdf;
	double *cmp;
	double y_min;
	double y_max;
	double ctrd_beta_a;
	double ctrd_beta_b;
	double *ctrd_vBetaData_a;
	double *ctrd_vBetaData_b;
	double ctrd_var_max;
};
typedef struct struct_trainspace TrainSpace_t;

struct struct_fld
{
	FILE **logSample;
	double *vMean;
	double **mMean;
	double ***S;
	double **mSB;
	double **mSW;
	double** mWeight;
	double weight_trps;
	double* vEgien;
	double *vProjMean;
	double *vProjS;
};
typedef struct struct_fld fld_t;

struct struct_classspace
{
	int id_fold;
	int id_cls;
	char *sCLF;
	FILE *log;
	int *isSeed;
	int *dimSize;
	int *ctrdSize;
	TrainSpace_t trainspace;
	TestSpace_t testspace;
	SpaceDim_t *vTrainSpaceDim;
};
typedef struct struct_classspace ClassSpace_t;

struct struct_samplespace
{
	int id_fold;
	char* sCLF;
	FILE* log;
	int *isSeed;
	int *dimSize;
	int *clsSize;
	int *ctrdSize;
	fld_t fld;
	ClassSpace_t *vClassSpace;
	int **mDiscriminant;

	//TrainSpace_t trainspace;
	//TestSpace_t testspace;
};
/*
data: raw data record array of dim.
datasize: record size
dimsize: dimension size
first: front of datapram. it use for attatch class-wide variable
y_min: min value
y_max: max value
pdf_tr: pdf of 
pdf_fs: pdf of feature scaling
cdf: cdf
cmp: compare...what?
discriminant: discriminant result
*/
typedef struct struct_samplespace SampleSpace_t;
double getYMin(SampleSpace_t *this);
double getYMax(SampleSpace_t *this);

struct struct_oneclass
{
	int isseed;
	int dimsize;
	int clssize;
	int ctrdsize;
	char sclf[15];
	FILE* log;
	SampleSpace_t *vSampleSpace;
};
typedef struct struct_oneclass OneClass_t;


/***********************************
Main Level Methods
************************************/
bool readData(char* filepath, int dim, int record_size, Data_t* _data);
void openDatabase(OneClass_t *db);
void closeDatabase(OneClass_t *db);
void mountData(OneClass_t *db);
//각 dimention별 mean과 variance를 계산합니다.
void setDimentionRange(ClassSpace_t *clsc);
//Fold별로 생성될 LogFile을 준비합니다.
void readySampleSpaceLogFile(SampleSpace_t *spsc);
void finishSampleSpaceLogFIle(SampleSpace_t *spsc);
//Class별로 생성될 LogFile을 준비합니다.
void readyClassSpaceLogFile(ClassSpace_t *clsc);
void finishClassSpaceLogFIle(ClassSpace_t *clsc);

//One class training algorithm을 수행합니다. 이를 통해 최적의 centroid 크기를 구합니다.
int trainOneClass(int f, int c, int ec, OneClass_t *db);
//opt president centroid for class via CPON
int optPresidentCPON(int f, int c, int t, int clst_c, OneClass_t *db);
//get CPON Power Ratio
void getCPONPowerRatio(int f, int c, int t, int ec, OneClass_t *db);
//read sead raw for concreted working
int readSeedRaw(OneClass_t *db);
//get filepath by type for open
void get_data_path_by_type(char typeCounter, int foldCounter, int classCounter, char* path);
//get data record size
int read_data_record_size(char typeCounter_, int foldCounter_, int classCounter_, int dim);
//get data dimension size
int read_data_dim_size();
//setCPON -> define for 'trainOneClass'
void fisher_linear_discriminant(SampleSpace_t *spsc);

/***********************************
Submethods participant in the method 'trainOneClass'
************************************/
//get cluster size via LBG clustering algorithm
int getLBGClusterSize(int ec, ClassSpace_t *clsc);
//set Range of Class Probability Output Network.
void setCPON(int ec, int clst_c, int l, ClassSpace_t *clsc);


/***********************************
getLBGClusterSize
************************************/
//find closest centroid index via distance
int find_closest_centroid_index(int ec, int dc, Centroid_t *ctrd, Data_t *data);


/***********************************
Submethods participant in the method 'setCPON'
************************************/
//set Probability Density Function with CPON range
void setPDF(int ec, int clst_s, int cdd_c, TrainSpace_t *trainspace);
//get mean via the method for sample
double getSampleMean(int recordsize, Record_t* r);
//get variance via the method for sample
double getSampleVariance(double mean, int recordsize, Record_t* r);
//set Beta Shape range
void setBetaSpaceShape(double mean, double var, double *betaA, double *betaB);
//set p-value for train
double p_value_train(int a, double* __y_cdf, double* __y_cmp, double beta_a, double beta_b, Record_t* __y_nor);
//set PDF with feature-scaling
void setFeatureScaledPDF(ClassSpace_t *clsc);
//set beta shape on CPONrange...maybe?
void centroid_draw_betashape(int candi, Centroid_t *centroid, TrainSpace_t *trsc);
//Submethods participant in the method 'p_val_train'
//call the method 'KS_test'
double getLInfinityNorm(int a, double* _y_cdf, double* _y_cmp, Record_t* _y_nor, double beta_a, double beta_b);


/***********************************
Submethods participant in the method 'fisher_linear_discriminant-'
************************************/
//get scatter matrix on each class
void getScatterMatrix(int i_cls, SampleSpace_t *spsc);


/***********************************
Candidated functions
************************************/
void printCentroid(Centroid_t *c, int no);
void printCentroidTable(Centroid_t **ct);
void printsamplespace(SampleSpace_t *d);
void printDimparam(SpaceDim_t *d, int no);
void fprintCentroid(Centroid_t *ctrd, int f, int c, int t, int e);
void fprintCentroidTable(Centroid_t **ctrds, int f, int c, int t);
void fprintsamplespace(SampleSpace_t *d, int f, int c, int t);


/***********************************
Deperated
************************************/
//set mean and variance for the normalized record
void sample_mean_variance(int a, double* _s_mean, double* _s_var, Record_t* _y_nor);
void parameter_beta_mm(double mean, double var, Centroid_t *centroid);
void parameter_beta_mm_jk(int a, int b, double s_mean_, double s_var_, Record_t* _y_nor, Centroid_t *centroid);


/***********************************
넌 누구니?
************************************/
int partition(Record_t* a, int n, double pivot);
int findpivot(Record_t* a, int n, double* pivot_ptr);
void quicksort(Record_t* a, int n);
//fisher's linear discriminant
void filidi();

struct struct_result
{
	int **discriminant;
	double acc;
	double pre;
	double rec;
	double fm;
};
typedef struct struct_result result_t;