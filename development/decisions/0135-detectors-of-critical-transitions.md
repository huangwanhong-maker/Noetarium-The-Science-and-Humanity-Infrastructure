# 0135. Detectors of Critical Transitions, for Every Kind of System and of State

- **Status:** Accepted, provisionally
- **Date:** 2026-11-19
- **Decided by:** the user. Asked whether climate criticality detection algorithms exist, or dynamical-system criticality detectors more generally, they answered: "I think this is an important infrastructure, no only in climate change detection, but also city goverance, like population critical transition, population/economics structural critical change / brain-dynamics disorder detection, etc. So it should support various dynamical system. So state variable is highly structural, right? and even multi-dimensional. Anyways, survey and proceed." Claude surveyed, designed and built it.

## 1. Context

kinesis held signs of criticality for one coordinate: rolling variance, autocorrelation and skewness with Kendall's τ against red noise, DFA, avalanches and the branching ratio (ADR 0103). The climate domain held only the classification of climates. The survey [Critical transitions in structured systems](../../research/literature/reports/Critical%20transitions%20in%20structured%20systems.md) covered the field in five parts written from the literature: the theory and its detectors on a record; structured and high-dimensional states; the Earth system, ecosystems and epidemics; brains and bodies; and cities, populations, economies, societies and infrastructure. About 75,000 words and 928 DOIs, most verified against their records. It found:

- **Detectors exist by mechanism, not by field.** Slowing down, the one family a theorem backs, precedes only local bifurcations of codimension one under slow forcing. Noise-induced tipping has only detectors that count states; rate-induced tipping needs a model of the forcing; non-smooth folds, cascades and percolation need structural measures.
- **Every multivariate indicator estimates one of three objects:** the lag-1 propagator's spectrum, the covariance's leading eigenpair, or a projection on the critical direction. Each goes silent in ways the structure predicts. Nothing published defines slowing down on compositions, distributions or covariance matrices.
- **The empirical record splits in two.** Controlled or physically guaranteed transitions are often anticipated: lake experiments, epidemic thresholds, traffic, power grids. Series chosen because a transition happened mostly are not: 9–13% true positives in field ecology; no slowing before financial crashes; conflicting results before seizures; tipping times of the Atlantic overturning spanning millennia. A detector must report its error rates, and an absence only where it could have seen a presence.

## 2. Decision

1. **kinesis's module `transitions`** (its decision 0008; 77 checks):
   - **Charts.** A state space declares a chart into linear coordinates: identity; isometric log-ratios for compositions, with each unit's log total; quantile functions for histograms; the whitened matrix logarithm for covariances. It may also declare a neighbourhood and a hierarchy of its units. One battery serves every kind of state.
   - **Indicators** of one coordinate, of many, and on a neighbourhood. Each says what it estimates, sees, is blind to and assumes. On a hierarchy, every level is read in its totals and in its contrasts within groups.
   - **The test** is of the change between the first and the last window against the stationary linear process fitted to the residuals. That null and a declared alternative (the leading mode slowing to a tenth) give the false-alarm rate, detection rate and ROC area at the record's length. Further nulls and a grid of windows and bandwidths check robustness.
   - **Verdicts** are signal; no signal, only where the test was powered and the change is small under the alternative; or undecidable, with every reason. Among the reasons: a red noise that the red-noise rate does not confirm.
   - **Further detectors:** restoring rates by four estimators; the dominant eigenvalue's path; where the leading mode shows and what excites it; wells; a likelihood test that gives a tipping time only when bounded.
   - **Test models with known answers**, positive and negative.
2. **The climate domain's first C++** (31 checks): `energy_balance` (Budyko and Sellers, with a ramp albedo whose folds give no slowing and a smooth one whose folds do) and `overturning` (Stommel and Cessi in Berglund and Gentz's form: the folds y± = 2/3 ± √(η² − 3)/(3η), the interval [0.964, 1.480]). Its checks read the models' records with kinesis's detectors.
3. **The API's `science/transitions.hpp`** (`assess`, `likelihood`, `wells`, `model`, `climate`), answered by the Dynamics Lab's engine as `transitions.<op>` and on a run as `transitions`, with `science.transitions` for scripts (23 checks).
4. **The visualization layer's `dynamics/transitions.js`** (29 checks): a verdict by glyph and word; an indicator's windows with the change tested; the eigenvalue's path against the circle; the tests' error rates; the grid; where the mode shows; the wells.
5. **The Dynamics Lab:**
   - two tools, *Critical transitions* on a run and *Critical transitions in a record* (as one state, or as a composition with its total);
   - an example of six systems: a fold; noise alone; reddening noise; a branch outrun; an oscillator; a ring of units;
   - 7 checks in its suite (87).

## 3. Alternatives Considered

- **A new foundation.** Rejected: the detectors study what a system does over time from its record, which is kinesis's scope (its decision 0002).
- **Kendall's τ as the test.** Rejected after the checks: with windows of half the record, a stationary record's windowed series is monotone too often for τ to have power. τ is reported.
- **"No signal" whenever the test was powered.** Rejected after the Lab's example. A ring that slowed, but less than the alternative, was called no signal at p = 0.06. No signal now also asks that a change so small be rare under the alternative.
- **Trusting variance and autocorrelation when they rise together.** Rejected after the example. Noise that reddens raised both, and the AR(1) rate fell with them. Where the noise is red, a slowing the red-noise rate does not confirm is undecidable.
- **Learned classifiers, finite-size scaling, structural thresholds for cascades, persistent homology, transfer operators in windows.** Deferred; kinesis's theory note lists them.

## 4. Consequences

- **What can now be asked of any record,** in any of the infrastructure's works: whether it shows a loss of stability, of what kind, where in its state, how sure the test is at its length, and what no detector could have seen. A city's records by block and district, a population by age, an economy by sector, a multichannel recording: each is a space with a chart, a neighbourhood and a hierarchy.
- **The verdicts are honest about the field's limits.** A short record is undecidable, never "no signal". A noise that changes is not slowing down. A tipping time is given only when bounded.
- **Found while building:**
  - Detrending with a bandwidth near the correlation time at the transition removes part of the slowing.
  - A state cannot follow a ramp faster than its relaxation, so its variance lags the quasi-static value (the Lab's first ring).
  - The red-noise rate needs the noise's correlation resolved by the sampling. Its record identifies the pair of rates, not which is the system's.
- **Not built:**
  - demonstrations on open records (the NTL lakes, EPICA, a city's records by block, scalp EEG);
  - a work of the Urban Studio for a city's records;
  - the brain's records;
  - North's diffusive energy balance as a field;
  - the climate-risk domain.
- **Provisional:** the module's name; the default window, bandwidth, alternative and power; the bounds for red noise (0.75 of white noise's residual, θΔt < 3).
