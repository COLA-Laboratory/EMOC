#pragma once
#include "core/individual.h"
namespace emoc {

	//do non-dominated sorting, and the rank results are stored in individual's member data.
	void NonDominatedSort(Individual **pop, int pop_num, int obj_num);

}