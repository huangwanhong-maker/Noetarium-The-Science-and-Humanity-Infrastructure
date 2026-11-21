# Case Studies of the Generative Ecology of Projects: The Plan

*Written on 2026-11-21 before any run. It fixes, for each study, the question, the material, the procedure, the measures and what would count against the design, so that the analysis does not follow the results. The record of what happened is [README.md](README.md); where a run departed from this plan, the record says so.*

## 1. Purpose

The [prototypes](../prototypes/README.md) used invented material: a toy unit, a toy genome, laws of magmas. Before a paper is written from the design, the user asked that it be shown to work on case studies. Three studies follow, on real material of this repository, one for each kind of site: two units of code (cases 1 and 2), the design's own documents (case 3), and a fragment of the theoretical corpus (case 4). Each is run at the lowest rung that its question needs.

The studies remain small. They show whether a mechanism operates on real material and where it first breaks. They measure no rate that could be carried to a project at large.

## 2. Study A: Two Units of the Repository

### 2.1 Material

Two units of provisional JavaScript, chosen because their checks run in seconds without a build and because each has an accepted implementation and checks that the repository already holds.

- `domains/astronomy/js/sun.js`: the Sun seen from the Earth, 83 lines, eleven exported names. It has a theory note, and ten checks of the repository's suite concern it, three of them against a fixture fetched from JPL Horizons.
- `domains/geodesy/js/sphere.js`: the Earth as a sphere, 70 lines, ten exported names. It has a header comment and no note, and six checks of the repository's suite concern it.

The checks are taken from the repository's test files by script, verbatim. The accepted implementation of each unit is the *reference*, held by the harness and shown to no model except the sequencer.

### 2.2 A1: The Strength of the Checks (the Claim *kills*)

Mutants of each unit are made by declared operators at every site where one applies: an arithmetic operator replaced by its opposite; a comparison made strict or loose; a numeric constant multiplied by 1.001; a numeric constant increased by one; sine and cosine, arcsine and arccosine, maximum and minimum exchanged. A mutant that does not load is set aside. A mutant is *distinct* when differential tests against the unit itself tell it from the unit, and *killed* when at least one of the repository's checks for the unit fails on it.

*Measure.* The share of distinct mutants that the repository's checks kill, by operator and by function.

### 2.3 A2: Regeneration (the Claim *regenerates*)

Three genomes are given to developers who never see the body or the checks.

1. *Identity.* The purpose of the unit in one sentence, its exported names with their parameters, and the names it may import.
2. *The repository's texts as they stand.* The unit's header comment and the comments above its functions, with the bodies removed, and the theory note where one exists.
3. *A sequenced genome.* A model reads the body and writes a genome in four bounded parts: identity (at most 120 words), laws (220), exemplars (at most 12 calls) and development (80). A genome beyond a bound is refused and asked for again, three times at most. The sequencer chooses the calls of the exemplars, and the harness computes their results from the reference, since a model's arithmetic is no fact.

Each genome is developed five times by the cheap model and twice by the careful model, forty-two developments in all for the two units.

*Facts and truth.* For each development: the number of the repository's checks that pass; and, for each exported name, whether it agrees with the reference on hidden inputs under the unit's documented contract. The hidden inputs are of two tiers, *main* (random inputs inside the documented domain) and *edge* (boundaries and special values). A function is *defective* when it disagrees on a main input, and *edge-defective* when it disagrees only at the edge. Tolerances are fixed in `units.py` before any development is made: tight where the documents give the formula, and at the documented accuracy where they give only an accuracy.

### 2.4 A3: The Warranted Change on the Ground

Every development is treated as an artefact answering a need, on the ground of the prototypes (`ground.py`), with energy conserved and the journal sealed.

1. A person posts the need with a bounty. The producer is the developer of A2; with its module it gives a stake from 0 to 20 on each exported function, as the claim that the function agrees with the reference on its documented domain. The producer is shown its balance and the prices.
2. *A reviewer of the producer's model* reads the genome and the artefact and accepts or rejects each function, with no harness.
3. *A challenger of the producer's model* and *a challenger of the other model* each read the genome, the artefact and the stakes, and put inputs to the functions, each attempt at a price. The harness decides each attempt against the reference. The two challengers act on separate copies of the same state.

### 2.5 Measures

- M1. Checks passed and functions agreeing, by genome and by developer.
- M2. The functions that are defective in developments on which every one of the repository's checks passes.
- M3. Of the defective functions with a positive stake, the share refuted by each challenger; the attempts made; the trials in which a challenger abstained.
- M4. Of the defective functions, the share that the reviewer rejects; of the sound ones, the share that it rejects.
- M5. Stakes on defective and on sound functions.
- M6. What would reach a person, and how much of it is defective, under three filters: the repository's checks alone; those and the reviewer's acceptance; those and survival of the challenge.

### 2.6 What Would Count against the Design

1. The repository's texts regenerate a unit no better than its identity alone. The claim of case 1 that the repository already holds genomes would then be empty.
2. The sequenced genome regenerates no better than the repository's texts. Sequencing would then add nothing here.
3. Challengers refute no defective function that the repository's checks let pass. The first rung would then add nothing to the pipeline on these units.
4. Stakes are no lower on defective functions than on sound ones.
5. The challenger of the other model refutes no more than the challenger of the producer's model. Difference of origin, at least within one family of models, would then be no help.
6. The reviewer without a harness does as well as the challengers.

A result under 3 with few defective functions overall is no evidence either way, and will be reported as such.

## 3. Study B: The Documents of the Design

### 3.1 Material and Claims

The material is the documents of this folder and the prototypes' results. Two kinds of claim are decided by procedure: *recomputes* (a number in prose equals what a stated computation over the data yields) and *cites* (a work cited in the text is an entry of the survey or of the paper's bibliography).

### 3.2 Procedure

- B1. Every quantitative statement about the prototypes, in every document of the design, is bound to a computation over `prototypes/results/` and recomputed.
- B2. Every citation in the text of the four case documents is resolved to an entry by first author and year.
- B3. Seven lineages (five of the cheap model, two of the careful) write a paragraph of results from one results file, each number with a binding in a small declared language. The harness evaluates every binding and finds the numbers left unbound.

### 3.3 Measures and What Would Count against the Design

The measures are the statements checked, the mismatches found, and for B3 the share of numbers bound, bound correctly, and left unbound. If the documents written by the assistant without such a harness held no error, the claim of case 4 that a paper needs this harness would have no support from this study. If lineages bound their numbers wrongly as often as they stated them wrongly, the binding would add nothing.

This study needs no challenger. Where a procedure decides every statement of a kind, facts suffice, which is the lowest rung.

## 4. Study C: A Fragment of the Theoretical Corpus

### 4.1 Material

Section 1 of the theory note of the economics domain: transfer networks, the holonomy and the loop deficit of a closed walk, prices as a potential, and retention. Its Section 2 records a finding of the audit of the papers behind it: by the proposition on retention, the split of a deficit among the positions of a cycle is a property of the flows and no invariant of the network.

### 4.2 The Site

The harness computes with exact rationals. A network has at most six positions and legs with positive rational transports. A vocabulary of properties of networks is declared, each with a procedure. Three kinds of claim follow.

- *implies*: every network with the premises has the conclusion. It is refuted by a network, verified exactly. It is *supported within a bound* by an exhaustive table of small networks, whose extent is part of its standing.
- *separates*: the premises do not give the conclusion. The claim carries its witness network and is decided at once.
- *determined*: a quantity is fixed by the network. It is refuted by one network with two admissible flows that give the quantity two values.

### 4.3 Procedure

- C1. *Calibration.* The harness is run on statements whose truth the note or elementary graph theory fixes, true ones and false ones, before any model is called.
- C2. *Lineages.* Five producers of the cheap model and two of the careful model receive the definitions and the vocabulary, with no statement of the note's results, and warrant claims with stakes on the ground. A challenger of each model attacks each producer's claims. Each claim left standing is then given the fact of the table.
- C3. *The draft's claim.* A scripted seat holds the claim that the audit found imprecise, that the position which absorbs a deficit is determined by the network, with a stake. Challengers of both models are asked to attack it.
- C4. *The dossier.* The statements with their standing are written for the author.

### 4.4 The Seats Left Empty

Asking, reading, accepting and signing are reserved to the inquirer, and no part of this study performs them. The question put to the lineages is posed by the assistant to exercise the site and is marked as such. No answer is written first, since that answer is the author's to write. The study can show that a fragment of the corpus can be declared as a site whose harness decides claims and that lineages and challengers act on it. It cannot show that inquiry gains.

### 4.5 What Would Count against the Design

1. The fragment cannot be declared with claim kinds that a procedure decides.
2. Producers warrant false implications that neither a challenger nor the table refutes.
3. No challenger refutes the draft's claim, which the audit refuted.
4. Every claim that stands is one of the note's own propositions restated.

## 5. Terms

What leaves the machine: the comments and the note of the two units, and the definitions of Section 1 of the economics note, all of them public in the repository, with the names of persons and of the project removed. They go to one provider of models through the key that the user supplied for this work. No more than 400 calls are made.

## 6. Reporting

Every run is reported, the failed ones included. Seeds are fixed. Results are kept as data under `results/` before any later stage reads them, and each program runs again from this folder.

## 7. Addendum: A Third Unit, Written before It Was Run

*Added on 2026-11-21 after the first two units had been evaluated and before anything was run on the third.*

The two units of Section 2 turned out to be weak tests of regeneration. From the identity alone, with no formula and no convention, seven of seven developments of the sphere unit and three of seven of the Sun unit agreed with the accepted implementation on every function. Both units implement published formulas that the models know. A unit whose content is public knowledge cannot show whether a genome carries what a body holds.

A third unit is therefore added, chosen for the opposite property: `domains/climate/js/climate.js`, the Köppen–Geiger classification of a climate from its normals, 109 lines (this plan first said 113, which counted the four lines of the index file), seven exported names, a theory note that states its rules, and seven checks of the repository's suite. The classification exists in published variants that differ in their thresholds and in their rules of precedence, and the unit follows one of them and fixes several conventions of its own.

Its contract, its domains and its hidden inputs are fixed in `units.py` before any development of it is made. The procedure is that of Sections 2.2 to 2.4, unchanged.

*Prediction.* The identity alone will not regenerate this unit: fewer than half of its developments will have every function sound. The repository's texts, which here include a note that states the rules, will regenerate it better than the identity, and the sequenced genome at least as well as the repository's texts.

*What would count against the design.* The identity alone regenerates this unit as well as the genomes do. That result would mean that on all three units the knowledge needed lay in the models' own stock, and that the study had found no unit on which a genome mattered.
