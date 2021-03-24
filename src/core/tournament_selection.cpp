#include "core/tournament_selection.h"

#include "random/random.h"
#include <cstdio>
namespace emoc {

	Individual* TournamentByRank(Individual *ind1, Individual *ind2)
	{
		if (ind1->rank_ < ind2->rank_)
		{
			return ind1;
		}
		else if (ind2->rank_ < ind1->rank_)
		{
			return ind2;
		}
		else
		{
			if ( randomperc() <= 0.5)
				return (ind1);
			else
				return (ind2);
		}
	}

	Individual* TournamentByFitness(Individual *ind1, Individual *ind2, int greater_is_better)
	{
		if (ind1->fitness_ < ind2->fitness_)
		{
			return greater_is_better ? ind2 : ind1;
		}
		else if (ind2->fitness_ < ind1->fitness_)
		{
			return greater_is_better ? ind1 : ind2;
		}
		else
		{
			if (randomperc() <= 0.5)
				return (ind1);
			else
				return (ind2);
		}
	}

}