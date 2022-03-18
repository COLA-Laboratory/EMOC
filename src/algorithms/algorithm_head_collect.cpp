#include "algorithms/algorithm_head_collect.h"

#include <string>
#include <vector>
#include <unordered_map>

#include "core/macro.h"
#include "core/emoc_manager.h"
#include "algorithms/algorithm_factory.h"

namespace emoc {

	// Docomposition Based Algorithms
	EMOC_REGIST_ALGORITHM(Docomposition Based, MOEADDYTS);
	EMOC_REGIST_ALGORITHM(Docomposition Based, MOEADDYTSTEST);
	EMOC_REGIST_ALGORITHM(Docomposition Based, MOEADCDTS);
	EMOC_REGIST_ALGORITHM(Docomposition Based, MOEAD);
	EMOC_REGIST_ALGORITHM(Docomposition Based, MOEADDE);
	EMOC_REGIST_ALGORITHM(Docomposition Based, MOEADDRA);
	EMOC_REGIST_ALGORITHM(Docomposition Based, MOEADUCB);
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