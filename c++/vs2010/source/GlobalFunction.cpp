/*
Copyright 2015. Bae Hanbin(bhb0722@nate.com or bhb0722@skku.edu) all rights reserved.
Beta Functions for Global Function
*/
#include "GlobalFunction.h"

void getCentroid(double **sample, int n, double **result, double *sigma, int nCent, int ndim)
{
	int i, j, k, ihi, niter = 0;
	double md, dist, rtol;
	int* count = (int*)malloc(sizeof(int) * nCent);
	double** fsum = (double**)malloc(sizeof(double*) * nCent);

	for (i = 0; i < nCent; i++)
	{
		fsum[i] = (double*)malloc(sizeof(double) * ndim);

		for (j = 0; j < ndim; j++)
			result[i][j] = sample[i][j];
	}

	do{
		rtol = 0.f;
		for (i = 0; i < nCent; i++)
		{
			count[i] = 0;
			sigma[i] = FPMIN;
			for (j = 0; j < ndim; j++)
				fsum[i][j] = 0.f;
		}

		for (i = 0; i < n; i++){
			md = -1.f;

			for (j = 0; j < nCent; j++){
				dist = 0.f;

				for (k = 0; k < ndim; k++)
					dist += (sample[i][k] - result[j][k])*(sample[i][k] - result[j][k]);

				if (md == -1.f || dist < md)
				{
					md = dist;
					ihi = j;
				}
			}

			count[ihi]++;
			sigma[ihi] += md;

			for (j = 0; j < ndim; j++){
				fsum[ihi][j] += sample[i][j];
			}
		}

		for (i = 0; i < nCent; i++){
			//cout << "getcent " <<  count[i] << endl;
			sigma[i] /= (double)count[i];
			for (j = 0; j < ndim; j++){

				fsum[i][j] /= (double)count[i];
				//cout << fsum[i][j] << ' ' ;
				rtol += (fsum[i][j] - result[i][j])*(fsum[i][j] - result[i][j]);
				result[i][j] = fsum[i][j];
			}
		}

		niter++;
	} while (rtol > 1.0e-6 && niter < NMAX);


	for (i = 0; i < nCent; i++)
		free(fsum[i]);

	free(count);
	free(fsum);

}


double** getMatrixInverse(double **a, int n)
{
	double **y, d, *col;
	int i, j, *indx;

	y = (double**)malloc(sizeof(double*)*n);
	col = (double*)malloc(sizeof(double)*n);
	indx = (int*)malloc(sizeof(int)*n);

	for (i = 0; i < n; i++)
		y[i] = (double*)malloc(sizeof(double)*n);

	ludcmp(a, n, indx, &d);
	for (j = 0; j < n; j++){
		for (i = 0; i < n; i++) col[i] = 0.f;
		col[j] = 1.f;
		lubksb(a, n, indx, col);
		for (i = 0; i < n; i++) y[i][j] = col[i];
	}

	free(col);
	free(indx);
	return y;
}

void ludcmp(double **a, int n, int *indx, double *d)
{
	int i, imax, j, k;
	double big, dum, sum, temp;
	double *vv = (double*)malloc(sizeof(double)*n);
	
	*d = 1.f;

	for (i = 0; i < n; i++){
		big = 0.f;
		for (j = 0; j < n; j++)
			if ((temp = fabs(a[i][j])) > big) big = temp;

		vv[i] = 1.f / big;
	}

	for (j = 0; j < n; j++){
		for (i = 0; i < j; i++){
			sum = a[i][j];
			for (k = 0; k < i; k++) sum -= a[i][k] * a[k][j];
			a[i][j] = sum;
		}
		big = 0.f;
		for (i = j; i < n; i++){
			sum = a[i][j];
			for (k = 0; k < j; k++)
				sum -= a[i][k] * a[k][j];
			a[i][j] = sum;
			if ((dum = vv[i] * fabs(sum)) >= big){
				big = dum;
				imax = i;
			}
		}

		if (j != imax){
			for (k = 0; k < n; k++){
				dum = a[imax][k];
				a[imax][k] = a[j][k];
				a[j][k] = dum;
			}
			*d = -(*d);
			vv[imax] = vv[j];
		}
		indx[j] = imax;
		if (a[j][j] == 0.f) a[j][j] = TINY;
		
		if (j != n){
			dum = 1.f / (a[j][j]);
			for (i = j+1; i < n; i++) a[i][j] *= dum;
		}
	}

	delete[] vv;
}

void lubksb(double **a, int n, int *indx, double *b)
{
	int i, ii = -1, ip, j;
	double sum;

	for (i = 0; i < n; i++){
		ip = indx[i];
		sum = b[ip];
		b[ip] = b[i];
		if (ii >= 0)
			for (j = ii; j <= i - 1; j++) sum -= a[i][j] * b[j];
		else if (sum) ii = i;
		b[i] = sum;
	}
	for (i = n - 1; i >= 0; i--){
		sum = b[i];
		for (j = i + 1; j < n; j++) sum -= a[i][j] * b[j];
		b[i] = sum / a[i][i];
	}
}

double getDWLS(double *sample, int n, double params[])
{
	//LOG("BD", "getDWLS");
	int i, j;
	double cdfValue;
	double result = 0.f, coeff1, coeff2, coeff3 ,coeff4,coeff5;
	double np = (double)n + 1;

	coeff1 = np * np * (np + 1);

	for (i = 0; i < n; i++){
		if (i != n - 1 && sample[i] == sample[i + 1])
			continue;

		j = i + 1;
		coeff2 = j * (np - j);
		coeff3 = j / np;
		coeff4 = i / np;

		if (coeff3 > coeff4) coeff3 = coeff4;

		cdfValue = getBetaCDF(sample[i], params);

		coeff5 = (cdfValue - coeff3);
		result += coeff1 / coeff2 * coeff5 * coeff5;
		//result += coeff5 * coeff5;
	}

	result -= params[0] + params[1] + (sample[0] - params[2]) + (params[3] - sample[n - 1]);

	if (result == NAN) result = INFINITY;

	return result;
}

double getBetaCDF(double px, double params[])
{
	//LOG("BD", "getBetaCDF()");
	double a = params[0], b = params[1], L = params[2], U = params[3];
	double x;
	double bCoeff;

	if (a <= 0.f || b <= 0.f){
		//LOG("EXCEPTION", "a or b is negative value.\n");
		return 0.f;
	}

	if (px <= L)
		return 0.f;

	if (px >= U)
		return 1.f;

	x = (px - L) / (U - L);
	bCoeff = tgamma(a + b) / (tgamma(a) * tgamma(b)) * pow(x, a) * pow(1 - x, b);

	if (x < (a + 1.f) / (a + b + 2.f))
		return bCoeff * getBetaCoeff(x, a, b) / a;
	else
		return 1.f - bCoeff * getBetaCoeff(1.f - x, b, a) / b;
}

double getBetaCoeff(double x, double a, double b){
	int m, m2;
	double aa, c, d, del, h;
	double ab, ap, am;
	double fpmin = (double)FPMIN, feps = (double)EPS;

	ab = a + b;
	ap = a + 1.f;
	am = a - 1.f;
	c = 1.f;
	d = 1.f - ab*x / ap;
	if (fabs(d) < fpmin) d = fpmin;
	d = 1.f / d;
	h = d;

	for (m = 1; m <= MAXIT; m++){
		m2 = 2 * m;
		aa = m * (b - m) * x / ((am + m2)*(a + m2));
		d = 1.f + aa * d;
		if (fabs(d) < fpmin) d = fpmin;
		c = 1.f + aa / c;
		if (fabs(c) < fpmin) c = fpmin;
		d = 1.f / d;
		h *= d*c;
		aa = -(a + m)*(ab + m)*x / ((a + m2)*(ap + m2));
		d = 1.f + aa * d;
		if (fabs(d) < fpmin) d = fpmin;
		c = 1.f + aa / c;
		if (fabs(c) < fpmin) c = fpmin;
		d = 1.f / d;
		del = d * c;
		h *= del;
		if (fabs(del - 1.f) <= feps) break;
	}

	//if (m > MAXIT) LOG("EXCEPTION", "Exceed MAXIT.");
	return h;

}

void amoeba(double* sample, int n, double p[5][4], double y[], double ftol, int *nfunk)
{
	//LOG("BD", "amoeba()");
	int ndim = 4;
	int i, ihi, ilo, inhi, j, mpts = ndim + 1;
	double rtol, sum, swap, ysave, ytry, *psum;

	psum = (double*)malloc(sizeof(double)*ndim);
	*nfunk = 0;
	GET_PSUM;
	for (;;){
		ilo = 0;
		ihi = y[0] > y[1] ? (inhi = 1, 0) : (inhi = 0, 1);
		for (i = 0; i < mpts; i++){
			if (y[i] <= y[ilo]) ilo = i;
			if (y[i] > y[ihi]){
				inhi = ihi;
				ihi = i;
			}
			else if (y[i] > y[inhi] && i != ihi) inhi = i;
		}

		rtol = 2.f * fabs(y[ihi] - y[ilo]) / (fabs(y[ihi]) + fabs(y[ilo]) + (double)TINY);

		if ( rtol < EPS  || *nfunk >= NMAX){
			SWAP(y[0], y[ilo]);
			for (i = 0; i < ndim; i++) SWAP(p[0][i], p[ilo][i]);
			break;
		}

		*nfunk += 2;

		ytry = amotry(sample, n, p, y, psum, ihi, -1.f);

		if (ytry <= y[ilo])
			ytry = amotry(sample, n, p, y, psum, ihi, 2.0);
		else if (ytry >= y[inhi]){
			ysave = y[ihi];
			ytry = amotry(sample, n, p, y, psum, ihi, 0.5);
			if (ytry >= ysave){
				for (i = 0; i < mpts; i++){
					if (i != ilo){
						for (j = 0; j < ndim; j++)
							p[i][j] = psum[j] = 0.5f*(p[i][j] + p[ilo][j]);
						y[i] = getDWLS(sample, n, psum);
					}
				}
				*nfunk += ndim;
				GET_PSUM;
			}
		}
		else --(*nfunk);
	}

	delete[] psum;
	//LOG("Iteration", *nfunk);
}

double amotry(double* sample, int n, double p[5][4], double y[], double psum[], int ihi, double fac)
{
	//LOG("BD", "amotry()");
	int ndim = 4;
	int j;
	double fac1, fac2, ytry, *ptry;

	ptry = (double*)malloc(sizeof(double)*ndim);
	fac1 = (1.f - fac) / ndim;
	fac2 = fac1 - fac;
	for (j = 0; j < ndim; j++) ptry[j] = psum[j] * fac1 - p[ihi][j] * fac2;
	ytry = getDWLS(sample, n, ptry);
	if (ytry < y[ihi]){
		y[ihi] = ytry;
		for (j = 0; j < ndim; j++){
			psum[j] += ptry[j] - p[ihi][j];
			p[ihi][j] = ptry[j];
		}
	}
	delete[] ptry;
	return ytry;
}