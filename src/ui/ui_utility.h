#pragma once
#include <vector>
#include <string>

namespace emoc{

	// text center help function
	void TextCenter(std::string text);

	// tooltip help function
	void HelpMarker(const char* desc);

	// init function for different lists
	void InitAlgorithmList(std::vector<char*>& algorithm_names);
	void InitProlbemList(std::vector<char*>& problem_names);
	void InitDisplayList(std::vector<char*>& display_names);

	void DisplayAlgorithmParameters(const std::string& algorithm);				// display algorithm's parameters

}