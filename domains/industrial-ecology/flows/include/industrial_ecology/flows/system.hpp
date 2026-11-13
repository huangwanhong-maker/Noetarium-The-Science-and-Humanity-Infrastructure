// Material flow analysis: a system of processes joined by flows, with stocks, and the balance that
// every process keeps (theory note §3; Brunner and Rechberger 2016, Handbook of Material Flow Analysis,
// 2nd ed., ch. 2).
//
// A system has processes and variables. A variable is a flow from one process to another, from outside
// the system's boundary into a process (an import, an extraction), from a process to outside (an
// export, an emission), or the change of a process's stock in the period. Each process balances:
//
//   Σ flows into p − Σ flows out of p − ΔS_p = 0,     that is   A x = 0,
//
// with A the incidence of processes on variables (+1 where a variable enters p, −1 where it leaves p,
// −1 for p's own stock change). Mass is the conserved quantity; a system of goods and a system of a
// substance (its concentration in each good) balance alike (substances(), below).
//
//   transfer(model, imports)     the steady flows of a system given by transfer coefficients: each
//                                process sends fixed shares of its throughput to other processes, to
//                                outside and to its stock, so that T = u + Kᵀ T, T = (I − Kᵀ)⁻¹ u
//                                (Baccini and Brunner 2012, Metabolism of the Anthroposphere, 2nd ed., ch. 4)
//   substances(system, x, c)     the flows of a substance carried by the goods at concentrations c, each
//                                process's imbalance in it, and its transfer coefficients: the share of
//                                what enters a process that leaves by each output

#pragma once

#include <calx/linalg/dense.hpp>
#include <calx/linalg/rectangular.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

namespace industrial_ecology::flows {

inline constexpr int outside = -1;  // the environment beyond the system's boundary

struct Variable {
  std::string name;
  int from = outside, to = outside;  // processes by index, or outside
  bool stock = false;                // the change of process `from`'s stock (to is ignored)
};

struct System {
  std::vector<std::string> processes;
  std::vector<Variable> variables;

  void validate() const {
    const int n = static_cast<int>(processes.size());
    for (const auto& v : variables) {
      if (v.from < outside || v.from >= n || v.to < outside || v.to >= n) throw std::invalid_argument("industrial_ecology::flows: a variable names a process that does not exist: " + v.name);
      if (v.stock && v.from == outside) throw std::invalid_argument("industrial_ecology::flows: a stock change belongs to a process: " + v.name);
      if (!v.stock && v.from == outside && v.to == outside) throw std::invalid_argument("industrial_ecology::flows: a flow touches at least one process: " + v.name);
      if (!v.stock && v.from == v.to) throw std::invalid_argument("industrial_ecology::flows: a flow from a process to itself is no flow: " + v.name);
    }
  }
  // A, processes × variables.
  calx::linalg::Rectangular incidence() const {
    validate();
    calx::linalg::Rectangular A(processes.size(), variables.size());
    for (std::size_t j = 0; j < variables.size(); j++) {
      const auto& v = variables[j];
      if (v.from != outside) A(static_cast<std::size_t>(v.from), j) -= 1;
      if (!v.stock && v.to != outside) A(static_cast<std::size_t>(v.to), j) += 1;
    }
    return A;
  }
  // Each process's imbalance, inflows − outflows − stock change, at values x.
  std::vector<double> imbalance(const std::vector<double>& x) const {
    if (x.size() != variables.size()) throw std::invalid_argument("industrial_ecology::flows: one value per variable");
    return calx::linalg::multiply(incidence(), x);
  }
};

// ---- transfer coefficients ----
struct TransferModel {
  std::vector<std::string> processes;
  calx::linalg::Matrix shares;        // shares(p, q): the share of p's throughput sent to process q
  std::vector<double> accumulation;   // the share of p's throughput added to its stock; the rest leaves the system
};
struct Transfer {
  std::vector<double> throughput, to_outside, stock_change;
  calx::linalg::Matrix flows;         // flows(p, q) = shares(p, q) T_p
  double imports = 0, exports = 0, accumulated = 0;
  double balance = 0;                 // |imports − exports − accumulated|, rounding
};

inline Transfer transfer(const TransferModel& m, const std::vector<double>& imports) {
  const std::size_t n = m.processes.size();
  if (m.shares.n != n || m.accumulation.size() != n || imports.size() != n) throw std::invalid_argument("industrial_ecology::flows::transfer: one row of shares, one accumulation and one import per process");
  for (std::size_t p = 0; p < n; p++) {
    double out = m.accumulation[p];
    for (std::size_t q = 0; q < n; q++) {
      if (m.shares(p, q) < 0) throw std::invalid_argument("industrial_ecology::flows::transfer: shares ≥ 0");
      out += m.shares(p, q);
    }
    if (m.accumulation[p] < 0 || out > 1 + 1e-12) throw std::invalid_argument("industrial_ecology::flows::transfer: the shares of a process and its accumulation sum to at most one");
  }
  calx::linalg::Matrix I_Kt = calx::linalg::Matrix::identity(n), U(n);
  for (std::size_t p = 0; p < n; p++)
    for (std::size_t q = 0; q < n; q++) I_Kt(q, p) -= m.shares(p, q);
  for (std::size_t p = 0; p < n; p++) U(p, 0) = imports[p];
  const auto X = calx::linalg::solve(I_Kt, U);
  Transfer r;
  r.throughput.resize(n), r.to_outside.resize(n), r.stock_change.resize(n);
  r.flows = calx::linalg::Matrix(n);
  for (std::size_t p = 0; p < n; p++) {
    const double T = X(p, 0);
    r.throughput[p] = T;
    double sent = 0;
    for (std::size_t q = 0; q < n; q++) sent += r.flows(p, q) = m.shares(p, q) * T;
    r.stock_change[p] = m.accumulation[p] * T;
    r.to_outside[p] = T - sent - r.stock_change[p];
    r.imports += imports[p], r.exports += r.to_outside[p], r.accumulated += r.stock_change[p];
  }
  r.balance = std::fabs(r.imports - r.exports - r.accumulated);
  return r;
}

// ---- a substance carried by the goods ----
struct Substance {
  std::vector<double> flows;          // per variable: x_j c_j
  std::vector<double> imbalance;      // per process: what of the substance its goods' balance leaves unaccounted
  calx::linalg::Rectangular transfer; // processes × variables: the share of what enters p (by flows in) that leaves by each variable
};

inline Substance substances(const System& s, const std::vector<double>& x, const std::vector<double>& concentration) {
  if (x.size() != s.variables.size() || concentration.size() != x.size()) throw std::invalid_argument("industrial_ecology::flows::substances: one value and one concentration per variable");
  Substance r;
  r.flows.resize(x.size());
  for (std::size_t j = 0; j < x.size(); j++) r.flows[j] = x[j] * concentration[j];
  r.imbalance = s.imbalance(r.flows);
  r.transfer = calx::linalg::Rectangular(s.processes.size(), x.size());
  for (std::size_t p = 0; p < s.processes.size(); p++) {
    double in = 0;
    for (std::size_t j = 0; j < x.size(); j++)
      if (!s.variables[j].stock && s.variables[j].to == static_cast<int>(p)) in += r.flows[j];
    if (in > 0)
      for (std::size_t j = 0; j < x.size(); j++)
        if (s.variables[j].from == static_cast<int>(p)) r.transfer(p, j) = r.flows[j] / in;
  }
  return r;
}

}  // namespace industrial_ecology::flows
