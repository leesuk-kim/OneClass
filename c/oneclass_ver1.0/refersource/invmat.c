#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "invmat.h"

void allocSquareMatrix(double ***m, int n){
	int i;
	*m = (double**)calloc(n, sizeof(double*));
	for (i = 0; i < n; i++)
		(*m)[i] = (double*)calloc(n, sizeof(double));
}

void pMatrix(double ***m, int n){
	int i, j;

	for (i = 0; i < n; i++)
		for (j = 0; j < n; putchar(++j == n ? '\n' : ' '))
			printf("%5.2lf", (*m)[i][j]);
}


void minverse(double ***im, int n, double ***om)
{
	int i, j, s = 0;
	double d = det(im, n);

	printf("cofactor of [%d][%d] : %lf\n", 2, 2, getCofactor(im, 1, 1, n));
	printf("det : %lf\n", d);
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++, s++)
			(*om)[i][j] = getCofactor(im, i, j, n) * (s % 2 == 0 ? 1. : -1.) / d;
	madjugate(om, n);
}

double det(double*** m, int n)
{
	double d = 0, diagonal;
	int ic, i, j;
	int dot, dotSize = n < 3 ? n : 2 * n;

	if (n == 1)
		return *m[0][0];
	if (n == 2)
		return (*m)[0][0] * (*m)[1][1] - (*m)[1][0] * (*m)[0][1];
	
	for (dot = 0, ic = 0; dot < dotSize; dot++)
	{
		diagonal = dot >= dotSize / 2 ? -1 : 1;
		//ic의 시작 좌표를 지정해야 합니다. row index야 무조건 0부터 시작하니까 괜찮..
		for (i = 0, j = ic; i < n; i++)
		{
			diagonal *= (*m)[i][j];
			if (isnan(diagonal))
				printf("gskjflbgfaklgbfdalkjgb");
			j = dot < n ? (++j == n ? 0 : j) : (--j == -1 ? n - 1 : j);
		}
		d += diagonal;
		if (dot != n - 1)
			ic += dot < n ? 1 : -1;
	}

	return d == 0 ? 1.0e-20 :d;
}

double getCofactor(double ***m, int r, int c, int n)
{
	double cof = 0;
	double **sub;
	int i, j, g, h;

	allocSquareMatrix(&sub, n - 1);

	for (g = 0, i = 0; i < n; i++)
	{
		if (i != r)
		{
			for (h = 0, j = 0; j < n; j++)
			{
				if (j != c)
				{
					sub[g][h] = (*m)[i][j];
					h++;
				}
			}
			g++;
		}
	}
	cof = det(&sub, n - 1);
	//pMatrix(&sub, n - 1);
	for (i = 0; i < n - 1; free(sub[i++]));
	free(sub);

	return cof;
}

void madjugate(double ***m, int n)
{
	int i, j;
	double t;
	for (i = 0; i < n; i++)
		for (j = 0; j < i; j++)
		{
			t = (*m)[i][j];
			(*m)[i][j] = (*m)[j][i];
			(*m)[j][i] = t;
		}
}