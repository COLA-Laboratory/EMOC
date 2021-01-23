#include "algorithms/nsga2.h"

#include <algorithm>
#include <vector>
#include <iostream>

#include "core/global.h"
#include "core/nd_sort.h"
#include "core/tournament_selection.h"
#include "operator/mutation.h"
#include "operator/sbx.h"
#include "random/random.h"

namespace emoc {

	NSGA2::NSGA2(Problem *problem):Algorithm(problem)
	{

	}

	NSGA2::~NSGA2()
	{

	}

	void NSGA2::Run()
	{
		Initialization();
		while (!g_GlobalSettings->IsTermination())
		{
			Crossover(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->offspring_population_.data());
			MutationPop(g_GlobalSettings->offspring_population_.data(), 2 * g_GlobalSettings->population_num_ / 2);
			EvaluatePop(g_GlobalSettings->offspring_population_.data(), 2 * g_GlobalSettings->population_num_ / 2);
			MergePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_, g_GlobalSettings->offspring_population_.data(), 
				2 * g_GlobalSettings->population_num_ / 2, g_GlobalSettings->mixed_population_.data());
			EnvironmentalSelection(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->mixed_population_.data());
		}
	}

	void NSGA2::Initialization()
	{
		g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		EvaluatePop(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
	}

	void NSGA2::Crossover(Individual **parent_pop, Individual **offspring_pop)
	{
		int *index1 = new int[g_GlobalSettings->population_num_];
		int *index2 = new int[g_GlobalSettings->population_num_];
		random_permutation(index1, g_GlobalSettings->population_num_);
		random_permutation(index2, g_GlobalSettings->population_num_);

		for (int i = 0; i < g_GlobalSettings->population_num_ / 2; ++i)
		{
			Individual *parent1 = TournamentByRank(parent_pop[index1[2 * i]], parent_pop[index1[2 * i + 1]]);
			Individual *parent2 = TournamentByRank(parent_pop[index2[2 * i]], parent_pop[index2[2 * i + 1]]);
			SBX(parent1, parent2, offspring_pop[2 * i], offspring_pop[2 * i + 1]);
		}
		
		delete[] index1;
		delete[] index2;
	}
	
	void NSGA2::SetDistanceInfo(std::vector<DistanceInfo> &distanceinfo_vec, int target_index, double distance)
	{
		for (int i = 0; i < distanceinfo_vec.size(); ++i)
		{
			if (distanceinfo_vec[i].index == target_index)
			{
				distanceinfo_vec[i].distance += distance;
				break;
			}
		}
	}


	int NSGA2::CrowdingDistance(Individual **mixed_pop,  int pop_num, int *pop_sort, int rank_index)
	{
		int num_in_rank = 0;
		std::vector<int> sort_arr(pop_num, 0);
		std::vector<DistanceInfo> distanceinfo_vec(pop_num, { -1,0.0 });

		/*�ҳ����ж�Ӧrank��ֵ*/
		for (int i = 0; i < pop_num; i++)
		{
			mixed_pop[i]->fitness_ = 0;
			if (mixed_pop[i]->rank_ == rank_index)
			{
				distanceinfo_vec[num_in_rank].index = i;
				sort_arr[num_in_rank] = i;
				num_in_rank++;
			}
		}

		for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
		{
			sort(sort_arr.begin(), sort_arr.begin()+num_in_rank, [=](int left, int right){
				return mixed_pop[left]->obj_[i] < mixed_pop[right]->obj_[i];
			});

			/*��һ�������һ����ֵΪ�����Ϊ��ʹ���ܹ���������*/
			mixed_pop[sort_arr[0]]->fitness_ = INF;
			SetDistanceInfo(distanceinfo_vec, sort_arr[0], INF);
			mixed_pop[sort_arr[num_in_rank - 1]]->fitness_ = INF;
			SetDistanceInfo(distanceinfo_vec, sort_arr[num_in_rank - 1], INF);

			for (int j = 1; j < num_in_rank - 1; j++)
			{
				if (INF != mixed_pop[sort_arr[j]]->fitness_)
				{
					if (mixed_pop[sort_arr[num_in_rank - 1]]->obj_[i] == mixed_pop[sort_arr[0]]->obj_[i])
					{
						mixed_pop[sort_arr[j]]->fitness_ += 0;
					}
					else
					{
						mixed_pop[sort_arr[j]]->fitness_ += (mixed_pop[sort_arr[j + 1]]->obj_[i] - mixed_pop[sort_arr[j - 1]]->obj_[i]) / 
							(mixed_pop[sort_arr[num_in_rank - 1]]->obj_[i] - mixed_pop[sort_arr[0]]->obj_[i]);
						SetDistanceInfo(distanceinfo_vec, sort_arr[j], mixed_pop[sort_arr[j]]->fitness_);
					}
				}
			}
		}

		sort(distanceinfo_vec.begin(), distanceinfo_vec.begin()+num_in_rank, [](DistanceInfo &left, DistanceInfo &right) {
			return left.distance < right.distance;
		});

		for (int i = 0; i < num_in_rank; i++)
		{
			pop_sort[i] = distanceinfo_vec[i].index;
		}

		return num_in_rank;
	}

	void NSGA2::EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop)
	{
	
		int current_popnum = 0, rank_index = 0;
		int mixed_popnum = g_GlobalSettings->population_num_ + 2 * g_GlobalSettings->population_num_ / 2;

		NonDominatedSort(mixed_pop, mixed_popnum);

		while (1)
		{
			int temp_number = 0;
			for (int i = 0; i < mixed_popnum; i++)
			{
				if (mixed_pop[i]->rank_ == rank_index)
				{
					temp_number++;
				}
			}
			if (current_popnum + temp_number <= g_GlobalSettings->population_num_)
			{
				for (int i = 0; i < mixed_popnum; i++)
				{
					if (mixed_pop[i]->rank_ == rank_index)
					{
						CopyIndividual(mixed_pop[i], parent_pop[current_popnum]);
						current_popnum++;
					}
				}
				rank_index++;
			}
			else
				break;
		}

		int sort_num = 0;
		int *pop_sort = new int[mixed_popnum];

		if (current_popnum < g_GlobalSettings->population_num_)
		{
			sort_num = CrowdingDistance(mixed_pop, mixed_popnum, pop_sort, rank_index);
			while (1)
			{
				if (current_popnum < g_GlobalSettings->population_num_)
				{
					int index = pop_sort[sort_num - 1];
					CopyIndividual(mixed_pop[pop_sort[--sort_num]], parent_pop[current_popnum]);
					current_popnum++;
				}
				else {
					break;
				}
			}
		}
		for (int i = 0; i < g_GlobalSettings->population_num_; i++)
		{
			parent_pop[i]->fitness_ = 0;
		}

		delete[] pop_sort;
	}

}