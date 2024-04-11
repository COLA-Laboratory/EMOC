#pragma once

#include "stdafx.h"

class Arm_DTS {
public:
    Arm_DTS();
    Arm_DTS(const vec& probabilities);

    friend ostream& operator<<(ostream& os, const Arm_DTS& a);

    //Will return true if this arm won
    bool compareWithArm(size_t index);

    /* Will give you the double value for probability. Used for calculation of
     * regret, and evaluating performance but should not be used for the algorithm
     */
    double getProbability(size_t index);

    ~Arm_DTS();


private:
    //Defined as the probability that THIS arm will win against the arm in the index
    vec probabilities;
};


ostream& operator<<(ostream& os, const Arm_DTS& a);
