#include "core/nd_sort.h"

#include <cstdlib>
#include <iostream>

#include "core/utility.h"

namespace emoc {

	void NonDominatedSort(Individual **pop, int pop_num)
	{
		int index = 0; /*��ʱ������*/
		int current_rank = 0, unrank_num = pop_num; /*rank���ڵȼ���ֵ��unrank_num�����ж��Ƿ�ֹͣѭ��*/
		int dominate_relation = 0;
		int *ni = nullptr, **si = nullptr, *Q = nullptr;/*ni���ڱ�ʾ֧���i��solution�Ľ�ĸ�����si��һ�����ϣ���ŵ�i��Ԫ��֧��Ľ�,Q�������ڴ�ŵ�ǰniΪ0��solution*/
		int *dominate_num = nullptr;   /*���ڴ洢I֧��Ľ�ĸ���*/
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
				dominate_relation = CheckDominance(ind_tempA, ind_tempB);
				if (DOMINATE == dominate_relation)
				{
					/*I֧��J*/
					si[i][index++] = j;

				}
				else if (DOMINATED == dominate_relation)/*J֧��I*/
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

		free(ni);
		for (int i = 0; i < pop_num; i++)
		{
			free(si[i]);
		}
		free(si);
		free(Q);
		free(dominate_num);
		return;
	}

}