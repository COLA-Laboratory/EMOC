#include "problem/problem_factory.h"

namespace emoc {

	ProblemFactory::Garbo ProblemFactory::garbo_;
	ProblemFactory* ProblemFactory::s_Instance = nullptr;
	std::mutex ProblemFactory::singleton_mutex_;

	emoc::ProblemFactory* ProblemFactory::Instance()
	{
		if (s_Instance == nullptr)
		{
			std::lock_guard<std::mutex> locker(singleton_mutex_);
			if (s_Instance == nullptr)
			{
				s_Instance = new ProblemFactory();
			}
		}

		return s_Instance;
	}

	emoc::Problem* ProblemFactory::CreateProblemObject(const std::string& name, int dec_num, int obj_num)
	{
		std::unordered_map<std::string, ProblemCreator>& IMPLEMENTED_PROBLEMS_METHOD = ProblemFactory::Instance()->GetImplementedProblemsMethod();

		auto iter = IMPLEMENTED_PROBLEMS_METHOD.find(name);
		if (iter == IMPLEMENTED_PROBLEMS_METHOD.end())
			return nullptr;
		else
			return iter->second(dec_num,obj_num);
	}

	ProblemFactory::ProblemFactory()
	{

	}

	ProblemFactory::~ProblemFactory()
	{

	}


}