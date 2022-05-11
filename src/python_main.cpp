#include <iostream>
#include <memory>

#include "problem/problem.h"
#include "core/individual.h"
#include "core/emoc_utility_structures.h"
#include "core/emoc_manager.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

using emoc::Individual;
using emoc::EMOCManager;
using emoc::EMOCParameters;
namespace py = pybind11;

namespace emoc {

	// for polymorphism of Problem Class in python
	class PyProblem : public Problem
	{
	public:
		using Problem::Problem;

		/* Trampoline (need one for each virtual function) */
		void CalObj(Individual* ind) override {
			PYBIND11_OVERRIDE_PURE(
				void, /* Return type */
				Problem,      /* Parent class */
				CalObj,          /* Name of function in C++ (must match Python name) */
				ind      /* Argument(s) */
			);
		}
	};
}

PYBIND11_MODULE(EMOC, m) {
	// define EMOC parameter class
	py::class_<EMOCParameters>(m, "EMOCParameters")
		.def(py::init<>())
		.def("SetProblem", &EMOCParameters::SetProblem)
		.def("SetInitialPop", &EMOCParameters::SetInitialPop)
		.def_readwrite("algorithm_name", &EMOCParameters::algorithm_name)
		.def_readwrite("problem_name", &EMOCParameters::problem_name)
		.def_readwrite("population_num", &EMOCParameters::population_num)
		.def_readwrite("decision_num", &EMOCParameters::decision_num)
		.def_readwrite("objective_num", &EMOCParameters::objective_num)
		.def_readwrite("max_evaluation", &EMOCParameters::max_evaluation)
		.def_readwrite("output_interval", &EMOCParameters::output_interval);

	// define EMOC result class
	py::class_<emoc::EMOCGeneralResult>(m, "EMOCGeneralResult")
		.def(py::init<>())
		.def_readonly("igd", &emoc::EMOCGeneralResult::igd)
		.def_readonly("gd", &emoc::EMOCGeneralResult::gd)
		.def_readonly("hv", &emoc::EMOCGeneralResult::hv)
		.def_readonly("spacing", &emoc::EMOCGeneralResult::spacing)
		.def_readonly("igdplus", &emoc::EMOCGeneralResult::igdplus)
		.def_readonly("gdplus", &emoc::EMOCGeneralResult::gdplus)
		.def_readonly("best_value", &emoc::EMOCGeneralResult::best_value)
		.def_readonly("runtime", &emoc::EMOCGeneralResult::runtime)
		.def_readonly("pop_num", &emoc::EMOCGeneralResult::pop_num)
		.def_readwrite("pop_decs", &emoc::EMOCGeneralResult::pop_decs)
		.def_readwrite("pop_objs", &emoc::EMOCGeneralResult::pop_objs);

	// define EMOCManager class
	py::class_<EMOCManager, std::unique_ptr<EMOCManager, py::nodelete>>(m, "EMOCManager")
		.def(py::init([]() {
			return std::unique_ptr<EMOCManager, py::nodelete> (EMOCManager::Instance());
			}))
		.def("Run", &EMOCManager::Run)
		.def("SetTaskParameters", &EMOCManager::SetTaskParameters)
		.def("GetResult", &EMOCManager::GetPythonResult);

	// define Individual class for problem's CalObj function parameters
	py::class_<Individual>(m, "Individual")
		.def(py::init<int, int>())
		.def_readwrite("dec", &Individual::dec_)
		.def_readwrite("obj", &Individual::obj_);

	// define Problem base class, so we can write custom problem derived from this class in python
	py::class_<emoc::Problem, emoc::PyProblem /* <--- trampoline*/>(m, "Problem")
		.def(py::init<int, int>())
		.def("CalObj", &emoc::Problem::CalObj)
		.def_readwrite("dec_num", &emoc::Problem::dec_num_)
		.def_readwrite("obj_num", &emoc::Problem::obj_num_)
		.def_readwrite("lower_bound", &emoc::Problem::lower_bound_)
		.def_readwrite("upper_bound", &emoc::Problem::upper_bound_);

}