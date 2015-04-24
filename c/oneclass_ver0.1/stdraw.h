/*
This header file works for various types of sourse file which have raw data
splitted comma(.csv), tab(.xls), whitespace.

source designer : leesuk kim. SungKyunKwan Univ., suwon, korea.
first file create : 2015.04.16
last update : 2015.04.16
*/
#include <stdio.h>
#include<stdlib.h>

#define DBCB_DAT 1
#define DBCB_CSV 2
#define DBCB_XLS 3

struct struct_DBCB
{
	void (*readdata) ();
	int(*read_rsize) ();
	int(*read_dsize) ();
	char* rootdir;
	int fold;
	int cls;
};

typedef struct struct_DBCB dbcb_t;

void init_dbcb(int dbtype, int dbdir, dbcb_t *dbcb);

void tab_readdata();
//read record size
int tab_read_rsize();
//read dimension size
int tab_read_dsize();
void csv_readdata();
//read record size
int csv_read_rsize();
//read dimension size
int csv_read_dsize();
void dat_read_data();
//read record size
int dat_read_rsize();
//read dimension size
int dat_read_dsize();