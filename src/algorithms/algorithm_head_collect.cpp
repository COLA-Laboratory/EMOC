#include "algorithms/algorithm_head_collect.h"

#include <string>
#include <vector>
#include <unordered_map>

#include "core/macro.h"
#include "core/emoc_manager.h"
#include "algorithms/algorithm_factory.h"

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

	// Docomposition Based Algorithms
	EMOC_REGIST_ALGORITHM(Docomposition Based, MOEADDYTS);
	EMOC_REGIST_ALGORITHM(Docomposition Based, MOEADSWTS);
	EMOC_REGIST_ALGORITHM(Docomposition Based, MOEADCDTS);
	EMOC_REGIST_ALGORITHM(Docomposition Based, MOEADDTS);
	EMOC_REGIST_ALGORITHM(Docomposition Based, MOEAD);
	EMOC_REGIST_ALGORITHM(Docomposition Based, MOEADDE);
	EMOC_REGIST_ALGORITHM(Docomposition Based, MOEADDRA);
	EMOC_REGIST_ALGORITHM(Docomposition Based, MOEADFRRMAB);
	EMOC_REGIST_ALGORITHM(Docomposition Based, MOEADGRA);
	EMOC_REGIST_ALGORITHM(Docomposition Based, MOEADIRA);
	EMOC_REGIST_ALGORITHM(Docomposition Based, ENSMOEAD);
	EMOC_REGIST_ALGORITHM(Docomposition Based, MOEADCDE);
	EMOC_REGIST_ALGORITHM(Docomposition Based, MOEADSTM);
	EMOC_REGIST_ALGORITHM(Docomposition Based, RVEA);
	EMOC_REGIST_ALGORITHM(Docomposition Based, MOEADPAS);
	EMOC_REGIST_ALGORITHM(Docomposition Based, MOEADM2M);
	EMOC_REGIST_ALGORITHM(Docomposition Based, MOEADD);

	// Dominance Based Algorithms
	EMOC_REGIST_ALGORITHM(Dominance Based, NSGA2);
	EMOC_REGIST_ALGORITHM(Dominance Based, NSGA3);
	EMOC_REGIST_ALGORITHM(Dominance Based, SPEA2);
	EMOC_REGIST_ALGORITHM(Dominance Based, SPEA2SDE);
	EMOC_REGIST_ALGORITHM(Dominance Based, tDEA);

	// Indicator Based Algorithms
	EMOC_REGIST_ALGORITHM(Indicator Based, IBEA);
	EMOC_REGIST_ALGORITHM(Indicator Based, HypE);
	EMOC_REGIST_ALGORITHM(Indicator Based, SMSEMOA);

}