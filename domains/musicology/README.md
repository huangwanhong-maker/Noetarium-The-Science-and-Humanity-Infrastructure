# Musicology

*Status: draft (2026-11-13; [ADR 0111](../../development/decisions/0111-musicology-and-philosophy-founded.md)).* The domain of music as an object of study: its pitches and the systems that tune them first, later its rhythm, harmony, form, notation and instruments. It is founded for Histos Harmonia, a work of history and learning whose first thread is tuning and temperament, from the Pythagoreans to equal temperament, in Greece, China, the Arabic-speaking world and Europe. It is a field at height 0, standing on the foundations alone, C++ from the start.

## 1. Purpose

A musical interval is a ratio of two frequencies, and intervals compose by multiplication. The consonances are ratios of small whole numbers, the octave 2 : 1, the fifth 3 : 2, the major third 5 : 4, and no power of one of them is a power of another. A scale built from pure fifths therefore never returns to its start, and one built from pure fifths cannot have pure thirds. Every tuning system is an answer to these two facts. The domain holds the objects in which the answers are stated:
- the interval, exactly: a ratio, or a product of primes to rational exponents when it is tempered;
- the commas and small intervals by which stacks of consonances differ;
- the tuning system, a scale of degrees within the octave, and the ways of building one that the history of music has used;
- the measures by which tunings are compared: how far each fifth and third stands from just, where the wolf is, what a temperament tempers out.

Every identity among intervals is decided in exact arithmetic, on morphe's integers and rationals: that quarter-comma meantone's major third is 5/4, that Werckmeister's circle closes, that the twelve lü of 三分损益 are the Pythagorean ratios. Only sizes in cents and frequencies in hertz are floating-point numbers.

Its theory note, [theory/tuning-and-temperament.md](theory/tuning-and-temperament.md), states and proves what the code relies on: the group of intervals and its quotient by octaves, the Pythagorean comma as the obstruction to closing the circle of fifths, and temperaments as homomorphisms from the free abelian group on the primes 2, 3 and 5, with the commas in their kernels.

## 2. Modules

| Module | Holds | Status |
|---|---|---|
| `pitch` | Intervals as exact frequency ratios, and as primes to rational exponents; cents; octave reduction; composition, the inverse and inversion; the named consonances, commas and small intervals (Pythagorean and syntonic commas, schisma, diaschisma, the two dieses, limma, apotome); the frequency of a tone from a reference; the beats of a mistuned consonance | draft: 53 checks |
| `tuning` | Tuning systems as scales within the octave. Built: chains of pure fifths (Pythagorean tuning, of any length, begun anywhere); 三分损益 and the twelve lü; just intonation in the 5-limit (Ptolemy's intense diatonic, a lattice of fifths and thirds, a just scale of twelve notes); meantone temperaments by any fraction of the syntonic comma; equal divisions of the octave; temperaments that circulate, stated by the tempering of each fifth (Werckmeister III, Vallotti); al-Urmawī's seventeen steps. Measured: the interval on every degree against a just one, fifths and thirds, the circle of fifths and the wolf, two tunings degree by degree, equal divisions against the just intervals and as homomorphisms of the 5-limit, the frequencies of a tuning's degrees | draft: 92 checks |
| `rhythm` | Durations as exact rationals, metre, cycles and their subdivisions | planned |
| `harmony` | Chords and their roots, measures of consonance and roughness, voice leading | planned |
| `form` | The parts of a piece and their relations | planned |
| `notation` | Names of notes and systems of notation, read and written | planned |
| `instruments` | Strings and pipes as sources of pitch: divisions of a string, frets, the correction of a pipe's length, inharmonic partials | planned |

## 3. Conventions

- **A ratio is a ratio of frequencies,** upper to lower, so that a rising interval is greater than 1: the fifth is 3/2. Lengths of strings and pipes are the inverse, and appear only where a method is stated in them (`sanfen_sunyi_lengths`), named as lengths.
- **Cents.** The size of an interval is 1200 log₂ of its ratio: the octave is 1200 cents, and cents add where ratios multiply. A departure from a just interval is positive when the interval is wide and negative when it is narrow.
- **The octave is 2 : 1** and every tuning repeats at it. A tuning lists its degrees from the first, the unison, in ascending order, without the octave above.
- **No standard pitch.** The reference frequency is always the caller's: no function assumes a value for any note, and none is a default.
- **Names of degrees** are places in a chain of fifths when the first degree is called C (… B♭ F C G D A E B F♯ …), or the names of the lü. They are not pitches. In a just lattice a name is followed by the number of syntonic commas by which the note stands from that place, as E(-1).
- **Historical tunings** are the schemes their sources state, not the practice of an instrument or a player; each constructor cites the source it follows.
- **Indices of tones.** For a tuning of n degrees, index i is degree i mod n in the octave ⌊i / n⌋ above the first degree's.

## 4. Dependencies

morphe (`arithmetic`: the integers, the rationals and the primality test) and calx (`verification`). No domain.

## 5. Code and Checks

Header-only C++20, each module a target `musicology::<module>`:
- `pitch/include/musicology/pitch/pitch.hpp`, namespace `musicology::pitch`;
- `tuning/include/musicology/tuning/tuning.hpp`, namespace `musicology::tuning`.

`node domains/musicology/tests/run-tests.mjs` builds and runs 145 checks against exact arithmetic and closed forms. No size in cents is taken from memory; each is computed in the check from the ratios it measures. Among them:
- twelve fifths against seven octaves are the Pythagorean comma, four fifths against a major third and two octaves the syntonic comma, and the schisma is the first less the second, all exactly;
- octave reduction, inversion and composition obey their laws, and intervals with rational exponents form a group;
- a chain of pure fifths never closes; its twelve notes are separated by seven limmas and five apotomes, with one wolf wherever the chain begins;
- 三分损益 gives the Pythagorean ratios exactly, and a thirteenth length falls short of the first by the Pythagorean comma;
- quarter-comma meantone's major third is exactly 5/4, its fifth 5^(1/4), and its eleven fifths leave the wolf 2^7 · 5^(−11/4);
- the fifth of twelve equal divisions is 700 cents exactly, and that of fifty-three is within 0.07 cents of pure;
- twelve, nineteen and thirty-one divisions temper out the syntonic comma and fifty-three do not; the syntonic comma and the lesser diesis generate all that twelve temper out;
- Werckmeister III and Vallotti close their circles exactly, and the temperament with equal fifths is the equal temperament;
- al-Urmawī's seventeen steps compose to the octave and are a chain of seventeen pure fifths;
- in every scale of twelve notes the fifths are narrow by the Pythagorean comma in all;
- beat rates against sums worked by hand.

## 6. Open Questions

- **Where the group of intervals belongs.** The factoring of a ratio into primes and the group of primes to rational exponents (`pitch::Interval`) name nothing of music. They are written here on morphe's integers, rationals and primality test because morphe has no factoring of its own yet; by the rule of reuse they should move to morphe's `arithmetic`, in a change of morphe's own.
- **Primes beyond 5.** The Greek genera and later just scales with 7 and 11 are held by the types and have no constructors yet.
- **The lü as pipes.** 三分损益 is computed on lengths of an ideal string. Pipes, their correction, and longer chains of lü wait for `instruments` and for sources read.
- **Temperaments that mix the two commas** are accepted by `circulating` as exact fifths; declared instances beyond Werckmeister III and Vallotti wait for their sources.
- **Later Arabic and Turkish theories** of the octave in twenty-four and in fifty-three parts, and scales known only by measurement, which need data with provenance from the data library.
- **Scales that do not repeat at the octave.**

## 7. References

Barbour (1951), *Tuning and Temperament: A Historical Survey*; Benson (2006), *Music: A Mathematical Offering*; Carey and Clampitt (1989); Cho (2003), *The Discovery of Musical Equal Temperament in China and Europe in the Sixteenth Century*; Ellis (1885); Hardy and Wright (1938), *An Introduction to the Theory of Numbers*; Helmholtz (1877), *On the Sensations of Tone*, tr. Ellis (1885); Lindley (2001), "Temperaments", "Mean-tone" and "Pythagorean intonation", *The New Grove Dictionary of Music and Musicians*; Needham (1962), *Science and Civilisation in China* IV.1; Ptolemy, *Harmonics*, tr. Barker (1989), *Greek Musical Writings II*; Werckmeister (1691), *Musicalische Temperatur*; Wright (1978), *The Modal System of Arab and Persian Music A.D. 1250–1300*. Full entries are in the theory note.
