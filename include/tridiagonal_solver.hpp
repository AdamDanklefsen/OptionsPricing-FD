#pragma once
#include <vector>

// Thomas algorithm for tridiagonal systems
// a: sub-diagonal (n-1), b: diagonal (n), c: super-diagonal (n-1)
// d: right-hand side (n) and overwritten with solution
class TridiagonalSolver {
public:
    // Returns true on success
    static bool solve(const std::vector<double>& a,
                      const std::vector<double>& b,
                      const std::vector<double>& c,
                      std::vector<double>& d);
};