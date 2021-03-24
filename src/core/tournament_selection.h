#pragma once
#include "core/individual.h"

namespace emoc {

	Individual* TournamentByRank(Individual *ind1, Individual *ind2);
	Individual* TournamentByFitness(Individual *ind1, Individual *ind2, int greater_is_better = 0);

}