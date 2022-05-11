#include "algorithm/tdea/tdea.h"

#include <vector>
#include <algorithm>
#include <iostream>

#include "core/macro.h"
#include "core/uniform_point.h"
#include "core/utility.h"
#include "core/global.h"
#include "core/nd_sort.h"
#include "operator/tournament_selection.h"
#include "operator/polynomial_mutation.h"
#include "operator/sbx.h"
#include "random/random.h"

namespace emoc {

	tDEA::tDEA(int thread_id) :Algorithm(thread_id),
		ideal_point_(new double[g_GlobalSettings->obj_num_]),
		nadir_point_(new double[g_GlobalSettings->obj_num_])
	{
		real_popnum_ = g_GlobalSettings->population_num_;
	}

	tDEA::~tDEA()
	{
		delete[] ideal_point_;
		delete[] nadir_point_;
		delete[] cluster_count_;

		for (int i = 0; i < weight_num_; i++)
		{
			delete[] cluster_[i];
			delete ndpop_[i];
			delete ndpop_[i + weight_num_];
			delete extreme_pop_[i];
			cluster_[i] = nullptr; 
			ndpop_[i] = nullptr;
			extreme_pop_[i] = nullptr;
		}
		delete[] cluster_;

		ideal_point_ = nullptr;
		nadir_point_ = nullptr;
		cluster_count_ = nullptr;
		cluster_ = nullptr;
	}

	void tDEA::Solve()
	{
		Initialization();

		while (!IsTermination())
		{
			// generate offspring population
			Crossover(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->offspring_population_.data());
			PolynomialMutation(g_GlobalSettings->offspring_population_.data(), 2 * (weight_num_ / 2), g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, mutation_para_);
			EvaluatePop(g_GlobalSettings->offspring_population_.data(), 2 * (weight_num_ / 2));
			MergePopulation(g_GlobalSettings->parent_population_.data(), weight_num_, g_GlobalSettings->offspring_population_.data(),
				2 * (weight_num_  / 2), g_GlobalSettings->mixed_population_.data());

			// select next generation's population
			EnvironmentalSelection(g_GlobalSettings->parent_population_.data(), 2 * (weight_num_ / 2)+weight_num_,  g_GlobalSettings->mixed_population_.data());
		}
	}

	void tDEA::Initialization()
	{
		// initialize parent population
		g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		EvaluatePop(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);

		// generate weight vectors
		lambda_ = UniformPoint(g_GlobalSettings->population_num_, &weight_num_, g_GlobalSettings->obj_num_);
		real_popnum_ = weight_num_;

		// initialize ideal point and nadir point
		UpdateIdealpoint(g_GlobalSettings->parent_population_.data(), weight_num_, ideal_point_, g_GlobalSettings->obj_num_);
		UpdateNadirpoint(g_GlobalSettings->parent_population_.data(), weight_num_, nadir_point_, g_GlobalSettings->obj_num_);

		// initialize other datas for tDEA
		cluster_count_ = new int[weight_num_];
		cluster_ = new int* [weight_num_];
		for (int i = 0; i < weight_num_; i++)
		{
			// cluster and cluster_count
			cluster_[i] = new int[weight_num_*2];
			for (int j = 0; j < weight_num_; j++)
				cluster_[i][j] = -1;
			cluster_count_[i] = 0;
		
			// ndpop - 2 * weightnum
			ndpop_.push_back(new Individual(g_GlobalSettings->dec_num_, g_GlobalSettings->obj_num_));
			ndpop_.push_back(new Individual(g_GlobalSettings->dec_num_, g_GlobalSettings->obj_num_));

			// extreme pop - weightnum
			extreme_pop_.push_back(new Individual(g_GlobalSettings->dec_num_, g_GlobalSettings->obj_num_));
		}

		// set mutation parameter
		mutation_para_.pro = 1.0 / g_GlobalSettings->dec_num_;
		mutation_para_.index1 = 20.0;

		// set crossover parameter
		cross_para_.pro = 1.0;
		cross_para_.index1 = 20.0;
	}
	void tDEA::Crossover(Individual** parent_pop, Individual** offspring_pop)
	{
		// generate random permutation index for tournment selection
		std::vector<int> index1(g_GlobalSettings->population_num_);
		std::vector<int> index2(g_GlobalSettings->population_num_);
		random_permutation(index1.data(), g_GlobalSettings->population_num_);
		random_permutation(index2.data(), g_GlobalSettings->population_num_);

		for (int i = 0; i <weight_num_ / 2; ++i)
		{
			Individual* parent1 = TournamentByRank(parent_pop[index1[2 * i]], parent_pop[index1[2 * i + 1]]);
			Individual* parent2 = TournamentByRank(parent_pop[index2[2 * i]], parent_pop[index2[2 * i + 1]]);
			SBX(parent1, parent2, offspring_pop[2 * i], offspring_pop[2 * i + 1],
				g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, cross_para_);
		}
	}

	void tDEA::EnvironmentalSelection(Individual** parent_pop, int mixpop_num, Individual** mixed_pop)
	{
		double** normalized_pop = new double*[mixpop_num];
		for (int i = 0; i < mixpop_num; i++)
			normalized_pop[i] = new double[g_GlobalSettings->obj_num_];

		int ndpop_num = 0;
		GetNdPop(mixed_pop, mixpop_num, ndpop_.data(), ndpop_num);
		Normalization(ndpop_.data(), ndpop_num, normalized_pop);
		Cluster(normalized_pop, ndpop_num);
		thetaNDSort(ndpop_.data(), normalized_pop, ndpop_num);

		// environmental selection
		int  current_pop_num = 0, temp_number = 0, rank_index = 0;

		while (1)
		{
			temp_number = 0;
			for (int i = 0; i < ndpop_num; i++)
				if (ndpop_[i]->rank_ == rank_index)
					temp_number++;
				
			
			if (current_pop_num + temp_number < weight_num_)
			{
				for (int i = 0; i < ndpop_num; i++)
				{
					if (ndpop_[i]->rank_ == rank_index)
					{
						CopyIndividual(ndpop_[i], parent_pop[current_pop_num]);
						current_pop_num++;
					}
				}
				rank_index++;
			}
			else
				break;
		}


		std::vector<int> index(temp_number);
		std::vector<int> perm(temp_number);
		random_permutation(perm.data(), temp_number);

		int  count = 0;
		for (int i = 0; i < ndpop_num; i++)
		{
			if (ndpop_[i]->rank_ == rank_index)
				index[count++] = i;
		}

		while (current_pop_num < weight_num_)
		{
			CopyIndividual(ndpop_[index[rnd(0, temp_number - 1)]], parent_pop[current_pop_num]);
			current_pop_num++;
		}

		for (int i = 0; i < mixpop_num; i++)
			delete[] normalized_pop[i];
		delete[] normalized_pop;
	}

	void tDEA::GetNdPop(Individual** mixed_pop, int mixpop_num, Individual** ndpop, int& ndpop_num)
	{
		int  current_pop_num = 0, temp_number = 0, rank_index = 0;
		NonDominatedSort(mixed_pop, mixpop_num,g_GlobalSettings->obj_num_);

		while (1)
		{
			temp_number = 0;
			for (int i = 0; i < mixpop_num; i++)
			{
				if (mixed_pop[i]->rank_ == rank_index)
				{
					temp_number++;
				}
			}
			if (current_pop_num + temp_number < weight_num_)
			{
				current_pop_num += temp_number;
				rank_index++;
			}
			else
				break;
		}

		ndpop_num = current_pop_num + temp_number;

		int index = 0;
		for (int i = 0; i < mixpop_num; i++)
		{
			if (mixed_pop[i]->rank_ <= rank_index)
			{
				CopyIndividual(mixed_pop[i], ndpop[index++]);
			}
		}
	}

	void tDEA::GetExtremePop(Individual** ndpop, int ndpop_num, Individual** extreme_pop)
	{
		std::vector<double> max_value(ndpop_num);
		std::vector<std::vector<double>> weight_vec(g_GlobalSettings->obj_num_, std::vector <double>(g_GlobalSettings->obj_num_));

		for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
		{
			for (int j = 0; j < g_GlobalSettings->obj_num_; j++)
			{
				if (i == j)
					weight_vec[i][j] = 1;
				else
					weight_vec[i][j] = 1e-6;
				
			}
		}

		// calculate the ASF
		int min_idx = 0;
		double temp_ASF = 0, temp_max_value = 0, min_value = 0;
		for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
		{
			for (int j = 0; j < ndpop_num; j++)
			{
				temp_max_value = 0;
				for (int k = 0; k < g_GlobalSettings->obj_num_; k++)
				{
					temp_ASF = (ndpop[j]->obj_[k] - ideal_point_[k])/ (nadir_point_[k] - ideal_point_[k]) / weight_vec[i][k];
					if (temp_ASF > temp_max_value)
					{
						temp_max_value = temp_ASF;
					}
				}
				max_value[j] = temp_max_value;
			}

			min_idx = 0;
			min_value = max_value[0];
			for (int j = 1; j < ndpop_num; j++)
			{
				if (max_value[j] < min_value)
				{
					min_value = max_value[j];
					min_idx = j;
				}
			}

			CopyIndividual(ndpop[min_idx], extreme_pop[i]);
		}
	}

	void tDEA::GetIntercepts(Individual** extreme_pop, Individual** ndpop, int ndpop_num, double* intercepts)
	{
		double** arg = (double**)malloc(sizeof(double*) * g_GlobalSettings->obj_num_);
		for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
			arg[i] = (double*)malloc(sizeof(double) * g_GlobalSettings->obj_num_);

		std::vector<double> u(g_GlobalSettings->obj_num_);
		std::vector<double> max_obj_value(g_GlobalSettings->obj_num_);

		// initialize 
		for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
			max_obj_value[i] = -EMOC_INF;
		

		/* traverse all the individuals of the population and get their maximum value of objective (The simplest way of
		 * calculating the nadir point is to get these maximum values among the first front individuals) */
		for (int i = 0; i < ndpop_num; i++)
		{
			for (int j = 0; j < g_GlobalSettings->obj_num_; j++)
			{
				if (ndpop[i]->rank_ == 0)
				{
					if (max_obj_value[j] < ndpop[i]->obj_[j])
						max_obj_value[j] = ndpop[i]->obj_[j];
				}
			}
		}

		for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
			u[i] = 1;
		

		for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
			for (int j = 0; j < g_GlobalSettings->obj_num_; j++)
				arg[i][j] = extreme_pop[i]->obj_[j] -ideal_point_[j];


		if (GaussianElimination(arg, u.data(), intercepts,g_GlobalSettings->obj_num_) == nullptr)
		{
			for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
				intercepts[i] = max_obj_value[i];
		}
		else
		{ 
			for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
				intercepts[i] = 1 / intercepts[i] + ideal_point_[i];
		}

		/* If any of the intercepts is still Zero (which means that one of the nadir values is Zero), then use the maximum
		 * value of each objective instead (remember that these values were calculated among all the individuals, not just
		 * the first-front individuals) */
		for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
		{
			if (intercepts[i] < EMOC_EPS)
			{
				for (int j = 0; j < g_GlobalSettings->obj_num_; j++)
					intercepts[j] = max_obj_value[j];
				break;
			}
		}

		for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
			free(arg[i]);
		free(arg);
	}

	void tDEA::Normalization(Individual** ndpop, int ndpop_num, double** normalized_pop)
	{
		std::vector<double> intercepts(g_GlobalSettings->obj_num_);

		for (int i = 0; i < ndpop_num; i++)
			UpdateIdealpoint(ndpop[i], ideal_point_, g_GlobalSettings->obj_num_);
		
		GetExtremePop(ndpop, ndpop_num, extreme_pop_.data());
		GetIntercepts(extreme_pop_.data(), ndpop, ndpop_num, intercepts.data());

		// normalzation
		for (int i = 0; i < ndpop_num; i++)
		{
			for (int j = 0; j < g_GlobalSettings->obj_num_; j++)
			{
				normalized_pop[i][j] = (ndpop[i]->obj_[j] - ideal_point_[j]) / (intercepts[j] - ideal_point_[j]);
			}
		}

		// update nadir point
		for (int i = 0; i < g_GlobalSettings->obj_num_;i++)
			nadir_point_[i] = intercepts[i];
	}

	void tDEA::Cluster(double** normalized_pop, int pop_num)
	{
		int n = 0;
		double min = 0, d2 = 0;

		for (int i = 0; i < weight_num_; i++)
			cluster_count_[i] = 0;

		for (int i = 0; i < pop_num; i++)
		{
			n = 0;
			min = CalPerpendicularDistance(normalized_pop[i], lambda_[0],g_GlobalSettings->obj_num_);

			for (int j = 1; j < weight_num_; j++)
			{
				d2 = CalPerpendicularDistance(normalized_pop[i], lambda_[j],g_GlobalSettings->obj_num_);
				if (d2 < min)
				{
					min = d2;
					n = j;
				}
			}

			cluster_[n][cluster_count_[n]++] = i;
		}
	}

	void tDEA::thetaNDSort(Individual** ndpop, double** normalized_pop, int pop_num)
	{
		int i = 0, j = 0, sum = 0, index = 0;
		double tempDistance = 0, d1 = 0, d2 = 0;
		std::vector<DistanceInfo> distanceInfo(pop_num);
		std::vector<double> theta(weight_num_);

		//assignment to theta
		for (i = 0; i < weight_num_; i++)
		{
			sum = 0;
			for (j = 0; j < g_GlobalSettings->obj_num_; j++)
			{
				if (lambda_[i][j] > 0.0001)
				{
					sum++;
				}
			}
			if (sum == 1)
				theta[i] = 100000.0;
			
			else
				theta[i] = 5.0;
		}

		for (i = 0; i < weight_num_; i++)
		{
			if (cluster_count_[i] == 0)
				continue;
			else
			{
				for (j = 0; j < cluster_count_[i]; j++)
				{
					index = cluster_[i][j];
					d1 = CalculateDotProduct(normalized_pop[index], lambda_[i], g_GlobalSettings->obj_num_) / CalculateNorm(lambda_[i], g_GlobalSettings->obj_num_);
					d2 = CalPerpendicularDistance(normalized_pop[index], lambda_[i],g_GlobalSettings->obj_num_);
					tempDistance = d1 + theta[i] * d2;

					distanceInfo[j].distance = tempDistance;
					distanceInfo[j].index = index;
				}
				std::sort(distanceInfo.begin(), distanceInfo.begin() + cluster_count_[i], [](DistanceInfo& left, DistanceInfo& right) {return left.distance < right.distance; });

				for (j = 0; j < cluster_count_[i]; j++)
				{
					ndpop[distanceInfo[j].index]->rank_ = j;
				}

			}
		}
	}

}