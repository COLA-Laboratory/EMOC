#pragma once
#include "core/individual.h"
namespace emoc {

	// Do non-dominated sorting, and the rank results are stored in individual's rank_ variable.
	void NonDominatedSort(Individual **pop, int pop_num, int obj_num, bool is_consider_cons = false);

}