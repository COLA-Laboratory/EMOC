#include "emoc_app.h"

namespace emoc {



	std::mutex finish_mutex;
	std::mutex pause_mutex;
	std::condition_variable cond;
}