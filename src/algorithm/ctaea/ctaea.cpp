#include "algorithm/ctaea/ctaea.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

#include "core/macro.h"
#include "core/global.h"
#include "core/uniform_point.h"
#include "core/utility.h"
#include "operator/tournament_selection.h"
#include "operator/polynomial_mutation.h"
#include "operator/sbx.h"
#include "random/random.h"

namespace emoc {

	void CTAEA::insert(list* node, int x)
	{
		list* temp;

		if (node == NULL)
		{
			printf("\n EE: asked to enter after a NULL pointer, hence exiting \n");
			exit(1);
		}

		temp = (list*)malloc(sizeof(list));
		temp->index = x;
		temp->child = node->child;
		temp->parent = node;
		if (node->child != NULL)
		{
			node->child->parent = temp;
		}
		node->child = temp;
	}

	emoc::CTAEA::list* CTAEA::del(list* node)
	{
		list* temp;

		if (node == NULL)
		{
			printf("\n Error!!! asked to delete a NULL pointer, hence exiting \n");
			exit(1);
		}

		temp = node->parent;
		temp->child = node->child;
		if (temp->child != NULL)
		{
			temp->child->parent = temp;
		}
		free(node);
	}

	CTAEA::CTAEA(int thread_id) :Algorithm(thread_id)
	{

	}

	CTAEA::~CTAEA()
	{
		for (int i = 0; i < weight_num; ++i)
		{
			delete[] lambda[i];
			delete DA[i];
			lambda[i] = nullptr;
			DA[i] = nullptr;
		}
		delete[] DA;
		delete[] lambda;
		delete[] ideal_point;
		delete[] c_CA_density;
		delete[] c_CA_location;
		delete[] c_DA_density;
		delete[] c_DA_location;
		DA = nullptr;
		lambda = nullptr;
		ideal_point = nullptr;
		c_CA_density = nullptr;
		c_CA_location = nullptr;
		c_DA_density = nullptr;
		c_DA_location = nullptr;
	}

	void CTAEA::Solve()
	{
		Initialization();
		while (!IsTermination())
		{
			CalculateNdProportion(CA, DA);
			Crossover(CA, DA, g_GlobalSettings->offspring_population_.data());
			PolynomialMutation(g_GlobalSettings->offspring_population_.data(), real_popnum_, g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, mutation_para_);
			EvaluatePop(g_GlobalSettings->offspring_population_.data(), real_popnum_);

			// update the ideal point
			for (int i = 0; i < real_popnum_; i++)
				UpdateIdealpoint(g_GlobalSettings->offspring_population_[i], ideal_point, g_GlobalSettings->obj_num_);

			// update the CA
			MergePopulation(CA, real_popnum_, g_GlobalSettings->offspring_population_.data(), real_popnum_, 
				g_GlobalSettings->mixed_population_.data());
			CASelection(g_GlobalSettings->mixed_population_.data(), CA);

			// update the DA
			MergePopulation(DA, real_popnum_, g_GlobalSettings->offspring_population_.data(), real_popnum_, 
				g_GlobalSettings->mixed_population_.data());
			DASelection(g_GlobalSettings->mixed_population_.data(), CA, DA);
		}
		MergePopulation(CA, real_popnum_, DA, real_popnum_, g_GlobalSettings->mixed_population_.data());
		CASelection(g_GlobalSettings->mixed_population_.data(), CA);
	}

	void CTAEA::Initialization()
	{
		// allocate memory
		ideal_point = new double[g_GlobalSettings->obj_num_];
		lambda = UniformPoint(g_GlobalSettings->population_num_, &weight_num, g_GlobalSettings->obj_num_);
		real_popnum_ = weight_num;

		c_CA_density  = new int[weight_num];
		c_CA_location = new int[2 * weight_num];
		c_DA_density  = new int[weight_num];
		c_DA_location = new int[2 * weight_num];

		DA = new Individual * [weight_num];
		for (int i = 0; i < weight_num; i++)
			DA[i] = new Individual(g_GlobalSettings->dec_num_, g_GlobalSettings->obj_num_);

		// initialize parent population
		g_GlobalSettings->InitializePopulation(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		EvaluatePop(g_GlobalSettings->parent_population_.data(), g_GlobalSettings->population_num_);
		UpdateIdealpoint(g_GlobalSettings->parent_population_.data(), real_popnum_, ideal_point, g_GlobalSettings->obj_num_);
		CA = g_GlobalSettings->parent_population_.data();

		// initialize DA
		for (int i = 0; i < weight_num; i++)
			CopyIndividual(CA[i], DA[i]);

		// set mutation parameter
		mutation_para_.pro = 1.0 / g_GlobalSettings->dec_num_;
		mutation_para_.index1 = 20.0;

		// set crossover parameter
		cross_para_.pro = 1.0;
		cross_para_.index1 = 20.0;
	}

	void CTAEA::CalculateNdProportion(Individual** CA, Individual** DA)
	{
		int i, j;
		int flag_CA, flag_DA;
		int nd_CA, nd_DA;

		std::vector<Individual*> mixed_pop;
		for (int i = 0; i < 2 * real_popnum_; i++)
			mixed_pop.push_back(new Individual(g_GlobalSettings->dec_num_, g_GlobalSettings->obj_num_));


		MergePopulation(CA, real_popnum_, DA, real_popnum_, mixed_pop.data()); // combine CA and DA to form a hybrid population
		nd_CA = nd_DA = 0;

		// count the number of non-dominated points in the CA
		for (i = 0; i < weight_num; i++)
		{
			for (j = 0; j < 2 * weight_num; j++)
			{
				flag_CA = CheckDominance(CA[i], mixed_pop[j], g_GlobalSettings->obj_num_);
				if (flag_CA == -1)
					break;
			}
			if (flag_CA != -1)
				nd_CA++;
		}

		// count the number of non-dominated points in the DA
		for (i = 0; i < weight_num; i++)
		{
			for (j = 0; j < 2 * weight_num; j++)
			{
				flag_DA = CheckDominance(DA[i], mixed_pop[j], g_GlobalSettings->obj_num_);
				if (flag_DA == -1)
					break;
			}
			if (flag_DA != -1)
				nd_DA++;
		}

		rho_CA = (double)nd_CA / (nd_CA + nd_DA);
		rho_DA = (double)nd_DA / (nd_CA + nd_DA);

		for (int i = 0; i < 2 * real_popnum_; ++i)
			delete mixed_pop[i];
	}

	void CTAEA::Crossover(Individual** CA, Individual** DA, Individual** offspring_pop)
	{
		int i, temp, rand;
		int* a1, * a2;
		Individual* parent1, * parent2;

		a1 = (int*)malloc(weight_num * sizeof(int));
		a2 = (int*)malloc(weight_num * sizeof(int));
		for (i = 0; i < weight_num; i++)
			a1[i] = a2[i] = i;

		for (i = 0; i < weight_num; i++)
		{
			rand = rnd(i, weight_num - 1);
			temp = a1[rand];
			a1[rand] = a1[i];
			a1[i] = temp;
			rand = rnd(i, weight_num - 1);
			temp = a2[rand];
			a2[rand] = a2[i];
			a2[i] = temp;
		}

		for (i = 0; i < weight_num; i += 2)
		{
			if (i == weight_num - 1)
				i--;
			if (rho_CA > rho_DA)
				parent1 = CTAEATournament(CA[a1[i]], CA[a1[i + 1]]);
			else
				parent1 = CTAEATournament(DA[a1[i]], DA[a1[i + 1]]);
			if (rndreal(0, 1) < rho_CA)
				parent2 = CTAEATournament(CA[a2[i]], CA[a2[i + 1]]);
			else
				parent2 = CTAEATournament(DA[a2[i]], DA[a2[i + 1]]);

			SBX(parent1, parent2, offspring_pop[i], offspring_pop[i + 1],
				g_GlobalSettings->dec_lower_bound_, g_GlobalSettings->dec_upper_bound_, cross_para_);
		}

		free(a1);
		free(a2);
	}

	emoc::Individual* CTAEA::CTAEATournament(Individual* ind1, Individual* ind2)
	{
		int flag;

		double con1 = 0.0, con2 = 0.0;
		for (int i = 0; i < ind1->con_.size(); i++)
		{
			con1 += std::max(0.0, ind1->con_[i]);
			con2 += std::max(0.0, ind2->con_[i]);
		}

		if (con1 < EMOC_EPS && con2 < EMOC_EPS)
		{
			flag = CheckDominance(ind1, ind2, g_GlobalSettings->obj_num_);
			if (flag == 1)
				return (ind1);
			else if (flag == -1)
				return (ind2);
			else
			{
				if ((randomperc()) <= 0.5)
					return (ind1);
				else
					return (ind2);
			}
		}
		else if (con1 < EMOC_EPS && con2 > 0)
			return (ind1);
		else if (con1 > 0 && con2 < EMOC_EPS)
			return (ind2);
		else
		{
			if ((randomperc()) <= 0.5)
				return (ind1);
			else
				return (ind2);
		}
	}

	void CTAEA::TAEAAssignRank(Individual** pop, int size, list* selected_pool)
	{
		int i, j;
		int flag, end;
		int rank = 0;
		int cur_size = 0;
		int request = real_popnum_;
		c_pool_count = 0;

		list* pool;
		list* elite;
		list* temp1, * temp2;

		pool = (list*)malloc(sizeof(list));
		elite = (list*)malloc(sizeof(list));
		pool->index = -1;
		pool->parent = NULL;
		pool->child = NULL;
		temp1 = selected_pool->child;
		temp2 = pool;
		while (temp1 != NULL)
		{
			insert(temp2, temp1->index);
			temp1 = temp1->child;
			temp2 = temp2->child;
		}
		temp1 = selected_pool->child;    // empty the 'selected' list
		while (temp1 != NULL)
		{
			temp2 = temp1->child;
			del(temp1);
			temp1 = temp2;
		}
		elite->index = -1;
		elite->parent = NULL;
		elite->child = NULL;

		i = 0;
		do
		{
			temp1 = pool->child;
			insert(elite, temp1->index);
			temp1 = del(temp1);
			temp1 = temp1->child;
			do
			{
				temp2 = elite->child;
				if (temp1 == NULL)
					break;

				do
				{
					end = 0;
					flag = CheckDominance(pop[temp1->index], pop[temp2->index],g_GlobalSettings->obj_num_);
					if (flag == 1)
					{
						insert(pool, temp2->index);
						temp2 = del(temp2);
						temp2 = temp2->child;
					}
					if (flag == 0)
					{
						temp2 = temp2->child;
					}
					if (flag == -1)
					{
						end = 1;
					}
				} while (end != 1 && temp2 != NULL);
				if (flag == 0 || flag == 1)
				{
					insert(elite, temp1->index);
					temp1 = del(temp1);
				}
				temp1 = temp1->child;
			} while (temp1 != NULL);

			// copy each level into candidate
			if (cur_size < request)
			{
				temp2 = elite->child;
				do
				{
					insert(selected_pool, temp2->index);
					c_pool_count++;
					pop[temp2->index]->rank_ = rank;
					cur_size++;
					temp2 = temp2->child;
				} while (temp2 != NULL);
				rank++;
			}
			else if (cur_size < size)
			{
				temp2 = elite->child;
				do
				{
					pop[temp2->index]->rank_ = rank;
					cur_size++;
					temp2 = temp2->child;
				} while (temp2 != NULL);
				rank++;
			}

			temp2 = elite->child;
			do {
				temp2 = del(temp2);
				temp2 = temp2->child;
			} while (elite->child != NULL);
		} while (cur_size < size);

		// garbage collection
		while (pool != NULL)
		{
			temp1 = pool;
			pool = pool->child;
			free(temp1);
		}
		while (elite != NULL)
		{
			temp1 = elite;
			elite = elite->child;
			free(temp1);
		}
	}

	void CTAEA::TAEAAssociation(Individual** pop, list* pool, int* density, int* location)
	{
		int i, j;
		int min_idx;
		double distance, min_distance;
		double diff, maxFun, feval;

		list* temp;

		c_max_density = 0;
		for (i = 0; i < weight_num; i++)
			density[i] = 0;

		temp = pool->child;
		do {
			i = temp->index;
			min_distance = CalPerpendicularDistance(pop[i]->obj_.data(), lambda[0], g_GlobalSettings->obj_num_);

			min_idx = 0;
			for (j = 1; j < weight_num; j++)
			{
				distance = CalPerpendicularDistance(pop[i]->obj_.data(), lambda[j], g_GlobalSettings->obj_num_);
				if (distance < min_distance)
				{
					min_distance = distance;
					min_idx = j;
				}
			}
			density[min_idx]++;
			location[i] = min_idx;
			if (density[min_idx] > c_max_density)
				c_max_density = density[min_idx];

			maxFun = -1.0;
			for (j = 0; j < g_GlobalSettings->obj_num_; j++)
			{
				diff = fabs(pop[i]->obj_[j] - ideal_point[j]);
				if (lambda[min_idx][j] == 0)
					feval = diff / 0.0001;
				else
					feval = diff / lambda[min_idx][j];
				if (feval > maxFun)
					maxFun = feval;
			}
			pop[i]->fitness_ = maxFun;

			temp = temp->child;
		} while (temp != NULL);
	}

	int CTAEA::CADominance(Individual* a, Individual* b)
	{
		int flag1, flag2;
		flag1 = flag2 = 0;

		double con1 = 0.0, con2 = 0.0;
		for (int i = 0; i < a->con_.size(); i++)
		{
			con1 += std::max(0.0, a->con_[i]);
			con2 += std::max(0.0, b->con_[i]);
		}

		if (con1 < con2)
			flag1 = 1;
		else
		{
			if (con1 > con2)
				flag2 = 1;

		}
		if (a->fitness_ < b->fitness_)
			flag1 = 1;
		else
		{
			if (a->fitness_ > b->fitness_)
				flag2 = 1;
		}

		if (flag1 == 1 && flag2 == 0)
			return (1);
		else if (flag1 == 0 && flag2 == 1)
			return (-1);
		else
			return (0);
	}

	void CTAEA::FillCANd(list* pool, Individual** mixed_pop, Individual** pop, int idx)
	{
		int i;
		int flag;
		int end;
		int front_size, archive_size;

		list* elite;
		list* temp1, * temp2;

		elite = (list*)malloc(sizeof(list));
		elite->index = -1;
		elite->parent = NULL;
		elite->child = NULL;

		archive_size = idx;
		do {
			temp1 = pool->child;
			insert(elite, temp1->index);
			front_size = 1;
			temp2 = elite->child;
			temp1 = del(temp1);
			temp1 = temp1->child;
			do {
				temp2 = elite->child;
				if (temp1 == NULL)
					break;
				do {
					end = 0;
					flag = CADominance(mixed_pop[temp1->index], mixed_pop[temp2->index]);
					if (flag == 1)
					{
						insert(pool, temp2->index);
						temp2 = del(temp2);
						front_size--;
						temp2 = temp2->child;
					}
					if (flag == 0)
						temp2 = temp2->child;
					if (flag == -1)
						end = 1;
				} while (end != 1 && temp2 != NULL);
				if (flag == 0 || flag == 1)
				{
					insert(elite, temp1->index);
					front_size++;
					temp1 = del(temp1);
				}
				temp1 = temp1->child;
			} while (temp1 != NULL);
			if ((archive_size + front_size) <= real_popnum_)
			{
				temp2 = elite->child;
				do {
					CopyIndividual(mixed_pop[temp2->index], pop[idx]);
					archive_size++;
					temp2 = temp2->child;
					idx++;
					if (idx == real_popnum_)
						break;
				} while (temp2 != NULL);
			}
			else
			{
				i = 0;
				std::vector<DistanceInfo> array(front_size);
				temp2 = elite->child;
				do {
					array[i].idx = temp2->index;

					double con = 0.0;
					for (int p = 0; p < mixed_pop[temp2->index]->con_.size(); p++)
						con += std::max(0.0, mixed_pop[temp2->index]->con_[p]);

					array[i].x = con;
					temp2 = temp2->child;
					i++;
				} while (temp2 != NULL);
				std::sort(array.begin(), array.end(), [](DistanceInfo& left, DistanceInfo& right) {
					return left.x > right.x;
					});

				i = 0;
				while (idx < real_popnum_)
				{
					CopyIndividual(mixed_pop[array[i].idx], pop[idx]);
					archive_size++;
					idx++;
					i++;
				}
			}

			temp2 = elite->child;
			do {
				temp2 = del(temp2);
				temp2 = temp2->child;
			} while (elite->child != NULL);
		} while (archive_size < real_popnum_);

		// garbage collection
		while (elite != NULL)
		{
			temp1 = elite;
			elite = elite->child;
			free(temp1);
		}
	}

	void CTAEA::CASelection(Individual** mixed_pop, Individual** CA)
	{
		int i, j, k;
		int cur_idx, delete_idx, temp_idx;
		int num_feasible;

		double temp_distance, close_distance, worst_fitness;

		list* loop1, * loop2;
		list* temp, * temp_feasible, * temp_infeasible, * delete_ind;
		list* feasible_pool, * infeasible_pool;

		feasible_pool = (list*)malloc(sizeof(list));
		feasible_pool->index = -1;
		feasible_pool->parent = NULL;
		feasible_pool->child = NULL;

		infeasible_pool = (list*)malloc(sizeof(list));
		infeasible_pool->index = -1;
		infeasible_pool->parent = NULL;
		infeasible_pool->child = NULL;

		// find the feasible solutions in the 'mixed_pop'
		temp_feasible = feasible_pool;
		temp_infeasible = infeasible_pool;
		num_feasible = 0;
		for (i = 0; i < 2 * real_popnum_; i++)
		{
			double con = 0.0;
			for (int j = 0; j < mixed_pop[i]->con_.size(); j++)
				con += std::max(0.0, mixed_pop[i]->con_[j]);
			
			if (con < EMOC_EPS)
			{
				num_feasible++;
				insert(temp_feasible, i);
				temp_feasible = temp_feasible->child;
			}
			else
			{
				insert(temp_infeasible, i);
				temp_infeasible = temp_infeasible->child;
			}
		}

		if (num_feasible > real_popnum_)
		{
			TAEAAssignRank(mixed_pop, num_feasible, feasible_pool);    // 'feasible_pool' corresponds {F1+F2+...Fl}, where Fl is the last acceptable front
			TAEAAssociation(mixed_pop, feasible_pool, c_CA_density, c_CA_location);    // associate solutions in 'feasible_pool' to the corresponding sub-regions
			while (c_pool_count > real_popnum_)
			{
				worst_fitness = -1.0;
				for (i = 0; i < weight_num; i++)
				{
					if (c_CA_density[i] == c_max_density)
					{
						temp = feasible_pool->child;
						do {
							cur_idx = temp->index;
							if (c_CA_location[cur_idx] == i && mixed_pop[cur_idx]->fitness_ > worst_fitness)
							{
								delete_idx = cur_idx;
								delete_ind = temp;
								worst_fitness = mixed_pop[cur_idx]->fitness_;

								// find the nearest neighbhor distance to 'cur_idx'
								loop1 = feasible_pool->child;
								close_distance = INFINITY;
								do {
									temp_idx = loop1->index;
									if (temp_idx != cur_idx)
									{
										temp_distance = CalEuclidianDistance(mixed_pop[cur_idx]->obj_.data(), mixed_pop[temp_idx]->obj_.data(), g_GlobalSettings->obj_num_);
										if (temp_distance < close_distance)
											close_distance = temp_distance;
									}
									loop1 = loop1->child;
								} while (loop1 != NULL);

								// find another pair of solutions which have smaller distance than 'close_distance'
								loop1 = feasible_pool->child;
								do {
									j = loop1->index;
									loop2 = feasible_pool->child;
									do {
										k = loop2->index;
										if (j != k)
										{
											temp_distance = CalEuclidianDistance(mixed_pop[j]->obj_.data(), mixed_pop[k]->obj_.data(), g_GlobalSettings->obj_num_);
											if (temp_distance < close_distance && c_CA_location[j] == i && c_CA_location[k] == i)
											{
												close_distance = temp_distance;
												if (mixed_pop[j]->fitness_ > mixed_pop[k]->fitness_)
												{
													delete_idx = j;
													delete_ind = loop1;
												}
												else if (mixed_pop[j]->fitness_ < mixed_pop[k]->fitness_)
												{
													delete_idx = k;
													delete_ind = loop2;
												}
												else
												{
													if ((randomperc()) <= 0.5)
													{
														delete_idx = j;
														delete_ind = loop1;
													}
													else
													{
														delete_idx = k;
														delete_ind = loop2;
													}
												}
											}
											else if (temp_distance == close_distance && c_CA_location[j] == i && c_CA_location[k] == i)
											{
												if (mixed_pop[j]->fitness_ > mixed_pop[k]->fitness_)
												{
													if (mixed_pop[j]->fitness_ > worst_fitness)
													{
														delete_idx = j;
														delete_ind = loop1;
													}
												}
												else if (mixed_pop[j]->fitness_ < mixed_pop[k]->fitness_)
												{
													if (mixed_pop[k]->fitness_ > worst_fitness)
													{
														delete_idx = k;
														delete_ind = loop2;
													}
												}
												else
												{
													if (mixed_pop[j]->fitness_ > worst_fitness)
													{
														if ((randomperc()) <= 0.5)
														{
															delete_idx = j;
															delete_ind = loop1;
														}
														else
														{
															delete_idx = k;
															delete_ind = loop2;
														}
													}
												}
											}
										}
										loop2 = loop2->child;
									} while (loop2 != NULL);
									loop1 = loop1->child;
								} while (loop1 != NULL);
								worst_fitness = mixed_pop[delete_idx]->fitness_;
							}
							temp = temp->child;
						} while (temp != NULL);
					}
				}
				c_pool_count--;
				del(delete_ind);
				c_CA_density[c_CA_location[delete_idx]]--;
				c_CA_location[delete_idx] = -1;

				// update 'c_max_density'
				c_max_density = c_CA_density[0];
				for (j = 1; j < weight_num; j++)
				{
					if (c_max_density < c_CA_density[j])
						c_max_density = c_CA_density[j];
				}
			}

			// copy the remaining solutions in 'feasible_pool' to the new CA
			j = 0;
			temp = feasible_pool->child;
			do {
				i = temp->index;
				CopyIndividual(mixed_pop[i], CA[j]);
				j++;
				temp = temp->child;
			} while (temp != NULL);
		}
		else    // num_feasible <= popsize
		{
			// copy all feasible solutions to the new CA
			i = 0;
			temp = feasible_pool->child;
			while (temp != NULL)
			{
				CopyIndividual(mixed_pop[temp->index], CA[i]);
				i++;
				temp = temp->child;
			}

			if (i < real_popnum_)
			{
				// fill the gap with promising infeasible solutions
				TAEAAssociation(mixed_pop, infeasible_pool, c_CA_density, c_CA_location);
				FillCANd(infeasible_pool, mixed_pop, CA, i);
			}
		}

		/* Garbage collection */
		while (feasible_pool != NULL)
		{
			temp = feasible_pool;
			feasible_pool = feasible_pool->child;
			free(temp);
		}
		while (infeasible_pool != NULL)
		{
			temp = infeasible_pool;
			infeasible_pool = infeasible_pool->child;
			free(temp);
		}

		return;
	}

	int CTAEA::FindBest(Individual** pop, list* pool)
	{
		int min_idx;
		int flag, end;

		double min_fitness;

		list* elite;
		list* temp1, * temp2;

		elite = (list*)malloc(sizeof(list));

		elite->index = -1;
		elite->parent = NULL;
		elite->child = NULL;

		// find the current non-dominated solutions
		temp1 = pool->child;
		insert(elite, temp1->index);
		temp1 = del(temp1);
		temp1 = temp1->child;
		do {
			temp2 = elite->child;
			if (temp1 == NULL)
				break;

			do {
				end = 0;
				flag = CheckDominance(pop[temp1->index], pop[temp2->index], g_GlobalSettings->obj_num_);
				if (flag == 1)
				{
					insert(pool, temp2->index);
					temp2 = del(temp2);
					temp2 = temp2->child;
				}
				if (flag == 0)
				{
					temp2 = temp2->child;
				}
				if (flag == -1)
				{
					end = 1;
				}
			} while (end != 1 && temp2 != NULL);
			if (flag == 0 || flag == 1)
			{
				insert(elite, temp1->index);
				temp1 = del(temp1);
			}
			temp1 = temp1->child;
		} while (temp1 != NULL);

		// find the best solution from the non-dominated set
		temp2 = elite->child;
		min_fitness = pop[temp2->index]->fitness_;
		min_idx = temp2->index;
		while (temp2->child != NULL)
		{
			temp2 = temp2->child;
			if (pop[temp2->index]->fitness_ < min_fitness)
			{
				min_fitness = pop[temp2->index]->fitness_;
				min_idx = temp2->index;
			}
		}

		// garbage collection
		while (elite != NULL)
		{
			temp1 = elite;
			elite = elite->child;
			free(temp1);
		}

		return min_idx;
	}

	void CTAEA::DASelection(Individual** mixed_pop, Individual** CA, Individual** DA)
	{
		int i, j, k;
		int itr;
		int best_idx;
		int updated_size;

		list* pool, * temp_archive, * temp;

		pool = (list*)malloc(sizeof(list));
		pool->index = -1;
		pool->parent = NULL;
		pool->child = NULL;

		temp_archive = (list*)malloc(sizeof(list));
		temp_archive->index = -1;
		temp_archive->parent = NULL;
		temp_archive->child = NULL;

		// association procedure for the CA
		temp = pool;
		for (i = 0; i < real_popnum_; i++)
		{
			insert(temp, i);
			temp = temp->child;
		}
		TAEAAssociation(CA, pool, c_CA_density, c_CA_location);

		// clear the 'pool' list
		while (pool->child != NULL)
		{
			temp = pool;
			pool = pool->child;
			free(temp);
		}

		// association procedure for the mixed population, i.e. DA + offspring
		temp = pool;
		for (i = 0; i < 2 * real_popnum_; i++)
		{
			insert(temp, i);
			temp = temp->child;
		}
		TAEAAssociation(mixed_pop, pool, c_DA_density, c_DA_location);

		itr = 0;
		updated_size = 0;
		while (updated_size < real_popnum_)
		{
			itr++;
			for (i = 0; i < weight_num; i++)
			{
				if (c_CA_density[i] < itr)
				{
					for (j = 0; j < (itr - c_CA_density[i]); j++)
					{
						if (c_DA_density[i] == 0)
							break;
						else    // c_DA_density[i] >= 1
						{
							// find solutions of 'mixed_pop' (DA + offspring) located in the i-th sub-region
							temp = pool->child;
							do {
								k = temp->index;
								if (c_DA_location[k] == i)
									insert(temp_archive, k);
								temp = temp->child;
							} while (temp != NULL);

							// find the best solution in the i-th sub-region
							best_idx = FindBest(mixed_pop, temp_archive);

							// clear the 'temp_archive' list
							while (temp_archive->child != NULL)
							{
								temp_archive = temp_archive->child;
								free(temp_archive->parent);
							}
							temp_archive->index = -1;
							temp_archive->parent = NULL;
							temp_archive->child = NULL;

							// remove the selected 'best_idx' solution
							temp = pool->child;
							do {
								k = temp->index;
								if (best_idx == k)
								{
									del(temp);
									break;
								}
								temp = temp->child;
							} while (temp != NULL);
							c_DA_density[c_DA_location[best_idx]]--;
							c_DA_location[best_idx] = -1;

							CopyIndividual(mixed_pop[best_idx], DA[updated_size]);
							updated_size++;

							if (updated_size == real_popnum_)
							{
								while (pool != NULL)
								{
									temp = pool;
									pool = pool->child;
									free(temp);
								}
								while (temp_archive != NULL)
								{
									temp = temp_archive;
									temp_archive = temp_archive->child;
									free(temp);
								}
								return;
							}
						}
					}
				}
			}
		}
	}

}