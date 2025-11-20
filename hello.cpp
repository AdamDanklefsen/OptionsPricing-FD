#include <pybind11/pybind11.h>
#include <string>

std::string hello() {
    return "Hello from C++!";
}

// Placeholder stub for an American option solver (replace with your PDE solver).
double american_solver_stub(double S, double K, double r, double sigma, double T) {
    // Simple intrinsic payoff placeholder: max(S-K, 0)
    double payoff = S - K;
    return payoff > 0.0 ? payoff : 0.0;
}

namespace py = pybind11;

PYBIND11_MODULE(cpphello, m) {
    m.doc() = "Minimal pybind11 example: hello and american_solver_stub";

    m.def("hello", &hello, "Return a hello string from C++");

    m.def("american_solver_stub", &american_solver_stub,
          py::arg("S"), py::arg("K"), py::arg("r"), py::arg("sigma"), py::arg("T"),
          "A placeholder stub for American option solver (replace with PDE solver)");
}