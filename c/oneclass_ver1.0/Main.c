#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "vsheader.h"
#include "Utility.h"
#include "Oneclass.h"

//LBG Algorithm의 clustering 횟수입니다. 기존값은 20입니다. 10이 더 나을 수도 있습니다.
//const static int CLUSTERING_COUNT = 20;

int main(int argc, char *args[])
{
	int i, j;
	static int n_dim, n_cls = CLASSSIZE, n_ctrd = 5, mLBGCentroidSize;
	static int i_fold, i_cls, i_ctrd;
	static int dat_s;
	int ctrd_LBG_s;
	int i_opt;
	OneClass_t *db;
	SampleSpace_t *spsc;
	ClassSpace_t *clsc;
	TrainSpace_t *trsc;
	TestSpace_t *ttsc;
	Centroid_t *centroid;
	SpaceDim_t *spacedim;
	char *filepath = (char*)malloc(FILEPATH_LENGTH);

	db = (OneClass_t*)malloc(sizeof(OneClass_t));
	n_dim = db->dimsize = read_data_dim_size();
	db->clssize = n_cls;
	db->ctrdsize = n_ctrd;

	openDatabase(db);
	mountData(db);
	readSeedRaw(db);

	printf("\nCLASSIFICATION START\n");
	fprintf(debug, "fold\tcls\ttt\tclst\tcand\tno_ctrd\tsigma\tweight\tbeta_a \tbeta_b\tbeta_a_bias\tbeta_b_bias\tbeta_a_var\tbeta_b_var\tp_val_tr\ty_min\ty_max\tvar_max\n");
	//10-fold data set이기 때문에 10입니다.
	for (i_fold = 0; i_fold < TENFOLD; i_fold++)
	{
		debug = freopen("dbg.log", "a", debug);
		spsc = &db->vSampleSpace[i_fold];
		//여기서부턴 한 개의 fold마다, 한 개의 class마다의 작업입니다. 다른 표현으로 한 개의 train file마다의 작업입니다.
		readySampleSpaceLogFile(spsc);
		fisher_linear_discriminant(spsc);

		for (i_cls = 0; i_cls < n_cls; i_cls++)
		{
			clsc = &spsc->vClassSpace[i_cls];
			trsc = &clsc->trainspace;
			ttsc = &clsc->testspace;
			spacedim = clsc->vTrainSpaceDim;
			centroid = trsc->vCentroid;

			readySampleSpaceLogFile(spsc);
			//getLBGClusterSize(i_fold, i_cls, 1, db);
			for (i_ctrd = 0; i_ctrd < n_ctrd; i_ctrd++)
			{
				readyClassSpaceLogFile(clsc);
				ctrd_LBG_s = trainOneClass(i_fold, i_cls, i_ctrd, db);
				//printsamplespace(spsc);//loggin on stdin for checking sample space
//				getCPONPowerRatio(i_fold, i_cls, TRAIN, ctrd_LBG_s, db);
				finishClassSpaceLogFIle(clsc);
			}

//			i_opt = optPresidentCPON(i_fold, i_cls, TRAIN, ctrd_LBG_s, db);
//			printf("f=%d, c=%d, i_opt=%d, ctrd=%2d, pVal(tr) = %e, pVal(tt)=%e\n", i_fold, i_cls, i_opt, ctrd_LBG_s, db->vCentroid[i_fold][i_cls][TRAIN][i_opt].cpon_range[db->vCentroid[i_fold][i_cls][TRAIN][i_opt].cponindex].p_value_train, db->vCentroid[i_fold][i_cls][TRAIN][i_opt].cpon_range[db->vCentroid[i_fold][i_cls][TRAIN][i_opt].cponindex].p_value_trts);
			//setCPON(i_fold, i_cls, TRAIN, n_ctrd, ctrd_LBG_s, i_opt, db);
			//putchar('\n');
			//set centroid
			for (i = 0; i < n_cls; i++)
			{
				//read test data
				//classify test data

			}
			//get centroid parameters
			finishSampleSpaceLogFIle(spsc);
		}
	}
	//print result
	fclose(debug);

	closeDatabase(db);
	free(db);
	free(filepath);

	return 0;
}