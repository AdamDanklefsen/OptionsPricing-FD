#include "american_solver.hpp"
#include "pde_operator.hpp"
#include "tridiagonal_solver.hpp"
#include <algorithm>
#include <cmath>
#include <vector>
#include <iostream>

AmericanPDESolver::AmericanPDESolver(const OptionData& opt, const SolverConfig& cfg)
: opt_(opt), cfg_(cfg) {}

void AmericanPDESolver::init_grid(double S0) {
    double Smax = std::max(1.0, cfg_.S_max_multiplier * S0);
    grid_ = Grid::make_uniform(Smax, cfg_.N_S);
}

void AmericanPDESolver::init_payoff(std::vector<double>& V) {
    const std::size_t n = grid_.S.size();
    V.resize(n);
    for (std::size_t i = 0; i < n; ++i) {
        double S = grid_.S[i];
        if (opt_.is_call()) V[i] = std::max(0.0, S - opt_.strike);
        else V[i] = std::max(0.0, opt_.strike - S);
    }
}

void AmericanPDESolver::apply_early_exercise_projection(std::vector<double>& V) {
    const std::size_t n = V.size();
    for (std::size_t i = 0; i < n; ++i) {
        double S = grid_.S[i];
        double payoff = opt_.is_call() ? std::max(0.0, S - opt_.strike) : std::max(0.0, opt_.strike - S);
        if (V[i] < payoff) V[i] = payoff;
    }
}

void AmericanPDESolver::timestep_backward(const std::vector<double>& V_old, std::vector<double>& V_new, double dt, std::vector<double>& /*exercise_boundary*/) {
    const std::size_t n = grid_.S.size();
    if (n < 3) {
        V_new = V_old;
        return;
    }

    std::vector<double> a, b, c;
    PDEOperator::build_implicit_coefficients(grid_.S, opt_.sigma, opt_.r, opt_.q, dt, a, b, c);

    // RHS for implicit Euler is simply V_old at interior nodes
    std::vector<double> rhs(n - 2);
    for (std::size_t i = 1; i + 1 < n; ++i) rhs[i - 1] = V_old[i];

    bool ok = TridiagonalSolver::solve(a, b, c, rhs);
    V_new = V_old;
    if (ok) {
        for (std::size_t i = 1; i + 1 < n; ++i) V_new[i] = rhs[i - 1];
    } else {
        // fallback: copy old values
        V_new = V_old;
    }

    // simple boundary conditions (could be improved)
    if (opt_.is_call()) {
        V_new[0] = 0.0;
        V_new[n - 1] = grid_.S.back() - opt_.strike;
    } else {
        V_new[0] = opt_.strike;
        V_new[n - 1] = 0.0;
    }

    // enforce early exercise via projection (simple approach)
    apply_early_exercise_projection(V_new);
}

SolverResult AmericanPDESolver::solve(double S0) {
    init_grid(S0);
    const std::size_t n = grid_.S.size();
    std::vector<double> V_old, V_new;
    init_payoff(V_old);

    const double T = opt_.maturity;
    const std::size_t Nt = std::max<std::size_t>(1, cfg_.N_t);
    const double dt = (T > 0.0) ? (T / static_cast<double>(Nt)) : 0.0;

    std::vector<double> exercise_boundary; // unused in simple projection implementation

    for (std::size_t t = 0; t < Nt; ++t) {
        timestep_backward(V_old, V_new, dt, exercise_boundary);
        V_old.swap(V_new);
        if (cfg_.verbose && (t % std::max<std::size_t>(1, Nt/10) == 0)) {
            std::cerr << "Progress: " << t << " / " << Nt << std::endl;
        }
    }

    SolverResult res;
    res.S_grid = grid_.S;
    res.option_values = V_old;
    res.early_exercise = exercise_boundary;
    return res;
}