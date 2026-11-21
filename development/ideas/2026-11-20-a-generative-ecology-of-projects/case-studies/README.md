# Case Studies of the Generative Ecology of Projects: A Record

*Seed (2026-11-21). Three studies on real material of this repository, made before a paper is written from the design: three units of code, the design's own documents, and a fragment of the theoretical corpus. [The plan](PLAN.md) was written before the runs and fixes the measures and what would count against the design; it was not sealed before them, as Section 2.6 says. This record says what happened, the departures from the plan and the failed runs included. Its numbers are computed by `report.py` from the files under `results/`, except where the text says that a figure was recorded by hand, and each program runs again from this folder. The record was corrected after two later readings, which Section 3.5 reports.*

## 1. Summary

The mechanisms of the design operated on real material. Genomes of three units of the repository were sequenced within enforced bounds, 63 bodies were developed from them and judged by the repository's own checks, 63 contests were carried on the ground in 126 runs, one for each of two challengers, with energy conserved and every journal sealed, the documents of the design were held to their data, and a fragment of the theory of the economics domain was declared as a site on which 50 claims were warranted and decided.

The studies also corrected the design, and the corrections are their main result.

1. *A genome carries what is particular to a unit, and the developer's stock supplies the rest.* Two of the three units implement published formulas. From the identity alone every development of the sphere was sound, and 49 of 56 functions of the Sun were, though only 3 of 7 developments were sound throughout: what they lost were conventions. The third unit follows one variant of a classification among several, and from its identity alone no development was sound. The sequenced genome gave 7 of 7 sound developments of the Sun and 5 of 7 of the third unit. Regeneration must therefore be read against what the developer knows already. The sample is three units and one family of models.
2. *The repository's own texts miss some of what only bodies and checks hold, and a short genome can lose what longer texts keep.* From the repository's texts one label was lost in all 7 developments of the third unit, since it is written only in the body and in a check, and the names of the classes of daylight in 3 of 7, although the unit's note gives them. The sequenced genome of the third unit, of 525 words against 2,350, lost part of the classification itself in 2 of 7 developments, which the longer texts kept in all 7.
3. *The repository's checks are weaker than the design supposed.* They kill 69, 87 and 43 percent of the distinct mutants of the three units, 272 of 411 together, and seven exported names have no check that kills any.
4. *The checks rejected nearly every development that held a fault, and they saw about half of the faults.* Of 23 developments with a faulty function, 22 failed a check, so that one faulty function of 57 lay in a development that passed, and both challengers refuted it. A failing check called the faulty function itself for 29 of the other 56; 27 went down only with their development, among them an azimuth mirrored east for west in two developments, which the repository's check of the azimuth cannot see.
5. *Challenging paid only where faults were common.* On the two units with few faults the challengers lost 152 units of energy in all; on the third they gained 111.
6. *The better model was the better challenger, whoever had produced.* No effect of a difference of origin could be told from an effect of capability.
7. *Stakes carried information.* The mean stake on functions that proved faulty was 6.6, against 11.2 on sound ones, and it was the lower of the two in each of the 23 developments that held both kinds.
8. *The records written by hand held errors, which a procedure, a reading by hand and an independent review each found in part.* Six of 59 sentences bound to the prototypes' data were wrong in the documents committed that morning, and the two later readings found errors of other kinds, among them errors of this record (Section 3.5).
9. *The plan itself held an error,* one tolerance set inconsistently, which the results exposed, and it was not sealed before the runs. Section 2.6 reports both.

## 2. Study A: Three Units of the Repository

### 2.1 Material and Harness

| Unit | File | Lines | Functions | Checks of the repository | Hidden inputs |
|---|---|---|---|---|---|
| The Sun seen from the Earth | `domains/astronomy/js/sun.js` | 83 | 8 | 10 | 2,400 main, 62 edge |
| The Earth as a sphere | `domains/geodesy/js/sphere.js` | 70 | 8 | 6 | 2,400 main, 70 edge |
| The classification of climates | `domains/climate/js/climate.js` | 109 | 6 | 7 | 1,290 main, 39 edge |

: The three units. The third was added after the first two had been evaluated, for the reason and with the prediction that Section 7 of the plan states.

The harness (`harness_js.py`) decides three kinds of fact about a body. It runs the repository's own checks of the unit, copied by script from the repository's test file and from its shared harness of checks. It compares the body with the accepted implementation on hidden inputs under the unit's documented contract (`units.py`). And it does the same for mutants. A body written by a model is screened and then run by Node under its permission model, which allowed reading inside one working folder and nothing else. Before any use the harness was held to the accepted implementations themselves: every check of the repository passed, and each unit agreed with itself on every hidden input.

A function is called *sound* when it agrees with the accepted implementation on every hidden input of both tiers, within tolerances that are tight where the unit's documents give a formula (10⁻⁶ degree for the Sun). Soundness is therefore agreement with the accepted body, which is what regeneration asks. It is not accuracy: a body can differ from the accepted one and lie within the unit's stated accuracy of 0.01 degree, as one development's sidereal time did, and from the identity alone a developer cannot know which published formula the unit follows.

### 2.2 A1: The Strength of the Repository's Checks

| Unit | Checks | Mutants | Not told apart | Distinct | Killed by the checks | Share |
|---|---|---|---|---|---|---|
| `domains/astronomy/js/sun.js` | 10 | 186 | 21 | 165 | 114 | 69% |
| `domains/geodesy/js/sphere.js` | 6 | 136 | 17 | 119 | 103 | 87% |
| `domains/climate/js/climate.js` | 7 | 151 | 24 | 127 | 55 | 43% |

: Mutants of the three units under the declared operators (`results/a1-kills.json`). A mutant is distinct when the hidden inputs tell it from the unit, and killed when a check of the repository fails on it. No mutant failed to load.

By operator, over the three units, the checks killed: arithmetic changed, 105 of 140; comparison changed, 5 of 26; constant plus one, 66 of 108; constant times 1.001, 44 of 78; function exchanged, 52 of 59. Changed comparisons survive most often: they move a boundary, and few checks stand on a boundary.

Some exported names have no check that kills any of their mutants: `SUNRISE_ALTITUDE`, `daylight` in the unit of the Sun seen from the Earth; `EARTH_RADIUS_M`, `zoneFraction` in the unit of the Earth as a sphere; `koppenGrid`, `koppenValue`, `meanTemperature` in the unit of the classification of climates. The repository's check of the mean radius compares a distance with the unit's own constant, so that no value of the constant can fail it.

*Limit.* A mutant that the hidden inputs do not tell apart may be equivalent to the unit or may lie beyond those inputs, and the share is taken over the distinct mutants only. The share depends on the tolerances of the contract: a coefficient changed by a thousandth is distinct for hidden inputs compared at 10⁻⁶ degree and may lie within the unit's stated accuracy.

### 2.3 A2: Regeneration

Three genomes were given to developers who saw neither the body nor the checks: the unit's identity (its purpose in a sentence, its exported names with their parameters and the shape of what they return, and the names it may import with their conventions); the repository's own texts (the unit's comments and signatures with every body and value removed, and its theory note where it has one); and a genome sequenced by a model within bounds of 120, 220 and 80 words for identity, laws and development and of twelve exemplars. Each genome was developed five times by the cheap model and twice by the careful one.

| Unit | Genome | Words | Characters | Developments passing every check | Developments with every function sound | Functions sound |
|---|---|---|---|---|---|---|
| The Earth as a sphere | identity alone | 89 | 618 | 7 of 7 | 7 of 7 | 56 of 56 |
|  | the repository's texts | 425 | 2,578 | 7 of 7 | 7 of 7 | 56 of 56 |
|  | sequenced genome | 371 | 2,645 | 7 of 7 | 7 of 7 | 56 of 56 |
| The Sun seen from the Earth | identity alone | 159 | 1,023 | 4 of 7 | 3 of 7 | 49 of 56 |
|  | the repository's texts | 1,176 | 6,590 | 4 of 7 | 4 of 7 | 53 of 56 |
|  | sequenced genome | 422 | 3,381 | 7 of 7 | 7 of 7 | 56 of 56 |
| The classification of climates | identity alone | 123 | 839 | 0 of 7 | 0 of 7 | 7 of 42 |
|  | the repository's texts | 2,350 | 13,228 | 0 of 7 | 0 of 7 | 35 of 42 |
|  | sequenced genome | 525 | 3,242 | 5 of 7 | 5 of 7 | 37 of 42 |

: Regeneration of three units from three genomes (`results/a-summary.json`). A function is sound when it agrees with the accepted implementation on every hidden input of both tiers.

**Sequencing kept its bounds at the first attempt for all three units.** The sequenced parts had 50, 98 and 47 words for the Sun, 49, 146 and 32 for the sphere and 47, 118 and 42 for the climates. The bounds did not bind: no part exceeded 66 percent of its bound. Words are an unsteady measure of such a text, since a formula written without spaces counts as one word: the laws of the Sun's genome run to 10.2 characters a word, against 6.9 and 6.7 in the other two. The sequencer's instruction allowed compact formulas and said how words would be counted. Stating a bound in characters or tokens is therefore a choice of design that this study suggests and does not test.

**One unit was regenerated whole from its identity alone, and a second in most of its functions.** From the identity alone, 7 of 7 developments of the sphere had every function sound. Of the Sun, 3 of 7 had, and 49 of 56 functions were sound. These units implement published formulas, and the developers supplied the formulas, with their coefficients, from their own stock. Where the Sun's developments differed from the accepted body they differed in conventions: the azimuth was mirrored east for west in two developments, the classes of daylight had other names in two, a class was decided otherwise at its exact boundary in one, and one used another series for sidereal time. From the sequenced genome all 7 developments of the Sun were sound, so that a genome mattered for this unit as well, in what is particular to it.

**The third unit was regenerated only from a genome.** From its identity alone, 0 of 7 developments of the classification of climates had every function sound, and 7 of 42 functions were sound. The developers wrote other variants of the classification: other thresholds of aridity, another rule where a dry summer and a dry winter both hold, another numbering of the classes. From the repository's texts 35 of 42 functions were sound, and from the sequenced genome 37 of 42, with 5 of 7 developments sound throughout. The prediction written before this unit was run held in its three parts, the second by the count of sound functions only: no development from the repository's texts was sound throughout, as none from the identity was.

**The sequenced genome lost substance that the longer texts kept.** The classification itself, the function `koppen`, was sound in 7 of 7 developments from the repository's texts and in 5 of 7 from the sequenced genome, where the other two classed 34 and 45 of 300 climates otherwise. The sequenced genome regenerated more developments whole, since it carried the conventions, and in two it dropped part of the rule, which 2,350 words of note and comments had kept and 525 words did not always keep.

**The repository's texts lost one convention in every development and another in some.** In all 7 developments of the climates from the repository's texts one function was faulty, and for one reason: it returns a label of the warmer half-year, and the two labels are written in the body and in the repository's check and in no document. In 3 of 7 developments of the Sun the classes of daylight were returned under other names, although the unit's note names the five classes in a table exactly as the function returns them; no text says that these names are the strings returned. And the comment of the sphere unit describes the mean radius by a formula whose value differs from the constant in the body by 0.03 m; the five developments of the cheap model from the repository's texts followed the comment, which the corrected contract counts as sound (Section 2.6).

**Bodies came in every size.** The accepted bodies have 83, 70 and 109 lines. The developed ones had from 1 to 80 lines; 23 of the 63 were written on a single line, all by the cheap model. No check sees the form of a body.

### 2.4 A3: The Warranted Change on the Ground

Every development was treated as an artefact answering a need. Its producer staked from 0 to 20 on each function, a reviewer of the producer's model read it without a harness, and a challenger of each model put inputs to it, which the harness decided against the accepted implementation. The 126 runs of the ground were all sealed, in 2,105 journal entries, with energy conserved after every act.

Of 462 functions in 63 developments, 405 were sound, 51 were defective and 6 were faulty only at the edge of their domain. By unit the faulty ones numbered 10, 0 and 47 for the Sun, the sphere and the climates.

| What a person would be shown | Functions | Faulty among them |
|---|---|---|
| Everything developed | 462 | 57 |
| What the repository's checks let pass | 318 | 1 |
| The same, and accepted by the reviewer | 318 | 1 |
| The same, and standing after the challenger of the producer's model | 308 | 0 |
| The same, and standing after the challenger of the other model | 308 | 0 |
| With no checks: accepted by the reviewer | 449 | 48 |
| With no checks: standing after the challenger of the producer's model | 437 | 41 |
| With no checks: standing after the challenger of the other model | 432 | 36 |
| With no checks: standing after both challengers | 423 | 27 |

: What reaches a person under each filter. A development passes the checks when every one of the repository's checks passes on it; a function stands when its claim was warranted and was not refuted. Ten functions fewer stand than pass, since nine claims were refused for lack of energy and one was refuted.

**The checks rejected nearly every development that held a fault.** Of the 63 developments, 41 passed every check. A faulty function lay in 23, and 22 of these failed a check, so that 56 of the 57 faulty functions lay in developments that failed.

**At the grain of the function the checks saw about half of the faults.** A failing check calls the faulty function itself for 29 of those 56. The other 27 went down only with their development, no failing check having called them: `koppenGrid` nine times, `dryMonths` seven times, `koppenValue` seven times, `sunAltAz` twice, `koppenQuantities` once, `subsolarPoint` once; three of them lay in a development whose suite stopped on an error. Two developments returned the azimuth mirrored east for west, wrong on every one of 300 inputs, and passed the repository's check of the azimuth: the check looks from a place due south of the subsolar point, where the Sun stands due north and a mirror changes nothing. Both developments failed for another reason, a name or a formula. The table above filters at the grain of the development, as a pipeline does, and hides this. A design that stakes and accepts by function cannot read the failure of a development as knowledge of each of its functions.

**The challenge caught what the checks let pass.** One faulty function lay in a development that passed every check: a class of daylight decided wrongly at its exact boundary, which no check of the repository visits. Both challengers put the boundary to it and collected its stake.

**Without checks the challenge is a partial substitute.** Taken alone, the challenger of the producer's model refuted 16 of 57 faulty functions and the challenger of the other model 21; 30 were refuted by one or the other. Neither refuted a sound function, in 317 attempts, since the harness decides.

**The reviewer without a harness caught little.** It rejected 9 of 57 faulty functions and 4 of 405 sound ones. It reads the same genome as the producer and shares what the genome leaves open.

**The better model was the better challenger.** Each model met every faulty function once as challenger. The careful model refuted 28 of 57 and the cheap model 9; the careful model abstained in 12 of 63 runs and made 238 attempts, the cheap model abstained in 30 and made 79. The challenger of the other model refuted more of the cheap model's faults (19 against 7 of 37) and fewer of the careful model's (2 against 9 of 20): what differed was the challenger's capability. With two models of one family, origin and capability cannot be told apart.

**Challenging paid where faults were common.** An attempt cost 1 and a refuted claim paid its stake. On the Sun and the sphere, where 10 of 336 functions were faulty, the challengers ended 152 units down. On the climates, where 47 of 126 were faulty, they ended 111 up. On sound work the profitable conduct is to abstain, and a challenger that abstains finds nothing.

**Stakes were lower on what proved faulty.** The mean stake was 6.61 on faulty functions and 11.20 on sound ones, and 43 of the 57 faulty functions had a stake at or below the median of their development. Stakes rose with the genome: 9.2, 10.6 and 12.1 on average from the identity, the repository's texts and the sequenced genome.

**The careful model staked beyond its means.** Shown its balance of 100, it staked more than it held in 6 of its 18 developments, and the ground refused 9 claims. The cheap model never did. Showing a lineage its balance, which the third prototype found necessary, falls short of what is needed: the refusal of the ground is what holds.

### 2.5 The Results against the Plan

| What would count against the design (plan, Sections 2.6 and 7) | Result |
|---|---|
| The repository's texts regenerate no better than the identity | Held on the two units of published formulas: 7 against 7 sound developments of the sphere, 4 against 3 of the Sun (53 against 49 sound functions of 56). Did not hold on the climates: 35 against 7 sound functions of 42 |
| The sequenced genome regenerates no better than the repository's texts | Did not hold: 19 against 11 sound developments of 21 |
| Challengers refute nothing that the checks let pass | Did not hold, on one case: the one fault that passed was refuted by both |
| Stakes are no lower on faulty functions | Did not hold |
| The challenger of the other model refutes no more | Undecided: the careful model refuted more in either seat |
| The reviewer without a harness does as well as the challengers | Did not hold: 9 against 16 and 21 of 57 |
| The identity alone regenerates the third unit as well as the genomes | Did not hold |

: The results set against the plan.

The first row counts against a claim of the design. The first case said that the repository already holds genomes in fragments. On two units of three, those texts added little or nothing to the identity, and on two they left out a convention that a sequenced genome carried.

### 2.6 Departures from the Plan, and Failed Runs

*A tolerance of the plan was wrong.* The plan allowed the mean radius of the sphere unit 0.1 m and required the arrival of a movement within 10⁻⁷ degree, and the first implies less than the second demands. Five developments that used the radius of the unit's own comment were counted defective in one function for that reason alone. A corrected contract compares the arrival as a place, within 0.5 m, as distances are compared. Both analyses are kept: under the plan's contract 56 functions are defective and 6 faulty functions pass the checks; under the corrected one, 51 and 1. No challenger attacked the function, so no standing changed. A contract is a claim of the site, and this one was refuted by the developments.

*A third unit was added.* Its reason, its contract and a prediction were written into the plan before it was run.

*The plan was not sealed.* By the session's transcript the plan was first written at 16:08 UTC on 20 November 2026, eight minutes before the first run, and its addendum was appended at 16:32, a minute before the third unit was first run. It was neither committed nor given a digest outside this folder before the runs, so that its priority rests on that transcript and on the assistant's word. What can be checked is this: Sections 1 to 6 of the present file are the first text unchanged, its first 11,718 bytes having the SHA-256 digest `a3d99cefb037eb1d…` of the text first written, which `report.py` verifies; and the addendum was corrected once afterwards, in a count of lines that it marks. A study that claims a plan should seal it on the ground before it runs.

*A family of networks was added to Study C.* The plan names the table of small networks. The 20,000 larger networks drawn at random were added when the site was built. For two implications few of them meet the premises, 10 and 15.

*One command ran beyond its limit of time.* The mutants of the third unit took longer than the two before, since its checks classify sixty thousand climates in each run. It finished in the background, and the first two units gave the same counts when run again.

*A command failed before it ran,* through a fault of quoting in the shell, and changed nothing.

*Exemplars are facts.* The sequencer was asked for the calls of the exemplars and the harness computed their results, since a model's arithmetic is no fact. Of 33 calls proposed, 6 lay outside the documented domains and were dropped.

## 3. Study B: The Documents of the Design

### 3.1 Numbers Bound to Their Data

`s3_documents.py` states each quantitative sentence about the prototypes as the data give it and looks for it in the document. Fifty-nine sentences were bound at first, and nine were not carried by their documents.

Six were faults of the documents: a count of lines of descent given as fourteen where the record holds eight; a least number of words given as 330 for 329; a saving given as 29 percent, computed from rounded figures, where the figures give 28; and three cells of a table whose ranges were those of the means by generation while the heading named no mean. Three were faults of the bindings, which had counted where variation fell from a lineage's own description of its change. Resolving these from the genomes themselves showed two further faults of the same record. In three of eighteen reproductions the offspring was an exact copy although the lineage's note described a change. And a rate of variation said to differ "by two orders of magnitude throughout" differed by a factor of four hundred in the mean and of ten at the least.

After correction, and with the sentences that the documents now carry about these studies bound as well, 113 sentences are bound and 113 are carried. The bindings are kept, so that a later change of a document or of a result shows.

The counts of that first run were recorded by hand at the time (`results/b1-first-run.json`), and its rows were not kept. What can be recomputed is the state of the first record: `python s3_documents.py committed` holds the 70 bindings of the prototypes' sentences, as they now stand, to the documents of commit `c21a3ec`, where 42 are carried and 28 are not. Those 28 are the sentences corrected since, for the faults listed here and in Section 3.5, and the sentences added since; they can be read in `results/b1-against-the-first-commit.json`.

### 3.2 Citations Resolved

Each citation in the text of the three case documents that have a list of references was read as a first author and a year and looked for in the list, and each listed work in the part of the survey that the list names. The reader found 154 citations; 0 were cited without being listed, and 0 listed works were missing from their part. The first reader of citations was itself at fault twice, taking the second author of three for the first and one year of a line that lists two, and reported faults that the documents did not have.

### 3.3 Paragraphs with Bound Numbers

Seven lineages wrote a paragraph of results from one results file, with a binding for every number: the keys that lead to a value and how it is reduced. The paragraphs held 87 numbers by the harness's count and 81 bindings. Of the bindings, 68 were evaluated and agreed with the number written, and 0 disagreed. The paragraphs all report one table of six rows, so that the 68 agreeing bindings are 20 distinct facts, several of them bound by every lineage. A further 12 bindings, all of the size of the suite, were written with another word than the task asked, through an ambiguity of the task's own wording, and one named a reduction that yields no number. The word *one* stood unbound in four paragraphs, in two of them as a numeral.

### 3.4 What the Study Shows

A record written by hand, by the assistant, from results that lay beside it, was wrong in six of fifty-nine bound sentences. Numbers written with their bindings were wrong in none of 68 evaluated. The two figures do not compare two ways of writing. The paragraphs report 20 facts of one table, read from it directly, where the errors of the hand-written record were in derived quantities over four prototypes: a count, a least value, a percentage, ranges of means. 13 bindings could not be evaluated, and no lineage wrote the same paragraph without bindings. What the study shows is narrower: a statement bound to its data is decided by a procedure, and a record so bound is held to its data at every change. The study needed no challenger: where a procedure decides every statement of a kind, facts suffice, which is the lowest rung. It also shows that the bindings and the readers are themselves claims that fail, three and two times here, and that each failure was found by setting the procedure against the document and resolving the difference at its source.

### 3.5 Two Later Readings

A binding fixes a number. It says nothing of what the number is said to mean, and it covers only the sentence that was bound. Two further readings were made after the bindings stood at no mismatch. Their counts are recorded by hand in `results/b4-later-readings.json`.

*A reading by hand.* The assistant read against the results and the sources every sentence with a number that no binding covered, about 200 of them, and corrected ten. Four were errors of fact: a count of calls to models that the kept ledgers do not support, 322 where they record 279, with the characters and tokens derived from it; a statement of what had been sent to the models that left out the bodies given to the sequencer; a range of the length of bodies; and a sentence on calls cut off at their limit. Two were hypotheses missing from formal statements of the general framework, and four were wordings that said more or less than the data.

*An independent review.* A second session of the same model was given the documents, the programs and the results, with a brief to look for errors and none of the assistant's conclusions. It recomputed the counts of this record and found none wrong. It reported twelve errors and ten doubts; each was then checked against the data or the source, and all twelve errors were confirmed. Four concern what this record had said its counts mean: that the checks had caught 56 of 57 faulty functions, where 27 were rejected only with their development and a mirrored azimuth passed its check (Section 2.4); that a bound in words had been met by notation, where no bound had bound (Section 2.3); that the six errors of fifty-nine and the none of sixty-eight compare two ways of writing (Section 3.4); and that a genome mattered only for the unit that is a choice among variants (Section 1). Three lie in the formal section of the general framework: a proposition stated for observations that are functions, where the framework's own facts are sampled; a caveat that called a loss a tie; and a bound that assumed an independence which the framework's own survey denies. The others were statements about the repository that its guide does not bear out, a provision that these studies had overtaken and the design had not taken in, counts that disagreed between documents, and a count of claims that included repeats.

Each reading found what the one before could not. None was a procedure of the ground: two were made by the assistant that wrote the record, and the third by a reader of the same model. The corrections are entered in the documents, and this section is kept so that the record shows how it came to its present state.

## 4. Study C: A Fragment of the Theoretical Corpus

### 4.1 The Site

The fragment is Section 1 of the theory note of the economics domain: transfer networks, the holonomy and the loop deficit of a closed walk, prices as a potential, and retention. `theory_site.py` computes with exact rationals. It declares twelve properties of a network, each with a procedure, and three kinds of claim: *implies*, refuted by a network; *separates*, decided by the witness it carries; and *determined*, refuted by one network with two flows. An implication that stands is given two facts: no countermodel in a table of 146,301 small networks, and none among 20,000 larger ones drawn at random. The extent of both is part of its standing.

### 4.2 C1: Calibration

Before any model was called the procedures were held to cases worked by hand, and then to statements whose truth is known. The 10 true statements had no countermodel in either family, and each of the 8 false ones was refuted by a network of the table. Two procedures of different method, one finding prices along a spanning forest and one computing the holonomy of every simple closed walk, both written by the assistant, agreed on all 166,301 networks, which is the note's proposition on prices held as a fact within that extent. The retentions of 2,000 cycles with flows drawn at random multiplied to the holonomy in every case, which is its proposition on retention.

### 4.3 C2: Lineages

Five producers of the cheap model and two of the careful one received the definitions and the vocabulary, with none of the note's results, and a question that the assistant posed to exercise the site: which properties are enough for a network to have prices, and which fall short. They offered 58 claims. Eight were refused for lack of energy, seven of them the careful model's, and 33 implications and 17 separations were admitted. Several producers offered the same statements: the implications are 11 distinct statements and the separations 9.

| Implication | Producers | Networks meeting the premises, small and larger | Against the note |
|---|---|---|---|
| `flat` implies `priced` | 6 | 33,525 and 5,193 | follows by the rules |
| `priced` implies `flat` | 6 | 33,525 and 5,193 | follows by the rules |
| `priced` implies `digons_flat` | 2 | 33,525 and 5,193 | follows by the rules |
| `unit` implies `priced` | 5 | 2,111 and 10 | follows by the rules |
| `acyclic` implies `priced` | 6 | 4,168 and 987 | follows by the rules |
| `directed_flat` and `strongly_connected` implies `priced` | 2 | 625 and 495 | follows by the rules |
| `flat` implies `directed_flat` | 1 | 33,525 and 5,193 | follows by the rules |
| `directed_flat` and `symmetric` implies `priced` | 1 | 353 and 15 | does not follow by the rules |
| `priced` implies `triangles_flat` | 2 | 33,525 and 5,193 | follows by the rules |
| `priced` implies `directed_flat` | 1 | 33,525 and 5,193 | follows by the rules |
| `complete` and `digons_flat` and `triangles_flat` implies `flat` | 1 | 1,318 and 128 | follows by the rules |

: The 11 distinct implications, all supported within the bound and none refuted. "Follows by the rules" means that the implication is derived from the note's proposition on prices and elementary facts about graphs by the rules listed in `s4_theory.py`.

| Separation | Producers | A witness |
|---|---|---|
| `directed_flat` does not give `priced` | 5 | `{"positions": 3, "legs": [[0, 1, "2"], [0, 2, "1"], [1, 2, "1"]]}` |
| `simple` and `triangles_flat` does not give `priced` | 1 | `{"positions": 4, "legs": [[0, 1, "2"], [1, 2, "1"], [2, 3, "1"], [3, 0, "1"]]}` |
| `symmetric` does not give `priced` | 2 | `{"positions": 2, "legs": [[0, 1, "2"], [1, 0, "1"]]}` |
| `triangles_flat` does not give `priced` | 3 | `{"positions": 4, "legs": [[0, 1, "2"], [1, 2, "1"], [2, 3, "1"], [3, 0, "1"]]}` |
| `connected` and `digons_flat` does not give `priced` | 1 | `{"positions": 3, "legs": [[0, 1, "2"], [1, 2, "2"], [2, 0, "2"]]}` |
| `strongly_connected` does not give `priced` | 2 | `{"positions": 2, "legs": [[0, 1, "2"], [1, 0, "3"]]}` |
| `directed_flat` and `triangles_flat` does not give `flat` | 1 | `{"positions": 4, "legs": [[0, 1, "2"], [2, 1, "1"], [2, 3, "1"], [0, 3, "1"]]}` |
| `complete` and `connected` and `simple` does not give `priced` | 1 | `{"positions": 3, "legs": [[0, 1, "2"], [1, 2, "1"], [0, 2, "1"]]}` |
| `digons_flat` and `triangles_flat` does not give `priced` | 1 | `{"positions": 4, "legs": [[0, 1, "2"], [1, 2, "1"], [2, 3, "1"], [0, 3, "1"]]}` |

: The 9 distinct separations, each proved by a witness that the harness verified.

No implication was refuted, by a challenger or by either family of networks. The challengers of both models made no attempt in fourteen runs (0 attempts): each read the claims, judged them true, and kept its energy.

Nine of the eleven distinct implications go beyond the two directions of the note's proposition, in 21 of the 33 claims. Most stay near it: two give prices from unit transports or from the absence of cycles, in 11 claims, and four are immediate weakenings of one direction, in 6. Three, in 4 claims, are of another kind, as are the separations. They concern the walks that suffice. The note states that prices exist exactly when every loop deficit vanishes, on walks that may take a leg against its direction. Five producers showed by a witness that the vanishing of the deficits of the walks that follow the legs falls short of prices, and others that it suffices when the network is strongly connected or symmetric. The second of these, `directed_flat` and `symmetric` implies `priced`, does not follow by the rules. An argument for it exists: in a symmetric network every leg has an opposite, the walk along a leg and back along its opposite has holonomy 1, so opposite legs are reciprocal, and every closed walk equals in holonomy a walk that follows the legs. The argument is the assistant's; the independent review of Section 3.5 found it valid, and no procedure of the site has checked it. The claim's standing is that it is supported within the bound.

### 4.4 C3: The Draft's Claim

The note records a finding of the audit of the papers behind it: by the proposition on retention, the split of a deficit among the positions of a cycle is a property of the flows, so that the "position that absorbs the deficit" is no invariant of the network. A scripted seat warranted the contrary claim with a stake, and two challengers of each model were asked to attack it. All four refuted it at the first attempt, each with a cycle of two legs and two systems of flows, and the harness verified each with exact rationals. The task gave the formula of a retention, in which the flows appear, and the form that a counterexample takes, so the refutation lay near at hand. What the run shows is that the site decides such a claim by a procedure, and that the finding of the audit is reached there.

### 4.5 The Dossier, and the Seats Left Empty

[The dossier](dossier-transfer-networks.md) sets out the statements with their standing for the author. No part of this study asked a question of the author's, wrote the author's answer, read a result or accepted one. The study shows that a fragment of the corpus can be declared as a site whose procedures decide claims, that lineages given only its definitions warrant statements which its procedures support, a few of them beyond the note, and that a claim which the audit refuted is refuted there. It does not show that inquiry gains, and it could not: no false implication was offered, so refutation among lineages was exercised only on the scripted claim.

## 5. The Four Cases after the Studies

*The science infrastructure and programming projects (cases 1 and 2).* The first rung ran end to end on three units of this repository. The founding steps of sequencing and of measuring the harness gave results that bear on the repository itself: which exported names have no check that kills a mutant of theirs, which check cannot see a mirrored azimuth, and which convention no text states. The claim that the repository's texts are already genomes is weakened to this: they are the larger part of one.

*Any project (case 3).* The documents of this design were a site of the lowest rung, and facts by recomputation found errors that reading had not.

*Inquiry and papers (case 4).* One fragment of the theory became a site. The acts reserved to the author were left alone, and the study says nothing of what an inquiry gains from such a site.

## 6. Consequences for the Design

1. *A third place of knowledge.* What a unit is may lie in its genome, in its body alone, or in the developer's stock. The claim *regenerates* is to be read against the same developments from the identity alone, and a genome has standing for what it adds. The reading of regenerative capacity becomes a difference of two shares.
2. *Sequencing looks for what is particular.* The yield of a genome lay in conventions: names of returned values, senses of angles, inclusive boundaries, the order of a list. A sequencer should be asked for these first, and the public part of a unit can be named and left to the developer. Brevity has its own cost: the short genome of the third unit dropped part of a rule in two developments of 7, and the claim *regenerates* is what shows such a loss.
3. *Checks first.* On units whose checks kill from 43 to 87 percent of the distinct mutants, and call about half of the faulty functions of the developments that they reject, the first needs of a garden are checks, as the general framework's founding says. The mutants that survive and the faults of development that pass are the list of what to check.
4. *The price of an attempt.* With faults in one function of thirty, challenging lost energy, and a challenger that learns from its losses will abstain. Either the stake, the price of an attempt or a subsidy of challenge must move with the rate of faults, or the first rung goes quiet exactly where work is mostly sound. That may be the right outcome, as the hypothesis on cost says, and it is to be stated as a property of the rung.
5. *Origin and capability.* The design's provision that challengers differ from producers in origin could not be tested with two models of one family. What the study saw is that a stronger challenger finds more.
6. *Accounts are not facts.* A lineage's description of its own variation was false in three of eighteen cases, a model's arithmetic was not trusted for exemplars, and the assistant's own record held errors that bindings, a reading by hand and an independent review each found in part. Whatever can be recomputed is recomputed by the ground, and what a count is said to mean needs a reader of another origin.
7. *Bounds in characters.* Words are an unsteady measure of a text that holds formulas. The bounds of this study did not bind, so the choice of characters or tokens is a matter of design and no finding.
8. *Contracts and plans are claims.* A tolerance of the plan was wrong, a binding was wrong three times and a reader of citations twice. A site's declaration is revised by a recorded act when its own results refute it, with both analyses kept. The plan itself was not sealed before the runs; a plan is a claim, and the ground should seal it.
9. *The grain of a fact is the grain of its claim.* The design stakes and accepts by function, and the checks of a repository fail by development. Here 27 faulty functions had no failing check that calls them. A fact attached to a claim must be a fact about that claim's object, and a harness should say which checks call which parts.
10. *A shown balance is no budget.* Showing a lineage its balance is necessary, as the third prototype found, and it does not suffice: the careful model staked beyond its means when shown its balance, here and on the theory site. The refusal of the ground is the provision.

## 7. Cost, Files and Reproduction

The studies made 291 calls to models: 3 for sequencing, 63 for developments, 189 for reviewers and challengers, 28 for the theory site, 7 for the bound paragraphs, 1 for a first call to see that a model answered. One challenger of the theory site had its answer cut off at its limit of tokens in three tries and was asked again with a larger limit; the four calls are among the 28. The counts are those of the ledgers that the stages printed, except for the developments of the third unit, whose ledger was not kept and which are counted at one call each. What left the machine was the bodies, comments, signatures and theory notes of the three units (the bodies to the sequencer alone), the definitions of the fragment, and a table of results of the second prototype, all of it public in the repository or made for this purpose, with the names of persons and of the project removed.

| File | Content |
|---|---|
| `PLAN.md` | The plan, written before the runs and not sealed before them, and its addendum |
| `units.py` | The three units: their contracts, domains and hidden inputs |
| `harness_js.py` | The harness: the screen, the working folder, the repository's checks, the differential comparison, the mutants |
| `s1_kills.py` | Study A1 |
| `s2_units.py`, `s2_analyse.py` | Studies A2 and A3 and their measures |
| `s3_documents.py` | Study B |
| `theory_site.py`, `s4_theory.py` | Study C: the site, and its stages |
| `report.py` | This record and the dossier, written from the results |
| `dossier-transfer-networks.md` | The statements of Study C with their standing, for the author |
| `results/` | Every result as data; `b1-first-run.json` and `b4-later-readings.json` are counts recorded by hand |

: The files of the case studies.

The programs that call no model run again from this folder as they are: `python s1_kills.py`, `python s2_units.py evaluate`, `python s2_units.py ground`, `python s2_analyse.py`, `python s3_documents.py recompute`, `python s3_documents.py committed`, `python s3_documents.py cites`, `python s4_theory.py calibrate`, `python s4_theory.py settle` and `python report.py`. The stages that call a model keep what they received, and run again only for what is missing.
