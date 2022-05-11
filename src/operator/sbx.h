#pragma once
#include <vector>

#include "core/global.h"
#include "core/individual.h"
#include "core/emoc_utility_structures.h"

namespace emoc {

	// simulated binary crossover
	void SBX(Individual* parent1, Individual* parent2, Individual* offspring1, Individual* offspring2,
		std::vector<double>& lower_bound, std::vector<double>& upper_bound, CrossoverParameter& cross_para);

}