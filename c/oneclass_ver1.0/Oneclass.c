#pragma once
#include "Oneclass.h"
#include "Utility.h"
/* declare structures */

int partition(Record_t* a, int n, double pivot)
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

int findpivot(Record_t* a, int n, double* pivot_ptr)
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

void quicksort(Record_t* a, int n)
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
void get_data_path_by_type(char typeCounter, int foldCounter, int classCounter, char* path)
{
	const char trainingDataPath[] = "data/train", testDataPath[] = "data/test";
	int fc = 0, cc = 0;
	int i;

	for (i = 0; i < 15; path[i++] = '\0');

	sprintf(path, "%s_%d_%d", typeCounter == DATATYPE_TRAIN ? trainingDataPath : testDataPath, foldCounter, classCounter);
}

/*data의 record 크기를 구합니다.
typeCounter_: data의 type을 결정합니다. DATATYPE_TRAIN은 training data, DATATYPE_TEST는 test data를 의미합니다.
foldCounter_: fold의 차례 번호를 받습니다. path를 결정하는 데에 사용합니다.
classCounter_: class 번호를 받습니다. path를 결정하는 데에 사용합니다.
dim: 차원의 크기를 확인하는 데에 사용합니다.
*/
int read_data_record_size(char typeCounter_, int foldCounter_, int classCounter_, int dim)
{
	int n;
	FILE *fp;
	char* filepath = (char*)malloc(FILEPATH_LENGTH);

	get_data_path_by_type(typeCounter_, foldCounter_, classCounter_, filepath);
	fp = fopen(filepath, "r");
	if (fp == NULL)
	{
		printf("File Open Error\n");
		exit(-1);
	}

	for (n = 0; fscanf(fp, "%[^\n]%*c", filepath) != EOF; n++);
	close(fp);
	free(filepath);
	return n;
}
/*data의 dimension 크기를 구합니다.
typeCounter_: data의 type을 결정합니다. DATATYPE_TRAIN은 training data, DATATYPE_TEST는 test data를 의미합니다.
foldCounter_: fold의 차례 번호를 받습니다. path를 결정하는 데에 사용합니다.
classCounter_: class 번호를 받습니다. path를 결정하는 데에 사용합니다.
*/
int read_data_dim_size()
{
	int n;
	FILE *fp;
	char x = 0;
	char *filepath = (char*)malloc(FILEPATH_LENGTH);
	get_data_path_by_type(DATATYPE_TRAIN, 0, 0, filepath);

	fp = fopen(filepath, "r");
	if (fp == NULL)
	{
		printf("File Open Error\n");
		exit(-1);
	}

	for (n = 0; x != '\n'; x == ' ' ? n++ : n)
		fscanf(fp, "%c", &x);
	free(filepath);
	close(fp);
	return n;
}

bool readData(char* filepath, int dim, int record_size, Data_t* _data)
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
/*로그 파일을 준비합니다. 만약 로그파일이 없다면 로그 파일을 생성합니다.
main()함수 상에서 하나의 함수가 끝날 때마다 지금까지 작성한 내용을 저장하고 다시 파일을 엽니다.
*/
void readySampleSpaceLogFile(SampleSpace_t *spsc)
{
	char *filepath = (char*)malloc(FILEPATH_LENGTH);
	sprintf(filepath, "log//%s_f%02d.log", spsc->sCLF, spsc->id_fold);
	spsc->log = spsc->log == NULL ? fopen(filepath, "a") : freopen(filepath, "a", spsc->log);
	free(filepath);
}
void finishSampleSpaceLogFIle(SampleSpace_t *spsc)
{
	close(spsc->log);
}
void readyClassSpaceLogFile(ClassSpace_t *clsc)
{
	char *filepath = (char*)malloc(FILEPATH_LENGTH);
	sprintf(filepath, "log//%s_f%02dc%02d.log", clsc->sCLF, clsc->id_fold, clsc->id_cls);
	clsc->log = clsc->log == NULL ? fopen(filepath, "a") : freopen(filepath, "a", clsc->log);
	free(filepath);
}
void finishClassSpaceLogFIle(ClassSpace_t *clsc)
{
	close(clsc->log);
}

void openDatabase(OneClass_t *db)
{
	int i, j, h;
	int ds = db->dimsize, cs = db->clssize;
	Centroid_t *ctrd;
	SampleSpace_t *spsc;
	TrainSpace_t *trsc;
	TestSpace_t *ttsc;
	ClassSpace_t *clsc;
	fld_t *fld;

	struct tm *t;
	time_t timer;

	if (db == NULL) return;
	time(&timer);
	t = localtime(&timer);
	makeDir("log");
	sprintf(db->sclf, "%d%02d%02d%02d%02d%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	
	db->log = debug = fopen("dbg.log", "a");
	fprintf(debug, "<TRAINING LOG>\ndate : %d%02d%02d\nUnique ID : %s\n", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, db->sclf);
	printf("OPEN DATABASE...");

	db->vSampleSpace = (SampleSpace_t*)calloc(TENFOLD, sizeof(SampleSpace_t));
	db->isseed = 0;
	for (i = 0; i < TENFOLD; i++)
	{ 
		spsc = &db->vSampleSpace[i];

		spsc->id_fold = i;
		spsc->sCLF = &db->sclf[0];
		spsc->isSeed = &db->isseed;
		spsc->dimSize = &db->dimsize;
		spsc->clsSize = &db->clssize;
		spsc->ctrdSize = &db->ctrdsize;
		spsc->vClassSpace = (ClassSpace_t*)calloc(cs, sizeof(ClassSpace_t));
		spsc->mDiscriminant = (int**)calloc(cs, sizeof(int*));

		//////////////////////////////////////
		fld = &spsc->fld;
		fld->mWeight = (double**)calloc(cs, sizeof(double*));
		for (j = 0; j < cs; fld->mWeight[j++] = (double*)calloc(ds, sizeof(double)));
		fld->vEgien = (double*)calloc(cs, sizeof(double));
		fld->vProjMean = (double*)calloc(cs, sizeof(double));
		fld->vMean = (double*)calloc(ds, sizeof(double));
		fld->mMean = (double**)calloc(cs, sizeof(double*));
		for (j = 0; j < cs; fld->mMean[j++] = (double*)calloc(ds, sizeof(double)));
		fld->S = (double***)calloc(cs, sizeof(double**));
		for (j = 0; j < cs; j++)
		{
			fld->S[j] = (double**)calloc(ds, sizeof(double*));
			for (h = 0; h < ds; fld->S[j][h++] = (double*)calloc(ds, sizeof(double)));
		}
		fld->mSB = (double**)calloc(ds, sizeof(double*));
		for (j = 0; j < ds; fld->mSB[j++] = (double*)calloc(ds, sizeof(double)));
		fld->mSW = (double**)calloc(ds, sizeof(double*));
		for (j = 0; j < ds; fld->mSW[j++] = (double*)calloc(ds, sizeof(double)));
		fld->vProjS = (double*)calloc(cs, sizeof(double));

		//////////////////////////////////////

		for (j = 0; j < cs; j++)
		{
			clsc = &spsc->vClassSpace[j];
			trsc = &clsc->trainspace;
			ttsc = &clsc->testspace;
			clsc->id_fold = spsc->id_fold;
			clsc->id_cls = j;
			clsc->sCLF = spsc->sCLF;
			//clsc->log = &spsc->log;
			clsc->isSeed = spsc->isSeed;
			clsc->dimSize = spsc->dimSize;
			clsc->ctrdSize = spsc->ctrdSize;
			clsc->vTrainSpaceDim = (SpaceDim_t*)calloc(ds, sizeof(SpaceDim_t));

			trsc->id_fold = clsc->id_fold;
			trsc->id_cls = clsc->id_cls;
			trsc->logSample = &clsc->log;
			trsc->isSeed = clsc->isSeed;
			trsc->dimSize = clsc->dimSize;
			trsc->ctrdSize = clsc->ctrdSize;
			trsc->vCentroid = (Centroid_t*)calloc(db->ctrdsize, sizeof(Centroid_t));
			trsc->datasize = read_data_record_size(TRAIN, i, j, db->dimsize);
			trsc->data = (Data_t*)calloc(trsc->datasize, sizeof(Data_t));
			for (h = 0; h < trsc->datasize; trsc->data[h++].raw = (double*)calloc(ds, sizeof(double)));
			trsc->pdf = (double*)calloc(trsc->datasize, sizeof(double));
			trsc->pdf_fs = (Record_t*)calloc(trsc->datasize, sizeof(Record_t));
			trsc->cdf = (double*)calloc(trsc->datasize, sizeof(double));
			trsc->cmp = (double*)calloc(trsc->datasize, sizeof(double));
			trsc->ctrd_vBetaData_a = (double*)calloc(trsc->datasize, sizeof(double));
			trsc->ctrd_vBetaData_b = (double*)calloc(trsc->datasize, sizeof(double));

			ttsc->id_fold = clsc->id_fold;
			ttsc->id_cls = clsc->id_cls;
			ttsc->logSample = &clsc->log;
			ttsc->dimSize = clsc->dimSize;
			ttsc->datasize = read_data_record_size(TEST, i, j, db->dimsize);
			ttsc->data = (Data_t*)calloc(ttsc->datasize, sizeof(Data_t));
			for (h = 0; h < ttsc->datasize; ttsc->data[h++].raw = (double*)calloc(ds, sizeof(double)));
			ttsc->pdf = (double*)calloc(ttsc->datasize, sizeof(double));
			ttsc->pdf_fs = (Record_t*)calloc(ttsc->datasize, sizeof(Record_t));


			for (h = 0; h < *trsc->ctrdSize; h++)
			{
				ctrd = &trsc->vCentroid[h];

				ctrd->id_fold = trsc->id_fold;
				ctrd->id_cls = trsc->id_cls;
				ctrd->id_ctrd = h;
				ctrd->logSample = trsc->logSample;
				ctrd->dimSize = trsc->dimSize;
				ctrd->betaA = &trsc->ctrd_beta_a;
				ctrd->betaB = &trsc->ctrd_beta_b;
				ctrd->betaDataA = &trsc->ctrd_vBetaData_a;
				ctrd->betaDataB = &trsc->ctrd_vBetaData_b;
				ctrd->varMax = &trsc->ctrd_var_max;
				ctrd->delegate.raw = (double*)calloc(ds, sizeof(double));
				ctrd->vSpaceDim = (SpaceDim_t*)calloc(ds, sizeof(SpaceDim_t));
				ctrd->vDimWeight = (double*)calloc(ds, sizeof(double));
				ctrd->pdfweight = (double*)calloc(CANDIDATE_SIZE, sizeof(double));
				ctrd->vBetaSpace = (BetaSpace_t*)calloc(CANDIDATE_SIZE, sizeof(BetaSpace_t));
				ctrd->seedRaw = (double*)calloc(ds, sizeof(double));
			}
			//sample space variable, allocation for efficency
			spsc->mDiscriminant[j] = (int*)calloc(cs, sizeof(int));

		}
	}
	printf("[COMPLETE]\n");
}

void closeDatabase(OneClass_t *db)
{
	int i, j, h;
	int ds = db->dimsize, cs = db->clssize;
	Centroid_t *ctrd;
	SampleSpace_t *spsc;
	TrainSpace_t *trsc;
	TestSpace_t *ttsc;
	ClassSpace_t *clsc;
	fld_t *fld;

	if (db == NULL) return;

	printf("vacuuming data for program finish...");

	for (i = 0; i < TENFOLD; i++)
	{
		spsc = &db->vSampleSpace[i];

		fld = &spsc->fld;

		for (j = 0; j < cs; j++)
		{
			clsc = &spsc->vClassSpace[j];
			trsc = &clsc->trainspace;
			ttsc = &clsc->testspace;

			for (h = 0; h < *trsc->ctrdSize; h++)
			{
				ctrd = &trsc->vCentroid[h];

				free(ctrd->delegate.raw);
				free(ctrd->vSpaceDim);
				free(ctrd->vDimWeight);
				free(ctrd->pdfweight);
				free(ctrd->vBetaSpace);
				free(ctrd->seedRaw);
			}

			for (h = 0; h < ttsc->datasize; free(ttsc->data[h++].raw));
			free(ttsc->data);
			free(ttsc->pdf);
			free(ttsc->pdf_fs);

			free(trsc->vCentroid);
			for (h = 0; h < trsc->datasize; free(trsc->data[h++].raw));
			free(trsc->data);
			free(trsc->pdf);
			free(trsc->pdf_fs);
			free(trsc->cdf);
			free(trsc->cmp);
			free(trsc->ctrd_vBetaData_a);
			free(trsc->ctrd_vBetaData_b);

			free(clsc->vTrainSpaceDim);

			//sample space variable, allocation for efficency
			free(spsc->mDiscriminant[j]);
		}
		//////////////////////////////////////
		for (j = 0; j < cs; free(fld->mWeight[j++]));
		free(fld->mWeight);
		for (j = 0; j < cs; free(fld->mMean[j++]));
		free(fld->mMean);
		free(fld->vMean);
		free(fld->vEgien);
		free(fld->vProjMean);
		for (j = 0; j < cs;free(fld->S[j++]))
			for (h = 0; h < ds; free(fld->S[j][h++]));
		free(fld->S);

		for (j = 0; j < ds; free(fld->mSB[j++]));
		free(fld->mSB);
		for (j = 0; j < ds; free(fld->mSW[j++]));
		free(fld->mSW);
		free(fld->vProjS);
		//////////////////////////////////////
		free(spsc->vClassSpace);
		free(spsc->mDiscriminant);
	}
	free(db->vSampleSpace);
	printf("[complete]\n");
}

void mountData(OneClass_t *db)
{
	int h, i, j;
	char *filepath = (char*)malloc(FILEPATH_LENGTH);
	makeDir("log");
	if (db == NULL) return;

	printf("\nMOUNT DATA...\n");
	for (i = 0; i < TENFOLD; i++)
		for (j = 0; j < CLASSSIZE; j++)
		{
			printf("Fold=%d, Class=%d,", i, j);

			for (h = 0; h < FILEPATH_LENGTH; filepath[h++] = '\0');
			get_data_path_by_type(TRAIN, i, j, filepath);

			if (readData(filepath, db->dimsize,
				db->vSampleSpace[i].vClassSpace[j].trainspace.datasize, 
				db->vSampleSpace[i].vClassSpace[j].trainspace.data))
				printf("[TRAIN OK]");
			else
				printf("[TRAIN FAIL]");

			for (h = 0; h < FILEPATH_LENGTH; filepath[h++] = '\0');
			get_data_path_by_type(TEST, i, j, filepath);

			if (readData(filepath, db->dimsize,
				db->vSampleSpace[i].vClassSpace[j].testspace.datasize,
				db->vSampleSpace[i].vClassSpace[j].testspace.data))
				printf("[TEST OK]\n");
			else
				printf("[TEST FAIL]\n");

			setDimentionRange(&db->vSampleSpace[i].vClassSpace[j]);
		}
	free(filepath);
	printf("[COMPLETE]\n");
}

int readSeedRaw(OneClass_t *db)
{
	FILE *rrd;
	char line[256];
	int i, j, l, m, n;

	db->isseed = 0;
	if ((rrd = fopen("randdatadmp.csv", "r")) == NULL)
	{
		printf("The dump file of random data for cluster does not exist. set Seed data up random.\n");
		return 1;
	}

	for (n = 0; fscanf(rrd, "%[^\n]%*c", &line) != EOF; n++);
	if (n != TENFOLD * CLASSSIZE * db->ctrdsize)
	{
		printf("the size of centroid does not same.\n");
		return 2;
	}
	rrd = freopen("randdatadmp.csv", "r", rrd);
	for (i = 0; i < TENFOLD; i++)
		for (j = 0; j < CLASSSIZE; j++)
			for (l = 0; l < db->ctrdsize; l++)
				for (m = 0; m < db->dimsize; m++)
					fscanf(rrd, "%lf,", &db->vSampleSpace[i].vClassSpace[j].trainspace.vCentroid[m].seedRaw);

	close(rrd);
	db->isseed = 1;
	return 0;
}

void setDimentionRange(ClassSpace_t *clsc)
{
	static int i, j;
	static double x, y;
	int datasize = clsc->trainspace.datasize;
	Data_t* table_data = clsc->trainspace.data;

	if (clsc == NULL) return;

	/* calculating the variances of data */
	for (j = 0; j < *clsc->dimSize; j++)
	{
		for (x = 0., i = 0; i < datasize; i++)
			x += table_data[i].raw[j];
		x /= datasize;

		for (y = 0., i = 0; i < datasize; i++)
			y += vdistance(table_data[i].raw[j], x);
		y /= (datasize - 1);//sample이기 때문에
		clsc->vTrainSpaceDim[j].mean = x;
		clsc->vTrainSpaceDim[j].variance = y;
	}
}


int getLBGClusterSize(int ec, ClassSpace_t *clsc)
{
	int i, j, u;
	int closest_ctrd_i;
	//SampleSpace_t *samplespace = &db->vSampleSpace[f];
	//ClassSpace_t *clsc = &samplespace->vClassSpace[c];
	TrainSpace_t *trsc = &clsc->trainspace;
	Centroid_t *centroid = clsc->trainspace.vCentroid;
	SpaceDim_t *spacedim = clsc->vTrainSpaceDim;

	//centroid의 dimension parameter들 초기화 해줍니다...이거땜시 나흘을 버렸어!!!
	for (i = 0; i < ec; i++)
		for (j = 0; j < *trsc->dimSize; j++)
			centroid[i].vSpaceDim[j].mean = centroid[i].vSpaceDim[j].variance = 0.;

	*(centroid->varMax) = 0.;//기존 프로그램에서는 호출될 때마다 초기화 되던데요?
	if (*trsc->isSeed)
	{
		for (i = 0; i < ec; i++)
			for (j = 0; j < *trsc->dimSize; j++)
				centroid[i].delegate.raw[j] = centroid[i].seedRaw[j];
	}
	else
	{
		srand(ec);
		for (i = 0; i < ec; i++)
			for (j = 0; j < *trsc->dimSize; j++)
				centroid[i].delegate.raw[j] = centroid[i].seedRaw[j] = sqrt(spacedim[j].variance) * (2.0 * rand() / RAND_MAX - 1.0) + spacedim[j].mean;

		for (u = 0; u < CLUSTERING_COUNT; u++)
		{
			for (i = 0; i < ec; i++)
			{
				centroid[i].delegatorsize = 0;
				for (j = 0; j < *trsc->dimSize; j++)
					centroid[i].vDimWeight[j] = 0.;
			}

			for (i = 0; i < trsc->datasize; i++)
			{
				closest_ctrd_i = find_closest_centroid_index(ec, i, centroid, trsc->data);
				for (j = 0; j < *trsc->dimSize; j++)
					centroid[closest_ctrd_i].vDimWeight[j] += trsc->data[i].raw[j];

				((*(centroid + closest_ctrd_i)).delegatorsize)++;
			}

			for (i = 0; i < ec; i++)
			{
				for (j = 0; j < *trsc->dimSize; j++)
				{
					centroid[i].delegate.raw[j] = centroid[i].vDimWeight[j];

					if (centroid[i].delegatorsize > CNTR_IGNORE_SIZE)
						centroid[i].delegate.raw[j] /= centroid[i].delegatorsize;
				}
			}
		}
	}

	fprintf(*trsc->logSample, "centroid size : %d\n", ec);
	fprintf(*trsc->logSample, "ctrd no.\tdatasize\tworktype\t");
	for (i = 0; i < *trsc->dimSize; fprintf(*trsc->logSample, "data[%d]%c", i++, *trsc->dimSize - i == 1 ? '\n' : '\t'));
	//cluster 결과를 log로 남기기 위한 코드입니다.
	for (i = 0; i < ec; i++)
	{
		fprintf(*trsc->logSample, "\t\tseed\t");
		for (j = 0; j < *trsc->dimSize; fprintf(*trsc->logSample, "%lf%c", centroid[i].seedRaw[j++], *trsc->dimSize - j == 1 ? '\n' : '\t'));
		fprintf(*trsc->logSample, "%d\t%d\tclst\t", i, centroid[i].delegatorsize);
		for (j = 0; j < *trsc->dimSize; fprintf(*trsc->logSample, "%lf%c", centroid[i].delegate.raw[j++], *trsc->dimSize - j == 1 ? '\n' : '\t'));
	}

	//set dimension parameter of centroids
	for (i = 0; i < trsc->datasize; i++)
	{
		closest_ctrd_i = find_closest_centroid_index(ec, i, centroid, trsc->data);
		for (j = 0; j < *trsc->dimSize; j++)
		{
			centroid[closest_ctrd_i].vSpaceDim[j].variance += vdistance(trsc->data[i].raw[j], centroid[closest_ctrd_i].delegate.raw[j]);
			centroid[closest_ctrd_i].vSpaceDim[j].mean += trsc->data[i].raw[j];
		}
	}
	for (i = 0; i < ec; i++)
	{
		if ((*(centroid + i)).delegatorsize > CNTR_IGNORE_SIZE)
		{
			for (j = 0; j < *trsc->dimSize; j++)
			{
				centroid[i].vSpaceDim[j].variance /= (centroid[i].delegatorsize - 1);
				centroid[i].vSpaceDim[j].mean /= centroid[i].delegatorsize;
			}
		}
	}

	for (i = 0; i < ec; i++)
	{
		for (j = 0; j < *trsc->dimSize; j++)
		{ 
			if (centroid[i].delegatorsize > CNTR_IGNORE_SIZE && centroid[i].vSpaceDim[j].variance > *(centroid->varMax))
				*(centroid->varMax) = centroid[i].vSpaceDim[j].variance;//varmax is share variable
		}
	}

	for (i = 0, j = 0; i < ec; i++)
		if (centroid[i].delegatorsize > CNTR_IGNORE_SIZE) j++;

	return j;
}

int find_closest_centroid_index(int ec, int dc, Centroid_t *ctrd, Data_t *data)
{
	int i, j, idx = 0;
	double mv = 1000., tmp;

	for (i = 0; i < ec; i++)
	{
		for (j = 0, tmp = 0.; j < *ctrd->dimSize; j++)
			tmp += vdistance(ctrd[i].delegate.raw[j], data[dc].raw[j]);
		
		if (tmp < mv)
			mv = tmp, idx = i;
	}
	return idx;
}

int trainOneClass(int f, int c, int ec, OneClass_t *db)
{
	int i;
	int clusterIndex;
	double x, y;

	SampleSpace_t *samplespace = &db->vSampleSpace[f];
	ClassSpace_t *clsc = &samplespace->vClassSpace[c];
	TrainSpace_t *trsc = &clsc->trainspace;
	Centroid_t *centroid = clsc->trainspace.vCentroid;
	SpaceDim_t *dimparam = clsc->vTrainSpaceDim;
	
	/*CAUTION
	본문에서 사용된 centroid_weight와 train_one_class에서의 weight는 공유할 것입니다.
	*/
	/* clustering: finding centroids */
	fprintf(clsc->log, "[class #%d] <getLBGClusterSize>\n", c);
	clusterIndex = getLBGClusterSize(ec + 1, clsc);

	fprintf(clsc->log, "[class #%d] <for of setCPON>\n", c);
	//겹치지 않도록, 초기에 설정한 centroid index에 따라 candidated cpon range를 저장해야 합니다.
	for (i = 0, x = 0.; i < CANDIDATE_SIZE; i++)
	{
		setCPON(ec, clusterIndex - 1, i, clsc);
		fprintf(debug, "%d\t%d\t%d\t%d\t%d\t%d\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\n",
			f, c, TRAIN, ec, i, centroid[ec].vBetaSpace[i].no_centroids, centroid[ec].vBetaSpace[i].sigma,
			centroid[ec].pdfweight[i], centroid[ec].vBetaSpace[i].beta_a.shape, centroid[ec].vBetaSpace[i].beta_b.shape,
			centroid[ec].vBetaSpace[i].beta_a.bias, centroid[ec].vBetaSpace[i].beta_b.bias,
			centroid[ec].vBetaSpace[i].beta_a.var, centroid[ec].vBetaSpace[i].beta_b.var,
			centroid[ec].vBetaSpace[i].p_value_train, clsc->trainspace.y_min,
			clsc->trainspace.y_max, clsc->trainspace.ctrd_var_max);
		
		if (x < (y = centroid[ec].vBetaSpace[i].p_value_train))
		{
			x = y;
			centroid[ec].dele_beta_idx = i;//todo 이게 과연 필요한 지 확인할 것.
			//선정된 beta를 공용 beta로 저장합니다.
			trsc->ctrd_beta_a = centroid[ec].vBetaSpace[i].beta_a.shape;
			trsc->ctrd_beta_b = centroid[ec].vBetaSpace[i].beta_b.shape;
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
void setCPON(int ec, int clst_c, int l, ClassSpace_t *clsc)
{
	TrainSpace_t *trsc = &clsc->trainspace;
	Centroid_t *centroid = trsc->vCentroid;
	SpaceDim_t *dimparam = clsc->vTrainSpaceDim;
	int datasize = trsc->datasize;

	//set no_centroids
	centroid[clst_c].vBetaSpace[l].no_centroids = clst_c + 1;
	//set up sigma in the last centroid
	centroid[ec].vBetaSpace[l].sigma = sqrt(*(centroid->varMax)) * (exp((l - 2.0) * log(2.0)));
	//set pdfweight being centroid and pdf being dataparam
	setPDF(ec, clst_c + 1, l, trsc);
	//set feature scaled pdf in the dataparam(it includes getta min and max of pdf)
	setFeatureScaledPDF(clsc);
	//printf("%lf, %lf\n", (*(*((db->table_dataparam) + f) + c))->y_min, (*(*((db->table_dataparam) + f) + c))->y_max);
	//set beta parameters belonging to cpon range
	centroid_draw_betashape(l, &centroid[clst_c], trsc);
	//set p value of train
	centroid[clst_c].vBetaSpace[l].p_value_train = p_value_train(datasize, trsc->cdf, trsc->cmp,
		centroid[clst_c].vBetaSpace[l].beta_a.shape, centroid[clst_c].vBetaSpace[l].beta_b.shape,
		trsc->pdf_fs);
}

/*각 centroid와 모든 data간의 euclidean distance를 계산해서 
각 centroid의 pdfweight 와 cpon_range
각 data의 pdf에 저장*/
void setPDF(int ec, int clst_s, int cdd_c, TrainSpace_t *trsc)
{
	Centroid_t *ctrd = trsc->vCentroid;
	int dim = *trsc->dimSize, i, j, k;
	double x;
	double sigma = ctrd[ec].vBetaSpace[cdd_c].sigma;

	for (i = 0; i < clst_s; i++)
		ctrd[i].pdfweight[cdd_c] = 1.0 / (clst_s * exp(dim * log(sigma * sqrt(2.0 * PI))));

	for (i = 0; i < trsc->datasize; i++)
	{
		trsc->pdf[i] = 0.;
		for (j = 0; j < clst_s; j++)
		{
			x = 0.;
			for (k = 0; k < dim; k++)
				x += vdistance(trsc->data[i].raw[k], ctrd[j].delegate.raw[k]);

			trsc->pdf[i] += ctrd[j].pdfweight[cdd_c] * exp(-x / (2.0 * sigma * sigma));
//			printf("%lf %lf is %s\n", *(((ctrd + j)->pdfweight) + cdd_c), ctrd[j].pdfweight[cdd_c], *(((ctrd + j)->pdfweight) + cdd_c) == ctrd[j].pdfweight[cdd_c] ? "same" : "diffrent");
		}
	}
}

/* sample mean and variance of normalized output */
//deperated
void sample_mean_variance(int a, double* _s_mean, double* _s_var, Record_t* _y_nor)
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
void parameter_beta_mm(double mean, double var, Centroid_t *centroid)
{
	double y = mean * (1 - mean) / var - 1.0;
	*centroid->betaA = mean * y;
	*centroid->betaB = (1 - mean) * y;
}

//deperated
void parameter_beta_mm_jk(int a, int b, double s_mean_, double s_var_, Record_t* _y_nor, Centroid_t *centroid)
{
	double x, mean, var, y;
	
	x = s_var_ * (a - 1) + a * s_mean_ * s_mean_ - _y_nor[b].value * _y_nor[b].value;
	mean = (s_mean_ * a - _y_nor[b].value) / (a - 1);
	var = (x - (a - 1) * mean * mean) / (a - 2);
	y = mean * (1 - mean) / var - 1.0;

	*centroid->betaA = mean * y;
	*centroid->betaB = (1 - mean) * y;
}

double p_value_train(int a, double* __y_cdf, double* __y_cmp, double beta_a, double beta_b, Record_t* __y_nor)
{
	return KS_test(getLInfinityNorm(a, __y_cdf, __y_cmp, __y_nor, beta_a, beta_b), a);
}

double getLInfinityNorm(int a, double* _y_cdf, double* _y_cmp, Record_t* _y_nor, double beta_a, double beta_b)
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

double getSampleMean(int recordsize, Record_t* r)
{
	int i;
	double m = 0.;

	for (i = 0; i < recordsize; i++) m += r[i].value;

	return m / recordsize;
}

double getSampleVariance(double mean, int recordsize, Record_t* r)
{
	int i;
	double v = 0.;

	for (i = 0; i < recordsize; i++) v += vdistance(r[i].value, mean);

	return v / (recordsize - 1);
}

void setFeatureScaledPDF(ClassSpace_t *clsc)
{
	int i, j, datasize = clsc->trainspace.datasize;

	//cdf를 구하기 위해서(인지 정확히는 잘 모르지만) normalized data들을 크기에 대한 오름차순으로 정렬합니다.
	for (i = 0; i < datasize; i++)
	{
		clsc->trainspace.pdf_fs[i].index = i;
		clsc->trainspace.pdf_fs[i].value = clsc->trainspace.pdf[i];
	}
	quicksort(clsc->trainspace.pdf_fs, datasize);

	clsc->trainspace.y_min = clsc->trainspace.pdf[clsc->trainspace.pdf_fs[0].index];
	clsc->trainspace.y_max = clsc->trainspace.pdf[clsc->trainspace.pdf_fs[clsc->trainspace.datasize - 1].index];
	//	fprintf(debug, "%d,%d,%d,%lf,%lf\n", f, c, t, db->table_dataparam[f][c]->y_min, db->table_dataparam[f][c]->y_max);

	for (i = 0; i < datasize; i++)
	{
		j = clsc->trainspace.pdf_fs[i].index;
		clsc->trainspace.pdf_fs[i].value = feature_scaling(
			clsc->trainspace.pdf[j], clsc->trainspace.y_min, clsc->trainspace.y_max);
	}
}

void getCPONPowerRatio(int f, int c, int t, int ec, OneClass_t *db)
{
	int i, j;
	double x, beta, z, pnew;
	//SampleSpace_t* samplespace = &db->vSampleSpace[f];
	ClassSpace_t *clsc = &db->vSampleSpace[f].vClassSpace[c];
	TrainSpace_t *trsc = &db->vSampleSpace[f].vClassSpace[c].trainspace;
	Centroid_t *centroid = &clsc->trainspace.vCentroid[ec - 1];
	int datasize = trsc->datasize;
	
	/* hypothesis testing of one-class problems for train data */
	//clear cdf
	for (i = 0; i < trsc->datasize; trsc->cdf[i++] = 0.);

	beta = integrate(INTEGRATE_FROM, INTEGRATE_TO, 2, INTEGRATE_TOLERANCE, trsc->ctrd_beta_a, trsc->ctrd_beta_b);
	for (i = 0; i < trsc->datasize; i++)
	{
		j = trsc->pdf_fs[i].index;
		x = feature_scaling(trsc->pdf[j], clsc->trainspace.y_min, clsc->trainspace.y_max);

		if (x <= 1.0e-8)
			x = 1.0e-8;
		else if (x >= 1.0 - 1.0e-8)
			x = 1.0 - 1.0e-8;
		z = integrate(INTEGRATE_FROM, x, 2, INTEGRATE_TOLERANCE, trsc->ctrd_beta_a, trsc->ctrd_beta_b);
		pnew = z / beta;
		trsc->cdf[i] = pnew;
	}
	//printf("[%d]{beta=%lf, x=%lf, z=%lf}", ec, beta, x, z);

	/* hypothesis testing of one-class problems for test data */
		/* CPON output */
	setPDF(ec, datasize, centroid->dele_beta_idx, trsc);
		/* normalization of CPON output */
		/* number of positive data for cdf_t */
		/* calculation of cdf_t */
	/* calculation of the p-value of K-S statistic */
}

void printCentroid(Centroid_t* c, int no)
{
	int i;
	int dim = *(c->dimSize);

	printf("[centroid No : %2d]\n", no);
	printf("<single values>\n");
	printf("int = {datasize : %d, dimsize : %d, cpon index : %d}\n", c->delegatorsize, dim, c->dele_beta_idx);
	printf("double = {var max : %8.4lf, beta_a : %8.4lf, beta_b : %8.4lf}\n", *(c->varMax), c->betaA, c->betaB);
	printf("<tables in order dimension>\nindex\t   data\t\tdimweight\tmean\t\tvariance\n");
	for (i = 0; i < dim; i++)
		printf("[%d]\t%8.4lf\t%8.4lf\t%8.4lf\t%8.4lf\n", i, c->delegate.raw[i], c->vDimWeight[i], c->vSpaceDim[i].mean, c->vSpaceDim[i].variance);
	printf("<tables in candidate>\nindex\tpdfweight\tctrd_s\t   sigma\tweight\t\tbeta_a\t\ta_var\t\ta_bias\t\tbeta_b\t\tb_var\t\tb_bias\t\tpv_tr\t\tpv_tt\n");
	for (i = 0; i < CANDIDATE_SIZE; i++)
		printf("[%d]\t%8.4lf\t%3d\t%8.4lf\t%8.4lf\t%8.4lf\t%8.4lf\t%8.4lf\t%8.4lf\t%8.4lf\t%8.4lf\t%8.4lf\t%8.4lf\n", 
		i, c->vDimWeight[i], c->vBetaSpace[i].no_centroids, c->vBetaSpace[i].sigma,
		c->vBetaSpace[i].weight, c->vBetaSpace[i].beta_a.shape, c->vBetaSpace[i].beta_a.var, 
		c->vBetaSpace[i].beta_a.bias, c->vBetaSpace[i].beta_b.shape,
		c->vBetaSpace[i].beta_b.var, c->vBetaSpace[i].beta_b.bias, c->vBetaSpace[i].p_value_train, 
		c->vBetaSpace[i].p_value_trts);
}

void printsamplespace(SampleSpace_t *d)
{
	int i, j;
	int dim = *(d->dimSize);
	TrainSpace_t trainspace = d->vClassSpace->trainspace;
	printf("[dataparam]\n");
	printf("<single values>\n");
	printf("int = {datasize : %d, dimsize : %d}\n", d->vClassSpace->trainspace.datasize, dim);
	printf("double = {y_min = %8.4lf, y_max = %8.4lf}\n", d->vClassSpace->trainspace.y_min, d->vClassSpace->trainspace.y_max);
	printf("<data of tables in data size>\nindex\t");
	
	for (i = 0; i < dim; ++i == dim ? putchar('\n') : printf("\t\t"))
		printf("<%02d>", i);
	for (i = 0; i < d->vClassSpace->trainspace.datasize; i++)
	{
		printf("[%d]\t", i);
		for (j = 0; j < dim; putchar(++j == dim ? '\n' : '\t'))
			printf("%8.4lf", d->vClassSpace->trainspace.data[i].raw[j]);
	}
	printf("<data param tables in datasize>\nindex\tpdf\t\tnor\t\tcdf\t\tcmp\n");
	for (i = 0; i < d->vClassSpace->trainspace.datasize; i++)
		printf("[%d]\t%8.4lf\t%8.4lf\t%8.4lf\t%8.4lf\n", 
		i, trainspace.pdf[i], trainspace.pdf_fs[i], trainspace.cdf[i], trainspace.cmp[i]);
}

/*set beta parameters for CPON range exported from mean and variance
centroid는 정확히 현재 선택된 centroid를 넘겨줘야 합니다.(i.e. centroid[clst_c])
*/
void centroid_draw_betashape(int candi, Centroid_t *centroid, TrainSpace_t *trsc)
{
	double mean, var, jkMean, jkVar;
	double betaA, betaB, jkBetaA, jkBetaB;
	double a_var, b_var, jk;
	double x, y;
	int i;
	int datasize = trsc->datasize;
	int samplesize = datasize - 1;
	Beta_t *beta_a = &centroid->vBetaSpace[candi].beta_a;
	Beta_t *beta_b = &centroid->vBetaSpace[candi].beta_b;

	//sample_mean_variance(datasize, &s_mean, &s_var, dataparam->nor);
	//아래의 코드가 실험상 더 빠릅니다.
	mean = getSampleMean(datasize, trsc->pdf_fs);
	var = getSampleVariance(mean, datasize, trsc->pdf_fs);
	setBetaSpaceShape(mean, var, &betaA, &betaB);

	for (i = 0, x = 0., y = 0.; i < datasize; i++)
	{
		//Jackknife method
		jk = var * samplesize + datasize * mean * mean - trsc->pdf_fs[i].value * trsc->pdf_fs[i].value;
		jkMean = (mean * datasize - trsc->pdf_fs[i].value) / samplesize;
		jkVar = (jk - samplesize * jkMean * jkMean) / (samplesize - 1);
		setBetaSpaceShape(jkMean, jkVar, &jkBetaA, &jkBetaB);
		//뭔진 모르겠는데 jk랑 아닌 거랑 빼고 곱하고 함
		x += jkBetaA, y += jkBetaB;
		
		trsc->ctrd_vBetaData_a[i] = datasize * betaA - samplesize * jkBetaA;
		trsc->ctrd_vBetaData_b[i] = datasize * betaB - samplesize * jkBetaB;
	}
	//set beta
	beta_a->shape = jkBetaA;
	beta_b->shape = jkBetaB;
	//set beta bias
	beta_a->bias = samplesize * ((x /= datasize) - betaA);
	beta_b->bias = samplesize * ((y /= datasize) - betaB);
	//set beta variance
	for (i = 0, x = 0., y = 0.; i < datasize; i++)
	{
		x += trsc->ctrd_vBetaData_a[i];
		y += trsc->ctrd_vBetaData_b[i];
	}
	for (x /= datasize, y /= datasize, a_var = 0., b_var = 0., i = 0; i < datasize; i++)
	{
		a_var += vdistance(trsc->ctrd_vBetaData_a[i], x);
		b_var += vdistance(trsc->ctrd_vBetaData_b[i], y);
	}
	a_var /= datasize * samplesize;
	b_var /= datasize * samplesize;
	beta_a->var = a_var;
	beta_b->var = b_var;
}

void setBetaSpaceShape(double mean, double var, double *betaA, double *betaB)
{
	double y = mean * (1 - mean) / var - 1.0;

	*betaA = mean * y;
	*betaB = (1 - mean) * y;
}

int optPresidentCPON(int f, int c, int t, int clst_c, OneClass_t *db)
{
	Centroid_t *centroid = db->vSampleSpace[f].vClassSpace[c].trainspace.vCentroid;
	int presi_idx, j, presi_turn;
	double x, y, pwr_p, pwr_r;

	presi_idx = -1;	pwr_p = pwr_r = 0.0;

	for (presi_turn = 0; presi_turn < clst_c; presi_turn++)
	{
		j = centroid[presi_turn].dele_beta_idx;
		x = centroid[presi_turn].vBetaSpace[j].p_value_trts;
		y = centroid[presi_turn].vBetaSpace[j].p_value_train;

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
			x = centroid[clst_c].vBetaSpace[centroid[clst_c].dele_beta_idx].p_value_train;

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

void fisher_linear_discriminant(SampleSpace_t *spsc)
{
	int i, j, k, h;
	int ds = *spsc->dimSize;
	int cs = *spsc->clsSize;
	ClassSpace_t *clsc;
	TrainSpace_t *trsc;
	fld_t *fld = &spsc->fld;
	Data_t *data;
	double temp;

	//step 1. get means for each fold and class and total mean for every fold and class
	for (i = 0; i < ds; i++)
	{
		for (temp = 0., j = 0; j < cs; j++)
		{
			trsc = &spsc->vClassSpace[j].trainspace;
			for (k = 0; k < trsc->datasize; k++)
				fld->mMean[j][i] += trsc->data[k].raw[i];
			fld->vMean[i] += fld->mMean[j][i] /= trsc->datasize;
		}
		fld->vMean[i] /= cs;
	}

	//result of step 1
	fprintf(spsc->log, "total mean\n");
	for (i = 0; i < ds; fprintf(spsc->log, "%c", ++i == ds ? '\n' : '\t'))
		fprintf(spsc->log, "%lf", fld->vMean[i]);
	fprintf(spsc->log, "mean of class\n");
	for (i = 0; i < cs;i++)
		for (j = 0; j < ds; fprintf(spsc->log, "%c", ++j == ds ? '\n' : '\t'))
			fprintf(spsc->log, "%lf", fld->mMean[i][j]);

	for (i = 0; i < cs; i++)
	{
		clsc = &spsc->vClassSpace[i];
		trsc = &clsc->trainspace;
		
		//step 2. get scatter matrix
		getScatterMatrix(i, spsc);
	}
	//step 3. get SW and SB
	for (i = 0; i < cs; i++)
		for (j = 0; j < ds; j++)
			for (k = 0; k < ds; k++)
				fld->mSW[j][k] += fld->S[i][j][k];
	for (i = 0; i < ds; i++)
	{
		fld->mSB[i][i] = (fld->mMean[0][i] - fld->mMean[1][i]) * (fld->mMean[0][i] - fld->mMean[1][i]);
		for (j = i; j < ds; j++)
			fld->mSB[j][i] = fld->mSB[i][j] = (fld->mMean[0][i] - fld->mMean[1][j]) * (fld->mMean[0][i] - fld->mMean[1][j]);
	}

	fprintf(spsc->log, "SW\n");
	for (i = 0; i < ds; i++)
		for (j = 0; j < ds; fprintf(spsc->log, "%c", ++j == ds ? '\n' : '\t'))
			fprintf(spsc->log, "%lf", fld->mSW[i][j]);
	fprintf(spsc->log, "SB\n");
	for (i = 0; i < ds; i++)
		for (j = 0; j < ds; fprintf(spsc->log, "%c", ++j == ds ? '\n' : '\t'))
			fprintf(spsc->log, "%lf", fld->mSB[i][j]);
	//step 4. get weight
	minverse(&fld->mSW, ds, &fld->mSB);
	for (i = 0; i < ds; i++)
		for (j = 0; j < ds;j++)
			fld->mWeight[0][i] += fld->mSB[i][j] * (fld->mMean[0][j] - fld->mMean[1][j]);

	fprintf(spsc->log, "\nwegight[fold=%d]\n", spsc->id_fold);
	for (i = 0; i < ds; fprintf(spsc->log, "%c",++i == ds ? '\n' : '\t'))
	{
		fprintf(spsc->log, "%lf", fld->mWeight[0][i]);
	}
	//step 5. get projection mean
	for (i = 0; i < cs; i++)
		for (j = 0; j < ds; j++)
			fld->vProjMean[i] += fld->mMean[i][j] * fld->mWeight[i][j];
	fprintf(spsc->log, "[y from w and x]\n");
	//step 6. get projected scatter
	for (i = 0; i < cs; i++)
	{
		trsc = &spsc->vClassSpace[i].trainspace;
		for (j = 0; j < trsc->datasize; j++)
		{
			for (temp = 0., k = 0; k < ds; k++)
				temp += fld->mWeight[0][k] * trsc->data[j].raw[k];
			fld->vProjS[i] += (temp - fld->vProjMean[i]) * (temp - fld->vProjMean[i]);
			fprintf(spsc->log, "%d\t%d\t%lf\n", i, j, temp);
		}
//		printf("%lf", fld->vProjS[i]);
	}

	//step 7 get J(w)
	fprintf(spsc->log, "J(w) : %lf\n", ((fld->vProjMean[0] - fld->vProjMean[1]) * (fld->vProjMean[0] - fld->vProjMean[1])) / (fld->vProjS[0] + fld->vProjS[1]));

	putchar(' ');
}

void getScatterMatrix(int i_cls, SampleSpace_t *spsc)
{
	int i, j, k;
	int datasize = spsc->vClassSpace[i_cls].trainspace.datasize;
	int dim = *spsc->dimSize;
	double tv;
	double **s = spsc->fld.S[i_cls];
	Data_t *data = spsc->vClassSpace[i_cls].trainspace.data;
	SpaceDim_t *spd = spsc->vClassSpace[i_cls].vTrainSpaceDim;

	for (i = 0; i < datasize; i++)
	{
		for (j = 0; j < dim; j++)
		{
			s[j][j] += (data[i].raw[j] - spd[j].mean) * (data[i].raw[j] - spd[j].mean);
			for (k = j + 1; k < dim; k++)
			{
				tv = (data[i].raw[j] - spd[j].mean) * (data[i].raw[k] - spd[k].mean);
				s[j][k] += tv, s[k][j] += tv;
			}
		}
	}

	//result of step 2. check scatter matrix on ith class
	//for (j = 0; j < dim; j++)
	//	for (k = 0; k < dim; putchar(++k == dim ? '\n' : '\t')){
	//		printf("%lf", s[j][k]);
	//	}
}