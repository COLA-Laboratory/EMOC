#pragma once
#include <string>
#include <vector>

#include "core/individual.h"

namespace emoc {

	double CalculateIGD(Individual** pop, int pop_num, int obj_num, std::string& problem_name);
	double CalculateIGD(const std::vector<std::vector<double>> &pop, int pop_num, int obj_num, std::string& problem_name);

}



