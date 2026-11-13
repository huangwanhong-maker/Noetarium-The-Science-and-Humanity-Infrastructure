// Checks of tuning (tuning.hpp): against exact arithmetic and closed forms. Identities among tempered intervals are decided
// exactly, as equalities of primes to rational exponents; sizes in cents are compared with sizes computed here from the ratios.
//
// - Pythagorean chains: their steps, their pure fifths and the wolf, wherever the chain begins.
// - Sanfen sunyi: the lengths by the rule, the twelve lü equal to the Pythagorean ratios exactly, the thirteenth short of the
//   first by the Pythagorean comma.
// - Just intonation: Ptolemy's intense diatonic and the just scale of twelve notes, their pure and impure fifths.
// - Meantone: the pure intervals of the quarter-, third-, sixth- and 2/7-comma temperaments, the wolf that closes the circle,
//   the errors of fifths and thirds as functions of the fraction.
// - Equal temperaments: 12, 19, 31, 53; their fifths and thirds; divisions as homomorphisms and what they temper out.
// - Circulating temperaments: Werckmeister III and Vallotti close, and the equal temperament is the one with equal fifths.
// - al-Urmawī's seventeen steps, a chain of pure fifths.
// - What holds of every scale of twelve notes; comparison; frequencies.

#include <musicology/tuning/tuning.hpp>

#include <calx/verification/check.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <string>
#include <utility>
#include <vector>

namespace mp = musicology::pitch;
namespace mt = musicology::tuning;
using mp::Interval;
using mp::Ratio;

namespace {

template <class F>
bool refuses(F&& f) {
  try {
    f();
  } catch (const std::exception&) {
    return true;
  }
  return false;
}

// The composition of the exact intervals of some dyads.
Interval product(const std::vector<mt::Dyad>& ds) {
  Interval x;
  for (const auto& d : ds) x = x * *d.exact;
  return x;
}
// How many dyads are exactly a given interval.
std::size_t count(const std::vector<mt::Dyad>& ds, const Interval& x) {
  return static_cast<std::size_t>(std::count_if(ds.begin(), ds.end(), [&](const mt::Dyad& d) { return d.exact && *d.exact == x; }));
}
std::size_t count_pure(const std::vector<mt::Dyad>& ds) {
  return static_cast<std::size_t>(std::count_if(ds.begin(), ds.end(), [](const mt::Dyad& d) { return d.pure; }));
}
// Whether two tunings have exactly the same degrees.
bool same(const mt::Tuning& a, const mt::Tuning& b) {
  if (a.size() != b.size()) return false;
  for (std::size_t i = 0; i < a.size(); i++)
    if (!a.degrees[i].exact || !b.degrees[i].exact || !(*a.degrees[i].exact == *b.degrees[i].exact)) return false;
  return true;
}
bool has_ratios(const mt::Tuning& t, const std::vector<Ratio>& ratios) {
  if (t.size() != ratios.size()) return false;
  for (std::size_t i = 0; i < ratios.size(); i++)
    if (!(t.degrees[i].ratio == ratios[i])) return false;
  return true;
}
double sum_departure(const std::vector<mt::Dyad>& ds) {
  double sum = 0;
  for (const auto& d : ds) sum += d.departure;
  return sum;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  const Ratio P8 = mp::octave(), P5 = mp::fifth(), P4 = mp::fourth(), M3 = mp::major_third(), m3 = mp::minor_third();
  const Ratio pc = mp::pythagorean_comma(), sc = mp::syntonic_comma();
  const Interval I5(P5), IM3(M3), Im3(m3), Ipc(pc), Isc(sc), seven_octaves = mp::stack(Interval(P8), 7);
  const double kappa = mp::cents(sc), kappa_p = mp::cents(pc), pure5 = mp::cents(P5);

  // ---- Pythagorean chains
  const mt::Tuning pyth = mt::pythagorean(12, 3);
  {
    const auto fs = mt::fifths(pyth);
    const mt::Dyad w = mt::wolf(pyth);
    s.require("Pythagorean tuning of twelve notes: eleven pure fifths and a wolf, the fifth less the Pythagorean comma, 262144/177147",
              pyth.size() == 12 && count_pure(fs) == 11 && *w.exact == I5 / Ipc && *w.exact == Interval(mp::ratio(262144, 177147)), w.exact->str());
    s.check("the wolf is narrow by the Pythagorean comma's cents", w.departure + kappa_p, 1e-12, "cents");
    s.require("with three fifths below the first degree the wolf stands from G♯ to E♭; with five below, from F♯ to D♭; with none, from E♯ to C",
              pyth.degrees[w.from].name == "G♯" && pyth.degrees[w.to].name == "E♭" && mt::pythagorean(12, 5).degrees[mt::wolf(mt::pythagorean(12, 5)).from].name == "F♯" &&
                  mt::pythagorean(12, 5).degrees[mt::wolf(mt::pythagorean(12, 5)).to].name == "D♭" && mt::pythagorean(12, 0).degrees[mt::wolf(mt::pythagorean(12, 0)).from].name == "E♯" &&
                  mt::wolf(mt::pythagorean(12, 0)).to == 0,
              pyth.degrees[w.from].name + " to " + pyth.degrees[w.to].name);
    const auto circle = mt::circle_of_fifths(pyth);
    s.require("its circle of fifths passes through all twelve degrees and composes to seven octaves exactly", circle.size() == 12 && product(circle) == seven_octaves);
    bool only23 = true;
    for (const auto& d : pyth.degrees) only23 = only23 && d.ratio && d.exact->factors().size() <= 2 && d.exact->exponent(5).is_zero() && d.form == d.exact->str();
    s.require("every degree is a ratio of powers of 2 and 3, written as it is", only23, pyth.degrees[1].form);
    const auto st = mt::steps(pyth);
    s.require("its steps are seven limmas and five apotomes, which compose to the octave",
              count(st, Interval(mp::limma())) == 7 && count(st, Interval(mp::apotome())) == 5 && product(st) == Interval(P8));
  }
  {
    const mt::Tuning diatonic = mt::pythagorean(7, 1), penta = mt::pythagorean(5, 0);
    const auto st = mt::steps(diatonic);
    s.require("seven notes of the chain, one below the first: 1, 9/8, 81/64, 4/3, 3/2, 27/16, 243/128, five major tones and two limmas",
              has_ratios(diatonic, {Ratio(1), mp::major_tone(), mp::ditone(), P4, P5, mp::ratio(27, 16), mp::ratio(243, 128)}) && count(st, Interval(mp::major_tone())) == 5 &&
                  count(st, Interval(mp::limma())) == 2);
    s.require("five notes of the chain: 1, 9/8, 81/64, 3/2, 27/16, a pentatonic scale of major tones and minor thirds of 32/27",
              has_ratios(penta, {Ratio(1), mp::major_tone(), mp::ditone(), P5, mp::ratio(27, 16)}) && count(mt::steps(penta), Interval(mp::major_tone())) == 3 &&
                  count(mt::steps(penta), Interval(mp::ratio(32, 27))) == 2);
    bool never = true;
    for (std::size_t n = 2; n <= 60; n++) {
      const auto circle = mt::circle_of_fifths(mt::pythagorean(n, 0));
      never = never && count_pure(circle) < circle.size();
    }
    s.require("a chain of pure fifths of two to sixty notes never closes: no circle of fifths in it is all pure", never);
    bool one = true;
    for (std::size_t n : {5, 7, 12, 17, 29, 41, 53}) {
      const auto circle = mt::circle_of_fifths(mt::pythagorean(n, 0));
      one = one && circle.size() == n && count_pure(circle) == n - 1;
    }
    s.require("in chains of 5, 7, 12, 17, 29, 41 and 53 notes the circle passes through every note, and all its fifths but one are pure", one);
  }

  // ---- 三分损益
  {
    const auto lengths = mt::sanfen_sunyi_lengths(12);
    bool rule = true;
    std::size_t shortened = 0, doubled = 0;
    for (std::size_t i = 1; i < lengths.size(); i++) {
      const Ratio q = lengths[i] / lengths[i - 1];
      rule = rule && (q == Ratio(2, 3) || q == Ratio(4, 3)) && lengths[i] > Ratio(1, 2) && !(lengths[i] > Ratio(1));
      shortened += q == Ratio(2, 3);
      if (i >= 2 && q == lengths[i - 1] / lengths[i - 2]) doubled++;
    }
    s.require("sanfen sunyi: each length is two thirds or four thirds of the last, all within the octave; five shortenings, six lengthenings, the two alternating except once",
              rule && shortened == 5 && doubled == 1 && lengths[7] / lengths[6] == Ratio(4, 3) && lengths[6] / lengths[5] == Ratio(4, 3));
    bool whole = true;
    for (std::size_t i = 0; i < 5; i++) whole = whole && (lengths[i] * Ratio(81)).is_integer();
    s.require("on a first length of 81 the first five are whole, 81, 54, 72, 48, 64, and the sixth is not",
              whole && lengths[1] * Ratio(81) == Ratio(54) && lengths[2] * Ratio(81) == Ratio(72) && lengths[3] * Ratio(81) == Ratio(48) && lengths[4] * Ratio(81) == Ratio(64) &&
                  !(lengths[5] * Ratio(81)).is_integer());
    s.require("the twelfth length is 2^17 / 3^11 of the first", lengths[11] == mp::ratio(131072, 177147));
    const auto thirteen = mt::sanfen_sunyi_lengths(13);
    s.require("a thirteenth length falls short of the first by the Pythagorean comma exactly", thirteen[12] == mp::inverse(pc), thirteen[12].str());
  }
  {
    const mt::Tuning lu = mt::sanfen_sunyi(12);
    s.require("the twelve lü are the Pythagorean tuning of twelve notes with none below the first, exactly", same(lu, mt::pythagorean(12, 0)));
    s.require("five lü are the Pythagorean pentatonic, and sixty are the chain of sixty fifths", same(mt::sanfen_sunyi(5), mt::pythagorean(5, 0)) && same(mt::sanfen_sunyi(60), mt::pythagorean(60, 0)));
    s.require("in ascending order the lü run Huangzhong, Dalü, Taicu, …, Yingzhong; Linzhong, the first generated, is a pure fifth above Huangzhong",
              lu.degrees[0].name == "Huangzhong" && lu.degrees[1].name == "Dalü" && lu.degrees[2].name == "Taicu" && lu.degrees[11].name == "Yingzhong" && lu.degrees[7].name == "Linzhong" &&
                  lu.degrees[7].ratio == P5,
              lu.degrees[7].name + " " + lu.degrees[7].form);
  }

  // ---- Just intonation
  const mt::Tuning ptolemy = mt::ptolemy_intense_diatonic();
  {
    s.require("Ptolemy's intense diatonic is 1, 9/8, 5/4, 4/3, 3/2, 5/3, 15/8", has_ratios(ptolemy, {Ratio(1), mp::major_tone(), M3, P4, P5, mp::major_sixth(), mp::ratio(15, 8)}));
    const auto st = mt::steps(ptolemy);
    const Interval S(mp::diatonic_semitone()), T(mp::major_tone()), t(mp::minor_tone());
    s.require("from its third degree it is two tetrachords of 16/15, 9/8, 10/9 rising, a major tone apart",
              *st[2].exact == S && *st[3].exact == T && *st[4].exact == t && *st[5].exact == T && *st[6].exact == S && *st[0].exact == T && *st[1].exact == t &&
                  *st[2].exact * *st[3].exact * *st[4].exact == Interval(P4));
    const auto fs = mt::fifths(ptolemy), M = mt::major_thirds(ptolemy), m = mt::minor_thirds(ptolemy);
    s.require("its triads on the first, fourth and fifth degrees are pure: thirds of 5/4 and 6/5, fifths of 3/2",
              M[0].pure && M[3].pure && M[4].pure && fs[0].pure && fs[3].pure && fs[4].pure && *mt::dyad(ptolemy, 2, 2, m3).exact == Im3 && *mt::dyad(ptolemy, 5, 2, m3).exact == Im3 &&
                  *mt::dyad(ptolemy, 6, 2, m3).exact == Im3);
    s.require("its fifth on the second degree is 40/27, narrow by the syntonic comma exactly, and its minor third there 32/27", *fs[1].exact == I5 / Isc && *fs[1].exact == Interval(mp::ratio(40, 27)) && *m[1].exact == Interval(mp::ratio(32, 27)));
    s.check("that fifth's departure is the comma's cents", fs[1].departure + kappa, 1e-12, "cents");
    const auto d = mt::compare(mt::pythagorean(7, 1), ptolemy);
    s.check("against the Pythagorean diatonic its third, sixth and seventh degrees are a syntonic comma lower, the rest the same",
            std::fabs(d[2] + kappa) + std::fabs(d[5] + kappa) + std::fabs(d[6] + kappa) + std::fabs(d[0]) + std::fabs(d[1]) + std::fabs(d[3]) + std::fabs(d[4]), 1e-12, "cents");
  }
  const mt::Tuning just12 = mt::just_chromatic();
  {
    s.require("the just scale of twelve notes is 1, 16/15, 9/8, 6/5, 5/4, 4/3, 45/32, 3/2, 8/5, 5/3, 9/5, 15/8",
              has_ratios(just12, {Ratio(1), mp::ratio(16, 15), mp::ratio(9, 8), m3, M3, P4, mp::ratio(45, 32), P5, mp::ratio(8, 5), mp::ratio(5, 3), mp::ratio(9, 5), mp::ratio(15, 8)}));
    bool holds = true;
    for (const auto& d : ptolemy.degrees) holds = holds && std::any_of(just12.degrees.begin(), just12.degrees.end(), [&](const mt::Degree& e) { return *e.exact == *d.exact; });
    s.require("it holds Ptolemy's intense diatonic, and its pure third above the first degree is named E(-1)", holds && just12.degrees[4].name == "E(-1)" && just12.degrees[8].name == "A♭(+1)", just12.degrees[4].name);
    const auto fs = mt::fifths(just12);
    s.require("nine of its fifths are pure, two narrow by the syntonic comma, and one wide by the diaschisma",
              count_pure(fs) == 9 && count(fs, I5 / Isc) == 2 && count(fs, I5 * Interval(mp::diaschisma())) == 1 && product(mt::circle_of_fifths(just12)) == seven_octaves);
  }

  // ---- Meantone
  const mt::Tuning quarter = mt::meantone(Ratio(1, 4));
  {
    const Interval mean = Interval::prime(5, Ratio(1, 4));
    const auto fs = mt::fifths(quarter);
    const mt::Dyad w = mt::wolf(quarter);
    s.require("quarter-comma meantone: eleven fifths of 5^(1/4), and the wolf 2^7 · 5^(−11/4) that closes the circle",
              mt::meantone_fifth(Ratio(1, 4)) == mean && count(fs, mean) == 11 && *w.exact == seven_octaves / mp::stack(mean, 11) &&
                  *w.exact == Interval(Ratio(128)) * Interval::prime(5, Ratio(-11, 4)) && product(mt::circle_of_fifths(quarter)) == seven_octaves && quarter.degrees[w.from].name == "G♯" &&
                  quarter.degrees[w.to].name == "E♭",
              w.exact->str());
    s.check("the wolf is wide by eleven quarters of the syntonic comma less the Pythagorean comma", w.departure - (11 * kappa / 4 - kappa_p), 1e-10, "cents");
    s.require("which is more than 35 cents and less than 36", w.departure > 35 && w.departure < 36, std::to_string(w.departure) + " cents");
    const auto M = mt::major_thirds(quarter);
    s.require("its major third is exactly 5/4: eight pure major thirds, and four of 32/25, wide by the lesser diesis",
              count_pure(M) == 8 && count(M, Interval(mp::ratio(32, 25))) == 4 && mp::ratio(32, 25) == mp::compose(M3, mp::lesser_diesis()) && quarter.degrees[4].ratio == M3);
    const Interval tone = *mt::dyad(quarter, 0, 2, mp::major_tone()).exact;
    s.require("its tone is the mean of the major third: two of them are 5/4, and it is √5 / 2", mp::stack(tone, 2) == IM3 && tone == Interval::prime(5, Ratio(1, 2)) / Interval(P8), tone.str());
    s.require("its degrees carry their exact forms and no ratio where there is none: the fifth is 5^(1/4), the major third 5/4",
              quarter.degrees[7].form == "5^(1/4)" && !quarter.degrees[7].ratio && quarter.degrees[4].form == "5/4" && quarter.degrees[7].name == "G");
  }
  {
    const mt::Tuning third = mt::meantone(Ratio(1, 3)), sixth = mt::meantone(Ratio(1, 6)), zarlino = mt::meantone(Ratio(2, 7));
    s.require("third-comma meantone: nine pure minor thirds, 6/5, and its major sixth is 5/3", count_pure(mt::minor_thirds(third)) == 9 && third.degrees[9].ratio == mp::major_sixth());
    s.require("sixth-comma meantone: its augmented fourth, six fifths up, is 45/32 exactly", sixth.degrees[6].ratio == mp::ratio(45, 32) && sixth.degrees[6].name == "F♯");
    s.require("2/7-comma meantone: its chromatic semitone, seven fifths up, is 25/24 exactly", zarlino.degrees[1].ratio == mp::chromatic_semitone() && zarlino.degrees[1].name == "C♯");
    s.require("meantone by no part of the comma is Pythagorean tuning", same(mt::meantone(Ratio(0)), pyth));
  }
  {
    double worst = 0;
    for (const Ratio& f : {Ratio(1, 3), Ratio(2, 7), Ratio(1, 4), Ratio(1, 5), Ratio(1, 6), Ratio(1, 11), Ratio(0)}) {
      const mt::Tuning t = mt::meantone(f);
      const double x = f.to_double();
      worst = std::max(worst, std::fabs(mt::fifths(t)[0].departure + x * kappa));
      worst = std::max(worst, std::fabs(mt::major_thirds(t)[0].departure - (1 - 4 * x) * kappa));
      worst = std::max(worst, std::fabs(mt::dyad(t, 9, 3, m3).departure - (3 * x - 1) * kappa));
      worst = std::max(worst, std::fabs(mt::dyad(t, 8, 7, P5).departure - (11 * x * kappa - kappa_p)));   // from G♯ to E♭
    }
    s.check("for a fraction f of the comma κ the fifth departs by −f κ, the major third by (1 − 4f) κ, the minor third by (3f − 1) κ, the wolf by 11 f κ less the Pythagorean comma", worst, 1e-10, "cents");
  }
  const mt::Tuning edo12 = mt::equal(12);
  {
    const auto d = mt::compare(edo12, mt::meantone(Ratio(1, 11)));
    double worst = 0;
    for (double x : d) worst = std::max(worst, std::fabs(x));
    s.check("1/11-comma meantone and the equal temperament of twelve notes agree on every degree", worst, 0.002, "cents");
    s.check("twelve of its fifths miss seven octaves by the schisma less an eleventh of the syntonic comma", 12 * (mt::fifths(mt::meantone(Ratio(1, 11)))[0].cents - 700.0) - (mp::cents(mp::schisma()) - kappa / 11), 1e-9, "cents");
    s.check("the fifth of 31 equal divisions is within 0.2 cents of quarter-comma meantone's", mt::approximation(31, P5).cents - mt::meantone_fifth(Ratio(1, 4)).cents(), 0.2, "cents");
    s.check("the fifth of 19 equal divisions is within 0.06 cents of third-comma meantone's", mt::approximation(19, P5).cents - mt::meantone_fifth(Ratio(1, 3)).cents(), 0.06, "cents");
  }

  // ---- Equal temperaments
  {
    bool exact = true;
    double worst = 0;
    for (std::size_t k = 0; k < 12; k++) {
      exact = exact && *edo12.degrees[k].exact == Interval::prime(2, Ratio(static_cast<long long>(k), 12)) && edo12.degrees[k].ratio.has_value() == (k == 0);
      worst = std::max(worst, std::fabs(edo12.degrees[k].cents - 100.0 * static_cast<double>(k)));
    }
    s.require("twelve equal divisions: the degrees are the powers of 2^(1/12), none but the first a ratio", exact && edo12.degrees[7].form == "2^(7/12)", edo12.degrees[7].form);
    s.check("its degrees are whole hundreds of cents exactly", worst, 0.0, "cents");
    const auto fs = mt::fifths(edo12);
    s.require("its twelve fifths are one interval, 2^(7/12), and the circle closes", count(fs, Interval::prime(2, Ratio(7, 12))) == 12 && product(mt::circle_of_fifths(edo12)) == seven_octaves);
    s.check("its fifth is 700 cents exactly", fs[0].cents - 700.0, 0.0, "cents");
    s.check("narrow by a twelfth of the Pythagorean comma", fs[0].departure + kappa_p / 12, 1e-11, "cents");
    s.check("its major third is 400 cents, wide by a third of the lesser diesis", std::fabs(mt::major_thirds(edo12)[0].cents - 400.0) + std::fabs(mt::major_thirds(edo12)[0].departure - mp::cents(mp::lesser_diesis()) / 3), 1e-11, "cents");
    s.check("2^(1/12) is the cube root of the square root of the square root of 2", mp::frequency(1.0, *edo12.degrees[1].exact) - std::cbrt(std::sqrt(std::sqrt(2.0))), 1e-14);
  }
  {
    const mt::Approximation a12 = mt::approximation(12, P5), a19 = mt::approximation(19, P5), a31 = mt::approximation(31, P5), a53 = mt::approximation(53, P5);
    s.require("the fifth is 7 steps of 12, 11 of 19, 18 of 31, 31 of 53; the major third 4, 6, 10 and 17",
              a12.steps == 7 && a19.steps == 11 && a31.steps == 18 && a53.steps == 31 && mt::approximation(12, M3).steps == 4 && mt::approximation(19, M3).steps == 6 &&
                  mt::approximation(31, M3).steps == 10 && mt::approximation(53, M3).steps == 17);
    s.check("the fifth of 53 equal divisions is within 0.07 cents of pure", a53.error, 0.07, "cents");
    s.check("and is 31 · 1200 / 53 cents", a53.cents - 31.0 * 1200.0 / 53.0, 1e-12, "cents");
    s.check("its major third, 17 steps, is within 1.5 cents of pure", mt::approximation(53, M3).error, 1.5, "cents");
    s.check("the major third of 31 equal divisions is within 0.8 cents of pure", mt::approximation(31, M3).error, 0.8, "cents");
    const mt::Tuning edo53 = mt::equal(53);
    const auto fs = mt::fifths(edo53);
    s.require("53 equal divisions as a tuning: 53 degrees, every fifth 31 steps, 2^(31/53)", edo53.size() == 53 && mt::steps_nearest(edo53, P5) == 31 && count(fs, Interval::prime(2, Ratio(31, 53))) == 53);
    s.check("the approximation's error is the tuning's departure", fs[0].departure - a53.error, 1e-10, "cents");
  }
  {
    const auto best = mt::best_divisions(P5, 60);
    // By brute force, without the function: the divisions whose nearest step beats every smaller division's.
    std::vector<long long> brute;
    double least = 1e300;
    for (long long n = 1; n <= 60; n++) {
      double e = 1e300;
      for (long long k = 0; k <= n; k++) e = std::min(e, std::fabs(1200.0 * static_cast<double>(k) / static_cast<double>(n) - pure5));
      if (e < least) least = e, brute.push_back(n);
    }
    std::vector<long long> found;
    for (const auto& a : best) found.push_back(a.divisions);
    s.require("the divisions up to 60 that come nearer the fifth than every smaller one are 1, 2, 3, 5, 7, 12, 29, 41, 53, as brute force finds",
              found == brute && found == std::vector<long long>{1, 2, 3, 5, 7, 12, 29, 41, 53});
  }
  {
    const auto v12 = mt::val(12), v19 = mt::val(19), v31 = mt::val(31), v53 = mt::val(53);
    s.require("as homomorphisms of the 5-limit, 12, 19, 31 and 53 divisions send (2, 3, 5) to (12, 19, 28), (19, 30, 44), (31, 49, 72), (53, 84, 123)",
              v12 == std::array<long long, 3>{12, 19, 28} && v19 == std::array<long long, 3>{19, 30, 44} && v31 == std::array<long long, 3>{31, 49, 72} && v53 == std::array<long long, 3>{53, 84, 123});
    s.require("the syntonic comma is tempered out by 12, 19 and 31 divisions and is one step of 53", mt::image(v12, sc) == 0 && mt::image(v19, sc) == 0 && mt::image(v31, sc) == 0 && mt::image(v53, sc) == 1);
    s.require("the Pythagorean comma is tempered out by 12 divisions, is one step of 53, and one step down in 19 and 31", mt::image(v12, pc) == 0 && mt::image(v53, pc) == 1 && mt::image(v19, pc) == -1 && mt::image(v31, pc) == -1);
    s.require("the schisma is tempered out by 12 and 53 divisions; the lesser diesis by 12, and is one step of 19 and of 31",
              mt::image(v12, mp::schisma()) == 0 && mt::image(v53, mp::schisma()) == 0 && mt::image(v12, mp::lesser_diesis()) == 0 && mt::image(v19, mp::lesser_diesis()) == 1 &&
                  mt::image(v31, mp::lesser_diesis()) == 1);
    s.require("the image of a ratio is its steps: the fifth 7 of 12, the major third 4, the octave 12; a ratio beyond the 5-limit is refused",
              mt::image(v12, P5) == 7 && mt::image(v12, M3) == 4 && mt::image(v12, P8) == 12 && mt::image(v53, P5) == 31 && refuses([&] { mt::image(v12, mp::ratio(7, 4)); }));
    // The kernel of (12, 19, 28) is generated by 81/80 and 128/125: the cross product of their exponents is the val, up to sign.
    const Interval a(sc), b(mp::lesser_diesis());
    const auto e = [](const Interval& x, long long p) { return x.exponent(p).numerator().to_int64(); };
    const std::array<long long, 3> cross{e(a, 3) * e(b, 5) - e(a, 5) * e(b, 3), e(a, 5) * e(b, 2) - e(a, 2) * e(b, 5), e(a, 2) * e(b, 3) - e(a, 3) * e(b, 2)};
    s.require("the syntonic comma and the lesser diesis generate all that twelve divisions temper out: the cross product of their exponents is −(12, 19, 28)",
              cross == std::array<long long, 3>{-12, -19, -28});
    // Meantone: 2 ↦ (1, 0), 3 ↦ (1, 1), 5 ↦ (0, 4), in octaves and fifths.
    const std::array<long long, 3> in_octaves{1, 1, 0}, in_fifths{0, 1, 4};
    s.require("meantone counts octaves and fifths: the fifth is (0, 1), the major third (−2, 4), the minor third (2, −3), and the syntonic comma (0, 0)",
              mt::image(in_octaves, P5) == 0 && mt::image(in_fifths, P5) == 1 && mt::image(in_octaves, M3) == -2 && mt::image(in_fifths, M3) == 4 && mt::image(in_octaves, m3) == 2 &&
                  mt::image(in_fifths, m3) == -3 && mt::image(in_octaves, sc) == 0 && mt::image(in_fifths, sc) == 0 && (mt::image(in_octaves, pc) != 0 || mt::image(in_fifths, pc) != 0));
    bool factors = true;
    for (const auto& [n, f] : std::vector<std::pair<long long, long long>>{{12, 7}, {19, 11}, {31, 18}}) {
      const auto v = mt::val(n);
      for (std::size_t i = 0; i < 3; i++) factors = factors && v[i] == n * in_octaves[i] + f * in_fifths[i];
    }
    s.require("12, 19 and 31 divisions are meantone temperaments: each is the meantone map followed by n steps to the octave and 7, 11 or 18 to the fifth; 53 is not",
              factors && v53[2] != 4 * (v53[1] - v53[0]));
  }

  // ---- Temperaments that circulate
  const mt::Tuning werck = mt::werckmeister_iii(), vallotti = mt::vallotti();
  {
    const auto fs = mt::fifths(werck);
    const Interval narrow = I5 / Ipc.power(Ratio(1, 4));
    s.require("Werckmeister III: eight pure fifths and four narrow by a quarter of the Pythagorean comma, on C, G, D and B; the circle closes exactly",
              count_pure(fs) == 8 && count(fs, narrow) == 4 && *fs[0].exact == narrow && *fs[7].exact == narrow && *fs[2].exact == narrow && *fs[11].exact == narrow &&
                  werck.degrees[7].name == "G" && werck.degrees[2].name == "D" && werck.degrees[11].name == "B" && product(mt::circle_of_fifths(werck)) == seven_octaves);
    s.check("its major third on the first degree is wide by the syntonic comma less three quarters of the Pythagorean", mt::major_thirds(werck)[0].departure - (kappa - 3 * kappa_p / 4), 1e-10, "cents");
  }
  {
    const auto fs = mt::fifths(vallotti);
    const Interval narrow = I5 / Ipc.power(Ratio(1, 6));
    s.require("Vallotti: six pure fifths and six narrow by a sixth of the Pythagorean comma, on F, C, G, D, A and E; the circle closes exactly",
              count_pure(fs) == 6 && count(fs, narrow) == 6 && *fs[5].exact == narrow && *fs[0].exact == narrow && *fs[7].exact == narrow && *fs[2].exact == narrow && *fs[9].exact == narrow &&
                  *fs[4].exact == narrow && vallotti.degrees[5].name == "F" && product(mt::circle_of_fifths(vallotti)) == seven_octaves);
    const auto M = mt::major_thirds(vallotti);
    double least = 1e300, most = -1e300;
    for (const auto& d : M) least = std::min(least, d.departure), most = std::max(most, d.departure);
    s.require("its major thirds run from the ditone less two thirds of the Pythagorean comma, on F, C and G, to the ditone itself",
              *M[0].exact == Interval(mp::ditone()) / Ipc.power(Ratio(2, 3)) && *M[5].exact == *M[0].exact && *M[7].exact == *M[0].exact && count(M, Interval(mp::ditone())) == 3);
    s.check("so that the widest is wide by the syntonic comma, and the narrowest by that less two thirds of the Pythagorean", std::fabs(most - kappa) + std::fabs(least - (kappa - 2 * kappa_p / 3)), 1e-10, "cents");
  }
  {
    const std::vector<Ratio> twelfths(12, Ratio(1, 12));
    std::vector<Ratio> open(12, Ratio(0));
    open[0] = Ratio(1, 2);
    s.require("the well temperament whose twelve fifths are each narrowed by a twelfth of the Pythagorean comma is the equal temperament, exactly", same(mt::well_temperament("equal", twelfths), edo12));
    s.require("fractions that do not sum to the whole comma are refused, as are fifths that do not close", refuses([&] { mt::well_temperament("open", open); }) &&
                                                                                                          refuses([&] { mt::circulating("pure", std::vector<Interval>(12, I5)); }));
    std::vector<Ratio> wolf(12, Ratio(0));
    wolf[8] = Ratio(1);
    s.require("the whole comma on the fifth from G♯ gives Pythagorean tuning with its wolf there", same(mt::well_temperament("wolf", wolf), pyth));
  }

  // ---- al-Urmawī's seventeen steps
  {
    const mt::Tuning urmawi = mt::urmawi();
    const auto st = mt::steps(urmawi);
    s.require("al-Urmawī's division: seventeen steps, twelve limmas and five Pythagorean commas, which compose to the octave exactly",
              urmawi.size() == 17 && count(st, Interval(mp::limma())) == 12 && count(st, Ipc) == 5 && product(st) == Interval(P8) &&
                  mp::compose(mp::stack(mp::limma(), 12), mp::stack(pc, 5)) == P8);
    s.require("its notes are a chain of seventeen pure fifths, twelve below the first degree and four above", same(urmawi, mt::pythagorean(17, 12)));
    s.require("its tone of two limmas and a comma is 9/8; its eighth degree is the fourth, its eleventh the fifth", urmawi.degrees[3].ratio == mp::major_tone() && urmawi.degrees[7].ratio == P4 && urmawi.degrees[10].ratio == P5);
    s.require("its sixth degree, 8192/6561, is a schisma below the major third 5/4", urmawi.degrees[5].ratio == mp::ratio(8192, 6561) && mp::between(*urmawi.degrees[5].ratio, M3) == mp::schisma());
    const auto fs = mt::fifths(urmawi);
    s.require("of its seventeen fifths of ten steps sixteen are pure, and one is wide by a limma less a comma",
              mt::steps_nearest(urmawi, P5) == 10 && count_pure(fs) == 16 && count(fs, I5 * Interval(mp::limma()) / Ipc) == 1 && product(mt::circle_of_fifths(urmawi)) == mp::stack(Interval(P8), 10));
  }

  // ---- What holds of every scale of twelve notes
  {
    double worst5 = 0, worst3 = 0, worstm = 0;
    for (const mt::Tuning& t : {pyth, mt::pythagorean(12, 0), just12, quarter, mt::meantone(Ratio(1, 6)), edo12, werck, vallotti}) {
      const auto M = mt::major_thirds(t), m = mt::minor_thirds(t);
      worst5 = std::max(worst5, std::fabs(sum_departure(mt::fifths(t)) + kappa_p));
      for (std::size_t i = 0; i < 4; i++) worst3 = std::max(worst3, std::fabs(M[i].departure + M[i + 4].departure + M[i + 8].departure - mp::cents(mp::lesser_diesis())));
      for (std::size_t i = 0; i < 3; i++) worstm = std::max(worstm, std::fabs(m[i].departure + m[i + 3].departure + m[i + 6].departure + m[i + 9].departure + mp::cents(mp::greater_diesis())));
    }
    s.check("in every scale of twelve notes the twelve fifths are narrow by the Pythagorean comma in all", worst5, 1e-9, "cents");
    s.check("three major thirds that fill an octave are wide by the lesser diesis in all", worst3, 1e-9, "cents");
    s.check("four minor thirds that fill an octave are narrow by the greater diesis in all", worstm, 1e-9, "cents");
  }

  // ---- Comparison
  {
    auto d = mt::compare(edo12, pyth);
    std::sort(d.begin(), d.end());
    double worst = 0;
    for (int k = -3; k <= 8; k++) worst = std::max(worst, std::fabs(d[static_cast<std::size_t>(k + 3)] - k * kappa_p / 12));
    s.check("against the equal temperament, the Pythagorean note k fifths along the chain stands k twelfths of the Pythagorean comma away, k from −3 to 8", worst, 1e-10, "cents");
    const auto self = mt::compare(quarter, quarter);
    s.require("a tuning compared with itself differs nowhere; tunings of different sizes are not compared",
              std::all_of(self.begin(), self.end(), [](double x) { return x == 0; }) && refuses([&] { mt::compare(edo12, ptolemy); }));
  }

  // ---- A scale known by measurement
  {
    const mt::Tuning five = mt::scale("a measured scale of five notes", {mt::measured(0, "1"), mt::measured(240.5), mt::measured(478), mt::measured(721.5), mt::measured(958)});
    const auto fs = mt::fifths(five);
    s.require("a scale given in cents alone has no exact intervals, and its fifths are measured in cents", !five.degrees[1].exact && five.degrees[1].form.empty() && !fs[0].exact && !fs[0].pure && mt::steps_nearest(five, P5) == 3);
    s.check("its fifth on the first degree is 721.5 cents, and on the fourth it passes the octave", std::fabs(fs[0].cents - 721.5) + std::fabs(fs[3].cents - (1200 + 240.5 - 721.5)) + std::fabs(fs[0].departure - (721.5 - pure5)), 1e-12, "cents");
    s.require("degrees out of order, beyond the octave, or not begun at the unison are refused",
              refuses([] { mt::scale("x", {mt::measured(0), mt::measured(300), mt::measured(200)}); }) && refuses([] { mt::scale("x", {mt::measured(0), mt::measured(1200)}); }) &&
                  refuses([] { mt::scale("x", {mt::measured(10), mt::measured(300)}); }) && refuses([&] { mt::from_intervals("x", {I5, IM3}); }) &&
                  refuses([&] { mt::from_intervals("x", {Interval(), I5, I5 * Interval(P8)}); }) && refuses([&] { mt::from_steps("x", {I5, I5}); }));
  }

  // ---- Frequencies
  {
    const double c = mt::first_degree(edo12, 440.0, 9);
    s.check("twelve equal divisions with the tenth degree at 440 Hz: the first degree is 440 / 2^(9/12) Hz", c - 440.0 / std::pow(2.0, 9.0 / 12.0), 1e-12, "Hz");
    s.check("then index 9 sounds 440 Hz, index 21 sounds 880, index −3 sounds 220, and index 0 the first degree",
            std::fabs(mt::frequency(edo12, c, 9) - 440.0) + std::fabs(mt::frequency(edo12, c, 21) - 880.0) + std::fabs(mt::frequency(edo12, c, -3) - 220.0) + std::fabs(mt::frequency(edo12, c, 0) - c), 1e-12, "Hz");
    const auto tones = mt::frequencies(ptolemy, 264.0, 0, 7);
    const std::vector<double> hz{264, 297, 330, 352, 396, 440, 495, 528};
    double worst = 0;
    for (std::size_t i = 0; i < hz.size(); i++) worst = std::max(worst, std::fabs(tones[i].hz - hz[i]));
    s.check("Ptolemy's intense diatonic on 264 Hz sounds 264, 297, 330, 352, 396, 440, 495 and 528 Hz, each 264 times its ratio", worst, 1e-12, "Hz");
    const auto range = mt::frequencies(quarter, 100.0, -12, 24);
    bool ascending = true, doubling = true, indexed = true;
    for (std::size_t i = 0; i < range.size(); i++) {
      if (i > 0) ascending = ascending && range[i].hz > range[i - 1].hz && range[i].cents > range[i - 1].cents;
      if (i >= 12) doubling = doubling && range[i].hz == 2 * range[i - 12].hz && range[i].degree == range[i - 12].degree && range[i].octave == range[i - 12].octave + 1;
      indexed = indexed && range[i].index == static_cast<long long>(i) - 12 && std::fabs(range[i].hz - 100.0 * std::exp2(range[i].cents / 1200.0)) < 1e-10;
    }
    s.require("the tones of a tuning over three octaves ascend, double at each octave exactly, and each is the reference raised by its cents",
              range.size() == 37 && ascending && doubling && indexed && range.front().octave == -1 && range.front().degree == 0 && range.back().octave == 2);
    s.check("the fifth of quarter-comma meantone on 100 Hz sounds 100 · 5^(1/4) Hz", mt::frequency(quarter, 100.0, 7) - 100.0 * std::pow(5.0, 0.25), 1e-12, "Hz");
    const mp::Beat b = mp::beats(mt::frequency(quarter, 100.0, 0), mt::frequency(quarter, 100.0, 4), M3), b5 = mp::beats(mt::frequency(quarter, 100.0, 0), mt::frequency(quarter, 100.0, 7), P5);
    s.check("its major third does not beat, and its fifth beats |2 · 5^(1/4) − 3| · 100 times a second", std::fabs(b.rate_hz) + std::fabs(b5.rate_hz - 100.0 * std::fabs(2 * std::pow(5.0, 0.25) - 3)), 1e-11, "Hz");
    s.require("a reference that is not positive is refused", refuses([&] { mt::frequency(edo12, 0.0, 3); }) && refuses([&] { mt::first_degree(edo12, -440.0, 9); }));
  }

  return s.done();
}
