#pragma once
#include <vector>
#include <numeric>
#include <queue>
#include "core/individual.h"
#include "algorithm/algorithm.h"
#include "problem/problem.h"
#include "core/global.h"
#include "Python.h"


namespace emoc
{
    class R2_IBEA_LTR: public Algorithm
    {
        public:
            R2_IBEA_LTR(int thread_id);
            virtual ~R2_IBEA_LTR();

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
            void UpdateTrainingSet();
            void RecordCurrentPop(PyObject *pop);
            void LoadTrainingSet(PyObject *winners, PyObject *losers);
            PyObject *TrainRankNet_ReturnScore(PyObject *pFunction,PyObject *winners, PyObject *losers, PyObject *currPop);
            void UpdateScoreByRankNet(PyObject * res, double *score);
            void BiasingWeightSet();
            void Consultation_PreferenceElicitation();
            // void CalculateTotalDis(double *best);
            // void CalculateMinDis(double *best);

        private:
            double kappa;
            double *reference_point;
            double **lambda_;
            double *weight_;
            double step_size_;
            double *ideal_point;
            double retention_rate_;
            std::queue<double *> winners_;
            std::queue<double *> losers_;
            double *score_RankNet_;
            int weight_num_;
            int first_tau;      // first time to consult
            int tau;


    };
}
