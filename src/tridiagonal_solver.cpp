#include "tridiagonal_solver.hpp"
#include <vector>

bool TridiagonalSolver::solve(const std::vector<double>& a,
                              const std::vector<double>& b,
                              const std::vector<double>& c,
                              std::vector<double>& d) {
    const std::size_t n = b.size();
    if (n == 0 || d.size() != n) return false;
    if (n == 1) {
        d[0] = d[0] / b[0];
        return true;
    }
    std::vector<double> cp(n - 1);
    std::vector<double> dp(n);
    // forward sweep
    cp[0] = c[0] / b[0];
    dp[0] = d[0] / b[0];
    for (std::size_t i = 1; i < n - 1; ++i) {
        double m = b[i] - a[i - 1] * cp[i - 1];
        if (m == 0.0) return false;
        cp[i] = c[i] / m;
        dp[i] = (d[i] - a[i - 1] * dp[i - 1]) / m;
    }
    double m = b[n - 1] - a[n - 2] * cp[n - 2];
    if (m == 0.0) return false;
    dp[n - 1] = (d[n - 1] - a[n - 2] * dp[n - 2]) / m;
    // back substitution
    d[n - 1] = dp[n - 1];
    for (std::size_t i = n - 1; i-- > 0;) {
        d[i] = dp[i] - cp[i] * d[i + 1];
    }
    return true;
}