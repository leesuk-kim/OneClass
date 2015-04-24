#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "vsheader.h"
#include "Utility.h"
#include "Oneclass.h"

//LBG Algorithm의 clustering 횟수입니다. 기존값은 20입니다. 10이 더 나을 수도 있습니다.
//const static int CLUSTERING_COUNT = 20;

#define row 2
#define col 2

int main(int argc, char *args[])
{
	int i, j;
	static int dim_s, cls_s = 2, ctrd_s = 5, mLBGCentroidSize;
	static int f_c, cls_c, ctrd_c;
	static int dat_s;
	int ctrd_LBG_s;
	int i_opt;
	oneclass_t *db;
	centroid_t *centroid;
	dimparam_t *dimparam;
	dataparam_t *dataparam;

	db = (oneclass_t*)calloc(1, sizeof(oneclass_t));
	dim_s = db->dimsize = read_data_dim_size(DATATYPE_TRAIN, 0, 0);
	db->ctrdsize = ctrd_s;

	openDatabase(db);
	mountData(db);
	readSeedRaw(db);

	printf("\nCLASSIFICATION START\n");
	fprintf(debug, "fold,cls,tt,clst,cand,no_ctrd,sigma,weight,beta_a ,beta_b,beta_a_bias,beta_b_bias,beta_a_var,beta_b_var,p_val_tr,y_min,y_max\n");
	//10-fold data set이기 때문에 10입니다.
	for (f_c = 0; f_c < TENFOLD; f_c++)
	{
		//여기서부턴 한 개의 fold마다, 한 개의 class마다의 작업입니다. 다른 표현으로 한 개의 train file마다의 작업입니다.
		for (cls_c = 0; cls_c < cls_s; cls_c++)
		{
			debug = freopen("dbg.log", "a", debug);
			centroid = db->table_centroid[f_c][cls_c][TRAIN];
			dimparam = db->table_dimparam[f_c][cls_c][TRAIN];
			dataparam = &db->table_dataparam[f_c][cls_c][TRAIN];

			setDimentionRange(f_c, cls_c, TRAIN, db);
			setDimentionRange(f_c, cls_c, TEST, db);

			readyFoldLogFile(f_c, db);

			//getLBGClusterSize(f_c, cls_c, TRAIN, 1, centroid, dimparam, dataparam);
			for (ctrd_c = 0; ctrd_c < ctrd_s; ctrd_c++)
			{
				readyFoldLogFile(f_c, db);
				ctrd_LBG_s = trainOneClass(f_c, cls_c, ctrd_c, db);
//				printDataparam(dataparam);
//				getCPONPowerRatio(f_c, cls_c, TRAIN, ctrd_LBG_s, db);
			}

			i_opt = optPresidentCPON(f_c, cls_c, TRAIN, ctrd_LBG_s, db);
			printf("f=%d, c=%d, i_opt=%d, ctrd=%2d, pVal(tr) = %e, pVal(tt)=%e\n", f_c, cls_c, i_opt, ctrd_LBG_s, db->table_centroid[f_c][cls_c][TRAIN][i_opt].cpon_range[db->table_centroid[f_c][cls_c][TRAIN][i_opt].cponindex].p_value_train, db->table_centroid[f_c][cls_c][TRAIN][i_opt].cpon_range[db->table_centroid[f_c][cls_c][TRAIN][i_opt].cponindex].p_value_trts);
			//setCPON(f_c, cls_c, TRAIN, ctrd_s, ctrd_LBG_s, i_opt, db);
			//putchar('\n');
			//set centroid
			for (i = 0; i < cls_s; i++)
			{
				//read test data
				//classify test data

			}
			//get centroid parameters
//			free(table_data);
		}
	}
	//print result
	fclose(debug);

	closeDatabase(db);
	free(db);

	return 0;
}