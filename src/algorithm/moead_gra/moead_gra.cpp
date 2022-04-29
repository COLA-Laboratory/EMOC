#include "algorithm/moead_gra/moead_gra.h"

#include <cmath>
#include <algorithm>

#include "core/global.h"
#include "core/utility.h"
#include "core/uniform_point.h"
#include "operator/de.h"
#include "operator/polynomial_mutation.h"
#include "random/random.h"

namespace emoc {

	MOEADGRA::MOEADGRA(int thread_id) :
		Algorithm(thread_id),
		lambda_(nullptr),
		weight_num_(0),
		neighbour_(nullptr),
		neighbour_selectpro_(0.8),
		ideal_point_(new double[g_GlobalSettings->obj_num_])
	{

	}

	MOEADGRA::~MOEADGRA()
	{
		for (int i = 0; i < 20; ++i)
		{
			delete[] fitness_history_[i];
			fitness_history_[i] = nullptr;
		}

		for (int i = 0; i < weight_num_; ++i)
		{
			delete[] lambda_[i];
			delete[] neighbour_[i];
			lambda_[i]	  = nullptr;
			neighbour_[i] = nullptr;
		}

		delete[] fitness_history_;
		delete[] lambda_;
		delete[] neighbour_;
		delete[] ideal_point_;
		delete[] delta_;
		delete[] P_;
		lambda_		 = nullptr;
		neighbour_	 = nullptr;
		ideal_point_ = nullptr;
		delta_		 = nullptr;
		P_			 = nullptr;
	}

	void MOEADGRA::Solve()
	{
		Initialization();
		
		// calculate first generation's fitness
		CalculateFitness(g_GlobalSettings->parent_population_.data(), weight_num_, fitness_history_[0]);
		Individual *offspring = g_GlobalSettings->offspring_population_[0];

		while (!IsTermination())
		{
			for (int i = 0; i < weight_num_; ++i)
			{
				if(randomperc() > P_[i])
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
			if (g_GlobalSettings->iteration_num_ >= 20)
				UpdateProbability();

			// record population's fitness
			for (int i = 0; i < weight_num_; ++i)
				fitness_history_[g_GlobalSettings->iteration_num_ % 20][i] = g_GlobalSettings->parent_population_[i]->fitness_;
		}
	}

	void MOEADGRA::Initialization()
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

		// initialize GRA related data
		fitness_history_ = new double*[20];
		for (int i = 0; i < 20; ++i)
			fitness_history_[i] = new double[weight_num_];
		delta_ = new double[weight_num_];
		P_ = new double[weight_num_];

		for (int i = 0; i < weight_num_; ++i)
		{
			P_[i] = 0.5;
			delta_[i] = 0.0;
		} 

		// set mutation parameter
		mutation_para_.pro = 1.0 / g_GlobalSettings->dec_num_;
		mutation_para_.index1 = 20.0;

		// set crossover parameter
		cross_para_.pro = 1.0;
		cross_para_.index1 = 0.5;
	}

	void MOEADGRA::SetNeighbours()
	{
		// set neighbour size and allocate memory
		neighbour_num_ = 20;
		neighbour_ = new int*[weight_num_];
		for (int i = 0; i < weight_num_; ++i)
		{
			neighbour_[i] = new int[neighbour_num_];
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

			for (int j = 0; j < neighbour_num_; j++)
			{
				neighbour_[i][j] = sort_list[j + 1].index;
			}
		}
	}

	void MOEADGRA::Crossover(Individual **parent_pop, int current_index, Individual *offspring)
	{
		int size = neighbour_type_ == NEIGHBOUR ? neighbour_num_ : weight_num_;
		int parent2_index = 0, parent3_index = 0;

		// randomly select two parents according to neighbour type
		if (neighbour_type_ == NEIGHBOUR)
		{
			parent2_index = neighbour_[current_index][rnd(0, size - 1)];
			parent3_index = neighbour_[current_index][rnd(0, size - 1)];
		}
		else
		{
			parent2_index = rnd(0, size - 1);
			parent3_index = rnd(0, size - 1);
		}

		Individual *parent1 = parent_pop[current_index];
		Individual *parent2 = parent_pop[parent2_index];
		Individual *parent3 = parent_pop[parent3_index];
		DE(parent1, parent2, parent3, offspring, g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, cross_para_);
	}

	void MOEADGRA::UpdateSubproblem(Individual *offspring, int current_index)
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



	void MOEADGRA::CalculateFitness(Individual **pop, int pop_num, double *fitness)
	{
		for (int i = 0; i < pop_num; ++i)
		{
			double fit = CalInverseChebycheff(pop[i], lambda_[i], ideal_point_, g_GlobalSettings->obj_num_);
			fitness[i] = fit;
		}
	}

	void MOEADGRA::UpdateProbability()
	{
		int index = g_GlobalSettings->iteration_num_ % 20;
		for (int i = 0; i < weight_num_; ++i)
			delta_[i] = fabs(g_GlobalSettings->parent_population_[i]->fitness_ - fitness_history_[index][i]) / fitness_history_[index][i];

		double max = delta_[0];
		for (int i = 1; i < weight_num_; ++i)
		{
			if (max < delta_[i])
				max = delta_[i];
		}

		for (int i = 0; i < weight_num_; ++i)
		{
			P_[i] = (delta_[i] + 0.0000000000001) / (max + 0.0000000000001);
		}
	}

}