#pragma once
#include <string>
#include <vector>

#include "core/individual.h"

namespace emoc {

	double CalculateIGDPlus(Individual** pop, int pop_num, int obj_num, double** pf_data, int pf_size);

}



