// Pitch: intervals as frequency ratios, exactly. The domain's theory note, §1–§3 and §7.
//
// An interval is the ratio of two frequencies, and intervals compose by multiplication: the group of intervals is (ℝ_{>0}, ·),
// and the just intervals are its subgroup ℚ_{>0}, free abelian on the primes. Two types hold them here.
// - Ratio, morphe's exact rational: a just interval, such as the fifth 3/2.
// - Interval: a product of primes raised to rational exponents, Π p^{e_p} with e_p ∈ ℚ. It holds the just intervals (integer
//   exponents) and the tempered ones this domain builds: the fifth of quarter-comma meantone is 5^(1/4), the semitone of the
//   equal temperament of twelve notes 2^(1/12). The logarithms of the primes are linearly independent over ℚ (the theory note,
//   Proposition 1.2), so the exponents are unique and two intervals are equal exactly when their exponents are: every identity
//   among tempered intervals is decided in exact arithmetic, with no tolerance.
//
// Sizes are measured in cents, 1200 log₂ of the ratio (Ellis 1885, in the additions to his translation of Helmholtz), so that
// the octave is 1200 cents and cents add where ratios multiply.
//
// Sources: Barbour 1951, Tuning and Temperament: A Historical Survey, for the commas and small intervals named below;
// Helmholtz 1877, On the Sensations of Tone (tr. Ellis 1885), part II, for the beats of mistuned consonances; Benson 2006,
// Music: A Mathematical Offering, for the mathematics of both.
//
// Limits. A ratio is a ratio of frequencies, never of lengths. An Interval is factored by trial division up to 10^6, and what
// is then left must be 1 or one prime below 2^63 (morphe's test): a ratio with two prime factors above 10^6 is refused. Cents
// are doubles, correct to about 10^-12 cents for the intervals of music. Beats assume ideal harmonic tones (see beats()).

#pragma once

#include <morphe/arithmetic/integer.hpp>
#include <morphe/arithmetic/modular.hpp>
#include <morphe/arithmetic/rational.hpp>

#include <cmath>
#include <cstdint>
#include <map>
#include <numbers>
#include <optional>
#include <stdexcept>
#include <string>

namespace musicology::pitch {

using Integer = morphe::arithmetic::Integer;
using Ratio = morphe::arithmetic::Rational;   // a just interval: a positive rational, exactly

namespace detail {
inline void require(bool ok, const char* what) {
  if (!ok) throw std::invalid_argument(std::string("musicology::pitch: ") + what);
}
// morphe's primality test, exact for every 64-bit integer.
inline bool is_prime(long long p) { return p >= 2 && morphe::arithmetic::is_prime(static_cast<std::uint64_t>(p)); }
inline constexpr long long trial_limit = 1000000;
}  // namespace detail

// ---- Just intervals ---------------------------------------------------------------------------------------------------------

// The interval p : q, a ratio of frequencies (p, q > 0).
inline Ratio ratio(long long p, long long q) {
  detail::require(p > 0 && q > 0, "a ratio of frequencies has positive terms");
  return Ratio(Integer(p), Integer(q));
}

// The size of a ratio in cents, 1200 log₂ r. The octaves are taken out exactly, and what is left, in (1/2, 2), goes through
// log1p of its exact excess over 1, so that a comma's size keeps its digits.
inline double cents(const Ratio& r) {
  detail::require(r.sign() > 0, "an interval is a positive ratio");
  const long long k = static_cast<long long>(r.numerator().bit_length()) - static_cast<long long>(r.denominator().bit_length());
  const Ratio m = r * morphe::arithmetic::pow(Ratio(2), -k);
  return 1200.0 * static_cast<double>(k) + 1200.0 / std::numbers::ln2 * std::log1p((m - Ratio(1)).to_double());
}

// Cents of a ratio given as a real number, and the real ratio of a size in cents.
inline double cents_from_ratio(double r) {
  detail::require(r > 0, "an interval is a positive ratio");
  return 1200.0 * std::log2(r);
}
inline double ratio_from_cents(double c) { return std::exp2(c / 1200.0); }

// Composition: the interval a followed by the interval b. Stacking n of one interval; n may be negative.
inline Ratio compose(const Ratio& a, const Ratio& b) { return a * b; }
inline Ratio stack(const Ratio& r, long long n) { return morphe::arithmetic::pow(r, n); }
// The interval from the tone `lower` to the tone `upper`, both given as intervals from one reference.
inline Ratio between(const Ratio& lower, const Ratio& upper) { return upper / lower; }
// The same interval taken downward: the inverse in the group.
inline Ratio inverse(const Ratio& r) { return Ratio(1) / r; }

// The representative of r within the octave: r · 2^k in [1, 2).
inline Ratio octave_reduce(const Ratio& r) {
  detail::require(r.sign() > 0, "an interval is a positive ratio");
  const long long k = static_cast<long long>(r.numerator().bit_length()) - static_cast<long long>(r.denominator().bit_length());
  const Ratio m = r * morphe::arithmetic::pow(Ratio(2), -k);   // in (1/2, 2)
  return m < Ratio(1) ? m * Ratio(2) : m;
}

// The inversion of an interval within the octave, its complement: the fifth becomes the fourth, the major third the minor
// sixth. For r in (1, 2), r · inversion(r) = 2; the unison is its own inversion.
inline Ratio inversion(const Ratio& r) { return octave_reduce(Ratio(2) / r); }

// ---- The named intervals ----------------------------------------------------------------------------------------------------
//
// The consonances of the 5-limit, and the commas and small intervals by which stacks of them differ (Barbour 1951; the relations
// among them are checked exactly in check_pitch.cpp and proved in the theory note, §3–§4).

inline Ratio unison() { return ratio(1, 1); }
inline Ratio octave() { return ratio(2, 1); }
inline Ratio fifth() { return ratio(3, 2); }
inline Ratio fourth() { return ratio(4, 3); }
inline Ratio major_third() { return ratio(5, 4); }
inline Ratio minor_third() { return ratio(6, 5); }
inline Ratio major_sixth() { return ratio(5, 3); }
inline Ratio minor_sixth() { return ratio(8, 5); }
inline Ratio major_tone() { return ratio(9, 8); }             // fifth − fourth
inline Ratio minor_tone() { return ratio(10, 9); }            // major third − major tone
inline Ratio diatonic_semitone() { return ratio(16, 15); }    // fourth − major third
inline Ratio chromatic_semitone() { return ratio(25, 24); }   // major third − minor third
inline Ratio ditone() { return ratio(81, 64); }               // two major tones: the Pythagorean major third

// Twelve fifths less seven octaves, 3^12 / 2^19.
inline Ratio pythagorean_comma() { return ratio(531441, 524288); }
// Four fifths less two octaves and a major third, 3^4 / (2^4 · 5): the ditone over the major third.
inline Ratio syntonic_comma() { return ratio(81, 80); }
// The Pythagorean comma less the syntonic: eight fifths and a major third less five octaves, 3^8 · 5 / 2^15.
inline Ratio schisma() { return ratio(32805, 32768); }
// The syntonic comma less the schisma, 2^11 / (3^4 · 5^2).
inline Ratio diaschisma() { return ratio(2048, 2025); }
// An octave less three major thirds, 2^7 / 5^3.
inline Ratio lesser_diesis() { return ratio(128, 125); }
// Four minor thirds less an octave, 2^3 · 3^4 / 5^4.
inline Ratio greater_diesis() { return ratio(648, 625); }
// A fourth less two major tones, 2^8 / 3^5: the diatonic semitone of Pythagorean tuning.
inline Ratio limma() { return ratio(256, 243); }
// A major tone less a limma, 3^7 / 2^11: the chromatic semitone of Pythagorean tuning. It exceeds the limma by the Pythagorean comma.
inline Ratio apotome() { return ratio(2187, 2048); }

// ---- Intervals with rational exponents --------------------------------------------------------------------------------------

// Π p^{e_p}, e_p ∈ ℚ: an element of the group the primes generate under multiplication and the extraction of roots. Its
// exponents are kept by prime, none zero, so that equality of intervals is equality of what is kept (Proposition 1.2).
class Interval {
 public:
  Interval() = default;   // the unison
  // A just interval, factored. Refuses a ratio that is not positive, or one that is beyond the factoring (see Limits above).
  Interval(const Ratio& r) {
    detail::require(r.sign() > 0, "an interval is a positive ratio");
    factor(r.numerator(), 1);
    factor(r.denominator(), -1);
  }
  // p^e for a prime p.
  static Interval prime(long long p, const Ratio& e = Ratio(1)) {
    detail::require(detail::is_prime(p), "the base of a prime power is a prime");
    Interval x;
    if (!e.is_zero()) x.f_[p] = e;
    return x;
  }

  const std::map<long long, Ratio>& factors() const { return f_; }
  Ratio exponent(long long p) const {
    const auto it = f_.find(p);
    return it == f_.end() ? Ratio(0) : it->second;
  }
  bool is_unison() const { return f_.empty(); }
  bool is_rational() const {
    for (const auto& [p, e] : f_)
      if (!e.is_integer()) return false;
    return true;
  }
  // The ratio, when every exponent is an integer.
  std::optional<Ratio> ratio() const {
    if (!is_rational()) return std::nullopt;
    Ratio r(1);
    for (const auto& [p, e] : f_) r *= morphe::arithmetic::pow(Ratio(p), e.numerator().to_int64());
    return r;
  }
  // The size in cents, Σ e_p · 1200 log₂ p; through the exact ratio when there is one, and exactly for powers of 2.
  double cents() const {
    if (const auto r = ratio()) return pitch::cents(*r);
    double c = 0;
    for (const auto& [p, e] : f_) c += p == 2 ? (e * Ratio(1200)).to_double() : e.to_double() * 1200.0 * std::log2(static_cast<double>(p));
    return c;
  }
  // x^q: q of the interval stacked, a root of it, or both.
  Interval power(const Ratio& q) const {
    Interval x;
    if (q.is_zero()) return x;
    for (const auto& [p, e] : f_) x.f_[p] = e * q;
    return x;
  }
  // The exact form in writing: "3/2" for a ratio, otherwise the prime powers, as "2^(7/12)" or "2^(-1/2)·5^(1/4)".
  std::string str() const {
    if (const auto r = ratio()) return r->numerator().str() + "/" + r->denominator().str();
    std::string s;
    for (const auto& [p, e] : f_) {
      if (!s.empty()) s += "·";
      s += std::to_string(p);
      if (e == Ratio(1)) continue;
      s += e.is_integer() ? "^" + e.str() : "^(" + e.str() + ")";
    }
    return s;
  }

  friend Interval operator*(const Interval& a, const Interval& b) {
    Interval c = a;
    for (const auto& [p, e] : b.f_) {
      Ratio& x = c.f_[p];
      x += e;
      if (x.is_zero()) c.f_.erase(p);
    }
    return c;
  }
  friend Interval operator/(const Interval& a, const Interval& b) { return a * b.power(Ratio(-1)); }
  friend bool operator==(const Interval& a, const Interval& b) { return a.f_ == b.f_; }

 private:
  void factor(Integer n, long long sign) {
    const auto take = [&](long long p) {
      long long e = 0;
      for (;;) {
        Integer q, r;
        divmod(n, Integer(p), q, r);
        if (!r.is_zero()) break;
        n = q;
        e++;
      }
      if (e) f_[p] += Ratio(sign * e);
    };
    take(2);
    take(3);
    for (long long p = 5, step = 2; !n.is_one(); p += step, step = 6 - step) {
      if (n.fits_int64() && p * p > n.to_int64()) {   // what is left has no factor up to its root: it is prime
        f_[n.to_int64()] += Ratio(sign);
        break;
      }
      if (p > detail::trial_limit) {   // no factor below 10^6 is left: kept when it is one prime, by morphe's test
        detail::require(n.fits_int64() && detail::is_prime(n.to_int64()), "a ratio with two prime factors above 10^6, or one above 2^63, is not factored");
        f_[n.to_int64()] += Ratio(sign);
        break;
      }
      take(p);
    }
  }

  std::map<long long, Ratio> f_;
};

inline double cents(const Interval& x) { return x.cents(); }
inline Interval compose(const Interval& a, const Interval& b) { return a * b; }
inline Interval stack(const Interval& x, long long n) { return x.power(Ratio(n)); }
inline Interval between(const Interval& lower, const Interval& upper) { return upper / lower; }
inline Interval inverse(const Interval& x) { return x.power(Ratio(-1)); }

// The representative within the octave, of size in [0, 1200) cents: exactly for a ratio; otherwise the octaves are counted from
// the size in cents, which no interval of music brings within rounding of a whole octave.
inline Interval octave_reduce(const Interval& x) {
  if (const auto r = x.ratio()) return Interval(octave_reduce(*r));
  const long long k = static_cast<long long>(std::floor(x.cents() / 1200.0));
  return x / Interval::prime(2, Ratio(k));
}
inline Interval inversion(const Interval& x) { return octave_reduce(Interval(Ratio(2)) / x); }

// ---- Frequencies ------------------------------------------------------------------------------------------------------------

// The frequency of the tone an interval above a reference. The reference is the caller's: the domain has no standard pitch.
inline double frequency(double reference_hz, const Ratio& above) {
  detail::require(reference_hz > 0 && above.sign() > 0, "a positive reference and a positive ratio");
  return reference_hz * above.to_double();
}
inline double frequency(double reference_hz, const Interval& above) {
  detail::require(reference_hz > 0, "a positive reference");
  if (const auto r = above.ratio()) return reference_hz * r->to_double();
  return reference_hz * std::exp2(above.cents() / 1200.0);
}
inline double frequency_from_cents(double reference_hz, double cents_above) {
  detail::require(reference_hz > 0, "a positive reference");
  return reference_hz * std::exp2(cents_above / 1200.0);
}

// ---- Beats ------------------------------------------------------------------------------------------------------------------

// The beating of a consonance near the just ratio p : q (upper to lower, in lowest terms). The p-th partial of the lower tone
// and the q-th of the upper coincide when the interval is just, at p · f_lower = q · f_upper; mistuned, the two partials sound
// together at nearly one pitch, and their sum swells and fades |q · f_upper − p · f_lower| times a second (Helmholtz 1877,
// part II; the theory note, §7).
//
// Limits. Both tones are taken to be ideal harmonic tones, their partials whole multiples of their fundamentals: the partials of
// a real string are sharp of these, and the beats of a piano's intervals differ accordingly. The rate says nothing of how loud
// the beats are, which depends on the strength of the two partials; and beats of higher coinciding partials (2p and 2q, …),
// faster by whole multiples, are not listed.
struct Beat {
  long long lower_partial = 1;   // p: the partial of the lower tone
  long long upper_partial = 1;   // q: the partial of the upper tone
  double rate_hz = 0;            // beats a second, |q f_upper − p f_lower|
  double pitch_hz = 0;           // where they are heard: the mean of the two partials
  double mistuning_cents = 0;    // the interval's departure from the just ratio; positive when it is wide
};

inline Beat beats(double lower_hz, double upper_hz, const Ratio& just) {
  detail::require(lower_hz > 0 && upper_hz > 0, "positive frequencies");
  detail::require(just.sign() > 0 && just.numerator().fits_int64() && just.denominator().fits_int64(), "a just ratio of moderate terms");
  Beat b;
  b.lower_partial = just.numerator().to_int64();
  b.upper_partial = just.denominator().to_int64();
  const double low = static_cast<double>(b.lower_partial) * lower_hz, high = static_cast<double>(b.upper_partial) * upper_hz;
  b.rate_hz = std::fabs(high - low);
  b.pitch_hz = (high + low) / 2;
  b.mistuning_cents = 1200.0 * std::log2(high / low);
  return b;
}

}  // namespace musicology::pitch
