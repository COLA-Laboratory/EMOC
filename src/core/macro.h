#pragma once
#define EMOC_INF 1.0e9
#define EMOC_EPS 1.0e-6
#define PI 3.14159265358979323846
#define MAX_THREAD_NUM 128
#define MAX_BUFFSIZE 256
#define GNUPLOT_TERMINAL "wxt"

#define EMOC_REGIST_ALGORITHM(OPTIMIZATION_TYPE,CATEGORY,NAME)\
Algorithm * CreateAlgorithm##NAME(int thread_id) {\
	return new NAME(thread_id);\
}\
AlgorithmRegisterAction NAME##Register(#OPTIMIZATION_TYPE, #CATEGORY, #NAME, CreateAlgorithm##NAME);

#define EMOC_REGIST_PROBLEM(OPTIMIZATION_TYPE,CATEGORY,NAME)\
Problem * CreateProblem##NAME(int dec_num, int obj_num) {\
	return new NAME(dec_num, obj_num);\
}\
ProblemRegisterAction NAME##Register(#OPTIMIZATION_TYPE, #CATEGORY, #NAME, CreateProblem##NAME);