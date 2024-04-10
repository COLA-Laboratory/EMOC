#include "algorithm/r2_ibea/r2_ibea.h"

#include <cmath>
#include <iostream>
#include <algorithm>

#include "core/global.h"
#include "operator/tournament_selection.h"
#include "operator/sbx.h"
#include "operator/polynomial_mutation.h"
#include "random/random.h"
#include "core/utility.h"
#include "core/uniform_point.h"
using namespace std;

namespace emoc {

	R2_IBEA::R2_IBEA(int thread_id) :
    Algorithm(thread_id),
    kappa(0.05),
    ideal_point(new double[g_GlobalSettings->obj_num_]),
    reference_point(new double[g_GlobalSettings->obj_num_]),
    lambda_(nullptr),
    step_size_(0.2),
    retention_rate_(0.2),
    score_RankNet_(new double[g_GlobalSettings->population_num_])
	{
		real_popnum_ = g_GlobalSettings->population_num_;
	}

	R2_IBEA::~R2_IBEA()
	{
        for(int i = 0; i < g_GlobalSettings->population_num_; i++)
        {
            delete[] lambda_[i];
            lambda_[i] = nullptr;
        }
        delete[] lambda_;
        delete[] ideal_point;
        delete[] reference_point;
        delete[] score_RankNet_;
        lambda_ = nullptr;
        ideal_point = nullptr;
        reference_point = nullptr;
        score_RankNet_ = nullptr;

        while (winners_.size())
        {
            delete[] winners_.front();
            winners_.pop();
            delete[] losers_.front();
            losers_.pop();
        }
        

	}

	void R2_IBEA::Solve()
	{
		Initialization();

		while (!IsTermination())
		{
            
			// generate offspring population
			Crossover(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->offspring_population_.data());
			PolynomialMutation(g_GlobalSettings->offspring_population_.data(), 2 * g_GlobalSettings->population_num_ / 2, g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, mutation_para_);
			EvaluatePop(g_GlobalSettings->offspring_population_.data(), 2 * g_GlobalSettings->population_num_ / 2);
			MergePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_, g_GlobalSettings->offspring_population_.data(),
				2 * g_GlobalSettings->population_num_ / 2, g_GlobalSettings->mixed_population_.data());
			
            // update ideal point
            UpdateIdealpoint(g_GlobalSettings->offspring_population_.data(),g_GlobalSettings->population_num_, ideal_point, g_GlobalSettings-> obj_num_);


			// select next generation's population
			EnvironmentalSelection(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->mixed_population_.data());
		}
	}

	void R2_IBEA::Initialization()
	{
        weight_num_ = g_GlobalSettings->population_num_;
		// initialize parent population
		g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		EvaluatePop(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
        
        // char file[256];
        // sprintf(file,"./UniformWeights/%dd_%d.txt",g_GlobalSettings->obj_num_,g_GlobalSettings->population_num_);
        // lambda_=LoadUniformWeights(g_GlobalSettings->population_num_,g_GlobalSettings->obj_num_,file);
        lambda_=UniformPoint(g_GlobalSettings->population_num_, &weight_num_,g_GlobalSettings->obj_num_);

        // initialize reference point
        for(int i = 0; i < g_GlobalSettings->obj_num_; i++)
        {
            reference_point[i] = 0;
        }

        UpdateIdealpoint(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_, ideal_point, g_GlobalSettings->obj_num_);

		// set mutation parameter
		mutation_para_.pro = 1.0 / g_GlobalSettings->dec_num_;
		mutation_para_.index1 = 20.0;

		// set crossover parameter
		cross_para_.pro = 1.0;
		cross_para_.index1 = 20.0;
	}

	void R2_IBEA::Crossover(Individual **parent_pop, Individual **offspring_pop)
	{
		// generate random permutation index for tournment selection
		std::vector<int> index1(g_GlobalSettings->population_num_);
		std::vector<int> index2(g_GlobalSettings->population_num_);
		random_permutation(index1.data(), g_GlobalSettings->population_num_);
		random_permutation(index2.data(), g_GlobalSettings->population_num_);

		for (int i = 0; i < g_GlobalSettings->population_num_ / 2; ++i)
		{
			// Individual *parent1 = TournamentByFitness(parent_pop[index1[2 * i]], parent_pop[index1[2 * i + 1]]);
			// Individual *parent2 = TournamentByFitness(parent_pop[index2[2 * i]], parent_pop[index2[2 * i + 1]]);
            Individual *parent1 = TournamentByBiR2Indicator(parent_pop[index1[2 * i]], parent_pop[index1[2 * i + 1]]);
            Individual *parent2 = TournamentByBiR2Indicator(parent_pop[index2[2 * i]], parent_pop[index2[2 * i + 1]]);
			SBX(parent1, parent2, offspring_pop[2 * i], offspring_pop[2 * i + 1],
				g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, cross_para_);
		}
    
	}

	double R2_IBEA::CalEpsIndicator(Individual *ind1, Individual *ind2)
	{
		double max_eps = 0.0, temp_eps = 0.0;
		double r = g_GlobalSettings->dec_upper_bound_[0] - g_GlobalSettings->dec_lower_bound_[0];

		max_eps = (ind1->obj_[0] - g_GlobalSettings->dec_upper_bound_[0]) / r - (ind2->obj_[0] - g_GlobalSettings->dec_upper_bound_[0]) / r;
		for (int i = 1; i < g_GlobalSettings->obj_num_; ++i)
		{
			r = g_GlobalSettings->dec_upper_bound_[i] - g_GlobalSettings->dec_lower_bound_[1];
			temp_eps = (ind1->obj_[i] - g_GlobalSettings->dec_upper_bound_[i]) / r - (ind2->obj_[i] - g_GlobalSettings->dec_upper_bound_[i]) / r;

			if (temp_eps > max_eps)
				max_eps = temp_eps;
		}

		return max_eps;
	}

	void R2_IBEA::CalFitness(Individual **pop, int pop_num, std::vector<double> &fitness)
	{
		// determine indicator values and their maximum
		double max_fitness = 0;
		for (int i = 0; i < pop_num; ++i)
		{
			for (int j = 0; j < pop_num; ++j)
			{
				// fitness[i * pop_num + j] = CalEpsIndicator(pop[i], pop[j]);
                fitness[i * pop_num + j] = CalBiR2Indicator(pop[i], pop[j]);
				if (max_fitness < fabs(fitness[i * pop_num + j]))
					max_fitness = fabs(fitness[i * pop_num + j]);
			}
		}

		// calculate for each pair of individuals the corresponding value component
		for (int i = 0; i < pop_num;++i)
			for (int j = 0; j < pop_num; j++)
				fitness[i * pop_num + j] = exp((-fitness[i * pop_num + j] / max_fitness) / kappa);

		// set individual's fitness
		for (int i = 0; i < pop_num; ++i)
		{
			double sum = 0;
			for (int j = 0; j < pop_num; ++j)
				if (i != j)
					sum += fitness[j * pop_num + i];

			pop[i]->fitness_ = sum;
		}
	}

	void R2_IBEA::EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop)
	{
		int mixed_popnum = g_GlobalSettings->population_num_ + 2 * g_GlobalSettings->population_num_ / 2;

		// calculate fitness and store it in fitness array
		std::vector<double> fitness(mixed_popnum * mixed_popnum);
		CalFitness(mixed_pop, mixed_popnum, fitness);

		// select next generation's individuals
		int worst = -1;
		std::vector<int> flag(mixed_popnum,0);

		for (int i = g_GlobalSettings->population_num_; i > 0; --i)
		{
			int j = 0;
			for (j = 0; j < mixed_popnum && flag[j] == 1; ++j);
			worst = j;

			for (j = j + 1; j < mixed_popnum; ++j)
			{
				if (flag[j] != 1)
				{
					if (mixed_pop[j]->fitness_ > mixed_pop[worst]->fitness_)
						worst = j;
				}
			}

			for (j = 0; j < mixed_popnum; ++j)
			{
				if (flag[j] != 1)
				{
					mixed_pop[j]->fitness_ -= fitness[worst * mixed_popnum + j];
				}
			}

			flag[worst] = 1;
		}

		int count = 0;
		for (int i = 0; i < mixed_popnum; ++i)
		{
			if (flag[i] != 1)
				CopyIndividual(mixed_pop[i], parent_pop[count++]);
		}
	}

    double R2_IBEA::CalR2Indicator(Individual *x)
    {
        double res = 0;
        double a, ChebycheffValue;
        for (int i = 0; i < g_GlobalSettings->population_num_; ++i)
        {
            ChebycheffValue = -INF;
            for (int j = 0; j < g_GlobalSettings->obj_num_; ++j)
            {
                a = fabs(ideal_point[j] - x->obj_[j]) / lambda_[i][j];
                if (a > ChebycheffValue)
                {
                    ChebycheffValue = a;
                }
            }
            res += ChebycheffValue;
        }
        res /= g_GlobalSettings->population_num_;
        return res;
    }

    double R2_IBEA::CalR2Indicator(Individual *x, Individual *y)
    {
        double res = 0;
        double a, b, ChebycheffValue_x, ChebycheffValue_y;
        for (int i = 0; i < g_GlobalSettings->population_num_; ++i)
        {
            ChebycheffValue_x = -INF;
            ChebycheffValue_y = -INF;
            for (int j = 0; j < g_GlobalSettings->obj_num_; ++j)
            {
                a = fabs(ideal_point[j] - x->obj_[j]) / lambda_[i][j];
                b = fabs(ideal_point[j] - y->obj_[j]) / lambda_[i][j];
                if (a > ChebycheffValue_x)
                {
                    ChebycheffValue_x = a;
                }
                if (b > ChebycheffValue_y)
                {
                    ChebycheffValue_y = b;
                }
            }
            if (ChebycheffValue_x < ChebycheffValue_y)
            {
                res += ChebycheffValue_x;
            }
            else
            {
                res += ChebycheffValue_y;
            }
        }
        res /= g_GlobalSettings->population_num_;
        return res;
    }

    double R2_IBEA::CalBiR2Indicator(Individual *x, Individual *y)
    {
        return CalR2Indicator(x) - CalR2Indicator(x,y);
    }

    Individual* R2_IBEA::TournamentByBiR2Indicator(Individual *ind1, Individual *ind2)
    {
        double I_R2_1,I_R2_2;
        I_R2_1 = CalBiR2Indicator(ind1, ind2);
        I_R2_2 = CalBiR2Indicator(ind2, ind1);
        /*if (I_R2_1 < I_R2_2)
        {
            #if DEBUG
            std::cout<<"[INFO] Select parent 1."<<std::endl;
            #endif
            return ind1;
        }
        else if(I_R2_1 > I_R2_2)
        {
            #if DEBUG
            std::cout<<"[INFO] Select parent 2."<<std::endl;
            #endif
            return ind2;
        }*/
        if (I_R2_1 == 0)
        {
            return ind1;
        }
        else if(I_R2_2 == 0)
        {
            return ind2;
        }
        else
        {
            #if DEBUG
            std::cout<<"[INFO] Select parent randomly."<<std::endl;
            #endif
            if (randomperc() <= 0.5)
            {
                return (ind1);
            }
            else
            {
                return (ind2);
            }
        }
    }

}