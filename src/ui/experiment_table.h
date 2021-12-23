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

		// render function for table
		void Render(bool is_displayM, bool is_displayD, bool is_displayN, bool is_displayEvaluation, 
			const std::string& display_para, const std::string& format, const std::string& hypothesis, const std::string& comp);
		bool Save();

		// update table data and reformat its orders
		void UpdateExperimentTable(const std::vector<std::string>& algorithms, const std::vector<std::string>& problems,
			const std::vector<int>& Ms, const std::vector<int>& Ds, const std::vector<int>& Ns, const std::vector<int>& Evaluations, const std::vector<int>& parameter_indexes);
		
		// print some useful informations
		void PrintTable();
		void PrintMetric(std::string metric_name);


	private:
		void DisplayTableProblemProperty(const std::string& col_name, int row, int row_height);	
		void DisplayTableResult(int row, int row_height, int col_in_algorithms, const std::string& para, const std::string& format, const std::string& hypothesis);
		
		void SetTableContent(char* display, const std::string& format, double mean, double std, double median, double iqr);
		bool CheckIsBest(const std::string& para, const std::string& format, int parameter_index);
		char GetHypothesisSymbol(int mean_ranksum, int median_ranksum, int mean_signrank, int median_signrank,
			const std::string& hypothesis, const std::string& format);

		// The save path is fixed now, it maybe improved in following version
		bool CreateDirectTable();
		bool SaveToCSV();
		bool SaveToLatex();

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

		// current table state
		bool isM;
		bool isD;
		bool isN;
		bool isEvaluation;
		std::string para_name;
		std::string format_name;
		std::string hypothesis_name;
		std::string comp_name;

		// current table content (no order)
		std::unordered_map<std::string, std::vector<ProblemTableProperty>> problem_map;
		std::vector<std::string> algorithm_names;
		
		// real table order in UI display 
		std::vector<TableRowProperty> rows;

		std::vector<std::vector<std::string>> direct_table;
	};

}