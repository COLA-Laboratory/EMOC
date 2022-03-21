#pragma once
#include <string>
#include <vector>

#include "core/global.h"
#include "core/individual.h"
#include "core/emoc_manager.h"

namespace emoc {

	void PrintObjective(const char* filename, int obj_num, Individual** pop_table, int pop_num);
	std::vector<std::vector<double>> ReadPop(char* filepath, int obj_num);
	int CreateDirectory(const std::string& path);
	void RecordPop(int run_index, int generation, Global* para, int real_popnum);
	void FormalizeStr(char* buff);
	void ReadParametersFromFile(const char* filename, EMOCParameters* para);
	void CopyFile(const char* src, const char* dest);

	bool CheckEMOCParameter(const EMOCParameters& para, std::string& description);
	bool EMOCAlgorithmCheck(const std::string &algorithm, std::string& description);
	bool EMOCProblemCheck(const std::string& problem, int M, int D, int N, int Evaluation, std::string& description);
	void EMOCParamerterParse(int argc, char* argv[], EMOCParameters &para, bool &is_gui);

}
