// Well-being: what a disaster's losses cost households in consumption over their recovery, valued by the utility of what they
// consume, after Unbreakable (Hallegatte, Vogt-Schilb, Bangalore and Rozenberg 2017) and its two working papers (theory/
// wellbeing.md).
//
// THE TWO PAPERS. Hallegatte, Bangalore and Vogt-Schilb (2016, World Bank Policy Research Working Paper 7886; "P1") define
// socioeconomic resilience as the ratio of asset losses to well-being losses, with income from capital at its average productivity
// Π, a share of income diversified through transfers, and reconstruction at a rate λ paid from consumption (their Eqs 1–11). Walsh
// and Hallegatte (2019, PRWP 8723, the working paper of Economics of Disasters and Climate Change 4, 249–293, 2020; "P2") carry it to
// households: income from effective capital and from transfers (Eqs 2–6), losses of assets, of income and of transfers (Eqs 7–16),
// reconstruction paid from consumption, at a rate each household may choose (Eqs 17–23), public assets rebuilt by a later tax (Eq. 24),
// savings and support spent to cap the deepest part of the loss (Eqs 25–27), and the well-being loss integrated over ten years with an
// equivalent consumption loss at national mean consumption (Eqs 28–35). Both are CC BY 3.0 IGO; the equations here are written from
// them, not from any code. P2's typographical slips are read as its other equations require (theory note §6).
//
// THE MODEL. A household h stands for ω_h persons, each with income i_h a year (consumption taken equal), of which i_h^sp from
// transfers and remittances. Transfers are financed by a flat tax δ_sp = Σ ω i^sp / Σ ω i (P2 Eq. 3), so that effective capital is
// k_h = (i_h − i_h^sp)/((1 − δ_sp) Π) (Eq. 5) and K = Σ ω k (Eq. 6). In an event a share f_h of the household is affected and loses
// Δk_h = v_h k_h, v reduced by a share π for households warned in time (pp. 45–46). With L = Σ ω f Δk the nation's asset loss:
//   - the affected lose income (1 − δ_sp) Π Δk e^{−λ_h t} from their own assets (Eq. 12) and rebuild their private share s at λ_h,
//     λ_h s Δk e^{−λ_h t} from consumption (Eq. 20);
//   - everyone loses (L/K) i^sp e^{−λ t} of transfers, which fall with the nation's assets (Eq. 14);
//   - public assets, the share 1 − s, are rebuilt by the government and paid later by a flat tax i_h L_pub/K (Eq. 24).
// λ = ln 20 / N, with 95% rebuilt in N years (P1 p. 5; P2 Eq. 18). A household may instead choose λ_h to maximise its well-being over the
// horizon (P2 Eqs 22–23). Where consumption would fall below a declared floor, reconstruction slows until it does not (P2 pp. 53–55);
// where income loss alone takes it below, the household rebuilds no faster than a declared slowest pace, and its consumption is held at
// a hard minimum, the loss beyond it reported as understated.
//
// BUFFERS. Savings and support received are spent to cap the consumption loss at a level γ until they are spent at t̂, the area above
// γ equal to what was spent (P2 Eqs 25 and 27, read with γ the level of the loss). Support is spent before savings, and savings spent
// are rebuilt later.
//
// SUPPORT. A payout to everyone helped, a declared share of the mean asset loss of a reference group's affected members (P2 pp. 26–27;
// P1 p. 17), reaching the affected but for a share excluded and the unaffected for a share included in error (P1 Eq. 16), financed by a
// flat tax on income paid later.
//
// WELL-BEING. ΔW_h = ∫_0^H [u(c) − u(c − Δc(t))] e^{−ρt} dt + c^{−η} (taxes paid later + savings rebuilt), u(c) = c^{1−η}/(1−η)
// (log c at η = 1), the later term undiscounted as P2's Eq. 31 prints it. The equivalent consumption loss is ΔC_eq = ΔW / c̄^{−η}, c̄
// national mean consumption (P2 Eq. 34), and socioeconomic resilience ΔK / ΔC_eq (P1 p. 2). Over return periods the expectations are
// formed with the step weights p_j = 1/r_j − 1/r_{j+1}, p_m = 1/r_m, and resilience is their ratio (P1 p. 13).
//
// Units: incomes, consumption, savings and capital a person, in one currency a year; well-being in utility-years.

#pragma once

#include <calx/optimisation/minimise.hpp>
#include <calx/quadrature/gauss_kronrod.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace risk_management::wellbeing {

namespace detail {
inline void require(bool ok, const std::string& what) {
  if (!ok) throw std::invalid_argument("risk_management::wellbeing: " + what);
}
}  // namespace detail

struct Readings {
  double eta = 1.5;              // the elasticity of marginal utility (P1 p. 9; P2 p. 58)
  double rho = 0.06;             // the discount rate a year (the papers' code; the report's 6%)
  double productivity = 0.33;    // Π: output a year per unit of effective capital (the country's own, from national accounts)
  double rebuild_years = 3;      // N: 95% rebuilt in N years (P1 p. 15)
  double horizon = 10;           // years over which well-being is counted (P2 pp. 53, 59)
  double early_warning = 0.2;    // π: the share of vulnerability removed for a household warned in time (P1 p. 13; P2 p. 46)
  double floor = 0;              // subsistence consumption a person a year: reconstruction slows to keep above it (0: none)
  double hard_minimum = 0.05;    // consumption never modelled below this share of a household's own, the excess reported
  double slowest_rebuild_years = 20;  // the slowest pace of a household whose income loss alone breaches the floor
  bool optimal_rebuilding = false;    // each household chooses λ_h to maximise its well-being (P2 Eqs 22–23)
  double lambda() const { return std::log(20.0) / rebuild_years; }
  void check() const {
    detail::require(eta > 0 && rho >= 0 && productivity > 0 && rebuild_years > 0 && horizon > 0, "readings: η > 0, ρ ≥ 0, Π > 0, N > 0, a horizon > 0");
    detail::require(early_warning >= 0 && early_warning < 1 && floor >= 0 && hard_minimum > 0 && hard_minimum < 1 && slowest_rebuild_years >= rebuild_years,
                    "readings: π in [0, 1), a floor at least zero, a hard minimum in (0, 1), the slowest pace no faster than N");
  }
};

struct Household {
  std::string id;
  std::string group;
  double weight = 1;          // the persons it stands for
  double income = 0;          // a person a year (consumption taken equal)
  double transfers = 0;       // of which from transfers and remittances
  double savings = 0;         // a person
  double private_share = 1;   // the share of its effective capital it rebuilds itself
  double vulnerability = 0.4; // v: the share of its effective capital lost if affected
  bool warned = false;
};

struct Support {
  double share = 0;                // θ: the payout as a share of the reference group's mean asset loss (0: none)
  std::string reference_group;     // "" for every affected household
  double exclusion = 0;            // the share of the affected not reached
  double inclusion = 0;            // the share of the unaffected reached in error
};

struct Event {
  double return_period = 0;
  std::vector<double> exposed;     // f_h: the share of each household affected
};

struct Calibration {
  double delta_sp = 0;             // the flat tax financing transfers
  std::vector<double> capital;     // k_h a person
  double K = 0;                    // Σ ω k
  double mean_consumption = 0;     // c̄
  double persons = 0;
};

inline void check(const std::vector<Household>& H) {
  detail::require(!H.empty(), "at least one household");
  for (const auto& h : H) {
    detail::require(h.weight >= 0 && h.income > 0 && h.transfers >= 0 && h.transfers < h.income && h.savings >= 0,
                    "a household has a weight at least zero, a positive income, transfers below it and savings at least zero (" + h.id + ")");
    detail::require(h.private_share >= 0 && h.private_share <= 1 && h.vulnerability >= 0 && h.vulnerability <= 1,
                    "a household's private share and vulnerability lie in [0, 1] (" + h.id + ")");
  }
}

inline Calibration calibrate(const std::vector<Household>& H, const Readings& R) {
  check(H);
  R.check();
  Calibration C;
  double income = 0, sp = 0;
  for (const auto& h : H) {
    income += h.weight * h.income;
    sp += h.weight * h.transfers;
    C.persons += h.weight;
  }
  detail::require(C.persons > 0, "the households stand for someone");
  C.delta_sp = sp / income;
  for (const auto& h : H) {
    C.capital.push_back((h.income - h.transfers) / ((1 - C.delta_sp) * R.productivity));
    C.K += h.weight * C.capital.back();
  }
  C.mean_consumption = income / C.persons;
  return C;
}

inline double utility(double c, double eta) { return std::fabs(eta - 1) < 1e-12 ? std::log(c) : std::pow(c, 1 - eta) / (1 - eta); }

// A consumption loss Δc(t) = a e^{−αt} + b e^{−βt} (a, b ≥ 0), never above a ceiling (consumption held at a hard minimum), and capped
// further at γ until t̂ by a buffer, then followed.
struct Path {
  double a = 0, alpha = 1, b = 0, beta = 1;
  double ceiling = std::numeric_limits<double>::infinity();
  double gamma = std::numeric_limits<double>::infinity(), t_hat = 0;  // the buffer's cap and when it ends
  double raw(double t) const { return a * std::exp(-alpha * t) + b * std::exp(-beta * t); }
  double held(double t) const { return std::min(raw(t), ceiling); }
  double operator()(double t) const { return t < t_hat ? std::min(held(t), gamma) : held(t); }
  // ∫_0^t of the raw loss, and of the loss held below the ceiling.
  double area_raw(double t) const { return a * (1 - std::exp(-alpha * t)) / alpha + b * (1 - std::exp(-beta * t)) / beta; }
  double ceiling_until() const;  // when the raw loss falls to the ceiling (0 if it starts below)
  double area(double t) const {
    const double tc = ceiling_until();
    if (tc <= 0) return area_raw(t);
    return t <= tc ? ceiling * t : ceiling * tc + area_raw(t) - area_raw(tc);
  }
  double total() const {
    const double tc = ceiling_until();
    const double tail = a / alpha + b / beta - area_raw(tc);
    return (tc > 0 ? ceiling * tc : 0.0) + (tc > 0 ? tail : a / alpha + b / beta);
  }
};

namespace detail {
// The time at which a decreasing function of time falls to γ (0 if it starts below).
template <class F>
inline double when(F f, double g) {
  if (f(0) <= g) return 0;
  double lo = 0, hi = 1;
  while (f(hi) > g && hi < 1e6) hi *= 2;
  for (int k = 0; k < 200; k++) {
    const double mid = 0.5 * (lo + hi);
    (f(mid) > g ? lo : hi) = mid;
  }
  return 0.5 * (lo + hi);
}
}  // namespace detail

inline double Path::ceiling_until() const {
  if (!std::isfinite(ceiling) || raw(0) <= ceiling) return 0;
  return detail::when([&](double t) { return raw(t); }, ceiling);
}

// Spend a buffer B on the deepest part of the loss held below the ceiling: γ with ∫_0^{t̂} (loss − γ) = B, loss(t̂) = γ (P2 Eqs 25 and
// 27). Returns what was spent; nothing is spent against the part of a loss the hard minimum already removed.
inline double cap(Path& p, double B) {
  detail::require(B >= 0, "a buffer is at least zero");
  if (B <= 0 || p.held(0) <= 0) return 0;
  if (B >= p.total()) {
    const double all = p.total();
    p.gamma = 0;
    p.t_hat = std::numeric_limits<double>::infinity();
    return all;
  }
  const auto held = [&](double t) { return p.held(t); };
  // The area above γ falls as γ rises: bisect on γ in [0, held(0)].
  double lo = 0, hi = p.held(0);
  for (int k = 0; k < 200; k++) {
    const double g = 0.5 * (lo + hi), t = detail::when(held, g);
    const double above = p.area(t) - g * t;
    (above > B ? lo : hi) = g;
  }
  p.gamma = 0.5 * (lo + hi);
  p.t_hat = detail::when(held, p.gamma);
  return B;
}

// ∫_0^H [u(c) − u(c − Δc(t))] e^{−ρt} dt, the path held at most c − c_min, by Gauss–Kronrod on the pieces where it is smooth.
inline double recovery_loss(const Path& p, double c, const Readings& R) {
  const auto f = [&](double t) { return (utility(c, R.eta) - utility(c - p(t), R.eta)) * std::exp(-R.rho * t); };
  std::vector<double> cuts = {0, R.horizon};
  for (const double t : {p.t_hat, p.ceiling_until()})
    if (t > 0 && t < R.horizon) cuts.push_back(t);
  std::sort(cuts.begin(), cuts.end());
  double s = 0;
  for (std::size_t k = 0; k + 1 < cuts.size(); k++)
    if (cuts[k + 1] > cuts[k]) s += calx::quadrature::integrate(f, cuts[k], cuts[k + 1], 1e-14, 1e-11).value;
  return s;
}

// P2 Eq. 22: the household's own well-being over the horizon per unit of capital, omitting transfers and taxes, as a function of λ.
inline double own_wellbeing(double lambda, double v, const Readings& R) {
  const double Pi = R.productivity;
  const auto f = [&](double t) { return utility(Pi - (Pi + lambda) * v * std::exp(-lambda * t), R.eta) * std::exp(-R.rho * t); };
  return calx::quadrature::integrate(f, 0, R.horizon, 1e-14, 1e-11).value;
}

// The λ_h that maximises it (P2 Eq. 23), within the rates that keep consumption positive at the start: (Π + λ) v < Π.
inline double optimal_lambda(double v, const Readings& R) {
  const double Pi = R.productivity;
  if (v <= 0) return R.lambda();
  const double top = std::min(50.0, v >= 1 ? 0.0 : Pi * (1 - v) / v * (1 - 1e-9));
  const double bottom = std::log(20.0) / R.slowest_rebuild_years;
  if (top <= bottom) return bottom;
  return calx::optimisation::brent_maximise([&](double l) { return own_wellbeing(l, v, R); }, bottom, top, 1e-10).x;
}

struct Category {
  double weight = 0;           // persons
  double consumption = 0;
  double initial_loss = 0;     // Δc at t = 0, after the buffer
  double wellbeing = 0;        // ΔW a person
  bool affected = false, helped = false, capped = false, slowed = false;
  double lambda = 0;
};

struct GroupOutcome {
  double persons = 0, affected = 0;
  double asset_loss = 0;        // Σ ω f Δk
  double wellbeing = 0;         // Σ ω ΔW
  double equivalent = 0;        // ΔW / c̄^{−η}
  double pushed_below = 0;      // persons above the poverty line before, below it at the start of recovery
};

struct Outcome {
  double return_period = 0;
  double asset_loss = 0, wellbeing = 0, equivalent = 0, resilience = 0;
  double income_loss = 0;  // Σ ω Δi at the start: own income lost by the affected and transfers lost by all (P2 Eq. 16: Π L)
  double support_paid = 0, support_payout = 0, support_tax_rate = 0, public_tax_rate = 0;
  double pushed_below = 0;
  std::size_t capped = 0, slowed = 0;        // household categories held at the hard minimum, or whose rebuilding slowed
  std::map<std::string, GroupOutcome> by_group;
  std::vector<double> household_equivalent;  // each household's equivalent loss a person (all its categories)
  std::vector<double> household_asset_loss;  // and its asset loss a person
};

struct Options {
  double poverty_line = 0;  // a person a year (0: none counted)
};

inline Outcome outcome(const std::vector<Household>& H, const Calibration& C, const Readings& R, const Event& e, const Support& S = {}, const Options& O = {}) {
  detail::require(e.exposed.size() == H.size(), "an event gives an exposed share for every household");
  detail::require(S.share >= 0 && S.exclusion >= 0 && S.exclusion <= 1 && S.inclusion >= 0 && S.inclusion <= 1, "support's share is at least zero and its errors lie in [0, 1]");
  const double Pi = R.productivity, lam = R.lambda();
  Outcome out;
  out.return_period = e.return_period;
  std::vector<double> dk(H.size());
  double L = 0, Lpub = 0, ref_loss = 0, ref_persons = 0, income = 0;
  for (std::size_t h = 0; h < H.size(); h++) {
    detail::require(e.exposed[h] >= 0 && e.exposed[h] <= 1, "an exposed share lies in [0, 1] (" + H[h].id + ")");
    const double v = H[h].vulnerability * (H[h].warned ? 1 - R.early_warning : 1);
    dk[h] = v * C.capital[h];
    const double w = H[h].weight * e.exposed[h];
    L += w * dk[h];
    Lpub += w * dk[h] * (1 - H[h].private_share);
    if (S.reference_group.empty() || H[h].group == S.reference_group) {
      ref_loss += w * dk[h];
      ref_persons += w;
    }
    income += H[h].weight * H[h].income;
  }
  out.asset_loss = L;
  out.public_tax_rate = L / C.K > 0 ? Lpub / C.K : 0;
  const double payout = S.share > 0 && ref_persons > 0 ? S.share * ref_loss / ref_persons : 0;
  double helped = 0;
  for (std::size_t h = 0; h < H.size(); h++) helped += H[h].weight * (e.exposed[h] * (1 - S.exclusion) + (1 - e.exposed[h]) * S.inclusion);
  out.support_payout = payout;
  out.support_paid = payout * helped;
  out.support_tax_rate = out.support_paid / income;
  const double c_bar_marginal = std::pow(C.mean_consumption, -R.eta);
  out.household_equivalent.assign(H.size(), 0);
  out.household_asset_loss.assign(H.size(), 0);
  for (std::size_t h = 0; h < H.size(); h++) {
    const auto& x = H[h];
    const double c = x.income, c_min = R.floor < c ? std::max(R.floor, R.hard_minimum * c) : R.hard_minimum * c;  // a household already below the floor is held at the hard minimum
    const double transfer0 = C.K > 0 ? L / C.K * x.transfers : 0;
    const double later_tax = x.income * (out.public_tax_rate + out.support_tax_rate);
    auto& g = out.by_group[x.group];
    g.persons += x.weight;
    g.affected += x.weight * e.exposed[h];
    g.asset_loss += x.weight * e.exposed[h] * dk[h];
    out.household_asset_loss[h] = e.exposed[h] * dk[h];
    out.income_loss += x.weight * (e.exposed[h] * (1 - C.delta_sp) * Pi * dk[h] + transfer0);
    double w_household = 0;
    for (int affected = 1; affected >= 0; affected--)
      for (int help = 1; help >= 0; help--) {
        const double share = affected ? e.exposed[h] * (help ? 1 - S.exclusion : S.exclusion) : (1 - e.exposed[h]) * (help ? S.inclusion : 1 - S.inclusion);
        if (share <= 0) continue;
        Category k;
        k.weight = x.weight * share;
        k.affected = affected;
        k.helped = help;
        k.consumption = c;
        Path p;
        p.b = transfer0;
        p.beta = lam;
        if (affected && dk[h] > 0) {
          double lh = R.optimal_rebuilding ? optimal_lambda(x.vulnerability * (x.warned ? 1 - R.early_warning : 1), R) : lam;
          const double own_income = (1 - C.delta_sp) * Pi * dk[h];
          // Reconstruction slows to keep consumption at the floor (P2 pp. 53–55).
          if (R.floor > 0 && x.private_share > 0 && own_income + lh * x.private_share * dk[h] + transfer0 > c - R.floor) {
            const double lf = (c - R.floor - own_income - transfer0) / (x.private_share * dk[h]);
            lh = std::max(lf, std::log(20.0) / R.slowest_rebuild_years);
            k.slowed = true;
          }
          k.lambda = lh;
          p.a = own_income + lh * x.private_share * dk[h];
          p.alpha = lh;
        }
        p.ceiling = c - c_min;
        k.capped = p.raw(0) > p.ceiling;
        const double buffer = (help ? payout : 0) + x.savings;
        const double spent = cap(p, buffer);
        const double savings_used = std::max(0.0, spent - (help ? payout : 0));
        k.initial_loss = p(0);
        k.wellbeing = recovery_loss(p, c, R) + std::pow(c, -R.eta) * (later_tax + savings_used);
        out.wellbeing += k.weight * k.wellbeing;
        g.wellbeing += k.weight * k.wellbeing;
        w_household += share * k.wellbeing;
        out.capped += k.capped;
        out.slowed += k.slowed;
        if (O.poverty_line > 0 && c >= O.poverty_line && c - k.initial_loss < O.poverty_line) {
          out.pushed_below += k.weight;
          g.pushed_below += k.weight;
        }
      }
    out.household_equivalent[h] = w_household / c_bar_marginal;
  }
  out.equivalent = out.wellbeing / c_bar_marginal;
  out.resilience = out.equivalent > 0 ? out.asset_loss / out.equivalent : 0;
  for (auto& [name, g] : out.by_group) g.equivalent = g.wellbeing / c_bar_marginal;
  return out;
}

struct Annual {
  std::vector<Outcome> events;
  std::vector<double> weights;          // the step weights of the return periods
  double asset_loss = 0, equivalent = 0, resilience = 0, pushed_below = 0;
  std::map<std::string, GroupOutcome> by_group;
  std::vector<double> household_equivalent, household_asset_loss;
};

// Expected annual values over return periods with the step weights p_j = 1/r_j − 1/r_{j+1}, p_m = 1/r_m (P1 p. 13).
inline Annual annual(const std::vector<Household>& H, const Readings& R, std::vector<Event> events, const Support& S = {}, const Options& O = {}) {
  detail::require(!events.empty(), "at least one event");
  std::sort(events.begin(), events.end(), [](const Event& a, const Event& b) { return a.return_period < b.return_period; });
  for (std::size_t j = 0; j < events.size(); j++) {
    detail::require(events[j].return_period >= 1, "a return period of at least one year");
    if (j > 0) detail::require(events[j].return_period > events[j - 1].return_period, "two events share a return period");
  }
  const auto C = calibrate(H, R);
  Annual A;
  A.household_equivalent.assign(H.size(), 0);
  A.household_asset_loss.assign(H.size(), 0);
  for (std::size_t j = 0; j < events.size(); j++) {
    const double p = 1 / events[j].return_period - (j + 1 < events.size() ? 1 / events[j + 1].return_period : 0.0);
    A.weights.push_back(p);
    A.events.push_back(outcome(H, C, R, events[j], S, O));
    const auto& o = A.events.back();
    A.asset_loss += p * o.asset_loss;
    A.equivalent += p * o.equivalent;
    A.pushed_below += p * o.pushed_below;
    for (const auto& [name, g] : o.by_group) {
      auto& G = A.by_group[name];
      G.persons = g.persons;
      G.affected += p * g.affected;
      G.asset_loss += p * g.asset_loss;
      G.wellbeing += p * g.wellbeing;
      G.equivalent += p * g.equivalent;
      G.pushed_below += p * g.pushed_below;
    }
    for (std::size_t h = 0; h < H.size(); h++) {
      A.household_equivalent[h] += p * o.household_equivalent[h];
      A.household_asset_loss[h] += p * o.household_asset_loss[h];
    }
  }
  A.resilience = A.equivalent > 0 ? A.asset_loss / A.equivalent : 0;
  return A;
}

}  // namespace risk_management::wellbeing
