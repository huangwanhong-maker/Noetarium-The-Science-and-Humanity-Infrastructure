# 0111. Musicology and Philosophy Founded

- **Status:** Accepted, provisionally (2026-11-13).
- **Date:** 2026-11-13
- **Decided by:**
  - **The request.** For the history archive and learning infrastructure (ADR 0110), the user set the first step: "only consider philosophy history and music history". Asked whether to found musicology and philosophy as domains and build a new work, they answered "of course".
  - **The design.** Claude, here; each domain's code was written by a helper of the same session on a brief, and its checks run again here.

## 1. Context

A record of historical objects says that a tuning or a doctrine was, and what it rested on. It does not hold the thing itself: what the fifths of a tuning are, or how an argument stands against another. Those belong to the sciences of the things. Musicology and philosophy were rows of the README's §4, each a field at priority 4, with no code.

## 2. Decision

1. **Musicology is a field at height 0**, on calx and morphe, C++ from the start. Its first modules:
   - `pitch`: an interval as an exact frequency ratio (morphe's rationals) or, when tempered, a product of primes to rational exponents, so that tempered intervals are compared exactly; cents; the named commas; the frequency of a degree from a reference the caller states; the beating of a mistuned consonance.
   - `tuning`: a tuning as its degrees within the octave. Pythagorean tuning by a chain of fifths; sanfen sunyi (三分損益), which is found to give the Pythagorean ratios exactly; Ptolemy's intense diatonic and a just scale of twelve; meantone by any fraction of the comma; equal divisions; well temperaments by how each fifth is narrowed (Werckmeister III, Vallotti); al-Urmawī's seventeen steps. Each fifth and third against the pure interval, the wolf, two tunings side by side, the best equal divisions, the tones to sound.
2. **Philosophy is a field at height 0**, on calx and morphe, C++ from the start. Its first module, `arguments`:
   - abstract argumentation after Dung: conflict-free and admissible sets, the grounded extension as a least fixed point with the round at which each argument is settled, complete, preferred and stable extensions by enumeration over the undecided, refused above a stated size; supports only through the attacks they induce;
   - argument maps after IBIS: questions, the positions that answer them, arguments for and against; every fault with where and why; the framework a map induces; each position's standing and the open questions; a written form of lines, since no reader of JSON is shared below the domains.
3. **Neither names a history.** No date, no attribution beyond the name a method is known by. What appeared when, and why, is the history domain's record; a work joins the two.
4. **The API composes them** for any work: `science/sounding.hpp` and `science/arguments.hpp`.
5. **No pitch is a default.** The caller states the frequency of the first degree.

## 3. Alternatives Considered

- **Modules of the history domain.** A tuning is no history, and other works will need it with no record at all.
- **One domain of the arts.** The README's §4 has them apart, and their methods share nothing yet.
- **A bridge "history of music".** No code of its own would stand in it: a record is data, and the history domain reads it whatever its discipline.
- **Floating-point intervals.** "The circle closes" and "the meantone third is pure" would then hold to a tolerance. With exact exponents they are equalities.

## 4. Consequences

- **Musicology:** 145 checks against exact arithmetic and closed forms (twelve fifths against seven octaves the Pythagorean comma; quarter-comma meantone's third exactly 5:4 and its wolf; sanfen sunyi the Pythagorean ratios; every well temperament's circle closing). Its theory note is `domains/musicology/theory/tuning-and-temperament.md`.
- **Philosophy:** 90 checks, the semantics held by brute force on all 66,067 attack relations of up to four arguments. Its theory note is `domains/philosophy/theory/arguments.md`.
- **Thirty domains have code.** The order's height 0 gains two.
- **Waiting to move to morphe**, each said in its domain's README: the group of primes to rational exponents with its factoring (morphe's `arithmetic`); the fixed points of a function on the subsets of a graph (morphe's planned `logic`). The cycle of "raised by" in a map could use morphe's new `acyclic.hpp`.
- **Stated with care:** two exclusive positions that each have an unanswered argument for them both fall under the grounded semantics with supported attacks; the question is open either way. It is a caveat of the reading chosen, checked and written down.
- **Not built:** rhythm, harmony, form, notation and instruments; formal validity, concepts, doctrines across texts, ethics. References' page ranges in the READMEs are from memory and are to be checked before they enter `docs/references.bib`.
