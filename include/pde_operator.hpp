#pragma once
#include <vector>

// Build implicit finite-difference operator coefficients for pricing equations.
// The API computes the a, b, c vectors for interior nodes (i = 1 .. n-2)
// such that the tridiagonal matrix times V_new = RHS.
class PDEOperator {
public:
    // S: grid (size n)
    // sigma, r, q: model params
    // dt: time step
    // a,b,c are sized n-2 on return
    static void build_implicit_coefficients(const std::vector<double>& S,
                                            double sigma, double r, double q,
                                            double dt,
                                            std::vector<double>& a,
                                            std::vector<double>& b,
                                            std::vector<double>& c);
};