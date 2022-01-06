#define IMGUI_DEFINE_MATH_OPERATORS
#include "ui/ui_utility.h"

#include <vector>
#include <iostream>
#include <thread>

#include "core/emoc_manager.h"
#include "imgui.h"
#include "imgui_internal.h"

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


	void InitAlgorithmList(std::vector<char*> &algorithm_names)
	{
		algorithm_names.push_back("MOEAD");
		algorithm_names.push_back("MOEADDE");
		algorithm_names.push_back("MOEADDRA");
		algorithm_names.push_back("MOEADFRRMAB");
		algorithm_names.push_back("MOEADGRA");
		algorithm_names.push_back("MOEADIRA");
		algorithm_names.push_back("MOEADDYTS");
		algorithm_names.push_back("MOEADCDE");
		algorithm_names.push_back("ENSMOEAD");
		algorithm_names.push_back("NSGA2");
		algorithm_names.push_back("SPEA2");
		algorithm_names.push_back("IBEA");
		algorithm_names.push_back("HYPE");
		algorithm_names.push_back("SMSEMOA");
	}

	void InitDecompositionAlgorithmList(std::vector<char*>& decomposition_algorithm_names)
	{
		decomposition_algorithm_names.push_back("MOEAD");
		decomposition_algorithm_names.push_back("MOEADDE");
		decomposition_algorithm_names.push_back("MOEADDRA");
		decomposition_algorithm_names.push_back("MOEADFRRMAB");
		decomposition_algorithm_names.push_back("MOEADGRA");
		decomposition_algorithm_names.push_back("MOEADIRA");
		decomposition_algorithm_names.push_back("ENSMOEAD");
		decomposition_algorithm_names.push_back("MOEADDYTS");
		decomposition_algorithm_names.push_back("MOEADCDE");
	}

	void InitDominanceAlgorithmList(std::vector<char*>& dominance_algorithm_names)
	{
		dominance_algorithm_names.push_back("NSGA2");
		dominance_algorithm_names.push_back("SPEA2");
	}

	void InitIndicatorAlgorithmList(std::vector<char*>& indicator_algorithm_names)
	{
		indicator_algorithm_names.push_back("IBEA");
		indicator_algorithm_names.push_back("HYPE");
		indicator_algorithm_names.push_back("SMSEMOA");
	}

	void InitAlgorithmCategoryList(std::vector<char*>& algorithm_category)
	{
		algorithm_category.push_back("Docomposition Based");
		algorithm_category.push_back("Dominance Based");
		algorithm_category.push_back("Indicator Based");
	}

	void InitProlbemList(std::vector<char*>& problem_names)
	{
		problem_names.push_back("DTLZ1");
		problem_names.push_back("DTLZ2");
		problem_names.push_back("DTLZ3");
		problem_names.push_back("DTLZ4");
		problem_names.push_back("DTLZ5");
		problem_names.push_back("DTLZ6");
		problem_names.push_back("DTLZ7");
		problem_names.push_back("UF1");
		problem_names.push_back("UF2");
		problem_names.push_back("UF3");
		problem_names.push_back("UF4");
		problem_names.push_back("UF5");
		problem_names.push_back("UF6");
		problem_names.push_back("UF7");
		problem_names.push_back("UF8");
		problem_names.push_back("UF9");
		problem_names.push_back("UF10");
		problem_names.push_back("ZDT1");
		problem_names.push_back("ZDT2");
		problem_names.push_back("ZDT3");
		problem_names.push_back("ZDT4");
		problem_names.push_back("ZDT6");
	}

	void InitDTLZList(std::vector<char*>& dtlz_names)
	{
		dtlz_names.push_back("DTLZ1");
		dtlz_names.push_back("DTLZ2");
		dtlz_names.push_back("DTLZ3");
		dtlz_names.push_back("DTLZ4");
		dtlz_names.push_back("DTLZ5");
		dtlz_names.push_back("DTLZ6");
		dtlz_names.push_back("DTLZ7");
		dtlz_names.push_back("MDTLZ1");
		dtlz_names.push_back("MDTLZ2");
		dtlz_names.push_back("MDTLZ3");
		dtlz_names.push_back("MDTLZ4");
	}

	void InitZDTList(std::vector<char*>& zdt_names)
	{
		zdt_names.push_back("ZDT1");
		zdt_names.push_back("ZDT2");
		zdt_names.push_back("ZDT3");
		zdt_names.push_back("ZDT4");
		zdt_names.push_back("ZDT6");
	}

	void InitUFList(std::vector<char*>& uf_names)
	{
		uf_names.push_back("UF1");
		uf_names.push_back("UF2");
		uf_names.push_back("UF3");
		uf_names.push_back("UF4");
		uf_names.push_back("UF5");
		uf_names.push_back("UF6");
		uf_names.push_back("UF7");
		uf_names.push_back("UF8");
		uf_names.push_back("UF9");
		uf_names.push_back("UF10");
	}

	void InitWFGList(std::vector<char*>& wfg_names)
	{
		wfg_names.push_back("WFG1");
		wfg_names.push_back("WFG2");
		wfg_names.push_back("WFG3");
		wfg_names.push_back("WFG4");
		wfg_names.push_back("WFG5");
		wfg_names.push_back("WFG6");
		wfg_names.push_back("WFG7");
		wfg_names.push_back("WFG8");
		wfg_names.push_back("WFG9");
	}

	void InitLSMOPList(std::vector<char*>& lsmop_names)
	{
		lsmop_names.push_back("LSMOP1");
		lsmop_names.push_back("LSMOP2");
		lsmop_names.push_back("LSMOP3");
		lsmop_names.push_back("LSMOP4");
		lsmop_names.push_back("LSMOP5");
		lsmop_names.push_back("LSMOP6");
		lsmop_names.push_back("LSMOP7");
		lsmop_names.push_back("LSMOP8");
		lsmop_names.push_back("LSMOP9");
	}

	void InitProblemCategoryList(std::vector<char*>& problem_category)
	{
		problem_category.push_back("DTLZ Series");
		problem_category.push_back("ZDT Series");
		problem_category.push_back("UF Series");
		problem_category.push_back("WFG Series");
		problem_category.push_back("LSMOP Series");
		//problem_category.push_back("BT Series");
	}

	void InitDisplayList(std::vector<char*>& display_names)
	{
		display_names.push_back("Runtime");
		display_names.push_back("IGD");
		display_names.push_back("HV");
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
		if (D < 0)
		{
			description = "The D parameter cannot be negative integer!\n\n";
			res = false;
		}
		if (M < 0)
		{
			description = "The M parameter cannot be negative integer!\n\n";
			res = false;
		}
		if (N < 0)
		{
			description = "The N parameter cannot be negative integer!\n\n";
			res = false;
		}
		if (Evaluation < 0)
		{
			description = "The Evaluation parameter cannot be negative integer!\n\n";
			res = false;
		}

		std::string problem_name(problem);
		for (auto& c : problem_name)
		{
			if (c >= '0' && c <= '9') continue;
			c = tolower(c);
		}

		//std::cout << problem_name << std::endl;
		if (problem_name.substr(0,3) == "zdt")
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
		else if (problem_name.substr(0, 4) == "dtlz"|| problem_name.substr(0, 5) == "mdtlz")
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
		else if (problem_name.substr(0, 2) == "uf")
		{
			if (problem_name[2]>= '1' && problem_name[2] <= '7')
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
			else if (problem_name[2] >= '8' && problem_name[2] <= '10')
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
		else if (problem_name.substr(0, 3) == "wfg")
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
			GetComparedMetric(format, res.igd_mean, res.igd_std, res.igd_median, res.igd_iqr, metric1, metric2);
		}
		else if (para == "Runtime")
		{
			GetComparedMetric(format, res.runtime_mean, res.runtime_std, res.runtime_median, res.runtime_iqr, metric1, metric2);
		}
		else if (para == "HV")
		{
			GetComparedMetric(format, res.hv_mean, res.hv_std, res.hv_median, res.hv_iqr, metric1, metric2);
		}
		else
		{
			// TODO
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

	bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size)
	{
		using namespace ImGui;
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		ImGuiID id = window->GetID("##Splitter");
		ImRect bb;
		bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
		bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
		return ImGui::SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
	}

	CategorizedAlgorithmList::CategorizedAlgorithmList()
	{
		InitAlgorithmCategoryList(algorithm_category);
		InitDecompositionAlgorithmList(decomposition_algorithm_names);
		InitDominanceAlgorithmList(dominance_algorithm_names);
		InitIndicatorAlgorithmList(indicator_algorithm_names);
	}

	CategorizedAlgorithmList::~CategorizedAlgorithmList()
	{

	}

	CategorizedProblemList::CategorizedProblemList()
	{
		InitProblemCategoryList(problem_category);
		InitDTLZList(dtlz_names);
		InitZDTList(zdt_names);
		InitUFList(uf_names);
		InitWFGList(wfg_names);
		InitLSMOPList(lsmop_names);
	}

	CategorizedProblemList::~CategorizedProblemList()
	{

	}

}