#pragma once
#include <vector>
#include <string>

namespace emoc{

	// text center help function
	void TextCenter(std::string text);
	void TextCenterInTableCell(std::string text, float height);

	// tooltip help function
	void HelpMarker(const char* desc);

	// init function for different lists
	void InitAlgorithmList(std::vector<char*>& algorithm_names);
	void InitDecompositionAlgorithmList(std::vector<char*>& decomposition_algorithm_names);
	void InitDominanceAlgorithmList(std::vector<char*>& dominance_algorithm_names);
	void InitIndicatorAlgorithmList(std::vector<char*>& indicator_algorithm_names);
	void InitAlgorithmCategoryList(std::vector<char*>& algorithm_category);

	void InitProlbemList(std::vector<char*>& problem_names);
	void InitDTLZList(std::vector<char*>& dtlz_names);
	void InitZDTList(std::vector<char*>& zdt_names);
	void InitUFList(std::vector<char*>& uf_names);
	void InitWFGList(std::vector<char*>& wfg_names);
	void InitProblemCategoryList(std::vector<char*>& problem_category);

	void InitDisplayList(std::vector<char*>& display_names);
	void InitFormatList(std::vector<char*>& format_names);
	void InitHypothesisList(std::vector<char*>& hypothesis_names);
	void InitPlotMetricList(std::vector<char*>& plot_metric_names);

	// the splitter function is just for test now, it maybe remove in further commits
	bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f);
	
	// display algorithm's parameters
	void DisplayAlgorithmParameters(const std::string& algorithm);	

	// parameter validity check
	bool CheckProblemParameters(const std::string& problem, int D, int M, int N, int Evaluation, std::string &description);
	bool CheckTestPlotSettings(int size_x, int size_y, int offset_x, int offset_y, std::string& description);
	bool CheckExpSettings(int thread_num, int runs_num, int save_interval, std::string& description);

	struct CategorizedAlgorithmList
	{
		CategorizedAlgorithmList();
		~CategorizedAlgorithmList();

		std::vector<char*> algorithm_category;
		std::vector<char*> decomposition_algorithm_names;
		std::vector<char*> dominance_algorithm_names;
		std::vector<char*> indicator_algorithm_names;

		int category_index = 0;
		//int algorithm_index = 0;	// need to reset when category changed every time
	};

	struct CategorizedProblemList
	{
		CategorizedProblemList();
		~CategorizedProblemList();

		std::vector<char*> problem_category;
		std::vector<char*> dtlz_names;
		std::vector<char*> zdt_names;
		std::vector<char*> uf_names;
		std::vector<char*> wfg_names;

		int category_index = 0;
		//int problem_index = 0;		// need to reset when category changed every time
	};

}