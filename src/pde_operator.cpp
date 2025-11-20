#include "pde_operator.hpp"
#include <cmath>
#include <vector>

void PDEOperator::build_implicit_coefficients(const std::vector<double>& S,
                                              double sigma, double r, double q,
                                              double dt,
                                              std::vector<double>& a,
                                              std::vector<double>& b,
                                              std::vector<double>& c) {
    const std::size_t n = S.size();
    if (n < 3) {
        a.clear(); b.clear(); c.clear();
        return;
    }
    a.assign(n - 2, 0.0);
    b.assign(n - 2, 0.0);
    c.assign(n - 2, 0.0);

    for (std::size_t i = 1; i + 1 < n; ++i) {
        double Si = S[i];
        double dS_f = S[i + 1] - S[i];
        double dS_b = S[i] - S[i - 1];
        double sigma2 = sigma * sigma;

        // Second derivative coefficient approximation on non-uniform grid
        double A = 0.5 * sigma2 * Si * Si / (dS_b * (dS_b + dS_f));
        double C = 0.5 * sigma2 * Si * Si / (dS_f * (dS_b + dS_f));
        double B = r * Si / (dS_f + dS_b);

        // Coefficients for implicit Euler: (I + dt*L) V_new = V_old
        a[i - 1] = -dt * (A - B);
        b[i - 1] = 1.0 + dt * (A + C + r);
        c[i - 1] = -dt * (C + B);
    }
}