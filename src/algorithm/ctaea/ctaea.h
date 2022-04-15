#pragma once
#include "core/individual.h"
#include "algorithm/algorithm.h"
#include "problem/problem.h"

#include <vector>

namespace emoc {

	class CTAEA : public Algorithm
	{
	public:
		typedef struct
		{
			int idx;
			double x;
		}DistanceInfo;  

		typedef struct lists
		{
			int index;
			int index2;
			struct lists* parent;
			struct lists* child;
		} list;
		/* Insert an element X into the list at location specified by NODE */
		void insert(list* node, int x);

		/* Delete the node NODE from the list */
		list* del(list* node);

	public:
		CTAEA(int thread_id);
		virtual ~CTAEA();
		
		void Solve();
		 
	private:
		void Initialization();
		void CalculateNdProportion(Individual** CA, Individual** DA);
		void Crossover(Individual** CA, Individual** DA, Individual** offspring_pop);
		Individual* CTAEATournament(Individual* ind1, Individual* ind2);
		void TAEAAssignRank(Individual** pop, int size, list* selected_pool);
		void TAEAAssociation(Individual** pop, list* pool, int* density, int* location);
		int CADominance(Individual* a, Individual* b);
		void FillCANd(list* pool, Individual** mixed_pop, Individual** pop, int idx);
		void CASelection(Individual** mixed_pop, Individual** CA);
		int FindBest(Individual** pop, list* pool);
		void DASelection(Individual** mixed_pop, Individual** CA, Individual** DA);

	private:
		Individual** CA;
		Individual** DA;
		
		double **lambda;                  // weight vector
		int weight_num;                   // the number of weight vector
		int c_pool_count;
		int c_max_density;
		double rho_CA, rho_DA;
		double* ideal_point;
		int* c_CA_density, * c_CA_location, * c_DA_density, * c_DA_location;

	};

}