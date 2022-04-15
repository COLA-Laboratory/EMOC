#include "problem/problem_head_collect.h"

#include "core/macro.h"
#include "problem/problem_factory.h"

namespace emoc{

	// ZDT Series
	EMOC_REGIST_PROBLEM(Multiobjective, ZDT Series, ZDT1);
	EMOC_REGIST_PROBLEM(Multiobjective, ZDT Series, ZDT2);
	EMOC_REGIST_PROBLEM(Multiobjective, ZDT Series, ZDT3);
	EMOC_REGIST_PROBLEM(Multiobjective, ZDT Series, ZDT4);
	EMOC_REGIST_PROBLEM(Multiobjective, ZDT Series, ZDT6);

	// DTLZ Series
	EMOC_REGIST_PROBLEM(Multiobjective, DTLZ Series, DTLZ1);
	EMOC_REGIST_PROBLEM(Multiobjective, DTLZ Series, DTLZ2);
	EMOC_REGIST_PROBLEM(Multiobjective, DTLZ Series, DTLZ3);
	EMOC_REGIST_PROBLEM(Multiobjective, DTLZ Series, DTLZ4);
	EMOC_REGIST_PROBLEM(Multiobjective, DTLZ Series, DTLZ5);
	EMOC_REGIST_PROBLEM(Multiobjective, DTLZ Series, DTLZ6);
	EMOC_REGIST_PROBLEM(Multiobjective, DTLZ Series, DTLZ7);
	EMOC_REGIST_PROBLEM(Multiobjective, DTLZ Series, C1DTLZ1);
	EMOC_REGIST_PROBLEM(Multiobjective, DTLZ Series, C1DTLZ3);
	EMOC_REGIST_PROBLEM(Multiobjective, DTLZ Series, C2DTLZ2);
	EMOC_REGIST_PROBLEM(Multiobjective, DTLZ Series, C3DTLZ4);
	EMOC_REGIST_PROBLEM(Multiobjective, DTLZ Series, MinusDTLZ1);
	EMOC_REGIST_PROBLEM(Multiobjective, DTLZ Series, MinusDTLZ2);
	EMOC_REGIST_PROBLEM(Multiobjective, DTLZ Series, MinusDTLZ3);
	EMOC_REGIST_PROBLEM(Multiobjective, DTLZ Series, MinusDTLZ4);
	EMOC_REGIST_PROBLEM(Multiobjective, DTLZ Series, MDTLZ1);
	EMOC_REGIST_PROBLEM(Multiobjective, DTLZ Series, MDTLZ2);
	EMOC_REGIST_PROBLEM(Multiobjective, DTLZ Series, MDTLZ3);
	EMOC_REGIST_PROBLEM(Multiobjective, DTLZ Series, MDTLZ4);
	EMOC_REGIST_PROBLEM(Multiobjective, DCDTLZ Series, DC1DTLZ1);
	EMOC_REGIST_PROBLEM(Multiobjective, DCDTLZ Series, DC1DTLZ3);
	EMOC_REGIST_PROBLEM(Multiobjective, DCDTLZ Series, DC2DTLZ1);
	EMOC_REGIST_PROBLEM(Multiobjective, DCDTLZ Series, DC2DTLZ3);
	EMOC_REGIST_PROBLEM(Multiobjective, DCDTLZ Series, DC3DTLZ1);
	EMOC_REGIST_PROBLEM(Multiobjective, DCDTLZ Series, DC3DTLZ3);

	// UF Series
	EMOC_REGIST_PROBLEM(Multiobjective, UF Series, UF1);
	EMOC_REGIST_PROBLEM(Multiobjective, UF Series, UF2);
	EMOC_REGIST_PROBLEM(Multiobjective, UF Series, UF3);
	EMOC_REGIST_PROBLEM(Multiobjective, UF Series, UF4);
	EMOC_REGIST_PROBLEM(Multiobjective, UF Series, UF5);
	EMOC_REGIST_PROBLEM(Multiobjective, UF Series, UF6);
	EMOC_REGIST_PROBLEM(Multiobjective, UF Series, UF7);
	EMOC_REGIST_PROBLEM(Multiobjective, UF Series, UF8);
	EMOC_REGIST_PROBLEM(Multiobjective, UF Series, UF9);
	EMOC_REGIST_PROBLEM(Multiobjective, UF Series, UF10);

	// WFG Series
	EMOC_REGIST_PROBLEM(Multiobjective, WFG Series, WFG1);
	EMOC_REGIST_PROBLEM(Multiobjective, WFG Series, WFG2);
	EMOC_REGIST_PROBLEM(Multiobjective, WFG Series, WFG3);
	EMOC_REGIST_PROBLEM(Multiobjective, WFG Series, WFG4);
	EMOC_REGIST_PROBLEM(Multiobjective, WFG Series, WFG5);
	EMOC_REGIST_PROBLEM(Multiobjective, WFG Series, WFG6);
	EMOC_REGIST_PROBLEM(Multiobjective, WFG Series, WFG7);
	EMOC_REGIST_PROBLEM(Multiobjective, WFG Series, WFG8);
	EMOC_REGIST_PROBLEM(Multiobjective, WFG Series, WFG9);

	// LSMOP Series
	EMOC_REGIST_PROBLEM(Multiobjective, LSMOP Series, LSMOP1);
	EMOC_REGIST_PROBLEM(Multiobjective, LSMOP Series, LSMOP2);
	EMOC_REGIST_PROBLEM(Multiobjective, LSMOP Series, LSMOP3);
	EMOC_REGIST_PROBLEM(Multiobjective, LSMOP Series, LSMOP4);
	EMOC_REGIST_PROBLEM(Multiobjective, LSMOP Series, LSMOP5);
	EMOC_REGIST_PROBLEM(Multiobjective, LSMOP Series, LSMOP6);
	EMOC_REGIST_PROBLEM(Multiobjective, LSMOP Series, LSMOP7);
	EMOC_REGIST_PROBLEM(Multiobjective, LSMOP Series, LSMOP8);
	EMOC_REGIST_PROBLEM(Multiobjective, LSMOP Series, LSMOP9);

	// BT Series
	EMOC_REGIST_PROBLEM(Multiobjective, BT Series, BT1);
	EMOC_REGIST_PROBLEM(Multiobjective, BT Series, BT2);
	EMOC_REGIST_PROBLEM(Multiobjective, BT Series, BT3);
	EMOC_REGIST_PROBLEM(Multiobjective, BT Series, BT4);
	EMOC_REGIST_PROBLEM(Multiobjective, BT Series, BT5);
	EMOC_REGIST_PROBLEM(Multiobjective, BT Series, BT6);
	EMOC_REGIST_PROBLEM(Multiobjective, BT Series, BT7);
	EMOC_REGIST_PROBLEM(Multiobjective, BT Series, BT8);
	EMOC_REGIST_PROBLEM(Multiobjective, BT Series, BT9);

	// MOEADDE_F Series
	EMOC_REGIST_PROBLEM(Multiobjective, MOEADDE_F Series, MOEADDE_F1);
	EMOC_REGIST_PROBLEM(Multiobjective, MOEADDE_F Series, MOEADDE_F2);
	EMOC_REGIST_PROBLEM(Multiobjective, MOEADDE_F Series, MOEADDE_F3);
	EMOC_REGIST_PROBLEM(Multiobjective, MOEADDE_F Series, MOEADDE_F4);
	EMOC_REGIST_PROBLEM(Multiobjective, MOEADDE_F Series, MOEADDE_F5);
	EMOC_REGIST_PROBLEM(Multiobjective, MOEADDE_F Series, MOEADDE_F6);
	EMOC_REGIST_PROBLEM(Multiobjective, MOEADDE_F Series, MOEADDE_F7);
	EMOC_REGIST_PROBLEM(Multiobjective, MOEADDE_F Series, MOEADDE_F8);
	EMOC_REGIST_PROBLEM(Multiobjective, MOEADDE_F Series, MOEADDE_F9);

	// IMMOEA_F Series
	EMOC_REGIST_PROBLEM(Multiobjective, IMMOEA_F Series, IMMOEA_F1);
	EMOC_REGIST_PROBLEM(Multiobjective, IMMOEA_F Series, IMMOEA_F2);
	EMOC_REGIST_PROBLEM(Multiobjective, IMMOEA_F Series, IMMOEA_F3);
	EMOC_REGIST_PROBLEM(Multiobjective, IMMOEA_F Series, IMMOEA_F4);
	EMOC_REGIST_PROBLEM(Multiobjective, IMMOEA_F Series, IMMOEA_F5);
	EMOC_REGIST_PROBLEM(Multiobjective, IMMOEA_F Series, IMMOEA_F6);
	EMOC_REGIST_PROBLEM(Multiobjective, IMMOEA_F Series, IMMOEA_F7);
	EMOC_REGIST_PROBLEM(Multiobjective, IMMOEA_F Series, IMMOEA_F8);
	EMOC_REGIST_PROBLEM(Multiobjective, IMMOEA_F Series, IMMOEA_F9);
	EMOC_REGIST_PROBLEM(Multiobjective, IMMOEA_F Series, IMMOEA_F10);

	// MOEADM2M Series
	EMOC_REGIST_PROBLEM(Multiobjective, MOEADDE_F Series, MOEADM2M_F1);
	EMOC_REGIST_PROBLEM(Multiobjective, MOEADDE_F Series, MOEADM2M_F2);
	EMOC_REGIST_PROBLEM(Multiobjective, MOEADDE_F Series, MOEADM2M_F3);
	EMOC_REGIST_PROBLEM(Multiobjective, MOEADDE_F Series, MOEADM2M_F4);
	EMOC_REGIST_PROBLEM(Multiobjective, MOEADDE_F Series, MOEADM2M_F5);
	EMOC_REGIST_PROBLEM(Multiobjective, MOEADDE_F Series, MOEADM2M_F6);
	EMOC_REGIST_PROBLEM(Multiobjective, MOEADDE_F Series, MOEADM2M_F7);

	// Single Objective Problems
	EMOC_REGIST_PROBLEM(Singleobjective, Single Objective, Sphere);
	EMOC_REGIST_PROBLEM(Singleobjective, Single Objective, Ackley);
	EMOC_REGIST_PROBLEM(Singleobjective, Single Objective, Rastrigin);
	EMOC_REGIST_PROBLEM(Singleobjective, Single Objective, TSP);
	EMOC_REGIST_PROBLEM(Singleobjective, Single Objective, Knapsack);

}