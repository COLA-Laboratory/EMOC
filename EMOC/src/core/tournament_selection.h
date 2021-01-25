#pragma once
#include "core/individual.h"

namespace emoc {

	// use tournament selection to choose one winner based on dominance rank.
	Individual* TournamentByRank(Individual *ind1, Individual *ind2);

}