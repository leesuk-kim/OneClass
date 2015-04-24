#include <stdio.h>
#include "invmat.h"

#define size 10

double ie[] = { 1, 4, 5, 7, 8, 4, 3, 7, 8, 4,
9, 4, 6, 7, 4, 2, 5, 6, 7, 4,
5, 4, 5, 9, 7, 8, 9, 7, 8, 8,
1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
2, 3, 2, 3, 2, 3, 2, 3, 4, 5,
7, 5, 6, 3, 5, 4, 3, 9, 8, 1,
1, 1, 3, 3, 4, 5, 3, 4, 2, 2,
4, 9, 5, 8, 6, 0, 3, 9, 2, 4,
5, 3, 4, 1, 6, 7, 5, 4, 3, 6,
6, 4, 7, 8, 5, 6, 7, 5, 6, 4 };

int main(){
	double **im, **om;
	int i, j, k = 0;

	allocSquareMatrix(&im, size);
	allocSquareMatrix(&om, size);

	//init
	for (i = 0; i < size; i++)
		for (j = 0; j < size; j++)
			im[i][j] = ie[k++];
			//im[i][j] = k++;
	pMatrix(&im, size, size);
	minverse(&im, size, &om);
	pMatrix(&om, size, size);
	for (i = 0; i < size; i++) free(im[i]), free(om[i]);
	free(im), free(om);
	return 0;
}