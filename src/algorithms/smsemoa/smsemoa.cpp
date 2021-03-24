#include "algorithms/smsemoa/smsemoa.h"

#include <cmath>
#include <vector>

#include "core/global.h"
#include "core/nd_sort.h"
#include "core/utility.h"
#include "operator/mutation.h"
#include "operator/sbx.h"
#include "random/random.h"
#include "wfg/iwfg.h"
#include "metric/hv.h"

namespace emoc {

	SMSEMOA::SMSEMOA(Problem *problem, int thread_num):
		Algorithm(problem, thread_num),
		nadir_point_(new double[problem->obj_num_])
	{

	}

	SMSEMOA::~SMSEMOA()
	{
		// free memory for iwfg algorithm
		for (int i = 0; i < g_GlobalSettings->population_num_ + 1; i++)
			free(stacks[i]);

		free(partial);
		free(heap);
		free(stacksize);
		free(stacks);

		for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
			free(fsorted[i].points);
		free(fsorted);
		for (int i = 0; i < MAX(g_GlobalSettings->obj_num_, g_GlobalSettings->population_num_ + 1); i++)
			free(torder[i]);
		for (int i = 0; i <  g_GlobalSettings->population_num_ + 1; i++)
			free(tcompare[i]);

		free(torder);
		free(tcompare);

		delete[] nadir_point_;
	}

	void SMSEMOA::Run()
	{
		Initialization();
		Individual *offspring = g_GlobalSettings->offspring_population_[0];
		while (!g_GlobalSettings->IsTermination())
		{
			// begin each iteration
			g_GlobalSettings->iteration_num_++;

			for (int i = 0; i < g_GlobalSettings->population_num_; ++i)
			{
				// generate offspring for current subproblem
				Crossover(g_GlobalSettings->parent_population_.data(), offspring);
				MutationInd(offspring, g_GlobalSettings);
				EvaluateInd(offspring);

				// update nadir point
				UpdateNadirpoint(offspring, nadir_point_, g_GlobalSettings->obj_num_);

				// environmental selection
				EnvironmentalSelection(g_GlobalSettings->parent_population_.data(), offspring);
			}

			// record the population every interval generations and the first and last genration 
			if (g_GlobalSettings->iteration_num_ % g_GlobalSettings->output_interval_ == 0 || g_GlobalSettings->iteration_num_ == 1
				|| g_GlobalSettings->IsTermination())
				TrackPopulation(g_GlobalSettings->iteration_num_);
		}
	}

	void SMSEMOA::Initialization()
	{
		// initialize parent population
		g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		EvaluatePop(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);

		// initialize nadir point
		UpdateNadirpoint(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_, nadir_point_, g_GlobalSettings->obj_num_);

		// preparation for IWFG algorithm, which is used for calculating the individual Hypervolume contribution
		i_maxn = g_GlobalSettings->obj_num_;
		i_maxm = g_GlobalSettings->population_num_ + 1;
		int max_depth = i_maxn - 2;
		if(max_depth > 0)
			i_fs		= (FRONT*)malloc(sizeof(FRONT) * max_depth);
		partial		= (double*)malloc(sizeof(double) * i_maxm);
		heap		= (int*)malloc(sizeof(int) * i_maxm);
		stacksize	= (int*)malloc(sizeof(int) * i_maxm);
		stacks		= (SLICE**)malloc(sizeof(SLICE*) * i_maxm);
		fsorted		= (FRONT*)malloc(sizeof(FRONT) * i_maxn);
		torder		= (int**)malloc(sizeof(int *) * MAX(i_maxm, i_maxn));
		tcompare	= (int**)malloc(sizeof(int *) * i_maxm);
		int max_stacksize = MIN(i_maxn - 2, i_slicingDepth(i_maxn)) + 1;

		if (max_depth > 0)
		{
			for (int i = 0; i < max_depth; i++) {
				i_fs[i].points = (POINT*)malloc(sizeof(POINT) * i_maxm);
				for (int j = 0; j < i_maxm; j++) {
					i_fs[i].points[j].objectives = (OBJECTIVE*)malloc(sizeof(OBJECTIVE) * (i_maxn - i - 1));
				}
			}
		}

		for (int i = 0; i < i_maxm; i++)
		{
			stacks[i] = (SLICE*)malloc(sizeof(SLICE) * max_stacksize);
			for (int j = 1; j < max_stacksize; j++)
				stacks[i][j].front.points = (POINT*)malloc(sizeof(POINT) * i_maxm);
		}
		for (int i = 0; i < i_maxn; i++)
			fsorted[i].points = (POINT*)malloc(sizeof(POINT) * i_maxm);
		for (int i = 0; i < MAX(i_maxn, i_maxm); i++)
			torder[i] = (int*)malloc(sizeof(int) * i_maxn);
		for (int i = 0; i < i_maxm; i++)
			tcompare[i] = (int*)malloc(sizeof(int) * i_maxn);
	}

	void SMSEMOA::Crossover(Individual **parent_pop, Individual *offspring)
	{
		int *index = new int[g_GlobalSettings->population_num_];
		random_permutation(index, g_GlobalSettings->population_num_);

		Individual *parent1 = parent_pop[index[0]];
		Individual *parent2 = parent_pop[index[1]];
		Individual **offspring_pop = g_GlobalSettings->offspring_population_.data();
		SBX(parent1, parent2, offspring_pop[1], offspring_pop[2], g_GlobalSettings);

		DominateReleation res = CheckDominance(offspring_pop[1], offspring_pop[2], g_GlobalSettings->obj_num_);
		if (DOMINATED == res)
			CopyIndividual(offspring_pop[2], offspring);
		else
			CopyIndividual(offspring_pop[1], offspring);

		delete[] index;
	}

	int SMSEMOA::FindMinVolumeIndex(Individual **pop, int pop_num)
	{
		FILECONTENTS f;
		double *min = NULL;
		int i = 0, j = 0, num_same = 0;

		min = (double *)malloc(sizeof(double) * (g_GlobalSettings->obj_num_ + 2));
		i_n = g_GlobalSettings->obj_num_;
		iwfg_read_data(&f, pop, nadir_point_, pop_num, g_GlobalSettings->obj_num_);

		if (g_GlobalSettings->obj_num_ == 2)
		{
			i_ihv2(f.fronts[0], min);
		}
		else
		{
			i_ihv(f.fronts[0], min);
		}

		// find the ind with minimal hv contribution
		for (j = 0; j < pop_num; j++)
		{
			num_same = 0;
			for (i = 0; i < g_GlobalSettings->obj_num_; i++)
			{
				if ((fabs((nadir_point_[i] - min[i]) - pop[j]->obj_[i]) < 1e-4))
					num_same++;
			}
			if (num_same == g_GlobalSettings->obj_num_)
				break;
		}

		free(min);
		return j;
	}

	void SMSEMOA::EnvironmentalSelection(Individual **parent_pop, Individual *offspring)
	{
		// allocate memory for restoring each front index
		int **front = new int*[g_GlobalSettings->population_num_ + 1];
		for (int i = 0; i < g_GlobalSettings->population_num_ + 1; i++)
		{
			front[i] = new int[g_GlobalSettings->population_num_ + 1];
		}

		int *front_size = new int[g_GlobalSettings->population_num_ + 1];
		memset(front_size, 0, g_GlobalSettings->population_num_ + 1);

		// nds
		for (int i = 0; i < g_GlobalSettings->population_num_; i++)
		{
			CopyIndividual(parent_pop[i], g_GlobalSettings->mixed_population_[i]);
		}
		CopyIndividual(offspring, g_GlobalSettings->mixed_population_[g_GlobalSettings->population_num_]);
		NonDominatedSort(g_GlobalSettings->mixed_population_.data(), g_GlobalSettings->population_num_ + 1, g_GlobalSettings->obj_num_);

		// restore each front's index
		for (int i = 0; i < g_GlobalSettings->population_num_ + 1; i++)
		{
			int current_rank = g_GlobalSettings->mixed_population_[i]->rank_;
			front[current_rank][front_size[current_rank]++] = i;
		}

		// calculate the number of front
		int front_num = 0;
		for (int i = 0; i < g_GlobalSettings->population_num_ + 1; i++)
			if (front_size[i])
				front_num++;

		// copy individual to next generation
		int archive_num = 0;
		int last_front_index = -1;
		for (int i = 0; i < front_num; ++i)
		{
			last_front_index = i;
			if (archive_num + front_size[i] <= g_GlobalSettings->population_num_)
			{
				for (int j = 0; j < front_size[i]; ++j)
				{
					CopyIndividual(g_GlobalSettings->mixed_population_[front[i][j]],  parent_pop[archive_num]);
					archive_num += 1;
				}
			}
			else
			{
				break;
			}
		}

		// copy the remaining individual if necessary
		if (front_size[last_front_index] != 1)
		{
			int temp_popnum = 0, min_hv_index = 0;
			std::vector<Individual*> temp_pop;

			for (int i = 0; i < g_GlobalSettings->population_num_ + 1; ++i)
				temp_pop.push_back(new Individual(g_GlobalSettings->dec_num_, g_GlobalSettings->obj_num_));

			for (int j = 0; j < front_size[last_front_index]; j++)
			{
				CopyIndividual(g_GlobalSettings->mixed_population_[front[last_front_index][j]], temp_pop[temp_popnum]);
				temp_popnum++;
			}

			min_hv_index = FindMinVolumeIndex(temp_pop.data(), temp_popnum);

			for (int i = 0; i < temp_popnum; i++)
			{
				if (i == min_hv_index)
				{
					continue;
				}
				CopyIndividual(temp_pop[i], parent_pop[archive_num]);
				archive_num++;
			}

			for (int i = 0; i < g_GlobalSettings->population_num_ + 1; ++i)
			{
				delete temp_pop[i];
				temp_pop[i] = nullptr;
			}
		}

		delete[] front_size;
		for (int i = 0; i < g_GlobalSettings->population_num_ + 1; i++)
			delete[] front[i];
		delete[] front;
	}

}