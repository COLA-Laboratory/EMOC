#include "core/global.h"
#include "core/individual.h"

namespace emoc {

	struct EMOCParameters
	{
		std::string algorithm_name;
		std::string problem_name;
		int population_num;
		int decision_num;
		int objective_num;
		int max_evaluation;
		int output_interval;
		int runs_num;
		int is_open_multithread;
		int thread_num;

		double *igd_value;
	};

	void PrintObjective(const char *filename, int obj_num, Individual **pop_table, int pop_num);
	void RecordPop(int run_index, int generation, Global *para);
	void ReadParametersFromFile(const char *filename, EMOCParameters *para);
	void ParseParamerters(int argc, char *argv[], EMOCParameters *para);
	void FormalizeStr(char *buff);
}