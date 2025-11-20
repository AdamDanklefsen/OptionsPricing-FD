#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include "american_solver.hpp"
#include "options.hpp"

namespace py = pybind11;

PYBIND11_MODULE(cppamerican, m) {
    m.doc() = "American option PDE solver (finite-difference)";

    py::enum_<OptionData::Type>(m, "OptionType")
        .value("European", OptionData::Type::European)
        .value("American", OptionData::Type::American)
        .value("Call", OptionData::Type::Call)
        .value("Put", OptionData::Type::Put)
        .export_values();

    py::class_<OptionData>(m, "OptionData")
        .def(py::init<>())
        .def_readwrite("type", &OptionData::type)
        .def_readwrite("strike", &OptionData::strike)
        .def_readwrite("maturity", &OptionData::maturity)
        .def_readwrite("r", &OptionData::r)
        .def_readwrite("q", &OptionData::q)
        .def_readwrite("sigma", &OptionData::sigma)
        .def_readwrite("id", &OptionData::id);

    py::class_<SolverConfig>(m, "SolverConfig")
        .def(py::init<>())
        .def_readwrite("N_S", &SolverConfig::N_S)
        .def_readwrite("N_t", &SolverConfig::N_t)
        .def_readwrite("S_max_multiplier", &SolverConfig::S_max_multiplier)
        .def_readwrite("tol", &SolverConfig::tol)
        .def_readwrite("omega", &SolverConfig::omega)
        .def_readwrite("max_iter", &SolverConfig::max_iter)
        .def_readwrite("use_crank_nicolson", &SolverConfig::use_crank_nicolson)
        .def_readwrite("verbose", &SolverConfig::verbose);

    py::class_<SolverResult>(m, "SolverResult")
        .def_readonly("S_grid", &SolverResult::S_grid)
        .def_readonly("option_values", &SolverResult::option_values)
        .def_readonly("early_exercise", &SolverResult::early_exercise);

    py::class_<AmericanPDESolver>(m, "AmericanPDESolver")
        .def(py::init<const OptionData&, const SolverConfig&>())
        .def("solve", [](AmericanPDESolver &self, double S0) {
            SolverResult res = self.solve(S0);
            // Return numpy copies
            py::array_t<double> S(res.S_grid.size(), res.S_grid.data());
            py::array_t<double> V(res.option_values.size(), res.option_values.data());
            py::array_t<double> boundary(res.early_exercise.size(), res.early_exercise.data());
            return py::make_tuple(S, V, boundary);
        });
}