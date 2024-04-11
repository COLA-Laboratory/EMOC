#pragma once

#include "Arm.h"
#include "stdafx.h"

class DTS {
public:
    DTS();
    DTS(const vector<Arm_DTS>& arms, double learningRate = 1);
    DTS(const mat& vals, double learningRate = 1);
    ~DTS();

    void timestep(int max_round, string problem, int Trial);
    int timestep(int max_round, int Trial);

    void updateUCB();

    size_t winner(int Trial);

    void CalculateLoss(int round);

    void CalculateCopelandScore();

    friend ostream& operator<<(ostream& os, const DTS& a);

private:
    vector<Arm_DTS> arms;

    mat wins;
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

ostream& operator<<(ostream& os, const DTS& a);
