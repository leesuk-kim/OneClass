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

typedef struct struct_record{
	int    index;
	double value;
}record_t;

struct struct_dim_param{
	double mean;
	double variance;
};
/*각 dimention별 mean과 variance입니다.
매 fold마다, 매 class마다 새로 계산됩니다.
*/
typedef struct struct_dim_param dimparam_t;

struct struct_cpon_range{
	int	 no_centroids;
	double sigma;
	double weight;
	double beta_a;
	double beta_a_variance;
	double beta_a_bias;
	double beta_b;
	double beta_b_variance;
	double beta_b_bias;
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
typedef struct struct_cpon_range cpon_range_t;

struct struct_data
{
	double *raw;//raw data. each dimension.
//	double dev_ctrd;//deviation with closest centroid
};
/*
raw: raw data. each dimension.
dev_ctrd: deviation with closest centroid
*/
typedef struct struct_data data_t;

struct struct_dataparam
{
	data_t *data;
	int datasize;
	int dimsize;
	struct struct_dataparam *first;
	double y_min;
	double y_max;
	double *pdf_tr;
	record_t *pdf_fs;
	double *cdf;
	double *cmp;
	int **discriminant;
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
typedef struct struct_dataparam dataparam_t;
double getYMin(dataparam_t *this);
double getYMax(dataparam_t *this);

struct struct_centroid
{
	double *raw;
	double *dimweight;
	double var_max;
	int dimsize;
	dimparam_t *dimparam;
	int datasize;
	double *pdfweight;
	int cponindex;
	double beta_a;
	double beta_b;
	struct struct_centroid *first;
	double *betadata_a;
	double *betadata_b;
	double *seedRaw;
	cpon_range_t *cpon_range;
};
typedef struct struct_centroid centroid_t;
double* getBetaA(centroid_t *this);
double* getBetaB(centroid_t *this);
double* getVarMax(centroid_t *this);

struct struct_result
{
	int **discriminant;
	double acc;
	double pre;
	double rec;
	double fm;
};
typedef struct struct_result result_t;

struct struct_oneclass
{
	dataparam_t ***table_dataparam;
	centroid_t ****table_centroid;
	dimparam_t ****table_dimparam;
	int dimsize;
	int ctrdsize;
	int isRRD;
	char clfs[16];
	FILE* mlf;
	FILE** flf;
};
typedef struct struct_oneclass oneclass_t;


/***********************************
Main Level Methods
************************************/
bool readData(char* filepath, int dim, int record_size, data_t* _data);
void openDatabase(oneclass_t *db);
void closeDatabase(oneclass_t *db);
void mountData(oneclass_t *db);
//각 dimention별 mean과 variance를 계산합니다.
void setDimentionRange(int f, int c, int t, oneclass_t* db);
//Fold별로 생성될 LogFile을 준비합니다.
void readyFoldLogFile(int f, oneclass_t *db);
//One class training algorithm을 수행합니다. 이를 통해 최적의 centroid 크기를 구합니다.
int trainOneClass(int f, int c, int ec, oneclass_t *db);
//opt president centroid for class via CPON
int optPresidentCPON(int f, int c, int t, int clst_c, oneclass_t *db);
//get CPON Power Ratio
void getCPONPowerRatio(int f, int c, int t, int ec, oneclass_t *db);
//read sead raw for concreted working
int readSeedRaw(oneclass_t *db);
//get filepath by type for open
char* get_data_path_by_type(char typeCounter, int foldCounter, int classCounter);
//get data record size
int read_data_record_size(char typeCounter_, int foldCounter_, int classCounter_, int dim);
//get data dimension size
int read_data_dim_size(char typeCounter_, int foldCounter_, int classCounter_);
//setCPON -> define for 'trainOneClass'

/***********************************
Submethods participant in the method 'trainOneClass'
************************************/
//get cluster size via LBG clustering algorithm
int getLBGClusterSize(int f, int c, int ec, oneclass_t *db);
//set Range of Class Probability Output Network.
void setCPON(int f, int c, int t, int ec, int clst_c, int l, oneclass_t *db);


/***********************************
getLBGClusterSize
************************************/
//find closest centroid index via distance
int find_closest_centroid_index(int ec, int dc, centroid_t *ctrd, data_t *data);


/***********************************
Submethods participant in the method 'setCPON'
************************************/
//set Probability Density Function with CPON range
void setPDF(int ec, int clst_s, int cdd_c, centroid_t *ctrd, dataparam_t *dataparam);
//get mean via the method for sample
double getSampleMean(int recordsize, record_t* r);
//get variance via the method for sample
double getSampleVariance(double mean, int recordsize, record_t* r);
//set Beta Shape range
void setBetaShapeRange(double mean, double var, double *betaA, double *betaB);
//set p-value for train
double p_value_train(int a, double* __y_cdf, double* __y_cmp, double beta_a, double beta_b, record_t* __y_nor);
//set PDF with feature-scaling
void setFeatureScaledPDF(dataparam_t *dtpm);
//set beta shape on CPONrange...maybe?
void setCPONRangeBetaParams(int candi, centroid_t *centroid, dataparam_t *dataparam);
//Submethods participant in the method 'p_val_train'
//call the method 'KS_test'
double getLInfinityNorm(int a, double* _y_cdf, double* _y_cmp, record_t* _y_nor, double beta_a, double beta_b);


/***********************************
Candidated functions
************************************/
void printCentroid(centroid_t *c, int no);
void printCentroidTable(centroid_t **ct);
void printDataparam(dataparam_t *d);
void printDimparam(dimparam_t *d, int no);
void fprintCentroid(centroid_t *ctrd, int f, int c, int t, int e);
void fprintCentroidTable(centroid_t **ctrds, int f, int c, int t);
void fprintDataparam(dataparam_t *d, int f, int c, int t);


/***********************************
Deperated
************************************/
//set mean and variance for the normalized record
void sample_mean_variance(int a, double* _s_mean, double* _s_var, record_t* _y_nor);
void parameter_beta_mm(double mean, double var, centroid_t *centroid);
void parameter_beta_mm_jk(int a, int b, double s_mean_, double s_var_, record_t* _y_nor, centroid_t *centroid);

int partition(record_t* a, int n, double pivot);
int findpivot(record_t* a, int n, double* pivot_ptr);
void quicksort(record_t* a, int n);