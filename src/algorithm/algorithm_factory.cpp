#include "algorithm/algorithm_factory.h"

namespace emoc {

	AlgorithmFactory::Garbo AlgorithmFactory::garbo_;
	AlgorithmFactory* AlgorithmFactory::s_Instance = nullptr;
	std::mutex AlgorithmFactory::singleton_mutex_;

	emoc::AlgorithmFactory* AlgorithmFactory::Instance()
	{
		if (s_Instance == nullptr)
		{
			std::lock_guard<std::mutex> locker(singleton_mutex_);
			if (s_Instance == nullptr)
			{
				s_Instance = new AlgorithmFactory();
			}
		}

		return s_Instance;
	}

	emoc::Algorithm* AlgorithmFactory::CreateAlgorithmObject(const std::string& name, int thread_id)
	{
		std::unordered_map<std::string, AlgorithmCreator>& IMPLEMENTED_ALGORITHMS_METHOD = AlgorithmFactory::Instance()->GetImplementedAlgorithmsMethod();
		
		auto iter = IMPLEMENTED_ALGORITHMS_METHOD.find(name);
		if (iter == IMPLEMENTED_ALGORITHMS_METHOD.end())
			return nullptr;
		else
			return iter->second(thread_id);
	}

	AlgorithmFactory::AlgorithmFactory()
	{

	}

	AlgorithmFactory::~AlgorithmFactory()
	{

	}


}