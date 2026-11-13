// Checks of pitch (pitch.hpp): against exact arithmetic and closed forms. No size in cents is taken from memory: each is
// computed here from the ratios it measures.
//
// - Cents: the octave is 1200 exactly, cents add where ratios compose, and the two directions undo each other.
// - The commas, each from the stack of consonances that defines it, exactly: twelve fifths against seven octaves, four fifths
//   against a major third and two octaves, and the relations among them.
// - Octave reduction, inversion and the inverse: their laws.
// - Intervals with rational exponents: the group laws, uniqueness of the form, the tempered fifths as exact elements.
// - Frequencies from a reference, and the beats of a mistuned consonance against sums worked by hand.

#include <musicology/pitch/pitch.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

namespace mp = musicology::pitch;
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
}  // namespace

int main() {
  calx::verification::Suite s;
  const Ratio one(1), two(2);
  const Ratio P8 = mp::octave(), P5 = mp::fifth(), P4 = mp::fourth(), M3 = mp::major_third(), m3 = mp::minor_third();
  const std::vector<Ratio> sample{P5, P4, M3, m3, mp::major_sixth(), mp::minor_sixth(), mp::major_tone(), mp::minor_tone(), mp::diatonic_semitone(),
                                  mp::ratio(7, 4), mp::ratio(11, 8), mp::ratio(45, 32), mp::ratio(243, 128)};

  // ---- Cents
  s.check("the octave is 1200 cents exactly", mp::cents(P8) - 1200.0, 0.0, "cents");
  s.check("the fifth's cents are 1200 log₂(3/2)", mp::cents(P5) - 1200.0 * std::log2(1.5), 1e-12, "cents");
  {
    double worst = 0;
    for (const Ratio& a : sample)
      for (const Ratio& b : sample) worst = std::max(worst, std::fabs(mp::cents(mp::compose(a, b)) - mp::cents(a) - mp::cents(b)));
    s.check("cents add where ratios compose", worst, 1e-11, "cents");
  }
  {
    double worst = 0;
    for (const Ratio& a : sample) {
      worst = std::max(worst, std::fabs(mp::ratio_from_cents(mp::cents(a)) / a.to_double() - 1));
      worst = std::max(worst, std::fabs(mp::cents_from_ratio(a.to_double()) - mp::cents(a)) / 1200.0);
    }
    s.check("a ratio from its cents is the ratio, and cents of the real ratio are the exact ratio's", worst, 1e-14);
  }
  s.check("cents of a ratio far from the octave: 3^40 is 40 times the cents of 3", mp::cents(mp::stack(Ratio(3), 40)) - 40 * 1200.0 * std::log2(3.0), 1e-9, "cents");
  s.check("the Pythagorean comma's cents are twelve fifths' less seven octaves'", mp::cents(mp::pythagorean_comma()) - (12 * mp::cents(P5) - 8400.0), 1e-9, "cents");

  // ---- The consonances among themselves
  s.require("fifth and fourth compose to the octave; major and minor third to the fifth", mp::compose(P5, P4) == P8 && mp::compose(M3, m3) == P5);
  s.require("the major tone is the fifth less the fourth, the minor tone the major third less the major tone, the semitone the fourth less the major third",
            mp::between(P4, P5) == mp::major_tone() && mp::between(mp::major_tone(), M3) == mp::minor_tone() && mp::between(M3, P4) == mp::diatonic_semitone());
  s.require("the chromatic semitone is the major third less the minor; the ditone two major tones", mp::between(m3, M3) == mp::chromatic_semitone() && mp::stack(mp::major_tone(), 2) == mp::ditone());

  // ---- The commas, each from its definition
  const Ratio pc = mp::pythagorean_comma(), sc = mp::syntonic_comma();
  s.require("twelve fifths against seven octaves is the Pythagorean comma, 531441/524288", mp::between(mp::stack(P8, 7), mp::stack(P5, 12)) == pc && pc == mp::ratio(531441, 524288),
            mp::between(mp::stack(P8, 7), mp::stack(P5, 12)).str());
  s.require("four fifths against a major third and two octaves is the syntonic comma, 81/80", mp::between(mp::compose(M3, mp::stack(P8, 2)), mp::stack(P5, 4)) == sc && sc == mp::ratio(81, 80));
  s.require("the syntonic comma is the ditone over the major third, and the major tone over the minor", mp::between(M3, mp::ditone()) == sc && mp::between(mp::minor_tone(), mp::major_tone()) == sc);
  s.require("the schisma is the Pythagorean comma less the syntonic, 32805/32768", mp::between(sc, pc) == mp::schisma() && mp::schisma() == mp::ratio(32805, 32768), mp::between(sc, pc).str());
  s.require("the schisma is eight fifths and a major third against five octaves", mp::between(mp::stack(P8, 5), mp::compose(mp::stack(P5, 8), M3)) == mp::schisma());
  s.require("the lesser diesis is an octave against three major thirds, 128/125", mp::between(mp::stack(M3, 3), P8) == mp::lesser_diesis() && mp::lesser_diesis() == mp::ratio(128, 125));
  s.require("the greater diesis is four minor thirds against an octave, 648/625", mp::between(P8, mp::stack(m3, 4)) == mp::greater_diesis());
  s.require("the diaschisma is the syntonic comma less the schisma, and the lesser diesis less the syntonic comma",
            mp::between(mp::schisma(), sc) == mp::diaschisma() && mp::between(sc, mp::lesser_diesis()) == mp::diaschisma());
  s.require("the limma is a fourth less two major tones, and three octaves less five fifths, 256/243",
            mp::between(mp::stack(mp::major_tone(), 2), P4) == mp::limma() && mp::between(mp::stack(P5, 5), mp::stack(P8, 3)) == mp::limma() && mp::limma() == mp::ratio(256, 243));
  s.require("the apotome is a major tone less a limma, and seven fifths less four octaves, 2187/2048",
            mp::between(mp::limma(), mp::major_tone()) == mp::apotome() && mp::between(mp::stack(P8, 4), mp::stack(P5, 7)) == mp::apotome() && mp::apotome() == mp::ratio(2187, 2048));
  s.require("the apotome exceeds the limma by the Pythagorean comma", mp::between(mp::limma(), mp::apotome()) == pc);
  s.require("the Pythagorean comma is two syntonic commas less a diaschisma", mp::between(mp::diaschisma(), mp::stack(sc, 2)) == pc);
  s.require("in order of size: schisma, diaschisma, syntonic comma, Pythagorean comma, lesser diesis, greater diesis, limma, apotome",
            one < mp::schisma() && mp::schisma() < mp::diaschisma() && mp::diaschisma() < sc && sc < pc && pc < mp::lesser_diesis() && mp::lesser_diesis() < mp::greater_diesis() &&
                mp::greater_diesis() < mp::limma() && mp::limma() < mp::apotome());
  {
    const Ratio excess = mp::octave_reduce(mp::stack(P5, 53));
    s.require("fifty-three fifths exceed thirty-one octaves by less than a syntonic comma and more than a schisma",
              excess == mp::between(mp::stack(P8, 31), mp::stack(P5, 53)) && mp::schisma() < excess && excess < sc, std::to_string(mp::cents(excess)) + " cents");
  }

  // ---- Octave reduction, inversion, the inverse
  s.require("octave reduction: 3 to 3/2, 1/3 to 4/3, 9/4 to 9/8, 5 to 5/4", mp::octave_reduce(Ratio(3)) == P5 && mp::octave_reduce(mp::ratio(1, 3)) == P4 &&
                                                                               mp::octave_reduce(mp::ratio(9, 4)) == mp::major_tone() && mp::octave_reduce(Ratio(5)) == M3);
  {
    bool ok = true;
    for (long long k = -40; k <= 40; k++) ok = ok && mp::octave_reduce(mp::stack(two, k)) == one;
    s.require("every power of 2 reduces to the unison", ok);
  }
  {
    bool within = true, idempotent = true, by_octaves = true;
    for (long long k = -30; k <= 30; k++) {
      const Ratio r = mp::stack(P5, k) * mp::stack(M3, k % 4), o = mp::octave_reduce(r);
      within = within && !(o < one) && o < two;
      idempotent = idempotent && mp::octave_reduce(o) == o;
      const Interval q = Interval(r) / Interval(o);   // a power of 2 alone
      by_octaves = by_octaves && q.exponent(3).is_zero() && q.exponent(5).is_zero() && q.exponent(2).is_integer();
    }
    s.require("a reduced ratio lies in [1, 2), differs from the ratio by octaves alone, and is its own reduction", within && idempotent && by_octaves);
  }
  s.require("inversion: the fifth and the fourth, the major third and the minor sixth, the minor third and the major sixth; the unison its own",
            mp::inversion(P5) == P4 && mp::inversion(P4) == P5 && mp::inversion(M3) == mp::minor_sixth() && mp::inversion(m3) == mp::major_sixth() && mp::inversion(one) == one);
  {
    bool complement = true, involution = true, antihom = true;
    for (const Ratio& a : sample) {
      complement = complement && mp::compose(a, mp::inversion(a)) == P8;
      involution = involution && mp::inversion(mp::inversion(a)) == a;
      for (const Ratio& b : sample) antihom = antihom && mp::inversion(mp::compose(a, b)) == mp::octave_reduce(mp::compose(mp::inversion(a), mp::inversion(b)));
    }
    s.require("an interval within the octave and its inversion compose to the octave; inversion is an involution, and a homomorphism modulo octaves", complement && involution && antihom);
  }
  {
    bool ok = true;
    double worst = 0;
    for (const Ratio& a : sample) {
      ok = ok && mp::compose(a, mp::inverse(a)) == one && mp::inverse(mp::inverse(a)) == a && mp::stack(a, -3) == mp::inverse(mp::stack(a, 3));
      worst = std::max(worst, std::fabs(mp::cents(mp::inverse(a)) + mp::cents(a)));
    }
    s.require("an interval and its inverse compose to the unison; stacking a negative number of times stacks the inverse", ok);
    s.check("the inverse's cents are the negative", worst, 1e-12, "cents");
  }
  {
    bool ok = true;
    for (const Ratio& a : sample)
      for (const Ratio& b : sample)
        for (const Ratio& c : {P5, M3, mp::ratio(7, 4)})
          ok = ok && mp::compose(mp::compose(a, b), c) == mp::compose(a, mp::compose(b, c)) && mp::compose(a, b) == mp::compose(b, a) && mp::between(a, mp::compose(a, b)) == b;
    s.require("composition is associative and commutative, and the interval between a tone and a tone raised by b is b", ok);
  }

  // ---- Intervals with rational exponents
  {
    bool ok = true;
    for (const Ratio& a : sample) ok = ok && Interval(a).ratio() == a && Interval(a).is_rational();
    ok = ok && Interval(pc).ratio() == pc && Interval(pc).exponent(3) == Ratio(12) && Interval(pc).exponent(2) == Ratio(-19) && Interval(pc).factors().size() == 2;
    s.require("a ratio factored and multiplied out is the ratio; the Pythagorean comma is 3^12 / 2^19", ok);
  }
  {
    const Interval sch(mp::schisma()), dia(mp::diaschisma());
    s.require("the schisma is 3^8 · 5 / 2^15, the diaschisma 2^11 / (3^4 · 5^2)",
              sch.exponent(2) == Ratio(-15) && sch.exponent(3) == Ratio(8) && sch.exponent(5) == Ratio(1) && dia.exponent(2) == Ratio(11) && dia.exponent(3) == Ratio(-4) && dia.exponent(5) == Ratio(-2));
  }
  {
    bool ok = true;
    for (const Ratio& a : sample)
      for (const Ratio& b : sample) {
        const Interval x(a), y(b);
        ok = ok && x * y == Interval(mp::compose(a, b)) && (x * y) / y == x && x / x == Interval() && mp::inverse(x) * x == Interval() && mp::stack(x, 3) == Interval(mp::stack(a, 3));
        for (const Ratio& q : {Ratio(1, 4), Ratio(-2, 7), Ratio(5, 12)}) ok = ok && (x * y).power(q) == x.power(q) * y.power(q) && x.power(q).power(mp::inverse(q)) == x;
      }
    s.require("intervals with rational exponents form a group that holds the ratios, and x ↦ x^q is a homomorphism undone by x ↦ x^(1/q)", ok);
  }
  const Interval semitone = Interval::prime(2, Ratio(1, 12)), tempered_fifth = mp::stack(semitone, 7);
  s.require("2^(7/12) is no ratio, and twelve of it are seven octaves exactly", !tempered_fifth.is_rational() && !tempered_fifth.ratio() && mp::stack(tempered_fifth, 12) == Interval(mp::stack(P8, 7)));
  s.check("2^(7/12) is 700 cents exactly", tempered_fifth.cents() - 700.0, 0.0, "cents");
  {
    const Interval mean = Interval(P5) / Interval(sc).power(Ratio(1, 4));
    s.require("the fifth narrowed by a quarter of the syntonic comma is 5^(1/4) exactly, and four of them reduce to the major third 5/4",
              mean == Interval::prime(5, Ratio(1, 4)) && mp::octave_reduce(mp::stack(mean, 4)) == Interval(M3), mean.str());
    s.check("its cents are a quarter of the cents of 5", mean.cents() - 300.0 * std::log2(5.0), 1e-12, "cents");
  }
  {
    const Interval x = mp::stack(Interval(P5) / Interval(sc).power(Ratio(1, 6)), 9), o = mp::octave_reduce(x);
    const Interval q = x / o;
    s.require("an irrational interval reduced lies within the octave and differs from itself by whole octaves",
              o.cents() >= 0 && o.cents() < 1200 && q.factors().size() == 1 && q.exponent(2).is_integer() && mp::inversion(mp::inversion(o)) == o && mp::inversion(o) * o == Interval(P8), o.str());
  }
  s.require("the forms in writing: 3/2, 2/1, 2^(7/12), 5^(1/4), 2^(-1/3)·3^(1/3)·5^(1/6)",
            Interval(P5).str() == "3/2" && Interval(P8).str() == "2/1" && tempered_fifth.str() == "2^(7/12)" && Interval::prime(5, Ratio(1, 4)).str() == "5^(1/4)" &&
                (Interval(P5) / Interval(sc).power(Ratio(1, 6))).str() == "2^(-1/3)·3^(1/3)·5^(1/6)",
            (Interval(P5) / Interval(sc).power(Ratio(1, 6))).str());
  // 1000003 and 1000000007 are primes; 2^61 − 1 is Mersenne's.
  s.require("a prime above 10^6 is kept as a factor, 2^61 − 1 among them; a product of two is refused, as are a ratio that is not positive and a base that is not prime",
            Interval(mp::ratio(1000003, 2)).exponent(1000003) == Ratio(1) && Interval(mp::ratio(3, 1000000007)).exponent(1000000007) == Ratio(-1) &&
                Interval(mp::ratio(2305843009213693951LL, 9)).exponent(2305843009213693951LL) == Ratio(1) && refuses([] { Interval(mp::ratio(1000003LL * 1000003LL, 1)); }) &&
                refuses([] { Interval(Ratio(-3, 2)); }) &&
                refuses([] { mp::cents(Ratio(0)); }) && refuses([] { mp::ratio(3, 0); }) && refuses([] { Interval::prime(4); }));

  // ---- Frequencies
  s.check("a fifth above 440 Hz is 660 Hz, a major third above 264 Hz is 330 Hz", std::fabs(mp::frequency(440.0, P5) - 660.0) + std::fabs(mp::frequency(264.0, M3) - 330.0), 0.0, "Hz");
  s.check("twelve tempered semitones above 440 Hz are 880 Hz; one is 440 · 2^(1/12)",
          std::fabs(mp::frequency(440.0, mp::stack(semitone, 12)) - 880.0) + std::fabs(mp::frequency(440.0, semitone) - 440.0 * std::pow(2.0, 1.0 / 12.0)), 1e-12, "Hz");
  s.check("1200 cents above a reference doubles it, and 700 cents is 2^(7/12) of it", std::fabs(mp::frequency_from_cents(220.0, 1200.0) - 440.0) + std::fabs(mp::frequency_from_cents(220.0, 700.0) - mp::frequency(220.0, tempered_fifth)), 1e-12, "Hz");
  s.require("a reference that is not positive is refused", refuses([&] { mp::frequency(0.0, P5); }) && refuses([&] { mp::frequency_from_cents(-1.0, 100.0); }));

  // ---- Beats
  {
    // By hand: 200 Hz and 301 Hz near 3 : 2. The third partial of the lower is 600 Hz, the second of the upper 602 Hz.
    const mp::Beat b = mp::beats(200.0, 301.0, P5);
    s.require("a fifth of 200 and 301 Hz: the third and second partials, 600 and 602 Hz, beat twice a second about 601 Hz, the fifth wide",
              b.lower_partial == 3 && b.upper_partial == 2 && b.rate_hz == 2.0 && b.pitch_hz == 601.0 && b.mistuning_cents > 0, std::to_string(b.rate_hz) + " Hz");
  }
  {
    // By hand: 400 Hz and 499 Hz near 5 : 4. 5 · 400 = 2000 Hz, 4 · 499 = 1996 Hz.
    const mp::Beat b = mp::beats(400.0, 499.0, M3);
    s.require("a major third of 400 and 499 Hz: the fifth and fourth partials, 2000 and 1996 Hz, beat four times a second, the third narrow",
              b.lower_partial == 5 && b.upper_partial == 4 && b.rate_hz == 4.0 && b.pitch_hz == 1998.0 && b.mistuning_cents < 0);
  }
  s.require("a unison of 440 and 442 Hz beats twice a second; a pure fifth and a pure major third do not beat",
            mp::beats(440.0, 442.0, one).rate_hz == 2.0 && mp::beats(220.0, 330.0, P5).rate_hz == 0.0 && mp::beats(264.0, 330.0, M3).rate_hz == 0.0 && mp::beats(264.0, 330.0, M3).mistuning_cents == 0.0);
  {
    const double lower = 220.0, upper = mp::frequency(lower, tempered_fifth);
    const mp::Beat b = mp::beats(lower, upper, P5), above = mp::beats(2 * lower, 2 * upper, P5);
    s.check("the tempered fifth on 220 Hz beats |2 · 220 · 2^(7/12) − 3 · 220| times a second, narrow", b.rate_hz - std::fabs(2 * 220.0 * std::pow(2.0, 7.0 / 12.0) - 660.0), 1e-12, "Hz");
    s.check("its mistuning is 700 cents less the pure fifth's", b.mistuning_cents - (700.0 - mp::cents(P5)), 1e-11, "cents");
    s.check("the same interval an octave higher beats twice as fast", above.rate_hz - 2 * b.rate_hz, 1e-12, "Hz");
    s.check("the rate is p f (2^(δ/1200) − 1) for a mistuning of δ cents", b.rate_hz - 3 * lower * std::fabs(std::exp2(b.mistuning_cents / 1200.0) - 1), 1e-10, "Hz");
  }
  s.require("frequencies that are not positive are refused", refuses([&] { mp::beats(0.0, 330.0, P5); }) && refuses([&] { mp::beats(220.0, -1.0, P5); }));

  return s.done();
}
