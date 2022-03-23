#pragma once
#include <string>
#include <vector>

#include "core/global.h"
#include "core/individual.h"
#include "core/emoc_manager.h"

namespace emoc {

	// Copy file from src to dest
	void CopyFile(const char* src, const char* dest);

	// Create dirtectory path
	int CreateDirectory(const std::string& path);

	// Read population data from file 
	std::vector<std::vector<double>> ReadPop(char* filepath, int obj_num);

	// Write population data to file
	void RecordPop(int run_index, int generation, Global* para, int real_popnum);

	// EMOC parameter related functions
	bool CheckEMOCParameter(const EMOCParameters& para, std::string& description);
	bool EMOCAlgorithmCheck(const std::string &algorithm, std::string& description);
	bool EMOCProblemCheck(const std::string& problem, int M, int D, int N, int Evaluation, std::string& description);
	void EMOCParamerterParse(int argc, char* argv[], EMOCParameters &para, bool &is_gui);

}
