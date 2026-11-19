# Critical Transitions in Structured Systems: Their Detection Across the Sciences. A Survey

*A survey of the theory of critical transitions and of the detectors that read them from records (critical slowing down and its estimators, model-based and learned detectors, finite-time singularities, and the indicators of criticality in cascades of events), for states that are scalars, vectors, fields on grids and graphs, networks, compositions and distributions, matrices, hierarchies across scales and multichannel signals; of their empirical record in the Earth system, ecosystems and epidemics, in brains and bodies, and in populations, cities, economies, finance, societies and infrastructure; of the test models whose answers are known and the open records with their terms; and of what a detector must report, when it must refuse, and what it must never claim.*

Status: `draft` (a survey for design, 2026-11-19). Activity: research, in service of use. It serves [ADR 0135](../../../development/decisions/0135-detectors-of-critical-transitions.md) (the infrastructure's decision on detectors of critical transitions) and kinesis's decision 0008 (the dynamical-systems foundation's detectors).

## 0. Scope, Method and Markers

### 0.1 The Question

The request was for detectors of criticality for climate change; for the governance of cities, that is, for critical transitions of populations and for the structural change of populations and economies; for disorders of brain dynamics; and for other systems. The detectors are to support dynamical systems of every kind whose state variables are highly structured and multi-dimensional. The survey therefore asks three things. First, whether detectors of criticality and of critical transitions exist for dynamical systems of every kind: climate, cities and populations, economies and finance, brains and bodies, ecosystems and epidemics. Second, how an infrastructure should build them when a system's state is a field on a grid or a graph, a network, a composition or a distribution, a hierarchy across scales, a matrix or a multichannel signal. Third, for each detector, what it can see, what it is blind to, how often it is wrong, and when it must refuse.

*Remark.* The literature uses "criticality" for three different things, and the survey keeps them apart:

- (a) the approach to a **critical transition** (a tipping point) of a system driven slowly towards a bifurcation;
- (b) a **critical point** in the sense of statistical physics, a continuous phase transition with a diverging correlation length, of which the "critical brain" is a claimed instance (Part IV §1) and finite-size scaling the classical test (Part II §7);
- (c) a **finite-time singularity** of positive feedback: material failure, eruptions, landslides, bubbles (Part I §7; Part V §2, §4).

They share some statistics (the branching ratio of a counting process is the AR(1) coefficient of the same linear process, Part I §2), but not their theorems.

### 0.2 How the Survey Was Made

Five surveys were written in parallel from the literature on 2026-11-19, each by a separate agent to one brief. They are kept whole as Parts I–V:

- Part I, the theory of critical transitions and the detectors that read them from scalar and low-dimensional records;
- Part II, structured and high-dimensional states;
- Part III, the Earth system, ecosystems and epidemics;
- Part IV, brains and bodies;
- Part V, populations, cities, economies, finance, societies and infrastructure, and the use of warnings in governance.

Each Part follows one form. Every section has a Takeaway, Cited Findings, Inferences and Gaps, so that what a source says is kept apart from what the survey infers from it, and each Part closes with what its findings mean for the design. Records were retrieved from Crossref's API, the arXiv API, PubMed (NCBI E-utilities), Semantic Scholar, OpenAlex, Europe PMC, CRAN's metadata, PyPI's JSON, GitHub's API and providers' own pages. Some publisher pages (nature.com, PMC, eLife) were blocked by redirects or captchas; Crossref throttled batches shared between agents; GitHub's API was rate-limited for some packages. Several abstracts therefore came through OpenAlex or Europe PMC, and some licences remain unchecked; each Part says which.

Section 1 is a synthesis, written after reading all five Parts. It adds no reference that is not in them and cites the Parts (for example "Part II §1") rather than repeating their references. Inside a Part, a bare section reference (for example "§1.4" in Part III) means that Part's own section, not a section of this synthesis. The Parts overlap by design: Boettiger & Hastings 2012 and Ditlevsen & Ditlevsen 2023, for instance, appear in several. The discrepancies found between them are listed in §0.4. Part VI is reserved for what is built.

### 0.3 Verification Markers

- **[V]** verified this session (2026-11-19) against a primary record: Crossref, arXiv, PubMed, Semantic Scholar, OpenAlex, CRAN, PyPI or GitHub, or a provider's own page.
- **[S]** confirmed only from a secondary source: a search summary, a review, a mirror, a news release.
- **[M]** from memory, unverified.

A marker attests the bibliographic record (authors, year, title, venue, DOI), not the claim. Statements of content say separately whether they come from the full text, an abstract, a secondary summary or memory. The Parts use slightly different codes for this, and each Part's opening remark states its own. Formulas written from memory are flagged where they are given ("from memory", *[formula M]*, "(eq. M)"), and formulas derived in the notes are marked "derived here". Neither kind should become a test fixture before it is re-derived, or checked against its source or a reference package (Part I §9, Part III §6 and Part IV §9, each under Gaps).

**Counts.** Table 0.1 gives the occurrences of each marker in each Part, counted by pattern on the notes.

- Repeated citations of one work are counted each time, so the counts are of occurrences, not of distinct works.
- They are approximate, to within a few per Part, because some markers are written in variant forms ("[V record]", "[V via PubMed]", "{V, crandb}", "[S, record via OpenAlex]").
- The [M] column counts bracketed record markers only. Content from memory is counted in the last column, by each Part's own code.

**Table 0.1.** Markers per Part (approximate occurrences).

| Part | [V] | [S] | [M] (records) | Content flags (approximate occurrences) |
|---|---|---|---|---|
| I | ≈ 200 | 2 | ≈ 16 | {txt} ≈ 32, {abs} ≈ 72, {mem} ≈ 39; "from memory" ≈ 37 |
| II | ≈ 210 | ≈ 15 | ≈ 25 | "(abstract)" ≈ 82; full text ≈ 6; "content [M]" ≈ 27; *[formula M]* ≈ 19 |
| III | ≈ 265 | ≈ 24 | ≈ 6, plus ≈ 11 DOIs or pages "from memory" | "content M" ≈ 84; "eq. M" ≈ 15 |
| IV | ≈ 285 | ≈ 28 | ≈ 4 | "(abstract)" ≈ 27; "from memory" ≈ 87 |
| V | ≈ 355 | ≈ 12 | ≈ 4, plus a list of unverified licences | "(abstract)" ≈ 81; "from memory" ≈ 94 |

*Caveat.* The share of content from memory matters more than the count of [V]. The records are mostly verified, but much of the content of Parts III–V is from memory or from abstracts. Where a design choice rests on a claim marked as memory, it should be checked before it is relied on.

### 0.4 Corrections Found While Surveying

1. **Drake et al. 2019.** A brief cited "Drake et al. 2019, Nature" for epidemic reemergence. No such paper exists. The matching paper is Drake et al. 2019, *PLoS Computational Biology* 15:e1006917, "The statistics of epidemic transitions", which treats emergence and reemergence as a critical transition seen through noisy dynamic bifurcation (Part III §4.1, Gaps).
2. **Ditlevsen & Ditlevsen 2023's interval.** The estimated collapse of the AMOC was published at about 2057, with a 95% interval of 2025–2095 (Part I §5, from the full text; Part III §1.2). An Author Correction was published in 2025 (record [S], DOI 10.1038/s41467-025-63201-y). Morr et al. 2026 quote the interval as 2037–2109, which suggests that the correction changed it, but the correction was not read. Either way, Ben-Yami et al. 2024 and Morr et al. 2026 argue that the tipping time is not identified from historical data (§1.4).
3. **Gijzel et al. 2017.** A Crossref query for Gijzel et al. 2017's paper "Resilience in clinical care" (*J. Am. Geriatr. Soc.*) returned no correct match. The item is kept as [M] and was not attached to the record Crossref offered. The verified Gijzel et al. 2017 record is a different paper, *J. Gerontol. A* 72:991–996, on frailty and self-rated health (Part IV §5).
4. **Boettiger & Hastings 2012: letters and volume.** The letters differ between Parts:
   - Part I calls the *J. R. Soc. Interface* paper (doi 10.1098/rsif.2012.0125; limits to detection, the likelihood-ratio test and ROC curves) "2012a", and the *Proc. R. Soc. B* paper (doi 10.1098/rspb.2012.2085; the prosecutor's fallacy) "2012b";
   - Part III uses the opposite letters;
   - Part V gives the *Proc. R. Soc. B* volume as 280, where Parts I and III give 279:4734–4739.

   The DOIs are authoritative. Section 1 names the two papers by venue.
5. **Records verified in one Part and only remembered in another.** The stronger marker holds; the Parts are left as written.
   - Ben-Yami, Morr, Bathiany & Boers 2024, *Sci. Adv.* 10:eadl4841: [V] in Parts I and III; [M] in Part II, which found only an EGU abstract.
   - Boulton, Lenton & Boers 2022, *Nat. Clim. Change* 12:271–278: [V] in Part III (doi 10.1038/s41558-022-01287-8); only the Research Square preprint record in Parts I and II.
   - Huang, Bathiany, Ashwin & Boers 2024, *Nat. Mach. Intell.*: [V] in Part I; [M] in Part II.
   - Deb et al. 2022 (EWSNet), *R. Soc. Open Sci.*: [V] in Part I; [M] in Part II.
   - Lapeyrolerie & Boettiger 2023, *Methods Ecol. Evol.*: [V] in Part I; [M] in Part II.
   - Gidea et al. 2020 (cryptocurrencies), *Physica A*: [V] in Part V; [M] in Part II.
   - Dakos et al. 2008, *PNAS*: [V] in Part I; [S] in Part III.
   - Suweis & D'Odorico 2014, *PLoS ONE*: [S] in Part III; [M] in Part II, which verified only the bioRxiv preprint.
6. **Spatial early warnings on networks.** Part II cites MacLaren, Aihara & Masuda, *J. R. Soc. Interface* 22:20240696 (2025), and Part V cites arXiv:2410.04303 (2024) by the same authors on the same question. Both are verified, and they appear to be the preprint and the article of one study; this was not checked.
7. **Text reconstructed from damaged PDFs** (Part I §2, §4, §5). Conversion to text lost glyphs:
   - in Kuehn 2013's variance laws: the square root in the cusp law, restored from his Table 1;
   - in Boettiger & Hastings's model equations, which were reconstructed.

   Ditlevsen & Ditlevsen's waiting time for noise-induced tipping was re-derived from Kramers's formula because their expression was garbled. All three should be checked against the typeset papers.
8. **Items unresolved rather than corrected** are listed in each Part's Gaps. Among them:
   - which methodological choice Hengen & Shew 2025 say resolves the controversy over brain criticality;
   - the content of Sipling et al. 2026;
   - the seizure-forecasting metrics of Maturana et al. 2020;
   - the frequencies of the dynamotypes in Saggio et al. 2020;
   - the TRANS-ID recurrence figure (32.9%, [S]);
   - the DOI of Maschke et al. 2024;
   - details of the ENTSO-E report on the Iberian blackout, on which secondary sources disagree;
   - the DOI of the EWSmethods paper, inferred from review records.

## 1. What the Five Parts Say Together

The answer to the first question is yes and no. Every field surveyed has a body of indicators, applications and critiques (Parts III–V). But detectors exist for mechanisms of transition, not for fields:

- **Bifurcation-induced tipping** has a family of detectors backed by a theorem, critical slowing down (CSD), with known blind spots and measurable error rates.
- **Noise-induced tipping** has only detectors of how many states there are and how close the noise is to switching between them.
- **Rate-induced tipping** is detectable only with a model of the forcing. The detectors claimed to work from records alone are unreplicated research.
- **Shocks, non-smooth folds, saddle escapes, percolation and cascades** need structural or model-based measures, not CSD.
- **Continuous phase transitions** have finite-size scaling.
- **Finite-time singularities** have power-law acceleration fitted on the right likelihood.

When the state is structured, every indicator in the literature reduces to one of a few functionals of a windowed covariance, a propagator (or transfer or Koopman matrix), a neighbourhood operator or a scale axis. Its blind spots can therefore be stated from the structure before any data are seen, provided the state space declares its chart, metric, neighbourhood, hierarchy and constraints.

The empirical record splits cleanly:

- **Controlled, replicated or physically guaranteed transitions** are often anticipated: whole-lake experiments, laboratory populations, epidemic thresholds, traffic, power grids, anaesthesia.
- **Records chosen because a transition happened** mostly give false positives, low agreement among indicators, or results that depend on processing: field ecology, financial crashes, seizures in long-term recordings, mood, paleoclimate.

An infrastructure should therefore build detectors whose assumptions are explicit and whose behaviour has closed forms. Every result should carry its null, its false-alarm and detection rates at the record's length, and a three-valued verdict. It should refuse to extrapolate a tipping time unless a stated model allows it.

### 1.1 Transitions Are Classified by Mechanism, and Only One Class Carries a Theorem of Warning

**The classes.** Ashwin, Wieczorek, Vitolo & Cox (2012) fixed the classification now used everywhere (Part I §1):

- **B-tipping:** a quasi-static parameter passes a bifurcation of the frozen system;
- **N-tipping:** noise carries the state across a basin boundary while the attractor still exists;
- **R-tipping:** the parameter moves faster than a critical rate, so that the state fails to track a branch that never bifurcates.

Later work adds shock tipping, tipping under periodic or chaotic forcing, tipping between chaotic attractors and fractality-induced tipping (Part I §1).

**The theorem.** CSD is a theorem only for B-tipping through a local codimension-one bifurcation of the fast subsystem, under timescale separation and small additive noise (Kuehn 2011, 2013; Part I §2).

- Near such a point the linear restoring rate κ goes to zero as a power of the distance δ to the bifurcation, the stationary variance is σ²/(2κ) and the autocorrelation time is 1/κ.
- Kuehn's Theorem 5.1 gives the variance laws: σ²/√y for the fold, σ²/y for transcritical and pitchfork points, and σ²/y₂ for the cusp. Theorem 5.2 gives O(1/y) for each coordinate at a Hopf point.

**The signatures differ by type** (Part I §2, table):

- a fold adds skewness towards the far state, and flickering when noise is large;
- a Hopf point gives a spectral peak at ω₀ of width κ, and a lag-1 autocorrelation e^{−κΔt}cos(ω₀Δt) that *falls* when ω₀Δt > π/2;
- a period-doubling drives the lag-1 autocorrelation to −1;
- a Neimark–Sacker point moves a complex pair of multipliers onto the unit circle;
- global bifurcations (SNIC, homoclinic) have no local κ → 0, only a lengthening period;
- boundary crises, border collisions and other non-smooth transitions are silent.

**Slow passage and noise.** A slow passage at sweep rate ε jumps O(ε^{2/3}) past the static fold. Noise usually tips the system *before* the bifurcation, at a rate given by Kramers's formula. For Ditlevsen & Ditlevsen's model:

τ_n(λ) = (π/√(A|λ|)) exp(8|λ|^{3/2}/(3σ²√A)), re-derived in Part I §5.

So the bifurcation's parameter value is an upper bound on the tipping time, not the tipping time (Part I §1, §5).

**Transitions with no generic precursor.** The Parts document the following.

- **R-tipping at constant frozen stability.** The derived case ẋ = (x + rt)² − 1 has a frozen eigenvalue fixed at −2 and a critical rate r_c = 1 exactly, so CSD indicators must show no trend for constant r < 1 (Part I §1). Indicators lag the fastest drift (Ritchie & Sieber 2016). R-tipping can be established with a model of the forcing, by critical-rate analysis or by the basin instability of O'Keeffe & Wieczorek 2020 (Part I §9, Gaps). The record-only detectors (the deep-learning framework of Huang et al. 2024, and the residual-Koopman preprint of Nathaniel et al. 2026) are unreplicated (Part I §6; Part II §6).
- **N-tipping at fixed parameters.** Dansgaard–Oeschger events were read as noise-induced with "very limited predictability" (Ditlevsen & Johnsen 2010; Part I §1). Seizure onsets by a noise-driven jump across a separatrix give no precursor (Part IV §2). Revolutions driven by hidden heterogeneous thresholds and preference falsification give no observable warning in aggregate records (Kuran 1989, 1991; Part V §5).
- **Shock tipping.** The minimal fatal shock and basin stability need a model of the state space, not a record (Part I §1, §3.6).
- **Non-smooth folds.** Systems without smooth potentials give no warning (Hastings & Wysham 2010). In the piecewise-linear energy-balance model the linear rate is exactly −B/C on each stable branch until the branch meets the kink, so the relaxation time is constant up to the fold. Stommel's |Δρ| and Welander's convective flip-flop behave the same way (Part III §2).
- **Silent catastrophes in structured populations.** Commonly used warnings can be silent because of the direction of the critical eigenvector (Boerlijst, Oudman & de Roos 2013; Part II §1.5).
- **Saddle escapes.** Non-bifurcative saddle-type transitions are frequent in high dimension: the state lingers near a saddle, looks stable, and escapes (Kuehn, Zschaler & Gross 2015; Part I §2; Part II §3.2).
- **Percolation and cascades.** Explosive percolation, first-order collapse of interdependent networks, and contagion in "robust-yet-fragile" financial networks are distances to a structural threshold, not vanishing eigenvalues (Part II §3.4; Part V §4, §6).
- **Oscillatory regimes and crises.** Endogenous business cycles as limit cycles beyond a Hopf point imply periodicity, not CSD before each recession (Beaudry, Galizia & Portier 2020; Part V §3). The same holds for spontaneous millennial oscillations (Vettoretti et al. 2022; Part III §1.9) and for the crisis of a chaotic attractor (Part II §6).
- **Forcing faster than relaxation.** Before Europe's second COVID-19 wave the indicators *decreased* (Dablander et al. 2022; Part III §4).
- **Evasion and removal.** Spatial self-organisation can let a system evade tipping (Rietkerk et al. 2021). The bistability of simple sea-ice models disappears in fuller ones (Wagner & Eisenman 2015; Part III §1.7).

**Specificity.** CSD also precedes non-catastrophic transitions (Kéfi et al. 2013). A CSD signal answers "is a linear restoring rate trending to zero?", never "is a catastrophe coming?" (Part I §2).

**Unification.** The criticality detectors of neuroscience and infrastructure belong to this family: they are not separate theories. The branching ratio m of a linear branching process with immigration is its dominant eigenvalue, m → 1 is a transcritical bifurcation of that process, and the autocorrelation at lag k is m^k (Part I §2; Part IV §1.3). The same holds for the mean offspring of cascades in outage logs (Part V §6) and for the branching ratio of a Hawkes fit to crime (Part V §2).

**Table 1.1.** The dominant mechanisms of transition in each field, and what can be warned of.

| Field | Dominant mechanisms in the record | What can be warned of | Part |
|---|---|---|---|
| Climate | B claimed (AMOC, Greenland, Amazon); N for DO events; R for meltwater forcing of the AMOC; non-smooth folds and bifurcations removed by complexity (sea ice) | Whether a restoring rate is declining, with care; not when tipping will occur | III §1–2 |
| Ecosystems | B in experiments (lakes, *Daphnia*, yeast, cyanobacteria); patterns that evade tipping; structured populations that collapse silently | Experiments: yes. Observational series: mostly false positives | III §3 |
| Epidemics | Transcritical at R₀ = 1 (emergence, elimination), the cleanest B case, with closed forms | Yes, when forcing is slow relative to relaxation | III §4 |
| Brains | (b) for healthy dynamics (near-critical, "reverberating"); seizures as bursters in 16 onset × offset classes, some with precursors (SN with drift, supercritical Hopf) and some without (subcritical Hopf, noise); termination | Risk cycles and distance to criticality; single seizures rarely; termination | IV §1–4 |
| Bodies | Slowing of recovery; period-doubling (alternans); loss of variability; uncoupling | Research only; one randomised success (neonatal heart-rate characteristics), not CSD per se | IV §5 |
| Cities | Segregation thresholds; traffic jamming; Turing-type crime hotspots; slow, ratchet-like decline | Traffic: yes, and repeatedly; segregation: contested; decline: not as a fold | V §1–2 |
| Economies and finance | Regime switching without slowing; endogenous cycles; explosive bubbles; slow build-up of leverage before a fast collapse | Structural slow variables (credit gap) beat dynamical indicators; crashes show no rising autocorrelation | V §3–4 |
| Societies | Threshold cascades; norms that tip at a critical mass; collapses known only retrospectively | Structural risk forecasting (conflict); CSD only retrospectively on proxies | V §5 |
| Infrastructure | Fold of voltage collapse; oscillatory modes; cascades; heavy-tailed noise | CSD has a physical basis in grids, combined with structural measures | V §6 |

### 1.2 Every Multivariate Indicator Estimates One of Three Objects of the Linearised Problem

**The linear model.** The multivariate theory rests on the linear stochastic model dx = Jx dt + B dW, with J stable (Part II §1.1).

- The stationary covariance C solves JC + CJᵀ + BBᵀ = 0, and the lagged covariance is C(τ) = e^{Jτ}C.
- Sampled at step Δ, the propagator is A = e^{JΔ}, estimated by Â = Ĉ(1)Ĉ(0)⁻¹.
- If the critical eigenvalue λ₁ → 0⁻ is simple, with right eigenvector v and left eigenvector w (wᵀv = 1), then

  C = (wᵀBBᵀw)/(2|λ₁|) · vvᵀ + O(1).

  The left eigenvector decides whether noise excites the critical mode at all; the right eigenvector decides where in the state it appears (Masuda 2026, abstract).

**The three objects.** Every multivariate indicator surveyed estimates one of three things (Part II §1, Inferences):

1. the **spectrum of A**: decay rates −ln|μ|/Δ and frequencies arg μ/Δ;
2. the **dominant eigenpair of C**: variance concentrated along one direction;
3. a **projection pᵀx onto an estimate of v**: PCA, MAF, degenerate fingerprinting along the leading EOF, the dominant group of a dynamical network biomarker, the "best indicator" species, or the effective coordinate of a network reduction.

They agree when J is normal and the noise isotropic. Then C = −(σ²/2)J⁻¹, the eigenvalues of C are σ²/(2|λᵢ|), and the leading eigenvector of C is v. They disagree exactly when J is non-normal (widespread in empirical networks) or the noise is anisotropic (Part II §1.1; Part IV §6). A detector must therefore declare which object it estimates.

**When indicators go silent.** Each of the following conditions can be stated before any data are seen.

- **A projection.** A projection pᵀx shows divergent variance only if pᵀv ≠ 0 and wᵀBBᵀw > 0. A detector on fixed observed coordinates is blind to critical modes orthogonal to them. Boerlijst's silent catastrophes are the special case in which the observed variable lies near v⊥, and a model with wᵀB = 0 is a silent catastrophe that every variance-based detector must miss (Part II §1, §11.3).
- **A non-normal Jacobian.** The leading eigenvector of C is then no longer v. Transient amplification can raise variance with no loss of stability, so covariance-based indicators can mislead where propagator-based ones do not (Part II §1.1; Part IV §6).
- **A delocalised critical mode.** The dynamical network biomarker assumes a critical eigenvector supported on a small group. For a delocalised mode the contrast between within-group and between-group correlation vanishes (Part II §1.4).
- **A uniform mode on a graph.** On a connected graph with dx = −(γI + κL)x dt + σ dW, the Laplacian modes have variance σ²/(2(γ + κλ_k)). As γ → 0 only the constant mode diverges, and spatial variance, which subtracts the mean, removes exactly that mode and stays bounded; the mean diverges instead. This is the closed form behind two results:
  - strong spatial coupling makes spatial warnings disappear while temporal ones remain (Clarke et al. 2026);
  - spatial variance diverges only when the critical direction is noise-excited and non-uniform after centring, while the coefficient of variation saturates and skewness, kurtosis and Moran's I approach limits that depend on the network (Masuda 2026).

  (Part II §2.)
- **Aggregation across units.** Summing a block keeps each mode's projection on the block's constant vector. Aggregation therefore sharpens CSD when the critical mode is locally uniform and erases it when the mode is a dipole within blocks. The aggregate's lag-1 autocorrelation rises only once the critical mode dominates its variance (Part II §7). French unemployment shows both sides: the national aggregate gave "no clear signal before recessions", while a moving-window PCA and Moran's I across 96 départements preceded 2008 (Halleck Vega et al. 2025; Part II §2.6).
- **Estimating Â near the transition.** Â needs a window T ≫ n. It is ill-conditioned precisely near the transition, because variance concentrates on one direction and Ĉ(0) becomes nearly singular. Shrinkage or reduced rank is required, and a detector must refuse when the effective sample size per parameter is too small (Part II §1).
- **Multiple testing over nodes.** The maximum of node-wise indicators inflates false alarms (n correlated tests per window), and the mean dilutes localised signals. Both need surrogates that preserve cross-correlation, such as fitted VAR(1) surrogates or multivariate phase randomisation (Part II §1).

**The operator view.** The Ulam transfer matrix on a partition, DMD/EDMD on a dictionary and the VAR(1) on linear coordinates are one object: the Koopman/transfer semigroup restricted to a function space, in three bases. Exact DMD on pairs (x_t, x_{t+1}) is the VAR(1) least-squares estimator up to truncation (Part II §6).

- Operator spectra generalise CSD from fixed points to attractors. They see oscillatory slowing (a complex μ), the approach to a homoclinic bifurcation (a falling frequency at a fixed decay rate, after Williamson & Lenton 2015) and crises (a Ruelle–Pollicott resonance approaching the imaginary axis while no fixed point loses stability).
- Through the fluctuation–dissipation relation the response operator equals C(s)C⁻¹, so degenerate fingerprinting is a measurement of response (Lucarini & Chekroun 2024), and the static susceptibility −J⁻¹ diverges along vwᵀ/|λ₁| (Part II §6.3).
- The cost is heavy data and bias. Ulam's method needs many visits to each cell, and a non-Markovian reduction biases the spectral gap (Tantet et al. 2015; Part II §6).

**Information and thermodynamics** (Part II §9). In the Gaussian linear regime, mutual information and transfer entropy are functions of C and Â and add nothing. Their value lies in non-Gaussian regimes and in directed coupling: which component drives the critical mode, a data estimate of the support of w. Information flow in the kinetic Ising model peaks on the disordered side, so such indicators must be read by trend, with the side stated (Barnett et al. 2013).

### 1.3 An Indicator on a Structured State Must Respect Its Chart, Metric, Neighbourhood, Hierarchy and Constraints

**The abstraction.** Part II §11 proposes an abstraction over state spaces that the other Parts' recommendations fit (Part IV §6 and §9.1; Part V §8). A state space declares six things:

1. **A chart** φ: 𝒳 → ℝ^d into linear coordinates, with its inverse where defined. The cases are:
   - the identity (vectors, fields);
   - isometric log-ratio coordinates in a stated orthonormal basis (compositions);
   - a quantile grid (distributions on the line);
   - the matrix logarithm after whitening by a reference (covariances);
   - the Laplacian eigenbasis (signals on graphs);
   - a dictionary of observables (operator methods).
2. **A metric** on chart coordinates, with a statement of whether the chart is an isometry.
   - Euclidean after ilr is Aitchison's metric.
   - Frobenius after the matrix logarithm is the log-Euclidean metric.
   - The affine-invariant metric is obtained by whitening at a reference point.
   - ilr and one-dimensional quantiles are isometries; the SPD tangent space is one only locally.
3. **A neighbourhood:** a weighted graph W on units (a lattice, contiguity, a distance band, flows), or none. It provides the Laplacian L, Moran's weights and the graph Fourier basis.
4. **A hierarchy:** levels with carry operators labelled by what they keep and what they remove.
   - `sum` and `mean` pass the uniform mode.
   - `share`, `logit` and `zscore` remove it; their output is compositional or centred.
   - `laplacian` removes constants.
   - `recompute` is nonlinear.
5. **Constraints:** simplex closure (with a policy for zeros), positivity, unit diagonal (correlation as against covariance), and invariance under permutation of units.
6. **Units and scale of measurement**, so that indicators invariant under rescaling are preferred where units are arbitrary.

A detector, in turn, declares five things:

- its **estimand**: the spectrum of Â; the dominant eigenpair of Ĉ; a projection; a neighbourhood statistic; a scale statistic; or a structural threshold;
- what it **sees**: fold, transcritical and pitchfork points by a real μ → 1; a Hopf point by a complex |μ| → 1; a homoclinic approach by a falling frequency; a Turing instability by k* > 0 in the dispersion relation; a continuous phase transition by finite-size scaling; percolation by a structural threshold;
- what it is **blind to**;
- its **null model**;
- its **conditions of refusal**.

*Remark.* In algebraic terms, a critical mode v reaches a level of the hierarchy through a carry operator T exactly when v ∉ ker T. The level channel (block sums) has as its kernel the patterns of zero sum within each block, so it is blind to dipoles. The share channel (closure, centred log-ratios, z-scores among siblings, the Laplacian) contains the uniform direction in its kernel, so it is blind to a common factor. A transition carried by a common factor across units is invisible in shares, and one carried by redistribution is invisible in sums. A detector on a hierarchy must therefore test both channels at every level and report which one carries the signal (Part II §7, §11.1). Closure is the quotient of the positive orthant by scaling, which is why a transition carried by a total, such as population size, must be tested on the total separately (Part II §4).

**The requirements of each structure.**

- **Vectors and multichannel signals.**
  - Use the linear core of §1.2: windowed Ĉ with shrinkage, Â of reduced rank, moduli and arguments of its spectrum, and left and right eigenvectors.
  - Use the baseline-invariant leading eigenvalue λ_max(Ĉ₀⁻¹Ĉ_t). It is the leading term of the affine-invariant distance from a baseline and is invariant under any linear change of the observed channels, which matters for EEG montages (Part II §5, Inferences).
  - For Hopf-type onsets, use band-limited indicators (narrowing of a spectral peak, power of the oscillatory mode), not broadband lag-1 autocorrelation.
  - Choosing a channel after the fact must be forbidden or declared, and circadian and multidien rhythms must be beaten as a baseline (Part IV §2, §6, §9.4).
- **Fields on grids and graphs.**
  - Spatial variance, skewness and Moran's I on the declared W; W is a modelling choice to which results must be reported as sensitive (Part II §2).
  - Graph Fourier power by Laplacian frequency.
  - Baseline referencing (subtracting each node's far-from-tipping baseline) removes the structural term that heterogeneous equilibria add to spatial variance. It "markedly improves" variance-based signals and survives the omission of up to 80% of nodes (Bandara, Yu & Masuda 2026, abstract), but it needs a reference period, which the detector must ask for.
  - The estimated dispersion relation σ(k) tells a homogeneous fold (k* = 0) from a Turing destabilisation (k* > 0) (Sanders & Bastiaansen 2025). This is the only principled answer to Rietkerk's point that patterns may signal resilience.
  - Nulls must preserve the spatial covariance of a stable state (surrogate fields with the same spectrum, or a fitted CAR/SAR model); otherwise Moran's I trends are confounded with smooth drift, as in the French case (Part II §2).
- **Networks.** Three questions must be kept apart (Part II §3).
  - *Dynamical proximity* by a reduction R = aᵀx with a closure. Such reductions (Gao–Barzel–Barabási, spectral, DART) are accurate only when node states are homogeneous, and their observable inherits the blindness of §1.2 when a differs from v (Tu et al. 2017; Kundu et al. 2022).
  - *Structural thresholds*: Molloy–Reed, the non-backtracking spectral radius, the k-core, branching ratios of cascades, the spectral radius of an exposure matrix, DebtRank (Part II §3.4; Part V §4, §6).
  - *Structural change* in a sequence of graphs: graph-based scan statistics (Chen & Zhang 2015), whose output says that the structure changed and not that the dynamics approaches a bifurcation.

  A structural change can move a system towards tipping or away from it. Network nulls must preserve degree heterogeneity: Dutch interbank exposures show an "unpredictable" break in 2008 under a density-only null but a gradual transition under a heterogeneity-preserving one (Squartini et al. 2013; Part V §4, §8).
- **Compositions.**
  - Test in ilr coordinates: the leading eigenvalue of the ilr covariance and of an ilr VAR(1) propagator, and Aitchison's total variation. These are invariant under a change of orthonormal basis, since two ilr bases differ by an element of O(D − 1).
  - Raw shares are wrong twice: closure forces negative correlations, and it removes the uniform scaling mode.
  - Shares near 0 or 1 have bounded variance, so variance indicators on raw shares are biased at the boundary (Part V §8, inference).
  - Zeros break log-ratios. A detector must state its zero policy and refuse when zeros dominate.
  - No published work defines CSD in Aitchison geometry; this is a gap the design fills (Part II §4; Part V §8).
- **Distributions.**
  - On the line, W₂(μ, ν) = ‖F_μ⁻¹ − F_ν⁻¹‖_{L²(0,1)} embeds the space isometrically in a convex cone of L². Covariance, AR(1) and DMD are therefore well defined on quantile coordinates, with an OU closed form in a finite quantile basis.
  - In higher dimension no linear embedding is exact. The options are a moment or density basis with its truncation error stated, tangent-space linearisation of W₂ at a reference measure, or graph-based change-point tests with W₂, which detect change but not slowing (Part II §4).
  - Trait-based indicators (body size) add information to abundance (Clements & Ozgul 2016; Clements et al. 2017).
- **Matrices.**
  - Covariance and connectivity matrices live on the SPD cone, with the affine-invariant distance δ_R(A, B) = (Σᵢ ln²λᵢ(A⁻¹B))^{1/2} and the log-Euclidean distance. These are mature in brain–computer interfaces, but no published work uses them for CSD (Part II §5).
  - Correlation matrices lie on the elliptope (unit diagonal). SPD metrics applied to them discard the variance channel, which is the primary CSD signal, so a detector should take covariances or test variances and correlations separately (Part II §5).
  - Trajectory speed and distance from a baseline Fréchet mean are indicators of state change, decomposed by the eigenvalues of A⁻¹B (Part IV §6).
- **Hierarchies across scales.**
  - Every indicator should be computed at every level on both channels (the Remark above; Part II §7).
  - Finite-size scaling (susceptibility, and Binder cumulant crossings against block size) detects continuous transitions without a time series. It assumes an order parameter and a scaling form, and it is blind to first-order transitions except through hysteresis and bimodality (Part II §7).
  - Phenomenological renormalisation yields scaling exponents for any multichannel signal, but scaling alone is not proximity to a critical point (Morales, di Santo & Muñoz's edge-of-instability reading), so it is a descriptor, not a warning (Part II §7).
- **Event cascades and point processes.**
  - Avalanche statistics must be fitted by Clauset–Shalizi–Newman with alternatives, and tested by the crackling relation γ = (α − 1)/(τ − 1) and by shape collapse.
  - The branching ratio must be estimated by multistep regression, which is unbiased under binomial subsampling (r_k = b·m^k), where the conventional estimator returns b·m (Part IV §1).
  - Power laws arise without criticality, so the Touboul–Destexhe surrogates and subsampled non-critical models are mandatory negative controls (Part IV §1.8).
- **Topological summaries.** The L^p norms of persistence landscapes on sliding windows grow with the spread of the point cloud, that is, with volatility. Without volatility-matched nulls a topological warning cannot be told from a variance warning (Part II §5, an inference; Part V §4).

**Status.** None of these structured constructions on compositions, quantile functions or SPD trajectories has a published benchmark of false-alarm and detection rates. Their rates must be established in-house on closed forms before any claim on real data (Part II §11, Gaps).

### 1.4 The Empirical Record Is Positive Where Transitions Are Controlled or Physically Guaranteed, and Mostly Negative on Records Chosen After the Fact

**Climate** (Part III §1).

- **The AMOC: claims of slowing down.**
  - Boers 2021 reports significant CSD in eight AMOC fingerprints.
  - Ditlevsen & Ditlevsen 2023 extrapolate a collapse "around mid-century" (see §0.4 for the interval).
- **The AMOC: critiques.**
  - With alternative SST products the same estimator gives tipping times from the 2000s (HadISST) to the 3000s; with alternative fingerprints, from the 2000s to beyond 4700. Across ensembles almost a quarter of the tipping times go to infinity (Ben-Yami et al. 2024, full text).
  - With structural, statistical, representativeness and data uncertainties propagated, the times "extend several millennia into the future" (Morr et al. 2026, preprint).
  - Subpolar SST "cannot robustly reconstruct the AMOC", and air–sea heat fluxes show no weakening at 26.5°N from 1963 to 2017 (Terhaar et al. 2025).
  - The relation between the AMOC and SST is state-dependent (Fan et al. 2026).
  - Thirty-four models show no complete collapse this century (Baker et al. 2025).
  - The SST fingerprint gives no warning in a CESM collapse, where salinity near the southern boundary does better (Smolders et al. 2025, [S] content).
- **The AMOC: physics-based indicators.** The minimum of the overturning's freshwater transport at the southern boundary (F_ovS) is proposed as a warning (van Westen et al. 2024). Across 25 models, collapse onset falls by 2063 (2026–2095) under SSP2-4.5 (van Westen et al. 2025).
- **The AMOC: noise separated from destabilisation.** A hierarchical Bayesian AR(2) with time-varying parameters still finds EWS after separating noise from destabilisation (Hallali et al. 2025). Shin et al. 2025 find declining stability in the eastern subpolar North Atlantic in observations and models alike.
- **Greenland** shows significant EWS in its central-western part (Boers & Rypdal 2021).
- **The Amazon.** More than three quarters of the Amazon has been losing resilience since the early 2000s (Boulton et al. 2022). But resilience estimation from optical vegetation data is "broadly problematic" in dense tropical and boreal forests, and reliable in low-biomass mid-latitudes (Smith & Boers 2023).
- **Sea ice** is judged not to tip in summer. The 2007 jump in its seasonal amplitude carries "no robust early warning signal" and depends on how the seasonal cycle is removed (Livina & Lenton 2013).
- **Dansgaard–Oeschger events** went from "no warning" (2010) to "warning" in the decadal band (2018) to "fewer significant EWS than previous studies" and no constraint on mechanism (Hummel et al. 2025).
- **Climate networks** claimed hit rates above 0.5 and false-alarm rates below 0.1 for El Niño. Only the authors' evaluation and a blog were found (Part III §1.10).
- **The common pattern.** The strongest critiques attack the *proxy* (fingerprint, vegetation index, melt reconstruction), not the statistics. Whether a restoring rate is declining is answerable with care; when tipping will occur is not answerable from historical data.

**Ecosystems** (Part III §3).

- **Experiments succeed.**
  - Whole-lake food-web manipulation gave warnings more than a year before the transition (Carpenter et al. 2011).
  - Enrichment stopped when indicators crossed preset bounds averted the bloom that occurred in a continuously enriched lake; this is the only managed prevention found (Pace et al. 2017).
  - *Daphnia* (Drake & Griffen 2010), yeast with a mapped fold (Dai et al. 2012) and cyanobacterial recovery rates (Veraart et al. 2012) also succeeded.
- **Systematic observational tests fail.**
  - Burthe et al. 2016 tested 126 series of 55 taxa. True positives were 9% (16/170) for variance and 13% (19/152) for autocorrelation; false positives outnumbered false negatives (53% vs 38%; 47% vs 40%); and 43% of series were classified differently by the two indicators.
  - Agreement among indicators was low (Gsell et al. 2016).
  - Few critical transitions were found in lakes worldwide, results were "highly processing dependant", most indicators were no better than chance, multivariate EWS were only weakly superior, and a machine-learning model performed poorly (O'Brien et al. 2023).
- **The more positive review.** Dakos et al. 2024 judge performance positive "in the majority of the case studies", but the cases are a selected literature (Boettiger & Hastings, *Proc. R. Soc. B*).
- **Patterns and traits.** Spatial power laws of patch sizes were proposed as warnings, and may instead signal resilience (Rietkerk 2021). Body-size trends warned of whale-stock collapse up to 40 years ahead (Clements et al. 2017).

**Epidemics** (Part III §4).

- **Theory.** Emergence and elimination are transcritical points at R₀ = 1, and the birth–death–immigration process gives every indicator in closed form.
- **Simulation tests.**
  - Mean, variance and first-differenced variance survive aggregation and reporting error unless reporting is highly overdispersed. Autocorrelation needs aggregation coarser than the serial interval. The coefficient of variation, skewness and kurtosis are unreliable (Brett et al. 2018).
  - CSD appeared before re-emergence in all five measles-like models, from non-seasonal SEIR to an agent-based simulator, with AUC > 0.75 a year ahead (Brett et al. 2020).
- **Empirical successes:** malaria in Kericho; measles in Niger with fitted models; mumps in England anticipated four years ahead by a classifier of "dynamical footprints".
- **Failures** come when forcing is fast relative to relaxation (COVID-19).
- **Selection.** A systematic review found 36 of 37 studies reporting CSD, with sensitivity from 0.03 to 1 (Delecroix et al. 2023). Beside Burthe's 9–13% in ecology, this is a warning of publication selection.

**Brains and bodies** (Part IV).

- **Healthy dynamics.**
  - In vivo spiking gives m̂ ≈ 0.98, a "reverberating" regime slightly below criticality (Wilting & Priesemann 2018).
  - Power-law avalanches arise from non-critical networks and even from independent surrogates (Touboul & Destexhe 2017).
  - A meta-analysis of 140 datasets attributes the controversy to "a methodological choice" that its abstract does not name (Hengen & Shew 2025).
- **Seizures.**
  - Variance and autocorrelation cycle with susceptibility (Maturana et al. 2020; 14 patients).
  - A surrogate-tested study of 28 subjects and 105 seizures found "no evidence for critical slowing down" (Wilkat et al. 2019); nearly 300 seizures showed no common signatures (Milanowski & Suffczynski 2016).
  - The robust finding is that seizure *risk* follows circadian and multidien cycles, "most commonly 20–30 days" (Baud et al. 2018). Forecasts of risk, not predictions of single seizures, are the defensible target, judged out of sample against a random predictor and seizure-time surrogates.
  - Termination shows "slowing, increased correlation, and flickering" (Kramer et al. 2012).
- **Depression.** About one third of people show warnings before recurrence (32.9%, [S]), and a 2024 perspective finds little support for clinical use (Part IV §3).
- **Anaesthesia.** Under anaesthesia the brain moves away from criticality, with ketamine as the natural control, a cut-off PCI* ≈ 0.31 (from memory), and hysteresis ("neural inertia") in animals (Part IV §4).
- **Physiology.**
  - "Surprise loss" placed transitions a median of more than 35 h before septic shock and outperformed autocorrelation and variance, retrospectively (Ghalati et al. 2019).
  - Heart-rate autocorrelation rose before re-intubation, with limited utility (Khalil et al. 2025).
  - Frailty went with higher variance and cross-correlation but not autocorrelation (Gijzel et al. 2017).
  - The only randomised benefit is neonatal monitoring of heart-rate characteristics, which uses reduced variability and decelerations, not CSD (Moorman et al. 2011).

**Populations, cities, economies, societies and infrastructure** (Part V).

- **Finance** is the most important negative result.
  - Five major markets showed no CSD before crashes over a century (Guttal et al. 2016).
  - Of four crises, only Black Monday 1987 showed CSD (Diks et al. 2019).
  - Log-periodic patterns lose significance when the last year before the crash is excluded (Feigenbaum 2001).
  - Rising variance and cross-correlation are the consistent pre-crisis features, and volatility clustering produces them without any bifurcation.
- **Banking crises** are forecast by structural slow variables.
  - Credit growth predicts them (Schularick & Taylor 2012). Rapid credit and asset-price growth carries a 40% probability of crisis within three years, against about 7% normally (Greenwood et al. 2022).
  - Simple logits beat machine learning in recursive out-of-sample tests (Beutel et al. 2019).
- **Housing.** GSADF dates explosive regimes already under way. CSD in housing rests on a single team and episode.
- **Traffic.** Jamming is reproducible on a ring without bottlenecks (Sugiyama et al. 2008), and CSD indicators feed an operational risk algorithm (Ghadami et al. 2022).
- **Segregation.** Tipping is found at 5–20% minority share by a split-sample discontinuity design (Card, Mas & Rothstein 2008), but "largely fails the tests" on the same tracts (Easterly 2009).
- **Power grids** have the clearest physical basis for CSD (Cotilla-Sanchez et al. 2012). But their frequency is heavy-tailed (Schäfer et al. 2018), and the Iberian blackout of 2025 was an overvoltage cascade after oscillations, not a slow approach to a fold ([S]).
- **Past societies.** Seven of nine Neolithic regions (Downey et al. 2016) and the Pueblo societies (Scheffer et al. 2021) show CSD before transformation, retrospectively on proxies chosen because a collapse is known.
- **Conflict.** Operational forecasting (ViEWS and its challenges) succeeds by structural risk factors and conflict history, and does worst for low-level violence and first onsets.
- **Not found:** no test of CSD before recessions, fertility collapses, migration surges, the onset of urban shrinkage, or the collapse of urban services.

**What the record says.** Two populations of evidence stand apart (Part III §3, Inferences; Part V §10.3).

- **Transitions that are controlled, replicated or physically guaranteed** are anticipated, often with lead times that matter: experiments, epidemic thresholds, traffic, grids, anaesthesia.
- **Series selected because they ended in a transition** mostly give false positives, low agreement, processing-dependent results, or positive findings that cannot estimate a false-alarm rate. The prosecutor's fallacy (Boettiger & Hastings, *Proc. R. Soc. B*) predicts this.

Where structural slow variables are known (leverage, credit gap, minority share, loading relative to limits), indicators of the slow variable outperform generic indicators of the fast one (Part V §4, §10.3).

### 1.5 Every Detector Must Report What It Sees, What It Is Blind To, Its Null, Its Operating Point and a Three-Valued Verdict

The five Parts converge on one form of result (Part I §9.2; Part II §11.1; Part III §6.3; Part IV §9.4; Part V §10.2.3). Each field answers a failure the record documents.

- **Estimand.** The object estimated (§1.3), and the observable used together with the evidence that it represents the state variable. The strongest critiques in climate attack the proxy, not the statistics (Part III §1, Inferences).
- **Sees.** The transitions it can see. For example:
  - {fold, transcritical, pitchfork} for variance and lag-1 autocorrelation;
  - {Hopf, period-doubling, Neimark–Sacker} added for the complex-eigenvalue and spectral detectors;
  - {homoclinic} for the multivariate frequency indicator;
  - {Turing} for the dispersion relation;
  - {continuous phase transition} for finite-size scaling;
  - {percolation, cascade} for structural thresholds;
  - {finite-time blow-up} for the failure forecast method.
- **Blind.** Always including:
  - R-tipping at constant frozen stability;
  - N-tipping at fixed parameters;
  - S-tipping;
  - global bifurcations (SNIC, homoclinic);
  - crises;
  - non-smooth or border-collision transitions;
  - transitions without a slow manifold;
  - for structured states, critical modes outside the observed channel (pᵀv = 0 or wᵀB = 0) and, for each hierarchy channel, the modes in the kernel of its carry operator;
  - for detectors using autocorrelation alone, also Hopf points with ω₀Δt > π/2, and period-doubling.
- **Assumes.**
  - timescale separation, with the estimated ratio of forcing rate to relaxation rate reported, since CSD needs slow forcing (Part III §4; Dablander et al. 2022);
  - the noise model (additive, white or red, Gaussian or heavy-tailed);
  - smoothness of the vector field;
  - a stationary process of measurement, with regular sampling or a likelihood with variable Δt;
  - for brains and bodies, the observation model: the fraction subsampled, the signal type and volume conduction. A claim from scalp EEG concerns a coarse-grained field, not neurons (Part IV §9.4).
- **Settings.** The window, detrending bandwidth, filter band, avalanche threshold and Δt, fit ranges and zero policy, fixed before outcomes are seen, together with a sensitivity grid and the fraction of the grid that is significant. Results are "highly processing dependant" (O'Brien et al. 2023), and seasonal-cycle removal can decide the answer (Livina & Lenton 2013).
- **Null, in words.** For example "stationary AR(1) with the residuals' lag-1 autocorrelation and variance", or "stationary linear Gaussian with the residuals' spectrum". The surrogates must match it: phase-randomised, IAAFT, block bootstrap, red-noise OU, fitted VAR(1) or multivariate phase randomisation for vectors, fields with matched spatial spectrum or CAR/SAR, heterogeneity-preserving graphs, seizure-time surrogates, Touboul–Destexhe surrogates for power laws. A rejection is phrased as "trend unlikely under this null", never as "approaching a bifurcation" (Part I §4).
- **Operating point.** The false-alarm rate α̂ and the detection rate 1 − β̂ against a declared alternative, at the record's length, sampling and processing. They are computed by simulating the fitted null and the fitted alternative, after Boettiger & Hastings (*J. R. Soc. Interface*), with closed-form OU transitions. Error rates belong to a pair of models, not to an indicator. Several indicators on one record are strongly dependent, so they get one composite statistic with a jointly simulated null (no standard exists in the literature; Part I §4).
- **Verdict** ∈ {signal, no signal, undecidable}.
- **Location.** Where in the structured state the slowing lies: the loadings of the right eigenvector over blocks, sectors, ages, nodes or parcels, and the support of the left eigenvector (where noise excites the mode). Action is local (Part V §10.2.3; Part II §11.2).
- **Quantities rather than labels where the question is one of distance.** For the restoring rate, κ̂ with an interval and the noise parameters, with the consistency check that the variance equals σ²/(2κ) under white noise of constant amplitude. For cascades, m̂ and τ̂ with intervals, and the residual of the crackling relation. The regime is called "consistent with near-critical, reverberating or subcritical", never "critical" (Part IV §9.4).

**When the verdict must be undecidable.** The verdict must be undecidable, not "no signal", when any of the following holds (Part I §9.2; Part II §1, §2, §4, §6, §11.1; Part III §6.3; Part IV §9.4):

1. The record is shorter than the window needed to detect the hypothesised change at the declared confidence. Ditlevsen & Ditlevsen give closed forms; autocorrelation needs a longer window than variance.
2. The baseline lag-1 autocorrelation is so close to 1 that a window holds few independent points.
3. Variance and autocorrelation disagree with the fluctuation–dissipation relation (the noise changed). A rise in variance alone may be noise (Ditlevsen & Johnsen 2010).
4. The red-noise model fits better than the white, and the restoring-rate estimator under red noise is not significant.
5. The results of the sensitivity grid change sign.
6. The forcing is not slow relative to the estimated relaxation time.
7. For multivariate estimators, the window is not much longer than the dimension, or Ĉ(0) is ill-conditioned beyond what shrinkage repairs.
8. For operator estimators, cells have too few transitions, or the spectrum changes with the lag τ (the reduction is not Markovian).
9. For compositions, zeros dominate. For patch statistics, the threshold is arbitrary and the result depends on it.
10. The record's trend cannot be removed, or the surrogates cannot be told from the record.
11. For learned detectors, the input lies outside the training manifest: bifurcation types, noise types, rates, record lengths, sampling, detrending.
12. For a time to tipping, its profile-likelihood interval is unbounded or extends past the declared horizon. The report then says "tipping time not identified", with the Kramers hazard of noise-induced tipping beside it (Part I §5; Ben-Yami et al. 2024).

**What a decision-maker needs as well.** A detector that informs action reports more than the above (Part V §7, §10.2.3; Part IV §9.4).

- A probability of transition within a stated horizon, with its calibration (Brier score, CRPS).
- The distribution of lead times, and skill as a function of horizon.
- The hit rate, the false-alarm rate per unit time, AUROC, and usefulness under a declared preference μ between missed transitions and false alarms (Sarlin's loss function L(μ) = μT₁P₁ + (1 − μ)T₂P₂, from memory).
- The stability of the signal over successive windows.
- The assumed lag of action. A warning shorter than the response time is worth little, however accurate (Biggs, Carpenter & Brock 2009).
- Real-time conditions: one-sided detrending and data vintages, since the HP filter's end-point values differ from its mid-sample values (Hamilton 2018).
- Dating a regime change after the fact (Bai–Perron, Markov switching, GSADF date-stamping), reported separately from anticipation.
- For anticipation of events: sensitivity, false prediction rate per hour or time in warning, the seizure occurrence period and prediction horizon, and improvement over a Poisson chance predictor, p ≈ 1 − exp(−FPR·SOP). All are computed on held-out, continuous, unselected data, against a baseline of cycles alone.
- For clinical signals, the status "research use; not validated for diagnosis or prediction".

The institutional practices of finance and conflict forecasting are more demanding than the ecological test by Kendall's τ against surrogates, and should be the default: recursive out-of-sample evaluation, true-future windows, independent scoring, probabilistic scores. The ecological test is necessary but insufficient (Part V §4, §7).

**Table 1.2.** Families of detector: what each estimates, what it sees, what it is blind to or misled by, and when it must refuse.

| Family | Estimates | Sees | Blind to, or misled by | Must refuse or say undecidable when | Part |
|---|---|---|---|---|---|
| Moment indicators: variance, lag-1 AC, skewness, kurtosis, CV in rolling windows | σ²/(2κ), e^{−κΔt}, shape of the distribution | Fold, transcritical, pitchfork (real multiplier → +1); skewness and flickering at a fold | Hopf with ω₀Δt > π/2 and period-doubling (AC falls); R, N, S tipping; global bifurcations, crises, non-smooth folds. Misled by red or nonstationary noise, merged instruments, interpolation, gaps; variance can fall near a transition | Record shorter than the required window; baseline AC near 1; variance and AC inconsistent with σ²/(2κ); sensitivity grid changes sign | I §2–4 |
| Restoring rate κ̂: AR(1)/OU, increment regression by GLS under AR(1) noise, ACS and PSD fits under red noise, Kramers–Moyal drift slope, recovery from perturbations | κ and the noise parameters, separately | The same types, robust to noise amplitude, and to noise colour for the ACS/PSD fits; extends to a VAR(1) or Jacobian | The same mechanism classes. The "unbiased" estimator of Boettner & Boers is ill-defined on a set of positive probability and needs very long series | The red model fits better and κ̂ under red noise is not significant; the consistency check fails | I §3.2, §9 |
| Model-based likelihood ratio: drifting-κ OU, linearised saddle-node, ramped fold, against stationary OU and OU driven by OU | Deviance; ROC by parametric bootstrap; profile of t_c | B-tipping of the declared shape, with stated error rates; irregular sampling handled exactly | Misspecification of the ramp, the noise colour or the fingerprint; R and N tipping | The interval for t_c is unbounded or passes the horizon ("not identified") | I §4–5 |
| Type-aware linear part: spectral fold/Hopf/null fits, complex dominant eigenvalue (DEV, AR(p), S-map), the power law that linearises κ̂(t) | Position of the dominant eigenvalue; peak frequency and width | Fold against Hopf; period-doubling; Neimark–Sacker; from several series, the homoclinic frequency drop | R, N, S tipping; non-smooth folds; subcritical and supercritical Hopf not told apart by the linear part | Inherits the conditions of the two rows above | I §3.3, §3.5, §9; II §1.3 |
| State count: potential analysis, bimodality, flickering, Kramers escape of a fitted double well | Number of wells; proximity to switching | The only family with purchase on N-tipping; flickering before a fold | Fooled by measurement artefacts; needs long windows; does not extend beyond a few dimensions; reports proximity to switching, not to a bifurcation | Windows too short to resolve the density | I §3.4, §9 |
| Multivariate linear: λ_max(Ĉ), λ_max(Ĉ₀⁻¹Ĉ_t), spectrum of Â, degenerate fingerprinting, PCA and MAF projections, DNB, maximum or mean of node-wise indicators | One of the three objects of §1.2 | B-tipping when the critical mode is observed and excited; *where* it lies | pᵀv = 0 or wᵀB = 0 (silent catastrophes); non-normal J (covariance misaligned); delocalised modes (DNB); the maximum inflates false alarms, the mean dilutes | Window not ≫ dimension; Ĉ(0) ill-conditioned beyond shrinkage; only per-series surrogates available | II §1; IV §6 |
| Spatial: spatial variance and skewness, Moran's I, r-spectrum and graph Fourier power, patch sizes, dispersion relation | Neighbourhood statistics of a snapshot on W | Homogeneous fold under weak coupling, even under fast forcing; Turing against tipping by k* | A uniform critical mode (strong coupling); the structural term of heterogeneous equilibria; saturating CV; patterns that evade tipping; choice of W and zoning | No reference period where heterogeneity matters; arbitrary patch threshold; surrogate fields not matched in spectrum | II §2; III §3.6 |
| Network reductions: Gao–Barzel–Barabási, spectral, DART, Ott–Antonsen | An effective coordinate R = aᵀx | Collective tipping of networks whose node states are nearly homogeneous | Heterogeneous equilibria (a ≠ v); saddle escapes | The dispersion of node states is large | II §3.1 |
| Structural thresholds: Molloy–Reed, non-backtracking spectral radius, k-core, branching ratio of cascades, spectral radius of exposures, DebtRank, basin stability, minimal fatal shock | Distance to a threshold computed from the graph or the model | Percolation, cascades, systemic importance; risk of S-tipping given a model | Dynamical proximity on the current structure; noise-driven events | No graph or model is declared | II §3.4; V §4, §6; I §1, §3.6 |
| Change points and regime dating: graph scan statistics, Bai–Perron, Markov switching, GSADF | Dates and probabilities of regimes | That a structure or regime has changed; an explosive regime already under way | Anticipation (it dates after the fact); size distortions under time-varying volatility | Minimum window not declared; two-sided detrending claimed as real time | II §3.3; V §1–4 |
| Operators: Ulam transfer gap, DMD/EDMD and Koopman, Ruelle–Pollicott resonances, linear response | Spectrum of the transfer or Koopman semigroup on a dictionary | Slowing of attractors of any kind; complex μ; homoclinic approach; crises; R-tipping only by control-augmented Koopman (a preprint) | Bias of the gap from non-Markovian reductions; heavy data requirements | Too few transitions per cell; spectrum depends on the lag τ | II §6 |
| Scale: finite-size scaling, Binder cumulant, aggregation susceptibility; phenomenological RG as a descriptor | Statistics against block size | Continuous phase transitions, without a time series; which channel (sum or share) carries a signal | First-order transitions except through hysteresis and bimodality; scaling alone is not proximity | No order parameter or scaling form; too few scales; flag discontinuity (Potts q > 4) | II §7 |
| Criticality of cascades: Clauset fits, crackling relation, shape collapse, multistep-regression branching ratio, DFA of envelopes, aperiodic exponent | Exponents, m̂, τ̂, crackling residual | Distance to critical branching; consistency of scaling | Power laws without criticality; subsampling; time-varying drive; aperiodic exponent confounded by synaptic kinetics | Never certifies "critical"; reports quantities with intervals over a sweep of Δt and threshold | IV §1, §9 |
| Finite-time singularity: Voight's relation and the failure forecast method by Poisson or negative-binomial GLM; Fukuzono's inverse velocity (α = 2); LPPLS | t_f and α | Positive-feedback acceleration (failure, eruption, landslide, runaway growth) | Fold-type CSD under stationary noise; least squares on the inverse rate is biased; log-periodicity contested | α not identified; LPPLS only as a null with Feigenbaum's exclusion test | I §7; V §4 |
| Information, thermodynamics, topology: MI, transfer entropy, entropy production, persistence landscapes, ecological "Fisher information" | Functionals of the joint law | Non-Gaussian regimes; directed coupling (the support of w) | Peaks offset from the transition (information flow peaks on the disordered side); landscapes track volatility; the ecological index is not Fisher information of a parameter | No volatility-matched null (topology); trend not readable on the stated side | II §5, §9 |
| Learned classifiers: CNN–LSTM, EWSNet, SDML, GNN, reservoir computers | Class probabilities; post-tipping state; location on a network | What the training library holds | Out-of-distribution inputs; preprocessing; uncalibrated uncertainty; empirical performance weak on lakes | Input outside the training manifest; not calibrated against the same surrogate nulls | I §6; II §8 |
| Model-required global analysis: critical rates, basin instability, edge states and instantons | Thresholds of rate and basin in a declared model | R-tipping, given a model of the forcing; which observables carry CSD (edge states) | Needs a declared model, not a record | No model is declared | I §1, §9; II §1.5, §6.4 |

### 1.6 Detectors Should Be Built in an Order Set by Explicit Assumptions and Closed Forms

**The starting point.** The dynamical-systems foundation already holds, according to the Parts:

- rolling variance and lag-1 autocorrelation, with Kendall's τ against red-noise surrogates;
- DFA, Clauset fits, avalanches and the branching ratio;
- Lyapunov exponents, bifurcation diagrams and a transfer operator.

These cover the univariate core (Part III §6; Part IV §9). The order below merges the five Parts' recommendations (Part I §9.1; Part II §11.4; Part III §6; Part IV §9; Part V §10.3). Each step is verified on closed forms before it is shown on data.

1. **The restoring-rate core, with one result object** (§1.5). Four estimators of κ̂ share one interface:
   - OLS AR(1), κ̂ = −log φ̂/Δt;
   - increment regression by GLS under AR(1) noise (Boers 2021);
   - ACS and PSD fits of the OU-driven-by-OU model (Morr & Boers 2024);
   - the Kramers–Moyal drift slope.

   The existing trend tests are extended with surrogates beyond AR(1) (phase-randomised, IAAFT, stationary block bootstrap, red-noise OU), sensitivity grids, and one joint null for the set of indicators.
2. **The likelihood-ratio detector.** Drifting-κ OU (linear, fold-shaped or transcritical-shaped) or the linearised saddle-node, against stationary OU and OU driven by OU, with exact transition densities at variable Δt. Bootstrap distributions of the deviance give the ROC and the operating point. t_c is reported only with its interval and the Kramers hazard beside it.
3. **The state-space interface** of §1.3, with five charts (identity, ilr, one-dimensional quantiles, matrix logarithm with whitening, Laplacian basis) and labelled carry operators. Part II proposes building it on the foundations' compositions and graphs.
4. **The linear multivariate core:**
   - windowed Ĉ with shrinkage;
   - Â of reduced rank, its spectrum (moduli and arguments), and left and right eigenvectors;
   - the baseline-invariant leading eigenvalue;
   - multivariate surrogates (fitted VAR(1) under the null of constant Â; multivariate phase randomisation);
   - refusal rules on sample size and conditioning.

   It is verified on the graph-OU and general-OU closed forms, including a non-normal case and a case with wᵀB = 0.
5. **Type-aware detectors:**
   - spectral fold/Hopf/null fits with AIC weights;
   - the complex dominant eigenvalue (its path towards +1, −1, or the unit circle or imaginary axis);
   - the power law that linearises κ̂(t);
   - band-limited indicators for Hopf onsets;
   - the multivariate frequency indicator for homoclinic approach.
6. **Spatial statistics on any weighted graph:** spatial variance with baseline referencing, skewness, Moran's I, graph spectral power, and the dispersion relation over Laplacian frequencies. They are verified on graph OU and checked against reference packages (esda, spatialwarnings).
7. **The hierarchy runner:** every detector at every level on both channels, with the report saying which channel carries a warning. Aggregation-scale susceptibility and the Binder cumulant are verified on Ising and Potts models, which the foundations already hold.
8. **Operator detectors** through one dictionary interface (Ulam partition, linear coordinates, EDMD), reusing the existing transfer operator, and verified on double wells and VAR(1).
9. **State count and finite-time singularity:**
   - potential wells, flickering and Kramers escape, reported as proximity to switching;
   - Voight's relation and the failure forecast method by Poisson or negative-binomial GLM, with α profiled and Fukuzono's inverse velocity as the case α = 2.
10. **Evaluation harnesses:**
    - a criticality-consistency suite: Clauset fits with alternatives, the crackling relation with bootstrap, shape collapse, multistep regression with the offset test for non-stationarity;
    - an event-anticipation harness: occurrence period and horizon, improvement over chance, seizure-time surrogates, a baseline of cycles;
    - an institutional harness: AUROC, noise-to-signal ratio, usefulness under a declared loss, stability of the signal, real-time vintages and one-sided detrending.
11. **Structural detectors and regime dating**, each kept as a separate class from dynamical proximity:
    - spectral radius of a graph or of the non-backtracking or exposure matrix, k-core, branching ratio of cascades, distance of a declared slow variable to its threshold;
    - graph-sequence change points (scan statistics on a metric);
    - regime dating (Bai–Perron, Markov switching, GSADF).
12. **Later:**
    - SPD-trajectory detectors for EEG and fMRI covariances;
    - an onset and offset dynamotype classifier, verified on Saggio's unfolding;
    - generalised modelling;
    - persistent homology, only with volatility-matched nulls;
    - learned detectors, only with a training manifest, out-of-distribution refusal, and calibration against the same nulls;
    - Koopman indicators of R-tipping, as research.

*Rationale.* The order puts first the detectors whose assumptions are explicit and whose behaviour has closed forms.

- **κ̂ before moments.** Variance and autocorrelation give spurious or masked alarms when the colour of the noise changes, and variance can fall near transitions; κ̂ estimators address both (Morr & Boers 2024; Part I §9).
- **Likelihood and ROC early.** Without them, error rates are unknown and can be "quite severe" (Boettiger & Hastings).
- **Type-aware detectors early.** Conventional indicators do not tell a Hopf point from a fold (Bury, Bauch & Anand 2020).
- **Learned detectors late.** They are trained on exactly the normal forms the infrastructure can simulate, and their superiority rests on test sets drawn from related families (Part I §6).
- **No benchmark exists.** No community benchmark suite of univariate or multivariate detectors exists, so the infrastructure's own suite of simulations with known κ(t) and type would itself be a contribution (Part I §8; Part II §1, Gaps; Part V §10.4).

### 1.7 Test Models with Closed Forms Supply Positive Cases and Negative Controls

**The role of test models.** Each model below has a known answer: the location and type of its transition, or a closed form for its indicators. A detector is scored against the true distance to the transition, and its κ̂ against the true restoring rate (Part III §2, Inferences). Each case declares its expected verdict for each detector: signal, silent, or undecidable (Part I §9.4). The scores are ROC/AUC against null runs, the lead time, and the calibration of κ̂ (Part III §6).

**What the known answers check.** The positive cases test that a detector finds a transition that is there. The negative controls test that it stays silent at its nominal false-alarm rate, or reports its blindness, where the theory says no warning exists. Formulas marked "from memory" or "derived here" in the Parts must be re-derived before they serve as fixtures.

**Table 1.3a.** Positive cases with known answers.

| Model | Known answer | Part |
|---|---|---|
| OU dx = −κx dt + σ dW | Var = σ²/(2κ); ACF e^{−κ\|τ\|}; φ = e^{−κΔt}, innovation variance σ²(1 − e^{−2κΔt})/(2κ); spectrum σ²/(2π(κ² + ω²)); D⁽¹⁾ = −κx, D⁽²⁾ = σ²/2 | I §9.3 |
| Fold normal form dx = (μ − x²)dt + σ dW, μ ramped | x* = √μ, κ = 2√μ, Var ≈ σ²/(4√μ) (var⁻² linear in μ); slow passage jumps O(ε^{2/3}) past the fold | I §9.3 |
| Transcritical, pitchfork, Hopf normal forms | κ = \|μ\|, Var ≈ σ²/(2\|μ\|); Hopf peak at ω₀, coordinate variance ≈ σ²/(2\|μ\|) | I §9.3 |
| Period-doubling map x_{n+1} = −(1 − δ)x_n + noise | Lag-1 AC = −(1 − δ); Var = σ²/(1 − (1 − δ)²) | I §9.3 |
| Double well U = x⁴/4 − x²/2 | Two modes of exp(−2U/σ²); Kramers time 2π/√(\|U″(a)\|U″(b)) · exp(2ΔU/σ²); generator gap for transfer operators | I §9.3; II §11.3 |
| Ramped fold of Ditlevsen & Ditlevsen | Square-root equilibrium; α(λ) = 2√(\|λ\|A); τ_n(λ) by Kramers | I §5 |
| Linear SDE with known J, B (normal, non-normal, wᵀB = 0); VAR(1) with real and complex \|μ₁\| → 1 | Exact Lyapunov solution; 1/(2\|λ₁\|) divergence and the excitation factor wᵀBBᵀw; directions of propagator and covariance differ when non-normal | II §11.3; IV §9.2 |
| Graph OU dx = −(γI + κL)x dt + σ dW | C = (σ²/2)(γI + κL)⁻¹; mode variances σ²/(2(γ + κλ_k)); λ_max(Ĉ) → σ²/(2γ) with eigenvector 𝟏/√n; heterogeneous γ_i make spatial variance diverge | II §11.3 |
| Logistic-normal composition with an OU in ilr; Gaussian laws with (m, ln s) an OU | Basis invariance of ilr eigenvalues; a common factor invisible in shares; W₂ between Gaussians and quantiles m + sΦ⁻¹(p) in closed form | II §11.3 |
| 2D Ising; q-state Potts on the square lattice | T_c = 2/ln(1 + √2), ν = 1, γ = 7/4, β = 1/8; Binder crossings at T_c; Potts T_c = 1/ln(1 + √q), continuous for q ≤ 4 | II §11.3; IV §9.2 |
| Erdős–Rényi; configuration model; Kuramoto by Ott–Antonsen; Gao's mutualistic dynamics on homogeneous and heterogeneous graphs | Giant component at ⟨k⟩ = 1; Molloy–Reed threshold; exact order-parameter dynamics; error of the reduction growing with the dispersion of x_i* | II §11.3 |
| Stommel–Cessi reduced model ẏ = −y[1 + η²(y − 1)²] + p | Folds y± = 2/3 ± √(η² − 3)/(3η); cusp at η² = 3; at η² = 7.5, p ∈ [0.964, 1.480] | III §2.2 |
| Zero-dimensional EBM with tanh albedo; Benzi–Nicolis cubic with periodic forcing; North's diffusive EBM on the sphere | Smooth folds by continuation; stochastic resonance; small ice-cap instability and snowball fold of a field | III §2.1 |
| Logistic growth with type-III grazing, r = 1, K = 10, h = 1 | Folds at c ≈ 2.604 (x ≈ 4.78) and c ≈ 1.787 (x ≈ 1.135); bistable between | III §3.8 |
| Klausmeier's PDE | Homogeneous fold at a = 2m; patterns persist below it | III §3.8 |
| Birth–death–immigration process with R₀ = λ/μ → 1⁻ | Mean ζ/(μ − λ), variance ζμ/(μ − λ)², index of dispersion μ/(μ − λ), ACF e^{−(μ−λ)τ}; from above (elimination) the mean falls | III §4.4 |
| Branching process with immigration at m ∈ {0.9, 0.98, 0.999, 1}, binomially subsampled | m̂_MR → m; the conventional estimator → b·m; ⟨A⟩ = h/(1 − m); τ = −Δt/ln m; at m = 1, τ_S = 3/2, α_T = 2, γ = 2 and parabolic collapse | IV §9.2 |
| Kinouchi–Copelli network; stochastic Wilson–Cowan; Epileptor and Saggio's unfolding; neural-field Epileptor; generator of aperiodic spectra with known g_E/g_I | Dynamic range peaks at σ = 1; fold and Hopf by continuation; SN onset with DC shift, SH offset with logarithmic ISI growth, 16 dynamotypes; known E/I exponent shift | IV §9.2 |
| Schelling (one- and two-sided, all-integrationist, exactly solvable); Granovetter; Brock–Durlauf | Segregated and mixed phases; fixed point of r_{t+1} = F(r_t); bistability for βJ > 1 | V §10.2 |
| Optimal-velocity model on a ring; Nagel–Schreckenberg; IDM with an on-ramp | Uniform flow unstable when V′(h) > a/2; repeated breakdowns for estimating hit and false-alarm rates | V §10.2 |
| Eisenberg–Noe, Gai–Kapadia, DebtRank, Motter–Lai, interdependent percolation, outage branching; swing equations with heavy-tailed noise; crime reaction–diffusion | Analytic cascade thresholds; robustness to Lévy noise and oscillatory loss of stability; Turing-type hotspots | V §10.2 |
| Voight's relation with α = 2; GSADF on simulated mildly explosive processes | Inverse rate exactly linear with root t_f; GLM coverage nominal under Poisson thinning; size under time-varying volatility | I §7; V §10.2 |

**Table 1.3b.** Negative and specificity controls: the expected answer is silence at the nominal false-alarm rate, a declared blindness, or undecidable.

| Model | Why a CSD detector must not warn, or must flag | Part |
|---|---|---|
| Stationary OU and AR(1) | No change at all; calibrates α̂ | I §9.4 |
| ẋ = (x + rt)² − 1 at constant r < 1, and above r_c = 1 | Frozen eigenvalue constant at −2; tipping above r_c with no CSD trend | I §1, §9.4 |
| Fixed double well with noise-induced escape; double well with α-stable or increasing noise | N-tipping: no bifurcation. Variance may rise while autocorrelation stays flat | I §9.4; III §6.1 |
| Red noise whose correlation rises with κ constant; red noise whose correlation falls while κ falls | Spurious alarm for variance and AC; masked alarm that only red-noise estimators may detect | I §9.4 |
| Rising measurement noise; change of instrument | Changes of noise structure mistaken for loss of resilience | I §9.4 |
| Deterministic trend in the mean under red noise, no bifurcation | Ben-Yami's third case | I §9.4 |
| Piecewise-linear EBM; Stommel's \|Δρ\|; Welander's flip-flop; border collision | Non-smooth folds: relaxation rate constant (−B/C) up to the kink | III §2; I §9.4 |
| Stage-structured "silent catastrophe"; linear SDE with wᵀB = 0 | Critical mode not excited, or not observed | I §9.4; II §11.3 |
| Graph OU with a uniform critical mode | Centred spatial variance stays bounded; the detector must report the blindness | II §11.3 |
| SNIC or homoclinic approach; boundary crisis | No local CSD; period lengthening; transient lifetimes | I §9.4 |
| Loss of sensitivity to environmental fluctuations; coarsely sampled Hopf | Variance falls; lag-1 AC falls | I §9.4 |
| Wagner–Eisenman sea-ice EBM | The bifurcation of the simpler model disappears with fuller physics | III §2, §6.1 |
| Welander's relaxation oscillator; limit-cycle analogue of DO events; a model forced faster than it relaxes | Oscillation, not a fold; timescale separation fails | III §6.1 |
| Hamilton's Markov-switching AR(4) | Abrupt switches with no slowing; a regime filter must detect them, a CSD detector must stay silent | V §10.2 |
| GARCH-type volatility process | Rising variance without tipping | V §10.2 |
| A non-catastrophic transition that shows CSD | Specificity: CSD is not specific to tipping points | V §10.2 |
| Touboul–Destexhe surrogates; a time-varying drive on a subcritical process | Power laws without criticality; the crackling relation, shape collapse or m̂ must expose them | IV §9.2 |
| Potts model with q > 4 | First-order transition; the finite-size detector must flag the discontinuity | II §11.3 |
| Durable-housing decline; Krugman's multiple equilibria | Slow, ratchet-like decline, not a fold; bistability as a hypothesis | V §10.2 |
| LPPL as a null | A fitted log-periodic pattern must survive Feigenbaum's exclusion test | V §10.2 |

### 1.8 Honest Demonstrations Need Open Records with Independently Dated Transitions, Kept Under Their Terms

**The kinds of demonstration** (Part III §3, §6.2; Part IV §9.3; Part V §10.3).

- **Known-truth experiments** show what works: the North Temperate Lakes Cascade lakes, with a reference lake as a built-in control.
- **Repeated transitions with a physical basis** are the most statistically honest place to estimate hit and false-alarm rates: traffic breakdowns, grid frequency, seizure termination, anaesthetic induction and emergence.
- **Few events but mature chronologies and evaluation norms** come second: banking crises, conflict onsets.
- **Contested observational cases** are run as robustness grids that reproduce the critiques, and are presented as such: the AMOC fingerprints, DO events in NGRIP, Amazon vegetation optical depth.
- **Segregation and demographic regime changes** are research questions, and the detector is presented as exploratory there.

A "known transition" is itself a coded judgement: crisis chronologies differ, conflict is counted from 25 battle deaths, breakdowns from speed thresholds. A test must declare which chronology defines a hit (Part V §9). The data library should record each licence string verbatim, with any condition ("must cite", "acknowledge NERC/NSF/NOAA", "registration required"). Where no licence is stated, the record is marked "terms unclear; attribution required" rather than assumed open (Part III §5).

**Table 1.4.** Open records for demonstrations, with their terms as the providers state them.

| Record | Field; transition it holds | Terms | Use |
|---|---|---|---|
| HadISST1 (Met Office) | AMOC fingerprints | Open Government Licence (page links v2.0); cite Rayner et al. 2003 and © Crown Copyright; to be replaced in 2026, so pin the version [V] | Robustness grid with ERSST |
| ERSST v5/v6 (NOAA NCEI) | SST from 1850/1854 | No licence or terms on the product page [V] | Alternative product |
| RAPID-MOCHA 26°N | AMOC transport 2004–2024 | "Freely available", acknowledgement requested; email needed to download [V] | Validation only; too short for CSD |
| NGRIP δ¹⁸O and dust | DO events | Citation only, no licence stated [V]; terms unclear | Hummel 2025's robustness grid, presented as inconclusive |
| EPICA Dome C (PANGAEA) | Glacial terminations | CC BY 3.0 [V] | As in Dakos et al. 2008 |
| VODCA v1 (Zenodo) | Amazon resilience | CC BY 4.0 [V] (v2 not checked) | After Boulton et al. 2022 |
| PKU GIMMS NDVI v1.2 (Zenodo) | Vegetation 1982–2022 | CC BY 4.0 [V] | Low-biomass regions, where estimates are reliable |
| MODIS via NASA Earthdata | Vegetation | CC0 unless marked otherwise [V] | Low-biomass regions |
| NSIDC Sea Ice Index G02135 | Arctic sea ice | Citation a condition of use, no formal licence [V] | Demonstration of *no* CSD and of sensitivity to deseasonalisation |
| NTL LTER Cascade lakes (EDI) | Peter/Paul food-web and enrichment experiments | "CC-BY" per NTL [S]; check each EDI package | Known-truth demonstration |
| Project Tycho | US measles, pertussis | CC BY 4.0 [S]; registration [V] | After Southall 2021 and Brett & Rohani 2020 |
| CHB-MIT Scalp EEG; Siena Scalp EEG (PhysioNet) | Seizures, scalp | ODC-By 1.0 [V]; CC BY 4.0 [V] | Onset/offset classes; pre-ictal indicators |
| OpenNeuro ds003029, ds003876, ds004100, ds004752, ds005398 | Seizures, intracranial; onset zones | CC0 [V] | Fragility and critical modes on cortex; termination (Kramer 2012) |
| Sleep-EDF Expanded; CAP Sleep (PhysioNet); OpenNeuro ds004902 | Sleep; sleep deprivation | ODC-By [V]; CC0 [V] | Criticality across sleep and wake |
| OpenNeuro ds003478 | Depression, resting EEG | CC0 [V] | LRTC, aperiodic exponent |
| PhysioNet/CinC 2019 (sepsis); afpdb, ltafdb, Fantasia | Sepsis; onset of atrial fibrillation; HRV and age | CC BY 4.0 [V]; ODC-By [V] | Physiological deterioration |
| VitalDB (PhysioNet) | Intraoperative anaesthesia | CC BY 4.0 [V] | Induction, emergence, hysteresis |
| DANDI Neuropixels datasets | Avalanches, branching ratio | CC0 or CC-BY per dataset [S] | Avoids CRCNS's and Allen's terms |
| UCDP (incl. GED) | Conflict onsets, terminations, fatalities | CC BY 4.0 [V] | Against ViEWS's published forecasts |
| Penn World Table 11.0; World Bank data | Growth, structural change | CC BY 4.0 [V]; World Bank with additional mandatory terms [V] | Structural change |
| HMD / HFD | Mortality and fertility by age | CC BY 4.0 for constructed data; inputs under providers' licences [V] | Lowest-low onsets, age structures |
| V-Dem | Episodes of autocratisation and democratisation | CC BY-SA 4.0 [V] | Regime change |
| Jordà–Schularick–Taylor Macrohistory | Credit, asset prices, crisis dates since 1870 | CC BY-NC-SA 4.0; resale forbidden [V] | Non-commercial; CSD against the credit-gap logit |
| BIS statistics | Credit-to-GDP gaps, debt service | Redistribution for non-commercial purposes [V] | Non-commercial |
| Open power-grid frequency database | Frequency at one second, 12 synchronous areas | Licence not verified | Grid CSD with heavy-tailed noise |

**Gated or restricted, to be described or fetched by the user, never bundled:**

- TUH EEG (signed form);
- Epilepsy Ecosystem (registration; no licence stated on the homepage);
- CRCNS (science only, no redistribution);
- the Bonn segments (citation only; too short for anticipation);
- MIMIC-IV and eICU (credentialing, CITI training, data use agreement);
- Allen Institute data (noncommercial);
- ACLED (EULA; public-sector and corporate licences; limits on AI and ML use);
- copyrighted FRED series (the owner's permission);
- highD and its siblings (non-commercial, on request, no redistribution).

(Part IV §7; Part V §9.)

**Not verified, to be checked before use:** VODCA v2, GIMMS NDVI3g, the OSI SAF and ESA CCI sea-ice records, WHO measles, OWID, the Kericho and Niger data, the individual EDI packages, OECD ICIO, UN WPP, PeMS, NGSIM, Seshat, ENTSO-E, GHSL, NHGIS, Shiller's data, ViEWS's forecasts, the IMF terms of the Laeven–Valencia database, and the ECB/ESRB crises database (Part III §5; Part V §9).

None of the recommended demonstrations uses a Japanese city (Part III §6).

### 1.9 What a Detector Must Not Claim

The negative results of the five Parts translate into claims a detector, or a report built on it, must not make.

- **On transitions in general:**
  - that a rejection of a trend null shows approach to a bifurcation; it shows a trend unlikely under that null (Part I §4);
  - that CSD is specific to tipping points (Kéfi et al. 2013), or that every transition is preceded by it (Hastings & Wysham 2010; Ditlevsen & Johnsen 2010);
  - that variance alone indicates a bifurcation; a rise in noise raises variance alone (Ditlevsen & Johnsen 2010; Part III §6.3);
  - that rising variance signals loss of stability where the noise statistics may have changed: merged instruments, heavy-tailed grid frequency, trading intervals and renewables (Part I §4; Part V §6).
- **On timing:**
  - a tipping time without an explicit model and its full uncertainty; propagated uncertainty spans centuries to millennia (Ben-Yami et al. 2024; Morr et al. 2026);
  - that a fitted log-periodic pattern predicts a crash date (Laloux et al. 1999; Feigenbaum 2001; Chang & Feigenbaum 2006).
- **On skill:**
  - that success on series known to end in a transition establishes forecasting skill. The prosecutor's fallacy applies to the Neolithic and Pueblo results, and to case-study reviews (Part V §10.2.4; Part III §3);
  - that high in-sample fit implies out-of-sample skill (Beutel et al. 2019; Ward et al. 2010);
  - that forecasts are equally good for rare, low-level or first onsets (Hegre et al. 2021; Mueller & Rauh 2018).
- **On finance:** that rising autocorrelation anticipates financial crashes in general (Guttal et al. 2016; Diks et al. 2019).
- **On thresholds, networks and phases:**
  - that a threshold estimated from the same data is evidence of tipping. Sample splitting is required (Card, Mas & Rothstein 2008; Easterly 2009);
  - that a change of network topology is predictable, or unpredictable, without a null that preserves degree heterogeneity (Squartini et al. 2013);
  - that an urban scaling exponent identifies a city's phase regardless of how its boundary is drawn (Arcaute et al. 2015; Leitão et al. 2016);
  - that a social tipping point has been identified when only a narrative of positive feedback is offered (Milkoreit 2022).
- **On brains and bodies:**
  - that a power law establishes criticality (Touboul & Destexhe 2010, 2017);
  - that the aperiodic exponent is an E/I ratio (Brake et al. 2024; Gerster et al. 2022);
  - that phenomenological-RG scaling is proximity to a critical point (Part II §7);
  - that an indicator rising before seizures is a precursor rather than a multidien rhythm, unless it beats a baseline of cycles (Part IV §2);
  - that any criticality or EWS metric for mood, intensive care or ageing is clinically validated; only neonatal monitoring of heart-rate characteristics has randomised evidence of benefit (Part IV §9.4).
- **On particular indicators:**
  - that a topological warning differs from a variance warning without volatility-matched nulls (Part II §5);
  - that an ecological "Fisher information" index is the Fisher information of a parameter (Part II §5);
  - that a spatial pattern is a warning; it may be a sign of resilience (Rietkerk et al. 2021).
- **On what a detector outputs:**
  - that dating a regime after the fact is anticipation (Part V §3);
  - that a physics-based AMOC diagnostic (F_ovS, buoyancy flux) is a generic detector. It is a diagnostic of model or reanalysis fields (Part III §1, §6);
  - that the thresholds of assessments such as the Global Tipping Points Report 2025 are detector outputs. They are declared data with their stated confidence (Part III §6).

### 1.10 What the Survey Leaves Open

- **No benchmark.** No benchmark suite exists, with standard datasets and metrics, for univariate, multivariate, spatial or social detectors (Part I §8; Part II §1; Part V §10.4).
- **No calibrated multivariate rates.** No study reports calibrated false-alarm and detection rates for multivariate indicators against cross-correlated nulls (Part II §1), nor for spectral-gap indicators (Part II §6).
- **No R-tipping detector from records.** No validated record-only detector of R-tipping exists (Part I §9).
- **No structured geometry for CSD.** No published work defines CSD in Aitchison geometry, in Wasserstein space or on the SPD cone, or tests early warnings of economic structural change on compositions or input–output matrices (Part II §4–5).
- **Untested social and economic transitions.** No test of generic indicators was found before fertility onsets or collapses, migration surges, the onset of urban shrinkage, the collapse of urban services, recessions, or episodes of structural change (Part V §1–3).
- **No validation on a real city.** No multivariate or spatial CSD detector has been evaluated on a real city's block-level records, so the design's flagship use has no precedent (Part V §10.4).
- **Zoning.** The modifiable areal unit problem has no general theory or benchmark for early warnings across irregular administrative hierarchies (Part II §2, §7).
- **Whether CSD is visible at all** in empirical structured systems remains contested: seizures (Maturana 2020 against Wilkat 2019), lakes (O'Brien 2023), tipping times (Ben-Yami 2024).
- **Key texts read only in part:** Ashwin et al. 2012, Wieczorek et al. 2023, Feudel 2023, Boers 2021, Bury et al. 2020's spectral formulas, Hengen & Shew 2025, Sipling et al. 2026, O'Brien et al. 2023, and the Author Correction to Ditlevsen & Ditlevsen 2023 (Parts I, III, IV, Gaps).
- **Unverified equations.** Several formulas the design would use as fixtures are written from memory and must be re-derived first: the Bury 2020 spectra, Wissel's exponent, the SNIC and homoclinic period scalings, crisis lifetimes, the entropy production of an OU process, the MR estimator, the crackling relation, specparam, the Clauset MLE, the Epileptor, and the parameter tables of Welander's, Brovkin's, Carpenter's and North's models (Parts I–IV, Gaps).

## Part I. Theory of Critical Transitions and Detectors on a Record

*Remark.* Part I's conventions. The bibliographic markers are those of §0.3. Content markers say where each claim's content comes from: {txt} the full text was read this session (arXiv or publisher PDF converted to text); {abs} the abstract was read this session (from Crossref's abstract field or the arXiv page); {sec} a secondary summary (search result); {mem} memory, not checked this session. Formulas marked "(from memory)" were written without seeing the source's equation; equation numbers are given only where the text was seen.

*Caveat.* Article numbers and pages. For [V] items the DOI, authors, year, title, venue and volume were verified. Where Crossref returned no page or article number, an article number or page range shown here (for example "Sci. Rep. 9:8654", "GRL 31:L23207", "Nat. Commun. 14:4254") was added from memory and should be checked; the DOI is authoritative. Boulton, Lenton & Boers 2022: only the Research Square preprint record was verified in this Part; the Nature Climate Change volume and pages are from memory.

Notation. x is the state, μ (or λ, r, y) the slowly varying parameter, μ_c its critical value, ε the ratio of slow to fast timescales, σ the noise amplitude, κ (or λ in Boers's papers) the linear restoring rate (minus the dominant eigenvalue of the drift's Jacobian), Δt the sampling step.

### 1. Tipping Is Classified by Mechanism: Bifurcation-, Noise-, Rate- and Shock-Induced

#### Takeaway
Ashwin et al. (2012) fixed the classification now used everywhere: B-tipping (a quasi-static parameter passes a bifurcation of the frozen system), N-tipping (noise carries the state across a basin boundary while the attractor still exists) and R-tipping (the parameter changes faster than a critical rate, so the state fails to track a branch that never bifurcates). Shock tipping (S-tipping, a single large perturbation) and tipping under periodic or chaotic forcing are later additions. Critical slowing down (CSD) is a theorem only for B-tipping through local codimension-one bifurcations under timescale separation. R-tipping and N-tipping are in general blind spots of every CSD indicator, and a detector must say so.

#### Cited Findings
- **The three types.** Ashwin, Wieczorek, Vitolo & Cox define B-, N- and R-tipping. They show that R-tipping "need not be associated with either bifurcations or noise", and give all three in a simple global energy balance model {abs} — [Ashwin, Wieczorek, Vitolo & Cox 2012, Phil. Trans. R. Soc. A 370:1166–1184 [V]](https://doi.org/10.1098/rsta.2011.0306).
- **The first worked R-tipping example.** The "compost-bomb instability" is a soil-carbon model in which a sufficiently fast rise in temperature triggers runaway heating without any bifurcation of the frozen system {mem}. Its formalism of critical rates is the precursor of the 2012 classification {mem} — [Wieczorek, Ashwin, Luke & Cox 2011, Proc. R. Soc. A 467:1243–1269 [V]](https://doi.org/10.1098/rspa.2010.0485).
- **R-tipping in terms of pullback attractors.** For a smooth "parameter shift" between two asymptotic values, there is a unique local pullback point attractor attached to each linearly stable past equilibrium. If a stable branch exists over the whole range of the shift, the pullback attractor tracks the branch for small enough rates; larger rates may lead to R-tipping {abs} — [Ashwin, Perryman & Wieczorek 2017, Nonlinearity 30:2185–2210 [V]](https://doi.org/10.1088/1361-6544/aa675b).
- **R-tipping in many dimensions.** Wieczorek, Xie & Ashwin give a framework for R-tipping in multidimensional nonautonomous systems with an autonomous future limit, where tipping comes from loss of tracking of base equilibria by crossing "regular R-tipping thresholds". In general it "cannot … be understood in terms of autonomous bifurcations in the frozen system" {abs}. Its criteria use edge states (saddles on the basin boundary) and connecting orbits from the past-limit attractor to them {mem} — [Wieczorek, Xie & Ashwin 2023, Nonlinearity 36:3238–3293 [V]](https://doi.org/10.1088/1361-6544/accb37).
- **A testable criterion: basin instability.** O'Keeffe & Wieczorek define "basin instability" of equilibria, a testable criterion read from the frozen system's global properties. It identifies parameter paths along which genuine R-tipping occurs without crossing a bifurcation. Their bistable plant–herbivore example also contains a degenerate codimension-three Bogdanov–Takens point that organises B-tipping {abs} — [O'Keeffe & Wieczorek 2020, SIAM J. Appl. Dyn. Syst. 19:2371–2402 [V]](https://doi.org/10.1137/19M1242884).
- **A review of rate-induced tipping.** Feudel reviews R-tipping in ecosystems and climate, with emphasis on R-tipping by crossing basin boundaries, unstable states and transients {abs} — [Feudel 2023, Nonlin. Processes Geophys. 30:481–502 [V]](https://doi.org/10.5194/npg-30-481-2023).
- **R-tipping across applications.** Ritchie, Alkhayuon, Cox & Wieczorek survey R-tipping across natural and human systems {mem} — [Ritchie, Alkhayuon, Cox & Wieczorek 2023, Earth Syst. Dynam. 14:669–683 [V]](https://doi.org/10.5194/esd-14-669-2023).
- **Chaotic internal dynamics.** Under parameter drift, systems with chaotic internal dynamics show further tipping types: tipping from and into several coexisting attractors, "fractality-induced tipping" (from the fractality of scenario-dependent basins), tipping into a chaotic attractor, and random tipping from an extended chaotic attractor into coexisting regular attractors {abs} — [Kaszás, Feudel & Tél 2019, Sci. Rep. 9:8654 [V]](https://doi.org/10.1038/s41598-019-44863-3). A broad review of chaos under parameter drift exists — [Jánosi & Tél 2024, Phys. Rep. 1092:1–64 [V]](https://doi.org/10.1016/j.physrep.2024.09.003).
- **Tipping probabilities between chaotic attractors.** For systems with autonomous past limits, physical measures can be defined relative to the past limit. Rate-dependent tipping between chaotic attractors is then quantified as "tipping probabilities" (examples: a Chua double scroll, and a Stommel model forced by Lorenz chaos) {abs} — [Ashwin & Newman 2021, Eur. Phys. J. Spec. Top. 230:3235–3248 [V]](https://doi.org/10.1140/epjs/s11734-021-00114-z).
- **Rate and noise together.** Noise can cause tipping well below the deterministic critical rate, with probability significantly higher than noise alone. The most probable tipping path is a heteroclinic connection of the Euler–Lagrange system of the Freidlin–Wentzell action, and it exists for all rates up to the critical one {abs} — [Slyman & Jones 2023, Chaos 33:023114 [V]](https://doi.org/10.1063/5.0129341). An elementary introduction appeared in 2026 — [Slyman 2026, Notices AMS 73 [V]](https://doi.org/10.1090/noti3426).
- **The probability of N- and R-tipping.** Ritchie & Sieber compute the probability of noise- and rate-induced tipping — [Ritchie & Sieber 2017, Phys. Rev. E 95:052209 [V]](https://doi.org/10.1103/PhysRevE.95.052209) {mem for content}.
- **Overshoot.** If a parameter crosses a fold threshold and turns back, tipping is avoided when the maximal exceedance and the time spent above threshold satisfy an inverse-square law, expressed in observable properties of the system near the fold. A probability of tipping is given under stochastic forcing {abs} — [Ritchie, Karabacak & Sieber 2019, Proc. R. Soc. A 475:20180504 [V]](https://doi.org/10.1098/rspa.2018.0504). The applied version is "overshooting tipping point thresholds" — [Ritchie, Clarke, Cox & Huntingford 2021, Nature 592:517–523 [V]](https://doi.org/10.1038/s41586-021-03263-2).
- **A cascade with R-tipping.** In a conceptual cascade (sea ice to a Stommel box ocean), the ocean shows R-tipping: an abrupt resurgence of overturning "before a bifurcation point is reached due to the fast rate of change of the sea-ice" {abs} — [Lohmann, Castellana, Ditlevsen & Dijkstra 2021, Earth Syst. Dynam. Discuss. (esd-2021-7) [V]](https://doi.org/10.5194/esd-2021-7).
- **Shock tipping.** The "minimal fatal shock" is the shortest perturbation vector that leaves the desired state's basin. Its length is a global stability measure and its direction locates weak motifs (shown in plant–pollinator networks and the British grid) {abs} — [Halekotte & Feudel 2020, Sci. Rep. 10:11783 [V]](https://doi.org/10.1038/s41598-020-68805-6).
- **Periodic forcing.** Existing methods presuppose timescale separation between system and forcing. When periodic forcing has a timescale comparable to the internal dynamics, the system is not near a fixed point, and alternative early-warning signals exist for local bifurcations of the periodic state {abs} — [Williamson, Bathiany & Lenton 2016, Earth Syst. Dynam. 7:313–326 [V]](https://doi.org/10.5194/esd-7-313-2016). Small shifts in the amplitude or timescale of environmental oscillations (seasons, Milankovitch cycles, internal oscillations) can evoke abrupt and even irreversible change {abs} — [Bathiany, Scheffer, van Nes, Williamson & Lenton 2018, Sci. Rep. 8:5040 [V]](https://doi.org/10.1038/s41598-018-23377-4).
- **Reviews of tipping beyond bifurcation.** [Bathiany et al. 2016, Dyn. Stat. Clim. Syst. 1:dzw004 [V]](https://doi.org/10.1093/climsys/dzw004); [Feudel, Pisarchik & Showalter 2018, Chaos 28:033501 [V]](https://doi.org/10.1063/1.5027718) {abs: classes of multistable systems and ways to distinguish critical transitions}; [Boers, Ghil & Stocker 2022, Environ. Res. Lett. 17:093006 [V]](https://doi.org/10.1088/1748-9326/ac8944) {mem: theory and paleoclimatic evidence of abrupt transitions}; [Lucarini & Bódai 2020, Nonlinearity 33:R59–R92 [V]](https://doi.org/10.1088/1361-6544/ab86cc) {mem: edge states, "Melancholia" saddles and the large-deviation view of multistable climate}; [Ghil & Lucarini 2020, Rev. Mod. Phys. 92:035002 [V]](https://doi.org/10.1103/RevModPhys.92.035002).
- **Climate tipping elements.** The concept and its updates: [Lenton et al. 2008, PNAS 105:1786–1793 [V]](https://doi.org/10.1073/pnas.0705414105) {abs}; [Armstrong McKay et al. 2022, Science 377:eabn7950 [V]](https://doi.org/10.1126/science.abn7950) {abs}; [Lenton et al. 2019, Nature 575:592–595 [V]](https://doi.org/10.1038/d41586-019-03595-0).
- **N-tipping in a paleo record.** Ditlevsen & Johnsen analysed a high-resolution ice core and excluded the bifurcation scenario for Dansgaard–Oeschger events. This "strongly suggests that they are noise induced and thus have very limited predictability" {abs} — [Ditlevsen & Johnsen 2010, GRL 37:L19703 [V]](https://doi.org/10.1029/2010GL044486).

#### Inferences
- A detector's report should name the mechanism class it presupposes. CSD indicators answer only one question: is a local linear restoring rate trending to zero? They are silent by construction for R-tipping when the frozen system's stability is constant along the path. They are silent for N-tipping at fixed parameters, for S-tipping, and for global or non-smooth transitions (section 2).
- A clean R-tipping test case can be derived (derived here, not from a source). Take ẋ = (x + rt)² − 1. In the co-moving variable y = x + rt it becomes ẏ = y² − 1 + r. The frozen system has a stable equilibrium with constant eigenvalue −2 for every frozen parameter, so it has no bifurcation and no CSD. The moving system has a tracking equilibrium y = −√(1 − r), with eigenvalue −2√(1 − r), only for r < 1, so the critical rate is r_c = 1 exactly.
  - At constant r < 1, fluctuations about the tracking path are stationary, and CSD indicators must show no trend.
  - If r itself increases toward 1, the co-moving eigenvalue goes to zero, and indicators computed in the co-moving frame (detrended residuals) can rise. This is consistent with Ritchie & Sieber's finding of delayed indicators (section 2).
- N-tipping and B-tipping are not exclusive. Near a fold the escape time falls exponentially in the barrier height, so noise usually tips the system before μ_c (Ditlevsen & Ditlevsen's closed form, section 5). Any time-to-tipping estimate must therefore treat μ_c as an upper bound on the tipping time, not the tipping time.

#### Gaps
- No general, model-free indicator of R-tipping from a scalar record was found that is established. The 2024 deep-learning detector and a 2026 Koopman preprint claim progress (section 6), but neither is independently validated.
- Full texts of Ashwin et al. 2012, Wieczorek et al. 2023 and Feudel 2023 were not read. Statements beyond their abstracts are from memory.

### 2. Critical Slowing Down Follows from Fast–Slow Normal Forms and Fails Where Their Hypotheses Fail

#### Takeaway
Near a local codimension-one bifurcation of the fast subsystem, the linearised restoring rate κ goes to zero with a power of the distance to the bifurcation. For small additive noise the stationary variance scales as σ²/(2κ) and the autocorrelation time as 1/κ. Kuehn (2013) derives the leading-order variance laws from normal forms with a slow parameter: σ²/√y for the fold, σ²/y for transcritical and pitchfork, σ²/y₂ for the cusp, and O(1/y) for each coordinate at a Hopf. Each type has a distinct signature:
- a fold adds skewness toward the other state;
- a Hopf gives a sharpening spectral peak at ω₀ and an oscillating autocorrelation;
- a period-doubling gives lag-1 autocorrelation tending to −1 (decreasing).

CSD fails or misleads in the following cases, each documented:
- systems without smooth potentials (non-smooth transitions);
- non-bifurcative saddle escapes;
- global bifurcations and crises;
- noise-induced transitions;
- environments to which the system becomes less sensitive (variance can fall);
- red or nonstationary noise (spurious or masked alarms).

#### Cited Findings
- **A critical transition defined geometrically.** Kuehn (2011) defines a critical transition in fast–slow systems εẏ… (fast x, slow y): a trajectory leaves a normally hyperbolic attracting slow manifold through a bifurcation of the fast subsystem and follows a fast segment. The paper relates this to bifurcations, fast–slow theory and stochastic dynamics {mem} — [Kuehn 2011, Physica D 240:1020–1035 [V]](https://doi.org/10.1016/j.physd.2011.02.012).
- **Classification by normal form.** Kuehn (2013) classifies critical transitions by normal forms in the singular limit, for fast-subsystem bifurcations up to codimension two. Table 1 gives the criticality conditions on the slow flow g and the linearisation A₀(y) = D_x f along the attracting critical manifold {txt}:
  - fold: A₀ = O(√y);
  - transcritical and pitchfork: A₀ = O(y);
  - Hopf: A₀ = [[y, −1], [1, y]];
  - cusp, Bautin, Bogdanov–Takens, fold–Hopf: further entries;
  - Hopf–Hopf: only a special case.

  — [Kuehn 2013, J. Nonlinear Sci. 23:457–510 [V]](https://doi.org/10.1007/s00332-012-9158-x) (arXiv:1101.2908 [V]).
- **Kuehn's Theorem 5.1 (one fast variable).** Var(ξ_s) = σ²[H(y)] + O(σ³), with H₀ solving 0 = 2A₀(y)X + N(y), i.e. H₀ = −N(y)/(2A₀(y)) (his eq. 55) {txt}. To lowest order, for constant noise matrix N, "the variance scales … as σ²/√y for the fold, as σ²/y for the transcritical/pitchfork and as σ²/y₂ for the cusp". Here y₂ is the slow variable multiplying the linear term of the cusp normal form (the square root is lost in the PDF text; consistent with A₀ = O(√y) in Table 1) {txt}.
- **Kuehn's Theorem 5.2 (two fast variables: Hopf, Bogdanov–Takens, Bautin).** It gives the covariance matrix H₀(y) explicitly. For the Hopf with fixed noise level "the variance of the coordinates increases as O(1/y)" {txt}. The paper also shows that in a predator–prey example "normal-form early warnings can be misleading" near a codimension-two point. In Euler buckling the warning depends on the control strategy (multiplicative noise) {txt/abs}.
- **Slow passage through a fold.** Near the fold, the slow passage has the canonical scaling region (x, y) = (O(ε^{1/3}), O(ε^{2/3})) {txt; the exponents read from Kuehn 2013 §2}. The jump therefore happens O(ε^{2/3}) past the static fold. A delay of this kind, and the dynamic-Hopf delay with memory, are standard results of fast–slow theory {mem} — [Kuehn 2015, *Multiple Time Scale Dynamics*, Springer [V]](https://doi.org/10.1007/978-3-319-12316-5); [Berglund & Gentz 2006, *Noise-Induced Phenomena in Slow-Fast Dynamical Systems*, Springer [V]](https://doi.org/10.1007/1-84628-186-5). Berglund and Gentz also give noise thresholds above which escape precedes the fold (scaling σ ≫ ε^{1/2} for the fold) {mem}.
- **Climate tipping as a noisy bifurcation.** Thompson & Sieber review this view and propose predicting the tipping point by estimating the decay rate in windows and extrapolating it to zero {mem}. Near a fold κ² is linear in μ; near a transcritical point κ itself is linear (from memory) — [Thompson & Sieber 2011, Int. J. Bifurc. Chaos 21:399–423 [V]](https://doi.org/10.1142/S0218127411028519); [Thompson & Sieber 2011, IMA J. Appl. Math. 76:27–46 [V]](https://doi.org/10.1093/imamat/hxq060). "Nonlinear softening" of the potential is offered as a further precursor {mem} — [Sieber & Thompson 2012, Phil. Trans. R. Soc. A 370:1205–1227 [V]](https://doi.org/10.1098/rsta.2011.0372).
- **The return-time law.** Wissel derived a "universal law of the characteristic return time near thresholds": the return time diverges as the threshold is approached {mem for the exponent} — [Wissel 1984, Oecologia 65:101–107 [V]](https://doi.org/10.1007/BF00384470).
- **Noisy precursors.** Wiesenfeld gave noisy precursors of nonlinear instabilities of periodic orbits: spectral features growing before codimension-one bifurcations, such as a peak at half the drive frequency before period doubling {mem} — [Wiesenfeld 1985, J. Stat. Phys. 38:1071–1097 [V]](https://doi.org/10.1007/BF01010430).
- **The linear stochastic model behind it.** Hasselmann's stochastic climate model (a slow variable integrating fast weather noise, giving an Ornstein–Uhlenbeck red spectrum) is the model behind all CSD indicators {mem} — [Hasselmann 1976, Tellus 28:473–485 [V]](https://doi.org/10.1111/j.2153-3490.1976.tb00696.x).
- **The canonical reviews.** CSD implies rising variance, rising autocorrelation, skewness and flickering — [Scheffer et al. 2009, Nature 461:53–59 [V]](https://doi.org/10.1038/nature08227); [Scheffer et al. 2012, Science 338:344–348 [V]](https://doi.org/10.1126/science.1225244); [Lenton 2011, Nat. Clim. Change 1:201–209 [V]](https://doi.org/10.1038/nclimate1143); [Kéfi et al. 2014, PLoS ONE 9:e92097 [V]](https://doi.org/10.1371/journal.pone.0092097) (spatial methods, for reference).
- **Skewness.** Guttal & Jayaprakash propose changing skewness as an early warning. They argue that both reduced resilience and increased external fluctuations can tip ecosystems, and that skewness warns for both routes {abs} — [Guttal & Jayaprakash 2008, Ecol. Lett. 11:450–460 [V]](https://doi.org/10.1111/j.1461-0248.2008.01160.x).
- **Variance can decrease.** Variance "may sometimes decrease close to a transition". This happens when environmental factors fluctuate and the ecosystem becomes less sensitive to them near the threshold, or when CSD reduces the capacity to follow high-frequency environmental fluctuations {abs} — [Dakos, van Nes, D'Odorico & Scheffer 2012, Ecology 93:264–271 [V]](https://doi.org/10.1890/11-0889.1).
- **Systems without smooth potentials.** The class of systems with leading indicators is limited. Nonlinearity combined with environmental variability gives models without smooth potentials, which "will not show typical leading indicators" {abs} — [Hastings & Wysham 2010, Ecol. Lett. 13:464–472 [V]](https://doi.org/10.1111/j.1461-0248.2010.01439.x).
- **Silent catastrophes.** Structured population models collapse catastrophically without early warning {mem; title} — [Boerlijst, Oudman & de Roos 2013, PLoS ONE 8:e62033 [V]](https://doi.org/10.1371/journal.pone.0062033).
- **Saddle-escape transitions.** In high-dimensional systems, a frequent non-bifurcative saddle-type mechanism leads to critical transitions. Near a saddle the system moves slowly and the state may be perceived as stable, and then escapes {abs} — [Kuehn, Zschaler & Gross 2015, Sci. Rep. 5:13190 [V]](https://doi.org/10.1038/srep13190).
- **Non-catastrophic transitions also warn.** CSD indicators indicate "a broader class of situations where a system becomes increasingly sensitive to perturbations"; they are not specific to tipping points {abs} — [Kéfi, Dakos, Scheffer, van Nes & Rietkerk 2013, Oikos 122:641–648 [V]](https://doi.org/10.1111/j.1600-0706.2012.20838.x).
- **Robustness across transition types.** Most ecological EWS were studied for the saddle-node under density-independent white noise. Dutta, Sharma & Abbott test other transition types and noise schemes {abs} — [Dutta, Sharma & Abbott 2018, Oikos 127:1251–1263 [V]](https://doi.org/10.1111/oik.05172).
- **Two-species models.** CSD as a transition indicator was studied in two-species models {mem} — [Chisholm & Filotas 2009, J. Theor. Biol. 257:142–149 [V]](https://doi.org/10.1016/j.jtbi.2008.11.008).
- **Spatial evasion of tipping.** Spatial pattern formation can let ecosystems and Earth-system components evade tipping, so some tipping-prone systems may not undergo critical transitions {abs} — [Rietkerk et al. 2021, Science 374:eabj0359 [V]](https://doi.org/10.1126/science.abj0359).
- **R-tipping indicators lag.** In the saddle-node normal form with time-varying equilibrium drift and noise, "both most commonly used early-warning indicators, increase in variance and increase in autocorrelation, occur not when the equilibrium drift is fastest but with a delay". The most likely tipping trajectory (from a variational problem solved by continuation) also crosses with a delay {txt} — [Ritchie & Sieber 2016, Chaos 26:093116 [V]](https://doi.org/10.1063/1.4963012) (arXiv:1509.01696 [V]).
- **Crises.** A boundary crisis of a chaotic attractor (collision with an unstable periodic orbit) makes the attractor disappear into transient chaos. The mean transient lifetime scales as a power of the distance past the crisis {mem for the scaling}. No equilibrium-type slowing is implied — [Grebogi, Ott & Yorke 1983, Physica D 7:181–200 [V]](https://doi.org/10.1016/0167-2789(83)90126-4).
- **An ambiguity review (2025).** Rietkerk, Skiba, Weinans, Hébert & Laepple argue that statistical early warnings are ambiguous for climate tipping points. Among the confounders: CSD also occurs without tipping points, and noise- and rate-induced tipping cannot be anticipated by EWSs {sec} — [Rietkerk et al. 2025, Nat. Clim. Change 15:479–488 [V]](https://doi.org/10.1038/s41558-025-02328-8).

The signatures of each bifurcation follow from the linearisation (closed forms from memory, standard; they are the basis of Bury et al. 2020, section 3):

| Transition (type of record) | Restoring rate κ vs distance δ = μ_c − μ | Variance | Lag-1 AC | Spectrum | Higher moments, other signs |
|---|---|---|---|---|---|
| Fold / saddle-node (flow) | κ ∝ √δ | ∝ δ^{−1/2} | e^{−κΔt} → 1 | Lorentzian σ²/(2π(κ² + ω²)), reddening | Skewness toward the far state; flickering if noise is large |
| Transcritical, pitchfork (flow) | κ ∝ δ | ∝ δ^{−1} | → 1 | Lorentzian, reddening | Symmetric for the pitchfork; non-catastrophic for the supercritical pitchfork |
| Hopf (flow) | Re λ = −κ ∝ δ, Im λ = ω₀ | ∝ δ^{−1} per coordinate | e^{−κΔt} cos(ω₀Δt): may fall if ω₀Δt > π/2 | Peak at ω₀ sharpening (width κ) | Oscillating ACF; subcritical vs supercritical not told by the linear part |
| Period-doubling (map) | multiplier → −1 | grows | → −1 (decreasing) | Peak at the Nyquist frequency | Bury et al. 2023 classify it |
| Neimark–Sacker (map) | complex multipliers → unit circle | grows | oscillating | Peak at the rotation frequency | |
| Fold of a map, transcritical, pitchfork (map) | multiplier → +1 | grows | → +1 | Reddening | |
| SNIC, homoclinic (global) | no local κ → 0 on the cycle | not generic | not generic | — | Period lengthens (∝ δ^{−1/2} for SNIC, ∝ −log δ for homoclinic; from memory) |
| Boundary crisis, border collision, non-smooth | eigenvalues need not approach 0 | not generic | not generic | — | Transient lifetimes; silent |

#### Inferences
- The exponents give an implementation check and a classifier.
  - On a slow linear ramp, var⁻² is linear in time for a fold, while var⁻¹ is linear for transcritical or pitchfork transitions.
  - Equivalently κ̂² or κ̂ is linear. A test of which power linearises the estimated κ̂(t) is a weak but cheap discriminant of the type.
- Since lag-1 AC can fall at a Hopf (coarse sampling) and does fall at a period-doubling, a detector that tests only "AC increasing" is blind to those types. It must test |dominant eigenvalue| → 1 or use the spectrum.
- The infrastructure's existing branching-ratio estimator (Wilting & Priesemann) estimates the dominant eigenvalue m of a linear branching process. m → 1 is a transcritical bifurcation of that process, so the branching ratio is the same indicator as the AR(1) coefficient, applied to counts. This links the "criticality" detectors (avalanches, branching) to the CSD family; they are not separate theories.

#### Gaps
- Kuehn's 2011 paper and the Berglund–Gentz book were not read this session. The ε^{1/2} noise threshold is from memory.
- No quantitative source was found this session for the exponent in Wissel's law, or for the spectral signatures of global bifurcations from records. The table's last rows are from standard bifurcation theory (Kuznetsov 2004 [V], https://doi.org/10.1007/978-1-4757-3978-7), not from early-warning literature.

### 3. Univariate Indicators Estimate the Restoring Rate, the Spectrum, the Shape of the Distribution or the Number of States

#### Takeaway
Indicators fall into five families:
- moments of detrended residuals in rolling windows (variance, CV, lag-1 AC, skewness, kurtosis);
- direct estimates of the restoring rate κ (AR(1) fits, regression of increments, GLS under red noise, autocorrelation- and spectrum-fitting under red noise, Kramers–Moyal drift slopes, recovery from perturbations);
- spectral shape (reddening, ratio of spectra, fitted fold/Hopf/null spectra);
- distributional and state-counting methods (flickering, bimodality, potential analysis, Fisher information, permutation entropy);
- nonlinearity and heteroskedasticity tests (BDS, ARCH-type tests, S-map nonlinearity).

The direct κ estimators are preferable when the noise may change: variance and AC conflate changes of κ with changes of noise amplitude and colour, and Morr & Boers (2024) show red noise causes spurious or masked alarms that their κ estimators avoid.

#### Cited Findings
**3.1 Moments of the residuals**
- **Rising variance.** Carpenter & Brock introduced rising variance as a leading indicator, in a lake eutrophication model with fluctuating recycling. They note the difficulty of telling variance due to an impending shift from that of exogenous drivers {abs} — [Carpenter & Brock 2006, Ecol. Lett. 9:311–318 [V]](https://doi.org/10.1111/j.1461-0248.2005.00877.x).
- **Autocorrelation in climate records.** Increased autocorrelation preceded eight ancient abrupt climate shifts {abs} — [Dakos et al. 2008, PNAS 105:14308–14312 [V]](https://doi.org/10.1073/pnas.0802430105).
- **Degenerate fingerprinting.** Held & Kleinen propose estimating "the smallest decay rate" of the system and its trend as the diagnostic most directly linked to distance from the threshold. They demonstrate it on CLIMBER-2's thermohaline circulation {abs} — [Held & Kleinen 2004, GRL 31:L23207 [V]](https://doi.org/10.1029/2004GL020972). The AR(1) coefficient φ = e^{−κΔt} of residuals is the estimator (formula from memory).
- **Moment formulas (from memory, standard).** For residuals r_t in a window of n points:
  - variance s²;
  - lag-1 AC ρ̂₁ = Σ(r_t − r̄)(r_{t+1} − r̄)/Σ(r_t − r̄)²;
  - skewness m₃/s³;
  - kurtosis m₄/s⁴;
  - CV = s/|x̄|, used when the mean is not detrended.

  For an AR(1) with innovation variance σ_η², Var = σ_η²/(1 − φ²). Under a fixed σ_η, variance and AC are therefore not independent pieces of evidence. Their joint rise is the consistency check Ditlevsen & Johnsen call for {mem}.
- **The method guide.** Dakos et al. (2012) is the standard guide to metric-based indicators (AC, variance, skewness, kurtosis, return rate, DFA, spectral density, conditional heteroskedasticity) and model-based ones (nonparametric drift–diffusion–jump, time-varying AR, threshold AR, potential analysis). It covers Gaussian-kernel detrending, rolling windows, sensitivity analysis and Kendall's τ against surrogates {mem for the list; record [V]} — [Dakos et al. 2012, PLoS ONE 7:e41010 [V]](https://doi.org/10.1371/journal.pone.0041010).

**3.2 The restoring rate estimated directly**
- **Boers's estimator.** Boers (2021) estimates the linear restoring rate by regressing increments on the state. Morr & Boers describe it {txt}: the discrete model Y_{k+1} = φY_k + cV_k, V_{k+1} = ρV_k + z_k (an AR(1) driven by an AR(1)), rearranged as Y_{k+1} − Y_k = (φ − 1)Y_k + cV_k. It is fitted "instead of an ordinary least squares model suitable for white noise" by generalised least squares that accounts for the AR(1) structure of V (statsmodels' GLSAR). The estimate of φ − 1 (the discrete restoring rate) is taken as the stability estimator, and its increase toward 0 as CSD. Applied to AMOC fingerprints — [Boers 2021, Nat. Clim. Change 11:680–688 [V]](https://doi.org/10.1038/s41558-021-01097-4).
- **The ARMA representation.** Morr & Boers's eq. (7) gives the equivalent ARMA representation of the AR(1)-driven-by-AR(1) model {txt}. They state that the unbiased estimator of Boettner & Boers [their ref. 13] is ill-defined on a set of positive probability and needs very long series {txt}. [Boettner & Boers 2022, "Critical slowing down in dynamical systems driven by nonstationary correlated noise", Phys. Rev. Research 4:013230 [M]].
- **Red noise.** In time-correlated red noise, "increasing (decreasing) the correlation of the noise will lead to spurious (masked) alarms for both variance and AC(1)". Morr & Boers propose two estimators of the linear restoring rate under Langevin dynamics driven by white or red noise. One fits the autocorrelation structure ("ACS") and one fits the power spectral density ("PSD"); the continuous-time model is an OU process driven by OU noise. Applied to the end of the African Humid Period, the estimators "rule out spurious signals" from changing noise {txt/abs} — [Morr & Boers 2024, Phys. Rev. X 14:021037 [V]](https://doi.org/10.1103/PhysRevX.14.021037) (arXiv:2310.05587 [V]).
- **Multidimensional extension.** Morr et al. extend this to multidimensional systems with time- and state-dependent noise — [Morr, Riechers, Rydin Gorjão & Boers 2024, Phys. Rev. Research 6:033251 [V]](https://doi.org/10.1103/PhysRevResearch.6.033251) {title}.
- **Applied κ estimates.** Greenland ice sheet height reconstructions {abs} — [Boers & Rypdal 2021, PNAS 118:e2024192118 [V]](https://doi.org/10.1073/pnas.2024192118). The Amazon, with AR(1) on vegetation optical depth {abs} — [Boulton, Lenton & Boers 2022, Nat. Clim. Change 12:271–278; preprint record [V]](https://doi.org/10.21203/rs.3.rs-379902/v1); a dispute and reply exist — [Boulton, Lenton & Boers 2023, Nat. Clim. Change 13:1321–1323 [V]](https://doi.org/10.1038/s41558-023-01854-7).
- **Recovery rate equals variability-based rate.** Smith, Traxl & Boers "empirically confirm that the recovery rates from large perturbations can be closely approximated from internal vegetation variability" across vegetation types and climate zones {abs} — [Smith, Traxl & Boers 2022, Nat. Clim. Change 12:477–484 [V]](https://doi.org/10.1038/s41558-022-01352-2). The relation used is κ = −log(ρ₁)/Δt (from memory).
- **The ratio of spectra (ROSA).** Clarke et al. propose the Ratio of Spectra of a state variable relative to a forcing variable. It is meant for systems forced by time-correlated processes, where the stationary-white-noise assumption fails {abs} — [Clarke, Huntingford, Ritchie & Cox 2023, Environ. Res. Lett. 18:035006 [V]](https://doi.org/10.1088/1748-9326/acbc8d).
- **Kramers–Moyal estimation.** The drift D⁽¹⁾(x) = lim_{τ→0} τ⁻¹E[X(t+τ) − x | X(t) = x] and diffusion D⁽²⁾(x) = lim (2τ)⁻¹E[(X(t+τ) − x)² | x] are estimated by conditional moments (formulas from memory) — [Siegert, Friedrich & Peinke 1998, Phys. Lett. A 243:275–280 [V]](https://doi.org/10.1016/S0375-9601(98)00283-7); review [Friedrich, Peinke, Sahimi & Tabar 2011, Phys. Rep. 506:87–162 [V]](https://doi.org/10.1016/j.physrep.2011.05.003).
  - The CSD indicator is the drift slope κ = −D⁽¹⁾′(x*) at the stable point, which separates changes in D⁽²⁾ (noise) from changes in κ (from memory).
  - It was applied to Dansgaard–Oeschger events {title} — [Rydin Gorjão et al. 2021, Earth Syst. Dynam. Discuss. esd-2021-95 [V]](https://doi.org/10.5194/esd-2021-95).
  - Python implementation [Rydin Gorjão & Meirinhos 2019, JOSS 4:1693 [V]](https://doi.org/10.21105/joss.01693).
- **Recovery from perturbations.** "Slow recovery from perturbations" is a generic indicator {mem; title} — [van Nes & Scheffer 2007, Am. Nat. 169:738–747 [V]](https://doi.org/10.1086/516845). It was shown experimentally in cyanobacteria under light stress — [Veraart et al. 2012, Nature 481:357–359 [V]](https://doi.org/10.1038/nature10723) {title}.
- **Recoveries at several amplitudes.** Recoveries from large perturbations at several amplitudes map the nonlinear restoring force and forecast the bifurcation itself (Epureanu's group; content from memory) — [Lim & Epureanu 2011, Phys. Rev. E 83:016203 [V]](https://doi.org/10.1103/PhysRevE.83.016203); [D'Souza, Epureanu & Pascual 2015, PLoS ONE 10:e0137779 [V]](https://doi.org/10.1371/journal.pone.0137779).
- **The same idea in medicine.** Slowing recovery is a marker of acute severity transitions in chronic diseases — [Olde Rikkert et al. 2016, Crit. Care Med. 44:601–606 [V]](https://doi.org/10.1097/CCM.0000000000001564).

**3.3 Spectral shape**
- **Reddening.** Kleinen, Held & Petschel-Held proposed spectral reddening as a threshold indicator for the thermohaline circulation {title; mem} — [Kleinen, Held & Petschel-Held 2003, Ocean Dyn. 53:53–63 [V]](https://doi.org/10.1007/s10236-002-0023-6).
- **Spectral EWS that tell types apart.** Bury, Bauch & Anand note that conventional EWS "do not indicate the type of transition" and may fail to distinguish Hopf from fold. They are also less reliable under density-dependent noise. Their spectral EWS fit the analytical power spectra of the linearised fold, Hopf and null models to windowed periodograms {abs}. The models are compared by AIC weights (from memory), and the fitted peak frequency and width estimate ω₀ and κ (from memory) — [Bury, Bauch & Anand 2020, J. R. Soc. Interface 17:20200482 [V]](https://doi.org/10.1098/rsif.2020.0482).
  - Analytic forms, from memory, to be checked against the paper's equations: null/fold S(ω) = σ²/(2π(κ² + ω²)); Hopf S(ω) = (σ²/4π)[1/(κ² + (ω − ω₀)²) + 1/(κ² + (ω + ω₀)²)].
- **The low-frequency ratio.** Biggs, Carpenter & Brock used indicators including the ratio of low- to high-frequency spectral density {mem}. They found that indicators may come too late for management to avert a shift {abs: "whether such indicators provide sufficient warning to adapt management"} — [Biggs, Carpenter & Brock 2009, PNAS 106:826–831 [V]](https://doi.org/10.1073/pnas.0811729106).

**3.4 Distribution shape and number of states**
- **Flickering.** Before the transition the system jumps between the alternative basins (a bimodal distribution, rising variance and kurtosis not caused by CSD) {mem} — [Dakos, van Nes & Scheffer 2013, Theor. Ecol. 6:309–317 [V]](https://doi.org/10.1007/s12080-013-0186-4). Observed in Lake Erhai's sediment record {title} — [Wang et al. 2012, Nature 492:419–422 [V]](https://doi.org/10.1038/nature11655).
- **Potential analysis.** Livina, Kwasniok & Lenton estimate the degree of a polynomial potential from the recorded series. This gives the number of wells, and "correctly detects changes in the number of wells in artificial data". In Greenland ice-core proxies it finds a reduction from two climate states to one before the LGM {abs} — [Livina, Kwasniok & Lenton 2010, Clim. Past 6:77–82 [V]](https://doi.org/10.5194/cp-6-77-2010).
  - Method, from memory: kernel density p̂(x), U(x) = −(σ²/2)log p̂(x), fit polynomials of even degree 2L by maximum likelihood, count local minima, slide the window.
  - Extended and applied from the Pliocene to the present — [Livina et al. 2011, Clim. Dyn. 37:2437–2453 [V]](https://doi.org/10.1007/s00382-010-0980-2); independently tested — [Livina, Ditlevsen & Lenton 2012, Physica A 391:485–496 [V]](https://doi.org/10.1016/j.physa.2011.08.025).
- **Nonstationary density modelling.** Kwasniok fits a parametric stationary density whose parameters drift in time, and extrapolates to predict the transition {title; mem} — [Kwasniok 2013, Phys. Rev. E 88:052917 [V]](https://doi.org/10.1103/PhysRevE.88.052917).
- **DFA.** Detrended fluctuation analysis: integrate the series, detrend in boxes of size s, F(s) ∝ s^α (formula from memory) — [Peng et al. 1994, Phys. Rev. E 49:1685–1689 [V]](https://doi.org/10.1103/PhysRevE.49.1685). Livina & Lenton map α (0.5 for white noise up to 1.5 for a random walk) to a "propagator" that tends to 1 at a bifurcation {mem} — [Livina & Lenton 2007, GRL 34:L03712 [V]](https://doi.org/10.1029/2006GL028672). DFA, ACF and filtering choices are compared on paleo and model data: robust indicators before the end of the Younger Dryas, less clear elsewhere {abs} — [Lenton, Livina, Dakos, van Nes & Scheffer 2012, Phil. Trans. R. Soc. A 370:1185–1204 [V]](https://doi.org/10.1098/rsta.2011.0304).
- **Fisher information.** FI = ∫ (p′(s))²/p(s) ds over the distribution of system states in windows; a falling FI is read as loss of order before a regime shift (formula and interpretation from memory) — [Karunanithi, Cabezas, Frieden & Pawlowski 2008, Ecol. Soc. 13:22 [V]](https://doi.org/10.5751/ES-02318-130122). The multivariate version collapses several variables into one index {abs} — [Ahmad, Derrible, Eason & Cabezas 2016, R. Soc. Open Sci. 3:160582 [V]](https://doi.org/10.1098/rsos.160582).
- **Permutation entropy.** H_d = −Σ_π p(π) log p(π) over the d! ordinal patterns (formula from memory). It is maximal (log d!) for white noise and 0 for monotone series — [Bandt & Pompe 2002, Phys. Rev. Lett. 88:174102 [V]](https://doi.org/10.1103/PhysRevLett.88.174102). Complexity measures were used as stress indicators in ecology {title} — [Dakos & Soler-Toscano 2017, Ecol. Complex. 32:144–155 [V]](https://doi.org/10.1016/j.ecocom.2016.08.005). No derivation links permutation entropy to the normal forms was found.

**3.5 Nonlinearity and heteroskedasticity**
- **Conditional heteroskedasticity.** Seekell, Carpenter & Pace propose it as a leading indicator {title}. The method, from memory: fit an AR model, regress squared residuals on lagged squared residuals in rolling windows, and test R² (a Lagrange-multiplier/ARCH test) — [Seekell, Carpenter & Pace 2011, Am. Nat. 178:442–451 [V]](https://doi.org/10.1086/661898).
- **Nonparametric drift–diffusion–jump models.** Carpenter & Brock warn of unknown nonlinear shifts with a nonparametric drift–diffusion–jump model {mem; title} — [Carpenter & Brock 2011, Ecology 92:2196–2201 [V]](https://doi.org/10.1890/11-0716.1); also [Brock & Carpenter 2012, PLoS ONE 7:e45586 [V]](https://doi.org/10.1371/journal.pone.0045586).
- **The BDS test.** The test of Brock, Dechert, Scheinkman & LeBaron detects nonlinearity in residuals and is used as a supplementary indicator in Dakos et al. 2012 {mem} [Brock, Dechert, Scheinkman & LeBaron 1996, Econometric Rev. 15:197–235 [M]].
- **Elevated nonlinearity.** Nonlinearity measured by S-maps (state-dependent local linear models) rises in exploited fish populations {title} — [Dakos, Glaser, Hsieh & Sugihara 2017, J. R. Soc. Interface 14:20160845 [V]](https://doi.org/10.1098/rsif.2016.0845).
- **Time-varying AR(p) and threshold models.** Locally linear AR(p) and VAR(1) state-space models with time-varying coefficients infer an approaching shift when the coefficients show CSD of the local dynamics. Threshold models identify switching {abs} — [Ives & Dakos 2012, Ecosphere 3:58 [V]](https://doi.org/10.1890/ES11-00347.1).
- **ARMA-based indicators.** [Faranda, Dubrulle & Pons 2014, J. Phys. A 47:252001 [V]](https://doi.org/10.1088/1751-8113/47/25/252001) {title}.
- **The dynamical eigenvalue (DEV).** It estimates the dominant eigenvalue of the system from data. |DEV| approaches 1 near a bifurcation, and its position in the complex plane indicates the type of transition. It was demonstrated on models with known bifurcation types and tested on empirical data {abs}; the estimation is by S-map Jacobians, per the abstract's root in "bifurcation theory" and Sugihara's co-authorship (from memory) — [Grziwotz et al. 2023, Sci. Adv. 9:eabq4558 [V]](https://doi.org/10.1126/sciadv.abq4558).

**3.6 Basin stability (requires a model, not a record)**
- **Definition.** Basin stability S_B is the probability that a random perturbation (from a declared distribution on state space) returns to the attractor. It is estimated by Monte Carlo, with binomial standard error √(S_B(1 − S_B)/N) (from memory). It complements linear stability, which can be large while the basin is small {mem} — [Menck, Heitzig, Marwan & Kurths 2013, Nat. Phys. 9:89–92 [V]](https://doi.org/10.1038/nphys2516). Its potentials and limits are discussed in [Schultz, Menck, Heitzig & Kurths 2017, New J. Phys. 19:023005 [V]](https://doi.org/10.1088/1367-2630/aa5a7b).

**3.7 Generalised modelling**
- **Method.** Lade & Gross parameterise the Jacobian of a generalised model by elasticities and timescales, estimated from short records and system knowledge. They compute the leading eigenvalue's approach to zero {mem; title} — [Lade & Gross 2012, PLoS Comput. Biol. 8:e1002360 [V]](https://doi.org/10.1371/journal.pcbi.1002360).

#### Inferences
- **A three-level hierarchy.**
  - (a) Moment indicators: cheap, assume white stationary noise and no change in measurement.
  - (b) κ estimators: robust to noise amplitude, and in Morr & Boers's versions to noise colour.
  - (c) Shape-sensitive estimators: spectra fitted per type, the complex dominant eigenvalue. These see Hopf and period-doubling, where (a) can be blind.
- **Distribution and state-count methods answer a different question.** "How many states are there now, and how close is the noise to switching between them?" This is the only family with any purchase on N-tipping (flickering, wells). It can be fooled by measurement artefacts and requires long windows.
- **Methods that extend to many dimensions:** κ (to a Jacobian or VAR(1) matrix and its eigenvalues), Kramers–Moyal (to a drift field), DEV (S-map Jacobians), spectral fits (to cross-spectra), Fisher information (multivariate by construction), generalised modelling. Potential analysis and permutation entropy do not extend naturally beyond a few dimensions.

#### Gaps
- Boers 2021's own methods section was not read. Its estimator is described here through Morr & Boers's account.
- Bury 2020's spectral formulas and the Livina potential-fitting likelihood were not read this session (from memory).
- No source tested permutation entropy or Fisher information against normal-form theory. Their link to CSD is heuristic.

### 4. Statistical Testing Must State Its Null, Its Error Rates and Its Sensitivity to Processing

#### Takeaway
The customary test computes Kendall's τ of a rolling-window indicator and compares it with τ from surrogates (AR(1) by default). It asks only whether a monotone trend is unusual under a stationary linear process; it does not test proximity to a bifurcation. Overlapping windows make τ's analytic null invalid. Window and bandwidth choices change results, so a sensitivity analysis is required. Selecting records because they ended in transitions biases evidence (the prosecutor's fallacy). Error rates must be quantified as false-alarm and detection rates, with ROC curves from simulations of fitted null and alternative models (Boettiger & Hastings).

Further confounders:
- realistic noise levels;
- sparse or irregular sampling;
- interpolation and aggregation;
- changes in instruments;
- red or nonstationary forcing;
- fast forcing.

Each can create or mask signals. Field evaluations show low agreement among indicators. Tipping-time estimates from historical records carry uncertainties of centuries to millennia (Ben-Yami et al. 2024).

#### Cited Findings
- **Kendall's τ.** τ = (n_c − n_d)/(n(n−1)/2) — [Kendall 1938, Biometrika 30:81–93 [V]](https://doi.org/10.1093/biomet/30.1-2.81). For independent data, Var(τ) = 2(2n + 5)/(9n(n − 1)) (formula from memory, standard). Rolling-window indicators are strongly serially dependent, so surrogates are needed.
- **A practical guide to τ.** It discusses how τ depends on window length, detrending and the part of the record used {mem; record [V]} — [Chen, Ghadami & Epureanu 2022, R. Soc. Open Sci. 9:211346 [V]](https://doi.org/10.1098/rsos.211346).
- **Surrogate data.**
  - Phase-randomised surrogates preserve the power spectrum (null: a stationary linear Gaussian process) — [Theiler et al. 1992, Physica D 58:77–94 [V]](https://doi.org/10.1016/0167-2789(92)90102-S) {mem for content}.
  - IAAFT preserves both spectrum and amplitude distribution (null: a static monotone transform of a linear Gaussian process) — [Schreiber & Schmitz 1996, Phys. Rev. Lett. 77:635–638 [V]](https://doi.org/10.1103/PhysRevLett.77.635) {mem}.
  - AR(1) surrogates fitted to the residuals (Dakos et al. 2012) test only against AR(1) red noise {mem}.
  - Block bootstrap preserves short-range dependence nonparametrically {mem}.
- **Model-based inference: the models.** Boettiger & Hastings (2012a) propose a model-based approach to quantify the trade-off between reliability and sensitivity and to compare indicators, and show that error rates "can be quite severe" {abs}. Their models {txt, glyphs partly lost in the PDF text, reconstructed}:
  - Eq. (1) deterministic fold dx/dt = r_t − x²; near the bifurcation dx/dt = r_t − (θ − x)², with fixed point x̂ = θ + √r_t =: φ(r_t).
  - Eq. (2) the "linearized saddle-node" (LSN) SDE dX = √r_t (φ(r_t) − X_t)dt + σ√φ(r_t) dB_t. The square root of φ in the noise term comes from internal (demographic) noise and could be removed for external noise.
  - Eq. (3) r_t = r₀ − m t. Eq. (4) with m = 0 is the OU null dX = r(θ − X)dt + σ dB.
  - Likelihood: log L = Σ log P(x_i | x_{i−1}, Δt_i) (eq. 5). Moments by the linear-noise closure dE/dt = f(E), dV/dt = 2f′(E)V + g(E)² (eqs 6–7).
  - OU transitions in closed form: E = X_{i−1}e^{−rΔt} + θ(1 − e^{−rΔt}) (eq. 8), V = (σ²/2r)(1 − e^{−2rΔt}) (eq. 9). LSN moments are integrated (eqs 10–11).

  — [Boettiger & Hastings 2012a, J. R. Soc. Interface 9:2527–2539 [V]](https://doi.org/10.1098/rsif.2012.0125) (arXiv:1204.6231 [V]).
- **Model-based inference: the test.** The likelihood ratio of LSN against OU has its null and alternative distributions simulated by parametric bootstrap from the two fitted models (McLachlan's approach). ROC curves summarise the overlap: a diagonal ROC means no power {txt}.
- **The prosecutor's fallacy.** Selecting systems known to have transitioned, and failing to account for the bias this introduces, inflates apparent detection {abs} — [Boettiger & Hastings 2012b, Proc. R. Soc. B 279:4734–4739 [V]](https://doi.org/10.1098/rspb.2012.2085) (arXiv:1210.1204 [V]). Also [Boettiger, Ross & Hastings 2013, Theor. Ecol. 6:255–264 [V]](https://doi.org/10.1007/s12080-013-0192-6) {title}.
- **Required window length and the n-tipping risk.** Ditlevsen & Ditlevsen give closed-form windows needed to detect increased variance or autocorrelation above the pre-ramp baseline at confidence q: their eq. (7) for variance and eq. (8) for autocorrelation {txt}.
  - Detecting a significant increase in autocorrelation needs a longer window than variance, so "variance [is] the better EWS of the two".
  - Close to the bifurcation, CSD reduces the number of independent points per window.
  - When λ ≳ −0.4 (their units) the mean waiting time for noise-induced tipping is shorter than the data window {txt}.

  — [Ditlevsen & Ditlevsen 2023, Nat. Commun. 14:4254 [V]](https://doi.org/10.1038/s41467-023-39810-w) (arXiv:2304.09160 [V]).
- **Tipping-time uncertainty.** Ben-Yami et al. identify three sources of uncertainty in predicting tipping times from historical data {txt}:
  - (i) the modelling assumptions of the methods, including nonstationary red noise, a nonlinear (decelerating) ramp of the forcing, and a mean trend without any bifurcation;
  - (ii) the representativeness of individual time series (fingerprints);
  - (iii) observational uncertainty and preprocessing.

  With DD23's MLE on alternative SST datasets they "obtain tipping times ranging from the 2000s for HadISST to the 3000s". With alternative fingerprints the times range "from the 2000s to beyond the year 4700 for ERSStv5". Across ensembles they find "multimillennial uncertainty ranges with, for some cases, almost a quarter of the tipping times going to infinity" {txt}. Press summaries quote 2050–8065 {sec}. The authors still hold that statistical methods can identify which systems have lost stability {sec} — [Ben-Yami, Morr, Bathiany & Boers 2024, Sci. Adv. 10:eadl4841 [V]](https://doi.org/10.1126/sciadv.adl4841).
- **Robustness to processing.** Sensitivity analyses vary the window (for example 25–75% of the record) and the detrending bandwidth, and report the distribution of τ {mem; record [V]} (Dakos et al. 2012, above).
- **Detrending in epidemics.** Detrending choices alter indicator trends in disease-elimination settings {title} — [Gama Dessavre, Southall, Tildesley & Dyson 2019, J. Theor. Biol. 481:183–193 [V]](https://doi.org/10.1016/j.jtbi.2019.04.011).
- **Systematic false positives.** EWS analysis produces systematic false positives {title; mem: rolling-window artefacts} — [Jäger & Füllsack 2019, PLoS ONE 14:e0211072 [V]](https://doi.org/10.1371/journal.pone.0211072).
- **Realistic noise and sampling.** With large correlated extrinsic noise, a rapidly changing driver and sparse sampling, "all indicators performed poorly" in a three-species model {abs} — [Perretti & Munch 2012, Ecol. Appl. 22:1772–1779 [V]](https://doi.org/10.1890/11-0161.1).
- **Imperfect data.** Epidemic transitions with imperfect data (reporting and aggregation) — [Brett et al. 2018, PLoS Comput. Biol. 14:e1006204 [V]](https://doi.org/10.1371/journal.pcbi.1006204) {title}. Discrete event-sequence data — [Southall, Tildesley & Dyson 2020, PLoS Comput. Biol. 16:e1007836 [V]](https://doi.org/10.1371/journal.pcbi.1007836) {title}.
- **Multi-instrument records.** Changes in the noise structure of merged instrument records can be mistaken for changes in resilience {abs} — [Smith et al. 2023, Earth Syst. Dynam. 14:173–183 [V]](https://doi.org/10.5194/esd-14-173-2023).
- **Forcing rate.** Faster forcing reduces forecastability {title} — [Clements & Ozgul 2016, Ecol. Evol. 6:7787–7793 [V]](https://doi.org/10.1002/ece3.2531). Detectability depends on factors including the rate of forcing and the sampling {title} — [Clements, Drake, Griffiths & Ozgul 2015, Am. Nat. 186:50–58 [V]](https://doi.org/10.1086/681573).
- **Field evaluations.**
  - In five freshwater ecosystems, EWIs were detected in several records, some years ahead, but "varied in reliability, and agreement between indicators was low". Applicability required system knowledge to choose the state variable {abs} — [Gsell et al. 2016, PNAS 113:E8089–E8095 [V]](https://doi.org/10.1073/pnas.1608242113).
  - Across 126 long-term datasets of 55 taxa, consistency between nonlinear change and the indicators was tested {abs; results not read} — [Burthe et al. 2016, J. Appl. Ecol. 53:666–676 [V]](https://doi.org/10.1111/1365-2664.12519).
  - Also [Litzow & Hunsicker 2016, Ecosphere 7:e01614 [V]](https://doi.org/10.1002/ecs2.1614) {title}.
- **Paleoclimate.**
  - The high-frequency band of NGRIP δ¹⁸O shows rising fluctuation levels and Hurst exponents before interstadial onsets {abs} — [Rypdal 2016, J. Clim. 29:4047–4056 [V]](https://doi.org/10.1175/JCLI-D-15-0828.1).
  - DO events in high-resolution records — [Boers 2018, Nat. Commun. 9:2556 [V]](https://doi.org/10.1038/s41467-018-04881-7).
  - Signals inconclusive across Greenland cores {title} — [Hummel, Boers & Rypdal 2025, Earth Syst. Dynam. 16:2035–2062 [V]](https://doi.org/10.5194/esd-16-2035-2025).
- **Review of performance.** Dakos et al. (2024) review 20 years of use across climate, ecology and human systems. Despite limitations, "in the majority of the case studies reviewed, the performance of most early warnings was positive". The supplement tallies variance (136 papers), autocorrelation (119), skewness (38) and power spectrum (18) {sec} — [Dakos et al. 2024, Earth Syst. Dynam. 15:1117–1135 [V]](https://doi.org/10.5194/esd-15-1117-2024). Earlier synthesis: [Dakos, Carpenter, van Nes & Scheffer 2015, Phil. Trans. R. Soc. B 370:20130263 [V]](https://doi.org/10.1098/rstb.2013.0263).
- **The brain.**
  - CSD governs the transition to neuron spiking {title} — [Meisel, Klaus, Kuehn & Plenz 2015, PLoS Comput. Biol. 11:e1004097 [V]](https://doi.org/10.1371/journal.pcbi.1004097).
  - CSD metrics fluctuate over hours to days and, combined with epileptiform discharges, associate with seizure risk {abs} — [Maturana et al. 2020, Nat. Commun. 11:2172 [V]](https://doi.org/10.1038/s41467-020-15908-3).
  - Contrary findings: [Wilkat, Rings & Lehnertz 2019, Chaos 29:091104 [V]](https://doi.org/10.1063/1.5122759) {title}; [Milanowski & Suffczynski 2016, Int. J. Neural Syst. 26:1650053 [V]](https://doi.org/10.1142/S0129065716500532) {abs: nearly 300 seizures, generic indicators not common}.
- **Economies.** [Guttal, Raghavendra, Goel & Hoarau 2016, PLoS ONE 11:e0144198 [V]](https://doi.org/10.1371/journal.pone.0144198) {title: no CSD before financial meltdowns, though rising variability may signal systemic risk}; [Diks, Hommes & Wang 2019, Empir. Econ. 57:1201–1228 [V]](https://doi.org/10.1007/s00181-018-1527-3).
- **Depression.** [van de Leemput et al. 2014, PNAS 111:87–92 [V]](https://doi.org/10.1073/pnas.1312114110).
- **Populations and epidemics.**
  - Experiments: [Drake & Griffen 2010, Nature 467:456–459 [V]](https://doi.org/10.1038/nature09389); [Dai et al. 2012, Science 336:1175–1177 [V]](https://doi.org/10.1126/science.1219805) {abs}; [Carpenter et al. 2011, Science 332:1079–1082 [V]](https://doi.org/10.1126/science.1203672) {abs: high-frequency monitoring gave early detection}.
  - Disease theory and evaluations: [O'Regan & Drake 2013, Theor. Ecol. 6:333–357 [V]](https://doi.org/10.1007/s12080-013-0185-5); [Southall et al. 2021, J. R. Soc. Interface 18:20210555 [V]](https://doi.org/10.1098/rsif.2021.0555) {abs}; [Dablander et al. 2022, Proc. R. Soc. B 289:20211809 [V]](https://doi.org/10.1098/rspb.2021.1809) {title}; [Proverbio et al. 2022, PLoS Comput. Biol. 18:e1009958 [V]](https://doi.org/10.1371/journal.pcbi.1009958); [Brett & Rohani 2020, PLoS Biol. 18:e3000697 [V]](https://doi.org/10.1371/journal.pbio.3000697).
  - Traits as indicators: [Clements & Ozgul 2016, Nat. Commun. 7:10984 [V]](https://doi.org/10.1038/ncomms10984) {abs: trait (body size) plus abundance composite indicators}; [Clements et al. 2017, Nat. Ecol. Evol. 1:0188 [V]](https://doi.org/10.1038/s41559-017-0188).
  - Recovery: [Clements, McCarthy & Blanchard 2019, Nat. Commun. 10:1681 [V]](https://doi.org/10.1038/s41467-019-09684-y) {abs: EWS also precede recovery}.
  - Reviews: [Clements & Ozgul 2018, Ecol. Lett. 21:905–919 [V]](https://doi.org/10.1111/ele.12948); [Ratajczak et al. 2018, Trends Ecol. Evol. 33:513–526 [V]](https://doi.org/10.1016/j.tree.2018.04.013).
- **Optimising indicators on distribution data.** [Proverbio, Skupin & Gonçalves 2023, iScience 26:107156 [V]](https://doi.org/10.1016/j.isci.2023.107156) {title}.

#### Inferences
- **Every test should declare its null in words**, for example "stationary AR(1) with the residuals' lag-1 AC and variance" or "stationary linear Gaussian with the residuals' spectrum" (phase or IAAFT). A rejection should be phrased as "trend unlikely under this null", never as "approaching a bifurcation".
- **False-alarm and detection rates belong to a pair of models,** not to an indicator.
  - The correct output is an ROC (or a single operating point) computed by simulating the fitted null (stationary OU or AR(1), or a red-noise OU) and a fitted alternative (LSN, or a drifting-κ OU), with the record's length, sampling and processing.
  - This is Boettiger & Hastings's procedure and can be implemented with closed-form OU transitions.
- **Multiple indicators on one record are strongly dependent.** Bonferroni or Holm over indicators is conservative. A better route is one composite statistic (or the likelihood ratio) whose null is simulated jointly. No standard was found in the literature.
- **Refusal conditions follow from Ditlevsen & Ditlevsen's window formulas.** If the record is shorter than the window needed to detect the hypothesised change at confidence q, the detector should answer "undecidable", not "no signal".
- **Paleo and irregular records need a different estimator.** Use likelihoods with variable Δt_i (eq. 5 of Boettiger & Hastings handles variable Δt_i natively; OU transitions in closed form), rather than interpolation onto a grid.
- **Confounders to test explicitly.** These are documented in sources above but not enumerated in one place: interpolation inflating AC, trends in sampling resolution, merged instruments.

#### Gaps
- Numerical ROC results of Boettiger & Hastings (their AUCs per indicator) were not extracted.
- Dakos 2012's exact sensitivity ranges are from memory.
- No source was found giving a principled multiple-testing procedure across correlated indicators. Brock & Carpenter-style composites and the Clements & Ozgul composites exist but lack calibrated nulls.
- How aggregation (block averaging) changes AR(1) parameters was not sourced this session. The closed form is easy to derive and should be derived in the implementation's notes.

### 5. Model-Based Detectors Fit Drifting Stochastic Models and Compare Them by Likelihood

#### Takeaway
Model-based detection fits a stochastic differential equation with a time-varying parameter (an OU with drifting κ; Boettiger & Hastings's LSN; Ditlevsen & Ditlevsen's fold normal form with a linear ramp; Kwasniok's nonstationary densities and dynamics). It tests the drifting model against a stationary one by likelihood ratio with a parametric-bootstrap null, and can extrapolate a tipping time.

Its strengths:
- closed-form transition densities for OU;
- exact handling of irregular sampling;
- an explicit model of what is assumed.

Its weakness is misspecification. Ben-Yami et al. show tipping times are not identified from historical records once the ramp shape, noise colour and fingerprint are uncertain. Generalised modelling and locally linear state-space models are intermediate options.

#### Cited Findings
- **The fold normal form with a ramp.** Ditlevsen & Ditlevsen's model {txt}:
  - Eq. (1): dX_t = −(A(X_t − m)² + λ)dt + σ dB_t, with stable point μ = m + √(|λ|/A). The saddle-node signature is the square-root dependence μ − m ∝ √(λ_c − λ), observed for AMOC shutdown in ocean-only and coupled models.
  - Eq. (2): λ(t) = λ₀(1 − Θ(t − t₀)(t − t₀)/τ_r), a linear ramp from t₀ reaching λ_c = 0 at t_c = t₀ + τ_r, "the tipping time".
  - Eq. (3), the OU approximation in a short window: dX_t ≈ −α(λ)(X_t − μ(λ))dt + σ dB_t, with α(λ) = 2√(|λ|A), variance σ²/(2α), lag-1 autocorrelation ρ = exp(−αΔt) at Δt = 1 month.
  - The parameters of (1) follow from the windowed estimates μ̂, σ̂², ρ̂: α = −log ρ/Δt, σ² = 2γ²α (γ² the variance), A = α/(2(μ − m)), λ = −(α/2)²/A.
  - Estimation is by maximum likelihood with closed-form estimators within running windows.
  - Result: tipping "most likely to occur around 2057", 95% interval 2025–2095, under business-as-usual {txt/abs}.
- **The n-tipping waiting time.** In the small-noise limit P(t, λ) = 1 − exp(−t/τ_n(λ)) {txt}. Re-derived here from Kramers's formula for drift −U′ with U′ = A(x − m)² + λ, which is consistent with the garbled PDF text of their expression:

  τ_n(λ) = (π/√(A|λ|)) exp(8|λ|^{3/2}/(3σ²√A)).
- **Its critique.** See section 4 [Ben-Yami et al. 2024] {txt}. Ben-Yami et al. show three failures in synthetic tests of DD23-type estimation {txt; panel descriptions}: a model driven by nonstationary red noise with linear forcing; a decelerating nonlinear forcing; and a model with no bifurcation but an added mean trend under red noise.
- **The LSN-versus-OU likelihood ratio with bootstrap ROC.** See section 4 {txt} [Boettiger & Hastings 2012a]. Boettiger's R package `earlywarning` implements it {mem}.
- **Nonstationary densities and dynamics.** Kwasniok (2013) models the nonstationary density. Kwasniok (2015) forecasts critical transitions by data-driven nonstationary dynamical modelling: SDEs with time-dependent parameters fitted to the record and extrapolated {title; mem} — [Kwasniok 2015, Phys. Rev. E 92:062928 [V]](https://doi.org/10.1103/PhysRevE.92.062928). Langevin models were derived from paleoclimatic records {title} — [Kwasniok & Lohmann 2009, Phys. Rev. E 80:066104 [V]](https://doi.org/10.1103/PhysRevE.80.066104).
- **Generalised modelling.** [Lade & Gross 2012] (section 3.7). Locally linear state-space models with time-varying AR(p)/VAR(1) coefficients and threshold models are also in this family {abs} [Ives & Dakos 2012].
- **DEV.** It estimates the dominant eigenvalue, real or complex, from local linear models {abs} [Grziwotz et al. 2023].

#### Inferences
- **The model-based detector worth building first** is the drifting-κ OU against the stationary OU, with exact transition densities: a Gaussian AR(1) with time-varying φ_i = e^{−κ(t_i)Δt_i}. A parametric family for κ(t) can be:
  - linear in t;
  - linear in √(t_c − t) for a fold;
  - linear in (t_c − t) for transcritical or pitchfork.

  Report the profile likelihood and its interval. When the interval for t_c is unbounded or extends past the declared horizon, report "tipping time not identified" (Ben-Yami). Report n-tipping hazard by Kramers's formula alongside, since τ_n < t_c − t is typical near the end.
- **A red-noise alternative must be in the model set** (OU driven by OU, Morr & Boers), so that "noise colour changed" competes with "κ fell".
- **Misspecification checks:** residual whiteness under the fitted model, constancy of σ² (fluctuation–dissipation consistency between variance and AC), and the ramp shape (linear vs decelerating).

#### Gaps
- The Bayesian versions (MCMC over κ(t) and t_c) were not surveyed in detail this session. tyche's MCMC could carry them.
- No benchmark comparison of the likelihood approach against Kendall's τ on common test sets beyond Boettiger & Hastings 2012a was found.

### 6. Learned Classifiers Recognise Bifurcation Types but Inherit the Limits of Their Training Libraries

#### Takeaway
Since Bury et al. (2021), deep classifiers (CNN–LSTM) are trained on very many simulated time series. These come from randomly generated low-dimensional models driven through local bifurcations, plus null series. The classifiers output probabilities of fold, Hopf, transcritical or none (and, for maps, period-doubling, Neimark–Sacker, fold, transcritical, pitchfork).

They report higher sensitivity and specificity than variance and lag-1 AC on test models and empirical records. Successors:
- EWSNet, catastrophic vs non-catastrophic;
- classifiers for discrete-time bifurcations;
- climate phase transitions;
- R-tipping detection with explainable AI;
- surrogate-trained, system-specific classifiers;
- reservoir computers that extrapolate post-tipping dynamics;
- a 2026 Koopman framework.

Out-of-distribution failure is the central risk. The classifiers know only the bifurcations, noise types, rates and record lengths of their training libraries, and Patel & Ott show ML extrapolation fails when "too much" is required.

#### Cited Findings
- **Bury et al. 2021.** The algorithm "predicts certain qualitative aspects of the new state, and is also more sensitive and generates fewer false positives than generic indicators". It "does not require data from the study system but instead learns from a universe of possible models" {abs} — [Bury et al. 2021, PNAS 118:e2106140118 [V]](https://doi.org/10.1073/pnas.2106140118).
  - Training library, from memory: two-dimensional polynomial models with random coefficients (up to third order), simulated with additive noise through fold, Hopf or transcritical bifurcations, plus null simulations.
  - Classifier: CNN–LSTM.
  - Tests: simulated ecological models, and empirical thermoacoustic, sediment-anoxia and paleoclimate records.
- **Discrete-time bifurcations.** A classifier trained for the five local codimension-one discrete-time bifurcations was tested on discrete-time models used in physiology, economics and ecology {abs; truncated}. Empirical tests included cardiac data (chick-heart aggregates) {mem} — [Bury et al. 2023, Nat. Commun. 14:6331 [V]](https://doi.org/10.1038/s41467-023-42020-z).
- **EWSNet.** A deep network trained on simulated outcomes from simple models with varying nonlinearity detects transitions and classifies "catastrophic (and hard to reverse) or non-catastrophic" {abs} — [Deb et al. 2022, R. Soc. Open Sci. 9:211475 [V]](https://doi.org/10.1098/rsos.211475).
- **Climate phase transitions.** Data-intensive deep learning can be trained on synthetic data for signatures of critical phenomena generic across systems {abs, truncated before results} — [Dylewsky et al. 2023, J. R. Soc. Interface 20:20220562 [V]](https://doi.org/10.1098/rsif.2022.0562).
- **High dimensions.** [Dylewsky, Anand & Bauch 2024, Sci. Rep. 14:18277 [V]](https://doi.org/10.1038/s41598-024-68177-1) {title}.
- **R-tipping.** "Critical-slowing-down-based indicators generally cannot distinguish these cases of noise-induced tipping from no tipping". The deep-learning framework with explainable AI "captures the fingerprints" for early detection of R-tipping {abs/sec} — [Huang, Bathiany, Ashwin & Boers 2024, Nat. Mach. Intell. 6:1556–1565 [V]](https://doi.org/10.1038/s42256-024-00937-0).
- **SDML.** Surrogate data-based machine learning trains classifiers directly on surrogates of past transitions in the system's own records. It outperforms variance and lag-1 AC in sensitivity and specificity on geology, climatology, sociology and cardiology data {txt/abs} — [Ma, Zeng, Zhang & Bury 2025, Commun. Phys. 8 [V]](https://doi.org/10.1038/s42005-025-02172-4) (arXiv:2410.09707 [V]).
- **Uncertainty of deep-learning forecasts.** Limits to ecological forecasting and uncertainty estimation for critical transitions with deep learning {title} — [Lapeyrolerie & Boettiger 2023, Methods Ecol. Evol. 14:785–798 [V]](https://doi.org/10.1111/2041-210X.14013).
- **Reservoir computing.** ML "can be surprisingly effective" at extrapolating non-stationary dynamics past tipping, "but do[es] … fail when 'too much' extrapolation is required" {abs} — [Patel & Ott 2023, Chaos 33:023143 [V]](https://doi.org/10.1063/5.0131787). Parameter-aware reservoir computing predicts transitions and collapse {title} — [Kong, Fan, Grebogi & Lai 2021, Phys. Rev. Research 3:013090 [V]](https://doi.org/10.1103/PhysRevResearch.3.013090); extended in [Köglmayr & Räth 2024, Sci. Rep. 14:1221 [V]](https://doi.org/10.1038/s41598-023-50726-9).
- **Koopman early warnings (preprint).** Residual Koopman mode decomposition, extended to the control setting with time-varying control variables in the observable space, recovers expected signatures near bifurcations. It "improve[s] detection in rate-induced regimes where classical indicators fail" and distinguishes tipping from non-tipping AMOC simulations {txt} — [Nathaniel et al. 2026, arXiv:2608.14716 [V], preprint, not peer-reviewed](https://arxiv.org/abs/2608.14716).
- **Supervised learning on indicator features** ("dynamical footprints") detects disease emergence {title} — [Brett & Rohani 2020] (section 4).

#### Inferences
- **A learned detector must carry its training manifest:** which bifurcations, noise types (additive, multiplicative, red), rates, record lengths, sampling, detrending. It should refuse inputs outside it.
  - Out-of-distribution scores can be computed as the classifier's embedding distance to its training set, or by checking basic statistics (length, κ̂ range, noise colour) against the training ranges.
- **Learned classifiers should not be the first detectors built.**
  - They are trained on exactly the normal forms the infrastructure can simulate.
  - The analytic detectors (spectral fits per type, complex DEV) target the same information with stated assumptions.
  - Their reported superiority rests on test sets drawn from related model families.
- **The R-tipping and Koopman results are the only routes to non-CSD transitions in this survey.** They should be tracked, but treated as research, not as tools to be relied on.

#### Gaps
- The quantitative AUCs of Bury 2021, 2023 and Ma 2025 were not extracted.
- Dylewsky 2023's training data and results were not read (abstract truncated).
- No independent replication of Huang et al. 2024 was found.

### 7. Finite-Time Singularities Are Forecast by Power-Law Acceleration, with Known Statistical Pitfalls

#### Takeaway
A different class of "critical" forecasting fits an accelerating rate that diverges at a finite time t_f:
- Voight's relation Ω̈ = AΩ̇^α for material failure and volcanic eruptions;
- for α = 2, Fukuzono's inverse-velocity method for landslides, 1/Ω̇ linear in time;
- Sornette's log-periodic power laws (LPPLS) for bubbles and crashes.

These are not CSD: they model positive feedback toward a singularity. Their statistical problems are documented:
- least-squares linearisation is inconsistent with the error structure of count data (Bell et al. 2011), giving biased, imprecise forecasts; GLMs do better;
- predictability limits exist;
- LPPLS evidence for log-periodicity is contested (Feigenbaum; Chang & Feigenbaum's Bayesian analysis; Brée & Joseph).

#### Cited Findings
- **Voight's relation.** "The simple relation Ω̈ Ω̇^{−α} − A = 0 … describes the behavior of materials in terminal stages of failure under conditions of approximately constant stress and temperature … and may be used to predict time to failure" {abs} — [Voight 1989, Science 243:200–203 [V]](https://doi.org/10.1126/science.243.4888.200). It was applied to eruptions in [Voight 1988, Nature 332:125–130 [V]](https://doi.org/10.1038/332125a0).
  - Integrating with α > 1 gives Ω̇^{1−α} = Ω̇₀^{1−α} − (α − 1)A(t − t₀). For α = 2 the inverse rate falls linearly and hits zero at t_f (derived here).
- **Fukuzono's inverse-velocity method** for slope failure [Fukuzono 1985, Proc. 4th Int. Conf. Field Workshop Landslides, Tokyo, 145–150 [M]]. The infrastructure's disaster-risk domain plans "warning (Fukuzono's failure forecast)".
- **The Failure Forecast Method (FFM) and its software** — [Cornelius & Voight 1995, J. Volcanol. Geotherm. Res. 64:295–320 [V]](https://doi.org/10.1016/0377-0273(94)00078-U).
- **FFM evaluated.** The FFM's assumptions "are inconsistent with the error structure of the data, leading to biased and imprecise forecasts". A generalised linear model gives better forecasts that converge to the failure time {abs} — [Bell, Naylor, Heap & Main 2011, GRL 38:L15304 [V]](https://doi.org/10.1029/2011GL048155). For Poisson counts of events, the GLM is log-rate Poisson regression on the power-law form (from memory).
- **Predictability limits.** Limits of predictability of eruptions from accelerating earthquake rates {title} — [Bell, Naylor & Main 2013, Geophys. J. Int. 194:1541–1553 [V]](https://doi.org/10.1093/gji/ggt191).
- **Fracturing and reviews.** Multiscale fracturing as the physics behind it {title} — [Kilburn 2003, J. Volcanol. Geotherm. Res. 125:271–289 [V]](https://doi.org/10.1016/S0377-0273(03)00117-3); beyond FFM, [Kilburn 2018, Front. Earth Sci. 6:133 [V]](https://doi.org/10.3389/feart.2018.00133). Landslide time-of-failure methods are reviewed in [Intrieri, Carlà & Gigli 2019, Earth-Sci. Rev. 193:333–349 [V]](https://doi.org/10.1016/j.earscirev.2019.03.019).
- **LPPLS.** Johansen, Ledoit & Sornette model crashes as critical points — [JLS 2000, Int. J. Theor. Appl. Finance 3:219–255 [V]](https://doi.org/10.1142/S0219024900000115).
  - Formula from memory: ln p(t) ≈ A + B(t_c − t)^m + C(t_c − t)^m cos(ω ln(t_c − t) − φ), with 0 < m < 1.
  - Filimonov & Sornette's calibration reduces the nonlinear parameters to (t_c, m, ω), with the linear parameters (A, B, C₁, C₂) solved exactly (from memory) — [Filimonov & Sornette 2013, Physica A 392:3698–3707 [V]](https://doi.org/10.1016/j.physa.2013.04.012).
  - "Dragon-kings" as outliers beyond power laws — [Sornette & Ouillon 2012, Eur. Phys. J. Spec. Top. 205:1–26 [V]](https://doi.org/10.1140/epjst/e2012-01559-5).
- **Critiques of LPPLS.** Feigenbaum's statistical analyses of log-periodic precursors — [Feigenbaum 2001, "More on a statistical analysis…", Quant. Finance 1:527–532 [V]](https://doi.org/10.1080/713665875); the first paper ["A statistical analysis of log-periodic precursors to financial crashes", Quant. Finance 1:346–360 [M]]. A Bayesian analysis of log-periodic precursors — [Chang & Feigenbaum 2006, Quant. Finance 6:15–36 [V]](https://doi.org/10.1080/14697680500511017). Testing for crashes with LPPL — [Brée & Joseph 2013, Int. Rev. Financ. Anal. 30:287–297 [V]](https://doi.org/10.1016/j.irfa.2013.05.005). The content of these critiques is from memory: weak or no evidence of log-periodicity beyond noise, and fragile fits.

#### Inferences
- **A finite-time-singularity detector sees positive-feedback acceleration** (material failure, runaway growth, bubbles). It is blind to fold-type CSD in a stationary-noise regime. It must fit on the event-count likelihood (Poisson or negative-binomial GLM), not by least squares on the inverse rate. It should report t_f with a profile-likelihood interval and refuse when α is not identified.
- **Closed form for checks:** with α = 2 and deterministic rates, the inverse rate is exactly linear and the intercept is t_f. With Poisson thinning, the GLM estimator's coverage should match nominal in simulation.

#### Gaps
- Fukuzono's original paper was not verified (no DOI).
- Bell et al. 2011's GLM specification was not read (abstract only).
- No source verified this session evaluates LPPLS out of sample with calibrated false-alarm rates.

### 8. Software, Licences and Benchmark Models

#### Takeaway
The mature univariate toolkits are:
- ewstools (Python, MIT; rolling indicators, spectral EWS, deep-learning classifiers);
- earlywarnings (R, BSD-2; the Dakos et al. 2012 toolbox, including conditional heteroskedasticity, BDS, potential analysis and sensitivity);
- EWSmethods (R, MIT; uni- and multivariate EWS, trait-based composites, EWSNet);
- spatialwarnings (R, MIT; spatial, for reference only);
- TransitionsInTimeseries.jl (Julia, JOSS 2024).

For continuation and bifurcation analysis: AUTO-07p, MatCont, BifurcationKit.jl and the DynamicalSystems.jl ecosystem (with CriticalTransitions.jl for noise-induced transitions and minimum-action paths). No community benchmark suite exists. Methods are compared on recurring test models:
- May's grazing/harvesting model;
- consumer–resource (Rosenzweig–MacArthur) Hopf;
- the Stommel/Stommel–Cessi box model;
- lake eutrophication (Carpenter);
- SEIR or SIS epidemic models;
- Ricker or other maps for period doubling;
- randomly generated polynomial models (Bury 2021).

#### Cited Findings
- **ewstools.** [Bury 2023, JOSS 8:5038 [V]](https://doi.org/10.21105/joss.05038). PyPI reports version 2.1.3 with the classifier "License :: OSI Approved :: MIT License" {V, PyPI JSON}. Functions, from memory: detrending (Gaussian, LOWESS), rolling variance, AC, skew, kurtosis, CV, Kendall τ, the spectral EWS of Bury 2020, and pretrained classifiers of Bury 2021.
- **earlywarnings (R).** Version 1.1.29, licence "BSD_2_clause + file LICENSE", published 2022-10-10 {V, crandb}. Functions, from memory: generic_ews, ch_ews, bdstest_ews, livpotential_ews, movpotential_ews, sensitivity_ews, surrogates_ews, qda_ews. Associated with [Dakos et al. 2012].
- **EWSmethods (R).** Version 1.3.3, licence "MIT + file LICENSE", 2026-03-30 {V, crandb}; [CRAN record [V]](https://doi.org/10.32614/CRAN.package.EWSmethods). The paper is in Ecography (DOI 10.1111/ecog.06674 inferred from Crossref's review records [S]); preprint [O'Brien et al. 2022, Authorea [V]](https://doi.org/10.22541/au.166801190.00303336/v2).
- **spatialwarnings (R).** Version 3.1.2, "MIT + file LICENSE", 2026-07-23 {V, crandb}; [Génin et al. 2018, Methods Ecol. Evol. 9:2067–2075 [V]](https://doi.org/10.1111/2041-210X.13058).
- **TransitionsInTimeseries.jl.** [Swierczek-Jereczek & Datseris 2024, JOSS 9:6464 [V]](https://doi.org/10.21105/joss.06464); licence MIT [M, not checked: GitHub's API was rate-limited this session].
- **DynamicalSystems.jl.** [Datseris 2018, JOSS 3:598 [V]](https://doi.org/10.21105/joss.00598); MIT [M].
- **CriticalTransitions.jl** (JuliaDynamics): large-deviation tools (minimum action and geometric minimum action methods), noise-induced and rate-induced transitions [M]. Licence MIT [M]. No paper record found via Crossref.
- **BifurcationKit.jl** (R. Veltz; HAL preprint hal-02902346, 2020) [M]; MIT [M].
- **MatCont.** [Dhooge, Govaerts & Kuznetsov 2003, ACM TOMS 29:141–164 [V]](https://doi.org/10.1145/779359.779362). Licence not verified.
- **AUTO.** [Doedel 1981, "AUTO: a program for the automatic bifurcation analysis of autonomous systems", Congr. Numer. 30:265–284 [M]]. AUTO-07p's licence is not verified (reported as BSD-type [M]).
- **kramersmoyal (Python)** [JOSS 2019 [V]] (section 3); licence MIT [M]. statsmodels' GLSAR is used by Boers 2021's estimator {txt via Morr & Boers}.
- **Boettiger's `earlywarning` R package** (GitHub cboettig/earlywarning) implements the likelihood and ROC approach [M].
- **EWSNet** (Deb et al. 2022) is distributed as Python code and wrapped by EWSmethods [M].
- **Test models used in the papers.**
  - Kuehn 2013 uses the Stommel–Cessi box model (detrending comparison), an SIS epidemic on an adaptive network, an activator–inhibitor switch, a predator–prey model and Euler buckling {txt}.
  - DD23 is calibrated against AMOC models {txt}.
  - Bury 2021 uses a random-polynomial library {mem}.
  - Dakos 2012 uses an overharvesting model with simulated ecological data {mem}.
  - Lenton et al. 2012 compare three paleo records and three models of AMOC collapse {abs}.

#### Inferences
- **Licences.** ewstools (MIT), EWSmethods (MIT), spatialwarnings (MIT) and earlywarnings (BSD-2) are compatible with the infrastructure's MIT licence as references, and their test outputs can serve as fixtures.
  - Fixture values should be generated by running these packages in a `make_*fixtures` script, per the project's practice, not copied from memory.
  - GPL tools (if MatCont or others prove GPL) can be used only as external reference runs.
- **Benchmark suite.** Because no benchmark suite exists, the infrastructure's own suite of normal-form simulations with known κ(t) and known transition type would itself be a contribution (section 9).

#### Gaps
- GitHub licences for the Julia packages, AUTO-07p and MatCont could not be checked: the API was rate-limited and no `gh` CLI was available.
- ewstools' current function list was not re-read.
- No published benchmark suite with standard datasets and metrics for univariate EWS was found. Dakos 2024's supplement may hold the closest catalogue.

### 9. What This Means for the Design

#### Takeaway
Build first the detectors whose assumptions are explicit and whose behaviour has closed forms:
1. Direct restoring-rate estimators (AR(1)/OU κ, Boers's GLS under AR(1) noise, Morr–Boers ACS/PSD under red noise, Kramers–Moyal drift slope).
2. A model-based likelihood-ratio test (drifting-κ OU or LSN against stationary OU, and red-noise alternatives) with bootstrap ROC, so that every verdict carries false-alarm and detection rates.
3. Type-aware detectors (spectral fold/Hopf/null fits; the complex dominant eigenvalue for flows and maps, which sees Hopf, period-doubling and Neimark–Sacker).
4. A state-count detector (potential wells, flickering) for proximity to noise-induced switching.
5. A finite-time-singularity detector fitted by GLM.

Extend the existing kinesis module with surrogates beyond AR(1) and with sensitivity grids. Every detector should return what it sees, what it is blind to, its null, its operating characteristics, and a refusal when the record cannot distinguish.

#### Cited Findings
- **Why κ estimators first.** Variance and AC give spurious or masked alarms when noise colour changes {txt} [Morr & Boers 2024]. Variance can fall near transitions {abs} [Dakos et al. 2012 Ecology]. κ estimators address both and generalise to Jacobians [Morr et al. 2024 PRR; Ives & Dakos 2012; Williamson & Lenton 2015, Chaos 25:036407 [V]](https://doi.org/10.1063/1.4908603).
- **Why likelihood and ROC.** Without them, error rates are unknown and "can be quite severe" {abs} [Boettiger & Hastings 2012a]. Selection bias is otherwise unaccounted for {abs} [Boettiger & Hastings 2012b].
- **Why type-aware detectors.** Conventional EWS do not indicate type and fail to tell Hopf from fold {abs} [Bury et al. 2020]. DEV's place in the complex plane indicates type {abs} [Grziwotz et al. 2023]. Lag-1 AC decreases at period-doubling (from the linearisation; section 2 table).
- **Why the explicit blind-spot report.** R- and N-tipping cannot be anticipated by CSD indicators {sec/abs} [Rietkerk et al. 2025; Huang et al. 2024]. R-tipping indicators lag {txt} [Ritchie & Sieber 2016]. Non-smooth and structured systems give silent catastrophes {abs} [Hastings & Wysham 2010; Boerlijst et al. 2013]. Saddle escapes are not bifurcations {abs} [Kuehn et al. 2015]. CSD also precedes non-catastrophic transitions {abs} [Kéfi et al. 2013].
- **Why refusals of time-to-tipping.** Tipping times range over centuries to millennia, with up to a quarter infinite, under reasonable alternatives {txt} [Ben-Yami et al. 2024].

#### Inferences
**9.1 Detectors in order of construction.**
1. **κ̂(t), the restoring rate**, with four estimators sharing one interface:
   - (a) OLS AR(1): κ̂ = −log φ̂/Δt;
   - (b) increment regression with GLSAR (AR(1) noise), as in Boers 2021;
   - (c) ACS and PSD fits to the OU-driven-by-OU model (Morr & Boers 2024);
   - (d) Kramers–Moyal drift slope at the windowed mode.

   Each reports κ̂ with an interval, the noise parameters (σ², noise correlation time) and a consistency check: under white noise and constant σ, the variance must equal σ²/(2κ). It extends to many dimensions as a VAR(1) or Jacobian with its leading eigenvalue.
2. **Trend tests on any indicator series.**
   - Kendall's τ and the slope of κ̂(t).
   - Nulls: AR(1) (existing), phase-randomised, IAAFT, stationary block bootstrap, and a red-noise OU surrogate fitted to the early part.
   - A sensitivity grid over window (for example 25–75%) and detrending bandwidth, reporting the fraction of grid cells significant.
   - One joint null for the set of indicators, to control multiplicity.
3. **Likelihood-ratio detector.**
   - H₀ stationary OU (and OU driven by OU); H₁ drifting κ (linear, or fold-shaped √(t_c − t), or transcritical-shaped) or LSN.
   - Exact transition densities with variable Δt_i (Boettiger & Hastings eqs 8–9).
   - Parametric-bootstrap distributions of the deviance under both models give the ROC, and from it the false-alarm rate at the chosen threshold and the detection rate against the fitted alternative.
   - Optional t_c by profile likelihood with the n-tipping hazard (Kramers) beside it. Refuse t_c when its interval is unbounded (Ben-Yami).
4. **Type classifier from the linear part.**
   - Spectral fits: null/fold Lorentzian vs Hopf double-Lorentzian with AIC weights (Bury 2020).
   - Complex dominant eigenvalue from windowed AR(2)/AR(p) or S-map (DEV). It reports the path toward +1 (fold, transcritical, pitchfork), −1 (period-doubling, for maps or sampled flows), or the unit circle or imaginary axis (Hopf, Neimark–Sacker).
   - The power law that linearises κ̂(t) (κ̂² linear for a fold, κ̂ linear for transcritical).
5. **States and switching.** Potential analysis (number of wells), bimodality and flickering, and the Kramers escape time of the fitted double well. This is the only family that speaks to N-tipping. It reports "proximity to switching", not "approach to a bifurcation".
6. **Finite-time singularity.** Voight/FFM by Poisson/negative-binomial GLM (Bell 2011), α profiled, and t_f with an interval. Fukuzono's inverse velocity is the α = 2 special case. LPPLS only as an optional research tool, with a test of log-periodicity against noise.
7. **Later.** Learned classifiers with a training manifest and out-of-distribution refusal. Generalised modelling. Koopman-based indicators for R-tipping, as research.

**9.2 What each detector reports.** A uniform result object:
- `sees`: for example {fold, transcritical, pitchfork} for variance/AC; plus {Hopf, period-doubling, Neimark–Sacker} for the eigenvalue and spectral detectors; {finite-time blow-up} for FFM.
- `blind`: always including {R-tipping at constant frozen stability, N-tipping at fixed parameters, S-tipping, global bifurcations (SNIC, homoclinic), crises, non-smooth or border-collision transitions, transitions without a slow manifold}. For AC-only detectors also {Hopf with ω₀Δt > π/2, period-doubling}.
- `assumes`: timescale separation (ε small), additive white stationary noise (or the red model), stationary measurement process, regular sampling (or the likelihood with variable Δt).
- `null`: in words.
- `operating point`: false-alarm rate α̂ and detection rate 1 − β̂ against the declared alternative at the record's length, from simulation.
- `verdict` ∈ {signal, no signal, undecidable}.

Undecidable is mandatory when any of the following holds:
- the record is shorter than the window needed to detect the hypothesised change (Ditlevsen & Ditlevsen's window formulas);
- the baseline lag-1 AC is so close to 1 that a window holds few independent points;
- the variance and AC disagree with the fluctuation–dissipation relation (noise changed);
- the red-noise model fits better than white and the κ estimator under red noise is not significant;
- sensitivity-grid results flip sign;
- for learned detectors, the input is outside the training manifest.

**9.3 Closed forms and known cases for verifying implementations** (derived or standard; the derivations belong in the implementation notes):
- **OU** dx = −κx dt + σ dW:
  - Var = σ²/(2κ);
  - ACF(τ) = e^{−κ|τ|};
  - AR(1) coefficient φ = e^{−κΔt}, innovation variance σ²(1 − e^{−2κΔt})/(2κ);
  - spectrum σ²/(2π(κ² + ω²));
  - Kramers–Moyal D⁽¹⁾ = −κx, D⁽²⁾ = σ²/2.
- **Kendall's τ** under independence: mean 0, variance 2(2n + 5)/(9n(n − 1)).
- **Fold normal form** dx = (μ − x²)dt + σ dW (μ > 0): x* = √μ, κ = 2√μ, Var ≈ σ²/(4√μ), so var⁻² is linear in μ. Kuehn's σ²/√y law.
- **Transcritical and pitchfork:** κ = |μ|, Var ≈ σ²/(2|μ|).
- **Hopf normal form** (complex z): κ = −μ, a peak at ω₀; each coordinate's variance ≈ σ²/(2|μ|) for isotropic noise (Kuehn's O(1/y)).
- **Period-doubling map** x_{n+1} = −(1 − δ)x_n + noise: lag-1 AC = −(1 − δ), variance = σ²/(1 − (1 − δ)²).
- **Slow passage through a fold** at sweep rate ε: the jump lies O(ε^{2/3}) past the static fold (Kuehn 2013 scaling region). Detectors fed such a run must not place the transition before the fold except through noise.
- **Kramers time** for drift −U′: τ = 2π/√(|U″(a)|U″(b)) · exp(2ΔU/σ²). The fold-specific form is τ_n(λ) = (π/√(A|λ|)) exp(8|λ|^{3/2}/(3σ²√A)) (DD23's model).
- **Double well** U = x⁴/4 − x²/2: the stationary density ∝ exp(−2U/σ²) has exactly two modes, which potential analysis must report.
- **DFA:** α = 0.5 (white noise), α = 1.5 (random walk).
- **Permutation entropy:** log d! for white noise, 0 for a monotone series.
- **Basin stability:** Monte Carlo standard error √(S_B(1 − S_B)/N).
- **Voight with α = 2:** the inverse rate is exactly linear with root t_f.
- **R-tipping** ẋ = (x + rt)² − 1: critical rate r_c = 1 exactly (derived above). The frozen eigenvalue is constant (−2), so CSD indicators must show no trend for constant r < 1.

**9.4 Test models for the suite of checks.**
- **Positive cases, each with known κ(t) and type:**
  - normal forms of fold, transcritical, pitchfork and Hopf driven slowly through the bifurcation, at several ε, σ, record lengths and sampling steps;
  - period-doubling and Neimark–Sacker in maps (Ricker or logistic near r = 3);
  - May's grazing model and Carpenter's lake model (fold);
  - Rosenzweig–MacArthur (Hopf);
  - the Stommel–Cessi box (fold);
  - an SIS epidemic (transcritical);
  - DD23's ramped fold.
- **Negative cases, where detectors must stay silent at their nominal false-alarm rate:**
  - (i) stationary OU and AR(1);
  - (ii) R-tipping with constant frozen stability (ẋ = (x + rt)² − 1 at constant r < 1, and above r_c, where tipping occurs without a CSD trend);
  - (iii) N-tipping in a fixed double well with escape times comparable to the record;
  - (iv) red noise whose correlation increases with no change of κ (Morr & Boers's spurious case), and decreasing correlation with falling κ (the masked case, which must be detected only by red-noise estimators);
  - (v) increasing measurement-noise variance or a change of instrument (Smith et al. 2023);
  - (vi) a deterministic trend in the mean with no bifurcation under red noise (Ben-Yami's third case);
  - (vii) a non-smooth system whose eigenvalue jumps (border collision) or a structured "silent catastrophe";
  - (viii) a SNIC or homoclinic approach (no local CSD; period lengthening);
  - (ix) a boundary crisis.
- **Cases where variance must not be trusted:** environmental-sensitivity loss (Dakos et al. 2012 Ecology), and a Hopf sampled coarsely so that lag-1 AC falls.
- **Each case declares its expected verdict per detector:** signal, silent, or undecidable.

#### Gaps
- Several formulas above are standard results written from memory and should be re-derived in the implementation's theory notes before they serve as fixtures: the Bury 2020 spectra, Wissel's exponent, the SNIC and homoclinic period scalings, and the crisis lifetimes.
- Fixtures from the reference packages (ewstools, earlywarnings, EWSmethods) were not generated in this session.
- No source gives a validated, record-only detector of R-tipping. The design should present R-tipping as detectable only with a model of the forcing (critical-rate analysis on a declared model, basin instability as in O'Keeffe & Wieczorek 2020), not from the record alone.

## Part II. Structured and High-Dimensional States

Scope: how critical slowing down (CSD) and other signs of transition are defined, estimated and tested when the state of a dynamical system is not a scalar but an element of a structured space (a vector of coupled variables, a field on a grid or a graph, a network that changes, a composition or a distribution, a matrix, a hierarchy of units, a multichannel signal). General theory and univariate indicators are covered in Part I and appear here only where the structured case changes them.

*Remark.* Part II's conventions. The bibliographic markers are those of §0.3 (records were retrieved from Crossref's API, the arXiv API, Semantic Scholar's API by DOI, PubMed Central or the publisher's or package's own page).

- **Content provenance**: "(abstract)" means the statement was read in the abstract retrieved this session; "(full text, PMC)" means the open full text was read; "(content [M])" means the content is from memory of the paper even though the record is verified.
- **Formulas** are marked *[formula M]* when written from memory (standard results or recollection of the paper) and not checked against the source's equation; equation numbers are given only where they were seen.
- **Packages**: licence read this session from GitHub's API or the repository's LICENSE/DESCRIPTION file unless marked otherwise.

### 1. Multivariate Indicators Reduce a Vector State to Its Slowest Direction

#### Takeaway
In a linearised multivariate system near a codimension-one bifurcation, slowing down is concentrated along one eigendirection of the Jacobian; the multivariate indicators (leading eigenvalue of the covariance, leading eigenvalue of the estimated lag-1 propagator, decay rate along leading EOFs, DNB composites, PCA/MAF projections, max or mean of node-wise indicators) differ in whether they find that direction, and all can be silent when the observed variables are orthogonal to it or when the Jacobian is non-normal.

#### Cited Findings

**1.1 The linear theory that every multivariate indicator rests on** *[formula M; standard, e.g. Gardiner's Handbook of Stochastic Methods, Ornstein–Uhlenbeck section]*
- Model: $dx = Jx\,dt + B\,dW$, $x\in\mathbb R^n$, $J$ stable (all $\operatorname{Re}\lambda_i(J)<0$). Stationary covariance $C$ solves the continuous Lyapunov equation $JC + CJ^\top + BB^\top = 0$; lagged covariance $C(\tau)=e^{J\tau}C$ for $\tau\ge 0$. Sampled at step $\Delta$: $x_{t+1}=Ax_t+\varepsilon_t$, $A=e^{J\Delta}$, $C=ACA^\top+Q$ (discrete Lyapunov), and the least-squares/Yule–Walker estimate is $\hat A=\hat C(1)\hat C(0)^{-1}$; its eigenvalues $\mu_i=e^{\lambda_i\Delta}$ approach the unit circle as $\operatorname{Re}\lambda_1\to 0^-$.
- Concentration along the critical direction (derivation, *[formula M]*): if $\lambda_1\to 0^-$ is simple with right and left eigenvectors $v,w$ ($w^\top v=1$), then $C = \dfrac{w^\top BB^\top w}{2|\lambda_1|}\,vv^\top + O(1)$. The **left** eigenvector decides whether noise excites the critical mode at all; the **right** eigenvector decides where in the state it appears. A recent theory paper states exactly this split for networks: "the left eigenvector determines how strongly noise excites the critical fluctuation, while the right eigenvector determines its spatial pattern" — [Masuda 2026, arXiv:2608.15476 [V]](https://arxiv.org/abs/2608.15476) (abstract).
- Symmetric $J$ with isotropic noise $\sigma^2 I$: $C=-\tfrac{\sigma^2}{2}J^{-1}$, so the eigenvalues of $C$ are $\sigma^2/(2|\lambda_i|)$ and the leading eigenvector of $C$ *is* the critical eigenvector *[formula M]*. This is the closed form behind "eigenvalues of the covariance matrix as early warning signals"; the authors "show theoretically why these indicators may increase" and note the method "takes into account only the fluctuations of the system about its equilibrium, thus eliminating the effects of any change in equilibrium values" — [Chen, O'Dea, Drake & Epureanu 2019, Sci. Rep. 9:2572 [V]](https://doi.org/10.1038/s41598-019-38961-5) (abstract).
- Non-normal $J$ breaks the coincidence: the leading eigenvector of $C$ is no longer $v$, and transient amplification can raise variance without loss of stability. Non-normality is widespread in empirical networks — [Asllani, Lambiotte & Carletti 2018, Sci. Adv. 4:eaau9403 [V]](https://doi.org/10.1126/sciadv.aau9403) (record; content [M]).

**1.2 Degenerate fingerprinting (decay rate along the leading directions)**
- Held & Kleinen propose to "measure the smallest decay rate of the system under investigation and to consider its trend", "the diagnostic variable most directly linked to the distance from a bifurcation threshold", demonstrated on the North Atlantic thermohaline circulation in CLIMBER-2 — [Held & Kleinen 2004, GRL 31:L23207 [V]](https://doi.org/10.1029/2004GL020972) (abstract). Procedure *(content [M])*: compute EOFs of the windowed field, project on the leading EOFs, fit a (multivariate) AR(1), report the decay rate $\kappa=-\ln|\mu_{\max}(\hat A)|/\Delta$ or the AR coefficient of the leading projection; precursor: [Kleinen, Held & Petschel-Held 2003, Ocean Dynamics 53:53–63 [V]](https://doi.org/10.1007/s10236-002-0023-6).
- Response theory supplies the derivation: Lucarini & Chekroun "explain the underpinnings of degenerate fingerprinting, which offers early warning indicators for tipping points", by deriving optimal fingerprinting from linear response theory of nonequilibrium systems — [Lucarini & Chekroun 2024, PRL 133:244201 [V]](https://doi.org/10.1103/PhysRevLett.133.244201) (abstract); extension to time-dependent backgrounds (pullback measures) in [Lucarini 2026, arXiv:2602.08022 [V]](https://arxiv.org/abs/2602.08022) (abstract).

**1.3 Detection from multiple series (Williamson & Lenton)**
- They "generalize a method of detecting an approaching bifurcation ... from the special case of one dynamical variable to multiple dynamical variables"; in the multivariable case they "find additional indicators to decay rate, such as frequency of oscillation. In the case of approaching a homoclinic bifurcation, there is no change in decay rate but there is a decrease in frequency of oscillations" — [Williamson & Lenton 2015, Chaos 25:036407 [V]](https://doi.org/10.1063/1.4908603) (abstract). Implication: track the full complex spectrum of $\hat A$ (moduli *and* arguments), not only the leading modulus.
- Spectral variants that classify fold vs Hopf vs others from the power spectrum: [Bury, Bauch & Anand 2020, J. R. Soc. Interface 17:20200482 [V]](https://doi.org/10.1098/rsif.2020.0482) (abstract: conventional EWS "may fail to distinguish the onset of oscillations (e.g. Hopf bifurcation) from a transition to a distant attractor (e.g. Fold bifurcation)").
- Generalised modelling (Jacobian from a parametrised structural model, eigenvalues tracked) — [Lade & Gross 2012, PLoS Comput. Biol. 8:e1002360 [V]](https://doi.org/10.1371/journal.pcbi.1002360) (record; content [M]).
- Recovery rates from perturbations in multivariate living systems — [Ghadami, Gourgou & Epureanu 2018, Sci. Rep. 8:9271 [V]](https://doi.org/10.1038/s41598-018-27573-0) (record).
- A probabilistic VAR as an indicator, with regularisation and uncertainty treatment, improves sensitivity on a multispecies model — [Laitinen & Lahti 2022, arXiv:2205.07576 [V]](https://arxiv.org/abs/2205.07576) (abstract).

**1.4 Dynamical network biomarkers (DNB) and successors**
- Three criteria for a "dominant group" as a bifurcation approaches: the average absolute Pearson correlation within the group ($\mathrm{PCC}_d$) "drastically increase[s]", the average absolute correlation between the group and the rest ($\mathrm{PCC}_o$) "drastically decrease[s]", the group's average standard deviation ($\mathrm{SD}_d$) "drastically increase[s]"; a composite index $I$ (their **Eq. 2**) combines them. Model: discrete-time map with a fixed point whose leading Jacobian eigenvalue reaches modulus 1 at $P_c$, linearised with Gaussian noise; $\mathrm{Var}(y_1)$ diverges while other variances stay bounded; the identification algorithm is in their Supplementary C — [Chen, Liu, Liu, Li & Aihara 2012, Sci. Rep. 2:342 [V]](https://doi.org/10.1038/srep00342) (full text, PMC). The explicit form $I=\mathrm{SD}_d\cdot\mathrm{PCC}_d/\mathrm{PCC}_o$ is *[formula M]* (the equation is an image on PMC; inputs verified).
- Strong-noise variant: "the critical transition is not traditional state-transition but probability distribution-transition when the noise is not sufficiently small"; "making big noise smaller" by a distribution-embedding scheme — [Liu, Chen, Aihara & Chen 2015, Sci. Rep. 5:17501 [V]](https://doi.org/10.1038/srep17501) (abstract).
- Single-sample landscape DNB (l-DNB) for omics, influenza and TCGA tumours — [Liu et al. 2019, Natl Sci. Rev. 6:775–785 [V]](https://doi.org/10.1093/nsr/nwy162) (abstract); single-sample landscape entropy (record found only as a corrigendum, [Bioinformatics 36:2644 [V]](https://doi.org/10.1093/bioinformatics/btz935); original article details [M]).
- Structural weakness *(inference from the derivation)*: DNB assumes the critical eigenvector is supported on a small group of variables (localised); for delocalised critical modes the within/between correlation contrast vanishes.

**1.5 The direction of slowing down and which variables to observe**
- Multivariate fluctuations "reveal network regions with particularly slow dynamics ... the direction of minimal resilience"; the autocorrelation-based method is less robust for short or coarse series but more robust to varying noise than the variance-based method — [Weinans et al. 2019, J. R. Soc. Interface 16:20190629 [V]](https://doi.org/10.1098/rsif.2019.0629) (abstract).
- Which species warn: species sensitivity to disturbance in a community "is affected by the dominant eigenvector" — [Dakos 2018, Ecol. Indic. 94:494–502 [V]](https://doi.org/10.1016/j.ecolind.2017.10.024) (abstract); CSD indicators work for the collapse of mutualistic networks and suggest "a promising way for identifying most vulnerable components" — [Dakos & Bascompte 2014, PNAS 111:17546 [V]](https://doi.org/10.1073/pnas.1406326111) (abstract).
- Silent catastrophes: in "relatively simple ecological models the most commonly used early warning signals for a catastrophic collapse can be silent"; the reason "involves the direction of the eigenvectors of the system" — [Boerlijst, Oudman & de Roos 2013, PLoS ONE 8:e62033 [V]](https://doi.org/10.1371/journal.pone.0062033) (abstract; stage-structured consumer–resource models).
- Edge states pick the observables: fluctuations are "preferentially directed towards the edge state along a most likely path (the instanton)", which "can be exploited to find observables with substantial CSD", demonstrated on AMOC collapse in climate-model simulations — [Lohmann, Hansen, Lovo, Chapman, Bouchet & Lucarini 2024, arXiv:2410.16277 [V]](https://arxiv.org/abs/2410.16277) (abstract); published in Proc. R. Soc. A (record seen only as an author-response DOI 10.1098/rspa.2024.0753 [S]).

**1.6 Aggregating node-wise indicators and reducing dimension before testing**
- A systematic comparison of multivariate indicators was published by [Weinans, Quax, van Nes & van de Leemput 2021, Sci. Rep. 11:9148 [V]](https://doi.org/10.1038/s41598-021-87839-y) (abstract states the aim; its ranking was not retrieved).
- MAF (min/max autocorrelation factors) "looks for the subspace with the highest autocorrelation"; maximum autocorrelation across features, degenerate fingerprinting along PC1 and MAF are the main reductions; multivariate indicators outperformed univariate ones in data-poor settings in a West Nile virus study (PLoS Comput. Biol. 2025, PMC12539738) — [search summary [S]](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC12539738/). The R package EWSmethods' `multiEWS` offers MAF metrics ("eigenMAF", "mafAR", "mafSD") beside PCA — [rdrr.io mirror [S]](https://rdrr.io/cran/EWSmethods/man/multiEWS.html); its full list (mean/max AR and SD across series, PCA AR/SD, covariance eigenvalue, max covariance, mutual information) is *[M]*. MAF formula *[formula M]*: with $\Sigma=\mathrm{Cov}(x)$ and $\Sigma_\Delta=\mathrm{Cov}(x_{t+1}-x_t)$, solve the generalised eigenproblem $\Sigma_\Delta u=\eta\,\Sigma u$; the factor with smallest $\eta$ has lag-1 autocorrelation $1-\eta/2$ (Switzer & Green 1984 [M]).
- Explained variance of PC1 as a resilience indicator, used for mutualistic communities beyond collapse — [Lever et al. 2020, Ecol. Lett. 23:2–15 [V]](https://doi.org/10.1111/ele.13401) (record; content as cited by Halleck Vega et al. 2025, full text PMC).
- In high-dimensional epidemiological model hierarchies CSD was examined for genericity — [Brett et al. 2020, PLoS Comput. Biol. 16:e1007679 [V]](https://doi.org/10.1371/journal.pcbi.1007679) (abstract states aims; results [M]: CSD-based EWS remained informative across the hierarchy).
- Choosing which nodes' signals to combine: a method "to optimize the node set from which to construct an early warning signal", accounting for both magnitude and uncertainty of the signal — [Masuda et al. 2024, Nat. Commun. 15:1086 [V]](https://doi.org/10.1038/s41467-024-45476-9) (abstract); "small subsets of nodes could anticipate transitions as well as or even better than using all the nodes" and "no single best subset" exists for multi-stage transitions — [MacLaren et al. 2023, J. R. Soc. Interface 20:20220743 [V]](https://doi.org/10.1098/rsif.2022.0743) (abstract).

**1.7 Critiques that bear on all multivariate indicators**
- Classes of systems "always show early warning signals, even though they do not feature critical transitions" — [Jäger & Füllsack 2019, PLoS ONE 14:e0211072 [V]](https://doi.org/10.1371/journal.pone.0211072) (search summary of content [S]).
- Selection bias (prosecutor's fallacy) and limits to detection — [Boettiger & Hastings 2012, Proc. R. Soc. B 279:4734 [V]](https://doi.org/10.1098/rspb.2012.2085); [Boettiger & Hastings 2012, J. R. Soc. Interface 9:2527 [V]](https://doi.org/10.1098/rsif.2012.0125) (records; content [M]).
- On multi-trophic plankton data from many lakes, "few instances of critical transitions" were found and different trophic levels expressed different forms of abrupt change — [O'Brien et al. 2023, Nat. Commun. 14:7942 [V]](https://doi.org/10.1038/s41467-023-43744-8) (abstract).
- Review of detection across climate, ecological and human systems — [Dakos et al. 2024, Earth Syst. Dynam. 15:1117 [V]](https://doi.org/10.5194/esd-15-1117-2024).

#### Inferences
- Every multivariate indicator is an estimator of one of three objects of the linearised problem: the spectrum of $A=e^{J\Delta}$ (decay rates and frequencies), the dominant eigenpair of $C$ (variance concentration), or a projection of $x$ onto an estimate of $v$ (PCA, MAF, DNB group, best-indicator species). A detector should declare which object it estimates, because the three disagree exactly when $J$ is non-normal or noise is anisotropic.
- Blindness is structural and can be stated before data: a projection $p^\top x$ shows divergent variance only if $p^\top v\ne 0$ and $w^\top BB^\top w>0$; an indicator built on a fixed set of observed coordinates is blind to critical modes orthogonal to them (Boerlijst's silent catastrophes are the special case where the observed variable lies near $v^\perp$).
- $\hat A=\hat C(1)\hat C(0)^{-1}$ needs window length $T\gg n$ and is ill-conditioned when $\hat C(0)$ becomes nearly singular, which happens precisely near the transition (variance concentrates on one direction). Shrinkage or reduced-rank estimation is required; a detector must refuse when the effective sample size per parameter is too small.
- Taking the max of node-wise indicators inflates false alarms by multiple testing (n correlated tests per window); the mean dilutes localised signals. Both need surrogates that preserve cross-correlation (multivariate phase randomisation or fitted VAR(1) surrogates), not independent per-series surrogates.

#### Gaps
- No primary equation numbers were retrieved for degenerate fingerprinting, MAF or the DNB index; formulas above are standard or from memory.
- Weinans et al. 2021's comparative ranking, and Brett et al. 2020's quantitative results, were not read.
- No study found that reports calibrated false-alarm and detection rates for multivariate indicators against multivariate (cross-correlated) null models in a common benchmark.

### 2. Spatial Indicators Read Slowing Down from Snapshots, and Fail Where Patterns Form, Coupling Is Strong or Units Are Aggregated

#### Takeaway
On regular lattices, spatial variance, spatial skewness, short-range spatial correlation (Moran's I at lag 1), low-wavenumber spectral power and patch-size distributions rise or deform before homogeneous fold transitions; but they confound static heterogeneity with fluctuations, vanish under strong spatial coupling, change meaning near Turing (patterning) instabilities, and behave differently on irregular networks, where only recent theory (2025–2026) explains when spatial variance diverges.

#### Cited Findings

**2.1 Definitions** *[formulas M; standard forms as used in the cited papers]*
- Spatial variance $\sigma_s^2=\frac1n\sum_i(x_i-\bar x)^2$ and spatial skewness $\gamma_s=\frac{1}{n\sigma_s^3}\sum_i(x_i-\bar x)^3$ as leading indicators — [Guttal & Jayaprakash 2009, Theor. Ecol. 2:3–12 [V]](https://doi.org/10.1007/s12080-008-0033-1) (record; content [M]).
- Spatial correlation between neighbouring cells, Moran's $I=\dfrac{n}{W}\dfrac{\sum_{ij}w_{ij}(x_i-\bar x)(x_j-\bar x)}{\sum_i(x_i-\bar x)^2}$, $W=\sum_{ij}w_{ij}$: "increased spatial correlation may serve as a more powerful early-warning signal in systems consisting of many coupled units", rising "well before the transition" — [Dakos, van Nes, Donangelo, Fort & Scheffer 2010, Theor. Ecol. 3:163–174 [V]](https://doi.org/10.1007/s12080-009-0060-6) (abstract).
- Spatial spectra: the DFT "rescales system dynamics into spatial frequencies where early warnings may be sharpened", and "provides information about the spatial frequencies where destabilization first begins" — [Carpenter & Brock 2010, Ecosphere 1:art10 [V]](https://doi.org/10.1890/ES10-00016.1) (abstract). The r-spectrum (radially averaged power) is among the methods summarised, with null models and a step-by-step methodology and code, in [Kéfi et al. 2014, PLoS ONE 9:e92097 [V]](https://doi.org/10.1371/journal.pone.0092097) (abstract; the r-spectrum detail [M]).
- Coarse-graining of binary (presence/absence) snapshots before computing variance and skewness — Kéfi et al. 2014 and spatialwarnings' generic indicators *(content [M])*.
- Thresholds, not only warnings: spatial variance and autocorrelation computed along driver gradients estimate the critical threshold — [Majumder, Tamma, Ramaswamy & Guttal 2019, Ecology 100:e02722 [V]](https://doi.org/10.1002/ecy.2722) (abstract).
- Recovery in space: spatially extended yeast populations with a fold bifurcation, warning signals from spatio-temporal fluctuations and a novel spatial indicator — [Dai, Korolev & Gore 2013, Nature 496:355 [V]](https://doi.org/10.1038/nature12071) (abstract; the indicator's name, "recovery length", is [M]).
- Practical framework for remote sensing — [Nijp et al. 2019, Glob. Change Biol. 25:1905 [V]](https://doi.org/10.1111/gcb.14591) (abstract).

**2.2 Patch-size distributions and their failure**
- Power-law vegetation patch sizes in Mediterranean drylands deviate as grazing increases, proposed as a warning of desertification — [Kéfi et al. 2007, Nature 449:213 [V]](https://doi.org/10.1038/nature06111) (record; content [M]); "meltdown" of the power law before extinction — [Kéfi et al. 2011, Ecol. Lett. 14:29 [V]](https://doi.org/10.1111/j.1461-0248.2010.01553.x) (record; content [M]).
- Doubt from field data — Maestre & Escudero 2009, Ecology 90:1729 [M] (only their 2010 *Reply* record was retrieved, [doi:10.1890/10-0391 [V]](https://doi.org/10.1890/10-0391)).
- Cluster-size distributions vs CSD: "the empirical evidence in support for this hypothesis has been mixed"; local positive feedback drives clustering and also affects CSD — [Sankaran, Majumder, Viswanathan & Guttal 2019, Methods Ecol. Evol. 10:2079 [V]](https://doi.org/10.1111/2041-210x.13304) (abstract).
- Spatially heterogeneous stressors (grazing whose strength depends on local patchiness) alter equilibria and the robustness of early warnings — [Génin et al. 2018, Ecol. Indic. 94:520 [V]](https://doi.org/10.1016/j.ecolind.2017.10.071) (abstract).
- Global drylands: plant spatial patterns identify alternative multifunctionality states — [Berdugo et al. 2017, Nat. Ecol. Evol. 1:0003 [V]](https://doi.org/10.1038/s41559-016-0003) (record; content [M]).
- Rising variability without slowing down in a stochastically driven dryland transition — [Chen, Jayaprakash, Yu & Guttal 2018, Am. Nat. 191:E1 [V]](https://doi.org/10.1086/694821) (title; content beyond title [M]).
- Reference implementation: spatialwarnings (R; spatial variance, skewness, Moran's I, r-spectrum, patch-size fits, null models) — [Génin et al. 2018, Methods Ecol. Evol. 9:2067 [V]](https://doi.org/10.1111/2041-210X.13058); licence **MIT** ([DESCRIPTION file [V]](https://github.com/spatial-ews/spatialwarnings)).

**2.3 Turing (patterning) instabilities change what indicators mean**
- In spatially patterned ecosystems slowing down still occurs at the brink of collapse, but pattern geometry changes the indicators — [Dakos, Kéfi, Rietkerk, van Nes & Scheffer 2011, Am. Nat. 177:E153 [V]](https://doi.org/10.1086/659945) (record; content [M]).
- Pattern formation can let systems *evade* tipping: "Many of these complex systems may be more resilient than currently thought because of overlooked spatial dynamics and multiple stable states" — [Rietkerk et al. 2021, Science 374:eabj0359 [V]](https://doi.org/10.1126/science.abj0359) (abstract); patterned ecosystems respond to change "beyond Turing" through multistability of patterns — [Siteur et al. 2014, Ecol. Complex. 20:81 [V]](https://doi.org/10.1016/j.ecocom.2014.09.002) (record; content [M]); fragmented/partial tipping in heterogeneous space — Bastiaansen, Dijkstra & von der Heydt 2022, Environ. Res. Lett. 17:045006 [M] (only an EGU abstract "Partial tipping in a spatially heterogeneous world" was retrieved, [doi:10.5194/egusphere-egu22-2198 [V]](https://doi.org/10.5194/egusphere-egu22-2198)).
- Distinguishing tipping from Turing destabilisation: estimate the **dispersion relation** from noisy spatio-temporal data, "which reveals whether the system is approaching a spatially homogeneous (tipping) or spatially heterogeneous (Turing patterning) bifurcation"; validated on a modified Klausmeier model with data requirements stated — [Sanders & Bastiaansen 2025, arXiv:2510.01959 [V]](https://arxiv.org/abs/2510.01959) (abstract). Formula basis *[formula M]*: for a linear field $\partial_t\hat u_k=\sigma(k)\hat u_k+\text{noise}$, the lag-$\tau$ autocorrelation of each Fourier coefficient is $e^{\sigma(k)\tau}$ and its variance $\propto 1/(2|\sigma(k)|)$; the maximiser $k^*$ of $\sigma(k)$ distinguishes $k^*=0$ (homogeneous fold) from $k^*>0$ (Turing).

**2.4 Coupling strength and forcing rate**
- In a spatially coupled fold model, "in the weak coupling limit ... spatial EWIs give reliable warning of the bifurcation, even if the external forcing is changing rapidly. However, in the limit of strong spatial coupling, spatial early warning disappears"; temporal indicators remain reliable unless forcing is fast relative to the system's timescale — [Clarke, Huntingford, Ritchie & Cox 2026, J. Phys. Complex. 7:025007 [V]](https://doi.org/10.1088/2632-072X/ae6217) (abstract via NORA).

**2.5 Irregular graphs**
- Six spatial EWSs on networks: "the winning EWS depends on tipping scenarios, although the coefficient of variation and spatial skewness tend to outperform"; spatial EWSs "behave in a drastically different manner between the square lattice and complex networks and tend to be more reliable for the latter" — [MacLaren, Aihara & Masuda 2025, J. R. Soc. Interface 22:20240696 [V]](https://doi.org/10.1098/rsif.2024.0696) (abstract).
- Theory: "the expected spatial variance ... decomposes exactly into a structural contribution from heterogeneity in the equilibrium state and a fluctuation contribution determined by the stationary covariance"; spatial variance diverges only "when the limiting critical eigendirection is noise-excited and spatially nonuniform after centering"; "the spatial coefficient of variation generally saturates, while skewness, kurtosis, and Moran's I approach network-dependent limits without a universal warning direction"; Hopf bifurcations give qualitatively different limiting distributions — [Masuda 2026, arXiv:2608.15476 [V]](https://arxiv.org/abs/2608.15476) (abstract).
- Baseline referencing: compare each node with its own baseline far from tipping before computing the spatial statistic; it "markedly improves variance-based spatial signals", outperforms single-node temporal variance and retains performance "even when up to 80% of nodes are omitted" — [Bandara, Yu & Masuda 2026, arXiv:2608.06608 [V]](https://arxiv.org/abs/2608.06608) (abstract).
- Graph signal processing supplies the generalisation of spatial spectra: the graph Fourier transform in the eigenbasis of the Laplacian $L=D-W=U\Lambda U^\top$, $\hat x=U^\top x$, with eigenvalues as graph frequencies — [Ortega, Frossard, Kovačević, Moura & Vandergheynst 2018, Proc. IEEE 106:808 [V]](https://doi.org/10.1109/JPROC.2018.2820126) (record; formulas [M]). PyGSP (BSD-3-Clause [V]) and PySAL's esda (Moran's I on arbitrary weights, BSD-3-Clause [V]) are reference implementations.
- Interaction networks built from correlations of a field as indicators: changes in degree and clustering-coefficient distributions of the correlation network of the Atlantic temperature field before MOC collapse — [van der Mheen et al. 2013, GRL 40:2714 [V]](https://doi.org/10.1002/grl.50515) (abstract); the same for vegetation transitions — [Tirabassi et al. 2014, Ecol. Complex. 19:148 [V]](https://doi.org/10.1016/j.ecocom.2014.06.004) (record); deep-ocean EWS of MOC collapse — [Feng, Viebahn & Dijkstra 2014, GRL 41:6009 [V]](https://doi.org/10.1002/2014GL061019) (record). Reference implementation: pyunicorn (functional climate networks, recurrence networks) — [Donges et al. 2015, Chaos 25:113101 [V]](https://doi.org/10.1063/1.4934554); licence BSD 3-clause text ([LICENSE.txt [V]](https://github.com/pik-copan/pyunicorn)).

**2.6 Scale dependence and aggregation of spatial units**
- French unemployment, 96 départements, 1982–2019: variance and lag-1 autocorrelation of the *aggregate* show "no clear signal before recessions"; a moving-window PCA over départements (explained variance of PC1) and Moran's I on first-order contiguity, with a warning rule (Moran's I above trailing 3-year mean + 1 SD, Kendall τ > 0.9, p < 0.05), precede the 2008 crisis; sensitivities: small windows raise false positives, 2 SD misses 2000 and 2008, τ threshold 0.8 adds false positives; limitations: an upward trend in Moran's I, no proof of tipping points, one country — [Halleck Vega, Patuelli, van Voorn & Weinans 2025, iScience 28:114097 [V]](https://doi.org/10.1016/j.isci.2025.114097) (full text, PMC).
- Temporal aggregation: standard deviation robust to aggregation up to Δt = 10 (slowing down) and 5 (flickering), autocorrelation only up to Δt = 2 — [Frossard et al. 2015, Front. Ecol. Evol. 3:112 [V]](https://doi.org/10.3389/fevo.2015.00112) (search summary [S] of content).
- A 2026 EGU abstract on boreal forests reports EWS trends prominent at forest-community scales, "a potential scale dependence of the operating tipping archetype" — [EGU26-18335 [S]](https://meetingorganizer.copernicus.org/EGU26/EGU26-18335.html).
- Satellite-based vegetation resilience as lag-1 autocorrelation per pixel, mapped globally — [Smith, Traxl & Boers 2022, Nat. Clim. Change 12:477 [V]](https://doi.org/10.1038/s41558-022-01352-2); Amazon resilience loss — Boulton, Lenton & Boers 2022, Nat. Clim. Change 12:271 [M] (only the preprint record [doi:10.21203/rs.3.rs-379902/v1 [V]](https://doi.org/10.21203/rs.3.rs-379902/v1)); AMOC from fingerprints of multiple indices — [Boers 2021, Nat. Clim. Change 11:680 [V]](https://doi.org/10.1038/s41558-021-01097-4) (records; content [M]).

#### Inferences
- The graph Ornstein–Uhlenbeck model makes the blindness of spatial variance exact *[formula M; derivation]*: $dx=-(\gamma I+\kappa L)x\,dt+\sigma dW$ decouples in Laplacian modes with $\mathrm{Var}(\hat x_k)=\sigma^2/(2(\gamma+\kappa\lambda_k))$. As $\gamma\to 0$ only the $\lambda_0=0$ mode (constant on a connected graph) diverges; spatial variance subtracts the mean and so removes exactly that mode, staying bounded; the **mean** and temporal variance of the mean diverge instead. This reproduces Clarke et al.'s "strong coupling kills spatial warning" (large $\kappa$ separates modes) and Masuda's "nonuniform after centering" condition, and is a closed form for tests.
- Heterogeneous equilibria add a structural term to spatial variance that can rise or fall with the driver for reasons unrelated to stability; baseline referencing (node-wise subtraction of a far-from-tipping baseline) is the structural correction, but it requires a reference period, which a detector must ask for.
- Spatial indicators are statistics of a snapshot; their null distribution must preserve the spatial covariance of a stable state (e.g., surrogate fields with the same spatial spectrum or a fitted CAR/SAR model), otherwise Moran's I trends are confounded with smooth drift, as the French case shows.
- For administrative units, the neighbourhood matrix $W$ (contiguity, distance band, flows) is a modelling choice that changes Moran's I; detectors should take $W$ as an explicit input and report sensitivity to it.

#### Gaps
- No primary equation numbers for the spatial indicators were retrieved.
- I found no study quantifying the modifiable areal unit problem (zoning as well as scale) for spatial EWS beyond case studies; Halleck Vega et al. use one zoning.
- Calibrated false-alarm/detection rates of spatial indicators on irregular graphs exist only in simulation (MacLaren et al. 2025; Bandara et al. 2026).

### 3. Networked Dynamics Are Reduced to Effective Coordinates, Monitored Node by Node, or Watched for Structural Change

#### Takeaway
Three families coexist: (i) reductions of $\dot x_i=F(x_i)+\sum_j A_{ij}G(x_i,x_j)$ to one or a few effective coordinates whose bifurcations predict collective tipping (Gao–Barzel–Barabási, spectral/DART reductions), accurate only when node states are homogeneous; (ii) CSD read from nodes, with node selection mattering and non-bifurcative saddle-escape transitions a distinct class; (iii) change detection for sequences of graphs, which sees structural change but not dynamical proximity to a bifurcation. Percolation and cascade transitions need structural indicators (spectral radius, k-core), not CSD.

#### Cited Findings

**3.1 Reduction to effective one-dimensional dynamics**
- Gao, Barzel & Barabási: "Universal resilience patterns in complex networks" — [Nature 530:307 (2016) [V]](https://doi.org/10.1038/nature16948) (record; abstract not retrieved). Formulas *[formula M]*: operator $\mathcal L(z)=\mathbf 1^\top Az/\mathbf 1^\top A\mathbf 1$; $x_{\rm eff}=\mathcal L(x)$, $\beta_{\rm eff}=\mathcal L(s^{\rm in})=\langle s^{\rm out}s^{\rm in}\rangle/\langle s\rangle$; reduced law $\dot x_{\rm eff}=F(x_{\rm eff})+\beta_{\rm eff}G(x_{\rm eff},x_{\rm eff})$ whose fold in $\beta_{\rm eff}$ is the network's tipping point ("resilience function").
- Critique: Gao et al.'s conditions (negligible degree correlations, uniform node activities, positive interactions) "are neither sufficient nor necessary" — [Tu, Grilli, Schuessler & Suweis 2017, PRE 95:062307 [V]](https://doi.org/10.1103/PhysRevE.95.062307) (abstract); accuracy "hinges on the spread of the equilibrium value of the state variable across the nodes ... accurate when the dispersion of the node's state is small" — [Kundu, Kori & Masuda 2022, PRE 105:024305 [V]](https://doi.org/10.1103/PhysRevE.105.024305) (abstract).
- Two-dimensional reduction for bipartite mutualistic networks, applied to many empirical networks to predict tipping — [Jiang et al. 2018, PNAS 115:E639 [V]](https://doi.org/10.1073/pnas.1714958115) (abstract).
- Spectral reduction on dominant eigenvalues and eigenvectors of $A$, predicting multiple activation of modular networks and critical points for arbitrary degree distributions — [Laurence, Doyon, Dubé & Desrosiers 2019, PRX 9:011042 [V]](https://doi.org/10.1103/PhysRevX.9.011042) (abstract). Form *[formula M]*: observable $R=a^\top x$ with $a$ the (normalised) dominant eigenvector, reduced dynamics evaluated at $\alpha=\lambda_D$.
- DART (multi-dimensional reductions that preserve topological and dynamical features; synchronisation) — [Thibeault, St-Onge, Dubé & Desrosiers 2020, PRR 2:043215 [V]](https://doi.org/10.1103/PhysRevResearch.2.043215) (abstract); an S+1-dimensional manifold for S effective control parameters — [Tu, D'Odorico & Suweis 2021, iScience 24:101912 [V]](https://doi.org/10.1016/j.isci.2020.101912) (abstract); higher-order (hypergraph) reductions predicting tipping boundaries and hysteresis — [Qin, Moore, Zhang & Yan 2026, arXiv:2609.04692 [V]](https://arxiv.org/abs/2609.04692) (abstract).
- Oscillator networks: Ott–Antonsen reduction gives exact low-dimensional order-parameter dynamics for globally coupled phase oscillators — [Ott & Antonsen 2008, Chaos 18:037113 [V]](https://doi.org/10.1063/1.2930766); Kuramoto onset — [Strogatz 2000, Physica D 143:1 [V]](https://doi.org/10.1016/S0167-2789(00)00094-4); explosive transitions via a universal route — [Kuehn & Bick 2021, Sci. Adv. 7:eabe3824 [V]](https://doi.org/10.1126/sciadv.abe3824) (records; content [M]).
- k-core as predictor of structural collapse in mutualistic ecosystems — [Morone, Del Ferraro & Makse 2019, Nat. Phys. 15:95 [V]](https://doi.org/10.1038/s41567-018-0304-8) (record; content [M]); sudden collapse of pollinator communities — [Lever, van Nes, Scheffer & Bascompte 2014, Ecol. Lett. 17:350 [V]](https://doi.org/10.1111/ele.12236).

**3.2 Early warnings from nodes, and transitions that are not bifurcations**
- Saddle-escape transitions: in high-dimensional systems "we frequently encounter an additional non-bifurcative saddle-type mechanism leading to critical transitions ... missed in the search for early-warnings up to now"; near a saddle a system moves slowly and may be perceived as stable (adaptive networks) — [Kuehn, Zschaler & Gross 2015, Sci. Rep. 5:13190 [V]](https://doi.org/10.1038/srep13190) (abstract).
- Multi-stage transitions on heterogeneous networks (some nodes shift earlier); knowledge of the ongoing transition and the network enables effective EWS — [MacLaren et al. 2023 [V]](https://doi.org/10.1098/rsif.2022.0743) (abstract).
- Social–ecological networks: warnings need not appear in every node — [Suweis & D'Odorico 2014, preprint doi:10.1101/003269 [V]](https://doi.org/10.1101/003269) (record; published in PLoS ONE 9:e101851 [M]; content [M]).
- Accelerating cascades of coupled tipping elements: extrapolation of CSD breaks down for "downstream-within-upstream" tipping; skill assessed by ROC over ensembles — [Ashwin, Bastiaansen, von der Heydt & Ritchie 2025, arXiv:2506.01981 [V]](https://arxiv.org/abs/2506.01981) (abstract); interacting climate tipping elements — [Wunderling et al. 2021 (ESD discussion record) [V]](https://doi.org/10.5194/esd-2020-18).

**3.3 Structural change in time-varying networks**
- Change points in the large-scale structure of evolving networks (generative-model based; Bayesian) — [Peel & Clauset 2015, AAAI 29 [V]](https://doi.org/10.1609/aaai.v29i1.9574) (record; content [M]).
- Optimal change-point detection and localisation in sparse dynamic networks (minimax rates) — [Wang, Yu & Rinaldo 2021, Ann. Stat. 49:203 [V]](https://doi.org/10.1214/20-AOS1953) (record; content [M]).
- Graph-based scan statistics for change points in any sample space with a similarity measure, with analytic significance approximations; power better than alternatives in moderate-to-high dimension — [Chen & Zhang 2015, Ann. Stat. 43:139 [V]](https://doi.org/10.1214/14-AOS1269) (abstract). This applies directly to sequences of graphs, SPD matrices or distributions once a metric is chosen.
- Reference implementations: ruptures (BSD-2-Clause [V]) for change points in vector series; netrd (network distances between graphs, MIT [V]).

**3.4 Percolation and cascade transitions**
- Explosive percolation — [Achlioptas, D'Souza & Spencer 2009, Science 323:1453 [V]](https://doi.org/10.1126/science.1167782); global cascades on random networks — [Watts 2002, PNAS 99:5766 (journal details [M]; record retrieved as a 2011 book reprint [V])](https://doi.org/10.1515/9781400841356.497); catastrophic cascades in interdependent networks — [Buldyrev et al. 2010, Nature 464:1025 [V]](https://doi.org/10.1038/nature08932) (records; content [M]).
- Structural thresholds *[formula M]*: Molloy–Reed $\langle k^2\rangle/\langle k\rangle=2$; bond percolation threshold $\approx 1/\lambda_{\max}(B)$ for the non-backtracking matrix $B$; Watts's cascade condition on the degree distribution of vulnerable nodes. These are distances to a structural threshold, computable from the graph without dynamics.

**3.5 Learned network predictors** (see also §8)
- A deep-learning framework (graph isomorphism network layers + GRU) predicting "the specific location where critical transitions happen in networked systems with size up to hundreds of nodes" — [Liu, Zhang, Ru, Gao, Moore & Yan 2024, PRX 14:031009 [V]](https://doi.org/10.1103/PhysRevX.14.031009) (abstract); the accompanying Viewpoint says it uses 20 time points per node, was tested on coupled oscillators and Central African vegetation, and that transfer across network sizes is future work — [Masuda 2024, Physics 17:110 [V]](https://physics.aps.org/articles/v17/110) (page read).

#### Inferences
- A reduction is a **choice of observable** $R=a^\top x$ plus a closure; as an indicator, $R$ inherits the blindness of §1: it warns only if $a^\top v\neq 0$. For heterogeneous equilibria (large dispersion of $x_i^*$) the choice $a$ = dominant eigenvector of $A$ differs from $v$ (an eigenvector of the Jacobian $J=\mathrm{diag}(F'+\partial_1G\cdot s)+A\circ\partial_2G$), which is where the critiques bite.
- Saddle-escape and percolation transitions have no vanishing eigenvalue at a fixed point before the event; detectors based on CSD must declare themselves blind to them, and a structural detector (spectral radius, k-core, distance to a percolation threshold) is the right complement.
- For a sequence of graphs $G_t$, two different questions must be kept apart: (a) has the structure changed (change-point detection in graph space), and (b) is the dynamics on the current structure approaching a bifurcation (reduction or CSD). A structural change can move the system toward or away from tipping; a detector should report both and refuse to conflate them.

#### Gaps
- Gao et al.'s equations were written from memory; their equation numbers were not checked.
- I found no benchmark of graph change-point methods used as tipping warnings with stated false-alarm rates.
- Early warnings for adaptive networks beyond saddle escape (co-evolving topology and state) were not surveyed in depth.

### 4. Compositions and Distributions Need Indicators in Their Own Geometry, and the Literature Barely Supplies Them

#### Takeaway
Trait- and distribution-based warnings exist (body size, distribution embeddings, compositional disorder), and structured (age/stage) populations are a known case where abundance-based warnings fail or are masked; but I found no work that defines CSD indicators in Aitchison geometry or in Wasserstein space for distribution-valued states. Both constructions are straightforward (ilr coordinates; quantile functions for one-dimensional distributions) and are a gap the design can fill with clear closed-form tests.

#### Cited Findings

**4.1 Compositional geometry** *[formulas M; standard Aitchison geometry]*
- Isometric log-ratio (ilr) coordinates — [Egozcue, Pawlowsky-Glahn, Mateu-Figueras & Barceló-Vidal 2003, Math. Geol. 35:279 [V]](https://doi.org/10.1023/A:1023818214614) (record). For $x\in\mathcal S^D$: $\operatorname{clr}(x)=\ln(x/g(x))$ with $g$ the geometric mean; Aitchison inner product $\langle x,y\rangle_A=\langle\operatorname{clr}x,\operatorname{clr}y\rangle$; $\operatorname{ilr}(x)=V^\top\operatorname{clr}(x)$ with $V\in\mathbb R^{D\times(D-1)}$ orthonormal, $V^\top\mathbf 1=0$. Covariances in two ilr bases differ by an orthogonal rotation, so their eigenvalues, trace (Aitchison's total variation) and the leading eigen-*composition* are basis-independent.
- An EWS from compositional data: a diagnostic in "the coefficients of a correlation between compositional disorder and biodiversity", robust to unequal time increments, motivated by canary → keystone → weedy replacement — [Doncaster et al. 2016, Ecology 97:3079 [V]](https://doi.org/10.1002/ecy.1558) (abstract).
- Recent and possibly unreviewed work combining compositional data, robust Mahalanobis distance and perturbation analysis for tipping in river chemistry (University of Florence preprints, STOTEN 2024 and an ESR 2026 manuscript) — [search summary [S]](https://flore.unifi.it/retrieve/9f974e06-f3cd-4f48-a5db-d1965569db7d/STOTEN_2024.pdf).
- A search of arXiv for "critical slowing down" with "compositional" returned nothing relevant (search run this session).

**4.2 Trait-based and distributional early warnings**
- "Including information on the dynamics of phenotypic traits such as body size into composite early warning indices can produce more accurate inferences" (microcosm experiment) — [Clements & Ozgul 2016, Nat. Commun. 7:10984 [V]](https://doi.org/10.1038/ncomms10984) (abstract).
- Whale stocks: mean body size of caught whales declined by up to 4 m over 70 years, with warnings "detectable up to 40 years before the global collapse"; combining abundance and size reduces false signals — [Clements, Blanchard, Nash, Hindell & Ozgul 2017, Nat. Ecol. Evol. 1:0188 [V]](https://doi.org/10.1038/s41559-017-0188) (abstract).
- When trait shifts precede declines — Baruah, Clements, Guillaume & Ozgul 2019, Am. Nat. 193:633 [M] (preprint record [doi:10.1101/424671 [V]](https://doi.org/10.1101/424671)).
- Distribution data (single-cell, ensembles): systematic analysis and an optimised combination of dynamical EWS from distribution data, explaining discrepancies between simulated and real data — [Proverbio, Skupin & Gonçalves 2023, iScience 26:107156 [V]](https://doi.org/10.1016/j.isci.2023.107156) (abstract).
- DNB with distribution embedding (§1.4) treats the transition as a "probability distribution-transition" under strong noise — [Liu et al. 2015 [V]](https://doi.org/10.1038/srep17501).

**4.3 Distances between distributions as indicators**
- Wasserstein Stability Analysis detects changes in probability density functions of climate variables (warming slowdown) where mean trends show nothing; not framed as a tipping warning — [Xie, Chen & Li 2023, arXiv:2303.18067 [V]](https://arxiv.org/abs/2303.18067) (abstract).
- Empirical distributions "mapped to Wasserstein/quantile coordinates, and analyzed by Hankel dynamic mode decomposition", with alarms reported "jointly with false-alarm rate and uncertainty intervals" (AUROC ≈ 0.93 for a machine-learning phase transition, grokking) — [Wang, Ying & Kanamori 2026, arXiv:2605.08237 [V]](https://arxiv.org/abs/2605.08237) (abstract). A methodological precedent outside dynamical systems.
- Tipping indicator in the space of probability measures via the Schrödinger bridge's action functional, aligned with entropy production — [Zhang, Gao, Guo & Duan 2024, arXiv:2403.10405 [V]](https://arxiv.org/abs/2403.10405) (abstract).
- Wasserstein distances between persistence diagrams change before transitions in a slow–fast genetic model — [Berwald & Gidea 2013, arXiv:1309.7919 [V]](https://arxiv.org/abs/1309.7919) (abstract via search summary [S]).
- One-dimensional fact *[formula M]*: $W_2(\mu,\nu)=\|F_\mu^{-1}-F_\nu^{-1}\|_{L^2(0,1)}$, so the space of 1-D distributions with $W_2$ embeds isometrically in a convex cone of $L^2$ (quantile functions); linear indicators (covariance, AR(1), DMD) are then well defined on quantile coordinates. Reference implementation: POT, Python Optimal Transport (MIT [V]).

**4.4 Population structure as a state**
- Silent catastrophes in stage-structured models, due to eigenvector direction — [Boerlijst et al. 2013 [V]](https://doi.org/10.1371/journal.pone.0062033) (abstract).
- Age-at-maturity heterogeneity appeared to mask warnings in sockeye salmon while pink and chum showed signals; age/stage structure can also amplify fluctuations through resonance — [Krkošek & Drake 2014, Proc. R. Soc. B 281:20133221 [V]](https://doi.org/10.1098/rspb.2013.3221) (content from search summary [S]).
- Projection-matrix facts *[formula M; Caswell, Matrix Population Models]*: the convergence rate to the stable age distribution is the damping ratio $\rho=\lambda_1/|\lambda_2|$; the age composition is a point of the simplex, so the transient age-structure dynamics are naturally read in ilr coordinates.

#### Inferences
- A composition-valued state should be tested in ilr coordinates (any orthonormal basis): leading eigenvalue of the ilr covariance, leading eigenvalue of an ilr VAR(1) propagator, Aitchison total variation and its trend. Working on raw shares is wrong twice: the sum constraint forces negative correlations (spurious structure) and closure removes the uniform scaling mode, so a transition carried by the total (e.g., population size) is invisible in shares and must be tested on the total separately.
- For distribution-valued states on the line (incomes, ages, body sizes), quantile coordinates turn $W_2$ into a Hilbert-space metric; CSD then means slowing of the leading mode of quantile-function fluctuations, a test with an OU closed form (an OU process in a finite quantile basis).
- For distributions in higher dimension, no linear embedding is exact; the honest options are (a) a moment or density basis with its truncation error stated, (b) tangent-space linearisation of $W_2$ at a reference measure, or (c) graph-based change-point tests (Chen & Zhang) with $W_2$ as similarity, which detect change but not slowing.
- Zeros (absent sectors, empty age classes) break log-ratios; a detector must say how zeros are handled (replacement, amalgamation) and refuse when zeros dominate.

#### Gaps
- No primary literature found on CSD in Aitchison geometry, nor on Wasserstein-space early warnings for dynamical tipping; only adjacent work (WSA, grokking DMD, Schrödinger bridge).
- No study found on early warnings for economic structural transitions (sectoral compositions, input–output tables) using compositional or matrix geometry.

### 5. Matrices and Shapes: SPD Geometry, Persistent Homology and Fisher Information

#### Takeaway
Covariance and connectivity matrices live on the cone of symmetric positive definite (SPD) matrices, whose affine-invariant and log-Euclidean geometries are mature in brain–computer interfaces and support change-point tests, but I found no work using them for critical slowing down. Persistent homology warnings (Gidea & Katz) are popular in finance but are parameter-dependent and largely untested against proper nulls; their main norm tracks point-cloud spread, i.e. volatility. Fisher information diverges at continuous phase transitions for parametric families, but ecological "Fisher information" indices are a different quantity.

#### Cited Findings

**5.1 SPD matrices**
- Affine-invariant Riemannian framework for tensors — [Pennec, Fillard & Ayache 2006, IJCV 66:41 [V]](https://doi.org/10.1007/s11263-005-3222-z); log-Euclidean metric — [Arsigny, Fillard, Pennec & Ayache 2007, SIAM J. Matrix Anal. Appl. 29:328 [V]](https://doi.org/10.1137/050637996) (records). Formulas *[formula M]*: $\delta_R(A,B)=\|\log(A^{-1/2}BA^{-1/2})\|_F=\big(\sum_i\ln^2\lambda_i(A^{-1}B)\big)^{1/2}$; $\delta_{LE}(A,B)=\|\log A-\log B\|_F$; Fréchet (Karcher) mean; tangent-space projection $S\mapsto\log(\bar C^{-1/2}S\bar C^{-1/2})$.
- BCI classification by Riemannian geometry (minimum distance to Riemannian mean, tangent space) — [Barachant, Bonnet, Congedo & Jutten 2012, IEEE TBME 59:920 [V]](https://doi.org/10.1109/TBME.2011.2172210); primer and review — [Congedo, Barachant & Bhatia 2017, Brain-Comput. Interfaces 4:155 [V]](https://doi.org/10.1080/2326263X.2017.1297192). Reference implementations: pyRiemann (BSD-3-Clause [V]); geomstats (MIT [V]).
- Change points on SPD-valued series: a matrix-log mean model with heterogeneous noise and local scan statistics, consistent recovery of all change points, claimed first for SPD manifold-valued data; Log-Euclidean chosen for tractability; Human Connectome Project — [Lin, Kong & Sun 2019, arXiv:1907.03385 [V]](https://arxiv.org/abs/1907.03385) (abstract). Graph-based (minimal spanning tree) test with a Riemannian metric on SPD, common change-point patterns across subjects — [Dai, Zhang & Srivastava 2019, Med. Image Anal. (doi 10.1016/j.media.2019.101532) [V]](https://arxiv.org/abs/1904.12023) (arXiv record).

**5.2 Brain signals as the test case**
- Long-term iEEG from 14 patients: CSD metrics fluctuated over hours to days and seizure risk was associated with them — [Maturana et al. 2020, Nat. Commun. 11:2172 [V]](https://doi.org/10.1038/s41467-020-15908-3) (abstract).
- Contradiction: long-term multichannel recordings from 28 subjects with a surrogate-based evaluation found "no evidence for critical slowing down prior to human epileptic seizures" — [Wilkat, Rings & Lehnertz 2019, Chaos 29:091104 [V]](https://doi.org/10.1063/1.5122759) (abstract and title).
- Depression onset and termination — [van de Leemput et al. 2014, PNAS 111:87 [V]](https://doi.org/10.1073/pnas.1312114110) (record).

**5.3 Persistent homology**
- Sliding windows over four US index returns build point clouds; loops' persistence encoded as persistence landscapes; "temporal changes in persistence landscapes via their $L^p$-norms" exhibit strong growth near the 2000 and 2007–2009 crashes — [Gidea & Katz 2018, Physica A 491:820 [V]](https://doi.org/10.1016/j.physa.2017.09.028) (abstract). Cryptocurrencies — Gidea, Goldsmith, Katz, Roldan & Shmalo 2020, Physica A 548:123843 [M] (SSRN record [doi:10.2139/ssrn.3202721 [V]](https://doi.org/10.2139/ssrn.3202721)). Persistence landscapes — Bubenik 2015, JMLR 16:77 [M]; toolbox — [Bubenik & Dłotko 2017, J. Symb. Comput. 78:91 [V]](https://doi.org/10.1016/j.jsc.2016.03.009).
- Explanation offered by the method's authors: under the log-periodic power-law singularity bubble model, "whenever the LPPLS model is fitting with the data, TDA generates early warning signals" — [Akingbade, Gidea, Manzi & Nateghi 2023, arXiv:2304.06877 [V]](https://arxiv.org/abs/2304.06877) (abstract; heuristic, not a proof).
- Parameter dependence and a better-performing residual method in other markets — Ismail et al. 2021/2022 [S] (search summary); Bitcoin crashes — [Ismail, Hussain & Noorani 2020, IEEE Access 8:202042 [V]](https://doi.org/10.1109/ACCESS.2020.3036370); Singapore and Taiwan markets — [Yen & Cheong 2021, Front. Phys. 9:572216 [V]](https://doi.org/10.3389/fphy.2021.572216) (records).
- A 2026 review lists the limits: "representation dependence, inferential challenges, interpretability, computational scaling, and the narrowness of one-parameter workflows", and finds topology most effective "when the scientific target is organizational rather than scalar" — [Bailey 2026, arXiv:2603.25760 [V]](https://arxiv.org/abs/2603.25760) (abstract).
- Persistent homology of fields (Kolmogorov flow, Rayleigh–Bénard convection) as a dynamical descriptor of patterns — [Kramár et al. 2016, Physica D 334:82 [V]](https://doi.org/10.1016/j.physd.2016.02.003) (record; content [M]); "early warning signals for topological tipping points" (random attractors) — [Charó et al. 2022, EGU abstract [V]](https://doi.org/10.5194/egusphere-egu22-12686) (record only).
- Reference implementations: Ripser (C++, MIT [V]) — [Bauer 2021, J. Appl. Comput. Topol. 5:391 [V]](https://doi.org/10.1007/s41468-021-00071-5); ripser.py (MIT [V]); GUDHI (MIT [V] per GitHub API); giotto-tda (**AGPL-3.0** [V], copyleft).

**5.4 Fisher information**
- Fisher information related to order parameters (its divergence at continuous transitions) — [Prokopenko, Lizier, Obst & Wang 2011, PRE 84:041116 [V]](https://doi.org/10.1103/PhysRevE.84.041116) (record; content [M]). *[formula M]*: $F(\theta)=\mathbb E[(\partial_\theta\ln p(x\mid\theta))^2]$; for an exponential family $p\propto e^{\theta T(x)}$, $F=\mathrm{Var}_\theta T=\partial_\theta\langle T\rangle$, the susceptibility, which diverges at a second-order transition.
- Ecological "Fisher information" as an index of dynamic regimes (shift-invariant FI of the distribution of states along a trajectory) — [Mayer, Pawlowski & Cabezas 2006, Ecol. Model. 195:72 [V]](https://doi.org/10.1016/j.ecolmodel.2005.11.011); management use — [Eason, Garmestani & Cabezas 2014, Clean Technol. Environ. Policy 16:773 [V]](https://doi.org/10.1007/s10098-013-0687-2) (records; content [M]).

#### Inferences
- CSD on SPD-valued windows has a natural form: if $C_t$ is the windowed covariance, approach to a fold makes $\lambda_{\max}(C_0^{-1}C_t)$ grow like $1/|\lambda_1|$, so the affine-invariant distance from a baseline $\delta_R(C_0,C_t)$ is dominated by $|\ln\lambda_{\max}(C_0^{-1}C_t)|$; this is the leading-eigenvalue indicator of §1 made baseline-invariant (invariant under any linear change of coordinates of the observed channels, which matters for EEG montages).
- Correlation (connectivity) matrices are on the elliptope (unit diagonal), not the full SPD cone; using SPD metrics on correlations discards the variance channel, which is the primary CSD signal. A detector should take covariances, or test variances and correlations separately.
- Gidea–Katz $L^p$ norms grow with the spread of the windowed point cloud; without a null that preserves volatility (e.g. volatility-matched surrogates) a TDA warning cannot be distinguished from a variance warning. This is an inference, consistent with Ismail et al.'s parameter dependence and the authors' own LPPLS heuristic, not a published refutation.
- Fisher information as susceptibility requires a parametrised family; ecological FI indices are descriptive and should not be presented as Fisher information of a parameter.

#### Gaps
- No publication found using SPD geometry (affine-invariant or log-Euclidean) for critical slowing down or tipping detection.
- No formal critique of persistent-homology warnings with surrogate nulls was found.
- Fisher-information EWS critiques were not located.

### 6. Operators: Transfer-Operator Gaps, Koopman/DMD Eigenvalues, Response Theory and Edge States

#### Takeaway
Operator methods generalise CSD from fixed points to attractors of any kind: the second eigenvalue (spectral gap) of a transfer operator estimated on a partition, Ruelle–Pollicott resonances of the generator, and DMD/Koopman eigenvalues approaching the unit circle all measure the slowest decay of correlations, and response theory links them to the system's sensitivity to forcing (fluctuation–dissipation). They also reach transitions CSD misses (attractor crises, homoclinic bifurcations, rate-induced tipping with Koopman residuals), at the cost of heavy data requirements and estimator bias.

#### Cited Findings

**6.1 Transfer operators on a partition**
- Estimated transfer operators on a reduced phase space give an early-warning indicator of the zonal-to-blocked transition in a barotropic model; "the spectrum ... can be used to study the slow dynamics ... as well as the non-Markovian character of the reduction"; slowest timescales three to six weeks associated with metastable regimes detectable as almost-invariant sets — [Tantet, van der Burgt & Dijkstra 2015, Chaos 25:036406 [V]](https://doi.org/10.1063/1.4908174) (abstract). Ulam estimator *[formula M]*: $P_{ij}=\#\{x_t\in B_i,x_{t+\tau}\in B_j\}/\#\{x_t\in B_i\}$; eigenvalues $\mu_k=e^{s_k\tau}$; indicator $\operatorname{Re}s_2\to 0$ or $1-|\mu_2|\to 0$.
- Crisis of a chaotic attractor: local bifurcations are inferred from slowing of correlations (citing Held & Kleinen), whereas global stability of chaotic attractors is characterised by the spectra of Koopman/transfer operators — [Tantet, Lucarini, Lunkeit & Dijkstra 2018, Nonlinearity 31:2221 [V]](https://doi.org/10.1088/1361-6544/aaaf42) (abstract); Lorenz flow crisis resonances — [Tantet, Lucarini & Dijkstra 2018, J. Stat. Phys. 170:584 [V]](https://doi.org/10.1007/s10955-017-1938-0) (record).
- Ruelle–Pollicott resonances, defined for SDEs as eigenvalues of the Kolmogorov generator; correlation functions and spectra decompose over them; "reduced RP resonances can be rigorously defined, as soon as the dynamics is partially observed within a reduced state space" — [Chekroun, Tantet, Dijkstra & Neelin 2020, J. Stat. Phys. 179:1366 [V]](https://doi.org/10.1007/s10955-020-02535-x) (abstract); Part II on the stochastic Hopf bifurcation — [Tantet, Chekroun, Dijkstra & Neelin 2020, J. Stat. Phys. 179:1403 [V]](https://doi.org/10.1007/s10955-020-02526-y); rough parameter dependence and RP resonances in climate models — [Chekroun et al. 2014, PNAS 111:1684 [V]](https://doi.org/10.1073/pnas.1321816111) (records).

**6.2 Koopman operator and DMD**
- DMD — [Schmid 2010, J. Fluid Mech. 656:5 [V]](https://doi.org/10.1017/S0022112010001217); extended DMD as a Koopman approximation — [Williams, Kevrekidis & Rowley 2015, J. Nonlinear Sci. 25:1307 [V]](https://doi.org/10.1007/s00332-015-9258-5) (records). *[formula M]*: exact DMD fits $Y\approx\hat AX$ by $\hat A=YX^+$ via truncated SVD; eigenvalues $\mu_k$, rates $\omega_k=\ln\mu_k/\Delta$; slowing appears as $|\mu_1|\to 1$ ($\operatorname{Re}\omega_1\to 0$). For stochastic linear data, DMD on $(x_t,x_{t+1})$ pairs is the same least-squares estimator as the VAR(1) propagator of §1 up to truncation.
- The DMD reconstruction error detects transient dynamics and regime changes (Kuramoto–Sivashinsky; atmospheric reanalysis) — [Gottwald & Gugole 2020, J. Stat. Phys. 179:1028 [V]](https://doi.org/10.1007/s10955-019-02392-3) (abstract).
- Eigenvalue tracking and Floquet multipliers applied without detrending or deseasoning, robust to changing noise levels, extended to spatio-temporal data (glacier surges; Amazon productivity) — [Smith, Morr, Bookhagen & Boers 2025, arXiv:2506.23946 [V]](https://arxiv.org/abs/2506.23946) (abstract).
- Koopman-based generalised stochastic resilience separating noise-induced fluctuations from a continuous spectrum above tipping — [Miyauchi, Ikeda & Kawahara 2025, arXiv:2508.19655 [V]](https://arxiv.org/abs/2508.19655) (abstract).
- Residual Koopman mode decomposition with control-augmented observables gives a unified indicator for bifurcation and **rate-induced** tipping; learned embeddings outperform prescribed dictionaries in high dimension; applied to AMOC simulations — [Nathaniel, Roesch, DeSantis, Kooloth, Fan, Lucarini et al. 2026, arXiv:2608.14716 [V]](https://arxiv.org/abs/2608.14716) (abstract).
- A 2022 Physica A paper proposed DMD as a spatial EWS capturing the leading spatial mode (search summary [S]; authors not retrieved).
- Reference implementations: PyDMD — [Demo, Tezzele & Rozza 2018, JOSS 3:530 [V]](https://doi.org/10.21105/joss.00530) and v1.0 overview [Ichinaga et al. 2024, arXiv:2402.07463 [V]](https://arxiv.org/abs/2402.07463), MIT [V]; PyKoopman, MIT [V].

**6.3 Linear response and fluctuation–dissipation**
- Response theory decomposed over Koopman-generator eigenfunctions "towards identifying relaxation timescales and modes and towards relating forced and natural fluctuations" — [Santos Gutiérrez & Lucarini 2022, J. Phys. A 55:425002 [V]](https://doi.org/10.1088/1751-8121/ac90fd) (abstract).
- Response operators predict climate change in a GCM with $O(10^5)$ degrees of freedom, global averages and spatial patterns — [Lucarini, Ragone & Lunkeit 2017, J. Stat. Phys. 166:1036 [V]](https://doi.org/10.1007/s10955-016-1506-z) (abstract); review of climate variability and change — [Ghil & Lucarini 2020, Rev. Mod. Phys. 92:035002 [V]](https://doi.org/10.1103/RevModPhys.92.035002).
- A climate state close to a tipping point has a degenerate linear response (extreme sensitivity) — Ashwin & von der Heydt 2020, J. Stat. Phys. [M] (only the EGU abstract record [doi:10.5194/egusphere-egu2020-4671 [V]](https://doi.org/10.5194/egusphere-egu2020-4671); content via search summary [S]).
- *[formula M]*: linear response $\delta\langle\Phi\rangle(t)=\int_0^\infty G_\Phi(s)f(t-s)\,ds$; for an OU process $dx=Jx\,dt+B\,dW$ perturbed by $f(t)u$, $\delta\langle x\rangle(t)=\int e^{Js}u f(t-s)ds$, and $e^{Js}=C(s)C^{-1}$, so the response operator equals the normalised lagged covariance: the fluctuation–dissipation relation that makes degenerate fingerprinting a response measurement. Near a fold the static susceptibility $-J^{-1}$ diverges along $v w^\top/|\lambda_1|$.

**6.4 Edge states and Melancholia states**
- Edge states (unstable states on basin boundaries) in the climate system, methods to find them and their role in critical transitions — [Lucarini & Bódai 2017, Nonlinearity 30:R32 [V]](https://doi.org/10.1088/1361-6544/aa6b11) (abstract); noise-induced transitions between warm and snowball states pass through Melancholia states — [Lucarini & Bódai 2019, PRL 122:158701 [V]](https://doi.org/10.1103/PhysRevLett.122.158701) (abstract); edge tracking originated in shear flow — [Skufca, Yorke & Eckhardt 2006, PRL 96:174101 [V]](https://doi.org/10.1103/PhysRevLett.96.174101) (record); edge states select observables with CSD — Lohmann et al. 2024 (§1.5).

#### Inferences
- The three operator estimators (Ulam matrix, DMD/EDMD, VAR(1) on coordinates) are the same object, the Koopman/transfer semigroup restricted to a function space, with different bases (indicators of cells, dictionaries, linear coordinates). A detector abstraction can take "a dictionary of observables on the state space" as its single input and produce the spectrum, with the partition, the dictionary or the linear coordinates as cases.
- Ulam's method needs many visits to every cell; in more than a few dimensions it requires a reduced space, and the non-Markovianity of the reduction biases the gap (Tantet 2015 discusses it). Refusal criteria: cells with too few transitions; spectra that change with lag $\tau$ (non-Markovian).
- Operator indicators see what scalar CSD misses: oscillatory slowing (complex $\mu$), homoclinic approach (frequency drop with fixed decay rate), crisis (resonance approaching the imaginary axis without a fixed point losing stability). Rate-induced tipping needs nonautonomous tools (control-augmented Koopman, pullback measures).

#### Gaps
- No calibrated false-alarm/detection rates were found for rolling-window spectral-gap indicators.
- Equation numbers of the Ulam and DMD estimators in the sources were not checked.
- The 2022 Physica A DMD spatial-EWS paper could not be identified.

### 7. Hierarchies and Scales: Coarse-Graining, Renormalisation and Finite-Size Scaling

#### Takeaway
Continuous phase transitions are detected by how statistics change with system size and with coarse-graining (finite-size scaling, Binder cumulant crossings, growth of the correlation length, phenomenological renormalisation of neural populations); aggregation across units can amplify CSD when the critical mode is uniform and erase it when it is not, and quantities carried up a hierarchy as shares or logits remove exactly the uniform mode.

#### Cited Findings
- Finite-size scaling theory — [Fisher & Barber 1972, PRL 28:1516 (record retrieved as a 1988 reprint) [V]](https://doi.org/10.1016/B978-0-444-87109-1.50007-8); Binder's analysis of block distribution functions — [Binder 1981, Z. Phys. B 43:119 (record retrieved as a 1988 reprint) [V]](https://doi.org/10.1016/B978-0-444-87109-1.50012-1). *[formulas M]*: near $T_c$, $\xi\sim|t|^{-\nu}$, $\chi\sim|t|^{-\gamma}$, relaxation time $\tau\sim\xi^{z}$ (critical slowing down with dynamic exponent $z$); $\chi_L=L^{\gamma/\nu}\tilde\chi(tL^{1/\nu})$; Binder cumulant $U_L=1-\langle m^4\rangle/(3\langle m^2\rangle^2)$, whose curves for different $L$ cross at $T_c$ (2D Ising: $T_c=2/\ln(1+\sqrt2)$ exactly, $U^*\approx 0.61$).
- Scale-free correlations in starling flocks (correlation length growing with flock size) — [Cavagna et al. 2010, PNAS 107:11865 [V]](https://doi.org/10.1073/pnas.1005766107) (record; content [M]).
- Phenomenological RG for neural populations: coarse-grained variables approach "a fixed non-Gaussian form", "scaling in both static and dynamic quantities" in 1000+ hippocampal cells — [Meshulam, Gauthier, Brody, Tank & Bialek 2019, PRL 123:178103 [V]](https://doi.org/10.1103/PhysRevLett.123.178103) (abstract); PCA meets RG — [Bradde & Bialek 2017, J. Stat. Phys. 167:462 [V]](https://doi.org/10.1007/s10955-017-1770-6); scaling in the phenomenological RG and its relation to criticality — [Nicoletti, Suweis & Maritan 2020, PRR 2:023144 [V]](https://doi.org/10.1103/PhysRevResearch.2.023144); quasi-universal scaling as edge-of-instability dynamics rather than a critical point — Morales, di Santo & Muñoz 2023, PNAS 120:e2208998120 [M] (preprint record [doi:10.1101/2021.11.23.469734 [V]](https://doi.org/10.1101/2021.11.23.469734)). Criticality and information-based fitness — [Hidalgo et al. 2014, PNAS 111:10095 [V]](https://doi.org/10.1073/pnas.1319166111) (record).
- Aggregation in practice: aggregate unemployment showed no CSD while the disaggregated field did (French départements) — [Halleck Vega et al. 2025 [V]](https://doi.org/10.1016/j.isci.2025.114097) (full text); autocorrelation is more fragile than variance under temporal aggregation — [Frossard et al. 2015 [V]](https://doi.org/10.3389/fevo.2015.00112) ([S] content).

#### Inferences
- Block aggregation by a sum $S=\mathbf 1_b^\top x$ over a block of $b$ units keeps the projection of each mode on $\mathbf 1_b$: if the critical eigenvector is locally uniform at the block scale, aggregation averages out non-critical noise and *sharpens* CSD ($\mathrm{Var}(S)\propto b^2/|\lambda_1|$ against $b$ for the rest); if it is a dipole or oscillates in space within blocks, the sum cancels it and aggregation *erases* CSD. The lag-1 autocorrelation of an aggregate is a variance-weighted mixture of mode autocorrelations, so it rises only once the critical mode dominates the aggregate's variance.
- Operators that carry quantities up a hierarchy are filters in this sense: sums and means pass the uniform mode; **shares, logits and z-scores among siblings remove it** (shares are compositions; z-scores subtract the mean); the graph Laplacian annihilates constants. A transition carried by a common factor across units is therefore invisible in shares; one carried by redistribution is invisible in sums. A detector working on a hierarchy should test both channels at every level and report which one carries the signal.
- Finite-size scaling gives a detector for continuous transitions that does not need a time series: compute susceptibility or the Binder cumulant of an order parameter at several aggregation scales (block sizes) and look for scale collapse or crossings. It assumes an order parameter, a known (or fitted) scaling form and enough scales; it is blind to first-order (discontinuous) transitions except through hysteresis and bimodality.
- Phenomenological RG (coarse-graining by correlation, as in Meshulam et al.) yields scaling exponents for any multichannel signal, but scaling alone does not establish proximity to a critical point (Morales et al.'s edge-of-instability reading); it should be reported as a descriptor, not as a warning.

#### Gaps
- No general theory or benchmark was found for how EWS change under coarse-graining across irregular administrative hierarchies (zoning effects of the modifiable areal unit problem).
- Equation numbers of finite-size scaling and the Binder cumulant were not checked against the originals.

### 8. Learned Detectors Anticipate Transitions in High Dimension but Generalise Narrowly

#### Takeaway
Classifiers trained on normal-form simulations (CNN–LSTM), reservoir computers with a parameter channel, and graph neural networks can warn earlier and with fewer false positives than generic indicators, sometimes predicting the type of transition, the post-tipping state or where on a network it occurs; their failures are extrapolation beyond the training regime, sensitivity to preprocessing, short records and uncalibrated uncertainty.

#### Cited Findings
- A deep-learning classifier "predicts certain qualitative aspects of the new state, and is also more sensitive and generates fewer false positives than generic indicators"; trained on "a universe of possible models" rather than the study system — [Bury et al. 2021, PNAS 118:e2106140118 [V]](https://doi.org/10.1073/pnas.2106140118) (abstract).
- Climate phase transitions: deep learning trained on abundant synthetic data transferred to limited empirical data (proof of concept) — [Dylewsky et al. 2023, J. R. Soc. Interface 20:20220562 [V]](https://doi.org/10.1098/rsif.2022.0562) (abstract).
- Training on simple models predicts transitions in complex natural systems — Deb, Sidheekh, Clements, Krishnan & Dutta 2022, R. Soc. Open Sci. 9:211475 [M] (preprint record [doi:10.1101/2021.03.15.435556 [V]](https://doi.org/10.1101/2021.03.15.435556)).
- Reservoir computing with a parameter channel predicts critical transition and collapse — [Kong, Fan, Grebogi & Lai 2021, PRR 3:013090 [V]](https://doi.org/10.1103/PhysRevResearch.3.013090) (record; content [M]); machine learning for tipping in complex systems — [Panahi et al. 2024, PRR 6:043194 [V]](https://doi.org/10.1103/PhysRevResearch.6.043194) (record).
- Extrapolation: ML "can be surprisingly effective" but does "fail when 'too much' extrapolation is required" — [Patel & Ott 2023, Chaos 33:023143 [V]](https://doi.org/10.1063/5.0131787) (abstract).
- Rate-induced tipping by deep learning — Huang, Bathiany, Ashwin & Boers 2024, Nat. Mach. Intell. [M] (EGU abstract record "Anticipating rate-induced tipping by a deep learning framework" [doi:10.5194/egusphere-egu24-12187 [V]](https://doi.org/10.5194/egusphere-egu24-12187)); normal-form-informed prediction for irregularly sampled series — [Zhuge, Li & Chen 2024, arXiv:2407.18693 [V]](https://arxiv.org/abs/2407.18693) (abstract).
- Networks: GIN–GRU predicting where transitions occur (§3.5) — [Liu et al. 2024 [V]](https://doi.org/10.1103/PhysRevX.14.031009); learned governing rules guiding GNNs for critical behaviour of spreading — [Wang et al. 2025, arXiv:2504.09622 [V]](https://arxiv.org/abs/2504.09622) (abstract).
- Failures and limits: preprocessing matters for deep-learning EWS — [Dablander & Bury 2022, PsyArXiv doi:10.31234/osf.io/j7xug [V]](https://doi.org/10.31234/osf.io/j7xug) (record; content [M]); estimating uncertainty for critical transitions with deep learning — Lapeyrolerie & Boettiger 2023, Methods Ecol. Evol. 14:785 [M] (review record [doi:10.1111/2041-210x.14013/v2/response1 [V]](https://doi.org/10.1111/2041-210x.14013/v2/response1)); the GIN–GRU method needs 20 time points per node and transfer across sizes is future work — [Masuda 2024 Viewpoint [V]](https://physics.aps.org/articles/v17/110).
- Reference implementation for classical and DL-based univariate EWS: ewstools — [Bury 2023, JOSS 8:5038 [V]](https://doi.org/10.21105/joss.05038), MIT [V].

#### Inferences
- Learned detectors' "type of transition" outputs are only as wide as their training library (fold, Hopf, transcritical normal forms); a structured-state detector built this way inherits the library's blindness and must declare it. For structured states, training data must respect the state space (e.g., simulate compositions in ilr, fields with the right spatial covariance).
- Their false-alarm rates are measured on held-out simulations of the same families; on empirical data (O'Brien et al. 2023) generic and second-generation methods were unreliable. A learned detector should be calibrated against the same surrogate nulls as classical ones before use.

#### Gaps
- No published GNN tipping predictor with calibrated false-alarm rates on empirical networks was found.

### 9. Information-Theoretic and Thermodynamic Indicators Peak Near, but Not Always At, Transitions

#### Takeaway
Mutual information typically peaks at order–disorder transitions and can locate them with limited data; transfer entropy (information flow) in the kinetic Ising model peaks on the disordered side, not at the critical point; entropy production characterises nonequilibrium transitions and broken detailed balance in brain data. These are useful complements but their peaks are offset and model-dependent.

#### Cited Findings
- Mutual information of Ising systems — [Matsuda et al. 1996, Int. J. Theor. Phys. 35:839 [V]](https://doi.org/10.1007/BF02330576) (record; content [M]: MI peaks near $T_c$).
- In the Vicsek flocking model MI "is a sensitive indicator of the phase transition location" and, when few particles or components are observed, "a better measure ... than the susceptibility" — [Wicks, Chapman & Dendy 2007, PRE 75:051125 [V]](https://doi.org/10.1103/PhysRevE.75.051125) (abstract).
- "Information flow in a kinetic Ising model peaks in the disordered phase" — [Barnett, Lizier, Harré, Seth & Bossomaier 2013, PRL 111:177203 [V]](https://doi.org/10.1103/PhysRevLett.111.177203) (title).
- Information dissipation as an early-warning signal for the Lehman Brothers collapse — [Quax et al. 2013, Sci. Rep. 3:1898 [V]](https://doi.org/10.1038/srep01898) (record; content [M]).
- Entropy production "vanishes identically for equilibrium systems and is positive for the nonequilibrium case"; characterisation of continuous and discontinuous nonequilibrium transitions — [Noa, Harunari, de Oliveira & Fiore 2019, PRE 100:012104 [V]](https://doi.org/10.1103/PhysRevE.100.012104) (abstract); broken detailed balance and entropy production in the human brain — [Lynn, Cornblath, Papadopoulos, Bertolero & Bassett 2021, PNAS 118:e2109889118 [V]](https://doi.org/10.1073/pnas.2109889118) (record).
- *[formulas M]*: transfer entropy $T_{Y\to X}=\sum p(x_{t+1},x_t,y_t)\ln\frac{p(x_{t+1}\mid x_t,y_t)}{p(x_{t+1}\mid x_t)}$ (Schreiber 2000); for jointly Gaussian processes it equals half the Granger log-ratio of residual variances (Barnett, Barrett & Seth 2009). For a linear OU process $dx=Jx\,dt+B\,dW$ with $D=BB^\top/2$ and $JC+CJ^\top+2D=0$, the matrix $Q=JC+D$ is antisymmetric, the stationary probability velocity is $v(x)=QC^{-1}x$, detailed balance holds iff $Q=0$ (equivalently $JD=DJ^\top$), and the entropy production rate ($k_B=1$) is $\sigma=\langle v^\top D^{-1}v\rangle=\operatorname{tr}(D^{-1}QC^{-1}Q^\top)$ *[formula M; derivation in these notes, to be checked against a source before use as a fixture]*.

#### Inferences
- In the Gaussian linear regime, MI between components and transfer entropy are functions of $C$ and $\hat A$, so they add nothing beyond §1 there; their value is in non-Gaussian regimes and for directed coupling (which component drives the critical mode, i.e. a data estimate of the left eigenvector's support).
- Barnett et al.'s offset peak means information-flow indicators can warn on approach from the disordered side and then decline at the transition; a detector must use trends, not peak detection, and say on which side it works.

#### Gaps
- No calibrated evaluation of entropy-production-based EWS on empirical high-dimensional data was found; the OU entropy-production formula above needs checking.

### 10. Reference Implementations and Their Licences

#### Takeaway
Most reference packages are permissively licensed (MIT, BSD) and can be used as test oracles; giotto-tda is AGPL-3.0 and should not be linked.

#### Cited Findings (licences read this session from GitHub's API or the licence/DESCRIPTION file)
- spatialwarnings (R; spatial EWS, patch sizes, null models): MIT + file — [GitHub [V]](https://github.com/spatial-ews/spatialwarnings).
- EWSmethods (R; univariate and multivariate EWS incl. MAF/PCA, ML models): MIT + file — [GitHub [V]](https://github.com/duncanobrien/EWSmethods); paper Ecography 2023 e06674 [S] (only a review record found).
- earlywarnings (R; Dakos & Lahti's toolbox): BSD_2_clause + file — [GitHub [V]](https://github.com/earlywarningtoolbox/earlywarnings-R).
- ewstools (Python; univariate EWS, spectral EWS, DL classifiers): MIT — [GitHub [V]](https://github.com/ThomasMBury/ewstools).
- pyunicorn (Python; functional/climate networks, recurrence networks): BSD 3-clause text — [GitHub [V]](https://github.com/pik-copan/pyunicorn).
- PyDMD: MIT; PyKoopman: MIT — [GitHub [V]](https://github.com/PyDMD/PyDMD), [GitHub [V]](https://github.com/dynamicslab/pykoopman).
- pyRiemann: BSD-3-Clause; geomstats: MIT — [GitHub [V]](https://github.com/pyRiemann/pyRiemann), [GitHub [V]](https://github.com/geomstats/geomstats).
- Ripser (C++): MIT; ripser.py: MIT; GUDHI: MIT (per GitHub's SPDX field; parts of GUDHI have historically been GPL — check before adopting [M]); giotto-tda: **AGPL-3.0** — [GitHub [V]](https://github.com/giotto-ai/giotto-tda).
- POT (optimal transport): MIT; ruptures (change points): BSD-2-Clause; PyGSP: BSD-3-Clause; netrd: MIT; PySAL esda: BSD-3-Clause — [GitHub API [V]](https://github.com/PythonOT/POT).

#### Inferences
- Each package can serve as an external oracle for a check (e.g., esda's Moran's I on a given weight matrix, pyRiemann's Riemannian distance, PyDMD eigenvalues, spatialwarnings' r-spectrum), in the spirit of fixtures generated by running reference libraries rather than typed from memory.

#### Gaps
- GUDHI's per-module licences and EWSmethods' published paper were not checked.

### 11. What This Means for the Design

#### Takeaway
A detector needs to know five things of a state space: (1) linear coordinates in which fluctuations can be measured (possibly after a chart such as ilr, quantile functions, matrix logarithm or a dictionary of observables), (2) an inner product or metric in those coordinates, (3) a neighbourhood or adjacency structure (for spatial statistics and graph spectra), (4) a hierarchy of aggregations with the operators that carry quantities between levels, and (5) constraints and invariances (simplex closure, positivity, unit diagonal, permutation of nodes). Every indicator surveyed is then one of a small number of functionals of the windowed covariance $C$, the propagator $\hat A$ (or a transfer/Koopman matrix), the neighbourhood operator, or the scale axis, and its blind spots can be stated from the structure before any data are seen.

#### Cited Findings
- The structural facts the abstraction rests on are those of §1 (critical-mode concentration along $v$, excitation through $w$; [Masuda 2026 [V]](https://arxiv.org/abs/2608.15476)), §2 (spatial variance diverges only for nonuniform centred critical modes; strong coupling kills spatial warning, [Clarke et al. 2026 [V]](https://doi.org/10.1088/2632-072X/ae6217)), §4 (Aitchison geometry, [Egozcue et al. 2003 [V]](https://doi.org/10.1023/A:1023818214614)), §5 (SPD geometry, [Pennec et al. 2006 [V]](https://doi.org/10.1007/s11263-005-3222-z)), §6 (operators on dictionaries, [Williams et al. 2015 [V]](https://doi.org/10.1007/s00332-015-9258-5)) and §7 (finite-size scaling).

#### Inferences

**11.1 An abstraction over state spaces.** Proposed interface (names illustrative):

- `StateSpace` declares:
  - `chart`: a map from the native state to linear coordinates $\phi: \mathcal X\to\mathbb R^d$ and its inverse where defined. Cases: identity (vectors, fields), ilr with a stated basis (compositions), quantile grid (1-D distributions), $\log$ of SPD after whitening by a reference (covariances), Laplacian eigenbasis (graph signals), a dictionary of observables (operator methods).
  - `metric`: the inner product on chart coordinates (Euclidean after ilr equals Aitchison; Frobenius after matrix-log equals log-Euclidean; affine-invariant via whitening at a reference point), with the statement whether the chart is an isometry (ilr, 1-D quantiles: yes; tangent-space SPD: only locally).
  - `neighbourhood`: a weighted graph $W$ on units (lattice, contiguity, distance band, flows) or `None`; provides Laplacian $L$, Moran weights and graph Fourier basis.
  - `hierarchy`: levels with carry operators labelled by what they keep and kill: `sum/mean` (passes the uniform mode), `share/logit/zscore` (kills it; output is compositional or centred), `laplacian` (kills constants), `recompute` (nonlinear). The detector uses these labels to report which channel (level or distribution) a warning lives in.
  - `constraints`: simplex closure (and zero policy), positivity, unit diagonal (correlation vs covariance), permutation invariance of units; the detector refuses indicators that violate them (e.g., raw-share covariance).
  - `units/scale of measurement`: so that indicators invariant to rescaling (correlations, $\lambda_{\max}(C_0^{-1}C_t)$) are preferred where units are arbitrary.

- `Detector` declares: the object it estimates (spectrum of $\hat A$; dominant eigenpair of $C$; a projection; a neighbourhood statistic; a scale statistic; a structural threshold), the transitions it can see (fold, transcritical, pitchfork via real $\mu\to 1$; Hopf via complex $|\mu|\to 1$; homoclinic via frequency drop; Turing via $k^*>0$ in the dispersion relation; continuous phase transitions via finite-size scaling; percolation via structural thresholds), the transitions it is blind to (critical mode orthogonal to the observed channel; saddle escape; noise-induced tipping far from bifurcation; rate-induced tipping; discontinuous transitions without precursors), its null model and its refusal conditions (window shorter than a multiple of the dimension; $\hat C(0)$ ill-conditioned beyond shrinkage; too few transitions per cell; the record's trend not removable; surrogates indistinguishable).

**11.2 How each indicator generalises under it.**
- Variance → leading eigenvalue of the chart covariance $\lambda_{\max}(\hat C)$, or baseline-invariant $\lambda_{\max}(\hat C_0^{-1}\hat C_t)$ (equivalently the affine-invariant SPD distance's leading term); trace → total variation (Aitchison for compositions).
- Lag-1 autocorrelation → leading modulus and argument of $\hat A=\hat C(1)\hat C(0)^{-1}$ (shrinkage, reduced rank), decay rate $-\ln|\mu_1|/\Delta$ and frequency $\arg\mu_1/\Delta$ (Williamson & Lenton's extra indicator).
- Direction → estimated critical eigenvector (right) and its excitation support (left), reported as a field over units so a user sees *where*.
- Skewness/flickering → projected onto the estimated critical direction, not per coordinate.
- Spatial variance, skewness, Moran's I → on the `neighbourhood` graph, with the structural term removed by baseline referencing when a reference period exists; spatial spectrum → graph Fourier power by Laplacian frequency; Turing vs tipping → estimated dispersion relation $\sigma(\lambda_k)$ over graph frequencies.
- Patch sizes → connected components of thresholded fields on the graph (refuse when the threshold is arbitrary and results depend on it).
- DNB → a localised-mode detector: within-group vs between-group correlation for a group found by clustering of $\hat C$; declares blindness to delocalised modes.
- Networks → effective coordinate $R=a^\top x$ with $a$ chosen by the reduction (degree-weighted, dominant eigenvector, DART) as one projection among others; structural detectors (spectral radius of $A$ or the non-backtracking matrix, k-core) as a separate class for percolation/cascades; graph-sequence change points (Chen–Zhang scan on a graph metric) as a third class, kept apart from dynamical proximity.
- Distributions → quantile-chart covariance and propagator (1-D); otherwise change-point tests with $W_2$.
- Operators → spectrum of a transfer matrix on a partition or of EDMD on a dictionary; kinesis's existing transfer operator becomes one case of the dictionary interface.
- Hierarchy → every indicator computed at every level on both channels (sums and shares), plus finite-size/aggregation scaling (susceptibility and Binder cumulant against block size) for continuous transitions.

**11.3 Closed forms that verify an implementation.**
- OU on a graph: $dx=-(\gamma I+\kappa L)x\,dt+\sigma dW$; exact $C=\frac{\sigma^2}{2}(\gamma I+\kappa L)^{-1}$, $C(\tau)=e^{-(\gamma I+\kappa L)\tau}C$, Laplacian-mode variances $\sigma^2/(2(\gamma+\kappa\lambda_k))$. Checks: $\lambda_{\max}(\hat C)\to\sigma^2/(2\gamma)$ with eigenvector $\mathbf 1/\sqrt n$; centred spatial variance stays bounded as $\gamma\to 0$ while the mean's variance diverges (a test that the detector *reports* the blindness); heterogeneous $\gamma_i$ moves the critical mode off uniform and spatial variance then diverges (Masuda's condition).
- General linear SDE with known $J$, $B$: solve the Lyapunov equation exactly (Bartels–Stewart) and check estimators of $C$, $\hat A$, decay rates, left/right eigenvectors, the $1/(2|\lambda_1|)$ divergence and the $w^\top BB^\top w$ excitation factor; a non-normal $J$ (e.g., Jordan-like 2×2) as the case where covariance and propagator directions differ; a case with $w^\top B=0$ as a silent catastrophe that every variance-based detector must miss.
- Discrete VAR(1) with $|\mu_1|\to 1$ (real and complex pairs) for Hopf vs fold classification and Williamson–Lenton's frequency indicator.
- Compositions: logistic-normal process $x_t=\mathcal C(\exp(V y_t))$ with $y_t$ an OU in ilr; the ilr covariance and propagator are known; check basis invariance of eigenvalues under a change of orthonormal $V$; check that a common multiplicative factor (uniform mode of the totals) is invisible in shares and visible in totals.
- Distributions: Gaussian family $N(m_t,s_t^2)$ with $(m_t,\ln s_t)$ an OU: $W_2$ between Gaussians is closed form, quantile functions are $m+s\Phi^{-1}(p)$; check quantile-chart indicators.
- SPD: windowed covariances of the OU above have known expectations; check $\delta_R$ and $\delta_{LE}$ against pyRiemann/geomstats as oracles.
- Ising/Potts at known critical points: 2D Ising $T_c=2/\ln(1+\sqrt2)$, exponents $\nu=1$, $\gamma=7/4$, $\beta=1/8$, $z\approx 2.17$ (Metropolis, [M]); Binder crossings at $T_c$; finite-size scaling collapse with exact exponents; q-state Potts on the square lattice $T_c=1/\ln(1+\sqrt q)$ ($J=k_B=1$), continuous for $q\le4$, first order for $q>4$ (a check that the finite-size detector refuses or flags discontinuity) *[all M; exact results of Onsager and Baxter]*. The infrastructure's physis already has the Potts model (CLAUDE.md, physis decision 0022), so these can be generated in-house.
- Transfer operators: a double-well SDE whose Kramers rate is known asymptotically ($\sim e^{-\Delta V/D}$) gives the second eigenvalue of the generator; check the Ulam estimate's gap against it, and its bias for too-coarse partitions.
- Network reductions: Gao et al.'s mutualistic dynamics on a degree-uncorrelated graph versus a heterogeneous one, where Kundu et al. show the reduction's error grows with dispersion of $x_i^*$; Ott–Antonsen order-parameter dynamics for Kuramoto networks give an exact low-dimensional target.
- Percolation: Erdős–Rényi giant component at $\langle k\rangle=1$; configuration model Molloy–Reed threshold; check structural detectors.

**11.4 What to build first.**
1. The `StateSpace` interface with five charts (identity, ilr, 1-D quantile, matrix-log with whitening, Laplacian basis) and the `hierarchy` labels, on tyche's compositions and morphe's graphs.
2. The linear core: windowed $\hat C$, shrinkage, $\hat A$ with reduced rank, spectrum (moduli, arguments), left/right eigenvectors, baseline-invariant leading eigenvalue; multivariate surrogates (fitted VAR(1) under the null of constant $\hat A$, multivariate phase randomisation); refusal rules on sample size and conditioning. Verified first against the graph-OU and general-OU closed forms.
3. Spatial statistics on an arbitrary weighted graph (spatial variance with baseline referencing, skewness, Moran's I via tyche's spatial autocorrelation, graph spectral power, dispersion relation over Laplacian frequencies), verified on graph OU and checked against esda and spatialwarnings as oracles.
4. The hierarchy runner: every detector at every level on both channels (sums; shares or z-scores), with the report saying which channel carries a warning; aggregation-scale susceptibility and Binder cumulant, verified on Ising/Potts.
5. Operator detectors through one dictionary interface (Ulam partition, linear coordinates, EDMD), reusing kinesis's transfer operator; verified on double-well and VAR(1) cases.
6. Later: SPD-trajectory detectors for EEG/fMRI covariances; graph-sequence change points (Chen–Zhang scan) kept as a separate structural class; persistent homology only with volatility-matched nulls; learned detectors only after calibration against the same nulls.

#### Gaps
- The proposed indicators on compositions, quantile functions and SPD trajectories have, as far as this survey found, no published false-alarm/detection benchmarks; their rates would have to be established in-house on the closed forms above before any claim on real data.
- Whether CSD is visible at all in empirical structured systems remains contested (seizures: Maturana et al. 2020 vs Wilkat et al. 2019; lakes: O'Brien et al. 2023; tipping-time prediction from historical data judged too uncertain for major Earth-system components by Ben-Yami et al. 2024, Sci. Adv. [M], record found only as [EGU abstract doi:10.5194/egusphere-egu24-5870 [V]](https://doi.org/10.5194/egusphere-egu24-5870)).

## Part III. The Earth System, Ecosystems and Epidemics

Scope: applications of early-warning signals (EWS) and critical-transition analysis in climate, ecology and epidemiology; the records used, with access and terms; the results and their critiques; and the low-order models with known bifurcations that serve as test cases. Compiled 2026-11-19.

*Remark.* Part III's conventions. The bibliographic markers are those of §0.3; here [S] covers secondary indexes and summaries (OpenAlex, Europe PMC, a search-engine summary, a news release). Statements about *content* say where they come from: "(abstract)" means the abstract was read this session (via OpenAlex, Europe PMC or arXiv); "(content M)" means the content statement is from memory. Equations marked "(eq. M)" are from memory; equations marked "(derived here)" were derived in these notes from the stated model and should be checked before implementation.

Note on method: Crossref was queried in batches (some agents share the IP, so a few queries hit HTTP 429 and were re-run); abstracts came from OpenAlex and Europe PMC because nature.com and PMC pages were blocked by redirects or captchas.

### 1. Climate Tipping Elements and the Evidence of Slowing Down

#### Takeaway
Assessments since Lenton et al. 2008 list a few "core" tipping elements. The 2025 Global Tipping Points Report judges that warm-water coral reefs have already passed theirs, and that Greenland and West Antarctica may have passed theirs. Observational slowing-down evidence exists for the AMOC fingerprints, central-western Greenland and the Amazon (vegetation optical depth), but every one of these claims is contested. The disputes concern what the fingerprint represents, data coverage and gap filling, noise colour, which vegetation product is used, and extrapolating to a tipping *time*, which a 2024–2026 line of work shows cannot be done reliably. Sea ice is widely judged not to tip (summer) or to tip only at very high warming (winter). Dansgaard–Oeschger events, once the flagship of "no warning" (2010) and then of "warning at decadal scales" (2018), were re-evaluated in 2025 as inconclusive.

#### Cited Findings

##### 1.1 Assessments of Tipping Elements
- Lenton et al. 2008 [V] introduced "tipping element" for large-scale components of the Earth system that may pass a tipping point. They compiled a short list from the literature and a workshop, ranked sensitivity by expert elicitation, and explained "how, in principle, early warning systems could be established" (abstract) — [Lenton et al. 2008, PNAS 105:1786–1793](https://doi.org/10.1073/pnas.0705414105).
- Armstrong McKay et al. 2022 [V] revised the list into global "core" and regional "impact" tipping elements with temperature thresholds. They found that ~1.1 °C of warming "already lies within the lower end of some tipping point uncertainty ranges", that several tipping points may be triggered at 1.5 to <2 °C, and they called for "improved tipping point risk assessment, early warning capability" (abstract). Their central estimates and ranges (content M; check the paper's Table 1): AMOC collapse ~4 °C (1.4–8); Greenland 1.5 °C (0.8–3); West Antarctica 1.5 °C (1–3); low-latitude coral reefs 1.5 °C (1–2); boreal permafrost abrupt thaw 1.5 °C (1–2.3); Labrador–Irminger Sea convection 1.8 °C (1.1–3.8); Amazon dieback 3.5 °C (2–6); Arctic winter sea ice 6.3 °C (4.5–8.7) — [Armstrong McKay et al. 2022, Science 377:eabn7950](https://doi.org/10.1126/science.abn7950).
- Lenton et al. 2019 [V], a Nature comment ("too risky to bet against"), framed tipping as a planetary emergency — [Lenton et al. 2019](https://doi.org/10.1038/d41586-019-03595-0). A review of mechanisms, evidence and impacts is Wang et al. 2023 [V] — [Wang et al. 2023, Rev. Geophys. 61:e2021RG000757](https://doi.org/10.1029/2021RG000757). Interactions and cascades: Wunderling et al. 2021 [V] — [ESD 12:601–619](https://doi.org/10.5194/esd-12-601-2021); Klose et al. 2021 [V] on what a "tipping cascade" means — [ERL 16:125011](https://doi.org/10.1088/1748-9326/ac3955).
- Global Tipping Points Report 2023 [M]: Lenton et al. (eds.), University of Exeter, released at COP28 (December 2023). Not verified this session.
- Global Tipping Points Report 2025 [V, provider page]: led by T. M. Lenton, with more than 160 authors from more than 87 institutions in 23 countries. Its Section 2 gives these values (provider page; no ranges are given there except where noted):
  - warm-water coral reefs: central threshold 1.2 °C, already crossed, with the 2023–25 bleaching the worst on record;
  - Greenland and West Antarctic ice sheets: "around 1.5 °C", at risk since at least 1 °C, with high confidence in irreversible collapse, and tipping "may already have been crossed";
  - AMOC: "around 2.0 °C"; it "cannot be ruled out at current warming levels", although the likelihood is uncertain because models and observations are limited;
  - subpolar gyre convection: "around 1.5 °C";
  - land permafrost: "around 1.5 °C" (medium confidence, regional);
  - boreal forest: ~2.0 °C;
  - mountain glaciers: ~2.0 °C;
  - Amazon: "below 2 °C" when deforestation and climate change combine;
  - Indian summer monsoon: "limited evidence for tipping dynamics", although recent modelling supports them;
  - Arctic summer sea ice: "unlikely to reach tipping points";
  - Antarctic sea ice: "highly uncertain", and change "may already be underway";
  - jet stream: "evidence against tipping has strengthened".

  Source: [GTPR 2025 summary, Section 2](https://global-tipping-points.org/summary-report/section-1/). The release date of 13 October 2025 and the range 1–1.5 °C for coral reefs come from [NOC news, 13 Oct 2025](https://www.noc.ac.uk/news/global-tipping-points-report-2025-reveals-first-earth-system-tipping-point-reached) [S]. The provider page also states that the AMOC appears in 45% of the interactions assessed (search summary of the same page [S]).
- Dakos et al. 2024 [V] reviewed 20 years of real-world EWS case studies across climate, ecology and human systems, and found that "in the majority of the case studies we reviewed, the performance of most early warnings was positive" (abstract) — [Dakos et al. 2024, ESD 15:1117–1135](https://doi.org/10.5194/esd-15-1117-2024). (See §3 for meta-analyses that reach the opposite conclusion on systematically sampled data.)
- Lenton et al. 2024 [V] argued that where satellite records are too short for temporal EWS, "complementary spatial indicators can leverage the exceptional spatial-temporal coverage of remotely sensed data" (abstract) — [Lenton et al. 2024, Nat. Commun. 15:343](https://doi.org/10.1038/s41467-023-44609-w).
- Boers, Ghil and Stocker 2022 [V] reviewed theoretical and paleoclimatic evidence for abrupt transitions — [ERL 17:093006](https://doi.org/10.1088/1748-9326/ac8944).

##### 1.2 Atlantic Meridional Overturning Circulation
- **Fingerprint.** Caesar et al. 2018 [V] inferred an AMOC weakening of "about 3 ± 1 sverdrups (around 15 per cent) since the mid-twentieth century". The evidence is a spatial and seasonal SST fingerprint (cooling in the subpolar Atlantic, warming in the Gulf Stream region) calibrated on CMIP5 and a high-resolution model, consistent with record-low RAPID values (abstract) — [Caesar et al. 2018, Nature 556:191–196](https://doi.org/10.1038/s41586-018-0006-5). Rahmstorf et al. 2015 [V] earlier claimed an "exceptional twentieth-century slowdown" from a subpolar-gyre SST index (content M) — [NCC 5:475–480](https://doi.org/10.1038/nclimate2554).
- **CSD in eight indices.** Boers 2021 [V] reported significant increases of variance and autocorrelation (via the restoring rate) in eight AMOC indices, four SST-based and four salinity-based, and interpreted them as a loss of stability over the 20th century (content M; the abstract was not retrievable) — [Boers 2021, NCC 11:680–688](https://doi.org/10.1038/s41558-021-01097-4).
- **Tipping time.** Ditlevsen & Ditlevsen 2023 [V] gave "statistical significance and data-driven estimators for the time of tipping" and estimated "a collapse of the AMOC to occur around mid-century under the current scenario of future emissions" (abstract) — [Nat. Commun. 14:4254](https://doi.org/10.1038/s41467-023-39810-w).
  - Method (content M): a Caesar-type HadISST fingerprint (subpolar SST minus twice the global mean) from 1870, fitted by maximum likelihood to a stochastic fold normal form whose control parameter drifts linearly after an onset year.
  - The widely reported 95% interval was 2025–2095 with central ~2057 (2057 per press reports [S], e.g. [EHN](https://www.ehn.org/scientists-predict-collapse-of-key-atlantic-ocean-current-by-2057)).
  - An Author Correction was published in 2025 [S, Europe PMC record; DOI 10.1038/s41467-025-63201-y]. Morr et al. 2026 (below) quote the interval as 2037–2109, which suggests the correction changed it; I could not read the correction to confirm.
- **Critique: tipping times are unreliable.** Ben-Yami, Morr, Bathiany & Boers 2024 [V] argue "the involved uncertainties are too high to robustly predict tipping times", from (i) the modelling assumptions behind extrapolation, (ii) whether a single time series represents a component, and (iii) data uncertainty and preprocessing, especially "nonstationary observational coverage and gap filling". Even if a tipping point is approaching, timing cannot be reliably extrapolated (abstract) — [Ben-Yami et al. 2024, Sci. Adv. 10:eadl4841](https://doi.org/10.1126/sciadv.adl4841).
- **Critique: four untested uncertainties.** Morr, Ben-Yami, … Rypdal, Boers 2026 [V, arXiv preprint] list four uncertainties insufficiently explored in DD23: structural (the assumed low-order model), statistical (the fit), representativeness of SST fingerprints, and data (coverage, preprocessing). With these propagated, DD23's tipping times "extend several millennia into the future" (abstract) — [arXiv:2604.20341](https://arxiv.org/abs/2604.20341).
- **Partial collapses.** Lohmann 2025 [V, arXiv] shows that an AMOC collapse may be a sequence of partial shutdowns of convection in distinct regions. Because "the system only follows the expected saddle-node normal form when very close to each tipping point", EWS trends differ between observables, and the type and time of collapse are hard to determine (abstract) — [arXiv:2512.17142](https://arxiv.org/abs/2512.17142).
- **SST fingerprints do not reconstruct the AMOC.** Terhaar, Vogt & Foukal 2025 [V] show in 24 CMIP6 models that subpolar SST anomalies "cannot robustly reconstruct the AMOC". Air–sea heat-flux anomalies north of a latitude (26.5–50°N) track the AMOC at that latitude on decadal–centennial scales. Applied to reanalyses, the decadal-mean AMOC at 26.5°N "has not weakened from 1963 to 2017" (abstract) — [Nat. Commun. 16:222](https://doi.org/10.1038/s41467-024-55297-5).
- **State dependence.** Fan et al. 2026 [V] find the AMOC–SST relationship state-dependent, with three regimes: strong AMOC with the dipole fingerprint, intermediate AMOC with amplified subpolar anomalies, and weak AMOC with muted signals. "SST-based AMOC indicators must account for state dependence" (abstract) — [Nat. Commun. 17:9416](https://doi.org/10.1038/s41467-026-76149-4).
- **No complete collapse in 34 models.** Baker et al. 2025 [V] found the AMOC "resilient to extreme greenhouse gas and North Atlantic freshwater forcings across 34 climate models". Wind-driven Southern Ocean upwelling sustains a weakened AMOC, and the Pacific overturning that emerges is too weak to balance it, so collapse "is unlikely this century" (abstract) — [Nature 638:987–994](https://doi.org/10.1038/s41586-024-08544-0).
- **Physics-based indicator.** van Westen, Kliphuis & Dijkstra 2024 [V] produced the first AMOC tipping event in CESM under quasi-equilibrium freshwater forcing. They propose "the minimum of the AMOC-induced freshwater transport at the southern boundary of the Atlantic" (F_ovS) as an observable warning, and find reanalyses indicate the AMOC is "on route to tipping". Classical statistical EWS on the simulated event were "sensitive to the analyzed time interval before tipping" (abstract) — [Sci. Adv. 10:eadk1189](https://doi.org/10.1126/sciadv.adk1189).
  - van Westen et al. 2025 [V] give a buoyancy-flux indicator and, across 25 models, a collapse onset "by 2063 (from 2026 to 2095 …) under … SSP2-4.5, or by 2055 (from 2023 to 2076 …) under … SSP5-8.5" (abstract) — [JGR Oceans 130:e2025JC022651](https://doi.org/10.1029/2025JC022651).
  - Smolders et al. 2025 [V] found that the subpolar SST fingerprint index gives no warning in the CESM quasi-equilibrium simulation, and that salinity near the southern boundary is better (search-engine summary [S] of the paper) — [GRL 52:e2025GL116242](https://doi.org/10.1029/2025GL116242).
  - Lineage: the idea that the overturning's freshwater transport (M_ov) indicates bistability goes back to Rahmstorf 1996 [V] (content M) — [Clim. Dyn. 12:799–811](https://doi.org/10.1007/s003820050144).
- **Separating noise from destabilisation.** Hallali, Myrvoll-Nilsen & Franzke 2025 [V] fit a hierarchical Bayesian AR(2) model with time-dependent parameters, "so that statistical changes from increased external variability and destabilization" are separated. On a subpolar-gyre/global-mean fingerprint they still find significant EWS. Code is distributed as an extension of the R package INLA.ews (abstract) — [NPG 32:383–395](https://doi.org/10.5194/npg-32-383-2025).
- **Observations and models reconciled.** Shin et al. 2025 [V, arXiv] identify a CSD-based decline of stability in the *eastern* subpolar North Atlantic in both observations and ESMs. It is consistent with the freshwater-convergence indicator, and the observed signal is reproduced by models only under warming beyond the Paris goal, "suggesting that AMOC stability is overestimated in ESMs" (abstract) — [arXiv:2503.22111](https://arxiv.org/abs/2503.22111).
- **Millennial AMV reconstruction.** Michel et al. 2022 [V] built 312 reconstructions of Atlantic multidecadal variability over the last millennium (best: random forest). In the best one, "memory in variations … have strongly increased recently", which they offer as a potential EWS (abstract) — [Nat. Commun. 13:5176](https://doi.org/10.1038/s41467-022-32704-3).
- **Rate-induced tipping.** Lohmann & Ditlevsen 2021 [V]: the rate of ice-melt forcing can tip the overturning in a global ocean model before the equilibrium threshold, which limits predictability (content M) — [PNAS 118:e2017989118](https://doi.org/10.1073/pnas.2017989118).
- **Direct observations.** The RAPID array at 26°N (2004–2024) [V, provider page] is too short for CSD. Moat et al. 2020 [V] — [Ocean Sci. 16:863–874](https://doi.org/10.5194/os-16-863-2020). Worthington et al. 2021, "A 30-year reconstruction … shows no decline" [S: only the review-comment record found; Ocean Sci. 17:285–299 is M] — [discussion record](https://doi.org/10.5194/os-2020-71-rc1). Kilbourne et al. 2022 [V], "Atlantic circulation change still uncertain" — [Nat. Geosci. 15:165–167](https://doi.org/10.1038/s41561-022-00896-4); with Caesar et al.'s reply [V] — [Nat. Geosci. 15:168–170](https://doi.org/10.1038/s41561-022-00897-3).
- **Model-based EWS and hysteresis.** Boulton, Ritchie & Lenton 2014 [V] found EWS of AMOC collapse in a fully coupled model (FAMOUS; content M) — [Nat. Commun. 5:5752](https://doi.org/10.1038/ncomms6752). Rahmstorf et al. 2005 [V] compared hysteresis across 11 models of intermediate complexity (content M) — [GRL 32, art. 2005GL023655](https://doi.org/10.1029/2005GL023655).

##### 1.3 Greenland and West Antarctic Ice Sheets
- Boers & Rypdal 2021 [V] analysed long melt-rate and ice-sheet-height reconstructions from the central-western Greenland Ice Sheet (the Jakobshavn basin; content M) with model simulations. They "reveal significant early-warning signals (EWS) indicating that the central-western GrIS is close to a critical transition" and attribute the destabilisation to the melt–elevation feedback (abstract) — [PNAS 118:e2024192118](https://doi.org/10.1073/pnas.2024192118).
- Robinson, Calov & Ganopolski 2012 [V]: multistability of the Greenland ice sheet, with a threshold of ~0.8–3.2 °C, best estimate ~1.6 °C (content M) — [NCC 2:429–432](https://doi.org/10.1038/nclimate1449).
- Garbe et al. 2020 [V]: hysteresis of the Antarctic ice sheet in PISM, with thresholds for West Antarctica near 2 °C (content M) — [Nature 585:538–544](https://doi.org/10.1038/s41586-020-2727-5).
- Rosier et al. [V, EarthArXiv preprint 2020; The Cryosphere 2021 is M]: three tipping points of Pine Island Glacier in a flowline/ice-sheet model, with EWS in model output (content M) — [preprint](https://doi.org/10.31223/osf.io/y95r6).

##### 1.4 The Amazon and Global Vegetation Resilience
- Boulton, Lenton & Boers 2022 [V] applied lag-1 autocorrelation and related indicators to "remotely sensed vegetation data with a focus on vegetation optical depth (1991–2016)". They found "more than three-quarters of the Amazon rainforest has been losing resilience since the early 2000s", faster where rainfall is lower and nearer human activity (abstract) — [NCC 12:271–278](https://doi.org/10.1038/s41558-022-01287-8).
- Smith, Traxl & Boers 2022 [V] confirmed empirically that recovery rates from large perturbations are "closely approximated from internal vegetation variability" (theory: AR(1) ↔ restoring rate). Using two independent satellite records (VOD and NDVI; content M for the product names), they quantified resilience globally for 1992–2017: increasing long-term in the tropics and decreasing at higher latitudes, but with a shift towards global decline since the early 2000s, especially in the equatorial rainforest (abstract) — [NCC 12:477–484](https://doi.org/10.1038/s41558-022-01352-2).
- **Reliability critique.** Smith & Boers 2023 [V] systematically compared datasets, preprocessing and estimators. They found resilience estimation from *optical* vegetation data "broadly problematic in dense tropical and high-latitude boreal forests, regardless of the vegetation index chosen", but reliable in low-biomass mid-latitudes, with net losses especially in Africa, Australia and central Asia (abstract) — [Nat. Ecol. Evol. 7:1799–1808](https://doi.org/10.1038/s41559-023-02194-7). Smith & Boers 2023b [V]: resilience is greater where water availability is higher and lower where inter-annual precipitation variability is greater (abstract) — [Nat. Commun. 14:498](https://doi.org/10.1038/s41467-023-36207-7).
- Forzieri et al. 2022 [V] used satellite vegetation indices with machine learning for 2000–2020. They found declining resilience (CSD indicators) in tropical, arid and temperate forests and increasing on average in boreal forests; "approximately 23% of intact undisturbed forests … have already reached a critical threshold" (abstract) — [Nature 608:534–539](https://doi.org/10.1038/s41586-022-04959-9).
- Verbesselt et al. 2016 [V]: temporal autocorrelation of remotely sensed tropical forest greenness as a resilience indicator, higher near rainfall thresholds and before mortality (content M) — [NCC 6:1028–1031](https://doi.org/10.1038/nclimate3108).
- **Tree-cover bimodality.** Hirota et al. 2011 [V] found "three distinct attractors: forest, savanna, and a treeless state" in tree-cover data for Africa, Australia and South America, whose resilience varies with precipitation (abstract) — [Science 334:232–235](https://doi.org/10.1126/science.1210657). Staver, Archibald & Levin 2011 [V] found tree cover bimodal at 1000–2500 mm with mild seasonality, where "only fire differentiates between savanna and forest" (abstract) — [Science 334:230–232](https://doi.org/10.1126/science.1210465).
  - Critique (content M, record M): Hanan et al. 2014 (Global Ecol. Biogeogr., "is the CART pulling the horse?") argued the bimodality may partly be an artefact of the regression-tree algorithm in the MODIS VCF tree-cover product.
- Flores et al. 2024 [V] reviewed five drivers of water stress and their potential thresholds, and estimated that "by 2050, 10% to 47% of Amazonian forests will be exposed to compounding disturbances that may trigger unexpected ecosystem transitions" (abstract) — [Nature 626:555–564](https://doi.org/10.1038/s41586-023-06970-0).
- **Data product.** VODCA, a global long-term microwave VOD climate archive (Moesinger et al. 2020 [V]) — [ESSD 12:177–196](https://doi.org/10.5194/essd-12-177-2020); VODCA v2 (Zotta et al. 2024 [V]) — ESSD 16:4573 (record matched; DOI string 10.5194/essd-16-4573-2024 inferred).

##### 1.5 Monsoons
- Zickfeld et al. 2005 [V] and Levermann et al. 2009 [V] proposed a moisture-advection feedback giving a threshold for monsoon circulations (content M) — [GRL 32, art. 2005GL022771](https://doi.org/10.1029/2005GL022771); [PNAS 106:20572–20577](https://doi.org/10.1073/pnas.0901414106).
- Critique: Boos & Storelvmo 2016 [V record] found a near-linear response of mean monsoon strength to a broad range of radiative forcings in GCMs, against a tipping threshold (content M) — PNAS 113:1510–1515 (DOI string from memory: [10.1073/pnas.1517143113](https://doi.org/10.1073/pnas.1517143113)).
- South America: Boers et al. 2017 [V] proposed a deforestation-induced tipping point of the South American monsoon (content M) — [Sci. Rep. 7:41489](https://doi.org/10.1038/srep41489). Bochow & Boers 2023 [V] reported CSD in South American monsoon rainfall, "approaching a critical transition in response to deforestation" (title; content M) — [Sci. Adv. 9:eadd9973](https://doi.org/10.1126/sciadv.add9973).
- GTPR 2025: Indian summer monsoon — "limited evidence for tipping dynamics", although recent modelling supports them (provider page, §1.1).

##### 1.6 Permafrost and Coral Reefs
- Coral reefs: GTPR 2025 places the threshold at 1.2 °C (range 1–1.5 °C [S]) and judges it crossed (§1.1). Mumby, Hastings & Edwards 2007 [V] showed thresholds and hysteresis in Caribbean reefs as a function of grazing by parrotfish, in a model (content M) — [Nature 450:98–101](https://doi.org/10.1038/nature06252). Hughes et al. 2018 [S, record via a Faculty Opinions entry] documented the shrinking interval between mass bleachings (content M) — Science 359:80–83 [M]. I found no statistical-EWS study of reef state that would serve as a test case; reef "tipping" is argued from thresholds of heat stress, not from slowing down.
- Permafrost: GTPR 2025 places regional tipping "around 1.5 °C" with medium confidence; Armstrong McKay 2022 lists abrupt thaw as a regional impact element (content M). I found no observational EWS study of permafrost in this session.

##### 1.7 Sea Ice and Why It Is Thought Not to Tip
- Eisenman & Wettlaufer 2009 [V]: in a single-column model, summer ice loss shows no bifurcation (it is reversible), but the loss of *winter* ice, from seasonal to perennially ice-free, can be a saddle-node (content M) — [PNAS 106:28–32](https://doi.org/10.1073/pnas.0806887106). Eisenman 2012 [V] analysed the factors controlling the bifurcation structure (content M) — [JGR 117, art. 2011JD016164](https://doi.org/10.1029/2011JD016164).
- Wagner & Eisenman 2015 [V]: the bistability of simple sea-ice models disappears when meridional heat transport and the seasonal cycle are represented together, so complexity removes the tipping point (content M) — [J. Climate 28:3998–4014](https://doi.org/10.1175/JCLI-D-14-00654.1).
- Tietsche et al. 2011 [V]: artificially removed summer ice recovers within about two years in a GCM (content M) — [GRL 38, art. 2010GL045698](https://doi.org/10.1029/2010GL045698).
- Notz & Stroeve 2016 [V]: September Arctic ice area falls linearly with cumulative CO₂ emissions, about 3 m² per tonne (content M) — [Science 354:747–750](https://doi.org/10.1126/science.aag2345).
- Bathiany et al. 2016 [V]: abrupt *winter* sea-ice loss occurs in some CMIP5 models at high warming, but as a loss of the seasonally ice-covered area by smooth processes rather than by a bifurcation (content M) — [J. Climate 29:2703–2719](https://doi.org/10.1175/JCLI-D-15-0466.1). Hankel & Tziperman 2021 [V] examined atmospheric feedbacks in abrupt winter loss (content M) — [J. Climate 34:4435–4447](https://doi.org/10.1175/JCLI-D-20-0558.1).
- Livina & Lenton 2013 [V] found an abrupt, persistent increase in the amplitude of the seasonal cycle of satellite Arctic ice area in 2007. They had first read it as a bifurcation (unimodal to multimodal residuals), but "this interpretation is clearly sensitive to how the seasonal cycle is removed", "there was no robust early warning signal of critical slowing down", and they call it a "(non-bifurcation) 'tipping point'" (abstract) — [The Cryosphere 7:275–286](https://doi.org/10.5194/tc-7-275-2013).
- GTPR 2025: Arctic summer sea ice "unlikely to reach tipping points"; Armstrong McKay 2022 places Arctic winter sea ice at ~6.3 °C (content M).

##### 1.8 The Sahara Greening and the African Humid Period
- Claussen et al. 1999 [V]: an abrupt mid-Holocene collapse of Saharan vegetation in an Earth-system model of intermediate complexity (CLIMBER-2), driven by slow insolation change through a vegetation–precipitation feedback (content M) — [GRL 26:2037–2040](https://doi.org/10.1029/1999GL900494). Brovkin et al. 1998 [V]: a conceptual model of multiple equilibria in the Sahara/Sahel atmosphere–vegetation system (content M) — [JGR 103:31613–31624](https://doi.org/10.1029/1998JD200006).
- deMenocal et al. 2000 [V]: abrupt onset and termination of the African Humid Period in a marine dust record off West Africa (content M) — [QSR 19:347–361](https://doi.org/10.1016/S0277-3791%2899%2900081-5).
- Controversy: Kröpelin et al. 2008 (Science 320:765 [M]) read a gradual ecological succession from the Lake Yoa (Chad) record. Brovkin and colleagues' comment [V] contested it — [Science 322:1326](https://doi.org/10.1126/science.1163381). Abruptness depends on the archive and location (content M).
- Dakos et al. 2008 included North African desertification among its eight events (content M; see §1.9).

##### 1.9 Past Transitions and Dansgaard–Oeschger Events
- Dakos et al. 2008 [S, record via OpenAlex] analysed "eight ancient abrupt climate shifts" and found them "all preceded by a characteristic slowing down … measured as increased autocorrelation" (abstract) — [PNAS 105 (2008); pages M](https://doi.org/10.1073/pnas.0802430105). The events were the end of greenhouse Earth (Eocene–Oligocene), glacial terminations in Vostok, the end of the Younger Dryas and North African desertification (content M). For the Eocene–Oligocene record see Coxall et al. 2005 [V] — [Nature 433:53–57](https://doi.org/10.1038/nature03135) — and the Cenozoic compilation of Westerhold et al. 2020 [V] — [Science 369:1383–1387](https://doi.org/10.1126/science.aba6853).
- Livina & Lenton 2007 [V] introduced a DFA-based "propagator" indicator of incipient bifurcation, applied to Greenland paleotemperature at the end of the last glaciation (content M) — [GRL 34, art. 2006GL028672](https://doi.org/10.1029/2006GL028672). Lenton et al. 2012 [V record] compared ACF, DFA and power-spectrum methods for robustness (content M) — Phil. Trans. R. Soc. A 370:1185–1204 (DOI string from memory: [10.1098/rsta.2011.0304](https://doi.org/10.1098/rsta.2011.0304)).
- Ditlevsen & Johnsen 2010 [V], "Tipping points: Early warning and wishful thinking": in the NGRIP record, neither variance nor autocorrelation increases consistently before DO events, which is consistent with noise-induced transitions. They insist that *both* indicators must rise for a bifurcation, since a noise increase raises variance alone (content M) — [GRL 37, art. 2010GL044486](https://doi.org/10.1029/2010GL044486).
- Rypdal 2016 [V] looked for warnings in the high-frequency (decadal) fluctuation levels before Greenland interstadial onsets and the Younger Dryas–Preboreal transition (title; for content see Boers 2018's abstract below) — [J. Climate 29:4047–4056](https://doi.org/10.1175/JCLI-D-15-0828.1).
- Boers 2018 [V] derived a 5-year-resolution series from the raw NGRIP record and found "significant numbers of EWS in terms of variance and autocorrelation increases in the decadal-scale variability", most pronounced in the 10–50-year band. He concluded that DO events are "neither directly noise-induced nor purely externally forced" (abstract) — [Nat. Commun. 9:2556](https://doi.org/10.1038/s41467-018-04881-7).
- **Re-evaluation.** Hummel, Boers & Rypdal 2025 [V] applied CSD and wavelet indicators with a systematic robustness test across several Greenland cores. They found "fewer significant EWS than previous studies"; counts were significant for some variability estimators but "not significant for most ice core records as well as for estimators of correlation times". Hence "it is not possible to constrain mechanisms underlying the DO events" (abstract) — [ESD 16:2035–2062](https://doi.org/10.5194/esd-16-2035-2025).
- Competing mechanistic readings:
  - Lohmann & Ditlevsen 2019 [V] extracted the statistical features of DO events objectively (content M: they favour noise-induced transitions in δ¹⁸O) — [Clim. Past 15:1771–1792](https://doi.org/10.5194/cp-15-1771-2019).
  - Riechers et al. 2023 found a possible bifurcation structure in dust records (search summary of Hummel et al. [S]).
  - Mitsui et al. 2023 [V, EarthArXiv] report "Statistical precursor signals for Dansgaard-Oeschger cooling transitions" (title) — [preprint](https://doi.org/10.31223/X54S90).
  - Vettoretti et al. 2022 [V]: spontaneous millennial oscillations in a coupled GCM within a window of atmospheric CO₂, i.e. self-sustained oscillation rather than forced bifurcation (content M) — [Nat. Geosci. 15:300–306](https://doi.org/10.1038/s41561-022-00920-7).
- Hennekam et al. 2020 [V]: EWS before marine anoxic events in Mediterranean sapropel records (content M) — [GRL 47:e2020GL089183](https://doi.org/10.1029/2020GL089183).
- Boulton & Lenton 2015 [V record]: slowing down of North Pacific climate variability and its implications for abrupt ecosystem change (title; content M) — PNAS 112:11496 (DOI string from memory: [10.1073/pnas.1501781112](https://doi.org/10.1073/pnas.1501781112)).
- Methodological: Boettner & Boers 2022 [V] showed that nonstationary *correlated* noise can bias standard CSD indicators, and proposed estimators that account for noise colour (content M) — [Phys. Rev. Research 4:013230](https://doi.org/10.1103/PhysRevResearch.4.013230). Bathiany et al. 2018 [V] treated abrupt change in systems with strong seasonal or periodic forcing ("an oscillating world"; content M) — [Sci. Rep. 8:5040](https://doi.org/10.1038/s41598-018-23377-4).

##### 1.10 Spatial Early Warnings in Climate Fields, and Climate Networks
- Bathiany et al. 2013 [V] detected "hotspots of atmosphere–vegetation interaction via slowing down", a stochastic approach on gridded fields (title; content M) — [ESD 4:63–78](https://doi.org/10.5194/esd-4-63-2013). Bathiany, Hidding & Scheffer 2020 [V] used edge detection on gridded model output to find abrupt and extreme climate events, an objective spatial screening (content M) — [J. Climate 33:6399–6421](https://doi.org/10.1175/JCLI-D-19-0449.1).
- Spatial indicators from ecology carry over to fields: spatial variance and skewness (Guttal & Jayaprakash 2009 [V] — [Theor. Ecol. 2:3–12](https://doi.org/10.1007/s12080-008-0033-1)), spatial correlation (Dakos et al. 2010 [V] — [Theor. Ecol. 3:163–174](https://doi.org/10.1007/s12080-009-0060-6)), and the methods review of Kéfi et al. 2014 [V] — [PLoS ONE 9:e92097](https://doi.org/10.1371/journal.pone.0092097).
- **Climate networks for El Niño.** Ludescher et al. 2013 [S, record via OpenAlex; its Correction is V — [PNAS 110:19172](https://doi.org/10.1073/pnas.1317354110)] measured cooperative links between the El Niño basin and the rest of the Pacific that build up in the year before an event. They claimed a 12-month forecast with "hit rates above 0.5, whereas false-alarm rates are below 0.1" (abstract) — [PNAS 110 (2013); pages M](https://doi.org/10.1073/pnas.1309353110).
  - Ludescher et al. 2014 [V] announced (from September 2013) "the return of El Niño in late 2014 with a 3-in-4 likelihood" (abstract) — [PNAS 111:2064–2066](https://doi.org/10.1073/pnas.1323058111).
  - Ludescher et al. 2021 [V] review network forecasting of climate phenomena — [PNAS 118:e1922872118](https://doi.org/10.1073/pnas.1922872118).
  - Bunde, Ludescher & Schellnhuber 2024 [S] evaluated their real-time forecasts for 2011–2022: p ≈ 0.005 against random guessing, onsets of 2014–16 and 2018–19 predicted, one false alarm in 2019 (search summary) — [Theor. Appl. Climatol.](https://doi.org/10.1007/s00704-024-05035-0).
  - Foundations: Tsonis & Swanson 2008 [V] — [PRL 100:228502](https://doi.org/10.1103/PhysRevLett.100.228502); Donges et al. 2009 [V] — [EPJ ST 174:157–179](https://doi.org/10.1140/epjst/e2009-01098-2); percolation framework, Meng et al. 2017 [V] — [Chaos 27:035807](https://doi.org/10.1063/1.4975766); review, Fan et al. 2021 [V] — [Phys. Rep. 896:1–84](https://doi.org/10.1016/j.physrep.2020.09.005).
  - An informal critique (Baez's Azimuth project blog) re-examined the 2014 paper's false alarms and missed events (search summary [S], not peer-reviewed) — [Azimuth notes](https://math.ucr.edu/home/baez/ecological/el_nino/El_Nino_project_part_3.txt).

#### Inferences
- Each claimed observational warning (AMOC, Greenland, Amazon) rests on a *proxy* of the system state: an SST fingerprint, a melt reconstruction, an optical or microwave vegetation index. The strongest critiques attack the proxy (Terhaar 2025; Fan 2026; Smith & Boers 2023), not the statistics. A detector library should therefore attach to every result the observable used and the evidence that it represents the state variable.
- Two questions now stand apart in the literature. (a) Is the restoring rate declining? This is answerable, with care. (b) When will the system tip? Ben-Yami 2024 and Morr 2026 show this is not answerable from historical data, because propagated uncertainty spans millennia. A library should report (a) and refuse (b), or report (b) only as an interval conditional on a stated model.
- "Physics-based" indicators (F_ovS, buoyancy flux) are model-specific diagnostics, not generic detectors. In a climate domain they belong beside the generic statistics, as observables computed from fields.
- The DO literature has swung from "no warning" (2010) to "warning" (2018) to "inconclusive" (2025). It is the cleanest example of how indicator choice, resolution, detrending and the count of events determine the answer. As an honest demonstration it should be reproduced as a robustness grid, not as a single result.
- Sea ice is the canonical case where simple models (single column, zero-dimensional EBM) have a bifurcation that disappears in fuller models (Wagner & Eisenman 2015). A test suite should include such a model pair, so that a detector is seen to give no warning where the fuller model has no bifurcation.

#### Gaps
- I could not read the 2025 Author Correction to Ditlevsen & Ditlevsen 2023, so I cannot confirm whether the published interval changed from 2025–2095 to 2037–2109.
- No observational EWS studies were found this session for permafrost, coral reefs (statistical indicators), boreal forest dieback or mountain glaciers. Assessments for these rest on thresholds and process models.
- I found no independent peer-reviewed critique of the climate-network El Niño forecasts in this session, only the authors' own evaluation and a blog.
- The GTPR 2023 record and numbers were not verified; the GTPR 2025 ranges, except for coral reefs, are not given on the summary page.
- The Boers 2021 abstract and the exact indices used were not retrieved, so the details are from memory.

### 2. Climate Test Models with Known Bifurcations

#### Takeaway
Two low-order climate models have closed-form folds and are used throughout the literature: the Budyko–Sellers energy-balance model with ice–albedo feedback (snowball hysteresis), and the Stommel/Cessi two-box model (AMOC hysteresis in freshwater forcing). Both should be implemented in two forms. The smooth form shows critical slowing down; the piecewise/absolute-value form has non-smooth "folds" with *no* slowing down before them, which makes it a ready-made negative control. Welander's oscillator and DO models (α-stable double wells, relaxation oscillators, CO₂-windowed oscillations) supply cases of transitions without bifurcation. A diffusive one-dimensional EBM (North) and the Wagner–Eisenman sea-ice EBM supply spatial fields, and a "bifurcation removed by complexity" pair.

#### Cited Findings

##### 2.1 Energy-Balance Models with Ice–Albedo Feedback
- Records: Budyko 1969 [V] — [Tellus 21:611–619](https://doi.org/10.1111/j.2153-3490.1969.tb00466.x); Sellers 1969 [V] — [J. Appl. Meteor. 8:392–400](https://doi.org/10.1175/1520-0450%281969%29008%3C0392%3AAGCMBO%3E2.0.CO%3B2); North 1975 [V] — [J. Atmos. Sci. 32:2033–2043](https://doi.org/10.1175/1520-0469%281975%29032%3C2033%3ATOEBCM%3E2.0.CO%3B2); North, Cahalan & Coakley 1981 [V] — [Rev. Geophys. 19:91–121](https://doi.org/10.1029/RG019i001p00091); Ghil 1976 [V] — [J. Atmos. Sci. 33:3–20](https://doi.org/10.1175/1520-0469%281976%29033%3C0003%3ACSFAST%3E2.0.CO%3B2). Snowball Earth as geological motivation: Hoffman et al. 1998 [V] — [Science 281:1342–1346](https://doi.org/10.1126/science.281.5381.1342). Edge states and global instabilities: Lucarini & Bódai 2017 [V] — [Nonlinearity 30:R32–R66](https://doi.org/10.1088/1361-6544/aa6b11).
- **Zero-dimensional form (eq. M).** C dT/dt = Q(1 − α(T)) − (A + B T), with Q = S₀/4 ≈ 340 W m⁻². North et al. 1981 give the outgoing-longwave fit A ≈ 203.3 W m⁻², B ≈ 2.09 W m⁻² K⁻¹ with T in °C (values M). The albedo α(T) equals α_i (ice) for T ≤ T_i and α_w (open surface) for T ≥ T_w, linear in between; or it is smooth, e.g. α(T) = α_i − (α_i − α_w)·½[1 + tanh((T − T₀)/ΔT)].
  - **Closed-form folds for the piecewise-linear albedo (derived here).** The pure branches have T* = (Q(1 − α) − A)/B. The cold branch exists iff Q ≤ (A + B T_i)/(1 − α_i); the warm branch exists iff Q ≥ (A + B T_w)/(1 − α_w). The ramp equilibrium is unstable iff Q(α_i − α_w)/(T_w − T_i) > B, and then the system is bistable for (A + B T_w)/(1 − α_w) ≤ Q ≤ (A + B T_i)/(1 − α_i).
  - The folds are *non-smooth*: on each stable branch the linearised rate is exactly −B/C until the branch meets the kink. The relaxation time therefore stays constant and **there is no critical slowing down before this fold**. With the tanh albedo the folds are smooth (solve f = f′ = 0 numerically) and the restoring rate → 0 at the fold.
  - This matches Hastings & Wysham's warning that systems without smooth potentials give no warning — [Ecol. Lett. 13:464–472](https://doi.org/10.1111/j.1461-0248.2010.01439.x) [V].
- **One-dimensional diffusive form (North 1975; eq. M).** C ∂T/∂t = ∂/∂x[D(1 − x²) ∂T/∂x] + Q s(x) a(x, x_s) − (A + B T), with x = sin(latitude), s(x) = 1 + s₂P₂(x), s₂ ≈ −0.482 and coalbedo a = a_w (≈0.68) equatorward of the ice edge x_s, a_i (≈0.38) poleward (values M). The ice edge sits where T = −10 °C. The model has a "small ice-cap instability" and a large-ice-cap fold to snowball, with hysteresis in Q; North 1975 gives analytic solutions in Legendre polynomials (content M).
  - This is the natural *field* test model, a temperature field on the sphere: spatial indicators can be tested on it, since the ice edge and its neighbourhood slow down first.
  - Kooloth et al. 2024 [V, arXiv] study control of an idealised diffusive EBM that "has two tipping points due to strong co-albedo feedback", one a "'small icecap' instability responsible for a rapid transition to an ice-free climate state" (abstract) — [arXiv:2407.17357](https://arxiv.org/abs/2407.17357).
- **Benzi–Nicolis form used for stochastic resonance (verified equations).** Berglund & Gentz [V, arXiv physics/0111110; published in Stochastics and Dynamics 2002, M] give the following, citing Benzi et al. (records M):
  - eq. (3.1): c dT/dt = Q(1 + A cos ωt)(1 − α(T)) − E(T);
  - eq. (3.2): γ(T) = Q(1 − α(T))/E(T) − 1 = β(1 − T/T₁)(1 − T/T₂)(1 − T/T₃), with T₁ = 278.6 K, T₂ = 283.3 K (unstable), T₃ = 288.6 K;
  - relaxation time τ ≈ 8 years (eq. 3.4); modulation amplitude A ≈ 5×10⁻⁴ with a period of 92,000 years;
  - eq. (3.5), the dimensionless form: dx/dt = ε⁻¹[−x(x − x₁)(x − x₃)(1 + A cos 2πt) + K cos 2πt], with x₁ ≈ −0.94, x₃ ≈ 1.06, ε ≈ 1.8×10⁻⁴ and K ≈ 0.12;
  - the Ginzburg–Landau potential V = x⁴/4 − μx²/2 − λx has two wells iff 27λ² < 4μ³ (eq. 1.8), so for μ = 1, λ_c = 2/(3√3) ≈ 0.38 (text after eq. 3.7).

  Source: [Berglund & Gentz, arXiv:physics/0111110](https://arxiv.org/abs/physics/0111110).
- **Sea-ice EBMs.** Eisenman & Wettlaufer 2009 [V] (single column) and Wagner & Eisenman 2015 [V] (diffusive EBM with seasonal cycle and sea-ice thermodynamics), in which bistability vanishes as the model becomes more complete (content M); citations in §1.7.

##### 2.2 Stommel's Two-Box Model and Cessi's Reduction
- Records: Stommel 1961 [V] — [Tellus 13 (2)](https://doi.org/10.3402/tellusb.v13i2.12985) (pages 224–230, M); Cessi 1994 [V] — [J. Phys. Oceanogr. 24:1911–1920](https://doi.org/10.1175/1520-0485%281994%29024%3C1911%3AASBMOS%3E2.0.CO%3B2); Rahmstorf 1996 [V] (§1.2); Rahmstorf et al. 2005 hysteresis intercomparison [V] (§1.2); Wood et al. 2019 [V], a five-box model whose thresholds are expressed in observable quantities and calibrated on a GCM (content M) — [Clim. Dyn. 53:6815–6834](https://doi.org/10.1007/s00382-019-04956-1).
- **Stommel–Cessi equations (verified via Berglund & Gentz, eq. numbers theirs).**
  - (4.1) dΔT/dt = −(1/τ_r)(ΔT − θ) − Q(Δρ)ΔT;
  - (4.2) dΔS/dt = F(t)/(H S₀) − Q(Δρ)ΔS;
  - (4.3) Δρ = α_S ΔS − α_T ΔT;
  - (4.4) Cessi's exchange law Q(Δρ) = 1/τ_d + (q/V)Δρ². Stommel used |Δρ| in place of Δρ², which "leads to a singularity";
  - (4.5) in y = α_S ΔS/(α_T θ), z = ΔT/θ, with time in units of τ_d: ẏ = p(t) − y[1 + η²(y − z)²] and ε₀ż = −(z − 1) − ε₀z[1 + η²(y − z)²]. Here ε₀ = τ_r/τ_d and η² = τ_d(α_T θ)² q/V. Cessi's estimates are η² ≈ 7.5, τ_r ≈ 25 days and τ_d ≈ 219 years, so ε₀ ≈ 3×10⁻⁴ (a slow–fast system);
  - (4.6) on the slow manifold z = 1 + O(ε₀): ẏ = −y[1 + η²(y − 1)²] + p(t);
  - (4.8) the Ginzburg–Landau form, with μ = η²/3 − 1 and λ = η(p̄ − 2/3 − 2η²/27). The double well exists iff λ² < 4μ³/27, which for η² = 7.5 means p̄ ∈ [0.96, 1.48]; the potential is symmetric at p̄₀ ≈ 1.22.

  Source: [Berglund & Gentz, arXiv:physics/0111110, §4](https://arxiv.org/abs/physics/0111110).
- **Closed-form folds of the reduced model (derived here, consistent with eq. 4.8).** Steady states satisfy p = y[1 + η²(y − 1)²]. Setting dp/dy = 0 gives 3η²y² − 4η²y + (1 + η²) = 0, so y± = 2/3 ± √(η² − 3)/(3η). Bistability requires η² > 3; the cusp is at η² = 3, equivalently μ = 0. For η² = 7.5, y± ≈ 0.925 and 0.408, giving p_fold ≈ 0.964 and 1.480, which reproduces the published interval.
- **Stommel's original non-dimensional form (eq. M).** dT/dt = η₁ − T(1 + |T − S|), dS/dt = η₂ − S(η₃ + |T − S|). The absolute value makes the vector field non-smooth on T = S, so it has the same non-smooth-fold caveat as §2.1.
- **Driving schedules in published studies.** Berglund & Gentz use a periodic p̄(t) plus white noise, with hysteresis when the amplitude exceeds the threshold. Model studies drive a quasi-equilibrium linear ramp of freshwater forcing (van Westen 2024, abstract) or rates of ice melt (Lohmann & Ditlevsen 2021, content M). DD23 fits a linear ramp of the control parameter starting at an unknown onset year (content M).

##### 2.3 Welander's Oscillator and Models of Dansgaard–Oeschger Events
- Welander 1982 [V], a heat–salt oscillator with convective flip-flop — [Dyn. Atmos. Oceans 6:233–242](https://doi.org/10.1016/0377-0265%2882%2990030-6). Form (eq. M):
  - dT/dt = k_T(T_A − T) − κ(ρ)T and dS/dt = k_S(S_A − S) − κ(ρ)S;
  - ρ = −αT + γS, with κ = κ₀ for ρ ≤ ε and κ₁ ≫ κ₀ for ρ > ε, or smoothed by a tanh;
  - it gives self-sustained relaxation oscillations for suitable k_T ≠ k_S. It is a test case for oscillations and canards rather than a fold.
- Ditlevsen 1999 [V]: millennial switches in an ice-core record modelled as a double well driven by α-stable (Lévy) noise, i.e. noise-induced jumps without bifurcation (content M) — [GRL 26:1441–1444](https://doi.org/10.1029/1999GL900252).
- Cimatoribus et al. 2013 [V]: DO events as bifurcation points of an ocean box model (content M) — [Clim. Past 9:323–333](https://doi.org/10.5194/cp-9-323-2013).
- Kwasniok 2013 [V]: predicting transitions from time series by nonstationary probability-density modelling, applied to DO-type data (content M) — [PRE 88:052917](https://doi.org/10.1103/PhysRevE.88.052917).
- Vettoretti et al. 2022 [V]: spontaneous millennial oscillations within a CO₂ window in a GCM (content M), i.e. a limit cycle, for which fold-type EWS are not expected (§1.9).
- Ashwin et al. 2012 [V record]: a taxonomy of bifurcation-, noise- and rate-induced tipping with climate examples (content M) — Phil. Trans. R. Soc. A 370:1166–1184 (DOI string from memory: [10.1098/rsta.2011.0306](https://doi.org/10.1098/rsta.2011.0306)).

##### 2.4 Vegetation–Precipitation Models
- Brovkin et al. 1998 [V] (content M): precipitation increases with vegetation cover, P = P₀ + kV, and equilibrium vegetation is a sigmoid V*(P). Where the graphs cross three times there are green and desert states, and slow insolation forcing moves P₀ through a fold (equations M). Claussen 1999 [V] reproduces the abrupt mid-Holocene collapse in CLIMBER-2 (content M). Citations in §1.8.
- Forest–savanna–grassland models with fire feedback (Staver & Levin 2012, Am. Nat. [M]) underlie the tree-cover bimodality literature (§1.4).
- Klausmeier and Rietkerk dryland models (§3) are the spatial counterparts, with Turing patterns.

#### Inferences
- **Priority models.** The Stommel–Cessi reduced model (one dimension, closed-form folds, verified equations and parameters) and the zero-dimensional EBM are the cheapest test models with exact bifurcation values. A detector can then be scored against the true distance to the fold, and the true restoring rate λ(t) = ∂f/∂y can be computed and compared with AR(1)-based estimates.
- **Negative controls.** Each smooth model should come with its non-smooth sibling: the piecewise-linear albedo, Stommel's |Δρ|, Welander's flip-flop κ. There the fold or switch arrives without slowing down. Hastings & Wysham's theorem-like warning becomes a unit test: a detector that "warns" in these cases has a false-positive mechanism (detrending, window, noise colour).
- **Field model.** The diffusive North EBM is the natural first *field* model for a climate domain. Its state is a temperature field, its bifurcation (ice-cap instability) is known semi-analytically, and spatial indicators (variance near the ice edge, spatial autocorrelation, leading eigenvalue of the covariance) can be tested on it.
- **Driving.** Studies drive by slow linear ramps of the control parameter, periodic forcing, or ramps at different rates (rate tipping). A test harness should support all three, and should report the ratio of forcing rate to relaxation rate, since the theory requires timescale separation (Dablander 2022, §4).

#### Gaps
- The Welander, Brovkin and North equations are given from memory; their parameter tables should be taken from the original papers before implementation. North 1975 and Cessi 1994 were verified as records only.
- The fold values for the smooth tanh-albedo EBM have no closed form; they must be computed by continuation.
- The published DD23 model equation (onset year, drift form) is described from memory.

### 3. Ecological Applications and Their Empirical Record

#### Takeaway
Controlled experiments give clear positive results: whole lakes (Carpenter 2011; Pace 2017 reversed a bloom when indicators crossed preset bounds), Daphnia (Drake & Griffen 2010), yeast (Dai 2012) and cyanobacteria (Veraart 2012). Systematic tests on observational time series do not. Burthe et al. 2016 found true positives in only 9–13% of cases and more false positives than false negatives. Gsell et al. 2016 found low agreement among indicators. O'Brien et al. 2023 found few critical transitions in global lake data and most indicators no better than chance. Spatial vegetation patterns were proposed as warnings (Rietkerk 2004; Kéfi 2007), but may instead signal resilience, because patterns can evade tipping (Rietkerk 2021). Test models are standard and simple: logistic growth with type-III grazing (May, Noy-Meir, Ludwig's budworm), Carpenter's lake phosphorus model, and the Klausmeier and Rietkerk PDEs.

#### Cited Findings

##### 3.1 Theory and Reviews
- Scheffer et al. 2001 [V], catastrophic shifts in ecosystems — [Nature 413:591–596](https://doi.org/10.1038/35098000); Scheffer et al. 2009 [V], early-warning signals for critical transitions — [Nature 461:53–59](https://doi.org/10.1038/nature08227); Carpenter & Brock 2006 [V], rising variance as a leading indicator, on a lake phosphorus model (content M) — [Ecol. Lett. 9:311–318](https://doi.org/10.1111/j.1461-0248.2005.00877.x).
- Dakos et al. 2012 [V], a methods paper with simulated data: rolling-window AR(1), variance, skewness, DFA, spectral and model-based indicators on an overharvesting model (content M) — [PLoS ONE 7:e41010](https://doi.org/10.1371/journal.pone.0041010). Dakos et al. 2012 [V], robustness of variance and autocorrelation (content M) — [Ecology 93:264–271](https://doi.org/10.1890/11-0889.1). Dakos et al. 2015 [V], prospects and limitations — [Phil. Trans. R. Soc. B 370:20130263](https://doi.org/10.1098/rstb.2013.0263).
- Critiques of principle:
  - Hastings & Wysham 2010 [V]: many ecological systems lack smooth potentials and "will not show typical leading indicators of regime shifts" (abstract) — [Ecol. Lett. 13:464–472](https://doi.org/10.1111/j.1461-0248.2010.01439.x).
  - Boettiger & Hastings 2012a [V]: selecting systems *because* they had a transition (the prosecutor's fallacy) inflates false positives. They show "an elevated rate of false-positives in common warning signal statistics", recommend model-based approaches, and note that "experimental studies with replicates avoid this pitfall entirely" (abstract) — [Proc. R. Soc. B 279:4734–4739](https://doi.org/10.1098/rspb.2012.2085).
  - Boettiger & Hastings 2012b [V], limits to detection (content M: likelihood-ratio test between a stable and a destabilising OU model) — [J. R. Soc. Interface 9:2527–2539](https://doi.org/10.1098/rsif.2012.0125).
  - Hughes et al. 2013 [V] on slow, unrecognised regime shifts — [TREE 28:149–155](https://doi.org/10.1016/j.tree.2012.08.022). Ratajczak et al. 2018 [V] on inference and diagnosis of abrupt change — [TREE 33:513–526](https://doi.org/10.1016/j.tree.2018.04.013).
- Multivariate and network indicators:
  - Chen et al. 2019 [V], eigenvalues of the covariance matrix as EWS — [Sci. Rep. 9:2572](https://doi.org/10.1038/s41598-019-38961-5);
  - Weinans et al. 2021 [V], evaluating multivariate indicators — [Sci. Rep. 11:9148](https://doi.org/10.1038/s41598-021-87839-y);
  - Suweis & D'Odorico 2014 [S, OpenAlex]: in social-ecological networks the maximum element of the covariance matrix is a leading indicator, whose reliability depends more on the *sign* of interactions than on network structure (abstract) — [PLoS ONE 9:e101851](https://doi.org/10.1371/journal.pone.0101851).
- Machine learning: Bury et al. 2021 [S, OpenAlex record] trained a deep classifier on normal forms and report EWS "in 268 empirical and model time series … with much greater sensitivity and specificity than generic EWS", which also predicts the bifurcation type (abstract) — [PNAS 118:e2106140118](https://doi.org/10.1073/pnas.2106140118). O'Brien et al. 2023 found "a recent machine learning model performing poorly" on lake data (§3.4).

##### 3.2 Lakes
- Carpenter et al. 2011 [V], a whole-ecosystem experiment. Top predators were added gradually to a lake over 3 years to destabilise its food web, with an adjacent reference lake; "warning signals … were evident in the manipulated lake … more than a year before the food web transition was complete" (abstract) — [Science 332:1079–1082](https://doi.org/10.1126/science.1203672). The lakes are Peter (manipulated) and Paul (reference), North Temperate Lakes LTER/Cascade, with largemouth bass added (content M).
- Pace et al. 2017 [V] enriched an experimental lake gradually, with an unenriched and a continuously enriched reference. "When resilience indicators exceeded preset boundaries, nutrient enrichment was stopped"; pigments, oxygen and pH returned to unenriched levels, while "a large bloom occurred in the continuously enriched lake" (abstract) — [PNAS 114:352–357](https://doi.org/10.1073/pnas.1612424114). This is the only managed *prevention* by EWS found.
- Butitta et al. 2017 [V], spatial early warnings in a lake manipulation (content M) — [Ecosphere 8:e01941](https://doi.org/10.1002/ecs2.1941).
- Wang et al. 2012 [V]: "flickering", repeated switching between states, preceded a lake's eutrophic transition in a sediment diatom record (Lake Erhai, China; content M) — [Nature 492:419–422](https://doi.org/10.1038/nature11655).
- Spanbauer et al. 2014 [V], prolonged instability before a regime shift in a paleolimnological diatom record (content M) — [PLoS ONE 9:e108936](https://doi.org/10.1371/journal.pone.0108936).
- Gsell et al. 2016 [V] assessed four EWIs on long-term data from five freshwater ecosystems with well-understood transitions (competition, trophic cascade, intraguild predation). "Agreement among the four indicators was low", so "EWIs do not provide reliable and consistent signals", and "a priori knowledge of the underlying mechanisms … is necessary to identify relevant state variables" (abstract) — [PNAS 113 (2016)](https://doi.org/10.1073/pnas.1608242113).
- O'Brien et al. 2023 [V], multi-trophic plankton data from lakes worldwide (nine lakes per a preprint summary [S]). The authors "find few instances of critical transitions, with different trophic levels often expressing different forms of abrupt change". Predictive ability is "highly processing dependant, with most indicators not performing better than chance, multivariate EWSs being weakly superior to univariate, and a recent machine learning model performing poorly"; ecology "should start to move away from the concept of critical transitions" (abstract) — [Nat. Commun. 14:7942](https://doi.org/10.1038/s41467-023-43744-8).

##### 3.3 Laboratory Populations
- Drake & Griffen 2010 [V]: in Daphnia populations under deteriorating food supply, EWS (variance, autocorrelation, skewness, spatial correlation) appeared generations before extinction, with replicated controls (content M) — [Nature 467:456–459](https://doi.org/10.1038/nature09389).
- Dai et al. 2012 [V]: replicate budding-yeast populations with a cooperative-growth Allee effect. They "mapped the bifurcation diagram experimentally"; "fluctuations of population density increased in size and duration near the tipping point" (abstract) — [Science 336:1175–1177](https://doi.org/10.1126/science.1219805).
- Veraart et al. 2012 [V]: recovery rates from perturbation reflect the distance to a tipping point in a cyanobacterial culture under light stress (content M) — [Nature 481:357–359](https://doi.org/10.1038/nature10723).
- Clements et al. 2015 [V]: spatial and temporal subsampling of wild-like data "may serve to either muffle or magnify early warning signals" (abstract) — [Am. Nat. 186:50–58](https://doi.org/10.1086/681573). Clements & Ozgul 2016 [V]: trait-based EWS (body size) improve prediction (content M) — [Nat. Commun. 7:10984](https://doi.org/10.1038/ncomms10984).

##### 3.4 Long-Term Field Data and Meta-Analyses
- Burthe et al. 2016 [V] tested 126 datasets of 55 taxa in marine and freshwater systems:
  - true positives were "only 9% (16 of 170) of cases using variance, and 13% (19 of 152) … using autocorrelation";
  - false positives outnumbered false negatives (53% vs 38% for variance; 47% vs 40% for autocorrelation);
  - "43% of time series [were] categorized differently based on variance compared to autocorrelation";
  - false results occurred in every decade and at all trophic levels.

  Source (abstract): [J. Appl. Ecol. 53:666–676](https://doi.org/10.1111/1365-2664.12519).
- Litzow & Hunsicker 2016 [V], EWS, nonlinearity and signs of hysteresis in real ecosystems (content M: EWS are associated with nonlinear threshold dynamics, not with linear change) — [Ecosphere 7:e01614](https://doi.org/10.1002/ecs2.1614).
- Dakos et al. 2024 [V] reached a more positive view from the published case-study literature (§1.1); note the selection issue raised by Boettiger & Hastings 2012a.

##### 3.5 Fisheries and Marine Systems
- Frank et al. 2005 [V], a trophic cascade after the cod collapse on the Scotian Shelf (content M) — [Science 308:1621–1623](https://doi.org/10.1126/science.1113075).
- Lindegren et al. 2012 [V], a multi-method evaluation of early detection of regime shifts for management, on Baltic data (content M) — [PLoS ONE 7:e38410](https://doi.org/10.1371/journal.pone.0038410).
- Krkošek & Drake 2014 [V], signals of phase transitions in salmon dynamics (content M) — [Proc. R. Soc. B 281:20133221](https://doi.org/10.1098/rspb.2013.3221).
- Clements et al. 2017 [V]: body-size shifts and EWS precede the historic collapse of whale stocks, from whaling records (content M) — [Nat. Ecol. Evol. 1:0188](https://doi.org/10.1038/s41559-017-0188). Clements et al. 2019 [V]: "both abundance and trait-based signals are independently detectable prior to the recovery of stocks" in a trait-based model and real fisheries data (abstract) — [Nat. Commun. 10:1681](https://doi.org/10.1038/s41467-019-09684-y).

##### 3.6 Drylands and Spatial Vegetation Patterns
- Klausmeier 1999 [V] — [Science 284:1826–1828](https://doi.org/10.1126/science.284.5421.1826); HilleRisLambers et al. 2001 [V] — [Ecology 82:50–61](https://doi.org/10.1890/0012-9658%282001%29082%5B0050%3AVPFISA%5D2.0.CO%3B2); Rietkerk et al. 2002 [V] — [Am. Nat. 160:524–530](https://doi.org/10.1086/342078); Rietkerk et al. 2004 [V], self-organised patchiness and catastrophic shifts — [Science 305:1926–1929](https://doi.org/10.1126/science.1101867).
- Kéfi et al. 2007 [V]: vegetation patch-size distributions in Mediterranean drylands (Spain, Greece, Morocco; content M) follow power laws that deviate as grazing increases, proposed as an indicator of imminent desertification; a cellular automaton with facilitation (content M) — [Nature 449:213–217](https://doi.org/10.1038/nature06111).
- Berdugo et al. 2017 [V record], plant spatial patterns identify alternative multifunctionality states in global drylands (title; DOI tail M) — [Nat. Ecol. Evol. 1:0003](https://doi.org/10.1038/s41559-016-0003). Berdugo et al. 2020 [V]: across 20 attributes of global drylands, aridification caused "systemic and abrupt changes", with three phases at aridity values of 0.54, 0.7 and 0.8 (abstract) — [Science 367:787–790](https://doi.org/10.1126/science.aay5958).
- **Critique: patterns can evade tipping.** Rietkerk et al. 2021 [V] review "how spatial self-organization can aid complex systems to evade tipping points and can therefore be a signal of resilience instead" (abstract) — [Science 374:eabj0359](https://doi.org/10.1126/science.abj0359).
- Tools: Génin et al. 2018 [V], the R package spatialwarnings (spatial variance, skewness, Moran's I, spectral and patch-size indicators; content M) — [Methods Ecol. Evol. 9:2067–2075](https://doi.org/10.1111/2041-210X.13058). Eby et al. 2017 [V], spatial indicators along a savanna rainfall gradient (content M) — [Global Ecol. Biogeogr. 26:638–649](https://doi.org/10.1111/geb.12570) (DOI tail M).

##### 3.7 Forests, Savannas and Coral Reefs
- Tree-cover bimodality (Hirota 2011; Staver 2011) and its product-artefact critique: §1.4. Forest resilience from satellites (Forzieri 2022; Smith & Boers 2023): §1.4. Reef hysteresis (Mumby 2007): §1.6.

##### 3.8 Ecological Test Models
- **Logistic growth with type-III grazing or harvesting** (May 1977 [V] — [Nature 269:471–477](https://doi.org/10.1038/269471a0); Noy-Meir 1975 [V] — [J. Ecol. 63:459](https://doi.org/10.2307/2258730); Ludwig, Jones & Holling 1978 spruce budworm [V] — [J. Anim. Ecol. 47:315](https://doi.org/10.2307/3939)). Form (eq. M): dx/dt = r x(1 − x/K) − c x²/(x² + h²), with c the grazing pressure.
  - For r = 1, K = 10, h = 1 (eq. M), steady states satisfy c(x) = (1 − x/10)(x² + 1)/x. Folds solve x³ − 5x² + 5 = 0: x ≈ 4.78 gives the collapse fold c ≈ 2.604, and x ≈ 1.135 gives the recovery fold c ≈ 1.787. The system is bistable for c ∈ (1.787, 2.604) (derived here).
  - Dakos et al. 2012 PLoS ONE ramp c linearly towards ≈2.677 with additive noise (values M).
- **Lake phosphorus model** (Carpenter, Ludwig & Brock 1999 [M]; used by Carpenter & Brock 2006 [V]): dP/dt = l − sP + rP^q/(m^q + P^q) (eq. M), with loading l as the slow driver. It is bistable for q ≥ 2 over a range of l (folds by f = f′ = 0).
- **Klausmeier 1999 (eq. M):** ∂w/∂t = a − w − wn² + v ∂w/∂x and ∂n/∂t = wn² − mn + ∇²n.
  - Homogeneous vegetated states are n* = [a ± √(a² − 4m²)]/(2m) with w* = m/n*, so they exist iff a ≥ 2m, a saddle-node at a = 2m (derived here). The desert (n = 0, w = a) is always linearly stable.
  - Patterns (stripes on slopes, spots) appear for intermediate a, and pattern states persist below a = 2m: the "evasion" of Rietkerk 2021.
- **Rietkerk et al. 2002:** three fields (surface water, soil water, plant biomass) with infiltration feedback (equations M).
- **Kéfi et al. 2007:** a probabilistic cellular automaton (vegetated, empty, degraded) with local facilitation (rules M).
- **Yeast with a cooperative Allee effect** (Dai 2012): an experimental fold in dilution rate (abstract).

#### Inferences
- **Two populations of evidence.** Replicated experiments with known drivers (Carpenter, Pace, Drake, Dai, Veraart) succeed. Observational series selected afterwards (Burthe, Gsell, O'Brien) mostly fail, as Boettiger & Hastings predicted. A library's demonstrations should use the first kind to show what works, and the second kind to show the false-positive rate honestly.
- **Spatial indicators.** Spatial indicators (Kéfi, Génin) are directly relevant to "states that are fields". Rietkerk 2021, however, means a pattern-forming system can lose homogeneous-state stability without tipping. A spatial detector must separate "approaching a fold of the homogeneous state" from "approaching a Turing instability", which is itself a bifurcation but a benign one.
- **Multivariate gains are small.** Multivariate indicators gave only weak gains on lake data (O'Brien 2023). The gain from structured-state detectors (covariance eigenvalues, spatial correlation) should be measured, not assumed.

#### Gaps
- The content of several key papers (Wang 2012, Drake & Griffen 2010, Veraart 2012, Kéfi 2007) is from memory; the abstracts were not retrievable.
- Coral reef time series analysed with statistical EWS were not found.
- I did not find a recent (2024–2026) systematic test on fisheries data comparable to Burthe 2016.
- The full numbers of O'Brien 2023 (lakes, true-positive rates) were not read; the page was blocked.

### 4. Epidemiological Transitions

#### Takeaway
Emergence and elimination are transcritical bifurcations at R₀ = 1. Near-threshold (subcritical) transmission is well modelled by a birth–death–immigration process whose indicators (mean, variance, autocorrelation) diverge in closed form as R₀ → 1 (O'Regan & Drake 2013). Simulation studies show most indicators survive aggregation and reporting error, and critical slowing down persists in high-dimensional models (SEIR with age structure, networks, agent-based). Empirical tests are encouraging for slowly changing systems: malaria resurgence in Kericho, measles in Niger (fitted models), mumps in England, pertussis in the USA. They fail when forcing is fast relative to relaxation, as in the COVID-19 second wave in Europe, where indicators *decreased*. A 2023 systematic review found 36 of 37 studies reporting CSD, with sensitivity from 0.03 to 1, a publication-bias warning in itself.

#### Cited Findings

##### 4.1 Theory
- O'Regan & Drake 2013 [V] developed the theory of EWS of disease emergence and leading indicators of elimination (title; content M) — [Theor. Ecol. 6:333–357](https://doi.org/10.1007/s12080-013-0185-5).
- Drake et al. 2019 [V] view emergence and re-emergence as "a 'critical transition'" and use "noisy dynamic bifurcation to understand the relationship between the system observables and the distance to this transition"; "the motion of perturbations slows as the system approaches the transition" (abstract) — [PLoS Comput. Biol. 15:e1006917](https://doi.org/10.1371/journal.pcbi.1006917). The brief's "Drake et al. 2019 Nature" was not found; this PLoS CB perspective is the matching 2019 paper.
- Brett, Drake & Rohani 2017 [V], anticipating the emergence of infectious diseases — [J. R. Soc. Interface 14:20170115](https://doi.org/10.1098/rsif.2017.0115).
- Miller et al. 2017 [V], forecasting emergence with seasonal forcing — [Theor. Biol. Med. Model. 14:17](https://doi.org/10.1186/s12976-017-0063-8).
- O'Dea, Park & Drake 2018 [V], estimating the distance to an epidemic threshold — [J. R. Soc. Interface 15:20180034](https://doi.org/10.1098/rsif.2018.0034).
- Southall et al. 2021 [V], a review: "the exact time evolution of EWSs depends on the transition"; the review gives guidance on trends to expect, recommends validating on synthetic and historical data before real-time use, and demonstrates limitations on US pertussis reports (abstract) — [J. R. Soc. Interface 18:20210555](https://doi.org/10.1098/rsif.2021.0555).
- Southall, Tildesley & Dyson 2020 [V], EWS in discrete event-sequence (incidence) data — [PLoS Comput. Biol. 16:e1007836](https://doi.org/10.1371/journal.pcbi.1007836). Southall et al. 2022 [V, medRxiv], "How early can an upcoming critical transition be detected?" — [preprint](https://doi.org/10.1101/2022.05.27.22275693).

##### 4.2 Simulation Tests
- Brett et al. 2018 [V] scored EWS by AUC under aggregation and reporting error:
  - "the mean, variance and first differenced variance all perform well unless reporting error is highly overdispersed";
  - autocorrelation, autocovariance and decay time perform well "provided that the aggregation period … is larger than the serial interval";
  - "the coefficient of variation, skewness and kurtosis are found to be unreliable"; seven of ten EWS perform well.

  Source (abstract): [PLoS Comput. Biol. 14:e1006204](https://doi.org/10.1371/journal.pcbi.1006204).
- Brett et al. 2020 [V] tested five measles-like models: non-seasonal SEIR, seasonal SEIR, age-structured SEIR, a multiplex network model, and the agent-based FRED. In each, vaccine uptake falls linearly from 92% to 70% over 15 years, against a ~90% herd-immunity threshold. "We found evidence of CSD prior to disease re-emergence in all models"; the best EWS were the mean and variance, with AUC > 0.75 one year before the transition (abstract) — [PLoS Comput. Biol. 16:e1007679](https://doi.org/10.1371/journal.pcbi.1007679).

##### 4.3 Empirical Tests
- **Malaria.** Harris, Hay & Drake 2020 [V]: "the first empirical test" during malaria resurgence in Kericho, Kenya. "Nine of the statistics increased as predicted and variance, the first difference of variance, autocovariance, lag-1 autocorrelation and decay time returned early warning signals … based on permutation tests" (abstract) — [Biol. Lett. 16:20190713](https://doi.org/10.1098/rsbl.2019.0713).
- **Measles.** Tredennick et al. 2022 [V], fitted mechanistic measles models for four cities in Niger: "several EWS accurately anticipate measles re-emergence and elimination" (abstract) — [J. R. Soc. Interface (2022)](https://doi.org/10.1098/rsif.2022.0123). Code: [project-aero/measles-ews](https://www.github.com/project-aero/measles-ews) (search result [S]).
- **Mumps, pertussis, dengue, plague.** Brett & Rohani 2020 [V] trained a supervised classifier on simulations to detect "dynamical footprints" of (re-)emergence. It "anticipated mumps reemergence [in England] 4 years in advance", reliably classified US pertussis resurgence from 1992, and was applied to dengue serotypes in Puerto Rico and the 2017 plague in Madagascar (abstract) — [PLoS Biol. 18:e3000697](https://doi.org/10.1371/journal.pbio.3000697).
- **COVID-19.**
  - O'Brien & Clements 2021 [V], 24 countries: composite EWS "can predict nonlinear case increases, but … the predictive ability … varies between waves" (abstract) — [Biol. Lett. 17:20210487](https://doi.org/10.1098/rsbl.2021.0487).
  - Proverbio et al. 2022 [V]: EWS succeed "when some basic assumptions are satisfied: a slow forcing through the transitions and not-fat-tailed noise" (abstract) — [PLoS Comput. Biol. 18:e1009958](https://doi.org/10.1371/journal.pcbi.1009958).
  - Dablander et al. 2022 [V]: before the second European wave, indicators "generally decreased rather than increased". The authors explain this by "transient dynamics and the multiple timescales of relaxation", and state that CSD theory "applies only when the external forcing … across a critical point is slow relative to the internal system dynamics" (abstract) — [Proc. R. Soc. B 289:20211809](https://doi.org/10.1098/rspb.2021.1809).
- **Systematic review.** Delecroix et al. 2023 [V]: 37 studies (21 simulated, 16 real-world); "36 out of 37 studies detected significant signs of critical slowing down", with sensitivity "ranging from 0.03 to 1" and lead times of 10 days to 68 months. Challenges include short and coarse series, too-rapid increases and strong seasonality (abstract) — [PLOS Glob. Public Health 3:e0002253](https://doi.org/10.1371/journal.pgph.0002253).

##### 4.4 Epidemiological Test Models
- **SIS or SIR with slowly varying R₀ (eq. M).** dI/dt = β(t) S I/N − γI (+ ζ for imported cases); R₀(t) = β(t)/γ crosses 1 by a linear ramp, by declining vaccine coverage (Brett 2020: 92% → 70% over 15 years, abstract) or with seasonal β.
- **Birth–death–immigration process (BDI; O'Regan & Drake 2013 [V record]; formulas derived here).** Infection is a per-capita "birth" rate λ = β, recovery a "death" rate μ = γ, and importation an immigration rate ζ. For λ < μ the stationary law is negative binomial with:
  - mean ζ/(μ − λ);
  - variance ζμ/(μ − λ)²;
  - index of dispersion μ/(μ − λ);
  - autocorrelation at lag τ equal to exp[−(μ − λ)τ].

  All diverge, or tend to 1 for the autocorrelation, as R₀ = λ/μ → 1⁻. Approached from above (elimination), the mean falls, so variance-type indicators can *decrease* even as CSD occurs. Southall 2021 notes that the trends depend on the transition (content M for the elimination detail).
- **Seasonally forced SEIR, age-structured models, network and agent-based simulators** (Brett 2020), as tests of whether CSD survives dimension.

#### Inferences
- The BDI process is the cleanest epidemiological test model: indicator trajectories are known exactly as functions of R₀, so a detector's bias and variance can be scored analytically.
- The COVID-19 failures (Dablander 2022) and the Kericho and mumps successes fit one rule, timescale separation. A detector should estimate the relaxation time and the forcing timescale, and report when separation fails.
- Delecroix's 36 of 37 positive studies sits beside Burthe's 9–13% true positives in ecology. This suggests strong publication selection in the epidemic literature; demonstrations should include null or negative cases (e.g. stationary periods, the elimination direction).

#### Gaps
- No Nature paper by Drake et al. (2019) on reemergence was found; the brief may conflate it with the PLoS CB perspective.
- The data sources for Kericho and Niger were not checked for access or licence.
- I did not verify the elimination-direction formulas in O'Regan & Drake against the paper.

### 5. Datasets for Tests and Demonstrations, with Access and Terms

#### Takeaway
Most records needed for honest demonstrations are openly available. HadISST is under the UK Open Government Licence. VODCA and PKU GIMMS NDVI are under CC BY 4.0 on Zenodo. NASA Earthdata (MODIS) defaults to CC0. EPICA Dome C on PANGAEA is under CC BY 3.0. NTL LTER lake data are stated to be CC BY. Project Tycho is under CC BY 4.0 but needs registration. Several key providers state *no licence*, only a citation request: NSIDC (citation as a condition), the NGRIP data page in Copenhagen, RAPID ("freely available", acknowledgement requested) and NOAA ERSST (no terms on the product page). For these the terms are unclear rather than restrictive, and the data library should record the condition verbatim.

#### Cited Findings

##### 5.1 Sea-Surface Temperature
- **HadISST1** (Met Office Hadley Centre):
  - "HadISST1 is subject to Crown copyright protection and is provided under an Open Government Licence"; the page links OGL **v2.0**;
  - citation of Rayner et al. 2003 and a © Crown Copyright attribution are required, and "the source should also be quoted in the acknowledgements";
  - the main page states that data are free "for the purposes of private study and scientific research", refers to the terms page, and notes that HadISST1 "will be replaced by a new dataset within 2026".

  Sources: [terms page](https://www.metoffice.gov.uk/hadobs/hadisst/terms_and_conditions.html) [V]; [main page](https://www.metoffice.gov.uk/hadobs/hadisst/) [V]. Rayner et al. 2003 [V] — [JGR 108(D14):4407](https://doi.org/10.1029/2002JD002670). Used by Caesar 2018, Boers 2021 and DD23 (content M for the latter two).
- **ERSST v5/v6** (NOAA NCEI): 2° × 2° monthly; v5 from January 1854, v6 from January 1850; "data are more reliable after the 1940's". The product page states no licence or terms; NCEI was migrating to the cloud, with some portals unavailable — [NCEI ERSST page](https://www.ncei.noaa.gov/products/extended-reconstructed-sst) [V]. Huang et al. 2017 [V] — [J. Climate 30:8179–8205](https://doi.org/10.1175/JCLI-D-16-0836.1). US federal data are generally public domain (M; not stated on the page).
- **RAPID-MOCHA 26°N** (2004–2024, v2024-2): "made freely available to the public", with no named licence. Download requires an email address; acknowledgement of NERC, NSF and NOAA is requested, with dataset DOI 10.5285/5c39bf4e-5140-5546-e063-7086abc027bc — [RAPID data page](https://rapid.ac.uk/data/data-download) [V].

##### 5.2 Ice Cores
- **NGRIP δ¹⁸O and dust** (Physics of Ice, Climate and Earth, University of Copenhagen):
  - δ¹⁸O at 5 cm resolution on GICC05 (0–60 ka), 20-year means to 123 ka (GICC05modelext), 50-year means, and calcium and dust series;
  - the only condition stated is "Please cite the relevant articles when using the data", and no licence is given.

  Source: [iceandclimate.nbi.ku.dk/data](https://www.iceandclimate.nbi.ku.dk/data/) [V]. NGRIP members 2004 [V] — [Nature 431:147–151](https://doi.org/10.1038/nature02805); Rasmussen et al. 2014 [V record] — QSR 106:14–28 (INTIMATE stratigraphy; DOI M). Boers 2018 derived a 5-year series from the raw record (abstract). **Terms unclear (no licence); attribute and cite.**
- **EPICA Dome C** 800-kyr deuterium and temperature (Jouzel & Masson-Delmotte 2007) on PANGAEA, licence **CC BY 3.0 Unported** — [PANGAEA.683655](https://doi.org/10.1594/PANGAEA.683655) [V]. EPICA community members 2004 [V] — [Nature 429:623–628](https://doi.org/10.1038/nature02599).

##### 5.3 Vegetation
- **VODCA v1** (Zenodo): C-band 2002–2018, X-band 1997–2018, Ku-band 1987–2017; licence **CC BY 4.0** — [Zenodo 2575599](https://zenodo.org/records/2575599) [V]. VODCA v2 (Zotta et al. 2024 [V record]): licence not checked (M: likely CC BY 4.0).
- **PKU GIMMS NDVI** v1.2 (Li et al. 2023 [V] — [ESSD 15:4181–4203](https://doi.org/10.5194/essd-15-4181-2023)): January 1982 to December 2022, half-monthly, 1/12°; licence **CC BY 4.0** — [Zenodo 8253971](https://zenodo.org/records/8253971) [V]. The original GIMMS NDVI3g (Pinzon & Tucker 2014 [V] — [Remote Sens. 6:6929–6960](https://doi.org/10.3390/rs6086929)): access and terms not checked.
- **MODIS (e.g. MOD13 NDVI) via NASA Earthdata/LP DAAC:** "Unless marked otherwise, mission data are licensed as Creative Commons Zero (CC0)"; citation is "very strongly urged" but is not a licence condition — [NASA Earthdata data-use guidance](https://www.earthdata.nasa.gov/engage/open-data-services-software-policies/data-use-guidance) [V].
- Caveat for all optical indices: Smith & Boers 2023 found resilience estimation unreliable in dense tropical and boreal forests (§1.4).

##### 5.4 Sea Ice
- **NSIDC Sea Ice Index** (G02135) v3, 26 October 1978 to 31 July 2025, superseded by v4. No formal licence; "As a condition of using these data, you must cite the use of this data set" (Fetterer et al. 2017, DOI 10.7265/N5K072F8) — [NSIDC G02135 v3](https://nsidc.org/data/g02135/versions/3) [V].
- EUMETSAT OSI SAF / ESA CCI sea-ice concentration records (Lavergne et al. 2019 [S, review record only]): terms not checked.

##### 5.5 Lakes
- **North Temperate Lakes LTER** (including the Peter–Paul–Tuesday Cascade data used by Carpenter 2011 and Pace 2017), via EDI. The NTL site says its data are licensed "CC-BY", allowing scientific and commercial use with citation. An older NTL agreement asked users to send manuscripts to the lead PI. The licence should be checked per package in EDI's "Intellectual Rights" field (search summary of NTL pages [S]) — [NTL all data](https://lter.limnology.wisc.edu/all-data/); an example Cascade package record is [NSF PAR 10572636](https://par.nsf.gov/biblio/10572636-cascade-project-north-temperate-lakes-lter-temperature-chain-data) [S].

##### 5.6 Case Counts
- **Project Tycho** (University of Pittsburgh): pre-compiled US data for 93 conditions, including measles (~436,900 records), dengue for 99 countries and COVID-19 for 237 countries; registration is required to download [V, provider page](https://www.tycho.pitt.edu/data/). The dataset page states that all Project Tycho data are under **CC BY 4.0** (search summary [S]) — [US measles dataset](https://www.tycho.pitt.edu/dataset/US.14189004/). van Panhuis et al. 2013 [V] — [NEJM 369:2152–2158](https://doi.org/10.1056/NEJMms1215400).
- Johns Hopkins US measles repository, CC BY 4.0 (search summary [S]) — [CSSEGISandData/measles_data](https://github.com/cssegisanddata/measles_data).
- WHO measles surveillance and OWID/JHU COVID-19 series: terms not checked this session (M: OWID content CC BY 4.0).

#### Inferences
- The data library should store, per dataset, the licence string verbatim, plus any condition ("must cite", "acknowledge NERC/NSF/NOAA", "registration required"). Where there is no licence (NGRIP, RAPID, ERSST), the cache should be marked "terms unclear; attribution required" rather than assumed open.
- HadISST's page links OGL v2.0 while the current OGL is v3.0; the version stated should be recorded as given. HadISST1 is about to be replaced (2026), so the dataset version should be pinned.

#### Gaps
- Licences for VODCA v2, GIMMS NDVI3g (original), ESA CCI/OSI SAF sea ice, WHO measles data, OWID COVID and the Kericho and Niger data were not checked.
- No EDI package page was opened, so the licence of the specific Cascade packages is unconfirmed.

### 6. What This Means for the Design

#### Takeaway
A climate domain and a detector library should first implement:
1. the Stommel–Cessi box model and the energy-balance model, each in smooth and non-smooth form;
2. North's diffusive EBM as the first field;
3. the grazing model and Klausmeier's PDE;
4. the birth–death–immigration and SIS processes;
5. noise-induced and oscillatory negative controls.

Honest demonstrations use replicated or controlled transitions with open data: the NTL Cascade lakes, EPICA, PKU GIMMS and VODCA, and Project Tycho. Contested observational cases (AMOC fingerprints, NGRIP DO events, Amazon VOD) should be run as robustness grids that reproduce the critiques. Every detector result must carry the observable, preprocessing, window, surrogate and multiple-testing settings, the noise model, the timescale separation and the smoothness assumption, and must never extrapolate a tipping time without an explicit model and its full uncertainty.

#### Cited Findings

##### 6.1 Test Models to Implement First, in Order
1. **Stommel–Cessi**, as the slow–fast pair (4.5) and the reduced (4.6). Closed-form folds y± = 2/3 ± √(η² − 3)/(3η) with p̄ ∈ [0.96, 1.48] at η² = 7.5 (Berglund & Gentz eqs 4.1–4.8 [V]; folds derived here). Add Stommel's |Δρ| version as a non-smooth control (eq. M). Drive by linear ramps of p, by periodic p̄(t), and by ramps of different rates (rate tipping: Lohmann & Ditlevsen 2021 [V]; Ashwin 2012 [V record]).
2. **Zero-dimensional EBM** with tanh albedo (CSD present) and piecewise-linear albedo (no CSD before the non-smooth fold; window derived here), plus the Benzi–Nicolis cubic form with periodic forcing and noise (stochastic resonance; Berglund & Gentz eqs 3.1–3.7 [V]).
3. **North's diffusive EBM** on the sphere (North 1975 [V]; eq. M) as the first field: the small ice-cap instability and the snowball fold, with spatial indicators near the ice edge. Pair it with the **Wagner–Eisenman sea-ice EBM** [V], where bistability disappears with fuller physics (a "no tipping" control).
4. **Grazing / harvested logistic** (May 1977, Noy-Meir 1975, Ludwig 1978 [V]): folds at c ≈ 2.604 and 1.787 for r = 1, K = 10, h = 1 (derived here). Then **Carpenter's lake P model** (eq. M).
5. **Klausmeier's PDE** [V] (fold of homogeneous states at a = 2m, derived here; Turing patterns), and Rietkerk 2002 [V]. These test spatial indicators and the pattern-evasion case of Rietkerk 2021 [V].
6. **Birth–death–immigration and SIS with slowly varying R₀** (O'Regan & Drake 2013 [V]; closed-form indicators derived here), then seasonal and age-structured SEIR with declining vaccination (Brett 2020 [V]).
7. **Negative controls without bifurcation:**
   - the double well with α-stable or increasing noise (Ditlevsen 1999 [V]): variance rises while autocorrelation stays flat, per Ditlevsen & Johnsen 2010 [V];
   - Welander's relaxation oscillator [V] (eq. M);
   - a limit-cycle DO analogue (Vettoretti 2022 [V]);
   - a model forced faster than it relaxes (Dablander 2022 [V]).

##### 6.2 Records for Honest Demonstrations, with Terms
- **Known-truth experiments:** NTL LTER Peter/Paul lakes, Carpenter 2011 [V] and Pace 2017 [V], with a reference lake as a built-in control (CC BY per NTL [S]; check each EDI package).
- **AMOC fingerprint:** HadISST (OGL [V]) plus ERSST (NOAA; no terms stated [V]); RAPID (free, acknowledgement [V]) for validation only, since it is too short for CSD. The demonstration should reproduce the dependence on product, coverage and gap filling (Ben-Yami 2024 [V]; Morr 2026 [V]) and on fingerprint state (Terhaar 2025 [V]; Fan 2026 [V]).
- **Paleo:** NGRIP δ¹⁸O at 5 cm / 20 yr (citation only, terms unclear [V]) for DO events, run as Hummel 2025's robustness test [V] and presented as inconclusive. EPICA Dome C δD (CC BY 3.0 [V]) for glacial terminations, as in Dakos 2008 (content M).
- **Vegetation:** VODCA (CC BY 4.0 [V]) for the Amazon after Boulton 2022 [V]; PKU GIMMS NDVI (CC BY 4.0 [V]) and MODIS (CC0 [V]) for low-biomass regions, where Smith & Boers 2023 [V] found estimates reliable.
- **Sea ice:** the NSIDC Sea Ice Index (citation condition [V]) as a demonstration of *no* CSD and of sensitivity to deseasonalisation (Livina & Lenton 2013 [V]).
- **Epidemics:** Project Tycho US measles and pertussis (CC BY 4.0 [S], registration [V]), mirroring Southall 2021 [V] (pertussis) and Brett & Rohani 2020 [V].

##### 6.3 What the Empirical Record Says a Detector Must Report
- **False-positive rates on non-transitions, not only hits.** Burthe 2016 [V] found 9–13% true positives and 47–53% false positives; O'Brien 2023 [V] found most indicators no better than chance; and selecting on observed transitions inflates false positives (Boettiger & Hastings 2012a [V]).
- **Agreement among indicators.** Variance and autocorrelation disagreed in 43% of series (Burthe 2016 [V]) and agreement was low (Gsell 2016 [V]). Both must rise for a fold; variance alone may be noise (Ditlevsen & Johnsen 2010 [V]).
- **Sensitivity to processing.** O'Brien 2023 found results "highly processing dependant" [V]; Livina & Lenton 2013 found seasonal-cycle removal decisive [V]; van Westen 2024 found classical EWS sensitive to the time interval analysed [V]. A detector should report a grid over window length, detrending bandwidth and indicator, and the fraction of the grid that is significant.
- **Noise colour and changing external variability.** Correlated or nonstationary noise mimics CSD (Boettner & Boers 2022 [V]); separate the two by time-dependent AR(2) or similar (Hallali 2025 [V]). Surrogates should match the noise model, beyond AR(1) red noise.
- **Data coverage and gap filling.** Nonstationary observational coverage can create spurious trends in variance and AR(1) (Ben-Yami 2024 [V]).
- **Representativeness of the observable.** SST fingerprints may not track the AMOC (Terhaar 2025 [V]) and their relation is state-dependent (Fan 2026 [V]). Optical vegetation indices fail in dense forest (Smith & Boers 2023 [V]). Indicators can trend differently across observables before partial collapses (Lohmann 2025 [V]).
- **Timescale separation.** The theory needs slow forcing; when it fails, indicators can fall before a transition (Dablander 2022 [V]; Proverbio 2022 [V]).
- **Smoothness and mechanism.** Non-smooth potentials give no warning (Hastings & Wysham 2010 [V]). Oscillatory or noise-induced transitions are not folds (Ditlevsen 1999 [V]; Vettoretti 2022 [V]). Spatial patterns may mean resilience, not warning (Rietkerk 2021 [V]).
- **No tipping-time extrapolation by default.** Propagated uncertainty extends tipping times "several millennia into the future" (Morr 2026 [V]; Ben-Yami 2024 [V]).

#### Inferences
- **What the existing indicators lack.** Noetarium's foundation already has rolling variance and lag-1 AC with Kendall's τ against red-noise surrogates, DFA, avalanches and the branching ratio. The record suggests adding:
  - restoring-rate estimators robust to noise colour (Boettner & Boers);
  - a time-dependent AR(2) or state-space estimator that separates noise variance from the restoring rate (Hallali);
  - a likelihood-ratio test between a stationary and a destabilising OU model (Boettiger & Hastings 2012b);
  - recovery-rate estimation from large perturbations (Smith 2022);
  - flickering and bimodality detection (Wang 2012; Livina & Lenton 2013);
  - spatial indicators: spatial variance, skewness, Moran's I, patch-size distributions (Kéfi 2014; Génin 2018);
  - covariance-eigenvalue indicators for compositions and networks (Chen 2019; Weinans 2021).
- **Test-suite structure.** For each test model, a known λ(t) gives a ground truth. The scores reported should be ROC/AUC against null runs (Brett 2018/2020 practice), lead time, and calibration of the restoring-rate estimate, together with a fixed set of negative controls (§6.1 item 7).
- **Climate domain modules.** The climate domain's planned energy-balance and coupled ocean–atmosphere modules map onto test models 1–3. The planned climate-risk domain can take the GTPR 2025 thresholds as declared data with their stated confidence, never as detector outputs.
- **Japanese cities.** None of the recommended demonstrations uses a Japanese city; the case studies are global fields, Atlantic indices, ice cores, North American lakes and US or African case counts.

#### Gaps
- The parameter values for Carpenter's lake model, Welander, Brovkin and North's EBM, and the Dakos 2012 overharvesting ramp, must be taken from the primary papers before implementation (flagged eq. M).
- Licence checks remain for VODCA v2, OSI SAF/CCI sea ice, GIMMS NDVI3g and the individual EDI packages.
- Whether a library should implement "physics-based" AMOC indicators (F_ovS, buoyancy flux) depends on the climate domain having ocean fields. These indicators are diagnostics of model or reanalysis output, not generic detectors.

## Part IV. Brains and Bodies

*Remark.* Part IV's conventions. The bibliographic markers are those of §0.3; here [V] means the record was checked against Crossref's API (DOI, journal, volume, pages) or against the PubMed record from NCBI E-utilities, or the provider's own page for data and GitHub's API for software licences. Statements about *content* say "(abstract)" when they come from the abstract read this session (PubMed), and "(content from memory)" otherwise. Formulas are standard results; where a formula or equation number was not checked against the paper, it says so.

*Caveat.* Nothing here is medical advice; the validation status of each method is stated where known.

### 1. Healthy Brain Dynamics Are Hypothesised to Lie Near Criticality, Measured by Several Non-Equivalent Observables

#### Takeaway
The critical brain hypothesis is supported by four families of observable: avalanche statistics, the branching ratio, long-range temporal correlations, and the aperiodic spectrum. Each is necessary-but-not-sufficient. Power laws arise without criticality, subsampling biases every estimate, and in vivo spiking is better described as slightly subcritical ("reverberating", m ≈ 0.98) than as exactly critical. A 2025 meta-analysis of 140 datasets argues that the long-running controversy reflects a methodological choice, while a 2026 review assesses the hypothesis critically. A detector must therefore report several mutually constraining quantities with their null models. "Power law found" on its own does not establish criticality.

#### Cited Findings

##### 1.1 Neuronal avalanches and their exponents
- Beggs & Plenz defined neuronal avalanches as cascades of LFP events in cortical slice cultures, separated by empty time bins. Sizes followed P(S) ∝ S^(−3/2) and lifetimes ∝ T^(−2) when the bin width equalled the mean inter-event interval, with branching parameter σ ≈ 1 (content from memory). — [Beggs & Plenz 2003, J Neurosci 23:11167–11177](https://doi.org/10.1523/jneurosci.23-35-11167.2003) [V]
- Avalanche definition (content from memory; standard): threshold each channel (e.g. negative LFP peaks beyond −3 SD), bin at Δt (classically ⟨IEI⟩), and call a run of non-empty bins bounded by empty ones an avalanche. Its size S is the number of events (or the summed amplitude) and its duration T the number of bins. The exponents depend on Δt and the threshold, which is a known pitfall. — [Beggs & Plenz 2003](https://doi.org/10.1523/jneurosci.23-35-11167.2003) [V]
- In high-resolution cultured networks at single-neuron level, Friedman et al. report a universal scaling function of the mean temporal profiles of avalanches of very different durations. They also report approximate power laws for sizes and durations, samples in sub- and supercritical phases, and scaling laws between exponents (abstract). — [Friedman et al. 2012, PRL 108:208102](https://doi.org/10.1103/physrevlett.108.208102) [V]
- **Crackling-noise scaling relation** (Sethna, Dahmen & Myers; formula standard, equation number not verified). With P(S) ∝ S^(−τ), P(T) ∝ T^(−α) and ⟨S⟩(T) ∝ T^(γ) (γ ≡ 1/σνz), criticality implies **γ = (α − 1)/(τ − 1)**. Mean profiles collapse as ⟨s(t|T)⟩ = T^(γ−1) 𝓕(t/T). Mean-field directed percolation (an unbiased branching process at m = 1) gives τ = 3/2, α = 2, γ = 2. — [Sethna et al. 2001, Nature 410:242–250](https://doi.org/10.1038/35065675) [V]; [Friedman et al. 2012](https://doi.org/10.1103/physrevlett.108.208102) [V]
- Measured exponents vary across species, individuals, time and stimulus, yet still roughly obey the scaling relation. Quasicriticality explains this as a departure from criticality along a Widom line under external drive: exponents shrink in absolute value while the relation approximately holds (abstract). — [Fosque et al. 2021, PRL 126:098101](https://doi.org/10.1103/physrevlett.126.098101) [V]
- Fontenele et al. report that criticality, judged by the crackling relation, appears at an intermediate cortical state between synchronised and desynchronised regimes in anaesthetised rats, with an exponent set not of mean-field DP (content from memory). — [Fontenele et al. 2019, PRL 122:208101](https://doi.org/10.1103/physrevlett.122.208101) [V]
- Subsampled directed-percolation models reproduce the experimentally observed scaling relations (content from memory, title-level). — [Carvalho et al. 2021, Front Neural Circuits 14:576727](https://doi.org/10.3389/fncir.2020.576727) [V]
- Avalanches have been reported in awake monkeys, in resting human MEG and in fMRI point processes. Resting-state networks emerge at criticality in a connectome model. — [Petermann et al. 2009, PNAS 106:15921](https://doi.org/10.1073/pnas.0904089106) [V]; [Shriki et al. 2013, J Neurosci 33:7079](https://doi.org/10.1523/jneurosci.4286-12.2013) [V]; [Tagliazucchi et al. 2012, Front Physiol 3:15](https://doi.org/10.3389/fphys.2012.00015) [V]; [Haimovici et al. 2013, PRL 110:178101](https://doi.org/10.1103/physrevlett.110.178101) [V]
- A feedback-driven Ising-like model fitted to human resting-state recordings places the coexistence of oscillations and avalanches near a non-equilibrium critical point at the onset of self-sustained oscillations (abstract). — [Lombardi et al. 2023, Nat Comput Sci 3:254–263](https://doi.org/10.1038/s43588-023-00410-9) [V]

##### 1.2 Fitting heavy tails (Clauset–Shalizi–Newman)
- Continuous MLE: **α̂ = 1 + n [Σᵢ ln(xᵢ/x_min)]⁻¹**. The discrete approximation replaces x_min with x_min − ½. x_min is chosen by minimising the Kolmogorov–Smirnov distance. Goodness of fit is a semiparametric bootstrap p-value (the power law is plausible if p > 0.1). Alternatives (lognormal, exponential, truncated power law) are compared by Vuong's normalised log-likelihood ratio. Formulas standard; the paper's equation numbers (≈ Eq. 3.1 and 3.7) are from memory. — [Clauset, Shalizi & Newman 2009, SIAM Rev 51:661–703](https://doi.org/10.1137/070710111) [V]
- Reference implementation: `powerlaw` (Python, MIT licence [V via GitHub API]). — [Alstott, Bullmore & Plenz 2014, PLoS ONE 9:e85777](https://doi.org/10.1371/journal.pone.0085777) [V]
- The NCC toolbox (MATLAB) does avalanche detection, power-law fits with bootstrap, shape collapse and scaling-relation tests. — [Marshall et al. 2016, Front Physiol 7:250](https://doi.org/10.3389/fphys.2016.00250) [V]. Its licence was not verified: GitHub search found only a third-party mirror (`ZhenyuanJin/ncc_toolbox`, licence NOASSERTION) [V via GitHub API].

##### 1.3 The branching ratio, subsampling and reverberating dynamics
- **Branching process with immigration** (standard; Harris; Heathcote): A_{t+1} = Σ_{i=1}^{A_t} ξ_i + h_t, with E ξ = m. For m < 1 the stationary mean is ⟨A⟩ = ⟨h⟩/(1 − m). The autocorrelation at lag k is m^k, so the intrinsic timescale is **τ = −Δt / ln m**. One ancestor's mean total progeny is 1/(1 − m). At m = 1 the avalanche exponents are 3/2 (size) and 2 (duration). — [Harris 1963, The Theory of Branching Processes (Springer)](https://doi.org/10.1007/978-3-642-51866-9_6) [V]; [Heathcote 1965, JRSS B 27:138–143](https://doi.org/10.1111/j.2517-6161.1965.tb00596.x) [V]
- **Multistep regression (MR) estimator.** Under binomial subsampling a_t ~ Bin(A_t, α), the regression slope of a_{t+k} on a_t is **r_k = b·m^k** with b = α² Var(A)/Var(a). Fitting r_k across k gives m without bias from b, whereas the conventional r₁ (or descendant/ancestor ratio) estimates b·m and so underestimates m (derivation reproduced from memory; equation numbers not verified). — [Wilting & Priesemann 2018, Nat Commun 9:2325](https://doi.org/10.1038/s41467-018-04725-4) [V]
- In vivo spiking in rat, cat and macaque gives m̂ ≈ 0.98: a "reverberating" regime between critical and irregular, with intrinsic timescales of the order of a hundred milliseconds. A reverberating model captures and predicts cortical spike propagation (content from memory). — [Wilting & Priesemann 2018](https://doi.org/10.1038/s41467-018-04725-4) [V]; [Wilting & Priesemann 2019, Cereb Cortex 29:2759–2770](https://doi.org/10.1093/cercor/bhz049) [V]
- Reference implementation: `mrestimator` (Python, BSD-3-Clause [V via GitHub API]). Its fit functions include an exponential with offset, to flag non-stationarity (content from memory). — [Spitzner et al. 2021, PLoS ONE 16:e0249447](https://doi.org/10.1371/journal.pone.0249447) [V]
- Subsampling distorts avalanche distributions recorded in vivo. Spike avalanches in vivo suggest a driven, slightly subcritical state. "Subsampling scaling" gives a collapse of distributions across sampling fractions, by which criticality can be tested under subsampling (content from memory). — [Priesemann et al. 2009, BMC Neurosci 10:40](https://doi.org/10.1186/1471-2202-10-40) [V]; [Priesemann et al. 2014, Front Syst Neurosci 8:108](https://doi.org/10.3389/fnsys.2014.00108) [V]; [Levina & Priesemann 2017, Nat Commun 8:15140](https://doi.org/10.1038/ncomms15140) [V]
- Functional argument: excitable networks maximise dynamic range at branching ratio 1 (Kinouchi–Copelli). Cortical cultures show maximal dynamic range when avalanches are critical (content from memory). — [Kinouchi & Copelli 2006, Nat Phys 2:348–351](https://doi.org/10.1038/nphys289) [V]; [Shew et al. 2009, J Neurosci 29:15595](https://doi.org/10.1523/jneurosci.3864-09.2009) [V]
- Homeostasis: in freely behaving rats recorded for 9 days, visual-cortex networks were robustly organised around criticality. Visual deprivation disrupted this, and it was restored over 48 h, before firing-rate homeostasis (abstract). — [Ma et al. 2019, Neuron 104:655–664](https://doi.org/10.1016/j.neuron.2019.08.031) [V]
- In rats recorded continuously for 10–14 days, waking experience progressively disrupts criticality and sleep restores it. Deviation from criticality predicted future sleep/wake better than slow-wave activity (abstract). — [Xu et al. 2024, Nat Neurosci 27:328–338](https://doi.org/10.1038/s41593-023-01536-9) [V]
- Spontaneous cortical activity is only transiently poised close to criticality (content from memory, title-level). — [Hahn et al. 2017, PLoS Comput Biol 13:e1005543](https://doi.org/10.1371/journal.pcbi.1005543) [V]

##### 1.4 Long-range temporal correlations of oscillation amplitude (DFA)
- The amplitude envelopes of 10 and 20 Hz oscillations (MEG/EEG) are correlated over thousands of cycles and scale as power laws, with exponents invariant across subjects. The authors attribute this to self-organised criticality (abstract). — [Linkenkaer-Hansen et al. 2001, J Neurosci 21:1370–1377](https://doi.org/10.1523/jneurosci.21-04-01370.2001) [V]
- **DFA** (standard; Peng et al.; Hardstone et al.; formula from memory). Profile Y(k) = Σ_{i≤k}(x_i − x̄). Split it into windows of n samples, remove each window's least-squares linear trend, and take F(n) = √(⟨(Y − Ŷ_n)²⟩). Then F(n) ∝ n^α_DFA, with α = 0.5 for white noise and 1 for 1/f. For stationary fGn, α = (β + 1)/2 where S(f) ∝ f^(−β). For oscillations, DFA is applied to the band-pass filtered Hilbert envelope. The fitting range must start beyond the filter's impulse-response correlation time and end at about 10% of the record length (rule as recalled from Hardstone et al.). — [Peng et al. 1995, Chaos 5:82–87](https://doi.org/10.1063/1.166141) [V]; [Hardstone et al. 2012, Front Physiol 3:450](https://doi.org/10.3389/fphys.2012.00450) [V]; [Kantelhardt et al. 2002 (MF-DFA), Physica A 316:87–114](https://doi.org/10.1016/s0378-4371(02)01383-3) [V]
- LRTC and avalanche dynamics are correlated with each other and with behavioural scaling laws (content from memory). — [Palva et al. 2013, PNAS 110:3585](https://doi.org/10.1073/pnas.1216855110) [V]
- **Functional E/I (fE/I)** (Bruining et al.; formula from memory). Window the envelope; within each window compute the mean amplitude and the normalised fluctuation function of the amplitude-normalised profile. Then **fE/I = 1 − corr(W_amp, nF)** across windows, computed only where α_DFA indicates LRTC (≈ > 0.6). The method is validated in silico and applied to children with autism (abstract, partial). — [Bruining et al. 2020, Sci Rep 10:9195](https://doi.org/10.1038/s41598-020-65500-4) [V]

##### 1.5 Excitation–inhibition from the aperiodic spectral slope
- Gao, Peterson & Voytek: in simulated LFPs, the E:I conductance ratio maps monotonically onto the PSD slope (roughly 30–50 Hz), steeper with more inhibition. This was supported by rat hippocampus and macaque propofol ECoG (content from memory). — [Gao et al. 2017, NeuroImage 158:70–78](https://doi.org/10.1016/j.neuroimage.2017.06.078) [V]
- **specparam/FOOOF model** (Donoghue et al.; form from memory): log₁₀ P(f) = b − log₁₀(k + f^χ) + Σ_n a_n exp(−(f − c_n)²/(2w_n²)). Here χ is the aperiodic exponent, k the knee (k = 0 in "fixed" mode) and the Gaussians are periodic peaks. Implementation `fooof`/`specparam`, Apache-2.0 [V via GitHub API]. — [Donoghue et al. 2020, Nat Neurosci 23:1655–1665](https://doi.org/10.1038/s41593-020-00744-x) [V]
- Critiques of interpretation:
  - Separating oscillations from aperiodic activity faces fitting-range, knee and peak-overlap problems (content from memory). — [Gerster et al. 2022, Neuroinformatics 20:991–1012](https://doi.org/10.1007/s12021-022-09581-8) [V]
  - Biophysical modelling shows broadband EEG is shaped by synaptic properties. Propofol produced broadband changes matching its GABA-A effects; corrected for these, delta power uniquely rose within seconds of loss of consciousness (abstract). So the slope is not a pure E:I readout. — [Brake et al. 2024, Nat Commun 15:1514](https://doi.org/10.1038/s41467-024-45922-8) [V]
- 1/f scaling of brain signals need not reflect SOC: it can arise from filtering by extracellular media (content from memory). — [Bédard, Kröger & Destexhe 2006, PRL 97:118102](https://doi.org/10.1103/physrevlett.97.118102) [V]

##### 1.6 Extended critical regions: Griffiths phases and quasicriticality
- In hierarchical modular networks, Griffiths phases stretch critical-like behaviour (power laws, slow relaxation) over an extended parameter region rather than a point (content from memory). — [Moretti & Muñoz 2013, Nat Commun 4:2521](https://doi.org/10.1038/ncomms3521) [V]
- Quasicriticality (driven departure along a Widom line) is in §1.1. — [Fosque et al. 2021](https://doi.org/10.1103/physrevlett.126.098101) [V]

##### 1.7 Reviews, including the 2025 meta-analysis and a 2026 critique
- Reviews: Cocchi et al. (neurobiology, models, cognition); Muñoz (Colloquium, dynamical scaling in living systems); Wilting & Priesemann ("25 years of criticality", established results and controversies); Beggs & Timme ("Being critical of criticality"). — [Cocchi et al. 2017, Prog Neurobiol 158:132–152](https://doi.org/10.1016/j.pneurobio.2017.07.002) [V]; [Muñoz 2018, Rev Mod Phys 90:031001](https://doi.org/10.1103/revmodphys.90.031001) [V]; [Wilting & Priesemann 2019, Curr Opin Neurobiol 58:105–111](https://doi.org/10.1016/j.conb.2019.08.002) [V]; [Beggs & Timme 2012, Front Physiol 3:163](https://doi.org/10.3389/fphys.2012.00163) [V]
- O'Byrne & Jerbi review "how critical is brain criticality". They stress inconsistent operational definitions and the need for scaling relations beyond power laws (content from memory). — [O'Byrne & Jerbi 2022, Trends Neurosci 45:820–837](https://doi.org/10.1016/j.tins.2022.08.007) [V]
- Hengen & Shew lay out criticality as a general computational set point and **meta-analyse 140 datasets (2003–2024)**. They find that "a long-standing controversy is the product of a methodological choice with no bearing on underlying dynamics" (abstract; which choice is not named in the abstract). — [Hengen & Shew 2025, Neuron 113:2582–2598.e2](https://doi.org/10.1016/j.neuron.2025.05.020) [V]. Press coverage frames sleep-based therapy for restoring criticality; this is the authors' hypothesis, not a finding — [WashU Source 2025](https://source.washu.edu/2025/06/a-unified-theory-of-the-mind/) [S].
- A 2026 critical review exists: Sipling et al., "A critical assessment of the brain criticality hypothesis" (record only; content not read). — [Sipling et al. 2026, Trends Open 1:259–269](https://doi.org/10.1016/j.treopn.2026.06.001) [V]
- Clinical relevance is reviewed in a scoping review. — [Zimmern 2020, Front Neural Circuits 14:54](https://doi.org/10.3389/fncir.2020.00054) [V]

##### 1.8 Critiques and negative results
- Power-law avalanche statistics can arise from stochastic dynamics without criticality. — [Touboul & Destexhe 2010, PLoS ONE 5:e8982](https://doi.org/10.1371/journal.pone.0008982) [V]
- Networks in self-sustained irregular regimes away from criticality, and even independent stochastic surrogates of their units, produce the same power-law statistics and universal scaling. These features are "not sufficient to establish criticality" (abstract). — [Touboul & Destexhe 2017, PRE 95:012413](https://doi.org/10.1103/physreve.95.012413) [V]
- Destexhe & Touboul argue the evidence for criticality in cortex is insufficient (content from memory). — [Destexhe & Touboul 2021, eNeuro 8:ENEURO.0551-20.2021](https://doi.org/10.1523/eneuro.0551-20.2021) [V]
- A time-varying external drive can produce apparent criticality signatures (content from memory, title-level). — [Priesemann & Shriki 2018, PLoS Comput Biol 14:e1006081](https://doi.org/10.1371/journal.pcbi.1006081) [V]
- Random subsampling produces maximum-entropy "criticality" signatures in simple population models (title-level). Effective latent variables also generate criticality signatures (title-level). — [Nonnenmacher et al. 2017, PLoS Comput Biol 13:e1005718](https://doi.org/10.1371/journal.pcbi.1005718) [V]; [Morrell, Nemenman & Sederberg 2024, eLife 12:RP89337](https://doi.org/10.7554/elife.89337.3) [V]
- Multielectrode recordings in cat, monkey and human cortex during wake and sleep did not show clear power-law avalanches (content from memory). — [Dehghani et al. 2012, Front Physiol 3:302](https://doi.org/10.3389/fphys.2012.00302) [V]

##### 1.9 Criticality as a biomarker
- Epilepsy: adaptive self-organised criticality fails during seizures (content from memory). Intrinsic excitability measures track antiepileptic drug withdrawal and rise and fall over the wake/sleep cycle (content from memory). — [Meisel et al. 2012, PLoS Comput Biol 8:e1002312](https://doi.org/10.1371/journal.pcbi.1002312) [V]; [Meisel et al. 2015, PNAS 112:14694](https://doi.org/10.1073/pnas.1513716112) [V]
- Sleep deprivation: signatures of critical dynamics fade with sustained wakefulness in human EEG (content from memory). In rats, sleep restores criticality (abstract, §1.3). — [Meisel et al. 2013, J Neurosci 33:17363](https://doi.org/10.1523/jneurosci.1516-13.2013) [V]; [Xu et al. 2024](https://doi.org/10.1038/s41593-023-01536-9) [V]
- Alzheimer's disease: parietal alpha and prefrontal theta LRTC are altered in early-stage disease (content from memory). — [Montez et al. 2009, PNAS 106:1614](https://doi.org/10.1073/pnas.0811699106) [V]
- Depression: theta LRTC breaks down in major depressive disorder (content from memory). — [Linkenkaer-Hansen et al. 2005, J Neurosci 25:10131](https://doi.org/10.1523/jneurosci.3244-05.2005) [V]
- Autism: fE/I (§1.4). — [Bruining et al. 2020](https://doi.org/10.1038/s41598-020-65500-4) [V]
- Anaesthesia and consciousness: §4.

#### Inferences
- No single observable identifies criticality. The defensible claim is consistency across (i) power-law fits that survive comparison with alternatives, (ii) an independent γ from ⟨S⟩(T) matching (α − 1)/(τ − 1) with confidence intervals, (iii) shape collapse, (iv) an MR-estimated m near 1 with τ, and, for EEG, (v) DFA and aperiodic exponent within the expected ranges. The Touboul–Destexhe surrogates and subsampled non-critical models are mandatory negative controls.
- "Distance to criticality" (m, or deviation from the crackling relation) is the clinically interesting quantity, not a binary verdict. This matches the homeostatic set-point framing (Ma 2019; Xu 2024; Hengen & Shew 2025).
- The aperiodic exponent is a confounded E/I proxy (Brake 2024; Gerster 2022). It should be reported as a spectral feature with its fit range and knee, not as an E/I ratio.

#### Gaps
- Which methodological choice Hengen & Shew identify as resolving the controversy: the abstract does not name it, and the full text was not read.
- The content of Sipling et al. 2026 was not read.
- The exact Beggs & Plenz and Friedman exponent values with confidence intervals, and the equation numbers in Clauset 2009, Wilting & Priesemann 2018 and Donoghue 2020, were not re-checked against the papers.
- No large, preregistered clinical validation of any criticality metric as a diagnostic biomarker was found.

### 2. Seizures Are Anticipated Through Cycles of Risk and Slowing, Classified by Bifurcations, and Judged Against Chance

#### Takeaway
Evidence for critical slowing down before seizures is mixed. Maturana et al. (2020, 14 NeuroVista patients) found variance and autocorrelation cycling with seizure susceptibility. Wilkat et al. (2019, 28 patients, 105 seizures, surrogate-tested) found no evidence. Milanowski & Suffczynski (2016) found seizures start without common signatures. The robust finding is that seizure *risk* follows circadian and multidien cycles (Baud 2018; Karoly 2018, 2021; Leguia 2021; Proix 2021), and forecasts of risk, rather than predictions of single seizures, are the defensible target. Seizure termination shows slowing, correlation and flickering (Kramer 2012). Onsets and offsets fall into 16 bifurcation "dynamotypes" (Jirsa 2014; Saggio 2017, 2020), so the expected precursor depends on the bifurcation, and none is expected for noise-induced jumps. Performance must be judged out of sample against a random predictor (Winterhalder 2003; Snyder 2008; Andrzejak 2003; Mormann 2007).

#### Cited Findings

##### 2.1 Critical slowing down before seizures: positive, negative and mechanistic results
- Maturana et al. use long-term intracranial EEG to ask whether signals show critical slowing down before seizures (abstract, truncated after this point). Secondary summaries: in 14 patients with focal epilepsy, iEEG variance and autocorrelation track seizure susceptibility, with circadian and multidien cycles, and are proposed as forecasting features (content [S]/from memory). — [Maturana et al. 2020, Nat Commun 11:2172](https://doi.org/10.1038/s41467-020-15908-3) [V]; summary via [search result, AES/PMC listing](https://pmc.ncbi.nlm.nih.gov/articles/PMC7195436) [S]
- Wilkat, Rings & Lehnertz analysed long-term multichannel recordings from 28 subjects. Using "a surrogate-based evaluation procedure of sensitivity and specificity of time-resolved estimates of early warning indicators", they "found no evidence for critical slowing down prior to 105 epileptic seizures" (abstract). — [Wilkat et al. 2019, Chaos 29:091104](https://doi.org/10.1063/1.5122759) [V]
- Seizures start without common signatures of critical transition (content from memory: variance and autocorrelation did not rise consistently in human iEEG before onset). — [Milanowski & Suffczynski 2016, Int J Neural Syst 26:1650053](https://doi.org/10.1142/s0129065716500532) [V]
- Loss of network resilience precedes seizures: recovery from perturbation slows in models and in recordings, and this determines whether interictal synaptic perturbations become ictogenic (content from memory). — [Chang et al. 2018, Nat Neurosci 21:1742–1752](https://doi.org/10.1038/s41593-018-0278-y) [V]
- Active probing (stimulation-based anticipation) measures excitability by the response to perturbation rather than passive fluctuations (content from memory). — [Kalitzin et al. 2010, Epilepsy Behav 17:310–323](https://doi.org/10.1016/j.yebeh.2009.12.023) [V]
- Seizure *termination* is a discontinuous critical transition. Across spatial scales, activity shows "slowing, increased correlation, and flickering" before termination. Status epilepticus "repeatedly approach[es], but do[es] not cross" the transition (abstract). — [Kramer et al. 2012, PNAS 109:21116–21121](https://doi.org/10.1073/pnas.1210047110) [V]
- Theory: epilepsies are dynamical diseases. Seizure onset in a bistable system may follow a parameter-driven route through a bifurcation, which can give precursors, or a noise-driven jump between coexisting attractors, which gives none (content from memory). — [Lopes da Silva et al. 2003, Epilepsia 44:72–83](https://doi.org/10.1111/j.0013-9580.2003.12005.x) [V]. Noise-induced tipping gives no early warning in general. — [Ditlevsen & Johnsen 2010, GRL 37](https://doi.org/10.1029/2010gl044486) [V]
- Recent work (not peer reviewed or conference-level):
  - A 2025 medRxiv preprint combines multi-timescale cycles of autocorrelation and variance with neural-mass features and reports a best average AUC of 0.83 ([S] via search summary). — [Liu et al. 2025, medRxiv](https://doi.org/10.1101/2025.06.02.25328687) [V record]
  - A 2024 FUSION conference paper tests multichannel CSD features with logistic regression, because the best channel is unknown a priori ([S]). — [Liu et al. 2024, FUSION](https://doi.org/10.23919/fusion59988.2024.10706283) [V record]

##### 2.2 Cycles of seizure risk
- In 37 subjects with an implanted responsive neurostimulator recording interictal epileptiform activity (IEA) for years, IEA oscillates with circadian and subject-specific multidien periods, "most commonly 20–30 days" (abstract). Seizures preferentially occur on the rising phase of the multidien cycle (content from memory; that part of the abstract was truncated). — [Baud et al. 2018, Nat Commun 9:88](https://doi.org/10.1038/s41467-017-02577-y) [V]
- Circadian and circaseptan rhythms appear in seizure diaries (retrospective cohort). — [Karoly et al. 2018, Lancet Neurol 17:977–985](https://doi.org/10.1016/s1474-4422(18)30274-6) [V]
- Further studies and reviews:
  - Seizure cycles across a large RNS cohort (content from memory: circadian cycles in most and multidien cycles in a majority of patients). — [Leguia et al. 2021, JAMA Neurol 78:454](https://doi.org/10.1001/jamaneurol.2020.5370) [V]
  - The review "Cycles in epilepsy". — [Karoly et al. 2021, Nat Rev Neurol 17:267–284](https://doi.org/10.1038/s41582-021-00464-1) [V]
  - The circadian profile improves forecasting. — [Karoly et al. 2017, Brain 140:2169–2182](https://doi.org/10.1093/brain/awx173) [V]
- A development and validation study forecasts seizure risk from chronic IEA cycles, with day-ahead (and longer) horizons informative in many patients (content from memory). — [Proix et al. 2021, Lancet Neurol 20:127–135](https://doi.org/10.1016/s1474-4422(20)30396-3) [V]
- Biological rhythms confound EEG features and EEG-derived networks, so any slow trend in an indicator may be a rhythm rather than a precursor (content from memory). — [Lehnertz et al. 2021, Front Netw Physiol 1:755016](https://doi.org/10.3389/fnetp.2021.755016) [V]

##### 2.3 Bifurcation classes of seizure onset and offset
- **Epileptor** (Jirsa et al.): a five-dimensional phenomenological model with a fast subsystem (x₁, y₁), a spike-wave subsystem (x₂, y₂) and a slow permittivity variable z. Equations as recalled from memory (verify against the paper or TVB's `Epileptor`):
  - ẋ₁ = y₁ − f₁(x₁,x₂) − z + I₁
  - ẏ₁ = y₀ − 5x₁² − y₁
  - ż = (1/τ₀)(4(x₁ − x₀) − z)
  - ẋ₂ = −y₂ + x₂ − x₂³ + I₂ + 0.002 g(x₁) − 0.3(z − 3.5)
  - ẏ₂ = (1/τ₂)(−y₂ + f₂(x₂))
  - g(x₁) = ∫ e^(−γ(t−s)) x₁(s) ds
  - f₁ = a x₁³ − b x₁² for x₁ < 0, and (x₂ − 0.6(z − 4)²) x₁ for x₁ ≥ 0
  - f₂ = 0 for x₂ < −0.25, and 6(x₂ + 0.25) for x₂ ≥ −0.25
  - Typical values: x₀ ≈ −1.6, τ₀ ≈ 2857, τ₂ = 10, I₁ = 3.1, I₂ = 0.45, γ = 0.01.

  Onset is a saddle-node bifurcation and offset a saddle-homoclinic one (the "SN/SH" square-wave burster). The model makes a falsifiable signature prediction: a DC shift at onset, and an inter-spike interval that grows logarithmically towards offset (content from memory). — [Jirsa et al. 2014, Brain 137:2210–2230](https://doi.org/10.1093/brain/awu133) [V]
- The unfolding of a high-codimension singularity (a degenerate Takens–Bogdanov point) organises fast–slow bursters. Slow paths on its parameter sphere give **4 onset × 4 offset = 16 classes**:
  - onsets: saddle-node (SN), saddle-node on invariant circle (SNIC), supercritical Hopf (SupH), subcritical Hopf (SubH);
  - offsets: saddle homoclinic (SH), SNIC, fold of limit cycles (FLC), SupH.

  Ultra-slow transitions between classes can occur (content from memory). — [Saggio et al. 2017, J Math Neurosci 7:7](https://doi.org/10.1186/s13408-017-0050-8) [V]
- The "taxonomy of seizure dynamotypes" (eLife) analysed over 2,000 focal-onset seizures from multiple centres and found evidence for all 16 dynamotypes. Patients' dynamotypes vary systematically, with some types more common ([S] via search summaries). SN/SH is reported as the most common class (from memory). — [Saggio et al. 2020, eLife 9:e55632](https://doi.org/10.7554/elife.55632) [V record; content S/M]
- **Signal signatures** used to classify (standard normal-form scaling, from memory; e.g. Izhikevich's bursting classification):

  | Bifurcation | Amplitude | Frequency | DC shift |
  |---|---|---|---|
  | SN onset | jumps to a finite value | arbitrary | yes |
  | SNIC (onset or offset) | finite | ∝ √(μ − μ_c) towards 0 | no |
  | SupH (onset or offset) | ∝ √(μ − μ_c) | finite, constant | no |
  | SubH onset | jumps | finite | no |
  | SH offset | finite | ISI ∝ −ln(μ − μ_c), frequency → 0 | — |
  | FLC offset | finite, then abrupt stop | finite | — |

- On scalp EEG (1,177 seizures, 158 patients, ICA components labelled visually), SNIC and SupH onsets were more prevalent in wakefulness than in NREM sleep, especially NREM3. The authors note that scalp noise may obscure some bifurcation morphologies (abstract). — [Guendelman, Vekslar & Shriki 2025, eNeuro 12:ENEURO.0157-24.2024](https://doi.org/10.1523/ENEURO.0157-24.2024) [V via PubMed]
- Spatial models and their translation:
  - A neural-field Epileptor explains the diversity of seizure propagation and termination (synchronous vs asynchronous, travelling 2–3 Hz spike-wave sources) through wave propagation in excitable media, coupled oscillators and multiple timescales (abstract). — [Proix et al. 2018, Nat Commun 9:1088](https://doi.org/10.1038/s41467-018-02973-y) [V]
  - The Virtual Epileptic Patient. — [Jirsa et al. 2017, NeuroImage 145:377–388](https://doi.org/10.1016/j.neuroimage.2016.04.049) [V]
  - Personalised virtual brain models in presurgical evaluation (review; the EPINOV trial context, from memory). — [Jirsa et al. 2023, Lancet Neurol 22:443–454](https://doi.org/10.1016/s1474-4422(23)00008-x) [V]
- Neural fragility: fit linear time-varying models x_{t+1} = A_t x_t to iEEG. A node's fragility is the minimum-norm perturbation of its column (row) of A_t that moves an eigenvalue onto the unit circle. It is proposed as a marker of the seizure onset zone, tested retrospectively against surgical outcome in a multicentre cohort (content from memory). The data are on OpenNeuro, ds003029, CC0 [V]. — [Li et al. 2021, Nat Neurosci 24:1465–1474](https://doi.org/10.1038/s41593-021-00901-w) [V]

##### 2.4 History and statistical standards of seizure prediction
- Reviews:
  - "The long and winding road": early claims of preictal changes often failed under stringent statistics, out-of-sample testing and long continuous data (content from memory). — [Mormann et al. 2007, Brain 130:314–333](https://doi.org/10.1093/brain/awl241) [V]; also [Mormann et al. 2005, Clin Neurophysiol 116:569–587](https://doi.org/10.1016/j.clinph.2004.08.025) [V]
  - "Seizure prediction — ready for a new era": a shift to probabilistic forecasting, long-term ambulatory data and wearables (content from memory). — [Kuhlmann et al. 2018, Nat Rev Neurol 14:618–630](https://doi.org/10.1038/s41582-018-0055-2) [V]
  - "A forward-looking review of seizure prediction". — [Freestone, Karoly & Cook 2017, Curr Opin Neurol 30:167–173](https://doi.org/10.1097/wco.0000000000000429) [V]
- **Seizure prediction characteristic** (Winterhalder et al.; definitions from memory). A prediction is correct if the seizure falls in the *seizure occurrence period* (SOP), which begins after the *seizure prediction horizon* (SPH). Performance is sensitivity as a function of the *false prediction rate* FPR (per hour) for given SOP and SPH; time in warning is an equivalent alternative. — [Winterhalder et al. 2003, Epilepsy Behav 4:318–325](https://doi.org/10.1016/s1525-5050(03)00105-7) [V]
- **Chance level** (Snyder et al. and the Schelter group; formulas from memory, equation numbers not verified):
  - A Poisson random predictor with alarm rate FPR covers a seizure with probability **p ≈ 1 − exp(−FPR·SOP)**.
  - The chance of predicting ≥ k of K seizures is **Σ_{j≥k} C(K,j) p^j (1 − p)^(K−j)**.
  - Improvement over chance is sensitivity − p, or sensitivity − time in warning for warning-based systems.
  - [Snyder et al. 2008, J Neural Eng 5:392–401](https://doi.org/10.1088/1741-2560/5/4/004) [V]
- **Seizure-time surrogates**: permute seizure onset times (respecting constraints) and re-evaluate. The original performance must exceed the surrogate distribution (content from memory). — [Andrzejak et al. 2003, PRE 67:010901](https://doi.org/10.1103/physreve.67.010901) [V]
- The Melbourne–NeuroVista trial was the first-in-human prospective study of an implanted seizure advisory system: long-term intracranial EEG with patient-specific algorithms issuing low, moderate and high likelihood advisories in drug-resistant epilepsy (n = 15; content from memory, detailed performance figures not re-checked). — [Cook et al. 2013, Lancet Neurol 12:563–571](https://doi.org/10.1016/s1474-4422(13)70075-9) [V]
- Crowdsourcing:
  - Kaggle contests (American Epilepsy Society 2014, dogs and humans; Melbourne University AES/MathWorks/NIH 2016). — [Brinkmann et al. 2016, Brain 139:1713–1722](https://doi.org/10.1093/brain/aww045) [V]
  - The Epilepsy Ecosystem continues the 2016 contest on NeuroVista data, with top algorithms invited for evaluation on the full trial database; it now includes wearable data (My Seizure Gauge) ([V] provider page, see §7). Kuhlmann et al., "Epilepsyecosystem.org: crowd-sourcing reproducibility in seizure prediction and forecasting", Brain 2018 — DOI believed to be 10.1093/brain/awy210 [M]; Crossref returned no match for the query this session.

#### Inferences
- The expected precursor depends on the onset bifurcation:
  - SN onset from a fixed point: slowing of recovery (λ → 0), if the slow variable drifts towards the bifurcation;
  - SupH onset: growing oscillatory fluctuations at a fixed frequency, with spectral narrowing rather than lag-1 autocorrelation;
  - SubH and noise-induced jumps across a separatrix: none.

  This plausibly explains the mixed CSD evidence. A detector should report *which* transition class it assumes.
- Cycles are both signal and confound. An indicator that rises before seizures may be tracking a multidien rhythm. Any CSD claim must beat a cycle-only baseline (phase of IEA or of the indicator's own rhythm) and seizure-time surrogates.
- Kramer 2012 shows termination is the cleaner critical transition in human data and is a good demonstration target.

#### Gaps
- The Maturana 2020 forecasting metrics (AUC, number of patients better than chance) and the Saggio 2020 class frequencies could not be read; the eLife and PMC pages were blocked this session.
- Cook 2013 and Proix 2021 performance numbers were not re-checked.
- No peer-reviewed 2024–2026 replication settling the Wilkat (negative) vs Maturana (positive) disagreement was found.
- No regulatory-approved seizure *forecasting* device was confirmed this session.

### 3. Critical Slowing in Mood Before Transitions in Depression Is Detectable in a Minority of People

#### Takeaway
The founding study (van de Leemput et al. 2014) found that elevated autocorrelation, variance and correlation between emotions in experience-sampling data relate to the probability of upcoming shifts into and out of depression. Single-subject studies (Wichers 2016, 2020) showed it in individuals. Prospective tests in the TRANS-ID programme find warning signals before transitions in only a minority: about one third before depression recurrence (Smit et al., abstract via secondary source). A 2024 perspective argues there is little support for clinical use. The approach remains a research tool with no clinical validation.

#### Cited Findings
- In a large group of healthy individuals and patients, "the probability of an upcoming shift between a depressed and a normal state is related to elevated temporal autocorrelation, variance, and correlation between emotions in fluctuations of autorecorded emotions" (abstract). — [van de Leemput et al. 2014, PNAS 111:87–92](https://doi.org/10.1073/pnas.1312114110) [V]
- Two comment letters followed in PNAS (PubMed lists E878 and E879). Bos & de Jonge: "'Critical slowing down in depression' is a great idea that still needs empirical proof" (content from memory: the original effects were small, group-level and not person-specific). — [Bos & de Jonge 2014, PNAS 111:E878](https://doi.org/10.1073/pnas.1323672111) [V]
- Single patient: rising autocorrelation, variance and network connectivity in momentary affect preceded a transition during antidepressant dose reduction (content from memory). — [Wichers et al. 2016, Psychother Psychosom 85:114–116](https://doi.org/10.1159/000441458) [V]
- A confirmatory single-subject time-series replication. — [Wichers, Smit & Snippe 2020, J Pers Oriented Res 6:1–15](https://doi.org/10.17505/jpor.2020.22042) [V]
- TRANS-ID (Groningen, ERC project 681466):
  - Among individuals receiving treatment for depression, part of the sample showed CSD, but early warning signals may have limited value as a personalised prediction method ([S] via search summary). — [Helmich et al. 2023, Clin Psychol Sci 11:994–1010](https://doi.org/10.1177/21677026221137006) [V] (online 2022)
  - Recurrence study, preprint "Critical slowing down in momentary affect as early warning signal of impending transitions in depression". — [Smit et al. 2022, PsyArXiv](https://doi.org/10.31234/osf.io/u6vdg) [V record]. A 2025 journal version reports that rising autocorrelation and variance preceded recurrence in **32.9%** of participants across robustness checks ([S], from the Tilburg University portal via search; journal and DOI not verified) — [Tilburg portal](https://research.tilburguniversity.edu/en/publications/critical-slowing-down-in-momentary-affect-as-early-warning-signal/) [S]
  - EU reporting describes variability and complexity measures as distinguishing recurrence from remission ([S]). — [CORDIS 681466](https://cordis.europa.eu/project/id/681466/reporting) [S]
- Helmich et al. (2024, Nature Reviews Psychology perspective, "Slow down and be critical before using early warning signals in psychopathology") argue there is little support for EWS-based CSD in clinical settings ([S] via search; record not verified). — [listing](https://ouci.dntb.gov.ua/works/40Nq3Pp9) [S]
- Challenges and recommendations, and the theoretical and practical conditions for EWS in psychological systems. Psychological transitions need not be bifurcations; sampling frequency, measurement error and nonstationarity limit detectability (content from memory). — [Helmich et al. 2021, Curr Opin Psychol 41:51–58](https://doi.org/10.1016/j.copsyc.2021.02.008) [V]; [Dablander et al. 2023, Psychol Methods 28:765–790](https://doi.org/10.1037/met0000450) [V]
- Other approaches:
  - Rising EWS in affect associated with future changes in depression. — [Curtiss et al. 2021, Psychol Med 53:3124–3132](https://doi.org/10.1017/s0033291721005183) [V]
  - Anticipating the *direction* of symptom change with CSD (proof of concept). — [Schreuder et al. 2022, BMC Psychiatry 22:49](https://doi.org/10.1186/s12888-022-03686-9) [V]
  - Critical fluctuations (dynamic complexity) before sudden gains and losses in psychotherapy. — [Olthof et al. 2020, Clin Psychol Sci 8:25–35](https://doi.org/10.1177/2167702619865969) [V]
- Framing reviews: a dynamical-systems view of psychiatric disorders, theory and practical implications; static vs dynamic models of onset. — [Scheffer et al. 2024a, JAMA Psychiatry 81:618](https://doi.org/10.1001/jamapsychiatry.2024.0215) [V]; [Scheffer et al. 2024b, JAMA Psychiatry 81:624](https://doi.org/10.1001/jamapsychiatry.2024.0228) [V]; [Nelson et al. 2017, JAMA Psychiatry 74:528](https://doi.org/10.1001/jamapsychiatry.2017.0001) [V]
- Bipolar disorder: CSD as an early warning of episode transitions was shown in a simulation with a circadian activity-rhythm model (title-level). Empirical bipolar tests were not found this session. — [Bayani et al. 2017, Chronobiol Int 34:235–245](https://doi.org/10.1080/07420528.2016.1272608) [V]

#### Inferences
- Rough sensitivity is about one in three people before recurrence, with false positives not stated in the snippet. That is far from a usable alarm. Psychological data are short (tens to a few hundred samples), irregular and ordinal, which is the regime where Kendall-τ trend tests have little power (Boettiger & Hastings 2012, §6).
- A library demonstration on mood data should present EWS as hypothesis tests with power analysis and person-specific nulls, not as predictions.

#### Gaps
- The TRANS-ID recurrence paper's journal, n and false-positive rate were not verified. The Helmich 2024 Nature Reviews Psychology record was not verified via Crossref.
- No open ESM dataset with labelled transitions and clear licence was identified this session. Some TRANS-ID and Wichers single-subject data have reportedly been shared (from memory; unverified).

### 4. Loss of Consciousness Under Anaesthesia and in Sleep Moves the Brain Away From Criticality, With Hysteresis at the Transition

#### Takeaway
Consciousness tracks near-critical dynamics. Unconscious states under propofol and xenon depart from avalanche criticality and from the edge of chaos, while ketamine, which preserves dreaming, does not. Resting-state criticality measures predict the perturbational complexity index (PCI). The aperiodic exponent steepens with propofol and in NREM sleep but is confounded by GABAergic synaptic kinetics. Induction and emergence show hysteresis ("neural inertia") in animals. Human evidence is contested.

#### Cited Findings
- Near-critical slow cortical electrodynamics support consciousness: across anaesthesia, seizures, coma and psychedelics, conscious states sit closer to a critical point in a measure of low-frequency chaoticity (abstract, partial; detailed content from memory). — [Toker et al. 2022, PNAS 119:e2024455119](https://doi.org/10.1073/pnas.2024455119) [V via PubMed]
- Under propofol and xenon, unconsciousness distances the brain from both avalanche criticality and the edge of chaos, while ketamine keeps near-critical dynamics. Resting-state criticality predicted individual PCI_max with mean absolute error below 7% (preprint abstract). Published as Maschke et al. 2024, Communications Biology 7:946 ([S]; DOI not verified). — [bioRxiv 2023.10.26.564247](https://www.biorxiv.org/content/10.1101/2023.10.26.564247.full.pdf) [S]; [DOAJ listing](https://doaj.org/article/0e60359d5c29489fbfcc4f7cf8ec9235) [S]
- **PCI** (Casali et al.; from memory). Perturb cortex with TMS, estimate cortical sources, binarise significant source activity by a bootstrap statistic, compute Lempel–Ziv complexity of the binary spatiotemporal matrix, and normalise by its length and source entropy. An empirical cut-off **PCI\* ≈ 0.31** separated conscious from unconscious conditions in a benchmark population and stratified unresponsive patients (from memory). — [Casali et al. 2013, Sci Transl Med 5 (article no. 198ra105 from memory)](https://doi.org/10.1126/scitranslmed.3006294) [V]; [Casarotto et al. 2016, Ann Neurol 80:718–729](https://doi.org/10.1002/ana.24779) [V]
- Aperiodic exponent:
  - It indexes arousal: steeper in NREM, REM and propofol anaesthesia than in wake (content from memory). — [Lendner et al. 2020, eLife 9:e55092](https://doi.org/10.7554/elife.55092) [V]
  - It indexes consciousness during unresponsiveness: propofol and xenon steepen it, ketamine does not (content from memory). — [Colombo et al. 2019, NeuroImage 189:631–644](https://doi.org/10.1016/j.neuroimage.2019.01.024) [V]
  - The confound with synaptic kinetics is in §1.5. — [Brake et al. 2024](https://doi.org/10.1038/s41467-024-45922-8) [V]
- fMRI signatures of unconsciousness (sleep, anaesthesia) are consistent with a departure from critical dynamics (content from memory). — [Tagliazucchi et al. 2016, J R Soc Interface 13:20151027](https://doi.org/10.1098/rsif.2015.1027) [V]
- Brain connectivity dissociates responsiveness from drug exposure across propofol-induced transitions (content from memory). Its sedation EEG data have been shared (from memory; terms unverified). — [Chennu et al. 2016, PLoS Comput Biol 12:e1004669](https://doi.org/10.1371/journal.pcbi.1004669) [V]
- Hysteresis:
  - "Neural inertia" is a behavioural state barrier impeding transitions between anaesthetic-induced unconsciousness and wakefulness, independent of pharmacokinetics in mice and flies (content from memory). — [Friedman et al. 2010, PLoS ONE 5:e11903](https://doi.org/10.1371/journal.pone.0011903) [V]
  - A stochastic account of neural inertia. — [Proekt & Hudson 2018, Br J Anaesth 121:86–94](https://doi.org/10.1016/j.bja.2018.02.035) [V]
  - A test of neural inertia in humans during general anaesthesia (result not re-checked; reported as contested, from memory). — [Kuizenga et al. 2018, Br J Anaesth 120:525–536](https://doi.org/10.1016/j.bja.2017.11.072) [V]
- Sleep: §1.3 and §1.9 (Xu 2024; Meisel 2013).

#### Inferences
- Anaesthesia is the best-controlled human critical transition: a slow, known control parameter (effect-site concentration), repeated induction and emergence, and ground-truth responsiveness. It is ideal for testing whether a detector shows slowing before loss of responsiveness and hysteresis between induction and emergence. An open intraoperative record is VitalDB (CC BY 4.0, §7), though its EEG is mainly processed BIS-related channels (content from memory).
- Ketamine is the natural negative control for "unresponsive = subcritical".

#### Gaps
- The Maschke 2024 DOI and the Kuizenga 2018 conclusion were not verified.
- No open, high-density EEG anaesthesia dataset with an explicit licence was confirmed this session.

### 5. Physiological Deterioration Shows Slowing of Recovery, Loss of Variability and Network Uncoupling, With One Randomised Success

#### Takeaway
In physiology the evidence is scattered and mostly retrospective:
- generic slowing-of-recovery arguments (Olde Rikkert 2016; Scheffer 2018);
- frailty linked to raised variance and cross-correlation of self-rated health (Gijzel 2017);
- small ICU studies of autocorrelation before re-intubation (Khalil 2025);
- a model-forecast "surprise loss" beating autocorrelation and variance before septic shock (Ghalati 2019);
- asthma and cardiac alternans as dynamical transitions (Frey 2005; Venegas 2005; Quail 2015).

The only randomised evidence of clinical benefit is heart-rate-characteristics (HRC) monitoring in very-low-birth-weight neonates (Moorman 2011). It uses reduced variability and decelerations, not CSD per se.

#### Cited Findings
- Slowing of recovery is proposed as a generic risk marker for acute severity transitions in chronic diseases, with examples across organ systems (content from memory). — [Olde Rikkert et al. 2016, Crit Care Med 44:601–606](https://doi.org/10.1097/ccm.0000000000001564) [V]
- Wearables now allow resilience to be estimated "from the pattern of microrecoveries observed in natural time series" (abstract). — [Scheffer et al. 2018, PNAS 115:11883–11890](https://doi.org/10.1073/pnas.1810630115) [V]
- Frailty:
  - In 22 institutionalised older adults monitored for 100 days, frail individuals showed higher variance and stronger cross-correlations between self-rated physical, mental and social health (all P < 0.001); temporal autocorrelation was not associated (abstract). — [Gijzel et al. 2017, J Gerontol A 72:991–996](https://doi.org/10.1093/gerona/glx065) [V]
  - Dynamical indicators in postural balance relate to successful ageing (title-level). — [Gijzel et al. 2019, J Gerontol A 74:1119–1126](https://doi.org/10.1093/gerona/gly170) [V]
  - Lessons from physiological time series in geriatric inpatients (preprint record; journal version in Exp Gerontol 2021 not verified). — [Rector et al. 2020, medRxiv](https://doi.org/10.1101/2020.12.04.20243121) [V record]
  - The JAGS paper "Resilience in clinical care" (Gijzel et al. 2017) was not matched by Crossref [M].
- ICU:
  - In paediatric patients after extubation, heart-rate autocorrelation rose significantly more often, and by more, in those who failed extubation and needed re-intubation within 48 h. Adding these magnitudes improved a logistic model over the mean and SD of vital signs. "While immediate clinical utility is limited…" (abstract). — [Khalil et al. 2025, PLoS ONE 20:e0317211](https://doi.org/10.1371/journal.pone.0317211) [V via PubMed]
  - "Surprise loss" (out-of-sample forecast degradation of an unsupervised model) on MIMIC-III placed critical transitions a median of over 35 h before septic shock and "outperformed other indicators of critical transition…, such as temporal autocorrelation and variance" (abstract; retrospective). — [Ghalati et al. 2019, Sci Rep 9:12888](https://doi.org/10.1038/s41598-019-49006-2) [V via PubMed]
  - Ecological indicators (lag-1 autocorrelation, SD, skewness, kurtosis, heteroskedasticity) and entropy on the heart rate of six MIMIC-II septic patients ([S], conference paper). — [Almeida & Nabney, EMBC 2016](https://publications.aston.ac.uk/id/eprint/28294) [S]
  - Machine-learning sepsis prediction (AISE) and the PhysioNet/CinC 2019 challenge, which supplies open hourly ICU data. — [Nemati et al. 2018, Crit Care Med 46:547–553](https://doi.org/10.1097/ccm.0000000000002936) [V]; [Reyna et al. 2020, Crit Care Med 48:210–217](https://doi.org/10.1097/ccm.0000000000004145) [V]
  - Uncoupling of biological oscillators in multiple organ dysfunction; "the community of the self" (content from memory). — [Godin & Buchman 1996, Crit Care Med 24:1107–1116](https://doi.org/10.1097/00003246-199607000-00008) [V]; [Buchman 2002, Nature 420:246–251](https://doi.org/10.1038/nature01260) [V]
- Neonatal sepsis and the one randomised result:
  - Abnormal heart-rate characteristics (reduced variability, transient decelerations) precede neonatal sepsis (content from memory). — [Griffin & Moorman 2001, Pediatrics 107:97–104](https://doi.org/10.1542/peds.107.1.97) [V]
  - A randomised trial of HRC monitoring in very-low-birth-weight neonates reduced mortality (content from memory: about 3,000 infants, about 10% to 8%). — [Moorman et al. 2011, J Pediatr 159:900–906](https://doi.org/10.1016/j.jpeds.2011.06.044) [V]
- Heart and lungs:
  - Fractal loss: DFA of heartbeat intervals; "loss of complexity" with ageing; fractal dynamics altered with disease; multiscale entropy (content from memory). — [Peng et al. 1995](https://doi.org/10.1063/1.166141) [V]; [Lipsitz & Goldberger 1992, JAMA 267:1806](https://doi.org/10.1001/jama.1992.03480130122036) [V]; [Goldberger et al. 2002, PNAS 99:2466](https://doi.org/10.1073/pnas.012579499) [V]; [Costa et al. 2002, PRL 89:068102](https://doi.org/10.1103/physrevlett.89.068102) [V]
  - Asthma: fluctuation analysis of daily peak expiratory flow predicts the risk of severe episodes; airway patchiness self-organises as a prelude to catastrophic shifts (content from memory). — [Frey et al. 2005, Nature 438:667–670](https://doi.org/10.1038/nature04176) [V]; [Venegas et al. 2005, Nature 434:777–782](https://doi.org/10.1038/nature03490) [V]
  - Cardiac rhythm: predicting the onset of period-doubling (alternans) bifurcations in noisy cardiac systems (content from memory). Paroxysmal atrial fibrillation onset was the 2001 PhysioNet/CinC challenge (data: afpdb, §7). — [Quail, Shrier & Glass 2015, PNAS 112:9358–9363](https://doi.org/10.1073/pnas.1424320112) [V]; [Moody et al. 2001, Computers in Cardiology 28:113–116](https://doi.org/10.1109/cic.2001.977604) [V]
- Transfer of ecological concepts to systems medicine (review). — [Trefois et al. 2015, Curr Opin Biotechnol 34:48–55](https://doi.org/10.1016/j.copbio.2014.11.020) [V]

#### Inferences
- Physiological signals sit under strong control loops and clinical interventions (drugs, ventilator changes). Treatment is an uncontrolled perturbation that can masquerade as, or mask, slowing. Detectors on ICU data must condition on interventions.
- Model-based indicators (forecast degradation, fitted recovery rates) may outperform raw variance and autocorrelation (Ghalati 2019). A library should implement both and compare them.

#### Gaps
- No large prospective trial comparing CSD indicators with standard early-warning scores (MEWS, NEWS, SOFA) was found.
- AF-onset CSD studies with verified records were not gathered this session.

### 6. Structured States Call for Multivariate Indicators: Covariance Spectra, Dynamical Network Biomarkers, Fitted Jacobians and Riemannian Geometry

#### Takeaway
For multichannel EEG/MEG/iEEG, connectivity matrices and fields on the cortex, the critical mode is a direction in state space. Indicators therefore become spectral:
- the largest eigenvalue of the covariance or of a fitted propagator;
- variance explained by the leading mode;
- degenerate fingerprinting (AR(1) of the projection onto the leading EOF);
- the dynamical network biomarker index;
- distances on the manifold of SPD matrices.

A systematic comparison found no single best multivariate indicator; the choice depends on the scenario (Weinans 2021). In brains, channel choice is unknown a priori, which motivates network-wide and spatial measures. Biological rhythms confound network features.

#### Cited Findings
- **Linear theory** (standard; Scheffer et al.; Dakos et al.; formulas from memory):
  - Near a stable state with leading eigenvalue λ < 0, dx = λx dt + σ dW. Sampling at Δt gives AR(1) with ρ = e^(λΔt) and Var = σ²/(2|λ|).
  - As λ → 0⁻, ρ → 1 and Var → ∞.
  - Trends in rolling ρ and Var are tested by Kendall's τ against surrogates.
  - [Scheffer et al. 2009, Nature 461:53–59](https://doi.org/10.1038/nature08227) [V]; [Scheffer et al. 2012, Science 338:344–348](https://doi.org/10.1126/science.1225244) [V]; [Dakos et al. 2012, PLoS ONE 7:e41010](https://doi.org/10.1371/journal.pone.0041010) [V]; [Wissel 1984, Oecologia 65:101–107](https://doi.org/10.1007/bf00384470) [V]
- **Multivariate extension** (standard; formulas from memory):
  - For dX = AX dt + B dW, the stationary covariance C solves the Lyapunov equation **AC + CAᵀ + BBᵀ = 0**.
  - As Re λ_max(A) → 0, the leading eigenvalue of C diverges along the critical mode when A is normal. With non-normal A this alignment can fail.
  - A VAR(1) fit x_{t+1} = Φx_t + ε gives Φ = e^(AΔt), and the spectral radius ρ(Φ) → 1 is a direct multivariate CSD indicator.
  - Covariance eigenvalues as EWS: [Chen et al. 2019, Sci Rep 9:2572](https://doi.org/10.1038/s41598-019-38961-5) [V]
  - Degenerate fingerprinting: [Held & Kleinen 2004, GRL 31](https://doi.org/10.1029/2004gl020972) [V]
- Comparing multivariate indicators: "there is not one method outperforming the others"; the best depends on the scenario. Guidelines are offered (abstract). — [Weinans et al. 2021, Sci Rep 11:9148](https://doi.org/10.1038/s41598-021-87839-y) [V]
- **Dynamical network biomarkers** (Chen et al.):
  - A model-free index for an imminent bifurcation from few samples with many measurements, demonstrated on microarray data of three diseases (abstract).
  - The DNB criteria (from memory): within a dominant group, SD rises sharply and Pearson correlation PCC_in rises, while correlation with the rest PCC_out falls. Composite index **I = SD_d · PCC_in / PCC_out** (form from memory).
  - [Chen et al. 2012, Sci Rep 2:342](https://doi.org/10.1038/srep00342) [V]
- DNB applications:
  - pulmonary metastasis of hepatocellular carcinoma: [Yang et al. 2018, Nat Commun 9:678](https://doi.org/10.1038/s41467-018-03024-2) [V]
  - influenza outbreak: [Chen et al. 2019, J Cell Mol Med 23:395–404](https://doi.org/10.1111/jcmm.13943) [V]
  - landscape DNB: [Liu et al. 2019, Natl Sci Rev 6:775–785](https://doi.org/10.1093/nsr/nwy162) [V]
- Single-sample DNB variants exist (sKLD, landscape entropy), validated mostly on simulations and retrospective omics, without prospective clinical validation in the results found ([S], search summary). — [PMC6988219](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC6988219/) [S]
- Deep learning for EWS: trained on simulated bifurcations, classifies the approaching bifurcation type (fold, Hopf, transcritical). Implemented in `ewstools` (MIT) [V via GitHub API]. — [Bury et al. 2021, PNAS 118:e2106140118](https://doi.org/10.1073/pnas.2106140118) [V]
- **Riemannian geometry of connectivity** (formulas standard, from memory):
  - Affine-invariant distance **δ(A,B) = ‖log(A^(−1/2) B A^(−1/2))‖_F = (Σ ln² λᵢ(A⁻¹B))^(1/2)**.
  - Log-Euclidean distance **‖log A − log B‖_F**.
  - The Fréchet mean, and tangent-space projection at a reference.
  - [Pennec et al. 2006, IJCV 66:41–66](https://doi.org/10.1007/s11263-005-3222-z) [V]; [Arsigny et al. 2007, SIAM J Matrix Anal Appl 29:328–347](https://doi.org/10.1137/050637996) [V]
  - Applied to EEG covariance in BCI: [Barachant et al. 2012, IEEE TBME 59:920–928](https://doi.org/10.1109/tbme.2011.2172210) [V]. Implementation `pyRiemann`, BSD-3-Clause [V via GitHub API].
- Brain networks before seizures:
  - Seizure precursors arise from specific spatial–temporal modifications of evolving large-scale epileptic brain networks (content from memory). — [Rings, von Wrede & Lehnertz 2019, Sci Rep 9:10623](https://doi.org/10.1038/s41598-019-47092-w) [V]
  - Fragility as eigenvalue-based instability of fitted linear models (§2.3). — [Li et al. 2021](https://doi.org/10.1038/s41593-021-00901-w) [V]
- Spatial spread and channel choice:
  - The best channel cannot be known a priori ([S]). — [Liu et al. 2024](https://doi.org/10.23919/fusion59988.2024.10706283) [V record]
  - Kramer 2012 reports termination signatures "at various spatial scales" (abstract). — [Kramer et al. 2012](https://doi.org/10.1073/pnas.1210047110) [V]
  - A seizure-onset-zone-specific spatial CSD study was not found in this session's searches.
- **Taxonomy of tipping**: bifurcation (B), noise-induced (N) and rate-induced (R). Only B-tipping guarantees CSD. The mathematical framework is fast–slow stochastic systems. — [Ashwin et al. 2012, Phil Trans R Soc A 370:1166–1184](https://doi.org/10.1098/rsta.2011.0306) [V]; [Kuehn 2011, Physica D 240:1020–1035](https://doi.org/10.1016/j.physd.2011.02.012) [V]
- Limits:
  - Quantifying limits to detection: conditioning on transitions that occurred (a prosecutor's-fallacy-like selection) inflates apparent performance; false alarms are often uncounted (content from memory). — [Boettiger & Hastings 2012, J R Soc Interface 9:2527–2539](https://doi.org/10.1098/rsif.2012.0125) [V]
  - A recent cross-domain review of tipping-point detection. — [Dakos et al. 2024, Earth Syst Dyn 15:1117–1135](https://doi.org/10.5194/esd-15-1117-2024) [V]

#### Inferences
- For cortical fields, the natural indicators are:
  - the spectrum of the fitted propagator on a parcellation, with the eigenvector giving *where* the slowing is;
  - spatial variance and spatial autocorrelation over the parcellation graph;
  - the leading covariance mode projected onto the cortex.

  Fragility (Li 2021) is effectively a node-resolved distance-to-instability of the fitted propagator. It suits a library that already has Lyapunov exponents and transfer operators.
- Connectivity matrices should be treated on the SPD manifold. A trajectory's speed and its distance from a baseline Fréchet mean are state-change indicators, and the eigenvalues of A⁻¹B decompose that change.
- Non-normality matters for brain networks with asymmetric coupling: covariance-based indicators can mislead where propagator-based ones do not.

#### Gaps
- No verified study directly compares covariance-spectrum, DNB and fragility indicators on the same seizure data.
- Spatial EWS such as Moran's I on cortical parcellations before seizures were not found.

### 7. Open Records Allow Honest Demonstrations, Each Under Its Own Terms

#### Takeaway
Open (attribution-only) records exist for scalp seizures (CHB-MIT ODC-By; Siena CC BY 4.0), intracranial seizures and SOZ (OpenNeuro CC0: ds003029, ds003876, ds004100, ds004752, ds005398), sleep (Sleep-EDF and CAP, ODC-By), sleep deprivation (OpenNeuro ds004902, CC0), depression rest EEG (ds003478, CC0), sepsis (PhysioNet 2019, CC BY 4.0), AF onset (afpdb, ODC-By), HRV and ageing (Fantasia, ODC-By) and intraoperative anaesthesia (VitalDB, CC BY 4.0).

The rest is gated or limited:
- TUH requires a signed form;
- the Epilepsy Ecosystem requires registration and states no licence on its homepage;
- CRCNS forbids redistribution and limits use to science;
- the Bonn segments ask only for citation, with no licence;
- MIMIC-IV and eICU need credentialing, CITI training and a data use agreement;
- Allen Institute data are noncommercial.

#### Cited Findings
- **CHB-MIT Scalp EEG** (PhysioNet): "Anyone can access the files, as long as they conform to the terms of the specified license. License: Open Data Commons Attribution License v1.0"; DOI 10.13026/C2K01R. — [PhysioNet chbmit](https://physionet.org/content/chbmit/1.0.0/) [V]. Paediatric long-term scalp EEG with annotated seizures; content (≈23 cases, 256 Hz) from memory. Cite PhysioNet: [Goldberger et al. 2000, Circulation 101:e215](https://doi.org/10.1161/01.cir.101.23.e215) [V].
- **Siena Scalp EEG**: CC BY 4.0, DOI 10.13026/5d4a-j060; 14 patient folders (PN00–PN17 listing) [V]. — [PhysioNet siena-scalp-eeg](https://physionet.org/content/siena-scalp-eeg/1.0.0/) [V]; [Detti et al. 2020, Processes 8:846](https://doi.org/10.3390/pr8070846) [V]
- **TUH EEG Corpus (TUEG, TUSZ, …)**: fill in the form and email a signed copy to help@nedcdata.org; credentials follow in about 24–48 h. As of January 2026, corpora are distributed by ssh keys and rsync; the TUSZ path is v2.0.6 ([S], search summary of the provider page, which timed out when fetched). — [NEDC TUH EEG](https://isip.piconepress.com/projects/nedc/html/tuh_eeg/) [S]; [Obeid & Picone 2016, Front Neurosci 10:196](https://doi.org/10.3389/fnins.2016.00196) [V]; [Shah et al. 2018, Front Neuroinform 12:83](https://doi.org/10.3389/fninf.2018.00083) [V]
- **Epilepsy Ecosystem** (Melbourne): NeuroVista long-term human iEEG for a crowd-sourced contest, plus wearable data (My Seizure Gauge). The homepage shows Register/Login and Terms and Conditions links but states no data licence ([V] provider page; terms page not read). — [epilepsyecosystem.org](https://www.epilepsyecosystem.org/) [V]
- **Kaggle seizure prediction contests** (2014 AES; 2016 Melbourne University): Kaggle account and competition rules apply (from memory). — [Brinkmann et al. 2016](https://doi.org/10.1093/brain/aww045) [V]
- **Bonn EEG**: five sets A–E (Z, O, N, F, S), 100 single-channel segments each, 4,096 samples at 173.61 Hz (≈23.6 s), 0.5–85 Hz. The only stated condition is "Please make sure that you cite the paper … Phys. Rev. E, 64, 061907"; no licence is named ([V] provider page). — [Univ. Bonn downloads](https://www.ukbonn.de/epileptologie/arbeitsgruppen/ag-lehnertz-neurophysik/downloads/) [V]; [Andrzejak et al. 2001, PRE 64:061907](https://doi.org/10.1103/physreve.64.061907) [V]. *Caveat:* short isolated segments; unsuitable for anticipation, which needs continuous pre-ictal data.
- **OpenNeuro datasets**, all "License: CC0" per their `dataset_description.json` ([V] via the OpenNeuroDatasets GitHub mirrors):

  | Dataset | Content | Note |
  |---|---|---|
  | ds003029 | Epilepsy-iEEG-Multicenter-Dataset | Li et al.; fragility |
  | ds003876 | Epilepsy-iEEG-Interictal-Multicenter-Dataset | |
  | ds004100 | HUP iEEG Epilepsy Dataset | Bernabei et al. |
  | ds004752 | iEEG, scalp EEG and beamformed sources from epilepsy patients | Dimakopoulos et al. |
  | ds005398 | Open iEEG Dataset | Paediatric; Wayne State University and UCLA |
  | ds004902 | Resting-state EEG Dataset for Sleep Deprivation | |
  | ds003478 | EEG: Depression rest | Cavanagh |
  | ds002778 | UC San Diego resting EEG in Parkinson's disease | |

  — [OpenNeuro](https://openneuro.org/); [Markiewicz et al. 2021, eLife 10:e71774](https://doi.org/10.7554/elife.71774) [V]; [Bernabei et al. 2023, Brain 146:2248–2258](https://doi.org/10.1093/brain/awad007) [V]
- **Sleep-EDF Expanded**: ODC-By v1.0, DOI 10.13026/C2X676. — [PhysioNet sleep-edfx](https://physionet.org/content/sleep-edfx/1.0.0/) [V]; [Kemp et al. 2000, IEEE TBME 47:1185–1194](https://doi.org/10.1109/10.867928) [V]
- **CAP Sleep Database**: 108 polysomnograms (Parma), ODC-By v1.0. — [PhysioNet capslpdb](https://physionet.org/content/capslpdb/1.0.0/) [V]
- **PhysioNet/CinC 2019 (sepsis)**: CC BY 4.0. — [PhysioNet challenge-2019](https://physionet.org/content/challenge-2019/1.0.0/) [V]; [Reyna et al. 2020](https://doi.org/10.1097/ccm.0000000000004145) [V]
- **MIMIC-IV and eICU-CRD**: "Only credentialed users who sign the DUA can access the files"; PhysioNet Credentialed Health Data License 1.5.0; required training CITI "Data or Specimens Only Research". Derived datasets must be shared under the same agreement. — [PhysioNet mimiciv](https://physionet.org/content/mimiciv/3.1/) [V]; [PhysioNet eicu-crd](https://physionet.org/content/eicu-crd/2.0/) [V]; [Johnson et al. 2023, Sci Data 10:1](https://doi.org/10.1038/s41597-022-01899-x) [V]; [Pollard et al. 2018, Sci Data 5:180178](https://doi.org/10.1038/sdata.2018.178) [V]
- **AF and HRV**: PAF Prediction Challenge DB (afpdb), Long-Term AF DB (ltafdb) and Fantasia (young vs elderly), all ODC-By v1.0. — [afpdb](https://physionet.org/content/afpdb/1.0.0/) [V]; [ltafdb](https://physionet.org/content/ltafdb/1.0.0/) [V]; [fantasia](https://physionet.org/content/fantasia/1.0.0/) [V]
- **VitalDB** (intraoperative multiparameter vital signs, anaesthesia), CC BY 4.0 on PhysioNet. — [PhysioNet vitaldb](https://physionet.org/content/vitaldb/1.0.0/) [V]; [Lee et al. 2022, Sci Data 9:279](https://doi.org/10.1038/s41597-022-01411-5) [V]
- **CRCNS**: "Data is made available only for scientific purposes. Redistribution of the data is not permitted (except to direct collaborators … or to students in your classroom). Any publications derived from the data must state the data contributors and CRCNS.org as being the source … [and] cite the original paper(s)", unless a dataset page says otherwise ([V] provider page). Download needs an account (site shows Log in/Register; not stated in the terms). The ssc-3 dataset (thousands of neurons, rat somatosensory cultures) is commonly used for avalanche work (from memory). — [CRCNS terms](https://crcns.org/terms) [V]; [Teeters et al. 2008, Neuroinformatics 6:47–55](https://doi.org/10.1007/s12021-008-9009-y) [V]
- **DANDI**: per-dataset CC0 or CC-BY; metadata CC0 ([S]). — [DANDI policies](https://docs.dandiarchive.org/terms-policies/policies/) [S]. **Allen Brain Observatory**: Allen Institute terms, research and noncommercial use unless agreed in writing ([S]). — [Allen terms of use](https://alleninstitute.org/terms-of-use/) [S]
- Spontaneous brainwide Neuropixels and two-photon data (Stringer et al.) are shared openly (licence from memory: CC BY 4.0 on figshare; unverified). — [Stringer et al. 2019, Science 364:eaav7893](https://doi.org/10.1126/science.aav7893) [V]

#### Inferences
- For a public, redistributable demonstration of seizure anticipation, CHB-MIT and Siena (scalp) and the OpenNeuro CC0 iEEG sets are the cleanest. The longest continuous pre-ictal records (NeuroVista) are gated. Wilkat 2019's Bonn long-term data are not public; the public Bonn set is only short segments.
- For avalanches and branching ratio, DANDI CC-BY/CC0 Neuropixels datasets avoid CRCNS's no-redistribution clause and Allen's noncommercial terms.

#### Gaps
- Epilepsy Ecosystem's terms page, TUH's form text and the Stringer data licence were not read directly.
- No open ESM mood dataset with labelled transitions and an open licence was identified.

### 8. Reference Software Is Mostly Permissively Licensed, With The Virtual Brain the Exception to Check

#### Takeaway
Permissive licences cover:
- spectra: specparam/fooof and neurodsp (Apache-2.0);
- signal processing: MNE-Python (BSD-3), NeuroKit2 (MIT);
- criticality: mrestimator (BSD-3), powerlaw (MIT), nolds and MFDFA (MIT);
- early warning signals: ewstools (MIT);
- connectivity and sleep: pyRiemann, antropy and YASA (BSD-3).

GitHub cannot classify The Virtual Brain's licence (NOASSERTION); it is understood to be GPL-3.0 (from memory) and must be checked before any code is borrowed. The NCC toolbox's licence was not found.

#### Cited Findings
- Licences per GitHub's API this session [V]:
  - `fooof-tools/fooof`: Apache-2.0
  - `mne-tools/mne-python`: BSD-3-Clause
  - `neuropsychology/NeuroKit`: MIT
  - `Priesemann-Group/mrestimator`: BSD-3-Clause
  - `jeffalstott/powerlaw`: MIT
  - `the-virtual-brain/tvb-root`: NOASSERTION
  - `ThomasMBury/ewstools`: MIT
  - `neurodsp-tools/neurodsp`: Apache-2.0
  - `pyRiemann/pyRiemann`: BSD-3-Clause
  - `raphaelvallat/antropy`: BSD-3-Clause
  - `raphaelvallat/yasa`: BSD-3-Clause
  - `CSchoel/nolds`: MIT
  - `LRydin/MFDFA`: MIT

  — [GitHub API](https://api.github.com/repos/fooof-tools/fooof)
- Not verified (rate-limited): neurolib (believed MIT), Elephant (believed BSD-3), mne-connectivity [M].
- Papers:
  - MNE-Python: [Gramfort et al. 2013, Front Neurosci 7:267](https://doi.org/10.3389/fnins.2013.00267) [V]
  - NeuroKit2: [Makowski et al. 2021, Behav Res Methods 53:1689–1696](https://doi.org/10.3758/s13428-020-01516-y) [V]
  - mrestimator: [Spitzner et al. 2021](https://doi.org/10.1371/journal.pone.0249447) [V]
  - powerlaw: [Alstott et al. 2014](https://doi.org/10.1371/journal.pone.0085777) [V]
  - NCC toolbox: [Marshall et al. 2016](https://doi.org/10.3389/fphys.2016.00250) [V]
  - specparam: [Donoghue et al. 2020](https://doi.org/10.1038/s41593-020-00744-x) [V]
  - NeuroDSP: [Cole et al. 2019, JOSS 4:1272](https://doi.org/10.21105/joss.01272) [V]
  - YASA: [Vallat & Walker 2021, eLife 10:e70092](https://doi.org/10.7554/elife.70092) [V]
  - The Virtual Brain, including the Epileptor among its models: [Sanz Leon et al. 2013, Front Neuroinform 7:10](https://doi.org/10.3389/fninf.2013.00010) [V]; [Sanz-Leon et al. 2015, NeuroImage 111:385–430](https://doi.org/10.1016/j.neuroimage.2015.01.002) [V]
  - neurolib (Wilson–Cowan, ALN and other neural masses): [Cakan et al. 2023, Cogn Comput 15:1132–1152](https://doi.org/10.1007/s12559-021-09931-9) [V]
  - ewstools: [Bury et al. 2021](https://doi.org/10.1073/pnas.2106140118) [V]
  - Wilson–Cowan model: [Wilson & Cowan 1972, Biophys J 12:1–24](https://doi.org/10.1016/s0006-3495(72)86068-5) [V]

#### Inferences
- The library can read and re-implement any of the permissive packages and cross-check against them: MR estimates against mrestimator, Clauset fits against powerlaw, aperiodic fits against specparam, DFA against nolds and MFDFA, EWS against ewstools.
- For TVB's Epileptor, re-implement from the published equations. Use TVB only as an external numerical reference, never vendored, until its licence is confirmed.

#### Gaps
- The TVB and NCC toolbox licences remain unconfirmed.

### 9. What This Means for the Design

#### Takeaway
A criticality detector for structured, multidimensional states should be a family of estimators, each with a declared assumption about the transition class (bifurcation, noise or rate), a known-answer test model, a null model and an out-of-sample protocol. It should report distance-to-instability with uncertainty rather than a verdict. The brain and body literature shows that the failures are statistical rather than mathematical:
- unchecked power laws;
- subsampling bias;
- conditioning on transitions that occurred;
- uncounted false alarms;
- cycles mistaken for precursors;
- single best channels chosen after the fact.

#### Cited Findings (detectors and test models, each tied to the sources above)

##### 9.1 Detectors to implement, by kind of state
- **Spike or event rasters (avalanche and branching family).**
  - Avalanche extraction with declared threshold and Δt, reported over a sweep of Δt.
  - Clauset fits for size and duration, with x_min, bootstrap p and likelihood ratios against lognormal, exponential and truncated alternatives.
  - An independent γ from ⟨S⟩(T), checked against (α − 1)/(τ − 1) with bootstrap confidence intervals, and shape collapse with a quality score.
  - The MR estimator m̂ and τ̂ with confidence intervals and the offset-model test for non-stationarity.
  - Sources: [Clauset 2009](https://doi.org/10.1137/070710111) [V]; [Sethna 2001](https://doi.org/10.1038/35065675) [V]; [Friedman 2012](https://doi.org/10.1103/physrevlett.108.208102) [V]; [Wilting & Priesemann 2018](https://doi.org/10.1038/s41467-018-04725-4) [V]; [Spitzner 2021](https://doi.org/10.1371/journal.pone.0249447) [V].
- **Multichannel continuous signals (EEG, MEG, iEEG).**
  - Per-band envelope DFA with declared fitting range, and fE/I.
  - specparam exponent with knee, fit range and R².
  - Univariate EWS per channel, plus multivariate EWS: the largest covariance eigenvalue, explained variance of PC1, mean absolute correlation, degenerate fingerprinting, the DNB index, and the spectral radius of a fitted VAR(1) or linear time-varying propagator, with node fragility.
  - For Hopf-type transitions, band-limited indicators: spectral peak narrowing and the power of the oscillatory mode, rather than broadband lag-1 autocorrelation.
  - Sources: [Hardstone 2012](https://doi.org/10.3389/fphys.2012.00450) [V]; [Bruining 2020](https://doi.org/10.1038/s41598-020-65500-4) [V]; [Donoghue 2020](https://doi.org/10.1038/s41593-020-00744-x) [V]; [Weinans 2021](https://doi.org/10.1038/s41598-021-87839-y) [V]; [Held & Kleinen 2004](https://doi.org/10.1029/2004gl020972) [V]; [Chen 2012](https://doi.org/10.1038/srep00342) [V]; [Li 2021](https://doi.org/10.1038/s41593-021-00901-w) [V].
- **Connectivity matrices.**
  - SPD covariance or coherence per window.
  - Affine-invariant or log-Euclidean distance to a baseline Fréchet mean, the speed of the trajectory, and the eigen-decomposition of A⁻¹B.
  - Sources: [Pennec 2006](https://doi.org/10.1007/s11263-005-3222-z) [V]; [Arsigny 2007](https://doi.org/10.1137/050637996) [V]; [Barachant 2012](https://doi.org/10.1109/tbme.2011.2172210) [V].
- **Fields on the cortex (parcellations, the Brain Atlas's sources).**
  - Spatial variance, Moran's I on the parcellation adjacency, and the critical mode (the leading eigenvector of the propagator or covariance) projected onto cortex, which says where slowing concentrates.
  - Spatial EWS for brains are not validated in the literature found (§6 Gaps); present them as exploratory.
- **Model-based indicators.**
  - Forecast degradation ("surprise loss") and fitted recovery rates after perturbations (active probing).
  - Sources: [Ghalati 2019](https://doi.org/10.1038/s41598-019-49006-2) [V]; [Kalitzin 2010](https://doi.org/10.1016/j.yebeh.2009.12.023) [V]; [Chang 2018](https://doi.org/10.1038/s41593-018-0278-y) [V].
- **Onset and offset classifier.**
  - Amplitude scaling, frequency scaling and DC shift at onset and offset, giving one of the 16 dynamotypes.
  - Sources: [Jirsa 2014](https://doi.org/10.1093/brain/awu133) [V]; [Saggio 2017](https://doi.org/10.1186/s13408-017-0050-8) [V]; [Saggio 2020](https://doi.org/10.7554/elife.55632) [V].

##### 9.2 Test models with known answers, to verify implementations against
1. **Branching process with Poisson immigration** at known m (0.9, 0.98, 0.999, 1), binomially subsampled at known α.
   - Checks: m̂_MR → m independent of α; the conventional estimator → b·m (biased); ⟨A⟩ = h/(1 − m); τ = −Δt/ln m; at m = 1, τ_S = 3/2, α_T = 2, γ = 2 and parabolic shape collapse.
   - [Wilting & Priesemann 2018](https://doi.org/10.1038/s41467-018-04725-4) [V]; [Harris 1963](https://doi.org/10.1007/978-3-642-51866-9_6) [V]; [Heathcote 1965](https://doi.org/10.1111/j.2517-6161.1965.tb00596.x) [V]
2. **Kinouchi–Copelli excitable network**: dynamic range peaks at σ = 1. — [Kinouchi & Copelli 2006](https://doi.org/10.1038/nphys289) [V]
3. **Negative controls for power laws**: independent stochastic surrogates and irregular non-critical networks (Touboul–Destexhe) and a time-varying drive on a subcritical process (Priesemann–Shriki). The detector must *not* certify criticality: power laws may appear, but the crackling relation, shape collapse or m̂ should expose them. — [Touboul & Destexhe 2017](https://doi.org/10.1103/physreve.95.012413) [V]; [Priesemann & Shriki 2018](https://doi.org/10.1371/journal.pcbi.1006081) [V]
4. **Ornstein–Uhlenbeck and AR(1) with a slowly drifting λ → 0** (fold normal form ẋ = μ − x² + noise, with μ ramped), checked against the closed forms ρ = e^(λΔt) and Var = σ²/(2|λ|). Then a **double well at fixed parameters** with noise-induced jumps, where the correct answer is "no warning". — [Scheffer 2009](https://doi.org/10.1038/nature08227) [V]; [Ditlevsen & Johnsen 2010](https://doi.org/10.1029/2010gl044486) [V]
5. **Multivariate OU, dX = A(μ)X dt + B dW**, with A's leading eigenvalue ramped to 0, in normal and non-normal variants. Covariance from the Lyapunov equation; the spectral radius of e^(AΔt) → 1; recovery of the critical eigenvector.
6. **Stochastic Wilson–Cowan model**, with saddle-node and Hopf bifurcation points located by the library's own continuation. Expect variance and AR(1) growth at the fold and spectral narrowing at the Hopf. — [Wilson & Cowan 1972](https://doi.org/10.1016/s0006-3495(72)86068-5) [V]
7. **Epileptor** (SN onset, SH offset).
   - Logarithmic ISI growth towards offset and a DC shift at onset.
   - x₀ ramped to move between interictal and ictal regimes.
   - Networked Epileptors on a connectome for spread, as in the Virtual Epileptic Patient.
   - Plus **Saggio's unfolding model**, sampling all 16 dynamotypes to test the onset/offset classifier.
   - [Jirsa 2014](https://doi.org/10.1093/brain/awu133) [V]; [Saggio 2017](https://doi.org/10.1186/s13408-017-0050-8) [V]; [Jirsa 2017](https://doi.org/10.1016/j.neuroimage.2016.04.049) [V]
8. **A neural-field Epileptor** for spatial spread and asynchronous termination, for cortical-field detectors. — [Proix 2018](https://doi.org/10.1038/s41467-018-02973-y) [V]
9. **Potts and Ising models** (already in physis) for susceptibility and correlation length at a known critical temperature, as the equilibrium analogue.
10. **An aperiodic-spectrum generator**: sums of exponentially filtered E and I synaptic currents with a known g_E/g_I. Use it to check that the fitted exponent moves as Gao et al. describe, and to show where the knee and peaks bias it. — [Gao 2017](https://doi.org/10.1016/j.neuroimage.2017.06.078) [V]; [Gerster 2022](https://doi.org/10.1007/s12021-022-09581-8) [V]

##### 9.3 Open records for honest demonstrations, with their terms
- Seizure onset and offset classification, and pre-ictal indicators on scalp EEG: CHB-MIT (ODC-By), Siena (CC BY 4.0).
- Intracranial data and node-level fragility or critical modes on cortex: OpenNeuro ds003029, ds003876, ds004100, ds004752, ds005398 (CC0).
- Termination as a critical transition: reproduce Kramer 2012's slowing, correlation and flickering on any of the above. — [Kramer 2012](https://doi.org/10.1073/pnas.1210047110) [V]
- Sleep and sleep deprivation: Sleep-EDF and CAP (ODC-By), ds004902 (CC0).
- Depression rest EEG (LRTC, aperiodic exponent): ds003478 (CC0).
- Physiology: PhysioNet 2019 sepsis (CC BY 4.0), afpdb, ltafdb and Fantasia (ODC-By), VitalDB anaesthesia (CC BY 4.0).
- Avalanches: DANDI CC0/CC-BY Neuropixels datasets.
- Gated, to be described rather than redistributed:
  - CRCNS (science-only, no redistribution);
  - TUH (signed form);
  - Epilepsy Ecosystem (registration; licence unstated);
  - MIMIC-IV and eICU (credentialed, DUA, CITI);
  - Allen (noncommercial);
  - Bonn (citation only, segments too short for anticipation).

  Sources: §7 provider pages [V/S].

##### 9.4 What every detector must report, to avoid the field's known pitfalls
1. **The assumed transition class** (B, N or R tipping; for B, fold, Hopf or other) and the observable's timescale. CSD is expected only for B-tipping, and lag-1 autocorrelation is the wrong statistic at a Hopf. — [Ashwin 2012](https://doi.org/10.1098/rsta.2011.0306) [V]; [Bury 2021](https://doi.org/10.1073/pnas.2106140118) [V]
2. **All analysis choices, fixed before seeing outcomes**: window length, detrending bandwidth, filter band, avalanche threshold, Δt and DFA fit range. Report a sensitivity sweep over them. — [Dakos 2012](https://doi.org/10.1371/journal.pone.0041010) [V]; [Hardstone 2012](https://doi.org/10.3389/fphys.2012.00450) [V]
3. **Null models.**
   - Phase-randomised or IAAFT surrogates for indicator trends (Kendall τ against the surrogate distribution).
   - Seizure-time surrogates and an analytic Poisson random predictor for event anticipation.
   - Touboul–Destexhe-type surrogates for power laws.
   - [Andrzejak 2003](https://doi.org/10.1103/physreve.67.010901) [V]; [Snyder 2008](https://doi.org/10.1088/1741-2560/5/4/004) [V]; [Touboul & Destexhe 2017](https://doi.org/10.1103/physreve.95.012413) [V]
4. **For anticipation**:
   - sensitivity, false prediction rate per hour or time in warning, SOP and SPH, and improvement over chance;
   - for probabilistic forecasts, AUC with calibration (Brier score);
   - all on held-out, continuous, unselected data with every event and every false alarm counted, per subject.
   - Compare against a cycle-only baseline (circadian or multidien phase), because risk cycles alone forecast seizures.
   - [Winterhalder 2003](https://doi.org/10.1016/s1525-5050(03)00105-7) [V]; [Mormann 2007](https://doi.org/10.1093/brain/awl241) [V]; [Baud 2018](https://doi.org/10.1038/s41467-017-02577-y) [V]; [Proix 2021](https://doi.org/10.1016/s1474-4422(20)30396-3) [V]; [Boettiger & Hastings 2012](https://doi.org/10.1098/rsif.2012.0125) [V]
5. **Observation model**: the subsampling fraction (neurons recorded vs network), the signal type (spikes, LFP, scalp EEG) and its volume conduction. A criticality claim from scalp EEG is about a coarse-grained field, not about neurons. — [Priesemann 2009](https://doi.org/10.1186/1471-2202-10-40) [V]; [Levina & Priesemann 2017](https://doi.org/10.1038/ncomms15140) [V]
6. **Quantities with uncertainty, not verdicts**: m̂ and τ̂ with confidence intervals; τ_S, α_T and γ with confidence intervals and the crackling-relation residual; the spectral radius with confidence intervals. Call the regime "consistent with near-critical / reverberating / subcritical" rather than "critical".
7. **Multiple comparisons across channels, bands and windows**, and the rule by which a channel or network was chosen. Choosing it after the fact must be forbidden or declared. — [Liu 2024](https://doi.org/10.23919/fusion59988.2024.10706283) [V record]
8. **Confounders**: rhythms (circadian, multidien, sleep stage), interventions (drugs, ventilators, stimulation) and nonstationarity of the measurement. — [Lehnertz 2021](https://doi.org/10.3389/fnetp.2021.755016) [V]; [Guendelman 2025](https://doi.org/10.1523/ENEURO.0157-24.2024) [V]
9. **Clinical status**: every output should carry "research use; not validated for diagnosis or prediction". Per the sources found, only neonatal HRC monitoring has randomised evidence of benefit ([Moorman 2011](https://doi.org/10.1016/j.jpeds.2011.06.044) [V]). Seizure advisory systems were tested in first-in-human trials ([Cook 2013](https://doi.org/10.1016/s1474-4422(13)70075-9) [V]), and PCI is a validated research index ([Casarotto 2016](https://doi.org/10.1002/ana.24779) [V]). Criticality and EWS metrics for mood, ICU or ageing are not clinically validated ([Helmich 2023](https://doi.org/10.1177/21677026221137006) [V]; [Khalil 2025](https://doi.org/10.1371/journal.pone.0317211) [V]).

#### Inferences
- The existing foundation (rolling variance, AR(1) with Kendall τ and red-noise surrogates, DFA, Clauset fits, MR branching ratio, Lyapunov exponents, bifurcation diagrams, transfer operator) covers the univariate core.
- The missing pieces, in order of value:
  1. multivariate CSD on propagators and covariances, including non-normal cases;
  2. the crackling-relation and shape-collapse test with bootstrap;
  3. a seizure-anticipation evaluation harness (SOP/SPH, IoC, seizure-time surrogates, cycle baseline);
  4. SPD-manifold state tracking;
  5. an onset and offset dynamotype classifier verified on Saggio's model;
  6. band-limited (Hopf-aware) indicators.
- The library's continuation and bifurcation tools give exact ground truth (the location of λ = 0) for the Wilson–Cowan and Epileptor tests. That is a stronger verification than most published pipelines have had.

#### Gaps
- The equation numbers of the key formulas (MR estimator, crackling relation, specparam, Clauset MLE, Epileptor) are not verified against the papers. Implementers should check them against the PDFs and the reference packages listed.
- A verified, openly licensed dataset with long continuous pre-ictal iEEG comparable to NeuroVista, usable without gating, was not identified.

## Part V. Cities, Populations, Economies, Societies and Infrastructure

*Remark.* Part V's conventions. The bibliographic markers are those of §0.3 (records verified against Crossref's API, OpenAlex, Semantic Scholar, the arXiv API or the provider's own page). For content: "(abstract)" means the statement is taken from the abstract retrieved this session; "(content from memory)" means it is not verified this session. Formulas are given from memory unless stated; **no equation number was verified this session**, so equation numbers are omitted rather than guessed. Volume and page numbers, where given, are from memory unless they are encoded in the DOI; the DOI, authors, year, title and venue are what [V] attests. Session date: 2026-11-19.

### 1. Populations: Demographic Transitions, Fertility Shifts, Shrinking Cities and Migration as Regime Changes

#### 1.1 Takeaway
Demography has rich threshold *theories* (fertility "onset" thresholds, the low-fertility trap, cumulative causation of migration, multiple urban equilibria) but almost no published, out-of-sample test of generic early-warning indicators (rising variance or autocorrelation) before a demographic regime change; detection in practice is retrospective (structural breaks, onset definitions, stalls), and the evidence that urban decline is abrupt is weak: durable housing makes it slow and asymmetric.

#### 1.2 Cited Findings

**1.2.1 Fertility transitions and fertility regimes**
- Onset of the European fertility transition was operationalised by the Princeton European Fertility Project as a sustained fall of marital fertility (Coale's index I_g) by about 10%, a threshold definition of a regime change applied retrospectively to provinces (content from memory). Coale & Watkins (eds.) 1986/2017, *The Decline of Fertility in Europe*, Princeton UP — [V record](https://doi.org/10.1515/9781400886692).
- "Lowest-low fertility" (period TFR ≤ 1.3) emerged in Southern, Central and Eastern Europe in the 1990s, driven by postponement and tempo effects interacting with economic uncertainty (content from memory). Kohler, Billari & Ortega 2002, *Population and Development Review* 28 — [V](https://doi.org/10.1111/j.1728-4457.2002.00641.x).
- The low-fertility-trap hypothesis posits three self-reinforcing mechanisms (demographic: shrinking cohorts of potential mothers; normative: lower ideal family size learned from smaller families; economic: aspirations rising faster than expected income) that could lock a population below about TFR 1.5, i.e. a positive-feedback account of a second stable state (content from memory). Lutz, Skirbekk & Testa 2006, *Vienna Yearbook of Population Research* 4:167 — [V](https://doi.org/10.1553/populationyearbook2006s167).
- Myrskylä, Kohler & Billari 2009 claimed that at very high Human Development Index levels fertility declines reverse (a "J-shape"), i.e. that the trap is escapable (content from memory). *Nature* 460:741 — [V](https://doi.org/10.1038/nature08230). Harttgen & Vollmer 2013 questioned the robustness of this reversal (title framed as a question; content from memory: it is sensitive to the HDI's construction and to country samples). *Demography* — [V](https://doi.org/10.1007/s13524-013-0252-y). A 2024 study of US states 1969–2018 supports the inverse J-shape under most specifications but finds it "might have vanished since the 2007–2008 financial crisis" (abstract) — "Revisiting the J-Shape: Human Development and Fertility in the United States", *Demography* 2024 [V](https://doi.org/10.1215/00703370-11680156).
- Fertility *stalls* (interruptions of a decline at intermediate levels) in developing countries are documented by Bongaarts 2008 (content from memory), *Studies in Family Planning* 39 — [V](https://doi.org/10.1111/j.1728-4465.2008.00157.x). Sub-Saharan Africa's TFR stood at 5.1 in 2005–10; whether its transition follows the conventional path is debated (abstract) — Bongaarts & Casterline 2013, *PDR* — [V](https://doi.org/10.1111/j.1728-4457.2013.00557.x).
- A search this session for early-warning signals (variance, autocorrelation) preceding fertility onsets or fertility collapses returned no relevant study (OpenAlex search returned unrelated ecology papers).

**1.2.2 Shrinking and declining cities**
- Urban shrinkage is a common pathway for large cities since the mid-twentieth century, manifested as falling population, vacancy and underused infrastructure; the literature uses overlapping concepts (decline, decay, blight, abandonment, disurbanisation) tied to national contexts (abstract). Haase et al. 2014, *Environment and Planning A* 46 — [V](https://doi.org/10.1068/a46269).
- Durable housing makes decline slow and asymmetric: city growth rates are leptokurtic (cities grow faster than they decline), highly persistent especially among declining cities, positive shocks raise population more than prices, negative shocks lower prices more than population (abstract). Glaeser & Gyourko 2005, *JPE* 113 — [V](https://doi.org/10.1086/427465).
- Rank clocks show that the stable rank–size law of a city system hides large rank volatility of individual cities over centuries (content from memory). Batty 2006, *Nature* 444 — [V](https://doi.org/10.1038/nature05302).

**1.2.3 Multiple equilibria in urban population**
- Krugman's "history versus expectations" model gives multiple equilibria in the location of activity, with history or self-fulfilling expectations selecting among them (content from memory). Krugman 1991, *QJE* 106 — [V](https://doi.org/10.2307/2937950).
- A large temporary shock (wartime bombing) did not move long-run relative city sizes: "Long-run city size is robust even to large temporary shocks", supporting locational fundamentals plus increasing returns rather than multiple equilibria (abstract). Davis & Weinstein 2002, *AER* 92 — [V](https://doi.org/10.1257/000282802762024502). (Cited as evidence only; its setting is not proposed as a case study.)
- The same design applied to German city growth after WWII bombing finds that "multiple equilibria seem to be present", supporting a model with two stable equilibria, once spatial interdependence is included (abstract via Semantic Scholar). Bosker, Brakman, Garretsen & Schramm 2007, *J. Urban Economics* 61 — [V](https://doi.org/10.1016/j.jue.2006.07.001).
- Path dependence: US settlements at obsolete portage sites retained population long after their original advantage disappeared (content from memory). Bleakley & Lin 2012, *QJE* 127 — [V](https://doi.org/10.1093/qje/qjs011).

**1.2.4 Migration tipping**
- Cumulative causation: each act of migration alters the social context (networks, remittances, inequality) so that further migration becomes more likely, a positive-feedback mechanism (content from memory). Massey 1990, *Population Index* 56 — [V](https://doi.org/10.2307/3644186). The feedback is weaker in urban origin areas ("limits to cumulative causation"; content from memory) — Fussell & Massey 2004, *Demography* 41 — [V](https://doi.org/10.1353/dem.2004.0003).
- No study found this session that tests early-warning indicators before a migration surge or reversal.

**1.2.5 Detection tools in use for demographic regime changes (methods, not demographic results)**
- Structural breaks with unknown break dates: Andrews 1993 sup-F tests, *Econometrica* 61 — [V](https://doi.org/10.2307/2951764); multiple breaks by Bai & Perron 1998, *Econometrica* 66 — [V](https://doi.org/10.2307/2998540). Markov-switching (Hamilton 1989) — see §3.

#### 1.3 Inferences
- For populations, the defensible detector claims are *retrospective dating* (onset, stall, break) and *structural* indicators of approaching a threshold (e.g., the damping ratio of a Leslie matrix, which governs how fast a perturbed age structure returns to its stable distribution, is a demographic analogue of a recovery rate; this is standard matrix-population theory, content from memory). Generic critical slowing down has no demographic validation record to cite.
- The Glaeser–Gyourko asymmetry means a detector on city population counts will see decline as a slow ratchet, not a fold bifurcation; vacancy, prices and occupancy are the faster-responding variables in which a regime change would show first.
- Conflicting evidence on urban multiple equilibria (single equilibrium in one bombing study, two stable equilibria in another) means a design should treat bistability as a hypothesis per city system, not a default.

#### 1.4 Gaps
- No peer-reviewed test of variance/autocorrelation indicators before fertility onsets, fertility collapses (e.g., to lowest-low), or migration surges was found; this is a genuine gap, not a search failure claim beyond the queries made.
- No study found that anticipates (rather than describes) the onset of urban shrinkage from population or occupancy records.
- Equation-level formulations of the low-fertility-trap feedbacks were not retrieved.

### 2. Cities: Segregation Tipping, Neighbourhood Change, Housing Bubbles, Traffic Jamming, Crime and Urban Scaling

#### 2.1 Takeaway
Cities supply the best-documented social tipping evidence (segregation tipping by regression discontinuity; traffic jamming as a dynamical phase transition reproduced in experiments; explosive-root bubble detection in housing), but each claim has a serious critique: segregation tipping is contested (smooth dynamics; thresholds estimated from the same data), three-phase traffic theory is disputed, critical-slowing-down claims for housing rest on few studies, and urban scaling exponents depend on how cities are delimited.

#### 2.2 Cited Findings

**2.2.1 Segregation tipping: theory**
- Schelling 1971: agents relocate when the share of like neighbours falls below a tolerance threshold; mild preferences produce extreme segregation. *J. Mathematical Sociology* 1:143 — [V](https://doi.org/10.1080/0022250x.1971.9989794). Rule (from memory): agent i moves if (like neighbours)/(occupied neighbours) < f; the macro state is segregated for a wide range of f well below 1/2.
- Segregation can arise even when every agent prefers integration ("all-integrationist world"; title; content from memory: stochastic best-response dynamics select segregated states). Zhang 2004, *JEBO* 54 — [V](https://doi.org/10.1016/j.jebo.2003.03.005).
- An exactly solvable Schelling-like model interpolating between individual and collective utility shows a qualitative transition from a segregated low-utility phase to a mixed high-utility phase as cooperativity increases (abstract). Grauwin, Bertin, Lemoy & Jensen 2009, *PNAS* 106 — [V](https://doi.org/10.1073/pnas.0906263106).
- Survey evidence on preferences was used to test the underpinnings of Schelling's model (abstract). Clark 1991, *Demography* 28 — [V](https://doi.org/10.2307/2061333).
- Binary choice with social interactions (mean-field logit) has multiple equilibria when interactions are strong; the canonical form (from memory) is m = tanh(β(h + J m)), with bistability for βJ > 1. Brock & Durlauf 2001, *Review of Economic Studies* 68 — [V](https://doi.org/10.1111/1467-937x.00168).

**2.2.2 Segregation tipping: empirical tests and critiques**
- Card, Mas & Rothstein 2008 use regression-discontinuity methods on US Census tracts 1970–2000 and find "strong evidence that white population flows exhibit tipping-like behavior in most cities, with a distribution of tipping points ranging from 5% to 20% minority share", in suburbs and near minority enclaves (abstract). *QJE* 123(1):177 — [V](https://doi.org/10.1162/qjec.2008.123.1.177). They report no systematic non-linearity of rents or prices at the tipping point — [NBER summary via search](https://www.nber.org/papers/w13052) ([V record NBER w13052](https://doi.org/10.3386/w13052)). Method (content from memory): the tipping point m* for each city is found by a search for the break that best fits a step function of white population change on initial minority share, estimated on a random half of tracts, and the discontinuity is then tested on the other half to avoid specification search.
- The same authors propose "one-sided" tipping, in which neighbourhoods below the threshold can remain stably mixed, against Schelling's two-sided instability (abstract). Card, Mas & Rothstein 2008, NBER w14470 — [V](https://doi.org/10.3386/w14470).
- Critique: Easterly, on the same census tract data 1970–2000, finds that the Schelling model "largely fails the tests": there is *more* white flight from neighbourhoods with a high initial white share than from mixed ones (abstract). Easterly 2009, *B.E. Journal of Macroeconomics* 9 — [V](https://doi.org/10.2202/1935-1690.1492).
- Methodological commentary characterises the CMR design as an "inverted" regression discontinuity whose threshold is itself estimated from the data — [Harvard IQSS blog, via search](https://blogs.iq.harvard.edu/regression_disc) [S].
- Replications in other settings: Sweden 1990–2007, native population growth drops discontinuously once non-European immigrant share exceeds the identified tipping point; high-earning, educated natives move out more (abstract). Aldén, Hammarstedt & Neuman 2015, *International Migration Review* — [V](https://doi.org/10.1111/imre.12066). Occupational gender segregation tips at 25–45% female (white-collar) and 13–30% (blue-collar), 1940–1990, lower where male attitudes are more sexist (abstract). Pan 2015, *J. Labor Economics* 33 — [V](https://doi.org/10.1086/678518). Swedish tipping thresholds used as instruments find little evidence that a higher immigrant share worsens outcomes (abstract). Böhlmark & Willén 2020, *AEJ: Applied* 12 — [V](https://doi.org/10.1257/app.20170579). Early-twentieth-century US: white flight in response to Black arrivals was large and accelerated 1900–1930, and segregation "could have arisen solely from the flight behavior of whites" (abstract). Shertzer & Walsh 2019, *REStat* 101 — [V](https://doi.org/10.1162/rest_a_00786).
- A bibliometric review at fifty years of Schelling's models — Ubarevičienė et al. 2024, *Cities* — [V record](https://doi.org/10.1016/j.cities.2024.104838) (content not retrieved).

**2.2.3 Gentrification and neighbourhood change thresholds**
- Endogenous gentrification: price growth is concentrated in poorer neighbourhoods adjacent to rich ones when a city-wide demand shock arrives, i.e. a spatial spillover with threshold-like spread (content from memory). Guerrieri, Hartley & Hurst, Federal Reserve Bank of Cleveland WP 2010-08r3 (2013 revision) — [V](https://doi.org/10.26509/frbc-wp-201008r3); the *J. Public Economics* version was not verified.
- In Chicago, gentrification proceeds less in neighbourhoods with larger Black shares (content from memory; a threshold of roughly 40% Black is often quoted from this paper but not verified here). Hwang & Sampson 2014, *American Sociological Review* 79 — [V](https://doi.org/10.1177/0003122414535774).

**2.2.4 Housing bubbles and crashes**
- Recursive right-tailed unit-root tests: Phillips, Wu & Yu 2011 (SADF), *International Economic Review* 52 — [V](https://doi.org/10.1111/j.1468-2354.2010.00625.x); Phillips & Yu 2011 date bubble origination and collapse in seven series including a home price index during the subprime crisis, and present the tests as "an early warning diagnostic of bubble activity" (abstract), *Quantitative Economics* 2 — [V](https://doi.org/10.3982/qe82); Phillips, Shi & Yu 2015 (GSADF, flexible windows for multiple bubbles; proposed for "real-time" surveillance by central banks and regulators) (abstract), *IER* 56 — [V](https://doi.org/10.1111/iere.12132).
- Formulas (from memory): ADF regression on window [r1, r2] of a log price series, Δy_t = α + β y_{t−1} + Σ_{i=1}^{k} ψ_i Δy_{t−i} + ε_t; H0: β = 0 (unit root) against H1: β > 0 (mildly explosive). BSADF_{r2}(r0) = sup_{r1 ∈ [0, r2−r0]} ADF_{r1}^{r2}; GSADF(r0) = sup_{r2 ∈ [r0, 1]} BSADF_{r2}(r0). Origination is the first date at which BSADF exceeds its critical value; termination the first subsequent date it falls below. Minimum window rule of thumb r0 = 0.01 + 1.8/√T.
- Critiques and refinements: size distortions under non-stationary volatility (content from memory: wild-bootstrap corrections) — Harvey, Leybourne, Sollis & Taylor 2016, *J. Empirical Finance* — [V](https://doi.org/10.1016/j.jempfin.2015.09.002); comparison of bubble tests — Homm & Breitung 2012, *J. Financial Econometrics* — [V](https://doi.org/10.1093/jjfinec/nbr009); detecting the collapse phase ("implosion") — Phillips & Shi 2018, *Econometric Theory* — [V](https://doi.org/10.1017/s0266466617000202). Housing applications: international exuberance episodes — Pavlidis et al. 2016, *J. Real Estate Finance and Economics* — [V](https://doi.org/10.1007/s11146-015-9531-2) (content not retrieved); OECD countries — Engsted, Hviid & Pedersen 2016, *J. Int. Financial Markets, Institutions & Money* — [V](https://doi.org/10.1016/j.intfin.2015.07.006); US regional markets — Shi 2017, *Economic Modelling* — [V](https://doi.org/10.1016/j.econmod.2017.06.002).
- Critical slowing down in housing: Tan & Cheong 2014 report CSD associated with regime shifts in the US housing market, *Eur. Phys. J. B* 87 — [V](https://doi.org/10.1140/epjb/e2014-41038-1) (no abstract retrieved; the positive finding is reported in [Diks et al. 2019](https://doi.org/10.1007/s00181-018-1527-3) via search). Tan & Cheong 2016 model the 2004–2008 US bubble as a regime shift between alternative stable states consistent with the CSD signals, validated by estimating when the bubble burst, and argue loose monetary policy was a plausible cause (abstract). *PLoS ONE* 11 — [V](https://doi.org/10.1371/journal.pone.0162140). These are single-team, single-episode results.

**2.2.5 Traffic flow phase transitions and the onset of congestion**
- Empirical phases: measurements on German highways show transitions between free flow, synchronised flow and jams; a short localised perturbation can trigger free flow → synchronised flow, which can persist for hours (abstract). Kerner & Rehborn 1997, *PRL* 79:4030 — [V](https://doi.org/10.1103/physrevlett.79.4030); Kerner 1998, *PRL* 81 — [V](https://doi.org/10.1103/physrevlett.81.3797); Kerner & Klenov 2002, *J. Phys. A* 35 — [V](https://doi.org/10.1088/0305-4470/35/3/102).
- Critique: Schönhof & Helbing 2009, "Criticism of three-phase traffic theory", *Transportation Research B* 43 — [V](https://doi.org/10.1016/j.trb.2009.02.004) (content from memory: two-phase models with a fundamental diagram reproduce the stylised facts); Treiber, Kesting & Helbing 2010, *Transportation Research B* — [V](https://doi.org/10.1016/j.trb.2010.03.004) (DOI as listed by Crossref, truncated in output; content from memory).
- Jams without bottlenecks: on a circular track, a jam emerges spontaneously when density exceeds a critical value; "a bottleneck is only a trigger and not the essential origin" (abstract). Sugiyama et al. 2008, *New J. Physics* 10:033001 — [V](https://doi.org/10.1088/1367-2630/10/3/033001).
- Test models (formulas from memory): Nagel–Schreckenberg cellular automaton (accelerate v ← min(v+1, v_max); brake v ← min(v, gap); randomise v ← max(v−1, 0) with probability p; move), *J. Phys. I* 2:2221 (1992) — [V](https://doi.org/10.1051/jp1:1992277). Optimal-velocity model ẍ_n = a[V(Δx_n) − ẋ_n], uniform flow linearly unstable when V′(h) > a/2 — Bando et al. 1995, *PRE* 51:1035 — [V](https://doi.org/10.1103/physreve.51.1035). Intelligent driver model v̇ = a[1 − (v/v0)^δ − (s*/s)²], s* = s0 + vT + vΔv/(2√(ab)) — Treiber, Hennecke & Helbing 2000, *PRE* 62:1805 — [V](https://doi.org/10.1103/physreve.62.1805). Review: Helbing 2001, *Rev. Mod. Phys.* 73 — [V](https://doi.org/10.1103/revmodphys.73.1067); textbook Treiber & Kesting 2013 — [V](https://doi.org/10.1007/978-3-642-32460-4).
- Early warnings in traffic: changes in traffic stability "are signaled by generic features, known as early warning signals in the theory of critical slowing down, observed before traffic instabilities occur", incorporated "in an operational data-driven algorithm to evaluate the risk of traffic jams on highways" (abstract). Ghadami, Doering, Drake, Rohani & Epureanu 2022, *IEEE Trans. Intelligent Transportation Systems* — [V](https://doi.org/10.1109/tits.2021.3095897). For heterogeneous ("disordered") traffic, variance and lag-1 autocorrelation rise before the onset of congestion in a model (abstract). Chattopadhyay & Gupta 2024, arXiv:2401.09364 — [V](https://arxiv.org/abs/2401.09364). Older detector-based work on "incipient congestion" prediction exists (NISS TR-79, 1998, "Real-Time Prediction of Incipient Congestion on Freeways from Detector Data") — [S, via search](https://www.niss.org/node/2819). An operating queue-warning system (Minnesota MN-QWARN) reportedly reduced crashes by 22% but works on shockwaves, not CSD indicators — [S, via search](https://mntransportationresearch.org/2023/09/07/improving-and-expanding-the-queue-warning-system/).

**2.2.6 Crime and disorder dynamics**
- Reaction–diffusion models of crime: hotspots form when the enhanced risk of repeat victimisation diffuses slowly relative to offender movement; analysis distinguishes subcritical hotspots (suppressed by policing, which dissipates them) from supercritical ones (displaced) (abstract and content from memory). Short, Brantingham, Bertozzi & Tita 2010, *PNAS* 107 — [V](https://doi.org/10.1073/pnas.0910921107). Dimensionless model (from memory): ∂A/∂t = η∇²A − A + A₀ + ρA; ∂ρ/∂t = ∇·(∇ρ − 2ρ∇A/A) − ρA + B̄, with A attractiveness and ρ offender density; hotspots arise by a Turing-type instability.
- Self-exciting (Hawkes) point processes capture near-repeat crime: λ(t, x) = μ(x) + Σ_{t_i<t} g(t − t_i, x − x_i) (from memory). Mohler et al. 2011, *JASA* 106 — [V](https://doi.org/10.1198/jasa.2011.ap09546). A branching ratio approaching 1 in such a fit is the point-process analogue of approaching criticality (inference).

**2.2.7 Urban scaling and phase transitions**
- Urban indicators scale with population as Y = Y₀ N^β: superlinear for wealth and innovation, sublinear for infrastructure; the abstract claims these relations are general across urban systems (abstract). Bettencourt, Lobo, Helbing, Kühnert & West 2007, *PNAS* 104 — [V](https://doi.org/10.1073/pnas.0610172104). Content from memory: the growth equation dN/dt = (Y₀/E) N^β − (R/E) N implies, for β > 1, a finite-time singularity, so sustained growth requires successive innovations at accelerating frequency, a claim of recurrent "phase transitions" in urban development. Deviations from scaling as a local measure: Bettencourt et al. 2010, *PLoS ONE* — [V](https://doi.org/10.1371/journal.pone.0013541). A derivation of scaling exponents from network models: Bettencourt 2013, *Science* 340 — [V](https://doi.org/10.1126/science.1235823). Batty 2008, *Science* 319 — [V](https://doi.org/10.1126/science.1151419); Batty 2013, *The New Science of Cities* — [V](https://doi.org/10.7551/mitpress/9399.001.0001).
- Critiques: for thousands of realisations of city boundaries in England and Wales, "population size alone does not provide us enough information" to describe urban indicators; exponents depend on definitions (abstract). Arcaute et al. 2015, *J. R. Soc. Interface* 12 — [V](https://doi.org/10.1098/rsif.2014.0745). With fluctuations modelled explicitly, the evidence for β ≠ 1 must be quantified and often weakens (abstract). Leitão, Miotto, Gerlach & Altmann 2016, *R. Soc. Open Sci.* 3 — [V](https://doi.org/10.1098/rsos.150649). Systematic paradox of urban scaling laws — Cottineau et al. 2017, *CEUS* 63 — [V](https://doi.org/10.1016/j.compenvurbsys.2016.04.006) (content not retrieved).
- Percolation as an urban phase transition: hierarchical percolation of the British street network reveals cities and regions as clusters at thresholds of intersection distance (content from memory). Arcaute et al. 2016, *R. Soc. Open Sci.* 3 — [V](https://doi.org/10.1098/rsos.150691). Review: Barthelemy 2019, *Nat. Rev. Phys.* 1 — [V](https://doi.org/10.1038/s42254-019-0054-2).

**2.2.8 Urban systems as states with early warnings; collapse of urban services**
- Visits to points of interest (POIs) in 17 US metropolitan areas were examined for early-warning signals of phase transitions in population response to COVID-19 and policy (abstract). Li, Tang, Coleman & Mostafavi 2020, arXiv:2008.02905 — [V](https://arxiv.org/abs/2008.02905).
- Local fiscal distress has been predicted by composite indicator models (content from memory: a ten-variable scale tested on Michigan local governments). Kloha, Weissert & Kleine 2005, *Public Administration Review* 65 — [V](https://doi.org/10.1111/j.1540-6210.2005.00456.x).
- A search this session for 2023–2024 empirical studies of critical slowing down in urban systems found no clear example beyond the above (search summary) — [search results](https://arxiv.org/pdf/2008.02905).

#### 2.3 Inferences
- Segregation tipping is the best test case for a *spatial-compositional* detector (tract shares over time), but any detector that estimates the threshold from the record must use sample splitting (as CMR did) or it will manufacture discontinuities; Easterly's negative result shows the same records support a smooth reading.
- For traffic, the transition is reproducible (ring-road experiments, detector data), short-timescale and repeated thousands of times, making it the most statistically honest domain for estimating hit and false-alarm rates of CSD indicators; the dispute between three-phase and two-phase theories matters less for detection than for interpretation.
- GSADF is a *detector of an explosive regime already under way*, not a predictor of the crash date; its real-time use requires a declared minimum window and corrections for time-varying volatility.
- Urban scaling results should not be used as indicators of a city's "phase" without boundary-robustness checks.

#### 2.4 Gaps
- No published replication that re-runs CMR's tipping-point search with modern RD inference (bias-corrected, robust confidence intervals) on later censuses was found this session.
- No validated early-warning study (out-of-sample) for crime regime shifts in cities was found.
- No study found of CSD before collapse of urban services (water, transit, municipal finance) beyond composite fiscal-distress indices.

### 3. Economies: Structural Change, Structural Breaks, Regime Switching and Endogenous Cycles

#### 3.1 Takeaway
Macroeconomics detects regime changes mainly with structural-break tests and Markov-switching models (which date or classify regimes, often in near real time) rather than with critical-slowing-down indicators; there is recent evidence that business cycles may be endogenous limit cycles near a locally unstable steady state, which implies periodicity rather than CSD before each recession, and no direct test of CSD before recessions was found.

#### 3.2 Cited Findings
- **Markov switching.** Hamilton 1989 models output growth as an autoregression whose mean switches with an unobserved two-state Markov chain and dates US recessions from smoothed state probabilities. *Econometrica* 57:357 — [V](https://doi.org/10.2307/1912559). Model (from memory): y_t − μ(s_t) = Σ_{i=1}^{4} φ_i (y_{t−i} − μ(s_{t−i})) + ε_t, s_t ∈ {0, 1} with P(s_t = j | s_{t−1} = i) = p_ij; estimated on US real GNP growth (1950s–1984) with an AR(4). Business-cycle turning-point dating overview: Chauvet & Piger-type surveys, e.g. Chauvet 2006 chapter "Dating Business Cycle Turning Points" — [V record](https://doi.org/10.1016/s0573-8555(05)76001-6).
- **Structural breaks.** Andrews 1993 — [V](https://doi.org/10.2307/2951764) (sup-Wald/LM/LR over unknown break dates; corrigendum 2003 [V](https://doi.org/10.1111/1468-0262.00405)); Bai & Perron 1998 — [V](https://doi.org/10.2307/2998540) (multiple breaks, sequential and global tests).
- **Business cycles as endogenous cycles.** Beaudry, Galizia & Portier 2020 find evidence that cycles reflect "a strong endogenous mechanism which produces recurrent boom-bust phenomena", favoured by a slightly wider frequency window, strategic complementarities from financial frictions and "a locally unstable steady state in estimation" (abstract). *AER* 110 — [V](https://doi.org/10.1257/aer.20190789). Inference: a locally unstable steady state with a limit cycle is a post-Hopf regime; the generic CSD signal belongs to the approach to the Hopf point, not to each recession.
- **Filtering pitfalls for real-time indicators.** The HP filter introduces spurious dynamics, end-of-sample values differ from mid-sample values, and a regression of y_{t+h} on four lags as of t is a better alternative (abstract). Hamilton 2018, *REStat* 100 — [V](https://doi.org/10.1162/rest_a_00706). This bears directly on detrending before computing variance/autocorrelation indicators.
- **Structural change in sectoral composition.** Premature deindustrialisation: manufacturing shares peak at lower levels and earlier incomes in later developers (content from memory). Rodrik 2016, *J. Economic Growth* 21 — [V](https://doi.org/10.1007/s10887-015-9122-3). The product space: countries diversify into products near those they already export, so structural change is path-dependent and can stall (content from memory). Hidalgo, Klinger, Barabási & Hausmann 2007, *Science* 317 — [V](https://doi.org/10.1126/science.1144581).
- **Network origins of aggregate fluctuations.** Sectoral shocks need not average out when the input–output network has dominant suppliers (content from memory). Acemoglu, Carvalho, Ozdaglar & Tahbaz-Salehi 2012, *Econometrica* 80 — [V](https://doi.org/10.3982/ECTA9623).
- **Currency crises.** The signals approach: an indicator issues a signal when it crosses a threshold chosen to minimise the noise-to-signal ratio; NSR = [B/(B+D)] / [A/(A+C)], with A signal-and-crisis within the window (24 months), B signal-no-crisis, C no-signal-crisis, D no-signal-no-crisis (formula from memory). Kaminsky, Lizondo & Reinhart 1998, *IMF Staff Papers* 45 — [V](https://doi.org/10.2307/3867328). Banking problems typically precede currency crises; crises occur as the economy enters recession after a credit-fuelled boom with capital inflows and overvaluation (abstract). Kaminsky & Reinhart 1999, *AER* 89 — [V](https://doi.org/10.1257/aer.89.3.473). Out-of-sample evaluation of the indicators approach against the Asian crisis: Berg & Pattillo 1999, *J. Int. Money and Finance* 18 — [V](https://doi.org/10.1016/s0261-5606(99)00024-8) (content from memory: forecasts better than guesswork but with many false alarms and misses). Probit approach: Frankel & Rose 1996, *J. International Economics* 41 — [V](https://doi.org/10.1016/s0022-1996(96)01441-9). Currency-crisis EWS should be dynamic — Candelon, Dumitrescu & Hurlin 2014, *Int. J. Forecasting* — [V record](https://doi.org/10.1016/j.ijforecast.2014.03.015).
- **CSD in currency markets.** A systematic study of two major currency pairs explains "how one might miss early warning signals of critical transitions" (title) — [PMC5851542, via search](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC5851542/) [S; record not verified against Crossref].
- **CSD before recessions.** Searches this session ("critical slowing down early warning recessions business cycle empirical test"; OpenAlex and Semantic Scholar queries) found no direct test of CSD before recessions; the closest evidence is financial (§4) — [search summary](https://link.springer.com/doi/10.1007/s00181-018-1527-3).

#### 3.3 Inferences
- For economies, a library should treat *regime detection* (Hamilton filtering, Bai–Perron dating) and *transition anticipation* (CSD) as different products with different evaluation: the former is judged by dating accuracy and delay after the event, the latter by lead time and false alarms before it.
- Structural change in sectoral shares is a compositional, slowly drifting state; abrupt "transitions" there (deindustrialisation, stalls) have been studied descriptively, and none of the cited work tests generic indicators on them.
- The HP-filter critique implies that any detrending step in a detector must be one-sided and vintage-aware when claimed to work in real time.

#### 3.4 Gaps
- No peer-reviewed study testing rising variance/autocorrelation (CSD) before NBER-dated recessions or before episodes of structural change in input–output tables was found.
- Equation-level statement of Rodrik's and Hidalgo et al.'s structural-change measures was not retrieved.

### 4. Finance: Crashes, Banking Crises, Systemic Risk and the Evaluation of Early-Warning Systems

#### 4.1 Takeaway
The finance record is the most important negative result for generic detectors: market crashes mostly do *not* show critical slowing down (rising variance, sometimes rising correlation, but not rising autocorrelation), log-periodic crash prediction has repeatedly failed statistical tests, and in banking crises the useful predictors are structural (credit growth, credit-to-GDP gap, debt service, asset price growth), evaluated by AUROC, noise-to-signal ratios and policymakers' loss functions, where simple logits beat machine learning out of sample.

#### 4.2 Cited Findings

**4.2.1 Critical slowing down before crashes: claims and negative results**
- Scheffer et al. 2012 review anticipating critical transitions in ecology, climate, medicine and finance, *Science* 338:344 — [V](https://doi.org/10.1126/science.1225244) (content from memory: networks that are homogeneous and highly connected tend to show critical transitions, heterogeneous and modular ones more gradual change; financial systems discussed as candidates). Generic indicators: slowing down, rising autocorrelation and variance, skewness, flickering — Scheffer et al. 2009, *Nature* 461:53 — [V](https://doi.org/10.1038/nature08227) (content from memory). Indicator (from memory): near a fold, a linearised AR(1) x_{t+1} = φ x_t + σε_t with φ = e^{λΔt}, λ → 0⁻, so lag-1 autocorrelation φ → 1 and variance σ²/(1 − φ²) → ∞.
- Guttal, Raghavendra, Goel & Hoarau 2016: three US (Dow Jones, S&P 500, NASDAQ) and two European (DAX, FTSE) markets did not exhibit CSD before major crashes over the last century; strong indicator trends not followed by crashes suggest false alarms; rising variability could still signal systemic risk (abstract and title). *PLoS ONE* 11:e0144198 — [V](https://doi.org/10.1371/journal.pone.0144198).
- Diks, Hommes & Wang 2019: of four crises (Black Monday 1987, the 1997 Asian crisis, the 2000 dot-com bust, 2008), statistical evidence for CSD is found only before Black Monday 1987; mixed or insignificant for the others (abstract). *Empirical Economics* 57 — [V](https://doi.org/10.1007/s00181-018-1527-3).
- Positive claims: information dissipation length (Shannon information transmission across maturities) of USD and EUR interest-rate swaps rose steadily toward the Lehman bankruptcy, peaked at it and decreased afterwards (abstract). Quax, Kandhai & Sloot 2013, *Sci. Rep.* 3:1898 — [V](https://doi.org/10.1038/srep01898). Persistence landscapes of sliding-window point clouds of four US index returns: their L^p norms grow before the 2000 and 2007–09 crashes (abstract; the lead of roughly 250 trading days is from memory). Gidea & Katz 2018, *Physica A* 491 — [V](https://doi.org/10.1016/j.physa.2017.09.028); cryptocurrencies — Gidea et al. 2020, *Physica A* — [V](https://doi.org/10.1016/j.physa.2019.123843). L¹-norm persistence series combined with lag-1 autocorrelation, variance and mean power spectrum, trends judged by Kendall's τ, for the dot-com and Lehman episodes (abstract). Ismail et al. 2022, *Frontiers in Applied Mathematics and Statistics* 8 — [V](https://doi.org/10.3389/fams.2022.940133). These positive results are on two or three episodes chosen because a crash happened.
- Correlations and variance rise in crises ("correlations, risk and crisis"; content from memory) — Gorban, Smirnova & Tyukina 2010, *Physica A* 389 — [V](https://doi.org/10.1016/j.physa.2010.03.035); stock correlations increase under market stress (content from memory) — Preis et al. 2012, *Sci. Rep.* 2 — [V](https://doi.org/10.1038/srep00752); the absorption ratio (variance share of the first few principal components) as a systemic-risk measure (content from memory) — Kritzman, Li, Page & Rigobon 2011, *J. Portfolio Management* 37 — [V](https://doi.org/10.3905/jpm.2011.37.4.112).
- A 2026 arXiv preprint on retail herding found rolling autocorrelation and variance warnings did not Granger-cause regime flips at 5–20-day leads (via search) — [arXiv 2601.11602](https://arxiv.org/pdf/2601.11602) [S; not verified].

**4.2.2 Log-periodic power laws (LPPL)**
- Claim: crashes are critical points preceded by log-periodic oscillations; ln p(t) = A + B(t_c − t)^m + C(t_c − t)^m cos(ω ln(t_c − t) − φ), with 0 < m < 1 and ω typically 6–13 (formula and ranges from memory). Sornette, Johansen & Bouchaud 1996, *J. Phys. I* 6 — [V](https://doi.org/10.1051/jp1:1996135); Johansen, Ledoit & Sornette 2000, *IJTAF* 3 — [V](https://doi.org/10.1142/s0219024900000115); calibration by reduction to a stable nonlinear problem — Filimonov & Sornette 2013, *Physica A* 392 — [V](https://doi.org/10.1016/j.physa.2013.04.012).
- Critiques: the October 1997 "correction" does not appear to be the accumulation point of a geometric series of minima (abstract) — Laloux, Potters, Cont, Aguilar & Bouchaud 1999, *Europhys. Lett.* 45 — [V](https://doi.org/10.1209/epl/i1999-00122-9) (comment and reply: [V](https://doi.org/10.1209/epl/i2002-00381-x), [V](https://doi.org/10.1209/epl/i2002-00669-3)). The log-periodic component of S&P 500 log-differences before October 1987 "is not statistically significant if we exclude the last year of data before the crash" (abstract) — Feigenbaum 2001, *Quantitative Finance* 1 — [V](https://doi.org/10.1088/1469-7688/1/3/306). A Bayesian analysis likewise finds little support (content from memory) — Chang & Feigenbaum 2006, *Quant. Finance* 6 — [V](https://doi.org/10.1080/14697680500511017). Testing crashes with LPPL; fits unstable (content from memory) — Brée & Joseph 2013, *Int. Rev. Financial Analysis* 30 — [V](https://doi.org/10.1016/j.irfa.2013.05.005). Review of estimation difficulties — Geraskin & Fantazzini 2013, *European J. Finance* 19 — [V](https://doi.org/10.1080/1351847x.2011.601657).

**4.2.3 Agent-based models with transitions (test models)**
- Heterogeneous beliefs (fundamentalists vs trend-followers) produce a bifurcation route to chaos as intensity of choice rises (content from memory). Brock & Hommes 1998, *JEDC* 22 — [V](https://doi.org/10.1016/s0165-1889(98)00011-6). A multi-agent model reproduces fat tails and volatility clustering near criticality (content from memory). Lux & Marchesi 1999, *Nature* 397 — [V](https://doi.org/10.1038/17290).

**4.2.4 Systemic risk on networks: cascades**
- Clearing vector (from memory): p = min(p̄, Πᵀ p + e), the greatest fixed point of a monotone map on the lattice of payment vectors. Eisenberg & Noe 2001, *Management Science* 47 — [V](https://doi.org/10.1287/mnsc.47.2.236.9835).
- Financial systems are "robust-yet-fragile": contagion is rare but widespread when it occurs; the system's pre-2007 resilience was not a reliable guide to its future robustness (abstract). Gai & Kapadia 2010, *Proc. R. Soc. A* 466 — [V](https://doi.org/10.1098/rspa.2009.0410).
- DebtRank, a feedback-centrality measure of systemic impact; in the USD 1.2 trillion Fed emergency loan data 2008–2010, 22 institutions formed a strongly connected graph in which each became systemically important at the crisis peak (abstract). Battiston et al. 2012, *Sci. Rep.* 2:541 — [V](https://doi.org/10.1038/srep00541). Recursion (from memory): h_i(t) = min{1, h_i(t−1) + Σ_{j distressed at t−1} W_ji h_j(t−1)}, each node propagating once.
- Phase transition in network stability: dense interconnection stabilises against small shocks and propagates large ones (content from memory). Acemoglu, Ozdaglar & Tahbaz-Salehi 2015, *AER* 105 — [V](https://doi.org/10.1257/aer.20130456). Integration and diversification in cascades (content from memory) — Elliott, Golub & Jackson 2014, *AER* 104 — [V](https://doi.org/10.1257/aer.104.10.3115). Ecological analogy — Haldane & May 2011, *Nature* 469 — [V](https://doi.org/10.1038/nature09659). Instability when the leverage/exposure matrix's spectral radius exceeds 1 (content from memory) — Bardoscia et al. 2017, *Nat. Commun.* 8 — [V](https://doi.org/10.1038/ncomms14416). Network amplification of losses is bounded and often modest under realistic balance sheets (content from memory) — Glasserman & Young 2015, *J. Banking & Finance* 50 — [V](https://doi.org/10.1016/j.jbankfin.2014.02.006). Review: Bardoscia et al. 2021, *Nat. Rev. Phys.* — [V record](https://doi.org/10.1038/s42254-021-00322-5).
- Interbank topology as a warning: quarterly Dutch interbank exposures 1998–2008 show an abrupt topological change in 2008 ("a clear — but unpredictable — signature") when only link density is controlled, but a gradual transition when degree heterogeneity is controlled (abstract). Squartini, van Lelyveld & Garlaschelli 2013, *Sci. Rep.* 3:3357 — [V](https://doi.org/10.1038/srep03357); follow-up on stationarity — Squartini et al. 2014, *J. Complex Networks* — [V record](https://doi.org/10.1093/comnet/cnu012).
- Connectedness measures from Granger-causality networks of returns rose before 2008 (content from memory). Billio, Getmansky, Lo & Pelizzon 2012, *J. Financial Economics* 104 — [V](https://doi.org/10.1016/j.jfineco.2011.12.010).

**4.2.5 Banking-crisis early-warning systems used by institutions**
- Credit growth is "a powerful predictor of financial crises" in 14 countries 1870–2008 (abstract). Schularick & Taylor 2012, *AER* 102 — [V](https://doi.org/10.1257/aer.102.2.1029); a replication exists (Summers 2015, SSRN) — [V record](https://doi.org/10.2139/ssrn.2638502).
- The "financial hockey stick": a century-long stable credit/GDP gave way to rapid financialisation; leverage correlates with business-cycle moments and tail risk (abstract). Jordà, Schularick & Taylor 2017, *NBER Macroeconomics Annual* 31 — [V](https://doi.org/10.1086/690241).
- Rapid credit and asset-price growth over three years is associated with a 40% probability of a crisis within three years, against about 7% in normal times; crises are not "bolts from the sky" (abstract). Greenwood, Hanson, Shleifer & Sørensen 2022, *J. Finance* 77 — [V](https://doi.org/10.1111/jofi.13105).
- Bank equity declines predict crises and output losses even without panics (content from memory). Baron, Verner & Xiong 2021, *QJE* 136 — [V](https://doi.org/10.1093/qje/qjaa034).
- Policy requirements for indicators: timing (signals early enough to act, commonly 1.5–5 years ahead), stability of signals, and interpretability; the credit-to-GDP gap is the best long-horizon indicator and the debt-service ratio the best at short horizons, both with high AUROC (content from memory; no abstract retrieved). Drehmann & Juselius 2014, *Int. J. Forecasting* 30 — [V](https://doi.org/10.1016/j.ijforecast.2013.10.002). The Basel III countercyclical capital buffer guide uses the credit-to-GDP gap from a one-sided HP filter with λ = 400,000, buffer starting at a 2-point gap and reaching 2.5% at 10 points (content from memory; BCBS 2010 guidance not verified) [M].
- Evaluation frameworks: signal extraction and loss functions — L(μ) = μ T₁ P₁ + (1 − μ) T₂ P₂ (T₁ missed-crisis rate, T₂ false-alarm rate, P₁, P₂ unconditional probabilities, μ the policymaker's relative aversion to missing a crisis), usefulness U = min(μ P₁, (1 − μ) P₂) − L (formulas from memory). Sarlin 2013, *Economics Letters* 119 — [V](https://doi.org/10.1016/j.econlet.2012.12.030); comment by Alessi et al. 2014 — [V](https://doi.org/10.1016/j.econlet.2014.06.015); quasi-real-time indicators for costly asset-price booms — Alessi & Detken 2011, *Eur. J. Political Economy* 27 — [V](https://doi.org/10.1016/j.ejpoleco.2011.01.003). A unified statistical framework for evaluating EWS (optimal cut-offs, comparison tests) — Candelon, Dumitrescu & Hurlin 2012, *IMF Economic Review* 60 — [V](https://doi.org/10.1057/imfer.2012.4).
- Machine learning: a horse race of conventional and machine-learning early-warning models with ensembles and model-uncertainty estimates (abstract) — Holopainen & Sarlin 2017, *Quantitative Finance* 17 — [V](https://doi.org/10.1080/14697688.2017.1357972); but in recursive out-of-sample evaluation over 45 years of advanced economies, machine-learning methods with very high in-sample fit "are outperformed by the logit approach", robust to metric, crisis definition, preference parameter, sample length and variables (abstract) — Beutel, List & von Schweinitz 2019, *J. Financial Stability* 45 — [V](https://doi.org/10.1016/j.jfs.2019.100693).
- Crisis chronologies: Laeven & Valencia 2013, *IMF Economic Review* 61 — [V](https://doi.org/10.1057/imfer.2013.12); 2020 "Systemic Banking Crises Database II", *IMF Economic Review* 68 — [V](https://doi.org/10.1057/s41308-020-00107-3) (content from memory: 151 systemic banking crises 1970–2017); an IMF working paper "Systemic Banking Crises Database: 1970–2025" exists — [V record](https://doi.org/10.5089/9798229045971.001.a001) (year and content not retrieved). ECB/ESRB EU crises database — Lo Duca et al. 2017 — [V](https://doi.org/10.2139/ssrn.3011859).

#### 4.3 Inferences
- A detector that reports "rising autocorrelation" before market crashes would contradict the two most systematic tests (Guttal 2016; Diks 2019); rising variance and rising cross-correlation are the more consistent pre-crisis features, and they are also produced by volatility clustering without any bifurcation.
- In banking, the transition is better modelled as the endogenous build-up of a slow variable (leverage, credit gap) that raises the hazard of a fast collapse; indicators of the slow variable, evaluated against crisis chronologies, outperform generic dynamical indicators of the fast variable.
- The institutional evaluation apparatus (AUROC, noise-to-signal, loss-function usefulness, signal stability, real-time vintages) is directly transferable to any criticality detector and is more demanding than the ecological literature's Kendall-τ-against-surrogates test.
- LPPL should enter a library only as a *null to test against*, with Feigenbaum's out-of-sample exclusion test as the standard.

#### 4.4 Gaps
- Exact AUROC values of Drehmann & Juselius 2014 were not retrieved; the claims above are from memory.
- The numbers in the Laeven–Valencia 1970–2025 update (count, coverage, date) were not retrieved.
- No study was found that compares CSD indicators head-to-head with credit-gap indicators on the same crisis chronology under a policymaker's loss function.

### 5. Societies: Norms, Polarisation, Revolutions, Conflict Onset and the Collapse of Past Societies

#### 5.1 Takeaway
Experiments show social conventions tip at a critical mass, theory explains revolutions as unforeseeable threshold cascades, and archaeological records show critical slowing down before some past transformations; but operational forecasting of political violence (ViEWS and its competitions) succeeds mainly by structural risk factors and conflict history, performs worst for low-level violence and onsets in previously peaceful places, and the "social tipping" concept is now criticised for overuse.

#### 5.2 Cited Findings

**5.2.1 Threshold models and norm tipping**
- Granovetter's threshold model: each actor joins when the fraction already participating exceeds its threshold; the equilibrium is the fixed point of r_{t+1} = F(r_t), F the cumulative distribution of thresholds, so a tiny change in the distribution can switch the outcome from near-zero to full participation (the uniform-thresholds example; content from memory). Granovetter 1978, *AJS* 83:1420 — [V](https://doi.org/10.1086/226707).
- Experimental evidence: in an artificial system of conventions, committed minorities overturned an established convention once they reached a critical mass, "direct empirical demonstration of the existence of a tipping point" (abstract); the critical mass of about 25% is from memory. Centola, Becker, Brackbill & Baronchelli 2018, *Science* 360:1116 — [V](https://doi.org/10.1126/science.aas8827).
- Social tipping interventions for decarbonisation: removing fossil-fuel subsidies and incentivising decentralised energy, carbon-neutral cities, divestment, revealing the moral implications of fossil fuels, climate education, disclosure of emissions information; drawn from expert elicitation, a workshop and a literature review (abstract). Otto et al. 2020, *PNAS* 117:2354 — [V](https://doi.org/10.1073/pnas.1900577117).
- Social tipping processes as self-amplifying (positive) feedbacks in technology and energy systems, political mobilisation, financial markets and norms; a conceptual framework (abstract). Winkelmann et al. 2022, *Ecological Economics* 192 — [V](https://doi.org/10.1016/j.ecolecon.2021.107242). Operationalising positive tipping points — Lenton et al. 2022, *Global Sustainability* 5 — [V](https://doi.org/10.1017/sus.2021.30).
- Critique: "social tipping points" are often conceptualised as desirable solutions, unlike natural-science tipping points; a qualitative review warns of overuse (abstract). Milkoreit 2022, *WIREs Climate Change* 13 — [V](https://doi.org/10.1002/wcc.813). Definitions for social-ecological systems — Milkoreit et al. 2018, *ERL* 13 — [V](https://doi.org/10.1088/1748-9326/aaaa75).
- Early warnings in a coupled behaviour–disease system: geocoded, sentiment-classified tweets about the MMR vaccine and measles-related Google searches were examined for critical slowing down as a tipping point in vaccinating behaviour was approached (abstract); the finding of rising indicators before the 2014–15 California outbreak is from memory. Pananos et al. 2017 (co-authors include Bury and Bauch, from memory), *PNAS* 114 — [V](https://doi.org/10.1073/pnas.1704093114).

**5.2.2 Polarisation**
- Opinion dynamics with influence and homophily in small, densely connected bodies show tipping points that make reversal of polarisation problematic (abstract). Macy, Ma, Tabin, Gao & Szymanski 2021, *PNAS* 118 — [V](https://doi.org/10.1073/pnas.2102144118). Nonlinear feedback dynamics of asymmetric polarisation — Leonard et al. 2021, *PNAS* 118 — [V](https://doi.org/10.1073/pnas.2102149118); echo chambers as a transition in an activity-driven network model — Baumann et al. 2020, *PRL* 124 — [V](https://doi.org/10.1103/physrevlett.124.048301); preventing extreme polarisation — Axelrod, Daymude & Forrest 2021, *PNAS* 118 — [V](https://doi.org/10.1073/pnas.2102139118). (Contents beyond Macy et al.'s abstract from memory.)

**5.2.3 Revolutions and regime change**
- Preference falsification makes revolutions unanticipated: private thresholds are hidden, so a small event can trigger a cascade no observer could foresee (content from memory). Kuran 1989, *Public Choice* 61 — [V](https://doi.org/10.1007/bf00116762); Kuran 1991 on 1989 Eastern Europe, *World Politics* 44 — [V](https://doi.org/10.2307/2010422). This is a theoretical argument that some social transitions have *no observable early warning*.
- Political-instability forecasting: onsets 1955–2003 distinguished with a two-year lead and "over 80% accuracy" by a model with few variables, chiefly a nonlinear five-category regime type (partial democracies with factionalism most unstable), infant mortality, neighbouring conflict and state-led discrimination (abstract; variable list beyond regime type from memory). Goldstone et al. 2010, *AJPS* 54 — [V](https://doi.org/10.1111/j.1540-5907.2009.00426.x).

**5.2.4 Civil conflict onset forecasting and its evaluation**
- Statistical significance is a poor guide to prediction: variables significant in onset models add little predictive power (abstract). Ward, Greenhill & Bakke 2010, *J. Peace Research* 47 — [V](https://doi.org/10.1177/0022343309356491). Conflict predicted in space and time — Weidmann & Ward 2010, *J. Conflict Resolution* 54 — [V](https://doi.org/10.1177/0022002710371669).
- Newspaper-text topics, used through within-country variation, predict the timing of conflict and become useful when risk rises in previously peaceful countries, avoiding the tendency to predict conflict only where it occurred before (abstract). Mueller & Rauh 2018, *APSR* 112 — [V](https://doi.org/10.1017/s0003055417000570).
- ViEWS: monthly forecasts 36 months ahead at country and subnational (grid-cell) level for the three UCDP types of organised violence in Africa, transparent and public (abstract). Hegre et al. 2019, *JPR* 56 — [V](https://doi.org/10.1177/0022343319823860). ViEWS2020 adds ensemble training/weighting and a retrospective evaluation of published forecasts (abstract) — Hegre et al. 2021, *JPR* 58 — [V](https://doi.org/10.1177/0022343320962157). Nine years of out-of-sample forecasts made in 2011 were "meaningful and reasonably accurate" but "performed worse in predicting low level incidence of conflict" (abstract) — Hegre, Nygård & Landsverk 2021, *ISQ* 65 — [V](https://doi.org/10.1093/isq/sqaa094).
- Prediction competitions: forecasting (de-)escalation of state-based violence in Africa for a true future (October 2020–March 2021) and a held-out test set, scored by an independent committee (abstract). Vesco et al. 2022, *International Interactions* 48 — [V](https://doi.org/10.1080/03050629.2022.2029856); lessons — Hegre et al. 2022 — [V record](https://doi.org/10.1080/03050629.2022.2070745). The 2023/24 challenge required *probabilistic* forecasts of UCDP fatalities, functioning like a pre-analysis plan before the true-future window (arXiv abstract) — Hegre, Vesco, Colaresi, Vestby, Timlick et al. 2025, *JPR* — [V](https://doi.org/10.1177/00223433241300862), arXiv:2407.11045 [V](https://arxiv.org/abs/2407.11045); 23 models from 13 institutions, true-future window July 2024–June 2025, scored by CRPS, interval and ignorance scores; one Transformer entry beat benchmarks in CRPS and interval score but not in ignorance score — [PRIO and University of Turin pages via search](https://iris.unito.it/handle/2318/2074350) [S]. Conformal prediction intervals for fatalities — Randahl et al. 2025, *Political Analysis* — [V record](https://doi.org/10.1017/pan.2025.10010).
- Data: ACLED introduced by Raleigh, Linke, Hegre & Karlsen 2010, *JPR* 47 — [V](https://doi.org/10.1177/0022343310378914); UCDP GED by Sundberg & Melander 2013, *JPR* 50 — [V](https://doi.org/10.1177/0022343313484347); media-based event data accuracy — Weidmann (Crossref lists 2014), *J. Conflict Resolution* [V record](https://doi.org/10.1177/0022002714530431).

**5.2.5 Collapse of past societies and social-ecological systems**
- Seven of nine regional reconstructions of European Neolithic population (8–4 kya) show increasing autocorrelation and variance before collapse, following rapid growth after the introduction of agriculture (abstract). Downey, Haas & Shennan 2016, *PNAS* 113 — [V](https://doi.org/10.1073/pnas.1602504113).
- Annual-resolution tree-ring-dated building activity shows CSD before repeated transformations of Pueblo societies in the pre-Hispanic US Southwest, consistent with archaeological evidence of rising violence (abstract). Scheffer, van Nes, Bird, Bocinsky & Kohler 2021, *PNAS* 118 — [V](https://doi.org/10.1073/pnas.2024397118) (title: "Loss of resilience preceded transformations of pre-Hispanic Pueblo societies").
- Catastrophic-climate "endgame" scenarios including societal collapse are dangerously underexplored (abstract). Kemp et al. 2022, *PNAS* 119 — [V](https://doi.org/10.1073/pnas.2108146119). Unifying social-ecological resilience and collapse — Cumming & Peterson 2017, *TREE* 32 — [V](https://doi.org/10.1016/j.tree.2017.06.014) (content not retrieved). Critical perspectives: collapse is rarer than claimed and often transformation — Butzer 2012, *PNAS* 109 — [V](https://doi.org/10.1073/pnas.1114845109) (content from memory). Seshat's single dimension of social complexity — Turchin et al. 2018, *PNAS* 115 — [V](https://doi.org/10.1073/pnas.1708800115); secular cycles — Turchin & Nefedov 2009 — [V](https://doi.org/10.1515/9781400830688). Tainter 1988, *The Collapse of Complex Societies* — [S] (only reviews located).
- Social-ecological regime shifts in a model coupling resource and harvesters' norms — Lade, Tavoni, Levin & Schlüter 2013, *Theoretical Ecology* 6 — [V](https://doi.org/10.1007/s12080-013-0187-3).
- Across fields, a review of empirical early-warning case studies reports that "in the majority of the case-studies we reviewed the performance of most early-warnings was positive in detecting tipping" (preprint abstract via search) — Dakos et al. 2024, *Earth System Dynamics* 15:1117 — [V](https://doi.org/10.5194/esd-15-1117-2024) (the review covers climate, ecological and human systems; the quoted sentence is from the 2023 preprint snippet, [S](https://egusphere.copernicus.org/preprints/2023/egusphere-2023-1773/egusphere-2023-1773.pdf)).

#### 5.3 Inferences
- Society-scale CSD evidence (Neolithic, Pueblo) is *retrospective on proxies* chosen because a collapse is known; it is subject to the prosecutor's fallacy (§7) and cannot estimate false-alarm rates.
- Operational political-violence forecasting is the opposite profile: prospective, scored, base-rate-aware, and dominated by structural and historical predictors; generic dynamical indicators have not been shown to add skill there.
- Kuran's argument is the social analogue of a noise-induced or hidden-threshold transition: a detector should state that a transition driven by hidden heterogeneous thresholds may give no warning in aggregate records.
- The positive-tipping literature (Otto, Winkelmann, Lenton 2022) is largely conceptual or expert-elicited; it supplies mechanisms and candidate state variables, not validated detectors.

#### 5.4 Gaps
- The quantitative CRPS results of the 2023/24 VIEWS challenge and the final leaderboard were not retrieved.
- No out-of-sample test of CSD indicators in conflict or protest records was found.
- Polarisation transitions have models but no detector validation found.

### 6. Infrastructure: Power Grids, Cascades, Structural Health and Supply Networks

#### 6.1 Takeaway
Power grids are where critical slowing down has the clearest physical basis (a stressed system's slowest eigenvalue approaches zero) and some empirical support from phasor data, but real grid frequency is heavy-tailed and non-Gaussian, large blackouts follow power laws consistent with operation near criticality, and the most recent large blackout (Iberia, 28 April 2025) was ultimately an overvoltage cascade shaped by earlier oscillations — a reminder that grids can fail through mechanisms other than a slow approach to a fold.

#### 6.2 Cited Findings
- Critical slowing down in phasor data: building on fast–slow systems, time series alone can estimate the temporal distance to a critical transition such as voltage collapse (abstract). Cotilla-Sanchez, Hines & Danforth 2012, *IEEE Trans. Smart Grid* 3 — [V](https://doi.org/10.1109/tsg.2012.2213848). Metrics of proximity to critical points from PMU data as cascading-failure risk indicators (abstract) — Hines, Cotilla-Sanchez, O'Hara & Danforth 2011, IEEE PES GM — [V](https://doi.org/10.1109/pes.2011.6039631) (content from memory: autocorrelation increased in frequency data before the 1996 western US blackout). Which statistics are useful (variance of bus voltages vs autocorrelation; content from memory) — Ghanavati, Hines & Lakoba 2016, *IEEE Trans. Power Systems* 31 — [V](https://doi.org/10.1109/tpwrs.2015.2412115).
- Blackout statistics: blackout data from several countries suggest a power-law frequency of large blackouts, consistent with a system "designed and operated near a critical point"; loading relative to limits is key (abstract). Dobson, Carreras, Lynch & Newman 2007, *Chaos* 17 — [V](https://doi.org/10.1063/1.2737822); self-organised criticality in NERC blackout series (content from memory) — Carreras, Newman, Dobson & Poole 2004, *IEEE TCAS-I* 51 — [V](https://doi.org/10.1109/tcsi.2004.834513).
- Branching-process estimates of cascade propagation from utility outage data: the mean offspring λ estimates propagation; λ → 1 is criticality (content from memory) — Dobson 2012, *IEEE Trans. Power Systems* 27 — [V](https://doi.org/10.1109/tpwrs.2012.2190112); Ren & Dobson 2008, *IEEE TCAS-II* 55 — [V](https://doi.org/10.1109/tcsii.2008.924365). Multi-type branching processes estimated by EM; the largest eigenvalue of the offspring mean matrix indicates closeness to criticality (abstract) — Qi, Ju, Sun et al. 2016, *IEEE Trans. Power Systems* — [V](https://doi.org/10.1109/tpwrs.2016.2577633).
- Grid frequency is non-Gaussian: frequency series from North American, Japanese and European grids deviate strongly from Gaussianity, best described by Lévy-stable and q-Gaussian laws, interpreted superstatistically (abstract). Schäfer, Beck, Aihara, Witthaut & Timme 2018, *Nature Energy* 3 — [V](https://doi.org/10.1038/s41560-017-0058-z). An open database of frequency at 17 locations in 12 synchronous areas on three continents (abstract) — Rydin Gorjão et al. 2020, *Nat. Commun.* 11 — [V](https://doi.org/10.1038/s41467-020-19732-7); the database (one-second resolution, twelve synchronous areas) — arXiv:2006.01771 — [V](https://doi.org/10.48550/arxiv.2006.01771); a data-driven stochastic model — Gorjão et al. 2020, *IEEE Access* — [V](https://doi.org/10.1109/access.2020.2967834).
- Cascades as dynamics, not static overloads: dynamically induced cascading failures — Schäfer, Witthaut, Timme & Latora 2018, *Nat. Commun.* 9 — [V](https://doi.org/10.1038/s41467-018-04287-5); load-redistribution cascades — Motter & Lai 2002, *PRE* 66 — [V](https://doi.org/10.1103/physreve.66.065102); interdependent networks collapse abruptly (first-order percolation) — Buldyrev et al. 2010, *Nature* 464 — [V](https://doi.org/10.1038/nature08932); dead-end trees undermine basin stability — Menck, Heitzig, Kurths & Schellnhuber 2014, *Nat. Commun.* 5 — [V](https://doi.org/10.1038/ncomms4969). (Contents from memory.) Swing-equation test model (from memory): M θ̈_i + D θ̇_i = P_i + Σ_j K_ij sin(θ_j − θ_i).
- Iberian blackout, 28 April 2025: ENTSO-E's expert panel final report (about March 2026) identifies no single cause; two periods of oscillations in the preceding half-hour were mitigated by measures that raised voltage, and the collapse proceeded through overvoltage and generator disconnections; increased reactive-power margins could have prevented it — [pv magazine, 23 Mar 2026](https://www.pv-magazine.com/2026/03/23/entso-e-report-finds-systemic-failures-behind-2025-iberian-blackout/), [PV Tech](https://www.pv-tech.org/combination-of-voltage-and-oscillation-caused-iberian-blackout-says-entso-e-report/) [S]. Conflicting account: a 2025 IEEE STPEC paper attributes the blackout to inter-area oscillations and a sudden 2.5 GW loss of solar power with frequency below 48 Hz and low inertia (abstract) — "Power Grid Failures in the Renewable Era: the April 2025 Blackout in Spain and Portugal" — [V via Semantic Scholar](https://doi.org/10.1109/STPEC66316.2025.11490314). Secondary sources disagree on report details (page and recommendation counts) — [search summary](https://www.powermag.com/anatomy-of-a-blackout-findings-from-the-spain-portugal-grid-collapse-final-report/) [S].
- Structural health monitoring: SHM as damage detection from vibration features, with environmental variability as the main confounder (content from memory) — Farrar & Worden 2007, *Phil. Trans. R. Soc. A* 365 — [V](https://doi.org/10.1098/rsta.2006.1928). A 2026 study proposes prospective bridge monitoring on critical slowing ("a structure approaching a change of state recovers more slowly from small disturbances and becomes more variable"), separating structural change from seasonal effects and evaluated without failure labels (abstract) — *Frontiers in Built Environment* 2026 — [V via OpenAlex](https://doi.org/10.3389/fbuil.2026.1930162).
- Supply networks: from a national VAT-based firm network, 0.035% of companies have extraordinarily high economic systemic risk, each affecting about 23% of national production on default; firm size does not explain it, network position does (abstract) — Diem, Borsos, Reisch, Kertész & Thurner 2022, *Sci. Rep.* 12 — [V](https://doi.org/10.1038/s41598-022-11522-z). Firm-level propagation of shocks through supply chains (content from memory) — Inoue & Todo 2019, *Nature Sustainability* 2 — [V](https://doi.org/10.1038/s41893-019-0351-x) (cited as method evidence only). Reconstructing firm networks with realistic systemic risk from sector-level input–output data by maximum entropy (abstract) — *Sci. Rep.* 2026 — [V via OpenAlex](https://doi.org/10.1038/s41598-026-47883-y).

#### 6.3 Inferences
- For grids, an honest detector should combine (i) CSD indicators on PMU/frequency data, robust to heavy tails (e.g. rank-based or quantile autocorrelation), and (ii) structural indicators (branching ratio from outage logs, spectral margins); the Iberian case shows a detector must also watch oscillatory (Hopf-type) modes and voltage, not only frequency.
- Heavy-tailed frequency noise biases variance-based indicators; the superstatistical description implies that rising variance can reflect changing noise statistics (trading intervals, renewables) rather than loss of stability.
- Supply-network systemic risk is a structural property computed on the network, not a time-series signal; it is the infrastructure analogue of DebtRank.

#### 6.4 Gaps
- No peer-reviewed analysis was found of whether CSD indicators in open frequency data rose before the Iberian blackout.
- The empirical claim about autocorrelation before the 1996 western US blackout (Hines et al. 2011) was not verified from the full text.
- The licence of the open power-grid frequency database was not verified on its OSF page (the provider site lists the database and authors but no licence text in what was retrieved: [power-grid-frequency.org](https://power-grid-frequency.org/)).

### 7. Governance: The Decision Value of Warnings, Their Evaluation and Their Communication

#### 7.1 Takeaway
A warning has value only through a decision: its worth depends on lead time relative to the lag of action, on the false-alarm rate against the base rate, and on the costs of acting falsely versus acting late; ecological theory shows warnings often come too late to act on, statistics shows generic indicators carry high false-positive rates and are biased by selecting systems that did transition, and finance has the most developed practice of evaluating warnings against policymakers' declared preferences.

#### 7.2 Cited Findings
- **Too late to act.** In a lake–fishery model, leading indicators detected an impending regime shift, but avoiding it required a fast, often drastic management response; slow responses could not avert the shift (content from memory). Biggs, Carpenter & Brock 2009, *PNAS* 106 — [V](https://doi.org/10.1073/pnas.0811729106). Indicators must give warning far enough ahead to overcome management and ecological lags (content from memory). Contamin & Ellison 2009, *Ecological Applications* 19 — [V](https://doi.org/10.1890/08-0109.1).
- **Error rates.** Proposed methods "hardly ever characterize their expected error rates"; applications must quantify false alarms and missed warnings without replicates or controls (abstract). Boettiger & Hastings 2012, *J. R. Soc. Interface* 9 — [V](https://doi.org/10.1098/rsif.2012.0125). Selecting systems for study because a transition was observed conditions the analysis (the "prosecutor's fallacy") (abstract) — Boettiger & Hastings 2012, *Proc. R. Soc. B* 280 — [V](https://doi.org/10.1098/rspb.2012.2085). Noise-induced transitions give no early warning (title; content from memory) — Boettiger & Hastings 2013, *Proc. R. Soc. B* 280 — [V](https://doi.org/10.1098/rspb.2013.1372); review of charted and uncharted territories — Boettiger, Ross & Hastings 2013, *Theoretical Ecology* 6 — [V](https://doi.org/10.1007/s12080-013-0192-6). Abrupt shifts can occur with no warning (title) — Hastings & Wysham 2010, *Ecology Letters* 13 — [V](https://doi.org/10.1111/j.1461-0248.2010.01439.x). Dansgaard–Oeschger events appear noise-induced, "thus have very limited predictability" (abstract) — Ditlevsen & Johnsen 2010, *GRL* 37 — [V](https://doi.org/10.1029/2010gl044486). CSD also precedes non-catastrophic transitions, so it is not specific to tipping points (abstract) — Kéfi, Dakos, Scheffer, van Nes & Rietkerk 2013, *Oikos* 122 — [V](https://doi.org/10.1111/j.1600-0706.2012.20838.x).
- **Methods and practical conditions.** Standard workflow (rolling windows, detrending, Kendall τ, sensitivity analysis, surrogate nulls) — Dakos et al. 2012, *PLoS ONE* 7 — [V](https://doi.org/10.1371/journal.pone.0041010) (content from memory); prospects and limitations of resilience indicators — Dakos et al. 2015, *Phil. Trans. R. Soc. B* 370 — [V](https://doi.org/10.1098/rstb.2013.0263); theoretical and practical conditions (sampling rate, series length, slowly changing parameter) that psychological data rarely meet (abstract and content from memory) — Dablander, Pichler, Cika & Bacilieri 2023, *Psychological Methods* 28 — [V](https://doi.org/10.1037/met0000450); deep learning trained on normal forms detects approaching bifurcations and their type, which generic indicators cannot (abstract) — Bury et al. 2021, *PNAS* 118 — [V](https://doi.org/10.1073/pnas.2106140118); indicators of transitions in biological systems — Clements & Ozgul 2018, *Ecology Letters* 21 — [V](https://doi.org/10.1111/ele.12948).
- **Skill as a function of horizon.** Early-warning skill quantified with a time horizon by receiver-operating methodology over ensembles; extrapolation can break down in accelerating cascades of coupled tipping elements (abstract). Ashwin, Bastiaansen, von der Heydt & Ritchie 2025, arXiv:2506.01981 — [V](https://arxiv.org/abs/2506.01981).
- **ROC and signal detection.** Swets 1988, *Science* 240 — [V](https://doi.org/10.1126/science.3287615). Cost–loss decision rule (from memory, Murphy/Richardson tradition, not verified this session [M]): act when P(transition | signal) > C/L, C the cost of protective action, L the loss averted; relative value of a warning system V = (E_none − E_system)/(E_none − E_perfect), which depends on the hit rate H, false-alarm rate F, base rate s and the cost–loss ratio α.
- **Finance's evaluation practice** (§4): noise-to-signal ratio (KLR 1998 [V](https://doi.org/10.2307/3867328)), AUROC and policy requirements of timing and stability (Drehmann & Juselius 2014 [V](https://doi.org/10.1016/j.ijforecast.2013.10.002)), loss-function usefulness (Sarlin 2013 [V](https://doi.org/10.1016/j.econlet.2012.12.030); Alessi & Detken 2011 [V](https://doi.org/10.1016/j.ejpoleco.2011.01.003)), unified evaluation framework (Candelon et al. 2012 [V](https://doi.org/10.1057/imfer.2012.4)), recursive out-of-sample tests (Beutel et al. 2019 [V](https://doi.org/10.1016/j.jfs.2019.100693)). Conflict forecasting's practice: true-future windows, independent scoring, probabilistic scores (CRPS) (Vesco 2022 [V](https://doi.org/10.1080/03050629.2022.2029856); Hegre 2025 [V](https://doi.org/10.1177/00223433241300862)).
- **Safe operating spaces and thresholds under uncertainty.** Planetary boundaries are set at the safe end of a zone of uncertainty about thresholds (content from memory) — Rockström et al. 2009, *Nature* 461:472 — [V](https://doi.org/10.1038/461472a); Steffen et al. 2015, *Science* 347 — [V](https://doi.org/10.1126/science.1259855) (comment on freshwater boundary [V](https://doi.org/10.1126/science.aaa9629)). Tipping elements — Lenton et al. 2008, *PNAS* 105 — [V](https://doi.org/10.1073/pnas.0705414105); early warning of climate tipping points — Lenton 2011, *Nat. Clim. Change* 1 — [V](https://doi.org/10.1038/nclimate1143); "too risky to bet against" — Lenton et al. 2019, *Nature* 575 — [V](https://doi.org/10.1038/d41586-019-03595-0).
- **Uncertainty about the threshold undermines collective action**: in experiments, uncertainty about where a dangerous threshold lies makes groups fail to avoid it (content from memory). Barrett & Dannenberg 2014, *Nat. Clim. Change* 4 — [V](https://doi.org/10.1038/nclimate2059).
- **Decision approaches.** Expected utility, thresholds, robust and precautionary approaches, resilience thinking compared for management under great uncertainty (content from memory) — Polasky, Carpenter, Folke & Keeler 2011, *TREE* 26 — [V](https://doi.org/10.1016/j.tree.2011.04.007).
- **Communicating uncertainty.** A framework for communicating epistemic uncertainty about facts and numbers (who, what, in what form, to whom, with what effect; content from memory) — van der Bles et al. 2019, *R. Soc. Open Sci.* 6 — [V](https://doi.org/10.1098/rsos.181870). Public risk perception of tipping points (Norway; preprint) — [V record](https://doi.org/10.5194/esd-2023-23).

#### 7.3 Inferences
- A detector should output not "warning/no warning" but a probability with a horizon and a declared loss trade-off, and its report should be judged by usefulness under that loss, not by significance of a trend.
- The finance and conflict communities' practices (out-of-sample recursion, true-future windows, independent scoring, stability of signals) should be the default standard; the ecological practice (Kendall τ against surrogates on a series known to end in a transition) is a necessary but insufficient check.
- Where lead time is shorter than the time needed to act (Biggs et al.), the warning's value is near zero regardless of accuracy; a report must state the action lag it assumes.

#### 7.4 Gaps
- The cost–loss formulas above were not verified against a primary source this session.
- No study was found that measures the realised value of an early-warning system to a city or infrastructure operator in money or avoided losses.

### 8. Structured States: Representations and the Detectors Suited to Them

#### 8.1 Takeaway
Social and economic states are compositions (age structures, sectoral shares, ethnic shares), networks (interbank, inter-firm, mobility), spatial fields (land use, prices, crime) and point processes (events); the multivariate early-warning literature offers dimension reduction onto the slowest mode, network biomarkers, spatial variance/correlation and topological summaries, but most of it was validated on simulated or ecological systems, and performance on general networks and on compositions is largely unknown.

#### 8.2 Cited Findings
- **Multivariate indicators.** A comparison of multivariate indicators of resilience loss in systems of interacting entities (abstract) — Weinans et al. 2021, *Sci. Rep.* 11 — [V](https://doi.org/10.1038/s41598-021-87839-y) (content from memory: indicators based on the leading principal component and on the maximum of univariate indicators performed comparably; dimension reduction matters). Degenerate fingerprinting: project onto the direction of slowest decay before measuring autocorrelation (content from memory) — Held & Kleinen 2004, *GRL* 31 — [V](https://doi.org/10.1029/2004gl020972). Dynamical network biomarkers: a dominant group whose members' standard deviations and mutual correlations rise while their correlation with the rest falls (content from memory) — Chen, Liu, Liu, Li & Aihara 2012, *Sci. Rep.* 2 — [V](https://doi.org/10.1038/srep00342). Reliability of resilience estimates from multi-instrument series — Smith & Boers 2023, *ESD* 14 — [V](https://doi.org/10.5194/esd-14-173-2023); observation-based AMOC indicators — Boers 2021, *Nat. Clim. Change* 11 — [V](https://doi.org/10.1038/s41558-021-01097-4).
- **Spatial indicators.** Spatial correlation as a leading indicator — Dakos, van Nes, Donangelo, Fort & Scheffer 2010, *Theoretical Ecology* 3 — [V](https://doi.org/10.1007/s12080-009-0060-6); methods for spatial patterns (spatial variance, skewness, patch-size distributions) — Kéfi et al. 2014, *PLoS ONE* 9 — [V](https://doi.org/10.1371/journal.pone.0092097). Spatial EWS use a single sample per location; their performance on general networks (rather than lattices) is "mostly unknown" and is assessed in a 2024 study (abstract) — MacLaren, Aihara & Masuda 2024, arXiv:2410.04303 — [V](https://arxiv.org/abs/2410.04303).
- **Networks.** Topological change with proper null models (degree-heterogeneity-controlled) gives gradual warning where density-only nulls show only an abrupt break (Squartini et al. 2013, [V](https://doi.org/10.1038/srep03357)); systemic impact measures (DebtRank [V](https://doi.org/10.1038/srep00541)); spectral conditions (Bardoscia 2017 [V](https://doi.org/10.1038/ncomms14416)); branching ratios of cascades (Dobson 2012 [V](https://doi.org/10.1109/tpwrs.2012.2190112)); firm-level systemic risk (Diem 2022 [V](https://doi.org/10.1038/s41598-022-11522-z)).
- **Topological summaries of multivariate series.** Persistence landscapes and their L^p norms on sliding-window point clouds (Gidea & Katz 2018 [V](https://doi.org/10.1016/j.physa.2017.09.028); Ismail 2022 [V](https://doi.org/10.3389/fams.2022.940133)).
- **Compositions.** Segregation tipping is measured on tract composition (minority share) — CMR 2008 [V](https://doi.org/10.1162/qjec.2008.123.1.177); occupational composition — Pan 2015 [V](https://doi.org/10.1086/678518). No study found applies log-ratio (Aitchison) geometry to early warnings in compositions.
- **Mobility and activity.** POI visit series for cities (Li et al. 2020 [V](https://arxiv.org/abs/2008.02905)); traffic detector series (Ghadami 2022 [V](https://doi.org/10.1109/tits.2021.3095897)).
- **Point processes.** Hawkes models of crime (Mohler 2011 [V](https://doi.org/10.1198/jasa.2011.ap09546)) and media-based conflict events (Weidmann 2014 [V record](https://doi.org/10.1177/0022002714530431)).

#### 8.3 Inferences
- For a population by age and place, an economy by sector, a city by block: the natural detector is (i) a log-ratio transform of compositions, (ii) estimation of a linear(ised) multivariate model (VAR or state-space) in rolling windows, (iii) tracking the leading eigenvalue (modulus → 1) and its eigenvector (which identifies *where* the slowing occurs), with (iv) spatial or network indicators where the units have a neighbourhood structure. This generalises degenerate fingerprinting and DNB, and the eigenvector output answers "which blocks/sectors/ages" — a decision-maker's question.
- Raw shares near 0 or 1 (e.g., fully segregated tracts) have bounded variance, so variance-based indicators on raw shares are biased near the boundary; log-ratios or logit transforms are needed (inference; no source found).
- Network indicators require null models that preserve degree heterogeneity, or they will report abrupt, unpredictable changes (Squartini 2013).

#### 8.4 Gaps
- Multivariate EWS have not been validated (out-of-sample) on any social or economic structured state found this session.
- Weinans et al.'s ranking of indicators was not retrieved from the full text.

### 9. Data for Honest Tests: Open Records with Known Transitions and Their Terms

#### 9.1 Takeaway
Several open records with dated transitions carry permissive licences (UCDP CC BY 4.0; Penn World Table 11.0 CC BY 4.0; Human Mortality and Fertility Databases CC BY 4.0; V-Dem CC BY-SA 4.0; World Bank CC BY 4.0), the macro-financial record that dates crises is non-commercial (Jordà–Schularick–Taylor, CC BY-NC-SA 4.0), and several widely used sources are restrictive or unverified (ACLED's EULA requires licences for governments and commercial users; FRED's third-party series need the owner's permission; BIS permits non-commercial redistribution; highD is non-commercial on request).

#### 9.2 Cited Findings
- **UCDP** (conflict, incl. GED events): "All datasets are free of charge and licensed under CC BY 4.0 — you are free to use and redistribute them provided you cite the relevant publications" — [UCDP downloads, verified](https://ucdp.uu.se/downloads/) [V]. Transitions: conflict onsets/terminations, fatalities by month and location.
- **ACLED** (political violence and protest events): use is "expressly conditioned on the user's strict adherence" to the EULA, Content Usage Terms and Attribution Policy; governmental and multilateral users "require a public-sector license"; "Commercial entities may not access or use the Content ... without first obtaining a corporate license" (EULA last updated 1 November 2026) — [ACLED EULA, verified](https://acleddata.com/eula/) [V]; use with AI/ML/LLM systems "is subject to strict limitations" — [Content Usage Terms, verified](https://acleddata.com/content-usage-terms/) [V]. **Restrictive**: unsuitable for redistribution with a library.
- **Jordà–Schularick–Taylor Macrohistory Database** (credit, asset prices, crisis dates, 18 advanced economies since 1870): licensed under CC BY-NC-SA 4.0; commercial data providers "strictly forbidden" to integrate or resell; citation required — [macrohistory.net, verified](https://www.macrohistory.net/database/) [V]. **Non-commercial.**
- **Penn World Table 11.0**: "licensed under a Creative Commons Attribution 4.0 International License"; cite Feenstra, Inklaar & Timmer 2015 (page last modified 19 March 2026) — [GGDC PWT page, verified](https://www.rug.nl/ggdc/productivity/pwt/) [V]; Feenstra et al. 2015, *AER* 105 — [V](https://doi.org/10.1257/aer.20130954).
- **Human Mortality Database / Human Fertility Database**: data constructed by HMD/HFD are CC BY 4.0; input data remain under each provider's licence — [HMD user agreement](https://www.mortality.org/Data/UserAgreement) [V]; [HFD user agreement](https://www.humanfertility.org/Data/UserAgreement) [V]. Transitions: fertility declines to lowest-low, mortality shocks; population pyramids by country and year.
- **V-Dem** (regimes, episodes of autocratisation/democratisation): CC BY-SA 4.0, including commercial use with attribution and share-alike — [V-Dem dataset page](https://www.v-dem.net/data/the-v-dem-dataset/) [V].
- **World Bank data** (WDI etc.): default CC BY 4.0 with additional mandatory terms; other licences labelled per dataset — [World Bank Data Catalog licences](https://datacatalog.worldbank.org/public-licenses) [V].
- **FRED**: series may be owned by third parties; for copyrighted series "YOU MUST CONTACT THE DATA OWNER TO OBTAIN PERMISSION"; redistribution of third-party content for commercial use requires written permission; public-domain series (e.g., many US federal statistics) are freer — [FRED legal page via WebFetch](https://fred.stlouisfed.org/legal/) [V]. **Mixed: check each series.**
- **BIS statistics** (credit-to-GDP gaps, debt service ratios, property prices): users may "download, display, print out, photocopy or redistribute any BIS Material for non-commercial purposes"; limited extracts in other publications exclude statistics in the BIS Data Portal — [BIS terms and conditions, verified](https://www.bis.org/terms_conditions.htm) [V]. **Non-commercial redistribution.**
- **highD / inD / rounD / uniD** (drone trajectories on German roads): free for non-commercial use only, access by manually checked request; no redistribution — [levelXdata highD page, verified](https://levelxdata.com/highd-dataset/) [V]. **Non-commercial, no redistribution.**
- **Open power-grid frequency database** (one-second frequency, 12 synchronous areas): described in [arXiv:2006.01771](https://doi.org/10.48550/arxiv.2006.01771) [V] and [Nat. Commun. 2020](https://doi.org/10.1038/s41467-020-19732-7) [V]; hosted on OSF per the provider site — [power-grid-frequency.org](https://power-grid-frequency.org/) [V page]; licence **not verified**.
- **Laeven & Valencia banking-crisis database** (crisis start/end dates, 1970–2017 and an update to 2025): records verified ([2020](https://doi.org/10.1057/s41308-020-00107-3), [1970–2025 WP](https://doi.org/10.5089/9798229045971.001.a001)); IMF terms of use **not verified** (imf.org returned 403).
- **ECB/ESRB EU crises database** — record [V](https://doi.org/10.2139/ssrn.3011859); terms not verified.
- **Not verified this session (from memory, [M])**: OECD Inter-Country Input–Output tables (OECD moved its data to CC BY 4.0 in 2024; oecd.org returned 403); UN World Population Prospects (CC BY 3.0 IGO; page returned no licence text); PeMS (Caltrans loop-detector data, free with registration; the landing page fetched showed no licence text — [pems.dot.ca.gov](https://pems.dot.ca.gov/)); NGSIM trajectories (US DOT open data); Seshat Global History Databank (site links a "User Agreement & Data License" at /terms/current/, which returned 403 — [seshat-db.com](https://seshat-db.com/)); ENTSO-E Transparency Platform; GHSL (CC BY 4.0, page unreachable over TLS here); IPUMS NHGIS census-tract tables (free with registration, redistribution restricted); Shiller's US housing and stock data (page fetched, no licence text — [econ.yale.edu/~shiller/data.htm](http://www.econ.yale.edu/~shiller/data.htm)); ViEWS forecasts (data page fetched, no licence text — [viewsforecasting.org/data](https://viewsforecasting.org/data/)).
- **Records with known transitions suited to each field** (inference from §§1–6): segregation — US census tract racial composition 1970–2020 (CMR's setting); traffic — loop-detector speed/flow/occupancy with breakdown events (PeMS; breakdown defined operationally, e.g., five consecutive one-minute intervals below 30 mph in an older study — [NISS TR-79 via search](https://www.niss.org/sites/default/files/technicalreports/tr79.pdf) [S]); housing — national and regional house price indices with GSADF-dated episodes; banking crises — JST and Laeven–Valencia chronologies; conflict — UCDP GED with onsets; populations — HMD/HFD fertility series with lowest-low onsets; grids — open frequency database and outage logs; regimes — V-Dem episodes.

#### 9.3 Inferences
- For demonstrations that may be redistributed with a library: UCDP, PWT, HMD/HFD, V-Dem and World Bank data are safe (attribution; share-alike for V-Dem); JST and BIS are usable for non-commercial demonstrations only; ACLED, highD and copyrighted FRED series should be fetched by the user under their own agreement, never bundled.
- The "known transition" in each record is itself a coded judgement (crisis chronologies differ; conflict thresholds of 25 battle deaths; breakdown speed thresholds); a test must declare which chronology defines a hit.

#### 9.4 Gaps
- Licences of OECD ICIO, UN WPP, PeMS, Seshat, ENTSO-E, GHSL, NHGIS and the open grid-frequency database were not verified this session and must be checked before use.

### 10. What This Means for the Design

#### 10.1 Takeaway
A library of criticality detectors for structured social and economic states should be verified first against test models whose transitions are known analytically (including models with *no* warning), demonstrated on a small set of openly licensed records with independently dated transitions, and should report like an institutional early-warning system: probability with horizon, lead-time distribution, hit and false-alarm rates under a declared loss, the location of slowing in the state (which blocks, sectors, ages), and an explicit statement of the transitions it cannot see.

#### 10.2 Cited Findings (the evidence each design choice rests on)

**10.2.1 Test models to implement and verify against**
1. *Normal forms with additive noise* (fold, transcritical, Hopf, with a slowly drifting parameter) as calibration of every indicator; deep-learning work trains on exactly these (Bury et al. 2021 [V](https://doi.org/10.1073/pnas.2106140118)). Verify: lag-1 autocorrelation → 1 and variance divergence as λ → 0 (Scheffer 2009 [V](https://doi.org/10.1038/nature08227)).
2. *Negative and specificity controls*: noise-induced switching in a fixed double well (no warning; Ditlevsen & Johnsen 2010 [V](https://doi.org/10.1029/2010gl044486); Boettiger & Hastings 2013 [V](https://doi.org/10.1098/rspb.2013.1372)); a shift with no smooth approach (Hastings & Wysham 2010 [V](https://doi.org/10.1111/j.1461-0248.2010.01439.x)); a non-catastrophic transition that still shows CSD (Kéfi 2013 [V](https://doi.org/10.1111/j.1600-0706.2012.20838.x)); a GARCH-type volatility process with no bifurcation (rising variance without tipping, cf. Guttal 2016 [V](https://doi.org/10.1371/journal.pone.0144198)).
3. *Schelling's model* on lattices and networks, with one-sided and two-sided tipping variants (CMR 2008 [V](https://doi.org/10.1162/qjec.2008.123.1.177); NBER w14470 [V](https://doi.org/10.3386/w14470)), the all-integrationist variant (Zhang 2004 [V](https://doi.org/10.1016/j.jebo.2003.03.005)) and the exactly solvable variant (Grauwin et al. 2009 [V](https://doi.org/10.1073/pnas.0906263106)) as an analytic check. State: block-level compositions.
4. *Granovetter's threshold model* and its network cascade form (Granovetter 1978 [V](https://doi.org/10.1086/226707)); *Brock–Durlauf* mean-field social interactions with bistability for βJ > 1 (Brock & Durlauf 2001 [V](https://doi.org/10.1111/1467-937x.00168)); committed-minority convention change as an experimental benchmark (Centola 2018 [V](https://doi.org/10.1126/science.aas8827)).
5. *A regime-switching economy*: Hamilton's MS-AR(4) (Hamilton 1989 [V](https://doi.org/10.2307/1912559)) — a model with abrupt switches and *no* slowing down, on which a CSD detector must stay silent while a regime filter detects; plus a limit-cycle economy near a Hopf point (Beaudry et al. 2020 [V](https://doi.org/10.1257/aer.20190789)) and a heterogeneous-beliefs asset market (Brock & Hommes 1998 [V](https://doi.org/10.1016/s0165-1889(98)00011-6)); LPPL only as a null hypothesis with Feigenbaum's exclusion test (Feigenbaum 2001 [V](https://doi.org/10.1088/1469-7688/1/3/306)); GSADF on simulated mildly explosive processes with time-varying volatility (PSY 2015 [V](https://doi.org/10.1111/iere.12132); Harvey et al. 2016 [V](https://doi.org/10.1016/j.jempfin.2015.09.002)).
6. *Traffic with a jamming transition*: optimal-velocity model on a ring with density as the slow parameter (instability at V′(h) = a/2; Bando 1995 [V](https://doi.org/10.1103/physreve.51.1035)), Nagel–Schreckenberg (NaSch 1992 [V](https://doi.org/10.1051/jp1:1992277)), IDM with an on-ramp (Treiber et al. 2000 [V](https://doi.org/10.1103/physreve.62.1805)); calibrate against the ring-road experiment (Sugiyama 2008 [V](https://doi.org/10.1088/1367-2630/10/3/033001)). Thousands of repeatable breakdowns make this the place to estimate hit/false-alarm rates.
7. *Cascades on networks*: Eisenberg–Noe clearing (2001 [V](https://doi.org/10.1287/mnsc.47.2.236.9835)), Gai–Kapadia contagion (2010 [V](https://doi.org/10.1098/rspa.2009.0410)), DebtRank (2012 [V](https://doi.org/10.1038/srep00541)), Motter–Lai load redistribution (2002 [V](https://doi.org/10.1103/physreve.66.065102)), interdependent percolation (Buldyrev 2010 [V](https://doi.org/10.1038/nature08932)), and a branching process of outages whose mean offspring is the control parameter (Dobson 2012 [V](https://doi.org/10.1109/tpwrs.2012.2190112)). Verify structural indicators (spectral radius, branching ratio) against the analytic cascade threshold.
8. *Power-grid swing equations/Kuramoto with inertia* under rising load, with heavy-tailed (Lévy/q-Gaussian) noise (Schäfer et al. 2018 [V](https://doi.org/10.1038/s41560-017-0058-z)), testing indicator robustness to non-Gaussian noise and to oscillatory (Hopf) loss of stability.
9. *Crime reaction–diffusion* (Short et al. 2010 [V](https://doi.org/10.1073/pnas.0910921107)) as a spatial-field test of Turing-type hotspot onset for spatial indicators (Kéfi 2014 [V](https://doi.org/10.1371/journal.pone.0092097); MacLaren et al. 2024 [V](https://arxiv.org/abs/2410.04303)).
10. *Urban growth with multiple equilibria* (Krugman 1991 [V](https://doi.org/10.2307/2937950)) and a durable-housing decline model with asymmetric response (Glaeser & Gyourko 2005 [V](https://doi.org/10.1086/427465)) as a control for slow, ratchet-like decline.

**10.2.2 Open records for honest demonstrations (with terms)**
- Conflict onsets and escalation: UCDP GED (CC BY 4.0, [V](https://ucdp.uu.se/downloads/)); benchmark against ViEWS's published forecasts and challenge protocols ([V](https://doi.org/10.1177/00223433241300862)).
- Banking crises and credit: JST Macrohistory (CC BY-NC-SA 4.0, non-commercial, [V](https://www.macrohistory.net/database/)); Laeven–Valencia chronologies (terms unverified); BIS credit gaps (non-commercial redistribution, [V](https://www.bis.org/terms_conditions.htm)). Compare any CSD detector head-to-head with the credit-gap logit under AUROC and a declared loss (Drehmann & Juselius 2014 [V](https://doi.org/10.1016/j.ijforecast.2013.10.002); Beutel et al. 2019 [V](https://doi.org/10.1016/j.jfs.2019.100693)).
- Growth and structural change: PWT 11.0 (CC BY 4.0, [V](https://www.rug.nl/ggdc/productivity/pwt/)); World Bank (CC BY 4.0, [V](https://datacatalog.worldbank.org/public-licenses)); OECD ICIO (licence to verify).
- Populations: HMD/HFD (CC BY 4.0, [V](https://www.humanfertility.org/Data/UserAgreement)) for age-structured series with fertility regime changes.
- Political regimes: V-Dem (CC BY-SA 4.0, [V](https://www.v-dem.net/data/the-v-dem-dataset/)).
- Power grids: the open frequency database (licence to verify, [V record](https://doi.org/10.1038/s41467-020-19732-7)).
- Traffic: PeMS (registration; terms to verify); highD only for non-commercial work by the user's own request ([V](https://levelxdata.com/highd-dataset/)).
- Avoid bundling: ACLED (EULA; public-sector and corporate licences; AI/ML limits, [V](https://acleddata.com/eula/)); copyrighted FRED series ([V](https://fred.stlouisfed.org/legal/)).

**10.2.3 How a detector should report to a decision-maker**
- A probability of transition within a stated horizon, with its calibration (Brier/CRPS, as in conflict forecasting — Hegre et al. 2025 [V](https://doi.org/10.1177/00223433241300862)).
- Lead-time distribution on test models and records, and skill as a function of horizon (ROC over ensembles — Ashwin et al. 2025 [V](https://arxiv.org/abs/2506.01981)).
- Hit rate, false-alarm rate per unit time, AUROC and usefulness under a declared preference μ between missed transitions and false alarms (Sarlin 2013 [V](https://doi.org/10.1016/j.econlet.2012.12.030); Alessi & Detken 2011 [V](https://doi.org/10.1016/j.ejpoleco.2011.01.003); KLR noise-to-signal [V](https://doi.org/10.2307/3867328)).
- Stability of the signal over successive windows (a policy requirement in Drehmann & Juselius 2014 [V](https://doi.org/10.1016/j.ijforecast.2013.10.002)).
- Where in the structured state the slowing is (eigenvector loadings over blocks, sectors, ages, nodes), since action is local.
- The action lag assumed, since warnings shorter than the response time have little value (Biggs et al. 2009 [V](https://doi.org/10.1073/pnas.0811729106); Contamin & Ellison 2009 [V](https://doi.org/10.1890/08-0109.1)).
- What it cannot see: noise-induced transitions, transitions forced faster than the system's relaxation (rate-induced), exogenous shocks, hidden heterogeneous thresholds (Kuran 1989/1991 [V](https://doi.org/10.1007/bf00116762), [V](https://doi.org/10.2307/2010422)), and changes of noise statistics masquerading as rising variance (Schäfer et al. 2018 [V](https://doi.org/10.1038/s41560-017-0058-z)).
- Real-time conditions: one-sided detrending and data vintages (Hamilton 2018 [V](https://doi.org/10.1162/rest_a_00706)).
- Detection after the fact (dating a regime change: Bai–Perron [V](https://doi.org/10.2307/2998540), Hamilton filtering, GSADF date-stamping [V](https://doi.org/10.3982/qe82)) reported separately from anticipation.

**10.2.4 What the negative results say a detector must avoid claiming**
- That rising autocorrelation anticipates financial crashes in general (Guttal 2016 [V](https://doi.org/10.1371/journal.pone.0144198); Diks et al. 2019 [V](https://doi.org/10.1007/s00181-018-1527-3)).
- That a fitted log-periodic pattern predicts a crash date (Laloux 1999 [V](https://doi.org/10.1209/epl/i1999-00122-9); Feigenbaum 2001 [V](https://doi.org/10.1088/1469-7688/1/3/306); Chang & Feigenbaum 2006 [V](https://doi.org/10.1080/14697680500511017)).
- That CSD is specific to tipping points (Kéfi 2013 [V](https://doi.org/10.1111/j.1600-0706.2012.20838.x)) or that every transition is preceded by it (Hastings & Wysham 2010 [V](https://doi.org/10.1111/j.1461-0248.2010.01439.x); Ditlevsen & Johnsen 2010 [V](https://doi.org/10.1029/2010gl044486)).
- That success on series known to end in a transition establishes forecasting skill (prosecutor's fallacy — Boettiger & Hastings 2012 [V](https://doi.org/10.1098/rspb.2012.2085); this applies to the Neolithic and Pueblo results, [V](https://doi.org/10.1073/pnas.1602504113), [V](https://doi.org/10.1073/pnas.2024397118)).
- That a tipping threshold estimated from the same data is evidence of tipping (CMR's split-sample design vs Easterly's smooth-dynamics result, [V](https://doi.org/10.2202/1935-1690.1492)).
- That high in-sample fit implies out-of-sample skill (Beutel et al. 2019 [V](https://doi.org/10.1016/j.jfs.2019.100693); Ward et al. 2010 [V](https://doi.org/10.1177/0022343309356491)).
- That forecasts are equally good for rare, low-level or first onsets (Hegre et al. 2021 ISQ [V](https://doi.org/10.1093/isq/sqaa094); Mueller & Rauh 2018 [V](https://doi.org/10.1017/s0003055417000570)).
- That network topology changes are predictable without a heterogeneity-preserving null (Squartini 2013 [V](https://doi.org/10.1038/srep03357)).
- That urban scaling exponents identify a city's phase independently of boundary definitions (Arcaute 2015 [V](https://doi.org/10.1098/rsif.2014.0745); Leitão 2016 [V](https://doi.org/10.1098/rsos.150649)).
- That a "social tipping point" has been identified when only a positive-feedback narrative is offered (Milkoreit 2022 [V](https://doi.org/10.1002/wcc.813)).

#### 10.3 Inferences
- The strongest design is dual: *structural* indicators computed from the model or network (spectral radius, branching ratio, leading eigenvalue of a fitted multivariate model, distance to a threshold of a slow variable such as leverage or minority share) together with *statistical* CSD indicators on the observed fast variables, each evaluated separately and jointly against a declared loss. Finance shows the structural route outperforming; traffic and grids show the statistical route working where physics guarantees a slowly approached bifurcation.
- Traffic and power grids are the honest first demonstrations (many repeated transitions, open data, physical basis); banking crises and conflict onsets are the honest second (few events but mature chronologies and evaluation norms); segregation and demographic regime changes are research questions where the detector should be presented as exploratory.
- A compositional/spatial/network state needs transforms (log-ratios), null models (heterogeneity-preserving) and locality in the report (eigenvector loadings); none of this is validated in the social literature yet, so the library should verify it on Schelling, cascade and reaction–diffusion test models before any claim about real cities.

#### 10.4 Gaps
- No published benchmark suite of social/economic test models with known transitions was found; the list above is a proposal built from the cited models.
- No study found evaluates a multivariate or spatial CSD detector on a real city's block-level records; the design's flagship use case has no precedent to compare against.

## Part VI. What Was Built

Built on 2026-11-19 ([ADR 0135](../../../development/decisions/0135-detectors-of-critical-transitions.md)), in four layers.

### 1. kinesis's `transitions` (its decision 0008, 77 checks)

- **The abstraction over state spaces** of §1 and Part II §11. A space declares a chart, a neighbourhood and a hierarchy:
  - identity;
  - isometric log-ratios, with each unit's log total kept apart;
  - quantile functions (W₂ on the line);
  - the whitened matrix logarithm (an isometry of the affine-invariant metric at its reference).

  A hierarchy is read at every level in its totals and in its contrasts within groups, the two channels Part II §7 identifies.
- **Indicators of one coordinate, of many, and on a neighbourhood** (Part I §9.1, Part II §11.2). Each declares what it estimates, sees, is blind to and assumes.
- **The restoring rate by four estimators:** AR(1); Boers's increments with AR(1) noise; the OU-driven-by-OU autocorrelation of Morr and Boers; a Kramers–Moyal drift slope.
- **Nulls, error rates and robustness** (Part I §4): a fitted linear null and a declared alternative give each test's false-alarm and detection rates and ROC area at the record's length; phase, IAAFT and block surrogates; a grid of windows and bandwidths.
- **The three verdicts with their reasons** (§1, Part I §9.2).
- **Further detectors:** the dominant eigenvalue's path (Part I §5); wells and switching (Part I §3); a likelihood test of a drifting against a constant rate that refuses an unbounded tipping time (Part I §4).
- **Test models** with known answers, positive and negative (Part I §9.4).

### 2. The climate domain's first C++ (31 checks)

- **The energy balance**, in a smooth form and a ramp form. The ramp form is the non-smooth negative control of Part III §6.1.
- **The overturning in two boxes**, in Berglund and Gentz's form, with the folds of Part III §2.2.

Each model's records are read by kinesis's detectors in the checks.

### 3. The API, the drawings and the Dynamics Lab

- **The API's `science/transitions.hpp`** (23 checks).
- **The visualization layer's `dynamics/transitions.js`** (29 checks).
- **The Lab:** two tools and an example of six systems, with 7 checks in its suite. A fold is a signal. Noise alone, a branch outrun at constant rate and reddening noise are not called slowing. An oscillator points to the circle, and a ring of units slowing together shows in its covariance.

### 4. Departures from the Parts, found while building

- **The test is of the change, not of Kendall's τ.** With windows of half the record, a stationary record's windowed series is monotone too often for τ to have power. A slowly driven fold whose variance rose sevenfold was not significant by τ. τ is reported as the literature reports it.
- **"No signal" asks more than a powered test** (Part I §9.2): the observed change must also be rare under the declared alternative. A ring that slowed, but less than the alternative, had been called no signal at p = 0.06.
- **Where the noise is red, a slowing that the red-noise rate does not confirm is undecidable.** This is Part I §9.2's condition, implemented with declared bounds: the red fit leaves under three quarters of white noise's residual, at θΔt < 3. Reddening noise had otherwise raised variance, autocorrelation and the AR(1) rate together.
- **The red-noise autocorrelation is symmetric in its two rates.** So a record identifies the pair, not which is the system's; the estimator reports both, and assumes the noise is the faster.

### 5. Not Built

- the demonstrations on open records of §1 (the NTL lakes, EPICA, PKU GIMMS NDVI, Project Tycho, a city's records by block, scalp EEG);
- learned classifiers;
- finite-size scaling;
- structural thresholds for cascades and percolation;
- persistent homology with volatility-matched nulls;
- transfer operators and Koopman spectra in windows;
- trajectories of covariances beyond their chart at a reference;
- Wasserstein geometry beyond the line;
- North's diffusive energy balance as a field;
- the Epileptor and Wilson–Cowan test models;
- the seizure-anticipation harness of Part IV §9;
- the reporting standard of Part V §10 for a decision-maker: probabilities with horizons, lead times, losses.
