#include "ui/ui_utility.h"

#include <vector>

#include "imgui.h"

namespace emoc {

	// Center Help Function
	void TextCenter(std::string text)
	{
		float window_width = ImGui::GetWindowSize().x;
		float text_width = ImGui::CalcTextSize(text.c_str()).x;
		ImGui::SetCursorPosX(window_width / 2 - text_width / 2);
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
		algorithm_names.push_back("NSGA2");
		algorithm_names.push_back("SPEA2");
		algorithm_names.push_back("IBEA");
		algorithm_names.push_back("ENSMOEAD");
		algorithm_names.push_back("HYPE");
		algorithm_names.push_back("SMSEMOA");
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

	void InitDisplayList(std::vector<char*>& display_names)
	{
		display_names.push_back("Runtime");
		display_names.push_back("IGD");
		display_names.push_back("HV");
	}

	void DisplayAlgorithmParameters(const std::string& algorithm)
	{
		// TODO... according the algorithm name to display different parameter settings
		//if(algorithm == "MOEAD")
		//else if()
	}

}