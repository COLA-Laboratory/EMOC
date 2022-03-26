#pragma once
#include <mutex>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>

#include "problem/problem.h"

namespace emoc {

	typedef Problem* (*ProblemCreator)(int dec_num, int obj_num);

	class ProblemFactory
	{
	public:
		static ProblemFactory* Instance();

		Problem* CreateProblemObject(const std::string& name, int dec_num, int obj_num);

		inline std::unordered_map<std::string, std::vector<char*>>& GetImplementedProblemsName() { return IMPLEMENTED_PROBLEMS_NAME; }
		inline std::unordered_map<std::string, ProblemCreator>& GetImplementedProblemsMethod() { return IMPLEMENTED_PROBLEMS_METHOD; }
		inline std::unordered_set<std::string>& GetSingleObjectiveProblems() { return SINGLE_OBJECTIVE_PROBLEMS; }
		inline std::unordered_set<std::string>& GetMultiObjectiveProblems() { return MULTI_OBJECTIVE_PROBLEMS; }

	private:
		ProblemFactory();
		~ProblemFactory();
		ProblemFactory(const ProblemFactory&);
		ProblemFactory& operator=(const ProblemFactory&);

	private:
		// for release ProblemFactory instance
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
		static ProblemFactory* s_Instance;
		static std::mutex singleton_mutex_;


		// implemented problems - we use c-style strings for the compatibility with Dear ImGUI
		std::unordered_map<std::string, std::vector<char*>> IMPLEMENTED_PROBLEMS_NAME;
		std::unordered_map<std::string, ProblemCreator> IMPLEMENTED_PROBLEMS_METHOD;
		std::unordered_set<std::string> SINGLE_OBJECTIVE_PROBLEMS;
		std::unordered_set<std::string> MULTI_OBJECTIVE_PROBLEMS;
	};

	class ProblemRegisterAction
	{
	public:
		ProblemRegisterAction(const std::string& optimization_type, const std::string& category, char* name, ProblemCreator create_method)
		{
			std::unordered_map<std::string, std::vector<char*>>& IMPLEMENTED_PROBLEMS_NAME = ProblemFactory::Instance()->GetImplementedProblemsName();
			std::unordered_map<std::string, ProblemCreator>& IMPLEMENTED_PROBLEMS_METHOD = ProblemFactory::Instance()->GetImplementedProblemsMethod();
			std::unordered_set<std::string>& SINGLE_OBJECTIVE_PROBLEMS = ProblemFactory::Instance()->GetSingleObjectiveProblems();
			std::unordered_set<std::string>& MULTI_OBJECTIVE_PROBLEMS = ProblemFactory::Instance()->GetMultiObjectiveProblems();

			IMPLEMENTED_PROBLEMS_NAME[category].push_back(name);
			IMPLEMENTED_PROBLEMS_METHOD[name] = create_method;
			if (optimization_type == "Multiobjective")
				MULTI_OBJECTIVE_PROBLEMS.insert(name);
			else if (optimization_type == "Singleobjective")
				SINGLE_OBJECTIVE_PROBLEMS.insert(name);
		}
	};

}