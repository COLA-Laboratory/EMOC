#include "stdafx.h"


#include "Arm.h"

Arm_DTS::Arm_DTS() : Arm_DTS(mat())
{

}

Arm_DTS::Arm_DTS(const vec& probabilities)
    : probabilities(probabilities)
{

}

double Arm_DTS::getProbability(size_t index) {
    return ((index >= 0 && index < probabilities.n_elem) ? probabilities(index) : -1);
}

ostream& operator<<(ostream& os, const Arm_DTS& a)
{
    os << "Arm_DTS p-values: ";/*{";

    for(size_t i = 0; i < a.probabilities.n_elem; i++) {
        os << fixed << setprecision(2) << a.probabilities[i];
        if(i < a.probabilities.size() - 1)
            os << ", ";
    }

    os << "}";*/

    os << trans(a.probabilities);

    return os;
}

bool Arm_DTS::compareWithArm(size_t index)
{
    /*if(index >= probabilities.size() && index < 0)
        throw exception;*/

    double prob = probabilities(index);
    double randVal = (double)rand() / (double)RAND_MAX;

    return randVal <= prob;
}

Arm_DTS::~Arm_DTS()
{

}
