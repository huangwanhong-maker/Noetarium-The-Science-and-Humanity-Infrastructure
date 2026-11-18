# 0127. Theories of the Case in Space and Time, Searched For, Read Under the Law and Tested

- **Status:** Accepted, provisionally (2026-11-18). The module's name (`theories`), the costs of an invention and of a hole, the example and its numbers are provisional.
- **Date:** 2026-11-18
- **Decided by:**
  - **The request.** "Let us revise the jurisprudence and law foundation/domains. Integrated the envoluntionary reasoing/discovery/interpretation mechaisim into it first (may refer to needs discovery envoluntionary algorithms. envoluntionary computing foundation, etc). And has evidence spacetime visualizer. We are in fact building a multi-modal archieve with well-formed ontologies, for recognize trajectory generativity of a system."
  - **The design.** Claude, here, on tekmerion, kinesis, heuresis, apodeixis, archeion and kategoria. The user was away; every choice below is open to revision.

## 1. Context

The law domain reasons from facts under norms: what follows, under which readings, and what remains to be proved (ADRs 0113, 0115, 0121). It takes the facts as alleged, admitted or proved; it does not ask what happened. tekmerion compares scenarios against a chart (`reconstruction`), places evidence in space and time (`located`, ADR 0123) and reads findings under their open terms (`construal`, ADR 0126). But nothing searched for accounts of what happened, nothing checked an account against how fast people move, and nothing tested an account on evidence it was not built from. The needs domain (ADRs 0125, 0126) had shown the mechanism the user asked for: hypotheses made, varied and crossed by heuresis's evolution, judged, kept by an archive that admits and retires with reasons, read under the readings of their terms, a model proposing in four roles and never judging, and a confirmation part the search never sees, each test sealed before it is read.

In law the hypotheses are theories of the case. Two classical difficulties make a search worth having. A prosecution offers one account and a defence another; an account the evidence allows equally well, which no one put forward, may never be weighed (Wagenaar, van Koppen and Crombag 1993 and Bex 2011 ask that alternatives be sought). And whether a person could have been at a door between being seen at an inn and answering a telephone at home is exact, and is often checked by hand if at all.

The user's last sentence names what the infrastructure is becoming: a multi-modal archive (documents, images, records, testimony, each kept with its digest) with well-formed ontologies (schemas verified by kategoria), for recognising the trajectory generativity of a system. Here a system is people moving at bounded speeds among places and events; its trajectories are the paths it can generate between what the archive's items manifest; to recognise them is to find which trajectories the manifestations admit, never to assert what happened (the ontological register is left open, after the user's Generative Relational framework).

## 2. Decision

### 2.1 The Foundations

- **kinesis's `prisms`** ([its decision 0006](https://github.com/huangwanhong-maker/kinesis/blob/main/development/decisions/0006-prisms.md); 32 checks, kinesis 496). What a record of positions identifies of a path under a bound on speed: Hägerstrand's space-time prism, the Alexandrov interval of a cone structure of speed v.
  - The deepest point of finitely many discs, exactly (one, two or three active).
  - Consistency: every pair of fixes joinable (the pair that breaks named, short by so much); a witness history by cyclic projections, checked exactly; otherwise undecided.
  - Reach: on each piece between fixes the depth of the slice and a region is convex in time, so the instants within reach form one interval a piece; presence in a region is the set {in, out} over admissible histories, an alibi certified.
- **tekmerion's `scenarios`** ([its decision 0009](https://github.com/huangwanhong-maker/tekmerion/blob/main/development/decisions/0009-scenarios-in-space-and-time.md); 33 checks, tekmerion 304). tekmerion now declares heuresis and kinesis.
  - **An account**: events of declared kinds with their hands, places and instants, and a stance on every located item (a sighting, an event of a kind or a slot of the story, an absence, an interval), believed (a sighting at an instant within its span) or doubted.
  - **Coherence**: every person's fixes admit a history at their speed (kinesis), every believed report explained by the person it names, no believed absence contradicted, every believed interval kept.
  - **Plausibility**: believing an item of credibility c costs −log c, doubting it −log(1 − c); an invention costs a nat (an event nothing speaks for, an event of a slot beyond its first, a role left to no one: "someone else did it" costs what positing an unevidenced event costs); a hole two. Relative plausibility over the accounts found is exp(−score) normalised (Allen and Pardo 2019); it ranks what was found only.
  - **The search**: heuresis's evolution, four generators, nine mutations (among them a repair that retimes an event where a path cannot be travelled, and an alternative that gives the central event to another hand or kind), three crossovers, a distance between accounts; the archive keeps, in each niche, the best coherent account. A niche is what the account says of the central slot (by which kind, whose hand) and what a caller adds: the law's answers. So the best account in which the accused did not do it is sought as actively as the best in which they did (MAP-Elites's idea).
  - **A model's four roles** (seed, mutate, cross, reflect), its proposals read like anyone's and credited to their source, as in the needs search.
  - **Nothing held back reaches the search**; `test` asks whether an account admits items it was not made from (its events held, its sightings' instants free within their spans), and what it must doubt when it does not.

### 2.2 The Law Domain

A module `theories` (22 checks) and a schema (`schema/theories.cat`, 12 statements, 6 checks); the domain has 170 checks. Its note is [theory/theories-of-the-case.md](../../domains/law/theory/theories-of-the-case.md).

- **What a theory makes hold.** Bindings say that an event of a kind with an agent in a role makes a fact hold, and that a believed item makes one hold. The facts the bindings address are proved where the theory makes them hold and not proved where it does not; the rest of the record stands as the matter declares it. Each question is then asked under the theory as any question is: clear, arguable or untenable over the admissible readings.
- **Over the theories found.** For each question: the theories under which it is clear, arguable and untenable, the relative plausibility of each side, the best of each. Clear over the theories found when it follows under all, untenable under none, arguable otherwise; said of what was found.
- **The burden.** An alternative is reasonable when it is a coherent theory found under which the question is not clear, of a relative plausibility of at least a threshold the matter declares. No threshold is any jurisdiction's; "so that the court is sure" is not a number in any law. An alternative not found is not thereby excluded.
- **The test.** Each theory kept is tested once on what the matter holds back, the test registered in archeion's hash-chained journal before the items are read and its outcome written after: admitted, must doubt, or no longer holds.
- **Could they have been there.** Whether a person could have been at the scene at the time is read off their sightings (kinesis's presence) under every reading of "at the scene" and "at the time", each reading with its source (tekmerion's construal): clear, arguable or untenable, with its pivots.
- **The schema as categories.** The layers of a theory (a holding, an item read from it, an account it bears on, what happened) lie in the three registers in order, what happened the terminal object; a theory's record (events of kinds in accounts, parts in roles, items read from holdings of a modality, stances taken in accounts, bindings) is thin, a part is played in a role of its event's kind, a holding is manifestational whatever its modality, and no arrow lets an item name an event. Accounts written by the domain are verified as set-valued functors on it by kategoria, whose verdict shares none of the domain's code.

### 2.3 The Layers Above

- **The API.** `science/theories.hpp` (the composition: the matter's evidence in space and time read with clock times on an epoch, the search run a few generations from its state, the tests sealed, the issues over the theories, presence, the evidence for drawing; and `assess`, one theory written by a person), `science/evolution_state.hpp` (heuresis's state as JSON for any genotype), and `science/law.hpp`'s case-building made one function both use. `science.law` gains `theories`, `assess_theory` and `fulfil_theories` (a model's theories in its four roles, read by the engine, entered as grade C; the model is shown the evidence as the search sees it and never what is held back).
- **The visualization layer's cube** (`evidence/spacetime.js`, 29 checks; 747 in the layer): the place on the floor and time rising, each item a bar over its span and a ring for its error, each person's prisms, a theory's paths and events, a plane at an instant with each person's slice, and the plan from above; the slices mirror kinesis's.
- **The Law Studio** gains two views and a plug-in.
  - *Space and time*: the cube as the page, turned by hand, a ribbon of time with the plane, the theories as chips, an item opened where it stands, and beside it whether each person could have been at the scene.
  - *Theories*: each question over the theories found and whether the alternatives are excluded; the field of theories as a band, each as wide as its relative plausibility and toned by the question under it; a theory opened as its story minute by minute, what the law makes of it and its test; the search's generations and operators; a theory of one's own, assessed and entered; the ledger of tests.
  - Routes `theories`, `theories/run`, `theories/model` (only from the work's own page), `theories/assess`, `theories/propose`, `theories/reset`; a project's `theories/` folder.
  - The plug-in `incident`: an invented ordinance and *The fire at the boathouse* (invented), twelve documents across modalities, one disclosed late and held back.

## 3. What the Example Shows

On the invented fire (an investigator's window 21:35–21:55 and a flame put to paraffin; a telephone mast, an inn's camera and a landline placing Corrie Vane, on a bicycle at 6 m/s; a caretaker who walks at 1.2 m/s, at home and later paying at the inn; a man in a grey coat seen on a footbridge; a witness who saw a light at about a quarter to ten; a receipt for paraffin; a till roll placing Vane at the inn at 21:44:20, disclosed late and held back):

- **Whether each could have been there.** Vane, clearly, under every reading. Tom Ashby arguably (three readings of four): not at the east door while Mr Hale saw the light, though within the grounds; it turns on the time.
- **The parties' theories tie.** The prosecution's (Vane lit it at 21:45:10, between the inn and home) and the defence's (the man in the grey coat lit it) are each coherent believing every item, and equally plausible. Where people were does not say whose hand it was.
- **The search finds what neither party put forward.** The caretaker's hand, coherent only if Mr Hale is doubted; an ignition by someone not named, a nat dearer; a fault of the heater, if the caretaker's log is doubted; and Vane's hand with the receipt doubted, under which the charge is untenable (and which is too implausible to count).
- **Damage by fire is clear under none of the theories found.** Under Vane's it is arguable, turning on whether intention may be inferred from buying paraffin beforehand; the alternatives are not excluded at the matter's threshold of 0.05.
- **The test.** The till roll held back could not be admitted by the prosecution's theory: Vane could not have paid at 21:44:20 and still reached the door and then home by 21:51, so the theory must doubt a record of credibility 0.97, at a cost of 3.5 nats. The defence's theory admitted it. Each test was registered in the journal before the till roll was read.

None of this is any court's finding. It is what the declared evidence, speeds, costs and threshold give.

## 4. Alternatives Considered

- **The search in the law domain**, as the needs search lives in the needs domain. The operators and the archive name no legal object; the infrastructure's principle puts general methods in the foundation where they belong, and tekmerion is the foundation of the reconstruction of what happened.
- **Prisms in physis**, beside causal sets. What is asked of them is identification from a record, kinesis's question.
- **A probability over theories** (a Bayesian network of each scenario). It needs likelihoods for every event under every account, which the evidence rarely states; the stance's likelihood needs only each item's credibility, and says what it is.
- **Exhaustive enumeration of accounts.** Possible only on toy problems; the checks use small ones to hold the search to known answers.
- **Holding evidence back by a keyed hash**, as the needs search splits clusters. A matter's items are few and not exchangeable; which evidence is kept aside (late disclosure, or a deliberate test) is the matter's to declare.
- **A three-dimensional view on three.js.** The cube is SVG with its own projection, turned by hand, printable, held by checks in Node; three.js stays available to a work that wants it.

## 5. Consequences

- kinesis 496 checks (32 new); tekmerion 304 (33 new); the law domain 170 (28 new); the visualization layer 747 (29 new); the Law Studio 65 (19 new, among them the stand-in checks of the model's part); the page driven through both views (13 steps, not in the suite), twelve pictures in the gallery's §41.1.
- Every foundation that depends on tekmerion (zetesis) now reaches heuresis and kinesis through it; the dependency checks and `check-alone` know it.
- **Not built:**
  - streets and obstacles in the prisms (a path that must cross a river at a bridge);
  - dependence between items within an account (fold shared origins first);
  - located items read from documents by a model, each with its passage;
  - theories exported as linked data (CIDOC CRM's events and periods, or SEM) for the Archive Studio;
  - an account's findings construed beyond presence (what counts as "the time of the fire");
  - the search for the ratio of precedents, and theories of the law as well as of the facts;
  - the needs search's copy of the evolution state moved onto `science/evolution_state.hpp`.
- **Provisional:** the costs (a nat an invention, two a hole), the threshold of the example, the module's name.

## 6. References

- Hägerstrand, T. (1970), What about people in regional science?, *Papers of the Regional Science Association* 24. Miller, H. J. (2005), A measurement theory for time geography, *Geographical Analysis* 37.
- Wagenaar, W. A., van Koppen, P. J. and Crombag, H. F. M. (1993), *Anchored Narratives*. Bex, F. J. (2011), *Arguments, Stories and Criminal Evidence*. Pennington, N. and Hastie, R. (1993), Reasoning in explanation-based decision making, *Cognition* 49.
- Allen, R. J. and Pardo, M. S. (2019), Relative plausibility and its critics, *International Journal of Evidence and Proof* 23.
- Mouret, J.-B. and Clune, J. (2015), Illuminating search spaces by mapping elites, arXiv:1504.04909.
- Huang, W., *Rethinking Relation in a Generative Relational Framework* (read here from the author's summary).
