#pragma once
#include <string>

#include "core/individual.h"

namespace emoc {

	double CalculateGD(Individual** pop, int pop_num, int obj_num, double** pf_data, int pf_size);

}