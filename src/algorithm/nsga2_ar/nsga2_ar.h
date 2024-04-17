//
// Created by gylai on 23-1-10.
// Contact: Lai Guiyu <guiyulai.chn@gmail.com>
// COLA-Lab@UESTC
//

#pragma once

#include <vector>
#include <set>
#include <queue>
#include <algorithm>

#include <armadillo>
#include "algorithm/algorithm.h"
#include "problem/problem.h"
#include "metric/metric_head_collect.h"

namespace emoc {

    class NSGA2_AR : public Algorithm
    {
    public:
        typedef struct
        {
            int index;
            double distance;
        }DistanceInfo;  // store crowding distance of index-th individual

        typedef struct
        {
            int index;
            double value;
        }SortList;

        NSGA2_AR(int thread_num);
        virtual ~NSGA2_AR();

        void Solve();

    private:
        void Initialization();
        void Crossover(Individual **parent_pop, Individual **offspring_pop);

        // set the crowding distance of given individual index
        void SetDistanceInfo(std::vector<DistanceInfo> &distanceinfo_vec, int target_index, double distance);

        // use crowding distance to sort the individuals with rank rank_index, the result is stored in pop_sort
        // return the number of indviduals of rank rank_index
        int CrowdingDistance(Individual **mixed_pop, int pop_num, int *pop_sort, int rank_index);

        // do nsga2's environment selection on mixed_pop, the result is stored in parent_pop
        bool EnvironmentalSelection(Individual **parent_pop, Individual **mixed_pop, bool is_update_model, bool is_use_preference);

        // ********************************************
        //
        // active ranking related
        //
        // ********************************************
        void active_ranking(Individual **parent_pop);
        void unique_matrix(arma::mat &objects);
        void rank_by_preference(Individual **mixed_pop);
        void rank_by_domination(Individual **mixed_pop);
        void build_hyperplanes(arma::mat X, arma::cube &H);
        void get_undefined_preference(arma::Mat<int> Qh, arma::uvec hi, int j, arma::uvec &goodInds);
        arma::uvec get_toSort(arma::uvec goodInds);
        arma::uvec compare_sort(arma::uvec goodInds, arma::Mat<int> Qhyp);
        void test_compare_sort();
        void quicksort_handle(arma::Mat<int> cmp, arma::uvec &index, int l, int r);

        // solve linear programming problem using lp_solve package
        double lp_solver(arma::mat X, arma::Mat<int> Y);
        void test_lp_solver();

        // solve linear programming problem using GLPK package
        double glpk_solver(arma::mat X, arma::Mat<int> Y);
        void test_glpk_solver();


        arma::vec approximate_gold(arma::mat X, arma::Mat<int> Y);
        bool preference_function_L2(arma::vec obj1, arma::vec obj2, arma::vec w_star);
        bool preference_function_InverseChebycheff(arma::vec obj1, arma::vec obj2, double *weight, int dimension);

        // element-wise equality evaluation of two objects
        bool is_equal_matrix(arma::Mat<int> Q1, arma::Mat<int> Q2);

        void judge_convergenced_w_approximate(arma::vec w_new, arma::vec w_old);

    private:
        double *weight_;                   // weight for the DM
        int weight_num_;                   // the number of weight vector

        // gold point
        arma::vec w_star_;

        // ambiguous hyperplanes and its associated label
        arma::mat ambiguous_hyperplanes;
        arma::Mat<int> label_of_ambiguous_hyperplanes;

        // approximated point by active ranking
        arma::vec w_approximate_;

        // ambiguous query in total (serval rankings)
        int num_ambiguous_query;

        // linear problem programming succeed or fail
        bool is_lp_solve_suceed_;

        // is w_approximate convergenced
        bool is_w_approximate_convergenced_;

    };

}