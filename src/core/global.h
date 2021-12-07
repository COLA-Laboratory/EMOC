#pragma once
#define EMOC_INF 1.0e9
#define EMOC_EPS 1.0e-6
#define PI 3.14159265358979323846
#define MAX_THREAD_NUM 20
#define MAX_BUFFSIZE 256

#include <string>
#include <vector>

#include "core/individual.h"
#include "problem/problem.h"
#include "algorithms/algorithm.h"



namespace emoc {

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



	// Global class holds all necessary parameter settings and datas for algorithms to run and
	// provides some useful foundmental functions.
	// Note: the structure may be changed
	class Global
	{
	public:
		Global(const char *algorithn_name, const char *problem_name, int population_num, 
			int dec_num, int obj_num, int max_evaluation, int thread_num,int output_interval, int run_id = 0);
		~Global();

		void Init();
		void Start();

		// utility functions called by concrete algorithm
		void InitializePopulation(Individual** pop, int pop_num);	// initialize given population, i.e. set the decision variables' value
		void InitializeIndividual(Individual* ind);

	public:
		int dec_num_;
		int obj_num_;
		int population_num_;
		int iteration_num_;
		int current_evaluation_;
		int max_evaluation_;
		int output_interval_;
		std::string algorithm_name_;
		std::string problem_name_;

		std::vector<Individual*> parent_population_;
		std::vector<Individual*> offspring_population_;
		std::vector<Individual*> mixed_population_;

		double *dec_lower_bound_;	// set by problem's lower bound
		double *dec_upper_bound_;	// set by problem's lower bound

		SBXPara sbx_parameter_;
		DEPara de_parameter_;
		PolyMutationPara pm_parameter_;

		Problem *problem_;				
		Algorithm *algorithm_;

		int run_id_;
		int thread_id_;

	private:
		void SetDecBound();
		void InitializeProblem();
		void InitializeAlgorithm();
		void AllocateMemory();
		void DestroyMemory();
	};

	//extern Global *g_GlobalSettingsArray[MAX_THREAD_NUM];  // provide global settings to whole project 
}