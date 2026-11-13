// Tuning systems: scales of degrees within the octave, how they are built, and how far they stand from the just intervals. The
// domain's theory note, §3–§6.
//
// A tuning is an ordered scale of degrees above its first, repeating at the octave. A degree built here is exact, an interval
// of pitch.hpp (a ratio, or primes to rational exponents); it carries its ratio when it has one, its size in cents, and its
// exact form in writing. A degree known only by measurement carries its cents alone.
//
// The constructors, each with its source:
// - pythagorean: a chain of pure fifths, of any length, begun anywhere in the chain (Barbour 1951, the chapter "Greek Tunings";
//   Lindley, "Pythagorean intonation", The New Grove Dictionary of Music and Musicians, 2nd ed. 2001);
// - sanfen_sunyi (三分损益): lengths alternately shortened and lengthened by a third, the twelve lü (Needham 1962, Science and
//   Civilisation in China IV.1, the section on sound, written with K. G. Robinson; Cho 2003, The Discovery of Musical Equal
//   Temperament in China and Europe);
// - ptolemy_intense_diatonic, five_limit_lattice, just_chromatic: just intonation in the 5-limit (Ptolemy, Harmonics, tr.
//   Barker 1989, Greek Musical Writings II; Barbour 1951, the chapter "Just Intonation");
// - meantone: fifths narrowed by a fraction of the syntonic comma (Barbour 1951, the chapter "Meantone Temperament"; Lindley,
//   "Mean-tone", New Grove);
// - equal: n equal divisions of the octave (Barbour 1951, the chapters "Equal Temperament" and "Multiple Division"; Cho 2003);
// - circulating, well_temperament, werckmeister_iii, vallotti: twelve fifths tempered unequally round a circle that closes
//   (Werckmeister 1691, Musicalische Temperatur; Barbour 1951, the chapter "Irregular Systems"; Lindley, "Temperaments", New
//   Grove);
// - urmawi: Ṣafī al-Dīn al-Urmawī's division of the octave into seventeen steps, limmas and commas (his Kitāb al-Adwār; Wright
//   1978, The Modal System of Arab and Persian Music A.D. 1250–1300).
//
// The measures: the interval on every degree and its departure from a just one; fifths, major and minor thirds; the circle of
// fifths and its wolf; two tunings compared degree by degree; how well n equal divisions reach a just interval; a division as a
// homomorphism of the 5-limit and what it sends to nothing (the theory note, §5); and the frequencies of a tuning's degrees from
// a reference the caller states.
//
// Limits. The octave is 2 : 1 in every tuning here: stretched octaves and scales that repeat at another interval are not held.
// A named historical tuning is the scheme a source states, not the practice of any instrument or player. A degree's name names
// its place in a chain of fifths when the first degree is called C; it is not a pitch, since the reference is the caller's.

#pragma once

#include <musicology/pitch/pitch.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <initializer_list>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace musicology::tuning {

using pitch::Interval;
using pitch::Ratio;

namespace detail {
inline void require(bool ok, const char* what) {
  if (!ok) throw std::invalid_argument(std::string("musicology::tuning: ") + what);
}
inline long long floor_div(long long a, long long b) { return a >= 0 ? a / b : -((-a + b - 1) / b); }
}  // namespace detail

// ---- A tuning ---------------------------------------------------------------------------------------------------------------

struct Degree {
  std::string name;                // may be empty
  double cents = 0;                // above the first degree, in [0, 1200)
  std::optional<Interval> exact;   // the interval above the first degree; absent for a degree known only by measurement
  std::optional<Ratio> ratio;      // the ratio, when the interval is rational
  std::string form;                // the exact form in writing: "3/2", "2^(7/12)", "5^(1/4)"; empty for a measured degree
};

struct Tuning {
  std::string name;
  std::vector<Degree> degrees;     // ascending from the first, the unison; the octave above it is not repeated
  std::size_t size() const { return degrees.size(); }
};

// A degree an exact interval above the first, and a degree measured in cents.
inline Degree degree(const Interval& above, std::string name = "") {
  Degree d;
  d.name = std::move(name);
  d.exact = above;
  d.ratio = above.ratio();
  d.cents = above.cents();
  d.form = above.str();
  return d;
}
inline Degree measured(double cents, std::string name = "") {
  Degree d;
  d.name = std::move(name);
  d.cents = cents;
  return d;
}

// A tuning from its degrees as given: the first the unison, the rest ascending within the octave.
inline Tuning scale(std::string name, std::vector<Degree> degrees) {
  detail::require(!degrees.empty(), "a tuning has a first degree");
  detail::require(degrees.front().cents == 0, "the first degree is the unison");
  for (std::size_t i = 1; i < degrees.size(); i++) detail::require(degrees[i].cents > degrees[i - 1].cents, "the degrees ascend");
  detail::require(degrees.back().cents < 1200, "the degrees lie within the octave");
  return Tuning{std::move(name), std::move(degrees)};
}

// A tuning from intervals in any order and any octave: each is brought within the octave, and they are put in ascending order.
// One of them must be the unison (or its octaves), and no two may fall on one degree.
inline Tuning from_intervals(std::string name, const std::vector<Interval>& intervals, const std::vector<std::string>& names = {}) {
  detail::require(!intervals.empty(), "a tuning has a first degree");
  detail::require(names.empty() || names.size() == intervals.size(), "a name for each interval, or none");
  std::vector<Degree> ds;
  for (std::size_t i = 0; i < intervals.size(); i++) ds.push_back(degree(pitch::octave_reduce(intervals[i]), names.empty() ? std::string() : names[i]));
  std::stable_sort(ds.begin(), ds.end(), [](const Degree& a, const Degree& b) { return a.cents < b.cents; });
  detail::require(ds.front().exact->is_unison(), "one of the intervals is the unison, the first degree");
  for (std::size_t i = 1; i < ds.size(); i++) detail::require(!(*ds[i].exact == *ds[i - 1].exact), "two intervals fall on one degree");
  return Tuning{std::move(name), std::move(ds)};
}

// A tuning from its successive steps, which must compose to the octave exactly.
inline Tuning from_steps(std::string name, const std::vector<Interval>& steps, const std::vector<std::string>& names = {}) {
  detail::require(!steps.empty(), "a tuning has a step");
  detail::require(names.empty() || names.size() == steps.size(), "a name for each degree, or none");
  std::vector<Degree> ds;
  Interval at;
  for (std::size_t i = 0; i < steps.size(); i++) {
    detail::require(steps[i].cents() > 0, "each step rises");
    ds.push_back(degree(at, names.empty() ? std::string() : names[i]));
    at = at * steps[i];
  }
  detail::require(at == Interval(pitch::octave()), "the steps compose to the octave");
  return Tuning{std::move(name), std::move(ds)};
}

// ---- Chains of fifths: Pythagorean tuning and the meantone temperaments -----------------------------------------------------

// The name of the note k fifths above C in the chain … B♭ F C G D A E B F♯ …: a place in the chain, not a pitch.
inline std::string note_name(long long k) {
  static const char* const letters[7] = {"F", "C", "G", "D", "A", "E", "B"};
  const long long q = detail::floor_div(k + 1, 7), r = k + 1 - 7 * q;
  std::string s = letters[r];
  for (long long i = 0; i < q; i++) s += "♯";
  for (long long i = 0; i > q; i--) s += "♭";
  return s;
}

// The notes of a chain of one fifth, `below` of them below the first degree and the rest above: fifth^k for
// k = −below, …, notes − 1 − below, each brought within the octave. Between the ends of the chain there is no fifth of this
// size: when the notes are twelve, the interval that closes the circle there is the wolf, and `below` chooses its place.
inline Tuning chain_of_fifths(std::string name, const Interval& fifth, std::size_t notes, std::size_t below) {
  detail::require(notes >= 1 && below < notes, "the first degree is one of the chain's notes");
  std::vector<Interval> xs;
  std::vector<std::string> names;
  for (long long k = -static_cast<long long>(below); k < static_cast<long long>(notes - below); k++) {
    xs.push_back(pitch::stack(fifth, k));
    names.push_back(note_name(k));
  }
  return from_intervals(std::move(name), xs, names);
}

// Pythagorean tuning: a chain of pure fifths, 3 : 2. Twelve notes with three fifths below the first degree are E♭ … G♯ when the
// first is called C, the wolf between G♯ and E♭; five notes with none below are a pentatonic scale, seven with one below a
// diatonic one. The chain never closes, whatever its length (the theory note, Proposition 3.1).
inline Tuning pythagorean(std::size_t notes, std::size_t below) {
  return chain_of_fifths("Pythagorean tuning of " + std::to_string(notes) + " notes", Interval(pitch::fifth()), notes, below);
}

// The fifth of the meantone temperament that narrows each fifth by the fraction f of the syntonic comma: (3/2) · (80/81)^f.
// f = 1/4 gives 5^(1/4), four of which make a pure major third and two octaves; f = 1/3, three of which make a pure major
// sixth and an octave; f = 0, the pure fifth.
inline Interval meantone_fifth(const Ratio& fraction) {
  return Interval(pitch::fifth()) / Interval(pitch::syntonic_comma()).power(fraction);
}

// A meantone temperament: a chain of the fifth above. By default twelve notes, E♭ … G♯ when the first is called C, a disposition
// common in the literature's tables; the wolf, wider than a fifth, then stands between G♯ and E♭.
inline Tuning meantone(const Ratio& fraction, std::size_t notes = 12, std::size_t below = 3) {
  return chain_of_fifths(fraction.str() + "-comma meantone temperament", meantone_fifth(fraction), notes, below);
}

// ---- 三分损益, the twelve lü ------------------------------------------------------------------------------------------------

// The twelve lü in the order in which the method generates them.
struct Lu {
  const char* pinyin;
  const char* hanzi;
};
inline const std::array<Lu, 12>& twelve_lu() {
  static const std::array<Lu, 12> lu{{{"Huangzhong", "黄钟"}, {"Linzhong", "林钟"}, {"Taicu", "太簇"}, {"Nanlü", "南吕"}, {"Guxian", "姑洗"}, {"Yingzhong", "应钟"},
                                      {"Ruibin", "蕤宾"}, {"Dalü", "大吕"}, {"Yize", "夷则"}, {"Jiazhong", "夹钟"}, {"Wuyi", "无射"}, {"Zhonglü", "仲吕"}}};
  return lu;
}

// The lengths the method of subtracting and adding a third generates, in the order of generation, as fractions of the first:
// from each length the next is two thirds of it (三分损一, a fifth higher) when that stays longer than half the first, and
// otherwise four thirds of it (三分益一, a fourth lower), so that all lie within one octave (Needham 1962; Cho 2003). Among the
// first twelve the two steps alternate except once, where two lengthenings follow each other (checked).
inline std::vector<Ratio> sanfen_sunyi_lengths(std::size_t notes = 12) {
  detail::require(notes >= 1, "a first length");
  const Ratio sun(2, 3), yi(4, 3), half(1, 2);
  std::vector<Ratio> lengths{Ratio(1)};
  while (lengths.size() < notes) {
    const Ratio shorter = lengths.back() * sun;
    lengths.push_back(shorter > half ? shorter : lengths.back() * yi);
  }
  return lengths;
}

// The tuning of those lengths, each degree the frequency ratio 1 / length, in ascending order and named by its lü. It is the
// Pythagorean tuning of as many notes with none below the first, exactly (checked; the theory note, Proposition 3.5).
//
// Limits. Frequency is taken to be inversely proportional to length, which holds of an ideal string at one tension; a pipe
// sounds below what its length alone gives.
inline Tuning sanfen_sunyi(std::size_t notes = 12) {
  const auto lengths = sanfen_sunyi_lengths(notes);
  std::vector<Interval> xs;
  std::vector<std::string> names;
  for (std::size_t i = 0; i < lengths.size(); i++) {
    xs.push_back(Interval(pitch::inverse(lengths[i])));
    names.push_back(i < 12 ? twelve_lu()[i].pinyin : "");
  }
  return from_intervals("Sanfen sunyi (三分损益), " + std::to_string(notes) + " lü", xs, names);
}

// ---- Just intonation in the 5-limit -----------------------------------------------------------------------------------------

// Ptolemy's intense (syntonic) diatonic: the diatonic whose tetrachord is a semitone 16 : 15, a major tone 9 : 8 and a minor
// tone 10 : 9, rising. It is written here from the degree on which it is the just major scale, 1, 9/8, 5/4, 4/3, 3/2, 5/3, 15/8,
// with the steps 9/8, 10/9, 16/15, 9/8, 10/9, 9/8, 16/15 (Ptolemy, Harmonics; Barbour 1951, the chapter "Greek Tunings").
inline Tuning ptolemy_intense_diatonic() {
  const Interval T(pitch::major_tone()), t(pitch::minor_tone()), s(pitch::diatonic_semitone());
  return from_steps("Ptolemy's intense diatonic", {T, t, s, T, t, T, s}, {"C", "D", "E", "F", "G", "A", "B"});
}

// The notes (3/2)^i (5/4)^j for i in [fifths_from, fifths_to] and j in [thirds_from, thirds_to]: rows of pure fifths, each row a
// pure major third above the one beneath. A note's name is its place in the chain of fifths, followed for j ≠ 0 by the number
// of syntonic commas by which it stands from that place: E(-1) is the pure third above C, a comma below the E of four fifths.
inline Tuning five_limit_lattice(std::string name, int fifths_from, int fifths_to, int thirds_from, int thirds_to) {
  detail::require(fifths_from <= 0 && fifths_to >= 0 && thirds_from <= 0 && thirds_to >= 0, "the lattice holds the first degree");
  std::vector<Interval> xs;
  std::vector<std::string> names;
  for (int j = thirds_from; j <= thirds_to; j++)
    for (int i = fifths_from; i <= fifths_to; i++) {
      xs.push_back(pitch::stack(Interval(pitch::fifth()), i) * pitch::stack(Interval(pitch::major_third()), j));
      names.push_back(note_name(i + 4 * j) + (j == 0 ? "" : j > 0 ? "(-" + std::to_string(j) + ")" : "(+" + std::to_string(-j) + ")"));
    }
  return from_intervals(std::move(name), xs, names);
}

// A common just scale of twelve notes: three rows of four pure fifths (F C G D; a major third above, A E B F♯; a major third
// below, D♭ A♭ E♭ B♭), which is 1, 16/15, 9/8, 6/5, 5/4, 4/3, 45/32, 3/2, 8/5, 5/3, 9/5, 15/8 (Barbour 1951, the chapter "Just
// Intonation", for scales of this kind). It holds Ptolemy's intense diatonic.
inline Tuning just_chromatic() { return five_limit_lattice("5-limit just scale of twelve notes", -1, 2, -1, 1); }

// ---- Equal temperaments -----------------------------------------------------------------------------------------------------

// n equal divisions of the octave: the degrees 2^(k/n). For n = 12, the semitone 2^(1/12) is ∛√√2, two square roots and a
// cube root (Cho 2003, for its computation by Zhu Zaiyu).
inline Tuning equal(std::size_t n) {
  detail::require(n >= 1, "at least one division");
  std::vector<Degree> ds;
  for (std::size_t k = 0; k < n; k++) ds.push_back(degree(Interval::prime(2, Ratio(static_cast<long long>(k), static_cast<long long>(n))), std::to_string(k)));
  return Tuning{"Equal temperament of " + std::to_string(n) + " divisions", std::move(ds)};
}

// ---- Temperaments that circulate --------------------------------------------------------------------------------------------

// A tuning of twelve notes from the twelve fifths of its circle, in the order C–G, G–D, D–A, A–E, E–B, B–F♯, F♯–C♯, C♯–G♯,
// G♯–E♭, E♭–B♭, B♭–F, F–C when the first degree is called C. The circle must close: the twelve compose to seven octaves, exactly.
inline Tuning circulating(std::string name, const std::vector<Interval>& fifths) {
  detail::require(fifths.size() == 12, "twelve fifths");
  std::vector<Interval> xs;
  std::vector<std::string> names;
  Interval at;
  for (std::size_t i = 0; i < 12; i++) {
    xs.push_back(at);
    names.push_back(note_name(i <= 8 ? static_cast<long long>(i) : static_cast<long long>(i) - 12));
    at = at * fifths[i];
  }
  detail::require(at == pitch::stack(Interval(pitch::octave()), 7), "the circle of fifths closes: twelve fifths make seven octaves");
  return from_intervals(std::move(name), xs, names);
}

// A well temperament stated as the fraction of the Pythagorean comma by which each fifth of the circle is narrowed, in the
// order above (a negative fraction widens). The fractions must sum to one, which is the closing of the circle: twelve pure
// fifths exceed seven octaves by exactly that comma.
inline Tuning well_temperament(std::string name, const std::vector<Ratio>& narrowed) {
  detail::require(narrowed.size() == 12, "twelve fifths");
  Ratio sum(0);
  std::vector<Interval> fifths;
  for (const Ratio& g : narrowed) {
    sum += g;
    fifths.push_back(Interval(pitch::fifth()) / Interval(pitch::pythagorean_comma()).power(g));
  }
  detail::require(sum == Ratio(1), "the fractions of the Pythagorean comma sum to one, or the circle does not close");
  return circulating(std::move(name), fifths);
}

// Werckmeister's temperament usually numbered III: the fifths C–G, G–D, D–A and B–F♯ each narrowed by a quarter of the
// Pythagorean comma, the other eight pure (Werckmeister 1691; Barbour 1951, the chapter "Irregular Systems").
inline Tuning werckmeister_iii() {
  const Ratio q(1, 4), o(0);
  return well_temperament("Werckmeister III", {q, q, q, o, o, q, o, o, o, o, o, o});
}

// The temperament known as Vallotti's: the six fifths F–C, C–G, G–D, D–A, A–E and E–B each narrowed by a sixth of the
// Pythagorean comma, the other six pure (Barbour 1951, the chapter "Irregular Systems"; Lindley, "Temperaments").
inline Tuning vallotti() {
  const Ratio s(1, 6), o(0);
  return well_temperament("Vallotti", {s, s, s, s, s, o, o, o, o, o, o, s});
}

// ---- The seventeen steps of al-Urmawī ---------------------------------------------------------------------------------------

// Ṣafī al-Dīn al-Urmawī's division of the octave into seventeen steps, each a limma L (256 : 243) or a Pythagorean comma C.
// The tone is L L C, the tetrachord two tones and a limma, L L C  L L C  L, and the octave two tetrachords and a tone (Wright
// 1978). Its seventeen notes are those of a chain of pure fifths, twelve below the first degree and four above (checked).
inline Tuning urmawi() {
  const Interval L(pitch::limma()), C(pitch::pythagorean_comma());
  const std::vector<Interval> tone{L, L, C}, tetrachord{L, L, C, L, L, C, L};
  std::vector<Interval> steps;
  for (const auto* part : {&tetrachord, &tetrachord, &tone}) steps.insert(steps.end(), part->begin(), part->end());
  return from_steps("al-Urmawī's seventeen-step division", steps);
}

// ---- Measures ---------------------------------------------------------------------------------------------------------------

// Two degrees sounded together: the interval from one degree to the degree `steps` above it (in the next octave when the scale
// is passed), against a just interval.
struct Dyad {
  std::size_t from = 0, to = 0;
  std::size_t steps = 0;
  double cents = 0;
  std::optional<Interval> exact;   // when both degrees are exact
  double departure = 0;            // cents less the just interval's: positive when wide, negative when narrow
  bool pure = false;               // exactly the just interval
};

// The number of a tuning's steps that stands for a just interval: the nearest in an equal division of as many notes,
// round(n log₂ r). It is 7 of twelve for the fifth, 4 of seven, 10 of seventeen.
inline std::size_t steps_nearest(const Tuning& t, const Ratio& just) {
  const long long s = std::llround(static_cast<double>(t.size()) * pitch::cents(just) / 1200.0);
  detail::require(s >= 0, "a just interval that rises");
  return static_cast<std::size_t>(s);
}

inline Dyad dyad(const Tuning& t, std::size_t from, std::size_t steps, const Ratio& just) {
  const std::size_t n = t.size();
  detail::require(from < n, "a degree of the tuning");
  Dyad d;
  d.from = from;
  d.steps = steps;
  d.to = (from + steps) % n;
  const long long octaves = static_cast<long long>((from + steps) / n);
  const Degree& a = t.degrees[from];
  const Degree& b = t.degrees[d.to];
  if (a.exact && b.exact) {
    d.exact = *b.exact / *a.exact * Interval::prime(2, Ratio(octaves));
    const Interval off = *d.exact / Interval(just);
    d.cents = d.exact->cents();
    d.departure = off.cents();
    d.pure = off.is_unison();
  } else {
    d.cents = b.cents - a.cents + 1200.0 * static_cast<double>(octaves);
    d.departure = d.cents - pitch::cents(just);
  }
  return d;
}

// The interval of `steps` steps on every degree, against a just interval; with no number of steps given, the nearest.
inline std::vector<Dyad> dyads(const Tuning& t, const Ratio& just, std::size_t steps = 0) {
  if (steps == 0) steps = steps_nearest(t, just);
  std::vector<Dyad> ds;
  for (std::size_t i = 0; i < t.size(); i++) ds.push_back(dyad(t, i, steps, just));
  return ds;
}
inline std::vector<Dyad> fifths(const Tuning& t) { return dyads(t, pitch::fifth()); }
inline std::vector<Dyad> major_thirds(const Tuning& t) { return dyads(t, pitch::major_third()); }
inline std::vector<Dyad> minor_thirds(const Tuning& t) { return dyads(t, pitch::minor_third()); }
// The successive steps of the scale; a step's departure is its own size.
inline std::vector<Dyad> steps(const Tuning& t) { return dyads(t, pitch::unison(), 1); }

// The circle of fifths from a degree: each fifth begins where the last ended, until the circle returns. When the fifth's steps
// and the number of notes are coprime, as 7 and 12, it passes through every degree, and its fifths compose to as many octaves
// as the fifth has steps.
inline std::vector<Dyad> circle_of_fifths(const Tuning& t, std::size_t start = 0) {
  const std::size_t s = steps_nearest(t, pitch::fifth());
  detail::require(start < t.size(), "a degree of the tuning");
  std::vector<Dyad> circle;
  std::size_t at = start;
  do {
    circle.push_back(dyad(t, at, s, pitch::fifth()));
    at = circle.back().to;
  } while (at != start && circle.size() < t.size());
  return circle;
}

// The fifth that departs most from pure: in a chain of twelve notes, the wolf that closes it.
inline Dyad wolf(const Tuning& t) {
  const auto fs = fifths(t);
  return *std::max_element(fs.begin(), fs.end(), [](const Dyad& a, const Dyad& b) { return std::fabs(a.departure) < std::fabs(b.departure); });
}

// Two tunings of as many notes, degree by degree: the cents by which each degree of b stands above the same degree of a.
inline std::vector<double> compare(const Tuning& a, const Tuning& b) {
  detail::require(a.size() == b.size(), "two tunings of as many notes");
  std::vector<double> d;
  for (std::size_t i = 0; i < a.size(); i++) {
    const Degree& x = a.degrees[i];
    const Degree& y = b.degrees[i];
    d.push_back(x.exact && y.exact ? (*y.exact / *x.exact).cents() : y.cents - x.cents);
  }
  return d;
}

// ---- Equal divisions against the just intervals -----------------------------------------------------------------------------

// The step of n equal divisions of the octave nearest a just interval.
struct Approximation {
  long long divisions = 0;
  long long steps = 0;   // round(n log₂ r)
  double cents = 0;      // 1200 · steps / n
  double error = 0;      // cents less the just interval's
};
inline Approximation approximation(long long n, const Ratio& just) {
  detail::require(n >= 1, "at least one division");
  const double c = pitch::cents(just);
  Approximation a;
  a.divisions = n;
  a.steps = std::llround(static_cast<double>(n) * c / 1200.0);
  a.cents = 1200.0 * static_cast<double>(a.steps) / static_cast<double>(n);
  a.error = a.cents - c;
  return a;
}

// The divisions up to a bound that come nearer a just interval than every smaller one. For the fifth they begin 1, 2, 3, 5, 7,
// 12, 29, 41, 53; the convergents of the continued fraction of log₂(3/2) are among them (the theory note, §3).
inline std::vector<Approximation> best_divisions(const Ratio& just, long long up_to) {
  std::vector<Approximation> best;
  for (long long n = 1; n <= up_to; n++) {
    const Approximation a = approximation(n, just);
    if (best.empty() || std::fabs(a.error) < std::fabs(best.back().error)) best.push_back(a);
  }
  return best;
}

// An equal division as a homomorphism of the 5-limit: the steps nearest the primes 2, 3 and 5, so that 2^a 3^b 5^c is sent to
// a · v₂ + b · v₃ + c · v₅ steps. For twelve divisions it is (12, 19, 28).
inline std::array<long long, 3> val(long long n) {
  return {n, approximation(n, Ratio(3)).steps, approximation(n, Ratio(5)).steps};
}

// The image of a 5-limit ratio under the homomorphism that sends 2, 3 and 5 to the given integers. A ratio whose image under
// an equal division's val is 0 is tempered out by it: the syntonic comma by 12, 19 and 31 divisions, the schisma by 53. With
// (1, 1, 0) and (0, 1, 4) it counts the octaves and the fifths of a ratio in a meantone temperament (the theory note, §5).
inline long long image(const std::array<long long, 3>& of_primes, const Ratio& r) {
  long long v = 0;
  const Interval x(r);
  for (const auto& [p, e] : x.factors()) {
    detail::require(p == 2 || p == 3 || p == 5, "a ratio of the 5-limit");
    v += e.numerator().to_int64() * of_primes[p == 2 ? 0 : p == 3 ? 1 : 2];
  }
  return v;
}

// ---- Frequencies ------------------------------------------------------------------------------------------------------------

// The degrees of a tuning are indexed through every octave: index i is degree i mod n, in octave ⌊i / n⌋ above the first
// degree's own. The frequency at an index, from the frequency of index 0. There is no standard pitch here: the caller states it.
inline double frequency(const Tuning& t, double first_hz, long long index) {
  detail::require(first_hz > 0, "a positive reference");
  const long long n = static_cast<long long>(t.size()), octave = detail::floor_div(index, n);
  const Degree& d = t.degrees[static_cast<std::size_t>(index - octave * n)];
  return std::ldexp(first_hz * (d.ratio ? d.ratio->to_double() : std::exp2(d.cents / 1200.0)), static_cast<int>(octave));
}

// The frequency index 0 must have for a given index to sound at a given frequency: for twelve notes, a tuning whose index 9 is
// to sound at 440 Hz has its first degree at first_degree(t, 440, 9).
inline double first_degree(const Tuning& t, double hz, long long index) {
  detail::require(hz > 0, "a positive frequency");
  return hz / frequency(t, 1.0, index);
}

struct Tone {
  long long index = 0;
  std::size_t degree = 0;
  long long octave = 0;
  double cents = 0;   // above index 0
  double hz = 0;
};

// The tones from one index to another, both included, for a work to sound.
inline std::vector<Tone> frequencies(const Tuning& t, double first_hz, long long from, long long to) {
  const long long n = static_cast<long long>(t.size());
  std::vector<Tone> tones;
  for (long long i = from; i <= to; i++) {
    Tone x;
    x.index = i;
    x.octave = detail::floor_div(i, n);
    x.degree = static_cast<std::size_t>(i - x.octave * n);
    x.cents = t.degrees[x.degree].cents + 1200.0 * static_cast<double>(x.octave);
    x.hz = frequency(t, first_hz, i);
    tones.push_back(x);
  }
  return tones;
}

}  // namespace musicology::tuning
