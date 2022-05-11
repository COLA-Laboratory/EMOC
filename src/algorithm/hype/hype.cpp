#include "algorithm/hype/hype.h"

#include <vector>
#include <algorithm>

#include "core/global.h"
#include "core/utility.h"
#include "core/nd_sort.h"
#include "operator/tournament_selection.h"
#include "operator/sbx.h"
#include "operator/polynomial_mutation.h"
#include "random/random.h"

namespace emoc {

	HypE::HypE(int thread_id) :
		Algorithm(thread_id),
		ideal_point_(new double[g_GlobalSettings->obj_num_]),
		nadir_point_(new double[g_GlobalSettings->obj_num_])
	{
		real_popnum_ = g_GlobalSettings->population_num_;
	}

	HypE::~HypE()
	{
		delete[] ideal_point_;
		delete[] nadir_point_;
	}

	void HypE::Solve()
	{
		Initialization();

		while (!IsTermination())
		{
			CalculateFitness(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_, g_GlobalSettings->population_num_);

			// generate offspring population
			Crossover(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->offspring_population_.data());
			PolynomialMutation(g_GlobalSettings->offspring_population_.data(), 2 * g_GlobalSettings->population_num_ / 2, g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, mutation_para_);
			EvaluatePop(g_GlobalSettings->offspring_population_.data(), 2 * g_GlobalSettings->population_num_ / 2);

			// update ideal point and nadir point
			UpdateIdealpoint(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_, ideal_point_, g_GlobalSettings->obj_num_);
			for (int i = 0; i < g_GlobalSettings->population_num_; i++) UpdateNadirpoint(g_GlobalSettings->parent_population_[i], nadir_point_, g_GlobalSettings->obj_num_);

			// select for next generation
			MergePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_, g_GlobalSettings->offspring_population_.data(),
				2 * g_GlobalSettings->population_num_ / 2, g_GlobalSettings->mixed_population_.data());
 			EnvironmentalSelection(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->mixed_population_.data());
		}
	}

	void HypE::Initialization()
	{
		// initialize parent population
		g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		EvaluatePop(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);

		// initialize ideal and nadir point
		UpdateIdealpoint(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_, ideal_point_, g_GlobalSettings->obj_num_);
		UpdateNadirpoint(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_, nadir_point_, g_GlobalSettings->obj_num_);
		for (int i = 0; i < g_GlobalSettings->obj_num_; i++)
		{
			nadir_point_[i] *= 1.2;
		}

		// set mutation parameter
		mutation_para_.pro = 1.0 / g_GlobalSettings->dec_num_;
		mutation_para_.index1 = 20.0;

		// set crossover parameter
		cross_para_.pro = 1.0;
		cross_para_.index1 = 20.0;
	}

	void HypE::Crossover(Individual **parent_pop, Individual **offspring_pop)
	{
		// generate random permutation index for tournment selection
		std::vector<int> index1(g_GlobalSettings->population_num_);
		std::vector<int> index2(g_GlobalSettings->population_num_);
		random_permutation(index1.data(), g_GlobalSettings->population_num_);
		random_permutation(index2.data(), g_GlobalSettings->population_num_);

		int greater_is_better = 1;
		for (int i = 0; i < g_GlobalSettings->population_num_ / 2; ++i)
		{
			Individual *parent1 = TournamentByFitness(parent_pop[index1[2 * i]], parent_pop[index1[2 * i + 1]], greater_is_better);
			Individual *parent2 = TournamentByFitness(parent_pop[index2[2 * i]], parent_pop[index2[2 * i + 1]], greater_is_better);
			SBX(parent1, parent2, offspring_pop[2 * i], offspring_pop[2 * i + 1],
				g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, cross_para_);
		}
	}



	void HypE::CalculateFitness(Individual **pop, int pop_num, int parameter_k)
	{
		FitnessInfo *fitness_info = new FitnessInfo[pop_num];
		double *rho = new double[parameter_k+1];

		// set alpha 
		rho[0] = 0;
		for (int i = 1; i <= parameter_k; ++i)
		{
			rho[i] = 1.0 / (double)i;
			for (int j = 1; j <= i - 1; ++j)
				rho[i] *= (double)(parameter_k - j) / (double)(pop_num - j);
		}
		for (int i = 0; i < pop_num; ++i)
			fitness_info[i].value = 0.0;

		// memory for HypeExactRecursive
		pvec_recursive_ = new int[pop_num];
		p_recursive_ = new double[g_GlobalSettings->obj_num_ * pop_num];
		tmpfit_recursive_ = new double[pop_num];

		if (g_GlobalSettings->obj_num_ <= 2)
		{
			HypeExact(fitness_info, parameter_k, rho, pop, pop_num);
		}
		else
		{
			HypeSampling(fitness_info, 20000, parameter_k, rho, pop, pop_num);
		}

		for (int i = 0; i < pop_num; ++i)
		{
			pop[i]->fitness_ = fitness_info[i].value;
			//printf("%f \n", fitness_info[i].value);
		}

		delete[] fitness_info;
		delete[] rho;
		delete[] pvec_recursive_;
		delete[] p_recursive_;
		delete[] tmpfit_recursive_;
	}

	void HypE::HypeSampling(FitnessInfo *fitness_info, int samples_num, int parameter_k, double *rho, Individual **pop, int pop_num)
	{
		
		int i, s, k;
		int domCount, counter;
		std::vector<double> hitstat(pop_num);
		std::vector<double> sample(g_GlobalSettings->obj_num_);

		// monte carlo to calculate hv
		for (i = 0; i < pop_num; i++)
			fitness_info[i].value = 0.0;
		for (s = 0; s < samples_num; s++)
		{
			for (k = 0; k < g_GlobalSettings->obj_num_; k++)
				sample[k] = rndreal(ideal_point_[k], nadir_point_[k]);

			domCount = 0;
			counter = 0;

			for (i = 0; i < pop_num; ++i)
			{
				if (WeaklyDominates(pop[i]->obj_.data(), sample.data(), g_GlobalSettings->obj_num_))
				{
					domCount++;
					if (domCount > parameter_k)
						break;
					hitstat[counter] = 1;
				}
				else
					hitstat[counter] = 0;
				counter++;
			}


			if (domCount > 0 && domCount <= parameter_k)
			{
				counter = 0;
				for (i = 0; i < pop_num; ++i)
				{
					if (hitstat[counter] == 1)
						fitness_info[counter].value += rho[domCount];
					counter++;
				}
			}
		}
		counter = 0;

		for (i = 0; i < pop_num; ++i)
		{
			fitness_info[counter].index = i;
			fitness_info[counter].value = fitness_info[counter].value / (double)samples_num;
			for (k = 0; k < g_GlobalSettings->obj_num_; k++)
				fitness_info[counter].value *= (nadir_point_[k] - ideal_point_[k]);
			counter++;
		}

	}

	void HypE::HypeExact(FitnessInfo *fitness_info, int parameter_k, double *rho, Individual **pop, int pop_num)
	{
		int i, j;
		int *indices = new int[pop_num];
		double *boundsVec = new double[g_GlobalSettings->obj_num_];
		double *fitness = new double[pop_num];
		double *p = new double[pop_num * g_GlobalSettings->obj_num_];

		for (i = 0; i < pop_num; ++i)
		{
			for (j = 0; j < g_GlobalSettings->obj_num_; ++j)
			{
				p[i * g_GlobalSettings->obj_num_ + j] = pop[i]->obj_[j];
			}
		}

		for (i = 0; i < g_GlobalSettings->obj_num_; i++)
			boundsVec[i] = nadir_point_[i];
		for (i = 0; i < pop_num; i++)
			indices[i] = i;

		HypeExactRecursive(p, pop_num, g_GlobalSettings->obj_num_, pop_num, g_GlobalSettings->obj_num_ - 1, boundsVec,
			indices, fitness, rho, parameter_k);

		for (i = 0; i < pop_num; ++i)
		{
			fitness_info[i].value = fitness[i];
		}

		delete[] fitness;
		delete[] boundsVec;
		delete[] p;
		delete[] indices;
	}

	void HypE::HypeExactRecursive(double* input_p, int pnts, int dim, int nrOfPnts, int actDim, double* bounds, int* input_pvec, double* fitness, double* rho, int param_k)
	{
		int i, j;
		double extrusion;

		for (i = 0; i < pnts; i++) {
			fitness[i] = 0;
			pvec_recursive_[i] = input_pvec[i];
		}
		for (i = 0; i < pnts*dim; i++)
			p_recursive_[i] = input_p[i];

		RearrangeIndicesByColumn(p_recursive_, nrOfPnts, dim, actDim, pvec_recursive_);

		for (i = 0; i < nrOfPnts; i++)
		{
			if (i < nrOfPnts - 1)
				extrusion = p_recursive_[(pvec_recursive_[i + 1])*dim + actDim] - p_recursive_[pvec_recursive_[i] * dim + actDim];
			else
				extrusion = bounds[actDim] - p_recursive_[pvec_recursive_[i] * dim + actDim];

			if (actDim == 0) {
				if (i + 1 <= param_k)
					for (j = 0; j <= i; j++) {
						fitness[pvec_recursive_[j]] = fitness[pvec_recursive_[j]]
							+ extrusion * rho[i + 1];
					}
			}
			else if (extrusion > 0) {
				HypeExactRecursive(p_recursive_, pnts, dim, i + 1, actDim - 1, bounds, pvec_recursive_,
					tmpfit_recursive_, rho, param_k);
				for (j = 0; j < pnts; j++)
					fitness[j] += extrusion * tmpfit_recursive_[j];
			}
		}
	}

	void HypE::RearrangeIndicesByColumn(double *mat, int rows, int columns, int col, int *ind)
	{
		#define  MAX_LEVELS  300
		int  beg[MAX_LEVELS], end[MAX_LEVELS], i = 0, L, R, swap;
		double pref, pind;
		double *ref = new double[rows];

		for (i = 0; i < rows; i++) {
			ref[i] = mat[col + ind[i] * columns];
		}
		i = 0;

		beg[0] = 0; end[0] = rows;

		while (i >= 0) {
			L = beg[i]; R = end[i] - 1;
			if (L < R) {
				pref = ref[L];
				pind = ind[L];
				while (L < R) {
					while (ref[R] >= pref && L < R)
						R--;
					if (L < R) {
						ref[L] = ref[R];
						ind[L++] = ind[R];
					}
					while (ref[L] <= pref && L < R)
						L++;
					if (L < R) {
						ref[R] = ref[L];
						ind[R--] = ind[L];
					}
				}
				ref[L] = pref; ind[L] = pind;
				beg[i + 1] = L + 1; end[i + 1] = end[i];
				end[i++] = L;
				if (end[i] - beg[i] > end[i - 1] - beg[i - 1]) {
					swap = beg[i]; beg[i] = beg[i - 1]; beg[i - 1] = swap;
					swap = end[i]; end[i] = end[i - 1]; end[i - 1] = swap;
				}
			}
			else {
				i--;
			}
		}

		delete[] ref;
	}

	void HypE::EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop)
	{
		int i = 0, j = 0;
		int temp_number = 0, rank_index = 0, current_pop_num = 0;
		int mixed_popnum = g_GlobalSettings->population_num_ + 2 * g_GlobalSettings->population_num_ / 2;
		std::vector<Individual*> temp_pop(mixed_popnum, nullptr);
		FitnessInfo *fitness_info = new FitnessInfo[mixed_popnum];

		// allocate memory for temp_pop
		for (int i = 0; i < mixed_popnum; ++i)
			temp_pop[i] = new Individual(g_GlobalSettings->dec_num_, g_GlobalSettings->obj_num_);

		NonDominatedSort(mixed_pop, mixed_popnum, g_GlobalSettings->obj_num_);

		// find the front need to extra selection
		while (1)
		{
			temp_number = 0;
			for (i = 0; i < mixed_popnum; i++)
			{
				if (mixed_pop[i]->rank_ == rank_index)
				{
					temp_number++;
				}
			}
			if (current_pop_num + temp_number <= g_GlobalSettings->population_num_)
			{
				for (i = 0; i < mixed_popnum; i++)
				{
					if (mixed_pop[i]->rank_ == rank_index)
					{
						CopyIndividual(mixed_pop[i], parent_pop[current_pop_num]);
						current_pop_num++;
					}
				}
				rank_index++;
			}
			else
				break;
		}


		if (current_pop_num != g_GlobalSettings->population_num_)
		{
			for (i = 0; i < mixed_popnum; i++)
			{
				if (mixed_pop[i]->rank_ == rank_index)
				{
					CopyIndividual(mixed_pop[i], temp_pop[j]);
					j++;
				}
			}

			while (temp_number != g_GlobalSettings->population_num_ - current_pop_num)
			{
				CalculateFitness(temp_pop.data(), temp_number, current_pop_num + temp_number - g_GlobalSettings->population_num_);

				for (i = 0; i < temp_number; ++i)
				{
					fitness_info[i].index = i;
					fitness_info[i].value = temp_pop[i]->fitness_;
				}

				std::sort(fitness_info, fitness_info + temp_number, [](FitnessInfo &left, FitnessInfo &right) {
					return left.value < right.value;
				});

				if (fitness_info[0].index != temp_number - 1)
				{
					//CopyIndividual(temp_pop[fitness_info[0].index], temp_pop[temp_number - 1]);
					SwapIndividual(temp_pop[fitness_info[0].index], temp_pop[temp_number - 1]);
				}

				temp_number--;
			}

			for (i = 0; i < temp_number; i++)
			{
				CopyIndividual(temp_pop[i], parent_pop[current_pop_num]);
				current_pop_num++;
			}

		}

		for (int i = 0; i < mixed_popnum; ++i)
		{
			delete temp_pop[i];
			temp_pop[i] = nullptr;
		}
		delete[] fitness_info;
	}

}