#pragma once
#include <vector>
#include <string>
#include <unordered_map>


namespace emoc{

	class ExperimentTable
	{
	public:
		ExperimentTable();
		~ExperimentTable();

		void Render(bool is_displayM, bool is_displayD, bool is_displayN, bool is_displayEvaluation, const std::string& display_para);

		void UpdateExperimentTable(const std::vector<std::string>& algorithms, const std::vector<std::string>& problems,
			const std::vector<int>& Ms, const std::vector<int>& Ds, const std::vector<int>& Ns, const std::vector<int>& Evaluations, const std::vector<int>& parameter_indexes);
		void PrintTable();

	private:
		void DisplayTableProblemProperty(const std::string& col_name, int row, int row_height);	
		void DisplayTableResult(int row, int row_height, int col_in_algorithms, const std::string& para);

	public:
		struct ProblemTableProperty
		{
			int M;
			int D;
			int N;
			int Evaluation;
			int parameter_index;		// Parameter index for first algorithm, so the following algorithms' parameter indexes are increasing by their index
		};

		struct TableRowProperty
		{
			std::string problem_name;
			int M;
			std::vector<int> Ds;
			std::vector<int> Ns;
			std::vector<int> Evaluations;
			std::vector<int> parameter_indexes; // each element of this array is similar to ProblemTableProperty's parameter_index
		};

		

		std::unordered_map<std::string, std::vector<ProblemTableProperty>> problem_names;
		std::vector<std::string> algorithm_names;
		
		std::vector<TableRowProperty> rows;
	};

}