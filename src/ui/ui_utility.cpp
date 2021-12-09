#define IMGUI_DEFINE_MATH_OPERATORS
#include "ui/ui_utility.h"

#include <vector>
#include <iostream>

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

	void InitProblemCategoryList(std::vector<char*>& problem_category)
	{
		problem_category.push_back("DTLZ Series");
		problem_category.push_back("ZDT Series");
		problem_category.push_back("UF Series");
	}

	void InitDisplayList(std::vector<char*>& display_names)
	{
		display_names.push_back("Runtime");
		display_names.push_back("IGD");
		display_names.push_back("HV");
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
	}

	CategorizedProblemList::~CategorizedProblemList()
	{

	}

}