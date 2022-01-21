#include "algorithms/algorithm_head_collect.h"

#include <string>
#include <vector>
#include <unordered_map>

#include "core/macro.h"
#include "core/emoc_manager.h"


//void InitDecompositionAlgorithmList(std::vector<char*>& decomposition_algorithm_names)
//{
//	decomposition_algorithm_names.push_back("MOEADDYTS    ");
//	decomposition_algorithm_names.push_back("MOEADSWTS    ");
//	decomposition_algorithm_names.push_back("MOEADCDTS    ");
//	decomposition_algorithm_names.push_back("MOEADDTS    ");
//	decomposition_algorithm_names.push_back("MOEAD       ");
//	decomposition_algorithm_names.push_back("MOEADDE    ");
//	decomposition_algorithm_names.push_back("MOEADDRA     ");
//	decomposition_algorithm_names.push_back("MOEADFRRMAB    ");
//	decomposition_algorithm_names.push_back("MOEADGRA        ");
//	decomposition_algorithm_names.push_back("MOEADIRA        ");
//	decomposition_algorithm_names.push_back("ENSMOEAD        ");
//	decomposition_algorithm_names.push_back("MOEADCDE        ");
//	decomposition_algorithm_names.push_back("MOEADSTM        ");
//	decomposition_algorithm_names.push_back("RVEA         ");
//	decomposition_algorithm_names.push_back("MOEADPAS          ");
//	decomposition_algorithm_names.push_back("MOEADM2M          ");
//}
//
//void InitDominanceAlgorithmList(std::vector<char*>& dominance_algorithm_names)
//{
//	dominance_algorithm_names.push_back("NSGA2");
//	dominance_algorithm_names.push_back("SPEA2");
//}
//
//void InitIndicatorAlgorithmList(std::vector<char*>& indicator_algorithm_names)
//{
//	indicator_algorithm_names.push_back("IBEA");
//	indicator_algorithm_names.push_back("HYPE");
//	indicator_algorithm_names.push_back("SMSEMOA");
//}

namespace emoc {

	void EMOCAlgorithmRegister()
	{
		std::unordered_map<std::string, std::vector<char*>>& IMPLEMENTED_ALGORITHMS = EMOCManager::Instance()->GetImplementedAlgorithms();

		// Docomposition Based Algorithms
		EMOC_ALGORITHM_REGISTER(Docomposition Based, MOEADDYTS);
		EMOC_ALGORITHM_REGISTER(Docomposition Based, MOEADSWTS);
		EMOC_ALGORITHM_REGISTER(Docomposition Based, MOEADCDTS);
		EMOC_ALGORITHM_REGISTER(Docomposition Based, MOEADDTS);
		EMOC_ALGORITHM_REGISTER(Docomposition Based, MOEAD);
		EMOC_ALGORITHM_REGISTER(Docomposition Based, MOEADDE);
		EMOC_ALGORITHM_REGISTER(Docomposition Based, MOEADDRA);
		EMOC_ALGORITHM_REGISTER(Docomposition Based, MOEADFRRMAB);
		EMOC_ALGORITHM_REGISTER(Docomposition Based, MOEADGRA);
		EMOC_ALGORITHM_REGISTER(Docomposition Based, MOEADIRA);
		EMOC_ALGORITHM_REGISTER(Docomposition Based, ENSMOEAD);
		EMOC_ALGORITHM_REGISTER(Docomposition Based, MOEADCDE);
		EMOC_ALGORITHM_REGISTER(Docomposition Based, MOEADSTM);
		EMOC_ALGORITHM_REGISTER(Docomposition Based, RVEA);
		EMOC_ALGORITHM_REGISTER(Docomposition Based, MOEADPAS);
		EMOC_ALGORITHM_REGISTER(Docomposition Based, MOEADM2M);

		// Dominance Based Algorithms
		EMOC_ALGORITHM_REGISTER(Dominance Based, NSGA2);
		EMOC_ALGORITHM_REGISTER(Dominance Based, SPEA2);

		// Indicator Based Algorithms
		EMOC_ALGORITHM_REGISTER(Indicator Based, IBEA);
		EMOC_ALGORITHM_REGISTER(Indicator Based, HYPE);
		EMOC_ALGORITHM_REGISTER(Indicator Based, SMSEMOA);
	}

}