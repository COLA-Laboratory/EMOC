#include "algorithm/algorithm_head_collect.h"

#include "core/macro.h"
#include "algorithm/algorithm_factory.h"

namespace emoc {

	// Docomposition Based Algorithms
	EMOC_REGIST_ALGORITHM(Multiobjective, Docomposition Based, MOEADDYTS);
	EMOC_REGIST_ALGORITHM(Multiobjective, Docomposition Based, MOEAD);
	EMOC_REGIST_ALGORITHM(Multiobjective, Docomposition Based, MOEADDE);
	EMOC_REGIST_ALGORITHM(Multiobjective, Docomposition Based, MOEADDRA);
	EMOC_REGIST_ALGORITHM(Multiobjective, Docomposition Based, MOEADUCB);
	EMOC_REGIST_ALGORITHM(Multiobjective, Docomposition Based, MOEADFRRMAB);
	EMOC_REGIST_ALGORITHM(Multiobjective, Docomposition Based, MOEADGRA);
	EMOC_REGIST_ALGORITHM(Multiobjective, Docomposition Based, MOEADIRA);
	EMOC_REGIST_ALGORITHM(Multiobjective, Docomposition Based, ENSMOEAD);
	EMOC_REGIST_ALGORITHM(Multiobjective, Docomposition Based, MOEADCDE);
	EMOC_REGIST_ALGORITHM(Multiobjective, Docomposition Based, MOEADSTM);
	EMOC_REGIST_ALGORITHM(Multiobjective, Docomposition Based, RVEA);
	EMOC_REGIST_ALGORITHM(Multiobjective, Docomposition Based, MOEADPAS);
	EMOC_REGIST_ALGORITHM(Multiobjective, Docomposition Based, MOEADM2M);
	EMOC_REGIST_ALGORITHM(Multiobjective, Docomposition Based, MOEADD);
	EMOC_REGIST_ALGORITHM(Multiobjective, Docomposition Based, MOEADPLVF);

	// Dominance Based Algorithms
	EMOC_REGIST_ALGORITHM(Multiobjective,Dominance Based, NSGA2);
	EMOC_REGIST_ALGORITHM(Multiobjective,Dominance Based, NSGA3);
	EMOC_REGIST_ALGORITHM(Multiobjective,Dominance Based, SPEA2);
	EMOC_REGIST_ALGORITHM(Multiobjective,Dominance Based, SPEA2SDE);
	EMOC_REGIST_ALGORITHM(Multiobjective,Dominance Based, tDEA);

	// Indicator Based Algorithms
	EMOC_REGIST_ALGORITHM(Multiobjective,Indicator Based, IBEA);
	EMOC_REGIST_ALGORITHM(Multiobjective,Indicator Based, HypE);
	EMOC_REGIST_ALGORITHM(Multiobjective,Indicator Based, SMSEMOA);

	// Constraint Algorithms
	EMOC_REGIST_ALGORITHM(Multiobjective, Constraint, CNSGA2);
	EMOC_REGIST_ALGORITHM(Multiobjective, Constraint, CMOEAD);
	EMOC_REGIST_ALGORITHM(Multiobjective, Constraint, CTAEA);

	// Single Objective Algorithms
	EMOC_REGIST_ALGORITHM(Singleobjective, Single Objective, GA);
	EMOC_REGIST_ALGORITHM(Singleobjective, Single Objective, DifferentialEvolution);
	EMOC_REGIST_ALGORITHM(Singleobjective, Single Objective, SA);

}