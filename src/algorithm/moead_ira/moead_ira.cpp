#include "algorithm/moead_ira/moead_ira.h"

#include <cmath>
#include <algorithm>

#include "core/macro.h"
#include "core/global.h"
#include "core/utility.h"
#include "core/uniform_point.h"
#include "operator/de.h"
#include "operator/polynomial_mutation.h"
#include "random/random.h"

namespace emoc {

	MOEADIRA::MOEADIRA(int thread_id) :
		Algorithm(thread_id),
		lambda_(nullptr),
		weight_num_(0),
		neighbour_(nullptr),
		neighbour_selectpro_(0.8),
		ideal_point_(new double[g_GlobalSettings->obj_num_]),
		beta_(0.98)
	{

	}

	MOEADIRA::~MOEADIRA()
	{
		for (int i = 0; i < weight_num_; ++i)
		{
			delete[] lambda_[i];
			delete[] neighbour_[i];
			lambda_[i] = nullptr;
			neighbour_[i] = nullptr;
		}
		delete[] lambda_;
		delete[] neighbour_;
		delete[] ideal_point_;
		delete[] old_obj_;
		delete[] delta_;
		delete[] P_;
		delete[] sd_;
		lambda_ = nullptr;
		neighbour_ = nullptr;
		ideal_point_ = nullptr;
		old_obj_ = nullptr;
		delta_ = nullptr;
		P_ = nullptr;
		sd_ = nullptr;
	}

	void MOEADIRA::Solve()
	{
		Initialization();

		// calculate first generation's fitness
		CalculateFitness(g_GlobalSettings->parent_population_.data(), weight_num_, old_obj_);
		Individual *offspring = g_GlobalSettings->offspring_population_[0];

		while (!IsTermination())
		{
			for (int i = 0; i < weight_num_; ++i)
			{
				if (randomperc() > P_[i])
					continue;

				// set current iteration's neighbour type
				if (randomperc() < neighbour_selectpro_)
					neighbour_type_ = NEIGHBOUR;
				else
					neighbour_type_ = GLOBAL;

				// generate offspring for current subproblem
				Crossover(g_GlobalSettings->parent_population_.data(), i, offspring);
				PolynomialMutation(offspring, g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, mutation_para_);
				EvaluateInd(offspring);

				// update ideal point
				UpdateIdealpoint(offspring, ideal_point_, g_GlobalSettings->obj_num_);

				// update neighbours' subproblem 
				UpdateSubproblem(offspring, i);
			}

			// update selection probability if necessary
			if (g_GlobalSettings->iteration_num_ % 20 == 0)
				UpdateProbability();
		}
	}

	void MOEADIRA::Initialization()
	{
		// initialize parent population
		g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		EvaluatePop(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);

		// generate weight vectors
		lambda_ = UniformPoint(g_GlobalSettings->population_num_, &weight_num_, g_GlobalSettings->obj_num_);
		real_popnum_ = weight_num_;

		// set the neighbours of each individual
		SetNeighbours();

		// initialize ideal point
		UpdateIdealpoint(g_GlobalSettings->parent_population_.data(), weight_num_, ideal_point_, g_GlobalSettings->obj_num_);

		// initialize IRA related data
		old_obj_ = new double[weight_num_];
		delta_ = new double[weight_num_];
		P_ = new double[weight_num_];
		sd_ = new int[weight_num_];

		for (int i = 0; i < weight_num_; ++i)
		{
			P_[i] = 0.5;
			delta_[i] = 0.0;
			old_obj_[i] = 0.0;
			sd_[i] = 0;
		}

		// set mutation parameter
		mutation_para_.pro = 1.0 / g_GlobalSettings->dec_num_;
		mutation_para_.index1 = 20.0;

		// set crossover parameter
		cross_para_.pro = 1.0;
		cross_para_.index1 = 0.5;
	}

	void MOEADIRA::SetNeighbours()
	{
		// set neighbour size and allocate memory
		neighbour_num_ = 20;
		neighbour_ = new int*[weight_num_];
		for (int i = 0; i < weight_num_; ++i)
		{
			neighbour_[i] = new int[weight_num_ - 1];
		}

		std::vector<DistanceInfo> sort_list(weight_num_);
		for (int i = 0; i < weight_num_; ++i)
		{
			for (int j = 0; j < weight_num_; ++j)
			{
				// calculate distance to each weight vector
				double distance_temp = 0;
				for (int k = 0; k < g_GlobalSettings->obj_num_; ++k)
				{
					distance_temp += (lambda_[i][k] - lambda_[j][k]) * (lambda_[i][k] - lambda_[j][k]);
				}

				sort_list[j].distance = sqrt(distance_temp);
				sort_list[j].index = j;
			}

			std::sort(sort_list.begin(), sort_list.end(), [](DistanceInfo& left, DistanceInfo& right) {
				return left.distance < right.distance;
				});

			for (int j = 0; j < weight_num_-1; j++)
			{
				neighbour_[i][j] = sort_list[j + 1].index;
			}
		}
	}

	void MOEADIRA::Crossover(Individual **parent_pop, int current_index, Individual *offspring)
	{
		int size = neighbour_type_ == NEIGHBOUR ? neighbour_num_ : weight_num_ - 1;
		int parent2_index = 0, parent3_index = 0;

		// calculate probability of selecting each subproblem as parent
		std::vector<double> pn(size);
		double sum = 0;
		for (int i = 0; i < size; ++i)
		{
			pn[i] = 0.05 + 0.95*(1 - 1 / (1 + 0.05 * exp(-20 * (double)(i + 1) / neighbour_num_ - 0.7)));
			sum += pn[i];
		}

		for (int i = 0; i < size; ++i)
			pn[i] = pn[i] / sum;
		
		// select parent
		for (int i = 0; i < 2; ++i)
		{
			int index = -1;
			double temp_sum = 0;
			double rand = randomperc();
			for (int j = 0; j < size; ++j)
			{
				if (rand >= temp_sum && rand < temp_sum + pn[j])
				{
					index = j;
					break;
				}
				temp_sum += pn[j];
			}

			if (i % 2 == 0)
				parent2_index = neighbour_[current_index][index];
			else
				parent3_index = neighbour_[current_index][index];
		}

		Individual *parent1 = parent_pop[current_index];
		Individual *parent2 = parent_pop[parent2_index];
		Individual *parent3 = parent_pop[parent3_index];
		DE(parent1, parent2, parent3, offspring, g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, cross_para_);
	}

	void MOEADIRA::UpdateSubproblem(Individual *offspring, int current_index)
	{
		double offspring_fitness = 0.0;
		double neighbour_fitness = 0.0;
		std::vector<DistanceInfo> sort_list(weight_num_);

		// calculate fitness improvement for each subproblem;
		for (int i = 0; i < weight_num_; ++i)
		{
			offspring_fitness = CalInverseChebycheff(offspring, lambda_[i], ideal_point_, g_GlobalSettings->obj_num_);
			neighbour_fitness = CalInverseChebycheff(g_GlobalSettings->parent_population_[i], lambda_[i], ideal_point_, g_GlobalSettings->obj_num_);

			sort_list[i].index = i;
			sort_list[i].distance = (neighbour_fitness - offspring_fitness) / neighbour_fitness;
		}

		std::sort(sort_list.begin(), sort_list.end(), [](DistanceInfo &left, DistanceInfo &right) {
			return left.distance < right.distance;
		});

		// update population
		int index = sort_list[weight_num_ - 1].index;
		offspring_fitness = CalInverseChebycheff(offspring, lambda_[index], ideal_point_, g_GlobalSettings->obj_num_);
		neighbour_fitness = CalInverseChebycheff(g_GlobalSettings->parent_population_[index], lambda_[index], ideal_point_, g_GlobalSettings->obj_num_);
		if (offspring_fitness < neighbour_fitness)
			CopyIndividual(offspring, g_GlobalSettings->parent_population_[index]);

	}



	void MOEADIRA::CalculateSD()
	{
		for (int i = 0; i < weight_num_; ++i)
			sd_[i] = 0;

		// find maximum and minimum of each objective
		std::vector<double> max(g_GlobalSettings->obj_num_);
		std::vector<double> min(g_GlobalSettings->obj_num_);

		for (int j = 0; j < g_GlobalSettings->obj_num_; ++j)
		{
			max[j] = g_GlobalSettings->parent_population_[0]->obj_[j];
			min[j] = g_GlobalSettings->parent_population_[0]->obj_[j];
		}

		for (int i = 0; i < weight_num_; ++i)
		{
			for (int j = 0; j < g_GlobalSettings->obj_num_; ++j)
			{
				if (max[j] < g_GlobalSettings->parent_population_[i]->obj_[j])
					max[j] = g_GlobalSettings->parent_population_[i]->obj_[j];
				if (min[j] > g_GlobalSettings->parent_population_[i]->obj_[j])
					min[j] = g_GlobalSettings->parent_population_[i]->obj_[j];
			}
		}

		std::vector<double> dis(weight_num_);
		std::vector<double> point(g_GlobalSettings->obj_num_);

		int min_index = 0;
		double temp_min = 0;
		for (int i = 0; i < weight_num_; i++)
		{
			// normalize the solution
			for (int k = 0; k < g_GlobalSettings->obj_num_; k++)
				point[k] = (g_GlobalSettings->parent_population_[i]->obj_[k] - min[k]) / (max[k] - min[k]);

			// find belonged subregion
			for (int j = 0; j < weight_num_; j++)
				dis[j] = CalPerpendicularDistance(point.data(), lambda_[j], g_GlobalSettings->obj_num_);

			temp_min = EMOC_INF;
			for (int j = 0; j < weight_num_; ++j)
			{
				if (dis[j] < temp_min)
				{
					temp_min = dis[j];
					min_index = j;
				}
			}

			sd_[min_index] += 1;
		}

	}

	void MOEADIRA::CalculateFitness(Individual **pop, int pop_num, double *fitness)
	{
		for (int i = 0; i < pop_num; ++i)
		{
			double fit = CalInverseChebycheff(pop[i], lambda_[i], ideal_point_, g_GlobalSettings->obj_num_);
			fitness[i] = fit;
		}
	}

	void MOEADIRA::UpdateProbability()
	{
		// update delta_
		for (int i = 0; i < weight_num_; ++i)
		{
			delta_[i] = fabs(g_GlobalSettings->parent_population_[i]->fitness_ - old_obj_[i]) / old_obj_[i];
			old_obj_[i] = g_GlobalSettings->parent_population_[i]->fitness_;
		}

		// calculate the number of solution in each subregion
		CalculateSD();

		// find max delta and max sd
		double max_delta = delta_[0];
		int max_sd = sd_[0];
		for (int i = 1; i < weight_num_; ++i)
		{
			if (max_delta < delta_[i]) max_delta = delta_[i];
			if (max_sd < sd_[i])	   max_sd = sd_[i];
		}

		// update selection probability of each subproblem
		for (int i = 0; i < weight_num_; ++i)
			P_[i] = beta_ * ((delta_[i] + 0.0000000000001) / (max_delta + 0.0000000000001)) + 
			(1 - beta_) * (1 - (double)sd_[i] / (double)max_sd);
	}

}