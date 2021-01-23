#pragma once
#include "core/global.h"
#include "core/individual.h"

namespace emoc {

	enum DominateReleation 
	{
		DOMINATED = -1,
		NON_DOMINATED = 0,
		DOMINATE = 1
	};

	DominateReleation CheckDominance(Individual *ind1, Individual *ind2);

}