#pragma once
#include "core/global.h"
#include "core/individual.h"

namespace emoc {

	// differential evolution operator
	void DE(Individual *parent1, Individual *parent2, Individual *parent3, Individual *offspring, Global *g_GlobalSettings);

}