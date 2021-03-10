#ifndef _WFG_H_
#define _WFG_H_

# include <stdio.h>
# include <stdlib.h>
# include <string.h>

#include "core/individual.h"

#define BEATS(x, y)   (x > y)
#define WORSE(x, y)   (BEATS(y, x) ? (x) : (y))
#define MIN(a, b) (a < b ? (a) : (b))
#define MAX(a, b) (a > b ? (a) : (b))
#define SLICELIMIT 5
typedef double OBJECTIVE;

typedef struct
{
	OBJECTIVE *objectives;
} POINT;

typedef struct
{
	int nPoints;
	int n;
	POINT *points;
} FRONT;

typedef struct
{
	FRONT sprime;   // reduced front
	int id;         // index in the original list
	int k;          // next segment to be evaluated
	double partial; // volume so far
	int left;       // left child in the heap
	int right;      // right child in the heap
} JOB;

typedef struct
{
	int nFronts;
	FRONT *fronts;
} FILECONTENTS;


typedef struct
{
	double width;
	FRONT front;
	int index;
} SLICE;

extern int i_n;     // the number of objectives
extern POINT i_ref; // the reference point
extern POINT i_dirs;// records the directions of the objectives

extern FRONT *i_fs;    // memory management stuff

extern int i_maxm; // maxmimum number of points
extern int i_maxn; // maximum number of objectives

extern double* partial; //partial exclusive hypervolumes
extern int* heap; //heap-based priority queue
extern int heapsize; //number of points in queue
extern SLICE **stacks; //set of slices per point per slicing depth
extern int *stacksize; //current slicing depth per point

extern int* gorder; //objective order used by comparison functions
extern int** torder; //order of objectives per point
extern int** tcompare;
extern FRONT* fsorted; //front sorted in each objective

void i_printContents(FILECONTENTS *f);
void free_file_content(FILECONTENTS *fc);

double i_hv_contribution(FRONT ps, int id, double whole);
double i_hv(FRONT ps);
double i_hv2(FRONT ps, int k);
int i_slicingDepth(int d);
void i_ihv(FRONT ps, double *min);
void i_ihv2(FRONT ps, double *min);
double i_inclhvOrder(POINT p, int *order);
double i_inclhv(POINT p);
double i_inclhv2(POINT p, POINT q);
double i_inclhv3(POINT p, POINT q, POINT r);
double i_inclhv4(POINT p, POINT q, POINT r, POINT s);
double i_inclhv5(POINT p, POINT q, POINT r, POINT s, POINT t);
void i_runHeuristic(FRONT ps);
int i_binarySearch(POINT p, int d);
void i_slice(FRONT pl);
void i_sliceOrder(int nPoints);
void i_insert(POINT p, int k, FRONT pl, int i, int j, int *order);
void i_initialiseHeap(int capacity);
int i_peekFromHeap(void);
void i_heapify(int location, int index);
double i_exclhvPoint(FRONT ps, POINT p, int* order);
void i_makeDominatedBitPoint(FRONT ps, POINT p, int* order);
double i_exclhv(FRONT ps, int p);
void i_makeDominatedBit(FRONT ps, int p);
void i_removeDominated(int l, int limit);
int i_dominates1wayOrder(POINT p, POINT q, int k, int* order);
int i_dominates1way(POINT p, POINT q, int k);
int i_dominates2way(POINT p, POINT q, int k);
int i_greaterabbrevorder(const void *v1, const void *v2);
int i_greaterabbrev(const void *v1, const void *v2);
int i_greaterorder(const void *v1, const void *v2);
int i_same(const void *v1, const void *v2);
int i_greater(const void *v1, const void *v2);
int i_sorter(const void *a, const void *b);
void iwfg_read_data(FILECONTENTS *fc, emoc::Individual **pop_table, double *nadir_point, int pop_size, int obj_num);
#endif