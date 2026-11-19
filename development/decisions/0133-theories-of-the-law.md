# 0133. Theories of the Law, Searched For, Judged by Fit and Justification and Tested

- **Status:** Accepted, provisionally (2026-11-19). The module's name (`doctrine`), the default weights, the kind of a doctrine, the view's name and the example are provisional.
- **Date:** 2026-11-19
- **Decided by:**
  - **The request.** "Let us revise the jurisprudence and law foundation/domains. Integrated the envoluntionary reasoing/discovery/interpretation mechaisim into it first." Then, to the plan Claude set out: "go".
  - **The design.** Claude, here, on the law domain's `norms`, `precedent` and `authority`, apodeixis, heuresis and archeion.

## 1. Context

ADR 0127 brought the evolutionary mechanism of the needs search (ADRs 0125, 0126) into the law domain for the reconstruction of facts: theories of the case, accounts of what happened, searched for, kept by kind, read under the law and tested on evidence held back. It left two things listed as not built: the search for the ratio of precedents, and theories of the law as well as of the facts. The request named reasoning, discovery and interpretation; the first of these that ADR 0127 did not reach is the interpretation of the law itself.

The domain already reads open terms in several ways (apodeixis's points and admissible interpretations, ADR 0112), weighs the courts' holdings for a forum (`authority`, ADR 0121) and reads precedents by Horty's reason model (`precedent`, ADR 0115). What it did not do is ask which reading of the whole is best. A matter with several open terms and several precedents whose courts left their reasons unstated has many readings of its law; the domain could say which are admissible, not which fit the case law best, and could not say that a precedent read one way forces a side for the matter and read another way leaves it open. Dworkin's account of interpretation (fit, then justification) and Bench-Capon and Sartor's theories of a case law constructed and compared are the classical frames; neither had been made to run on a record.

## 2. Decision

### 2.1 The Law Domain's `doctrine`

A module of two headers (38 checks; the domain has 210). Its note is [theory/theories-of-the-law.md](../../domains/law/theory/theories-of-the-law.md).

- **A doctrine** is a reading of every open term of the matter's rules and a ratio for every precedent whose court left its reason unstated (a non-empty set of the factors it had for its outcome). A written form, a line each (`reading vehicle = narrow`, `ratio emergency/parent: child_injured`), is read line by line; a line that does not read is refused with its number, never repaired.
- **Fit**: the decided cases the readings get wrong, each weighted by its court's force for the forum (`authority`'s new `force_for`: binding when the forum is bound by the court's order and the court has authority over the instrument, persuasive otherwise, at a declared weight); the readings outside those the binding holdings and arguments leave admissible; the pairs of precedents its ratios make inconsistent.
- **Justification**: the declared purposes' worth of the readings, as a share of their best. The score is the weighted misfit less the weighted worth, every weight the matter's.
- **What a doctrine makes of the matter**: for each case base, the side its precedents force for the matter's factors, or that they leave it open; then each question follows, fails, or turns on the open fact. That is the doctrine's kind.
- **Exactness**: when the doctrines are few (up to 200,000), every one is visited and the best of each kind there is is known. The checks hold the search to it, and all 162 doctrines of the checks' problem to formulas worked by hand.
- **Standing**: over the doctrines within a declared tolerance of the best, a question is clear, arguable or untenable; and each fact a case base decides is held, denied, left open, or turns on the ratios.
- **The search**: heuresis's evolution, three makers, four mutations, two crossovers, an archive that keeps the best two of each kind (MAP-Elites's idea), and proposals from outside credited to their source. A model has the four roles of the other searches (seed, mutate, cross, reflect), is shown the law as the search sees it, and never a decision held back; its doctrines are grade C.
- **The test**: a decision given after the matter, or listed by it, is held back. The search never reads it (changing what it held leaves the archive unchanged, generation by generation, in the checks). Each doctrine kept is tested on it once, the test registered in archeion's journal before it is read and its outcome written after: a decided case predicted or contradicted, a precedent predicted, contradicted or left open.

### 2.2 The Layers Above

- **The API.** `science/doctrine.hpp` composes the domain with the matter a work keeps (`doctrines`, the search run from its state with proposals and requests; `assess_doctrine`, one doctrine written by a person), its answer falling through to theories of the case and the law. `science.law` gains `doctrines`, `assess_doctrine` and `fulfil_doctrines` (a model's doctrines in its four roles, each read by the engine, entered as grade C, or refused with its line).
- **The visualization layer's `evidence/doctrine.js`** (15 checks): the field of doctrines (misfit to the right, worth upward, the acceptable half-plane shaded, each kind's exact best ringed), the case law along time (each case a stone by its force, agreeing or not, the decisions held back veiled beyond the matter's date with their verdicts), and a board of precedents (factors by side, the reasons read ringed, the matter's side forced).
- **The Law Studio** gains the view *Doctrine*, routes `doctrines`, `doctrines/run`, `doctrines/model` (only from the work's own page), `doctrines/assess`, `doctrines/propose` and `doctrines/reset`, a project's `doctrines/` folder, and in the park plug-in a scheme with an invented case law and the invented matter *Council of Avalon v Dashwood*. The searches for theories of the case and of the law now share one set of routes (`SEARCHES` in `matters.py`).

## 3. What the Example Shows

Mr Dashwood rode his bicycle across the park to reach his son, who had fallen from a climbing frame (invented). The case law (invented): Okafor (a motor-assisted machine is a vehicle, the High Court of Avalon), Lindqvist (a pedal bicycle is not, the same court), Tandem (a tandem is, a court of Erewhon); and on what counts as an emergency, four precedents by factors with their reasons unstated. Moreau (the Supreme Court, 2027: a bicycle is not a vehicle) and the jogger (2027) were given after the matter and are held back. The purposes declared: the safety of people on foot and the liberty to use the park.

- **There are 162 doctrines**, of five kinds; the search finds the best of every kind in twelve generations.
- **The best fit reads "vehicle" narrowly.** It agrees with the binding Lindqvist and disagrees only with the persuasive Tandem (misfit 0.3), and serves the declared purposes better (worth 0.70 against 0.50). The broad reading disagrees with Lindqvist (misfit 1). Within the tolerance of 0.5, every acceptable doctrine reads narrowly: the offence is untenable over them.
- **Whether Mr Dashwood rode in an emergency turns on the reasons the precedents are read as decided on.** Among the acceptable doctrines, which fit equally well, the precedents hold it under some (the parent's case read as decided on the child's injury), deny it under others (the cyclist's case read as decided on help being at hand), and leave it open under others. Under the narrow reading it does not change the outcome; under the broad reading it would decide it.
- **The test.** Moreau upholds every narrow doctrine and contradicts every broad one. The jogger tells apart narrow doctrines that fit equally well: those that read the commuter's case as decided on another way of getting help being at hand predicted it, and those that read it as decided on no one being hurt left it open. Each test was registered in the journal before the decisions were read.

None of this is any court's reading. It is what the declared cases, factors, purposes, weights and tolerance give.

## 4. Alternatives Considered

- **A probability over readings**, as tekmerion's interpreters give (a credal set, precedents under a Dirichlet prior). They answer how likely a reading is; Dworkin's question is which reading of the whole fits and justifies best, and a court's holdings are not samples. The interpreters remain for a work that wants them.
- **Searching readings and ratios separately.** A reading changes which decided cases fit; a ratio changes which side the precedents force. The matter's outcome depends on both together, which is why the kind is of the whole doctrine.
- **The doctrine's kind including its readings.** The archive would keep the best broad doctrine of each outcome beside the best narrow one. It multiplies the niches by the readings' product. The field already shows every kind's exact best, and a person can write a doctrine of their own.
- **The search in a foundation.** The operators name terms, precedents and factors, objects of the law; heuresis holds what is general.
- **Holding decisions back by a keyed hash.** Which decisions came after the matter is a fact of the record, not a draw; a matter may also list others.

## 5. Consequences

- The law domain 210 checks (40 new), passing alone with its down-set; the API's law checks 34 (15 new); the visualization layer 856 (15 new); the Law Studio 86 (21 new, among them a model's part with a stand-in); the page driven through the view by `tests/drive-doctrine.mjs` (13 steps, once with Claude proposing six doctrines, all read; not in the suite), eleven pictures in the gallery's §41.2.
- `authority` gains `force_for`, used by `weigh` and by `doctrine` alike.
- **Not built:**
  - precedent by dimensions (Horty's later model), and the hierarchy of courts within a case base (a precedent's weight by its court);
  - decided cases whose facts are read from documents, each with its passage;
  - justification beyond declared worths (values ordered, as Bench-Capon and Sartor's theories carry them);
  - a doctrine's kind by its readings as well, as a choice;
  - doctrines and theories of the case searched together (what happened and how the law reads it, each depending on the other).
- **Provisional:** the default weights (10, 1, 2, 1; persuasive 0.3), the tolerance of the example, the module's and the view's names.

## 6. References

- Dworkin, R. (1986), *Law's Empire*, ch. 7. Raz, J. (1979), *The Authority of Law*, ch. 10.
- Horty, J. F. (2011), Rules and reasons in the theory of precedent, *Legal Theory* 17. Horty, J. F. and Bench-Capon, T. J. M. (2012), A factor-based definition of precedential constraint, *Artificial Intelligence and Law* 20.
- Bench-Capon, T. and Sartor, G. (2003), A model of legal reasoning with cases incorporating theories and values, *Artificial Intelligence* 150.
- Mouret, J.-B. and Clune, J. (2015), Illuminating search spaces by mapping elites, arXiv:1504.04909.
