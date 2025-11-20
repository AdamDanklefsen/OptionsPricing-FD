#pragma once
#include "options.hpp"
#include "grid.hpp"
#include <vector>
#include <cstddef>

struct SolverConfig {
    std::size_t N_S = 400;         // number of S intervals (grid will have N_S+1 points)
    std::size_t N_t = 400;         // number of time steps
    double S_max_multiplier = 3.0; // Smax = S_max_multiplier * S0
    double tol = 1e-8;             // tolerance placeholder
    double omega = 1.2;            // PSOR relaxation factor (unused in simple projection)
    std::size_t max_iter = 10000;  // PSOR max iterations
    bool use_crank_nicolson = false;
    bool verbose = false;
};

struct SolverResult {
    std::vector<double> S_grid;        // S grid
    std::vector<double> option_values; // values at t=0 (same length as S_grid)
    std::vector<double> early_exercise; // optional S* per time slice
    std::size_t Ns() const { return S_grid.size(); }
};

class AmericanPDESolver {
public:
    AmericanPDESolver(const OptionData& opt, const SolverConfig& cfg);

    // Solve starting from S0 (used for grid sizing); returns SolverResult
    SolverResult solve(double S0);

private:
    OptionData opt_;
    SolverConfig cfg_;
    Grid grid_;

    void init_grid(double S0);
    void init_payoff(std::vector<double>& V);
    void timestep_backward(const std::vector<double>& V_old, std::vector<double>& V_new, double dt, std::vector<double>& exercise_boundary);
    void apply_early_exercise_projection(std::vector<double>& V);
};