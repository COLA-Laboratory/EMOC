#include "algorithm/smsemoa/smsemoa.h"

#include <cstring>
#include <cmath>
#include <vector>

#include "core/global.h"
#include "core/nd_sort.h"
#include "core/utility.h"
#include "operator/polynomial_mutation.h"
#include "operator/sbx.h"
#include "random/random.h"

namespace emoc {

	SMSEMOA::SMSEMOA(int thread_id) :
		Algorithm(thread_id),
		nadir_point_(new double[g_GlobalSettings->obj_num_])
	{
		real_popnum_ = g_GlobalSettings->population_num_;
	}

	SMSEMOA::~SMSEMOA()
	{
		delete[] nadir_point_;
	}

	void SMSEMOA::Solve()
	{
		Initialization();
		Individual *offspring = g_GlobalSettings->offspring_population_[0];

		while (!IsTermination())
		{
			for (int i = 0; i < g_GlobalSettings->population_num_; ++i)
			{
				// generate offspring for current subproblem
				Crossover(g_GlobalSettings->parent_population_.data(), offspring);
				PolynomialMutation(offspring, g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, mutation_para_);
				EvaluateInd(offspring);

				// update nadir point
				UpdateNadirpoint(offspring, nadir_point_, g_GlobalSettings->obj_num_);

				// environmental selection
				EnvironmentalSelection(g_GlobalSettings->parent_population_.data(), offspring);
			}
		}
	}

	void SMSEMOA::Initialization()
	{
		// initialize parent population
		g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		EvaluatePop(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);

		// initialize hv calculator
		hv_calculator_.Init(g_GlobalSettings->obj_num_, g_GlobalSettings->population_num_);

		// initialize nadir point
		UpdateNadirpoint(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_, nadir_point_, g_GlobalSettings->obj_num_);

		// set mutation parameter
		mutation_para_.pro = 1.0 / g_GlobalSettings->dec_num_;
		mutation_para_.index1 = 20.0;

		// set crossover parameter
		cross_para_.pro = 1.0;
		cross_para_.index1 = 20.0;
	}

	void SMSEMOA::Crossover(Individual **parent_pop, Individual *offspring)
	{
		std::vector<int> index(g_GlobalSettings->population_num_);
		random_permutation(index.data(), g_GlobalSettings->population_num_);

		Individual *parent1 = parent_pop[index[0]];
		Individual *parent2 = parent_pop[index[1]];
		Individual **offspring_pop = g_GlobalSettings->offspring_population_.data();
		SBX(parent1, parent2, offspring_pop[1], offspring_pop[2],
			g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, cross_para_);

		int res = CheckDominance(offspring_pop[1], offspring_pop[2], g_GlobalSettings->obj_num_);
		if (-1 == res)
			CopyIndividual(offspring_pop[2], offspring);
		else
			CopyIndividual(offspring_pop[1], offspring);
	}

	int SMSEMOA::FindMinVolumeIndex(Individual **pop, int pop_num)
	{
		FILECONTENTS *f = (FILECONTENTS *)malloc(sizeof(FILECONTENTS));
		double *min = NULL;
		int i = 0, j = 0, num_same = 0;

		min = (double *)malloc(sizeof(double) * (g_GlobalSettings->obj_num_ + 2));
		hv_calculator_.ReadData(f, pop, nadir_point_, pop_num, g_GlobalSettings->obj_num_);

		if (g_GlobalSettings->obj_num_ == 2)
		{
			hv_calculator_.MinExclusiveHV2(f->fronts[0], min);
		}
		else
		{
			hv_calculator_.MinExclusiveHV(f->fronts[0],  min);
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

		// free the data
		for (i = 0; i < pop_num; i++)
			free(f->fronts[0].points[i].objectives);
		free(f->fronts[0].points);
		free(f->fronts);
		free(f);

		return j == pop_num? j - 1: j;
	}

	void SMSEMOA::EnvironmentalSelection(Individual **parent_pop, Individual *offspring)
	{
		// allocate memory for restoring each front index
		std::vector<std::vector<int>> front(g_GlobalSettings->population_num_ + 1, std::vector<int>(g_GlobalSettings->population_num_ + 1));
		std::vector<int> front_size(g_GlobalSettings->population_num_ + 1, 0);

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
	}

}