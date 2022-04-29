#pragma once
#define BEATS(x, y)   (x > y)
#define WORSE(x, y)   (BEATS(y, x) ? (x) : (y))
#define MIN(a, b) (a < b ? (a) : (b))
#define MAX(a, b) (a > b ? (a) : (b))
#define SLICELIMIT 5

#include <string>

#include "core/individual.h"

namespace emoc {


	typedef double OBJECTIVE;

	typedef struct
	{
		OBJECTIVE* objectives;
	} POINT;

	typedef struct
	{
		int nPoints;
		int n;
		POINT* points;
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
		FRONT* fronts;
	} FILECONTENTS;


	typedef struct
	{
		double width;
		FRONT front;
		int index;
	} SLICE;

	class HVCalculator
	{
	public:
		HVCalculator();
		HVCalculator(int obj_num, int pop_num);
		~HVCalculator();

		void Init(int obj_num, int pop_num);
		void CleanUp();
		double Calculate(Individual** pop, int pop_num, int obj_num, double **pf_data, int pf_size);
		void MinExclusiveHV2(FRONT ps, double* min);
		void MinExclusiveHV(FRONT ps, double* min);
		void ReadData(FILECONTENTS* fc, emoc::Individual** pop_table, double* nadir_point, int pop_size, int obj_num);

	private:
		int i_greaterabbrevorder(const void* v1, const void* v2);
		int i_greaterabbrev(const void* v1, const void* v2);
		int i_greaterorder(const void* v1, const void* v2);
		int i_greater(const void* v1, const void* v2);
		int i_sorter(const void* a, const void* b);
		int m_greaterabbrevorder(const POINT& p, const POINT& q);
		int m_greaterabbrev(const POINT& p, const POINT& q);
		int m_greaterorder(const POINT& p, const POINT& q);
		int m_greater(const POINT& p, const POINT& q);
		int m_sorter(const int& i, const int& j);
		
		int i_same(const void* v1, const void* v2);
		int i_dominates2way(POINT p, POINT q, int k);
		int i_dominates1way(POINT p, POINT q, int k);
		int i_dominates1wayOrder(POINT p, POINT q, int k, int* order);
		void i_removeDominated(int l, int limit);
		void i_makeDominatedBit(FRONT ps, int p);
		double i_hv2(FRONT ps, int k);
		double i_inclhv(POINT p);
		double i_inclhvOrder(POINT p, int* order);
		double i_inclhv2(POINT p, POINT q);
		double i_inclhv3(POINT p, POINT q, POINT r);
		double i_inclhv4(POINT p, POINT q, POINT r, POINT s);
		double i_inclhv5(POINT p, POINT q, POINT r, POINT s, POINT t);
		double i_exclhv(FRONT ps, int p);
		double i_hv_contribution(FRONT ps, int id, double whole_hv);
		double i_hv(FRONT ps);
		void i_makeDominatedBitPoint(FRONT ps, POINT p, int* order);
		double i_exclhvPoint(FRONT ps, POINT p, int* order);
		void i_heapify(int location, int index);
		int i_peekFromHeap(void);
		void i_initialiseHeap(int capacity);
		void i_insert(POINT p, int k, FRONT pl, int i, int j, int* order);
		void i_sliceOrder(int nPoints);
		void i_slice(FRONT pl);
		int i_binarySearch(POINT p, int d);
		void i_runHeuristic(FRONT ps);
		int i_slicingDepth(int d);

	private:
		int i_fr = 0;		// current depth
		int i_n;			// the number of objectives
		POINT i_ref;		// the reference point
		POINT i_dirs;		// records the directions of the objectives

		FRONT* i_fs;		// memory management stuff

		int i_maxm;			// maxmimum number of points
		int i_maxn;			// maximum number of objectives
		int i_safe;
		int objective_num;

		double* partial;	//partial exclusive hypervolumes
		int* heap;			//heap-based priority queue
		int heapsize;		//number of points in queue
		SLICE** stacks;		//set of slices per point per slicing depth
		int* stacksize;		//current slicing depth per point

		int* gorder;		//objective order used by comparison functions
		int** torder;		//order of objectives per point
		int** tcompare;
		FRONT* fsorted;		//front sorted in each objective

		int obj_num_;
		int pop_num_;
	};

}