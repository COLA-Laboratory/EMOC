#define IMGUI_DEFINE_MATH_OPERATORS
#include "ui/ui_utility.h"

#include <vector>
#include <iostream>
#include <thread>
#include <algorithm>

#include "core/emoc_manager.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "problem/problem_factory.h"
#include "algorithms/algorithm_factory.h"

namespace emoc {

	// Center Help Function
	void TextCenter(std::string text)
	{
		float window_width = ImGui::GetWindowSize().x;
		float text_width = ImGui::CalcTextSize(text.c_str()).x;
		ImGui::SetCursorPosX(ImGui::GetCursorPosX()+window_width / 2 - text_width / 2);
		ImGui::Text(text.c_str());
	}

	void TextCenterInTableCell(std::string text, float height)
	{
		float width = ImGui::GetContentRegionAvail().x;
		float text_width = ImGui::CalcTextSize(text.c_str()).x;
		float text_height = ImGui::GetTextLineHeightWithSpacing();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + width / 2 - text_width / 2);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + height / 2 - text_height / 2);
		ImGui::Text(text.c_str());
	}

	// Tooltip Help Function
	void HelpMarker(const char* desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	void InitAlgorithmCategoryList(std::vector<std::string>& algorithm_category_list)
	{
		std::unordered_map<std::string, std::vector<char*>>& IMPLEMENTED_ALGORITHMS = AlgorithmFactory::Instance()->GetImplementedAlgorithmsName();
		for (const auto& n : IMPLEMENTED_ALGORITHMS)
		{
			algorithm_category_list.push_back(n.first);
		}
		std::sort(algorithm_category_list.begin(), algorithm_category_list.end());
	}

	void InitProblemCategoryList(std::vector<std::string>& problem_category_list)
	{
		std::unordered_map<std::string, std::vector<char*>>& IMPLEMENTED_PROBLEMS = ProblemFactory::Instance()->GetImplementedProblemsName();
		for (const auto& n : IMPLEMENTED_PROBLEMS)
		{
			problem_category_list.push_back(n.first);
		}
		std::sort(problem_category_list.begin(), problem_category_list.end());
	}

	void InitDisplayList(std::vector<char*>& display_names)
	{
		display_names.push_back("Runtime");
		display_names.push_back("IGD");
		display_names.push_back("HV");
		display_names.push_back("GD");
		display_names.push_back("Spacing");
		display_names.push_back("IGDPlus");
		display_names.push_back("GDPlus");
	}

	void InitFormatList(std::vector<char*>& format_names)
	{
		format_names.push_back("Mean");
		format_names.push_back("Mean(STD)");
		format_names.push_back("Median");
		format_names.push_back("Median(IQR)");
	}

	void InitHypothesisList(std::vector<char*>& hypothesis_names)
	{
		hypothesis_names.push_back("RankSumTest");
		hypothesis_names.push_back("SignRankTest");

	}

	void InitPlotMetricList(std::vector<char*>& plot_metric_names)
	{
		plot_metric_names.push_back("IGD");
		plot_metric_names.push_back("Population");
	}

	void DisplayAlgorithmParameters(const std::string& algorithm)
	{
		// TODO... according the algorithm name to display different parameter settings
		//if(algorithm == "MOEAD")
		//else if()
	}

	bool CheckProblemParameters(const std::string& problem, int D, int M, int N, int Evaluation, std::string& description)
	{
		bool res = true;
		if (D <= 0)
		{
			description = "The D parameter should be greater than 0!\n\n";
			res = false;
		}
		if (M <= 0)
		{
			description = "The M parameter should be greater than 0!\n\n";
			res = false;
		}
		if (N <= 0)
		{
			description = "The N parameter should be greater than 0!\n\n";
			res = false;
		}
		if (Evaluation <= 0)
		{
			description = "The Evaluation parameter should be greater than 0!\n\n";
			res = false;
		}

		std::string problem_name(problem);
		for (auto& c : problem_name)
		{
			if (c >= '0' && c <= '9') continue;
			c = tolower(c);
		}

		//std::cout << problem_name << std::endl;
		if (problem_name.size() >= 3 && problem_name.substr(0,3) == "zdt")
		{
			if (M != 2)
			{
				description = "The M parameter of " + problem + " must be 2!\n\n";
				res = false;
			}

			if (D < 2)
			{
				description = "The D parameter of " + problem + " must greater than 1!\n\n";
				res = false;
			}
		}
		else if ((problem_name.size() >= 4 && problem_name.substr(0, 4) == "dtlz")|| (problem_name.size() >= 5 && problem_name.substr(0, 5) == "mdtlz"))
		{
			if (M <= 1)
			{
				description = "The M parameter of " + problem + " must greater than 1!\n\n";
				res = false;
			}

			if (D < M - 1)
			{
				description = "The D parameter of " + problem + " must greater than M - 2!\n\n";
				res = false;
			}
		}
		else if (problem_name.size() >= 2 && problem_name.substr(0, 2) == "uf")
		{
			if (problem_name[2]>= '1' && problem_name[2] <= '7' && problem_name.size() == 3)
			{
				if (M != 2)
				{
					description = "The M parameter of " + problem + " must be 2!\n\n";
					res = false;
				}
				
				if (D < 3)
				{
					description = "The D parameter of " + problem + " must greater than 2!\n\n";
					res = false;
				}
			}
			else
			{
				if (M != 3)
				{
					description = "The M parameter of " + problem + " must be 3!\n\n";
					res = false;
				}

				if (D < 5)
				{
					description = "The D parameter of " + problem + " must greater than 4!\n\n";
					res = false;
				}
			}
		}
		else if (problem_name.size() >= 3 && problem_name.substr(0, 3) == "wfg")
		{
			// TODO ... wfg needs extra parameter which is not implemented now.
			int defualt_WFGK = 10;

			if (M > defualt_WFGK + 1)
			{
				description = "The M parameter of " + problem + " must less than K + 2! (K is a parameter of WFG problem, which is 10 as defualt in EMOC. You can change it in wfg.cpp file.)\n\n";
				res = false;
			}

			if (D < defualt_WFGK)
			{
				std::cout << "here!\n";
				description = "The D parameter of " + problem + " must greater than K! (K is a parameter of WFG problem, which is 10 as defualt in EMOC. You can change it in wfg.cpp file.)\n\n";
				res = false;
			}
		}
		else if (problem_name.size() >= 9 && problem_name.substr(0, 9) == "moeadde_f")
		{
			if (problem_name[9] == '6')
			{
				if (M != 3)
				{
					description = "The M parameter of " + problem + " must be 3!\n\n";
					res = false;
				}

				if (D < 5)
				{
					description = "The D parameter of " + problem + " must greater than 4!\n\n";
					res = false;
				}
			}
			else
			{
				if (M != 2)
				{
					description = "The M parameter of " + problem + " must be 2!\n\n";
					res = false;
				}

				if (D < 3)
				{
					description = "The D parameter of " + problem + " must greater than 2!\n\n";
					res = false;
				}
			}
		}
		else if (problem_name.size() >= 2 && problem_name.substr(0, 2) == "bt")
		{
			if (problem_name[2] >= '1' && problem_name[2] <= '8')
			{
				if (M != 2)
				{
					description = "The M parameter of " + problem + " must be 2!\n\n";
					res = false;
				}
			}
			else if (problem_name[2] == '9')
			{
				if (M != 3)
				{
					description = "The M parameter of " + problem + " must be 3!\n\n";
					res = false;
				}

				if (D <= 1)
				{
					description = "The D parameter of " + problem + " must greater than 1!\n\n";
					res = false;
				}
			}
		}
		else if (problem_name.size() >= 8 && problem_name.substr(0, 8) == "immoea_f")
		{
			if (problem_name[8] >= '4' && problem_name[8] <= '8')
			{
				if (M != 3)
				{
					description = "The M parameter of " + problem + " must be 3!\n\n";
					res = false;
				}

				if (D <= 2)
				{
					description = "The D parameter of " + problem + " must greater than 2!\n\n";
					res = false;
				}
			}
			else
			{
				if (M != 2)
				{
					description = "The M parameter of " + problem + " must be 2!\n\n";
					res = false;
				}

				if (D <= 1)
				{
					description = "The D parameter of " + problem + " must greater than 1!\n\n";
					res = false;
				}
			}
		}
		else if (problem_name.size() >= 10 && problem_name.substr(0, 10) == "moeadm2m_f")
		{
			if (problem_name[10] >= '6' && problem_name[10] <= '7')
			{
				if (M != 3)
				{
					description = "The M parameter of " + problem + " must be 3!\n\n";
					res = false;
				}

				if (D <= 1)
				{
					description = "The D parameter of " + problem + " must greater than 1!\n\n";
					res = false;
				}
			}
			else
			{
				if (M != 2)
				{
					description = "The M parameter of " + problem + " must be 2!\n\n";
					res = false;
				}

				if (D <= 0)
				{
					description = "The D parameter of " + problem + " must greater than 0!\n\n";
					res = false;
				}
			}
		}
		else
		{
			// TODO: ADD MORE TEST PROBLEMS
		}

		return res;
	}

	bool CheckTestPlotSettings(int size_x, int size_y, int offset_x, int offset_y, std::string& description)
	{
		bool res = true;
		if (size_x < 0)
		{
			description = "The width of plot window cannot be negative integer!\n\n";
			res = false;
		}
		if (size_y < 0)
		{
			description = "The height of plot window cannot be negative integer!\n\n";
			res = false;
		}
		if (offset_x < 0)
		{
			description = "The horizontal offset of plot window cannot be negative integer!\n\n";
			res = false;
		}
		if (offset_y < 0)
		{
			description = "The vertical offset of plot window cannot be negative integer!\n\n";
			res = false;
		}
		return res;
	}

	bool CheckExpSettings(int thread_num, int runs_num, int save_interval, std::string& description)
	{
		bool res = true;
		int max_threadnum = std::thread::hardware_concurrency();

		if (thread_num < 0)
		{
			description = "The number of thread cannot be negative integer!\n\n";
			res = false;
		}
		else if (thread_num > max_threadnum)
		{
			description = "We recommend the number of thread should not beyond your cpu cores:"+ std::to_string(max_threadnum)+"!\n\n";
			res = false;
		}

		if (runs_num < 0)
		{
			description = "The number of runs cannot be negative integer!\n\n";
			res = false;
		}

		if (save_interval < 0)
		{
			description = "The interval of population save cannot be negative integer!\n\n";
			res = false;
		}

		return res;
	}

	void GetComparedMetric(const std::string& para, const std::string& format, int parameter_index, double& metric1, double& metric2)
	{
		EMOCMultiThreadResult res = EMOCManager::Instance()->GetMultiThreadResult(parameter_index);
		if (res.valid_res_count == 0)
		{
			metric1 = EMOC_INF;
			metric2 = EMOC_INF;
			return;
		}

		if (para == "IGD")
		{
			GetComparedMetric(format, res.igd.metric_mean, res.igd.metric_std, res.igd.metric_median, res.igd.metric_iqr, metric1, metric2);
		}
		else if (para == "Runtime")
		{
			GetComparedMetric(format, res.runtime.metric_mean, res.runtime.metric_std, res.runtime.metric_median, res.runtime.metric_iqr, metric1, metric2);
		}
		else if (para == "HV")
		{
			GetComparedMetric(format, res.hv.metric_mean, res.hv.metric_std, res.hv.metric_median, res.hv.metric_iqr, metric1, metric2);
		}
		else if(para == "GD")
		{
			GetComparedMetric(format, res.gd.metric_mean, res.gd.metric_std, res.gd.metric_median, res.gd.metric_iqr, metric1, metric2);
		}
		else if (para == "Spacing")
		{
			GetComparedMetric(format, res.spacing.metric_mean, res.spacing.metric_std, res.spacing.metric_median, res.spacing.metric_iqr, metric1, metric2);
		}
		else if (para == "IGDPlus")
		{
			GetComparedMetric(format, res.igdplus.metric_mean, res.igdplus.metric_std, res.igdplus.metric_median, res.igdplus.metric_iqr, metric1, metric2);
		}
		else if (para == "GDPlus")
		{
			GetComparedMetric(format, res.gdplus.metric_mean, res.gdplus.metric_std, res.gdplus.metric_median, res.gdplus.metric_iqr, metric1, metric2);
		}
		else
		{
			// TODO: add more metrics
		}
	}

	void GetComparedMetric(const std::string& format, double mean, double std, double median, double iqr, double& metric1, double& metric2)
	{
		if (format == "Mean" || format == "Mean(STD)")
		{
			metric1 = mean;
			metric2 = std;
		}
		else if (format == "Median" || format == "Median(IQR)")
		{
			metric1 = median;
			metric2 = iqr;
		}
	}

	bool ItemGet(void* vec, int idx, const char** out_text)
	{
		std::vector<std::string>* vector = reinterpret_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= vector->size())return false;
		*out_text = vector->at(idx).c_str();
		return true;
	}

	void SelectCurrentAlgorithmCombo(const std::string& category, std::vector<char*>** current_algorithm_name)
	{

		std::unordered_map<std::string, std::vector<char*>>& IMPLEMENTED_ALGORITHMS = AlgorithmFactory::Instance()->GetImplementedAlgorithmsName();
		*current_algorithm_name = &IMPLEMENTED_ALGORITHMS[category];
	}

	void SelectCurrentProblemCombo(const std::string& category, std::vector<char*>** current_problem_name)
	{
		std::unordered_map<std::string, std::vector<char*>>& IMPLEMENTED_PROBLEMS = ProblemFactory::Instance()->GetImplementedProblemsName();
		*current_problem_name = &IMPLEMENTED_PROBLEMS[category];
	}
}