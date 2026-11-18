# 0125. The Evolutionary Exploration and Validation of Needs

- **Status:** Accepted, provisionally (2026-11-18). The domain's name, `needs`, and the plug-in's name and colour are provisional.
- **Date:** 2026-11-18
- **Decided by:**
  - **The request.** "Write a needs analysis domain, I think we can do this. And an envoluntionary algorithm's based searching method. Specifically, from given data, we can utilize evoluntionary methods like GA, to test and evolve hypothesis. If necessary, we may utilize LLM Agent … from dataset, AI generate initial population of needs, then structural methods, like causuality analysis, ABM, statistical methods, etc can utilize to test hypothesis, or search new needs. Save robust needs, can repeat-repeat-repeat, old needs of course can participate in evolution, but should record and throw it when efficient long time … basically in each stage of GA framework, implement as many strategies as possible. But I think it should also envoluntionary strategy be flexible in the envoluntionary algorithm foundation … I not sure whether we have methods not utilize LLM to do envoluntionary exploration of needs … And try this feature in urban science studio." Then: improve any foundation found insufficient; survey papers on evidence-based decision making, causal analysis, complex systems for the social sciences, simulation-based policy, emerging evolutionary methods (coevolution among them) and formal reasoning.
  - **The design.** Claude, here, on heuresis, tyche, peira, nomos, apodeixis and archeion, with the survey `research/literature/reports/Evolutionary exploration and validation of needs.md`, which corrected two of the first design's safeguards.

## 1. Context

Needs assessment is the first step of social policy: who lacks what, judged by which standard. Its practice is a few comparisons chosen by an analyst, and its methodological literature is old (Bradshaw 1972; Witkin and Altschuld 1995). Subgroup discovery has long searched for groups with unusual targets (Klösgen 1996; Wrobel 1997), and evolutionary rule induction for decades (del Jesus et al. 2007). Large language models now propose hypotheses (Zhou et al. 2024). What was missing was their composition under a protocol that does not deceive itself:
- needs as statements in a language that a person, a program and a model all write;
- generators of every kind, a model among them, in one loop;
- tests of several kinds;
- data kept aside so that a search of thousands of statements confirms only what holds.

The infrastructure held the parts (heuresis's algorithms, tyche's tests, peira's designs, nomos's accessibility and agents, apodeixis's argumentation, archeion's journal) but not the loop whose every stage a caller chooses, nor online control of false discoveries, nor a reusable holdout.

## 2. Decision

### 2.1 Foundations First

- **heuresis `evolution`** (its decision 0006): one loop over any genotype assembled from a strategy at every stage:
  - scoring, parents, the schedule and credit of operators, survival, novelty;
  - the archive's admission by a judge and its retirement after long inefficacy, by age, on recheck or when full, each with its reason;
  - proposals from outside credited to their source; lineage and resumption.

  42 checks; heuresis 590.
- **tyche `testing`** (its decision 0027): comparisons of two groups; the bootstrap of clusters; online control of the false discovery rate (LOND, LORD++, SAFFRON, α-investing, e-LOND); e-values; the reusable holdout (Thresholdout). tyche 1673.
- **peira `surveys`** (its decision 0013): the difference of two ratios from one sample, the covariance of shared clusters kept. peira 445.

### 2.2 The Needs Domain

A field at height 0 on the foundations, C++ from the start; 151 checks; its theory note is `domains/needs/theory/needs-and-their-discovery.md`.
- **`groups`**: tables of units and groups as conjunctions of conditions, with canonical keys and distances.
- **`language`**: a statement of need, a group, a frame, a domain, an indicator and one of Bradshaw's four standards. Its grammar is BNF read by morphe. Errors come at line and column, and a text can be read line by line, so that a model's bad line is refused alone and never repaired.
- **`evidence`**:
  - the three parts of the data, of whole clusters, by a keyed hash;
  - the gap by design (the clusters as primary sampling units, peira) or by binomial reading (tyche);
  - stability under resampling of clusters;
  - causal support, provision's effect within the group adjusted by a declared graph's back-door set (peira, tyche);
  - a simulation of seeking satisfiers within reach of capacity, by the floating catchment and by agents choosing by a logit (nomos);
  - entitlement by declared rules through forward chaining (morphe);
  - the standing read from an argumentation framework of the tests (apodeixis).
- **`search`**:
  - seven generators without a language model (the language breadth first and depth first, random, beam search, a regression tree, the rules, the simulation's unmet demand); ten mutations and five crossovers;
  - the search on heuresis's evolution, the admission of §2.3, confirmation in order sealed in archeion's journal, and resumption.

### 2.3 The Protocol of the Search

As built, after the survey (the theory note's §5):
1. **Three parts.** Discovery (half) is seen by the search; validation (a quarter) is consulted only through Thresholdout with a budget; confirmation (a quarter) is used once for each statement admitted.
2. **Admission.** A statement is admitted when it meets every condition below; it is then kept and breeds:
   - its gap clears z* on discovery;
   - its effect reaches a least size;
   - it is stable under resampling;
   - every condition of its group and frame carries a gap of its own (after Webb 2007);
   - it reaches people in enough clusters to be testable;
   - it is no weaker than a kept statement of nearly the same people;
   - the holdout does not refuse it.
3. **Confirmation online.** In the order of admission, by the online rule chosen (e-LOND under any dependence by default), each test registered before and written after in a sealed journal.
4. **Nothing of the confirmation reaches the search**, held by a check: with the confirmation part's outcomes permuted, the population and the archive are the same in every generation.
5. **Retirement of two kinds.** Retirement from the search (superseded, idle, too old, the archive full) leaves the confirmation record. Retirement on recheck follows new data.

### 2.4 Models Propose, and Never Judge

`science.needs.propose` asks a model through `science.models`, in the manner of an optimiser's prompt (Yang et al. 2024, OPRO). The model is given:
- the language;
- the data's vocabulary;
- the population's best statements, with their scores on discovery alone.

It is never given a validation answer, a confirmation or a standing. What it writes is read by the engine like anyone's; a line that does not read is refused with the reason. Its statements enter the evolution as proposals of grade C, credited to it like any operator. A model is asked only from the work's own page at this computer's own address.

### 2.5 The Layers Above

- **The data library's `needs_tables.py`** writes `noetarium.needs-data/1`. Its first case is the City of Melbourne at the 2021 Census: 353 SA1s × sex × age, 5,837 rows, 149,049 people. It holds need for assistance (G18), personal income under $500 a week (G17), and unpaid care given (G25), beside each area's tenure, social housing, flats, household income and moves. The suppliers of care are CLUE's 58 blocks with floor space of hospitals and clinics, community use and institutional accommodation, one person served per 40 m², declared. The registry's ABS DataPack entry gains those tables.
- **The API's `science/needs.hpp`** (describe, read, test, map, generate, evolve, options; a search's state to JSON and back, a state whose journal does not verify refused) and **`science.needs`** (the engine, the proposer, rounds of evolution and proposals); 20 checks.
- **The Urban Studio's plug-in *Needs***:
  - the view: the field of statements as the page; a need opened in place with its tests in words, its argument and its line of operators; the loop of the search, each stage a place to choose its strategy; the areas by their part of the data, or by where a need lies; the ledger of confirmations against the rule's levels; the generations; statements of one's own and a model's proposals;
  - three tools (test, evolve, a need area by area) and the template *Needs in the City of Melbourne*;
  - a server side that runs a search in steps and keeps its state;
  - checks: 15 in the Urban Studio's suite (101 in all); the page driven in headless Chrome (23 steps, not in the suite; once with a real model); nine pictures in the gallery's §45.

## 3. Alternatives Considered

- **Needs as a module of social provision or sociology.** Needs are judged in every domain of satisfaction (care, housing, income, mobility, green space), and the method is independent of any; a field of its own, which social provision and others may use.
- **Benjamini–Hochberg over every statement admitted** (the first draft). Offline over an online process, and needing positive dependence; replaced by online rules (§2.3.3).
- **Two parts, the confirmed breeding** (the first draft). The confirmation leaks into the search (Dwork et al. 2015); replaced by three parts (§2.3.1).
- **A model as judge or as a source of felt need.** The literature found models' self-evaluation unreliable and synthetic respondents biased (Si et al. 2024; Bisbee et al. 2024); a model only proposes.

## 4. Consequences

- **What the Melbourne example shows** (ten generations, 537 statements judged, about two seconds): 55 admitted, 37 kept, 28 confirmed by e-LOND at α = 0.05; the holdout told 9 of its budget of 100.
  - Those over 65 in areas of low household income need assistance at 53% against 1%. These are likely homes for the aged, whose residents the census counts where the home is; the search found them without being told.
  - Young people around the universities mostly have personal incomes under $500 a week.
  - Older people in settled areas, where few have moved, give the most unpaid care.

  None of this is a cause. Provision's effect within the groups is not shown by the declared graph.
- **What the checks hold**:
  - on synthetic populations, the search finds, admits and confirms the planted needs and stands them robust, and on data with nothing planted confirms nothing;
  - resumption equals a search that never stopped;
  - the journal verifies, and an altered entry is found.
- **Found while building.**
  - The archive filled with variants of one need: 278 admissions on the planted data. The distinctness and productivity gates reduced them to 24.
  - Groups living in one area of a part have no standard error by the clustered design, so a least number of clusters became a constraint.
  - The holdout's budget was spent in 99 queries when its threshold was smaller than the ordinary disagreement between parts, so effects are clipped to one spread and the threshold is 0.3.
  - The spending constant printed by Javanmard and Montanari makes their sequence sum to 0.976.
- **Not built** (the survey's §11):
  - coevolution of statements with adverse strata (heuresis's planned `coevolution`);
  - heterogeneous effects of provision (peira);
  - e-processes for revoking on new waves of data;
  - counting standards over several indicators (Alkire–Foster, nomos);
  - expressed need corrected for the propensity to report;
  - a grading scheme for needs (tekmerion);
  - value-based argumentation among standards (apodeixis);
  - the vocabulary as a kategoria schema;
  - asynchronous online testing.
