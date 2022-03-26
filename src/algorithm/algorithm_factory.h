#pragma once
#include <mutex>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>

#include "algorithm/algorithm.h"

namespace emoc {

	typedef Algorithm* (*AlgorithmCreator)(int thread_id);

	class AlgorithmFactory
	{
	public:
		static AlgorithmFactory* Instance(); 

		Algorithm* CreateAlgorithmObject(const std::string &name, int thread_id);

		inline std::unordered_map<std::string, std::vector<char*>>& GetImplementedAlgorithmsName() { return IMPLEMENTED_ALGORITHMS_NAME; }
		inline std::unordered_map<std::string, AlgorithmCreator>& GetImplementedAlgorithmsMethod() { return IMPLEMENTED_ALGORITHMS_METHOD; }
		inline std::unordered_set<std::string>& GetSingleObjectiveAlgorithms() { return SINGLE_OBJECTIVE_ALGORITHMS; }
		inline std::unordered_set<std::string>& GetMultiObjectiveAlgorithms() { return MULTI_OBJECTIVE_ALGORITHMS; }

	private:
		AlgorithmFactory();
		~AlgorithmFactory();
		AlgorithmFactory(const AlgorithmFactory&);
		AlgorithmFactory& operator=(const AlgorithmFactory&);

	private:
		// for release AlgorithmFactory instance
		class Garbo
		{
		public:
			Garbo() {};
			~Garbo()
			{
				if (s_Instance != nullptr)
				{
					delete s_Instance;
					s_Instance = nullptr;
				}
			}
		};
		static Garbo garbo_;
		static AlgorithmFactory* s_Instance;
		static std::mutex singleton_mutex_;


		// implemented algorithms - we use c-style strings for the compatibility with Dear ImGUI
		std::unordered_map<std::string, std::vector<char*>> IMPLEMENTED_ALGORITHMS_NAME;
		std::unordered_map<std::string, AlgorithmCreator> IMPLEMENTED_ALGORITHMS_METHOD;
		std::unordered_set<std::string> SINGLE_OBJECTIVE_ALGORITHMS;
		std::unordered_set<std::string> MULTI_OBJECTIVE_ALGORITHMS;


	};

	class AlgorithmRegisterAction
	{
	public:
		AlgorithmRegisterAction(const std::string& optimization_type, const std::string& category, char* name, AlgorithmCreator create_method)
		{
			std::unordered_map<std::string, std::vector<char*>>& IMPLEMENTED_ALGORITHMS_NAME = AlgorithmFactory::Instance()->GetImplementedAlgorithmsName();
			std::unordered_map<std::string, AlgorithmCreator>& IMPLEMENTED_ALGORITHMS_METHOD = AlgorithmFactory::Instance()->GetImplementedAlgorithmsMethod();
			std::unordered_set<std::string>& SINGLE_OBJECTIVE_ALGORITHMS = AlgorithmFactory::Instance()->GetSingleObjectiveAlgorithms();
			std::unordered_set<std::string>& MULTI_OBJECTIVE_ALGORITHMS = AlgorithmFactory::Instance()->GetMultiObjectiveAlgorithms();

			IMPLEMENTED_ALGORITHMS_NAME[category].push_back(name);
			IMPLEMENTED_ALGORITHMS_METHOD[name] = create_method;
			if (optimization_type == "Multiobjective")
				MULTI_OBJECTIVE_ALGORITHMS.insert(name);
			else if (optimization_type == "Singleobjective")
				SINGLE_OBJECTIVE_ALGORITHMS.insert(name);
		}
	};

}