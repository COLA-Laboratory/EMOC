#include "emoc_app.h"

namespace emoc {

	std::mutex EMOCLock::finish_mutex;
	std::mutex EMOCLock::pause_mutex;
	std::condition_variable EMOCLock::pause_cond;




}