#pragma once

namespace emoc {

	double** UniformPoint(int num, int* weight_num, int obj_num);
	double** LoadUniformWeights(int num,int obj_num,char* file);
	double** HitAndRun(int num, int obj_num, char *run, char *outputFile);
}


