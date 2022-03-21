#include "algorithms/spea2/spea2.h"

#include <cmath>
#include <iostream>
#include <algorithm>

#include "core/macro.h"
#include "core/global.h"
#include "core/utility.h"
#include "core/tournament_selection.h"
#include "operator/sbx.h"
#include "operator/mutation.h"
#include "random/random.h"

namespace emoc {

	SPEA2::SPEA2(int thread_id) :Algorithm(thread_id)
	{
		real_popnum_ = g_GlobalSettings->population_num_;
	}

	SPEA2::~SPEA2()
	{

	}

	void SPEA2::Run()
	{
		Initialization();
		CalFitness(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);

		while (!IsTermination())
		{
			// generate offspring population
			Crossover(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->offspring_population_.data());
			MutationPop(g_GlobalSettings->offspring_population_.data(), 2 * g_GlobalSettings->population_num_ / 2, g_GlobalSettings);
			EvaluatePop(g_GlobalSettings->offspring_population_.data(), 2 * g_GlobalSettings->population_num_ / 2);
			MergePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_, g_GlobalSettings->offspring_population_.data(),
				2 * g_GlobalSettings->population_num_ / 2, g_GlobalSettings->mixed_population_.data());
			// select next generation's population
			EnvironmentalSelection(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->mixed_population_.data());
		}
	}

	void SPEA2::Initialization()
	{
		// initialize parent population
		g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		EvaluatePop(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);

		// initialize parameter
		para_k = (int)sqrt(2 * g_GlobalSettings->population_num_ / 2 + g_GlobalSettings->population_num_);
	}

	void SPEA2::Crossover(Individual **parent_pop, Individual **offspring_pop)
	{
		// generate random permutation index for tournment selection
		std::vector<int> index1(g_GlobalSettings->population_num_);
		std::vector<int> index2(g_GlobalSettings->population_num_);
		random_permutation(index1.data(), g_GlobalSettings->population_num_);
		random_permutation(index2.data(), g_GlobalSettings->population_num_);

		for (int i = 0; i < g_GlobalSettings->population_num_ / 2; ++i)
		{
			Individual *parent1 = TournamentByFitness(parent_pop[index1[2 * i]], parent_pop[index1[2 * i + 1]]);
			Individual *parent2 = TournamentByFitness(parent_pop[index2[2 * i]], parent_pop[index2[2 * i + 1]]);
			SBX(parent1, parent2, offspring_pop[2 * i], offspring_pop[2 * i + 1], g_GlobalSettings);
		}
	}

	void SPEA2::CalDistance(Individual **pop, int pop_num, double **distance,bool is_sort)
	{
		for (int i = 0; i < pop_num; ++i)
		{
			for (int j = 0; j < pop_num; ++j)
			{
				if (i == j)
				{
					distance[i][j] = EMOC_INF;
					continue;
				}

				distance[i][j] = CalEuclidianDistance(pop[i]->obj_, pop[j]->obj_, g_GlobalSettings->obj_num_);
			}

			if(is_sort)
				std::sort(distance[i], distance[i] + pop_num);
		}
	}

	void SPEA2::CalFitness(Individual **pop, int pop_num)
	{
		std::vector<int> dominate_num(pop_num);      //dominate_num[i] store the number of individuals which i-th ind dominates
		std::vector<int> dominated_size(pop_num);    //dominated_size[i] store the number of individuals which dominate i-th ind
		std::vector<std::vector<int>> dominated_index(pop_num, std::vector<int>(pop_num)); //dominated_index[i] store the index of individuals which dominate i-th ind

		// initialize 
		for (int i = 0; i < pop_num; ++i)
		{
			dominate_num[i] = 0;
			dominated_size[i] = 0;
		}

		// calculate R[i]
		for (int i = 0; i < pop_num; ++i)
		{
			for (int j = 0; j < pop_num; ++j)
			{
				if (i == j)
					continue;

				DominateReleation res = CheckDominance(pop[i], pop[j], g_GlobalSettings->obj_num_);
				if (res == DOMINATE)
				{
					dominate_num[i]++;
				}
				else if (res == DOMINATED)
				{
					dominated_index[i][dominated_size[i]++] = j;
				}
			}
		}

		for (int i = 0; i < pop_num; ++i)
		{
			if (0 == dominated_size[i])
			{
				pop[i]->fitness_ = 0;
			}
			else
			{
				for (int j = 0; j < dominated_size[i]; ++j)
				{
					int index = dominated_index[i][j];
					pop[i]->fitness_ += dominate_num[index];
				}
			}
		}

		// calculate D[i]
		double **distance = new double*[pop_num];
		for (int i = 0; i < pop_num; ++i)
			distance[i] = new double[pop_num];

		CalDistance(pop, pop_num, distance, true);
		for (int i = 0; i < pop_num; ++i)
			pop[i]->fitness_ += 1 / (distance[i][para_k] + 2);
		
		
		// deallocate
		for (int i = 0; i < pop_num; ++i)
			delete[] distance[i];
		delete[] distance;
	}

	std::unordered_map<int, int> SPEA2::TruncatePop(double **distance, int candidate_num)
	{
		std::vector<std::vector<double>> temp(candidate_num, std::vector<double>(candidate_num));
		std::unordered_map<int, int> delete_map;
		// truncate
		while (candidate_num - delete_map.size() > g_GlobalSettings->population_num_)
		{
			// create temp distance matrix according delete_map and original distance matrix
			for (int i = 0; i < candidate_num; ++i)
				for (int j = 0; j < candidate_num; ++j)
					temp[i][j] = distance[i][j];

			for (auto& p : delete_map)
			{
				int index_temp = p.first;
				for (int i = 0; i < candidate_num; ++i)
				{
					temp[index_temp][i] = EMOC_INF;
					temp[i][index_temp] = EMOC_INF;
				}
			}

			// sort the temp distance matrix
			for (int i = 0; i < candidate_num; ++i)
				std::sort(temp[i].begin(), temp[i].begin() + candidate_num);

			// sort temp distance matrix's rows and get index
			std::vector<int> sort_index(candidate_num);
			for (int i = 0; i < candidate_num; ++i)
				sort_index[i] = i;

			std::sort(sort_index.begin(), sort_index.end(), [=](int left, int right) {
				int index = 0;
				for (int i = 0; i < candidate_num; ++i)
				{
					if (fabs(temp[left][i] - temp[right][i]) > EMOC_EPS)
					{
						index = i;
						break;
					}
				}
				return temp[left][index] < temp[right][index];
			});

			delete_map[sort_index[0]] = 1;
		}

		return delete_map;
	}

	void SPEA2::EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop)
	{
		int candidate_num = 0;
		int mixed_popnum = 2 * g_GlobalSettings->population_num_ / 2 + g_GlobalSettings->population_num_;

		// count the candidates
		CalFitness(mixed_pop, mixed_popnum);
		for (int i = 0; i < mixed_popnum; ++i)
			if (mixed_pop[i]->fitness_ < 1)
				candidate_num++;

		if (candidate_num <= g_GlobalSettings->population_num_)
		{
			// copy directly according to fitness
			std::vector<int> sort_index(mixed_popnum);
			for (int i = 0; i < mixed_popnum; ++i)
				sort_index[i] = i;

			std::sort(sort_index.begin(), sort_index.end(), [mixed_pop](int left, int right) {
				return mixed_pop[left]->fitness_ < mixed_pop[right]->fitness_;
			});

			for (int i = 0; i < g_GlobalSettings->population_num_; ++i)
				CopyIndividual(mixed_pop[sort_index[i]], parent_pop[i]);
		}
		else
		{
			// truncate the popualtion
			int index = 0;
			std::vector<Individual*> truncate_pop(candidate_num, nullptr);
			for (int i = 0; i < mixed_popnum; ++i)
				if (mixed_pop[i]->fitness_ < 1)
					truncate_pop[index++] = mixed_pop[i];

			// calculate distance in truncate population
			double **distance = new double*[candidate_num];
			for (int i = 0; i < candidate_num; ++i)
				distance[i] = new double[candidate_num];
			CalDistance(truncate_pop.data(), candidate_num, distance, false);

			// get truncated individuals' index
			std::unordered_map<int, int> delete_map = TruncatePop(distance, candidate_num);
			
			// copy to next generation
			int count = 0;
			for (int i = 0; i < candidate_num; ++i)
			{
				if (delete_map.find(i) == delete_map.end())
				{
					CopyIndividual(truncate_pop[i], parent_pop[count++]);
				}
			}

			for (int i = 0; i < candidate_num; ++i)
				delete[] distance[i];
			delete[] distance;
		}

	}
}