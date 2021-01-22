#pragma once
#include "individual.h"

namespace emoc {

	void PolynomialMutation(Individual *ind);

	void MutationPop(Individual **pop, int pop_num);
	void MutationInd(Individual *ind);

}