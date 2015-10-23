#pragma once
#include"vsheader.h"
#include "cpon.h"
#include "Utility.h"
#include <math.h>
#ifdef _WIN32
void makeDir(char *full_path){ mkdir(full_path); }
#else
void makeDir(char *full_path){ mkdir(full_path, 0755); }
#endif
/* beta function */
static __inline double getBetaDistribution(double x, double beta_a_, double beta_b_)
{
	//double x = exp((beta_a_ - 1.0) * log(a));
	//double y = exp((beta_b_ - 1.0) * log(1.0 - a));
	//return x * y;
	return exp((beta_a_ - 1.0) * log(x)) * exp((beta_b_ - 1.0) * log(1.0 - x));
}


/* numerical integration using an adaptive simpson method
[parameter]
f: double type function pointer
a: start range
b: end range
n: count of fragment

[return]
integrated scala value from a to b
*/
double simpsonIntegrate(double a, double b, int n, double beta_a__, double beta_b__)
{
	int i;
	double answer, h, x;

	answer = getBetaDistribution(a, beta_a__, beta_b__);
	h = (b - a) / n;
	for (i = 1; i <= n; i++)
	{
		x = a + i * h;
		answer += 4 * getBetaDistribution(x - h / 2, beta_a__, beta_b__) + 2 * getBetaDistribution(x, beta_a__, beta_b__);
	}
	answer -= getBetaDistribution(b, beta_a__, beta_b__);

	return (h * answer / 6.0);
}

/* 적분한다능
numerical integration using an adaptive simpson method
[parameter]
f: double type function pointer
a: start range
b: end range
n: count of fragment
tolerance: 일종의 정확도. 낮으면 낮을 수록 높은 정확도를 보인다.
[return]
*/
double integrate(double a, double b, int n, double tolerance, double beta_a, double beta_b)
{
	double val, check;

	val = simpsonIntegrate(a, b, 2 * n, beta_a, beta_b);
	check = fabs((simpsonIntegrate(a, b, n, beta_a, beta_b) - val) / val);

	if (check > tolerance && fabs(b - a) > 0.01)
		val = integrate(a, (a + b) / 2.0, n, tolerance, beta_a, beta_b) +
		integrate((a + b) / 2.0, b, n, tolerance, beta_a, beta_b);

	return val;
}


/* calculate p-value using K-S test */
double KS_test(double a, int b)
{
	int j;
	double d, a2, fac, sum, term, termbf;
	fac = 2.0; sum = termbf = 0.0;

	d = sqrt((double)b);
	a2 = -2.0 * (a * d) * (a * d);

	for (j = 1; j <= 100; j++)
	{
		term = fac * exp(a2 * j * j);
		sum += term;

		if (fabs(term) <= 0.001 * termbf || fabs(term) <= 1.0e-8 * sum)
			return sum;

		fac = -fac;
		termbf = fabs(term);
	}

	return 1.0;
}

/* calculate p-value using two-sample K-S test */
double KS_test_two(double a, int b, int c)
{
	int j;
	double d, a2, fac, sum, term, termbf;
	fac = 2.0; sum = termbf = 0.0;

	d = sqrt((double)b * c / (b + c));
	a2 = -2.0 * (a * d) * (a * d);

	for (j = 1; j <= 100; j++)
	{
		term = fac * exp(a2 * j * j);
		sum += term;

		if (fabs(term) <= 0.001 * termbf || fabs(term) <= 1.0e-8 * sum)
			return sum;

		fac = -fac;
		termbf = fabs(term);
	}

	return 1.0;
}

double vdistance(double x, double y){ return x == y ? 0 : (x - y) * (x - y); }
double feature_scaling(double pos, double min, double max){ return (pos - min) / (max - min); }
double standard_score(double x, double ave, double stddev){ return (x - ave) / stddev; }


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

	//printf("cofactor of [%d][%d] : %lf\n", 2, 2, getCofactor(im, 1, 1, n));
	//printf("det : %lf\n", d);
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

	return d == 0 ? 1.0e-20 : d;
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