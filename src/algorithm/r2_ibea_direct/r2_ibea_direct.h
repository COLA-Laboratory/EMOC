#pragma once
#include <vector>
#include <numeric>
#include <queue>
#include "core/individual.h"
#include "algorithm/algorithm.h"
#include "problem/problem.h"
#include "core/global.h"

#include "../vendor/armadillo/armadillo"

using namespace arma;


namespace emoc
{
    class DPBEMO_R2_IBEA: public Algorithm
    {
        public:
            DPBEMO_R2_IBEA(int thread_id);
            virtual ~DPBEMO_R2_IBEA();

            void Solve();

        private:
            void Initialization();
            void Crossover(Individual **parent_pop, Individual **offspring_pop);

            double CalEpsIndicator(Individual *ind1, Individual *ind2);
            void CalFitness(Individual **pop, int pop_num, std::vector<double>& fitness);
            void EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop);

            double CalR2Indicator(Individual *x);
            double CalR2Indicator(Individual *x, Individual *y);
            double CalBiR2Indicator(Individual *x, Individual *y);
            Individual* TournamentByBiR2Indicator(Individual *ind1, Individual *ind2);

            void  UtilityFunction_Gaussian(Individual **pop);
            void StorePreference(Individual **pop);
            std::tuple<int, int, vec> Consult_DM();
            void SetBiasedWeight(double *best);
            void SetBiasedWeight();
            float CalculateKLDivergence(Individual **pop);
            double UpdateUtilityPrediction(double obj, int index);
            
            // void UpdateTrainingSet();
            // void RecordCurrentPop(PyObject *pop);
            // void LoadTrainingSet(PyObject *winners, PyObject *losers);
            // PyObject *TrainRankNet_ReturnScore(PyObject *pFunction,PyObject *winners, PyObject *losers, PyObject *currPop);
            // void UpdateScoreByRankNet(PyObject * res, double *score);
            // void BiasingWeightSet();
            // void Consultation_PreferenceElicitation();

        private:
            double kappa;
            double **lambda_;
            // double *weight_;
            double step_size_;
            double *ideal_point;
            double retention_rate_;
            std::queue<double *> winners_;
            std::queue<double *> losers_;
            double *score_RankNet_;
            int weight_num_;

            int count;      // count consultation times
            double sigma;   // the width of the Gaussian function.
            double *weight; // weight for DM
            double *utl;
            int select_sum;
            
            mat PVals;				// store the winning probability of each solution
		    mat PSubset;			// store the winning probability of each subset
            int max_consultation;
            double **recommend_point;
            int nPromisingWeight;	//num of promising weights
            double *util_prob1;
		    double *util_prob2;
            double gamma;
            int best_index;
            int best_region;
            vec win_times;
            int subset_solution_num;			// number of solutions in each subset
            double *best_weight;
		    double **best_weights;


    };
}
