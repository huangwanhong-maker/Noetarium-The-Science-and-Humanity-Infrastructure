// Checks of returns: what is kept along a graph of use adds to the pot on 300 random graphs (Proposition 7), exactly in rationals; a
// worked case; a pot divided by declared weights; an operative's return paid to the party that answers for it.

#include <communities/returns/returns.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <string>

using namespace communities::returns;
using thesmos::kernel::Kind;

namespace {
long long between(calx::random::Stream& r, long long lo, long long hi) { return lo + static_cast<long long>(r.below(static_cast<std::uint64_t>(hi - lo + 1))); }
}  // namespace

int main() {
  calx::verification::Suite s;
  calx::random::Stream rng(2, 6, 0, 0);
  int bad = 0;
  for (int trial = 0; trial < 300; trial++) {
    std::vector<std::string> order;
    for (long long i = 0, n = between(rng, 2, 9); i < n; i++) order.push_back("a" + std::to_string(i));
    std::map<std::string, std::map<std::string, Rational>> passes;
    for (std::size_t k = 1; k < order.size(); k++) {
      const long long sources = between(rng, 0, std::min<long long>(3, static_cast<long long>(k)));
      if (!sources) continue;
      std::vector<std::string> pool(order.begin(), order.begin() + static_cast<long>(k));
      for (std::size_t i = pool.size(); i > 1; i--) std::swap(pool[i - 1], pool[rng.below(i)]);
      const Rational total(between(rng, 1, 9), 10);
      std::vector<long long> w;
      long long sum = 0;
      for (long long j = 0; j < sources; j++) w.push_back(between(rng, 1, 5)), sum += w.back();
      for (long long j = 0; j < sources; j++) passes[order[k]][pool[static_cast<std::size_t>(j)]] = total * Rational(w[static_cast<std::size_t>(j)], sum);
    }
    const auto kept = upstream(order, passes, {{order.back(), Rational(100)}});
    Rational sum(0);
    for (const auto& [name, v] : kept) {
      sum = sum + v;
      bad += v < Rational(0);
    }
    bad += !(sum == Rational(100));
  }
  s.require("what is kept along the graph of use adds exactly to the pot and is never negative (300 random graphs)", bad == 0);
  const auto kept = upstream({"method", "dataset", "translation", "service"},
                             {{"translation", {{"method", Rational(1, 5)}, {"dataset", Rational(1, 5)}}}, {"service", {{"translation", Rational(1, 2)}}}},
                             {{"service", Rational(100)}});
  s.require("a worked case: 100 at a service is kept as 50, 30, 10 and 10",
            kept.at("service") == Rational(50) && kept.at("translation") == Rational(30) && kept.at("method") == Rational(10) && kept.at("dataset") == Rational(10));
  const auto shares = by_shares(Rational(90), {{"reviewer", Rational(1)}, {"author", Rational(2)}});
  s.require("a pot of 90 by declared weights 1 and 2 gives 30 and 60", shares.at("reviewer") == Rational(30) && shares.at("author") == Rational(60));
  thesmos::kernel::Parties ps;
  ps.add({"mei", Kind::person});
  ps.add({"mei/agent", Kind::operative, "mei"});
  ps.add({"lab", Kind::organisation});
  ps.add({"lab/a", Kind::operative, "lab"});
  ps.add({"lab/b", Kind::operative, "lab"});
  const auto paid = to_answerable(ps, {{"mei/agent", Rational(10)}, {"lab/a", Rational(5)}, {"lab/b", Rational(7)}, {"mei", Rational(1)}});
  s.require("returns owed to operatives are paid to the parties that answer for them: 11 to the sponsor, 12 to the organisation",
            paid.size() == 2 && paid.at("mei") == Rational(11) && paid.at("lab") == Rational(12));
  return s.done();
}
