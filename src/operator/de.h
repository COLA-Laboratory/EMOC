#pragma once
#include <vector>

#include "core/global.h"
#include "core/individual.h"
#include "core/emoc_utility_structures.h"

namespace emoc {

	// differential evolution operator
	void DE(Individual *parent1, Individual *parent2, Individual *parent3, Individual *offspring, 
		std::vector<double>& lower_bound, std::vector<double>& upper_bound, CrossoverParameter& cross_para);
}