#pragma once
#define INF 1.0e9
#define PI 3.14159265358979323846
#define EPS 1.0e-6
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
	// provides some useful foundmental functions. It can be considered as a manager class. A
	// global variable g_GlobalSettings is given to access all settings.
	// Note: the structure may be changed
	class Global
	{
	public:
		Global(const char *algorithn_name, const char *problem_name, int population_num, 
			int dec_num, int obj_num, int max_evaluation, int thread_num);
		~Global();

		// initialize given population, i.e. set the decision variables' value
		void InitializePopulation(Individual **pop, int pop_num);
		void InitializeIndividual(Individual *ind);

		void Start();
		bool IsTermination();
		void DisplayMetrics();

	public:
		int dec_num_;
		int obj_num_;
		int thread_num_;

		int population_num_;
		int iteration_num_;
		int current_evaluation_;
		int max_evaluation_;

		std::string algorithm_name_;
		std::string problem_name_;

		std::vector<Individual*> parent_population_;
		std::vector<Individual*> offspring_population_;
		std::vector<Individual*> mixed_population_;

		double *dec_lower_bound_;
		double *dec_upper_bound_;

		SBXPara sbx_parameter_;
		DEPara de_parameter_;
		PolyMutationPara pm_parameter_;

		Problem *problem_;				
		Algorithm *algorithm_;

	private:
		void Init();
		void SetDecBound();
		void InitializeProblem();
		void InitializeAlgorithm();
		void AllocateMemory();
		void DestroyMemory();
	};

	extern Global *g_GlobalSettingsArray[MAX_THREAD_NUM];  // provide global settings to whole project 
}