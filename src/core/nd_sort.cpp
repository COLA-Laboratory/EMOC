#include "core/nd_sort.h"

#include <cstring>
#include <cstdlib>
#include <iostream>

#include "core/utility.h"

namespace emoc {

	void NonDominatedSort(Individual **pop, int pop_num, int obj_num, bool is_consider_cons)
	{
		int index = 0; 
		int dominate_relation = 0;
		int current_rank = 0, unrank_num = pop_num; 

		int *ni = nullptr;             // store the number of points that dominate i-th solution
		int **si = nullptr;            // store the solution index of which i-th solution dominates
		int	*Q = nullptr;              // store the solution which ni is 0
		int *dominate_num = nullptr;   // store the number of dominate points of i-th solution
		Individual *ind_tempA = nullptr, *ind_tempB = nullptr;

		ni = (int *)malloc(sizeof(int) * pop_num);
		memset(ni, 0, sizeof(int) * pop_num);

		si = (int **)malloc(sizeof(int *) * pop_num);
		for (int i = 0; i < pop_num; i++)
		{
			si[i] = (int *)malloc(sizeof(int) * pop_num);
			memset(si[i], 0, sizeof(int) * pop_num);
		}

		Q = (int *)malloc(sizeof(int) * pop_num);
		memset(Q, 0, sizeof(int) * pop_num);

		dominate_num = (int*)malloc(sizeof(int) * pop_num);
		memset(dominate_num, 0, sizeof(int) * pop_num);

		for (int i = 0; i < pop_num; i++)
		{
			ind_tempA = pop[i];
			index = 0;
			for (int j = 0; j < pop_num; j++)
			{
				if (i == j)
					continue;

				ind_tempB = pop[j];
				dominate_relation = is_consider_cons ? CheckDominanceWithConstraint(ind_tempA, ind_tempB, obj_num) 
					:CheckDominance(ind_tempA, ind_tempB, obj_num);
				if (1 == dominate_relation)
				{
					si[i][index++] = j;
				}
				else if (-1 == dominate_relation)
				{
					ni[i]++;
				}
				else;
			}
			dominate_num[i] = index;
		}

		while (unrank_num)
		{
			index = 0;
			for (int i = 0; i < pop_num; i++)
			{
				if (ni[i] == 0)
				{
					pop[i]->rank_ = current_rank;
					Q[index++] = i;
					unrank_num--;
					ni[i] = -1;
				}
			}
			current_rank++;
			for (int i = 0; i < index; i++)
			{
				for (int j = 0; j < dominate_num[Q[i]]; j++)
				{
					ni[si[Q[i]][j]]--;
				}
			}
		}

		for (int i = 0; i < pop_num; i++)
			free(si[i]);
		free(si);
		free(ni);
		free(Q);
		free(dominate_num);
	}

}