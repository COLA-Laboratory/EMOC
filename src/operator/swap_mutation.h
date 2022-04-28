#pragma once
#include "core/global.h"
#include "core/individual.h"
#include "core/emoc_utility_structures.h"

namespace emoc {

	// simulated binary crossover
	void SwapMutation(Individual *ind, MutationParameter &mutation_para);

}