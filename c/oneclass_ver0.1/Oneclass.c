#pragma once
#include "Oneclass.h"
#include "Utility.h"
/* declare structures */

int partition(record_t* a, int n, double pivot)
{
	int i = 0, j = n - 1, tmp1;
	double tmp2;

	while (i <= j)
	{
		while ((*(a + i)).value < pivot) i++;
		while ((*(a + j)).value >= pivot) j--;

		if (i < j)
		{
			tmp1 = (*(a + i)).index;
			tmp2 = (*(a + i)).value;
			(*(a + i)).index = (*(a + j)).index;
			(*(a + i)).value = (*(a + j)).value;
			(*(a + j)).index = tmp1;
			(*(a + j)).value = tmp2;
			i++; j--;
		}
	}

	return i;
}

int findpivot(record_t* a, int n, double* pivot_ptr)
{
	int i;

	for (i = 1; i < n; i++)
		if ((*a).value != (*(a + i)).value)
		{
			*pivot_ptr = ((*a).value >(*(a + i)).value) ? (*a).value : (*(a + i)).value;
			return 1;
		}

	return 0;
}

void quicksort(record_t* a, int n)
{
	int pivotIndex;
	double pivot;

	if (findpivot(a, n, &pivot) != 0)
	{
		pivotIndex = partition(a, n, pivot);
		quicksort(a, pivotIndex);
		quicksort(a + pivotIndex, n - pivotIndex);
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

/* 필요한 파일 경로를 얻습니다.  */
char* get_data_path_by_type(char typeCounter, int foldCounter, int classCounter)
{
	const char trainingDataPath[15] = "data/train_", testDataPath[15] = "data/test_";
	int fc = 0, cc = 0;
	static char path[15];
	int i;
	char file_idx[2];

	for (i = 0; i < 15; path[i++] = '\0');

	strcpy(path, typeCounter == DATATYPE_TRAIN ? trainingDataPath : testDataPath);
	sprintf(file_idx, "%d", foldCounter);	strcat(path, file_idx);
	strcat(path, "_");
	sprintf(file_idx, "%d", classCounter);	strcat(path, file_idx);

	return path;
}

/*data의 record 크기를 구합니다.
typeCounter_: data의 type을 결정합니다. DATATYPE_TRAIN은 training data, DATATYPE_TEST는 test data를 의미합니다.
foldCounter_: fold의 차례 번호를 받습니다. path를 결정하는 데에 사용합니다.
classCounter_: class 번호를 받습니다. path를 결정하는 데에 사용합니다.
dim: 차원의 크기를 확인하는 데에 사용합니다.
*/
int read_data_record_size(char typeCounter_, int foldCounter_, int classCounter_, int dim)
{
	int n, j;
	double x;
	FILE *fp;
	char line[255];

	fp = fopen(get_data_path_by_type(typeCounter_, foldCounter_, classCounter_), "r");
	if (fp == NULL)
	{
		printf("File Open Error\n");
		exit(-1);
	}

	for (n = 0; fscanf(fp, "%[^\n]%*c", &line) != EOF; n++);

	close(fp);
	return n;
}
/*data의 dimension 크기를 구합니다.
typeCounter_: data의 type을 결정합니다. DATATYPE_TRAIN은 training data, DATATYPE_TEST는 test data를 의미합니다.
foldCounter_: fold의 차례 번호를 받습니다. path를 결정하는 데에 사용합니다.
classCounter_: class 번호를 받습니다. path를 결정하는 데에 사용합니다.
*/
int read_data_dim_size(char typeCounter_, int foldCounter_, int classCounter_)
{
	int n, i;
	FILE *fp;
	char x = 0;

	fp = fopen(get_data_path_by_type(typeCounter_, foldCounter_, classCounter_), "r");
	if (fp == NULL)
	{
		printf("File Open Error\n");
		exit(-1);
	}

	for (n = 0; x != '\n'; x == ' ' ? n++ : n)
		fscanf(fp, "%c", &x);

	return n;
}

bool readData(char* filepath, int dim, int record_size, data_t* _data)
{
	int i, j;
	FILE* fp = NULL;

	fp = fopen(filepath, "r");

	if (fp == NULL)
	{
		printf("FILE OPEN ERROR : %s\n", filepath);
		return false;
	}

	for (i = 0; i < record_size; i++)
		for (j = 0; j < dim; j++)
			fscanf(fp, "%lf", &_data[i].raw[j]);

	if (i != record_size || j != dim)
	{
		printf("DATA SIZE ERROR.\n");
		return false;
	}

	close(fp);

	return true;
}

void mountData(oneclass_t *db)
{
	int i, j, k;

	if (db == NULL) return;
	
	printf("\nMOUNT DATA...\n");
	for (i = 0; i < TENFOLD; i++)
		for (j = 0; j < CLASSSIZE; j++)
		{
			printf("Fold=%d, Class=%d,", i, j);
			for (k = 0; k < TRAIN_TEST; putchar(++k == TRAIN_TEST ? '\n' : ' '))
			{
				printf(" %s data mount ", k == TRAIN ? "train" : "test"); 
				if (readData(get_data_path_by_type(k, i, j), db->dimsize, 
					db->table_dataparam[i][j][k].datasize, db->table_dataparam[i][j][k].data))
					printf("[OK]");
				else
					printf("[FAIL]");
				setDimentionRange(i, j, k, db);
			}
		}
	printf("[COMPLETE]\n");
}

int readSeedRaw(oneclass_t *db)
{
	FILE *rrd;
	char line[256];
	int i, j, k, l, m, n;

	db->isRRD = 0;
	if ((rrd = fopen("randrawdmp.csv", "r")) == NULL)
	{
		printf("The dump file of random raw for cluster does not exist. set Seed Raw up random.\n");
		return 1;
	}

	for (n = 0; fscanf(rrd, "%[^\n]%*c", &line) != EOF; n++);
	if (n != TENFOLD * CLASSSIZE * db->ctrdsize)
	{
		printf("the size of centroid does not same.\n");
		return 2;
	}
	rrd = freopen("randrawdmp.csv", "r", rrd);
	for (i = 0; i < TENFOLD; i++)
		for (j = 0; j < CLASSSIZE; j++)
			for (l = 0; l < db->ctrdsize; l++)
				for (m = 0; m < db->dimsize;m++)
					fscanf(rrd, "%lf,", &db->table_centroid[i][j][0][l].seedRaw[m]);

	close(rrd);
	db->isRRD = 1;
	return 0;
}

void setDimentionRange(int f, int c, int t, oneclass_t* db)
{
	static int i, j;
	static double x, y; 
	dimparam_t* table_dim = db->table_dimparam[f][c][t];
	int datasize = db->table_dataparam[f][c][t].datasize;
	data_t* table_data = db->table_dataparam[f][c][t].data;

	if (db == NULL) return;

	/* calculating the variances of data */
	for (j = 0; j < db->dimsize; j++)
	{
		for (x = 0., i = 0; i < datasize; i++)
			x += table_data[i].raw[j];
		x /= datasize;

		for (y = 0., i = 0; i < datasize; i++)
			y += vdistance(table_data[i].raw[j], x);
		y /= (datasize - 1);//sample이기 때문에
		table_dim[j].mean = x;
		table_dim[j].variance = y;
	}
}


/*로그 파일을 준비합니다. 만약 로그파일이 없다면 로그 파일을 생성합니다.
main()함수 상에서 하나의 함수가 끝날 때마다 지금까지 작성한 내용을 저장하고 다시 파일을 엽니다.
*/
void readyFoldLogFile(int f, oneclass_t *db)
{
	char fn[32];
	sprintf(fn, ".//log//%s_t%02d.log", db->clfs, f);
	db->flf[f] = db->flf[f] == NULL ? fopen(fn, "w") : freopen(fn, "a", db->flf[f]);
}

void openDatabase(oneclass_t *db)
{
	int i, j, k, h;
	char clfs[16];
	struct tm *t;
	time_t timer;

	if (db == NULL) return;
	time(&timer);
	t = localtime(&timer);

	sprintf(clfs, "%d%02d%02d%02d%02d%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	debug = fopen("dbg.log", "a");

	printf("OPEN DATABASE...");

	db->table_dataparam = (dataparam_t***)calloc(TENFOLD, sizeof(dataparam_t**));
	db->table_centroid = (centroid_t****)calloc(TENFOLD, sizeof(centroid_t***));
	db->table_dimparam = (dimparam_t****)calloc(TENFOLD, sizeof(dimparam_t***));
	strcpy(db->clfs, clfs);
	db->flf = (FILE**)calloc(TENFOLD, sizeof(FILE*));

	for (i = 0; i < TENFOLD; i++)
	{
		db->table_dataparam[i] = (dataparam_t**)calloc(CLASSSIZE, sizeof(dataparam_t*));
		db->table_centroid[i] = (centroid_t***)calloc(CLASSSIZE, sizeof(centroid_t**));
		db->table_dimparam[i]= (dimparam_t***)calloc(CLASSSIZE, sizeof(dimparam_t**));

		for (j = 0; j < CLASSSIZE; j++)
		{
			db->table_dataparam[i][j] = (dataparam_t*)calloc(TRAIN_TEST, sizeof(dataparam_t));
			db->table_centroid[i][j] = (centroid_t**)calloc(TRAIN_TEST, sizeof(centroid_t*));
			db->table_dimparam[i][j] = (dimparam_t**)calloc(TRAIN_TEST, sizeof(dimparam_t*));

			for (k = 0; k < TRAIN_TEST; k++)
			{
				db->table_dataparam[i][j][k].datasize = read_data_record_size(k, i, j, db->dimsize);
				db->table_dataparam[i][j][k].data = (data_t*)calloc(db->table_dataparam[i][j][k].datasize, sizeof(data_t));
				db->table_dataparam[i][j][k].cdf = (double*)calloc(db->table_dataparam[i][j][k].datasize, sizeof(double));
				db->table_dataparam[i][j][k].cmp = (double*)calloc(db->table_dataparam[i][j][k].datasize, sizeof(double));
				db->table_dataparam[i][j][k].pdf_fs = (record_t*)calloc(db->table_dataparam[i][j][k].datasize, sizeof(record_t));
				db->table_dataparam[i][j][k].pdf_tr = (double*)calloc(db->table_dataparam[i][j][k].datasize, sizeof(double));
				db->table_dataparam[i][j][k].discriminant = (int**)calloc(CLASSSIZE, sizeof(int*));
				db->table_dataparam[i][j][k].dimsize = db->dimsize;
				db->table_dataparam[i][j][k].first = &db->table_dataparam[i][j][TRAIN];

				db->table_centroid[i][j][k] = (centroid_t*)calloc(db->ctrdsize, sizeof(centroid_t));
				db->table_dimparam[i][j][k] = (dimparam_t*)calloc(db->dimsize, sizeof(dimparam_t));

				for (h = 0; h < db->table_dataparam[i][j][k].datasize; h++)
					db->table_dataparam[i][j][k].data[h].raw = (double*)calloc(db->dimsize, sizeof(double));
				for (h = 0; h < CLASSSIZE; h++)
					db->table_dataparam[i][j][k].discriminant[h] = (int*)calloc(CLASSSIZE, sizeof(int));

				for (h = 0; h < (*db).ctrdsize; h++)
				{
					db->table_centroid[i][j][k][h].raw = (double*)calloc(db->dimsize, sizeof(double));
					db->table_centroid[i][j][k][h].dimweight = (double*)calloc(db->dimsize, sizeof(double));
					db->table_centroid[i][j][k][h].dimparam = (dimparam_t*)calloc(db->dimsize, sizeof(dimparam_t));
					db->table_centroid[i][j][k][h].cpon_range = (cpon_range_t*)calloc(CANDIDATE_SIZE, sizeof(cpon_range_t));
					db->table_centroid[i][j][k][h].pdfweight = (double*)calloc(CANDIDATE_SIZE, sizeof(double));
					db->table_centroid[i][j][k][h].dimsize = db->dimsize;
					db->table_centroid[i][j][k][h].first = &(db->table_centroid)[i][j][TRAIN][0];
				}
			}
			h = db->table_dataparam[i][j][TRAIN].datasize > db->table_dataparam[i][j][TEST].datasize ? 
				db->table_dataparam[i][j][TRAIN].datasize : db->table_dataparam[i][j][TEST].datasize;
			db->table_centroid[i][j][TRAIN][0].betadata_a = (double*)calloc(h, sizeof(double));
			db->table_centroid[i][j][TRAIN][0].betadata_b = (double*)calloc(h, sizeof(double));

			for (h = 0; h < db->ctrdsize; h++)
				db->table_centroid[i][j][TRAIN][h].seedRaw = (double*)calloc(db->dimsize, sizeof(double));
		}
	}
	printf("[COMPLETE]\n");
}

void closeDatabase(oneclass_t *db)
{
	int i, j, k, h;

	if (db == NULL) return;

	printf("vacuuming data for program finish...");
	for (i = 0; i < TENFOLD; i++)
	{
		for (j = 0; j < CLASSSIZE; j++)
		{
			free(db->table_centroid[i][j][TRAIN][0].betadata_a);
			free(db->table_centroid[i][j][TRAIN][0].betadata_b);

			for (h = 0; h < db->ctrdsize; h++)
				free(db->table_centroid[i][j][TRAIN][h].seedRaw);

			for (k = 0; k < TRAIN_TEST; k++)
			{
				for (h = 0; h < db->table_dataparam[i][j][k].datasize; h++)
					free(db->table_dataparam[i][j][k].data[h].raw);
				for (h = 0; h < CLASSSIZE; h++)
					free(db->table_dataparam[i][j][k].discriminant[h]);

				for (h = 0; h < db->ctrdsize; h++)
				{
					db->table_centroid[i][j][k][h].first = NULL;
					free(db->table_centroid[i][j][k][h].raw);
					free(db->table_centroid[i][j][k][h].dimweight);
					free(db->table_centroid[i][j][k][h].dimparam);
					free(db->table_centroid[i][j][k][h].cpon_range);
					free(db->table_centroid[i][j][k][h].pdfweight);
				}

				free(db->table_dataparam[i][j][k].data);
				free(db->table_dataparam[i][j][k].cdf);
				free(db->table_dataparam[i][j][k].cmp);
				free(db->table_dataparam[i][j][k].pdf_fs);
				free(db->table_dataparam[i][j][k].pdf_tr);
				free(db->table_dataparam[i][j][k].discriminant);

				free(db->table_centroid[i][j][k]);
				free(db->table_dimparam[i][j][k]);
			}

			free(db->table_dataparam[i][j]);
			free(db->table_centroid[i][j]);
			free(db->table_dimparam[i][j]);
		}
		free(db->table_dataparam[i]);
		free(db->table_centroid[i]);
		free(db->table_dimparam[i]);
		close(db->flf[i]);
	}
	free(db->flf);

	free(db->table_dataparam);
	free(db->table_centroid);
	free(db->table_dimparam);

	printf("[complete]\n");
}

int getLBGClusterSize(int f, int c, int ec, oneclass_t *db)
{
	int i, j, u;
	int closest_ctrd_i;
	centroid_t *centroid = db->table_centroid[f][c][0];
	dimparam_t *dimparam = db->table_dimparam[f][c][0];
	dataparam_t *dataparam = &db->table_dataparam[f][c][0];

	centroid->var_max = 0.;//기존 프로그램에서는 호출될 때마다 초기화 되던데요?
	if (db->isRRD)
	{
		for (i = 0; i < ec; i++)
			for (j = 0; j < centroid->dimsize; j++)
				centroid[i].raw[j] = centroid[i].seedRaw[j];
	}
	else
	{
		srand(ec);
		for (i = 0; i < ec; i++)
			for (j = 0; j < centroid->dimsize; j++)
				centroid[i].raw[j] = centroid[i].seedRaw[j] = sqrt(dimparam[j].variance) * (2.0 * rand() / RAND_MAX - 1.0) + dimparam[j].mean;
	}
	//centroid의 dimension parameter들 초기화 해줍니다...이거땜시 나흘을 버렸어!!!
	for (i = 0; i < ec; i++)
		for (j = 0; j < centroid->dimsize; j++)
			centroid[i].dimparam[j].mean = centroid[i].dimparam[j].variance = 0.;


	for (u = 0; u < CLUSTERING_COUNT; u++)
	{
		for (i = 0; i < ec; i++)
		{
			centroid[i].datasize = 0;
			for (j = 0; j < dataparam->dimsize; j++)
				centroid[i].dimweight[j] = 0.;
		}
		
		for (i = 0; i < dataparam->datasize; i++)
		{
			closest_ctrd_i = find_closest_centroid_index(ec, i, centroid, dataparam->data);
			for (j = 0; j < centroid->dimsize; j++)
				centroid[closest_ctrd_i].dimweight[j] += dataparam->data[i].raw[j];

			((*(centroid + closest_ctrd_i)).datasize)++;
		}

		for (i = 0; i < ec; i++)
			for (j = 0; j < dataparam->dimsize; j++)
			{
				centroid[i].raw[j] = centroid[i].dimweight[j];

				if (centroid[i].datasize > CNTR_IGNORE_SIZE)
					centroid[i].raw[j] /= centroid[i].datasize;
			}
	}
	fprintf(db->flf[f], "centroid size : %d\n", ec);
	fprintf(db->flf[f], "ctrd no.\tdatasize\tworktype\t");
	for (i = 0; i < db->dimsize; fprintf(db->flf[f], "raw[%d]%c", i++, db->dimsize - i == 1 ? '\n' : '\t'));
	//cluster 결과를 log로 남기기 위한 코드입니다.
	for (i = 0; i < ec; i++)
	{
		fprintf(db->flf[f], "\t\tseed\t");
		for (j = 0; j < db->dimsize; fprintf(db->flf[f], "%lf%c", centroid[i].seedRaw[j++], db->dimsize - j == 1? '\n' : '\t'));
		fprintf(db->flf[f], "%d\t%d\tclst\t", i, centroid[i].datasize);
		for (j = 0; j < db->dimsize; fprintf(db->flf[f], "%lf%c", centroid[i].raw[j++], db->dimsize - j == 1 ? '\n' : '\t'));
	}

	//set dimension parameter of centroids
	for (i = 0; i < dataparam->datasize; i++)
	{
		closest_ctrd_i = find_closest_centroid_index(ec, i, centroid, dataparam->data);
		for (j = 0; j < dataparam->dimsize; j++)
		{
			centroid[closest_ctrd_i].dimparam[j].variance += vdistance(dataparam->data[i].raw[j], centroid[closest_ctrd_i].raw[j]);
			centroid[closest_ctrd_i].dimparam[j].mean += dataparam->data[i].raw[j];
		}
	}
	for (i = 0; i < ec; i++)
	{
		if ((*(centroid + i)).datasize > CNTR_IGNORE_SIZE)
		{
			for (j = 0; j < dataparam->dimsize; j++)
			{
				centroid[i].dimparam[j].variance /= (centroid[i].datasize - 1);
				centroid[i].dimparam[j].mean /= centroid[i].datasize;
			}
		}
	}

	for (i = 0; i < ec; i++)
	{
		for (j = 0; j < dataparam->dimsize; j++)
		{ 
			if (centroid[i].datasize > CNTR_IGNORE_SIZE && centroid[i].dimparam[j].variance > centroid->var_max)
				centroid->var_max = centroid[i].dimparam[j].variance;//varmax is share variable
		}
	}

	for (i = 0, j = 0; i < ec; i++)
		if (centroid[i].datasize > CNTR_IGNORE_SIZE) j++;

//	for (i = 0; i < ec; i++)
//		printf("%d:%d, ", i, (*(centroid + i)).datasize);

	return j;
}

int find_closest_centroid_index(int ec, int dc, centroid_t *ctrd, data_t *data)
{
	int i, j, idx = 0;
	double mv = 1000., tmp;

	for (i = 0; i < ec; i++)
	{
		for (j = 0, tmp = 0.; j < ctrd->dimsize; j++)
			tmp += vdistance(ctrd[i].raw[j], data[dc].raw[j]);
		
		if (tmp < mv)
			mv = tmp, idx = i;
	}
	return idx;
}

int trainOneClass(int f, int c, int ec, oneclass_t *db)
{
	int i;
	int clusterIndex;
	double x, y;

	centroid_t *centroid = db->table_centroid[f][c][TRAIN];
	dimparam_t *dimparam = db->table_dimparam[f][c][TRAIN];
	dataparam_t *dataparam = &db->table_dataparam[f][c][TRAIN];
	
	/*CAUTION
	본문에서 사용된 centroid_weight와 train_one_class에서의 weight는 공유할 것입니다.
	*/
	/* clustering: finding centroids */
	fprintf(db->flf[f], "[class #%d] <getLBGClusterSize>\n", c);
	clusterIndex = getLBGClusterSize(f, c, ec + 1, db);

	fprintf(db->flf[f], "[class #%d] <for of setCPON>\n", c);
	//겹치지 않도록, 초기에 설정한 centroid index에 따라 candidated cpon range를 저장해야 합니다.
	for (i = 0, x = 0.; i < CANDIDATE_SIZE; i++)
	{
		//이건 set cpon이죠
		//centroid[ec].cpon_range[i].sigma = sqrt(centroid->var_max) * exp((i - 2.0) * log(2.0));

		setCPON(f, c, TRAIN, ec, clusterIndex - 1, i, db);
		fprintf(debug, "%d,%d,%d,%d,%d,%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", 
			f, c, TRAIN, ec, i, (((centroid + ec)->cpon_range) + i)->no_centroids, (((centroid + ec)->cpon_range) + i)->sigma,
			*((centroid + ec)->pdfweight + i), (((centroid + ec)->cpon_range) + i)->beta_a, (((centroid + ec)->cpon_range) + i)->beta_b, 
			(((centroid + ec)->cpon_range) + i)->beta_a_bias, (((centroid + ec)->cpon_range) + i)->beta_b_bias, 
			(((centroid + ec)->cpon_range) + i)->beta_a_variance, (((centroid + ec)->cpon_range) + i)->beta_b_variance, 
			(((centroid + ec)->cpon_range) + i)->p_value_train, (*(*((db->table_dataparam) + f) + c))->y_min, 
			(*(*((db->table_dataparam) + f) + c))->y_max);
		
		if (x < (y = centroid[ec].cpon_range[i].p_value_train))
		{
			x = y;
			centroid[ec].cponindex = i;//todo 이게 과연 필요한 지 확인할 것.
			//선정된 beta를 공용 beta로 저장합니다.
			*getBetaA(centroid) = centroid[ec].cpon_range[i].beta_a;
			*getBetaB(centroid) = centroid[ec].cpon_range[i].beta_b;
		}
	}

	/*** evaluation of the selected CPON ***/
	/* clustering for the given value of clusterSize */
	/*
	This function is different to last version.
	The last version has one more calling the function getLBGClusterSize,
	but this program doesn't call again.
	*/

	return clusterIndex - 1;
}

/*set cpon
get cpon range from centroid, dimension parameters, and data parameters.
*/
void setCPON(int f, int c, int t, int ec, int clst_c, int l, oneclass_t *db)
{
	centroid_t *centroid = db->table_centroid[f][c][t];
	dimparam_t *dimparam = db->table_dimparam[f][c][t];
	dataparam_t *dataparam = &db->table_dataparam[f][c][t];
	int datasize = dataparam->datasize;

	//set no_centroids
	centroid[clst_c].cpon_range[l].no_centroids = clst_c + 1;
	//set up sigma in the last centroid
	centroid[ec].cpon_range[l].sigma = sqrt(centroid->var_max) * (exp((l - 2.0) * log(2.0)));
	//set pdfweight being centroid and pdf_tr being dataparam
	setPDF(ec, clst_c + 1, l, centroid, dataparam);
	//set feature scaled pdf in the dataparam(it includes getta min and max of pdf)
	setFeatureScaledPDF(dataparam);
	//printf("%lf, %lf\n", (*(*((db->table_dataparam) + f) + c))->y_min, (*(*((db->table_dataparam) + f) + c))->y_max);
	//set beta parameters belonging to cpon range
	setCPONRangeBetaParams(l, &centroid[clst_c], dataparam);
	//set p value of train
	centroid[clst_c].cpon_range[l].p_value_train = 
		p_value_train(datasize, dataparam->cdf, dataparam->cmp, centroid[clst_c].cpon_range[l].beta_a, centroid[clst_c].cpon_range[l].beta_b, dataparam->pdf_fs);
}

/*각 centroid와 모든 data간의 euclidean distance를 계산해서 
각 centroid의 pdfweight 와 cpon_range
각 data의 pdf에 저장*/
void setPDF(int ec, int clst_s, int cdd_c, centroid_t *ctrd, dataparam_t *dataparam)
{
	int i, j, k;
	double x;

	for (i = 0; i < clst_s; i++)
		ctrd[i].pdfweight[cdd_c] = 1.0 / (clst_s * exp(dataparam->dimsize * log(ctrd[ec].cpon_range[cdd_c].sigma * sqrt(2.0 * PI))));

	for (i = 0; i < dataparam->datasize; i++)
	{
		dataparam->pdf_tr[i] = 0.;
		for (j = 0; j < clst_s; j++)
		{
			x = 0.;
			for (k = 0; k < ctrd->dimsize; k++)
				x += vdistance(dataparam->data[i].raw[k], ctrd[j].raw[k]);

			dataparam->pdf_tr[i] += ctrd[j].pdfweight[cdd_c] * 
				exp(-x / (2.0 * ctrd[j].cpon_range[cdd_c].sigma * ctrd[j].cpon_range[cdd_c].sigma));
//			printf("%lf %lf is %s\n", *(((ctrd + j)->pdfweight) + cdd_c), ctrd[j].pdfweight[cdd_c], *(((ctrd + j)->pdfweight) + cdd_c) == ctrd[j].pdfweight[cdd_c] ? "same" : "diffrent");
		}
	}
}

/* sample mean and variance of normalized output */
//deperated
void sample_mean_variance(int a, double* _s_mean, double* _s_var, record_t* _y_nor)
{
	int i;
	double mean, var;

	for (i = 0, mean = 0.0; i < a; i++)
		mean += _y_nor[i].value;

	mean /= a;
	for (i = 0, var = 0.0; i < a; i++)
		var += vdistance(_y_nor[i].value, mean);

	var /= (a - 1);
	*_s_mean = mean;
	*_s_var = var;
}

/* parameter estimation using the moment matching method */
//deperated
void parameter_beta_mm(double mean, double var, centroid_t *centroid)
{
	double y = mean * (1 - mean) / var - 1.0;

	*(getBetaA(centroid)) = mean * y;
	*(getBetaB(centroid)) = (1 - mean) * y;
}

//deperated
void parameter_beta_mm_jk(int a, int b, double s_mean_, double s_var_, record_t* _y_nor, centroid_t *centroid)
{
	double x, mean, var, y;
	
	x = s_var_ * (a - 1) + a * s_mean_ * s_mean_ - _y_nor[b].value * _y_nor[b].value;
	mean = (s_mean_ * a - _y_nor[b].value) / (a - 1);
	var = (x - (a - 1) * mean * mean) / (a - 2);
	y = mean * (1 - mean) / var - 1.0;

	*(getBetaA(centroid)) = mean * y;
	*(getBetaB(centroid)) = (1 - mean) * y;
}

double p_value_train(int a, double* __y_cdf, double* __y_cmp, double beta_a, double beta_b, record_t* __y_nor)
{

	return KS_test(getLInfinityNorm(a, __y_cdf, __y_cmp, __y_nor, beta_a, beta_b), a);
}

double getLInfinityNorm(int a, double* _y_cdf, double* _y_cmp, record_t* _y_nor, double beta_a, double beta_b)
{
	int i;
	double x, z, beta_f;
	double max_dist;

	/* cdf values of beta dist */
	beta_f = integrate(1.0e-8, 1.0 - 1.0e-8, 2, 0.002, beta_a, beta_b);

	for (i = 0; i < a; i++)
	{
		x = _y_nor[i].value;
		z = x < 1.0e-8 ? z = 0.0 : x > 1 - 1.0e-8 ? z = beta_f : integrate(1.0e-8, x, 2, 0.002, beta_a, beta_b);
		_y_cdf[i] = z / beta_f;
	}

	/* determining the maximum distance between the theoretical and empirical dist.s */
	for (i = 0; i < a; i++)
		_y_cmp[i] = fabs((double)i / (a - 1.0) - _y_cdf[i]);

	for (max_dist = 0.0, i = 0; i < a; i++)
		if (_y_cmp[i] > max_dist)
			max_dist = _y_cmp[i];

	return max_dist;
}

double* getBetaA(centroid_t *this){ return &(this->first->beta_a); }
double* getBetaB(centroid_t *this){ return &(this->first->beta_b); }

double getSampleMean(int recordsize, record_t* r)
{
	int i;
	double m = 0.;

	for (i = 0; i < recordsize; i++) m += r[i].value;

	return m / recordsize;
}

double getSampleVariance(double mean, int recordsize, record_t* r)
{
	int i;
	double v = 0.;

	for (i = 0; i < recordsize; i++) v += vdistance(r[i].value, mean);

	return v / (recordsize - 1);
}

void setFeatureScaledPDF(dataparam_t *dataparam)
{
	int i, j, datasize = dataparam->datasize;

	//cdf를 구하기 위해서(인지 정확히는 잘 모르지만) normalized data들을 크기에 대한 오름차순으로 정렬합니다.
	for (i = 0; i < datasize; i++)
	{
		dataparam->pdf_fs[i].index = i;
		dataparam->pdf_fs[i].value = dataparam->pdf_tr[i];
	}
	quicksort(dataparam->pdf_fs, datasize);

	dataparam->y_min = dataparam->pdf_tr[dataparam->pdf_fs[0].index];
	dataparam->y_max = dataparam->pdf_tr[dataparam->pdf_fs[dataparam->datasize - 1].index];
	//	fprintf(debug, "%d,%d,%d,%lf,%lf\n", f, c, t, db->table_dataparam[f][c]->y_min, db->table_dataparam[f][c]->y_max);

	for (i = 0; i < datasize; i++)
	{
		j = dataparam->pdf_fs[i].index;
		dataparam->pdf_fs[i].value = feature_scaling(dataparam->pdf_tr[j], dataparam->y_min, dataparam->y_max);
	}
}

void getCPONPowerRatio(int f, int c, int t, int ec, oneclass_t *db)
{
	int i, j;
	double x, beta, z, pnew;
	dataparam_t* dataparam = &db->table_dataparam[f][c][t];
	centroid_t *centroid = &db->table_centroid[f][c][t][ec - 1];
	int datasize = dataparam->datasize;
	
	/* hypothesis testing of one-class problems for train data */
	//clear cdf
	for (i = 0; i < dataparam->datasize; dataparam->cdf[i++] = 0.);

	beta = integrate(INTEGRATE_FROM, INTEGRATE_TO, 2, INTEGRATE_TOLERANCE, *getBetaA(centroid), *getBetaB(centroid));
	for (i = 0; i < dataparam->datasize; i++)
	{
		j = dataparam->pdf_fs[i].index;
		x = feature_scaling(dataparam->pdf_tr[j], dataparam->first->y_min, dataparam->first->y_max);

		if (x <= 1.0e-8)
			x = 1.0e-8;
		else if (x >= 1.0 - 1.0e-8)
			x = 1.0 - 1.0e-8;
		z = integrate(INTEGRATE_FROM, x, 2, INTEGRATE_TOLERANCE, *getBetaA(centroid), *getBetaB(centroid));
		pnew = z / beta;
		dataparam->cdf[i] = pnew;
	}
	//printf("[%d]{beta=%lf, x=%lf, z=%lf}", ec, beta, x, z);

	/* hypothesis testing of one-class problems for test data */
		/* CPON output */
	setPDF(ec, datasize, centroid->cponindex, centroid, dataparam);
		/* normalization of CPON output */
		/* number of positive data for cdf_t */
		/* calculation of cdf_t */
	/* calculation of the p-value of K-S statistic */
}

void printCentroid(centroid_t* c, int no)
{
	int i;

	printf("[centroid No : %2d]\n", no);
	printf("<single values>\n");
	printf("int = {datasize : %d, dimsize : %d, cpon index : %d}\n", c->datasize, c->dimsize, c->cponindex);
	printf("double = {var max : %8.4lf, beta_a : %8.4lf, beta_b : %8.4lf}\n", c->var_max, c->beta_a, c->beta_b);
	printf("<tables in order dimension>\nindex\t   raw\t\tdimweight\tmean\t\tvariance\n");
	for (i = 0; i < c->dimsize; i++)
		printf("[%d]\t%8.4lf\t%8.4lf\t%8.4lf\t%8.4lf\n", i, c->raw[i], c->dimweight[i], c->dimparam[i].mean, c->dimparam[i].variance);
	printf("<tables in candidate>\nindex\tpdfweight\tctrd_s\t   sigma\tweight\t\tbeta_a\t\ta_var\t\ta_bias\t\tbeta_b\t\tb_var\t\tb_bias\t\tpv_tr\t\tpv_tt\n");
	for (i = 0; i < CANDIDATE_SIZE; i++)
		printf("[%d]\t%8.4lf\t%3d\t%8.4lf\t%8.4lf\t%8.4lf\t%8.4lf\t%8.4lf\t%8.4lf\t%8.4lf\t%8.4lf\t%8.4lf\t%8.4lf\n", i, c->dimweight[i], c->cpon_range[i].no_centroids, c->cpon_range[i].sigma, c->cpon_range[i].weight, c->cpon_range[i].beta_a, c->cpon_range[i].beta_a_variance, c->cpon_range[i].beta_a_bias, c->cpon_range[i].beta_b, c->cpon_range[i].beta_b_variance, c->cpon_range[i].beta_b_bias, c->cpon_range[i].p_value_train, c->cpon_range[i].p_value_trts);
}

void printDataparam(dataparam_t *d)
{
	int i, j;
	printf("[dataparam]\n");
	printf("<single values>\n");
	printf("int = {datasize : %d, dimsize : %d}\n", d->datasize, d->dimsize);
	printf("double = {y_min = %8.4lf, y_max = %8.4lf}\n", d->y_min, d->y_max);
	printf("<data of tables in data size>\nindex\t");
	
	for (i = 0; i < d->dimsize; ++i == d->dimsize ? putchar('\n') : printf("\t\t"))
		printf("<%02d>", i);
	for(i = 0; i < d->datasize; i++)
	{
		printf("[%d]\t", i);
		for (j = 0; j < d->dimsize; putchar(++j == d->dimsize ? '\n' : '\t'))
			printf("%8.4lf", d->data[i].raw[j]);
	}
	printf("<data param tables in datasize>\nindex\tpdf\t\tnor\t\tcdf\t\tcmp\n");
	for (i = 0; i < d->datasize; i++)
		printf("[%d]\t%8.4lf\t%8.4lf\t%8.4lf\t%8.4lf\n", i, d->pdf_tr[i], d->pdf_fs[i], d->cdf[i], d->cmp[i]);
}

/*set beta parameters for CPON range exported from mean and variance
centroid는 정확히 현재 선택된 centroid를 넘겨줘야 합니다.(i.e. centroid[clst_c])
*/
void setCPONRangeBetaParams(int candi, centroid_t *centroid, dataparam_t *dataparam)
{
	double mean, var, jkMean, jkVar;
	double betaA, betaB, jkBetaA, jkBetaB;
	double a_var, b_var, jk;
	double x, y;
	int i;
	int datasize = dataparam->datasize;
	int samplesize = datasize - 1;

	//sample_mean_variance(datasize, &s_mean, &s_var, dataparam->nor);
	//아래의 코드가 실험상 더 빠릅니다.
	mean = getSampleMean(datasize, dataparam->pdf_fs);
	var = getSampleVariance(mean, datasize, dataparam->pdf_fs);
	setBetaShapeRange(mean, var, &betaA, &betaB);

	for (i = 0, x = 0., y = 0.; i < datasize; i++)
	{
		//Jackknife method
		jk = var * samplesize + datasize * mean * mean - dataparam->pdf_fs[i].value * dataparam->pdf_fs[i].value;
		jkMean = (mean * datasize - dataparam->pdf_fs[i].value) / samplesize;
		jkVar = (jk - samplesize * jkMean * jkMean) / (samplesize - 1);
		setBetaShapeRange(jkMean, jkVar, &jkBetaA, &jkBetaB);
		//뭔진 모르겠는데 jk랑 아닌 거랑 빼고 곱하고 함
		x += jkBetaA, y += jkBetaB;
		centroid->first->betadata_a[i] = datasize * betaA - samplesize * jkBetaA;
		centroid->first->betadata_b[i] = datasize * betaB - samplesize * jkBetaB;
	}
	//set beta
	centroid->cpon_range[candi].beta_a = jkBetaA;
	centroid->cpon_range[candi].beta_b = jkBetaB;
	//set beta bias
	centroid->cpon_range[candi].beta_a_bias = samplesize * ((x /= datasize) - betaA);
	centroid->cpon_range[candi].beta_b_bias = samplesize * ((y /= datasize) - betaB);
	//set beta variance
	for (i = 0, x = 0., y = 0.; i < datasize; i++)
	{
		x += centroid->first->betadata_a[i];
		y += centroid->first->betadata_b[i];
	}
	for (x /= datasize, y /= datasize, a_var = 0., b_var = 0., i = 0; i < datasize; i++)
	{
		a_var += vdistance(centroid->first->betadata_a[i], x);
		b_var += vdistance(centroid->first->betadata_b[i], y);
	}
	a_var /= datasize * samplesize;
	b_var /= datasize * samplesize;
	centroid->cpon_range[candi].beta_a_variance = a_var;
	centroid->cpon_range[candi].beta_b_variance = b_var;
}

void setBetaShapeRange(double mean, double var, double *betaA, double *betaB)
{
	double y = mean * (1 - mean) / var - 1.0;

	*betaA = mean * y;
	*betaB = (1 - mean) * y;
}

int optPresidentCPON(int f, int c, int t, int clst_c, oneclass_t *db)
{
	centroid_t *centroid = db->table_centroid[f][c][t];
	int presi_idx, j, presi_turn;
	double x, y, pwr_p, pwr_r;

	presi_idx = -1;	pwr_p = pwr_r = 0.0;

	for (presi_turn = 0; presi_turn < clst_c; presi_turn++)
	{
		j = centroid[presi_turn].cponindex;
		x = centroid[presi_turn].cpon_range[j].p_value_trts;
		y = centroid[presi_turn].cpon_range[j].p_value_train;

		if (pwr_p < y && pwr_r < y / (x + y))
		{
			presi_idx = presi_turn;
			pwr_p = y;
			pwr_r = y / (x + y);
		}
	}

	/* selecting the best cpon using the maximum p-value (K-S test, train) */
	if (pwr_p >= 1.0e-1)
		printf("Selection of the best cpon is done.\n");
	else
	{
		for (pwr_p = 0.0, presi_turn = 0; presi_turn < clst_c; presi_turn++)
		{
			x = centroid[clst_c].cpon_range[centroid[clst_c].cponindex].p_value_train;

			if (pwr_p < x)
			{
				presi_idx = presi_turn;
				pwr_p = x;
			}
		}
		printf("Selection of the best cpon using the p-value (K-S test, train) is done.\n");
	}

	return presi_idx;
}