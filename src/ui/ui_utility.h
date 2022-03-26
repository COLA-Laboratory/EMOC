#pragma once
#include <vector>
#include <string>

namespace emoc{

	// init function for algorithm names' lists
	void InitAlgorithmCategoryList(std::vector<std::string>& algorithm_category_list);
	// init function for problem names' lists
	void InitProblemCategoryList(std::vector<std::string>& problem_category_list);
	// init function for some other lists
	void InitSingleDisplayList(std::vector<char*>& display_names);
	void InitMultiDisplayList(std::vector<char*>& display_names);
	void InitDisplayList(std::vector<char*>& display_names);
	void InitFormatList(std::vector<char*>& format_names);
	void InitHypothesisList(std::vector<char*>& hypothesis_names);
	void InitPlotMetricList(std::vector<char*>& plot_metric_names);

	// text center and tooltip help function
	void TextCenter(std::string text);
	void TextCenterInTableCell(std::string text, float height);
	void HelpMarker(const char* desc);

	// parameter validity check
	bool CheckOptimizationType(std::vector<std::string> algorithms, std::vector<std::string> problems, std::string &description, int *optimization_type = nullptr);
	bool CheckProblemParameters(const std::string& problem, int D, int M, int N, int Evaluation, std::string &description);
	bool CheckTestPlotSettings(int size_x, int size_y, int offset_x, int offset_y, std::string& description);
	bool CheckExpSettings(int thread_num, int runs_num, int save_interval, std::string& description);

	// get metric utilities
	void GetComparedMetric(const std::string& para, const std::string& format, int parameter_index, double& metric1, double& metric2);
	void GetComparedMetric(const std::string& format, double mean, double std, double median, double iqr, double& metric1, double& metric2);

	// item get help function for imgui::combo
	bool ItemGet(void* vec, int idx, const char** out_text);

	// change current algorithm list and problem list functions
	void SelectCurrentAlgorithmCombo(const std::string& category, std::vector<char*>** current_algorithm_name);
	void SelectCurrentProblemCombo(const std::string& category, std::vector<char*>** current_problem_name);


}