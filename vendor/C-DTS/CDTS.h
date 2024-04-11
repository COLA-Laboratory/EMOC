#pragma once

#include "Arm.h"
#include "stdafx.h"

class CDTS {
public:
    CDTS();
    CDTS(const vector<Arm_DTS>& solutions, const vector<Arm_DTS>& subsets, double learningRate);
    CDTS(const mat& solutions, const mat& subsets, double learningRate);
    ~CDTS();

    std::tuple<int, int, vec> timestep(int max_round, int Trial, int subset_solution_num);

    void updateUCB();

    int winner(int Trial);

    void CalculateLoss(int round);

    void CalculateCopelandScore();

    friend ostream& operator<<(ostream& os, const CDTS& a);

private:
    vector<Arm_DTS> arms;
    vector<Arm_DTS> full_arms;

    mat wins;
    vec wins_full;
    mat ucb;
    mat lcb;
    mat win_prob;
    mat theta_1;
    vec theta_2;
    vec CopelandScore;

    double learningRate;

    long t;

    long best_choose;
    size_t optimal_arm;
    size_t select1;
    size_t select2;
    double cum_regret;
    int max_round;
};

ostream& operator<<(ostream& os, const CDTS& a);
