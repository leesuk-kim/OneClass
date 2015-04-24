#include <stdio.h>
#include <math.h>
#include <stdlib.h>

void allocSquareMatrix(double ***m, int n);
void pMatrix(double ***m, int n);
void minverse(double ***im, int n, double ***om);
double det(double*** m, int n);
double getCofactor(double ***m, int r, int c, int n);
void madjugate(double ***m, int n);