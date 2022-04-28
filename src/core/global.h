#pragma once
#include <string>
#include <vector>

#include "core/individual.h"
#include "problem/problem.h"
#include "algorithm/algorithm.h"

namespace emoc {

	// Global class holds all necessary parameter settings and datas for algorithms to run and
	// provides some useful foundmental functions.
	class Global
	{
	public:
		// operator parameter structures
		typedef struct
		{
			double crossover_pro;
			double eta_c;
		}SBXPara;

		typedef struct
		{
			double crossover_pro;
			double F;
			double K;
		}DEPara;

		typedef struct
		{
			double muatation_pro;
			double eta_m;
		}PolyMutationPara;

	public:
		Global(const char* algorithn_name, const char* problem_name, int population_num,
			int dec_num, int obj_num, int max_evaluation, int thread_id, int output_interval, int run_id = 0);
		~Global();

		void Init();
		void Start();

		void InitializePopulation(Individual** pop, int pop_num);	// initialize given population, i.e. set the decision variables' value
		void InitializeIndividual(Individual* ind);					// initialize given individual, i.e. set the decision variables' value

		// for python dlls
		void SetCustomProblem(Problem* problem);
		void SetCustomInitialPop(std::vector<std::vector<double>>& initial_pop);

	public:
		int dec_num_;
		int obj_num_;
		int population_num_;
		int max_evaluation_;
		int output_interval_;
		std::string algorithm_name_;
		std::string problem_name_;

		std::vector<Individual*> parent_population_;
		std::vector<Individual*> offspring_population_;
		std::vector<Individual*> mixed_population_;

		std::vector<double> dec_lower_bound_;	// set by problem's lower bound
		std::vector<double> dec_upper_bound_;	// set by problem's lower bound

		SBXPara sbx_parameter_;
		DEPara de_parameter_;
		PolyMutationPara pm_parameter_;

		int iteration_num_;
		int current_evaluation_;
		Problem* problem_;
		Algorithm* algorithm_;

		int run_id_;
		int thread_id_;

		// for python dlls
		bool is_customized_init_pop_;
		bool is_customized_problem_;

	private:
		void SetDecBound();
		void InitializeProblem();
		void InitializeAlgorithm();
		void AllocateMemory();
		void DestroyMemory();
	};

}