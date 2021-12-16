#pragma once
#include <string>
#include <vector>

#include "core/global.h"
#include "core/individual.h"

namespace emoc {

	struct EMOCParameters
	{
		std::string algorithm_name;
		std::string problem_name;
		bool is_plot;
		int population_num;
		int decision_num;
		int objective_num;
		int max_evaluation;
		int output_interval;
		int runs_num;
		int is_open_multithread;
		int thread_num;

		EMOCParameters() :
			algorithm_name("NSGA2"),
			problem_name("ZDT1"),
			is_plot(false),
			population_num(100),
			decision_num(30),
			objective_num(2),
			max_evaluation(25000),
			output_interval(EMOC_INF),
			runs_num(1),
			is_open_multithread(false),
			thread_num(0)
		{
		}

		~EMOCParameters()
		{
		}
	};

	void PrintObjective(const char* filename, int obj_num, Individual** pop_table, int pop_num);
	std::vector<std::vector<double>> ReadPop(char* filepath, int obj_num);
	void RecordPop(int run_index, int generation, Global* para, int real_popnum);
	void ReadParametersFromFile(const char* filename, EMOCParameters* para);
	void ParseParamerters(int argc, char* argv[], EMOCParameters* para);
	void FormalizeStr(char* buff);
	int CreateDirectory(const std::string& path);
}