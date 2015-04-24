#pragma once
#include "vsheader.h"
#include <math.h>

#ifdef _Win32
#include <direct.h>
void makeDir(char *full_path);
#else
#include <sys/stat.h>
void makeDir(char *full_path);
#endif

/*** global variables ***/
//double beta_a, beta_b;
/* beta function */
static __inline double getBetaDistribution(double x, double beta_a_, double beta_b_);
double simpsonIntegrate(double a, double b, int n, double beta_a__, double beta_b__);
double integrate(double a, double b, int n, double tolerance, double beta_a, double beta_b);

FILE* debug;

/* calculate p-value using K-S test */
double KS_test(double a, int b);

double KS_test_two(double a, int b, int c);

//vector인 x와 y간의 distance을 계산합니다.
double vdistance(double x, double y);
double feature_scaling(double pos, double min, double max);
double standard_score(double x, double ave, double stddev);

/*allocate memory for square matrix*/
void allocSquareMatrix(double ***m, int n);
/*print matrix*/
void pMatrix(double ***m, int n);
/*inverse materix*/
void minverse(double ***im, int n, double ***om);
/*get determinant of given matrix*/
double det(double*** m, int n);
/*get cofactor at given row and column on given matrix*/
double getCofactor(double ***m, int r, int c, int n);
/*get adjugate(adjoint) of given matrix*/
void madjugate(double ***m, int n);