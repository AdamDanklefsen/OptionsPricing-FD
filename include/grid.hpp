#pragma once
#include <vector>
#include <cstddef>

struct Grid {
    std::vector<double> S;   // spot grid points (ascending)
    double dS = 0.0;         // nominal spacing (useful for uniform grids)

    std::size_t Ns() const { return S.size(); }

    // create a simple uniform grid 0..Smax with N intervals -> N+1 points
    static Grid make_uniform(double Smax, std::size_t N) {
        Grid g;
        g.S.resize(N + 1);
        g.dS = Smax / static_cast<double>(N);
        for (std::size_t i = 0; i <= N; ++i) g.S[i] = static_cast<double>(i) * g.dS;
        return g;
    }
};