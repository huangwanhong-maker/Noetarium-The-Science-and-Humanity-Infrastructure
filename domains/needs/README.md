# Needs: Their Exploration and Validation

*Status: draft (2026-11-18).* A field at height 0 on the foundations, C++ from the start ([ADR 0125](../../development/decisions/0125-the-evolutionary-exploration-and-validation-of-needs.md), [ADR 0126](../../development/decisions/0126-needs-read-under-their-terms-and-a-model-in-every-part.md)). It serves **research** and **use**: which groups of people have which needs, stated so that each claim can be read, tested and contested, and searched for without the search deceiving itself.

## 1. Concepts

**A need is a gap judged by a standard.** It is the distance between the state of some people and a standard, in a domain of satisfaction (care, income, housing, mobility). Bradshaw (1972) named the four standards: normative (a level an authority declares), felt (what the people say), expressed (demand beyond what is provided) and comparative (what others like them have). A need is therefore never observed: it is a judgement of observed states under a declared standard. The domain never decides a standard; it makes each one explicit and computes what follows from it.

**A statement of need** is written in a small language, read by morphe's BNF reader, so that a person, a program and a language model write needs in one form:

    need care for age >= 75 and tenure = renting within district = Hume by comparative need_assist higher
    need income for age >= 65 by normative low_income above 30%
    need care for district = Hume by expressed waiting over places

**Needs are searched for, by evolution.** The space of statements is far too large to list: a group is any conjunction of conditions, each with its indicators and standards. A population of statements is kept and improved by heuresis's evolution, every stage's strategy chosen by the caller:
- generators without a language model: the language walked breadth first and depth first, subgroup discovery by beam search, the leaves of a regression tree, the declared rules, and where a model of seeking leaves demand unmet;
- mutations and crossovers;
- a person's statements or a language model's, which enter as proposals and are read like any other.

**A search that does not deceive itself** (the theory note's §5). A search that tries thousands of statements finds large gaps by chance, and evolution finds them faster. So the data are cut into three parts of whole clusters (areas), by a keyed hash, never by their outcomes:
- **discovery**, half, which the search sees;
- **validation**, a quarter, consulted only through a reusable holdout (Thresholdout, with a budget) when a statement is admitted to the archive;
- **confirmation**, a quarter, used once for each statement admitted, in the order of admission, by an online procedure that controls false discoveries (e-LOND under any dependence by default; LOND, LORD++, SAFFRON, α-investing). Each test is registered in a journal sealed by archeion's hash chain before it is run.

Nothing of the confirmation part reaches the search: not the judge, not the credit of the operators, not retirement. A check holds this exactly: with the confirmation part's outcomes permuted, the search's population and archive are the same in every generation.

**What the archive keeps.** A statement is admitted when its gap clears z* on discovery, its effect (the gap over the indicator's spread across areas) reaches a least size, it is stable under resampling of areas, and every condition earns its place (each carries a gap of its own within the rest of the group, after Webb's productive rules). It must also reach people in enough areas to be testable by design, and be no weaker than a kept statement of nearly the same people. A kept statement takes part in later generations as a parent. It is retired, with the reason, when a stronger statement of nearly the same people supersedes it, when it has produced nothing that survives for a declared number of generations, when it is too old or the archive full, or when new data no longer uphold it.

**How a statement is read** (the theory note's §5a). A statement's terms are open: which income counts as low, whom a group is compared with, at what level a standard is set, which causal structure holds. The data declare points of interpretation, each with the readings it admits and where each reading comes from (authority, convention, analyst, data, or a model's proposal). Every statement is read under every combination of them on tekmerion's construal: clear, arguable or untenable, with the terms it turns on. Declared rivals (strata within which a gap might vanish) are assessed. A statement untenable under its readings, or explained away by a rival, is not kept. A model's readings and rivals are shown as proposed and never change the record's verdict.

**Its standing** is read from an argumentation framework of its tests (apodeixis's grounded semantics): robust (held, and confirmed), supported (held, not confirmed), unsupported, or refuted. Beside it stand what does not attack the claim but qualifies it: causal support (provision's effect within the group, adjusted by a declared graph's back-door set), a simulation of seeking satisfiers within reach of capacity, and declared rules by forward chaining.

**What needs is not.** The tests are the foundations': peira's surveys (the gap's design-based variance), graphs and adjustment; tyche's tests, online procedures, e-values, the holdout and regression; nomos's accessibility and agents; morphe's grammars and forward chaining; apodeixis's argumentation; archeion's journal; heuresis's evolution. The domain names what only needs name: statements of need, standards, domains of satisfaction, and the protocol of their search.

## 2. Modules

| Module | Holds | Status |
|---|---|---|
| [`groups`](groups/include/needs/groups/) | Tables of units with a weight; conditions and groups (conjunctions), their rows, canonical keys, subsumption, join and meet; Jaccard distances; values quoted so that a group reads back | draft: 37 checks |
| [`language`](language/include/needs/language/) | The statement and the dataset it is judged on; the grammar in BNF; the reader (whole texts, and line by line for proposals) with errors at line and column; binding and checking against the data | draft: 29 checks |
| [`evidence`](evidence/include/needs/evidence/) | The three parts; the gap by design (peira) or binomial reading (tyche), per cluster for fast resampling; stability; strata; causal support (under any graph); the seeking model and the floating catchment; rules; standing by argumentation; a statement read under every reading of its open terms, causal support under each graph, rivals and their strata (tekmerion's construal, ADR 0126) | draft: 81 checks |
| [`search`](search/include/needs/search/) | The space of statements; seven generators; ten mutations and five crossovers; the distance (one need under two declared measures is one); the search on heuresis's evolution with the admission (readings and rivals among its gates), the reusable holdout, confirmation in order, the journal, resumption; what to ask of a language model in four roles, its variations proposed with their parents | draft: 41 checks |

Planned (from the survey `research/literature/reports/Evolutionary exploration and validation of needs.md`): coevolution of statements with adverse strata (heuresis's planned `coevolution`); heterogeneous effects of provision (peira); counting standards over several indicators (Alkire–Foster, nomos); expressed need corrected for the propensity to report; a GRADE-like scheme for needs (tekmerion's warrant); e-processes for evidential revocation on new waves of data.

## 3. Checks

`node domains/needs/tests/run-tests.mjs` builds the C++ and runs its four check programs (189 checks), held to what follows from the definitions:
- gaps and their ultimate-cluster variance worked by hand from clusters' totals; the binomial reading against tyche's score test and Newcombe's interval;
- a planted gap recovered within its standard errors, and stable in every resample, while a group with no gap of its own seldom is;
- the effect of provision recovered with the confounder the graph names held fixed, and not identified without a back-door set;
- the floating catchment's access and the seeking model's unmet shares on three places and one supplier, as worked by hand;
- entitlement derived through an implication with its derivation;
- standing case by case;
- on synthetic populations with needs planted: the beam's best group and the tree's worst leaf are the planted ones; the search finds, admits and confirms them and stands them robust; on data with nothing planted it confirms nothing;
- the archive keeps distinct needs; every survival, parent, schedule, objective and online rule runs the search;
- resumption equals a search that never stopped; the journal verifies and an altered entry is found;
- nothing of the confirmation part reaches the search;
- every interpretation's verdict held to the gap test computed again directly; a statement's own reading added; a convention setting aside the analyst's readings; a model's reading shown only as proposed; causal support under each graph; a rival explaining a planted confound away, its pooled effect held to the strata's gaps by inverse variance; declarations refused;
- what is asked of a model shared among its roles by credit, every parent a living member described by discovery alone; a model's variation kept with its parent; with clearness asked, every statement kept is clear.

## 4. Dependencies

Foundations only: heuresis (`evolution`), tyche (`testing`, `regression`, `probability`), peira (`surveys`, `graphs`), nomos (`measurement`, `agents`), morphe (`formal`, `combinatorics`), apodeixis (`argumentation`), archeion (`fixity`), tekmerion (`construal`), calx (`random`, `verification`), physis as the order requires. The API's `science/needs.hpp` composes it for works, and `science.needs` asks it from scripts and asks a language model for proposals. Its first work is the Urban Studio's plug-in *Needs*.

## 5. Theory

[theory/needs-and-their-discovery.md](theory/needs-and-their-discovery.md) states what a need is, how a statement is judged, and the protocol of the search, with what each safeguard guarantees and what it does not.

## 6. References

- Bradshaw, J. (1972), The concept of social need, *New Society* 30, 640–643.
- Doyal, L. and Gough, I. (1991), *A Theory of Human Need*, Macmillan.
- Klösgen, W. (1996), Explora: a multipattern and multistrategy discovery assistant, in *Advances in Knowledge Discovery and Data Mining*, AAAI/MIT Press.
- Webb, G. I. (2007), Discovering significant patterns, *Machine Learning* 68, 1–33.
- Dwork, C. et al. (2015), The reusable holdout, *Science* 349, 636–638.
- Javanmard, A. and Montanari, A. (2018), Online rules for control of false discovery rate, *Annals of Statistics* 46, 526–554.
- Xu, Z. and Ramdas, A. (2024), Online multiple testing with e-values, *AISTATS*, PMLR 238.
- Wolter, K. M. (2007), *Introduction to Variance Estimation*, 2nd ed., Springer.
- Simpson, E. H. (1951), The interpretation of interaction in contingency tables, *JRSS B* 13, 238–241.
- Yang, C. et al. (2024), Large language models as optimizers, *ICLR* (OPRO).
