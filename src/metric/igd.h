#pragma once
#include <string>

#include "core/individual.h"

namespace emoc {

	double CalculateIGD(Individual **pop,int pop_num, int obj_num, std::string &problem_name);

}