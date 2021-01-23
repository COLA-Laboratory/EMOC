#pragma once

#include <string>
#include <vector>

#include "individual.h"


#define INF 1.0e14
#define PI 3.14159265358979323846

namespace emoc {
	typedef struct{
		double crossover_pro;
		double eta_c;
	}SBXPara;

	typedef struct{
		double muatation_pro;
		double eta_m;
	}PolyMutationPara;

	class Global
	{
	public:
		Global(const char *algorithn_name, const char *problem_name, int population_num, 
			int dec_num, int obj_num, int max_evaluation);
		~Global();

		void InitializePopulation(Individual **pop, int pop_num);
		void InitializeIndividual(Individual *pop);
		bool IsTermination();

	public:
		int dec_num_;
		int obj_num_;

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
		PolyMutationPara pm_parameter_;

	private:
		void Init();
		void SetDecBound();
		void AllocateMemory();
		void DestroyMemory();
	};

	extern Global *g_GlobalSettings;
}