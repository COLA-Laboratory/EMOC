#include "algorithms/rvea/rvea.h"

#include <cmath>
#include <ctime>
#include <iostream>
#include <algorithm>

#include "core/global.h"
#include "core/utility.h"
#include "core/uniform_point.h"
#include "core/emoc_manager.h"
#include "operator/sbx.h"
#include "operator/mutation.h"
#include "random/random.h"

namespace emoc {

	RVEA::RVEA(int thread_id) :
		Algorithm(thread_id),
		origin_lambda_(nullptr),
		normalized_lambda_(nullptr),
		weight_num_(0),
		max_gen_(0.0),
		ideal_point_(new double[g_GlobalSettings->obj_num_]),
		nadir_point_(new double[g_GlobalSettings->obj_num_])
	{

	}

	RVEA::~RVEA()
	{
		for (int i = 0; i < weight_num_; ++i)
		{
			delete[] origin_lambda_[i];
			delete[] normalized_lambda_[i];
			origin_lambda_[i] = nullptr;
			normalized_lambda_[i] = nullptr;
		}
		delete[] origin_lambda_;
		delete[] normalized_lambda_;
		delete[] ideal_point_;
		delete[] nadir_point_;
		origin_lambda_ = nullptr;
		normalized_lambda_ = nullptr;
		ideal_point_ = nullptr;
		nadir_point_ = nullptr;
	}

	void RVEA::Run()
	{
		Initialization();
		Individual* offspring = g_GlobalSettings->offspring_population_[0];

		while (!IsTermination())
		{
			// generate offspring for current subproblem
			Crossover(g_GlobalSettings->parent_population_.data(),g_GlobalSettings->offspring_population_.data());
			MutationPop(g_GlobalSettings->offspring_population_.data(), (real_popnum_ / 2) * 2, g_GlobalSettings);
			EvaluatePop(g_GlobalSettings->offspring_population_.data(), (real_popnum_ / 2) * 2);
			MergePopulation(g_GlobalSettings->parent_population_.data(), real_popnum_, g_GlobalSettings->offspring_population_.data(),
				2 * (real_popnum_ / 2), g_GlobalSettings->mixed_population_.data());

			real_popnum_ = ReferenceVectorGuidedSelection(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->mixed_population_.data(), 2 * (real_popnum_ / 2) + real_popnum_);

			if ((g_GlobalSettings->iteration_num_ % (int)(fr_ * max_gen_)) == 0)
				ReferenceVectorAdaption(g_GlobalSettings->parent_population_.data());
		}
	}

	void RVEA::Initialization()
	{
		// initialize parent population
		g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		EvaluatePop(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);

		// generate weight vectors
		origin_lambda_ = UniformPoint(g_GlobalSettings->population_num_, &weight_num_, g_GlobalSettings->obj_num_);
		real_popnum_ = weight_num_;
		max_gen_ = g_GlobalSettings->max_evaluation_ / (double)weight_num_;

		// generate normalized weight vectors and store the very first generation's lambda
		ReferenceVectorNormalization(origin_lambda_);
		normalized_lambda_ = (double**)malloc(sizeof(double*) * weight_num_);
		for (int i = 0; i < weight_num_; i++)
			normalized_lambda_[i] = (double*)malloc(sizeof(double) * g_GlobalSettings->obj_num_);
		for (int i = 0; i < weight_num_; i++)
			for (int j = 0; j < g_GlobalSettings->obj_num_; j++)
				normalized_lambda_[i][j] = origin_lambda_[i][j];

		// initialize ideal point
		UpdateIdealpoint(g_GlobalSettings->parent_population_.data(), weight_num_, ideal_point_, g_GlobalSettings->obj_num_);
	}

	void RVEA::Crossover(Individual** parent_pop, Individual** offspring_pop)
	{
		int index = 0;
		Individual* parent1 = nullptr, *parent2 = nullptr;

		for (int i = 0; i < weight_num_ / 2; i++)
		{
			// randomly select two parents
			int k = rnd(0, weight_num_ - 1);
			int l = rnd(0, weight_num_ - 1);
			parent1 = parent_pop[k];
			parent2 = parent_pop[l];

			SBX(parent1, parent2, offspring_pop[index], offspring_pop[index + 1], g_GlobalSettings);
			index += 2;
		}
	}

	void RVEA::ReferenceVectorNormalization(double **lambda)
	{
		for (int i = 0; i < weight_num_; i++)
		{
			double norm = CalculateNorm(lambda[i], g_GlobalSettings->obj_num_);
			for (int j = 0; j < g_GlobalSettings->obj_num_; j++)
			{
				lambda[i][j] = lambda[i][j] / norm;
			}
		}
	}


	int RVEA::ReferenceVectorGuidedSelection(Individual** parent_pop, Individual** mix_pop, int mix_pop_num)
	{
		int pop_count = 0;
		int M = g_GlobalSettings->obj_num_;

		int* index = nullptr;                      // the number of inds belong to each reference vector
		double* zmin = nullptr;                    // ideal point of the population
		double* gama = nullptr;
		double** pop_obj = nullptr;                // translated population objectives
		int** partition = nullptr;				   // the index of inds belong to each reference vector
		DistanceInfo** angle_info = nullptr;        
		DistanceInfo** APD_info = nullptr;       

		// allocate memeory
		index = (int*)malloc(sizeof(int) * weight_num_);
		zmin = (double*)malloc(sizeof(double) * M);
		gama = (double*)malloc(sizeof(double) * weight_num_);
		pop_obj = (double**)malloc(sizeof(double*) * mix_pop_num);
		for (int i = 0; i < mix_pop_num; i++)
			pop_obj[i] = (double*)malloc(sizeof(double) * M);
		partition = (int**)malloc(sizeof(int*) * weight_num_);
		for (int i = 0; i < weight_num_; i++)
			partition[i] = (int*)malloc(sizeof(int) * mix_pop_num);

		angle_info = (DistanceInfo**)malloc(sizeof(DistanceInfo*) * mix_pop_num);
		for (int i = 0; i < mix_pop_num; i++)
			angle_info[i] = (DistanceInfo*)malloc(sizeof(DistanceInfo) * weight_num_);

		APD_info = (DistanceInfo**)malloc(sizeof(DistanceInfo*) * weight_num_);
		for (int i = 0; i < weight_num_; i++)
			APD_info[i] = (DistanceInfo*)malloc(sizeof(DistanceInfo) * mix_pop_num);

		// initialize 
		for (int i = 0; i < weight_num_; i++)
			index[i] = 0;
		for (int i = 0; i < M; i++)
			zmin[i] = EMOC_INF;

		for (int i = 0; i < weight_num_; i++)
		{
			for (int j = 0; j < mix_pop_num; j++)
			{
				APD_info[i][j].index = -1;
				APD_info[i][j].distance = EMOC_INF;
			}
		}

		/* Objective Value Translation */
		for (int i = 0; i < mix_pop_num; i++)
		{
			for (int j = 0; j < M; j++)
				if (zmin[j] > mix_pop[i]->obj_[j])
					zmin[j] = mix_pop[i]->obj_[j];
		}

		for (int i = 0; i < mix_pop_num; i++)
			for (int j = 0; j < M; j++)
				pop_obj[i][j] = mix_pop[i]->obj_[j] - zmin[j];
			
		

		/* Population Partition */
		for (int i = 0; i < mix_pop_num; i++)
		{
			for (int j = 0; j < weight_num_; j++)
			{
				double cos_val = CalculateDotProduct(pop_obj[i], normalized_lambda_[j], M) / (CalculateNorm(pop_obj[i], M) * CalculateNorm(normalized_lambda_[j], M));
				angle_info[i][j].index = j;
				angle_info[i][j].distance = cos_val;
			}
		}

		for (int i = 0; i < mix_pop_num; i++)
		{
			std::sort(angle_info[i], angle_info[i] + weight_num_, [](DistanceInfo& left, DistanceInfo& right) {
				return left.distance < right.distance;
				});
			int temp_index = angle_info[i][weight_num_ - 1].index;

			partition[temp_index][index[temp_index]] = i;
			index[temp_index] += 1;
		}

		/* Angle-Penalized Distance Calculation */
		// 1. Calculate the smallest angle value between each vector and others
		for (int i = 0; i < weight_num_; i++)
		{
			gama[i] = EMOC_INF;
			for (int j = 0; j < weight_num_; j++)
			{
				if (i != j)
				{
					double cos_val = CalculateDotProduct(normalized_lambda_[i], normalized_lambda_[j], M) / (CalculateNorm(normalized_lambda_[i], M) * CalculateNorm(normalized_lambda_[j], M));
					if (gama[i] > acos(cos_val))
						gama[i] = acos(cos_val);
				}
			}
		}

		for (int i = 0; i < weight_num_; i++)
		{
			if (index[i] != 0)
			{
				for (int j = 0; j < index[i]; j++)
				{
					double cos_val = CalculateDotProduct(normalized_lambda_[i], pop_obj[partition[i][j]], M) / (CalculateNorm(pop_obj[partition[i][j]], M) * CalculateNorm(normalized_lambda_[i], M));
					double temp_value = M * pow((double)g_GlobalSettings->iteration_num_ / max_gen_, alpha_) * (acos(cos_val) / gama[i]);
					APD_info[i][j].index = partition[i][j];
					APD_info[i][j].distance = (1 + temp_value) * CalculateNorm(pop_obj[partition[i][j]], M);
				}
			}
		}

		/* Elitism Selection */
		for (int i = 0; i < weight_num_; i++)
		{
			if (index[i] != 0)
			{
				std::sort(APD_info[i], APD_info[i] + index[i], [](DistanceInfo &left, DistanceInfo &right) {
					return left.distance < right.distance;
					});
				CopyIndividual(mix_pop[APD_info[i][0].index], parent_pop[pop_count]);
				pop_count++;
			}
		}

		for (int i = 0; i < weight_num_; i++)
		{
			free(partition[i]);
			free(APD_info[i]);
		}
		for (int i = 0; i < mix_pop_num; i++)
		{
			free(pop_obj[i]);
			free(angle_info[i]);
		}
		free(pop_obj);
		free(angle_info);
		free(partition);
		free(APD_info);
		free(index);
		free(zmin);
		free(gama);

		return pop_count;
	}

	void RVEA::ReferenceVectorAdaption(Individual** parent_pop)
	{
		UpdateNadirpoint(parent_pop, real_popnum_, nadir_point_, g_GlobalSettings->obj_num_);
		UpdateIdealpoint(parent_pop, real_popnum_, ideal_point_, g_GlobalSettings->obj_num_);

		for (int i = 0; i < weight_num_; i++)
		{
			for (int j = 0; j < g_GlobalSettings->obj_num_; j++)
			{
				normalized_lambda_[i][j] = origin_lambda_[i][j] * (nadir_point_[j] - ideal_point_[j]);
			}
		}

		ReferenceVectorNormalization(normalized_lambda_);
	}

}