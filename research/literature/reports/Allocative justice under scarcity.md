# Allocative Justice under Scarcity: Who First, What and How Much, and by What Procedure. A Survey

*A survey of how limited resources, finances above all, may be allocated justly when allocation is not treated as a single optimisation: the distributive and procedural theories of justice and their operational content, pluralism and the alternatives to maximisation, lotteries, sequence and redress; the formal methods of fair allocation (claims and rationing rules, priority and reserve systems, sequencing and queues, the fair division of goods and chores, lotteries and their decomposition, participatory budgeting, dominance and partial orders, the conflicts among algorithmic fairness criteria); urban and spatial justice, fiscal equalisation, place-based policy, index-based targeting, distributional weights, disaster funding, local justice for housing, schools and facilities, and the urban economics of capitalisation and displacement; generative justice, circular, regenerative and doughnut economics, the commons, evolutionary and complexity economics, and money and circulation; and allocation in practice, its procedural safeguards and failures, its decision-support tools, deliberation with language models, and evaluation; with what every allocation should report, the order in which to build, the test cases and open records, and what a tool must never claim.*

Status: `draft` (a survey for design, 2026-11-20). Activity: research, in service of use. It serves ADR 0136 (the infrastructure's decision on allocative justice); the user will research further from it.

## 0. Scope, Method and Markers

### 0.1 The Question

The request concerned allocative justice in both its senses: substantive (distributive) justice and procedural justice. When resources, finances above all, are limited, how should they be allocated: to which entity first, and then to which; what; and how much? The user did not want this treated as a formal optimisation. They asked for a weighing of humanist concerns, welfare and justice, which are not always optimisation problems. They offered an intuition that circular economics, generative justice (after Eglash) and evolutionary economics can help, and asked that urban science, urban economics and the theory of justice be brought to bear.

The survey therefore asks four things:

1. what the theories of justice, the formal methods of fair allocation, urban and spatial justice and the urban economics of allocation, generative justice, circular and evolutionary economics, and the practice of priority setting and deliberation each offer to the questions "who first, then who", "what" and "how much";
2. how distributive and procedural justice, and humanist concerns and welfare, can be weighed without collapsing them into one objective;
3. what language models may and may not do in such weighing and in deliberation;
4. how an infrastructure should build tools that help people allocate justly and show their reasons, without pretending that a single number settles the question.

*Remark.* The survey keeps four questions apart that are often run together. *Who first* is an order over claimants (a priority, a queue, a reserve, a draw). *What* is the choice of goods or bundles (which projects, which units, which items). *How much* is a quantity (an award under a claims rule, a grant under an equalisation formula, a share of an envelope). *By what procedure* is the way the rules, their parameters and the final choice are fixed, recorded, contested and revised. A method usually answers one of the four and presupposes answers to the others (Table 1.1).

*Remark.* Two further distinctions run through all five Parts. Justice (the correctness of an allocation) is distinct from legitimacy (the acceptability of being bound by it); a fair procedure confers legitimacy, and confers justice only where there is no independent criterion of a just outcome (Part I §3). And a computed verdict is always conditional ("under principles P with parameters θ, option x is maximal"), never unconditional ("x is just") (Part I §6).

### 0.2 How the Survey Was Made

Five surveys were written in parallel from the literature on 2026-11-20, each by a separate agent to one brief. They are kept whole as Parts I–V:

- Part I, the theories of distributive and procedural justice that bear on allocating scarce resources, with pluralism, incommensurability and the alternatives to maximisation, allocation in time and sequence, and non-ideal theory, redress and participation;
- Part II, the formal methods of fair allocation and their axioms: claims and rationing rules, priority and reserve systems, sequencing, picking and queues, fair division, lotteries, participatory budgeting, dominance and social welfare orderings, algorithmic fairness, and methods that return sets, orders and distributions;
- Part III, urban and spatial justice and the allocation of public resources among places, groups and services, with fiscal equalisation, place-based policy, index-based targeting, distributional weights, disaster funding, local justice for urban goods, participatory budgeting, equity budgeting, procedure, and the urban economics of tax capacity, sorting, capitalisation and displacement;
- Part IV, generative justice, circular and regenerative economics, commons and polycentric governance, evolutionary and complexity economics, and money and circulation;
- Part V, allocation in practice (priority setting in health and in crises, public money, research funding, social housing, humanitarian targeting), procedural safeguards and their failures, decision-support tools, language models in deliberation and allocation, and evaluation.

Each Part follows one form. Every section has a Takeaway, Cited Findings, Inferences and Gaps, so that what a source says is kept apart from what the survey infers from it, and each Part closes with what its findings mean for the design.

Records were retrieved from Crossref's API, the arXiv API, Europe PMC and PubMed, the Stanford Encyclopedia of Philosophy, publishers' and institutions' pages, and CRAN's, PyPI's and GitHub's pages for licences. Some sources could not be reached:

- a Crossref lookup was rate-limited (Crisp 2003, Part I §1.4);
- a publisher returned HTTP 403 (Eglash et al. 2024, Part IV §1), as did the IFS's page (Part III §2);
- the Commonwealth Grants Commission's methodology and OEHHA's scoring page timed out (Part III §2, §4);
- the abstract of Fleurbaey and Tungodden 2010 was not deposited (Part I §2).

Each Part says what it could not read.

Section 1 is a synthesis, written after reading all five Parts. It adds no reference that is not in them and cites the Parts (for example "Part II §2") rather than repeating their references. Inside a Part, a bare section reference (for example "§1.4" in Part III) means that Part's own section, not a section of this synthesis. The Parts overlap by design: Elster's *Local Justice*, Pathak, Sönmez, Ünver and Yenmez's reserve systems, Porto Alegre's participatory budget and the Method of Equal Shares, for instance, appear in several. The discrepancies found between them are listed in §0.4. Part VI is reserved for what is built.

### 0.3 Verification Markers

- **[V]** verified this session (2026-11-20) against a primary record: Crossref, arXiv, PubMed or Europe PMC, the Stanford Encyclopedia of Philosophy, publishers', institutions' or providers' pages, or GitHub and PyPI for licences. Part I adds **[V-reprint]**: only a later reprint or edition was verified.
- **[S]** confirmed only from a secondary source: a search summary, a review's record, a reference work, a news release.
- **[M]** from memory, unverified.

A marker attests the bibliographic record (authors, year, title, venue, DOI), not the claim. Statements of content say separately whether they come from the text, an abstract, a search summary or memory. The Parts use slightly different codes for this, and each Part's opening remark states its own. Formulas and theorem numbers written from memory are flagged where they are given (Part II: *(memory)*; Part III: "Formula (M)"; Part IV: [formula M]); Part II gives theorem numbers only where it read the text.

**Counts.** Table 0.1 gives the occurrences of each marker in each Part, counted by pattern on the notes.

- Repeated citations of one work are counted each time, so the counts are of occurrences, not of distinct works. Part I alone closes with a bibliography grouped by marker, whose counts of distinct works are given in parentheses.
- They are approximate, to within a few per Part, because some markers are written in variant forms ("[V page]", "[V via reviews]", "[V for arXiv; venue M]", "[S, review record]") and the legends of the notes were counted with the rest.
- The content columns count each Part's own code for where content comes from.

**Table 0.1.** Markers per Part (approximate occurrences).

| Part | [V] | [S] | [M] (records) | Content flags (approximate occurrences) | Distinct DOIs |
|---|---|---|---|---|---|
| I | ≈ 160 (137 works); [V-reprint] ≈ 9 (8 works) | ≈ 31 (24 works) | ≈ 23 (15 works) | "content: memory" ≈ 100; "content: SEP" ≈ 17; title only ≈ 10; abstract, review or snippet ≈ 9 | 158 |
| II | ≈ 235 | ≈ 11 | ≈ 17 | *(memory)* ≈ 90; *(abstract)* ≈ 34; *(text)* ≈ 6, all from Pathak et al. | 188, and 24 arXiv identifiers |
| III | ≈ 235 | ≈ 50 | ≈ 18 | (M) or "Content M" ≈ 200; (A) ≈ 40; (T) ≈ 7 | 232 |
| IV | ≈ 160 | ≈ 40 | ≈ 12 | "memory" ≈ 55; "abstract" ≈ 19; "search summary" ≈ 16; "text read" ≈ 13; [formula M] 5 | 122, and 3 arXiv identifiers |
| V | ≈ 200 | ≈ 65 | ≈ 37 | "content: memory" ≈ 100; "content: search summary" ≈ 33; "content: abstract" ≈ 24; "content: page" or title ≈ 16 | 159, and 21 arXiv identifiers |

*Caveat.* The records are mostly verified; the content mostly is not. Few primary books were read in full: none for Part I, which read two encyclopedia entries, a publisher's page and a review; one paper for Part II (Pathak et al., arXiv v3); for Part III a government response, a statistical release, a statute, de Sousa Santos 1998 and a few official pages; for Part IV five papers; for Part V abstracts and pages. Rawls, Sen, Walzer, Elster, Nussbaum, Roemer, Adler, Ostrom, Raworth and Moulin, among others, are reported from memory or from secondary sources. Statements of what a text says that are marked as memory must be checked against the text before they are relied on, and formulas and theorem numbers from memory must be re-derived or checked before they become test fixtures.

### 0.4 Corrections and Discrepancies Found While Assembling

1. **Elster, *Local Justice*: one record, three markers.** Part I marks it [V]: the Russell Sage Foundation's page was fetched (it lists December 1993 and ISBN 978-0-87154-232-8, probably the paperback), and Choice's review record dates the original to 1992. Part II marks it [S], since Crossref holds only reviews. Part III marks it "[V via reviews]". The publisher's page settles the record; 1992 is the original year.
2. **Records verified in one Part and only remembered or seen secondarily in another.** The stronger marker holds; the Parts are left as written.
   - Rawls, *A Theory of Justice* (1971): [V] in Part I (doi 10.4159/9780674042605); [S] in Part III.
   - Stone, "Why Lotteries Are Just", *Journal of Political Philosophy* 15(3) (2007): [V] in Part II (doi 10.1111/j.1467-9760.2006.00274.x); [S] in Part I.
   - Peters, Pierczyński and Skowron, "Proportional Participatory Budgeting with Additive Utilities": [V] in Part II through arXiv:2008.13276 (the NeurIPS 2021 venue [M]); [M] in Parts III and V, which found no Crossref record.
   - Kolm, "Unequal inequalities. I", *JET* 12(3) (1976): [V] in Part III (doi 10.1016/0022-0531(76)90037-5); [M] in Part II, where Crossref returned only Part II of the paper.
   - Taurek, "Should the Numbers Count?" (1977): [V-reprint] in Part I; [M] in Part II.
   - Kleinberg, Mullainathan and Raghavan 2017: arXiv:1609.05807 [V] in Part II, the 2023 ACM reprint [V-reprint] in Part I; the ITCS 2017 record is [M] in both.
   - Arnstein 1969: [M] in Part I; a reprint record verified in Part III.
3. **Heyard, Ott, Salanti and Egger on the Swiss National Science Foundation's lottery.** Part II cites the preprint (arXiv:2102.09958, [V], content from the abstract); Part V cites the published article (*Statistics and Public Policy* 2022, doi 10.1080/2330443X.2022.2086190, [V], content from a search summary). They are one study.
4. **The Health Research Council of New Zealand's survey.** Part II reports 126 respondents at a 39% response rate; Part V reports 325 applicants surveyed at 39%. They agree (126/325 ≈ 0.388). Both report 63% for and 25% against randomisation for Explorer Grants, from the abstract.
5. **Arnosti and Shi 2020 on lotteries and waitlists.** Part II quotes the abstract: independent lotteries per item and a waitlist in which agents lose priority on refusing an offer "lead to identical outcomes in equilibrium"; a common lottery for all items improves matching; with many low-need agents there is "an unavoidable trade-off between matching and targeting". Part III (memory) agrees in substance. Part V (memory) says that waiting lists and lotteries "differ in how well they target" and that a restricted lottery can match a waitlist. The abstract governs; Part V's statement should be checked against it.
6. **Porto Alegre's formula.** Part IV's Gap ("only a 2003 description of 1999 practice was found") is closed by Part III, which read de Sousa Santos 1998 (pp. 474–477): in the 1998 budget, lack of services and regional priority weighed 4 and population 2; the 1997 worked example uses weights 3, 2 and 3. Part III notes that the text's "scale from 1 to 3" is inconsistent with the weights it reports. Part IV's banding ("under 20% access scores 5, 80% access scores 1", from a World Bank note) agrees with Part III's ("up to 20.99% of streets needing paving gets grade 1 … 80% or more gets grade 5") once access is read as the complement of need.
7. **Moretti 2010's multiplier.** Part III writes "2.5 for high-skilled", Part IV "2.5 for high-tech jobs"; both from memory, both with 1.6 for tradable jobs in general. The paper decides.
8. **Elster's taxonomy of principles.** Part I (memory) lists egalitarian, time-related and status-related principles, principles defined by other properties of individuals, and mixed systems; Part III (memory) adds power-related principles. Both are to be checked against chapters 3–4 of *Local Justice*.
9. **The OECD's count of gender budgeting.** 23 of 38 members in 2022 is reported by Part III from OECD pages (A, record [V]) and by Part V from memory [M]. Part III's governs, with its detail that only 9% said the information was always or often used in decisions.
10. **Precedence in reserve systems.** Part III (memory) says that Boston's 50% walk-zone reserve had almost no effect because of the order of processing (Dur et al. 2018). Part II, from Pathak et al.'s text, adds that Dur et al.'s comparative statics (with one preferential category, later processing weakly helps its beneficiaries) fail in general: Example 1 shows a category gaining beneficiaries by being processed *earlier*, and Proposition 4 gives the conditions (at most five categories, each patient a beneficiary of at most one) under which the monotone statement holds. Not a contradiction, but the general statement is non-monotone.
11. **Pathak et al.'s numbering.** The theorem and proposition numbers in Part II are those of arXiv:2008.00374v3; the *Management Science* version may differ (Part II §2.4). Parts I and III describe the same paper from memory, consistently with Part II's text.
12. **Typographical repairs.** Two duplicated venue strings in Part V were repaired: "NeurIPS 2021, NeurIPS 2021 proceedings" became "NeurIPS 2021 proceedings", and "ICML, ICML 2023 (PMLR)" became "ICML 2023 (PMLR)". The remaining edits are those of heading levels, the replacement of each note's method preamble by a remark, and the removal of horizontal rules in Part IV.
13. **Items unresolved rather than corrected** are listed in each Part's Gaps. Among them:
    - the exact axioms of Fleurbaey and Tungodden 2010, stated from memory in Parts I and II, neither of which read the text;
    - the functional form of Martens' Accessibility Sufficiency Index (Part III §1.6);
    - the IMD's exponential transformation constant (23) and CalEnviroScreen's rescaling (Part III §4);
    - whether the 2026 Green Book kept η = 1.3 (Part III §5);
    - the number of participatory budgeting processes in the world (over 11,000 in Part V, from memory; "some thousands" in Part III, which found no primary statistic);
    - the counterexample instance of "EFX need not exist" and the MMS counterexamples, not extracted (Part II §4);
    - the Habermas Machine's preference share (about 56%, memory; Part V §5);
    - the licences of Pabulib's data, equalshares.net's results, CalEnviroScreen and several official datasets (Parts II, III, V).

## 1. What the Five Parts Say Together

The five Parts agree on the answer's form before its content. **No theory, method or practice surveyed supplies one objective whose maximum is the just allocation**, and the attempts to make one either fail by theorem or hide a moral choice in a parameter. What the literature does supply is a small set of structural operations, each of which settles one part of the question and leaves named parameters to people:

- **constraints** that filter what may be done at all (basic liberties, blocked exchanges, prohibitions of stigmatising rules, entitlements and recorded wrongs);
- **thresholds** settled first (sufficiency floors of need, capability or access, and ecological ceilings), with the shortfall below them minimised lexically before anything else;
- **orders over claimants**, alone or as reserves in which each principle governs a declared share and the order of processing is declared, for "who first, then who";
- **rules for how much** with known axioms (claims rules, equalisation of needs against capacities, banded points, proportional voting), shown side by side;
- **partial orders** over whole allocations, intersected over the admissible principles and parameters, whose maximal elements are the defensible options and whose disagreements are shown as pivots;
- **a recorded human choice or a declared lottery** among the options that remain on a par;
- **a procedural record** that makes reasons public, lets an affected party contest a specific reason, and feeds upheld contestations back into the rule.

The traditions the user expected to help do help, but in specific and limited ways. Generative justice gives a direction (value back to its generators, through loops, under their control) but no operational definition of its central term and no floor for those who cannot generate. Circular and regenerative economics give the shape of a rule (a floor under a ceiling), not a theory of justice. The commons give tested procedural conditions and rule templates, including rules that switch under scarcity. Evolutionary economics gives a way to study rules over time and a warning: what is stable is not therefore fair. Urban economics adds the question every allocation to places must answer, who finally gains once prices respond, and urban justice adds sufficiency of access and the warning that area indices miss deprived people in non-deprived areas. Practice adds the documented failures, which were procedural before they were technical. Language models can organise, articulate and draft, and their own distributive judgements diverge from human ones in both directions, so they must not choose.

An infrastructure should therefore build allocation as a composition of declared, separately evaluated rules over exact computations, report every allocation as who receives, who is excluded, who pays and who finally gains under which principles and parameters, show where the result is sensitive and what it turns on, and keep a sealed record of reasons, contestations and revisions. It must never present a conditional verdict as the just allocation.

### 1.1 Allocation Is a Composition of Declared Rules, Not the Maximisation of One Objective

**Five reasons maximisation fails.**

1. **Values may be incomparable.** Value pluralism and incommensurability break the trichotomy of better, worse and equally good: Raz's small-improvement argument, Chang's parity, Parfit's rough equality, Broome's vagueness (Part I §2, content from the SEP). A single maximand then need not exist, and a weighted sum Σ λ_p s_p(x) asserts by fiat commensurabilities that may not hold (Part I §2, Inferences).
2. **Aggregation into a complete order is impossible under mild conditions.** Arrow's theorem, read with principles as voters, makes one principle a dictator; Sen's liberal paradox; Fleurbaey and Tungodden's dilemma between the tyranny of aggregation and the tyranny of non-aggregation (from memory, axioms not read); Pazner and Schmeidler's incompatibility of envy-freeness with efficiency under production; the incompatibility of full compensation with full natural reward; Diamond's ex ante fairness against the sure-thing principle beneath Harsanyi's aggregation theorem; the incompatibility of calibration with balanced error rates across groups with different base rates (Part I §2; Part II §7–8). Each binds only on a demand for a *complete* ranking that satisfies all principles at once (Part I §2, Inferences).
3. **The free parameters carry the moral weight.** The index of primary goods, the concavity γ, the threshold τ, the partition into circumstance and effort, the sphere of a good, the order of processing, the distributional elasticity η: a tool that fixes any of them by default takes a moral position silently (Part I §1, Inferences).
4. **The plural and relational theories give constraints and audits, not objectives.** Walzer, Young, Fraser, Anderson, care ethics and Sen's *The Idea of Justice* cannot be written as a function to maximise (Part I §1, Inferences); Young's and Purcell's critiques deny that justice is a distribution at all (Part III §1, §13).
5. **Practice mixes.** Institutions allocate by mixtures of principles that evolve under pressure from allocating bodies, governments and public opinion (Elster, Part I §1.14; Part III §7.1), and every national health scheme examined settles the trade-off between maximising and priority to the worse off by a weight, a lexical order, a reserve or a lottery, each contested and revised (Part V §1).

**What replaces maximisation.** Read together, Part I §7.1–7.3, Part II §9–10, Part III §1 and §13.1 and Part IV §2 describe the same composite. Let X be the finite set of feasible allocations and 𝒫 the declared admissible principles, each p ∈ 𝒫 inducing a preorder ≽_p on X.

1. *Constraints.* F = {x ∈ X : x satisfies C₁, …, C_k}. If F = ∅ the conflict is reported, never relaxed silently (Part I §7.1).
2. *Thresholds.* F_τ = argmin_{x∈F} Σ_i Σ_k w_k max(0, τ_k − s_ik(x)), subject to any ceiling Σ_i e_i(x) ≤ B, with every τ_k, w_k and B declared with its source (Part I §1.4, §7.1; Part III §13.1; Part IV §2, Inferences). The headcount #{i : u_i ≥ τ} is excluded as a primary objective because it rewards lifting those just below τ at the expense of those far below (Part I §1.4).
3. *Orders and reserves* for indivisible units and for sequence: categories with their own priority orders and a declared order of processing (Part I §7.1; Part II §2).
4. *Partial order.* x ≽_𝒫 y iff x ≽_p y for every p ∈ 𝒫 (Sen's intersection approach generalised, Part I §1.9, §2); present M(F_τ, ≽_𝒫) = {x : ¬∃y, y ≻_𝒫 x}.
5. *Choice within the maximal set* by a recorded human decision with reasons, or by a lottery with declared chances: equal for equal claims, weighted by declared claim strength for unequal ones, with the randomness committed before the draw and the losers recorded as bearing residual unfairness (Part I §3, §7.3; Part II §5).
6. *The record* of every stage: rule, parameters and their provenance, data, reasons, alternatives not chosen (Part I §7.5; Part V §7.1).

*Remark.* Each stage is a choice function on finite sets that returns a non-empty subset of a non-empty input, except the constraint stage, which may return the empty set and must then report a conflict. The composite therefore never needs a complete order. For a reflexive relation the optimal set O(S) = {x : x ≽ y for all y} is contained in the maximal set M(S), and M(S) is non-empty for finite S when ≻ is acyclic (Part I §2, from Sen 1997); Sen argues that rational and just choice needs only M(S). The ordering ≽_𝒫 is complete only when 𝒫 (or the set W of admissible weights) is a singleton, and grows more incomplete as it widens, with dominance as the limit case (Part I §1.9).

*Remark.* The **pivot** of a comparison is the principle, parameter value or reading on which it turns (Part I §2, §7.1). For a parametric family it is a switching value: the γ, Atkinson ε or generalised-Gini parameter at which a pairwise comparison flips (Part II §7.3), or the η at which equity-weighted appraisal changes its ranking (Part III §5). Part I §7.1 notes that this mirrors the infrastructure's reading of a norm's open terms under every reading, with γ, τ and the typing playing the part of the open terms.

*Remark.* A weighted sum enters only as one declared member of 𝒫, never as the arbiter (Part I §2, Inferences), and only with the hygiene the critiques of multi-criteria decision analysis demand: an explicit budget and opportunity cost, weights tied to scales, checks for double counting, and sanity tests of Morton's kind, in which a cheap effective intervention must outrank an ineffective one (Part V §4.1).

**Three return types.** An allocation engine organised this way returns a *set* (the undominated outcomes; all outcomes satisfying a list of axioms, such as all cutoff equilibria of a reserve system), an *order* (a partial order with its Hasse diagram, or the necessary and possible relations of robust ordinal regression), or a *distribution* (a lottery's assignment matrix and its decomposition; SMAA's rank-acceptability indices over the admissible weights). A single optimum is the degenerate case (Part II §9). Every property of every candidate is a verdict with a witness or a counterexample, in the format of kategoria's verdicts (Part II §9.3, §10.3).

### 1.2 The Currencies and Principles Are Plural, and Their Conflicts Are Theorems to Be Shown, Not Resolved

**The shape of every theory.** Each distributive theory is a triple of a currency, an aggregation structure and side-constraints (Part I §1, Inferences). The structures in use form a short list: the sum (utilitarian), the concave sum Σ g(u_i) (prioritarian), the lexicographic minimum (leximin), lexicographic thresholds (sufficientarian), an extremum conditioned on type (Roemer), pairwise envy tests (Dworkin, Varian), a registry of principles per good (Walzer), and priorities by category with a declared order of processing (Elster; Pathak et al.).

**The principles the user named, with their free parameters.**

- **Need.** Bradshaw's normative need is exactly a declared threshold (Part I §1.4); Doyal and Gough's health and autonomy, Max-Neef's nine needs and decent living standards supply candidate dimensions (Part IV §2); Goodin grounds special responsibilities in vulnerability, and every need-based rule creates an incentive to become or appear needy (Part I §1.7). *Free:* the standard, and whose definition of need counts (normative, felt, expressed, comparative).
- **Desert, merit and contribution.** Desert's bases are contribution, effort and compensation for costs; it is backward-looking, unlike incentive payments (Part I §1.7, content from the SEP). In local allocation "merit" means either predicted performance (efficiency) or past achievement (desert), and the two diverge (Elster, memory). Harvey ranks contribution to the common good second, after need, in territorial distribution (Part III §1.1). *Free:* whether desert applies to the good at all (its sphere), and what counts as merit.
- **Urgency.** "Sickest first" judges the worst off at a moment and ignores prognosis (Persad, Wertheimer and Emanuel); the rule of rescue favours identified over statistical lives; fair innings judges over a lifetime (Part I §4). *Free:* time-slice against lifetime accounting.
- **Capability.** Sen's capability set with no fixed list and a partial order by intersection over admissible weights; Nussbaum's ten central capabilities, each to a threshold, not mutually substitutable (Part I §1.9). *Free:* the list and the weight ranges.
- **Sufficiency.** Frankfurt's doctrine; Casal's positive thesis (it matters that people live above a threshold) and her rejection of the negative thesis (nothing matters above it); Shields's shift thesis; multiple and upper thresholds (Part I §1.4). The poverty-gap family P_α of Foster, Greer and Thorbecke is its computable form (Part I §1.4; Part III §1.6). *Free:* τ, the number of thresholds, what governs above τ.
- **Priority.** W(u) = Σ_i g(u_i) with g strictly increasing and strictly concave; the Atkinson family g(u) = u^{1−γ}/(1−γ) (ln u at γ = 1), utilitarian at γ = 0 and approaching leximin as γ → ∞; Pareto, anonymity, separability and the Pigou–Dalton principle hold; Adler argues that well-being then needs a ratio scale (Part I §1.3, memory). *Free:* γ, the zero of well-being, ex ante against ex post.
- **Equality of opportunity.** Roemer's types and effort ranks: φ* = argmax_φ ∫₀¹ min_t v^t(π, φ) dπ, against Van de gaer's max_φ min_t ∫₀¹ v^t(π, φ) dπ (Part I §1.6, memory). *Free:* which factors count as circumstances; the partition into types.
- **Relational equality.** Anderson's democratic equality: a threshold of the capabilities needed to function as an equal citizen, no stigmatising disclosure, no hierarchy of standing, no access dependent on a patron's discretion (Part I §1.8). These are constraints and tests, not objectives.
- **Walzer's spheres.** Each good's social meaning fixes its distributive criterion; injustice is the dominance of one good over others; complex equality blocks conversion across spheres (Part I §1.11, memory). Operationally: a registry of principles per good, conversion tests (does an allocation's ranking correlate with possession of an unrelated good?), and blocked exchanges as feasibility constraints. *Free:* the social meaning of each good.
- **Elster's local justice.** Institutions allocate goods and burdens (college places, organs, layoffs, military induction, adoption) by egalitarian, time-related, status-related and individual-property principles combined in point systems, lexical combinations, quotas and thresholds followed by a lottery; principles have incentive effects and are manipulable; first-, second- and third-order actors shape them (Part I §1.14; Part III §7.1; both partly from memory, see §0.4).
- **Sen's comparative justice.** Realisation-focused comparison of feasible states rather than the identification of perfectly just institutions; the three children and the flute, each with a claim supported by an impartial theory, as the case in which several impartial reasons survive scrutiny without one defeating the others; choice of a maximal, not an optimal, element (Part I §1.10).
- **Oppression, recognition and representation.** Young's five faces of oppression and the social connection model of responsibility; Fraser's redistribution, recognition and representation with participatory parity and the "all-subjected" principle (Part I §1.12). These become audits of who decides, whose needs are recognised, and whose position the structure reproduces.
- **Care.** Dependency is universal; the distribution of care work is itself a distributed quantity (Part I §1.13; Part IV §2).
- **History.** Nozick's justice in acquisition, transfer and rectification is historical, not patterned; past injustice undermines later distributions (Part I §4, content from the SEP).

Two parameters recur under different names. **The elasticity η in the social discount rate r = δ + ηg is an inequality-aversion parameter**, the same value choice as prioritarian γ (Part I §4, §7.4). **Atkinson's ε and the distributional-weight elasticity η are the same normative quantity** (Part III §1.9, §5). A tool should expose each once and say where it reappears.

**The conflicts, stated as what cannot be had together.**

| Conflict | What cannot be had together | Part |
|---|---|---|
| Fleurbaey–Tungodden (memory) | Pareto, minimal aggregation and minimal non-aggregation: leximin is the tyranny of non-aggregation; utilitarianism and every finitely concave prioritarianism are tyrannies of aggregation; the puzzle appears in people's own social preferences (Cowell, Fleurbaey and Tungodden 2015) | I §2; II §7 |
| Arrow (memory) | unrestricted domain, weak Pareto, independence of irrelevant alternatives and non-dictatorship, read with principles as voters | I §2 |
| Sen's liberal paradox (memory) | unrestricted domain, weak Pareto, minimal liberalism and acyclic social preference | I §2 |
| Pazner–Schmeidler 1974 | envy-freeness and efficiency with production and unequal skills; egalitarian equivalence as the alternative | I §1.5, §2 |
| Compensation and reward | full compensation for circumstances and full natural reward for responsibility | I §1.5, §2 |
| Diamond 1967 | ex ante fairness of chances and the sure-thing principle beneath expected-utility aggregation | I §1.2; II §5 |
| Kleinberg–Mullainathan–Raghavan; Chouldechova | calibration within groups and balance of both error rates, unless base rates are equal or prediction perfect; FPR = [p/(1−p)]·[(1−PPV)/PPV]·(1−FNR) | I §2; II §8 |
| Bogomolnaia–Moulin 2001 (memory) | sd-efficiency, sd-strategy-proofness and equal treatment of equals for n ≥ 4 | II §3 |
| Brill et al. 2023 | EJR-based proportionality for both common satisfaction functions in participatory budgeting | II §6 |
| Fair division of indivisible items | EFX need not exist for submodular valuations (three agents, eight goods, 2026) nor for additive chores with n ≥ 4 (2026); open for additive goods with n ≥ 4; MMS can fail to exist | II §4 |
| Casal; Uitermark–Nicholls | a sufficiency threshold with indifference above it; the just city's equity, diversity and democracy without trade-offs in practice | I §1.4; III §1.3 |

**How a conflict is displayed.** Part I §7.2 specifies it: the options in M(S); for each, the principles under which it is best or undominated; the trade-off between them in the principles' own units ("B saves 12 more life-years; A leaves 40 fewer people below τ"); the pivots; the people affected differently. The *standing* of an option is kept apart from its *measure* ("untenable under sufficiency" beside "Σ welfare = …"), the infrastructure's practice of dual verdicts. When a combination of demands is impossible, the tool states the theorem and asks which demand to relax.

### 1.3 Each Principle and Method Decides a Different Part of the Question and Leaves Different Parameters to People

Table 1.1 maps the principles and methods of the five Parts onto the four questions of §0.1. "Computable" means that, once its free parameters are declared, the rule can be evaluated or its properties checked exactly; the last column lists what must stay with people.

**Table 1.1.** What each principle or method decides, whether it is computable, and what stays human.

| Principle or method | Decides | Computable once declared | What stays human | Part |
|---|---|---|---|---|
| Constraints: basic liberties, fair equality of access, blocked exchanges, prohibitions of stigma, historical flags | what may be done at all | yes, as feasibility filters; an empty feasible set reported | which constraints; whether a practice is demeaning; the legitimacy of past acquisitions | I §1.1, §1.8, §1.11, §7.1 |
| Sufficiency thresholds: shortfall, FGT P_α, accessibility sufficiency | how much first (close gaps) | yes; headcount flagged as perverse | τ, how many thresholds, what governs above τ | I §1.4; III §1.6; IV §2 |
| Nussbaum's capability floors; doughnut floor and ceiling | how much (bounds) | yes, given indicators and thresholds | the list; the thresholds; the downscaling and fair shares of a ceiling | I §1.9; IV §2 |
| Utilitarian sum, CBA, CEA | how much (evaluation) | yes | the welfare metric; interpersonal comparability; which preferences count | I §1.2 |
| Prioritarian Σ g(u); Atkinson, Kolm, generalised Gini | how much (evaluation) | yes, for each declared γ, ε or weight vector | γ (show the range); the zero of well-being; ex ante or ex post | I §1.3; II §7 |
| Leximin, maximin, p-centre | who first (worst off); how much | yes, given an index | the index; who the least advantaged are | I §1.1; III §7.4 |
| Roemer's equality of opportunity | how much | yes, given types, outcome and policies | which factors are circumstances; whether to use data on effort at all | I §1.6 |
| Envy-freeness, EF1, EFX, PROP, MMS; CEEI, maximum Nash welfare | what (bundles) | checkable; existence known only in part; MNW is an optimisation | which notion; the valuations accepted | I §1.5; II §4 |
| Need (Bradshaw's four standards) | who first; how much | yes, as orders and thresholds | the standard; whose definition of need | I §1.7, §6 |
| Desert, merit, contribution | who first | yes, as an order on a declared measure | whether desert applies to the good; what counts as merit | I §1.7 |
| Urgency, rule of rescue, fair innings | who first | yes, as orders | time-slice or lifetime accounting | I §4; V §1.6 |
| Queues, waiting time, first-come first-served | who first | yes; bottleneck against scarcity checkable from capacity and demand | whether arrival may count | I §4; II §3 |
| Reserve systems with a precedence order | who first, by category | yes: DA, cutoffs, the three axioms checked, outcomes over all orders | the categories, their sizes, the order of processing, soft or hard reserves | I §1.14; II §2; V §1.5 |
| Serial dictatorship, RSD, probabilistic serial, picking sequences | who first; what | yes; RSD probabilities #P-complete, enumerated only when small | the priority order; ex ante or ex post properties | II §3 |
| Claims rules: PRO, CEA, CEL, Talmud, Piniles, RA, AP, TAL | how much | yes, exactly, in O(n log n) except RA | which rule (the attitude to small and large claims); what counts as a claim | II §1 |
| Equalisation of needs against capacities | how much, among places | yes | the standard; the equalisation rate λ; need indicators and unit costs; the envelope of floors, caps and guarantees | III §2 |
| Porto Alegre's banded points | how much, by sector and region | yes | the criteria, bands and weights (set by a citizens' council) | III §8; IV §3 |
| Greedy approval, MES, sequential Phragmén | what (projects) | yes; EJR and PJR checkable on small instances | the rule; MES's completion; who may vote | II §6; III §8.4; V §2.1 |
| Lotteries: equal, weighted, partial; BvN decomposition | who among equals | yes, verifiable: fractional assignment and its decomposition, committed seed | when claims are equal; the weights; the band of indistinguishable options | I §3; II §5; V §2.3 |
| Dominance and intersection orders: Lorenz, generalised Lorenz, sequential, ≽_𝒫; SMAA; robust ordinal regression | which allocations are undominated | yes | the admissible principles 𝒫; the choice within M(S) | I §2; II §7, §9 |
| Distributional weights w_i = (ȳ/y_i)^η | how much (valuation in appraisal) | yes, with switching values of η | η; the reference person | III §5; V §1.2 |
| Incidence: capitalisation, sorting, displacement; SAM multipliers and structural paths | who finally gains | yes, with ranges, given elasticities and coefficients | which model and elasticities are accepted | III §3, §11; IV §5 |
| Acequia switch rule | how much under scarcity | yes | the entitlements; the survival floor | IV §3 |
| Generative-justice return ratio, extraction share, circulation share | where value returns | as proposals, not in Eglash's text | what counts as unalienated value; control labels | IV §1, §6 |
| Commons design principles (congruence of costs and benefits, collective choice, nesting) | procedure; rule durability | partly: accounts of contributions against benefits | the rules themselves and who amends them | IV §3 |
| Agent-based and evolutionary models of rules; quality-diversity archives | how rules behave over time; a repertoire of options | yes, as maps of basins and persistence | the choice; never a certificate of fairness | IV §4, §6 |
| Leventhal's rules, A4R, Tyler's relational model | procedure | partly: consistency, publicity, appeal paths, representation, declared conflicts, data provenance | the relevance and ethicality of reasons; legitimacy | I §3, §7.5; V §1.4 |
| Contestation as argument (Dung, ASPIC+) | procedure; standing of decisions | yes, the standing recomputed | the arguments and their acceptance | I §7.5; II §8.3 |
| Language models | procedure support: organising, articulating, locating disagreement, drafting | not as judgement | every judgement | V §5, §7.2 |

### 1.4 Who First and Who Next Is Decided by Orders, Reserves, Queues and Lotteries, and by History

**Priority classes are the only admissible asymmetry under consistency and composition.** Moulin 2000 shows (memory) that rationing rules satisfying consistency, scale invariance, composition up and composition down are combinations of a priority order over classes of claimants with weighted PRO, CEA or CEL within classes; with equal treatment of equals only PRO, CEA and CEL remain. Part II §1.3 reads this as the formal bridge between rationing and priority, the mathematics behind "to which entity first".

**Reserve systems let several values govern without one score.** Pathak, Sönmez, Ünver and Yenmez (Part II §2, read in arXiv v3) divide q identical units into categories c with capacities r_c, each with its own priority order π_c (which may encode a different ethical value).

- *Three axioms.* Compliance with eligibility, non-wastefulness and respect of priorities are "minimal requirements for a reserve system".
- *Theorem 1.* A matching satisfies them iff it is supported by a cutoff vector as a cutoff equilibrium; the supporting cutoffs form an interval (Lemma 1), and the maximum cutoff is the one announced in practice.
- *Proposition 1.* Equivalently, iff it is the outcome of individual-proposing deferred acceptance for some artificial preferences over categories. Deferred acceptance, already in nomos, is therefore the engine (Part II §2.3).
- *Precedence.* Sequential processing by an order ▷ is one such matching (Proposition 2); processing a category earlier weakly raises its maximum cutoff (Proposition 3); but the effect on its beneficiaries is not monotone in general (Example 1), and Proposition 4 gives the conditions under which it is.
- *Smart reserves.* A family indexed by n ∈ {0, …, r_u} is also maximal in beneficiary assignment (Proposition 5, Theorem A.2), computed with Hopcroft–Karp and the Hungarian algorithm.

Because the order matters non-monotonically, a tool must present the outcomes over every precedence order, or over the whole smart-reserve family, as a set (Part II §2.3; Part I §4, Inferences). Reserves have been used: Pennsylvania's antivirals; Washington DC's 25% minimum guarantee and 20% over-and-above (December 2020); Richmond and Henrico's 10% for hard-hit areas (April 2021); NASEM's recommended 10% (Part II §2.2, text); Tennessee's 85/10/5 split and Massachusetts' 20% reserves (Part V §1.5, [S]). The same structure handles India's vertical and horizontal reservations (Sönmez and Yenmez 2022) and school seats, where Boston's walk-zone reserve showed how much the order of processing matters (Part II §2.2; Part III §7.3).

**Queues against lotteries is a question of scarcity against bottleneck.** In a bottleneck everyone is eventually served and a queue may be just; under true scarcity a queue rewards arrival, an arbitrary circumstance (Part I §4, with a 2024 paper whose author was not confirmed, [S]). The formal results refine this.

- With private values all agents prefer first-come first-served to the lottery, but waste is lower at the lottery; with common values all prefer first-come first-served and waste is minimised at the lottery (Bloch and Cantala 2017, abstract; Part II §3).
- Waiting times act like prices, their random fluctuation misallocates, and a simple randomised policy can reduce misallocation (Leshno 2022, abstract).
- Lotteries per item and a waitlist with loss of priority on refusal give identical equilibrium outcomes; a common lottery improves matching; with many low-need agents matching and targeting trade off (Arnosti and Shi 2020, abstract; see §0.4).
- Deterministic point systems that favour earlier arrivals can make assignment probabilities very unequal unless randomised ([S], Part I §4).

**Urgency against benefit.** "Sickest first" alone ignores prognosis and judges the worst off at a moment; the complete lives system favours the young (Persad et al. 2009); COVID-19 guidance made maximising benefits overriding, ruled out first-come first-served, and used random selection among similar prognoses (Emanuel et al. 2020) (Part I §1.15, §4; Part V §1.5). Urgency scores encode injustice through their proxies: SOFA overestimated Black patients' mortality (memory for the conclusion), creatinine-based MELD under-estimated women's severity by up to 2.4 points, and race-based eGFR delayed Black candidates' waiting time (Part V §1.5–1.6). Organ allocation is moving from classifications to a composite score (continuous distribution), a scalarisation (Part II §3.3; Part V §1.6).

**Sequential assignment without money.** Serial dictatorship is characterised by strategy-proofness, non-bossiness and neutrality; random serial dictatorship equals the core from random endowments; probabilistic serial ("eating") is sd-efficient and sd-envy-free but only weakly strategy-proof; computing RSD's probabilities is #P-complete; under dichotomous preferences the egalitarian random matching is efficient, Lorenz-dominant and group strategy-proof (Part II §3, memory). A tool should report ex ante properties of the assignment matrix and ex post properties of each realisation separately, since the rules differ exactly there (Part II §3.3).

**Allocation is a history, not a snapshot.** Part I §4 and §5 make four points.

1. *Lifetime against time-slice.* Fair innings, rotation and seniority need a ledger of past allocations, waiting times and burdens; verdicts can differ between the two accountings, and the difference should be shown.
2. *Historical entitlement and redress.* Nozick's historical principles, Waldron's supersession and reparations make some allocations unjust whatever their pattern; a claim from past injustice is a ground of its own, kept apart from a measure of disadvantage, and reservations are a computable form of redress whose size, beneficiaries and order are the decisions.
3. *Incentive effects.* Rules change behaviour: registering early, appearing needy (Elster).
4. *Retroactive correction.* The eGFR waiting-time restoration processed 6,103 modifications with a median of 1.7 years added within six months ([S]); a tool should keep enough history to recompute who would have been served under a corrected rule (Part V §1, Inferences).

Part I §4 found no philosophical treatment of the justice of *sequences* of allocations as such, beyond these.

**Path dependence.** Complexity economics treats the economy as forming, with increasing returns and path dependence (Arthur), and evolutionary game theory shows that a discriminatory norm can persist for times that grow exponentially with memory length and population (Axtell, Epstein and Young 2001) (Part IV §4). An allocation rule run over time is a dynamical system whose long-run outcome depends on where it starts; a tool should map that dependence rather than assume a steady state.

**Across generations.** The social discount rate r = δ + ηg separates pure time preference δ (ethically contested; Ramsey called it "ethically indefensible", wording [M]) from growth discounting, and η is the inequality aversion of §1.2 (Part I §4).

### 1.5 What and How Much Is Decided by Claims Rules, Equalisation, Banded Points, Proportional Voting, and a Floor under a Ceiling

**Claims rules.** For a claims problem (c, E) with c ∈ ℝ₊ᴺ, C = Σ c_i and 0 ≤ E ≤ C, a rule gives awards 0 ≤ x ≤ c with Σ x = E (Part II §1; definitions from memory, standard across Thomson's survey and book).

- PRO: x_i = λc_i, λ = E/C. CEA: x_i = min(c_i, λ). CEL: x_i = max(0, c_i − λ).
- Talmud: x = CEA(c/2, E) if E ≤ C/2, otherwise x = c/2 + CEL(c/2, E − C/2). Piniles: the same with CEA in both halves.
- Random arrival: the average over all n! arrival orders of first-come first-served, equal to the Shapley value of v(S) = max(0, E − Σ_{j∉S} c_j). The Talmud rule is the nucleolus of the same game (Aumann and Maschler).
- Duality: Rᵈ(c, E) = c − R(c, C − E); PRO, Talmud, RA and AP are self-dual; CEA and CEL are each other's dual.
- The TAL family Tᶿ runs from CEL (θ = 0) through Talmud (θ = ½) to CEA (θ = 1); Young's parametric rules x_i = f(λ, c_i) are characterised by symmetry, continuity and consistency.
- CEA's award vector Lorenz-dominates that of every rule satisfying the claims bounds, and CEL's is Lorenz-dominated by every other (memory).

Every rule except RA is a one-parameter water-filling computable exactly in O(n log n) (Part II §1.3). Their axioms (consistency, composition up and down, self-duality, truncation invariance, minimal rights, securement, order preservation) separate them, and different reasonable axioms select different rules, a microcosm of pluralism (Part I §2; Part II §1). The choice of rule is visible as an attitude to small and large claimants when the outcomes of all rules are shown at once, with the Lorenz order among them and the TAL curve between CEL and CEA (Part II §10.3). Extensions cover priority classes, several issues, eligibility on a bipartite graph, indivisible units and baselines (Part II §1).

**Equalisation of needs against capacities.** Every fiscal equalisation system computes, for each jurisdiction, a standardised expenditure need and a standardised revenue capacity, and pays a share of the difference (Part III §2; formula from memory, standard in Boadway and Shah):

G_i = λ P_i [(e_i − ē) + Σ_k t_k (b̄_k − b_ik)] + P_i g₀.

- *Canada* has no needs side: E_i = max{0, P_i(F̄C − FC_i)} against the ten-province standard, with partial inclusion of resources, a fiscal capacity cap and a GDP-indexed envelope ([S]).
- *Germany* since 2020: Z_i = 0.63 (AMZ_i − FKZ_i), with city-state inhabitants weighted 135% and municipal taxes at 75% ([S]).
- *England* from 2026–27: needs formulas reduced from 15 to 9 times an Area Cost Adjustment, minus council-tax capacity at a notional Band D of £2,060 (rising to £2,265 by 2028–29) for "full fiscal equalisation", phased in thirds, with floors protecting 100% of 2025–26 income for most authorities and 95% for those 15% or more above their assessment (T).
- *Australia*: the 2018 reforms (a benchmark to the stronger of New South Wales and Victoria, a relativity floor, and a No Worse Off guarantee) "have largely not achieved their goals and have cost more than 4 times as much as projected", almost $23 billion to 2024–25, leaving Western Australia with 113% of its assessed needs and the others with 98% (Productivity Commission interim report, 14 August 2026, A).
- *Sweden* guarantees a tax base of 115% of the national mean per capita ([S]).

Three lessons follow (Part III §2, Inferences). Hold the two sides and the political envelope (floors, caps, guarantees, phasing) separately and report how much of each allocation the envelope moves, because the envelope can dominate. The choice of standard (average, or the stronger of two named states) is a justice choice. And full against partial equalisation of tax capacity is Buchanan's horizontal equity against local autonomy, as the 29% and 17% of England's respondents show.

**Banded points.** Porto Alegre's participatory budget (de Sousa Santos 1998, read) allocated each sector's investment among regions in proportion to points:

points_{rs} = Σ_{c ∈ {need, population, priority}} w_c g_{crs},  share_{rs} = points_{rs} / Σ_{r'} points_{r's},

with grades g from banded indicators and weights w set annually by the citizens' council. In the 1997 paving example (20 km), Extremo Sul scored 3×4 + 2×1 + 3×4 = 26 of 262 points and received 9.9% (1,985 m); Centro scored 11 and received 4.2% (840 m) (Part III §8.1). Population is graded in bands, which caps the advantage of large regions; two earlier criteria were dropped as manipulable and as technocratic. The rule is transparent, contestable, simpler than an optimisation, and was revised by the people it served (Part III §8, Inferences). As participatory budgeting diffused, most adopters kept the vote and dropped the redistributive formula (Ganuza and Baiocchi; Cabannes and Lipietz); Porto Alegre suspended it in 2017 (Part III §8.3).

**Proportional voting.** Utilitarian greedy selection by approvals is not proportional. The Method of Equal Shares gives each voter b_i = L/n, calls a project ρ-affordable if Σ_i min(b_i, ρ u_i(p)) = cost(p), buys the project affordable at the least ρ, deducts the payments, and repeats (Part II §6, [S] for the definition; Part III §8.4). It satisfies EJR for approval ballots with arbitrary costs and EJR up to one project for additive utilities, in polynomial time; it is not exhaustive (it needs a completion), not Pareto optimal and not committee monotone (Part II §6). The equalshares.net example spends €1,000 of €1,100, which shows the need for a completion. Wieliczka, Aarau and Świecie adopted it in 2023; in Aarau it funded 17 projects against greedy's 7, from every district, but dropped the third most popular (Part III §8.4; Part V §2.1; the figures are the designers' and their partners', not independent audits). Part III §8 proposes the combination that restores the lost need term: need-weighted envelopes by district, Porto Alegre's step, then MES within each envelope, so that need and voice each govern a declared share.

**A floor under a ceiling.** The doughnut (a social foundation inside an ecological ceiling), theories of human need and decent living standards give the shape of a rule (Part IV §2): for each unit i and need k a declared floor f_k on satisfiers s_ik; a ceiling Σ_i e_i ≤ B on ecological use, possibly per capita with fair-share adjustments for historical responsibility; and between them a space settled by other principles and by procedure. The computable form proposed in Part IV (not Raworth's) is lexicographic: minimise Σ_i Σ_k w_k max(0, f_k − s_ik) subject to the ceiling; report overshoot; choose among allocations tied at the minimum by procedure or by priority to the worst off. Transport justice has the same form: Martens' sufficiency threshold of accessibility, a cap on the gap between best- and worst-served, and plans judged by their effect on those below the threshold (Part III §1.6). An upper limit on wealth (Robeyns's limitarianism) is the optional ceiling of distributive theory (Part I §1.16, §7.1). Under scarcity the commons supply the switch: New Mexico's acequias allocate water in proportion to land in plenty and secure each member's survival in drought (Cox et al. 2010, Part IV §3):

allocation_i = entitlement share × S if S ≥ Σ_i need_i; otherwise each unit to its survival floor first, then pro rata.

**What, among indivisible goods.** For divisible goods, competitive equilibrium from equal incomes (equivalently maximum Nash welfare for additive utilities, by the Eisenberg–Gale programme) is envy-free and efficient. For indivisible goods, EF1 with Pareto optimality always exists (maximum Nash welfare gives both); EFX exists for two and three additive agents, for four additive agents with at most nine goods (machine-checked, 2026) and for binary valuations, need not exist under submodular valuations (three agents, eight goods, 2026) and fails for additive chores with four or more agents (2026); the additive-goods case with four or more agents is open; MMS can fail, with 2/3 always achievable and the best guarantee just above 3/4 (Part II §4, the 2026 results from abstracts). A library should report the set of notions an allocation satisfies, each with a counterexample where it fails, rather than one fairness score (Part II §4.3).

**How much, in appraisal.** Unweighted benefit–cost analysis favours projects for the rich, because willingness to pay rises with income; weights w_i = (ȳ/y_i)^η correct this. The UK Green Book recommends η = 1.3; the US Circular A-4 of 2023 allowed η = 1.4 (a group at half the median weighted 2^{1.4} ≈ 2.64) and was rescinded in 2025; a meta-analysis tentatively concludes 1.6, with sensitivity at 1.2 and 2.0 (Part III §5). Health appraisal adds distributional cost-effectiveness analysis (equity impact and equity trade-off reported side by side), NICE's severity weights of 1.2 or 1.7, and a threshold raised to £25,000–35,000 per QALY from April 2026 against an estimated marginal productivity of the NHS near £13,000 ([S]; Claxton et al.'s £12,936 from memory) (Part V §1.1–1.2). A tool should compute unweighted and weighted results for η ∈ {0, 1, 1.3, 1.4, 1.6, 2} and show the switching values, and should apply weights to incidence-adjusted benefits (Part III §5, Inferences).

**How much, by chance.** A lottery is two objects: the fractional assignment (what each is entitled to in expectation) and its decomposition into feasible deterministic allocations, by Birkhoff–von Neumann or, when the constraints form a bihierarchy, by Budish, Che, Kojima and Milgrom. Both can be returned and verified exactly in rationals, and a weighted lottery is a claims rule applied to chances (Part II §5.3).

### 1.6 Procedure Confers Legitimacy, Not Correctness, and Failed First Where Burdens Shifted and Appeals Did Not Revise the Rule

**Allocation is mostly imperfect procedural justice.** Rawls separates perfect procedural justice (an independent criterion and a procedure sure to meet it), imperfect (a criterion but no such procedure) and pure (no independent criterion, so a fair procedure makes its outcome fair). Allocating scarce goods under reasonable disagreement about the criterion is mostly imperfect, which is why accountability for reasonableness (A4R) puts conditions on process (publicity, relevance, revision and appeals, enforcement), and why its critics say process cannot replace substantive principles (Rid; Friedman; Hasman and Holm) (Part I §3; Part V §1.4). A tool may therefore say "legitimately decided", never "correct" (Part I §7.7).

**The checkable criteria.** Leventhal's six rules (consistency, bias suppression, accuracy, correctability, representativeness, ethicality, applied to seven procedural components), Thibaut and Walker's process control, and Tyler's relational model (neutrality, trustworthiness, standing) give criteria of procedural fairness (Part I §3). Many are checkable properties of a record of decisions: the same rule applied to like cases; each decision's rationale stored and accessible; an appeal path with a reasoned response; who was consulted; conflicts of interest declared; the provenance of the data. Relevance and ethicality need human judgement (Part I §3, Inferences). Measured implementations score unevenly: Ontario hospitals met relevance (75.0%) more often than appeals (56.6%), publicity (56.0%) or enforcement (39.5%) (search snippet, Part I §3).

**Acceptance is not the justification.** Voice raises perceived fairness even with unfavourable outcomes, but the effect is double-edged: voice without influence legitimates bad outcomes (Part I §3). For decisions on policy, outcome favourability is "overwhelmingly the strongest determinant" of acceptance (Esaiasson et al. 2019, search summary), causal evidence that procedural justice changes compliance is weak (Nagin and Telep), and acceptance of lotteries is higher among winners (Part V §2.3, §3.7). **Safeguards must therefore be justified by rights and correctability, not by persuasion** (Part V §3, Inferences).

**The failures were procedural before they were technical.**

- *Robodebt* (Australia, 2016–2020): income averaging "inconsistent with social security legislation", against legal advice given in November 2014; about 470,000 wrongful debts; meant to save AUD 4.7 billion, it delivered AUD 406 million at a cost of AUD 971 million (Royal Commission, July 2023, [S]). Tribunal decisions against the method were not appealed, so no binding precedent formed (memory).
- *The Dutch childcare benefits affair*: the parliamentary inquiry *Ongekend onrecht* (17 December 2020) found that "fundamental principles of the rule of law have been violated", efficiency and fraud prevention having been so prioritised that individual circumstances received little or no consideration ([S]); a risk model used nationality as an indicator, and the cabinet resigned in January 2021 (memory). The System Risk Indication legislation was held to violate Article 8 ECHR in February 2020 (memory).
- *The 2020 English exam grades*: 39.1% of teacher-assessed A-level grades were lowered, hardest for students from disadvantaged schools, while small cohorts, common in private schools, escaped the adjustment; appeals were confused, and teacher grades were restored on 17 August 2020 ([S]; the cohort rule from memory). Ireland's system, which kept school-level teacher judgement, survived (memory).

Part V §3 reads one structure in them: (1) the burden of proof reversed onto the person; (2) no individual consideration (all-or-nothing rules, averaging); (3) known legal doubts not acted on; (4) explanations withheld or unintelligible; (5) appeals that resolved individual cases without correcting the rule; (6) protected characteristics or their proxies used as risk factors. Health allocation adds proxies that encode injustice, parameters moved for reasons outside the declared framework, discretionary circumvention of published rules (out-of-sequence organ offers, memory) and categorical exclusions found discriminatory (Part V §1, Inferences). Each is detectable only if the tool keeps the proxy, the parameter's provenance and every deviation on the record.

**What the law now requires.** Under Article 15(1)(h) GDPR the data subject may require an explanation of "the procedure and principles actually applied", concise and intelligible enough to understand and challenge the decision; neither a complex formula nor a description of every step suffices (CJEU C-203/22, 27 February 2025, press release [S]). The EU AI Act classes systems used by public authorities to evaluate eligibility for essential public benefits and for emergency triage as high-risk, with a fundamental-rights impact assessment, human oversight and a right to explanation (memory) (Part V §3.5).

**Human oversight is not a safeguard by itself.** People follow algorithmic advice selectively, more when it matches stereotypes (Alon-Barkat and Busuioc, memory), and are poor at judging and correcting algorithms (Green 2022, memory). In a randomised experiment with caseworkers on 770 eligibility questions, near-perfect chatbot suggestions raised accuracy by 27 points from 49%, but incorrect ones cut accuracy on easy questions by about two-thirds, and gains plateaued below the chatbot's own accuracy (Gosciak et al. 2026) (Part V §3.6).

**Explanation, control and participation.** Reasons for a decision raised perceived trustworthiness more than access to the code (Grimmelikhuijsen 2023, memory); outcome control over an algorithmic division raised perceived fairness more than transparency alone (Lee et al. 2019, memory) (Part III §10; Part V §3.7). Participation should set weights and thresholds, as Porto Alegre's council did, not only rank projects, and the record should keep who set each parameter, when and under what mandate: Fraser's "who" and the rungs of Arnstein's ladder (Part I §5, §7.5; Part III §10). Panels can be drawn with selection probabilities as equal as quotas allow (Flanigan et al. 2021), and stakeholders' own models can be elicited and aggregated (WeBuildAI) (Part II §5; Part V §4.3). Participatory budgets are captured when agencies shape and veto the choices or turnout does not reproduce the population (Part V §2.1).

**Contestation must be able to change the outcome.** Part I §7.5 and Part II §8.3 give the design: every allocation as a structured argument whose premises are the rule, the priorities, the eligibility and the data; any affected party files an objection attacking a reason, a datum, a parameter or a principle's applicability; the objection is evaluated in the argumentation frameworks already held (Dung, ASPIC+) and the standing of each decision recomputed; the record shows when a contestation changed an outcome. An upheld contestation triggers review of the rule for everyone similarly placed, not only the individual case, which is what Robodebt's unappealed losses prevented (Part V §7.1). Revision is a recorded act whose effect on past and pending cases is computed (Part I §7.5).

**Lotteries as just procedures.** A lottery satisfies equal claims to an indivisible good by giving each a surrogate satisfaction, an equal chance, and "residual unfairness" remains (Broome); it is just because it keeps bad reasons out of the decision, its "sanitizing effect" (Stone), though Segall objects that it remains "a fair procedure that nevertheless results in an inevitable unfairness" (Part I §3, from the NDPR review). Weighted lotteries answer unequal claims (Saunders; Hirose). In practice lotteries are legitimate mostly as tie-breakers among options the procedure cannot distinguish, where they also make the limits of judgement honest: reviewers of NIH applications showed "no agreement" (Pier et al. 2018); the Health Research Council of New Zealand draws among eligible, screened proposals; the Swiss National Science Foundation draws among proposals whose credible intervals of rank cross the funding line (Part II §5; Part V §2.3). A randomised comparison of New Zealand's lottery winners found no clear effect of winning on publications or citations ([S], Part V §2.3).

### 1.7 In the City the Question Is Who Finally Gains, Which Needs Incidence Through Prices as Well as Where the Money Lands

**Five separable principles, and a dispute about distribution itself.** Spatial and urban justice theory gives a city-scale tool five principles rather than one objective: distribution by need (Harvey's territorial social justice, ordering need, then contribution to the common good, then merit, with the prospects of the least advantaged territory as great as possible; Fainstein's equity); recognition of groups and differences (Young, Fraser, Schlosberg); participation and voice (the right to the city; procedural environmental justice); sufficiency of access (Martens); and attention to the processes that produce spatial distributions (Soja; Harvey's later work) (Part III §1, content mostly from memory). The theories converge on a lexicographic structure (thresholds first, then distribution of the remainder by need, contribution or desert, with procedure and recognition as constraints on how the allocation is reached), but disagree whether distribution is the right frame at all: Moroni holds that justice applies to rules and institutions, not to spatial patterns, and Young and Purcell deny that a just city is an optimal distribution (Part III §1, Inferences). Southern critiques add that rules keyed to formal tenure or registration exclude "gray" populations by construction (Part III §1.3).

**Sufficiency of access.** Martens, Singer and Cohen-Zada moved transport equity from disparity to insufficiency: across 49 of the 50 largest US metropolitan areas, even with a threshold of 1% of the jobs reachable by car, nearly 11 million people below the poverty line lacked sufficient transit access, and at 10% so did 90% of them (A, Part III §1.6). The Accessibility Sufficiency Index is FGT-shaped, ASI_α = (1/N) Σ_{i: A_i < T} ((T − A_i)/T)^α (functional form from memory). Accessibility, not mobility, is the evaluand (Pereira, Schwanen and Banister, memory).

**Index-based targeting makes thresholds on composite scores.** England's IMD 2025 weights income and employment 22.5% each, education and health 13.5% each, and crime, barriers to housing and services, and living environment 9.3% each, over 33,755 areas, and warns that it is relative, not a time series, and that "many deprived people live in non-deprived areas" (T). CalEnviroScreen multiplies pollution burden by population characteristics, a deliberate non-compensatory form (A). The US CEJST flagged a tract at the 90th percentile on one indicator and the 65th on low income; it was removed from the federal web on 21 January 2025 and restored unofficially within 48 hours (A) (Part III §4). The critiques are documented: arbitrary cut-offs that put near-identical neighbours on either side; aggregation that hides cumulative burden; ranks that cannot show change; the ecological fallacy and the modifiable areal unit problem; rankings sensitive to construction (Tate; Rufat et al.; Spielman et al.); and data such as complaints that over-represent those who complain more (Part III §4.5). A tool should store an index as a declared scheme, recompute it under alternatives and report the share of areas whose eligibility changes; replace or accompany hard thresholds by continuous entitlements or a band decided by further evidence; pair area targeting with person-level entitlements and report the coverage of deprived persons, not only areas; and archive the index outside any one government's web presence (Part III §4, Inferences).

**Disaster funding inherits the distribution of property values.** Mitigation tested by avoided property damage favours places with higher property values: Richwood, West Virginia, failed to obtain funding because its low values kept its benefit–cost ratio down (A). FEMA's 2024 reforms (a 3.1% discount rate, income-adjusted replacement values) were partly reversed when OMB reinstated 7% on 8 April 2025 (T), and BRIC was cancelled in April 2025, ruled unlawfully cancelled in December 2025 and resumed on 25 March 2026 (A). Losses should be reported as shares of household income and wealth, with non-property benefits included (Part III §6).

**Who captures spending directed at places.** In spatial equilibrium, benefits directed at a place are partly capitalised into land and housing (going to owners, not renters), partly dissipated by in-migration, and partly offset by losses elsewhere when agglomeration is not strongly convex (Kline and Moretti; Glaeser and Gottlieb's "help poor people, not poor places"); yet location is an informative tag for redistribution, and place-based transfers can be efficient where non-employment is elastic or spillovers local (Gaubert, Kline and Yagan; Fajgelbaum and Gaubert) (Part III §3, memory). Within a metropolis, Tiebout sorting and fiscal zoning compound inequality; public goods are capitalised (Oates; Black); large air-quality improvements in Los Angeles left some renters worse off once prices and sorting responded (Sieg et al.); green investments are associated with gentrification, more for large regional parks than small neighbourhood ones; but new market-rate buildings in low-income areas lowered nearby rents by 5–7% (Asquith, Mast and Reed), so not all investment gentrifies (Part III §11, memory). The incidence of an amenity increment ΔA_j follows the Rosen–Roback logic (formula from memory):

Δr_j ≈ [(∂V/∂A)/(∂V/∂r)] · ε_mig/(ε_mig + ε_supply) · ΔA_j;  renters' net gain = WTP·ΔA_j − h Δr_j;  owners' gain = H_j Δr_j.

The infrastructure's urban-science `housing` module already computes equilibria of this kind (Part III §11). A tool should apportion each place-directed amount among residents' consumption, landowners' rents, in-migrants and other places, with ranges, and show the gap between a people-based and a place-based allocation as the price of geographic targeting (Part III §3, Inferences).

**First-round, propagated and capitalised incidence.** Part IV §5 adds the monetary side. The first recipients of new money gain first (Cantillon), but the evidence for monetary policy is mixed: quantitative easing appears equalising through employment in the euro area and disequalising through asset prices in Japan. Propagated incidence is traced by social accounting matrix multipliers M_a = (I − A_n)^{−1} and structural path analysis, which shows which paths an injection travels to reach each household group (formulas from memory). LM3 = (R1 + R2 + R3)/R1 measures local retention from a survey (£100k, £80k, £40k give 2.20), not benefit to the poor and not a counterfactual impact. "Who finally gains" therefore has three parts that a report must keep apart: who receives first, where the money then goes, and what prices do.

**Local justice for urban goods.** England's social housing must give reasonable preference to the homeless, those owed homelessness duties, those in insanitary, overcrowded or unsatisfactory housing, those who must move on medical or welfare grounds, and those who must move to a locality to avoid hardship (s.166A, T). Choice-based lettings raise match quality but shift units toward less needy applicants (Waldinger) and can reproduce segregation (Manley and van Ham). In school choice the Boston mechanism rewards sophistication, so strategy-proofness is a fairness property (Pathak and Sönmez). In facility location the objective encodes a theory of justice: the p-median is efficiency, the p-centre Rawlsian minimax, maximal covering sufficiency, and ordered weighted averages interpolate between them (Part III §7, memory).

**Equity scoring and its utilisation gap.** Gender budgeting is practised by 23 of 38 OECD members (2022), but only 9% said its information was always or often used in decisions (A); Oakland has made equity a heavily weighted criterion in capital programme scoring since 2018 ([S]). Both are scoring overlays on existing budgets, whose value lies in putting distributive effects in the same view as the decision (Part III §9).

### 1.8 Generative, Circular and Evolutionary Perspectives Supply a Direction, a Shape, Conditions and a Repertoire, Not a Verdict

The user's intuition that these traditions can help is borne out, but each supports some claims and not others. Table 1.2 states which.

**Table 1.2.** What each tradition can and cannot support in allocation.

| Tradition | What it supplies | What it can support | What it cannot support | Part |
|---|---|---|---|---|
| Generative justice (Eglash 2016) | "The universal right to generate unalienated value and directly participate in its benefits; the rights of value generators to create their own conditions of production; and the rights of communities of value generation to nurture self-sustaining paths for its circulation"; three kinds of value (labour, ecological, expressive) | a directional preference: reach generators directly, build self-sustaining loops, leave generators control over their conditions of production (itself a procedural right) | an operational definition of "unalienated", which Eglash concedes is missing; a floor for those who cannot generate (children, the ill, the old, uncounted carers); a sum across the three kinds of value without a declared commensuration | IV §1 |
| Circulation measured on ledgers | Sarafu: 99.7% of volume within five top-level modules, short cycles in excess over null models, PageRank correlating with final balances at R ≈ 0.57; Sardex: firms in cycles with higher turnover and less excess accumulation | that "circulation back" is measurable and associated with system health and member outcomes | that circulation is justice: a cycle can be extractive (debt service returns to a lender); the evidence is associational and about money, not unalienated value | IV §5 |
| Circular economy | a theory of material loops; social equity in 19% of 114 definitions (2017) and 24–27% of 221 (2022) | the ecological ledger (recycling, cycling) | a justice metric; its critiques (rebound, thermodynamic limits, depoliticisation, vagueness) apply | IV §2 |
| Doughnut, human needs, decent living | a social floor inside an ecological ceiling; 35 indicators, 2000–2022 (Fanning and Raworth 2025); no country meets basic needs at a globally sustainable level of resource use | the shape of a rule: thresholds and shortfalls, not a maximum; switching to fair shares of the ceiling | thresholds as scientific facts: they are declared, contested and partly unquantified, and downscaling to nations and cities is contested | IV §2 |
| Community wealth building, LM3 | anchor procurement, local retention (Preston: of about £750m, 5% stayed in the city and 39% in the county at baseline, [S]) | a measure of local re-spending | a counterfactual impact; who in the locality benefits; justice between places (the siloing and protectionism critique) | IV §2 |
| Regenerative economics (ecological network analysis) | ascendency A = Σ T_ij log(T_ij T_·· / (T_i· T_·j)), overhead Φ, capacity C = A + Φ; Finn's cycling index | structural descriptors of concentration against redundancy | welfare or justice; the "window of vitality" optimum, which Goerner et al. concede the data cannot fix | IV §2, §5 |
| Commons and polycentric governance (Ostrom) | eight design principles; 91 studies of 77 cases, every principle at least twice as often supported as not, congruence of costs and benefits (2B) strongly supported (Cox et al. 2010); rule templates: lot and daily rotation, turns along a canal, proportional to land in plenty and survival first in drought | procedural conditions under which allocation rules are accepted and kept; tested templates; nested envelopes allocated by nested units under their own rules; proportional equivalence of contributions and benefits, with need as a declared exception | which rule is right: the evidence concerns durability, and "perceptions of equity can vary with resource availability"; scaling beyond communities is doubtful, and power and history are under-represented | IV §3 |
| Evolutionary game theory of fairness | equal split as the largest basin (about 62% in divide-the-dollar, [S]) and stochastically stable; discriminatory norms metastable, their exit time exponential in memory and population; minorities disadvantaged by being fewer (cultural Red King); ultimatum offers from 26% to 58% across 15 small-scale societies | maps of where each rule leads under each declared population structure; persistence times; the local, deliberated character of fairness parameters | a certificate of fairness: stability is not justice, and no universal fairness constant exists | IV §4 |
| Complexity economics, adaptive management | non-equilibrium, path dependence; reforms as experiments; experimentalist governance | procedure over time: allocate, monitor, revise, with a declared monitoring design and revision rule | a steady state assumed; evidence that evolved or quality-diversity policies perform well in real budgets (none found) | IV §4 |
| Quality-diversity search | an archive of high-performing allocations that differ along declared descriptors (who is reached first, concentration of flows, circulation share, shortfall left, overshoot, proportional equivalence) | a set of options for deliberation, input to collective choice | a decision: the AI Economist's single trained objective and Democratic AI's majority objective are shown only as contrasts | IV §4, §7 |
| Money and multipliers | Cantillon effects; SAM multipliers and structural paths; local multipliers (Moretti 1.6, memory) | tracing first-round and propagated incidence in a case | Cantillon effects as a law; multipliers as counterfactual impacts (fixed coefficients, idle capacity, no price response) | IV §5 |

**How they combine with distributive and procedural justice** (Part IV §7). The floor implements sufficiency; priority to the worst off or leximin breaks ties among allocations that meet it; generative justice adds a direction (to generators, through loops); proportional equivalence adds desert by contribution, overridden by need under scarcity (the acequia switch). These are distinct principles and a tool should show where they agree and conflict, never fold them into one score. On the procedural side, Ostrom's collective-choice, monitoring, graduated-sanction, conflict-resolution and nesting principles, participatory budgeting's deliberated weights, and generative justice's own right of generators to set their conditions of production all govern who sets thresholds, weights and rules and how they are revised.

**A proposal that links generative justice to existing mathematics.** Part IV §6 proposes, and marks as its own inference, that generative justice's "circulation" has an exact counterpart in the decomposition of a flow on a network into a gradient part (driven, one-way, the topology of extraction), a curl part (local eddies round faces) and a harmonic part (global circulation round holes), as the economics domain already computes. The non-gradient fraction is a candidate circulation share, computable by kind of value and restricted to cycles through generator nodes. It must be read with labels of control and kind, never alone, and compared with Finn's cycling index, which is defined on throughflow-normalised transitions rather than a least-squares orthogonal decomposition. The return ratio ρ_g and extraction share ε_g of a generator (Part IV §1) are likewise proposals, not Eglash's.

### 1.9 Language Models May Organise, Articulate, Locate Disagreement and Draft, but Must Not Choose, Weigh or Speak for the Absent

**What they have been shown to do.** The Habermas Machine iteratively drafted group statements from participants' opinions and critiques; across 5,734 participants its statements were preferred to human mediators', rated more informative, clear and unbiased, incorporated dissenting voices while respecting the majority, and moved groups toward agreement, a result replicated in a virtual citizens' assembly (Tessler et al. 2024, abstract) (Part V §5.1). Its precursor showed that the social welfare function chosen over predicted agreement (utilitarian or Rawlsian) changes which statements win (Bakker et al. 2022, memory). Generative social choice combines proportional representation of statements with generation by a model; real-time rephrasing raised the perceived quality of political conversations without changing views (Part V §5.1). Reinforcement learning designed a redistribution mechanism that redressed initial wealth imbalance, sanctioned free riders and won a majority vote against human designs, but its objective was majority preference, so it inherits majority rule's indifference to minorities (Koster et al. 2022; Part IV §4; Part V §5.1).

**What is documented against them.**

- *Their distributive judgements diverge from humans' in both directions.* On allocation instances, four models more often returned envy-free solutions, tolerated large inequality, could not use money as a transferable resource to reduce inequality, and did better selecting from a menu than generating an allocation (Hosseini and Khanna 2025). Across many models and matched human scenarios they preferred stricter fairness constraints than humans, behaved more self-interestedly, were framing-sensitive and hard to align by fine-tuning (Han et al. 2026). Told to be fair, they prioritised collective welfare over equal distribution, while a mathematical objective without the word "fairness" aligned better (Cookson, Ebadian and Shah, [S]) (Part V §5.3).
- *They hold an implicit social policy.* Asked to allocate Spanish and German social budgets over twelve areas, four models under-allocated pensions by a factor near three and over-allocated housing about fourfold and employment about twofold relative to actual spending; only one was substantially sensitive to national context (Benavides Cantos and Garrido-Merchán 2026) (Part V §5.3).
- *Their moral biases are amplified and wording-dependent.* More altruism in collective-action problems, stronger omission bias, a bias toward answering "no" and decisions that flip with wording (Cheung, Maier and Lieder 2025); bias audits themselves can reverse with small changes in querying ([S]) (Part V §5.3).
- *As simulated stakeholders they misportray and flatten groups*, portraying them as outsiders imagine them and with too little within-group diversity (Wang, Morgenstern and Dickerson 2025; Bisbee et al. 2024) (Part V §5.2).
- *Sycophancy, persuasion, homogenisation, invention and unfaithful reasons.* Assistants tailor answers to users' stated beliefs; with personal information a model out-persuaded humans in debate (memory); co-writing with an opinionated model shifted users' attitudes largely unnoticed (memory); AI ideas made individual stories more creative but stories more alike; commercial legal research tools still invented in roughly a sixth to a third of queries (memory); chains of thought rationalised answers changed by biasing features without mentioning them (Turpin et al. 2023). **A model's stated reasons for a proposed allocation cannot be taken as the reasons that produced it** (Part V §5.4).
- *Errors propagate to the humans who use them* (the caseworker experiment, §1.6).

The critiques of the Habermas Machine are normative rather than empirical refutations: optimising for agreement misreads Habermas, for whom agreement is a regulative ideal; private interaction with a machine is not deliberation; it may create an illusion of consensus (Part V §5.1, [S]).

**The roles, ordered by risk** (Part V §5, Inferences; §7.2):

1. organising what people said, with links back to each person's words;
2. articulating each stakeholder's claim in the strongest form the claimant endorses, checked by the claimant;
3. finding what a disagreement turns on (the open term, the disputed fact, the weight), as a proposal for people to confirm;
4. proposing candidate principles or allocation rules, shown alongside standard rules;
5. drafting reasons for a decision people have already taken, checked against the declared criteria and evidence; and translating and simplifying explanations for affected people, checked by a person.

A model must not choose or rank allocations, set or suggest numeric weights as defaults, stand in for absent stakeholders, grade the merit of claims, produce official reasons without human authorship, merge dissent into a consensus statement without keeping dissent visible, or persuade participants toward a position. Every output is marked as proposed, with the model, version and prompt kept; every factual claim carries provenance with citations checked; any model-proposed allocation is shown with a sensitivity check across rephrasings and never alone; participants are told that text was machine-drafted; and the effect of suggestions on human accuracy is evaluated with the actual users. In the infrastructure's practice a model's proposal is marked as such and never alters a computed verdict (Part III §13.4; Part IV §7). In allocation the model should surface conflicts, not dissolve them: a statement everyone can sign may hide the distributive choice (Part V §5, Inferences).

*Remark.* No study was found of a language model used inside a real public allocation process with measured effects on outcomes or legitimacy, nor of model-drafted against human-drafted reasons for allocation decisions (Part V §5, Gaps). The roles above are inferences from adjacent evidence.

### 1.10 Every Allocation Should Report Who Receives, Who Is Excluded, Who Pays and Who Finally Gains, Under Which Principles, How Sensitively and by What Procedure

The five Parts' reporting requirements (Part I §7.5; Part II §10.3; Part III §13.2; Part IV §7; Part V §6–7.3) combine into one report per allocation round.

1. **The pool.** Who was eligible, how the pool was formed, and who is missing from it, since most measured injustice lives in the pool before the declared rule runs (Part V §2, Inferences). Exclusion errors are counted against an external benchmark, or the report says that none exists: those excluded are absent from the programme's data. Proxy means tests have left out large shares of their intended recipients (one scheme 96%, [S]), and phone-data targeting in Togo reduced exclusion errors by 4–21% against geographic targeting but raised them by 9–35% against a hypothetical comprehensive registry (Part V §2.5).
2. **The declared scheme.** The currency; the admissible principles 𝒫; every parameter (τ, γ, η, the typing, the categories and their sizes, the order of processing, the weights, the lottery chances, the discount rate); each proxy with its validation and known differences between groups; and for each, who set it, when, under what mandate, from what source, in which version (Part I §7.5; Part V §7.1).
3. **Who receives.** The allocation per unit and per capita by area and by group (income decile, tenure, age, disability, gender, ethnicity where lawful and available, and the groups affected parties name themselves); Lorenz curves; Gini, Theil and Atkinson indices at several ε; the share reaching the worst-off decile; the people below each declared threshold before and after; the coverage of deprived *persons*, not only of deprived areas (Part III §13.2; Part V §6).
4. **Who pays.** The funding source (local taxes by decile, reallocation from other areas or services, borrowing) and the opportunity cost, what the next-best use would have bought, so that gains are net (Part III §13.2; Part V §4.1).
5. **Who finally gains.** First-round recipients; propagated incidence (multipliers and structural paths); incidence through prices: expected rent and price changes by tenure, gains to owners against renters, the share of the allocation's value expected to accrue to landowners, displacement risk and in-migration, each with ranges (Part III §3, §11; Part IV §5).
6. **The envelope.** The part of each allocation due to floors, caps, guarantees and phasing, reported separately from the formula (Part III §2, §13.2).
7. **Properties as verdicts.** Each axiom or fairness notion either holds, with its witness (a cutoff vector, an envy check, a satisfied cohesive group), or fails, with a counterexample; where demands are jointly unsatisfiable, the theorem is stated (Part I §7.2; Part II §10.3).
8. **Comparisons.** The allocation under standard alternatives side by side: all claims rules with the Lorenz order among them; every precedence order of a reserve system; greedy, Equal Shares and a need-weighted envelope for a participatory budget; maximin and utilitarian benchmarks. The maximal set M(S) under 𝒫 with its pivots, the switching values of γ, ε, η and τ, and rank-acceptability indices over admissible weights (Part I §7.2; Part II §10.3; Part V §7.1).
9. **Sensitivity.** The share of units whose eligibility or rank changes under alternative indices, weights, thresholds, units of area (the modifiable areal unit problem), η and discount rates, with rank-uncertainty intervals (Part III §4, §13.2).
10. **The procedural record.** For each unit the reasons that bore on it, written so that a non-expert can understand and challenge them, and which rule step decided; the options in M(S) not chosen, and why; who decided and who was consulted, with conflicts of interest; participation against the population; appeals lodged and upheld, and the revisions they caused; every discretionary deviation with its reason; the committed lottery seed and the losers recorded as bearing residual unfairness; acceptance or satisfaction with the caveat that it tracks outcomes; every run sealed with its inputs and parameters (Part I §7.3, §7.5; Part III §13.2; Part V §6, §7.1).
11. **Proposals.** Anything a model contributed, marked as proposed, with model, version and prompt (Part V §7.2).

*Remark.* Items 2, 7, 8 and 10 have homes in what the infrastructure already holds: the reading of open terms under every reading for pivots, kybernesis's dual verdicts and sealed record, archeion's journal, apodeixis's argumentation and kategoria's verdict format (Part I §7; Part II §10.3; Part III §13; Part V §7, Inferences). The gap all five Parts identify is the loop from reasons to contestation to revision, and the honest reporting of exclusion: no surveyed tool closes that loop (Part V §4, Inferences).

### 1.11 The Parts Should Be Built in an Order Set by Reuse, Closed Forms and the Record

Parts II §10.3, III §13.1, IV §7 and V §7 each propose an order. Merged, with what each step reuses:

1. **The declared scheme and the record.** Schemes as data (principles, parameters, provenance, versions), reasons per unit, sealed runs, and the contestation-and-revision loop as arguments on premises. Every later step writes into it (Part I §7.5; Part V §7). *Reuses* archeion's journal, kybernesis's record and dual verdicts, apodeixis's Dung and ASPIC+ frameworks.
2. **Claims rules with their axioms as verdicts.** PRO, CEA, CEL, Talmud, Piniles, RA, AP, the TAL family, Young's parametric family given f, duality, truncation and minimal-rights operators, weighted and priority-class versions, multi-issue, bipartite and indivisible extensions, all in exact rationals (Part II §10.3). *Reuses* nomos's Talmud rule, Shapley value and nucleolus as independent cross-checks.
3. **Thresholds.** Shortfall and FGT-type measures, the floor under a ceiling with lexicographic minimisation, the acequia switch, sufficiency of access (Part III §13.1; Part IV §7). *Reuses* the needs domain's declared standards, nomos's accessibility (2SFCA, cumulative opportunities), industrial ecology's footprints for ceilings.
4. **Priority and reserve systems.** Sequential reserve matching with a precedence order, cutoff equilibria and their extreme cutoffs, the smart-reserve family, India's 2SMG as a second instance, soft and hard reserves, minimum guarantees and over-and-above as declared options, outcomes over every order (Part II §10.3). *Reuses* nomos's deferred acceptance and top trading cycles, calx's Hungarian method, morphe's flows.
5. **Sequential and random assignment, and lotteries.** Serial and random serial dictatorship (exact probabilities only for small n, with the #P warning), probabilistic serial and its extension by flows, picking sequences, Birkhoff–von Neumann decomposition in rationals, the dichotomous egalitarian matching; weighted lotteries from claims rules applied to chances; threshold-then-lottery procedures with the band set by uncertainty (credible intervals of rank) and the seed committed (Part II §10.3; Part V §7.1). *Reuses* tyche's Bayesian tools for the band.
6. **Dominance and the admissible-set partial order.** Lorenz, generalised Lorenz and sequential (needs-ordered) dominance; the Atkinson, Kolm and generalised-Gini families with the parameter at which comparisons flip; ≽_𝒫 and M(S) with pivots; SMAA rank acceptabilities and robust ordinal regression as the distribution and order views (Part I §7.1; Part II §10.3). *Reuses* nomos's inequality measures, tyche's stochastic dominance and decisions under ambiguity.
7. **Fair division of indivisible goods.** EF, EF1, EFX, PROP and MMS checkers, round-robin, envy-cycle, maximum Nash welfare exact for small instances and flagged as an optimisation, leximin for dichotomous settings, weighted versions (Part II §10.3). *Reuses* nomos's Adjusted Winner for two agents.
8. **Places and budgets.** Needs-minus-capacity equalisation with the envelope itemised (England's settlement as the worked example); Porto Alegre's banded points with the history of criteria dropped; participatory budgeting by greedy selection, Equal Shares with a declared completion and sequential Phragmén, with EJR and PJR checkers and a Pabulib reader; need-weighted envelopes with proportional voting inside; equity-weighted appraisal over a range of η with switching values and no pass-or-fail ratio thresholds; facility siting under median, centre, covering and ordered-weighted objectives side by side (Part II §10.3; Part III §13.1). *Reuses* calx's knapsack and greedy submodular maximisation for the utilitarian baselines, nomos's appraisal.
9. **Incidence.** Who pays; capitalisation, sorting and displacement by tenure; multipliers and structural paths by household group; first-recipient incidence; losses as shares of income and wealth (Part III §6, §11; Part IV §7). *Reuses* urban science's `housing` equilibria, economics' input–output tables, markets' account of money and the order of steps, the risk domains' layers of loss.
10. **Flows, circulation and rules over time.** Value-flow ledgers by generator and kind of value, never summed without a declared commensuration; the circulation share from the flow decomposition beside Finn's cycling index, as structural descriptors, not welfare; agent-based and evolutionary models of rules with population structure declared, reported as basins and persistence times; a quality-diversity archive of allocations as input to collective choice (Part IV §7). *Reuses* economics' decomposition of flows, nomos's agents, kinesis, heuresis's quality-diversity archive, the policy domain's value of information for adaptive revision.
11. **Language models at the edges.** Organising, articulating, locating disagreement and drafting, as proposals under the controls of §1.9 (Part V §7.2).

*Rationale.* The order puts first what has closed forms and published values (claims rules, reserves, dominance), so that each later, less determinate step stands on verified parts, and puts the record before all of them because every step's output is only as contestable as its record.

### 1.12 Test Cases with Closed Forms and Published Values

**Table 1.3.** Test cases the Parts supply. "Memory" marks values to be confirmed against the source before they become fixtures.

| Case | Expected value or property | Part |
|---|---|---|
| Talmud rule, claims (100, 200, 300) | estate 100 → (33⅓, 33⅓, 33⅓); 200 → (50, 75, 75); 300 → (50, 100, 150) (memory, standard) | II §1, §10.2 |
| Contested garment, claims (1, ½), estate 1 | (¾, ¼) (memory, standard) | II §1 |
| Talmud and random arrival as game solutions | Talmud = nucleolus and RA = Shapley value of v(S) = max(0, E − Σ_{j∉S} c_j); two independent computations of one number | II §1.3, §10.2 |
| Axioms of the claims rules | consistency fails for RA; composition up and down fail for Talmud; PRO, Talmud, RA and AP self-dual; Rᵈᵈ = R; CEA Lorenz-maximal and CEL Lorenz-minimal among rules; Young's representation equals the closed form; the axiom table regenerated rather than asserted | II §1, §10.3 |
| Reserve systems | eligibility, non-wastefulness and respect of priorities on every output; Theorem 1 (a supporting cutoff exists, and conversely); Proposition 1 by brute force on small instances; Proposition 3; Example 1 (six units, seven patients, matched sets {i1, i2, i3, i4, i5, i7} and {i1, i2, i3, i4, i5, i6}); Example A.2 (open-first wastes the reserved unit); Proposition 5 and Theorem A.2 | II §2, §10.2 (text) |
| Random assignment | probabilistic serial sd-efficient and sd-envy-free; RSD ex post efficient; BvN components feasible and summing back exactly; TTC from random endowments reproduces RSD | II §3, §10.3 |
| Fair division | round-robin EF1 for additive valuations; maximum Nash welfare EF1 and Pareto optimal (brute force); EFX exists for n ≤ 3 (brute force); 2/3-MMS achievable on random instances; the published MMS and "EFX need not exist" counterexamples (still to be extracted) | II §4, §10.3 |
| Equal Shares | equalshares.net: €1,100, 11 voters at €100, winners bike path (€700), playground (€200), library (€100), €100 unspent; EJR verified by enumerating cohesive groups; not exhaustive; compared with pabutools on Pabulib instances with their declared rules | II §6, §10.2 |
| Welfare dominance | generalised Lorenz dominance implies higher EDE for every Atkinson ε ≥ 0 and every generalised-Gini weight vector; at equal means Lorenz dominance is equivalent to a sequence of Pigou–Dalton transfers | II §7, §10.3 |
| Porto Alegre, 1997 paving (20 km) | Extremo Sul 26 points of 262 → 9.9% (1,985 m); Centro 11 → 4.2% (840 m); 26/262 = 0.0992, 11/262 = 0.0420 | III §8.1 (text) |
| England's Fair Funding Assessment | needs formulas × Area Cost Adjustment − notional Band D (£2,060, £2,160, £2,265) × taxbase, phased in thirds, floors of 100% and 95% | III §2.5 (text) |
| Germany's equalisation | Z_i = 0.63 (AMZ_i − FKZ_i) with city-state weight 1.35 and municipal taxes at 75% | III §2.4 ([S]) |
| Distributional weights | Circular A-4's η = 1.4 gives weight 2^{1.4} ≈ 2.64 to a group at half the median; switching values over η ∈ {0, 1, 1.3, 1.4, 1.6, 2} | III §5 |
| Algorithmic fairness | Chouldechova's identity FPR = [p/(1−p)]·[(1−PPV)/PPV]·(1−FNR); equal PPV with unequal base rates forces unequal error rates | II §8 |
| Acequia switch | proportional to entitlement when S ≥ Σ need_i; survival floors first, then pro rata, when S < Σ need_i | IV §3 |
| LM3 | (£100k + £80k + £40k)/£100k = 2.20 | IV §2 |
| Ecological network analysis | C = A + Φ on any flow table; robustness −a log a maximal at a = 1/e ([formula M]); Finn's cycling index from N = (I − G)^{−1} ([formula M]) | IV §2, §5 |
| Evolution of division norms | random initial populations of divide-the-dollar reach equal split about 62% of the time ([S]); equity norm stochastically stable, discriminatory norms metastable with exit time growing exponentially in memory and population | IV §4 |

### 1.13 Honest Demonstrations Need Open Records Kept under Their Terms

**Table 1.4.** Open records the Parts name, with their terms as found.

| Record | Content | Terms | Use | Part |
|---|---|---|---|---|
| English indices of deprivation 2025 (MHCLG) | 33,755 LSOAs: ranks, scores, domains | Open Government Licence v3.0 ([V], page) | an index recomputed under alternative weights; a ward-level grant across a borough | III §4.1, §12 |
| Local government finance settlement 2026–27 to 2028–29 (MHCLG) | allocations and needs calculators | GOV.UK; OGL presumed, to be confirmed on the dataset pages | the needs-minus-resources structure as a worked example | III §2.5, §12 |
| Pabulib | 2,210 participatory budgeting instances in `.pb`, vote-level (Warsaw and other Polish cities, Amsterdam, Budapest) | no licence stated; it reserves the right to remove personally identifiable voter information | replay under greedy, Equal Shares and need-weighted envelopes; keep raw files in the cache, publish derived results with attribution, ask the maintainers for terms | II §6; III §8.4, §12; V §2.1, §7.4 |
| equalshares.net | per-city results with explanations of why each project won or lost (Wieliczka, Aarau, Świecie) | not checked | cases of Equal Shares | II §6; III §12; V §7.4 |
| Pol.is open data | vTaiwan's Uber consultation, a Brexit consensus conversation, Canadian electoral reform, Seattle's $15 minimum wage, New Zealand biodiversity | CC BY 4.0, attributed to The Computational Democracy Project | opinion mapping as a view | V §4.4, §7.4 |
| Eurostat regional data (NUTS) | GDP, poverty, employment, population | reuse with attribution under Decision 2011/833/EU, with exceptions; adaptations must be stated | threshold effects of the 75% cohesion rule | III §12 |
| CEJST 1.0 (2022) | tract-level designations | a US government work (public domain in the US, memory); removed in January 2025, kept by the Public Environmental Data Project and university mirrors | cite as archived and dated; not current | III §4.3, §12 |
| CalEnviroScreen 4.0 | tract scores and indicators | not verified on OEHHA's page | a non-compensatory index | III §4.2, §12 |
| Area Deprivation Index | block-group ranks | internal non-profit educational, research and public-health use; no redistribution, no commercial use ([V], terms) | **not** for a redistributed public demonstration | III §12 |
| Paris participatory budget | winning projects and their delivery | Licence Ouverte or ODbL, by dataset, to be checked | a budget with a reserved share for working-class neighbourhoods (memory) | III §8.3, §12 |
| Australia's GST relativities; Canada's equalization; Germany's Finanzausgleich | relativities, parameters, payments | not checked (Open Government Licence – Canada applies to much federal data, memory) | equalisation cases | III §12 |
| FEMA benefit–cost pages; OpenFEMA | policies; project data | pages [V]; data [M] | disaster funding by property value | III §6, §12 |
| Sarafu Community Inclusion Currency 2020–2021 | 421,329 transactions among 40,657 accounts | terms not stated in the notes; ledgers are privacy-restricted in general | circulation on a ledger | IV §5, §6 |
| OPTN and SRTR | allocation policy documents; public data | rules public; standard analysis files need an application and a data use agreement (memory) | rules, not individual data | V §7.4 |
| NICE appraisal documents; Norway's criteria | committee papers with reasons; published criteria | not stated | priority setting with published reasons | V §7.4 |
| HRC New Zealand's Explorer Grants; SNSF's Bayesian ranking | procedures; Heyard et al.'s code and data if released | not checked | threshold-then-lottery | V §2.3, §7.4 |
| Togo's Novissi targeting study | comparison of targeting regimes | replication materials not checked | exclusion errors against a benchmark | V §2.5, §7.4 |
| Official reports on Robodebt, the childcare benefits affair and the 2020 grades | findings | public reports | negative demonstrations | V §3, §7.4 |

*Remark.* Melbourne, the infrastructure's habitual case, can supply access and needs layers already held, but whether the city publishes its budget or capital works programme as open data was not verified; no Japanese city is proposed (Part III §12–13).

**Table 1.5.** Software the Parts checked, with licences.

| Software | Content | Licence | Use under MIT | Part |
|---|---|---|---|---|
| ClaimsProblems (R) | claims rules | GPL-3 | oracle for fixtures only | II §1 |
| GameTheory (R); CoopGame (R) | bankruptcy rules, nucleolus; cooperative games | GPL (≥ 2); GPL-2 | oracle only | II §1 |
| pabutools | participatory budgeting rules and analysis | GPL-3.0 (or later) | oracle only | II §6; V §4.2 |
| fairpy (erelsgl) | fair division algorithms | GPL-3.0 (the PyPI package `fairpy`, MIT, is an unrelated library); fairpyx declares no licence | oracle only | II §4 |
| abcvoting; pref_voting; matching | approval-based committees, Rule X; preference voting; matching games | MIT | adoptable | II §4, §6 |
| matchingR; matchingMarkets (R) | matching | GPL (≥ 2) | oracle only | II §4 |
| smaa; hitandrun (R) | SMAA; sampling weights from polytopes | GPL-3 | oracle only | II §9 |
| ineq (R) | Lorenz curves, inequality and poverty measures | GPL-2 or GPL-3 | oracle only | II §7 |
| fairlearn; AIF360 | algorithmic fairness metrics | MIT; Apache-2.0 | adoptable | II §8 |
| Decidim; Consul Democracy; Pol.is | participatory platforms | AGPL-3.0 | interoperate by data exchange, not incorporation | V §4.2, §4.4 |
| Habermas Machine; Talk to the City reports; Jigsaw sensemaking tools | model-mediated deliberation and summaries | Apache-2.0 | permissive | V §4.4 |
| Sortition Foundation's stratification app; citizens' assemblies replication code | fair selection of panels | GPL-3.0 | oracle only | V §4.4 |
| Spliddit; Stanford PB platform; EVIDEM; JSMAA | fair division service; ballots; MCDA framework | not found or not verified | none until checked | II §4, §10.4; V §4 |

### 1.14 What a Tool Must Never Claim

The five Parts' prohibitions share one form: presenting a conditional verdict ("under principles P with parameters θ, option x is maximal") as an unconditional one ("x is just") (Part I §6). Gathered:

1. That an allocation is just, fair, optimal or *the* just allocation. It may say "maximal under the admissible principles P with parameters θ" or "first under rule r" (Part I §7.7; Part III §13.4; Part IV §7; Part V §7.5).
2. That a weighted sum of principles, or a default γ, τ, η, weight, threshold or discount rate, is neutral, or that a parameter is an institution's or jurisdiction's own unless a scheme states and sources it; parameters change with governments (Circular A-4's rescission, FEMA's discount rate, CEJST's withdrawal, Australia's 2018 rules under review) (Part I §7.7; Part III §13.4; Part V §7.5).
3. That a fair procedure makes an outcome just where an independent criterion exists; that procedural safeguards will make those who lose accept the outcome; that those affected were heard when their objections could not change the outcome; that participation guarantees justice or welfare effects (Part I §7.7; Part III §13.4; Part V §7.5).
4. That a lottery leaves no unfairness (Part I §7.7).
5. That measured effort shows responsibility, that a merit score is desert, or that one kind of need, one theory or one number settles the question (Part I §7.7).
6. That fairness criteria have all been met when they are known to be jointly unsatisfiable (Part I §7.7; Part II §8).
7. That an area index measures a person's need, that a threshold is natural, or that a proxy is neutral without a validation by group (Part III §13.4; Part V §7.5).
8. That benefits accrue to the residents of a place without an incidence estimate, or that unweighted benefit–cost analysis is neutral (Part III §13.4).
9. That exclusion errors are low when the excluded are absent from its data (Part V §7.5).
10. That human review makes a system safe (Part V §7.5).
11. That evolutionary stability, quality-diversity elitism, multiplier size, local retention or any circulation, cycling or retention measure (LM3, Finn's index, a Hodge share, cycle density) measures justice, welfare, unalienated value or benefit to generators; that local retention is good in itself; that LM3 or model multipliers are counterfactual impacts; that the Cantillon effect is a law; that the "window of vitality" optimum is known for economies (Part IV §7).
12. That social floors or downscaled ecological boundaries are scientific facts, that fairness norms are universal, or that labour, ecological and expressive value can be summed without a declared, contestable commensuration (Part IV §7).
13. That a model's proposal is a finding or evidence, that a model's drafted reasons reflect its actual reasoning, or that it represents the views of groups it simulates (Part III §13.4; Part IV §7; Part V §7.5).

### 1.15 What the Survey Leaves Open

- **No precedent for the whole.** No deployed public allocation tool was found that publishes reasons per unit, takes contestation of specific reasons, revises the rule systemically and reports distributional outcomes (Part V §7, Gaps); no existing software implements partial orders over allocations with pivots and contestation (Part I §7, Gaps); no source unifies axiomatic rules, dominance and SMAA (Part II §9). The design is novel and should be evaluated as such.
- **Sequences.** No treatment was found of the justice of sequences of allocations as such (Part I §4), nor a computable treatment of supersession or reparations beyond reservations (Part I §5).
- **Procedure against distribution.** No agreed metric weighs a less equal allocation reached with more voice against a more equal one reached with less; the tool can only report both (Part III §13). No meta-analysis specific to procedural fairness and the acceptance of allocation decisions was found (Part V §3).
- **Generative justice.** No operational metric of unalienated value exists; the proposed return ratio, extraction share and circulation share are untested; no independent critique of the framework was found; how its right of generators to set their conditions of production should constrain a tool's governance must be decided with the user (Part IV §1, §7).
- **Evidence of outcomes.** The welfare effects of participatory budgeting are strongest in urban Brazil and thin elsewhere; Equal Shares has no independent evaluation; whether gender or equity budgeting changed allocations was not established; no study measures the incidence of intra-city allocations or the capitalisation of a whole municipal capital programme by tenure (Part III §3, §8, §9, §11; Part V §2).
- **Language models in real allocation.** No study of a model inside a real public allocation process with measured effects (Part V §5).
- **Unread texts and unverified formulas.** Elster's taxonomy, Walzer's blocked exchanges, Nussbaum's list, Adler's ratio-scale claim, Roemer's objective, Fleurbaey and Tungodden's axioms, the theorem numbers of the claims literature, Martens' index, the IMD's transformation, Ostrom's wording of the principles, the 62% basin figure, and the 2026 EFX instances (Parts I–IV, Gaps).
- **Licences.** Pabulib's data, equalshares.net, CalEnviroScreen, the official equalisation datasets, Heyard et al.'s materials and the Novissi replication data (Parts II, III, V).

## Part I. Theories of Distributive and Procedural Justice

*Remark.* Part I's scope and conventions. Scope: the distributive and procedural theories of justice that bear on allocating scarce resources (their currencies, patterns, structures and prohibitions), pluralism and the alternatives to maximisation, procedural justice and lotteries, allocation in time and sequence, non-ideal theory, redress and participation, and what can be computed, what people must decide and what a tool must never claim. The bibliographic markers are those of §0.3, with one more: **[V-reprint]** means that only a later reprint or edition was found in Crossref, so that the original's details are from secondary knowledge ("VR" in the bibliography at the end of §7 means the same). Statements of content say where they come from: *content: SEP* (an encyclopedia entry fetched this session); *content: abstract/page* (a publisher page, abstract or review fetched this session); *content: memory* (knowledge of the text, not re-read this session, to be checked against the text before it is relied on). Sources fetched in full or in part: the SEP entries *Distributive Justice* (first published 1996, substantively revised 26 Sep 2017) and *Incommensurable Values* (Andersson and Hsieh, first published 2007, substantively revised 26 Jun 2025); the Russell Sage Foundation's page for Elster's *Local Justice*; Segall's NDPR review of Stone's *The Luck of the Draw*; search results on accountability for reasonableness and on recent books. Inside this Part, a bare section reference (for example "§2") means this Part's own section.

*Caveat.* No primary book text was read in full for this Part. Its formal statements (leximin, Hammond equity, Harsanyi's theorem, Roemer's objective, Arrow, Fleurbaey–Tungodden) are from memory unless marked otherwise.

### 1. Distributive Theories and Their Operational Content

#### Takeaway
- **What each theory specifies.** Each distributive theory fixes four things:
  - a *currency* (what is distributed or assessed: primary goods, welfare, resources, opportunities, capabilities, standing);
  - a *pattern or principle*: maximise a sum, give priority to the worse off, secure a threshold, neutralise brute luck, match goods to spheres, or attend to history;
  - a *structure*: lexical, aggregative, threshold-based or constraint-based;
  - what it *forbids*.
- **What can be checked by machine.** Many are machine-checkable once their free parameters are fixed, among them leximin, the prioritarian sum Σg(u), poverty gaps below a threshold, envy-freeness and Roemer's objective.
- **What stays with people.** Those parameters (the index, the threshold, the concavity, the line between circumstance and choice, which good belongs to which sphere) are themselves the contested moral content, and must stay with people.
- **Plural theories.** The plural theories (Walzer, Sen 2009, Elster, Young and Fraser, care ethics) deny that one currency and one pattern govern all goods. Their operational content is structural: separate principles per good, comparisons that may be partial, and attention to relations and procedures.

#### Cited Findings

##### 1.1 Rawls: two principles in lexical order, the difference principle, primary goods

- **The principles.** Rawls's first principle secures equal basic liberties, with the political liberties given their "fair value". The second has two parts:
  - (a) offices and positions open under "fair equality of opportunity";
  - (b) inequalities to the "greatest benefit of the least advantaged members of society".

  Content: SEP.
- **Lexical order.** Principle 1 has lexical priority over Principle 2, and 2(a) over 2(b): liberties may not be traded for economic gains, even for the worst off. Sources: [SEP, Distributive Justice](https://plato.stanford.edu/entries/justice-distributive/) [V]; [Rawls 1971](https://doi.org/10.4159/9780674042605) [V]; [Rawls 1999, rev. ed.](https://doi.org/10.4159/9780674042582) [V]. Content: SEP.
- **Primary goods** are the currency, assessed by an index for the representative least-advantaged person. The goods are:
  - basic rights and liberties;
  - freedom of movement and free choice of occupation;
  - the powers and prerogatives of offices;
  - income and wealth;
  - the social bases of self-respect.

  Content: memory, from *Justice as Fairness: A Restatement* (2001) [M]. The SEP notes Dworkin's objection that a primary-goods metric misses the greater needs of the severely ill or disabled. Content: SEP.
- **Formal shape.** The difference principle is applied to groups, not individuals. Its standard formal extension is **leximin**: sort the vectors ascending; x ≻ y iff, at the first rank k where x₍ₖ₎ ≠ y₍ₖ₎, x₍ₖ₎ > y₍ₖ₎.
  - Leximin satisfies strong Pareto and anonymity.
  - It was characterised axiomatically in the 1970s (Hammond 1976; d'Aspremont and Gevers 1977 [M]) by Pareto, anonymity, separability (independence of unconcerned individuals) and *Hammond equity*: if u_j < u'_j < u'_i < u_i, with all others equal, then u' ≽ u.

  Content: memory.
- **Maximin as a rule of choice.** Maximin, the rule of choice under ignorance in the original position, is distinct from the difference principle as a distributive principle. Content: memory.
  - [Harsanyi 1975](https://doi.org/10.2307/1959090) [V] argued that maximin is irrational under uncertainty: it implies extreme risk aversion and lets the worst-off's tiny gains override everyone's large ones. He defended the average-utility principle from behind a veil (content: memory).
  - [Arrow 1973](https://doi.org/10.2307/2025006) [V] raised the problem of indexing primary goods and the extremity of maximin (content: memory).
- **Incentives.** [Cohen 2008](https://doi.org/10.4159/9780674029651) [V] argues that incentive-based inequalities are justified only when strictly necessary given others' choices, not by talented people's demands. He grounds this in an "egalitarian ethos". Content: SEP.
- **Scope.** Rawls's principles govern the *basic structure* of society, not local allocations by institutions. Elster's *Local Justice* is explicitly about the latter. Content: memory.
- **Operational form.**
  - Basic liberties become side-constraints that no allocation may breach.
  - Fair equality of opportunity becomes a constraint on access to positions.
  - The difference principle becomes leximin over an index for groups.
  - The construction of the index (weights across primary goods) is not supplied by the theory and is a value judgement.

##### 1.2 Utilitarianism and welfarism: maximise the sum of welfare

- **Currency.** Welfare: pleasure (Bentham), happiness (Mill), or preference satisfaction (modern accounts); expected utility under uncertainty. Content: SEP.
- **Criticisms recorded by the SEP** (content: SEP):
  - the separateness of persons (Rawls): sacrifice by some for others' net gain lacks a single experiencing subject;
  - offensive (racist, sexist) preferences count equally;
  - interpersonal comparisons may be impossible;
  - the information it requires is vast.
- **Harsanyi's aggregation theorem** ([Harsanyi 1955](https://doi.org/10.1086/257678) [V]). If individual and social preferences over lotteries both satisfy the von Neumann–Morgenstern axioms, and society is indifferent whenever all individuals are, then social utility is an affine combination of individual utilities: W = Σ aᵢuᵢ. Content: memory.
- **Diamond's objection** ([Diamond 1967](https://doi.org/10.1086/259353) [V]). Society may strictly prefer a lottery that gives two people equal chances of a good over a lottery that always gives it to the same person, although the two are equal in expected utility. This breaks the sure-thing principle, and it is the formal seed of the justification of lotteries in allocation. Content: memory.
- **Welfarism** is the restriction that social rankings depend only on individual utilities. Sen's critique is that this ignores rights, freedoms and how utilities arise: adaptive preferences, and expensive tastes (with Dworkin). Content: memory; [Sen 2009](https://doi.org/10.4159/9780674054578) [V].
- **In health economics.** The debate between welfarism and extra-welfarism (QALYs and capabilities rather than utility) is surveyed by [Brouwer, Culyer, van Exel and Rutten 2008](https://doi.org/10.1016/j.jhealeco.2007.07.003) [V]. Content: memory.
- **Operational form.** Cost–benefit and cost–effectiveness analysis maximise Σuᵢ or Σ QALYs. These are fully computable given a metric, but blind to distribution ("tyranny of aggregation", §2).

##### 1.3 Prioritarianism: benefits matter more the worse off their recipient

- **Parfit.** [Parfit 1997](https://doi.org/10.1111/1467-9329.00041) [V] separates *telic egalitarianism* (inequality is bad in itself) from the *priority view* (a benefit has more moral value the worse off the person, in absolute terms).
  - **The levelling-down objection** is the main argument against telic equality. Making the better off worse, with no gain to anyone, would be in one respect better on the telic view.
  - The priority view escapes it because it satisfies Pareto.

  Content: memory. On the earlier and later debate: [McKerlie 1994](https://doi.org/10.1017/s0953820800001308) [V]; [Tungodden 2003](https://doi.org/10.1017/s0266267103001007) [V]; [Temkin 1993](https://doi.org/10.1093/oso/9780195078602.001.0001) [V]; [Holtug 2010](https://doi.org/10.1093/acprof:oso/9780199580170.001.0001) [V].
- **Formal form** ([Adler 2012](https://doi.org/10.1093/acprof:oso/9780195384994.001.0001) [V]; [Adler 2019](https://doi.org/10.1093/oso/9780190643027.001.0001) [V]): W(u) = Σᵢ g(uᵢ), with g strictly increasing and strictly concave. Content: memory.
  - **The Atkinson family** is g(u) = u^(1−γ)/(1−γ) for γ ≥ 0, γ ≠ 1, and ln u at γ = 1. Here γ = 0 is utilitarian, and γ → ∞ approaches leximin.
  - **Properties.** It satisfies strong Pareto, anonymity, separability and the Pigou–Dalton transfer principle.
  - **What it requires of measurement.** Adler argues that a prioritarian social welfare function needs well-being measured on a ratio scale (a meaningful zero), since rankings change with the zero point.
  - **Ex ante against ex post.** Adler favours *ex post* prioritarianism (apply g to realised well-being, then take expectations). It can violate ex ante Pareto, whereas ex ante prioritarianism violates other conditions.
- **Against the priority view.** [Otsuka and Voorhoeve 2009](https://doi.org/10.1111/j.1088-4963.2009.01154.x) [V] argue that it wrongly gives priority *within one person's life under risk*. What matters is that some are worse off *than others*, a claim that is comparative between persons. Content: memory.
  - [Voorhoeve 2014](https://doi.org/10.1086/677022) [V] develops "competing claims" with partial aggregation of relevant claims. Content: memory.
  - A 2026 *Utilitas* article continues this line for possible people: [O'Brien 2026](https://doi.org/10.1017/s0953820826100375) [V]. Content: title only.
- **Whether the distinction matters.** [Fleurbaey 2015](https://doi.org/10.1017/s0266267115000085) [V] questions how relevant the distinction between equality and priority is for social evaluation. [Broome 2015](https://doi.org/10.1017/s0266267115000097) [V] defends it as useful. Content: titles; arguments from memory.
- **Over a whole lifetime.** [Adler, Ferranna, Hammitt and Treich 2021](https://doi.org/10.1016/j.jhealeco.2020.102412) [V] value reductions in mortality risk over whole lifetimes on utilitarian and prioritarian views, which formalises "fair innings" intuitions. Content: title and memory.
- **Operational form.** Fully computable given (i) a well-being metric with a zero, (ii) γ, and (iii) the ex ante or ex post choice. All three are value choices.

##### 1.4 Sufficientarianism: securing enough, with thresholds

- **Frankfurt.** [Frankfurt 1987](https://doi.org/10.1086/292913) [V] holds that economic equality is not of moral importance in itself. What matters is that each person has *enough*: the "doctrine of sufficiency". Content: memory. *On Inequality* (Princeton 2015) restates it ([review record, Weithman 2016](https://doi.org/10.1017/s0953820816000054)) [S].
- **Crisp.** In "Equality, Priority, and Compassion", *Ethics* 2003 [M] (the Crossref lookup was rate-limited), Crisp grounds the threshold in the compassion of an impartial spectator: priority below the threshold, none above. Content: memory.
- **Casal** ([Casal 2007](https://doi.org/10.1086/510692) [V]) separates two theses. Content: memory.
  - **The positive thesis:** it matters that people live above a threshold.
  - **The negative thesis:** above it, no further distributive requirements apply.

  She argues that the negative thesis is false, so sufficiency must be supplemented by priority or equality.
- **Shields** ([Shields 2012](https://doi.org/10.1017/s0953820811000392) [V]) replaces the negative thesis by the *shift thesis*: at the threshold there is a discontinuity in the rate of change of the weight of our reasons to benefit people. Content: memory.
- **Variants.**
  - Multiple thresholds: [Benbaji 2005](https://doi.org/10.1017/s0953820805001676) [V].
  - A lower and an upper threshold: [Huseby 2010](https://doi.org/10.1111/j.1467-9760.2009.00338.x) [V].
  - Sufficiency as freedom from duress across central domains of life: [Axelsen and Nielsen 2015](https://doi.org/10.1111/jopp.12048) [V].
  - That sufficiency is indispensable: [Herlitz 2019](https://doi.org/10.1080/13698230.2018.1479817) [V].
  - A Cambridge University Press monograph (12 Dec 2025) proposes an "umbel view" of eight spheres of capabilities relevant to justice ([CUP page](https://www.cambridge.org/core/books/new-theory-of-sufficientarian-justice/ABB223586124FA5D022923B2D77A5A10)) [S; author not seen].

  Content of the variants: memory.
- **Formal variants** (content: memory; standard in the literature):
  - **Headcount:** maximise #{i : uᵢ ≥ τ}. Its perverse property is that it favours those just below τ and licenses taking from the far-below to lift the near-threshold over τ ("upward transfers").
  - **Shortfall (lexical) sufficiency:** first minimise the aggregate shortfall Σ max(0, τ − uᵢ), or a weighted shortfall with priority Σ g(max(0, τ − uᵢ)); only then apply other principles to the rest. This is the poverty-gap family (Foster–Greer–Thorbecke P_α).
  - **Priority capped at τ:** W = Σ g(min(uᵢ, τ)), lexically before a second principle above τ.
- **Critiques** (content: memory):
  - the threshold is arbitrary;
  - indifference above it (Casal);
  - discontinuity;
  - the perversity of headcount;
  - a single threshold may not exist for multidimensional goods.
- **Operational form.** Thresholds become constraints or lexically prior objectives. τ is declared by people (Bradshaw's *normative need* is exactly a declared threshold); the computation then follows.

##### 1.5 Luck egalitarianism: sensitive to choices, insensitive to circumstances

- **Dworkin's equality of resources** (content: SEP):
  - It distinguishes *ambitions* (outcomes of choice) from *endowments* (brute luck).
  - Resources are allocated by an auction from equal purchasing power.
  - Hypothetical insurance against handicaps and low talent compensates for endowments.

  Sources: [Dworkin 2000](https://doi.org/10.2307/j.ctv1c3pd0r) [V]; "What is Equality? Part 2", *PPA* 10 (1981) [V-reprint: [Routledge 2018](https://doi.org/10.4324/9781315199795-7)]. The SEP text fetched does not name the envy test.
  - From memory: the auction ends when no one prefers anyone else's bundle (the envy test), and *option luck* (deliberate gambles) is distinguished from *brute luck*.
- **Variants.** [Arneson 1989](https://doi.org/10.1007/bf00646210) [V], equal opportunity for welfare; [Cohen 1989](https://doi.org/10.1086/293126) [V], equal access to advantage; [Vallentyne 2002](https://doi.org/10.1086/339275) [V], equality of initial opportunities; [Arneson 2000](https://doi.org/10.1086/233272) [V], a move toward responsibility-catering prioritarianism. Content: memory.
- **The envy test made formal.** Envy-freeness (Foley 1967 [M]; [Varian 1974](https://doi.org/10.1016/0022-0531(74)90075-1) [V]) is the condition ∀ i, j: uᵢ(xᵢ) ≥ uᵢ(xⱼ). Content: memory.
  - In exchange economies, competitive equilibrium from equal incomes is envy-free and Pareto efficient.
  - With production and unequal skills, envy-free efficient allocations may fail to exist ([Pazner and Schmeidler 1974](https://doi.org/10.2307/2296762) [V]).
  - Their alternative is *egalitarian-equivalent* allocations ([Pazner and Schmeidler 1978](https://doi.org/10.2307/1883182) [V]).
- **Compensation against reward.** In the economics of responsibility ([Fleurbaey 2008](https://doi.org/10.1093/acprof:osobl/9780199215911.001.0001) [V]; [Fleurbaey and Maniquet 2011](https://doi.org/10.1017/cbo9780511851971) [V]), full *compensation* (equal outcomes for equal responsibility characteristics) and full *natural reward* (no redistribution among those with equal circumstances) are generally incompatible. Each theory picks a compromise (conditional equality, egalitarian equivalence). Content: memory.
- **Critiques.**
  - [Anderson 1999](https://doi.org/10.1086/233897) [V] and [Wolff 1998](https://doi.org/10.1111/j.1088-4963.1998.tb00063.x) [V] argue that justice should express equal respect and standing, which luck-focused schemes may undermine; practically, talent cannot be cleanly separated from responsibility. Content: SEP.
  - From memory:
    - Anderson's *abandonment* objection: the negligent uninsured driver is left to die;
    - Anderson's charge that luck egalitarianism demands intrusive and disrespectful judgments;
    - Wolff's *shameful revelation*: claimants must prove their lack of talent.
  - [Fleurbaey 1995](https://doi.org/10.1017/s0266267100003217) [V] gives the case of the reckless motorcyclist ("Bert"): no help for chosen bad outcomes is too harsh. Content: memory.
- **Operational form.** Envy-freeness, egalitarian equivalence and compensation tests are checkable. The partition into circumstance and choice, and the data that would reveal choice, are moral and political and often intrusive.

##### 1.6 Roemer's equality of opportunity: circumstances, effort and types

- **The construction** ([Roemer 1998](https://doi.org/10.4159/9780674042872) [V]; [Roemer and Trannoy 2015, *Handbook of Income Distribution* vol. 2](https://doi.org/10.1016/b978-0-444-59428-0.00005-9) [V]; content: memory):
  - Society chooses which factors count as *circumstances*. These partition the population into *types* t.
  - Within a type, effort e has a distribution F_t.
  - Roemer's identification is that people at the same *rank* π = F_t(e) of their type's distribution of effort have exercised the same degree of responsibility.
  - With outcome v^t(π, φ) under policy φ, the **EOp policy** is φ* = argmax_φ ∫₀¹ min_t v^t(π, φ) dπ: the mean, over effort ranks, of the worst type's outcome at that rank.
  - Van de gaer's alternative is max_φ min_t ∫₀¹ v^t(π, φ) dπ (the minimum over types of the mean).
  - The literature separates *ex ante* (opportunity sets) from *ex post* (outcomes at equal effort) EOp, and principles of compensation from principles of reward.
- **The planner.** Roemer's "pragmatic theory of responsibility for the egalitarian planner" makes the choice of circumstances explicitly political ([Roemer 1994, in *Egalitarian Perspectives*](https://doi.org/10.1017/cbo9780511528293.011) [V]; first in *PPA* 1993 [M]). Content: memory.
- **Compatibility with Fleurbaey's approach** is discussed in [Trannoy 2016, *Revue économique*](https://doi.org/10.3917/reco.681.0057) [V]. Content: title.
- **Critiques** (content: memory):
  - the quantile identification assumes that differences between types in the distribution of effort are themselves circumstances;
  - the partitions into types are coarse in practice;
  - the luck-egalitarian objections of §1.5 apply.
- **Operational form.** Fully computable once types, outcome and the space of policies are declared. Declaring them is the contested step.

##### 1.7 Desert, merit, need and contribution as grounds

- **Desert** (content: SEP):
  - Its bases include *contribution* (Miller, Riley), *effort* (Sadurski, Milne) and *compensation* for costs (Dick, Lamont).
  - Desert is backward-looking, unlike incentive payments, which are forward-looking.
  - Rawls objects that productivity depends on factors beyond the agent's control.

  Source: [SEP](https://plato.stanford.edu/entries/justice-distributive/) [V]. See also [Sher 1987](https://doi.org/10.1515/9780691221366) [V].
- **Miller.** In *Principles of Social Justice* (Harvard 1999) [S, via a [review record](https://doi.org/10.1086/324246)], Miller ties three principles to three modes of relationship (content: memory):
  - need to solidaristic community;
  - desert to instrumental association;
  - equality to citizenship.
- **Need.** In *Protecting the Vulnerable* (Chicago 1985) [S, [review record](https://doi.org/10.1086/292876)], Goodin grounds special responsibilities in vulnerability. Content: memory.
  - Need-based rules create incentives to become, or appear, needy. Elster stresses the incentive effects of every principle, e.g. waiting time encourages early registration (content: memory).
- **Merit.** In local allocation (places at a college), "merit" means either *predicted* performance (efficiency) or *past* achievement (desert), and the two diverge. Content: memory, Elster.
- **Operational form.** Each ground is an order on claimants given a declared measurement. Each is also limited to a declared domain (Walzer, Miller), e.g. desert for prizes and need for care.

##### 1.8 Anderson's democratic (relational) equality

- **The point of equality.** [Anderson 1999](https://doi.org/10.1086/233897) [V] says the point of equality is to end oppression and to secure a community of equals. Content: memory.
  - The state owes every citizen the capabilities needed to function as an equal citizen (a sufficientarian threshold of capabilities relevant to citizenship), across a whole life.
  - It rejects compensation for brute luck as the aim.
- **Equality as a relation.** [Scheffler 2003](https://doi.org/10.1111/j.1088-4963.2003.00005.x) [V] and [Anderson 2010](https://doi.org/10.1080/00455091.2010.10717652) [V] develop equality as a social relation and contrast it with luck egalitarianism. Content: titles; arguments from memory.
  - A 2025 second edition of Hirose's *Egalitarianism* has a new chapter on relational egalitarianism ([Routledge 2025](https://doi.org/10.4324/9781003342007-9)) [V].
- **Operational form.** Constraints and tests rather than objectives:
  - no allocation rule that requires stigmatising disclosure;
  - no hierarchy of standing;
  - access that does not depend on a patron's discretion;
  - a threshold of capabilities for citizenship.

##### 1.9 Sen's capability approach and Nussbaum's thresholds

- **Currency.** *Functionings* (beings and doings) and the *capability set* (the attainable vectors of functionings). The informational base differs from resources (people convert resources into functionings at different rates) and from utility (adaptation). Sources: [Sen 1992, *Inequality Reexamined*, ch. "Equality of What?"](https://doi.org/10.1093/0198289286.003.0002) [V]; *Commodities and Capabilities* (1985) [S, review record]; *Development as Freedom* (1999) [S, review record]. Content: memory.
- **No fixed list.** Sen refuses a canonical list: its selection and weights belong to public reasoning ([Sen 2004, "Capabilities, Lists, and Public Reason"](https://doi.org/10.1080/1354570042000315163) [V]). Content: memory.
- **The intersection approach.** Sen's way to deal with uncertain weights is a *partial ordering*. With W the set of admissible weight vectors, x ≽_W y iff Σₖ wₖfₖ(x) ≥ Σₖ wₖfₖ(y) for every w ∈ W. Content: memory.
  - The ordering is complete only when W is a singleton.
  - It grows more incomplete as W widens, and dominance is the limit case.
  - This is directly computable.
- **Nussbaum's list** ([Nussbaum 2000](https://doi.org/10.1017/cbo9780511841286) [V]; [Nussbaum 2006](https://doi.org/10.4159/9780674041578) [V]; [Nussbaum 2011](https://doi.org/10.4159/harvard.9780674061200) [V]) has ten central capabilities, each to be secured up to a threshold of a life worthy of human dignity:
  1. life;
  2. bodily health;
  3. bodily integrity;
  4. senses, imagination and thought;
  5. emotions;
  6. practical reason;
  7. affiliation;
  8. other species;
  9. play;
  10. control over one's environment (political and material).

  Content: memory.
  - The capabilities are plural and not mutually substitutable: shortfall in one is not made up by surplus in another, and choices that force a sacrifice of one are "tragic".
  - *Frontiers of Justice* extends this to disability, nationality and other species.
- **The modular view.** [Robeyns 2017](https://doi.org/10.11647/obp.0130) [V] presents the capability approach as an open framework with compulsory and optional modules, not a single theory. Content: memory.
- **Operational form.**
  - Multidimensional thresholds, one per capability, as constraints (Nussbaum).
  - Dominance or intersection quasi-orderings over the vectors of functionings (Sen).
  - The list and its weights come from deliberation.

##### 1.10 Sen's *The Idea of Justice*: comparative, not transcendental

- **The record.** [Sen 2009](https://doi.org/10.4159/9780674054578) [V] (also [JSTOR edition](https://doi.org/10.2307/j.ctvjnrv7n)); its programme in brief is [Sen 2006, "What Do We Want from a Theory of Justice?"](https://doi.org/10.5840/jphil2006103517) [V].
- **Transcendental institutionalism** is the tradition, after Hobbes, Rousseau, Kant and Rawls, that identifies perfectly just institutions. Against it Sen sets *realisation-focused comparison* (after Smith, Condorcet, Bentham, Mill, Marx and social choice theory): ranking feasible social states, including how people's lives actually go. Content: memory; corroborated in a [review summary](https://cjdproject.web.nycu.edu.tw/2020/01/03/rawls-vs-sen-an-idea-of-justice/) [S].
- **Sen's three claims** (content: [review summary](https://cjdproject.web.nycu.edu.tw/2020/01/03/rawls-vs-sen-an-idea-of-justice/), from search results):
  - a transcendental theory is *neither necessary nor sufficient* for comparisons;
  - it is parochial;
  - it is inflexible.

  The 2006 paper argues the redundancy point by analogy: knowing that the *Mona Lisa* is the best painting does not help in ranking a Dalí against a Picasso. Content: memory.
- **Nyāya and nīti.** He distinguishes *nīti* (organisational propriety, rules and institutions) from *nyāya* (realised justice, the world that emerges). Content: memory.
- **The flute** (Introduction [M for location]). Three children claim one flute:
  - Anne, because she alone can play it;
  - Bob, because he is the poorest and has no toys;
  - Carla, because she made it.

  Each claim is supported by a recognisable impartial theory (roughly utilitarian, economic egalitarian, libertarian). Sen's point is that several impartial reasons can survive scrutiny without one defeating the others (*plural grounding*), so an impartial resolution may not be unique. Content: names and grounds corroborated in a [review](https://pide.org.pk/research/amartya-sen-the-idea-of-justice-london-penguin-books-ltd-2009-468-pages-hardback-25-00/) [S]; interpretation from memory.
- **Partial rankings and maximal choice.** Social choice supplies the framework:
  - rankings may be *partial*;
  - reasoned agreement may cover only some pairs;
  - choice should pick a *maximal* element (one not worse than any other), not demand an *optimal* one ([Sen 1997, "Maximization and the Act of Choice"](https://doi.org/10.2307/2171939) [V]).

  The SEP corroborates the general point: "choosing an option that is not worse than the others is justified", and incommensurable options can both be permissible. Content: [SEP Incommensurable Values](https://plato.stanford.edu/entries/value-incommensurable/) [V].
- **Open impartiality.** Adam Smith's impartial spectator, open to voices beyond the polity, is set against the *closed impartiality* of a contract among members. Content: memory.
- **Critiques.**
  - Valentini, "A Paradigm Shift in Theorizing about Justice? A Critique of Sen", *Economics and Philosophy* 2011 ([PhilArchive](https://philarchive.org/archive/VALAPS-3)) [S].
  - Another article in the search results argues that "a transcendental dimension is a necessary, albeit not sufficient, condition of any comparative judgment" ([dvkjournals summary](https://dvkjournals.in/index.php/jd/article/download/86/166/577)) [S].
  - It is also said that Sen leaves the choice of dimensions and weights underdetermined (content: memory).

##### 1.11 Walzer's *Spheres of Justice*: complex equality

- **The record.** Walzer, *Spheres of Justice: A Defense of Pluralism and Equality* (Basic Books 1983) [S, via reference-work records: [Kindlers](https://doi.org/10.1007/978-3-476-05728-0_20338-1), [Schlüsselwerke](https://doi.org/10.1007/978-3-531-90400-9_128)].
- **The principle** (content: memory). Each social good has a *social meaning* that fixes its proper distributive criterion. Injustice is the *dominance* of one good over others: money buying office, office buying healthcare. Complex equality forbids conversion across spheres:
  > "No social good x should be distributed to men and women who possess some other good y merely because they possess y and without regard to the meaning of x" (p. 20) [M for wording].
- **The spheres:** membership; security and welfare (need); money and commodities (free exchange); office (qualification); hard work; free time; education; kinship and love; divine grace; recognition; political power. Content: memory.
- **Blocked exchanges.** Things money may not buy (content: memory, approximate):
  - persons;
  - political power;
  - criminal justice;
  - basic freedoms;
  - exemption from military service or jury duty;
  - political office;
  - basic welfare services;
  - "desperate exchanges";
  - prizes and honours;
  - love and friendship.
- **Three criteria.** Free exchange, desert and need, none universal. Content: memory.
- **Critiques.** Dworkin's 1983 review "To Each His Own" (*NYRB*) [M]: social meanings are contested, and the view slides into conventionalism and relativism. A 2025 edited volume rereads Walzer for the 2020s ([*Walzer and Justice*, Palgrave, ed. J. T. Reiner](https://link.springer.com/book/9783031878084)) [S]. Content: memory and publisher snippet.
- **Operational form.**
  - A good-specific registry of principles: each good is tagged with its sphere and its admissible criteria.
  - *Conversion checks* test whether an allocation's ranking correlates with possession of an unrelated good, a test of dominance.
  - Blocked exchanges become feasibility constraints.

##### 1.12 Young and Fraser: oppression, recognition, representation, structural injustice

- **Young.** *Justice and the Politics of Difference* (Princeton 1990; [2011 reissue](https://doi.org/10.1515/9781400839902)) [V-reprint] criticises the *distributive paradigm* for treating justice as patterns of holdings, ignoring decision-making, the division of labour and culture. Content: memory.
  - **The five faces of oppression:** exploitation, marginalisation, powerlessness, cultural imperialism, violence.
- **The social connection model** ([Young 2006](https://doi.org/10.1017/s0265052506060043) [V]; [Young 2011](https://doi.org/10.1093/acprof:oso/9780195392388.001.0001) [V]). Responsibility for structural injustice is forward-looking and shared among all who take part in the processes that produce it. It is graded by *power, privilege, interest and collective ability*. Content: memory.
- **Fraser.** *Scales of Justice* (Columbia UP 2009; Polity 2008) [S, via [review record](https://doi.org/10.1177/0090591709354875)] has three dimensions: *redistribution* (economic), *recognition* (status) and *representation* (political: who counts, the "frame"). The norm is **participatory parity**: arrangements that let all participate as peers. "Misframing" is excluding the affected from the "who" of justice; the remedy is an "all-subjected" principle. Content: memory.
- **Operational form.** These are not patterns but *audits*: of who decides, whose needs are recognised, who is represented, and whose position the structure reproduces.

##### 1.13 Care ethics

- **The works.** [Held 2006](https://doi.org/10.1093/0195180992.001.0001) [V]; [Tronto 2013, *Caring Democracy*](https://doi.org/10.18574/nyu/9780814770450.001.0001) [V]; Kittay, *Love's Labor* (1999) [M].
- **What they hold** (content: memory):
  - Dependency is universal.
  - Care is a practice with phases: caring about, taking care of, care-giving, care-receiving, and, in Tronto 2013, *caring with*.
  - Allocation should attend to particular relations, responsiveness and the distribution of care work itself, not only to holdings.
- **Operational form.**
  - Make care burdens, and who carries them, a distributed quantity (the *care* domain in social provision).
  - Require attentiveness to particular need (an override or exception channel) over uniform rules.

##### 1.14 Elster's *Local Justice*: how institutions actually allocate

- **The record and its scope.** Elster, *Local Justice: How Institutions Allocate Scarce Goods and Necessary Burdens* (Russell Sage Foundation, 1992). The [publisher page](https://www.russellsage.org/publications/local-justice) [V] lists December 1993 with ISBN 978-0-87154-232-8, probably the paperback; the original was 1992 per the [Choice review record](https://doi.org/10.5860/choice.30-2319) [V]. There is a [review by Riley in *Ethics* 1996](https://doi.org/10.1086/233630) [V]. Content from the publisher page:
  - **Goods and burdens studied:** college admission, the selection of organ recipients, layoffs, military induction, adoption.
  - **What it does:** it classifies the main principles and procedures for matching goods with people, examines how these mechanisms interact, and assesses them for fairness and efficiency.
  - **How principles arise:** it traces how preferences become principles and principles become procedures.
  - **Comparisons:** it compares these systems with utilitarianism, Rawls, Nozick and the "common-sense" views of lawyers, economists and politicians.
  - **Perverse effects:** some methods worsen the problems they address; uniform strict screening reduces adoptions of hard-to-place children.
  - **Actors:** selective service boards, adoption agencies, employers, universities, prison and immigration authorities, and the agencies and applicants around them.
- **The taxonomy** (content: memory, to be checked against chs. 3–4):
  - **Egalitarian principles:** strict equality, lotteries, rotation, equalisation of outcomes.
  - **Time-related principles:** queuing, waiting lists, seniority.
  - **Status-related principles:** age, gender, civil status, residence, ethnicity, membership.
  - **Principles defined by other properties of individuals:** need, merit, desert, productivity or efficiency, contribution, welfare.
  - **Mixed systems:** *point systems* (e.g., kidneys: waiting time plus tissue match plus urgency), lexical combinations, quotas, thresholds followed by a lottery.
- **Three orders of actors** (content: memory):
  - *first-order* actors are the allocating institutions;
  - *second-order* actors are the governments that constrain them;
  - *third-order* actors are public opinion.
- **Elster's general findings** (content: memory):
  - Principles have *incentive effects* and are *manipulable*.
  - Mixtures are the rule in practice.
  - The commonsense conception of justice is roughly utilitarian with a floor (maximise total welfare subject to a minimum, with exceptions for those below it through their own choice) [M].
- **Lotteries.** Elster's earlier *Solomonic Judgements* (CUP 1989) [S, review records [Scobie 1992](https://doi.org/10.1163/002071592x00095)] argues for lotteries when reasons are indeterminate ("Solomonic" cases). Content: memory.
- **Mixtures made formal.** A recent formalisation is the *reserve system* in market design ([Pathak, Sönmez, Ünver and Yenmez 2024, *Management Science*](https://doi.org/10.1287/mnsc.2022.00930) [V]; [EC'21 version](https://doi.org/10.1145/3465456.3467604) [V]; content: memory):
  - Units are split into categories (e.g., essential workers, the disadvantaged, general), each with its own priority order.
  - Allocation is computed by a cutoff equilibrium or deferred acceptance.
  - The *order in which categories are processed* changes outcomes and must be declared.
  - The title's thesis is "leaving no ethical value behind".
- **Reservations.** [Sönmez and Yenmez 2022, *Econometrica*](https://doi.org/10.3982/ecta17788) [V] analyse India's vertical and horizontal reservations as choice rules. Content: memory.

##### 1.15 Composite frameworks for scarce medical goods (Elster's mixtures in practice)

- **Persad, Wertheimer and Emanuel** ([Persad, Wertheimer and Emanuel 2009, *Lancet*](https://doi.org/10.1016/s0140-6736(09)60137-9) [V]) group the allocation principles into four kinds (content: memory):
  1. treating people equally: lottery, first-come first-served;
  2. favouring the worst off: sickest first, youngest first;
  3. maximising total benefits: lives saved, prognosis or life-years;
  4. promoting and rewarding social usefulness: instrumental value, reciprocity.

  They argue that no principle suffices alone and propose a "complete lives system" that combines priority to the young, prognosis, saving the most lives and lotteries.
- **Emanuel et al.** [Emanuel et al. 2020, *NEJM*](https://doi.org/10.1056/nejmsb2005114) [V], on COVID-19, names four values: maximise benefits, treat people equally, promote and reward instrumental value, give priority to the worst off. Content: memory.
- **Public attitudes** to these principles are surveyed in [Fallucchi, Faravelli and Quercia 2020](https://doi.org/10.1136/medethics-2020-106524) [V]. Content: title.

##### 1.16 Recent books (2024–2025)

- **Robeyns,** *Limitarianism: The Case against Extreme Wealth* (Allen Lane, 2024) [S, via review records [EJPE 2024](https://doi.org/10.23941/ejpe.v17i1.871), [RFSP 2025](https://doi.org/10.3917/rfsp.744.0797)]: an *upper* threshold. Content: memory.
- **Segall,** *The Future of Equality* (OUP 2025): a pluralism combining egalitarianism with prioritarianism, and the problems of prioritarianism for future people ([HUJI record](https://cris.huji.ac.il/en/publications/the-future-of-equality/)) [S]. Content: search snippet.
- ***The Anatomy of Justice: On the Shape, Substance, and Power of Liberal Egalitarianism*** (OUP 2025): chapters "Two Pluralisms about Justice" and "Relational Equality as Legitimacy" ([OUP](https://academic.oup.com/book/57911)) [S; author not seen]. Content: search snippet.

#### Inferences
- **Every theory has the same shape.** Each is a triple (currency, aggregation structure, side-constraints). The aggregation structures used across the literature fall into a short list, all representable in a computer:
  - the sum (utilitarian);
  - the concave sum (prioritarian);
  - lexicographic minimum (leximin);
  - lexicographic thresholds (sufficientarian);
  - an extremum conditioned on type (Roemer);
  - pairwise envy tests (Dworkin, Varian);
  - a registry of principles per good (Walzer);
  - priorities by category with a declared order of processing (Elster, Pathak et al.).
- **The free parameters carry the moral weight.** They are the index, γ, τ, the partition into types, the circumstances, the sphere of a good, and the order of processing. A tool that hides any of them behind a default would be taking a moral position silently.
- **Two families cannot be optimised over.** The plural and relational theories (Walzer, Young, Fraser, Anderson, care ethics, Sen 2009) give mostly *constraints and audits*, not objectives. A design that represents justice only as an objective function cannot express them.
- **Practice mixes; theory names.** Elster's point that practice is a mixture fits the user's position: an allocation tool should compose principles as named rules with a declared order, not blend them into one score.

#### Gaps
- **Not read this session:** Elster's taxonomy (chs. 3–4), the commonsense conception, Walzer's list of blocked exchanges, the location of Sen's flute, Nussbaum's list, Adler's ratio-scale claim and Roemer's objective. All are from memory and should be checked against the texts.
- **Not verified:** Crisp 2003, and the original 1981 *PPA* record of Dworkin's paper (a 2018 reprint was found).
- **Authors not identified:** the authors of two 2025 books (the CUP sufficientarian monograph and *The Anatomy of Justice*).

### 2. Pluralism, Incommensurability, and Alternatives to Maximisation

#### Takeaway
- **Comparability is not guaranteed.** Value pluralism and incommensurability (Berlin, Raz, Chang) imply that options may be incomparable or "on a par". The trichotomy (better, worse, equally good) then fails, and a single maximand need not exist.
- **What replaces maximisation:**
  - partial orders and choice from the maximal set (Sen);
  - dominance or intersection over admissible weights or principles;
  - thresholds and satisficing;
  - lexical orders;
  - side-constraints;
  - reasoned agreement on partial rankings;
  - lotteries among maximal options.
- **What the impossibility results show.** They show formally that aggregating plural principles into one complete ordering forces some principle to be violated. Arrow's theorem, Sen's liberal paradox, Fleurbaey–Tungodden, Pazner–Schmeidler, compensation against reward, and the incompatibilities among algorithmic fairness criteria all do so.

#### Cited Findings
- **Definitions** ([SEP, Andersson and Hsieh, rev. 2025](https://plato.stanford.edu/entries/value-incommensurable/) [V]; content: SEP):
  - items are *incomparable* when no positive value relation holds between them;
  - they are *incommensurable* when neither is better and they are not equally good.

  On the traditional three relations these coincide. With more positive relations, incomparability becomes a subset of incommensurability.
- **The small-improvement argument** (content: SEP; Raz's lawyer against clarinettist, named by Chang). With A⁺ a slight improvement of A:
  1. A is neither better nor worse than B;
  2. A⁺ is better than A;
  3. A⁺ is not better than B;
  4. therefore A and B are not better, worse or equally good.

  This refutes the "Trichotomy Thesis". Sources: [Raz 1986](https://doi.org/10.1093/0198248075.001.0001) [V]; [Chang 2002](https://doi.org/10.1086/339673) [V].
- **Responses to the argument** (content: SEP):
  - Chang's **parity**: a fourth positive relation;
  - Parfit's **rough equality**: "evaluative imprecision";
  - Broome's **vagueness or indeterminacy**;
  - Forrester's **epistemic view** (2022);
  - Rabinowicz's **fitting-attitudes** model, on which X is better than Y iff preferring X is rationally required, and parity holds iff preferring either is permissible. It yields fifteen combinations.

  Chang's interval account is [Chang 2005, "Parity, Interval Value, and Choice"](https://doi.org/10.1086/426307) [V]. On the collection: Chang (ed.), *Incommensurability, Incomparability, and Practical Reason* (Harvard 1997) [S, review records].
- **Practical reason** (content: SEP):
  - **Optimisation:** only the three relations justify choice.
  - **Comparativism (Chang):** parity can ground correct choice.
  - **Maximisation (Sen):** choosing an option not worse than the others is justified, and incommensurable options can both be permissible.
  - **Satisficing:** "good enough" may be justified.
  - **Cycles:** individually permitted swaps can leave one worse off (the "money pump"); answers include Broome's binding choices and Chang's will-based reasons.
  - **Sweetening (Hare):** under risk, a slight "sweetening" can make one act better in expectation even between items on a par.
- **Pluralism and incommensurability** (content: SEP):
  - Berlin's value pluralism is "the most obvious argument" for incommensurability.
  - But "pluralism alone does not necessarily entail incommensurability": items realising different values may still be comparable under a covering consideration.

  Berlin, *Four Essays on Liberty* (OUP 1969) [S, [review record](https://doi.org/10.1017/s0012217300028821)]; *Liberty* (2002) [V-reprint].
- **Maximality against optimality made formal** ([Sen 1997](https://doi.org/10.2307/2171939) [V]; content: memory). For a reflexive, possibly incomplete relation ≽ on a feasible set S:
  - the optimal set is O(S) = {x ∈ S : x ≽ y ∀ y ∈ S}, possibly empty;
  - the maximal set is M(S) = {x ∈ S : ¬∃ y ∈ S, y ≻ x}, non-empty for finite S when ≻ is acyclic.

  Sen argues that rational and just choice needs only M(S). Sen 2009 makes this central to justice. See also *Collective Choice and Social Welfare* (1970; [1979 Elsevier ed.](https://doi.org/10.1016/b978-0-444-85127-7.50012-5) [V-reprint]) and [*Rationality and Freedom*](https://doi.org/10.2307/j.ctv1dv0td8) [V].
- **Arrow's theorem** (*Social Choice and Individual Values*, 1951; [Yale ed.](https://doi.org/10.12987/9780300186987) [V-reprint]). No social welfare function on at least three alternatives satisfies unrestricted domain, weak Pareto, independence of irrelevant alternatives and non-dictatorship. Content: memory.
  - Read with *principles* as voters, an ordinal aggregation of complete principle-orderings that respects these conditions makes one principle a dictator.
  - The multicriteria version is in Arrow and Raynaud, *Social Choice and Multicriterion Decision-Making* (MIT 1986) [M].
- **Sen's liberal paradox** ([Sen 1970](https://doi.org/10.1086/259614) [V]). Unrestricted domain, weak Pareto and minimal liberalism (at least two persons each decisive over one pair) are jointly inconsistent with acyclic social preference. Content: memory.
- **Aggregation against non-aggregation** ([Fleurbaey and Tungodden 2010, *Economic Theory* 44: 399–414](https://doi.org/10.1007/s00199-009-0462-0) [V]; title and pages via Crossref and Semantic Scholar; abstract not retrievable). Content: memory. Under Pareto and mild conditions, a social ordering cannot avoid both of:
  - the "tyranny of non-aggregation" (some arbitrarily small gain to the worst off outweighs arbitrarily large losses to many better off; leximin);
  - the "tyranny of aggregation" (small gains to enough better-off people outweigh a large loss to the worst off; utilitarianism, and any prioritarianism with finite concavity).

  An empirical follow-up finds the puzzle in people's social preferences ([Cowell, Fleurbaey and Tungodden 2015](https://doi.org/10.1007/s00355-015-0880-9) [V]; content: title).
- **Envy-freeness against efficiency** ([Pazner and Schmeidler 1974](https://doi.org/10.2307/2296762) [V]). With production, envy-freeness and efficiency can be incompatible. **Compensation against reward:** the two responsibility principles are incompatible in general ([Fleurbaey and Maniquet 2011](https://doi.org/10.1017/cbo9780511851971) [V]; [Fleurbaey 2008](https://doi.org/10.1093/acprof:osobl/9780199215911.001.0001) [V]). Content: memory.
- **Algorithmic fairness.** For risk scores, calibration within groups, balance for the positive class and balance for the negative class cannot all hold except with equal base rates or perfect prediction (Kleinberg, Mullainathan and Raghavan, ITCS 2017 [M; [2023 ACM reprint](https://doi.org/10.1145/3603195.3603206) V-reprint]). A calibrated instrument cannot have equal false positive and false negative rates across groups with different base rates ([Chouldechova 2017](https://doi.org/10.1089/big.2016.0047) [V]). Content: memory.
- **Welfarism and Pareto.** Kaplow and Shavell (2001, *JPE*), "Any Non-welfarist Method of Policy Assessment Violates the Pareto Principle" [M]: a result often invoked against giving fairness weight independent of welfare. Its implication is contested. Content: memory.
- **Ex ante fairness against expected utility** ([Diamond 1967](https://doi.org/10.1086/259353) [V]). Ex ante fairness conflicts with the sure-thing principle that underlies expected-utility aggregation (Harsanyi 1955). Content: memory.
- **Fair division.** The bridge to the claims problem and fair division is [Moulin 2003](https://doi.org/10.7551/mitpress/2954.001.0001) [V] and [Moulin 2019](https://doi.org/10.1146/annurev-economics-080218-025559) [V]; on equity in context, [H. P. Young 1994](https://doi.org/10.1515/9780691214054) [V].
  - The axioms of the familiar rules (proportional, constrained equal awards, constrained equal losses, Talmud) characterise them.
  - Different reasonable axioms select different rules, a microcosm of pluralism.
  - Content: memory.
- **Reasoned agreement without agreed theory.** Rawls's overlapping consensus [M], Sunstein's "incompletely theorized agreements" (*Harvard Law Review* 1995) [M], and Sen's partial agreement on rankings all allow agreement on particular verdicts without agreement on a full theory. Content: memory.

#### Inferences
- **The admissible-set operation.** The formal core needed is the *admissible-set* operation. Given a set P of admissible principles (or of weights, thresholds, readings), each inducing a preorder ≽_p on options, define x ≽_P y iff x ≽_p y for all p ∈ P, and present M(S, ≽_P). This is Sen's intersection approach generalised.
  - It respects incommensurability: incomparable pairs stay incomparable.
  - It shows exactly which principle separates two options, the pivot.
- **Weighted sums.** A weighted sum Σ λ_p s_p(x) reintroduces commensurability by fiat and, by Fleurbaey–Tungodden, inherits one of the tyrannies. It should appear only as one declared admissible principle among others, never as the arbiter.
- **Where the impossibility results bind.** They bind on any demand for a *complete* ranking that also satisfies all principles. Accepting incompleteness, the maximal set, is the principled escape. The remaining choice inside M(S) is a human decision or a declared lottery (§3).

#### Gaps
- **Not retrieved:** the exact axioms of Fleurbaey–Tungodden 2010. The abstract was elided by the publisher and the text was not accessible; the statement above is from memory.
- **Not verified:** Kaplow and Shavell 2001, Sunstein 1995, and Arrow and Raynaud 1986.

### 3. Procedural Justice

#### Takeaway
- **Procedure and outcome come apart.** Rawls separates *perfect*, *imperfect* and *pure* procedural justice: only where no independent criterion of outcome exists does a fair procedure make its outcome just. Allocation of scarce goods is mostly *imperfect* procedural justice under reasonable disagreement about the criterion. That is why frameworks such as Daniels and Sabin's accountability for reasonableness (A4R) put conditions on process (publicity, relevance, revision and appeals, enforcement), and why critics say process cannot replace substantive principles.
- **The social psychology.** Leventhal's six rules, Thibaut and Walker's voice, and Tyler's relational model (neutrality, trustworthiness, standing) give checkable criteria of procedural fairness and evidence of the "fair process effect", which can also legitimate unjust outcomes.
- **Lotteries.** They are just procedures when claims are equal or reasons indeterminate (Broome, Stone, Elster). Weighted lotteries answer unequal claims, and residual unfairness remains.

#### Cited Findings
- **Rawls's three kinds** (*A Theory of Justice*, §14; [Rawls 1971](https://doi.org/10.4159/9780674042605) [V]; content: memory):
  - **perfect:** an independent criterion and a procedure sure to meet it, e.g. cake-cutting where the cutter takes the last piece;
  - **imperfect:** an independent criterion but no procedure sure to meet it, e.g. the criminal trial;
  - **pure:** no independent criterion, so a fair procedure, properly carried out, makes any outcome fair, e.g. fair gambling.
- **Thibaut and Walker** (*Procedural Justice: A Psychological Analysis*, Erlbaum 1975 [S]; [Thibaut and Walker 1978, "A Theory of Procedure", *California Law Review*](https://doi.org/10.2307/3480099) [V]) separate *process control* (voice in presenting evidence) from *decision control*. Disputants judge procedures fairer when they keep process control. Content: memory.
- **Leventhal's rules** ([Leventhal 1980, in *Social Exchange*](https://doi.org/10.1007/978-1-4613-3087-5_2) [V]; content: memory):
  - **Six rules of fair procedure:** *consistency* across persons and time; *bias suppression*; *accuracy* of information; *correctability* (appeal); *representativeness* of the concerns of all affected; *ethicality* (accord with prevailing moral values).
  - **Seven procedural components to which they apply:** selection of agents, setting of ground rules, gathering of information, decision structure, appeals, safeguards, mechanisms of change.
- **Lind and Tyler's models** ([Lind and Tyler 1988](https://doi.org/10.1007/978-1-4899-2115-4) [V]; [Tyler and Lind 1992](https://doi.org/10.1016/s0065-2601(08)60283-x) [V]; content: memory). The group-value model, then the relational model: people judge authorities by
  - *neutrality* (unbiased, fact-based);
  - *trustworthiness* (benevolent motives);
  - *standing* (dignity, respect, recognition of status).

  These carry information about one's membership. Procedural justice drives *legitimacy* and compliance ([Tyler 2006, *Why People Obey the Law*, Princeton ed.; orig. Yale 1990](https://doi.org/10.1515/9781400828609) [V]; [Tyler 2006, *Annual Review of Psychology*](https://doi.org/10.1146/annurev.psych.57.102904.190038) [V]).
- **The fair process effect.**
  - Voice increases perceived fairness even with an unfavourable outcome ([Folger 1977](https://doi.org/10.1037/0022-3514.35.2.108) [V]; content: title and memory).
  - Procedures matter most when outcomes are unfavourable: outcome and procedure interact ([Brockner and Wiesenfeld 1996](https://doi.org/10.1037/0033-2909.120.2.189) [V]; content: title and memory).
  - Organisational justice has four dimensions: distributive, procedural, interpersonal and informational ([Colquitt 2001](https://doi.org/10.1037/0021-9010.86.3.386) [V]; content: memory).
  - The effect is "double-edged": perceived fairness can be produced by voice without influence, which legitimates bad outcomes. MacCoun 2005, *Annual Review of Law and Social Science* [M]. Content: memory.
- **Accountability for reasonableness (A4R)** ([Daniels and Sabin 1997, *PPA* 26](https://doi.org/10.1111/j.1088-4963.1997.tb00082.x) [V]; [*Setting Limits Fairly*, OUP 2002](https://doi.org/10.1093/acprof:oso/9780195149364.001.0001) [V], 2nd ed. 2008 [M]; [Daniels 2008, *Just Health*](https://doi.org/10.1017/cbo9780511809514) [V]). Four conditions:
  - **publicity:** decisions and their rationales are publicly accessible;
  - **relevance:** rationales rest on reasons and evidence that fair-minded people can accept as relevant;
  - **revision and appeals:** mechanisms to challenge and revise decisions in the light of new evidence or arguments, with a reasoned response;
  - **regulation or enforcement:** of the other three, voluntary or public.

  Content: [Wikipedia summary](https://en.wikipedia.org/wiki/Accountability_for_reasonableness) [S] and [PMC6706971](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC6706971/); the summary agrees with memory. Its rationale (memory) is that we lack consensus on distributive principles for health ("reasonable disagreement"), so legitimacy must come from fair process.
- **A4R measured and adapted.**
  - Ontario hospitals scored best on relevance (75.0%), then appeals and revision (56.6%), publicity (56.0%) and enforcement (39.5%) ([PMC548272](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC548272/)). Content: search snippet.
  - A study of Israeli resource-allocation committees proposes refinements to all four conditions ([PMC9161764](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC9161764/)).
  - Action research applied A4R at district level in Kenya, Tanzania and Zambia ([PMC4237792](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC4237792/)).
- **Critiques of A4R** (content: memory unless noted):
  - [Rid 2009](https://doi.org/10.1136/jme.2008.024430) [V] asks how A4R can yield fair decisions: the conditions do not by themselves guarantee fairness of outcome without substantive principles (the title states the question).
  - [Friedman 2008](https://doi.org/10.1111/j.1467-8519.2007.00605.x) [V]: the relevance condition is underspecified, and A4R is too thin on public participation.
  - [Hasman and Holm 2005](https://doi.org/10.1007/s10728-005-8124-2) [V] open the "black box" of process and call for real deliberation among stakeholders.
  - Daniels replies in [Daniels 2018, *AJOB*](https://doi.org/10.1080/15265161.2018.1427330) [V].
  - A4R is combined with multi-criteria decision analysis in [Baeroe and Baltussen 2014](https://doi.org/10.1093/phe/phu006) [V].
  - Wider priority setting: GPS-Health (Norheim et al. 2014) [M]; [Norheim 2015](https://doi.org/10.1002/gch2.1008) [V]; [Ottersen and Norheim 2019](https://doi.org/10.1093/oso/9780190912765.003.0017) [V].
- **Deliberative democracy** (content: memory):
  - **Habermas's discourse principle (D):** only those norms are valid to which all possibly affected persons could agree as participants in rational discourses ([*Between Facts and Norms*, MIT 1996](https://doi.org/10.7551/mitpress/1564.001.0001) [V]).
  - **J. Cohen's ideal deliberative procedure:** free, reasoned, among equals, aimed at rational consensus; "Deliberation and Democratic Legitimacy", 1989 [V-reprint: [Bohman and Rehg eds 1997](https://doi.org/10.7551/mitpress/2324.003.0006)].
  - **Gutmann and Thompson:** the principles are *reciprocity* (mutually acceptable reasons), *publicity* and *accountability*, and decisions are *provisional* (revisable) ([Gutmann and Thompson 2004](https://doi.org/10.1515/9781400826339) [V]).
  - Critical and discursive turn: [Dryzek 2000](https://doi.org/10.1093/019925043x.001.0001) [V].
  - Deliberative polling: [Fishkin 2009](https://doi.org/10.1093/acprof:osobl/9780199604432.001.0001) [V].
  - Open democracy with sortition and rotation: [Landemore 2020](https://doi.org/10.23943/princeton/9780691181998.001.0001) [V].
- **Measuring deliberation.** The Discourse Quality Index operationalises deliberation as coded indicators: participation, level and content of justification (common good against group interest), respect for groups, demands and counterarguments, and constructive politics ([Steenbergen et al. 2003](https://doi.org/10.1057/palgrave.cep.6110002) [V]; content: memory). An empirical review is [Thompson 2008](https://doi.org/10.1146/annurev.polisci.11.081306.070555) [V].
- **Epistemic proceduralism** ([Estlund 2008](https://doi.org/10.1515/9781400831548) [V]). A procedure's authority comes from its tendency to produce correct decisions better than chance, in terms acceptable to all qualified points of view. Content: memory.
- **Lotteries:**
  - **Broome** ([Broome 1984](https://doi.org/10.1086/292596) [V]; [Broome 1991, "Fairness"](https://doi.org/10.1093/aristotelian/91.1.87) [V]; content: memory). Fairness is the satisfaction of *claims* in proportion to their strength. When claims are equal and the good indivisible, a lottery gives each claimant a *surrogate* satisfaction (an equal chance), but "residual unfairness" remains. Claims are distinct from other reasons (e.g., efficiency) that may override fairness. The NDPR review below confirms that Broome's residual unfairness and surrogate good are central to the debate.
  - **Stone** (*The Luck of the Draw*, OUP 2011, [chapter record](https://doi.org/10.1093/acprof:oso/9780199756100.003.0001) [V]; "Why Lotteries Are Just", *JPP* 15(3) 2007: 276–295 [S]; [Stone 2010, "Three Arguments for Lotteries"](https://doi.org/10.1177/0539018409359362) [V]). From [Segall's NDPR review](https://ndpr.nd.edu/news/the-luck-of-the-draw-the-role-of-lotteries-in-decision-making/) [V], content: review:
    - lotteries are proper when "it is essential to prevent bad reasons from affecting decisions about allocation", the **sanitizing effect**;
    - they are justified when "the bad reasons they prevent outweigh the good ones they keep out";
    - Stone's three criteria are *equality* (like cases alike), *priority* (claims met in order of strength) and *efficiency* (Pareto-style);
    - his positive account combines "the agent does not deploy any reason for favouring one strongest claimant over another" with selecting each with equal probability (p. 83);
    - the publisher calls this "The Just Lottery Rule".
    - Segall objects that a lottery remains "a fair procedure that nevertheless results in an inevitable unfairness"; that folding efficiency into justice is unsupported; and that Solomonic non-allocation has a moral advantage, equality.
    - Stone relies on Wasserman to show Broome's surrogate good is incoherent ([Wasserman 1996](https://doi.org/10.1017/s0266267100003709) [V]).
  - **Others:** [Kornhauser and Sager 1988, "Just Lotteries"](https://doi.org/10.1177/053901888027004001) [V]; Goodwin, *Justice by Lottery* (Chicago 1992) [S, [review record](https://doi.org/10.1177/003231879404600214)]; [Peterson 2008](https://doi.org/10.1111/j.1467-8519.2008.00636.x) [V].
  - **Weighted lotteries** (chances proportional to the number of lives or the strength of claims): [Saunders 2009](https://doi.org/10.1007/s10677-009-9157-7) [V]; [Hirose 2007](https://doi.org/10.1111/j.1467-9329.2007.00345.x) [V]. Content: titles and memory.
  - **The numbers problem:** Taurek's coin toss between saving one and saving five ("Should the Numbers Count?", *PPA* 6, 1977 [V-reprint: [Routledge 2012](https://doi.org/10.4324/9780203723746-41)]); Kamm's and Scanlon's tie-breaking arguments ([Kamm, *Morality, Mortality* I](https://doi.org/10.1093/0195119118.001.0001) [V]; [Scanlon 1998](https://doi.org/10.2307/j.ctv134vmrn) [V]). The debate is still active: [Piller 2026, *Utilitas*](https://doi.org/10.1017/s0953820826100326) [V]. Content: memory and title.
  - **Lotteries against queues.** A 2024 paper in *Oxford Studies in Political Philosophy*, "Lotteries, Queues, and Bottlenecks", argues that lotteries suit *scarcity* (not everyone can be served) and queues suit *bottlenecks* (everyone is served eventually) ([PhilArchive SOBLQA](https://philarchive.org/rec/SOBLQA)) [S; author not confirmed]. Content: search snippet.
- **Procedure and outcome.**
  - Rawls: a fair procedure confers justice on outcomes only in pure procedural justice.
  - Legitimacy (the acceptability of being bound) is distinct from justice (correctness), and A4R aims at legitimacy under disagreement.
  - Estlund grounds legitimacy partly in epistemic quality.

  Content: memory.

#### Inferences
- **What is machine-checkable.** Leventhal's rules and A4R's conditions are largely checkable properties of a *record of decisions*:
  - consistency: was the same rule applied to like cases?
  - publicity: is each decision's rationale stored and accessible?
  - correctability: does each decision have an appeal path with a reasoned response?
  - representativeness: who was consulted?
  - bias suppression: are conflicts of interest declared?
  - accuracy: is the provenance of the data recorded?

  Relevance and ethicality need human judgement of reasons.
- **A lottery among maximal options.** A lottery is the principled way to choose within M(S) when the remaining options are on a par or claims are equal. Equal chances fit equal claims; weights proportional to claim strength fit unequal claims (Broome). The tool should still record the residual unfairness, the losers, rather than declare the outcome fair.
- **The fair process effect** is a warning for design. A tool that offers "voice" without influence produces legitimacy without justice. Contestation must be able to change outcomes, and the record must show when it did.

#### Gaps
- **Not verified:** MacCoun 2005 and GPS-Health 2014. Thibaut and Walker 1975 was confirmed only secondarily.
- **Not found:** a 2023–2026 systematic review of A4R. The search did not surface one.

### 4. Allocation in Time and Sequence

#### Takeaway
- **"Who first, who next" has its own principles:**
  - queueing and first-come first-served;
  - waiting time;
  - rotation;
  - urgency ("sickest first") against expected benefit;
  - the rule of rescue;
  - fair innings over lifetimes.
- **Allocation is a history, not a snapshot.** Distributions evolve; Nozick's entitlement theory makes justice historical (acquisition, transfer, rectification); and lifetime against time-slice accounting changes verdicts.
- **Intergenerational justice adds discounting.** Pure time preference is ethically contested; growth discounting is not.
- **Sequences can be formalised.** Reserve systems with a declared order of processing, waitlists against lotteries, and the probabilities of assignment under point systems.

#### Cited Findings
- **Time-related principles** in institutional practice are queuing, waiting lists and seniority; their incentive effects include early registration and strategic waiting. Source: [Elster, *Local Justice*](https://www.russellsage.org/publications/local-justice) [V]. Content: memory.
- **Point systems and waitlists.**
  - Deterministic point systems that favour earlier arrivals "can lead to severe inequality across agents' assignment probabilities unless they use randomization". Service in random order is the most equal of the policies that favour earlier arrivals ([summary of a *Management Science* paper on waitlists](https://marketdesigner.blogspot.com/2023/10/waitlist-equity-when-not-everyone-can.html)) [S; authors not confirmed]. Content: search snippet.
  - In booking systems, the opportunity cost of time spent queuing is a significant loss of efficiency that lotteries largely avoid ([CeDEx discussion paper 2023-04](https://www.nottingham.ac.uk/cedex/documents/papers/cedex-discussion-paper-2023-04.pdf)) [S]. Content: search snippet.
  - US kidney allocation moved away from "a coarse point system based on few donor and patient characteristics and the patient's waiting time" ([Kellogg Insight](https://insight.kellogg.northwestern.edu/article/is-there-a-better-way-to-allocate-organs-to-transplant-patients)) [S]. Content: search snippet.
- **Urgency against benefit.** Persad et al. criticise "sickest first" as the only principle: it ignores prognosis, and it judges "worst off" at a moment rather than over a lifetime. They favour priority to the young in the "complete lives system" ([Persad, Wertheimer and Emanuel 2009](https://doi.org/10.1016/s0140-6736(09)60137-9) [V]). Content: memory.
- **The rule of rescue** was named by Jonsen as the imperative to rescue identified lives in peril whatever the cost ([Jonsen 1986](https://doi.org/10.1111/j.1748-720x.1986.tb00974.x) [V]). It was analysed by [McKie and Richardson 2003](https://doi.org/10.1016/s0277-9536(02)00244-7) [V] and assessed for public allocation of healthcare by [Cookson, McCabe and Tsuchiya 2008](https://doi.org/10.1136/jme.2007.021790) [V]. Content: memory.
  - **For it:** urgency, the identified victim, symbolic value.
  - **Against it:** it favours identified over statistical lives, and it is costly and inequitable to those with unseen needs.
- **Fair innings** ([Williams 1997](https://doi.org/10.1002/(sici)1099-1050(199703)6:2<117::aid-hec256>3.0.co;2-b) [V]): everyone is entitled to a "normal" span of healthy life, and those who fall short have a stronger claim. This is a lifetime, intergenerational principle of equity. Content: memory; prioritarian formalisation in [Adler et al. 2021](https://doi.org/10.1016/j.jhealeco.2020.102412) [V].
- **Intergenerational justice** ([Gosseries and Meyer, eds, *Intergenerational Justice*, OUP 2009](https://doi.org/10.1093/acprof:oso/9780199282951.001.0001) [V]; content: memory):
  - Rawls's just-savings principle;
  - Parfit's non-identity problem (*Reasons and Persons*, 1984 [M]);
  - obligations to future people.
- **Discounting** ([Ramsey 1928](https://doi.org/10.2307/2224098) [V]; [Dasgupta 2008, "Discounting Climate Change"](https://doi.org/10.1007/s11166-008-9049-6) [V]; content: memory). The social discount rate is r = δ + ηg (pure time preference δ, elasticity of marginal utility η, growth g).
  - Ramsey called discounting later enjoyments "ethically indefensible" [M for wording].
  - η is itself an inequality-aversion parameter, so discounting and prioritarian concavity are one value choice.
- **Historical entitlement** (content: SEP):
  - **Nozick's three principles:** justice in acquisition, in transfer, and rectification. They are *historical*, not *patterned* or end-state.
  - **The Lockean proviso,** with Cohen's beach example against its weak version.
  - **Rectification:** Nozick never supplied a principle; Parfit's point about counterfactual identity makes rectification nearly impossible; Nozick suggests the difference principle as a rough rule of rectification.
  - Past injustice "systematically undermines" later distributions, more so for historical than patterned theories.

  Sources: [SEP](https://plato.stanford.edu/entries/justice-distributive/) [V]; Nozick, *Anarchy, State, and Utopia* (Basic Books 1974) [S, [reference record](https://doi.org/10.1007/978-3-531-90400-9_89)]. The Wilt Chamberlain argument ("liberty upsets patterns") is not in the SEP text fetched; content: memory.
- **The time-slice problem.** "Starting-gate" equality permits later inequality. Equal income per period still lets savings make wealth unequal, and family structure complicates equal starts. Content: [SEP](https://plato.stanford.edu/entries/justice-distributive/) [V].
- **Ordered priorities made formal.** In reserve systems, the order in which reserve categories are processed against open units changes who is served. Pathak et al. propose "smart reserves" and make the order a declared design parameter ([Pathak et al. 2024](https://doi.org/10.1287/mnsc.2022.00930) [V]). Content: memory.
  - Fair division in dynamic and online settings (goods arriving over time, envy-freeness up to one good, maximum Nash welfare) is surveyed in [Moulin 2019](https://doi.org/10.1146/annurev-economics-080218-025559) [V]. Content: memory.
- **Aggregation over claims.** [Voorhoeve 2014](https://doi.org/10.1086/677022) [V]: aggregate only claims "relevant" to the strongest. Taurek and Kamm on numbers (§3).

#### Inferences
- **Allocation as history.** A tool should treat allocation as a *history*: a ledger of past allocations, waiting times and burdens.
  - Lifetime or cumulative accounting (fair innings, rotation, seniority) and time-slice accounting must both be available and declared.
  - Verdicts can differ between them, and the difference should be shown.
- **Order of processing is a moral choice.** Priority among categories and the order of processing are moral choices with measurable effects. The tool should compute the outcome under each admissible order and show who changes.
- **Queues against lotteries.** In a bottleneck everyone is served eventually, and a queue may be just. Under true scarcity a queue rewards arrival, an arbitrary circumstance, and a weighted lottery may be preferable. Which case holds is checkable from capacity and demand.
- **History as a constraint.** Historical entitlement makes some allocations unjust whatever their pattern, e.g. taking from those who acquired legitimately, or ignoring prior wrongs. These are constraints or flags drawn from the ledger, not terms in an objective.

#### Gaps
- **Not verified:** Ramsey's exact wording and Parfit 1984.
- **Not identified:** the authors of the *Management Science* waitlist paper and the 2024 *OSPP* queues paper.
- **No dedicated philosophical treatment found:** I found no source that treats the justice of *sequences of allocations* as such (beyond lifetime against time-slice, rotation and historical entitlement). This may be a genuine gap in the literature or a gap in the search.

### 5. Non-ideal Theory, Redress and Participation

#### Takeaway
- **Allocating amid existing injustice** requires non-ideal theory: transitional, comparative, attentive to who was wronged and to how structures reproduce disadvantage (Mills, Valentini, Simmons, Young).
- **Redress** (reparative justice, rectification, compensation) is constrained by supersession and counterfactual problems (Waldron, Parfit's identity point), yet remains a ground of claims.
- **Participation by the affected** is both a demand of justice (Fraser's participatory parity, the "all-subjected" principle) and a condition of legitimacy (A4R, deliberative democracy).

#### Cited Findings
- **Valentini's map** ([Valentini 2012](https://doi.org/10.1111/j.1747-9991.2012.00500.x) [V]) separates three senses of "ideal against non-ideal": full against partial compliance, utopian against realistic, and end-state against transitional. Content: memory.
- **Simmons** ([Simmons 2010](https://doi.org/10.1111/j.1088-4963.2009.01172.x) [V]): non-ideal theory is transitional and guided by an ideal target. Content: memory.
- **Mills** ([Mills 2005, "'Ideal Theory' as Ideology"](https://doi.org/10.1111/j.1527-2001.2005.tb00493.x) [V]): idealisations abstract from oppression, entrench the perspective of the privileged and obscure the structures that most need redress. Content: memory.
- **Supersession** ([Waldron 1992](https://doi.org/10.1086/293468) [V]): historic injustices may be "superseded" by changed circumstances, since entitlements are sensitive to circumstance. Counterfactual reconstruction of what would have been is indeterminate, so redress should look to present need and justice. Content: memory.
- **Reparation and moral repair.**
  - Thompson, *Taking Responsibility for the Past* (Polity 2002) [S, [Choice record](https://doi.org/10.5860/choice.41-0867)]: reparation as an obligation of institutions across generations.
  - [Walker 2006, *Moral Repair*](https://doi.org/10.1017/cbo9780511618024) [V]: restoring moral relations after wrongdoing (acknowledgement, responsibility, repair).

  Content: memory.
- **Structural injustice** ([Young 2006](https://doi.org/10.1017/s0265052506060043) [V]; [Young 2011](https://doi.org/10.1093/acprof:oso/9780195392388.001.0001) [V]): forward-looking, shared responsibility, graded by power, privilege, interest and collective ability. Content: memory.
- **Redress through allocation rules.** India's vertical reservations (caste categories) and horizontal reservations (women, persons with disabilities) are analysed as choice rules in [Sönmez and Yenmez 2022](https://doi.org/10.3982/ecta17788) [V]. Reserve categories for disadvantaged groups in COVID allocation appear in [Pathak et al. 2024](https://doi.org/10.1287/mnsc.2022.00930) [V]. Content: memory.
- **The SEP's points on non-ideal conditions** (content: SEP):
  - large inequalities of wealth convert into political and economic power;
  - feminist critiques (Okin on the family; the burdens of child-rearing creating disadvantage in markets) show distributive injustice embedded in "private" institutions.
- **Participation.**
  - Fraser's participatory parity (§1.12).
  - Gutmann and Thompson's reciprocity and provisionality (§3).
  - A4R's stakeholder appeals (§3).
  - Minipublics chosen by lot ([Fishkin 2009](https://doi.org/10.1093/acprof:osobl/9780199604432.001.0001) [V]; [Landemore 2020](https://doi.org/10.23943/princeton/9780691181998.001.0001) [V]).
  - Arnstein's "ladder of citizen participation" (*JAIP* 1969) [M] separates tokenism from citizen power.

  Content: memory.
- **Compensation.** Luck egalitarianism's compensation principle (§1.5) and Fleurbaey–Maniquet's compensation axioms give formal versions of compensating for circumstances. Rectification for wrongs is distinct from compensation for brute bad luck. Content: memory.

#### Inferences
- **Two distinct grounds of claim.** Redress needs a recorded *history of wrongs*, distinct from a measure of disadvantage. A claim from past injustice is a ground of its own, alongside need and desert, and should not be folded into a disadvantage index without saying so.
- **Participation in setting the terms.** Participation should be recorded as *who set the principles, thresholds and categories*, not only who was consulted on outcomes (Fraser's "who" and Arnstein's ladder).
- **Reservations as data.** Reservations or reserve categories are a computable form of redress. Their size, beneficiaries and order of processing are the decisions, and must be declared and contestable.

#### Gaps
- **Not verified:** Arnstein 1969.
- **Not found:** a computable treatment of "supersession" or of reparations in allocation tools, beyond reservations and reserve categories.

### 6. What Can Be Computed, What People Must Decide, and What a Tool Must Never Claim

#### Takeaway
- **What a computer can do.** For every theory, a computer can evaluate a *declared* rule against data:
  - leximin;
  - Σ g(u);
  - shortfall below τ;
  - envy-freeness;
  - Roemer's objective;
  - dominance across admissible principles;
  - reserve systems;
  - lotteries with declared chances;
  - checks of a procedure's record.
- **What stays with people.** It cannot supply the currency, the index, γ, τ, the partition into types, the sphere of a good, which principles are admissible, the line between responsibility and circumstance, the choice among maximal options, or the legitimacy of the procedure.
- **What it must never claim.** That an allocation is "the just one", "optimal" in a moral sense, or made just by a fair procedure alone.

#### Cited Findings
The table synthesises §§1–5; the sources are cited there. For each theory it gives the currency and structure, what it forbids, what is machine-checkable, what stays human, and what the tool must never claim.

| Theory | Currency and structure | Forbids | Machine-checkable | Stays human | Tool must never claim |
|---|---|---|---|---|---|
| Rawls | Primary goods (index); lexical: liberties ≻ FEO ≻ leximin | Trading basic liberties for goods; inequalities that do not benefit the worst off | Lexical constraints; leximin given an index; FEO as an access constraint | Index weights; who the "least advantaged" group is; whether a local allocation falls under the basic structure | That leximin on its index *is* Rawlsian justice for a local good |
| Utilitarian | Welfare or QALYs; sum | Nothing intrinsically (distribution-blind) | Σuᵢ, CBA/CEA | The welfare metric; interpersonal comparability; which preferences count | That the maximum sum is just; that it respects persons |
| Prioritarian | Well-being (ratio scale); Σ g(uᵢ), g concave | Levelling down (Pareto holds) | Σ g(u) for declared g, γ; ex ante against ex post | γ; the zero of well-being; ex ante against ex post | That a γ is correct; that the result is not sensitive to γ (show the range) |
| Sufficientarian | Any currency; threshold(s) τ; lexical shortfall first | Leaving anyone below τ when avoidable (positive thesis) | Shortfalls, headcount, FGT gaps; feasibility of everyone ≥ τ | τ (and number of thresholds); what to do above τ | That τ is objective; that headcount is just (it is perverse) |
| Luck egalitarian / Roemer | Resources, welfare or opportunities; neutralise circumstances | Inequality from brute luck; compensating for choices | Envy-freeness; EOp objective given types; compensation and reward tests | Which factors are circumstances; whether to use data on "effort" at all; harshness exceptions | That measured "effort" shows responsibility; that the bad outcomes of those judged negligent are just |
| Desert, merit, contribution | Past performance, effort, contribution | Rewarding the undeserving | Ranking on a declared measure | Whether desert applies to this good (sphere); what counts as merit | That a merit score is desert |
| Need | Shortfall from a declared standard (Bradshaw) | Leaving urgent needs unmet for lesser goods | Need-based orders and thresholds | The standard; whose definition of need (normative, felt, expressed, comparative) | That one kind of need is *the* need |
| Anderson (relational) | Capabilities for equal citizenship; constraints | Stigma, domination, humiliating means tests | Flags (disclosure requirements, discretionary gatekeeping) | Whether a practice is demeaning | That a pattern of distribution alone secures equality of standing |
| Sen, Nussbaum | Functionings and capabilities; partial orders and thresholds | Trade-offs below Nussbaum's thresholds | Dominance and intersection orders; capability thresholds | The list; the weight ranges | A complete ranking where only a partial one is warranted |
| Sen 2009 | Comparative, plural grounding | Waiting for an ideal before improving | Maximal sets M(S); pairwise "better under every admissible reason" | Which reasons are admissible; the choice within M(S) | That the selected option is "the best"; that plurality can be dissolved |
| Walzer | Each good its own principle; blocked exchanges | Dominance (conversion across spheres) | Conversion tests (correlation of allocation with unrelated goods); blocked-exchange constraints | The social meaning of each good | That one principle governs all goods |
| Young, Fraser | Oppression, recognition, representation; audits | Exclusion from deciding; misrecognition | Representation audits (who decided, who was affected) | Whether structures oppress; whose voice is missing | That an allocation is just while those affected were not part of setting its terms |
| Care ethics | Care relations and care work | Ignoring dependency and caregivers | Accounts of care burdens and who carries them | Attentiveness to particular need | That uniform rules exhaust care |
| Elster (local) | Mixtures of principles in institutions | Context-blind rules with perverse effects | Point systems, reserves, lotteries, queues as composable rules; incentive simulations | The mixture and its order; the institution's purposes | That a mixture's weights are "neutral" |
| Nozick (historical) | Entitlements via acquisition, transfer, rectification | Patterned redistribution that overrides entitlements | Provenance and history checks (ledger) | Legitimacy of past acquisitions; rectification | That a pattern can be judged without its history |
| Procedural (Rawls, Leventhal, Tyler, A4R) | Procedure's properties | Biased, unappealable, secret procedures | Records of reasons, consistency, appeals, representation, conflicts | Relevance and ethicality of reasons; legitimacy | That a fair procedure makes an imperfect-procedural outcome just |
| Lotteries (Broome, Stone) | Equal or weighted chances | Choosing among equal claims for bad reasons | Declared chances, verifiable randomness, sanitisation (who saw what) | When claims are equal; the weights | That a lottery leaves no unfairness (residual unfairness remains) |

#### Inferences
- **The recurring pattern.** In every row the computer evaluates a declared rule and checks properties, while people declare the free parameters and choose within incompleteness.
- **The prohibited claims.** They share a form: presenting a *conditional* verdict ("under principle p with parameters θ, option x ranks first") as *unconditional* ("x is just").

#### Gaps
- **The table is my synthesis,** not taken from any single source. Its rows rest on the findings above, many of which are from memory (marked).

### 7. What This Means for the Design

#### Takeaway
Represent each principle as a separately evaluated, declared rule. Then:
- compose rules only by explicit structural operations: constraints, lexical order, thresholds, reserves with a declared order, dominance across admissible principles, and lotteries among maximal options;
- show conflicts and pivots instead of collapsing them into a weighted sum;
- record reasons, contestations and revisions as a sealed history;
- leave the moral parameters and the final choice among options on a par to people, recorded as their decisions.

#### Cited Findings
The recommendations below rest on the findings above:
- §2: SEP on maximisation against optimisation; Sen 1997; Fleurbaey–Tungodden 2010; Arrow.
- §3: A4R; Leventhal; Broome and Stone on lotteries.
- §4: Pathak et al. 2024 on the order of reserves.
- §1.11: Walzer's blocked exchanges.
- §1.4: Casal and Shields on thresholds.

#### Inferences

##### 7.1 Principles become five kinds of object
1. **Constraints (filters on the feasible set).**
   - Rawls's basic liberties and fair equality of opportunity of access.
   - Walzer's blocked exchanges.
   - Anderson's prohibitions of stigma (e.g., no rule requiring a shameful disclosure).
   - Nozick's flags from the ledger (assets acquired legitimately; prior wrongs).
   - Nussbaum's capability floors where feasible.

   A constraint that makes the feasible set empty must be reported as a conflict, never silently relaxed.
2. **Thresholds (lexically prior objectives).**
   - Sufficiency: minimise the shortfall below τ before anything else.
   - Bradshaw's normative need as τ.
   - Nussbaum's ten floors.
   - Robeyns's upper limit as an optional ceiling.

   τ is a declared, versioned parameter with its source (who set it, why). Sensitivity to τ is always shown.
3. **Priority orders over claimants (for "who first, who next").**
   - Urgency, worst-off first (leximin), waiting time or queue, seniority, age or fair innings, merit, instrumental value, reciprocity.
   - Mixtures follow Elster's practice and Pathak et al.'s reserve systems. Each unit category has its own priority order, and the *order of processing* is a declared parameter whose effect is computed and displayed. Noetarium's matching (deferred acceptance) is the computational base.
4. **Partial orders over whole allocations (for "what and how much").**
   - Each admissible principle p (utilitarian sum, prioritarian Σg(u) for each γ in a declared range, leximin, shortfall, Roemer's EOp for each declared typing, envy-freeness and Pareto checks) induces ≽_p.
   - The tool computes the intersection x ≽_P y iff x ≽_p y for all p ∈ P, and the maximal set M(S, ≽_P), following Sen 1997 and the intersection approach.
   - For each pair it shows which principles favour which option, and the *pivot*: the principle, parameter or reading on which the comparison turns. This mirrors the existing reading of a norm's open terms under every reading, where the parameters (γ, τ, the typing) play the part of the open terms.
5. **Tests on records (procedural checks).**
   - Leventhal's six rules: consistency across like cases; conflicts declared; provenance of the data; an appeal path for every decision; who was represented; the ethical review declared.
   - A4R's publicity, relevance (reasons stated and tagged with the principle they invoke), revision and appeals, and enforcement.
   - Tyler's voice, neutrality and respect, partly as interface obligations (a person can see and contest their own case).

##### 7.2 Conflicts are shown, not resolved by a weighted sum
- **Never a single score.** No single scalar should rank allocations unless people explicitly choose one, and even then it is shown beside the others.
  - Fleurbaey–Tungodden shows that any complete aggregation commits to one of the two tyrannies.
  - Arrow shows that ordinal aggregation of principles needs a dictator or a relaxed condition.
  - Incommensurability (Raz, Chang) means a weighted sum asserts commensurabilities that may not hold.
- **How a conflict is displayed:**
  - the options in M(S);
  - for each, the principles under which it is best or undominated;
  - the trade-off between them in the principles' own units (e.g., "B saves 12 more life-years; A leaves 40 fewer people below τ");
  - the pivots;
  - the people affected differently.
- **Dual verdicts.** Keep the *standing* of an option apart from its *measure*, e.g. "untenable under sufficiency" alongside "Σ welfare = …". This matches the infrastructure's existing practice of dual verdicts.
- **Impossibilities as warnings.** When a combination of demands is formally impossible, the tool states the theorem and asks which demand to relax. Examples: envy-freeness with efficiency under production (Pazner–Schmeidler); calibration with error balance under different base rates (Kleinberg et al., Chouldechova).

##### 7.3 Lotteries and the final choice
- **When options or claims are on a par.** When M(S) has several options, or claimants with equal claims compete for an indivisible good, the tool offers:
  - (a) a recorded human choice with its reasons; or
  - (b) a lottery with declared chances: equal for equal claims, or weighted by declared claim strength (Broome, Saunders).
- **How the lottery is run.**
  - Randomness is verifiable (seed committed before the draw, as in the archive's sealed journal).
  - The decision-maker's information can be restricted, to obtain Stone's "sanitizing" effect.
  - The losers are recorded as bearing residual unfairness (Broome; Segall's point), with any compensation owed.
- **Queues against lotteries.** The tool distinguishes *bottleneck* cases, where a queue may be just, from *scarcity* cases, where queueing rewards an arbitrary circumstance, by comparing capacity with demand.

##### 7.4 Time and history
- **A ledger, not a snapshot.** Allocation is kept as a sealed ledger of past allocations, burdens, waiting times and recorded wrongs. This enables:
  - lifetime against time-slice evaluation (fair innings, rotation, seniority), with both shown when they differ;
  - checks of historical entitlement and redress (Nozick, Waldron, reparations), as flags and as grounds of claim kept distinct from disadvantage;
  - incentive and manipulation analysis (Elster): simulating how a rule changes behaviour (e.g., registering early, appearing needy).
- **Intergenerational parameters.** Allocations across generations expose δ and η separately and note that η is the same value choice as prioritarian γ.

##### 7.5 What a procedure must record and allow
- **Records:**
  - for every decision, the rule and parameters in force (versioned);
  - the data with provenance;
  - the reasons, each tied to a principle;
  - who decided and who was consulted;
  - conflicts of interest;
  - the alternatives in M(S) that were not chosen, and why.
- **Contestation.** Any affected party can file an objection as an argument (attacking a reason, a datum, a parameter or a principle's applicability). It is evaluated in the existing argumentation frameworks (Dung, ASPIC+), and the standing of each decision is recomputed. A contestation must be able to change the outcome, and the record shows when it did, which guards against the "fair process effect" without influence.
- **Revision.** Rules and thresholds are revised by a recorded act (who, why, under which appeal). Their effect on past and pending cases is computed. Provisionality follows Gutmann and Thompson.
- **Participation.** The record keeps who set the principles, thresholds, categories and order (Fraser's "who"; the levels of Arnstein's ladder), not only who was consulted on outcomes.

##### 7.6 What must remain human judgement
- the currency and the index (primary goods, welfare, capabilities and their list);
- every parameter with moral content: γ, τ, the number of thresholds, the partition into circumstance and effort, the typing, the reserve categories and their sizes, the order of processing, the weights of a lottery;
- which principles are admissible for which good (Walzer's spheres; Miller's modes of relationship);
- the line between responsibility and brute luck in particular cases;
- the choice within the maximal set, unless people delegate it to a declared lottery;
- whether reasons are *relevant* (A4R) and *ethical* (Leventhal);
- whether a structure oppresses, misrecognises or misframes (Young, Fraser);
- the legitimacy of the whole procedure.

##### 7.7 What a tool must never claim
- That an allocation is "just", "fair" or "optimal" without qualification. It may say only "maximal under the admissible principles P with parameters θ" or "first under rule r".
- That a weighted sum of principles, or a default γ or τ, is neutral.
- That a fair procedure makes an outcome just where an independent criterion exists (imperfect procedural justice). It may say "legitimately decided", not "correct".
- That a lottery leaves no unfairness.
- That measured effort shows responsibility, or that a merit score is desert.
- That fairness criteria have all been met when they are known to be jointly unsatisfiable.
- That one kind of need, one theory, or one number settles the question.
- That those affected were heard, when their objections could not change the outcome.

#### Gaps
- **Not tested:** the design recommendations are inferences from the literature and have not been tested against users or institutions.
- **Not found:** an existing software system that implements partial orders over allocations with pivots and contestation. The closest relatives found are reserve-system market design (Pathak et al.) and A4R in health priority setting, used with MCDA (Baeroe and Baltussen 2014).

##### Bibliography with verification markers
The entries below are grouped by marker. "VR" means a reprint or later edition was verified; DOIs are given where found.

**Verified this session (Crossref, publisher page or SEP) [V]:**
- Adler 2012 (10.1093/acprof:oso/9780195384994.001.0001)
- Adler 2019 (10.1093/oso/9780190643027.001.0001)
- Adler, Ferranna, Hammitt and Treich 2021 (10.1016/j.jhealeco.2020.102412)
- Anderson 1999 (10.1086/233897)
- Anderson 2010 (10.1080/00455091.2010.10717652)
- Arneson 1989 (10.1007/bf00646210)
- Arneson 2000 (10.1086/233272)
- Arrow 1973 (10.2307/2025006)
- Axelsen and Nielsen 2015 (10.1111/jopp.12048)
- Baeroe and Baltussen 2014 (10.1093/phe/phu006)
- Benbaji 2005 (10.1017/s0953820805001676)
- Brockner and Wiesenfeld 1996 (10.1037/0033-2909.120.2.189)
- Broome 1984 (10.1086/292596)
- Broome 1991 "Fairness" (10.1093/aristotelian/91.1.87)
- Broome 1991 *Weighing Goods* (Wiley reissue 10.1002/9781119451266)
- Broome 2015 (10.1017/s0266267115000097)
- Brouwer et al. 2008 (10.1016/j.jhealeco.2007.07.003)
- Casal 2007 (10.1086/510692)
- Chang 2002 (10.1086/339673)
- Chang 2005 (10.1086/426307)
- Chouldechova 2017 (10.1089/big.2016.0047)
- Cohen 1989 (10.1086/293126)
- Cohen 2008 (10.4159/9780674029651)
- Cohen 2011 (10.23943/princeton/9780691148700.001.0001)
- Colquitt 2001 (10.1037/0021-9010.86.3.386)
- Cookson, McCabe and Tsuchiya 2008 (10.1136/jme.2007.021790)
- Cowell, Fleurbaey and Tungodden 2015 (10.1007/s00355-015-0880-9)
- Daniels 2008 *Just Health* (10.1017/cbo9780511809514)
- Daniels 2018 (10.1080/15265161.2018.1427330)
- Daniels and Sabin 1997 (10.1111/j.1088-4963.1997.tb00082.x)
- Daniels and Sabin 2002 (10.1093/acprof:oso/9780195149364.001.0001)
- Dasgupta 2008 (10.1007/s11166-008-9049-6)
- Diamond 1967 (10.1086/259353)
- Dryzek 2000 (10.1093/019925043x.001.0001)
- Dworkin 2000 (10.2307/j.ctv1c3pd0r)
- Elster 1992 *Local Justice* (Russell Sage page; Choice 10.5860/choice.30-2319; Riley review 10.1086/233630)
- Emanuel et al. 2020 (10.1056/nejmsb2005114)
- Estlund 2008 (10.1515/9781400831548)
- Fallucchi et al. 2020 (10.1136/medethics-2020-106524)
- Fishkin 2009 (10.1093/acprof:osobl/9780199604432.001.0001)
- Fleurbaey 1995 (10.1017/s0266267100003217)
- Fleurbaey 2008 (10.1093/acprof:osobl/9780199215911.001.0001)
- Fleurbaey 2015 (10.1017/s0266267115000085)
- Fleurbaey and Maniquet 2011 (10.1017/cbo9780511851971)
- Fleurbaey and Tungodden 2010 (10.1007/s00199-009-0462-0)
- Folger 1977 (10.1037/0022-3514.35.2.108)
- Frankfurt 1987 (10.1086/292913)
- Friedman 2008 (10.1111/j.1467-8519.2007.00605.x)
- Gosseries and Meyer 2009 (10.1093/acprof:oso/9780199282951.001.0001)
- Gutmann and Thompson 2004 (10.1515/9781400826339)
- Habermas 1996 (10.7551/mitpress/1564.001.0001)
- Harsanyi 1955 (10.1086/257678)
- Harsanyi 1975 (10.2307/1959090)
- Hasman and Holm 2005 (10.1007/s10728-005-8124-2)
- Hausman 2010 (10.1002/hec.1474)
- Held 2006 (10.1093/0195180992.001.0001)
- Herlitz 2019 (10.1080/13698230.2018.1479817)
- Hirose 2007 (10.1111/j.1467-9329.2007.00345.x)
- Hirose 2025 (10.4324/9781003342007)
- Holtug 2010 (10.1093/acprof:oso/9780199580170.001.0001)
- Huseby 2010 (10.1111/j.1467-9760.2009.00338.x)
- Jonsen 1986 (10.1111/j.1748-720x.1986.tb00974.x)
- Kamm 1993 (OUP 10.1093/0195119118.001.0001)
- Kornhauser and Sager 1988 (10.1177/053901888027004001)
- Landemore 2020 (10.23943/princeton/9780691181998.001.0001)
- Leventhal 1980 (10.1007/978-1-4613-3087-5_2)
- Lind and Tyler 1988 (10.1007/978-1-4899-2115-4)
- Mason 2006 (10.1093/acprof:oso/9780199264414.001.0001)
- McKerlie 1994 (10.1017/s0953820800001308)
- McKie and Richardson 2003 (10.1016/s0277-9536(02)00244-7)
- Mills 2005 (10.1111/j.1527-2001.2005.tb00493.x)
- Moulin 2003 (10.7551/mitpress/2954.001.0001)
- Moulin 2019 (10.1146/annurev-economics-080218-025559)
- Norheim 2015 (10.1002/gch2.1008)
- Nussbaum 2000 (10.1017/cbo9780511841286)
- Nussbaum 2006 (10.4159/9780674041578)
- Nussbaum 2011 (10.4159/harvard.9780674061200)
- O'Brien 2026 (10.1017/s0953820826100375)
- Otsuka and Voorhoeve 2009 (10.1111/j.1088-4963.2009.01154.x)
- Ottersen and Norheim 2019 (10.1093/oso/9780190912765.003.0017)
- Parfit 1997 (10.1111/1467-9329.00041)
- Pathak, Sönmez, Ünver and Yenmez 2024 (10.1287/mnsc.2022.00930; EC'21 10.1145/3465456.3467604)
- Pazner and Schmeidler 1974 (10.2307/2296762)
- Pazner and Schmeidler 1978 (10.2307/1883182)
- Persad, Wertheimer and Emanuel 2009 (10.1016/s0140-6736(09)60137-9)
- Peterson 2008 (10.1111/j.1467-8519.2008.00636.x)
- Piller 2026 (10.1017/s0953820826100326)
- Ramsey 1928 (10.2307/2224098)
- Rawls 1971 (10.4159/9780674042605)
- Rawls 1999 (10.4159/9780674042582)
- Raz 1986 (10.1093/0198248075.001.0001)
- Rid 2009 (10.1136/jme.2008.024430)
- Robeyns 2017 (10.11647/obp.0130)
- Roemer 1994 chapter (10.1017/cbo9780511528293.011)
- Roemer 1998 (10.4159/9780674042872)
- Roemer and Trannoy 2015 (10.1016/b978-0-444-59428-0.00005-9)
- Saunders 2009 (10.1007/s10677-009-9157-7)
- Scanlon 1998 (10.2307/j.ctv134vmrn)
- Scheffler 2003 (10.1111/j.1088-4963.2003.00005.x)
- Sen 1970 (10.1086/259614)
- Sen 1992 chapter (10.1093/0198289286.003.0002)
- Sen 1997 (10.2307/2171939)
- Sen 2002 (10.2307/j.ctv1dv0td8)
- Sen 2004 (10.1080/1354570042000315163)
- Sen 2006 (10.5840/jphil2006103517)
- Sen 2009 (10.4159/9780674054578)
- Sher 1987 (10.1515/9780691221366)
- Shields 2012 (10.1017/s0953820811000392)
- Simmons 2010 (10.1111/j.1088-4963.2009.01172.x)
- Sönmez and Yenmez 2022 (10.3982/ecta17788)
- Steenbergen et al. 2003 (10.1057/palgrave.cep.6110002)
- Stone 2010 (10.1177/0539018409359362)
- Stone 2011 (chapter 10.1093/acprof:oso/9780199756100.003.0001)
- Temkin 1993 (10.1093/oso/9780195078602.001.0001)
- Thibaut and Walker 1978 (10.2307/3480099)
- Thompson 2008 (10.1146/annurev.polisci.11.081306.070555)
- Trannoy 2016 (10.3917/reco.681.0057)
- Tronto 2013 (10.18574/nyu/9780814770450.001.0001)
- Tungodden 2003 (10.1017/s0266267103001007)
- Tyler 2006 book (10.1515/9781400828609)
- Tyler 2006 review (10.1146/annurev.psych.57.102904.190038)
- Tyler and Lind 1992 (10.1016/s0065-2601(08)60283-x)
- Valentini 2012 (10.1111/j.1747-9991.2012.00500.x)
- Vallentyne 2002 (10.1086/339275)
- Varian 1974 (10.1016/0022-0531(74)90075-1)
- Voorhoeve 2014 (10.1086/677022)
- Waldron 1992 (10.1086/293468)
- Walker 2006 (10.1017/cbo9780511618024)
- Wasserman 1996 (10.1017/s0266267100003709)
- Williams 1997 (10.1002/(sici)1099-1050(199703)6:2<117::aid-hec256>3.0.co;2-b)
- Wolff 1998 (10.1111/j.1088-4963.1998.tb00063.x)
- Young 2006 (10.1017/s0265052506060043)
- Young 2011 (10.1093/acprof:oso/9780195392388.001.0001)
- H. P. Young 1994 (10.1515/9780691214054)
- SEP *Distributive Justice* (rev. 2017)
- SEP *Incommensurable Values* (Andersson and Hsieh, rev. 2025)
- Segall's NDPR review of Stone (2011.12.19)

**A reprint or later edition verified [V-reprint]:**
- Arrow 1951 (Yale ed. 10.12987/9780300186987)
- Berlin 1969 (*Liberty* 2002)
- J. Cohen 1989 (MIT 1997 10.7551/mitpress/2324.003.0006)
- Dworkin 1981 "What is Equality? Part 2" (Routledge 2018 10.4324/9781315199795-7)
- Kleinberg, Mullainathan and Raghavan 2017 (ACM 2023 10.1145/3603195.3603206)
- Sen 1970 *Collective Choice and Social Welfare* (Elsevier 1979)
- Taurek 1977 (Routledge 2012 10.4324/9780203723746-41)
- Young 1990 (Princeton 2011 10.1515/9781400839902)

**Confirmed from secondary records only [S]:**
- Chang (ed.) 1997 (review records)
- Elster 1989 *Solomonic Judgements* (review records)
- Fraser 2008/2009 *Scales of Justice* (review record)
- Frankfurt 2015 *On Inequality* (review record)
- Goodin 1985 (review records)
- Goodwin 1992 (review record)
- Kolm 1997 *Modern Theories of Justice* (review record)
- Lippert-Rasmussen, *Luck Egalitarianism* (Routledge 2020 symposium record)
- Miller 1999 (review records)
- Nozick 1974 (reference record)
- Robeyns 2024 *Limitarianism* (review records)
- Roemer 1996 *Theories of Distributive Justice* (review records)
- Sen 1985 and Sen 1999 (review records)
- Stone 2007 *JPP* (citation in search results)
- Thibaut and Walker 1975
- Thompson 2002 (Choice record)
- Valentini 2011 *Economics and Philosophy* (PhilArchive)
- Walzer 1983 (reference records)
- "Lotteries, Queues, and Bottlenecks" (*OSPP* 2024)
- Segall 2025
- *The Anatomy of Justice* (OUP 2025)
- the CUP sufficientarian monograph (2025)
- Reiner (ed.) 2025 *Walzer and Justice*
- Knight 2025 "Basic-Needs Sufficientarianism" (*CRISPP*, seen in Crossref results; DOI truncated)

**From memory, not verified [M]:**
- Arnstein 1969
- Arrow and Raynaud 1986
- Crisp 2003 "Equality, Priority, and Compassion", *Ethics* 113 (rate-limited)
- d'Aspremont and Gevers 1977
- Dworkin 1983 "To Each His Own", *NYRB*
- Foley 1967
- GPS-Health (Norheim et al. 2014)
- Hammond 1976
- Kaplow and Shavell 2001
- Kittay 1999
- MacCoun 2005
- Parfit 1984
- Rawls 2001 *Restatement*
- Daniels and Sabin 2008, 2nd ed.
- Sunstein 1995

## Part II. Formal Methods of Fair Allocation

*Remark.* Part II's scope and conventions. Scope: the formal methods that say who is served first, what each receives and how much, when a resource is short, with particular weight on methods that are not optimisations (axiomatic rules, priority and reserve systems, lotteries, partial orders and dominance, procedures with checkable properties). The bibliographic markers are those of §0.3; records were verified against Crossref's API (the DOI resolves to the record as given), the arXiv API or record page, or the publisher's or package's own page, and licences were read this session on CRAN's, PyPI's and GitHub's pages. Statements of content say where they come from: *(text)* read in the paper this session; *(abstract)* read in the abstract this session; *(memory)* from memory, to be checked against the source before it is relied on. Theorem and equation numbers are given only where read this session *(text)*; all other numbering is flagged *(memory)*. The one paper read in full is Pathak, Sönmez, Ünver and Yenmez (arXiv:2008.00374v3), whose numbering may differ from the *Management Science* version; the equalshares.net explanation and Pabulib's format page were also read. Inside this Part, a bare section reference means this Part's own section.

Notation used throughout: N = {1,…,n} agents; a claims problem is (c, E) with c ∈ ℝ₊ᴺ, C = Σᵢ cᵢ, 0 ≤ E ≤ C; a rule R gives awards x = R(c,E) with 0 ≤ x ≤ c and Σ x = E.

### 1. Claims and Rationing Problems Have a Complete Axiomatic Map, and Its Rules Are Cheap and Exactly Checkable

#### 1.1 Takeaway
The claims (bankruptcy, rationing) problem has the most complete axiomatic theory of any allocation problem: a small set of rules (proportional, constrained equal awards, constrained equal losses, Talmud, random arrival, Piniles, the TAL family, Young's parametric family) is characterised by combinations of a short list of axioms (consistency, composition up and down, self-duality, claims truncation, minimal rights, no advantageous transfer, lower bounds), and every rule runs in O(n log n) with exact closed forms and published numerical examples, which makes it the natural first layer of an allocation library. Its extensions (priority classes, several issues, bipartite eligibility, indivisible units, baselines) cover most real rationing.

#### 1.2 Cited Findings

**Sources and surveys.**
- O'Neill, "A problem of rights arbitration from the Talmud", *Mathematical Social Sciences* 2(4):345–371, 1982 [V]. Introduces the claims problem from the Talmud's estate division, the random arrival rule and the associated coalitional game v(S) = max(0, E − Σ_{j∉S} cⱼ) whose Shapley value is random arrival *(memory)*. — [doi:10.1016/0165-4896(82)90029-4](https://doi.org/10.1016/0165-4896(82)90029-4)
- Aumann and Maschler, "Game theoretic analysis of a bankruptcy problem from the Talmud", *Journal of Economic Theory* 36(2):195–213, 1985 [V]. — [doi:10.1016/0022-0531(85)90102-4](https://doi.org/10.1016/0022-0531(85)90102-4)
- Young, "On dividing an amount according to individual claims or liabilities", *Mathematics of Operations Research* 12(3):398–414, 1987 [V]; Young, "Distributive justice in taxation", *JET* 44(2):321–335, 1988 [V]. — [doi:10.1287/moor.12.3.398](https://doi.org/10.1287/moor.12.3.398); [doi:10.1016/0022-0531(88)90007-5](https://doi.org/10.1016/0022-0531(88)90007-5)
- Thomson, "Axiomatic and game-theoretic analysis of bankruptcy and taxation problems: a survey", *Math. Soc. Sci.* 45(3):249–297, 2003 [V]; "… an update", *Math. Soc. Sci.* 74:41–59, 2015 [V]; *How to Divide When There Isn't Enough* (Cambridge University Press, Econometric Society Monographs), 2019 [V]. — [doi:10.1016/s0165-4896(02)00070-7](https://doi.org/10.1016/s0165-4896(02)00070-7); [doi:10.1016/j.mathsocsci.2014.09.002](https://doi.org/10.1016/j.mathsocsci.2014.09.002); [doi:10.1017/9781108161107](https://doi.org/10.1017/9781108161107)
- Thomson, "On the axiomatics of resource allocation: interpreting the consistency principle", *Economics and Philosophy* 28(3):385–421, 2012 [V]. Abstract: consistency is defined as agreement of the recommendation for each problem with that for each reduced problem obtained "by imagining some agents leaving with their assignments"; the paper disputes earlier fairness readings but argues it can be founded as "some" efficiency added to a post-application, efficiency-free expression of solidarity under population changes, and reviews readings as robustness, coherence and reinforcement *(abstract)*. — [doi:10.1017/s0266267112000296](https://doi.org/10.1017/s0266267112000296)
- Moulin, "Axiomatic cost and surplus sharing", *Handbook of Social Choice and Welfare* vol. 1, ch. 6, pp. 289–357, 2002 [V]; Moulin, *Fair Division and Collective Welfare*, MIT Press, 2003 [V]; Thomson, "Fair allocation rules", *Handbook of Social Choice and Welfare* vol. 2, pp. 393–506, 2011 [V]. — [doi:10.1016/s1574-0110(02)80010-8](https://doi.org/10.1016/s1574-0110(02)80010-8); [doi:10.7551/mitpress/2954.001.0001](https://doi.org/10.7551/mitpress/2954.001.0001); [doi:10.1016/s0169-7218(10)00021-3](https://doi.org/10.1016/s0169-7218(10)00021-3)

**Definitions of the standard rules** (all *(memory)*, standard across Thomson 2003/2019; the record of each source is [V] above or below).
- Proportional (PRO): xᵢ = λcᵢ with λ = E/C.
- Constrained equal awards (CEA): xᵢ = min(cᵢ, λ), λ chosen so that Σ xᵢ = E (Maimonides' rule).
- Constrained equal losses (CEL): xᵢ = max(0, cᵢ − λ), λ chosen so that Σ xᵢ = E.
- Talmud (T, Aumann–Maschler): if E ≤ C/2, x = CEA(c/2, E); otherwise x = c/2 + CEL(c/2, E − C/2), i.e. xᵢ = cᵢ − min(cᵢ/2, μ).
- Piniles (Pin): if E ≤ C/2, x = CEA(c/2, E); otherwise x = c/2 + CEA(c/2, E − C/2).
- Random arrival (RA, O'Neill): xᵢ = (1/n!) Σ_π min(cᵢ, max(0, E − Σ_{j ≺_π i} cⱼ)), the average over all orders of arrival of first-come-first-served; equal to the Shapley value of the bankruptcy game v(S) = max(0, E − Σ_{j∉S} cⱼ).
- Adjusted proportional (AP, Curiel–Maschler–Tijs): first minimal rights mᵢ = max(0, E − Σ_{j≠i} cⱼ), then PRO on the revised claims min(cᵢ − mᵢ, E − Σm) and the remainder E − Σm. — Curiel, Maschler and Tijs, "Bankruptcy games", *Zeitschrift für Operations Research* 31(5):A143–A159, 1987 [V] [doi:10.1007/bf02109593](https://doi.org/10.1007/bf02109593)
- Duality: Rᵈ(c,E) = c − R(c, C − E). PRO, T, RA and AP are self-dual; CEA and CEL are each other's dual *(memory)*.
- TAL family (Moreno-Ternero and Villar): for θ ∈ [0,1], Tᶿ(c,E) = CEA(θc, E) if E ≤ θC, and c − CEA((1−θ)c, C − E) otherwise; θ = 0 gives CEL, θ = ½ the Talmud, θ = 1 CEA *(memory)*. — Moreno-Ternero and Villar, "The TAL-family of rules for bankruptcy problems", *Social Choice and Welfare* 27(2):231–249, 2006 [V] [doi:10.1007/s00355-006-0121-3](https://doi.org/10.1007/s00355-006-0121-3)
- Parametric rules (Young 1987): R is parametric if there is f: [a,b] × ℝ₊ → ℝ₊, continuous and weakly increasing in λ, with f(a,c) = 0 and f(b,c) = c, such that xᵢ = f(λ, cᵢ) where λ solves Σᵢ f(λ, cᵢ) = E. PRO: f = λc on [0,1]; CEA: f = min(c, λ); CEL: f = max(0, c − 1/λ) (with a reparametrisation); T: f = min(c/2, λ) on the lower half and max(c/2, c − 1/λ) on the upper half *(memory)*. — [Young 1987](https://doi.org/10.1287/moor.12.3.398) [V]
- Hydraulic representation: CEA, CEL, T, Pin and others are "hydraulic" rules, water poured into vessels of claim-dependent shapes. — Kaminski, "'Hydraulic' rationing", *Math. Soc. Sci.* 40(2):131–155, 2000 [V] *(memory for content)* [doi:10.1016/s0165-4896(99)00045-1](https://doi.org/10.1016/s0165-4896(99)00045-1)

**The axioms** (definitions *(memory)*, as in Thomson 2003/2019).
- Equal treatment of equals; anonymity; order preservation in awards (cᵢ ≥ cⱼ ⇒ xᵢ ≥ xⱼ) and in losses (cᵢ ≥ cⱼ ⇒ cᵢ − xᵢ ≥ cⱼ − xⱼ); claims monotonicity; resource (estate) monotonicity; homogeneity; continuity.
- Consistency: for N′ ⊂ N, R(c_{N′}, Σ_{i∈N′} xᵢ) = x_{N′}; converse consistency; bilateral consistency.
- Composition down: for E′ < E, R(c, E′) = R(R(c,E), E′). Composition up: R(c, E) = R(c, E′) + R(c − R(c,E′), E − E′).
- Self-duality: R = Rᵈ.
- Claims truncation invariance: R(c, E) = R(min(c, E), E). Minimal rights first: R(c,E) = m + R(c − m, E − Σm). (These two are dual to each other.)
- No advantageous transfer / non-manipulability by merging or splitting claims.
- Lower bounds: securement, each claimant receives at least 1/n of her claim truncated at the estate. — Moreno-Ternero and Villar, "The Talmud rule and the securement of agents' awards", *Math. Soc. Sci.* 47(2):245–257, 2004 [V] *(memory for content: the Talmud rule satisfies securement; CEL does not)* [doi:10.1016/s0165-4896(03)00087-8](https://doi.org/10.1016/s0165-4896(03)00087-8)

**Characterisation theorems.**
- The Talmud rule is the unique consistent rule that agrees with the contested-garment principle for two claimants, and it is the nucleolus of the bankruptcy game *(memory; theorem numbering not checked)*. Numerical facts *(memory, but standard and reproducible)*: claims (100, 200, 300); estate 100 → (33⅓, 33⅓, 33⅓); estate 200 → (50, 75, 75); estate 300 → (50, 100, 150). Contested garment: claims 1 and ½ on an estate 1 → (¾, ¼). — [Aumann and Maschler 1985](https://doi.org/10.1016/0022-0531(85)90102-4) [V]
- Symmetry, continuity and consistency together characterise the parametric rules *(memory; Young 1987's theorem numbering not checked)*. — [Young 1987](https://doi.org/10.1287/moor.12.3.398) [V]
- CEA is characterised by equal treatment of equals, claims truncation invariance and composition up; by duality CEL by equal treatment of equals, minimal rights first and composition down *(memory)*. — Dagan, "New characterizations of old bankruptcy rules", *Social Choice and Welfare* 13(1):51–59, 1996 [V] [doi:10.1007/bf00179098](https://doi.org/10.1007/bf00179098)
- PRO is the only rule satisfying no advantageous transfer (for n ≥ 3) *(memory)*; non-manipulability by merging and splitting characterises proportionality and its generalisations. — Chun, "The proportional solution for rights problems", *Math. Soc. Sci.* 15(3):231–246, 1988 [V] [doi:10.1016/0165-4896(88)90009-1](https://doi.org/10.1016/0165-4896(88)90009-1); Ju, Miyagawa and Sakai, "Non-manipulable division rules in claim problems and generalizations", *JET* 132(1):1–26, 2007 [V] [doi:10.1016/j.jet.2005.08.003](https://doi.org/10.1016/j.jet.2005.08.003); Moulin, "Equal or proportional division of a surplus, and other methods", *IJGT* 16(3):161–186, 1987 [V] [doi:10.1007/bf01756289](https://doi.org/10.1007/bf01756289)
- Rules satisfying consistency, scale invariance, composition up and composition down are combinations of a priority order over classes of claimants with weighted PRO, CEA or CEL within classes; with equal treatment of equals only PRO, CEA and CEL remain *(memory)*. — Moulin, "Priority rules and other asymmetric rationing methods", *Econometrica* 68(3):643–684, 2000 [V] [doi:10.1111/1468-0262.00126](https://doi.org/10.1111/1468-0262.00126)
- A rule is a solution of the associated coalitional game (defined through v alone) if and only if it is invariant under truncation of claims at the estate *(memory)*. — [Curiel, Maschler and Tijs 1987](https://doi.org/10.1007/bf02109593) [V]
- Characterisations of PRO, CEA, CEL and RA compared axiom by axiom ("four classical solutions"). — Herrero and Villar, "The three musketeers: four classical solutions to bankruptcy problems", *Math. Soc. Sci.* 42(3):307–328, 2001 [V] *(content memory)* [doi:10.1016/s0165-4896(01)00075-0](https://doi.org/10.1016/s0165-4896(01)00075-0)
- Many axioms coincide on the claims domain. — Chun, "Equivalence of axioms for bankruptcy problems", *IJGT* 28(4):511–520, 1999 [V] [doi:10.1007/s001820050122](https://doi.org/10.1007/s001820050122)
- Two families (ICI and CIC) interpolating CEA, CEL, Talmud and reverse Talmud, and operators on rules (duality, claims truncation, attribution of minimal rights, convex combination) whose invariants are the axioms. — Thomson, "Two families of rules for the adjudication of conflicting claims", *SCW* 31(4):667–692, 2008 [V] [doi:10.1007/s00355-008-0302-3](https://doi.org/10.1007/s00355-008-0302-3); Thomson and Yeh, "Operators for the adjudication of conflicting claims", *JET* 143(1):177–198, 2008 [V] [doi:10.1016/j.jet.2007.12.005](https://doi.org/10.1016/j.jet.2007.12.005) *(content memory)*
- Weighted generalisations of the Talmud rule; asymmetric parametric rules (consistency and continuity without symmetry). — Hokari and Thomson, "Claims problems and weighted generalizations of the Talmud rule", *Economic Theory* 21(2–3):241–261, 2003 [V] [doi:10.1007/s00199-002-0314-7](https://doi.org/10.1007/s00199-002-0314-7); Stovall, "Asymmetric parametric division rules", *GEB* 84:87–110, 2014 [V] [doi:10.1016/j.geb.2013.11.006](https://doi.org/10.1016/j.geb.2013.11.006) *(content memory)*
- Lorenz comparisons: CEA's award vector Lorenz-dominates that of every other rule satisfying the claims bounds and CEL's is Lorenz-dominated by every other; the paper ranks nine rules *(memory; abstract not deposited at Crossref)*. — Bosmans and Lauwers, "Lorenz comparisons of nine rules for the adjudication of conflicting claims", *IJGT* 40(4):791–807, 2011 [V] [doi:10.1007/s00182-010-0269-z](https://doi.org/10.1007/s00182-010-0269-z)
- The uniform rule (single-peaked division) is the unique efficient, strategy-proof and anonymous (or envy-free) rule; its restriction to claims problems is CEA. — Sprumont, "The division problem with single-peaked preferences: a characterization of the uniform allocation rule", *Econometrica* 59(2):509, 1991 [V] [doi:10.2307/2938268](https://doi.org/10.2307/2938268); Schummer and Thomson, "Two derivations of the uniform rule and an application to bankruptcy", *Economics Letters* 55(3):333–337, 1997 [V] [doi:10.1016/s0165-1765(97)00079-7](https://doi.org/10.1016/s0165-1765(97)00079-7) *(content memory)*

**Axioms satisfied by the standard rules** (a summary table from memory of Thomson 2003/2019; to be regenerated by the library's own checks):

| Axiom | PRO | CEA | CEL | Talmud | RA | Piniles |
|---|---|---|---|---|---|---|
| Consistency | yes | yes | yes | yes | **no** | yes |
| Composition up | yes | yes | yes | no | no | no |
| Composition down | yes | yes | yes | no | no | no |
| Self-duality | yes | no (dual CEL) | no (dual CEA) | yes | yes | no |
| Claims truncation invariance | no | yes | no | yes | yes | yes |
| Minimal rights first | no | no | yes | yes | yes | no |
| Securement (1/n of truncated claim) | yes | yes | no | yes | yes (memory) | yes (memory) |
| Order preservation (awards, losses) | yes | yes | yes | yes | yes | awards yes |

**Extensions that matter for real rationing.**
- Priority classes: classes ordered by priority, weighted CEA within a class. — Flores-Szwagrzak, "Priority classes and weighted constrained equal awards rules for the claims problem", *JET* 160:36–55, 2015 [V] [doi:10.1016/j.jet.2015.08.008](https://doi.org/10.1016/j.jet.2015.08.008); [Moulin 2000](https://doi.org/10.1111/1468-0262.00126) [V] *(content memory)*
- Several issues (a claims matrix, agents × issues): two-stage rules (divide among issues, then within), the run-to-the-bank rule. — Calleja, Borm and Hendrickx, "Multi-issue allocation situations", *EJOR* 164(3):730–747, 2005 [V] [doi:10.1016/j.ejor.2003.10.042](https://doi.org/10.1016/j.ejor.2003.10.042); Lorenzo-Freire, Casas-Méndez and Hendrickx, "The two-stage constrained equal awards and losses rules for multi-issue allocation situations", *TOP* 18(2):465–480, 2010 (online 2009) [V] [doi:10.1007/s11750-009-0073-8](https://doi.org/10.1007/s11750-009-0073-8)
- Claims with a reference (an objective reference vector besides claims, e.g. a university's budget): extended bankruptcy model. — Pulido, Sánchez-Soriano and Llorca, "Game theory techniques for university management: an extended bankruptcy model", *Annals of OR* 109:129–142, 2002 [V] [doi:10.1023/a:1016395917734](https://doi.org/10.1023/a:1016395917734)
- Baselines (claims as one baseline among others) in a unifying framework. — Hougaard, Moreno-Ternero and Østerdal, "A unifying framework for the problem of adjudicating conflicting claims", working paper 2011 (SSRN) [V]; published in *Journal of Mathematical Economics* 48:107–114, 2012 [M]. [doi:10.2139/ssrn.1752204](https://doi.org/10.2139/ssrn.1752204)
- Eligibility on a bipartite graph (claimants can draw only on some resource types): the bipartite rationing problem, whose consistent "loss-calibrated" rules extend CEA, PRO and CEL; balancing supply and demand under bilateral constraints with an egalitarian (uniform) flow found by max-flow. — Moulin and Sethuraman, "The bipartite rationing problem", *Operations Research* 61(5):1087–1100, 2013 [V] [doi:10.1287/opre.2013.1199](https://doi.org/10.1287/opre.2013.1199); Bochet, İlkılıç, Moulin and Sethuraman, "Balancing supply and demand under bilateral constraints", *Theoretical Economics* 7(3):395–423, 2012 [V] [doi:10.3982/te893](https://doi.org/10.3982/te893) *(content memory)*
- Indivisible units (ventilators, dwellings): balanced allocation methods for claims problems with indivisibilities. — Herrero and Martínez, "Balanced allocation methods for claims problems with indivisibilities", working paper 2006 (SSRN) [V]; *SCW* 30:603–617, 2008 [M]. [doi:10.2139/ssrn.944416](https://doi.org/10.2139/ssrn.944416)
- The same axioms structure serial cost sharing and surplus sharing. — Moulin and Shenker, "Serial cost sharing", *Econometrica* 60(5):1009, 1992 [V] [doi:10.2307/2951537](https://doi.org/10.2307/2951537)

**Reference implementations** (licences read this session from CRAN's package pages).
- R package `ClaimsProblems` ("Analysis of Conflicting Claims"), version 1.0.0, licence GPL-3 [V] — [CRAN](https://cran.r-project.org/web/packages/ClaimsProblems/index.html). Its rule list (PRO, CEA, CEL, Talmud, RA, AP, Piniles, etc.) was not read this session.
- R package `GameTheory` ("Cooperative Game Theory"), version 2.7.1, licence GPL (≥ 2) [V]; it includes bankruptcy rules and the nucleolus *(memory for content)* — [CRAN](https://cran.r-project.org/web/packages/GameTheory/index.html).
- R package `CoopGame`, version 0.2.2, licence GPL-2 [V] — [CRAN](https://cran.r-project.org/web/packages/CoopGame/index.html).
- All three are copyleft; they can serve as independent oracles for fixtures (run once, record values with source and date), not as code to adopt into an MIT library (inference).

#### 1.3 Inferences
- Every standard rule is a water-filling with one parameter λ: sort claims, walk the breakpoints, solve a linear equation on the last segment, so O(n log n) in exact rationals; RA is the exception (an average over n! orders), computable as the Shapley value of v, exactly for small n and by a pseudo-polynomial dynamic programme over integer claims (inference).
- Two checks come free from what the infrastructure already holds: the Talmud rule must equal nomos's nucleolus of the bankruptcy game, and RA must equal nomos's Shapley value of the same game; both are independent computations of the same number.
- The axiom table is itself an artefact worth computing: each cell is a verdict (holds, or fails with a counterexample instance), so the library can regenerate Thomson's table rather than assert it.
- Moulin's 2000 theorem is the formal bridge between rationing and priority: once composition and consistency are demanded, priority classes are the only admissible asymmetry. This is the mathematics behind "to which entity first".

#### 1.4 Gaps
- Theorem numbers for Aumann–Maschler, Young 1987, Dagan 1996, Moulin 2000 and Thomson 2019 were not read in the texts this session; the characterisations are stated from memory and should be checked before being cited by number.
- Bosmans and Lauwers 2011: their exact Lorenz ranking of the nine rules (including where the Talmud and RA sit relative to PRO) was not read.

### 2. Priority and Reserve Systems Are Characterised by Three Axioms and Implemented by Deferred Acceptance with a Precedence Order

#### 2.1 Takeaway
A reserve system divides q identical units into categories c with capacities r_c, each with its own priority order (which may encode a different ethical value); Pathak, Sönmez, Ünver and Yenmez prove that a matching complies with eligibility, is non-wasteful and respects priorities if and only if it is a cutoff equilibrium (Theorem 1) and if and only if it is produced by deferred acceptance for some artificial preference over categories (Proposition 1); sequential processing by a precedence order is one such matching (Proposition 2), the order matters (Proposition 3, Example 1), and a "smart" reserve matching also maximises the reserves given to their beneficiaries (Proposition 5, Theorem A.2). This is the formal answer to "who first" that does not collapse several values into one score.

#### 2.2 Cited Findings

**The general theory (all *(text)*, read in arXiv:2008.00374v3 this session).**
- Record: Pathak, Sönmez, Ünver and Yenmez, "Fair allocation of vaccines, ventilators and antiviral treatments: leaving no ethical value behind in healthcare rationing", *Management Science* 70(6):3999–4036, 2024 [V]; arXiv 2008.00374, v1 2 Aug 2020, v3 1 Jul 2023 [V]. Abstract: the traditional priority system's limitations are "largely" driven by "a restrictive feature" (a single priority order); the paper formulates a reserve system "interfering only with" that feature *(abstract)*. — [doi:10.1287/mnsc.2022.00930](https://doi.org/10.1287/mnsc.2022.00930); [arXiv:2008.00374](https://arxiv.org/abs/2008.00374)
- Primitives (§3.1): patients I; q identical units; categories C with r_c reserved, Σ_c r_c = q; units are not attached to categories ("for the purposes of accounting"); each category has a linear priority order π_c over I ∪ {∅}; i is eligible for c iff i π_c ∅. Outcome (§3.2): a matching μ: I → C ∪ {∅} with |μ⁻¹(c)| ≤ r_c, which records through which category a unit is received (needed, e.g., for India's rule that SC-reserved units go to SC candidates with lower merit than any open-category recipient).
- Axioms (§3.3): Definition 1, compliance with eligibility (μ(i) = c ⇒ i π_c ∅); Definition 2, non-wastefulness (i π_c ∅ and μ(i) = ∅ ⇒ |μ⁻¹(c)| = r_c); Definition 3, respect of priorities (μ(i) = c and μ(i′) = ∅ ⇒ i π_c i′). The authors "see these three axioms as minimal requirements for a reserve system".
- Cutoff equilibrium (§3.4): a cutoff f_c ∈ I ∪ {∅} with f_c π_c ∅ (weakly); budget set Bᵢ(f) = {c : i π_c f_c (weakly)}; (f, μ) is a cutoff equilibrium if (1a) μ(i) ∈ Bᵢ(f) ∪ {∅}, (1b) Bᵢ(f) ≠ ∅ ⇒ μ(i) ∈ Bᵢ(f), (2) |μ⁻¹(c)| < r_c ⇒ f_c = ∅. **Theorem 1**: a matching satisfies the three axioms iff it is supported by some cutoff vector as a cutoff equilibrium. **Lemma 1**: the supporting cutoffs form an interval between the minimum and maximum cutoff vectors given by equations (1) and (2); the maximum cutoff is the one announced in practice.
- **Proposition 1** (§3.5): a matching satisfies the three axioms iff it is the outcome of individual-proposing deferred acceptance for some profile of artificial strict preferences of patients over the categories they are eligible for.
- Sequential reserve matching (§3.6): an order of precedence ▷ over C; categories processed in order, each taking its r_c highest-priority eligible remaining patients. **Proposition 2**: it is DA-induced from preferences that rank categories by ▷. **Proposition 3**: moving a category one step earlier weakly raises its maximum equilibrium cutoff ("the earlier a category is processed … the more selective it becomes").
- Baseline priority (§3.7): an unreserved category u with the baseline order; preferential categories rank their beneficiaries Iᶜ first, then everyone by the baseline. Soft reserves: everyone eligible everywhere (pandemic case); hard reserves: only beneficiaries eligible for a preferential category (H-1B case). Minimum guarantee = preferential category processed before open; over-and-above = open first (§3.8, citing Hafalir–Yenmez–Yildirim 2013 and Dur et al. 2018).
- **Example 1**: q = 6 units, one each in u and five preferential categories, seven patients i1 ≻ … ≻ i7 in the baseline; two precedence orders give matched sets {i1,i2,i3,i4,i5,i7} and {i1,i2,i3,i4,i5,i6}; processing category c earlier *increases* its matched beneficiaries from {i1,i3} to {i1,i3,i6}, so the comparative statics of Dur et al. fail in general.
- **Proposition 4**: with at most five categories and each patient a beneficiary of at most one preferential category, processing a preferential category later weakly enlarges its set of matched beneficiaries (under soft reserves).
- Smart reserves (Appendix B): **Example A.2**: hard reserves, two patients, sequential processing with open first leaves the reserved unit idle and is Pareto dominated. **Definition 4**: maximal in beneficiary assignment (μ maximises Σ_{c≠u} |μ⁻¹(c) ∩ Iᶜ|). The smart reserve matching algorithm, parameter n ∈ {0,…,r_u} (unreserved units processed first): Step 0 computes the maximum number n_b of beneficiaries matchable to their own categories (a bipartite maximum cardinality matching, Hopcroft–Karp); Step 1 walks patients in baseline order and commits a unit to each only if n_b is not reduced, by solving weighted assignment problems (Hungarian algorithm); Step 2 fills the rest by baseline priority. **Lemma A.2**: all smart reserve matchings with the same n match the same patients. **Proposition 5**: every smart reserve matching complies with eligibility, is non-wasteful, respects priorities and is maximal in beneficiary assignment. **Theorem A.2**: among cutoff equilibrium matchings maximal in beneficiary assignment, n = r_u gives the most selective and n = 0 the least selective unreserved cutoff. Footnote 16: n = 0 coincides with the meritorious horizontal algorithm of Sönmez and Yenmez (2022). Polynomial time.
- Adoption (§4, *(text)*): Pennsylvania's antiviral allocation; the NASEM framework recommended a 10 percent over-and-above reserve for hard-hit areas; a table of jurisdictions (e.g. Richmond & Henrico, April 2021, 10% over-and-above for hard-hit areas; Washington DC, December 2020, 25% minimum guarantee and 20% over-and-above); Massachusetts' monoclonal antibodies; Oregon's crisis care guidance.

**India, schools and slot-specific priorities.**
- Sönmez and Yenmez, "Affirmative action in India via vertical, horizontal, and overlapping reservations", *Econometrica* 90(3):1143–1176, 2022 [V]. Abstract: vertical reservations are "set asides", horizontal reservations "minimum guarantees"; a mechanism mandated by the Supreme Court in 1995 has anomalies; the 2SMG mechanism resolves them, is characterised by desiderata from Indian law, and is endorsed by *Saurav Yadav v. State of UP* (2020) *(abstract)*. — [doi:10.3982/ecta17788](https://doi.org/10.3982/ecta17788)
- Kominers and Sönmez, "Matching with slot-specific priorities: theory", *Theoretical Economics* 11(2):683–710, 2016 [V]: each institution's slots have their own priorities and a precedence order; the order of precedence was "formally introduced" here per Pathak et al. *(text of Pathak et al.)*; the cumulative offer mechanism is stable and strategy-proof for applicants although choice functions need not be substitutable *(memory)*. — [doi:10.3982/te1839](https://doi.org/10.3982/te1839)
- Hafalir, Yenmez and Yildirim, "Effective affirmative action in school choice", *TE* 8(2):325–363, 2013 [V]: minority reserves instead of quotas, reserves weakly better for minorities *(memory)*. — [doi:10.3982/te1135](https://doi.org/10.3982/te1135)
- Echenique and Yenmez, "How to control controlled school choice", *AER* 105(8):2679–2694, 2015 [V] *(content memory: axiomatic characterisation of diversity-respecting choice rules)*. — [doi:10.1257/aer.20130929](https://doi.org/10.1257/aer.20130929)
- Dur, Kominers, Pathak and Sönmez, "Reserve design: unintended consequences and the demise of Boston's walk zones", *JPE* 126(6):2457–2479, 2018 [V]: with one preferential category, the later a preferential unit is processed the weakly better for its beneficiaries *(as summarised in Pathak et al.'s text)*. — [doi:10.1086/699974](https://doi.org/10.1086/699974)
- Dur, Pathak and Sönmez, "Explicit vs. statistical targeting in affirmative action: theory and evidence from Chicago's exam schools", *JET* 187:104996, 2020 [V]. — [doi:10.1016/j.jet.2020.104996](https://doi.org/10.1016/j.jet.2020.104996)
- Ehlers, Hafalir, Yenmez and Yildirim, "School choice with controlled choice constraints: hard bounds versus soft bounds", *JET* 153:648–683, 2014 [V]. — [doi:10.1016/j.jet.2014.03.004](https://doi.org/10.1016/j.jet.2014.03.004)
- Kamada and Kojima, "Efficient matching under distributional constraints: theory and applications", *AER* 105(1):67–99, 2015 [V]: existing mechanisms under regional caps are inefficient and unstable; a proposed mechanism does better on efficiency, stability and incentives *(abstract)*. — [doi:10.1257/aer.20101552](https://doi.org/10.1257/aer.20101552)
- Aygün and Bó, "College admission with multidimensional privileges: the Brazilian affirmative action case", *AEJ: Micro* 13(3):1–28, 2021 [V]: Brazil's 2012 mandate makes privilege claims strategic and can reject high-achieving minority students while admitting low-achieving majority students; evidence consistent with this unfairness in more than 49 percent of programmes; a fair, incentive-compatible mechanism is proposed *(abstract)*. — [doi:10.1257/mic.20170364](https://doi.org/10.1257/mic.20170364)
- Abdulkadiroğlu and Sönmez, "School choice: a mechanism design approach", *AER* 93(3):729–747, 2003 [V]. — [doi:10.1257/000282803322157061](https://doi.org/10.1257/000282803322157061)

**Healthcare rationing beyond reserves.**
- Aziz and Brandl, "Efficient, fair, and incentive-compatible healthcare rationing", *EC '21*, pp. 103–104 [V] *(content memory: a rule for reserves with overlapping categories that is efficient, fair in the reserve sense and strategy-proof, via maximum-flow-like constructions)*. — [doi:10.1145/3465456.3467531](https://doi.org/10.1145/3465456.3467531)
- Grigoryan, "Effective, fair and equitable pandemic rationing", SSRN working paper 2020 [V]. — [doi:10.2139/ssrn.3646539](https://doi.org/10.2139/ssrn.3646539)
- Ethical inputs to the priorities: Persad, Wertheimer and Emanuel, "Principles for allocation of scarce medical interventions", *Lancet* 373(9661):423–431, 2009 [V] *(content memory: classifies principles as treating people equally (lottery, first-come-first-served), favouring the worst off (sickest first, youngest first), maximising total benefits (save most lives, prognosis), promoting social usefulness; proposes a "complete lives system")*; Emanuel et al., "Fair allocation of scarce medical resources in the time of Covid-19", *NEJM* 382(21):2049–2055, 2020 [V] *(content memory: maximise benefits; prioritise health workers; do not allocate on a first-come, first-served basis; random selection among patients with similar prognoses)*. — [doi:10.1016/s0140-6736(09)60137-9](https://doi.org/10.1016/s0140-6736(09)60137-9); [doi:10.1056/nejmsb2005114](https://doi.org/10.1056/nejmsb2005114)

#### 2.3 Inferences
- The three axioms (eligibility, non-wastefulness, respect of priorities) are exactly checkable on any output, and Theorem 1 gives a certificate: a cutoff vector that a person can compare with her own priority. This is a natural "verdict with evidence" for an allocation library: return the matching, the maximum cutoffs (eq. (1)), and the check of each axiom.
- Proposition 1 makes nomos's deferred acceptance the engine: a reserve system is DA on the patient side with categories as many-to-one institutions; a precedence order fixes the artificial preferences (Proposition 2).
- Because the order of precedence has distributional consequences that are not monotone in general (Example 1), the library should present the outcomes over all precedence orders (or the range n ∈ {0,…,r_u} of the smart reserve family, Theorem A.2) as a set, not one answer.
- The smart reserve algorithm needs a bipartite maximum matching and an assignment solver; calx's Hungarian method and morphe's flows already cover both.

#### 2.4 Gaps
- The Management Science version's numbering may differ from arXiv v3; numbers above are those of arXiv v3.
- Sönmez–Yenmez's 2SMG definition and its characterisation theorem were not read in the text; only the abstract.
- Aziz and Brandl's EC '21 extended abstract content was not read.

### 3. Sequencing, Picking and Queues: Serial Dictatorship, Probabilistic Serial and the Fairness of Waiting

#### 3.1 Takeaway
For indivisible objects without money, the sequential rules (serial dictatorship, random serial dictatorship, picking sequences, the probabilistic serial "eating" rule) are characterised by incentive and fairness axioms and trade ex post simplicity against ex ante fairness: RSD is strategy-proof but not ordinally efficient; PS is ordinally (sd-)efficient and sd-envy-free but only weakly strategy-proof, and no rule has all three for n ≥ 4. For waiting lists, recent work shows that first-come-first-served, lotteries and waitlists with penalties differ in targeting, matching and waste, and that randomisation can reduce the misallocation that fluctuating waits cause.

#### 3.2 Cited Findings
- Serial dictatorship is characterised by strategy-proofness, non-bossiness and neutrality *(memory)*. — Svensson, "Strategy-proof allocation of indivisible goods", *SCW* 16(4):557–567, 1999 [V] [doi:10.1007/s003550050160](https://doi.org/10.1007/s003550050160)
- Random serial dictatorship (random priority) equals the core from random endowments (TTC from a uniformly random initial assignment) *(memory)*. — Abdulkadiroğlu and Sönmez, "Random serial dictatorship and the core from random endowments in house allocation problems", *Econometrica* 66(3):689, 1998 [V] [doi:10.2307/2998580](https://doi.org/10.2307/2998580)
- Probabilistic serial (PS): agents "eat" their most preferred available object at unit speed simultaneously; the shares eaten form the random assignment. PS is sd-efficient and sd-envy-free; RP is sd-strategy-proof and ex post efficient but not sd-efficient; no mechanism is sd-efficient, sd-strategy-proof and treats equals equally when n ≥ 4 *(memory; theorem numbers not checked)*. — Bogomolnaia and Moulin, "A new solution to the random assignment problem", *JET* 100(2):295–328, 2001 [V] [doi:10.1006/jeth.2000.2710](https://doi.org/10.1006/jeth.2000.2710)
- Characterisations of PS: by sd-efficiency, sd-envy-freeness and bounded invariance *(memory)* — Bogomolnaia and Heo, "Probabilistic assignment of objects: characterizing the serial rule", *JET* 147(5):2072–2082, 2012 [V] [doi:10.1016/j.jet.2012.05.013](https://doi.org/10.1016/j.jet.2012.05.013); by ordinal fairness and non-wastefulness *(memory)* — Hashimoto, Hirata, Kesten, Kurino and Ünver, "Two axiomatic approaches to the probabilistic serial mechanism", *TE* 9(1):253–277, 2014 [V] [doi:10.3982/te1010](https://doi.org/10.3982/te1010)
- PS with indifferences (extended PS by parametric max-flow) *(memory)*. — Katta and Sethuraman, "A solution to the random assignment problem on the full preference domain", *JET* 131(1):231–250, 2006 [V] [doi:10.1016/j.jet.2005.05.001](https://doi.org/10.1016/j.jet.2005.05.001)
- PS becomes strategy-proof in large markets *(memory)* — Kojima and Manea, "Incentives in the probabilistic serial mechanism", *JET* 145(1):106–123, 2010 [V] [doi:10.1016/j.jet.2009.09.002](https://doi.org/10.1016/j.jet.2009.09.002); RP and PS are asymptotically equivalent — Che and Kojima, *Econometrica* 78:1625–1672, 2010 [M] (record not found at Crossref this session).
- Computing RP's assignment probabilities is #P-complete *(memory for the exact class)*. — Aziz, Brandt and Brill, "The computational complexity of random serial dictatorship", *Economics Letters* 121(3):341–345, 2013 [V] [doi:10.1016/j.econlet.2013.09.006](https://doi.org/10.1016/j.econlet.2013.09.006); Saban and Sethuraman, "The complexity of computing the random priority allocation matrix", *Math. of OR* 40(4):1005–1014, 2015 [V] [doi:10.1287/moor.2014.0707](https://doi.org/10.1287/moor.2014.0707)
- Dichotomous preferences (acceptable or not, as in eligibility for a scarce treatment): the egalitarian (leximin) random matching, built on the Gallai–Edmonds decomposition, is efficient, Lorenz-dominant among efficient random matchings and group strategy-proof *(memory)*. — Bogomolnaia and Moulin, "Random matching under dichotomous preferences", *Econometrica* 72(1):257–279, 2004 [V] [doi:10.1111/j.1468-0262.2004.00483.x](https://doi.org/10.1111/j.1468-0262.2004.00483.x)
- Picking sequences: strict and balanced alternation and other sequences. — Brams and Taylor, *Fair Division: From Cake-Cutting to Dispute Resolution*, Cambridge University Press, 1996 [V] [doi:10.1017/cbo9780511598975](https://doi.org/10.1017/cbo9780511598975); Brams and King, "Efficient fair division: help the worst off or avoid envy?", *Rationality and Society* 17(4):387–421, 2005 [V] [doi:10.1177/1043463105058317](https://doi.org/10.1177/1043463105058317). Bouveret and Lang's elicitation-free protocol (IJCAI 2011) and Kalinowski, Narodytska and Walsh's result that alternation maximises expected Borda-like welfare (IJCAI 2013) [M] (no DOI; not verified).
- Round-robin gives EF1 for additive valuations *(memory, as stated in Caragiannis et al. 2019)*; a weighted generalisation of round-robin produces strongly weighted-EF1 allocations in polynomial time *(abstract)*. — Chakraborty, Igarashi, Suksompong and Zick, "Weighted envy-freeness in indivisible item allocation", *ACM TEAC* 9(3):1–39, 2021 [V] [doi:10.1145/3457166](https://doi.org/10.1145/3457166); Chakraborty, Schmidt-Kraepelin and Suksompong, "Picking sequences and monotonicity in weighted fair division", *Artificial Intelligence* 301:103578, 2021 [V] [doi:10.1016/j.artint.2021.103578](https://doi.org/10.1016/j.artint.2021.103578)
- Fair queuing and probabilistic allocation of a server's time; queueing problems with transfers, where the Shapley value is characterised *(memory)*. — Moulin and Stong, "Fair queuing and other probabilistic allocation methods", *Math. of OR* 27(1):1–30, 2002 [V] [doi:10.1287/moor.27.1.1.336](https://doi.org/10.1287/moor.27.1.1.336); Maniquet, "A characterization of the Shapley value in queueing problems", *JET* 109(1):90–103, 2003 [V] [doi:10.1016/s0022-0531(02)00036-4](https://doi.org/10.1016/s0022-0531(02)00036-4); Chun, "A pessimistic approach to the queueing problem", *Math. Soc. Sci.* 51(2):171–181, 2006 [V] [doi:10.1016/j.mathsocsci.2005.08.002](https://doi.org/10.1016/j.mathsocsci.2005.08.002)
- Waiting lists for social housing and organs: "With private values, all agents prefer first-come first-served to the lottery, but waste is lower at the lottery. With common values, all agents prefer first-come first-served to any other mechanism, and waste is minimized at the lottery" *(abstract)*. — Bloch and Cantala, "Dynamic assignment of objects to queuing agents", *AEJ: Micro* 9(1):88–122, 2017 [V] [doi:10.1257/mic.20150183](https://doi.org/10.1257/mic.20150183)
- Waiting times act like prices; their random fluctuation causes misallocation and welfare loss; "a simple randomized assignment policy can reduce misallocation and increase welfare" *(abstract)*. — Leshno, "Dynamic matching in overloaded waiting lists", *AER* 112(12):3876–3910, 2022 [V] [doi:10.1257/aer.20201111](https://doi.org/10.1257/aer.20201111)
- Independent lotteries per item and a waitlist in which agents lose priority on rejecting an offer "lead to identical outcomes in equilibrium"; a common lottery for all items improves matching; with many low-need agents there is "an unavoidable trade-off between matching and targeting" *(abstract)*. — Arnosti and Shi, "Design of lotteries and wait-lists for affordable housing allocation", *Management Science* 66(6):2291–2307, 2020 [V] [doi:10.1287/mnsc.2019.3311](https://doi.org/10.1287/mnsc.2019.3311)
- Public housing with choice: Waldinger, "Targeting in-kind transfers through market design: a revealed preference analysis of public housing allocation", *AER* 111(8):2660–2696, 2021 [V] *(content memory: giving applicants choice improves match quality but weakens targeting of the neediest)*. — [doi:10.1257/aer.20190516](https://doi.org/10.1257/aer.20190516)
- Deceased-donor kidneys: Agarwal, Ashlagi, Rees, Somaini and Waldinger, "Equilibrium allocations under alternative waitlist designs: evidence from deceased donor kidneys", NBER WP 25607, 2019 [V]; *Econometrica* 89(1):37–76, 2021 [M]. — [doi:10.3386/w25607](https://doi.org/10.3386/w25607)
- Point systems designed under fairness constraints: a data-driven method designs a point system "that ranks patients according to some priority criteria, e.g., waiting time, medical urgency", approximately maximising life-years subject to chosen fairness constraints; one design matching the US proposal's form gives "an 8% increase in extra life-year gains" *(abstract)*. — Bertsimas, Farias and Trichakis, "Fairness, efficiency, and flexibility in organ allocation for kidney transplantation", *Operations Research* 61(1):73–87, 2013 [V] [doi:10.1287/opre.1120.1138](https://doi.org/10.1287/opre.1120.1138)
- Information asymmetry in kidney queues: candidates choose a queue by reporting a type; with fluid approximations the paper solves the division of organ supply among queues under aggregate-utility and max-min objectives; truth-telling is induced by giving better kidneys to those who wait longer *(abstract)*. — Su and Zenios, "Recipient choice can address the efficiency-equity trade-off in kidney transplantation: a mechanism design model", *Management Science* 52(11):1647–1660, 2006 [V] [doi:10.1287/mnsc.1060.0541](https://doi.org/10.1287/mnsc.1060.0541)
- US organ allocation is moving from rank-ordered classifications to a "composite allocation score" summing medical urgency, post-transplant survival, candidate biology, patient access and placement efficiency; lung continuous distribution took effect on 9 March 2023 [S]. — [OPTN, continuous distribution of lungs](https://optn.transplant.hrsa.gov/policies-bylaws/a-closer-look/continuous-distribution/continuous-distribution-lung); [OPTN ethical considerations of continuous distribution](https://optn.transplant.hrsa.gov/media/4778/ethical_considerations_of_continuous_distribution_in_organ_allocation.pdf)

#### 3.3 Inferences
- Sequential rules are the computational answer to "which entity first, and then which": a priority order (deterministic, or random as in RSD) plus a rule of what each takes at its turn. PS is the continuous version of the same idea and is the bridge to claims rules: on a single object type with unit demands PS degenerates to equal division, and with indifferences (eligibility) its extension is a max-flow computation (inference).
- Point systems (composite scores) are scalarisations; the reserve theory of §2 and the sequential-dominance theory of §7 are the formal alternatives for when values must not be traded at a fixed rate.
- A library should report both ex ante properties (of the random assignment matrix: sd-efficiency, sd-envy-freeness) and ex post properties (of each realised assignment), since RSD and PS differ exactly there.

#### 3.4 Gaps
- No primary source on social-housing waiting lists in Europe or Japan (points schemes, banding) was read; Thakral's Pittsburgh study is a working paper not verified.
- Kidney point values and lung score weights of the final OPTN policies were not found.

### 4. Fair Division Supplies Graded Fairness Notions (EF, PROP, EF1, EFX, MMS) and Rules (CEEI, Maximum Nash Welfare, Leximin) with Known Existence Frontiers

#### 4.1 Takeaway
For divisible goods, competitive equilibrium from equal incomes (equivalently maximum Nash welfare for additive utilities, computable by the Eisenberg–Gale convex programme) is envy-free and Pareto efficient. For indivisible goods envy-freeness can fail, and the field works with relaxations ordered by strength: EF1 always exists with Pareto optimality (maximum Nash welfare gives both), EFX exists for two and three additive agents, for four additive agents with at most nine goods (machine-checked, 2026) and for binary valuations, but a 2026 counterexample with three agents and eight goods shows it need not exist under submodular valuations, and it fails for additive chores with four or more agents, while the additive-goods case with four or more agents is still open; MMS can fail to exist and the best guarantee is just above 3/4. Entitlements (weights) and chores change the picture again.

#### 4.2 Cited Findings

**Definitions** (standard; *(memory)* unless noted). For additive vᵢ over a set M of items and allocation A = (A₁,…,Aₙ):
- Proportionality: vᵢ(Aᵢ) ≥ vᵢ(M)/n. Envy-freeness: vᵢ(Aᵢ) ≥ vᵢ(Aⱼ) for all j.
- EF1: for every j with Aⱼ ≠ ∅ there is g ∈ Aⱼ with vᵢ(Aᵢ) ≥ vᵢ(Aⱼ ∖ {g}) (Lipton et al. 2004; named in Budish 2011). EFX: the same for *every* g ∈ Aⱼ (Caragiannis et al.; Plaut and Roughgarden).
- Maximin share: MMSᵢ = max over partitions (P₁,…,Pₙ) of M of min_k vᵢ(P_k) (Budish 2011); an allocation is α-MMS if vᵢ(Aᵢ) ≥ α·MMSᵢ.
- Nash social welfare: Πᵢ vᵢ(Aᵢ); maximum Nash welfare (MNW) maximises it (first maximising the number of agents with positive value).
- CEEI: a competitive equilibrium in which all agents have equal budgets (Foley 1967 [M]; Varian 1974). Hylland–Zeckhauser: the same with fake money over probability shares.

**Divisible goods and markets.**
- Varian, "Equity, envy, and efficiency", *JET* 9(1):63–91, 1974 [V]: equal-income competitive equilibria are envy-free and efficient ("fair") *(memory)*. — [doi:10.1016/0022-0531(74)90075-1](https://doi.org/10.1016/0022-0531(74)90075-1)
- Eisenberg and Gale, "Consensus of subjective probabilities: the pari-mutuel method", *Annals of Mathematical Statistics* 30(1):165–168, 1959 [V]: the convex programme max Σ Bᵢ log uᵢ(x) over feasible x yields the equilibrium of a linear Fisher market; with equal budgets this is CEEI and MNW *(memory)*. — [doi:10.1214/aoms/1177706369](https://doi.org/10.1214/aoms/1177706369)
- Mixed manna (goods and bads): competitive division with equal incomes exists; with bads the competitive allocations can be several, related to the critical points of the Nash product on the efficient frontier *(memory)*. — Bogomolnaia, Moulin, Sandomirskiy and Yanovskaya, "Competitive division of a mixed manna", *Econometrica* 85(6):1847–1871, 2017 [V] [doi:10.3982/ecta14564](https://doi.org/10.3982/ecta14564)
- Moulin, "Fair division in the internet age", *Annual Review of Economics* 11:407–441, 2019 [V]: the message space is the critical design parameter; covers single-peaked, Leontief, ordinal, dichotomous and additive domains; some results implemented on Spliddit *(abstract)*. — [doi:10.1146/annurev-economics-080218-025559](https://doi.org/10.1146/annurev-economics-080218-025559)

**Indivisible goods.**
- Caragiannis, Kurokawa, Moulin, Procaccia, Shah and Wang, "The unreasonable fairness of maximum Nash welfare", *ACM TEAC* 7(3):1–32, 2019 [V]: MNW selects EF1 allocations (with Pareto optimality), approximates MMS "in theory and—even more so—in practice", is computationally hard but scales in a non-trivial implementation, and underlies its deployment on Spliddit *(abstract)*. The MMS approximation factor 2/(1+√(4n−3)) is *(memory)*. — [doi:10.1145/3355902](https://doi.org/10.1145/3355902)
- Maximising Nash welfare with indivisible items is APX-hard — Lee, *Information Processing Letters* 122:17–20, 2017 [V] [doi:10.1016/j.ipl.2017.01.012](https://doi.org/10.1016/j.ipl.2017.01.012); constant-factor approximation — Cole and Gkatzelis, *SIAM J. Computing* 47(3):1211–1236, 2018 [V] [doi:10.1137/15m1053682](https://doi.org/10.1137/15m1053682) (the factor 2e^{1/e} ≈ 2.89 is *(memory)*).
- EF1 and Pareto optimality in pseudo-polynomial time by a market algorithm *(memory)*. — Barman, Krishnamurthy and Vaish, "Finding fair and efficient allocations", *EC '18*, pp. 557–574 [V] [doi:10.1145/3219166.3219176](https://doi.org/10.1145/3219166.3219176)
- Lipton, Markakis, Mossel and Saberi, "On approximately fair allocations of indivisible goods", *EC '04*, pp. 125–131 [V]: envy bounded by one good (the envy-cycle procedure) *(memory)*. — [doi:10.1145/988772.988792](https://doi.org/10.1145/988772.988792)
- Budish, "The combinatorial assignment problem: approximate competitive equilibrium from equal incomes", *JPE* 119(6):1061–1103, 2011 [V]: A-CEEI with budgets differing slightly; approximate market clearing; satisfies envy bounded by a single good and the (n+1)-maximin share guarantee *(memory)*. — [doi:10.1086/664613](https://doi.org/10.1086/664613). Deployed as Course Match at Wharton: Budish, Cachon, Kessler and Othman, SSRN 2015 [V], *Operations Research* 65(2):314–336, 2017 [M] — [doi:10.2139/ssrn.2635243](https://doi.org/10.2139/ssrn.2635243)
- MMS: "such allocations may not exist, but allocations guaranteeing each player 2/3 of the above value always exist" (additive) *(abstract)*. — Kurokawa, Procaccia and Wang, "Fair enough: guaranteeing approximate maximin shares", *JACM* 65(2):1–27, 2018 [V] [doi:10.1145/3140756](https://doi.org/10.1145/3140756). A tight negative example (three agents; best possible factor 39/40 *(memory)*) — Feige, Sapir and Tauber, *WINE 2021*, LNCS, 2022 [V] [doi:10.1007/978-3-030-94676-0_20](https://doi.org/10.1007/978-3-030-94676-0_20); the 3/4 barrier broken (3/4 + 3/3836 *(memory)*) — Akrami and Garg, *SODA 2024*, pp. 74–91 [V] [doi:10.1137/1.9781611977912.4](https://doi.org/10.1137/1.9781611977912.4).
- EFX: "an EFX allocation always exists for three agents" (additive), constructively; and a partial EFX allocation can have higher Nash welfare than any complete EFX allocation, falsifying a conjecture of Caragiannis et al. *(abstract)*. — Chaudhury, Garg and Mehlhorn, "EFX exists for three agents", *JACM* 71(1):1–27, 2024 [V] [doi:10.1145/3616009](https://doi.org/10.1145/3616009). EFX exists for two agents with general valuations and for identical valuations *(memory)* — Plaut and Roughgarden, "Almost envy-freeness with general valuations", *SIAM J. Discrete Math.* 34(2):1039–1068, 2020 [V] [doi:10.1137/19m124397x](https://doi.org/10.1137/19m124397x). EFX with charity (at most n−1 goods unallocated, nobody envies the pool) *(memory)* — Chaudhury, Kavitha, Mehlhorn and Sgouritsa, "A little charity guarantees almost envy-freeness", *SODA 2020*, pp. 2658–2672 [V] [doi:10.1137/1.9781611975994.162](https://doi.org/10.1137/1.9781611975994.162). EFX exists for binary valuations — Bu, Song and Yu, LNCS 2023 [V] [doi:10.1007/978-3-031-39344-0_19](https://doi.org/10.1007/978-3-031-39344-0_19).
- **EFX in 2026: it need not exist beyond additive valuations.** Akrami, Mackenzie, Mayorov, Mehlhorn, Srinivas, Suzuki and Weidenbach, "EFX need not exist", arXiv:2604.18216 (v1 20 Apr 2026, v4 current) [V]: a counterexample "with three agents and eight goods", symmetric under simultaneous permutation of agents and good types, realisable with weighted-coverage (hence submodular) valuations, extending to n agents and m goods "for all n ≥ 3 and m ≥ n+5"; for three agents eight goods is the minimum ("every instance with three agents and at most seven goods admits an EFX allocation under arbitrary monotone valuations"); α-EFX need not exist for α > 2^{−1/6} ≈ 0.891 (subadditive) or α > 0.958 (submodular); a first version was found by SAT/SMT solving *(abstract)*. — [arXiv:2604.18216](https://arxiv.org/abs/2604.18216)
- **Additive goods remain open at n ≥ 4, with machine-checked progress.** Alkassar, Fouz and Mehlhorn, "Complete EFX allocations exist for four additive agents and up to nine goods", arXiv:2608.08590 (9 Aug 2026) [V]: every instance with four agents, additive non-negative valuations and at most nine goods has a complete EFX allocation; this passes the previous frontier m ≤ n+3; the proof combines hand-proven reduction lemmas with "a machine-verified certificate corpus" (linear-arithmetic unsatisfiability verdicts re-derived by an independent certifier) *(abstract)*. — [arXiv:2608.08590](https://arxiv.org/abs/2608.08590). An AAAI 2025 paper also states four-agent existence "remains open, even for additive valuations" [S] ([AAAI, "EF2X exists for four agents"](https://ojs.aaai.org/index.php/AAAI/article/view/33480)); Epoch AI lists EFX for goods as an open problem [S] ([Epoch AI](https://epoch.ai/frontiermath/open-problems/efx)).
- **Additive chores: EFX fails.** He and Tao, "EFX for additive chores: nonexistence, Pareto incompatibility, and bi-valued existence", arXiv:2606.08872 (7 Jun 2026, v2) [V]: for every n ≥ 4 there is a tri-valued additive chores instance with no EFX allocation (three chore types, two agent types, both tight); for bi-valued instances, for every n ≥ 4 there are instances in which no EFX allocation is Pareto-optimal; an EFX allocation is guaranteed for n = 4 (in the bi-valued setting, as the abstract's context reads) *(abstract)*. — [arXiv:2606.08872](https://arxiv.org/abs/2606.08872). Earlier non-existence for chores with non-additive costs: Christoforidis and Santorinaios, "On the pursuit of EFX for chores: non-existence and approximations", arXiv:2406.10752, 2024 [V] *(content memory)* — [arXiv:2406.10752](https://arxiv.org/abs/2406.10752). Bi-valued chores fair and efficient: Garg, Murhekar and Qin, arXiv:2110.09601, 2021 [V] — [arXiv:2110.09601](https://arxiv.org/abs/2110.09601).
- arXiv records of the earlier papers [V]: Chaudhury–Garg–Mehlhorn arXiv:2002.05119; Akrami–Garg arXiv:2307.07304; Feige–Sapir–Tauber arXiv:2104.04977; Amanatidis et al. arXiv:2208.08782 (authors listed: Amanatidis, Aziz, Birmpas, Filos-Ratsikas, Li, Moulin, Voudouris, …).
- A scale of fairness criteria for additive preferences: CEEI ⇒ envy-freeness ⇒ min-max fair share ⇒ proportionality ⇒ max-min fair share *(memory)*. — Bouveret and Lemaître, "Characterizing conflicts in fair division of indivisible goods using a scale of criteria", *Autonomous Agents and Multi-Agent Systems* 30(2):259–290, 2016 (online 2015) [V] [doi:10.1007/s10458-015-9287-3](https://doi.org/10.1007/s10458-015-9287-3)
- Ordinal notions (possible and necessary envy-freeness, sd-envy-freeness for bundles). — Aziz, Gaspers, Mackenzie and Walsh, "Fair assignment of indivisible objects under ordinal preferences", *Artificial Intelligence* 227:71–92, 2015 [V] [doi:10.1016/j.artint.2015.06.002](https://doi.org/10.1016/j.artint.2015.06.002)
- Goods and chores together (EF1 for mixed items via a double round-robin) *(memory)*. — Aziz, Caragiannis, Igarashi and Walsh, "Fair allocation of indivisible goods and chores", *AAMAS journal* 36(1), 2022 (online 2021) [V] [doi:10.1007/s10458-021-09532-8](https://doi.org/10.1007/s10458-021-09532-8)
- Binary and dichotomous valuations: MNW with a suitable tie-breaking is group-strategy-proof and fair for binary additive valuations *(memory)* — Halpern, Procaccia, Psomas and Shah, "Fair division with binary valuations: one rule to rule them all", *WINE 2020*, LNCS [V] [doi:10.1007/978-3-030-64946-3_26](https://doi.org/10.1007/978-3-030-64946-3_26); for submodular valuations with dichotomous marginals a deterministic truthful mechanism outputs a Lorenz-dominating allocation, hence EFX and MNW, and with random priorities is ex ante envy-free *(abstract)* — Babaioff, Ezra and Feige, "Fair and truthful mechanisms for dichotomous valuations", *AAAI* 35(6):5119–5126, 2021 [V] [doi:10.1609/aaai.v35i6.16647](https://doi.org/10.1609/aaai.v35i6.16647)
- Leximin in deployment: for allocating unused classrooms to charter schools, the randomised leximin mechanism "is proportional, envy-free, Pareto optimal, and group strategyproof" in a general framework, and a worst-case 4-approximation to the maximum number of classrooms *(abstract)*. — Kurokawa, Procaccia and Shah, "Leximin allocations in the real world", *ACM TEAC* 6(3–4):1–24, 2018 [V] [doi:10.1145/3274641](https://doi.org/10.1145/3274641)
- Minimum inequality or minimum envy as explicit objectives (an optimisation alternative). — Cornilly, Puccetti, Rüschendorf and Vanduffel, "Fair allocation of indivisible goods with minimum inequality or minimum envy", *EJOR* 297(2):741–752, 2022 [V] [doi:10.1016/j.ejor.2021.06.020](https://doi.org/10.1016/j.ejor.2021.06.020)
- Surveys: Amanatidis, Aziz, Birmpas, Filos-Ratsikas, Li, Moulin, Voudouris and Wu, "Fair division of indivisible goods: recent progress and open questions", *Artificial Intelligence* 322:103965, 2023 [V] (first six authors confirmed by Crossref) [doi:10.1016/j.artint.2023.103965](https://doi.org/10.1016/j.artint.2023.103965); Brandt, Conitzer, Endriss, Lang and Procaccia (eds.), *Handbook of Computational Social Choice*, CUP 2016 [V] [doi:10.1017/cbo9781107446984.002](https://doi.org/10.1017/cbo9781107446984.002)

**Entitlements (weighted fair division).**
- Weighted EF1 in strong and weak forms; for additive valuations a Pareto optimal and strongly WEF1 allocation exists and is computable in pseudo-polynomial time; maximum *weighted* Nash welfare may fail strong WEF1 but satisfies weak WEF1; weighted round-robin gives strong WEF1; for two agents adjusted winner gives strong WEF1 and Pareto optimality *(abstract)*. — [Chakraborty et al. 2021](https://doi.org/10.1145/3457166) [V]
- Weighted maximin share. — Farhadi et al., "Fair allocation of indivisible goods to asymmetric agents", *JAIR* 64:1–20, 2019 [V] [doi:10.1613/jair.1.11291](https://doi.org/10.1613/jair.1.11291)
- Fair shares for arbitrary entitlements (the AnyPrice share and truncated proportional share) *(memory)*. — Babaioff, Ezra and Feige, "Fair-share allocations for agents with arbitrary entitlements", *EC '21*, p. 127 [V] [doi:10.1145/3465456.3467559](https://doi.org/10.1145/3465456.3467559)
- Competitive equilibrium with indivisible goods and generic (unequal) budgets. — Babaioff, Nisan and Talgam-Cohen, *Math. of OR* 46(1):382–403, 2021 [V] [doi:10.1287/moor.2020.1062](https://doi.org/10.1287/moor.2020.1062)

**Cake cutting.**
- Steinhaus's proportional division (1948) [M; the related "Sur la division pragmatique", *Econometrica* 17:315, 1949, is V — [doi:10.2307/1907319](https://doi.org/10.2307/1907319)]; Dubins and Spanier, "How to cut a cake fairly", *American Mathematical Monthly* 68(1):1, 1961 [V] [doi:10.2307/2311357](https://doi.org/10.2307/2311357); Even and Paz, "A note on cake cutting", *Discrete Applied Mathematics* 7(3):285–296, 1984 [V] (O(n log n) cuts for proportionality, *(memory)*) [doi:10.1016/0166-218x(84)90005-2](https://doi.org/10.1016/0166-218x(84)90005-2); Aziz and Mackenzie, "A discrete and bounded envy-free cake cutting protocol for any number of agents", *FOCS 2016*, pp. 416–427 [V] [doi:10.1109/focs.2016.52](https://doi.org/10.1109/focs.2016.52); Procaccia, "Cake cutting: not just child's play", *CACM* 56(7):78–87, 2013 [V] [doi:10.1145/2483852.2483870](https://doi.org/10.1145/2483852.2483870); Segal-Halevi and Sziklai, "Monotonicity and competitive equilibrium in cake-cutting", *Economic Theory* 68(2):363–401, 2019 (online 2018) [V] [doi:10.1007/s00199-018-1128-6](https://doi.org/10.1007/s00199-018-1128-6).

**Software.**
- Spliddit: Goldman and Procaccia, "Spliddit: unleashing fair division algorithms", *ACM SIGecom Exchanges* 13(2):41–46, 2015 [V]. [doi:10.1145/2728732.2728738](https://doi.org/10.1145/2728732.2728738). No open repository or licence for Spliddit's code was found this session.
- `fairpy` (erelsgl/fairpy), "an open-source library of fair division algorithms in Python", licence GPL-3.0 (GitHub API) [V]; its page names iterated maximum matching, leximin for divisible goods, cut-and-choose and round-robin, and its README steers users to the newer `fairpyx` (ariel-research/fairpyx) for allocation with constraints such as course seats [V page] — [GitHub erelsgl/fairpy](https://github.com/erelsgl/fairpy). `fairpyx` on PyPI (v0.1, "Fair division algorithms in Python") declares no licence field [V] — [PyPI fairpyx](https://pypi.org/project/fairpyx/). Note: the PyPI package named `fairpy` (v0.1.0, MIT) is an unrelated machine-learning fairness library [V] — [PyPI fairpy](https://pypi.org/project/fairpy/).
- `matching` (daffidwilde/matching), "a package for solving matching games", MIT, v1.4.3 [V] — [PyPI matching](https://pypi.org/project/matching/); R `matchingR` (GPL ≥ 2, v2.0.0) and `matchingMarkets` (GPL ≥ 2, v1.0-5) [V] — [CRAN matchingR](https://cran.r-project.org/web/packages/matchingR/index.html); [CRAN matchingMarkets](https://cran.r-project.org/web/packages/matchingMarkets/index.html).

#### 4.3 Inferences
- The fairness notions form a partial order of strength (Bouveret–Lemaître's scale; EFX ⇒ EF1; EF ⇒ EFX; PROP ⇒ … for additive): the natural output of a library is the *set* of notions an allocation satisfies, with a counterexample for each it fails, rather than a single "fairness score".
- MNW is the single rule with the strongest package (EF1 + PO, MMS approximation, CEEI in the divisible limit) but it is an optimisation; for the user's concern it is better presented as one rule among several (round-robin, envy-cycle, leximin) whose properties are reported side by side.
- For exact verification in small instances, brute force over all nᵐ allocations is cheap for m ≤ 10–12 and n ≤ 4, which is enough to check every existence claim above on random instances and to reproduce the published counterexamples (inference).

#### 4.4 Gaps
- The 2026 EFX results were read in their abstracts only; the counterexample instance of "EFX need not exist" (three agents, eight goods) and the certificate corpus of Alkassar–Fouz–Mehlhorn were not inspected. Whether EFX exists for additive goods with n ≥ 4 and arbitrary m remains open as of November 2026, on these sources.
- The exact instances of the MMS counterexamples (Kurokawa et al.; Feige–Sapir–Tauber's 3 × 9 example) were not extracted; they are needed as fixtures.

### 5. Lotteries Are a Principled Answer When Claims Are Equal, and Any Fractional Assignment Can Be Implemented as One

#### 5.1 Takeaway
Philosophy (Broome, Stone, Taurek, Timmermann) justifies lotteries when claims are equal or incommensurable and weighted lotteries when they are unequal; economics shows that ex ante fairness (equal chances, sd-envy-freeness) and ex post fairness (EF1 of each realisation) can be obtained together in some settings; the Birkhoff–von Neumann theorem and its generalisation by Budish, Che, Kojima and Milgrom say exactly which constraint structures let any fractional assignment be implemented by a lottery over feasible deterministic ones. Partial lotteries (peer review to a quality threshold, then a draw) are in use in research funding.

#### 5.2 Cited Findings
- Birkhoff–von Neumann: every doubly stochastic matrix is a convex combination of permutation matrices (Birkhoff 1946; von Neumann 1953) [M]; a decomposition is found greedily by repeatedly extracting a perfect matching on the support (O(n²) matchings) *(memory)*.
- Generalisation: an expected allocation is implementable as a lottery over integral allocations satisfying all constraints when the constraint sets form a bihierarchy (two laminar families) *(memory; theorem number not checked)*. — Budish, Che, Kojima and Milgrom, "Designing random allocation mechanisms: theory and applications", *AER* 103(2):585–623, 2013 [V] [doi:10.1257/aer.103.2.585](https://doi.org/10.1257/aer.103.2.585)
- When some constraints are treated as goals, "any expected allocation that satisfies all the constraints and goals can be implemented by randomizing among nearby integer allocations that satisfy all the hard constraints exactly and the goals approximately"; applied to improve ex post properties of RSD *(abstract)*. — Akbarpour and Nikzad, "Approximate random allocation mechanisms", *Review of Economic Studies* 87(6):2473–2510, 2020 [V] [doi:10.1093/restud/rdz066](https://doi.org/10.1093/restud/rdz066)
- Hylland and Zeckhauser, "The efficient allocation of individuals to positions", *JPE* 87(2):293–314, 1979 [V]: a pseudo-market in probability shares with equal budgets gives ex ante efficient and envy-free random assignments *(memory)*. — [doi:10.1086/260757](https://doi.org/10.1086/260757)
- Ex ante and ex post together: an allocation lottery that is ex ante envy-free and ex post EF1 *(memory for the exact package)*. — Freeman, Shah and Vaish, "Best of both worlds: ex-ante and ex-post fairness in resource allocation", *EC '20*, pp. 21–22 [V] [doi:10.1145/3391403.3399537](https://doi.org/10.1145/3391403.3399537); arXiv:2005.14122 [V] [arXiv](https://arxiv.org/abs/2005.14122)
- Universal Pareto dominance (stochastic dominance) as the robust efficiency notion for lotteries across all plausible utility functions *(memory)*. — Aziz, Brandl and Brandt, "Universal Pareto dominance and welfare for plausible utility functions", *EC '14*, pp. 331–332 [V] [doi:10.1145/2600057.2602866](https://doi.org/10.1145/2600057.2602866)
- Ex ante equality matters ethically (the "Diamond critique" of expected-utility utilitarianism: society may strictly prefer the lottery that gives each an equal chance) *(memory)*. — Diamond, "Cardinal welfare, individualistic ethics, and interpersonal comparison of utility: comment", *JPE* 75(5):765–766, 1967 [V] [doi:10.1086/259353](https://doi.org/10.1086/259353)
- Fairness requires a lottery when claims are equal, and partial satisfaction of claims through chances *(memory)*. — Broome, "Selecting people randomly", *Ethics* 95(1):38–55, 1984 [V] [doi:10.1086/292596](https://doi.org/10.1086/292596)
- Lotteries are just because they exclude bad reasons ("sanitising" decisions) *(memory)*. — Stone, "Why lotteries are just", *Journal of Political Philosophy* 15(3):276–295, 2007 [V] [doi:10.1111/j.1467-9760.2006.00274.x](https://doi.org/10.1111/j.1467-9760.2006.00274.x); Stone, *The Luck of the Draw: The Role of Lotteries in Decision Making*, OUP 2011 [V] (chapter "Why lotteries?") [doi:10.1093/acprof:oso/9780199756100.003.0001](https://doi.org/10.1093/acprof:oso/9780199756100.003.0001)
- The numbers problem: Taurek's "Should the numbers count?" (*Philosophy & Public Affairs* 6:293–316, 1977) [M] argued for a coin toss between saving one and saving many; Timmermann's individualist lottery gives each person a chance while letting numbers count *(memory)* — Timmermann, "The individualist lottery: how people count, but not their numbers", *Analysis* 64(2):106–112, 2004 [V] [doi:10.1093/analys/64.2.106](https://doi.org/10.1093/analys/64.2.106); a reply, Kavka, "The numbers should count", *Philosophical Studies* 36(3):285–294, 1979 [V] [doi:10.1007/bf00372632](https://doi.org/10.1007/bf00372632)
- School-choice lotteries: ex ante stability and the fractional deferred acceptance *(memory)*. — Kesten and Ünver, "A theory of school-choice lotteries", *TE* 10(2):543–595, 2015 [V] [doi:10.3982/te1558](https://doi.org/10.3982/te1558)
- Sortition with fair selection probabilities: algorithms choose representative panels "while selecting individuals with probabilities as close to equal as mathematically possible"; one implementation "has already been used to select more than 40 citizens' assemblies" *(abstract)*. — Flanigan, Gölz, Gupta, Hennig and Procaccia, "Fair algorithms for selecting citizens' assemblies", *Nature* 596(7873):548–552, 2021 [V] [doi:10.1038/s41586-021-03788-6](https://doi.org/10.1038/s41586-021-03788-6)
- Partial randomisation in grant funding: a "modified lottery in which peer review is used to identify the most meritorious proposals, from which funded applications are selected by lottery" *(abstract)* — Fang and Casadevall, "Research funding: the case for a modified lottery", *mBio* 7(2), 2016 [V] [doi:10.1128/mbio.00422-16](https://doi.org/10.1128/mbio.00422-16). The Health Research Council of New Zealand's Explorer Grants are the first major government lottery; of 126 survey respondents (39% response), 63% favoured randomisation and 25% opposed *(abstract)* — Liu, Choy, Clarke, Barnett, Blakely and Pomeroy, "The acceptability of using a lottery to allocate research funding: a survey of applicants", *Research Integrity and Peer Review* 5(1), 2020 [V] [doi:10.1186/s41073-019-0089-z](https://doi.org/10.1186/s41073-019-0089-z). The Swiss National Science Foundation uses a lottery among proposals of equal merit in a middle group [S] ([RUG library blog](https://rug.nl/library/open-access/blog/using-lotteries-to-allocate-research-funding-perspectives-from-switzerland)). Heyard, Ott, Salanti and Egger, "Rethinking the funding line at the Swiss National Science Foundation: Bayesian ranking and lottery", arXiv:2102.09958 (v1 19 Feb 2021) [V]: the SNSF "introduced a lottery for proposals in the middle group of good but not excellent proposals"; a Bayesian hierarchical model estimates each proposal's expected rank with a credible interval; a provisional funding line is set from expected ranks and budget, and proposals whose intervals overlap the line enter a lottery *(abstract)* — [arXiv:2102.09958](https://arxiv.org/abs/2102.09958); RoRI's *Experimental Research Funders' Handbook* has case studies of the SNSF, Volkswagen Foundation, FWF and HRC NZ [S] ([FWF-hosted PDF](https://www.fwf.ac.at/fileadmin/Website/publications/Publikationen/FWF-Selbstevaluation/RoRI/The_experimental_research_funders_handbook_2nd_ed.pdf)).

#### 5.3 Inferences
- A lottery is two objects: the fractional assignment (what each is entitled to in expectation) and its decomposition (how it is drawn). Both can be returned and both verified exactly in rationals: the decomposition must sum back to the matrix, each component must be feasible.
- Weighted lotteries are the probabilistic form of claims rules: a fractional assignment computed by PRO or CEA on "chances" and then decomposed (inference). This links §1 and §5.
- Partial randomisation is a threshold rule followed by a lottery: it is the cleanest example of a method that admits it cannot rank finely and says so. Heyard et al.'s version makes the admission quantitative: the lottery set is exactly the proposals whose credible interval of rank crosses the funding line, which tyche's Bayesian tools could compute (inference).

#### 5.4 Gaps
- No primary evaluation of the SNSF lottery's outcomes was found.
- Freeman–Shah–Vaish's exact theorem (which ex ante and ex post properties hold together, for which valuations) was not read.

### 6. Participatory Budgeting Has Proportional Rules with Checkable Axioms, Public Instances and Open-Source Software

#### 6.1 Takeaway
For indivisible public projects with costs and a budget, utilitarian greedy (the common default) is not proportional; the Method of Equal Shares (MES) gives each voter an equal share of the budget, buys projects whose supporters can pay for them, and satisfies Extended Justified Representation for approval ballots with arbitrary costs and EJR up to one project for additive utilities, in polynomial time; it is not exhaustive (needs a completion), not Pareto optimal and not committee monotone. Sequential Phragmén satisfies proportional justified representation and priceability. Pabulib supplies real instances, and pabutools implements the rules and axioms.

#### 6.2 Cited Findings
- Peters, Pierczyński and Skowron, "Proportional participatory budgeting with additive utilities", arXiv:2008.13276 (v1 30 Aug 2020, v2 16 Oct 2022), NeurIPS 2021 [V for arXiv; venue M]: MES satisfies EJR for arbitrary costs and approval utilities and EJR up to one project for additive valuations, computable in polynomial time; PAV cannot be extended to arbitrary costs; Full Justified Representation is satisfiable by a less natural, more expensive rule *(abstract)*. — [arXiv:2008.13276](https://arxiv.org/abs/2008.13276)
- MES definition: each voter starts with budget bᵢ = L/n; a project p is ρ-affordable if Σᵢ min(bᵢ, ρ·uᵢ(p)) = cost(p); select the project affordable for the least ρ (minimising the largest payment of a supporter), deduct payments, repeat until none is affordable [S] ([Wikipedia, "Method of equal shares"](https://en.wikipedia.org/wiki/Method_of_equal_shares)). For approval, uᵢ(p) = 1; with "cost utilities" uᵢ(p) = cost(p) [S] (same source). The equalshares.net explanation states the same in words, with an "effective vote count" and equal splitting that minimises the maximum payment; it notes the method "may finish … without having spent all the available budget" and refers to a completion step; worked Example 1: €1,100, 11 voters, €100 each, winners bike path (€700), new playground (€200), library for kids (€100) [V page] ([equalshares.net explanation](https://equalshares.net/explanation/), maintained by D. Peters and P. Skowron, updated 2025-02-23).
- MES fails Pareto optimality and committee monotonicity; completions are the utilitarian method and adjusting the initial budget upward [S] ([Wikipedia](https://en.wikipedia.org/wiki/Method_of_equal_shares)).
- Uses: Wieliczka, Poland, 2023 ("Green Million", 1 million złoty); Aarau, Switzerland, 2023 (*Stadtidee*); Winterthur and Assen (years not given) [S] ([Wikipedia](https://en.wikipedia.org/wiki/Method_of_equal_shares)).
- Justified representation for committees (JR, EJR). — Aziz, Brill, Conitzer, Elkind, Freeman and Walsh, "Justified representation in approval-based committee voting", *SCW* 48(2):461–485, 2017 [V] [doi:10.1007/s00355-016-1019-3](https://doi.org/10.1007/s00355-016-1019-3). EJR for PB with costs *(memory)*: a group S is T-cohesive if |S| ≥ cost(T)·n/L and every member approves all of T; EJR requires some member of every T-cohesive group to have utility at least |T| (approval, cardinality satisfaction).
- Phragmén's sequential method satisfies PJR *(memory)*. — Brill, Freeman, Janson and Lackner, "Phragmén's voting methods and justified representation", *AAAI* 31(1), 2017 [V] [doi:10.1609/aaai.v31i1.10598](https://doi.org/10.1609/aaai.v31i1.10598)
- For PB with several satisfaction functions, EJR-based proportionality for the two common ones (number of approved projects; their total cost) cannot be achieved simultaneously, but a PJR-based notion can; a strengthening of priceability is satisfied by MES and Phragmén's sequential rule *(abstract)*. — Brill, Forster, Lackner, Maly and Peters, "Proportionality in approval-based participatory budgeting", *AAAI* 37(5):5524–5531, 2023 [V] [doi:10.1609/aaai.v37i5.25686](https://doi.org/10.1609/aaai.v37i5.25686)
- Priceability and Rule X (MES for committees) *(memory)*. — Peters and Skowron, "Proportionality and the limits of welfarism", *EC '20*, pp. 793–794 [V] [doi:10.1145/3391403.3399465](https://doi.org/10.1145/3391403.3399465)
- The core of PB and Lindahl equilibrium *(memory)*. — Fain, Goel and Munagala, "The core of the participatory budgeting problem", *WINE 2016*, LNCS, pp. 384–399 [V] [doi:10.1007/978-3-662-54110-4_27](https://doi.org/10.1007/978-3-662-54110-4_27)
- A framework of approval-based budgeting rules (greedy and optimal variants by approvals or cost satisfaction). — Talmon and Faliszewski, *AAAI* 33(01):2181–2188, 2019 [V] [doi:10.1609/aaai.v33i01.33012181](https://doi.org/10.1609/aaai.v33i01.33012181)
- Surveys and books: Aziz and Shah, "Participatory budgeting: models and approaches", in *Pathways Between Social Science and Computational Social Science*, Springer, 2021 (Crossref: 2020), pp. 215–236 [V] [doi:10.1007/978-3-030-54936-7_10](https://doi.org/10.1007/978-3-030-54936-7_10); Lackner and Skowron, *Multi-Winner Voting with Approval Preferences*, SpringerBriefs, 2023 [V via its chapter record, 2022] [doi:10.1007/978-3-031-09016-5_1](https://doi.org/10.1007/978-3-031-09016-5_1); Lackner and Skowron, "Consistent approval-based multi-winner rules", *JET* 192:105173, 2021 [V] [doi:10.1016/j.jet.2020.105173](https://doi.org/10.1016/j.jet.2020.105173); Rey, Schmidt and Maly, "The (computational) social choice take on indivisible participatory budgeting", arXiv:2303.00621 (v1 1 Mar 2023, v9) [V] [arXiv](https://arxiv.org/abs/2303.00621); Fairstein, Benadè and Gal, "Participatory budgeting designs for the real world", *AAAI* 37(5):5633–5640, 2023 [V] [doi:10.1609/aaai.v37i5.25699](https://doi.org/10.1609/aaai.v37i5.25699); Maly, Rey, Endriss and Lackner, "Fairness in participatory budgeting via equality of resources", arXiv:2205.07517, 2022 [V] [arXiv](https://arxiv.org/abs/2205.07517).
- Recent MES variants [V records; content from titles only]: Papasotiropoulos, Pishbin, Skibski, Skowron and Wąs, "Method of equal shares with bounded overspending", arXiv:2409.15005 (2024) — [arXiv](https://arxiv.org/abs/2409.15005); Baychkov, Brill and Peters, "Utilitarian guarantees for the method of equal shares", arXiv:2511.20929 (25 Nov 2025) — [arXiv](https://arxiv.org/abs/2511.20929).
- Pabulib: the `.pb` format is one UTF-8 file per instance with sections META (e.g. description, country, unit, instance, num_projects, num_votes, budget, rule, vote_type, min_length, max_length, date_begin, date_end), PROJECTS (project_id, cost, category …) and VOTES (voter_id, age, sex, vote); vote types approval, ordinal, cumulative, scoring [V page] ([pabulib.org/format](https://pabulib.org/format)). The format page states no licence and no instance count. The library paper: Stolicki, Szufa and Talmon, "Pabulib: a participatory budgeting library", arXiv:2012.06539 (1 Dec 2020) [V] — [arXiv](https://arxiv.org/abs/2012.06539).
- Software (licences read this session): `pabutools` (COMSOC-Community/pabutools), "a complete set of tools to work with participatory budgeting elections", licence GPL-3.0 (GitHub API), PyPI v1.2.3 with classifier GPLv3+ [V] — [GitHub](https://github.com/COMSOC-Community/pabutools), [PyPI](https://pypi.org/project/pabutools/); `abcvoting` (martinlackner/abcvoting), approval-based committee rules (PAV, Phragmén's methods, Rule X/MES among them *(memory)*), MIT, v2.19.2 [V] — [GitHub](https://github.com/martinlackner/abcvoting), [PyPI](https://pypi.org/project/abcvoting/); `pref_voting`, MIT, v1.18.2 [V] — [PyPI](https://pypi.org/project/pref_voting/).

#### 6.3 Inferences
- MES is a procedure, not an optimum: it is a sequence of purchases with a certificate (each voter's payments), and EJR is checkable on outputs by enumeration for small instances; this suits the user's demand for methods with checkable properties.
- Pabulib's instances with the rule actually used (`rule` in META) and pabutools' outputs give ready fixtures for an implementation of greedy, MES with completion and sequential Phragmén.
- calx's knapsack and greedy submodular maximisation already cover the utilitarian baselines (max approval subject to budget), against which proportional rules should be shown, not replaced.

#### 6.4 Gaps
- The theorem numbers in Peters–Pierczyński–Skowron were not read.
- Pabulib's data licence was not found on its format page or elsewhere this session; before redistributing instances as fixtures, the licence must be established (unknown). pabutools is GPL-3.0, so it can serve as an external oracle but not as code to incorporate into an MIT library (inference).
- The contents of the 2024–2025 MES variants (bounded overspending; utilitarian guarantees) were not read.

### 7. Social Welfare Orderings and Dominance Give Partial Orders Robust to the Degree of Inequality Aversion

#### 7.1 Takeaway
Instead of choosing one social welfare function, the dominance criteria give the comparisons on which *every* function of a class agrees: Lorenz dominance (all symmetric, concave or S-concave functions at equal means), generalised Lorenz dominance (all increasing concave additive functions, unequal means), sequential generalised Lorenz dominance for households of different needs, and multidimensional dominance. These are partial orders; their maximal elements are the defensible allocations. The parametric families (Atkinson, Kolm, generalised Gini, prioritarian) then index the remaining choices by one parameter, and Fleurbaey–Tungodden show that Pareto, aggregation and non-aggregation cannot all be had.

#### 7.2 Cited Findings
- Atkinson, "On the measurement of inequality", *JET* 2(3):244–263, 1970 [V]: equally distributed equivalent y_e(ε) = [(1/n) Σ yᵢ^{1−ε}]^{1/(1−ε)} (ε ≠ 1; geometric mean at ε = 1), index A_ε = 1 − y_e/μ; at equal means, Lorenz dominance is equivalent to higher welfare for every increasing strictly concave additively separable welfare function *(memory; equation numbers not checked)*. — [doi:10.1016/0022-0531(70)90039-6](https://doi.org/10.1016/0022-0531(70)90039-6)
- Extension of Atkinson's theorem to all symmetric quasi-concave (S-concave) welfare functions *(memory)*. — Dasgupta, Sen and Starrett, "Notes on the measurement of inequality", *JET* 6(2):180–187, 1973 [V] [doi:10.1016/0022-0531(73)90033-1](https://doi.org/10.1016/0022-0531(73)90033-1)
- Kolm's absolute (translation-invariant) equally distributed equivalent y_e = −(1/κ) log[(1/n) Σ e^{−κyᵢ}] *(memory)*. — Kolm, "Unequal inequalities. I", *JET* 12(3):416–442, 1976 [M]; "Unequal inequalities. II", *JET* 13(1):82–111, 1976 [V] [doi:10.1016/0022-0531(76)90068-5](https://doi.org/10.1016/0022-0531(76)90068-5)
- Generalised Gini (rank-dependent) welfare: y_e = Σᵢ wᵢ y₍ᵢ₎ with incomes sorted ascending and weights decreasing in rank, e.g. the single-parameter S-Gini family *(memory)*. — Donaldson and Weymark, "A single-parameter generalization of the Gini indices of inequality", *JET* 22(1):67–86, 1980 [V] [doi:10.1016/0022-0531(80)90065-4](https://doi.org/10.1016/0022-0531(80)90065-4); Weymark, "Generalized Gini inequality indices", *Math. Soc. Sci.* 1(4):409–430, 1981 [V] [doi:10.1016/0165-4896(81)90018-4](https://doi.org/10.1016/0165-4896(81)90018-4)
- Generalised Lorenz curve GL(p) = μ·L(p); GL dominance ⇔ second-order stochastic dominance ⇔ higher welfare for all increasing concave additive functions, allowing unequal means *(memory)*. — Shorrocks, "Ranking income distributions", *Economica* 50(197):3, 1983 [V] [doi:10.2307/2554117](https://doi.org/10.2307/2554117)
- Poverty orderings: dominance in the FGT index P_α over all poverty lines up to a bound ⇔ stochastic dominance of order α+1 *(memory)*. — Foster and Shorrocks, "Poverty orderings", *Econometrica* 56(1):173, 1988 [V] [doi:10.2307/1911846](https://doi.org/10.2307/1911846)
- Differences in needs: with households ordered from most to least needy, welfare dominance for all utilitarian functions whose marginal utility decreases in income and increases with need is equivalent to generalised Lorenz dominance of the cumulated groups, group by group (sequential dominance) *(memory)*. — Atkinson and Bourguignon, "Income distribution and differences in needs", in *Arrow and the Foundations of the Theory of Economic Policy*, Macmillan, 1987, pp. 350–370 [V] [doi:10.1007/978-1-349-07357-3_13](https://doi.org/10.1007/978-1-349-07357-3_13); with different population shares across needs groups — Jenkins and Lambert, "Ranking income distributions when needs differ", *Review of Income and Wealth* 39(4):337–356, 1993 [V] [doi:10.1111/j.1475-4991.1993.tb00466.x](https://doi.org/10.1111/j.1475-4991.1993.tb00466.x); Bourguignon, "Family size and social utility: income distribution dominance criteria", *Journal of Econometrics* 42(1):67–80, 1989 [V] [doi:10.1016/0304-4076(89)90076-6](https://doi.org/10.1016/0304-4076(89)90076-6); Lambert and Ramos, "Welfare comparisons: sequential procedures for heterogeneous populations", SSRN 2001 [V]; *Economica* 69:549–562, 2002 [M] [doi:10.2139/ssrn.277365](https://doi.org/10.2139/ssrn.277365); Ebert, "Social welfare, inequality, and poverty when needs differ", *SCW* 23(3):415–448, 2004 [V] [doi:10.1007/s00355-003-0266-2](https://doi.org/10.1007/s00355-003-0266-2)
- Multidimensional dominance: conditions on joint distributions with signs on cross derivatives (Atkinson–Bourguignon), multidimensional egalitarianism via bistochastic transformations (Kolm), compensation-based comparisons (Muller–Trannoy), robust multidimensional poverty comparisons (Duclos–Sahn–Younger) *(memory for content)*. — Atkinson and Bourguignon, "The comparison of multi-dimensioned distributions of economic status", *REStud* 49(2):183, 1982 [V] [doi:10.2307/2297269](https://doi.org/10.2307/2297269); Kolm, "Multidimensional egalitarianisms", *QJE* 91(1):1, 1977 [V] [doi:10.2307/1883135](https://doi.org/10.2307/1883135); Muller and Trannoy, "Multidimensional inequality comparisons: a compensation perspective", *JET* 147(4):1427–1449, 2012 [V] [doi:10.1016/j.jet.2011.09.002](https://doi.org/10.1016/j.jet.2011.09.002); Duclos, Sahn and Younger, "Robust multidimensional poverty comparisons", *Economic Journal* 116(514):943–968, 2006 [V] [doi:10.1111/j.1468-0297.2006.01118.x](https://doi.org/10.1111/j.1468-0297.2006.01118.x)
- Prioritarianism: benefits matter more the worse off their recipient, as opposed to telic egalitarianism, which faces the levelling-down objection *(memory)* — Parfit, "Equality and priority", *Ratio* 10(3):202–221, 1997 [V] [doi:10.1111/1467-9329.00041](https://doi.org/10.1111/1467-9329.00041); a continuous prioritarian welfare function W = Σᵢ g(uᵢ) with g strictly increasing and concave (e.g. the Atkinson form), ex ante versus ex post prioritarianism *(memory)* — Adler, *Well-Being and Fair Distribution: Beyond Cost-Benefit Analysis*, OUP 2012 (Crossref: 2011) [V] [doi:10.1093/acprof:oso/9780195384994.001.0001](https://doi.org/10.1093/acprof:oso/9780195384994.001.0001); Adler, *Measuring Social Welfare: An Introduction*, OUP 2019 [V] [doi:10.1093/oso/9780190643027.003.0001](https://doi.org/10.1093/oso/9780190643027.003.0001); Adler and Norheim (eds.), *Prioritarianism in Practice*, CUP 2022 [V] [doi:10.1017/9781108691734.001](https://doi.org/10.1017/9781108691734.001)
- Pareto against equality: a social ordering cannot satisfy the Pareto principle, a minimal aggregation condition and a minimal non-aggregation condition together, a "real dilemma" between the tyranny of aggregation and of non-aggregation *(memory; formal statement not read)*. — Fleurbaey and Tungodden, *Economic Theory* 44(3):399–414, 2010 (online 2009) [V] [doi:10.1007/s00199-009-0462-0](https://doi.org/10.1007/s00199-009-0462-0)
- Efficiency first versus equity first (no-envy): the order in which the two principles are applied affects the rationality of social choice *(memory)*. — Tadenuma, *JET* 104(2):462–472, 2002 [V] [doi:10.1006/jeth.2001.2844](https://doi.org/10.1006/jeth.2001.2844)
- Fair social orderings (e.g. leximin over equivalent incomes) built from fairness axioms. — Fleurbaey and Maniquet, *A Theory of Fairness and Social Welfare*, CUP 2011 (Crossref: 2009) [V] [doi:10.1017/cbo9780511851971](https://doi.org/10.1017/cbo9780511851971); Fleurbaey, *Fairness, Responsibility, and Welfare*, OUP 2008 [V] [doi:10.1093/acprof:osobl/9780199215911.001.0001](https://doi.org/10.1093/acprof:osobl/9780199215911.001.0001)
- Partial comparability and quasi-orderings (the intersection approach). — Sen, *Collective Choice and Social Welfare*, expanded edition, 2017 [V] [doi:10.2307/j.ctv2sp3dqx](https://doi.org/10.2307/j.ctv2sp3dqx)
- Software: R `ineq` ("Measuring Inequality, Concentration, and Poverty"), v0.2-13, GPL-2 | GPL-3 [V] — [CRAN ineq](https://cran.r-project.org/web/packages/ineq/index.html), usable as an oracle for Lorenz curves and Atkinson indices.

#### 7.3 Inferences
- Lorenz, generalised Lorenz and sequential dominance are computable in O(n log n) (sort and compare cumulative sums), exactly in rationals; tyche already holds stochastic dominance in `decision` and nomos holds inequality measures, so these are reuse, not new code.
- For an allocation library, dominance turns "which allocation is best" into "which allocations are undominated": the maximal elements of the dominance relation among the outcomes of all candidate rules, with the Atkinson ε or generalised-Gini parameter at which each pairwise comparison flips reported for the rest.
- Sequential dominance is the formal handle on heterogeneous needs, which the user's "humanist concerns" demand: it compares allocations without fixing an equivalence scale, only the order of needs.

#### 7.4 Gaps
- Kolm 1976 Part I's record was not returned by Crossref (only Part II).
- The exact conditions of Atkinson–Bourguignon 1987 (signs of derivatives across needs groups) and Fleurbaey–Tungodden's axioms were not read in the texts.

### 8. Algorithmic Fairness Criteria Conflict by Theorem, and Allocation Under Scarcity Needs Welfare, Not Only Prediction

#### 8.1 Takeaway
Group fairness criteria for scores (calibration within groups, balance of error rates, predictive parity) cannot hold together except under equal base rates or perfect prediction (Kleinberg–Mullainathan–Raghavan; Chouldechova); in allocation of limited interventions the binding question becomes whom the resource helps most and how the gains are distributed, where recent work questions whether prediction is needed at all when inequality is high and argues for contestability and recourse.

#### 8.2 Cited Findings
- Impossibility: calibration within groups, balance for the positive class and balance for the negative class cannot hold simultaneously except with perfect prediction or equal base rates *(memory)*. — Kleinberg, Mullainathan and Raghavan, "Inherent trade-offs in the fair determination of risk scores", arXiv:1609.05807 (19 Sep 2016) [V] [arXiv](https://arxiv.org/abs/1609.05807), published at ITCS 2017 [M]; their SIGMETRICS 2018 abstract "Inherent trade-offs in algorithmic fairness" [V] [doi:10.1145/3219617.3219634](https://doi.org/10.1145/3219617.3219634)
- With different prevalence p across groups, a score with equal positive predictive value cannot have equal false positive and false negative rates, since FPR = [p/(1−p)]·[(1−PPV)/PPV]·(1−FNR) *(memory; equation number not checked)*. — Chouldechova, "Fair prediction with disparate impact: a study of bias in recidivism prediction instruments", *Big Data* 5(2):153–163, 2017 [V] [doi:10.1089/big.2016.0047](https://doi.org/10.1089/big.2016.0047)
- Chouldechova's arXiv versions: arXiv:1610.07524 (2016) and arXiv:1703.00056 (2017) [V] — [arXiv](https://arxiv.org/abs/1703.00056).
- Equalised odds and equal opportunity — Hardt, Price and Srebro, "Equality of opportunity in supervised learning", arXiv:1610.02413 (7 Oct 2016) [V], NeurIPS 2016 [M]: a criterion that depends only on the joint statistics of predictor, target and protected attribute ("oblivious"), an optimal post-processing adjustment of any learned predictor, and the limits of what oblivious tests can identify, illustrated on FICO credit scores *(abstract)* — [arXiv:1610.02413](https://arxiv.org/abs/1610.02413); individual fairness as a Lipschitz condition D(M(x), M(y)) ≤ d(x, y) *(memory)* — Dwork, Hardt, Pitassi, Reingold and Zemel, "Fairness through awareness", *ITCS 2012*, pp. 214–226 [V] [doi:10.1145/2090236.2090255](https://doi.org/10.1145/2090236.2090255); constrained optimal rules are group-specific thresholds *(memory)* — Corbett-Davies, Pierson, Feller, Goel and Huq, "Algorithmic decision making and the cost of fairness", *KDD 2017*, pp. 797–806 [V] [doi:10.1145/3097983.3098095](https://doi.org/10.1145/3097983.3098095); a review — Mitchell, Potash, Barocas, D'Amour and Lum, "Algorithmic fairness: choices, assumptions, and definitions", *Annual Review of Statistics and Its Application* 8:141–163, 2021 [V] [doi:10.1146/annurev-statistics-042720-125902](https://doi.org/10.1146/annurev-statistics-042720-125902)
- Allocation of limited resources across groups with learning (fairness as equal chance that a deserving candidate is reached) *(memory)* — Elzayn, Jabbari, Jung, Kearns, Neel and Roth, "Fair algorithms for learning in allocation problems", *FAT* 2019*, pp. 170–179 [V] [doi:10.1145/3287560.3287571](https://doi.org/10.1145/3287560.3287571); fairness against utilisation under uncertain demand *(memory)* — Donahue and Kleinberg, *FAT* 2020*, pp. 658–668 [V] [doi:10.1145/3351095.3372847](https://doi.org/10.1145/3351095.3372847)
- Allocating homelessness services by predicted outcomes could reduce repeat episodes in a counterfactual analysis, but "a significant fraction of households are predicted to have a higher probability of re-entry in the optimal allocation than in the original one" *(abstract)*. — Kube, Das and Fowler, *AAAI* 33(01):622–629, 2019 [V] [doi:10.1609/aaai.v33i01.3301622](https://doi.org/10.1609/aaai.v33i01.3301622)
- Fair, efficient and interpretable prioritisation policies for housing homeless youth by mixed-integer optimisation *(memory)* — Azizi, Vayanos, Wilder, Rice and Tambe, *CPAIOR 2018*, LNCS, pp. 35–51 [V] [doi:10.1007/978-3-319-93031-2_3](https://doi.org/10.1007/978-3-319-93031-2_3); incompatibility of fairness metrics in contextual resource allocation *(memory)* — Jo, Tang, Dullerud, Aghaei, Rice and Vayanos, *AAAI* 37(10):11837–11846, 2023 [V] [doi:10.1609/aaai.v37i10.26397](https://doi.org/10.1609/aaai.v37i10.26397)
- Fairness as defined in machine learning can conflict with equality and welfare and leaves power unexamined *(memory)* — Kasy and Abebe, *FAccT 2021*, pp. 576–586 [V] [doi:10.1145/3442188.3445919](https://doi.org/10.1145/3442188.3445919); a legitimacy critique of decisions that optimise predictive accuracy — Wang, Kapoor, Barocas and Narayanan, "Against predictive optimization", *FAccT 2023*, p. 626 (abstract) [V] [doi:10.1145/3593013.3594030](https://doi.org/10.1145/3593013.3594030); Shirali, Abebe and Hardt, "Allocation requires prediction only if inequality is low", arXiv:2406.13882 (19 Jun 2024) [V], ICML 2024 [M] — [arXiv](https://arxiv.org/abs/2406.13882) (content from the title: when inequality between units is high, allocating by unit-level aggregates can match prediction-based targeting *(memory)*); Perdomo, Britton, Hardt and Abebe, "Difficult lessons on social prediction from Wisconsin public schools", arXiv:2304.06205 (2023) [V] — [arXiv](https://arxiv.org/abs/2304.06205).
- Software: `fairlearn`, MIT (GitHub API; PyPI v0.15.0) [V] — [GitHub](https://github.com/fairlearn/fairlearn); IBM `AIF360`, Apache-2.0 (GitHub API; PyPI v0.6.1) [V] — [GitHub](https://github.com/Trusted-AI/AIF360).
- Contestability and recourse: counterfactual explanations — Wachter, Mittelstadt and Russell, SSRN 2017 [V] [doi:10.2139/ssrn.3063289](https://doi.org/10.2139/ssrn.3063289) (*Harvard Journal of Law & Technology* 31(2), 2018 [M]); actionable recourse in linear classification — Ustun, Spangher and Liu, *FAT* 2019*, pp. 10–19 [V] [doi:10.1145/3287560.3287566](https://doi.org/10.1145/3287560.3287566); contestable AI by design, a framework of sociotechnical features "responsive to human intervention throughout the system lifecycle" *(abstract)* — Alfrink, Keller, Kortuem and Doorn, *Minds and Machines* 33(4):613–639, 2023 (online 2022) [V] [doi:10.1007/s11023-022-09611-z](https://doi.org/10.1007/s11023-022-09611-z)
- Allocation of exposure in rankings — Singh and Joachims, "Fairness of exposure in rankings", *KDD 2018*, pp. 2219–2228 [V] [doi:10.1145/3219819.3220088](https://doi.org/10.1145/3219819.3220088)

#### 8.3 Inferences
- For allocation, prediction enters only as an input to a priority order or a claim (a risk score becomes a category's priority in a reserve system); the group criteria then become properties to report per category, with the impossibility theorems stated as reasons why not all can be shown.
- Contestability maps onto what the infrastructure already has: every allocation as a structured argument (apodeixis's ASPIC+) whose premises are the rule, the priorities, the eligibility and the data, so that a person can attack a premise and see what changes.

#### 8.4 Gaps
- The ITCS 2017 and NeurIPS 2016 proceedings records of Kleinberg et al. and Hardt et al. were not verified (their arXiv records were); Shirali–Abebe–Hardt's and Perdomo et al.'s claims were not read beyond titles.

### 9. Methods That Return Sets, Orders and Distributions Instead of One Optimum

#### 9.1 Takeaway
A substantial toolkit returns structure rather than a single answer: maximal elements of partial orders (Pareto, Lorenz, stochastic dominance, the scale of fairness criteria), necessary and possible relations over all admissible weights (robust ordinal regression), rank-acceptability distributions over weight spaces (SMAA), lexicographic and threshold (satisficing) rules, and the set of all allocations meeting minimal axioms (e.g. all cutoff equilibria, Pathak et al. Theorem 1 and Proposition 1).

#### 9.2 Cited Findings
- SMAA: explores the weight space to describe the valuations under which each alternative is preferred; SMAA-2 considers all ranks (rank acceptability indices), computes confidence factors for uncertain criteria values, identifies compromise candidates, and handles partial preference information *(abstract)*. — Lahdelma and Salminen, "SMAA-2: stochastic multicriteria acceptability analysis for group decision making", *Operations Research* 49(3):444–454, 2001 [V] [doi:10.1287/opre.49.3.444.11220](https://doi.org/10.1287/opre.49.3.444.11220); Lahdelma, Hokkanen and Salminen, "SMAA – stochastic multiobjective acceptability analysis", *EJOR* 106(1):137–143, 1998 [V] [doi:10.1016/s0377-2217(97)00163-x](https://doi.org/10.1016/s0377-2217(97)00163-x); Tervonen and Figueira, "A survey on stochastic multicriteria acceptability analysis methods", *J. Multi-Criteria Decision Analysis* 15(1–2):1–14, 2008 (Crossref: 2007) [V] [doi:10.1002/mcda.407](https://doi.org/10.1002/mcda.407). Definitions *(memory)*: rank acceptability b_i^r = ∫ over weights w (uniform on the simplex or constrained) of the indicator that alternative i has rank r; central weight vector; confidence factor. Software: R `smaa` (v0.3-4, GPL-3) and `hitandrun` (v0.5-6, GPL-3, uniform sampling of weights from convex polytopes, used for constrained weights) [V] — [CRAN smaa](https://cran.r-project.org/web/packages/smaa/index.html); [CRAN hitandrun](https://cran.r-project.org/web/packages/hitandrun/index.html).
- Robust ordinal regression: necessary and possible preference relations over all additive value functions compatible with a person's pairwise judgements (UTA^GMS) *(memory)*. — Greco, Mousseau and Słowiński, *EJOR* 191(2):416–436, 2008 [V] [doi:10.1016/j.ejor.2007.08.013](https://doi.org/10.1016/j.ejor.2007.08.013)
- Lexicographic orders and decision rules — Fishburn, *Management Science* 20(11):1442–1471, 1974 [V] [doi:10.1287/mnsc.20.11.1442](https://doi.org/10.1287/mnsc.20.11.1442); satisficing — Simon, "A behavioral model of rational choice", *QJE* 69(1):99, 1955 [V] [doi:10.2307/1884852](https://doi.org/10.2307/1884852)
- Local justice: how institutions allocate scarce goods (kidneys, admissions, layoffs) by mixtures of need, desert, queue and lottery principles. — Elster, *Local Justice*, Russell Sage Foundation, 1992 [S] (Crossref holds only a review: [doi:10.2307/2131972](https://doi.org/10.2307/2131972)); Young, *Equity: In Theory and Practice*, Princeton University Press, 1994 (Crossref: 1995 edition) [V] [doi:10.1515/9780691214054](https://doi.org/10.1515/9780691214054)
- The set of reserve matchings satisfying three minimal axioms equals the set of cutoff equilibria (Theorem 1) and of DA-induced matchings (Proposition 1) *(text)*. — [Pathak et al. 2024](https://doi.org/10.1287/mnsc.2022.00930) [V]

#### 9.3 Inferences
- An allocation engine can be organised around three return types: a *set* (undominated outcomes; all outcomes satisfying a list of axioms), an *order* (a partial order with its Hasse diagram, or necessary and possible relations), and a *distribution* (a lottery's assignment matrix and its decomposition; SMAA's rank acceptabilities). A single optimum is the degenerate case.
- These map onto the infrastructure's verdict style (kategoria answers claims with evidence): each property of each candidate is a verdict with a witness or counterexample.

#### 9.4 Gaps
- No source was found that unifies axiomatic rules, dominance and SMAA into one framework for allocation; the integration proposed in §10 is a design inference.

### 10. What This Means for the Design

#### 10.1 Takeaway
Build a claims layer, a priority and reserve layer, a lottery layer and a dominance layer first, all in exact rationals, reusing nomos (Talmud, Shapley, nucleolus, deferred acceptance, TTC, inequality measures), calx (Hungarian assignment, knapsack, greedy submodular) and morphe (flows); verify each rule against closed forms and published examples, and each axiom on random instances with counterexamples recorded; and present results as sets, partial orders and lotteries with verdicts per axiom, never as one optimum.

#### 10.2 Cited Findings (the fixtures and examples to verify against)
- Talmud's three estates and the contested garment: claims (100, 200, 300), estates 100/200/300 → (33⅓, 33⅓, 33⅓), (50, 75, 75), (50, 100, 150); garment claims (1, ½) → (¾, ¼) *(memory; standard)*. — [Aumann and Maschler 1985](https://doi.org/10.1016/0022-0531(85)90102-4) [V]
- Talmud = nucleolus and RA = Shapley value of v(S) = max(0, E − Σ_{j∉S} cⱼ) *(memory)*. — [Aumann and Maschler 1985](https://doi.org/10.1016/0022-0531(85)90102-4) [V]; [O'Neill 1982](https://doi.org/10.1016/0165-4896(82)90029-4) [V]
- Pathak et al.'s Example 1 (six units, seven patients, two precedence orders, matched sets {i1,i2,i3,i4,i5,i7} and {i1,i2,i3,i4,i5,i6}) and Example A.2 (hard reserves; open-first wastes the reserved unit) *(text)*; Theorem 1, Propositions 1–5, Theorem A.2 as properties to test *(text)*. — [arXiv:2008.00374](https://arxiv.org/abs/2008.00374) [V]
- MES worked example on equalshares.net (€1,100, 11 voters) *(page)*; Pabulib instances with their declared rules [V page]. — [equalshares.net](https://equalshares.net/explanation/); [pabulib.org/format](https://pabulib.org/format)
- MNW is EF1 and PO; round-robin is EF1; MMS can fail; EFX exists for three additive agents *(abstracts of Caragiannis et al. 2019; Kurokawa et al. 2018; Chaudhury et al. 2024)*. — [doi:10.1145/3355902](https://doi.org/10.1145/3355902); [doi:10.1145/3140756](https://doi.org/10.1145/3140756); [doi:10.1145/3616009](https://doi.org/10.1145/3616009)

#### 10.3 Inferences (the design)

**What to reuse from what exists.**
- nomos's Talmud rule stays the reference; RA is nomos's Shapley value of the bankruptcy game; the nucleolus check of the Talmud rule is nomos's nucleolus; Adjusted Winner (already in nomos) is the two-agent weighted-EF1 + PO procedure Chakraborty et al. adapt.
- nomos's deferred acceptance is the engine of reserve systems (Proposition 1); its top trading cycles reproduces RSD through the core from random endowments (a cross-check, Abdulkadiroğlu–Sönmez 1998).
- calx's Hungarian method and morphe's maximum flows give the smart reserve matching, extended PS with indifferences, the bipartite rationing rules and Bogomolnaia–Moulin's dichotomous egalitarian matching; calx's knapsack and greedy submodular maximisation give the utilitarian PB baselines.
- nomos's inequality measures and tyche's stochastic dominance give Lorenz, generalised Lorenz and sequential dominance; tyche's decisions under ambiguity give maximin, Hurwicz and credal comparisons where weights are uncertain.
- apodeixis (ASPIC+) carries the reasons for an allocation as an argument that can be contested; kategoria's verdict format (holds, fails with the place, undecided with the reason) is the format of every axiom check.

**What to build first, in order.**
1. Claims rules (nomos `rationing` or within `games`): PRO, CEA, CEL, Talmud, Piniles, RA, AP, the TAL family, Young's parametric family given f, duality as an operator, minimal rights and truncation operators (Thomson–Yeh), weighted and priority-class versions (Moulin 2000; Flores-Szwagrzak 2015), multi-issue two-stage rules, bipartite rationing (Moulin–Sethuraman 2013), indivisible-unit versions (Herrero–Martínez). Exact rationals; O(n log n) except RA.
2. Priority and reserve systems: sequential reserve matching with a precedence order; the cutoff equilibrium and its max/min cutoffs (eqs. (1)–(2)); the smart reserve family n ∈ {0,…,r_u}; India's 2SMG (Sönmez–Yenmez) as a second instance; soft/hard reserves, minimum guarantee and over-and-above as declared options.
3. Sequential and random assignment: SD, RSD (exact probabilities by enumeration only for small n, with a warning that the problem is #P-hard, Monte Carlo otherwise), PS and extended PS, picking sequences (round-robin, balanced alternation, weighted sequences), BvN decomposition in rationals, Bogomolnaia–Moulin dichotomous egalitarian matching.
4. Dominance and orderings: Lorenz, generalised Lorenz, sequential (needs-ordered) dominance, the Atkinson, Kolm and generalised-Gini EDE families with the parameter value at which comparisons flip, and the maximal elements among candidate outcomes.
5. Fair division of indivisible goods: EF/EF1/EFX/PROP/MMS checkers, round-robin, envy-cycle, MNW (exact for small instances by enumeration or integer programming; flagged as an optimisation), leximin for dichotomous settings; weighted versions.
6. PB: utilitarian greedy (reuse calx), MES with declared completion, sequential Phragmén, EJR/PJR checkers, a Pabulib reader.
7. Lotteries and partial randomisation: weighted lotteries from claims rules applied to chances; threshold-then-lottery procedures with the threshold rule declared.

**Axioms to check on random instances** (each a property test with the counterexample kept):
- Claims: feasibility (0 ≤ x ≤ c, Σx = E); equal treatment; order preservation in awards and losses; claims and resource monotonicity; consistency (expected to fail for RA: the test must find a counterexample); composition up and down (expected to fail for Talmud); self-duality (PRO, T, RA, AP) and Rᵈᵈ = R; truncation invariance and minimal rights first; securement; CEA's Lorenz-maximality and CEL's Lorenz-minimality among rules; Young's representation (given f, the parametric output equals the closed form).
- Reserves: eligibility, non-wastefulness, respect of priorities on every output; Theorem 1 (a supporting cutoff vector exists, and conversely); Proposition 1 by brute force for small instances (enumerate all artificial preference profiles; the DA outcomes equal the set of matchings satisfying the three axioms); Proposition 3 (earlier precedence ⇒ weakly higher maximum cutoff); Proposition 4 within its hypotheses and Example 1 as the counterexample outside them; Proposition 5 and Theorem A.2 for smart reserves.
- Random assignment: PS outputs sd-efficient and sd-envy-free; RSD outputs ex post efficient; BvN components feasible and summing back exactly.
- Fair division: round-robin EF1 (additive); MNW EF1 + PO (brute force small instances); EFX existence for n ≤ 3 by brute force; MMS ≥ 2/3 achievable (Kurokawa et al.) on random instances; the published MMS counterexamples as fixtures (to be extracted).
- PB: MES satisfies EJR (approval, cardinality satisfaction) by brute-force enumeration of cohesive groups on small instances; MES is not exhaustive (completion needed); compare with pabutools on Pabulib instances.
- Welfare: GL dominance ⇒ higher EDE for every Atkinson ε ≥ 0 and every generalised-Gini weight vector (random pairs); Lorenz dominance at equal means ⇔ Pigou–Dalton transfers (constructive check).

**How to present sets, orders and lotteries instead of one optimum.**
- For a claims problem: the outcomes of all standard rules at once, with the Lorenz partial order among them and the TAL curve θ ↦ Tᶿ(c,E) between CEL and CEA, so that the choice of rule is visible as a choice of attitude to the small and large claimants.
- For a reserve system: the matching, the announced (maximum) cutoffs, and the outcomes over every precedence order or over n ∈ {0,…,r_u}, with the beneficiaries served under each.
- For a lottery: the fractional assignment (each entity's entitlement in chances), its decomposition into deterministic allocations, and ex ante versus ex post properties reported separately.
- For several principles (need, urgency, waiting, benefit): the set of allocations undominated under all of them, SMAA rank-acceptabilities over the admissible weights, and the necessary/possible relations of robust ordinal regression; a single weighted score only when a person declares the weights.
- Every property as a verdict with its witness (a cutoff vector, an envy-free check, a cohesive group satisfied) or its counterexample, so that the allocation can be contested premise by premise.

#### 10.4 Gaps
- Licences found this session: ClaimsProblems GPL-3; GameTheory GPL ≥ 2; CoopGame GPL-2; pabutools GPL-3.0; fairpy (fair division) GPL-3.0; abcvoting MIT; pref_voting MIT; matching MIT; fairlearn MIT; AIF360 Apache-2.0; smaa and hitandrun GPL-3; ineq GPL ≥ 2. Unknown: Pabulib's data, Spliddit's code, fairpyx, JSMAA, the citizens'-assembly selection code. The copyleft packages are oracles for fixtures, not code to adopt under MIT (CLAUDE.md §7 requires the licence check).
- Theorem numbers flagged *(memory)* must be checked in the texts before the library's documentation cites them.
- Exact numerical fixtures still to extract: the MMS counterexamples; the "EFX need not exist" instance; a full MES vote profile (the equalshares.net example gives only winners).

## Part III. Urban and Spatial Justice and the Urban Economics of Allocation

*Remark.* Part III's scope and conventions. Scope: spatial and urban justice and the allocation of public resources among places, groups and services: the theories, horizontal fiscal equalisation, the economics of place-based policy, index-based targeting, distributional weights in appraisal, disaster mitigation and recovery funding, local justice for housing, schools and facilities, participatory budgeting, gender and equity budgeting, procedure and legitimacy, the urban economics of tax capacity, sorting, capitalisation and displacement, and open records. The bibliographic markers are those of §0.3; Crossref was queried by `query.bibliographic` on 2026-11-20 and DOIs are given as links. Content is tagged separately from the record: **(T)** read in the text this session; **(A)** read in an abstract, an official summary page or a search summary this session; **(M)** from memory. Formulas are flagged by origin in the same way. Texts read this session: the government response to England's Fair Funding Review 2.0, the English indices of deprivation 2025 statistical release, section 166A of the Housing Act 1996, de Sousa Santos 1998 on Porto Alegre (pp. 474–477), FEMA's benefit–cost analysis news page and Pabulib's site; the Productivity Commission's inquiry page and the Area Deprivation Index's terms were fetched. Inside this Part, a bare section reference (for example "section 11") means this Part's own section.

### 1. Spatial and Urban Justice Theory Supplies Need, Contribution, Recognition, Voice and Sufficiency as Allocation Principles

#### Takeaway
The theoretical literature gives a city-scale allocation tool five separable principles, not one objective: distribution by need (Harvey's territorial social justice, Fainstein's equity), recognition of groups and differences (Young, Fraser, Schlosberg), participation and voice (the right to the city, procedural environmental justice), sufficiency thresholds for access (Martens' transport justice), and attention to the processes that produce spatial distributions (Soja, Harvey's later work). The theories disagree on whether distribution is the right frame at all; a tool that only optimises a distribution adopts one side of that dispute and must say so.

#### Cited Findings

**1.1 Territorial social justice (Harvey).**
- Harvey, D. *Social Justice and the City* (1973; rev. ed. University of Georgia Press 2009) [V] — [record](https://doi.org/10.1353/book13205). (M) Harvey defines social justice as "a just distribution justly arrived at" and orders three criteria for territorial distribution: need first, then contribution to the common good (inter-territorial multiplier effects), then merit (extra resources for territories facing special physical or social difficulty); he requires that the institutional mechanisms be such that the prospects of the least advantaged territory are as great as possible, a Rawlsian maximin over territories. In the book's second half he moves from these "liberal formulations" to "socialist formulations" arguing that the distribution follows from the mode of production and land-rent processes, so redistributive formulas alone cannot secure justice. The ordering need > contribution > merit is the closest thing in the theory to an allocation rule and is directly implementable as a lexicographic or weighted criterion. (Content M.)

**1.2 Spatial justice (Soja) and its critics.**
- Soja, E. W. *Seeking Spatial Justice* (University of Minnesota Press 2010) [V] — [chapter record](https://doi.org/10.5749/minnesota/9780816666676.003.0005). (M) Spatial justice is the fair and equitable distribution in space of socially valued resources and the opportunities to use them; geography is "consequential": injustice is produced through spatial processes (zoning, transit investment, segregation), not merely displayed in space. Soja's central case is the Bus Riders Union's civil rights suit against the Los Angeles MTA (consent decree 1996), which reallocated transit spending from rail serving richer suburbs toward buses used by low-income riders of colour. (Content M.)
- Marcuse, P. "Spatial justice: derivative but causal of social injustice", in *Justice et injustices spatiales* (Presses universitaires de Paris Nanterre 2010) [V] — [record](https://doi.org/10.4000/books.pupo.420). (M) Spatial injustice is derivative of social injustice but also causes and reinforces it; two cardinal forms are involuntary confinement (ghettos) and unequal allocation of resources over space; remedies must be both spatial and social. (Content M.)
- Dikeç, M. "Justice and the Spatial Imagination", *Environment and Planning A* 33(10) 1785–1805 (2001) [V] — [record](https://doi.org/10.1068/a3467). (M) Distinguishes "the spatiality of injustice" from "the injustice of spatiality".
- Soja, E. W. "The City and Spatial Justice", in *Justice et injustices spatiales* (Presses universitaires de Paris Nanterre 2010) 56–72 [V] — [record](https://doi.org/10.4000/books.pupo.415).
- Israel, E. and Frenkel, A. "Social justice and spatial inequality: Toward a conceptual framework", *Progress in Human Geography* 42(5) 647–665 (2018; online 2017) [V] — [record](https://doi.org/10.1177/0309132517702969). (M) Combines the capability approach with spatial inequality: justice concerns the spatial distribution of capabilities, i.e. of opportunities that people can convert into functionings given personal and environmental conversion factors. (Content M.)
- Moroni, S. "The just city. Three background issues: Institutional justice and spatial justice, social justice and distributive justice, concept of justice and conceptions of justice", *Planning Theory* 19(3) (2020; online 2019) [V] — [record](https://doi.org/10.1177/1473095219877670). (M) A liberal critique: justice applies to institutions and rules, not to spatial patterns as such; planners should design fair general rules rather than target end-state distributions. (Content M.) This is the strongest theoretical objection to a tool that optimises a spatial distribution.

**1.3 The just city (Fainstein) and its critiques.**
- Fainstein, S. S. *The Just City* (Cornell University Press 2010; e-book record) [V] — [record](https://doi.org/10.7591/9780801460487). (M) Three criteria, equity, diversity and democracy, with equity given priority where they conflict; derived from Rawls, Young, Nussbaum and Sen; operationalised as concrete criteria for urban policy (e.g. no involuntary displacement except for strong reasons and with compensation; new housing to include units affordable to those below median income; planning to favour the less well off in economic development; public space accessible to all). Evaluated against Amsterdam, London and New York; Amsterdam ranked most just. (Content M.)
- Hayward, review of *The Just City*, *Perspectives on Politics* 10(3) 837–838 (2012) [V] — [record](https://doi.org/10.1017/s1537592712001508). (Content not read.)
- Uitermark, J. and Nicholls, W. "Planning for social justice: Strategies, dilemmas, tradeoffs", *Planning Theory* 16(1) 32–50 (2017; online 2016) [V] — [record](https://doi.org/10.1177/1473095215599027). (M) Argues that just-city criteria conflict in practice (e.g. social mixing as "diversity" can displace low-income residents; empowerment and redistribution pull apart), so planners face strategic trade-offs that a list of criteria does not resolve. (Content M.)
- Low, S. and Iveson, K. "Propositions for more just urban public spaces", *City* 20(1) 10–31 (2016) [V] — [record](https://doi.org/10.1080/13604813.2015.1128679). (M) Proposes multiple frameworks of justice for public space (distributive, procedural, interactional, recognition, ethic of care) rather than one. (Content M.)
- Fainstein, S. "Spatial Justice and Planning", in *Readings in Planning Theory* (Wiley 2015) [V] — [record](https://doi.org/10.1002/9781119084679.ch13); Fainstein, S. S. "The just city", *International Journal of Urban Sciences* 18(1) 1–18 (2014; online 2013) [V] — [record](https://doi.org/10.1080/12265934.2013.834643). (Contents not read.)
- Common critiques (M): the just city is reformist and liberal-egalitarian, takes the capitalist land market as given (Marcuse, Purcell); its criteria are Western-European and fit Global South cities with informality poorly; "equity" is under-specified (equity of what: income, access, outcomes?). Southern critiques: Watson, V. "Seeing from the South: Refocusing Urban Planning on the Globe's Central Urban Issues", *Urban Studies* 46(11) 2259–2275 (2009) [V] — [record](https://doi.org/10.1177/0042098009342598); Roy, A. "Why India Cannot Plan Its Cities: Informality, Insurgence and the Idiom of Urbanization", *Planning Theory* 8(1) 76–87 (2009) [V] — [record](https://doi.org/10.1177/1473095208099299); Yiftachel, O. "Theoretical Notes On 'Gray Cities': the Coming of Urban Apartheid?", *Planning Theory* 8(1) 88–100 (2009) [V] — [record](https://doi.org/10.1177/1473095208099300). (M) Informality is produced by the state's planning itself; allocation rules keyed to formal tenure or registration exclude "gray" populations by construction. (Content M.)

**1.4 The right to the city and its successors.**
- Lefebvre, H. *Le Droit à la ville* (Anthropos 1968) [M]; English selection in *Writings on Cities*, ed. Kofman and Lebas (Blackwell 1996) [M] (Crossref returned only Kofman's later *Selected Writings*, Stanford 2007, [V](https://doi.org/10.1515/9781503617407)). (M) The right to the city is a "cry and a demand" for a transformed urban life: rights of participation (inhabitants, not owners or citizens, shape decisions on the production of urban space) and appropriation (use value over exchange value).
- Harvey, D. "The Right to the City", *New Left Review* 53 (2008) 23–40 [V] — [record](https://doi.org/10.64590/fmh). (M) The right is a collective right to reshape the process of urbanisation, i.e. democratic control over the production and use of the urban surplus. (Content M.)
- Purcell, M. "Excavating Lefebvre: The right to the city and its urban politics of the inhabitant", *GeoJournal* 58(2–3) 99–108 (2002) [V] — [record](https://doi.org/10.1023/b:gejo.0000010829.62237.8f). (M) Warns that the right is radical and open-ended, not a policy checklist; a right of inhabitants could exclude others, so its content is not determined in advance. (Content M.)
- Marcuse, P. "From critical urban theory to the right to the city", *City* 13(2–3) 185–197 (2009) [V] — [record](https://doi.org/10.1080/13604810902982177). (M) Asks whose right (the deprived and the discontented), what right (a moral claim, not a legal one), to which city; programme "expose, propose, politicize". (Content M.)
- Brenner, N., Marcuse, P. and Mayer, M. "Cities for people, not for profit", *City* 13(2–3) 176–184 (2009) [V] — [record](https://doi.org/10.1080/13604810903020548); book of the same title (Routledge 2012) [V] — [record](https://doi.org/10.4324/9780203802182).
- Attoh, K. A. "What *kind* of right is the right to the city?", *Progress in Human Geography* 35(5) 669–685 (2011) [V] — [record](https://doi.org/10.1177/0309132510394706). (M) Using Hohfeld's typology, shows the right is used for incompatible kinds of rights (claims, liberties, immunities), so conflicts among rights-holders are unresolved by the slogan. (Content M.)
- Mitchell, D. *The Right to the City: Social Justice and the Fight for Public Space* (Guilford 2003) [S] (records only of reviews, e.g. [Choice](https://doi.org/10.5860/choice.41-2502)).
- Kuymulu, M. B. "The Vortex of Rights: 'Right to the City' at a Crossroads", *International Journal of Urban and Regional Research* 37(3) 923–940 (2013) [V] — [record](https://doi.org/10.1111/1468-2427.12008). (M) The slogan has been co-opted by institutions (UN-Habitat, municipal charters) into a list of rights, diluting Lefebvre's radical content. (Content M.)
- Pierce, J., Williams, O. R. and Martin, D. G. "Rights in places: An analytical extension of the right to the city", *Geoforum* 70 79–88 (2016) [V] — [record](https://doi.org/10.1016/j.geoforum.2016.02.006).

**1.5 Environmental justice: distribution, recognition, procedure, capabilities.**
- Schlosberg, D. *Defining Environmental Justice: Theories, Movements, and Nature* (OUP 2007) [V] — [record](https://doi.org/10.1093/acprof:oso/9780199286294.001.0001). (M) EJ movements demand distributive equity, recognition of group difference and identity, participation in decisions, and the capabilities and functioning of communities; these are interrelated and not reducible to distribution. (Content M.)
- Schlosberg, D. "Theorising environmental justice: the expanding sphere of a discourse", *Environmental Politics* 22(1) 37–55 (2013) [V] — [record](https://doi.org/10.1080/09644016.2013.755387).
- Holifield, R., Porter, M. and Walker, G. "Spaces of Environmental Justice: Frameworks for Critical Engagement", *Antipode* 41(4) 591–612 (2009) [V] — [record](https://doi.org/10.1111/j.1467-8330.2009.00690.x).
- Walker, G. "Beyond Distribution and Proximity: Exploring the Multiple Spatialities of Environmental Justice", in *Spaces of Environmental Justice* (Wiley 2010) 23–46 [V] — [record](https://doi.org/10.1002/9781444322767.ch1). (M) Proximity-based analyses depend on the chosen scale and units; justice claims involve flows, networks and multiple scales. (Content M.)
- Mohai, P., Pellow, D. and Roberts, J. T. "Environmental Justice", *Annual Review of Environment and Resources* 34 405–430 (2009) [V] — [record](https://doi.org/10.1146/annurev-environ-082508-094348); Agyeman, J., Schlosberg, D., Craven, L. and Matthews, C. "Trends and Directions in Environmental Justice: From Inequity to Everyday Life, Community, and Just Sustainabilities", *Annual Review of Environment and Resources* 41 (2016) [V] — [record](https://doi.org/10.1146/annurev-environ-110615-090052).
- Young, I. M. *Justice and the Politics of Difference* (Princeton 1990; 2011 ed.) [V] — [record](https://doi.org/10.1515/9781400839902). (M) The "distributive paradigm" misses domination and oppression (exploitation, marginalisation, powerlessness, cultural imperialism, violence), which are not goods to be allocated. (Content M.) Young, I. M. *Inclusion and Democracy* (OUP 2000; chapter record) [V] — [record](https://doi.org/10.1093/0198297556.003.0002). (M) Democratic inclusion of affected groups as a condition of just outcomes; deliberation must correct for structural inequality of voice. (Content M.)
- Fraser, N. "Social Justice in the Age of Identity Politics: Redistribution, Recognition, and Participation", in *Culture and Economy after the Cultural Turn* (SAGE 1999) 25–52 [V] — [record](https://doi.org/10.4135/9781446218112.n2). (M) Parity of participation as the norm uniting redistribution and recognition. (Content M.)
- Banzhaf, S., Ma, L. and Timmins, C. "Environmental Justice: The Economics of Race, Place, and Pollution", *Journal of Economic Perspectives* 33(1) 185–208 (2019) [V] — [record](https://doi.org/10.1257/jep.33.1.185). (M) Correlations between pollution and minority/low-income residence can arise from discriminatory siting, from siting where land is cheap, or from households sorting after siting (Tiebout-style "coming to the nuisance"); the causal channel matters for remedy because cleanups can be capitalised into rents and displace the intended beneficiaries. (Content M.) Companion: "Environmental Justice: Establishing Causal Relationships", *Annual Review of Resource Economics* 11 377–398 (2019) [V] — [record](https://doi.org/10.1146/annurev-resource-100518-094131).

**1.6 Transport justice: sufficiency of accessibility.**
- Martens, K. *Transport Justice: Designing Fair Transportation Systems* (Routledge 2017; Crossref dates its chapters 2016) [V] — [record](https://doi.org/10.4324/9781315746852-9). (M) Accessibility (potential mobility) is the good whose distribution matters; drawing on Walzer and Dworkin, Martens proposes (i) a sufficiency threshold of accessibility for everyone, (ii) a cap on the gap between best- and worst-served, and (iii) assessment of plans by their effect on those below the threshold rather than by aggregate travel-time savings. (Content M.)
- Martens, K., Golub, A. and Robinson, G. "A justice-theoretic approach to the distribution of transportation benefits: Implications for transportation planning practice in the United States", *Transportation Research Part A* 46(4) 684–695 (2012) [V] — [record](https://doi.org/10.1016/j.tra.2012.01.004).
- Martens, K., Singer, M. E. and Cohen-Zada, A. L. "Equity in Accessibility: Moving From Disparity to Insufficiency Analyses", *Journal of the American Planning Association* 88(4) 479–494 (2022) [V] — [record](https://doi.org/10.1080/01944363.2021.2016476); Singer, M. E., Cohen-Zada, A. L. and Martens, K. "Core versus periphery: Examining the spatial patterns of insufficient accessibility in U.S. metropolitan areas", *Journal of Transport Geography* 100 103321 (2022) [V] — [record](https://doi.org/10.1016/j.jtrangeo.2022.103321). (A, from APA's blog via search) Applied to 49 of the 50 largest US metropolitan areas; even with a sufficiency threshold set at 1% of the jobs reachable by car, nearly 11 million people below the poverty line lacked sufficient transit access; at a 10% threshold, 90% of the below-poverty population had insufficient access; the regional *Accessibility Sufficiency Index* (ASI) summarises insufficiency — [APA blog](https://planning.org/blog/9260039/its-time-to-rethink-disparity-analyses-in-transportation/); [RePEc record](https://ideas.repec.org/a/taf/rjpaxx/v88y2022i4p479-494.html).
- Formula (M, functional form not verified): the ASI is built in the manner of Foster–Greer–Thorbecke poverty measures applied to accessibility, $\mathrm{ASI}_\alpha = \frac{1}{N}\sum_{i: A_i<T}\left(\frac{T-A_i}{T}\right)^\alpha$, with $A_i$ person (or zone, population-weighted) accessibility and $T$ the threshold; $\alpha=0$ is the headcount share, $\alpha=1$ the normalised gap, $\alpha=2$ gap-squared. FGT: Foster, Greer and Thorbecke, "A Class of Decomposable Poverty Measures", *Econometrica* 52(3) 761–766 (1984) [V] — [record](https://doi.org/10.2307/1913475).
- van der Veen, A. S., Annema, J. A., Martens, K., van Arem, B. and Correia, G. H. de A. "Operationalizing an indicator of sufficient accessibility – a case study for the city of Rotterdam", *Case Studies on Transport Policy* 8(4) 1360–1370 (2020) [V] — [record](https://doi.org/10.1016/j.cstp.2020.09.007). (A) Formalised method; data availability is the main constraint.
- Ryan, J. and Martens, K. "Defining and implementing a sufficient level of accessibility: What's stopping us?", *Transportation Research Part A* 175 103792 (2023) [V] — [record](https://doi.org/10.1016/j.tra.2023.103792). (A, title and Lund record) On accessibility standards as key performance indicators; the obstacles to adopting a sufficiency threshold in practice.
- Lucas, K., Mattioli, G., Verlinghieri, E. and Guzman, A. "Transport poverty and its adverse social consequences", *Proceedings of the ICE – Transport* 169(6) 353–365 (2016) [V] — [record](https://doi.org/10.1680/jtran.15.00073).
- Bocarejo S., J. P. and Oviedo H., D. R. "Transport accessibility and social inequities: a tool for identification of mobility needs and evaluation of transport investments", *Journal of Transport Geography* 24 (2012) [V] — [record](https://doi.org/10.1016/j.jtrangeo.2011.12.004). (M) Bogotá; accessibility combined with affordability to identify mobility needs. (Content M.)
- Lucas, K. "Transport and social exclusion: Where are we now?", *Transport Policy* 20 105–113 (2012) [V] — [record](https://doi.org/10.1016/j.tranpol.2012.01.013).
- Pereira, R. H. M., Schwanen, T. and Banister, D. "Distributive justice and equity in transportation", *Transport Reviews* 37(2) 170–191 (2017; online 2016) [V] — [record](https://doi.org/10.1080/01441647.2016.1257660). (M) Compares utilitarianism, libertarianism, intuitionism, Rawls' egalitarianism and the capability approach; concludes that transport policy should secure a minimum level of accessibility to key destinations for all, prioritise the disadvantaged, and address risks and externalities; recommends accessibility, not mobility, as the evaluand. (Content M.)
- van Wee, B. and Geurs, K. "Discussing Equity and Social Exclusion in Accessibility Evaluations", *EJTIR* 11(4) (2011) [V] — [record](https://doi.org/10.18757/ejtir.2011.11.4.2940).
- Lucas, K., Martens, K., Di Ciommo, F. and Dupont-Kieffer, A. (eds) *Measuring Transport Equity* (Elsevier 2019) [V] — [record](https://doi.org/10.1016/b978-0-12-814818-1.00001-9).
- Delbosc, A. and Currie, G. "Using Lorenz curves to assess public transport equity", *Journal of Transport Geography* 19(6) 1252–1259 (2011) [V] — [record](https://doi.org/10.1016/j.jtrangeo.2011.02.008).
- Schwanen, T., Lucas, K., Akyelken, N., Cisternas Solsona, D., Carrasco, J.-A. and Neutens, T. "Rethinking the links between social exclusion and transport disadvantage through the lens of social capital", *Transportation Research Part A* 74 123–135 (2015) [V] — [record](https://doi.org/10.1016/j.tra.2015.02.012).
- Pereira, R. H. M., Banister, D., Schwanen, T. and Wessel, N. "Distributional effects of transport policies on inequalities in access to opportunities in Rio de Janeiro" (preprint 2017; *Journal of Transport and Land Use* 2019 [M]) [V for preprint] — [record](https://doi.org/10.31235/osf.io/cghx2).
- Hu, Gao, Farber and Geurs, "Accessibility equity and transport-related social exclusion", *Transportation Research Part D* 156 105405 (2026) [V] — [record](https://doi.org/10.1016/j.trd.2026.105405). (Content not read; shows the topic is active in 2026.)

**1.7 Energy justice.**
- McCauley, D., Heffron, R., Stephan, H. and Jenkins, K. "Advancing energy justice: the triumvirate of tenets", *International Energy Law Review* 32(3) 107–110 (2013) [M] (not found in Crossref). (M) Distributional, recognition and procedural justice as the three tenets. Heffron, R. J. "Energy justice – the triumvirate of tenets revisited and revised", *Journal of Energy & Natural Resources Law* 42(2) 227–233 (2024; online 2023) [V] — [record](https://doi.org/10.1080/02646811.2023.2256593).
- Jenkins, K., McCauley, D., Heffron, R., Stephan, H. and Rehner, R. "Energy justice: A conceptual review", *Energy Research & Social Science* 11 174–182 (2016) [V] — [record](https://doi.org/10.1016/j.erss.2015.10.004).
- Sovacool, B. K. and Dworkin, M. H. "Energy justice: Conceptual insights and practical applications", *Applied Energy* 142 435–444 (2015) [V] — [record](https://doi.org/10.1016/j.apenergy.2015.01.002). (M) Eight principles: availability, affordability, due process, transparency and accountability, sustainability, intragenerational equity, intergenerational equity, responsibility. (Content M.)
- Walker, G. and Day, R. "Fuel poverty as injustice: Integrating distribution, recognition and procedure in the struggle for affordable warmth", *Energy Policy* 49 69–75 (2012) [V] — [record](https://doi.org/10.1016/j.enpol.2012.01.044).
- Bouzarovski, S. and Simcock, N. "Spatializing energy justice", *Energy Policy* 107 640–648 (2017) [V] — [record](https://doi.org/10.1016/j.enpol.2017.03.064). (M) Energy poverty is produced through spatial processes (landscapes of material deprivation, uneven infrastructure, peripheralisation); spatial justice and energy justice are joined. (Content M.)

**1.8 Climate justice and adaptation finance.**
- Shi, L., Chu, E., Anguelovski, I., Aylett, A., Debats, J., Goh, K. et al. "Roadmap towards justice in urban climate adaptation research", *Nature Climate Change* 6(2) 131–137 (2016) [V] — [record](https://doi.org/10.1038/nclimate2841).
- Anguelovski, I., Shi, L., Chu, E., Gallagher, D., Goh, K., Lamb, Z., Reeve, K. and Teicher, H. "Equity Impacts of Urban Land Use Planning for Climate Adaptation", *Journal of Planning Education and Research* 36(3) 333–348 (2016) [V] — [record](https://doi.org/10.1177/0739456x16645166). (M) Adaptation plans produce injustice by "acts of commission" (protecting elites' assets, displacing the poor through green or protective infrastructure) and "acts of omission" (leaving low-income areas unprotected). (Content M.)
- Meerow, S., Pajouhesh, P. and Miller, T. R. "Social equity in urban resilience planning", *Local Environment* 24(9) 793–808 (2019) [V] — [record](https://doi.org/10.1080/13549839.2019.1645103). (M) A content analysis of city resilience plans found equity mentioned often but operationalised rarely, mostly distributively, seldom procedurally or as recognition. (Content M; the sample of plans was not checked.)
- Amorim-Maia, A. T., Anguelovski, I., Chu, E. and Connolly, J. "Intersectional climate justice", *Urban Climate* 41 101053 (2022) [V] — [record](https://doi.org/10.1016/j.uclim.2021.101053).
- Persson, Å. and Remling, E. "Equity and efficiency in adaptation finance: initial experiences of the Adaptation Fund", *Climate Policy* 14(4) 488–506 (2014) [V] — [record](https://doi.org/10.1080/14693062.2013.879514); Remling, E. and Persson, Å. "Who is adaptation for? Vulnerability and adaptation benefits in proposals approved by the UNFCCC Adaptation Fund", *Climate and Development* 7(1) 16–34 (2015) [V] — [record](https://doi.org/10.1080/17565529.2014.886992). (M) The Adaptation Fund lacked an operational definition of "particularly vulnerable", allocating largely first-come-first-served under a per-country cap; approved proposals often did not identify the most vulnerable beneficiaries. (Content M.)
- The Fund for responding to Loss and Damage (FRLD): its Barbados Implementation Modalities (2025) allocate a USD 250 million start-up phase for 2025–2026 with a floor of at least 50% for least developed countries and small island developing states combined, requests of USD 5–20 million, and a safeguard against over-concentration in any country or region; the floor was pushed by developed countries, while many developing countries wanted a softer target [S] — [Climate Analytics](https://climateanalytics.org/comment/loss-and-damage-fund-gearing-up-to-deliver-early-support); [UNFCCC FRLD slides](https://unfccc.int/sites/default/files/resource/FRLD%20-%20PPT_CW2_Startup%20phase%20update%2001_Sept.pdf). (A)
- (M) The Green Climate Fund aims at a 50:50 balance of mitigation and adaptation in grant-equivalent terms and a floor of 50% of adaptation resources for particularly vulnerable countries (LDCs, SIDS, African states) [M].
- Khan, M., Robinson, S.-a., Weikmans, R., Ciplet, D. and Roberts, J. T. "Twenty-five years of adaptation finance through a climate justice lens", *Climatic Change* 161(2) 251–269 (2020; online 2019) [V] — [record](https://doi.org/10.1007/s10584-019-02563-x); Ciplet, D., Roberts, J. T. and Khan, M. R. *Power in a Warming World* (MIT Press 2015) [V] — [record](https://doi.org/10.7551/mitpress/9780262029612.001.0001).
- Weiler, F., Klöck, C. and Dornan, M. "Vulnerability, good governance, or donor interests? The allocation of aid for climate change adaptation", *World Development* 104 65–77 (2018) [V] — [record](https://doi.org/10.1016/j.worlddev.2017.11.001). (M) Adaptation aid responds only weakly to vulnerability; donor interests and recipient governance matter. (Content M.)
- Hughes, S. "Justice in Urban Climate Change Adaptation: Criteria and Application to Delhi", *Ecology and Society* 18(4) (2013) [V] — [record](https://doi.org/10.5751/es-05929-180448).

**1.9 Distributive principles that order "which first, then which".**
- Rawls, J. *A Theory of Justice* (Belknap/Harvard 1971) [S, via review records, e.g. [Feibleman 1973](https://doi.org/10.1093/ajj/18.1.198)]. (M) Difference principle (inequalities must benefit the least advantaged), lexically after equal liberty and fair equality of opportunity. (Content M.)
- Frankfurt, H. "Equality as a Moral Ideal", *Ethics* 98(1) 21–43 (1987) [V] — [record](https://doi.org/10.1086/292913) (sufficientarianism: what matters is that everyone has enough); Casal, P. "Why Sufficiency Is Not Enough", *Ethics* 117(2) 296–326 (2007) [V] — [record](https://doi.org/10.1086/510692) (sufficiency must be supplemented by distributive principles above the threshold); Parfit, D. "Equality and Priority" (1997; reprint record) [V] — [record](https://doi.org/10.4324/9781315199795-18) (prioritarianism: benefits matter more the worse off the recipient); Daniels, N. *Just Health Care* (CUP 1985) [V] — [record](https://doi.org/10.1017/cbo9780511624971) (need as what protects normal opportunity range); Brighouse, H. and Swift, A. "Educational Equality versus Educational Adequacy", *Journal of Applied Philosophy* 26(2) 117–128 (2009) [V] — [record](https://doi.org/10.1111/j.1468-5930.2009.00438.x); Arneson, R. "Equality and Equal Opportunity for Welfare" (1989; reprint record) [V] — [record](https://doi.org/10.4324/9781315199795-9). (Contents M.)
- Atkinson, A. B. "On the measurement of inequality", *Journal of Economic Theory* 2(3) 244–263 (1970) [V] — [record](https://doi.org/10.1016/0022-0531(70)90039-6); Kolm, S.-C. "Unequal inequalities I/II", *JET* 12(3) 416–442; 13(1) 82–111 (1976) [V] — [record I](https://doi.org/10.1016/0022-0531(76)90037-5), [record II](https://doi.org/10.1016/0022-0531(76)90068-5). Formula (M): Atkinson's equally distributed equivalent $y_{\varepsilon} = \big(\frac1N\sum_i y_i^{1-\varepsilon}\big)^{1/(1-\varepsilon)}$ and index $A_\varepsilon = 1 - y_\varepsilon/\bar y$; the inequality-aversion parameter $\varepsilon$ is the same normative quantity as the distributional-weight elasticity $\eta$ in section 5.
- These give the formal vocabulary for the user's question of order: sufficientarian (close gaps below a threshold first), prioritarian (weight gains by how badly off the recipient is, e.g. $w=y^{-\eta}$), maximin (raise the worst-off unit first), egalitarian (reduce dispersion), utilitarian (maximise the sum).

#### Inferences
- The theories converge on a *lexicographic* structure that a tool can hold: first secure thresholds (needs, sufficiency of access, the right not to be displaced), then distribute the remainder by need, contribution or desert, with procedural and recognitional conditions as constraints on how the allocation is reached rather than as terms in its objective.
- Harvey's "contribution to the common good" corresponds to the economist's agglomeration or multiplier argument (section 3); a tool can represent it as a separate, contestable term rather than burying it in efficiency.
- Young's and Purcell's critiques mean a tool must not claim that an optimal allocation is a just city; it can only show what a declared principle implies.

#### Gaps
- McCauley et al. 2013 (International Energy Law Review) and Lefebvre 1968 have no Crossref record; they stay [M].
- The exact functional form of Martens' ASI was not read; the FGT-style formula above is from memory.
- No primary GCF Governing Instrument text was read; the 50% adaptation floor is from memory.

### 2. Horizontal Fiscal Equalisation Allocates among Places by Assessed Needs Net of Capacities

#### Takeaway
Fiscal equalisation is the most mature, most formula-explicit practice of allocating money among places. Every system computes, for each jurisdiction, a standardised expenditure need (population times cost and demand factors) and a standardised revenue capacity (tax bases times a standard rate), and pays a share of the difference. The systems differ in the standard (average, top, a named state), in the share equalised (63% in Germany, 100% in Australia before 2018 and in England's council tax equalisation), in what is counted (resources at 50% in Canada), and above all in floors, caps and guarantees bolted on for political acceptance. Australia's 2018 reforms, judged by the Productivity Commission's 2026 interim report to have "largely not achieved their goals" at more than four times their projected cost, are a warning that guarantees can undo equalisation.

#### Cited Findings

**2.1 The general formula (representative tax system and expenditure needs).**
- Formula (M; standard in Boadway and Shah's texts, equation numbers not checked). For jurisdiction $i$ with population $P_i$, per-capita standardised expenditure need $e_i = \bar e \cdot n_i$ (where $\bar e$ is the national average per-capita spending and $n_i$ a need/cost index with population-weighted mean 1), per-capita tax base $b_{ik}$ for each base $k$, and standard (national average) rate $t_k = \sum_j R_{jk}/\sum_j B_{jk}$:
  $$G_i = \lambda\, P_i\Big[(e_i - \bar e) + \sum_k t_k(\bar b_k - b_{ik})\Big] + P_i\, g_0,$$
  where the bracket is the "fiscal gap" relative to the standard, $\lambda\in(0,1]$ the equalisation rate and $g_0$ an equal per-capita component that makes the pool self-financing (net-zero for $\lambda$-only Robin Hood schemes). The revenue term is the representative tax system (RTS); the need term is expenditure needs assessment, by regression on spending ("regression-based needs"), by service-specific unit costs times client counts ("bottom-up" or "assessed expenses"), or by indicators.
- Boadway, R. "The Theory and Practice of Equalization", *CESifo Economic Studies* 50(1) 211–254 (2004) [V] — [record](https://doi.org/10.1093/cesifo/50.1.211). (M) Rationales: horizontal equity (equal treatment of equals across jurisdictions, after Buchanan's "fiscal residuum"), efficiency (avoiding fiscally induced migration), and insurance; conflicts with incentives (tax effort, resource development). (Content M.)
- Buchanan, J. M. "Federalism and Fiscal Equity", *American Economic Review* 40(4) 583–599 (1950) [M] (not found in Crossref). (M) Equally situated individuals should receive equal fiscal residuum (benefits minus taxes) wherever they live; the original horizontal-equity argument for equalisation.
- Dahlby, B. and Wilson, L. S. "Fiscal Capacity, Tax Effort, and Optimal Equalization Grants", *Canadian Journal of Economics* 27(3) 657 (1994) [V] — [record](https://doi.org/10.2307/135789). (M) Equalising actual revenues penalises tax effort; equalising capacity at standard rates avoids this but RTS bases can still respond to policy. (Content M.)
- Ferede, E. "The Incentive Effects of Equalization Grants on Fiscal Policy", University of Calgary School of Public Policy Publications 7 (2014) [V] — [record](https://doi.org/10.55016/ojs/sppp.v7i1.42478). (Content not read.)
- Shah, A. *A Practitioner's Guide to Intergovernmental Fiscal Transfers*, World Bank Policy Research WP 4039 (2006) [V] — [record](https://doi.org/10.1596/1813-9450-4039); Boadway, R. and Shah, A. (eds) *Intergovernmental Fiscal Transfers: Principles and Practice* (World Bank 2007; record dated 2006) [V] — [record](https://doi.org/10.1596/978-0-8213-6492-5).
- Bradbury, K. L., Ladd, H. F., Perrault, M., Reschovsky, A. and Yinger, J. "State Aid to Offset Fiscal Disparities Across Communities", *National Tax Journal* 37(2) 151–170 (1984) [V] — [record](https://doi.org/10.1086/ntj41791942); Ladd, H. F. and Yinger, J. *America's Ailing Cities: Fiscal Health and the Design of Urban Policy* (Johns Hopkins 1989) [V via reviews] — [JPAM review](https://doi.org/10.2307/3325288); Wang, "Measuring Fiscal Condition as Need-Capacity Gap: An Illustration of State Aid to Counties in Michigan", *Municipal Finance Journal* 43(4) 43–63 (2023) [V] — [record](https://doi.org/10.1086/mfj43040043). (M) The "need–capacity gap" of a municipality (standardised expenditure need minus revenue capacity at a standard rate) as the basis of state aid to cities; the intra-metropolitan analogue of equalisation. (Content M.)
- Garnaut, R. and FitzGerald, V. "Issues in Commonwealth–State Funding", *Australian Economic Review* 35(3) 290–300 (2002) [V] — [record](https://doi.org/10.1111/1467-8462.00245) (a critique of the CGC's complexity and incentives); Petchey, J. and Levtchenkova, S. "Fiscal Capacity Equalization and Economic Efficiency: The Case of Australia", in *Fiscal Equalization* (Springer, n.d. in Crossref) 13–30 [V] — [record](https://doi.org/10.1007/978-0-387-48988-9_2); Rodden, J. *Hamilton's Paradox* (CUP 2005) [V] — [record](https://doi.org/10.1017/cbo9780511616075) (soft budget constraints when transfers dominate subnational revenue).
- Blöchliger, H. et al. *Fiscal Equalisation in OECD Countries*, OECD Working Papers on Fiscal Federalism 4 (2007) [V] — [record](https://doi.org/10.1787/5k97b11n2gxx-en); OECD, *Evaluating fiscal equalisation* (2021) [V] — [record](https://doi.org/10.1787/253da2b8-en); Dougherty, S., Montes Nebreda, A. and Urrutia, I. *Intergovernmental fiscal transfers and fiscal equalisation in a time of consolidation*, OECD WP (2025) [V] — [record](https://doi.org/10.1787/4853a4d0-en). (Contents not read.)

**2.2 Australia: the Commonwealth Grants Commission and the 2018 reforms.**
- (M) The CGC's statement of HFE: state governments should receive funding from the GST pool such that, after allowing for material factors affecting revenues and expenditures, each would have the fiscal capacity to provide services and associated infrastructure at the same standard, if each made the same effort to raise revenue from its own sources and operated at the same level of efficiency. Supporting principles: "what states collectively do" (average policy), policy neutrality, practicality, contemporaneity. Assessed by category: assessed expenses (socio-demographic composition, remoteness, wage costs, Indigenous status, service delivery scale, etc.) and assessed revenue capacity (payroll, land, stamp duty, mining royalties assessed mineral by mineral), averaged over three years lagged. Formula (M): state $i$'s assessed GST requirement $= $ assessed expenses $-$ assessed own-source revenue $-$ (included) Commonwealth payments; its relativity $r_i = $ per-capita assessed GST requirement$_i$ / national per-capita GST; GST share $=\; r_i P_i / \sum_j r_j P_j$. [CGC methodology documents were located but timed out on fetch.]
- The 2025 Methodology Review concluded in February 2025 with the Commission keeping the 2020 approach for the first step of GST distribution, keeping a mineral-by-mineral mining assessment (rejecting an aggregate or single-revenue assessment that would give too much weight to a dominant factor), and proposing external advice on its method given states' unease; Western Australia argued "true" HFE cannot be calculated (A, from a search summary of CGC documents) [S] — [CGC 2025 Review outcomes (HFE)](https://cgc.gov.au/sites/default/files/2025-03/2025R%20-%20Review%20outcomes%20-%20HFE_FINAL.pdf).
- The 2018 legislation benchmarks distribution to the fiscally stronger of New South Wales or Victoria (the "standard state"), sets a relativity floor (0.75 since 2024–25, 0.7 before), and legislates a No Worse Off (NoWO) guarantee from 2021–22 to 2026–27; on 29 November 2024 the Commonwealth and all states except Western Australia agreed to extend NoWO payments to 2029–30 (A) [S] — [CGC: 2018 GST distribution legislation](https://www.cgc.gov.au/node/53); [Budget Paper 3 2025–26](https://archive.budget.gov.au/2025-26/bp3/download/bp3_13_part_3.pdf).
- Productivity Commission, *GST distribution reforms*, interim report released 14 August 2026, final report due 31 December 2026 [V, page fetched] — [PC inquiry page](https://www.pc.gov.au/inquiries-and-research/gst-reforms/interim). (A) Findings: the 2018 changes "have largely not achieved their goals and have cost more than 4 times as much as projected" (Deputy Chair Alex Robson); almost $23 billion to 2024–25, mostly NoWO payments; before 2018 all states received enough GST to meet 100% of assessed fiscal needs, but in 2024–25 Western Australia received enough for 113% and other states 98%; only WA benefited from the standard-state benchmark, which can reverse the logic of distribution (e.g. extra GST to WA after a disaster elsewhere); lifting other states to WA's capacity would have cost about $47 billion in 2024–25; NoWO costs about $6 billion a year and could reach $12 billion, uncapped; little evidence that GST distribution shaped state policy decisions, but "dominant-state effects" are real (a mining-dominant state raising royalties loses more GST than from raising payroll tax). Interim recommendations: return to the pre-2018 system and have the Treasurer direct the CGC to address costly dominant-state effects; or return and pay affected states directly and transparently; failing both, remove the standard-state benchmark (reactivating the 0.75 floor) and make NoWO permanent.
- NSW Government proposal of a relativity safety-net floor of 0.50 funded by the Commonwealth (date not checked) [S] — [NSW ministerial release](https://nsw.gov.au/ministerial-releases/gst-grants-formula-should-be-simple-predictable-and-transparent).

**2.3 Canada: the ten-province standard.**
- (A) Before adjustments a province's per-capita equalisation entitlement equals the amount by which its fiscal capacity falls below the average fiscal capacity of all ten provinces (the "10 province standard"); since 2007 resource revenues are assessed by partial inclusion of actual revenue, and receiving provinces get the better of a calculation including 50% of resource revenues or excluding them; a fiscal capacity cap (2007) prevents payments from raising a receiving province's fiscal capacity above that of a non-receiving province when all resource revenues are counted; since 2009 total payments grow with nominal GDP (binding when disparities grow faster); fiscal capacity uses a weighted three-year moving average lagged two years [S, from a search summary of Finance Canada and Library of Parliament pages] — [Finance Canada: Equalization Program](https://www.canada.ca/en/department-finance/programs/federal-transfers/equalization.html); [Library of Parliament, *Canada's Equalization Formula*](https://lop.parl.ca/en/researchpublications/200820e).
- Formula (from the above, M for notation): $E_i = \max\{0,\; P_i(\overline{FC} - FC_i)\}$ with $FC$ per-capita fiscal capacity at national average rates across five revenue bases (personal income, business income, consumption, property, natural resources), $\overline{FC}$ the ten-province average; then the better of the 50%/0% resource treatments, the cap, and a pro-rata adjustment so the total equals the GDP-indexed envelope. Canada has no expenditure-needs side (a long-standing critique).
- Payments of $25.253 billion in 2024–25 [S] — [Johnson Shoyama School policy brief](https://www.schoolofpublicpolicy.sk.ca/research-ideas/publications-and-policy-insight/policy-brief/policy-paper-equalisation-canada.php); removing the fiscal capacity cap could shift entitlements by as much as 1.7% of GDP in Newfoundland and Labrador [S] — [PBO 2018](https://publications.gc.ca/collections/collection_2018/dpb-pbo/YN5-154-2018-eng.pdf).

**2.4 Germany: from Länderfinanzausgleich to Finanzkraftausgleich (2020).**
- (A) From 1 January 2020 the horizontal Länderfinanzausgleich in its earlier form was abolished; equalisation now runs through the distribution of the Länder share of VAT (Finanzkraftausgleich) using financial-capacity and equalisation measures with a uniform equalisation rate of 63% of the shortfall or surplus against the equalisation measure (§10 FAG); city-state inhabitants are weighted 135% (§9 FAG); municipal tax capacity enters at 75% (§8 FAG); federal supplementary grants (Bundesergänzungszuweisungen) continue vertically; in 2024 the Länder received 49.1% of VAT revenue [S, from a search summary of Bundestag Wissenschaftliche Dienste, Wirtschaftsdienst and BMF documents] — [Bundestag WD 4-056/21](https://www.bundestag.de/resource/blob/850474/6d276d4e4406edc5d28b0d0e5f1ca17d/WD-4-056-21-pdf.pdf); [Wirtschaftsdienst 2020](https://www.wirtschaftsdienst.eu/inhalt/jahr/2020/heft/8/beitrag/laenderfinanzausgleich-2020-neue-form-alte-probleme.html); [BMF Monatsbericht April 2025](https://www.bundesfinanzministerium.de/Monatsberichte/Ausgabe/2025/04/Inhalte/Kapitel-3-Analysen/3-1-bundesstaatlicher-finanzausgleich-2024.html).
- Formula (from the above): $Z_i = 0.63\,(AMZ_i - FKZ_i)$, where $FKZ_i$ is Land $i$'s financial capacity (Land taxes plus 75% of municipal taxes) and $AMZ_i = \frac{\sum_j FKZ_j}{\sum_j w_j P_j}\, w_i P_i$ its equalisation measure, with $w_i=1.35$ for Berlin, Hamburg and Bremen and 1 otherwise (Germany's only "need" factor in the horizontal step; municipal population weighting is reported in the sources but not verified here). Positive $Z_i$ is a VAT supplement, negative a deduction.
- Färber, G. "Intergovernmental Fiscal Relations during the COVID-19 Pandemic in Germany: Was the New Fiscal Equalisation Scheme able to Hamper Financial Dislocations?", in *Fiscal Federalism and Equalisation Transfers* (Brill 2025) 161–179 [V] — [record](https://doi.org/10.1163/9789004524514_009). (Content not read.)

**2.5 England: the Fair Funding Review 2.0 and the 2026–27 settlement.**
- MHCLG, *Government response to the Fair Funding Review 2.0* (November 2025) [V, text read] — [PDF](https://assets.publishing.service.gov.uk/media/691f15c59c8e8f345bf985a2/Fair_Funding_Review_2.0_Government_Response.pdf). (T) Key decisions:
  - Needs: formulas reduced from 15 to 9 (Foundation Formula upper and lower tier; adult social care; children and young people's services; temporary accommodation; fire and rescue; others), kept current by population projections and regular resets; deprivation from the **2025 English Indices of Multiple Deprivation, whose income measure is now after housing costs**, and IDACI for children's services. A previous government's 2018 review "did not implement the proposals consulted on" (§1.1.2).
  - Costs: an Area Cost Adjustment for each formula (labour costs, rates, accessibility); a contested "remoteness adjustment" applied only to the adult social care formula (§1.2.2); 42% of respondents raised concerns about remoteness, 32% strongly opposed it (§5.2.5, §5.3.7).
  - Resources: "the government will act as the equaliser for locally raised tax" (§1.2.4): a resource adjustment deducts each authority's council-tax capacity at a notional council tax level set at the national average to achieve "full fiscal equalisation" (§6.1.10): £2,060 (2026–27), £2,160 (2027–28), £2,265 (2028–29) (§6.1.11), with an assumed collection rate of 100%. 29% of respondents argued equalisation penalises places that kept council tax low; 17% asked for partial equalisation (§6.1.5–6.1.6).
  - Transition: authorities move to their Fair Funding Assessment in thirds over the three-year settlement; floors protect 100% of 2025–26 income for most authorities, 95% for authorities 15% or more above their Fair Funding Assessment (upper tier only if their council tax is below average), and real-terms income for standalone fire and rescue authorities (§9.4.12–9.4.13); a £600m Recovery Grant for the most deprived authorities is kept, with a "Recovery Grant Guarantee" capped at £35m per authority (§1.2.3); the business rates safety net rises to 100% of baseline in 2026–27, returning to 92.5% (§1.2.7); "by 2028-29, we expect that the 10% most deprived authorities will see a significant increase in their Core Spending Power per head compared to the least deprived" (§1.1.4).
  - Consultation dissent: 11% of respondents disagreed with including deprivation or its weighting in the Foundation Formula; 5% questioned the IMD's suitability; 8% argued deprivation should account for housing costs (§4.1.6–4.1.7).
- Formula (structure from the response's text; notation M): $\mathrm{FFA}_i = T\sum_k s_k\,\frac{N_{ik}\,\mathrm{ACA}_{ik}}{\sum_j N_{jk}\,\mathrm{ACA}_{jk}} - \tau^{*}\,\mathrm{CTB}_i$, where $N_{ik}$ is relative need from formula $k$ (a regression-weighted sum of client counts and deprivation indicators), $s_k$ the control-total share of service $k$, $\mathrm{CTB}_i$ the Band D-equivalent council tax base (after mandatory discounts and exemptions) and $\tau^*$ the notional Band D level; allocations phased $A_{i,t} = A_{i,2025} + \frac{t}{3}(\mathrm{FFA}_i - A_{i,2025})$, then floored.
- The provisional settlement was published 17 December 2025; the final settlement debated in the House of Commons on 11 February 2026, adding £740m to the £3.4bn of new grant [S] — [LGA briefing](https://www.local.gov.uk/parliament/briefings-and-responses/debate-local-government-finance-settlement-202627-house-commons); a council's papers report the national Fair Funding Allocation as £33.9bn rising to £34.9bn by 2028–29 [S] — [Havering Appendix B](https://democracy.havering.gov.uk/documents/s82699/5.2%20APPENDIX%20B%20The%20Local%20Government%20Finance%20Settlement.pdf); rural and county councils criticised the outcome [S] — [Local Government Lawyer](https://www.localgovernmentlawyer.co.uk/governance/396-governance-news/99099-government-unveils-first-multi-year-finance-settlement-in-a-decade-amid-criticism-from-rural-and-county-councils).

**2.6 Sweden (a Nordic system).**
- (A) The *kommunalekonomisk utjämning* (Lag 2004:773; Förordning 2004:881; in force since 1 January 2005) has five parts for both municipalities and regions: income equalisation, cost equalisation, structural grants, introduction grants and a regulating grant or fee; income equalisation guarantees a tax base of 115% of the national mean per capita (municipalities and regions above the guarantee level pay a fee); cost equalisation compensates for cost differences due to structural factors such as settlement pattern and age composition; a committee appointed in 2022 is reviewing the system [S, from a search summary of SCB and Riksdag documents] — [SCB, Beskrivning av kommunalekonomisk utjämning 2025](https://scb.se/contentassets/d93ddb5be2504691ba3912701fc1e32a/beskrivning-av-kommunalekonomisk-utjamning-2025_prel.pdf); [Lag 2004:773](https://data.riksdagen.se/dokument/sfs-2004-773.html).
- Formula fragment (A): grant or fee $=$ equalisation base $\times$ county tax rate $/$ population $/100$; the compensation rates (shares of the gap paid) were not verified.

#### Inferences
- Every equalisation system is a two-sided assessment (needs, capacities) plus a political envelope (floors, caps, guarantees, phasing). A justice-aware city tool should hold the two sides and the envelope separately and report how much of the final allocation each envelope term moves, because the PC's finding shows the envelope can dominate.
- The choice of standard is a justice choice: average (Canada, Germany, England's notional Band D) versus the stronger of two named states (Australia since 2018) changes who is lifted and who pays.
- Full equalisation of tax capacity (England) treats a low local tax as a choice the place must fund itself; partial equalisation rewards low-tax places. The 29%/17% split in England's consultation is the live form of Buchanan versus local autonomy.
- Within a city, the same structure applies to allocating among wards or districts: need indicators times unit costs, minus local resources (business improvement levies, developer contributions, ward-raised funds), times an equalisation rate.

#### Gaps
- The CGC's own methodology text timed out; its HFE statement and formula above are from memory.
- Canada's and Germany's parameters are from search summaries of official sources, not the statutes read in full; Sweden's compensation rates were not found.
- No independent quantitative evaluation of England's new formulas (e.g. by IFS; their page returned 403) was read.

### 3. Place-Based Policy Economics Asks Who Captures Spending Directed at Places

#### Takeaway
The economics of place-based policy cuts against naive geographic targeting: in spatial equilibrium, benefits directed at a place are partly capitalised into land and housing (going to owners, not renters), partly dissipated by in-migration, and partly offset by losses elsewhere when agglomeration is not strongly convex. Yet recent optimal-policy work (Gaubert, Kline and Yagan; Fajgelbaum and Gaubert) shows that location is an informative "tag" for redistribution and that place-based transfers can be efficient where non-employment is elastic or spillovers are local. The tool must report incidence (who ends up with the money), not only where the money lands.

#### Cited Findings
- Kline, P. and Moretti, E. "People, Places, and Public Policy: Some Simple Welfare Economics of Local Economic Development Programs", *Annual Review of Economics* 6 629–662 (2014) [V] — [record](https://doi.org/10.1146/annurev-economics-080213-041024). (M) In a spatial equilibrium model with mobile workers and inelastic housing, a subsidy to a place raises wages and rents; with perfect mobility, workers are indifferent and landowners capture the gain; equity rationales for place-based aid are weak if the poor are mobile and renters; efficiency rationales are agglomeration economies, spatial mismatch and local labour-market frictions (hysteresis). (Content M.)
- Kline, P. and Moretti, E. "Local Economic Development, Agglomeration Economies, and the Big Push: 100 Years of Evidence from the Tennessee Valley Authority", *Quarterly Journal of Economics* 129(1) 275–331 (2014; issue dated 2014, Crossref issued 2013) [V] — [record](https://doi.org/10.1093/qje/qjt034). (M) TVA's agricultural gains faded after federal transfers ended; manufacturing gains persisted through agglomeration; national productivity gains were modest because agglomeration effects were roughly linear, so local gains came partly at the expense of other places. (Content M.)
- Busso, M., Gregory, J. and Kline, P. "Assessing the Incidence and Efficiency of a Prominent Place Based Policy", *American Economic Review* 103(2) 897–947 (2013) [V] — [record](https://doi.org/10.1257/aer.103.2.897). (M) Federal Empowerment Zones raised employment and wages of zone residents working in the zone; rents and population responses were small; the deadweight loss was modest relative to the transfer. (Content M.)
- Austin, B., Glaeser, E. and Summers, L. "Jobs for the Heartland: Place-Based Policies in 21st-Century America", *Brookings Papers on Economic Activity* 2018(1) 151–255 [V] — [record](https://doi.org/10.1353/eca.2018.0002). (M) Persistent non-employment concentrated in the "eastern heartland"; argue for place-sensitive (not place-specific) policies such as employment subsidies scaled where the responsiveness of non-employment is higher, because a dollar there buys more employment. (Content M.)
- Neumark, D. and Simpson, H. "Place-Based Policies", *Handbook of Regional and Urban Economics* vol. 5 (Elsevier 2015) 1197–1287 [V] — [record](https://doi.org/10.1016/b978-0-444-59531-7.00018-1). (M) Reviews enterprise zones (mixed, often null employment effects), discretionary grants, cluster policies, infrastructure and EU structural funds; warns of displacement across places. (Content M.)
- Glaeser, E. and Gottlieb, J. D. "The Economics of Place-Making Policies", *Brookings Papers on Economic Activity* 2008(1) 155–253 [V] — [record](https://doi.org/10.1353/eca.0.0005). (M) Spatial equilibrium implies that place-making benefits are capitalised; the case for place-based policy requires nonlinear agglomeration, which evidence does not clearly support; "help poor people, not poor places". (Content M.)
- Gaubert, C., Kline, P. and Yagan, D. "Place-Based Redistribution", NBER WP 28337 (2021) [V] — [record](https://doi.org/10.3386/w28337) (later version with Vergara; journal publication not checked [M]). (M) In an optimal income tax framework with location, place-based transfers can improve on a purely income-based schedule because location reveals information about earnings ability and because transfers to low-wage places are partly offset by migration and rent responses; the optimal policy is modestly progressive in place. (Content M.)
- Fajgelbaum, P. D. and Gaubert, C. "Optimal Spatial Policies, Geography, and Sorting", *Quarterly Journal of Economics* 135(2) 959–1036 (2020) [V] — [record](https://doi.org/10.1093/qje/qjaa001). (M) With spillovers and sorting, the spatial equilibrium is inefficient; optimal policies are place-based transfers that depend on local spillover elasticities; for the US the authors find optimal transfers would reduce wage inequality across cities and raise welfare. (Content M.)
- Moretti, E. "Local Multipliers", *American Economic Review* 100(2) 373–377 (2010) [V] — [record](https://doi.org/10.1257/aer.100.2.373). (M) Each new tradable job creates about 1.6 local non-tradable jobs (2.5 for high-skilled). (Content M.)
- Ehrlich, M. v. and Overman, H. G. "Place-Based Policies and Spatial Disparities across European Cities", *Journal of Economic Perspectives* 34(3) 128–149 (2020) [V] — [record](https://doi.org/10.1257/jep.34.3.128). (M) EU cohesion spending has positive but modest effects; spatial disparities persist. (Content M.)
- Becker, S. O., Egger, P. H. and von Ehrlich, M. "Going NUTS: The effect of EU Structural Funds on regional performance", *Journal of Public Economics* 94(9–10) 578–590 (2010) [V] — [record](https://doi.org/10.1016/j.jpubeco.2010.06.006). (M) Regression discontinuity at the 75%-of-EU-average GDP per capita eligibility threshold for Objective 1 status; Objective 1 raised GDP per capita growth by roughly 1.6 percentage points, without employment effects. (Content M.) This is the canonical case of an index threshold (section 4) used for allocation among regions.
- Iammarino, S., Rodríguez-Pose, A. and Storper, M. "Regional inequality in Europe: evidence, theory and policy implications", *Journal of Economic Geography* 19(2) 273–298 (2019) [V] — [record](https://doi.org/10.1093/jeg/lby021); Rodríguez-Pose, A. "The Rise of Populism and the Revenge of the Places That Don't Matter", *LSE Public Policy Review* 1(1) (2020) [V] — [record](https://doi.org/10.31389/lseppr.4) and Rodríguez-Pose, A. "The revenge of the places that don't matter (and what to do about it)", *Cambridge Journal of Regions, Economy and Society* 11(1) 189–209 (2018) [V] — [record](https://doi.org/10.1093/cjres/rsx024). (M) "Places that don't matter" (left-behind regions) express discontent politically; place-sensitive development policy is justified on political-economy as well as efficiency grounds. (Content M.)
- Diemer, A., Iammarino, S., Rodríguez-Pose, A. and Storper, M. "The Regional Development Trap in Europe", *Economic Geography* 98(5) 487–509 (2022) [V] — [record](https://doi.org/10.1080/00130095.2022.2080655).
- Chetty, R., Hendren, N. and Katz, L. F. "The Effects of Exposure to Better Neighborhoods on Children: New Evidence from the Moving to Opportunity Experiment", *American Economic Review* 106(4) 855–902 (2016) [V] — [record](https://doi.org/10.1257/aer.20150572). (M) Children moved to lower-poverty areas before age 13 earned about 31% more as adults; people-based mobility can beat place-based investment for the movers, but it leaves places behind. (Content M.) Chetty, R. and Hendren, N. "The Impacts of Neighborhoods on Intergenerational Mobility I/II", NBER WPs 23001/23002 (2016) [V] — [record I](https://doi.org/10.3386/w23001), [record II](https://doi.org/10.3386/w23002).
- Hendren, N. and Sprung-Keyser, B. "A Unified Welfare Analysis of Government Policies", *Quarterly Journal of Economics* 135(3) 1209–1318 (2020) [V] — [record](https://doi.org/10.1093/qje/qjaa006). (M) The marginal value of public funds (MVPF = willingness to pay of beneficiaries / net government cost) ranks policies; direct investments in low-income children often have very high or infinite MVPF.

#### Inferences
- A tool that allocates to places should hold a spatial-equilibrium incidence module: given local housing supply elasticity, renter share and migration elasticity, it should apportion each place-directed dollar among (i) residents' consumption, (ii) landowners' rents, (iii) in-migrants, (iv) other places (displacement), with ranges, not points.
- "People versus places" is not binary in the newer literature: location is a tag. A tool can present both a people-based allocation (to households by need wherever they live) and a place-based one (to areas), and show the gap between them as the price of geographic targeting.
- Becker et al. show a threshold rule creates sharp discontinuities in entitlement; a just tool should smooth or justify thresholds (section 4).

#### Gaps
- Gaubert–Kline–Yagan's final published version and its quantitative results were not read.
- No study was found that measures the incidence of *intra-city* (ward-level) allocations as opposed to federal place-based programmes.

### 4. Index-Based Targeting Turns Justice into Thresholds on Composite Scores

#### Takeaway
Screening indices (England's IMD, CalEnviroScreen, the US CEJST, the Area Deprivation Index, social vulnerability indices) make allocation tractable and contestable by publishing indicators, weights and thresholds. Their critiques are serious and well documented: arbitrary weights and thresholds that flip areas in or out on tiny differences, aggregation that hides cumulative burden, relative ranks that cannot show change, ecological fallacy (most deprived people do not live in the most deprived areas), the modifiable areal unit problem, sensitivity of rankings to methodological choices, and, in the US, the political vulnerability of race-neutral and race-conscious designs alike (CEJST was taken offline on 21 January 2025). A justice-aware tool should treat any index as one reading among several, show its sensitivity, and never let a threshold be the only route to eligibility.

#### Cited Findings

**4.1 England's Indices of Deprivation 2025.**
- MHCLG, *English indices of deprivation 2025: statistical release* (30 October 2025; updated 17 November 2025) [V, page fetched] — [GOV.UK](https://www.gov.uk/government/statistics/english-indices-of-deprivation-2025/english-indices-of-deprivation-2025-statistical-release). (T) 33,755 LSOAs (2021 geography); IMD25 weights: Income 22.5%, Employment 22.5%, Education, Skills and Training 13.5%, Health Deprivation and Disability 13.5%, Crime 9.3%, Barriers to Housing and Services 9.3%, Living Environment 9.3%. Warnings in the release: "deprivation is measured on a relative rather than an absolute scale"; "Indices releases should not be viewed or construed as a time-series"; "Many non-deprived people live in deprived areas, and many deprived people live in non-deprived areas". Licence: "All content is available under the Open Government Licence v3.0, except where otherwise stated."
- The income domain's threshold is for the first time after housing costs (A) [S] — [Lloyd, QUB note](https://www.qub.ac.uk/research-centres/GIS/Research/Deprivation/FileStore/Lloyd_ToD_England061125_corrected.pdf); [GLA Datastore analysis](https://data.london.gov.uk/blog/indices-of-deprivation-2025-an-exploratory-analysis); confirmed by the FFR response (§1.2.1, T).
- Aggregation (M): domain scores are ranked, ranks transformed to an exponential distribution, $X = -23\ln\{1 - R[1-\exp(-100/23)]\}$ with $R$ the normalised rank in $(0,1]$, then combined with the weights above; the exponential transformation limits compensation between domains (deprivation in one domain is only partly cancelled by low deprivation in another). Technical report not read — [ID 2025 Technical Report](https://assets.publishing.service.gov.uk/media/68ff59c80f801e57b5bef907/ID_2025_Technical_Report.pdf).
- Noble, M., Wright, G., Smith, G. and Dibben, C. "Measuring Multiple Deprivation at the Small-Area Level", *Environment and Planning A* 38(1) 169–185 (2006) [V] — [record](https://doi.org/10.1068/a37168). (M) The IMD's conceptual basis (multiple deprivation as distinct domains, area as the unit of measurement not of the phenomenon).
- Deas, I., Robson, B., Wong, C. and Bradford, M. "Measuring Neighbourhood Deprivation: A Critique of the Index of Multiple Deprivation", *Environment and Planning C* 21(6) 883–903 (2003) [V] — [record](https://doi.org/10.1068/c0240). (M) Criticises the weighting, the shrinkage estimation that pulls small areas toward their district, the use of ranks for funding, and the misfit between index geography and policy areas. (Content M.)

**4.2 CalEnviroScreen.**
- OEHHA, *CalEnviroScreen 4.0* (October 2021) [S; OEHHA page timed out empty, report located] — [OEHHA scoring page](https://oehha.ca.gov/node/1668); [report PDF](https://calenviroscreen.oehha.ca.gov/sites/default/files/media/downloads/calenviroscreen/report/calenviroscreen40reportf2021.pdf). (A) Four components: Exposures and Environmental Effects (Pollution Burden; 13 indicators) and Sensitive Populations and Socioeconomic Factors (Population Characteristics; 8 indicators); indicator percentiles are averaged within components; Environmental Effects is weighted one-half relative to Exposures; the final score is the product Pollution Burden × Population Characteristics. Formula (partly M): $\mathrm{PB} = \frac{\bar p_{\text{exp}} + 0.5\,\bar p_{\text{env}}}{1.5}$, $\mathrm{PC} = \frac{\bar p_{\text{sens}} + \bar p_{\text{ses}}}{2}$, each rescaled to a 0–10 range by dividing by its statewide maximum, and $\mathrm{CES} = \mathrm{PB}\times\mathrm{PC}\in[0,100]$ by census tract. The multiplicative form means a tract needs both burden and vulnerability to score high (a deliberate non-compensatory choice). (M) Under SB 535 (2012), CalEPA designates the top 25% of tracts by score (plus some others) as "disadvantaged communities" for a statutory share of cap-and-trade proceeds [M].
- OEHHA's race/ethnicity analysis of CalEnviroScreen 4.0 [S] — [PDF](https://calenviroscreen.oehha.ca.gov/sites/default/files/media/downloads/calenviroscreen/document/calenviroscreen40raceanalysisf2021.pdf). Race is not an indicator; the analysis is published alongside.
- Cushing, L., Faust, J., August, L. M., Cendak, R., Wieland, W. and Alexeeff, G. "Racial/Ethnic Disparities in Cumulative Environmental Health Impacts in California: Evidence From a Statewide Environmental Justice Screening Tool (CalEnviroScreen 1.1)", *American Journal of Public Health* (2015) [V] — [record](https://doi.org/10.2105/ajph.2015.302643).
- Sadd, J. L., Pastor, M., Morello-Frosch, R., Scoggins, J. and Jesdale, B. "Playing It Safe: Assessing Cumulative Impact and Social Vulnerability through an Environmental Justice Screening Method in the South Coast Air Basin, California", *IJERPH* 8(5) 1441–1459 (2011) [V] — [record](https://doi.org/10.3390/ijerph8051441).

**4.3 The US Justice40 initiative and CEJST.**
- (A) CEJST 1.0 (November 2022) marked a census tract disadvantaged if it met a 90th-percentile threshold on at least one indicator in any of eight categories (climate change, energy, health, housing, legacy pollution, transportation, water and wastewater, workforce development) and was at or above the 65th percentile for low income (households below 200% of the federal poverty line, excluding students in higher education); the workforce category used an education criterion instead of low income; tracts surrounded by disadvantaged tracts qualified at the 50th income percentile; tracts within federally recognised tribal lands were included [S, from search summaries of WRI, the CEQ technical support document mirror and WHEJAC] — [CEJST 1.0 Technical Support Document (mirror)](https://www.datacommons.psu.edu/download/canary/canary-downloads/general-resources/climate-economic-justice-screening-tool/1.0-cejst-technical-support-document.pdf); [WRI](https://www.wri.org/technical-perspectives/ceq-climate-and-economic-justice-screening-tool-cumulative-burdens). Justice40 itself directed that 40% of the overall benefits of certain federal investments flow to disadvantaged communities (EO 14008, 2021) [M].
- CEJST was removed from the federal web in January 2025 (Inside Climate News: the first tool to go, on 21 January) as the Justice40 initiative was reversed; within 48 hours the Public Environmental Data Project restored an unofficial copy; public interest groups sued over removal of CEJST and other tools on 25 April 2025 [S] — [Harvard EELP tracker](https://eelp.law.harvard.edu/tracker/ceqs-climate-economic-justice-screening-tool-removed/); [EDGI](https://envirodatagov.org/trump-removes-access-to-cejst-our-new-coalition-restores-it/); [Inside Climate News](https://insideclimatenews.org/news/04022025/data-scientists-restore-climate-justice-tool-taken-down-by-trump/); [lawsuit report](https://insideclimatenews.org/news/14042025/environmental-groups-sue-trump-administration-to-restore-climate-web-tools/). (A)
- Critiques of CEJST [S]: it omits race, and some overburdened communities of colour are not flagged (Georgetown Environmental Law Review) — [link](https://www.law.georgetown.edu/environmental-law-review/blog/the-biden-administrations-new-tool-to-address-environmental-injustice/); cut-offs are arbitrary, so near-identical neighbouring tracts fall on either side (Greenlining) — [link](https://greenlining.org/?p=849); no measure of cumulative burden (WRI) — [link](https://www.wri.org/technical-perspectives/ceq-climate-and-economic-justice-screening-tool-cumulative-burdens). Counter-evidence that the race-neutral design still tracks race (Grist; Rhodium Group: tracts facing multiple burdens are disproportionately non-white) — [Grist](https://grist.org/equity/white-house-environmental-justice-tool-cejst-update-race/); [Rhodium](https://rhg.com/research/justice40-initiative-mapping-race-and-ethnicity-updated/). (A)
- Mullen, C., Whyte, K. P. and Holifield, R. "Indigenous Peoples and the Justice40 Screening Tool: Lessons from EJSCREEN", *Environmental Justice* 16(5) 360–369 (2023) [V] — [record](https://doi.org/10.1089/env.2022.0045).
- National Academies of Sciences, Engineering, and Medicine, *Constructing Valid Geospatial Tools for Environmental Justice* (2024) [S] — [chapter 5](https://www.nationalacademies.org/read/27317/chapter/8). (M) Recommends structured indicator selection, validation, sensitivity analysis and community engagement for such tools.

**4.4 Area Deprivation Index and social vulnerability indices.**
- Kind, A. J. H. and Buckingham, W. R. "Making Neighborhood-Disadvantage Metrics Accessible — The Neighborhood Atlas", *New England Journal of Medicine* 378(26) 2456–2458 (2018) [V] — [record](https://doi.org/10.1056/nejmp1802313). (A) ADI covers income, education, employment and housing quality, validated at block-group level, ranked nationally or within state — [Neighborhood Atlas](https://www.neighborhoodatlas.medicine.wisc.edu/).
- Cutter, S. L., Boruff, B. J. and Shirley, W. L. "Social Vulnerability to Environmental Hazards", *Social Science Quarterly* 84(2) 242–261 (2003) [V] — [record](https://doi.org/10.1111/1540-6237.8402002).
- Tate, E. "Social vulnerability indices: a comparative assessment using uncertainty and sensitivity analysis", *Natural Hazards* 63(2) 325–347 (2012) [V] — [record](https://doi.org/10.1007/s11069-012-0152-2). (M) Rankings are sensitive to construction choices (indicator set, normalisation, weighting, aggregation), with large rank uncertainty for many units. (Content M.)
- Rufat, S., Tate, E., Emrich, C. T. and Antolini, F. "How Valid Are Social Vulnerability Models?", *Annals of the AAG* 109(4) 1131–1153 (2019) [V] — [record](https://doi.org/10.1080/24694452.2018.1535887). (M) Tested SoVI, the CDC SVI and others against post-Sandy outcomes; weak and inconsistent construct validity. (Content M.) Commentary: Flanagan et al. (2021) [V] — [record](https://doi.org/10.1080/24694452.2020.1857220).
- Spielman, S. E., Tuccillo, J., Folch, D. C., Schweikert, A., Davies, R., Wood, N. and Tate, E. "Evaluating social vulnerability indicators: criteria and their application to the Social Vulnerability Index", *Natural Hazards* 100(1) 417–436 (2020) [V] — [record](https://doi.org/10.1007/s11069-019-03820-z). (M) SoVI fails internal consistency and theoretical consistency tests; small changes flip rankings. (Content M.)

**4.5 General critiques of composite indices and area targeting.**
- Robinson, W. S. "Ecological Correlations and the Behavior of Individuals", *American Sociological Review* 15(3) 351–357 (1950) [V] — [record](https://doi.org/10.2307/2087176). Ecological fallacy.
- Openshaw, S. *The Modifiable Areal Unit Problem*, CATMOG 38 (Geo Books 1984) [M]; Fotheringham, A. S. and Wong, D. W. S. "The Modifiable Areal Unit Problem in Multivariate Statistical Analysis", *Environment and Planning A* 23(7) 1025–1044 (1991) [V] — [record](https://doi.org/10.1068/a231025). (M) Results change with the scale and zoning of areal units, often unpredictably in multivariate models. (Content M.)
- Saisana, M., Saltelli, A. and Tarantola, S. "Uncertainty and Sensitivity Analysis Techniques as Tools for the Quality Assessment of Composite Indicators", *JRSS A* 168(2) (2005) [V] — [record](https://doi.org/10.1111/j.1467-985x.2005.00350.x); Greco, S., Ishizaka, A., Tasiou, M. and Torrisi, G. "On the Methodological Framework of Composite Indices: A Review of the Issues of Weighting, Aggregation, and Robustness", *Social Indicators Research* 141(1) 61–94 (2019; online 2018) [V] — [record](https://doi.org/10.1007/s11205-017-1832-9).
- Talen, E. and Anselin, L. "Assessing Spatial Equity: An Evaluation of Measures of Accessibility to Public Playgrounds", *Environment and Planning A* 30(4) 595–613 (1998) [V] — [record](https://doi.org/10.1068/a300595). (M) Different accessibility measures (container, gravity, minimum distance, travel cost) give different equity conclusions for the same facilities. (Content M.)
- Kontokosta, C. E. and Hong, B. "Bias in smart city governance: How socio-spatial disparities in 311 complaint behavior impact the fairness of data-driven decisions", *Sustainable Cities and Society* 64 102503 (2021) [V] — [record](https://doi.org/10.1016/j.scs.2020.102503); Boxer, K., Hong, B., Kontokosta, C. E. and Neill, D. B. "Estimating reporting bias in 311 complaint data", *Annals of Applied Statistics* 19(2) 1691–1713 (2025) [V] — [record](https://doi.org/10.1214/24-aoas2003). (M) Allocating services by complaint counts favours groups that complain more; under-reporting correlates with disadvantage. (Content M.)

#### Inferences
- A tool should store an index as a declared scheme (indicators, normalisation, weights, aggregation, unit, threshold) and recompute it under alternative schemes, reporting the share of areas whose eligibility changes (rank uncertainty), in the manner of Saisana et al. and Tate.
- Thresholds should be replaced or accompanied by continuous entitlements (e.g. allocation proportional to a smooth function of the score) or by a band within which eligibility is decided by further evidence, to avoid Greenlining's "barely distinguishable neighbours" problem.
- Because "many deprived people live in non-deprived areas" (the IMD's own warning), area targeting should be paired with person-level entitlements wherever services allow it, and the tool should report the coverage of deprived persons, not only of deprived areas.
- The CEJST episode shows that a tool's index must be archived and reproducible outside any one government's web presence.

#### Gaps
- The IMD's exponential transformation constant (23) and the CalEnviroScreen rescaling are from memory; technical reports were located but not read.
- SB 535's exact designation rule and the Justice40 executive orders' texts were not read.
- No published evaluation was found of how much IMD-based funding misses deprived individuals in non-deprived areas in quantitative terms for 2025.

### 5. Distributional Weights Bring Equity into Benefit–Cost Appraisal

#### Takeaway
Benefit–cost analysis (BCA) counts a pound or dollar equally whoever receives it, which favours projects for the rich because willingness to pay rises with income. Distributional (equity) weights, $w_i = (\bar y / y_i)^{\eta}$, correct this. The UK Green Book recommends $\eta = 1.3$; the US 2023 Circular A-4 allowed agencies to use $\eta = 1.4$ but was rescinded in 2025; FEMA adopted income-adjusted weights in 2024 and then had its lower discount rate reverted in April 2025. Weights remain contested on theory (whose social welfare function?), on efficiency (redistribution via projects versus taxes) and on politics, and the evidence base for $\eta$ ranges roughly from 1 to 2.

#### Cited Findings
- Fleurbaey, M. and Abi-Rafeh, R. "The Use of Distributional Weights in Benefit–Cost Analysis: Insights from Welfare Economics", *Review of Environmental Economics and Policy* 10(2) 286–307 (2016) [V] — [record](https://doi.org/10.1093/reep/rew003). (M) Unweighted BCA is a social welfare function only under strong assumptions; weights follow from a social welfare function $W=\sum_i \phi(u_i)$ as $w_i = \phi'(u_i)\,u'(y_i)$ relative to a reference person; the choice of reference and of inequality aversion is normative and should be explicit; weights can be applied to consumption-equivalent changes. (Content M.)
- Adler, M. D. "Benefit–Cost Analysis and Distributional Weights: An Overview" (SSRN 2013/2014 [V] — [record](https://doi.org/10.2139/ssrn.2467673); published *REEP* 10(2) 264–285 (2016) [M]). (M) Weighted BCA as an approximation to a prioritarian or utilitarian SWF; weights can be derived from Atkinson's inequality aversion. (Content M.)
- Hahn, R. W. "Equity in cost-benefit analysis", *Science* 372(6541) 439 (2021) [V] — [record](https://doi.org/10.1126/science.abg9534). (M) Argues for presenting distributional analysis alongside unweighted results and for caution in applying weights. (Content M.)
- Atkinson (1970) [V], section 1.9: inequality aversion $\varepsilon$ ↔ $\eta$.
- **UK Green Book.** HM Treasury's Green Book (2022 edition) Annex A3 recommends distributional weights based on diminishing marginal utility of income with elasticity 1.3, citing Layard et al. (2008) (A) [S, quoted in a DfT technical paper] — [DfT, *Audit of distributional weight analysis*](https://assets.publishing.service.gov.uk/media/67178d6ae319b91ef09e3812/dft-audit-distributional-weight-analysis-review-dft-technical-paper.pdf). Formula (M for exact Annex wording): $w_i = (\tilde y_{\text{median}}/\tilde y_i)^{1.3}$ with $\tilde y$ equivalised household income; equity-weighted NPV $=\sum_i w_i(B_i - C_i)$.
- **Green Book Review 2025 and the 2026 edition.** The review (June 2025) found insufficient emphasis on place-based objectives but "no structural regional bias", introduced place-based business cases, and stated the Green Book "does not endorse the use of arbitrary 'BCR thresholds'"; the revised Green Book (early 2026) treats BCRs as summary indicators, not decision rules, reiterates distributional and spatial analysis without major new guidance, and pilots place-based business cases in Plymouth, Birmingham, Liverpool and Port Talbot; a June 2026 "one year on" update exists (A) [S] — [GOV.UK review findings](https://www.gov.uk/government/publications/green-book-review-2025-findings-and-actions/green-book-review-2025-findings-and-actions); [one year on](https://www.gov.uk/government/publications/green-book-review-2025-one-year-on/green-book-review-2025-one-year-on); [GLA Datastore blog on the 2026 edition](https://data.london.gov.uk/blog/the-green-book-2026-updates-following-the-2025-review) (fetched; it does not say whether Annex A3 weights changed). A critique: place-based appraisal relies on land value uplift, measured by house prices, as a proxy for benefit (A) [S] — [ICE](https://ice.org.uk/news-views-insights/inside-infrastructure/what-does-green-book-review-mean-infrastructure).
- **US Circular A-4 (2023) and its rescission.** The November 2023 revision allowed (did not require) agencies to weight effects by each group's median income relative to the US median raised to $-\eta$ with $\eta = 1.4$ (so a group at half the median gets weight $2^{1.4}\approx 2.64$) (A) [S, from JBCA articles' summaries] — [Viscusi, JBCA](https://www.cambridge.org/core/product/A06E82D46DD2533F6AD0D33795001C9D); [GWU Regulatory Studies comment](https://regulatorystudies.columbian.gwu.edu/node/4526). Executive Order 14192 (January 2025) directed OMB to revoke the 2023 Circular A-4 and reinstate the 2003 version (A) [S] — [Federal Register 2025-02345](https://govinfo.gov/content/pkg/FR-2025-02-06/pdf/2025-02345.pdf); [Harvard EELP](https://eelp.law.harvard.edu/trump-administration-issues-deregulatory-order-undermining-agency-decision-making/).
- Viscusi, W. K. "Why Office of Management and Budget's (OMB) Social Welfare Function Is Not Society's Social Welfare Function", *Journal of Benefit-Cost Analysis* 15(2) 252–275 (2024) [V] — [record](https://doi.org/10.1017/bca.2024.25). (A) Argues the weights reflect an administration's SWF, not society's.
- Acland, D. and Greenberg, D. H. "The Elasticity of Marginal Utility of Income for Distributional Weighting and Social Discounting: A Meta-Analysis", *Journal of Benefit-Cost Analysis* 14(2) 386–405 (2023) [V] — [record](https://doi.org/10.1017/bca.2023.29). (A, from search) Tentatively concludes 1.6, with sensitivity at 1.2 and 2.0 — [PDF](https://core-prod.cambridgecore.org/core/services/aop-cambridge-core/content/view/D94D3809CDAB8437BCCB366D30337A6F/S2194588823000295a.pdf/elasticity_of_marginal_utility_of_income_for_distributional_weighting_and_social_discounting_a_metaanalysis.pdf).
- Equitable Growth's comment supporting weights (A) [S] — [PDF](https://equitablegrowth.org/wp-content/uploads/2023/07/Comment-Letter-Equitable-Growth-Reg-Modernization-A-4.pdf); critique that A-4 ignored redistribution's deadweight loss (A) [S] — [JBCA](https://www.cambridge.org/core/product/DC41CCB6B3BAE5E70F135F7F6BD6F38A/core-reader).
- Lockwood, B. et al. "Socioeconomic distributional impacts of evaluating flood mitigation activities using equity-weighted benefit-cost analysis", *Environmental Research Letters* 19(7) 074024 (2024) [V] — [record](https://doi.org/10.1088/1748-9326/ad4ef8). (Content not read; directly tests equity-weighted BCA on flood mitigation.)
- Fleurbaey, M. and Maniquet, F. *A Theory of Fairness and Social Welfare* (CUP 2011) [V] — [record](https://doi.org/10.1017/cbo9780511851971).

#### Inferences
- A tool should compute unweighted and weighted net benefits side by side for $\eta \in \{0, 1, 1.3, 1.4, 1.6, 2\}$ and show where the ranking of options changes ("switching values" of $\eta$); this makes the normative parameter visible rather than hidden.
- Distributional weights correct the *valuation* of benefits; they do not address who captures benefits through rents (section 11). Weights must be applied to incidence-adjusted benefits, or they will overweight benefits that low-income renters lose to landlords.
- The UK direction (no BCR thresholds, place-based business cases) and the US reversal (2025) show that the institutional standing of weights is unstable; a tool should therefore present weights as a declared scheme with its source and date.

#### Gaps
- The Green Book Annex A3 text and the 2023 Circular A-4 text were not read directly; parameter values are from secondary sources that agree.
- Whether the 2026 Green Book kept $\eta = 1.3$ was not confirmed.

### 6. Disaster Mitigation and Recovery Funding Inherit the Distribution of Property Values

#### Takeaway
Hazard mitigation funding in the US has been allocated by benefit–cost tests whose benefits are avoided property damage, so places with higher property values clear the test more easily; recovery aid has been associated with widening wealth inequality and racial disparities in buyouts. FEMA's 2024 reforms (3.1% discount rate, distributional adjustments, narrative cost-effectiveness for small projects) were partly reversed in April 2025 when OMB reinstated the 7% rate, and the BRIC programme was cancelled in April 2025, ruled unlawfully cancelled in December 2025 and resumed in March 2026. Risk-financing tools in a city must therefore report losses in proportion to income and wealth, not only in currency.

#### Cited Findings
- FEMA, BCA news page [V, fetched] — [FEMA](https://fema.gov/grants/guidance-tools/benefit-cost-analysis/news). (T) "On April 8, the Office of Management and Budget (OMB) revoked the November 2023 update to OMB Circular No. A-94", which had set the discount rate at 3.1%; OMB reinstated its October 1992 version, "returning the discount rate to 7%"; Hazard Mitigation Assistance projects not obligated or awarded as of 8 April 2025 must use 7%; projects under $1 million may show cost effectiveness by narrative (items dated 17 June 2025).
- FEMA had adopted the 3.1% rate in 2024 (Policy 206-23-001) to make cost-effectiveness easier for states, tribes and localities; its 2024 changes increased building replacement values for properties in census tracts with household incomes below the national median (A) [S] — [FEMA policy aid](https://FEMA.Gov/sites/default/files/documents/fema_bca-policy-aid-discount-rate.pdf); [Massachusetts advisory](https://www.mass.gov/doc/fema-advisory-fema-streamlines-cost-effectiveness-in-their-mitigation-grant-programs-may-7-2024/download); FEMA, *BCA Distributional Weights Methodology Report* (November 2024) [S] — [PDF](https://fema.gov/sites/default/files/documents/fema_bca_distributional_weights_methodology_report.pdf).
- Junod, A. N., Martín, C., Marx, R. and Rogin, A. *Equitable Investments in Resilience: A Review of Benefit-Cost Analysis in Federal Flood Mitigation Infrastructure* (Urban Institute 2021) [S] — [Urban Institute](https://www.urban.org/research/publication/equitable-investments-resilience). (A) Prioritising property in these investments often devalues people.
- McGee, K. "A Place Worth Protecting: Rethinking Cost-Benefit Analysis Under FEMA's Flood-Mitigation Programs", *University of Chicago Law Review* 88 1925 (2021) [S] — [PDF](https://lawreview.uchicago.edu/sites/default/files/McGee_A%20Place%20Worth%20Protecting_88UCLR1925.pdf). (A) FEMA's method measures physical risk to property, neglecting long-term social costs to vulnerable communities, and has led to higher-property-value communities receiving a disproportionate share of mitigation infrastructure.
- Headwaters Economics: projects in places with higher-value properties have higher BCRs; Richwood, West Virginia (about 2,100 people, high poverty) failed to obtain funding because low property values kept its BCR too low (A) [S] — [Headwaters](https://headwaterseconomics.org/equity/improving-benefit-cost-analyses/).
- RAND, *The Cost of Cost-Effectiveness: Expanding Equity in FEMA Hazard Mitigation Assistance Grants* (2023) [S] — [PDF](https://www.rand.org/content/dam/rand/pubs/research_reports/RRA2100/RRA2171-1/RAND_RRA2171-1.pdf).
- BRIC: cancelled in April 2025, halting about 2,000 projects; in December 2025 Judge Stearns (D. Mass.) found the cancellation unlawful in a suit by 22 states and DC; FEMA resumed the programme on 25 March 2026 with a focus on "shovel-ready" projects; critics had said BRIC was hard to access for rural and less wealthy communities because of complex applications and cost shares (A) [S] — [PBS/AP, 26 March 2026](https://www.pbs.org/newshour/politics/fema-complies-with-court-order-to-resume-major-disaster-preparedness-grant-program); [WBUR, 12 December 2025](https://www.wbur.org/news/2025/12/12/judge-orders-fema-grant-restored-Bric-massachusetts); [NY AG decision PDF](https://ag.ny.gov/sites/default/files/decisions/washington-v-fema-msj-decision-2025.pdf).
- Howell, J. and Elliott, J. R. "Damages Done: The Longitudinal Impacts of Natural Hazards on Wealth Inequality in the United States", *Social Problems* 66(3) 448–467 (2019; online 2018) [V] — [record](https://doi.org/10.1093/socpro/spy016). (M) Wealth inequality grows after disasters; the more FEMA aid a county receives, the more wealth inequality increases, with white and homeowning households gaining and Black and renting households losing. (Content M.)
- Elliott, J. R., Brown, P. L. and Loughran, K. "Racial Inequities in the Federal Buyout of Flood-Prone Homes: A Nationwide Assessment of Environmental Adaptation", *Socius* 6 (2020) [V] — [record](https://doi.org/10.1177/2378023120905439). Loughran, K., Elliott, J. R., Tickman, ... and Brown, P. L. "Colorblind retreat: how residents make sense of race and relocation from flood-prone areas", *Environmental Sociology* (2026) [V] — [record](https://doi.org/10.1080/23251042.2026.2712614).
- Emrich, C. T., Aksha, S. K. and Zhou, Y. "Assessing distributive inequities in FEMA's Disaster recovery assistance fund allocation", *International Journal of Disaster Risk Reduction* 74 102855 (2022) [V] — [record](https://doi.org/10.1016/j.ijdrr.2022.102855).
- Mach, K. J., Kraan, C. M., Hino, M., Siders, A. R., Johnston, E. M. and Field, C. B. "Managed retreat through voluntary buyouts of flood-prone properties", *Science Advances* 5(10) (2019) [V] — [record](https://doi.org/10.1126/sciadv.aax8995). (M) Buyouts concentrated in wealthier, whiter counties, but within them in poorer, more diverse neighbourhoods. (Content M.) Kraan, C. M., Hino, M., Niemann, J., Siders, A. R. and Mach, K. J. "Promoting equity in retreat through voluntary property buyout programs", *Journal of Environmental Studies and Sciences* 11(3) 481–492 (2021) [V] — [record](https://doi.org/10.1007/s13412-021-00688-z).

#### Inferences
- A BCA of mitigation in a city should report avoided losses also as shares of household income and wealth and should include non-property benefits (displacement avoided, health, schooling continuity), otherwise the allocation will reproduce the property-value gradient.
- Discount-rate and weighting choices are now politically volatile; a tool must keep each run's parameters with its result (the infrastructure's sealed archive is apt).
- The infrastructure's risk-management `financing` and disaster-risk modules can carry these distributional reports with no new mathematics: loss by income decile is a layer of each event's loss.

#### Gaps
- The Urban Institute report and RAND report were not read in full; Lockwood et al. 2024 was not read.
- The outcome of the federal appeal of the BRIC ruling was not found.

### 7. Specific Urban Goods Are Allocated by Local Justice: Housing, Schools, Facilities

#### Takeaway
Elster's *local justice* frames how institutions allocate indivisible goods: by need, desert, time (queues), lottery, status, or efficiency, usually in mixed point systems. Social housing in England is allocated under statutory "reasonable preference" categories, increasingly by choice-based lettings; school places by matching mechanisms whose design (Boston mechanism vs deferred acceptance; priorities and reserves) has measurable distributive effects; public facilities by location models whose objective (median, centre, covering, sufficiency) encodes a theory of justice. Market-design research quantifies the trade-off between targeting and match quality.

#### Cited Findings

**7.1 Local justice.**
- Elster, J. *Local Justice: How Institutions Allocate Scarce Goods and Necessary Burdens* (Russell Sage Foundation 1992) [V via reviews] — [Ethics review](https://doi.org/10.1086/233630); [Journal of Politics review](https://doi.org/10.2307/2131972); Elster, "Justice and the allocation of scarce resources", in *Psychological Perspectives on Justice* (CUP 1993) 259–278 [V] — [record](https://doi.org/10.1017/cbo9780511552069.013). (M) Principles: egalitarian (lottery, rotation, equal division), time-related (queuing, waiting time, seniority), status-related (age, gender, residence, citizenship), defined by other properties (need, merit, efficiency, contribution), power-related; allocations are made by "mixed systems" (point systems) that evolve under pressure from first-order actors (the institutions), second-order actors (politicians) and third-order actors (public opinion); point systems are opaque to the losers. (Content M.)

**7.2 Social housing.**
- Housing Act 1996 s.166A (England) [V, legislation.gov.uk fetched] — [s.166A](https://www.legislation.gov.uk/ukpga/1996/52/section/166A). (T) Schemes must give reasonable preference to: (a) people who are homeless; (b) people owed certain homelessness duties; (c) people "occupying insanitary or overcrowded housing or otherwise living in unsatisfactory housing conditions"; (d) people who need to move on medical or welfare grounds (including disability); (e) people who need to move to a particular locality where failure would cause hardship (s.166A(3)); additional preference may be given within these to urgent need and must be given to certain armed-forces applicants; schemes must state a policy on offering choice or the opportunity to express preferences (s.166A(2)); priority may consider financial resources, behaviour affecting suitability as a tenant, and local connection (s.166A(5)).
- Fitzpatrick, S. and Pawson, H. "Welfare Safety Net or Tenure of Choice? The Dilemma Facing Social Housing Policy in England", *Housing Studies* 22(2) 163–182 (2007) [V] — [record](https://doi.org/10.1080/02673030601132763). (M) Tension between needs-based rationing (residualisation) and choice. (Content M.)
- Brown, T. and Yates, N. "Allocations and Lettings – Taking Customer Choice Forward in England?", *European Journal of Housing Policy* 5(3) 343–357 (2005) [V] — [record](https://doi.org/10.1080/14616710500342242); Pawson, H. and Hulse, K. "Policy Transfer of Choice-based Lettings to Britain and Australia", *International Journal of Housing Policy* 11(2) 113–132 (2011) [V] — [record](https://doi.org/10.1080/14616718.2011.573199). (M) Choice-based lettings (from the Delft model) advertise vacancies; eligible applicants bid; the highest-priority bidder (by band and waiting time) is offered the home.
- Manley, D. and van Ham, M. "Choice-based Letting, Ethnicity and Segregation in England", *Urban Studies* 48(14) 3125–3143 (2011) [V] — [record](https://doi.org/10.1177/0042098010394685). (M) Choice-based letting is associated with ethnic minority applicants being more likely to move into deprived neighbourhoods; choice can reproduce segregation. (Content M.)
- Waldinger, D. "Targeting In-Kind Transfers through Market Design: A Revealed Preference Analysis of Public Housing Allocation", *American Economic Review* 111(8) 2660–2696 (2021) [V] — [record](https://doi.org/10.1257/aer.20190516). (M) Using Cambridge, Massachusetts, shows a trade-off between match quality (giving applicants choice) and targeting (giving units to the neediest); choice-based systems improve match but shift units toward less needy applicants. (Content M.)
- Arnosti, N. and Shi, P. "Design of Lotteries and Wait-Lists for Affordable Housing Allocation", *Management Science* 66(6) 2291–2307 (2020) [V] — [record](https://doi.org/10.1287/mnsc.2019.3311). (M) Lotteries and wait-lists produce equivalent outcomes in a stylised model, but design details (separate lotteries per building, allowing refusals) determine targeting and match quality. (Content M.)

**7.3 School places.**
- Abdulkadiroğlu, A. and Sönmez, T. "School Choice: A Mechanism Design Approach", *American Economic Review* 93(3) 729–747 (2003) [V] — [record](https://doi.org/10.1257/000282803322157061). (M) Proposes student-proposing deferred acceptance (stable, strategy-proof for students) and top trading cycles (efficient, strategy-proof) instead of the Boston mechanism. (Content M.)
- Pathak, P. A. and Sönmez, T. "Leveling the Playing Field: Sincere and Sophisticated Players in the Boston Mechanism", *American Economic Review* 98(4) 1636–1652 (2008) [V] — [record](https://doi.org/10.1257/aer.98.4.1636). (M) The Boston mechanism rewards strategic sophistication, disadvantaging sincere (often less-informed) families; strategy-proofness is thus a fairness property. (Content M.)
- Abdulkadiroğlu, A., Pathak, P. A., Roth, A. E. and Sönmez, T. "Changing the Boston School Choice Mechanism", NBER WP 11965 (2006) [V] — [record](https://doi.org/10.3386/w11965).
- Dur, U., Kominers, S. D., Pathak, P. A. and Sönmez, T. "Reserve Design: Unintended Consequences and the Demise of Boston's Walk Zones", *Journal of Political Economy* 126(6) 2457–2479 (2018) [V] — [record](https://doi.org/10.1086/699974). (M) The order in which reserved (walk-zone) and open seats are processed changes how much the priority matters; Boston's 50% walk-zone reserve had almost no effect because of precedence order; transparency about precedence is a justice requirement. (Content M.)
- Pathak, P. A., Sönmez, T., Ünver, M. U. and Yenmez, M. B. "Fair Allocation of Vaccines, Ventilators and Antiviral Treatments: Leaving No Ethical Value Behind in Healthcare Rationing", *Management Science* 70(6) 3999–4036 (2024) [V] — [record](https://doi.org/10.1287/mnsc.2022.00930). (M) Reserve systems let several ethical principles (need, essential workers, disadvantaged areas, lottery) each govern a share of units, rather than collapsing them into one priority score; the authors show this respects each principle within its category. (Content M.) Directly transferable to urban goods (housing units, school seats, grants).
- Hastings, J. S. and Weinstein, J. M. "Information, School Choice, and Academic Achievement: Evidence from Two Experiments", *QJE* 123(4) 1373–1414 (2008) [V] — [record](https://doi.org/10.1162/qjec.2008.123.4.1373); Calsamiglia, C., Haeringer, G. and Klijn, F. "Constrained School Choice: An Experimental Study", *AER* 100(4) 1860–1874 (2010) [V] — [record](https://doi.org/10.1257/aer.100.4.1860).

**7.4 Public service location.**
- Marsh, M. T. and Schilling, D. A. "Equity measurement in facility location analysis: A review and framework", *European Journal of Operational Research* 74(1) 1–17 (1994) [V] — [record](https://doi.org/10.1016/0377-2217(94)90200-3). (M) Catalogues some twenty equity measures (range, variance, Gini, maximum deviation, envy, etc.) and the choice of reference point and population. (Content M.)
- Karsu, Ö. and Morton, A. "Inequity averse optimization in operational research", *EJOR* 245(2) 343–359 (2015) [V] — [record](https://doi.org/10.1016/j.ejor.2015.02.035). (M) Reviews inequity-averse objectives: lexicographic maximin, ordered weighted averages, Gini-constrained, generalised Lorenz dominance. (Content M.)
- Formulas (M, standard). With demand $w_i$ at $i$, candidate sites $j$, distance $d_{ij}$, open sites $x_j$, assignment $y_{ij}$: p-median (efficiency) $\min\sum_{ij} w_i d_{ij} y_{ij}$; p-centre (Rawlsian minimax) $\min \max_i \sum_j d_{ij}y_{ij}$; maximal covering (sufficiency) $\max \sum_i w_i z_i$ s.t. $z_i \le \sum_{j: d_{ij}\le S} x_j$; ordered weighted average $\min\sum_k \omega_k d_{(k)}$ with $d_{(1)}\ge d_{(2)}\ge\dots$ sorted demand-weighted distances and $\omega$ non-increasing (interpolating between minimax and median).
- Talen, E. "Visualizing Fairness: Equity Maps for Planners", *JAPA* 64(1) 22–38 (1998) [V] — [record](https://doi.org/10.1080/01944369808975954); Talen, E. and Anselin, L. "City cents: Tracking the spatial imprint of urban public expenditures", *Cities* 108 102962 (2021) [V] — [record](https://doi.org/10.1016/j.cities.2020.102962). (Content of the 2021 paper not read; its title matches this survey's object: the spatial distribution of city spending.)
- Luo, W. and Wang, F. "Measures of Spatial Accessibility to Health Care in a GIS Environment: Synthesis and a Case Study in the Chicago Region", *Environment and Planning B* 30(6) 865–884 (2003) [V] — [record](https://doi.org/10.1068/b29120); Luo, W. and Qi, Y. "An enhanced two-step floating catchment area (E2SFCA) method", *Health & Place* 15(4) 1100–1107 (2009) [V] — [record](https://doi.org/10.1016/j.healthplace.2009.06.002); Wang, F. "Measurement, Optimization, and Impact of Health Care Accessibility: A Methodological Review", *Annals of the AAG* 102(5) 1104–1112 (2012) [V] — [record](https://doi.org/10.1080/00045608.2012.657146). Formula (M): $R_j = S_j / \sum_{k: d_{kj}\le d_0} P_k$, $A_i = \sum_{j: d_{ij}\le d_0} R_j$ (the infrastructure's nomos already holds this).

#### Inferences
- Elster's insight that real allocators use *mixed systems* argues for a tool that composes principles explicitly (reserves or categories with their shares, then within-category rules) rather than a single score.
- Reserve systems (Pathak et al.) are a formal, implementable bridge between justice theory and allocation: each principle governs a declared share; the order of processing must be declared because it changes outcomes (Dur et al.).
- Choice raises match quality but can reduce targeting and reproduce segregation (Waldinger; Manley and van Ham); a tool should report both.

#### Gaps
- No primary evaluation of England's choice-based lettings after 2015 was read.

### 8. Participatory Budgeting Allocates by Need-Weighted Formulas and Votes

#### Takeaway
Porto Alegre's participatory budgeting (from 1989) allocated investment among districts by a published points formula combining infrastructure need, population and the district's own ranked priorities, with weights set by the citizens' council; it redistributed toward poorer districts, and Brazilian evidence links PB to higher health and sanitation spending and lower infant mortality. As PB diffused (to some thousands of municipalities worldwide), most adopters kept the vote and dropped the redistributive formula; Porto Alegre itself suspended PB in 2017. Computational social choice now supplies proportional voting rules (the Method of Equal Shares, used in Wieliczka, Świecie and Aarau since 2023) and an open library of PB elections (Pabulib).

#### Cited Findings

**8.1 Porto Alegre's formula.**
- de Sousa Santos, B. "Participatory Budgeting in Porto Alegre: Toward a Redistributive Democracy", *Politics & Society* 26(4) 461–510 (1998) [V; text read] — [record](https://doi.org/10.1177/0032329298026004003); full text via [FES library](https://library.fes.de/libalt/journals/swetsfulltext/6859309.pdf). (T, pp. 474–475) "The general criteria are lack of urban infrastructures and/or services, total population of the region, and priority given by the region to a specific sector or theme. To each criterion is ascribed a weight in a scale from 1 to 3 in direct proportion to the importance attributed to it by the COP. In the 1998 budget, both the lack of services and the priority of the region carry the weight 4 while the size of the population carries the weight 2." (The text's "scale from 1 to 3" is inconsistent with the weights 4 and 2 it reports; the 1997 worked example uses weights 3, 2, 3.) Grades: for need, e.g. paving, a region with up to 20.99% of streets needing paving gets grade 1 and one with 80% or more gets grade 5 (1998); thematic priorities ranked first get grade 5 down to grade 1 for fifth (p. 474). "The grade received by each region in each criterion is then multiplied by the general criterion's weight. The sum of the partial points (grades × weight) gives the total grade of the region in that specific sectorial demand. This total grade determines the percentage of the investment resources that will be allocated to the region in that sector." Worked example, 1997 paving (20 km): Extremo Sul (80.21% need, 20,647 people, top priority) scored 3×4 + 2×1 + 3×4 = 26 points; Centro (0.14% need, 293,193 people, low priority) scored 3×1 + 2×4 + 3×0 = 11; total 262 points; Extremo Sul received 9.9% (1,985 m), Centro 4.2% (840 m). Two earlier criteria, "popular mobilisation in the region" and "importance of region for the organisation of the city", were abandoned as manipulable and as technocratic respectively (p. 477, T).
- Formula (T, from the passage): for sector $s$ and region $r$, $\text{points}_{rs} = \sum_{c\in\{\text{need},\,\text{pop},\,\text{priority}\}} w_c\, g_{crs}$, and region $r$'s share of sector $s$'s investment is $\text{points}_{rs}/\sum_{r'}\text{points}_{r's}$, with weights $w_c$ set annually by the PB Council (COP) and grades $g_{crs}$ from banded indicators. Note the population criterion is graded in bands, not proportional, which caps the advantage of large regions, and the need criterion is monotone in deprivation.
- Marquetti, A., Schonerwald da Silva, C. E. and Campbell, A. "Participatory Economic Democracy in Action: Participatory Budgeting in Porto Alegre, 1989–2004", *Review of Radical Political Economics* 44(1) 62–81 (2012; online 2011) [V] — [record](https://doi.org/10.1177/0486613411418055); Marquetti, "The Redistributive Effects of the Participatory Budgeting in Porto Alegre", in *Widening Democracy* (Brill 2010) 99–114 [V] — [record](https://doi.org/10.1163/ej.9789004177833.i-370.33). (M) Investment per capita was higher in poorer regions over 1989–2004. (Content M.)
- Baiocchi, G. *Militants and Citizens: The Politics of Participatory Democracy in Porto Alegre* (Stanford 2005) [V] — [record](https://doi.org/10.1515/9781503625129).
- Lincoln Institute (Goldsmith and Vainer): in 1999 weights covered population, poverty, shortages such as lack of paving, and citywide priorities (A) [S] — [Lincoln Institute](https://www.lincolninst.edu/pt-br/publications/articles/participatory-budgeting-power-politics-porto-alegre/).
- WRI case study: PB worked well for small neighbourhood infrastructure, less for large projects; political support declined and PB was suspended in Porto Alegre in 2017 (A) [S] — [WRI](https://www.wri.org/research/porto-alegre-participatory-budgeting-and-challenge-sustaining-transformative-change).

**8.2 Evidence on outcomes.**
- Gonçalves, S. "The Effects of Participatory Budgeting on Municipal Expenditures and Infant Mortality in Brazil", *World Development* 53 94–110 (2014) [V] — [record](https://doi.org/10.1016/j.worlddev.2013.01.009). (M) PB municipalities allocated more to sanitation and health and had lower infant mortality, with effects growing with years of PB. (Content M.)
- Touchton, M. and Wampler, B. "Improving Social Well-Being Through New Democratic Institutions", *Comparative Political Studies* 47(10) 1442–1469 (2014) [V] — [record](https://doi.org/10.1177/0010414013512601). (M; A via search) Over 100 PB programmes in Brazil's larger municipalities; PB associated with increased health and sanitation spending, more civil society organisations and lower infant mortality, stronger with longer PB.
- Boulding, C. and Wampler, B. "Voice, Votes, and Resources: Evaluating the Effect of Participatory Democracy on Well-being", *World Development* 38(1) 125–135 (2010) [V] — [record](https://doi.org/10.1016/j.worlddev.2009.05.002). (M) Found PB raised the share of spending on health and education but found little effect on well-being indicators. (Content M.)
- Touchton, M., Wampler, B. and Peixoto, T. "Of democratic governance and revenue: Participatory institutions and tax generation in Brazil", *Governance* 34(4) 1193–1212 (2021) [V] — [record](https://doi.org/10.1111/gove.12552).
- Wampler, B., McNulty, S. and Touchton, M. *Participatory Budgeting in Global Perspective* (OUP 2021) [V] — [record](https://doi.org/10.1093/oso/9780192897756.001.0001); Wampler, B. *Participatory Budgeting in Brazil: Contestation, Cooperation, and Accountability* (Penn State 2007) [S, reviews] — [Governance review](https://doi.org/10.1111/j.1468-0491.2008.01426_6.x).
- Campbell, M., Escobar, O., Fenton, C. and Craig, P. "The impact of participatory budgeting on health and wellbeing: a scoping review of evaluations", *BMC Public Health* 18 (2018) [V] — [record](https://doi.org/10.1186/s12889-018-5735-8); [PMC](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC6029380/). (A) 37 studies from 1,458 references; most evaluations (24) in South America; mostly case studies of political outcomes; quality not appraised.
- Touchton, M., McNulty, S. and Wampler, B. "Participatory budgeting and well-being: governance and sustainability in comparative perspective", *Journal of Public Budgeting, Accounting & Financial Management* (2023 per Crossref) [V] — [record](https://doi.org/10.1108/jpbafm-04-2022-0067). (A) An unstructured review across five regions; evidence of well-being effects in Brazil, Peru and South Korea; large-N analyses outside Brazil are rare for lack of data.
- Johnson, C., Carlson, H. J. and Reynolds, S. "Testing the Participation Hypothesis: Evidence from Participatory Budgeting", *Political Behavior* 45(1) 3–32 (2023; online 2021) [V] — [record](https://doi.org/10.1007/s11109-021-09679-w). (M) PB in New York City increased voter turnout among participants, especially among those with low prior propensity. (Content M.)
- Hagelskamp, C. et al. "Participatory Budgeting: Could It Diminish Health Disparities in the United States?", *Journal of Urban Health* 95(5) 766–771 (2018) [V] — [record](https://doi.org/10.1007/s11524-018-0249-3).

**8.3 Diffusion and dilution.**
- Cabannes, Y. "Participatory budgeting: a significant contribution to participatory democracy", *Environment and Urbanization* 16(1) 27–46 (2004) [V] — [record](https://doi.org/10.1177/095624780401600104); Cabannes, Y. and Lipietz, B. "Revisiting the democratic promise of participatory budgeting in light of competing political, good governance and technocratic logics", *Environment and Urbanization* 30(1) 67–84 (2018) [V] — [record](https://doi.org/10.1177/0956247817746279). (M) PB travels under three logics; only the political logic keeps redistribution central; technocratic and good-governance versions keep the procedure without redistribution. (Content M.)
- Ganuza, E. and Baiocchi, G. "The Power of Ambiguity: How Participatory Budgeting Travels the Globe", *Journal of Deliberative Democracy* 8(2) (2012) [V] — [record](https://doi.org/10.16997/jdd.142). (M) PB travelled as a procedural device stripped of its social justice criteria. (Content M.)
- Sintomer, Y., Herzberg, C. and Röcke, A. "Participatory Budgeting in Europe: Potentials and Challenges", *IJURR* 32(1) 164–178 (2008) [V] — [record](https://doi.org/10.1111/j.1468-2427.2008.00777.x); Sintomer, Herzberg, Röcke and Allegretti, "Transnational Models of Citizen Participation: The Case of Participatory Budgeting", *Journal of Deliberative Democracy* 8(2) (2012) [V] — [record](https://doi.org/10.16997/jdd.141); book (Routledge 2016) [V] — [record](https://doi.org/10.4324/9781315599472).
- Fung, A. and Wright, E. O. "Deepening Democracy: Innovations in Empowered Participatory Governance", *Politics & Society* 29(1) 5–41 (2001) [V] — [record](https://doi.org/10.1177/0032329201029001002); Goldfrank, B. *Deepening Local Democracy in Latin America* (Penn State 2011) [V] — [record](https://doi.org/10.5325/j.ctv14gp2fj).
- Paris: 104 winning projects in the 2025 (11th) edition, announced 8 October 2025; 121 in 2024 (A) [S] — [Paris](https://www.paris.fr/pages/budget-participatif-2025-decouvrez-les-104-projets-laureats-de-la-11e-edition-32684). (M) Paris reserves a share of its PB envelope for working-class neighbourhoods (quartiers populaires) [M].

**8.4 Voting rules and data.**
- Faliszewski, P., Flis, J., Peters, D., Pierczyński, G., Skowron, P., Stolicki, D., Szufa, S. and Talmon, N. "Participatory Budgeting: Data, Tools and Analysis", *Proceedings of IJCAI 2023* 2667–2674 [V] — [record](https://doi.org/10.24963/ijcai.2023/297) (the Pabulib companion paper).
- Pabulib [V, site fetched] — [pabulib.org](http://pabulib.org/); [about](http://pabulib.org/about). (T) A searchable collection of PB instances in the `.pb` format; 2,210 instances listed under default filters on 2026-11-20 (Warsaw and its districts, Amsterdam, Budapest among those visible); since 2025 hosted by the Polish association Nauka dla Demokracji, maintained with the Jagiellonian Center for Quantitative Studies in Political Science; the About page states **no licence or terms of use** for the datasets, only a disclaimer reserving the right to remove personally identifiable voter information.
- Aziz, H. and Shah, N. "Participatory Budgeting: Models and Approaches", in *Pathways Between Social Science and Computational Social Science* (Springer 2021) 215–236 [V] — [record](https://doi.org/10.1007/978-3-030-54936-7_10).
- Peters, D., Pierczyński, G. and Skowron, P. "Proportional Participatory Budgeting with Additive Utilities", *Advances in Neural Information Processing Systems* 34 (NeurIPS 2021) [M] (not found in Crossref; NeurIPS proceedings lack DOIs). (M) The Method of Equal Shares: each voter receives an equal share $B/n$ of the budget; projects are bought in order of the lowest "price per supporter" $\rho$ such that supporters can pay $c_p$ with each paying $\min(\text{their remaining budget}, \rho\cdot u_i(p))$; satisfies proportionality axioms (extended justified representation up to one project) that the standard greedy rule ($\arg\max$ votes subject to budget) violates. (Content M.)
- Adoption: Wieliczka's "Green Million" (April 2023, PLN 1,000,000; under MES 18% of voters got none of their projects funded), Aarau's *Stadtidee* (June 2023, CHF 50,000; MES selected 17 projects against 7 for greedy, of 35) and Świecie (September 2023, PLN 1,070,000) changed their statutes to MES; one Aarau study notes MES may trade off impact and novelty (A) [S] — [equalshares.net results](https://equalshares.net/elections); [arXiv 2505.14349](https://arxiv.org/html/2505.14349v2); [arXiv 2405.05085](https://arxiv.org/html/2405.05085v2).
- Boehmer, N., Faliszewski, P., Janeczko, Ł. and Peters, D. "Evaluation of Project Performance in Participatory Budgeting", *IJCAI 2024* 2678–2686 [V] — [record](https://doi.org/10.24963/ijcai.2024/296).

#### Inferences
- Porto Alegre's points formula is a transparent, contestable allocation rule that a city tool can hold first: banded grades, declared weights, shares proportional to points, by sector. It is simpler and more legible than optimisation and was explicitly revised by citizens (criteria dropped for manipulability).
- Modern PB often loses the need term. A tool can restore it by combining a need-weighted district envelope (Porto Alegre's step) with a proportional vote within each envelope (MES), so that both need and voice govern declared shares.
- The PB evidence of welfare effects is strongest in urban Brazil and weak elsewhere; a tool must not promise outcome effects from PB.

#### Gaps
- Peters et al. 2021 could not be verified in Crossref; the MES definition above is from memory.
- No primary statistic on the number of PB processes worldwide in 2025–2026 was found.

### 9. Gender, Equity and Capital Budgeting Score Spending for Distributive Effect

#### Takeaway
Gender budgeting is now practised by 23 of 38 OECD countries (2022), yet its information is rarely used in decisions; US cities such as Oakland have made equity a formal, heavily weighted criterion in capital programme scoring. Both are scoring overlays on existing budgets rather than allocation formulas, and both depend on disaggregated data that cities often lack.

#### Cited Findings
- OECD, *Gender Budgeting in OECD Countries 2023* [V record] — [record](https://doi.org/10.1787/647d546b-en); content (A) [S, from OECD pages via search] — [OECD](https://www.oecd.org/en/publications/gender-budgeting-in-oecd-countries-2023_647d546b-en). 2022 survey of all 38 members: 23 practised gender budgeting (12 in 2016); legal basis in 14 of 23; gender information in annual budget documents in 78%; only 9% said it was always or often used in decision-making, 59% sometimes, 32% rarely; main challenges lack of resources (70%), of knowledge (57%), of impact (52%); practices at subnational level not captured unless a central practice exists.
- Stotsky, J. G. "Gender Budgeting: Fiscal Context and Current Outcomes", IMF WP 16/149 (2016) [V] — [record](https://doi.org/10.5089/9781475575460.001); Stotsky, "Gender Budgeting", IMF WP 06/232 (2006) [V] — [record](https://doi.org/10.5089/9781451864922.001); OECD, *Gender budgeting in Brazil* (2024) [V] — [record](https://doi.org/10.1787/fa31b226-en).
- Budlender, D. *A Global Assessment of Gender Responsive Budget Initiatives* (Commonwealth Secretariat 2002) [V] — [record](https://doi.org/10.14217/9781848597648-6-en); Elson, D. *The Diane Elson Reader* (Agenda 2025) [V] — [record](https://doi.org/10.2307/jj.36233888).
- Oakland's Capital Improvement Program: a prioritisation process adopted by Council in 2018 with nine factors; equity together with health and safety are the highest-scoring factors; "underserved populations" defined by reference to the Oakland Equity Indicators report and MTC equity guidance; reported as the first US government to adopt racial equity as a formal scoring criterion for all capital projects (A) [S] — [Oakland CIP](https://oaklandca.gov/news/capital-improvement-plan-oaklands-new-way-to-prioritize-citys-improvement); [CIP FAQ](https://cao-94612.s3.amazonaws.com/documents/CIP-FAQ-8.30.23.pdf). The per-factor point values were not found.

#### Inferences
- Equity scoring in capital programmes is a weighted multi-criteria rule; a tool should show the score decomposition per project and the ranking's sensitivity to the equity weight, as with indices (section 4).
- Gender and equity budgeting have a utilisation gap (9% "always or often"); the tool's value lies in putting distributive effects in the same view as the decision, not in a separate report.

#### Gaps
- Oakland's resolution and point values were not read; no evaluation of whether equity scoring changed the spatial distribution of Oakland's capital spending was found.

### 10. Procedure Determines the Legitimacy of Allocation Formulas and Algorithms

#### Takeaway
People judge allocations by how they are made as well as by outcomes: voice, neutrality, transparency, explanation and the possibility of appeal raise acceptance even of unfavourable decisions, although outcome favourability also matters. For algorithmic allocation, explanations of reasons raise perceived trustworthiness more than code disclosure; perceptions vary by context. Failures of automated public allocation (e.g. in welfare and housing triage) show the cost of opacity and of training on biased records.

#### Cited Findings
- Arnstein, S. R. "A Ladder of Citizen Participation", *Journal of the American Institute of Planners* 35(4) 216–224 (1969) [M for the original; reprint records V] — [reprint](https://doi.org/10.4324/9780429261732-36).
- Fung, A. "Varieties of Participation in Complex Governance", *Public Administration Review* 66(s1) 66–75 (2006) [V] — [record](https://doi.org/10.1111/j.1540-6210.2006.00667.x). (M) The "democracy cube": who participates, how they communicate and decide, and how much authority they have. (Content M.)
- Lind, E. A. and Tyler, T. R. *The Social Psychology of Procedural Justice* (Plenum 1988) [V chapters] — [record](https://doi.org/10.1007/978-1-4899-2115-4_5); Tyler, T. R. and Lind, E. A. "A Relational Model of Authority in Groups", *Advances in Experimental Social Psychology* 25 115–191 (1992) [V] — [record](https://doi.org/10.1016/s0065-2601(08)60283-x); Tyler, T. R. "Procedural Justice, Legitimacy, and the Effective Rule of Law", *Crime and Justice* 30 283–357 (2003) [V] — [record](https://doi.org/10.1086/652233). (M) Voice, neutrality, respect and trustworthiness drive perceived fairness and legitimacy, independently of outcomes. (Content M.)
- Esaiasson, P., Gilljam, M. and Persson, M. "Which decision-making arrangements generate the strongest legitimacy beliefs? Evidence from a randomised field experiment", *European Journal of Political Research* 51(6) 785–808 (2012) [V] — [record](https://doi.org/10.1111/j.1475-6765.2011.02052.x). (M, uncertain) In a randomised field experiment on a local decision, arrangements with personal involvement generated stronger legitimacy beliefs than representative ones; outcome favourability also mattered. (Content M; the relative size of the two effects was not checked.)
- Grimmelikhuijsen, S. "Explaining Why the Computer Says No: Algorithmic Transparency Affects the Perceived Trustworthiness of Automated Decision-Making", *Public Administration Review* 83(2) 241–262 (2023; online 2022) [V] — [record](https://doi.org/10.1111/puar.13483); replication by Fang, Zhou and Chen, *Public Administration* 103(4) 1232–1252 (2025) [V] — [record](https://doi.org/10.1111/padm.70015). (M) Explainability (reasons for a decision) raised perceived trustworthiness; accessibility of the algorithm's code alone did little. (Content M.)
- Binns, R., Van Kleek, M., Veale, M., Lyngs, U., Zhao, J. and Shadbolt, N. "'It's Reducing a Human Being to a Percentage': Perceptions of Justice in Algorithmic Decisions", *CHI 2018* 1–14 [V] — [record](https://doi.org/10.1145/3173574.3173951).
- Lee, M. K. "Understanding perception of algorithmic decisions: Fairness, trust, and emotion in response to algorithmic management", *Big Data & Society* 5(1) (2018) [V] — [record](https://doi.org/10.1177/2053951718756684). (M) For tasks requiring human skills, algorithmic decisions were perceived as less fair. (Content M.)
- Starke, C., Baleis, J., Keller, B. and Marcinkowski, F. "Fairness perceptions of algorithmic decision-making: A systematic review of the empirical literature", *Big Data & Society* 9(2) (2022) [V] — [record](https://doi.org/10.1177/20539517221115189). (M) Perceptions depend on design, context and person; results are fragmented. (Content M.)
- Eubanks, V. *Automating Inequality* (St Martin's 2018) [V via reviews] — [review](https://doi.org/10.24908/ss.v16i3.12612). (M) Includes Los Angeles' coordinated entry system for homeless housing (VI-SPDAT scoring) as a case of opaque triage of scarce housing. (Content M.)
- (M) The Dutch childcare benefits scandal (*toeslagenaffaire*), in which a risk-scoring system flagged families, disproportionately with dual nationality, for fraud, led to the Rutte III government's resignation in January 2021 [M].

#### Inferences
- A justice-aware tool should make every allocation explainable as reasons ("your district received X because its need grade was 4 and its priority grade 5 under weights 4/2/4") and keep an appeal path: recompute under the claimant's stated correction and show the change.
- Participation should set weights and thresholds (as Porto Alegre's council did), not only rank projects; the tool should record who set each parameter and when.

#### Gaps
- No experimental evidence specific to *spatial* budget allocation formulas' legitimacy was found; the evidence is from adjacent domains (courts, policing, algorithmic decisions, local referendums).

### 11. Urban Economics of Scarcity: Tax Capacity, Sorting, Capitalisation and Displacement

#### Takeaway
Within a metropolis, Tiebout sorting lets richer households cluster in jurisdictions with high tax capacity and exclusionary zoning (Hamilton), so local finance compounds inequality; public goods are capitalised into land values (Oates; Black), so the beneficiaries of a new park, school or cleanup include landowners, and renters may lose through higher rents; green and transit investments have been associated with gentrification and displacement, though the evidence is mixed and depends on scale and type. Affordable housing in poor neighbourhoods can raise nearby values and lower crime. A tool must estimate who gains through prices, not only through services.

#### Cited Findings
- Tiebout, C. M. "A Pure Theory of Local Expenditures", *Journal of Political Economy* 64(5) 416–424 (1956) [V] — [record](https://doi.org/10.1086/257839).
- Oates, W. E. "The Effects of Property Taxes and Local Public Spending on Property Values: An Empirical Study of Tax Capitalization and the Tiebout Hypothesis", *JPE* 77(6) 957–971 (1969) [V] — [record](https://doi.org/10.1086/259584).
- Hamilton, B. W. "Zoning and Property Taxation in a System of Local Governments", *Urban Studies* 12(2) 205–211 (1975) [V] — [record](https://doi.org/10.1080/00420987520080301). (M) Fiscal zoning makes the property tax a benefit tax and permits homogeneous jurisdictions, i.e. exclusion. (Content M.)
- Fischel, W. A. *The Homevoter Hypothesis* (Harvard 2001; 2009 record) [V] — [record](https://doi.org/10.2307/j.ctv1p6hp64). (M) Homeowners vote to protect capitalised home values, favouring restrictive land use. (Content M.)
- Epple, D. and Platt, G. J. "Equilibrium and Local Redistribution in an Urban Economy when Households Differ in both Preferences and Incomes", *Journal of Urban Economics* 43(1) 23–51 (1998) [V] — [record](https://doi.org/10.1006/juec.1996.2030).
- Ross, S. and Yinger, J. "Sorting and voting: A review of the literature on urban public finance", *Handbook of Regional and Urban Economics* vol. 3 (1999) 2001–2060 [V] — [record](https://doi.org/10.1016/s1574-0080(99)80016-9); Yinger, J. "Capitalization and Sorting: a Revision", *Public Finance Quarterly* 23(2) 217–225 (1995) [V] — [record](https://doi.org/10.1177/109114219502300205).
- Black, S. E. "Do Better Schools Matter? Parental Valuation of Elementary Education", *QJE* 114(2) 577–599 (1999) [V] — [record](https://doi.org/10.1162/003355399556070). (M) Boundary discontinuity: a 5% rise in test scores raised house prices about 2.5%. (Content M.)
- Bayer, P., Ferreira, F. and McMillan, R. "A Unified Framework for Measuring Preferences for Schools and Neighborhoods", *JPE* 115(4) 588–638 (2007) [V] — [record](https://doi.org/10.1086/522381).
- Banzhaf, H. S. and Walsh, R. P. "Do People Vote with Their Feet? An Empirical Test of Tiebout's Mechanism", *AER* 98(3) 843–863 (2008) [V] — [record](https://doi.org/10.1257/aer.98.3.843). (M) Population density and income composition respond to changes in local air pollution (Toxics Release Inventory); evidence of environmental gentrification. (Content M.)
- Sieg, H., Smith, V. K., Banzhaf, H. S. and Walsh, R. "Estimating the General Equilibrium Benefits of Large Changes in Spatially Delineated Public Goods", *International Economic Review* 45(4) 1047–1077 (2004) [V] — [record](https://doi.org/10.1111/j.0020-6598.2004.00297.x). (M) Large air-quality improvements in Los Angeles: general-equilibrium welfare (with re-sorting and price changes) differs substantially from partial-equilibrium hedonic estimates, and some renters lose. (Content M.)
- Gamper-Rabindran, S. and Timmins, C. "Does cleanup of hazardous waste sites raise housing values? Evidence of spatially localized benefits", *JEEM* 65(3) 345–360 (2013) [V] — [record](https://doi.org/10.1016/j.jeem.2012.12.001). (M) Superfund cleanups raised values most at the lower end of the house price distribution, nearby. (Content M.)
- Diamond, R. and McQuade, T. "Who Wants Affordable Housing in Their Backyard? An Equilibrium Analysis of Low-Income Property Development", *JPE* 127(3) 1063–1117 (2019) [V] — [record](https://doi.org/10.1086/701354). (M) LIHTC developments in low-income neighbourhoods raised nearby house prices (about 6.5%) and lowered crime; in higher-income, low-minority areas they lowered prices (about 2.5%). (Content M.)
- Freedman, M. and Owens, E. G. "Low-income housing development and crime", *JUE* 70(2–3) 115–131 (2011) [V] — [record](https://doi.org/10.1016/j.jue.2011.04.001); Freedman, M. and McGavock, T. "Low-Income Housing Development, Poverty Concentration, and Neighborhood Inequality", *JPAM* 34(4) 805–834 (2015) [V] — [record](https://doi.org/10.1002/pam.21856).
- Asquith, B. J., Mast, E. and Reed, D. "Local Effects of Large New Apartment Buildings in Low-Income Areas", *Review of Economics and Statistics* 105(2) 359–375 (2023) [V] — [record](https://doi.org/10.1162/rest_a_01055). (M) New market-rate buildings in low-income areas lowered nearby rents by 5–7% and increased in-migration from low-income areas; supply effects dominated amenity effects. (Content M.)
- Brueckner, J. K. and Rosenthal, S. S. "Gentrification and Neighborhood Housing Cycles: Will America's Future Downtowns Be Rich?", *REStat* 91(4) 725–743 (2009) [V] — [record](https://doi.org/10.1162/rest.91.4.725).
- Zuk, M., Bierbaum, A. H., Chapple, K., Gorska, K. and Loukaitou-Sideris, A. "Gentrification, Displacement, and the Role of Public Investment", *Journal of Planning Literature* 33(1) 31–44 (2018; online 2017) [V] — [record](https://doi.org/10.1177/0885412217716439). (M) Reviews evidence that public investments (transit, parks, housing) can catalyse gentrification; displacement evidence is mixed and methodologically hard. (Content M.)
- Anguelovski, I., Connolly, J. J. T., Pearsall, H., Shokry, G., Checker, M., Maantay, J. et al. "Why green 'climate gentrification' threatens poor and vulnerable populations", *PNAS* 116(52) 26139–26143 (2019) [V] — [record](https://doi.org/10.1073/pnas.1920490117).
- Immergluck, D. and Balan, T. "Sustainable for whom? Green urban development, environmental gentrification, and the Atlanta Beltline", *Urban Geography* 39(4) 546–562 (2018; online 2017) [V] — [record](https://doi.org/10.1080/02723638.2017.1360041). (M) Homes near the Beltline appreciated faster after its announcement, before construction. (Content M.)
- Gould, K. A. and Lewis, T. L. *Green Gentrification: Urban Sustainability and the Struggle for Environmental Justice* (Routledge 2017) [V via reviews] — [JUA review](https://doi.org/10.1080/07352166.2017.1416219).
- Rigolon, A. and Németh, J. "Green gentrification or 'just green enough': Do park location, size and function affect whether a place gentrifies or not?", *Urban Studies* 57(2) 402–420 (2020; online 2019) [V] — [record](https://doi.org/10.1177/0042098019849380); Wolch, J. R., Byrne, J. and Newell, J. P. "Urban green space, public health, and environmental justice: The challenge of making cities 'just green enough'", *Landscape and Urban Planning* 125 234–244 (2014) [V] — [record](https://doi.org/10.1016/j.landurbplan.2014.01.017). (M) Large, regional, trail-type parks near downtown are more associated with gentrification than small neighbourhood parks; "just green enough" strategies aim at improvements without triggering displacement.
- Kline and Moretti (2014) and Busso et al. (2013), section 3: incidence of place-based spending depends on housing supply elasticity and mobility.
- Formula (M, standard Rosen–Roback spatial equilibrium for incidence): for a household of type $k$ in place $j$, $V_{kj} = v(w_{kj}, r_j; A_j)$, with mobility equalising $V$ across places up to idiosyncratic tastes (logit with dispersion $\sigma$); an amenity or service increment $\Delta A_j$ changes rents by $\Delta r_j \approx \frac{\partial V/\partial A}{\partial V/\partial r}\cdot \frac{\varepsilon_{\text{mig}}}{\varepsilon_{\text{mig}}+\varepsilon_{\text{supply}}}\,\Delta A_j$ (so with inelastic supply and elastic migration the gain is capitalised); renters' net gain $=$ willingness to pay $\times \Delta A_j - h\,\Delta r_j$, owners' gain $= H_j\,\Delta r_j$. The infrastructure's urban-science `housing` module (logit choice, Berry inversion, rents clearing) already computes this kind of equilibrium.

#### Inferences
- For each candidate allocation, a tool should estimate capitalisation (rent and price changes by tenure), re-sorting (who moves in and out) and displacement risk, using hedonic and equilibrium-sorting modules, and report gains to renters, owners, landlords and in-migrants separately.
- The direction of the price effect depends on what is built: amenity-rich investments raise rents; new housing supply can lower them (Asquith et al.). A tool must not assume all investment gentrifies.
- Tiebout sorting means intra-metropolitan equalisation (section 2) is the counterweight to fiscal exclusion; the same tool can show each jurisdiction's tax capacity per capita against its needs.

#### Gaps
- No study was found that quantifies capitalisation of a whole municipal capital programme by tenure, as opposed to single projects.

### 12. Open Records Permit Honest Demonstrations under Stated Licences

#### Takeaway
There are enough open records to demonstrate a justice-aware allocation tool honestly, but their terms differ sharply: GOV.UK data (IoD 2025, local government finance settlements) are under the Open Government Licence; Eurostat data are reusable with attribution under the Commission's 2011 decision; the CEJST survives only as unofficial archives of a US government work; Pabulib states no licence; the Area Deprivation Index forbids redistribution and commercial use; Paris's PB data appear under conflicting licences across portals.

#### Cited Findings
- **England, Indices of Deprivation 2025** (MHCLG): "All content is available under the Open Government Licence v3.0, except where otherwise stated" [V, page fetched] — [GOV.UK](https://www.gov.uk/government/statistics/english-indices-of-deprivation-2025/english-indices-of-deprivation-2025-statistical-release). LSOA-level ranks, scores and domain data; 33,755 areas.
- **England, Local Government Finance Settlement 2026–27 to 2028–29** (MHCLG): allocations and needs calculators published on GOV.UK [S for content; OGL presumed as GOV.UK default, not checked on the dataset pages] — [final settlement collection](https://www.gov.uk/government/collections/final-local-government-finance-settlement-england-2026-2027-to-2028-2029); the FFR response annexed a provisional needs calculator (§1.1.9, T).
- **Pabulib** (Nauka dla Demokracji; Jagiellonian Center): 2,210 PB instances in `.pb` format, including vote-level data for Warsaw and other cities; **no licence stated** on its About page; it reserves the right to remove personally identifiable voter information [V, fetched] — [About](http://pabulib.org/about). Reuse terms are unclear; the underlying municipal results are public records in Poland, but that was not verified.
- **equalshares.net**: per-city MES results (Wieliczka, Aarau, Świecie) [S] — [results](https://equalshares.net/elections). Licence not checked.
- **CEJST (US CEQ, 1.0, 2022)**: a US federal government work (public domain in the US, M); removed from federal sites in January 2025; preserved and rehosted by the Public Environmental Data Project; also mirrored by universities (e.g. UCSB) [S] — [EDGI](https://envirodatagov.org/trump-removes-access-to-cejst-our-new-coalition-restores-it/); [UCSB mirror](https://apps.bren.ucsb.edu/justice40/en/about); [technical support document mirror](https://www.datacommons.psu.edu/download/canary/canary-downloads/general-resources/climate-economic-justice-screening-tool/1.0-cejst-technical-support-document.pdf). Unofficial copies may not stay current.
- **CalEnviroScreen 4.0** (OEHHA): tract-level scores and indicators; listed on data.gov [S] — [catalog](https://catalog.data.gov/dataset/calenviroscreen-4-0-95325). Licence not verified on OEHHA's page.
- **Area Deprivation Index** (University of Wisconsin–Madison, Neighborhood Atlas): free with login; licence for "internal non-profit educational, research, and public health purposes"; it "does not include the right to sell, market, or otherwise commercially exploit, republish, or redistribute the Data"; non-transferable, non-sublicensable; citation required [V, terms fetched] — [terms](https://www.neighborhoodatlas.medicine.wisc.edu/terms-of-use). **Not suitable for a redistributed public demonstration.**
- **Eurostat regional data** (NUTS 2/3 GDP, poverty, employment, population): reuse authorised for commercial and non-commercial purposes if Eurostat is credited, with exceptions for data from other sources and some non-EU/EFTA countries; adaptations must be stated; legal basis Commission Decision 2011/833/EU; editorial content under CC BY 4.0 [S, from Eurostat help and Statistics Explained via search] — [Eurostat copyright notice](https://ec.europa.eu/eurostat/help/copyright-notice); [Statistics Explained](https://ec.europa.eu/eurostat/statistics-explained/index.php/Copyright/licence_policy).
- **Paris participatory budget** (winning projects and their delivery): published on opendata.paris.fr and harvested to data.gouv.fr; one listing shows Licence Ouverte / Open Licence, an older harvested dataset shows ODbL; the current licence must be checked per dataset [S] — [data.gouv.fr: projets lauréats](https://www.data.gouv.fr/datasets/budget-participatif-les-projets-laureats); [operations dataset](https://data.gouv.fr/en/datasets/budget-participatif-les-operations-des-projets-laureats-jusqua-leurs-realisations).
- **Australia's GST relativities** (CGC): annual relativities and assessment data on cgc.gov.au [S] — [GST relativities 2025–26](https://www.cgc.gov.au/sites/default/files/2025-03/GST%20Sharing%20Relativities%202025-26_Final.pdf). Licence not checked.
- **Canada's equalization** (Finance Canada): program parameters and payments [S] — [Finance Canada](https://www.canada.ca/en/department-finance/programs/federal-transfers/equalization.html). Licence not checked (M: Open Government Licence – Canada applies to much federal data).
- **Germany's Finanzausgleich** (BMF): annual results in the Monatsbericht [S] — [BMF April 2025](https://www.bundesfinanzministerium.de/Monatsberichte/Ausgabe/2025/04/Inhalte/Kapitel-3-Analysen/3-1-bundesstaatlicher-finanzausgleich-2024.html). Licence not checked.
- **US FEMA**: BCA toolkit and HMA project data (OpenFEMA) [M]; BCA policy pages [V] — [FEMA BCA news](https://fema.gov/grants/guidance-tools/benefit-cost-analysis/news).
- **Melbourne**: the infrastructure already holds CLUE and the city's open data; whether the City of Melbourne publishes its budget or capital works programme as machine-readable open data was not verified this session.

#### Inferences
- The most honest first demonstrations are (i) England: allocate a hypothetical ward-level grant across a borough's LSOAs using IoD 2025 domains under alternative weights, against the FFR's actual needs-minus-resources structure (OGL); (ii) PB: replay Warsaw or Amsterdam elections from Pabulib under greedy, MES and a Porto-Alegre-style need-weighted envelope (subject to Pabulib's unclear terms, so keep raw files in the cache and publish only derived summaries with attribution); (iii) EU regions: Eurostat NUTS data to illustrate threshold effects of the 75% cohesion rule.
- The ADI must not be redistributed; the CEJST should be cited as archived, with its date.

#### Gaps
- Licences for CalEnviroScreen, CGC data, Finance Canada and BMF data were not checked on the providers' pages.
- No machine-readable municipal capital budget with geocoded projects and open licence was verified this session (NYC, Paris and Melbourne are candidates).

### 13. What This Means for the Design

#### Takeaway
A city-scale tool for just allocation should hold allocation *rules* (formulas and procedures with declared parameters), not one optimiser; it should report incidence (who gains and who pays, including through rents and displacement) alongside the allocation; it should make every number traceable to a declared principle and a dated source; and it must never claim that its output is "the just allocation". The infrastructure already has most of the machinery (nomos accessibility and appraisal, tyche decision analysis and uncertainty, peira's designs, urban-science housing equilibria, policy and needs domains, kybernesis's dual verdicts and sealed records); the new content is the catalogue of rules, the incidence report and the procedural record.

#### Cited Findings
(This section draws on the findings above; sources are those cited in sections 1–12.)
- Rules seen in practice that are formula-explicit and contestable: equalisation's needs-minus-capacity with a rate and floors (section 2); Porto Alegre's banded points (section 8, Santos 1998, T); reserve systems with declared precedence (section 7, Pathak et al. 2024; Dur et al. 2018); sufficiency thresholds and FGT-type gap measures (section 1, Martens et al. 2022); facility objectives median/centre/covering/OWA (section 7); equity-weighted BCA (section 5); index thresholds with sensitivity (section 4).
- Failure modes documented: guarantees that undo equalisation (PC 2026, section 2); thresholds that exclude near-identical neighbours (Greenlining, section 4); property-value-driven BCA (Junod et al.; McGee; Headwaters, section 6); diffusion of PB without its redistributive criteria (Ganuza and Baiocchi; Cabannes and Lipietz, section 8); choice reproducing segregation (Manley and van Ham, section 7); capitalisation and displacement (sections 3, 11); tools withdrawn by changes of government (CEJST, Circular A-4, FEMA's discount rate; sections 4–6).

#### Inferences

**13.1 Allocation rules to hold first (in order).**
1. *Needs-minus-capacity equalisation among areas* (wards, districts, LSOAs): $G_i = \lambda P_i[(e_i-\bar e) + \sum_k t_k(\bar b_k - b_{ik})]$ with declared need indicators and unit costs, declared resources, an equalisation rate $\lambda$, and a separate, itemised envelope (floors, caps, phasing, guarantees) whose effect on each area is reported. Ship England's FFR structure (needs formulas × ACA − notional council tax × taxbase, phased in thirds, 100%/95% floors) as the worked example, with its consultation critiques attached.
2. *Banded points (Porto Alegre)*: per sector, grades from banded indicators (need, population, stated priority), weights set by a named body, shares proportional to points; plus the history of criteria dropped and why.
3. *Reserve (category) systems*: each principle (need, waiting time, local connection, lottery, essential workers) governs a declared share; within-category rule; declared processing order, with the effect of reordering shown. Housing lettings and school seats use this.
4. *Sufficiency first, then distribution*: compute access (2SFCA, cumulative opportunities) and an FGT-type insufficiency measure against a declared threshold; allocate first to close insufficiency (maximal covering / minimise $\mathrm{ASI}_2$), then distribute the remainder by a second rule; report the share of people (not areas) below threshold.
5. *Equity-weighted appraisal*: for project portfolios, unweighted and weighted NPV for $\eta\in\{0,1,1.3,1.4,1.6,2\}$, with switching values; no BCR thresholds as pass/fail (Green Book 2026).
6. *Proportional voting within need-weighted envelopes*: Porto-Alegre-style envelopes by district, then MES within each envelope for resident proposals.
7. *Facility siting* with median, centre, covering and OWA objectives side by side, showing the Pareto set between efficiency and equity.

**13.2 What every run must report.**
- *Distribution*: allocation per capita by area and by population group (income decile, tenure, age, disability, gender, ethnicity where lawful and available), with Lorenz curves, Gini/Theil/Atkinson at several $\varepsilon$, shares to the worst-off decile, and the people below each declared threshold before and after.
- *Who pays*: the funding source (local taxes by decile, reallocation from other areas or services, borrowing) and the opportunity cost (what the next-best use would have bought), so that "gains" are net.
- *Incidence through prices*: expected rent and price changes by tenure from capitalisation (hedonic and equilibrium-sorting estimates with ranges), gains to owners versus renters, displacement risk indicators, and in-migration; the share of the allocation's value expected to accrue to landowners.
- *Sensitivity*: how many areas change eligibility or rank under alternative indices, weights, thresholds, units (MAUP: LSOA vs ward vs grid), $\eta$ and discount rates; rank-uncertainty intervals.
- *Coverage of persons versus areas*: the share of deprived or below-threshold *people* reached by area targeting (the IMD's own warning).
- *Procedure*: who set each parameter, when, under what mandate; which principles were considered and rejected; the appeal path and the result of recomputing under a claimant's correction; every run sealed with its inputs and parameters (the infrastructure's archive and kybernesis record).
- *Envelope effects*: the part of each area's allocation due to floors, caps, guarantees and phasing, separately from the formula.

**13.3 Open records for honest demonstrations.**
- England: IoD 2025 (OGL v3.0) and the 2026–29 settlement's needs calculators (GOV.UK; OGL to be confirmed on the dataset pages).
- Pabulib PB elections (vote-level; licence unstated — keep raw data in the cache, cite the IJCAI 2023 paper, publish only derived results, and ask the maintainers for terms before redistribution).
- Eurostat NUTS regional data (reuse with attribution; Decision 2011/833/EU).
- CEJST 1.0 as archived by the Public Environmental Data Project (US government work; cite as archived and dated; not current).
- Paris participatory budget results (licence to be confirmed per dataset: Licence Ouverte or ODbL).
- Not for public redistribution: the Area Deprivation Index (non-commercial, no redistribution).
- Melbourne (the infrastructure's habitual case) can be used for access and needs layers already held; whether the city's budget is open data must be checked first. No Japanese city is proposed.

**13.4 What a tool must never claim.**
- That an allocation is *the* just allocation: it is the implication of declared principles, weights and data; the theories (Young, Purcell, Fraser) dispute that justice is a distribution at all.
- That an index score measures a person's need: area indices are relative, not comparable over time, and miss deprived people in non-deprived areas (IMD 2025's own statements).
- That benefits accrue to the residents of a place: without an incidence estimate, place-directed money may be capitalised into land.
- That a threshold is natural: thresholds are choices with discontinuities; show the margin.
- That BCA is neutral: unweighted BCA embeds the income distribution and, for hazards, the property-value distribution.
- That participation guarantees justice or welfare effects: PB's evidence of outcome effects is strongest in urban Brazil and thin elsewhere; diffused PB often lacks redistributive criteria.
- That a model's (or a language model's) proposal is a finding: in keeping with the infrastructure's practice, proposals are marked as such and never alter a computed verdict.
- That its parameters are institutional standards when they are not: every $\eta$, discount rate, threshold and weight is stated with its source and date, because these change with governments (Circular A-4 2023 rescinded; FEMA 3.1% reverted; CEJST withdrawn; Australia's 2018 rules under review).

#### Gaps
- No single source evaluates a city-scale justice-aware allocation tool end to end; the design above is an inference from the practices and critiques surveyed.
- The weighting of procedure against distribution (e.g. accepting a less equal allocation reached with more voice) has no agreed metric; the tool can only report both.

## Part IV. Generative Justice, Circular and Evolutionary Economics

*Remark.* Part IV's scope and conventions. Scope: generative justice; circular, regenerative and doughnut economics with theories of human need, degrowth and community wealth building; commons and polycentric governance; evolutionary and complexity economics and the evolutionary game theory of fairness; money and circulation; and how these become computable. The bibliographic markers are those of §0.3; records were matched against Crossref's API (`api.crossref.org/works?query.bibliographic=…`) or the publisher's or author's own page. Statements of content say where they come from: "text read" (the full text was read this session), "abstract" (an abstract or the publisher's or press summary was read this session), "search summary" (a search engine's summary of the source, not the source itself) or "memory". Formulas taken from memory are flagged [formula M]. Where Crossref dates differ from the print year (online-first), the print year is used and the online year is noted. Texts read in full this session: Eglash 2016, Kirchherr et al. 2023, Goerner, Lietaer and Ulanowicz 2009, Cox, Arnold and Villamayor Tomás 2010, and Mattsson, Criscione and Takes 2023 (arXiv HTML). Inside this Part, a bare section reference means this Part's own section.

*Caveat.* The operational quantities proposed here for generative justice (a return ratio, an extraction share, a circulation share read from a Hodge decomposition) are not in the generative-justice literature; they are proposals and are marked as such where they appear.

### 1. Generative Justice Defines Justice as the Circulation of Unalienated Value Back to Its Generators

#### Takeaway
Generative justice (Eglash 2016) replaces "extract, centralise, redistribute" with "keep value unalienated and let it circulate back through its generators", over three kinds of value (labour, ecological, expressive). That turns allocation into a question about the topology of value flows: who generates, along which paths value leaves them, and how much comes back under their control. Its central term, "unalienated", has no operational definition: Eglash says so himself. It is not a complete distributive principle, since it says nothing directly about those who cannot generate. And it has drawn almost no independent critique.

#### Cited Findings

**Definition and sources (text read: Eglash 2016, Teknokultura 13(2): 369–404).**
- Definition, verbatim: "The universal right to generate unalienated value and directly participate in its benefits; the rights of value generators to create their own conditions of production; and the rights of communities of value generation to nurture self-sustaining paths for its circulation" (p. 382). — [Eglash 2016, "An introduction to generative justice", Teknokultura 13(2):369–404, doi:10.5209/rev_TEKN.2016.v13.n2.52847] [V] (https://doi.org/10.5209/rev_TEKN.2016.v13.n2.52847)
- The three kinds of value are defined by the harm whose absence they mark: "unalienated labor value instead of poverty; unalienated ecological value instead of pollution, and unalienated expressive value instead of human rights violations" (p. 382). Labour alienation comes from Marx. Environmentalists added ecological value, and rights activists "the unalienated expressive value of free speech, sexuality, spirituality, etc." (abstract, p. 369). — [Eglash 2016] [V]
- Its common principle is "the bottom-up circulation of unalienated value" (p. 373). Eglash calls it "orthogonal" to the capitalism–communism spectrum, since "open source software, composted soil, and reproductive rights have been just as much a struggle in the context of state ownership as they have been under private ownership" (p. 373). — [Eglash 2016] [V]
- Contrast with distributive justice: he describes it as a theory of "how value should continue to be extracted and alienated, but afterwards centralized and redistributed" (p. 380). — [Eglash 2016] [V]
- Visibility: in alienated flows, "the gendered 'caring economy' carried out largely by women to maintain a workforce; the ecological resources stressed by extraction and pollution—all remain invisible" (p. 380). Indigenous artefacts, by contrast, show the labour embedded in them (Iroquois flows, Fig. 1; Fig. 4). — [Eglash 2016] [V]
- Design principle of scale: "nested loops in which networks of generative cycles are linking social, technical and ecological value circulation at multiple scales" (pp. 393–394). "Nested loops" is used as a synonym for "recursive depth" (fn. 15), developed in [Eglash & Banks 2014, "Recursive depth in generative spaces: democratization in three dimensions of technosocial self-organization", The Information Society 30(2):106–115, doi:10.1080/01972243.2014.875775] [V]. The earlier complexity framing is [Eglash & Garvey 2014, "Basins of attraction for generative justice", in Chaos Theory in Politics, Springer, pp. 75–88, doi:10.1007/978-94-017-8691-1_5] [V].
- **Limits Eglash states himself** (text read, pp. 395–396):
  - how to delimit "non-alienated" or "non-extracted" value "without resorting to organicism (romantic notions of authenticity or concreteness)";
  - how local currencies, time banks or barter can circulate "without creating barriers";
  - how to tell "true democratization of economy … 'putting value generators in charge of value circulation'" from neoliberal "precarity" in which capital co-opts bottom-up structures;
  - Chomsky's point on Mondragon, that "even financially stable cases of unalienated labor are not sufficient if they cannot move toward system-wide transformations";
  - whether dependence on non-generative inputs (mass-produced chips for the Arduino) is "parasitic" or transformable ("why not fair trade silicon chips?"). — [Eglash 2016] [V]

**Later work by Eglash and colleagues.**
- *Computational reparations.* [Eglash, Robinson, Bennett, Robert & Garvin 2024, "Computational reparations as generative justice: Decolonial transitions to unalienated circular value flow", Big Data & Society 11(1), doi:10.1177/20539517231221732] [V]. Content not read (publisher returned HTTP 403); only the title is known: it frames reparations as a transition to "unalienated circular value flow".
- *Computing for community-based economies.* [Robinson, Eglash, Robert, Bennett, Guzdial & Nayebare 2025, The Information Society 41(5):290–314, doi:10.1080/01972243.2025.2543926] [V]; arXiv:2504.06114 [V]. From the abstract, it proposes three principles for computing in community-based economies:
  - prefigurative: "the development process itself incorporates equity goals";
  - generative: "the prevention of value extraction, and its replacement by circulations" that return value to labour, nature and society;
  - solidarity: support individual freedoms and mutual aid at all scales.
- *AI governance.* [Eglash, Nayebare, Robinson, Robert, Bennett, Kimanuka & Maina 2024, "AI governance through fractal scaling: integrating universal human rights with emergent self-governance for democratized technosocial systems", AI & Society 40:2663–2676, doi:10.1007/s00146-024-02029-4] [V]. Content not read.
- *Artisanal economy.* [Eglash, Robert, Bennett, Robinson, Lachney & Babbitt 2019/2020, "Automation for the artisanal economy: enhancing the economic and environmental sustainability of crafting professions with human–machine collaboration", AI & Society, doi:10.1007/s00146-019-00915-w] [V for the SSRN record doi:10.2139/ssrn.3446265; the AI & Society DOI is from the Center for Generative Justice's list, so S]. According to a search summary of the accepted manuscript, it concedes that "artisanal" labour is easily simulated (fast-food "Artisan" buns), and that the Arduino ecosystem remains tied to exploitative supply chains.
- *Metrics papers in the 2016 Teknokultura special issue*; content not read, titles only, so whether either gives a formal metric is unknown:
  - [Callahan, Hathaway & Krishnamoorthy 2016, "Quantitative metrics for generative justice: graphing the value of diversity", Teknokultura 13(2):567–586, doi:10.5209/rev_tekn.2016.v13.n2.52838] [V];
  - [Banks 2016, "Anti-authoritarian metrics: recursivity as a strategy for post-capitalism", Teknokultura 13(2):405–438, doi:10.5209/rev_tekn.2016.v13.n2.52267] [V].
- *Empirical cases.*
  - [Dotson & Wilcox 2016, "Generating community, generating justice? The production and circulation of value in community energy initiatives", Teknokultura 13(2):511–540, doi:10.5209/rev_tekn.2016.v13.n2.52840] [V];
  - [Lachney, Eglash, Bennett & Babbitt 2019/2021, "Generative computing: African-American cosmetology as a link between computing education and community wealth", Interactive Learning Environments 29:1115–1135, doi:10.1080/10494820.2019.1636087] [V];
  - [Lokko & Eglash 2017, "Transforming the poor man's building block: value creation, translation and circulation for upcycled indigenous building materials", FOLIO 1:237–249] [S];
  - [Eglash 2022, "Computing for generative justice: decolonizing the circular economy", Raspberry Pi Foundation seminar paper] [S]. This explicitly ties generative justice to the circular economy.
- The full publication list (2013–2025) is on the Center for Generative Justice's page. — [generativejustice.org/publications] (https://generativejustice.org/publications/) [V as author's own page]
- **The name is used by others in other senses:**
  - criminology: [Weaver, Corcoran & McNeill 2025, "Generative justice: exploring its reintegrative possibilities and the implications for parole reform", in Parole Futures, Bloomsbury] [V] (pp. 301–320, doi:10.5040/9781509982202.ch-016; Crossref lists Corcoran first);
  - restorative justice: [Hansen & Umbreit 2018, "Regenerative justice, beyond restoring", Contemporary Justice Review 21(2):185–207] [V] (doi:10.1080/10282580.2018.1455508);
  - a practitioner's essay by the founder of Grassroots Economics, which issues the Sarafu currency (see §5): [Ruddick 2025, "On Generative Justice", Substack] [S].
  A tool must say which sense it means.

**Neighbouring frameworks on relational and Indigenous value.**
- *Relational values.* Values of nature that are neither instrumental nor intrinsic but lie in relationships: [Chan et al. 2016, "Why protect nature? Rethinking values and the environment", PNAS 113(6):1462–1465] [V] (doi:10.1073/pnas.1525002113) (content from memory).
- *IPBES.* The IPBES values assessment's synthesis: [Pascual et al. 2023, "Diverse values of nature for sustainability", Nature 620:813–823] [M] (Crossref did not match this record) (content from memory: decisions dominated by narrow market values, a typology of values, a call for value-plural decision-making).
- *Community economies.* The "diverse economies" iceberg and ethical coordinates of survival, surplus, consumption, commons and investment: [Gibson-Graham, Cameron & Healy 2013, Take Back the Economy, Univ. of Minnesota Press, doi:10.5749/minnesota/9780816676064.001.0001] [V] (content from memory).
- *Gift economy.* A reciprocity framing of economic exchange from an Indigenous botanist: [Kimmerer 2024, The Serviceberry: Abundance and Reciprocity in the Natural World, Scribner] [S] (record only via a third-party repository).
- *Peer production.* Eglash leans on Benkler's "mutualism" for commons-based peer production, quoting "expanding the domain of mutualism improves freedom and well-being under conditions of persistent market imperfection and an inevitably fallible state" (Eglash 2016, about p. 394, text read; page numbers inferred from running footers of the extracted PDF text). — [Benkler 2013, "Practical anarchism: peer mutualism, market power, and the fallible state", Politics & Society 41(2):213–251] [V] (doi:10.1177/0032329213483108)

**Critiques.**
- **No independent critique found.** No independent peer-reviewed critique of generative justice as a framework turned up (search on "generative justice" with critique, limitations, romantic, scaling). The critical material found is Eglash's own (above). — search summary (https://dialnet.unirioja.es/descarga/articulo/5764113.pdf; https://deepblue.lib.umich.edu/bitstream/2027.42/150492/1/Eglash%20et%20al.%20accepted%20in%202019.pdf)

#### Inferences
- **A flow network.** Generative justice needs a value-flow network whose nodes are generators (persons, households, cooperatives, ecosystems) and whose edges are flows, each labelled by its kind of value and by whether it stays under the generator's control. Two quantities are then definable for a generator *g*:
  - a return ratio ρ_g = (value returning to *g* or *g*'s community along cycles) / (value generated by *g*);
  - an extraction share ε_g = (value leaving *g*'s community and not returning within a declared horizon) / (value generated by *g*).
  Neither is in Eglash's text. They are a proposed operationalisation and must be presented as such.
- **Three ledgers that cannot be summed.** The three kinds of value need three ledgers: labour (hours, wages, ownership shares), ecological (material and energy flows, as in industrial ecology's MFA) and expressive (participation, voice, rights). Any sum across them needs a declared commensuration, which generative justice itself warns against: "generative justice cannot utilize abstraction to delineate alienation" (p. 395).
- **What it adds to allocation.** It is a priority and design principle: prefer allocations that (a) reach generators directly, (b) build "self-sustaining paths for circulation" (loops), and (c) leave generators control over their conditions of production.
- **What it lacks.** It has no floor for non-generators (children, the ill, the old, carers whose work is uncounted). Eglash names the care economy as invisible (p. 380) but gives no rule for it. It must be combined with a needs-based floor (§2) and procedural rules (§3).
- **Circulation is not justice.** "Circulation" in generative justice is about who controls value, not only about money moving in loops. A cyclic money flow can still be extractive: debt service cycles back to a lender. So circulation metrics (§5) are necessary but not sufficient evidence of generative justice.

#### Gaps
- No formal metric from the generative-justice literature could be read. Callahan et al. 2016 and Banks 2016 were verified as records only, so whether they define computable metrics is unknown.
- The 2024 *Big Data & Society* paper could not be read (HTTP 403), so how "computational reparations" is to be computed is unknown.
- No independent critique was found. The likely lines of critique (romanticism of craft, scale, exclusion of dependants, co-optation) are Eglash's own and are inferred here, not sourced.

### 2. Circular and Regenerative Economics Bound Allocation by a Floor and a Ceiling Rather Than a Maximum

#### Takeaway
The circular economy is a theory of material loops, not of justice. Its social dimension remains a minority concern even in recent definitions (24–27%), and its critiques (rebound, thermodynamic limits, depoliticisation, vagueness) mean a circularity rate cannot stand for a just allocation. What these traditions do supply is the shape of an allocation rule: a social floor of need satisfaction (Doyal and Gough; Max-Neef; decent living standards) and an ecological ceiling (Raworth's doughnut; safe and just boundaries). That makes allocation a matter of thresholds and shortfalls, not of maximising an aggregate. Community wealth building supplies local-circulation instruments (anchor procurement, LM3), with evidence that is mostly promotional.

#### Cited Findings

**Circular economy: definitions** (text read for Kirchherr et al. 2023).
- The 2017 study analysed 114 definitions. Its main finding: the circular economy's aim is economic prosperity first, environmental quality second, and social equity and future generations barely mentioned. — [Kirchherr, Reike & Hekkert 2017, "Conceptualizing the circular economy: an analysis of 114 definitions", Resources, Conservation and Recycling 127:221–232] [V] (Crossref returned the SSRN record doi:10.2139/ssrn.3037579; the RCR pagination comes from RePEc: https://ideas.repec.org/a/eee/recore/v127y2017icp221-232.html)
- The 2023 update analysed 221 definitions. Shares of definitions mentioning each theme, 2017 against 2022:
  - sustainable development as a principal aim: 11% → 27–31%;
  - environmental quality: 36% → 48–52%;
  - economic development: 46% → 46–52%;
  - social equity and related topics: 19% → 24–27%;
  - decoupling growth from resource depletion: none → 11–16% (the dimension under which degrowth and sufficiency were coded; degrowth "remains a niche");
  - future generations: 1 of 114 → 9–11%. — [Kirchherr, Yang, Schulze-Spüntrup, Heerink & Hartley 2023, "Conceptualizing the circular economy (revisited): an analysis of 221 definitions", RCR 194:107001, doi:10.1016/j.resconrec.2023.107001] [V]
- The 2023 meta-definition, verbatim: "The circular economy is a regenerative economic system which necessitates a paradigm shift to replace the 'end of life' concept with reducing, alternatively reusing, recycling, and recovering materials throughout the supply chain, with the aim to promote value maintenance and sustainable development, creating environmental quality, economic development, and social equity, to the benefit of current and future generations. It is enabled by an alliance of stakeholders (industry, consumers, policymakers, academia) and their technological innovations and capabilities." — [Kirchherr et al. 2023] [V]
- The same paper cites Kirchherr's complaint that much of the circular-economy literature is "scholarly bullshit". — [Kirchherr 2022, "Bullshit in the sustainability and transitions literature: a provocation", Circular Economy and Sustainability] [V] (vol. 3:167–172, 2023 issue, online 2022, doi:10.1007/s43615-022-00175-9)

**Circular economy: critiques.**
- Corvellec et al. find the concept has "diffused limits, unclear theoretical grounds", faces "structural obstacles" to implementation, and is a "theoretically, practically, and ideologically questionable notion". From the abstract and a Lund University press summary:
  - it is too diffuse to measure;
  - it overlooks existing material stocks;
  - it has a gap on social equity;
  - it depoliticises policy while favouring market and business power;
  - the authors propose a "more modest" concept. — [Corvellec, Stowell & Johansson 2022, "Critiques of the circular economy", Journal of Industrial Ecology 26(2):421–432, doi:10.1111/jiec.13187] [V] (online 2021)
- Korhonen et al. list limits, from memory: thermodynamic limits to recycling, spatial and temporal system boundaries, rebound, path dependence and lock-in, intra-organisational versus inter-organisational strategies, and the definition of physical flows. — [Korhonen, Honkasalo & Seppälä 2018, "Circular economy: the concept and its limitations", Ecological Economics 143:37–46, doi:10.1016/j.ecolecon.2017.06.041] [V]
- Rebound: secondary production may not displace primary production, so circular activity can raise total output. From memory of the abstract. — [Zink & Geyer 2017, "Circular economy rebound", Journal of Industrial Ecology 21(3):593–602, doi:10.1111/jiec.12545] [V]
- The concept largely ignores the social dimension (from memory). — [Murray, Skene & Haynes 2017, "The circular economy: an interdisciplinary exploration of the concept and application in a global context", Journal of Business Ethics 140:369–380, doi:10.1007/s10551-015-2693-2] [V] (online 2015)
- Measured circularity is low: about 6% of processed materials were recycled worldwide around 2005 (figure from memory). — [Haas, Krausmann, Wiedenhofer & Heinz 2015, "How circular is the global economy?", Journal of Industrial Ecology 19(5):765–777] [S] (Crossref returned a 2016 book-chapter version, doi:10.1007/978-3-319-33326-7_11)
- Circular bioeconomy and decoupling, critically: [Giampietro 2019, Ecological Economics] [V] (vol. 162:143–156, doi:10.1016/j.ecolecon.2019.05.001).
- *Social versions of the circular economy* (records only, content from memory):
  - human development: [Schröder, Lemille & Desmond 2020, "Making the circular economy work for human development", RCR 156:104686, doi:10.1016/j.resconrec.2020.104686] [V];
  - a typology of discourses from techno-optimist to transformational "circular society": [Calisto Friant, Vermeulen & Salomone 2020, RCR] [V] (vol. 161:104917, doi:10.1016/j.resconrec.2020.104917);
  - de-growing the circular economy: [Hobson & Lynch 2016, Futures] [V] (vol. 82:15–25, doi:10.1016/j.futures.2016.05.012).

**Doughnut economics: floor and ceiling.**
- The doughnut is a social foundation (shortfall below it) inside an ecological ceiling (overshoot beyond it). — [Raworth 2017, "A Doughnut for the Anthropocene: humanity's compass in the 21st century", Lancet Planetary Health 1(2):e48–e49, doi:10.1016/S2542-5196(17)30028-1] [V]; [Raworth 2017, Doughnut Economics: Seven Ways to Think Like a 21st-Century Economist, Random House/Chelsea Green] [S] (record seen only through a 2025 review in ZARCH)
- The 2025 renewal has 35 indicators of social shortfall and ecological overshoot, 2000–2022. It turns the doughnut from a single-year snapshot into an annual monitor. Overshoot is per-capita consumption against downscaled boundaries for climate, nutrients, freshwater and biodiversity. Overshoot "would have to stop immediately and accelerate nearly two times faster towards planetary boundaries" to safeguard stability by 2050. The richest 20% contribute disproportionately to overshoot. — [Fanning & Raworth 2025, "Doughnut of social and planetary boundaries monitors a world out of balance", Nature 646:47–56, doi:10.1038/s41586-025-09385-1] [V]. Content from abstract and press summaries (https://eprints.whiterose.ac.uk/id/eprint/233706; https://naturerecovery.ox.ac.uk/news/as-global-economy-doubles-in-two-decades-progress-to-end-poverty-is-slow-while-planetary-damage-accelerates/)
- No country meets basic needs at a globally sustainable level of resource use. From memory of the abstract. — [O'Neill, Fanning, Lamb & Steinberger 2018, "A good life for all within planetary boundaries", Nature Sustainability 1:88–95, doi:10.1038/s41893-018-0021-4] [V]
- The same thresholds are applied as a time series per nation. — [Fanning, O'Neill, Hickel & Roux 2022, "The social shortfall and ecological overshoot of nations", Nature Sustainability 5:26–36, doi:10.1038/s41893-021-00799-z] [V] (online 2021)
- Boundaries that are "safe" and "just" (the latter avoiding significant harm to people) are quantified for the Earth system. — [Rockström et al. 2023, "Safe and just Earth system boundaries", Nature 619:102–111] [S] (Crossref matched only an EGU abstract of the same title; the Nature pagination is from memory) (content from memory)
- Per-capita fair shares of resource use show high-income nations responsible for most excess use, 1970–2017 (from memory). — [Hickel, O'Neill, Fanning & Zoomkawala 2022, "National responsibility for ecological breakdown: a fair-shares assessment of resource use, 1970–2017", Lancet Planetary Health 6(4):e342–e349, doi:10.1016/S2542-5196(22)00044-4] [V]
- **Critiques of the doughnut** (search summaries):
  - *Scale.* Downscaling to nations and cities is hard: its indicators were defined globally, and political power is divided geographically. — (https://www.exeter.ac.uk/v8media/research/esi/documents/Downscaling_doughnut_economics.pdf) [S]; (https://dukespace.lib.duke.edu/server/api/core/bitstreams/ad26a662-aada-445f-97b3-660c2475e7fd/content) [S]
  - *Measurability.* Some planetary boundaries are not quantified.
  - *Derivation.* How the twelve social dimensions were derived from the SDGs is not explained. — (https://en.wikipedia.org/wiki/Doughnut_(economic_model)) [S, weak source]
  - The 2025 paper itself notes that nations differ greatly in responsibility for overshoot. — search summary of Fanning & Raworth 2025

**Human needs as the floor.**
- Health and autonomy are universal basic needs. "Intermediate needs" (universal satisfier characteristics) and culturally variable "satisfiers" lie between them and goods. — [Doyal & Gough 1991, A Theory of Human Need, Macmillan, doi:10.1007/978-1-349-21500-3] [V] (content from memory)
- Gough applies needs theory to sustainable welfare; needs are non-substitutable and satiable, unlike preferences (from memory). — [Gough 2015, "Climate change and sustainable welfare: the centrality of human needs", Cambridge Journal of Economics 39(5):1191–1214, doi:10.1093/cje/bev039] [V]; [Gough 2017, Heat, Greed and Human Need, Edward Elgar] [S] (record via a 2018 review in International Dialogue, doi:10.32873/uno.dc.id.8.1.1157)
- Nine axiological needs (subsistence, protection, affection, understanding, participation, idleness, creation, identity, freedom) are crossed with four existential categories (being, having, doing, interacting). Satisfiers are distinguished from needs, and include pseudo-satisfiers, inhibitors and synergic satisfiers (from memory). — [Max-Neef 1991, Human Scale Development, Apex Press] [S] (record via a book-summary chapter, doi:10.9774/gleaf.978-1-907643-44-6_18)
- Decent living standards: [Rao & Min 2018, "Decent living standards: material prerequisites for human wellbeing", Social Indicators Research 138:225–244, doi:10.1007/s11205-017-1650-0] [V].
- Decent living with minimum energy: decent living for all could be provided at roughly 1960s levels of global final energy despite a much larger population (figure from memory). — [Millward-Hopkins, Steinberger, Rao & Oswald 2020, Global Environmental Change 65:102168, doi:10.1016/j.gloenvcha.2020.102168] [V]
- Need satisfiers and provisioning systems: [Brand-Correa & Steinberger 2017, "A framework for decoupling human need satisfaction from energy use", Ecological Economics 141:43–52, doi:10.1016/j.ecolecon.2017.05.019] [V].

**Degrowth and sufficiency.**
- A review of degrowth research. Its distributive core, from memory: lower aggregate throughput in rich economies with reduced inequality, sufficiency and care. — [Kallis, Kostakis, Lange, Muraca, Paulson & Schmelzer 2018, "Research on degrowth", Annual Review of Environment and Resources 43:291–316, doi:10.1146/annurev-environ-102017-025941] [V]
- Sufficiency as a principle of "enough": [Princen 2005, The Logic of Sufficiency, MIT Press] [M].

**Community wealth building (the Preston model).**
- CWB's strategic framing: [Guinan & O'Neill 2019, "From community wealth-building to system change", IPPR Progressive Review 25(4):382–392, doi:10.1111/newe.12130] [V].
- Preston read as "entrepreneurial municipalism": [Thompson, Nowak, Southern, Davies & Furmedge 2020, "Re-grounding the city with Polanyi: from urban entrepreneurialism to entrepreneurial municipalism", Environment and Planning A 52(6):1171–1194, doi:10.1177/0308518X19899698] [V].
- **Baseline spend** (council and CLES figures, promotional): of about £750m of anchor-institution spend, 5% stayed in Preston and 39% in Lancashire. Retained procurement later rose to £112.3m, an increase of £74m from 2012/13; another figure is +£200m in Lancashire. The base set of institutions changes between analyses. — search summary of Preston City Council and CLES documents (https://www.preston.gov.uk/media/1792/How-we-built-community-wealth-in-Preston/pdf/CLES_Preston_Document_WEB_%20AW.pdf; https://www.local.gov.uk/inclusive-economies-preston-city-councils-approach-community-wealth-building) [S]
- **An NIHR-linked evaluation** compared Preston with similar areas. Per the search snippet (not read): a positive effect on employment over a short period, disproportionately benefiting disadvantaged groups. — (https://arc-nwc.nihr.ac.uk/wp-content/uploads/2026/03/Evaluation-of-Community-Wealth-Building-in-Preston-1.pdf) [S]
- **Critiques of CWB:**
  - protectionism, and the risk of siloing places from one another: [Dennis & Stanley 2023, "The de-globalisation of capital? The political economy of community wealth building", New Political Economy] [V] (vol. 28(4):677–692, doi:10.1080/13563467.2022.2159353);
  - "community wealth building or local authority rhetoric?", with tensions of power between the council and community groups: [Redwood, Smith, Steiner & Whittam 2022, Local Economy 37(7):602–621] [V] (doi:10.1177/02690942231171657).
  Both are search summaries.
- **LM3** was developed by the New Economics Foundation: LM3 = (R1 + R2 + R3) / R1. Here R1 is the initial income, R2 the share of it spent locally, and R3 the local re-spending of R2 by its recipients. Worked example: £100k, £80k and £40k give LM3 = 2.20. — [Sacks 2002, The Money Trail: Measuring Your Impact on the Local Economy Using LM3, NEF] [S]; formula from NEF Consulting (https://www.nefconsulting.com/?p=1208) and a university lecture adapted from Sacks (https://web2.mendelu.cz/inobio/html/nove/Economics_of_sustainable_management/8b_ESM_lecture.pdf) [S]

**Regenerative economics as network health** (text read for Goerner et al. 2009).
- **The formulas** (natural logarithms; a dot subscript means summation over that index), quoted from Goerner et al. 2009 footnotes 5–7:
  - systemic efficiency (ascendency) SE = A = Σ_ij T_ij log(T_ij T_·· / (T_i· T_·j));
  - resilience capacity (overhead) RC = Φ = −Σ_ij T_ij log(T_ij² / (T_i· T_·j));
  - developmental capacity C = SE + RC = −Σ_ij T_ij log(T_ij / T_··).
  Here T_ij is the flow from node i to j and T_·· is total system throughput (TST). — [Goerner, Lietaer & Ulanowicz 2009, "Quantifying economic sustainability: implications for free-enterprise theory, policy and practice", Ecological Economics 69(1):76–81, doi:10.1016/j.ecolecon.2009.07.018] [V]
- **The paper's claims** (text read):
  - "Both GDP and TST are poor measures of sustainability … because they measure volume, while ignoring the network structure";
  - sustainability is the balance of efficiency and resilience, with a single maximum situated "slightly toward the resilience side";
  - ecosystem data match a "Window of Vitality" ([Zorach & Ulanowicz 2003, "Quantifying the complexity of flow networks: how many roles are there?", Complexity 8(3):68–76, doi:10.1002/cplx.10075] [V]), but "are not sufficient to determine the exact optimum";
  - autocatalytic "centripetal pull" concentrates flows and erodes the surrounding network;
  - "the best way to restore robustness today would be to revitalize our small-scale fair-enterprise root system". The paper also makes political claims about NAFTA and Glass–Steagall that are advocacy, not demonstrated results. — [Goerner et al. 2009] [V]
- The robustness function R = −a log a with a = A/C, maximal at a = 1/e [formula M]: [Ulanowicz, Goerner, Lietaer & Gómez 2009, "Quantifying sustainability: resilience, efficiency and the return of information theory", Ecological Complexity 6(1):27–36] [V] (doi:10.1016/j.ecocom.2008.10.005).
- Ten principles of regenerative economics, with measures built on ecological network analysis: [Fath, Fiscus, Goerner, Berea & Ulanowicz 2019, "Measuring regenerative economics: 10 principles and measures undergirding systemic economic health", Global Transitions 1:15–27, doi:10.1016/j.glt.2019.02.002] [V] (content not read).
- [Fullerton 2015, Regenerative Capitalism, Capital Institute white paper] [M] (content from memory: eight principles, including "right relationship" and "robust circulatory flow").

**The care economy.**
- Capitalism depends on social reproduction, which it depletes (from memory). — [Fraser 2016, "Contradictions of capital and care", New Left Review 100:99–117] [V] (Crossref returned volume "2"; NLR's own numbering is issue 100)
- [Folbre 2001, The Invisible Heart: Economics and Family Values, New Press] [S] (record via a 2003 review in Gender & Society, doi:10.1177/0891243203017002017).

#### Inferences
- **Allocation as thresholds.** The doughnut and needs theory give an allocation rule shaped as thresholds:
  - (1) a floor: for each unit *i* and need dimension *k*, a declared threshold f_k for the satisfier level s_ik;
  - (2) a ceiling: total ecological use Σ_i e_i ≤ B, the declared, downscaled budget, possibly per capita with fair-share adjustments for historical responsibility (Hickel et al. 2022);
  - (3) between them, a space settled by other principles and by procedure.
  One computable form is lexicographic: first minimise weighted shortfall Σ_i Σ_k w_k max(0, f_k − s_ik), subject to the ceiling. Then report overshoot. Then, among allocations tied at the minimum, choose by declared procedure (§3) or by priority to the worst-off. This form is proposed here, not taken from Raworth.
- **The thresholds are normative.** They are declared and contested, not scientific facts: Raworth's social dimensions come from the SDGs, and some boundaries are unquantified. A tool must carry each threshold with its source and allow alternatives, in the manner of the needs domain's Bradshaw standards.
- **A circular-economy metric is not a justice metric.** The 2023 data show that most circular-economy definitions still omit social equity. Material circularity (recycling rates, cycling indices) belongs on the ecological ledger (industrial ecology), not on the justice ledger.
- **What LM3 measures.** It counts local re-spending in three rounds from a survey. It is not a counterfactual impact estimate: it ignores displacement and says nothing about who in the locality receives the money. CWB's "keep wealth local" can conflict with justice across places (the siloing critique).
- **Two different kinds of measure.** Ecological network analysis (ascendency and overhead) offers structural measures of concentration against redundancy in a flow network. They describe resilience-relevant structure, not justice. The claimed "window of vitality" optimum is not established for economies (Goerner et al. 2009 concede the data cannot fix the optimum).

#### Gaps
- The full critique list of Corvellec et al. 2022 (from abstract and press summary only).
- Independent, peer-reviewed counterfactual evaluation of the Preston model's local spend: the NIHR-linked evaluation was seen only in a snippet.
- Whether Raworth's social thresholds have a published derivation.
- Primary-text confirmation of Max-Neef's matrix and of the Doyal–Gough structure (from memory).

### 3. Commons and Polycentric Governance Supply Tested Allocation Rules and Their Procedural Conditions

#### Takeaway
Ostrom's design principles are procedural conditions for durable self-governance, and they are among the best empirically tested claims in this whole field. Cox et al. reviewed 91 studies: every principle has more supporting than unsupporting cases, and the principle of congruence between costs and benefits is strongly supported. Commons practice also holds a catalogue of concrete allocation rules (rotation, shares by land or contribution, priority by tenure, need-contingent rules), and some systems switch rules under scarcity: proportional in plenty, survival in drought. That is directly transferable to budgets. The critiques are incompleteness, missing power and history, blueprint thinking, and doubtful scaling beyond communities.

#### Cited Findings

**The design principles and their tests.**
- The eight design principles, wording from memory of Ostrom 1990, Table 3.1:
  1. clearly defined boundaries (of users and resource);
  2. congruence between appropriation and provision rules and local conditions;
  3. collective-choice arrangements (most of those affected can participate in modifying the rules);
  4. monitoring (by those accountable to the users);
  5. graduated sanctions;
  6. conflict-resolution mechanisms (low-cost, rapid);
  7. minimal recognition of rights to organise;
  8. nested enterprises (for larger systems). — [Ostrom 1990, Governing the Commons: The Evolution of Institutions for Collective Action, Cambridge UP, doi:10.1017/CBO9780511807763] [V]
- **The Cox et al. review** (text read):
  - 91 studies covering 77 cases; the average evaluation score was 3.73, just below "moderately supportive";
  - abstract (non-empirical) studies were the most critical (median 2.0), so most criticism is abstract rather than empirical;
  - in the cases, every principle had at least twice as many supportive as unsupportive cases; Fisher's exact test was significant at 5% for all except principle 8 (10%);
  - support was strong for 1A, 2A, 2B and 4B, moderate for the rest.
  They reformulated the principles:
  - 1A user boundaries;
  - 1B resource boundaries;
  - 2A congruence with local conditions;
  - 2B congruence between appropriation and provision, i.e. between costs and benefits;
  - 4A monitoring of users;
  - 4B monitoring of the resource. — [Cox, Arnold & Villamayor Tomás 2010, "A review of design principles for community-based natural resource management", Ecology and Society 15(4):38, doi:10.5751/ES-03704-150438] [V]
- **Evidence for 2B (proportional equivalence)** (text read in Cox et al. 2010):
  - Pomeroy et al. 2001: users expect benefits of compliance to exceed costs;
  - Klooster 2000: successful logging communities reinvested benefits;
  - Trawick 2001: an Andean irrigation system applies rules uniformly to all while obligations vary with each farmer's needs, which reinforced shared equity;
  - Cox 2010: New Mexico acequias allocate water in proportion to land owned during plenty, but shift to ensuring each member's survival during scarcity. "Perceptions of equity can vary with resource availability." — [Cox et al. 2010] [V]; [Trawick 2001, "Successfully governing the commons: principles of social organization in an Andean irrigation system", Human Ecology] [V] (vol. 29:1–25, doi:10.1023/A:1007199304395)
- The principles act in configurations, not singly (content from memory of the abstract). — [Baggio, Barnett, Perez-Ibarra et al. 2016, "Explaining success and failure in the commons: the configural nature of Ostrom's institutional design principles", International Journal of the Commons 10(2):417–439, doi:10.18352/ijc.634] [V]
- The principles generalise to the efficacy of human groups in general (content from memory). — [Wilson, Ostrom & Cox 2013, "Generalizing the core design principles for the efficacy of groups", Journal of Economic Behavior & Organization 90:S21–S32, doi:10.1016/j.jebo.2012.12.010] [V]

**Frameworks.**
- IAD: [Ostrom 2005, Understanding Institutional Diversity, Princeton UP, doi:10.1515/9781400831739] [V].
- SES framework: [Ostrom 2009, "A general framework for analyzing sustainability of social-ecological systems", Science 325:419–422, doi:10.1126/science.1172133] [V]; [McGinnis & Ostrom 2014, "Social-ecological system framework: initial changes and continuing challenges", Ecology and Society 19(2):30, doi:10.5751/ES-06387-190230] [V].
- **Polycentricity:**
  - Ostrom's Nobel lecture: [Ostrom 2010, "Beyond markets and states: polycentric governance of complex economic systems", American Economic Review 100(3):641–672, doi:10.1257/aer.100.3.641] [V];
  - the origin: [Ostrom, Tiebout & Warren 1961, "The organization of government in metropolitan areas: a theoretical inquiry", American Political Science Review 55(4):831–842, doi:10.2307/1952530] [V];
  - [Carlisle & Gruby 2019, "Polycentric systems of governance: a theoretical model for the commons", Policy Studies Journal 47(4):927–952, doi:10.1111/psj.12212] [V] (online 2017).
- Bundles of property rights: access, withdrawal, management, exclusion, alienation (content from memory). — [Schlager & Ostrom 1992, "Property-rights regimes and natural resources: a conceptual analysis", Land Economics 68(3):249–262] [V] (doi:10.2307/3146375)

**Allocation rules in common-pool resources** (cases in Ostrom 1990, from memory unless marked).
- Alanya, Turkey: fishing spots allocated by lot and rotated daily [M].
- Valencia's huertas: water taken in turn (tanda) by fixed order along a canal; in drought, by priority to crops in most need [M].
- Bali's subak: irrigation schedules synchronised by water-temple networks. The schedules emerge from local imitation and balance pest control against water sharing (content from memory). — [Lansing & Kremer 1993, "Emergent properties of Balinese water temple networks: coadaptation on a rugged fitness landscape", American Anthropologist 95(1):97–114, doi:10.1525/aa.1993.95.1.02a00050] [V]; [Lansing et al. 2017, "Adaptive self-organization of Bali's ancient rice terraces", PNAS 114(25):6504–6509, doi:10.1073/pnas.1605369114] [V]
- **Porto Alegre's participatory budgeting.** District investment was allocated by points (as described for 1999 practice):
  - need, measured by prior access to a service: under 20% access scores 5, 80% access scores 1;
  - population;
  - the community's own priority ranking;
  - each district ranked both district themes and city-wide themes.
  Source: a World Bank note citing de Sousa Santos 1998, search summary (https://documents1.worldbank.org/curated/en/741601468744101249/pdf/274620PAPER0snd71.pdf) [S]. The paper itself: [de Sousa Santos 1998, "Participatory budgeting in Porto Alegre: toward a redistributive democracy", Politics & Society 26(4):461–510] [V] (doi:10.1177/0032329298026004003). Goldsmith and Vainer describe the weights (population, poverty, shortages, citywide priorities) as "determined by popular debate" (https://www.lincolninst.edu/publications/articles/participatory-budgeting-power-politics-porto-alegre) [S].
- **Participatory budgeting in Brazil.** Municipalities using it shifted spending toward sanitation and health and had lower infant mortality (content from memory of the abstract). — [Gonçalves 2014, "The effects of participatory budgeting on municipal expenditures and infant mortality in Brazil", World Development 53:94–110, doi:10.1016/j.worlddev.2013.01.009] [V]

**Critiques.**
- Cox et al. 2010 summarise three critiques (text read):
  - the principles are incomplete: they miss trust, legitimacy, resource properties and external factors such as market integration (the authors agree);
  - they may not apply at larger scales;
  - they are blueprint-like (the authors argue the principles and diagnostic approaches are complementary). — [Cox et al. 2010] [V]
- Agrawal (from memory): too many variables relative to cases, causal heterogeneity, and neglect of politics and markets. — [Agrawal 2003, "Sustainable governance of common-pool resources: context, methods, and politics", Annual Review of Anthropology 32:243–262, doi:10.1146/annurev.anthro.32.061002.093112] [V]
- Clement proposes a "politicised" IAD that adds power and discourse. — [Clement 2010, Policy Sciences 43:129–156, doi:10.1007/s11077-009-9100-8] [V] (online 2009)
- Critical institutionalism (from memory): institutions as bricolage, embedded in history and power, not designed. — [Cleaver & de Koning 2015, "Furthering critical institutionalism", International Journal of the Commons 9(1):1–18, doi:10.18352/ijc.605] [V]

#### Inferences
- **Translations of the principles into a public budget.**
  - Proportional equivalence (2B): track, for each group, contributions (taxes, labour, unpaid care, ecological services) against benefits received, with exceptions for need declared openly.
  - Collective-choice arrangements (3): those affected can amend the allocation rule itself, not only its parameters.
  - Monitoring (4A, 4B): of allocations, and of the state of what they sustain.
  - Nested enterprises (8): envelopes allocated to nested units, which allocate within them under their own rules (polycentric allocation).
- **A rule that switches with scarcity.** The acequia's switch (proportional to entitlement in plenty, survival minimum in scarcity) is a rule template a tool can represent exactly:

  allocation_i = entitlement share × S if S ≥ Σ_i need_i; otherwise need-first (each unit to its survival floor, then pro rata).

  This is the commons' version of a sufficiency floor (§2), and of claims rules in bankruptcy problems.
- **Points rules are threshold allocation in practice.** Porto Alegre's rule combines need (a decreasing step function of prior access), population and voiced priority. It is precedent for a declared, multi-criterion, deliberated allocation that is not an optimisation.
- **What the evidence is about.** It concerns sustained self-governance of a resource, not justice. Cox et al. note that perceived equity varies with abundance, so the principles describe conditions under which allocation rules are accepted and kept, not which rule is right.

#### Gaps
- Primary-text verification of the wording of the eight principles and of the Alanya and Valencia details (from memory).
- Porto Alegre's 1998 formula itself: only a 2003 description of 1999 practice was found.
- Systematic evidence on transferring commons principles to municipal or national budgets beyond participatory budgeting.

### 4. Evolutionary and Complexity Economics Treat Allocation Rules as Evolving Routines and Norms

#### Takeaway
In evolutionary economics, allocation rules are routines and norms subject to variation, selection and retention. Complexity economics adds non-equilibrium, path dependence and adaptive agents. The evolutionary game theory of fairness gives three robust lessons:
- **Equal split is the largest attractor** in symmetric bargaining (about 62% of random starting populations reach it in Skyrms's divide-the-dollar), and it is stochastically stable in Axtell–Epstein–Young.
- **Unequal norms can persist.** Once agents carry arbitrary type tags, discriminatory norms emerge and last for times that grow exponentially with memory and population; minorities can be disadvantaged merely by being fewer.
- **Fairness is not universal.** Experimental norms of division vary widely across societies (ultimatum mean offers of 26–58% in 15 small-scale societies), with market integration and religion.

So evolutionary stability is not justice. A tool can use evolution to explore robustness and keep a diverse repertoire of good allocations (quality-diversity), and adaptive management to learn over time. It cannot use them to certify fairness.

#### Cited Findings

**Foundations.**
- Routines as genes, search, and selection by market (content from memory). — [Nelson & Winter 1982, An Evolutionary Theory of Economic Change, Belknap/Harvard UP] [S] (Crossref returned a 1983 review in the Southern Economic Journal, doi:10.2307/1058233)
- Generalised Darwinism: variation, selection and retention as general principles of social evolution. — [Hodgson & Knudsen 2010, Darwin's Conjecture: The Search for General Principles of Social and Economic Evolution, Univ. of Chicago Press, doi:10.7208/chicago/9780226346922.001.0001] [V]
- Critique (from memory): generalised Darwinism is an analogy without explanatory content; Witt proposes a "continuity hypothesis" instead. — [Levit, Hossfeld & Witt 2011, "Can Darwinism be generalized and of what use would this be?", Journal of Evolutionary Economics 21:545–562] [V] (doi:10.1007/s00191-011-0235-3)
- Complexity economics: the economy as a system always forming, of agents adapting to outcomes they jointly create; non-equilibrium, increasing returns, path dependence (from memory). — [Arthur 2021, "Foundations of complexity economics", Nature Reviews Physics 3:136–145, doi:10.1038/s42254-020-00273-3] [V]; [Arthur 2014, Complexity and the Economy, Oxford UP] [S] (Crossref did not match; widely cited); [Beinhocker 2006, The Origin of Wealth, Harvard Business School Press] [S] (record via a 2008 review in Economic Affairs, doi:10.1111/j.1468-0270.2008.00813_9.x)
- Agent-based macroeconomics: [Farmer & Foley 2009, "The economy needs agent-based modelling", Nature 460:685–686, doi:10.1038/460685a] [V]; [Dosi, Fagiolo & Roventini 2010, "Schumpeter meeting Keynes: a policy-friendly model of endogenous growth and business cycles", Journal of Economic Dynamics and Control 34(9):1748–1767, doi:10.1016/j.jedc.2010.06.018] [V].

**Evolutionary game theory of fairness.**
- **Binmore.** Fairness norms evolved (from food-sharing). Rawls's original position is an equilibrium-selection device by which players choose among the equilibria of the "game of life". The project reconciles Rawls's egalitarian and Harsanyi's utilitarian versions of the original position. — [Binmore 2005, Natural Justice, Oxford UP, doi:10.1093/acprof:oso/9780195178111.001.0001] [V]. Content from Binmore's own summaries and an Œconomia commentary via search summary (https://journals.openedition.org/oeconomia/19389) [S]
- **Skyrms; Alexander and Skyrms.** In the Nash demand game ("divide the dollar") under replicator dynamics, a population whose initial proportions are chosen at random converges to fair division about 62% of the time. Correlation and spatial structure raise this. — [Skyrms 1996/2014, Evolution of the Social Contract, Cambridge UP; 2nd edn doi:10.1017/CBO9781139924825] [V for 2nd edn] [S] (1996 edn known via a 1997 review in Philosophy, doi:10.1017/S0031819100062458) for 1996; [Alexander & Skyrms 1999, "Bargaining with neighbors: is justice contagious?", Journal of Philosophy 96(11):588–598, doi:10.2307/2564625] [V]. The 62% figure is from a search summary of a citing paper [S]. In Skyrms's three-type model, a fair-minded population and a greedy–modest polymorphism are both asymptotically stable, and the fair state has the larger basin (search summary).
- Spatial evolution of morality: [Alexander 2007, The Structural Evolution of Morality, Cambridge UP] [V] (doi:10.1017/CBO9780511550997).
- **Young.** Adaptive play with bounded memory and mistakes; the stochastically stable division approximates the Nash bargaining solution (content from memory). — [Young 1993, "An evolutionary model of bargaining", Journal of Economic Theory 59(1):145–168, doi:10.1006/jeth.1993.1009] [V]
- **Axtell, Epstein and Young.** Agents with arbitrary tags settle into one of three regimes:
  - an equity norm (equal shares, tags meaningless);
  - a discriminatory norm (shares by tag);
  - fractious states with no stable norm.
  The equity norm is asymptotically (stochastically) more stable. Discriminatory regimes are metastable and can last long: the waiting time to exit grows exponentially with memory length and number of agents. — [Axtell, Epstein & Young 2001, "The emergence of classes in a multi-agent bargaining model", in Durlauf & Young (eds), Social Dynamics, MIT Press, pp. 191–212, doi:10.7551/mitpress/6294.003.0009] [V]. Content from abstract and search summaries (https://www.brookings.edu/articles/the-emergence-of-classes-in-a-multi-agent-bargaining-model; https://arxiv.org/pdf/2205.05994) [S]
- **O'Connor; Bruner.** Social categories and cultural evolution produce unfair conventions. In the "cultural Red King" effect, minority groups can end with worse bargaining conventions because they learn from more frequent interactions with the majority (content from memory). — [O'Connor 2019, The Origins of Unfairness: Social Categories and Cultural Evolution, Oxford UP, doi:10.1093/oso/9780198789970.001.0001; chapter "Cultural evolution with social categories", doi:10.1093/oso/9780198789970.003.0004] [V]; [Bruner 2019, "Minority (dis)advantage in population games", Synthese 196:413–427, doi:10.1007/s11229-017-1487-8] [V] (online 2017)
- The ultimatum game: [Güth, Schmittberger & Schwarze 1982, "An experimental analysis of ultimatum bargaining", Journal of Economic Behavior & Organization 3(4):367–388, doi:10.1016/0167-2681(82)90011-7] [V].
- Reputation makes fair offers evolve in the ultimatum game (content from memory). — [Nowak, Page & Sigmund 2000, "Fairness versus reason in the ultimatum game", Science 289:1773–1775, doi:10.1126/science.289.5485.1773] [V]
- **Henrich et al.** Mean ultimatum offers in 15 small-scale societies ranged from 26% (Machiguenga) to 58%, against about 44% typical in industrial societies. Market integration and payoffs to cooperation explain much of the variation between groups. — [Henrich, Boyd, Bowles, Camerer, Fehr, Gintis & McElreath 2001, "In search of Homo economicus: behavioral experiments in 15 small-scale societies", American Economic Review 91(2):73–78, doi:10.1257/aer.91.2.73] [V]. Figures from a search summary of the paper's text (https://collections.lib.utah.edu/dl_files/ca/98/ca984df92c572134e7bc40ed50392adc4a2c28db.pdf) [S]
- The full study: [Henrich et al. 2005, "'Economic man' in cross-cultural perspective: behavioral experiments in 15 small-scale societies", Behavioral and Brain Sciences 28(6):795–815, doi:10.1017/S0140525X05000142] [V].
- Market integration and world religion are associated with fairer offers, and community size with more punishment (content from memory of the abstract). — [Henrich, Ensminger, McElreath et al. 2010, "Markets, religion, community size, and the evolution of fairness and punishment", Science 327:1480–1484, doi:10.1126/science.1182238] [V]
- Western student samples are unrepresentative. — [Henrich, Heine & Norenzayan 2010, "The weirdest people in the world?", Behavioral and Brain Sciences 33:61–83] [V] (Crossref returned the SSRN record, doi:10.2139/ssrn.1601785; the BBS volume and pages are from memory)
- Hypotheses for food transfers: kin selection, reciprocity, tolerated theft, costly signalling (content from memory). — [Gurven 2004, "To give and to give not: the behavioral ecology of human food transfers", Behavioral and Brain Sciences 27(4):543–559, doi:10.1017/S0140525X04000123] [V]
- Reverse dominance hierarchy in hunter-gatherers: [Boehm 1999, Hierarchy in the Forest, Harvard UP] [M].

**Adaptive and learning approaches to policy.**
- Reforms as experiments: [Campbell 1969, "Reforms as experiments", American Psychologist 24(4):409–429, doi:10.1037/h0027982] [V].
- Adaptive management: [Holling (ed.) 1978, Adaptive Environmental Assessment and Management, Wiley/IIASA] [S] (record via a 1979 review, doi:10.1177/030913257900300423); [Walters 1986, Adaptive Management of Renewable Resources] [M].
- Experimentalist governance (framework goals, local discretion, peer review, revision): [Sabel & Zeitlin 2012, "Experimentalist governance", in Oxford Handbook of Governance, doi:10.1093/oxfordhb/9780199560530.013.0012] [V].

**Search for many good allocations.**
- Quality-diversity search: [Pugh, Soros & Stanley 2016, "Quality diversity: a new frontier for evolutionary computation", Frontiers in Robotics and AI 3:40, doi:10.3389/frobt.2016.00040] [V]; [Mouret & Clune 2015, "Illuminating search spaces by mapping elites", arXiv:1504.04909] [S]; [Lehman & Stanley 2011, "Abandoning objectives: evolution through the search for novelty alone", Evolutionary Computation 19(2):189–223, doi:10.1162/EVCO_a_00025] [V].
- **Machine-designed policy:**
  - Tax policy designed by two-level multi-agent reinforcement learning in simulation, reported to improve the equality–productivity trade-off over baselines (content from memory). — [Zheng et al. 2022, "The AI Economist: taxation policy design via two-level deep multiagent reinforcement learning", Science Advances 8(18):eabk2607, doi:10.1126/sciadv.abk2607] [V]
  - A redistribution mechanism trained to win human majority votes in an investment game (content from memory). — [Koster et al. 2022, "Human-centred mechanism design with Democratic AI", Nature Human Behaviour 6:1398–1407, doi:10.1038/s41562-022-01383-x] [V]

#### Inferences
- **Stability is not justice.** Evolutionary stability, basin size and stochastic stability describe robustness and persistence. A discriminatory norm can be stable for very long times, so a tool that evolves allocation rules must never read "selected" as "fair".
- **What a simulation must represent.** The literature shows outcomes depend on population structure, tags and salience, memory, group sizes and correlation of interaction. An agent-based model of allocation rules over time (nomos's agents) must therefore make these explicit and run every declared population structure. A result is a map of where each rule leads under each structure.
- **Norms are local.** Henrich's results imply that acceptable division norms differ across communities. The parameters of an allocation rule (the floor, the weights, the sharing rule) must be declared and deliberated locally, not fixed by a universal "fair" constant.
- **Quality-diversity as an archive of options.** QD (heuresis) suits allocation because it returns an archive of high-performing allocations that differ along declared behavioural descriptors (for example who benefits first, how concentrated the flows are, how much circulates locally). That archive is a set of options for deliberation, consistent with treating allocation as non-optimisation. The AI Economist and Democratic AI show the risk of a single trained objective: optimising what a majority votes for is majoritarian.
- **Adaptive management as procedure over time.** It is the procedural counterpart: allocate, monitor, revise. It needs a declared monitoring design and a stopping and revision rule (policy domain, EVSI).

#### Gaps
- Primary-text check of the 62% figure and its discretisation; of Young 1993's link to the Nash bargaining solution; and of the details of Binmore's egalitarian/utilitarian argument.
- Empirical evidence that evolutionary or QD-designed allocation policies perform well in real budgets: none found.

### 5. Money and Circulation Determine Where an Allocation's Value Finally Accrues

#### Takeaway
Who receives money first, and how it re-circulates, shapes final incidence. It can be measured in three ways:
- by surveys of re-spending (LM3);
- by models (regional input–output and social accounting matrix multipliers, with structural path analysis tracing which paths carry an injection to which households);
- where transaction records exist, by network measures of circulation: directed cycles against null models, flow modules (Infomap), PageRank as long-run shares, ecological network analysis's cycling index.

Cantillon effects are theoretically clear, but the evidence for monetary policy is mixed: quantitative easing appears equalising through employment in the euro area and disequalising through asset prices in Japan. Complementary currencies localise circulation. Cycle participation is associated with better outcomes for firms in Sardex, and Sarafu circulated overwhelmingly within local communities. Evidence that they redistribute is thin.

#### Cited Findings

**Cantillon effects and monetary policy.**
- Cantillon 1755, Essai sur la nature du commerce en général: new money raises the incomes and prices first met by its first recipients [M].
- **Euro area.** Quantitative easing reduced income inequality, mainly by lowering unemployment among poorer households, and slightly reduced net-wealth inequality through house prices. — [Lenza & Slacalek 2018, ECB Working Paper 2190] [V] (SSRN record doi:10.2139/ssrn.3275976); search summary (https://ECB.eu/pub/pdf/scpwps/ecb.wp2190.en.pdf) [S]
- **Japan.** Unconventional monetary policy raised inequality through asset prices, especially after 2008. — [Saiki & Frost 2014, DNB Working Paper 423, published in Applied Economics] [V] (vol. 46(36):4445–4454, doi:10.1080/00036846.2014.962229); search summary [S]
- **Surveys.** Results depend on country, period, and whether income or wealth is measured (content from memory). — [Coibion, Gorodnichenko, Kueng & Silvia 2017, "Innocent bystanders? Monetary policy and inequality", Journal of Monetary Economics 88:70–89] [V] (doi:10.1016/j.jmoneco.2017.05.005); [Colciago, Samarina & de Haan 2019, "Central bank policies and income and wealth inequality: a survey", Journal of Economic Surveys 33(4):1199–1231] [V] (doi:10.1111/joes.12314)
- **The redistribution channel.** Differences in marginal propensities to consume and in unhedged interest-rate exposures (content from memory). — [Auclert 2019, "Monetary policy and the redistribution channel", American Economic Review 109(6):2333–2367] [S] (Crossref returned the NBER working paper record, doi:10.3386/w23451)

**Local multipliers and incidence.**
- Each additional tradable job creates about 1.6 local non-tradable jobs, 2.5 for high-tech jobs (figures from memory of the abstract). — [Moretti 2010, "Local multipliers", American Economic Review 100(2):373–377, doi:10.1257/aer.100.2.373] [V]
- A cross-sectional fiscal multiplier of about 1.8 (figure from memory). — [Chodorow-Reich 2019, "Geographic cross-sectional fiscal spending multipliers: what have we learned?", American Economic Journal: Economic Policy 11(2):1–34] [V] (doi:10.1257/pol.20160465) (Crossref returned NBER w23577)
- SAM multipliers [formula M]: with the endogenous accounts' coefficient matrix A_n, accounting multipliers are M_a = (I − A_n)^−1 for injections x. — [Pyatt & Round 1979, "Accounting and fixed price multipliers in a social accounting matrix framework", Economic Journal 89:850–873, doi:10.2307/2231503] [V]
- Structural path analysis [formula M]: the global influence of account i on j decomposes into a sum over elementary paths. Each path's direct influence (the product of its coefficients) is multiplied by the path multiplier from the loops adjacent to it. This shows which paths an injection travels to reach each household group. — [Defourny & Thorbecke 1984, "Structural path analysis and multiplier decomposition within a social accounting matrix framework", Economic Journal 94:111–136, doi:10.2307/2232220] [V]
- Fiscal and benefit incidence by household decile: [Lustig (ed.) 2018, Commitment to Equity Handbook, Brookings, doi:10.5040/9780815753834] [V] (content from memory).

**Circulation measured on transaction networks.**
- **Sarafu** (Kenya, Jan 2020–Jun 2021). Data: 421,329 transactions among 40,657 accounts, 293.7 million Sarafu. Methods and results (text read, arXiv HTML):
  - circulation was measured by Infomap's map-equation modules: 99.7% of volume lay within five top-level modules, four of them single localities, and 80% within 455 sub-sub-modules of ten or more accounts;
  - short directed cycles (lengths 2 and 3) were in excess over degree-preserving null models;
  - weighted PageRank (α = 0.85) was read as the share of funds an account would eventually control, correlating with final balances at R ≈ 0.57;
  - savings groups (chamas) were the most central accounts;
  - stated limits: the network is time-aggregated, a strong PageRank assumption, coarse geography, associational regressions. — [Mattsson, Criscione & Takes 2023, "Circulation of a digital community currency", Scientific Reports 13:5864, doi:10.1038/s41598-023-33184-1] [V]; arXiv:2207.08941 (https://ar5iv.labs.arxiv.org/html/2207.08941) [V]
- The dataset: [Mattsson, Criscione & Ruddick 2022, "Sarafu Community Inclusion Currency 2020–2021", Scientific Data 9:426, doi:10.1038/s41597-022-01539-4] [V].
- Evaluation of Eco-Pesa: [Ruddick 2016, International Journal of Community Currency Research] [M].
- **Sardex.** 1,477 businesses and 48,170 transactions in a mutual-credit system. Firms whose transactions lie in cycles have higher turnover and lower excess accumulation of currency or debt; the measures are "cyclic density" and "k-cycle centrality". — [Iosifidis, Charette, Airoldi, Littera, Tassiulas & Christakis 2018, "Cyclic motifs in the Sardex monetary network", Nature Human Behaviour 2:822–829, doi:10.1038/s41562-018-0450-0] [V]. Content from abstract and search summary.
- **Complementary currencies in general** (records verified; content from memory):
  - mapping community currencies: [Seyfang & Longhurst 2013, "Growing green money? Mapping community currencies for sustainable development", Ecological Economics 86:65–77, doi:10.1016/j.ecolecon.2012.11.003] [V];
  - a systematic review finding mixed and modest evidence of impact: [Michel & Hudon 2015, "Community currencies and sustainable development: a systematic review", Ecological Economics 116:160–171, doi:10.1016/j.ecolecon.2015.04.023] [V];
  - local currencies examined against degrowth goals: [Dittmer 2013, "Local currencies for purposive degrowth? A quality check of some proposals for changing money-as-usual", Journal of Cleaner Production 54:3–13, doi:10.1016/j.jclepro.2013.03.044] [V];
  - limited localising effect of the Bristol Pound: [Marshall & O'Neill 2018, "The Bristol Pound: a tool for localisation?", Ecological Economics 146:273–281, doi:10.1016/j.ecolecon.2017.11.002] [V];
  - [Gelleri 2009, "Chiemgauer regiomoney: theory and practice of a local currency", International Journal of Community Currency Research 13] [M].

**Cycling in ecological network analysis.**
- The Finn cycling index FCI = TST_c / TST [formula M]. Here TST_c = Σ_i ((n_ii − 1)/n_ii) T_i, n_ii is the diagonal of N = (I − G)^−1, and G_ij = T_ij / T_j is throughflow-normalised. It is the fraction of throughflow that returns to its compartment. — [Finn 1976, "Measures of ecosystem structure and function derived from analysis of flows", Journal of Theoretical Biology 56(2):363–380, doi:10.1016/S0022-5193(76)80080-X] [V]
- Network environ analysis, and an R implementation: [Fath & Patten 1999, Ecosystems 2:167–179] [V] (doi:10.1007/s100219900067); [Borrett & Lau 2014, "enaR: an R package for ecosystem network analysis", Methods in Ecology and Evolution 5(11):1206–1213] [V] (doi:10.1111/2041-210X.12282).
- Monetary "monoculture" as a cause of brittleness (text read). — [Goerner et al. 2009] [V]; [Lietaer, Ulanowicz, Goerner & McLaren 2010, "Is our monetary structure a systemic cause for financial instability?", Journal of Futures Studies] [M]

#### Inferences
- **Two incidences.** "To whom does an allocation finally accrue" splits into a first-round incidence (direct recipients, the Cantillon question) and a propagated incidence (SAM and IO multipliers with structural path analysis, or observed circulation in transaction data). Both are needed. They answer different questions, and model multipliers assume fixed coefficients, idle capacity and no price response.
- **Three measures, three different things.** LM3 measures local retention, not benefit to the poor. Moretti-type multipliers measure jobs, not who holds them. Network measures (cycles, modules, FCI) measure topology, not welfare.
- **Cycles and justice.** In Sardex and Sarafu, cycles are associated with system health and member outcomes. This is the strongest available evidence that "circulation back" (generative justice's loops) is measurable and meaningful. It is associational, and it speaks to monetary circulation, not to generative justice's unalienated value.
- **The Cantillon evidence is mixed.** A tool should present first-recipient effects as a channel to be traced in a given case, not as a law.

#### Gaps
- No study found that measures generative justice's "return to generators" on real transaction data.
- No primary text read on how the distributive effects of complementary currencies differ by income group.
- Primary check of Moretti's and Chodorow-Reich's numbers (from memory).

### 6. These Ideas Become Computable as Flow Accounting, Threshold Rules and Archives of Evolving Policies

#### Takeaway
Five families of computable parts emerge:
- (a) value-flow accounting with a ledger per kind of value and generator labels, to measure extraction against return;
- (b) circulation and incidence measures: LM3, IO/SAM multipliers and structural paths, cycle and module statistics, the cycling index, ascendency against overhead;
- (c) threshold allocation, a need floor and an ecological ceiling, with rules that switch under scarcity;
- (d) agent-based and evolutionary models of allocation rules over time, with population structure explicit;
- (e) archives of diverse allocation policies judged on several criteria (quality-diversity), feeding deliberation and adaptive revision.

Each needs specific data, carries assumptions, and is answerable to the critiques above.

#### Cited Findings (formulas, data, assumptions; sources as in §§1–5)
- **(a) Value-flow accounting.** Directed, labelled flows T_ij^(k) for each kind k ∈ {labour, ecological, expressive}, with node attributes for generator status and community. Generative justice supplies the categories and the loop principle (Eglash 2016 [V]; Robinson et al. 2025 [V]). The return ratio and extraction share in §1 are proposals.
  - Data: payrolls and ownership, procurement records, MFA and EEIO for material flows (industrial ecology), participation records for expressive value.
  - Assumptions: flows can be attributed to generators; "unalienated" is declared case by case.
  - Critique: Eglash's own (organicism, co-optation).
- **(b) Circulation and incidence.**
  - LM3 = (R1 + R2 + R3)/R1 (Sacks 2002 [S]). Data: surveys of suppliers and staff spending. Assumption: three rounds suffice. Critique: no counterfactual, ignores displacement.
  - Leontief x = (I − A)^−1 f and SAM M_a = (I − A_n)^−1 (Pyatt & Round 1979 [V]; formula M) with structural path analysis (Defourny & Thorbecke 1984 [V]). Data: a regional SAM with household groups. Assumptions: fixed coefficients, excess capacity.
  - Network circulation on transaction records: cycle counts against degree-preserving null models; Infomap retention of volume within modules; PageRank long-run shares (Mattsson et al. 2023 [V]); cyclic density and k-cycle centrality (Iosifidis et al. 2018 [V]). Data: digital ledgers, privacy-restricted.
  - Ecological network analysis on any flow table (methods review: [Ulanowicz 2004, "Quantitative methods for ecological network analysis", Computational Biology and Chemistry 28:321–339, doi:10.1016/j.compbiolchem.2004.09.001] [V]): FCI (Finn 1976 [V]; formula M); A, Φ and C (Goerner et al. 2009 [V], formulas from text); robustness −a log a (Ulanowicz et al. 2009; formula M). Critique: the claimed optimum is not established for economies.
- **(c) Threshold allocation.**
  - A floor per need dimension: Doyal & Gough 1991 [V]; Rao & Min 2018 [V]; Max-Neef 1991 [S] (record via a book-summary chapter, doi:10.9774/gleaf.978-1-907643-44-6_18); Raworth's social foundation (Fanning & Raworth 2025 [V]).
  - A ceiling per ecological dimension: per-capita downscaled boundaries (O'Neill et al. 2018 [V]; Fanning & Raworth 2025 [V]) with fair shares (Hickel et al. 2022 [V]).
  - Switching rules under scarcity: acequias, from Cox et al. 2010 [V].
  - Points-based need weighting: Porto Alegre [S].
  - Data: indicators per unit and dimension with declared thresholds.
  - Critique: thresholds are normative; downscaling is contested; some dimensions are unquantified.
- **(d) Agent-based and evolutionary models of rules.**
  - Replicator and adaptive-play dynamics of division norms (Skyrms [V]; Young 1993 [V]); tagged populations (Axtell, Epstein & Young 2001 [V]; O'Connor 2019 [V]; Bruner 2019 [V]); routines (Nelson & Winter [S]); macro agent-based models (Dosi et al. 2010 [V]).
  - Data: population structure, group sizes, interaction networks, memory. Calibration to experiments (Henrich et al. 2001/2005/2010 [V]) is possible, but norms vary by society.
  - Critique: stability is not justice; outcomes are sensitive to assumptions.
- **(e) Archives of diverse policies.**
  - QD archives (Pugh et al. 2016 [V]; Mouret & Clune 2015 [S]) keep the best allocation of each kind on descriptors.
  - Machine-designed policy (Zheng et al. 2022 [V]; Koster et al. 2022 [V]) illustrates single-objective and majoritarian risks.
  - Adaptive revision: Campbell 1969 [V]; Holling 1978 [S]; Sabel & Zeitlin 2012 [V].

#### Inferences
- **Hodge decomposition as a reading of circulation.** Generative justice's "circulation" has an exact counterpart in the decomposition of a flow on a network into a gradient part (driven, one-way: source to sink, the topology of extraction), a curl part (local eddies round faces) and a harmonic part (global circulation round holes). The fraction of flow that is non-gradient is a candidate "circulation share". It can be computed by kind of value and restricted to cycles through generator nodes.
  - Caveat: a cycle can still be extractive (a debt cycle), so the decomposition must be read with labels of control and of kind, never alone.
  - This is an inference linking the infrastructure's existing decomposition to generative justice. The generative-justice literature does not propose it.
- **Two cycling measures to compare.** The Finn cycling index and a Hodge circulation share are different measures of "return". A tool should compute both and say how they differ:
  - FCI is defined on throughflow-normalised Markov transitions;
  - Hodge is a least-squares orthogonal decomposition with declared edge weights.

#### Gaps
- No published operational metric of "unalienated value" exists to adopt; the return ratio and extraction share are proposals.
- No study found that combines threshold allocation with circulation metrics in a single allocation tool.

### 7. What This Means for the Design

#### Takeaway
These traditions do not supply an allocation optimum. They supply five things:
- a principle of where value should go: back to its generators, through loops;
- a shape for the rule: floor and ceiling, with switching under scarcity;
- procedural conditions for rules to be legitimate and durable: commons principles and participatory budgeting;
- a way to study rules over time: evolution, agent-based models, adaptive management;
- a way to keep many good options rather than one: quality-diversity.

Several already have homes in the infrastructure. The tool should compute incidence, circulation, shortfall and overshoot, and the long-run behaviour of rules, and present them as evidence for distributive and procedural deliberation. It must never certify an allocation as just.

#### Cited Findings
- Generative justice gives the principle and admits it lacks an operational definition (Eglash 2016 [V], pp. 382, 395–396).
- The social dimension is a minority concern in circular-economy definitions (Kirchherr et al. 2023 [V]).
- The doughnut is an annual threshold monitor (Fanning & Raworth 2025 [V]).
- 2B is strongly supported, and equity perceptions shift with scarcity (Cox et al. 2010 [V]).
- Discriminatory norms are metastable (Axtell, Epstein & Young 2001 [V]).
- Fairness norms vary across societies (Henrich et al. 2001 [V]).
- Circulation is measurable on ledgers (Mattsson et al. 2023 [V]; Iosifidis et al. 2018 [V]).
- Cantillon evidence is mixed (§5, [S]).
- The CWB evidence is promotional, and the protectionism critique applies (§2, [S]).

#### Inferences

**Computable parts, their homes, and their data.**
1. **Value-flow accounting by generator and kind of value.** Home: the economics domain's transports and circulation; industrial ecology's MFA, EEIO and footprints for the ecological ledger; archeion's provenance for "who generated what".
   - New part needed: generator labels and control labels on flows; the return ratio and extraction share (§1) as declared, revisable definitions; three ledgers never summed without a declared commensuration.
   - Data: procurement and payroll records, ownership, MFA tables, participation records.
2. **Circulation and incidence.** Home: economics' decomposition into driven flow, local eddies and global circulation as the "circulation share"; markets' money for first-recipient (Cantillon) incidence and the order of steps; economics' input–output for propagated incidence.
   - New parts: SAM multipliers with structural path analysis by household group; LM3 as a survey-based estimate with its limits stated; cycle-against-null and Infomap-retention statistics for transaction ledgers; Finn's cycling index and ascendency against overhead as structural descriptors, explicitly not welfare measures.
3. **Threshold allocation.** Floors per need dimension (the needs domain's declared standards; Doyal–Gough and Max-Neef dimensions as data); ceilings per ecological dimension (industrial ecology's footprints against declared downscaled boundaries, with fair-share options).
   - Rules: lexicographic minimisation of shortfall subject to the ceiling; rules that switch with scarcity (proportional in plenty, survival first in scarcity); points-based need weighting after Porto Alegre.
   - Every threshold carries its source and is declared, never built in.
4. **Rules over time.** Home: nomos's agents (agent-based allocation, tagged populations, adaptive play); kinesis for what a declared law of allocation does over time; the policy domain's adoption and EVSI for adaptive revision.
   - Every run declares its population structure, group sizes, memory and interaction network. Results are reported as basins and persistence times (as Axtell–Epstein–Young do), never as "the fair rule".
5. **An archive of diverse allocations.** Home: heuresis's quality-diversity archive, which keeps the best allocation of each kind on descriptors such as who is reached first, concentration of flows (ascendency), circulation share, shortfall left, overshoot, and proportional equivalence by group.
   - The archive is input to collective choice (commons principle 3), not a decision.
   - Single-objective learned policies (the AI Economist; Democratic AI's majority objective) are shown only as contrasts.

**How these combine with distributive and procedural justice.**
- *Distributive.* The floor implements sufficiency. Priority to the worst-off or leximin can break ties among allocations that meet the floor. Generative justice adds a directional preference: to generators, through loops. Proportional equivalence (2B) adds desert by contribution, overridden by need in scarcity (the acequia switch). These are distinct principles. A tool should show where they agree and conflict, never fold them into one score.
- *Procedural.* Ostrom's collective-choice, monitoring, graduated-sanction, conflict-resolution and nesting principles, and participatory budgeting's deliberated weights, govern who sets thresholds, weights and rules and how they are revised. The tool supports that procedure (records, alternatives, consequences) and does not replace it. Generative justice's own principle "the rights of value generators to create their own conditions of production" is procedural too: it requires generators to take part in setting the rules.

**What a tool must never claim.**
- That any allocation is just, optimal or "the" fair one. Evolutionary stability, quality-diversity elitism and multiplier size are not justice.
- That a circulation, cycling or retention measure (LM3, FCI, Hodge share, cycle density) measures unalienated value, welfare, or benefit to generators. These measure topology or retention. Cycles can be extractive.
- That local retention is good in itself. Community wealth building's protectionism and siloing critique and justice between places apply.
- That LM3 or model multipliers are counterfactual impacts. They omit displacement and price responses, and model multipliers assume fixed coefficients.
- That thresholds (social floors, downscaled boundaries) are scientific facts. They are declared, contested and partly unquantified.
- That fairness norms are universal. Henrich et al. show variation; parameters are local and deliberated.
- That the Cantillon effect is a law. The evidence is mixed by country and channel.
- That the "window of vitality" optimum is known for economies. Goerner et al. concede the data cannot fix it.
- That three kinds of value (labour, ecological, expressive) can be summed without a declared, contestable commensuration.
- That a model's or a language model's proposal of an allocation is evidence. It is a proposal, to be read like any other.

#### Gaps
- How generative justice's "conditions of production" right should constrain an allocation tool's governance (who may change the rules) is not specified in the literature read. It must be decided with the user.
- No literature was found that combines all five parts. The combination proposed here is design, not established practice.

## Part V. Allocation in Practice, Procedural Safeguards, Tools and Deliberation with Language Models

*Remark.* Part V's scope and conventions. Scope: priority setting in health and in crises; the allocation of public money in practice (participatory budgeting, equity and gender budgeting, research funding, social housing, humanitarian targeting, universal and targeted designs); procedural safeguards and the failures that show their need; decision-support tools and their licences; language models in deliberation and allocation; and the evaluation of allocation processes and outcomes. The bibliographic markers are those of §0.3; records were verified against the Crossref API, Europe PMC, the arXiv API or the provider's own page. Statements of content say where they come from: (content: abstract), (content: page), (content: search summary) or (content: memory). Search summaries were produced by a search tool from snippets and were not read in full; treat their numbers as [S]-grade until the primary document is opened. Inside this Part, a bare section reference means this Part's own section.

### 1. Priority Setting in Health and in Crises

#### Takeaway
Health systems that ration openly combine a few named criteria (benefit, cost or opportunity cost, severity or need) with a procedure that publishes reasons and allows appeal; every national scheme examined settles the trade-off between maximising and giving priority to the worse off by an explicit weight, a lexical order, a reserve of the supply, or a lottery among near-equals, and every one of these has been contested, revised, or overridden by politics, courts or circumvention, so a tool must record which combination rule was used and why, not only the criteria.

#### Cited Findings

**1.1 Cost-effectiveness thresholds (NICE) and their politics**
- NICE's standard range was £20,000–£30,000 per QALY until April 2026; the UK government announced on 1 December 2025 that it would rise to £25,000–£35,000 per QALY, effective April 2026, for standard technology appraisals (not highly specialised technologies); the change required new regulations giving the Secretary of State power to direct NICE on thresholds; NICE expected 3–5 additional medicines or indications per year (content: search summary of NICE pages) — [NICE news: changes confirmed](https://www.nice.org.uk/news/articles/changes-to-nice-s-cost-effectiveness-thresholds-confirmed) [S]; [NICE FAQ](https://www.nice.org.uk/what-nice-does/faqs/changes-to-nice-s-cost-effectiveness-thresholds) [S].
- An editorial in *Applied Health Economics and Health Policy* argued the rise was not evidence-based, since the marginal cost of producing a QALY in the NHS is estimated at about £13,000 (content: search summary) — [White Rose eprint s40258-025-01026-y](https://eprints.whiterose.ac.uk/id/eprint/236400/1/s40258-025-01026-y.pdf) [S]. The underlying estimate is Claxton et al. 2015, *Health Technology Assessment* 19(14), doi:10.3310/hta19140 [V]; its central figure of about £12,936 per QALY is (content: memory).
- *Remark.* The threshold is the opportunity-cost argument made explicit: raising it above the estimated marginal productivity of the NHS means, on that argument, that each approval displaces more health than it adds. That a threshold moved by ministerial direction in the context of trade and industry talks (the trade context is (content: memory)) is itself a datum about where the "technical" parameter sits politically.
- NICE replaced its end-of-life modifier (weight about 1.7 for life expectancy under 24 months with at least 3 months' gain) with a severity modifier in its January 2022 manual: QALY weights of 1.2 or 1.7 by absolute or proportional QALY shortfall, calibrated to be "opportunity cost neutral"; first-year data (57 topics) showed fewer topics qualifying than anticipated, most at 1.2, oncology and acute conditions more likely to qualify (content: search summary of NICE DSU and ISPOR abstracts) — [NICE DSU severity weights](https://sheffield.ac.uk/nice-dsu/methods-development/severity-weights) [S]; [ISPOR 2024 abstract](https://www.ispor.org/heor-resources/presentations-database/presentation/euro2024-4016/143749) [S].

**1.2 Equity weights and distributional cost-effectiveness analysis**
- Cookson, Mirelman, Griffin, Asaria, Dawkins, Norheim, Verguet, Culyer 2017, "Using Cost-Effectiveness Analysis to Address Health Equity Concerns", *Value in Health* 20(2), doi:10.1016/j.jval.2016.11.027 [V]. Content (memory): classifies approaches — equity impact analysis (who gains, who loses, by social group) and equity trade-off analysis (how much total health would be given up for a given reduction in inequality), the latter via equity weights or inequality-aversion parameters (Atkinson, Kolm); recommends reporting both rather than a single weighted number.
- Asaria, Griffin, Cookson, "Distributional Cost-Effectiveness Analysis: A Tutorial", *Medical Decision Making* 36(1), 2016 (online 2015), doi:10.1177/0272989X15583266 [V]. Content (memory): models the distribution of health opportunity costs as well as gains across socioeconomic groups; plots interventions on an "equity–efficiency impact plane"; dominance rankings and social welfare indices with explicit inequality aversion.
- Cookson, Griffin, Norheim, Culyer (eds.), *Distributional Cost-Effectiveness Analysis: Quantifying Health Equity Impacts and Trade-Offs*, Oxford University Press 2020 — record verified through its chapters, e.g. doi:10.1093/med/9780198838197.003.0003 [V].
- GPS-Health: Norheim et al. 2014, "Guidance on priority setting in health care (GPS-Health): the inclusion of equity criteria not captured by cost-effectiveness analysis", *Cost Effectiveness and Resource Allocation* 12:18, doi:10.1186/1478-7547-12-18 [V]. Content (memory): three groups of criteria — disease-related (severity of current state, realisation of potential, past health loss), characteristics of social groups (socioeconomic status, area of living, gender, race/ethnicity/religion/sexual orientation), non-health consequences (economic productivity, care for others, catastrophic expenditure); each with a recommendation for or against inclusion.
- Severity as a criterion remains conceptually unsettled (absolute versus proportional shortfall, "fair innings", current versus lifetime): Barra et al. 2019/2020, "Severity as a Priority Setting Criterion: Setting a Challenging Research Agenda", *Health Care Analysis*, doi:10.1007/s10728-019-00371-z [V] (content: title; detail memory). Nord 2013, "Priority to the Worse Off", in *Inequalities in Health* (OUP), doi:10.1093/acprof:oso/9780199931392.003.0006 [V].

**1.3 National commissions and their criteria**
- *Norway.* The Storting has approved three criteria: health benefit, resources, and severity (absolute QALY shortfall) (content: search summary) — [Meld. St. 34 (2015–2016), *Principles for priority setting in health care*](https://www.regjeringen.no/en/dokumenter/meld.-st.-34-20152016/id2502758) [S]. The commission behind it: Ottersen, Førde, Kakad, Kjellevold, Melberg, Moen, Ringard, Norheim 2016, "A new proposal for priority setting in Norway: Open and fair", *Health Policy* 120(3), doi:10.1016/j.healthpol.2016.01.012 [V] (the Norheim commission, NOU 2014:12 *Åpent og rettferdig*, (content: memory)). The Magnussen working group (2015) operationalised severity; a later committee (the Blankholm committee, NOU 2018:16, name (content: memory)) recommended the same three criteria for municipal health and care services, widened to physical, psychological and social mastery (content: search summary) — [NOU 2018:16 English PDF](https://cms12.regjeringen.no/contentassets/013aba8272d3412794ff7c78deb83e77/en-gb/pdfs/nou201820180016000engpdfs.pdf) [S]. Severity-graded willingness-to-pay ranges (roughly NOK 275,000 to 825,000 per QALY by severity class) are (content: memory).
- *Sweden.* The ethical platform (Parliament 1997, after SOU 1995:5 *Vårdens svåra val*) ranks three principles in fixed order: human dignity, need and solidarity, cost-effectiveness; cost-effectiveness to be applied mainly when comparing treatments for the same condition; the National Board of Health and Welfare's national guidelines (18 by 2021) operationalise it, but studies find the processes opaque and the principles applied variably across levels (content: search summary of LiU and IJHPM pages) — [Swedish National Centre for Priorities in Health](https://liu.se/en/research/swedish-national-centre-for-priorities-in-health) [S]; [IJHPM article 4020](https://www.ijhpm.com/article_4020.html) [S]. Sweden's four priority groups (I–IV) are (content: memory).
- *The Netherlands.* The Dunning committee (*Kiezen en delen*, 1991) proposed four successive "sieves": necessary care (from a community perspective), effectiveness, efficiency, and individual responsibility (content: memory) [M]. The National Health Care Institute (Zorginstituut) now assesses the basic package by necessity, effectiveness, cost-effectiveness and feasibility, with reference values of roughly €20,000, €50,000 and €80,000 per QALY rising with burden of disease measured as proportional shortfall (content: memory) [M]. No primary record was retrieved this session.
- Policy makers in Norway, in a discrete-choice study of 34 decision makers, weighted cost-effectiveness, large individual benefits and severity most (content: search summary) — [*BMC Health Services Research* 12:39, 2012, doi:10.1186/1472-6963-12-39](https://link.springer.com/doi/10.1186/1472-6963-12-39) [S] (first author Defechereux, from memory).
- Rumbold et al. 2017, "Universal health coverage, priority setting, and the human right to health", *Lancet* 390, doi:10.1016/S0140-6736(17)30931-5 [V] (content: memory: argues explicit, fair priority setting is compatible with and required by the right to health, against litigation that bypasses priority setting, e.g. in Latin American right-to-health litigation).

**1.4 Accountability for reasonableness (A4R) and its evaluation**
- Daniels & Sabin 1997, "Limits to Health Care: Fair Procedures, Democratic Deliberation, and the Legitimacy Problem for Insurers", *Philosophy & Public Affairs* 26(4), doi:10.1111/j.1088-4963.1997.tb00082.x [V]; Daniels 2000, "Accountability for reasonableness", *BMJ* 321:1300, doi:10.1136/bmj.321.7272.1300 [V]; update 2008, doi:10.1136/bmj.a1850 [V]. Content (memory): four conditions — publicity (decisions and their rationales public), relevance (rationales rest on reasons that fair-minded people can accept as relevant), revision and appeals (mechanisms to challenge and revise in light of new evidence or arguments), regulation/enforcement (voluntary or public regulation to ensure the other three). Its premise is that reasonable people disagree on substantive principles, so legitimacy must come from procedure.
- Evaluation frameworks: Gibson, Martin, Singer 2004, *BMC Health Services Research* 4:25, doi:10.1186/1472-6963-4-25 [V]; Sibbald, Singer, Upshur, Martin 2009, "Priority setting: what constitutes success?", *BMC HSR* 9:43, doi:10.1186/1472-6963-9-43 [V], and its pilot, doi:10.1186/1472-6963-10-131 [V] (content: memory: ten elements — five procedural: stakeholder engagement, explicit process, information management, consideration of values and context, revision or appeals; five outcome: improved stakeholder understanding, shifted priorities or reallocation, improved decision quality, stakeholder acceptance and satisfaction, positive externalities); Kapiriri & Martin 2010 (online 2009), "Successful priority setting in low and middle income countries: a framework for evaluation", *Health Care Analysis*, doi:10.1007/s10728-009-0115-2 [V]; Barasa, Molyneux, English, Cleary 2015, "Setting Healthcare Priorities at the Macro and Meso Levels: A Framework for Evaluation", *IJHPM* 4(11), doi:10.15171/ijhpm.2015.167 [V].
- Decision makers' views of fairness differ across systems and levels: Kapiriri, Norheim, Martin 2009, *Social Science & Medicine* 68(4), doi:10.1016/j.socscimed.2008.11.011 [V]; Martin, Giacomini, Singer 2002, *Health Policy*, doi:10.1016/S0168-8510(01)00237-8 [V].
- Critiques (content: memory) [M]: A4R says little about which reasons are "relevant" and so can legitimate exclusionary decisions; "relevance" smuggles substantive judgement (Friedman 2008, *Bioethics*); appeals in practice are rarely used and rarely change decisions; the REACT project in Tanzania, Kenya and Zambia found A4R raised transparency and stakeholder involvement at district level but was constrained by central budget control and earmarked donor funds (Maluka et al. 2011, *Implementation Science*). None of these records was retrieved this session.
- Evidence-informed deliberative processes as the HTA-agency form of A4R: Baltussen, Jansen, Bijlmakers et al. 2017, "Value Assessment Frameworks for HTA Agencies: The Organization of Evidence-Informed Deliberative Processes", *Value in Health* 20(2), doi:10.1016/j.jval.2016.11.019 [V] (content: memory: stakeholder involvement, evidence on multiple criteria, deliberation, and transparency, as a practical bridge between MCDA and A4R).

**1.5 Triage and the allocation of ventilators, vaccines and antivirals in COVID-19**
- Emanuel, Persad, Upshur, Thome, Parker, Glickman, Zhang, Boyle, Smith, Phillips 2020, "Fair Allocation of Scarce Medical Resources in the Time of Covid-19", *NEJM* 382:2049, doi:10.1056/NEJMsb2005114 [V]. Content (memory): four values (maximise benefits, treat people equally, promote and reward instrumental value, give priority to the worst off) and six recommendations — maximise benefits (lives and life-years saved) as the overriding value; prioritise health workers; no first-come first-served; respond to evidence; recognise research participation; same principles for COVID and non-COVID patients; random selection among patients with similar prognosis. The earlier synthesis is Persad, Wertheimer, Emanuel 2009, "Principles for allocation of scarce medical interventions", *Lancet* 373:423, doi:10.1016/S0140-6736(09)60137-9 [V] (the "complete lives system", content memory), criticised for age-weighting.
- White & Lo 2020, "A Framework for Rationing Ventilators and Critical Care Beds During the COVID-19 Pandemic", *JAMA* 323(18), doi:10.1001/jama.2020.5046 [V] (content: memory: a multiprinciple point system combining SOFA-based short-term survival with longer-term prognosis, with a triage officer and team separate from treating clinicians, and an appeals process).
- SOFA-based crisis standards disadvantage Black patients: Miller, Han, Peek, Ashana, Parker 2021, *JAMA Network Open* 4(6):e2113891, doi:10.1001/jamanetworkopen.2021.13891 [V] (content: abstract: eICU data from 233 US ICUs, 2014–2015, tested whether Black and White patients with equal SOFA have equal in-hospital mortality under three states' SOFA-based rules; conclusion that SOFA overestimates Black patients' mortality is (content: memory)); Ashana et al. 2021, "Equitably Allocating Resources during Crises: Racial Differences in Mortality Prediction Models", *AJRCCM* 204(2), doi:10.1164/rccm.202012-4383OC [V], with editorial doi:10.1164/rccm.202103-0809ED [V].
- Disability rights: several state crisis-standards protocols were withdrawn or revised after complaints to the HHS Office for Civil Rights (Alabama, Pennsylvania, Utah, Tennessee and others) about categorical exclusions and long-term survival criteria (content: memory) [M].
- Vaccines: Persad, Peek, Emanuel 2020, "Fairly Prioritizing Groups for Access to COVID-19 Vaccines", *JAMA* 324(16), doi:10.1001/jama.2020.18513 [V]. Schmidt, Weintraub, Williams et al. 2021, "Equitable allocation of COVID-19 vaccines in the United States", *Nature Medicine* 27, doi:10.1038/s41591-021-01379-6 [V] (content: abstract: by 30 March 2021, 37 of 64 CDC jurisdictions had adopted disadvantage indices, up from 19 in November 2020; 14 prioritised zip codes; five uses — larger shares for disadvantaged groups, defining priority groups or areas, outreach, siting, monitoring).
- WHO SAGE values framework (14 September 2020): overarching goal that vaccines "contribute significantly to the equitable protection and promotion of human well-being among all people of the world"; six principles (human well-being, equal respect, global equity, national equity, reciprocity, legitimacy — names (content: memory)), each with two or three objectives, including transparent processes based on shared values, best evidence and input from affected parties; the drafters chose "human well-being" over "human health" (content: search summary) — [WHO SAGE values framework](https://www.who.int/publications/i/item/who-sage-values-framework-for-the-allocation-and-prioritization-of-covid-19-vaccination) [S]; [Nature comment by an author](https://www.nature.com/articles/d41586-022-01899-2) [S].
- Reserve (categorised priority) systems: Pathak, Sönmez, Ünver, Yenmez, "Fair Allocation of Vaccines, Ventilators and Antiviral Treatments: Leaving No Ethical Value Behind in Health Care Rationing", ACM EC 2021, doi:10.1145/3465456.3467604 [V], and *Management Science* 2024, doi:10.1287/mnsc.2022.00930 [V], arXiv 2008.00374 [V] (content: abstract: a single priority order forces all values onto one ranking; reserves let different categories of units use different orders). Sönmez, Pathak, Ünver, Persad, Truog, White 2021, "Categorized Priority Systems", *Chest* 159(3), doi:10.1016/j.chest.2020.12.019 [V].
- Implementations (content: search summary): Tennessee distributed 85% statewide, 10% to disadvantaged areas by the CDC Social Vulnerability Index, 5% to hotspots; Massachusetts guidance reserved 20% of vaccines for communities with disproportionate burden and high vulnerability, and 20% of monoclonal antibodies for patients from disadvantaged areas — [MIT News 2021](https://news.mit.edu/2021/building-equity-vaccine-distribution-0204) [S]; [Boston College news](https://www.bc.edu/content/bc-web/bcnews/campus-community/faculty/covid-vaccine-allocation-system.html) [S]. The researchers' claim that reserves had never been used for medical rationing before is their own, unverified.
- Weighted lotteries for scarce COVID-19 medications were developed and implemented (Pittsburgh/UPMC remdesivir and monoclonal lotteries): White et al. 2021, *AJRCCM* conference abstract A1013, doi:10.1164/ajrccm-conference.2021.203.1_MeetingAbstracts.A1013 [V] (content: title only).

**1.6 Organ allocation: scores, waiting time, urgency, geography**
- US kidney allocation system (KAS, December 2014): Israni et al. 2014, "New National Allocation Policy for Deceased Donor Kidneys in the United States and Possible Effect on Patient Outcomes", *JASN* 25(8), doi:10.1681/ASN.2013070784 [V]. Content (memory): longevity matching of the top 20% of kidneys by KDPI to the top 20% of candidates by EPTS; waiting time counted from dialysis start rather than listing; priority for highly sensitised candidates; widely reported rises in transplant rates for Black and Hispanic candidates afterwards (Melanson et al. 2017, *CJASN*, content memory [M]).
- Race-based eGFR: since 5 January 2023 kidney programmes must identify Black candidates whose waiting time was delayed by race-inclusive eGFR and restore it; six months in, 6,103 modifications had been processed, median 1.7 years added, 491 recipients of deceased-donor and 15 of living-donor kidneys among them; only 12 of 232 programmes had then attested (content: search summary of HRSA pages) — [HRSA early monitoring](https://www.hrsa.gov/optn/news-events/news/early-monitoring-report-shows-black-kidney-candidates-are-receiving-waiting-time-modifications-after-implementation-of-new-policies/) [S]. A *JAMA Internal Medicine* paper (9 March 2026; PMID 41801177) associated the restoration with a short-term increase in transplant rates among Black candidates (content: search summary) [S].
- Liver: MELD ranks by 90-day mortality risk (urgency); creatinine-based MELD under-estimates severity in women by up to 2.4 points, and height and exception points explained most of the sex disparity — Allen, Heimbach, Larson, Mara, Kim, Kamath, Therneau 2018, *Transplantation* 102(10), doi:10.1097/TP.0000000000002196 [V] (content: abstract; among 90,720 registrants women had higher 4-year waitlist mortality, 22% versus 18%). MELD 3.0 (2023) added points for female sex (content: memory) [M].
- Geography: Snyder et al. 2018, "Organ distribution without geographic boundaries: A possible framework for organ allocation", *AJT* 18(11), doi:10.1111/ajt.15115 [V]. The OPTN board replaced donation service areas with "acuity circles" for livers (December 2018); transplant centres and patients sued in April 2019 (N.D. Georgia), an injunction was briefly granted pending appeal, then denied on 16 January 2020, and the policy took effect 4 February 2020 (content: search summary) — [OPTN liver policy page](https://optn.transplant.hrsa.gov/governance/policy-initiatives/liver) [S]; [VUMC news 2019](https://news.vumc.org/2019/04/23/transplant-centers-patients-unite-to-stop-new-organ-sharing-policy-that-threatens-longer-waits-for-a-liver/) [S]. The dispute was over whether "need" (sickest first, nationally) or local access and organ quality should govern, with large and small centres on opposite sides.
- "Continuous distribution" replaces classification-based sorting with a composite score of weighted attributes (medical urgency, post-transplant survival, candidate biology, patient access, placement efficiency); lung went first (implemented March 2023, date (content: memory)), with kidney, pancreas, heart and liver committees following (content: search summary) — [UNOS policy development](https://unos.org/?p=292368) [S]. The attribute weights were set partly by structured stakeholder elicitation (analytic hierarchy process) (content: memory) [M].
- Circumvention: reports in 2024–2025 that a large and rising share of US deceased-donor kidneys were allocated "out of sequence" (outside the match run, to expedite placement), concentrated at some centres, undermining the published rules (content: memory) [M]; no primary record retrieved.
- Kidney exchange: Freedman, Schaich Borg, Sinnott-Armstrong, Dickerson, Conitzer 2020, "Adapting a kidney exchange algorithm to align with human values", *Artificial Intelligence*, doi:10.1016/j.artint.2020.103261 [V] (content: memory: elicited lay judgements on which patient profiles to prioritise as tie-breakers and built them into the matching objective).

#### Inferences
- Five combination rules recur: (a) a weighted sum or weighted QALY (NICE severity weights, DCEA equity weights, continuous distribution's composite score); (b) a lexical order (Sweden's ranked principles; Dunning's sieves); (c) reserves that give different categories different orders (COVID reserve systems); (d) lotteries among near-equals (Emanuel et al.'s random selection; weighted lotteries for antivirals); (e) points with appeal to a triage committee (White & Lo). A tool should support all five as declared schemes, and should show for each decision which rule produced it, since the same criteria produce different winners under different rules.
- The recurrent failures are not missing criteria but (i) proxies that encode injustice (SOFA, creatinine-based MELD, race-based eGFR), (ii) parameters moved for reasons outside the declared framework (the 2025 NICE threshold), (iii) rules circumvented by discretion (out-of-sequence organ offers), and (iv) categorical exclusions found discriminatory (disability complaints). Each is detectable only if the tool keeps the proxy, the parameter's provenance and every deviation from the rule on the record.
- Retroactive correction is possible and has been done (eGFR waiting-time restoration): a tool should keep enough history to recompute who would have been served under a corrected rule.

#### Gaps
- Primary texts of the Dutch Dunning report and the Zorginstituut package criteria were not retrieved; their content above is from memory.
- No systematic evaluation was found of whether A4R implementations changed allocations, as opposed to perceived fairness; the evaluation literature mostly reports process measures.
- The 2024–2025 out-of-sequence organ allocation figures and any primary source for them were not retrieved.
- Whether NICE's DCEA guidance has been used in a decision that turned on distributional results was not established.

### 2. Allocation of Public Money in Practice

#### Takeaway
Participatory budgeting, partial lotteries in research funding, points and choice in social housing, needs-based humanitarian allocation and the design of transfers each show that the decisive questions are who is in the pool, which rule aggregates claims, how errors of exclusion are counted, and whether those excluded can find out and contest; the evidence on outcomes is mixed and often weak, and the best-documented failures are exclusion errors that go unrecorded.

#### Cited Findings

**2.1 Participatory budgeting (PB)**
- Origins and spread: Porto Alegre 1989; thousands of municipalities since (the *Participatory Budgeting World Atlas* counts over 11,000 cases, content: memory [M]). Cabannes 2004, "Participatory budgeting: a significant contribution to participatory democracy", *Environment and Urbanization*, doi:10.1177/095624780401600104 [V]; Wampler, McNulty, Touchton 2021, *Participatory Budgeting in Global Perspective* (OUP), doi:10.1093/oso/9780192897756.001.0001 [V].
- Outcomes in Brazil: Gonçalves 2014, "The Effects of Participatory Budgeting on Municipal Expenditures and Infant Mortality in Brazil", *World Development* 53, doi:10.1016/j.worlddev.2013.01.009 [V] (content: memory: PB municipalities shifted spending toward sanitation and health and had lower infant mortality); Touchton & Wampler 2014 (online 2013), "Improving Social Well-Being Through New Democratic Institutions", *Comparative Political Studies* 47(10), doi:10.1177/0010414013512601 [V] (content: memory: PB associated with higher health spending, more civil society organisations and lower infant mortality, larger the longer PB had run); Boulding & Wampler 2010, "Voice, votes, and resources", *World Development*, doi:10.1016/j.worlddev.2009.05.002 [V] (content: memory: little effect on well-being indicators, a counterweight to the positive studies).
- Outcomes in New York City: Hagelskamp, Silliman, Godfrey, Schleifer 2020, "Shifting Priorities: Participatory Budgeting in New York City is Associated with Increased Investments in Schools, Street and Traffic Improvements, and Public Housing", *New Political Science*, doi:10.1080/07393148.2020.1773689 [V] (content: title; detail memory). A follow-up asks who benefits when PB funds schools: Hagelskamp et al. 2023, "When Participatory Budgeting (PB) funds schools, who benefits? An equity analysis of PB-associated investments in NYC public schools", *Local Development & Society*, doi:10.1080/26883597.2023.2224950 [V] (content: title only).
- Participation effects: Johnson, Carlson, Reynolds 2023, "Testing the participation hypothesis: evidence from participatory budgeting", *Political Behavior*, doi:10.1007/s11109-021-09679-w [V] (online 2021) (content: memory: voting in PB raises later turnout in general elections, larger for groups with low baseline turnout).
- Who participates and capture: Su 2017, "Managed participation: City agencies and micropolitics in participatory budgeting", *Nonprofit and Voluntary Sector Quarterly*, doi:10.1177/0899764018757029 [V] (online 2018) (content: memory: city agencies shaped and vetoed projects, constraining what residents could choose). A study of Brno's PB over five years found turnout does not reproduce the city's demographic composition; a Chicago 49th Ward process is cited as an example where skewed turnout selected low-priority projects favouring specific demographics (content: search summary) — [Brno study, NISPAcee 2023](https://reference-global.com/article/10.2478/nispa-2023-0012) [S]; [Phil. Trans. A 2024 paper, PMC11558248](https://pmc.ncbi.nlm.nih.gov/articles/PMC11558248) [S].
- Decide Madrid (on Consul): Royo, Pina, Garcia-Rayado 2020, "Decide Madrid: A Critical Analysis of an Award-Winning e-Participation Initiative", *Sustainability*, doi:10.3390/su12041674 [V] (content: memory: low participation relative to population, very few citizen proposals reaching the support threshold, limited deliberation).
- Aggregation rules: most cities use approval voting with greedy selection by vote count until the budget runs out (content: memory). Alternatives: knapsack voting — Goel, Krishnaswamy, Sakshuwong, Aitamurto 2019, *ACM Transactions on Economics and Computation*, doi:10.1145/3340230 [V]; proportional rules — Peters, Pierczyński, Skowron 2021, "Proportional Participatory Budgeting with Additive Utilities" (Method of Equal Shares), NeurIPS 2021 proceedings, no DOI found in Crossref [M]; survey: Aziz & Shah 2021, doi:10.1007/978-3-030-54936-7_10 [V] (chapter, 2020); data and tools: Faliszewski et al. 2023, "Participatory Budgeting: Data, Tools, and Analysis", IJCAI, doi:10.24963/ijcai.2023/297 [V].
- Method of Equal Shares in use (content: search summary of the promoters' pages and papers): Wieliczka (Poland) used it for its 1 million złoty "Green Million" in 2023, the researchers reporting fewer voters left with none of their projects funded (18% under the new method); Aarau (Switzerland) used it for its first PB (CHF 50,000, 2023), funding 17 projects against 7 under greedy selection, from every district, but dropping the third most popular project; Świecie, Winterthur and Assen have also used it — [equalshares.net: Wieliczka](https://equalshares.net/elections/zielony-milion) [S]; [equalshares.net: Aarau](https://equalshares.net/elections/aarau-2023) [S]; [arXiv 2405.05085, "Fair Voting Outcomes with Impact and Novelty Compromises? Unraveling Biases of Equal Shares"](https://ar5iv.labs.arxiv.org/html/2405.05085) [S]. *Caveat.* The figures come from the method's designers and academic partners, not independent audits.
- Pabulib, the open library of PB instances, lists 2,210 instances (Poland, Netherlands, Hungary and others) in a `.pb` format (content: page) — [pabulib.org](http://pabulib.org/) [V]; its licence was not stated on the page.
- Computational support entering PB: privacy-preserving algorithmic shortlisting of large volumes of submissions (Zambrano et al., arXiv 2508.06577 [V], content: abstract: shows limitations as well as uses); LLMs inferring voter preferences in PB (Damle et al., arXiv 2508.06060 [V]); LLMs proposing PB rules (Thach et al., arXiv 2602.09349 [V]).

**2.2 Equity and gender budgeting**
- Stotsky 2016, "Gender Budgeting: Fiscal Context and Current Outcomes", IMF Working Paper, doi:10.5089/9781475575460.001 [V] (content: memory: surveys over 80 countries with gender budgeting efforts; finds most are tagging and reporting exercises with limited evidence of reallocation; stronger where tied to specific fiscal decisions).
- OECD surveys report that a majority of OECD members practise some form of gender budgeting (roughly 23 of 38 in 2022, content: memory [M]); evidence of effects on allocations is thin and mostly descriptive.
- Equity budgeting in US cities (racial equity tools, "equity lenses" on capital budgets) is widespread but almost entirely unevaluated (content: memory) [M].

**2.3 Research funding: peer review and partial lotteries**
- Reliability of peer review: Pier et al. 2018, "Low agreement among reviewers evaluating the same NIH grant applications", *PNAS* 115(12), doi:10.1073/pnas.1714379115 [V] (content: abstract: 43 reviewers, 25 applications, "no agreement" in qualitative or quantitative evaluations; outcome "depended more on the reviewer to whom the grant was assigned than the research proposed"). Graves, Barnett, Clarke 2011, *BMJ* 343:d4797, doi:10.1136/bmj.d4797 [V] (content: memory: bootstrapping panel scores showed large chance variation near the funding line).
- Bias: Witteman, Hendricks, Straus, Tannenbaum 2019, "Are gender gaps due to evaluations of the applicant or the science? A natural experiment at a national funding agency", *Lancet* 393:531, doi:10.1016/S0140-6736(18)32611-4 [V] (content: memory: at CIHR, the gender gap in success was larger when reviewers assessed the applicant's track record than when they assessed the proposed science).
- Theory and critique: Fang & Casadevall 2016, "Research Funding: the Case for a Modified Lottery", *mBio*, doi:10.1128/mBio.00422-16 [V]; Gross & Bergstrom 2019, "Contest models highlight inherent inefficiencies of scientific funding competitions", *PLoS Biology*, doi:10.1371/journal.pbio.3000065 [V]; Roumbanis 2019, "Peer Review or Lottery?", *Science, Technology, & Human Values*, doi:10.1177/0162243918822744 [V]; Avin 2019, "Mavericks and lotteries", *Studies in History and Philosophy of Science*, doi:10.1016/j.shpsa.2018.11.006 [V]; Horbach, Tijdink, Bouter 2022, "Partial lottery can make grant allocation more fair, more efficient, and more diverse", *Science and Public Policy*, doi:10.1093/scipol/scac009 [V]; Philipps 2021, "Science rules! A qualitative study of scientists' approaches to grant lottery", *Research Evaluation*, doi:10.1093/reseval/rvaa027 [V] (online 2020) (content: memory: scientists accept lotteries mainly as tie-breakers after peer review, not as replacements); Philipps 2021, "Research funding randomly allocated? A survey of scientists' views on peer review and lottery", *Science and Public Policy*, doi:10.1093/scipol/scab084 [V] (content: title only).
- Health Research Council of New Zealand (Explorer Grants, since 2013): screening for eligibility and quality, then random selection until the budget is exhausted; of 325 applicants surveyed (39% response), 63% favoured randomisation for Explorer Grants and 25% opposed it, support lower for other grant types (40% for, 37% against) and higher among winners — Liu, Choy, Clarke, Barnett, Blakely, Pomeroy 2020, *Research Integrity and Peer Review* 5:3, doi:10.1186/s41073-019-0089-z [V] (content: abstract). A randomised comparison of 88 researchers over about 3.8 years found no clear effect of winning on publications (rate ratio 0.95, 95% CI 0.67–1.39) or citations (1.06, 0.79–1.43) — Barnett et al. 2024, *Science and Public Policy*, doi:10.1093/scipol/scae045 (content: search summary) [S].
- Swiss National Science Foundation: a tie-breaking lottery trialled in Postdoc.Mobility 2018–2020, then extended as a last resort to all schemes; proposals in a middle band whose credible intervals overlap the funding line enter the lottery — Heyard, Ott, Salanti, Egger 2022, "Rethinking the funding line at the Swiss National Science Foundation: Bayesian ranking and lottery", *Statistics and Public Policy*, doi:10.1080/2330443X.2022.2086190 [V], arXiv 2102.09958 (content: search summary) [S]; [Research Professional News](https://www.researchprofessionalnews.com/?p=311348) [S]. A simplified three-review procedure in Postdoc.Mobility agreed with the official outcome for 80.6% (95% CI 73.9–87.3%) of applicants, underpowered (Bieri et al., *BMJ Open*, content: search summary) [S].
- Other funders: the British Academy (small grants, 2022) and the Volkswagen Foundation ("Experiment!", from 2017) use partial randomisation (content: memory) [M]; the Research on Research Institute's *Experimental Research Funder's Handbook* (2nd ed.) documents cases — [RoRI handbook PDF](https://www.fwf.ac.at/fileadmin/Website/publications/Publikationen/FWF-Selbstevaluation/RoRI/The_experimental_research_funders_handbook_2nd_ed.pdf) [S].

**2.4 Social housing allocation**
- Priority versus choice: Waldinger 2021, "Targeting In-Kind Transfers through Market Design: A Revealed Preference Analysis of Public Housing Allocation", *American Economic Review*, doi:10.1257/aer.20190516 [V] (content: memory: in Cambridge, Massachusetts, giving applicants choice raises match quality but weakens targeting toward the neediest; the trade-off is quantified with revealed preferences). Arnosti & Shi 2020, "Design of Lotteries and Wait-Lists for Affordable Housing Allocation", *Management Science*, doi:10.1287/mnsc.2019.3311 [V] (content: memory: waiting lists and lotteries differ in how well they target applicants with fewer outside options; a lottery with restrictions can match a waitlist's targeting). Thakral's Pittsburgh study of public-housing allocation, record not found in Crossref [M] (content: memory).
- Choice-based lettings in the UK moved from officer-allocated points to advertised vacancies with bids by banded priority: Brown & King 2005, "The Power to Choose: Effective Choice and Housing Policy", *European Journal of Housing Policy*, doi:10.1080/14616710500055729 [V].
- Homelessness prioritisation: the VI-SPDAT, a widely used vulnerability index in US "coordinated entry", was criticised for weak validity and racial disparities in scores; its publisher announced its phase-out around 2021 (content: memory) [M]. Cronley 2022, "Invisible intersectionality in measuring vulnerability among individuals experiencing homelessness — critically appraising the VI-SPDAT", *Journal of Social Distress and Homelessness*, doi:10.1080/10530789.2020.1852502 [V] (online 2020). Predictive allocation: Kube, Das, Fowler 2019, "Allocating interventions based on predicted outcomes: A case study on homelessness services", AAAI, doi:10.1609/aaai.v33i01.3301622 [V] (content: memory: counterfactual predictions of which intervention would most reduce re-entry could cut re-entry relative to current assignment, raising fairness questions about allocating by predicted benefit); extended in Kube, Das, Fowler 2023, "Fair and Efficient Allocation of Scarce Resources Based on Predicted Outcomes: Implications for Homeless Service Delivery", *Journal of Artificial Intelligence Research*, doi:10.1613/jair.1.12847 [V] (content: title only).

**2.5 Humanitarian aid and targeting**
- Principles: humanity, neutrality, impartiality (aid according to need alone, without discrimination) and independence (UN General Assembly resolutions 46/182 of 1991 and 58/114 of 2004) (content: memory) [M].
- Needs assessment: severity classifications such as the Joint Intersectoral Analysis Framework (five-phase severity), the IPC phases for food insecurity and the INFORM Severity Index inform allocations of pooled funds (content: memory) [M]; Darcy & Hofmann 2003, *According to need? Needs assessment and decision-making in the humanitarian sector*, HPG Report 15 (content: memory: allocations were driven by donor interests and supply rather than comparable needs assessment) [M].
- Targeting methods and errors: Coady, Grosh, Hoddinott 2004, "Targeting Outcomes Redux", *World Bank Research Observer*, doi:10.1093/wbro/lkh016 [V]; Alatas, Banerjee, Hanna, Olken, Tobias 2012, "Targeting the Poor: Evidence from a Field Experiment in Indonesia", *AER*, doi:10.1257/aer.102.4.1206 [V] (content: memory: community-based targeting was slightly less accurate than a proxy means test (PMT) by consumption but matched local notions of poverty better and produced higher satisfaction and fewer complaints); Brown, Ravallion, van de Walle 2018, "A poor means test? Econometric targeting in Africa", *Journal of Development Economics*, doi:10.1016/j.jdeveco.2018.05.004 [V] (content: memory: PMTs in nine African countries had high exclusion errors for the poorest, so that with realistic budgets they did little better than universal or categorical transfers); Devereux et al. 2017, "The targeting effectiveness of social transfers", *Journal of Development Effectiveness*, doi:10.1080/19439342.2017.1305981 [V]; Grosh, Leite, Wai et al. 2022, *Revisiting Targeting in Social Assistance*, World Bank, doi:10.1596/978-1-4648-1814-1 [V].
- Exclusion errors at scale: Kidd & Athias, *Hit and Miss* (2019/2020): Bolsa Família (a simple means test) excluded 44% of intended recipients; one PMT scheme excluded 96%; among programmes covering under 25% of the population, 12 had exclusion errors above 70%; four universal schemes had exclusion errors under 10% (content: search summary; the report itself was not opened, and the linked page, one of the search results, was not confirmed to be the report) — [social-protection.org resource](https://www.social-protection.org/gimi/ShowRessource.action?id=54248) [S]. Related peer-reviewed work: Kidd 2017, "Social exclusion and access to social protection schemes", *Journal of Development Effectiveness*, doi:10.1080/19439342.2017.1305982 [V] (content: title only). *Caveat.* Development Pathways is an advocacy-leaning consultancy favouring universal schemes; its figures rely on household surveys.
- Machine learning for targeting: Aiken, Bellue, Karlan, Udry, Blumenstock 2022, "Machine learning and phone data can improve targeting of humanitarian aid", *Nature* 603, doi:10.1038/s41586-022-04484-9 [V] (content: abstract: Togo's Novissi COVID-19 cash programme; relative to geographic targeting the phone-data model reduced exclusion errors by 4–21%, but relative to a (hypothetical) comprehensive social registry it increased them by 9–35%; outcomes compared on exclusion errors, total welfare and fairness). Aiken et al. 2023, "Program targeting with machine learning and mobile phone data: Evidence from an anti-poverty intervention in Afghanistan", *Journal of Development Economics*, doi:10.1016/j.jdeveco.2022.103016 [V]. Critiques (content: memory) [M]: people without phones, shared SIMs and women are under-represented; recipients cannot know or contest why they were excluded; strategic behaviour once criteria are known.
- Cash transfers: Haushofer & Shapiro 2016, *QJE*, doi:10.1093/qje/qjw025 [V]; Egger, Haushofer, Miguel, Niehaus, Walker 2022, "General Equilibrium Effects of Cash Transfers: Experimental Evidence From Kenya", *Econometrica*, doi:10.3982/ECTA17945 [V] (content: memory: local multiplier about 2.5, little price inflation); Baird, McIntosh, Özler 2011, "Cash or Condition?", *QJE*, doi:10.1093/qje/qjr032 [V] (content: memory: conditions raised schooling, unconditional cash reduced teenage pregnancy and marriage, a trade-off between objectives); Doocy & Tappis 2017, Campbell systematic review of cash in humanitarian emergencies, doi:10.4073/csr.2017.17 [V]; Bastagli et al. 2016, *Cash transfers: what does the evidence say?*, ODI, the ODI report itself [M]; its journal version is Bastagli et al. 2019, 'The Impact of Cash Transfers: A Review of the Evidence from Low- and Middle-income Countries', *Journal of Social Policy* (online 2018), doi:10.1017/S0047279418000715 [V].

**2.6 Universal and targeted designs**
- Hanna & Olken 2018, "Universal Basic Incomes versus Targeted Transfers: Anti-Poverty Programs in Developing Countries", *Journal of Economic Perspectives*, doi:10.1257/jep.32.4.201 [V] (content: memory: with Indonesian and Peruvian data, targeted transfers achieve higher welfare for a fixed budget than a UBI despite targeting errors, under their welfare function).
- Korpi & Palme 1998, "The Paradox of Redistribution and Strategies of Equality", *American Sociological Review*, doi:10.2307/2657333 [V] (content: memory: the more benefits are targeted at the poor, the less redistribution occurs, because universal programmes command larger budgets through middle-class support); later work contests the paradox for recent decades (Marx, Salanauskaite, Verbist 2013, content memory [M]).
- Universal basic services: Gough 2019, "Universal Basic Services: A Theoretical and Moral Framework", *Political Quarterly*, doi:10.1111/1467-923X.12706 [V]; Coote & Percy 2020, *The Case for Universal Basic Services*, Polity, book record confirmed only through a review in *Journal of Social Policy*, doi:10.1017/S0047279421000088 [S].
- Mkandawire 2005, *Targeting and Universalism in Poverty Reduction*, UNRISD (content: memory) [M].

#### Inferences
- In every field, the eligibility pool is decided before the declared rule runs (who applies to PB, which grants pass screening before the lottery, who is on the housing list, who appears in the registry or owns a phone), and most measured injustice lives there. A tool must model the pool, record how it was formed, and report who is missing from it.
- Lotteries are legitimate in practice mostly as tie-breakers among proposals the procedure cannot distinguish (SNSF, HRC), where they also make the limits of judgement honest; their acceptance is higher among winners, which warns against reading acceptance surveys as justification.
- Exclusion errors are systematically under-counted because those excluded are not in the programme's data; an honest tool reports exclusion errors against an external benchmark (census, survey) and says when it cannot.
- Proportional PB rules (Equal Shares) and greedy rules embody different distributive principles (fair shares to groups of voters versus maximal approval); the choice is substantive, and a tool should show the funded set under each rule side by side.

#### Gaps
- No independent, non-promoter evaluation of Equal Shares outcomes in cities was found.
- No quantitative evaluation was found of whether gender or equity budgeting changed allocations.
- Licence terms of Pabulib data were not found.
- Primary records for UN resolutions on humanitarian principles, JIAF and the Darcy & Hofmann report were not retrieved this session.

### 3. Procedural Safeguards and Their Evaluation

#### Takeaway
The algorithmic failures in public allocation (Robodebt, the Dutch childcare benefits affair, the 2020 English exam grades, Michigan's MiDAS, SyRI) failed procedurally before they failed technically: the burden of proof was reversed, individual circumstances were ignored, legal advice and tribunal rulings were disregarded, explanations were withheld and appeals were slow or absent; the evidence that procedural fairness raises acceptance is substantial in policing and organisations but contested for policy decisions, where outcome favourability dominates, so safeguards must be justified by rights and correctability, not only by acceptance.

#### Cited Findings

**3.1 Robodebt (Australia, 2016–2020)**
- The Royal Commission into the Robodebt Scheme reported in July 2023 that income averaging "in the way it was proposed" was "inconsistent with social security legislation"; government lawyers had advised in November 2014 that the averaging "did not accord with legislation"; about 470,000 wrongful debts were raised; the scheme was meant to save AUD 4.7 billion but delivered AUD 406 million at a cost of AUD 971 million; the Commission rejected the former Prime Minister's evidence as "untrue" and referred matters to the APSC, the National Anti-Corruption Commission and the AFP (content: search summary of news and an APRA-hosted overview) — [JURIST](https://www.jurist.org/news/2023/07/australia-robodebt-royal-commission-hands-down-findings-recommends-referrals-for-prosecution/) [S]; [APRA overview PDF](https://www.apra.gov.au/sites/default/files/2024-02/Robodebt%20Royal%20Commision%20-%20Overview%20of%20Final%20Report.pdf) [S]; [ANU Reporter on income averaging](https://reporter.anu.edu.au/all-stories/income-averaging-key-source-of-mistaken-robodebt-debts) [S].
- Procedural failures (content: memory of the report and commentary) [M]: debts raised by data-matching annual tax income averaged over fortnights without human assessment; the onus shifted to recipients to disprove debts years later; tribunal (AAT) decisions against the method were not appealed, so no binding precedent formed; Federal Court consent orders (*Amato*, 2019) and a class-action settlement of about AUD 1.8 billion followed. Carney 2019, "Robo-debt illegality: The seven veils of failed guarantees of the rule of law?", *Alternative Law Journal*, doi:10.1177/1037969X18815913 [V] (online 2018); Rinta-Kahila, Someh, Gillespie, Indulska, Gregor 2022, "Algorithmic decision-making and system destructiveness: A case of automatic debt recovery", *European Journal of Information Systems*, doi:10.1080/0960085X.2021.1960905 [V] (online 2021).

**3.2 The Dutch childcare benefits affair (toeslagenaffaire)**
- The parliamentary inquiry *Ongekend onrecht* (17 December 2020) concluded that "fundamental principles of the rule of law have been violated": efficiency and fraud prevention were so prioritised that individual circumstances received little or no consideration (content: search summary of the House of Representatives' English report) — [Unprecedented injustice (English PDF)](https://www.houseofrepresentatives.nl/sites/default/files/atoms/files/verslag_pok_definitief-en-gb.docx.pdf) [S]; [Tweede Kamer press release](https://www.tweedekamer.nl/nieuws/persberichten/grondbeginselen-rechtstaat-geschonden) [S]. A second inquiry on fraud policy and public services (report 2024) concluded that people's fundamental rights were violated and that all three branches were blind to inhumane treatment (content: search summary) — [Fraud Policy inquiry findings](https://www.houseofrepresentatives.nl/node/1217) [S]. The Venice Commission issued an opinion in 2021 (content: search summary) — [CDL-REF(2021)073](https://www.coe.int/en/web/venice-commission/-/cdl-ref-2021-073-e) [S].
- Further content (memory) [M]: tens of thousands of parents were labelled fraudsters, often for minor administrative errors, with all-or-nothing recovery of years of benefits; a risk-classification model used nationality (Dutch or not) as an indicator; the Dutch Data Protection Authority fined the Tax Administration (€2.75 million, December 2021); the Rutte cabinet resigned in January 2021; Amnesty International's *Xenophobic Machines* (2021). Hadwick & Lan 2021, *World Tax Journal*, doi:10.59403/27410pa [V].
- SyRI: the District Court of The Hague held on 5 February 2020 that the System Risk Indication welfare-fraud profiling legislation violated Article 8 ECHR for lack of transparency and safeguards (ECLI:NL:RBDHA:2020:865) (content: memory) [M].

**3.3 The 2020 English exam grading algorithm (Ofqual)**
- 39.1% of teacher-assessed A-level grades were lowered by the standardisation model; the reductions fell hardest on students from disadvantaged schools while private-school students benefited most; the appeals route was confused (free appeals announced, then criteria withdrawn for review) and Ofqual's former chair later admitted letting results through "knowing that they would need to be changed on appeal"; the government reverted to teacher grades on 17 August 2020, after Scotland had reversed a week earlier (content: search summary of news) — [TES](https://www.tes.com/magazine/news/secondary/obviously-wrong-level-grades-allowed-ofqual) [S]; [Dawn/AFP](https://www.dawn.com/news/1574930/englands-exams-regulator-reverses-course-after-grading-furore) [S].
- The model used teacher grades alone for subject cohorts under 5 and blended them for cohorts of 5–15, so small classes (common in private schools) escaped the historical-distribution adjustment (content: memory) [M]. Kelly 2021, "A tale of two algorithms: The appeal and repeal of calculated grades systems in England and Ireland in 2020", *British Educational Research Journal*, doi:10.1002/berj.3705 [V] (content: memory: Ireland's system, which kept school-level teacher judgement and did not use school history, survived).

**3.4 Other cases (content: memory) [M]**
- Michigan's MiDAS (2013–2015) issued automated unemployment-fraud determinations with penalties; a state review found most determinations reviewed were wrong (often cited as 93%); litigation and settlements followed.
- Arkansas's Medicaid home-care assessment algorithm (2016) cut hours of care; a court ordered a halt in 2018 after flaws were found in how it was coded and explained.
- The Allegheny Family Screening Tool (child welfare) was examined by Eubanks (*Automating Inequality*, 2018) and later for disability-related features.
- Sweden's Social Insurance Agency's fraud-risk model was reported in 2024 (Lighthouse Reports, Svenska Dagbladet) to flag women, migrants and low-income people disproportionately; Rotterdam's welfare-fraud risk model (2023) and France's CNAF scoring (La Quadrature du Net, 2023–2024) drew similar findings and legal challenges.

**3.5 Explanation, contestation and the law**
- CJEU, C-203/22 *Dun & Bradstreet Austria*, judgment of 27 February 2025 (ECLI:EU:C:2025:117): under Article 15(1)(h) GDPR the data subject may require "meaningful information about the logic involved", i.e. an explanation of "the procedure and principles actually applied" in a concise, intelligible form enabling them to understand and challenge the decision; neither a complex formula nor a description of every step suffices; trade secrets are weighed by the court (content: search summary of the Court's press release) — [CURIA press release 22/25](https://curia.europa.eu/jcms/upload/docs/application/pdf/2025-02/cp250022en.pdf) [S]. Earlier: C-634/21 *SCHUFA* (7 December 2023), a score that plays a determining role counts as an automated decision under Article 22 (content: memory) [M].
- The academic debate before these rulings: Wachter, Mittelstadt, Floridi 2017, "Why a Right to Explanation of Automated Decision-Making Does Not Exist in the GDPR", *International Data Privacy Law*, doi:10.1093/idpl/ipx005 [V]; Selbst & Powles 2017, "Meaningful information and the right to explanation", *IDPL*, doi:10.1093/idpl/ipx022 [V].
- EU AI Act (Regulation (EU) 2024/1689) (content: memory) [M]: systems used by public authorities to evaluate eligibility for essential public assistance benefits and services, for emergency triage, and for creditworthiness are high-risk (Annex III); deployers that are public bodies must carry out a fundamental-rights impact assessment (Art. 27); human oversight (Art. 14); a right to explanation of individual decisions based on high-risk systems (Art. 86).
- Due process and administration: Citron 2008, "Technological Due Process", *Washington University Law Review*, record not found in Crossref [M]; Mashaw 1983, *Bureaucratic Justice*, book record confirmed only through a 1983 review in *JPAM*, doi:10.2307/3324065 [S] (content: memory: three models of administrative justice — bureaucratic rationality, professional treatment, moral judgement — each with its own legitimacy); Bovens & Zouridis 2002, "From Street-Level to System-Level Bureaucracies", *PAR*, doi:10.1111/0033-3352.00168 [V]; Peeters & Widlak 2018, "The digital cage", *Government Information Quarterly*, doi:10.1016/j.giq.2018.02.003 [V]; Alkhatib & Bernstein 2019, "Street-Level Algorithms", CHI, doi:10.1145/3290605.3300760 [V] (content: memory: algorithms cannot exercise discretion at the gaps where cases fall outside the training distribution); Lyons, Velloso, Miller 2021, "Conceptualising Contestability", CSCW, doi:10.1145/3449180 [V]; Henman 2020, "Improving public services using artificial intelligence: possibilities, pitfalls, governance", *Asia Pacific Journal of Public Administration*, doi:10.1080/23276665.2020.1816188 [V]; Busuioc 2021, "Accountable Artificial Intelligence", *PAR*, doi:10.1111/puar.13293 [V] (online 2020); Raji et al. 2020, "Closing the AI accountability gap", FAT*, doi:10.1145/3351095.3372873 [V].

**3.6 Human oversight and automation bias**
- Alon-Barkat & Busuioc 2023, "Human–AI Interactions in Public Sector Decision Making: 'Automation Bias' and 'Selective Adherence' to Algorithmic Advice", *JPART*, doi:10.1093/jopart/muac007 [V] (online 2022) (content: memory: experiments with Dutch citizens found little general automation bias but "selective adherence": advice followed more when it matched stereotypes). Green & Chen 2019, "The Principles and Limits of Algorithm-in-the-Loop Decision Making", CSCW, doi:10.1145/3359152 [V]; Green 2022, "The flaws of policies requiring human oversight of government algorithms", *Computer Law & Security Review*, doi:10.1016/j.clsr.2022.105681 [V] (content: memory: people are poor at judging algorithmic accuracy and at correcting it; oversight requirements can legitimise flawed systems).
- LLM suggestions to social-service caseworkers: in a randomised experiment with caseworkers from Los Angeles outreach organisations on a 770-question benchmark of eligibility rules (e.g. SNAP), control accuracy was 49%; near-perfect chatbot suggestions (96–100% accurate) raised accuracy by 27 points, but incorrect suggestions cut accuracy on easy questions by about two-thirds, and gains plateaued below the chatbot's own accuracy ("AI underreliance plateau") — Gosciak, Giannella, Guo, Chen, Koenecke 2026, "LLMs in social services: How does chatbot accuracy affect human accuracy?", arXiv 2603.11213 [V] (content: page).

**3.7 Does procedural fairness raise acceptance?**
- Tyler 2003, "Procedural justice, legitimacy, and the effective rule of law", *Crime and Justice*, doi:10.1086/652233 [V]; Colquitt, Conlon, Wesson, Porter, Ng 2001, "Justice at the millennium", *Journal of Applied Psychology*, doi:10.1037/0021-9010.86.3.425 [V] (content: memory: meta-analysis of 183 studies; procedural justice strongly related to outcome satisfaction, trust and commitment).
- Walters & Bolger 2019, "Procedural justice perceptions, legitimacy beliefs, and compliance with the law: a meta-analysis", *Journal of Experimental Criminology*, doi:10.1007/s11292-018-9338-2 [V] (online 2018); a 2023 systematic review of procedural justice and legitimacy in policing covered 123 studies (N = 200,966) (content: search summary) [S]; Gulevich, Borovikova, Rodionova 2023, a meta-analysis of 69 samples (N = 50,814) on procedural justice and attitudes to the political system, *Political Psychology* (content: search summary) — [HSE-hosted PDF](https://www.hse.ru/data/2025/06/23/1991888479/The%20Relationship%20between%20Political%20Proce..olitical%20System_A%20Meta-analysis.pdf) [S].
- Contrary evidence: Nagin & Telep 2017, "Procedural justice and legal compliance", *Annual Review of Law and Social Science*, doi:10.1146/annurev-lawsocsci-110316-113310 [V] (content: memory: correlational evidence dominates; causal evidence that procedural justice changes compliance is weak); Esaiasson, Persson, Gilljam, Lindholm 2019, "Reconsidering the role of procedures for decision acceptance", *British Journal of Political Science*, doi:10.1017/S0007123416000508 [V] (online 2016) (content: search summary: outcome favourability is "overwhelmingly the strongest determinant" of willingness to accept authoritative decisions, though core claims of procedural fairness theory hold).
- Algorithmic decisions: Starke, Baleis, Keller, Marcinkowski 2022, "Fairness perceptions of algorithmic decision-making: A systematic review of the empirical literature", *Big Data & Society*, doi:10.1177/20539517221115189 [V]; Binns et al. 2018, "'It's Reducing a Human Being to a Percentage'", CHI, doi:10.1145/3173574.3173951 [V]; Lee, Jain, Cha, Ojha, Kusbit 2019, "Procedural Justice in Algorithmic Fairness: Leveraging Transparency and Outcome Control", CSCW, doi:10.1145/3359284 [V] (content: memory: giving people outcome control over an algorithmic division raised perceived fairness more than transparency alone). Surveys of over 3,200 people in the US and UK found welfare recipients reject AI-supported decisions significantly more often than non-recipients, even with faster processing promised (content: search summary) — [Max Planck Society news](https://www.mpg.de/25478774/news-page) [S].

#### Inferences
- The procedural failures share a structure: (1) reversal of the burden of proof onto the person; (2) no individual consideration (all-or-nothing rules, averaging); (3) known legal doubts not acted on; (4) explanations withheld or unintelligible; (5) appeal channels that were slow, costly, or that resolved individual cases without correcting the rule; (6) protected characteristics or their proxies used as risk factors. A safeguard design should address each explicitly; in particular, upheld appeals must feed back into revising the rule, not only the individual outcome (what Robodebt's unappealed tribunal losses prevented).
- Because outcome favourability dominates acceptance of policy decisions, a tool cannot claim its procedures will make losers accept outcomes; it can claim only that reasons are recorded, contestable and correctable, which is a rights and accuracy argument rather than a persuasion argument.
- "Human in the loop" is not a safeguard by itself: the caseworker experiment shows wrong machine suggestions degrade human accuracy, and selective adherence follows stereotypes; the safeguard must be measured with real users and the human must be able to see the evidence, not only the recommendation.

#### Gaps
- The Royal Commission's full report, the Dutch reports' detailed chapters, and Ofqual's technical report were not read in full this session; specific figures above rest on search summaries.
- No meta-analysis specific to procedural fairness and acceptance of allocation decisions (as opposed to policing or organisations) was found.
- Primary records of MiDAS, Arkansas, Swedish, Rotterdam and CNAF cases were not retrieved.

### 4. Decision-Support Tools and Software

#### Takeaway
Existing tools each solve one slice: MCDA frameworks (EVIDEM) structure criteria and weights but are criticised for double counting, ignoring opportunity cost and lending false precision; PB platforms (Decidim, Consul) and aggregation libraries (pabutools) record proposals and votes but rarely reasons; fair-division services (Spliddit) explain the fairness properties of a division; deliberation platforms (Pol.is, the Stanford platform, the Habermas Machine, Talk to the City) map opinion and find common ground but mostly do not attach reasons to final decisions or provide a channel of contestation after the decision; most open ones are AGPL, GPL or Apache licensed.

#### Cited Findings

**4.1 MCDA in priority setting and HTA**
- EVIDEM: Goetghebeur, Wagner, Khoury, Levitt, Erickson, Rindress 2008, "Evidence and Value: Impact on DEcisionMaking — the EVIDEM framework and potential applications", *BMC Health Services Research* 8:270, doi:10.1186/1472-6963-8-270 [V] (content: memory: a core model of quantitative criteria — disease severity, size of population, unmet need, comparative effectiveness, safety, patient-reported outcomes, type of benefit, cost consequences, quality of evidence — scored and weighted, plus contextual criteria discussed qualitatively; released as an open framework by a non-profit collaboration). Its licence was not verified this session.
- ISPOR task force: Thokala et al. 2016, Report 1, *Value in Health* 19(1), doi:10.1016/j.jval.2015.12.003 [V]; Marsh et al. 2016, Report 2, doi:10.1016/j.jval.2015.12.016 [V].
- Advocacy and critique: Baltussen & Niessen 2006, "Priority setting of health interventions: the need for multi-criteria decision analysis", *Cost Effectiveness and Resource Allocation* 4:14, doi:10.1186/1478-7547-4-14 [V]; Campillo-Artero, Puig-Junoy, Culyer 2018, "Does MCDA Trump CEA?", *Applied Health Economics and Health Policy* 16, doi:10.1007/s40258-018-0373-y [V] (and reply doi:10.1007/s40258-018-0446-y [V]) (content: memory: MCDA weights criteria without a budget constraint and so cannot say what is given up; criteria overlap with what QALYs already capture, causing double counting; additive models assume preferential independence); Morton 2017, "Treacle and Smallpox: Two Tests for Multicriteria Decision Analysis Models in Health Technology Assessment", *Value in Health* 20(3), doi:10.1016/j.jval.2016.10.005 [V] (content: memory: an MCDA model should rank a highly effective cheap intervention (smallpox vaccine) above an ineffective one (treacle); many published models fail such sanity tests because weights are not tied to scales); Baltussen et al. 2019, "Multicriteria Decision Analysis to Support Health Technology Assessment Agencies: Benefits, Limitations, and the Way Forward", *Value in Health* 22(11), doi:10.1016/j.jval.2019.06.014 [V] (content: memory: recommends MCDA as a support to deliberation, not a substitute, with opportunity cost made explicit).
- Ethics in HTA: Hofmann 2005, "Toward a procedure for integrating moral issues in health technology assessment", *IJTAHC* 21(3), doi:10.1017/S0266462305050415 [V] (content: memory: a list of questions to surface moral issues).

**4.2 Participatory budgeting platforms and aggregation software**
- Decidim (Barcelona): AGPL-3.0, actively maintained (GitHub API: last push 2026-11-19) — [decidim/decidim](https://github.com/decidim/decidim) [V]. Barandiaran, Calleja-López, Monterde, Romero 2024, *Decidim, a Technopolitical Network for Participatory Democracy* (SpringerBriefs), doi:10.1007/978-3-031-50784-7_5 [V] (record verified through its chapters) (content: memory: modules for proposals, debates, budgets, assemblies, a "social contract" of democratic guarantees, traceability of proposals to results; comments with alignment (for/against/neutral)).
- Consul Democracy (Madrid): AGPL-3.0, maintained (last push 2026-11-19) — [consuldemocracy/consuldemocracy](https://github.com/consuldemocracy/consuldemocracy) [V].
- pabutools: GPL-3.0-or-later (PyPI classifier; GitHub `COMSOC-Community/pabutools`, last push 2026-07-07); rules include greedy, Method of Equal Shares, sequential Phragmén, and analysis tools (rule list content: memory) — [PyPI pabutools](https://pypi.org/project/pabutools/) [V]; [GitHub](https://github.com/COMSOC-Community/pabutools) [V].
- Stanford Participatory Budgeting platform: knapsack, approval, ranked and point ballots, used by many US cities (content: memory) [M]; no public repository or licence was found.
- equalshares.net publishes the rule, per-election results and explanations of why each project won or lost (content: page summary) — [equalshares.net elections](https://equalshares.net/elections) [S].

**4.3 Fair division services**
- Spliddit: Goldman & Procaccia 2015, "Spliddit: Unleashing Fair Division Algorithms", *SIGecom Exchanges*, doi:10.1145/2728732.2728738 [V] (content: memory: rent division by an envy-free solution chosen to maximise the minimum utility, goods by maximum Nash welfare, credit and fare division, task assignment; each result comes with a plain statement of the fairness guarantee it satisfies). No public repository or licence was found this session.
- WeBuildAI (food donation allocation at 412 Food Rescue): Lee, Kusbit, Kahng, Kim, Yuan, Chan, See, Noothigattu, Lee, Psomas, Procaccia 2019, "WeBuildAI: Participatory Framework for Algorithmic Governance", CSCW, doi:10.1145/3359283 [V] (content: memory: stakeholders (donors, recipients, volunteers, staff) each built a model of their own allocation preferences by pairwise comparisons; the models vote (Borda) on each donation; an explanation interface shows how each stakeholder's model ranked the options; participants reported greater trust and perceived fairness). This is the closest published precedent for an allocation system in which stakeholders' judgements are explicit, inspectable and aggregated.

**4.4 Deliberation platforms**
- Pol.is: AGPL-3.0, maintained (last push 2026-11-19) — [compdemocracy/polis](https://github.com/compdemocracy/polis) [V]; open data exports under CC BY 4.0, attributed to The Computational Democracy Project, including vTaiwan's Uber consultation, a Brexit consensus conversation, Canadian electoral reform, Seattle's $15 minimum wage, and New Zealand biodiversity (content: page) — [compdemocracy/openData](https://github.com/compdemocracy/openData) [V]. Small, Bjorkegren, Erkkilä, Shaw, Megill 2021, "Polis: Scaling Deliberation by Mapping High Dimensional Opinion Spaces", *Recerca*, doi:10.6035/recerca.5516 [V] (content: memory: participants submit short statements and vote agree/disagree/pass; PCA and clustering produce opinion groups; "group-informed consensus" highlights statements agreed across groups; no replies, by design, to avoid flame wars).
- vTaiwan: Hsiao, Lin, Tang, Narayanan, Sarahe 2018, "vTaiwan: An empirical study of open consultation process in Taiwan", SocArXiv, doi:10.31235/osf.io/xyhft [V] (content: memory: of the cases run, a minority led to legislation; the government's commitment to act was uneven, and the process declined after 2018).
- Stanford Online Deliberation Platform: video small groups with an automated moderator and structured agenda; Gelauff et al. 2024 used an LLM to estimate contribution quality from transcripts (arXiv 2408.11936 [V], content: abstract); the platform's automated moderation is described in Gelauff et al. 2023, "Achieving parity with human moderators", in *The Routledge Handbook of Collective Intelligence for Democracy and Governance*, doi:10.4324/9781003215929-15 [V] (content: title only). Fishkin & Luskin 2005, "Experimenting with a democratic ideal: Deliberative polling and public opinion", *Acta Politica*, doi:10.1057/palgrave.ap.5500121 [V].
- Sortition: Flanigan, Gölz, Gupta, Hennig, Procaccia 2021, "Fair algorithms for selecting citizens' assemblies", *Nature* 596, doi:10.1038/s41586-021-03788-6 [V] (content: abstract: maximally equal selection probabilities subject to representative quotas; used for over 40 assemblies); stratification app GPL-3.0 — [sortitionfoundation/stratification-app](https://github.com/sortitionfoundation/stratification-app) [V]; replication code GPL-3.0 — [pgoelz/citizensassemblies-replication](https://github.com/pgoelz/citizensassemblies-replication) [V].
- LLM-based tools: the Habermas Machine's code is released under Apache-2.0 — [google-deepmind/habermas_machine](https://github.com/google-deepmind/habermas_machine) [V]; Talk to the City (AI Objectives Institute) report pipeline, Apache-2.0, archived — [AIObjectives/talk-to-the-city-reports](https://github.com/AIObjectives/talk-to-the-city-reports) [V]; Jigsaw's sensemaking tools, Apache-2.0, maintained — [Jigsaw-Code/sensemaking-tools](https://github.com/Jigsaw-Code/sensemaking-tools) [V].

**4.5 How tools record reasons, disagreement and contestation** (synthesis; licences [V] above, features (content: memory) unless stated)

| Tool | Reasons recorded | Disagreement | Contestation after decision | Licence |
|---|---|---|---|---|
| EVIDEM / MCDA | criterion scores, weights, qualitative contextual notes | spread of weights across panellists, if collected | none built in | framework open (terms not verified) |
| Decidim | proposals, debates, comments with for/against alignment; proposal-to-result traceability | comments and votes | follow-up of results; no formal appeal module | AGPL-3.0 |
| Consul Democracy | proposals, comments, debates | votes, supports | none formal | AGPL-3.0 |
| pabutools / equalshares.net | none (ballots only); equalshares.net explains why each project won or lost | ballots | none | GPL-3.0+ |
| Spliddit | fairness guarantee stated per result | inputs (bids, valuations) | none | not found |
| WeBuildAI | each stakeholder's model and its ranking per case | across stakeholder models | explanation interface, no appeal | research system |
| Pol.is | short statements only, no replies | opinion groups, divisive statements | none | AGPL-3.0; data CC BY 4.0 |
| Habermas Machine | individual opinions and critiques, group statements | critiques fed back into drafts | none | Apache-2.0 |
| Talk to the City / sensemaking tools | LLM clusters of quotes with links to sources | clusters by theme | none | Apache-2.0 |

#### Inferences
- No surveyed tool closes the loop from reasons to decision to contestation to revision: deliberation tools stop at common ground, PB tools at the funded set, MCDA at the score. A tool that records the reasons behind each allocation, lets an affected party contest a specific reason, and records the response and any revision would fill a real gap.
- The MCDA critiques give concrete tests a tool should apply to any weighted scheme it hosts: an explicit budget and opportunity cost, scale-anchored weights (swing weighting), checks for double counting with QALYs or other composite measures, and sanity tests of Morton's kind.
- Pol.is's design choice (no replies, statements as atoms) trades argument for scale; an allocation tool needs the argument (who claims what, on what ground), so it should borrow Pol.is's opinion mapping as a view, not its data model.
- Licence compatibility matters for reuse: AGPL-3.0 code (Pol.is, Decidim, Consul) imposes network-copyleft obligations on a hosted service that incorporates it; Apache-2.0 tools (Habermas Machine, sensemaking tools) are permissive; for an MIT-licensed infrastructure, interoperate with AGPL tools by data exchange rather than by incorporating their code.

#### Gaps
- EVIDEM's current licence, governance and use were not verified.
- Spliddit's and the Stanford PB platform's source availability and licences were not found.
- No published evaluation was found comparing these tools on how well reasons are recorded or contestations handled.

### 5. Language Models in Deliberation and Allocation

#### Takeaway
Language models can draft group statements that people prefer to human mediators' and that move groups toward agreement (the Habermas Machine), and AI-designed redistribution mechanisms can win majority votes (Democratic AI); but models' own distributive and moral judgements diverge from humans', are sensitive to wording, are tolerant of inequality in some settings and prefer stricter fairness in others, flatten identity groups when used as simulated stakeholders, are sycophantic and persuasive, invent facts, and give explanations that are not faithful to their reasoning; the defensible roles are articulating and organising claims and drafting under human control with checked provenance, not judging.

#### Cited Findings

**5.1 LLM-mediated consensus**
- Tessler, Bakker, Jarrett, Sheahan, Chadwick, Koster, Evans, Campbell-Gillingham, Collins, Parkes, Botvinick, Summerfield 2024, "AI can help humans find common ground in democratic deliberation", *Science* 386, doi:10.1126/science.adq2852 [V] (content: abstract: the "Habermas Machine" iteratively drafts and refines group statements from participants' opinions and critiques; N = 5,734; participants preferred its statements to those of human mediators, rating them more informative, clear and unbiased; discussants often updated views and converged; text embeddings showed successful statements incorporated dissenting voices while respecting the majority; replicated in a virtual citizens' assembly with a demographically representative UK sample). The approximate 56% preference share over human mediators is (content: memory).
- Precursor: Bakker et al. 2022, "Fine-tuning language models to find agreement among humans with diverse preferences", NeurIPS, doi:10.52202/068431-2766 [V] (content: memory: a reward model per participant predicts agreement; consensus statements are chosen by a social welfare function over predicted agreements; the choice of welfare function (utilitarian vs. Rawlsian) changes which statements win).
- Follow-up by the authors: Tessler, Evans, Bakker, Gabriel, Bridgers, Jain, Koster, Rieser, Dragan, Botvinick, Summerfield 2026, "Can AI mediation improve democratic deliberation?", arXiv 2601.05904 [V] (content: page: frames the problem as Fishkin's trilemma of participation, deliberation and political equality; argues LLMs could widen participation, improve equality through fair mediation and support deliberation with trustworthy information, but that empirical, technical and theoretical advances are needed first).
- Critiques (content: search summaries of commentaries; normative, not empirical refutations): optimising for agreement misreads Habermas, for whom agreement is a regulative ideal; making agreement the objective is itself a political choice; participants interact with a machine individually and privately, with no argumentative exchange, so it is not deliberation; it bypasses the interpersonal friction where democratic work happens; it may create an illusion of consensus; its evaluation omits variables central to deliberative theory — [AIES 2025, "Towards Automating Deliberation? The Idea of Deliberative Democracy Embedded in Google's Habermas Machine"](https://ojs.aaai.org/index.php/AIES/article/view/36687) [S]; [CACM blog, "Computation and Deliberation: The Ghost in the Habermas Machine"](https://cacm.acm.org/blogcacm/computation-and-deliberation-the-ghost-in-the-habermas-machine) [S]; [Knight First Amendment Institute](https://knightcolumbia.org/content/can-ai-mediation-improve-democratic-deliberation) [S]; [Noema, "Democracy Needs Friction To Function"](https://www.noemamag.com/democracy-needs-friction-to-function/) [S]. No peer-reviewed empirical rebuttal was found.
- Democratic AI and redistribution: Koster, Balaguer, Tacchetti, Weinstein, Zhu, Hauser, Williams, Campbell-Gillingham, Thacker, Botvinick, Summerfield 2022, "Human-centred mechanism design with Democratic AI", *Nature Human Behaviour* 6, doi:10.1038/s41562-022-01383-x [V] (content: abstract: reinforcement learning designed a redistribution mechanism for a public-goods investment game that "redressed initial wealth imbalance, sanctioned free riders and successfully won the majority vote" against human-designed mechanisms). *Remark.* Its objective is majority preference, so it inherits majority rule's indifference to minorities; it is evidence that machine-proposed mechanisms can be acceptable, not that they are just.
- Other LLM pipelines for collective input: Small, Vendrov, Durmus, Homaei, Barry, Cornebise, Suzman, Ganguli, Megill 2023, "Opportunities and Risks of LLMs for Scalable Deliberation with Polis", arXiv 2306.11932 [V] (content: abstract: pilot experiments with Claude on facilitating, moderating and summarising Polis conversations; the paper discusses risks of summaries misrepresenting minority views); Fish, Gölz, Parkes, Procaccia, Rusak, Shapira, Wüthrich, "Generative Social Choice", arXiv 2309.01291 [V], ACM EC 2024, doi:10.1145/3670865.3673547 [V], and *Journal of the ACM* 2026, doi:10.1145/3799709 [V] (content: abstract: combines social choice guarantees (proportional representation of statements) with LLM generation and prediction of approval); Boehmer et al. 2025, "Generative Social Choice: The Next Generation", arXiv 2505.22939 [V]; Konya, Schirch, Irwin, Ovadya 2023, "Democratic Policy Development using Collective Dialogues and AI", arXiv 2311.02242 [V] (content: abstract: AI-enabled collective dialogues with bridging-based ranking; GPT-4 translates consensus into policy clauses); Huang, Siddarth, Lovitt, Liao, Durmus, Tamkin, Ganguli 2024, "Collective Constitutional AI", FAccT, doi:10.1145/3630106.3658979 [V]; Argyle, Bail, Busby, Gubler, Howe, Rytting, Sorensen, Wingate 2023, "Leveraging AI for democratic discourse", *PNAS*, doi:10.1073/pnas.2311627120 [V] (content: memory: real-time rephrasing suggestions raised perceived quality and respect of political conversations without changing policy views).

**5.2 LLMs as simulated stakeholders**
- Argyle, Busby, Fulda, Gubler, Rytting, Wingate 2023, "Out of One, Many: Using Language Models to Simulate Human Samples", *Political Analysis*, doi:10.1017/pan.2023.2 [V] (content: memory: "algorithmic fidelity" of GPT-3 for some US partisan subgroups).
- Bisbee, Clinton, Dorff, Kenkel, Larson 2024, "Synthetic Replacements for Human Survey Data? The Perils of Large Language Models", *Political Analysis*, doi:10.1017/pan.2024.5 [V] (content: memory: synthetic responses had too little variance, were sensitive to prompt wording and model version, and gave different regression conclusions).
- Wang, Morgenstern, Dickerson 2025, "Large language models that replace human participants can harmfully misportray and flatten identity groups", *Nature Machine Intelligence*, doi:10.1038/s42256-025-00986-z [V], arXiv 2402.01908 [V] (content: abstract and memory: LLMs portray groups as outsiders imagine them rather than as members describe themselves, and flatten within-group diversity).
- Santurkar, Durmus, Ladhak, Lee, Liang, Hashimoto 2023, "Whose Opinions Do Language Models Reflect?", ICML 2023 (PMLR), no DOI found in Crossref [M]; Atari, Xue, Park, Blasi, Henrich 2023, "Which Humans?", doi:10.31234/osf.io/5b26t [V] (PsyArXiv) (content: memory: LLM responses most resemble WEIRD populations); Durmus et al. 2023, "Towards Measuring the Representation of Subjective Global Opinions in Language Models", arXiv (content: memory) [M]; Dillion, Tandon, Gu, Gray 2023, "Can AI language models replace human participants?", *Trends in Cognitive Sciences*, doi:10.1016/j.tics.2023.04.008 [V]; Horton 2023, "Large Language Models as Simulated Economic Agents: What Can We Learn from Homo Silicus?", arXiv 2301.07543 [V].

**5.3 LLMs' own moral and distributive judgements**
- Hosseini & Khanna 2025, "Distributive Fairness in Large Language Models: Evaluating Alignment with Human Values", arXiv 2502.00313 [V], NeurIPS 2025 (content: search summary and abstract: GPT-4o, Claude-3.5, Llama3-70b and Gemini-1.5 compared with human subjects on allocation instances; LLMs' responses do not align with human distributional preferences; they more often return envy-free solutions, tolerate large inequality, cannot use money as a transferable resource to reduce inequality, and do better selecting from a menu than generating an allocation).
- Han, Hosseini, Kavner, Khanna, Sikdar, Xia 2026, "Fair Like Us? Auditing LLM Alignment in Resource Allocation", arXiv 2609.29692 [V], EMNLP 2026 (content: page: across many models and matched human scenarios, LLMs prefer stricter fairness constraints than humans, behave more self-interestedly, are sensitive to framing, and are hard to align by fine-tuning on current datasets).
- Cookson, Ebadian, Shah, "Fairness Perceptions of Large Language Models", AAAI (content: search summary: instructed to be fair, models tend to prioritise collective welfare over equal distribution; stating the criterion as a mathematical objective without the word "fairness" aligns better) — [AAAI OJS](https://ojs.aaai.org/index.php/AAAI/article/view/40848) [S].
- Cheung, Maier, Lieder 2025, "Large language models show amplified cognitive biases in moral decision-making", *PNAS* 122, doi:10.1073/pnas.2412015122 [V] (content: abstract: compared with representative US samples, LLMs were more altruistic in collective-action problems, showed stronger omission bias (endorsing inaction), and most were biased toward answering "no", flipping decisions with question wording; biases likely arise from fine-tuning).
- Social budgets: Benavides Cantos & Garrido-Merchán 2026, "Social Policy of Large Language Models: How GPT, Claude, DeepSeek and Grok Allocate Social Budgets in Spain and Germany", arXiv 2605.10234 [V] (content: page: 48 allocations over twelve areas; all four models share an implicit social policy far from actual spending, under-allocating pensions by a factor near three and over-allocating housing (about fourfold) and employment (about twofold); differences between models are about concentrating versus spreading the budget, not geopolitics; only Claude was substantially sensitive to national context; authors conclude models may support but should not replace expert deliberation).
- Bias audits are fragile: a FairFund benchmark found small changes in querying can reverse the estimated direction of bias for the same model (content: search summary) [S] — [ACL Anthology 2026 Findings EMNLP 1340](https://aclanthology.org/2026.findings-emnlp.1340.pdf) [S]. Name-based demographic bias in evaluations: An, Huang, Lin, Tai 2025, *PNAS Nexus*, doi:10.1093/pnasnexus/pgaf089 [V].

**5.4 Risks: sycophancy, homogenisation, persuasion, invention, unfaithful reasons**
- Sycophancy: Sharma et al. 2023/2024, "Towards Understanding Sycophancy in Language Models", arXiv 2310.13548 [V], ICLR 2024 (no DOI) (content: abstract: five assistants consistently tailor answers to users' stated beliefs; human preference judgements reward this).
- Homogenisation: Doshi & Hauser 2024, *Science Advances* 10, doi:10.1126/sciadv.adn5290 [V] (content: abstract: AI ideas made individual stories more creative but stories more similar to one another, a social dilemma of collective novelty); Anderson, Shah, Kreminski 2024, "Homogenization Effects of Large Language Models on Human Creative Ideation", doi:10.1145/3635636.3656204 [V]; Jakesch, Bhat, Buschek, Zalmanson, Naaman 2023, "Co-Writing with Opinionated Language Models Affects Users' Views", CHI, doi:10.1145/3544548.3581196 [V] (content: memory: writing with a model biased toward a position shifted both what people wrote and their later stated attitudes, largely unnoticed).
- Persuasion: Salvi, Horta Ribeiro, Gallotti, West 2025, "On the conversational persuasiveness of GPT-4", *Nature Human Behaviour*, doi:10.1038/s41562-025-02194-6 [V] (content: memory: with personal information, GPT-4 was more persuasive than humans in debates); Hackenburg & Margetts 2024, *PNAS*, doi:10.1073/pnas.2403116121 [V] (content: memory: LLM messages were persuasive but microtargeting added little); Costello, Pennycook, Rand 2024, "Durably reducing conspiracy beliefs through dialogues with AI", *Science*, doi:10.1126/science.adq1814 [V].
- Invented facts: Dahl, Magesh, Suzgun, Ho 2024, "Large Legal Fictions: Profiling Legal Hallucinations in Large Language Models", *Journal of Legal Analysis*, doi:10.1093/jla/laae003 [V]; Magesh, Surani, Dahl, Suzgun, Manning, Ho 2025, "Hallucination-Free? Assessing the Reliability of Leading AI Legal Research Tools", *JELS*, doi:10.1111/jels.12413 [V] (content: memory: commercial retrieval-augmented legal tools still hallucinated in roughly a sixth to a third of queries).
- Unfaithful explanations: Turpin, Michael, Perez, Bowman 2023, "Language Models Don't Always Say What They Think: Unfaithful Explanations in Chain-of-Thought Prompting", arXiv 2305.04388 [V], NeurIPS 2023, doi:10.52202/075280-3275 [V] (content: abstract and memory: biasing features in prompts change answers while the chain of thought rationalises without mentioning them). *Remark.* A model's stated reasons for a proposed allocation therefore cannot be taken as the reasons that produced it; a drafted justification must be checked against the declared criteria, not trusted as a trace.
- Errors propagate to humans: the caseworker experiment (Gosciak et al. 2026, §3.6) shows wrong suggestions cut human accuracy sharply.

**5.5 Proposed safeguards and roles**
- Summerfield et al. 2024, "How will advanced AI systems impact democracy?", arXiv 2409.06729 [V], published as "The impact of advanced AI systems on democracy", *Nature Human Behaviour* 2025, doi:10.1038/s41562-025-02309-z [V] (content: arXiv abstract: epistemic, material and foundational impacts; AI can support or destabilise democratic mechanisms). Landemore 2024, "Can Artificial Intelligence Bring Deliberation to the Masses?", in *Conversations in Philosophy, Law, and Politics* (OUP), doi:10.1093/oso/9780198864523.003.0003 [V] (content: memory: AI facilitation, translation and summarisation could make deliberation at scale feasible, with risks of manipulation and loss of human authorship).
- Safeguards proposed across the literature (content: synthesis of Small et al. 2023, Tessler et al. 2026, Benavides Cantos & Garrido-Merchán 2026, and the critiques above): human final judgement; provenance of every claim to a participant's own words or a source; verification of citations; representing minority and dissenting positions explicitly rather than in a merged statement; diversity of model drafts and of prompts; testing with the actual users; disclosure that text was machine-drafted.

#### Inferences
- Useful roles for a model, ordered by risk: (1) organising what people said (clustering claims with links back to each person's words, as Talk to the City and Pol.is summaries do); (2) articulating each stakeholder's claim in the strongest form the claimant endorses, checked by the claimant; (3) finding what a disagreement turns on (the open term, the disputed fact, the weight) as a proposal for people to confirm; (4) proposing candidate principles or allocation rules for people to examine, shown alongside standard rules; (5) drafting reasons for a decision already taken by people, checked against the declared criteria and evidence. Roles a model must not take: choosing the allocation, setting weights, standing in for absent stakeholders, grading claims' merit, or producing the "official" reasons without human authorship.
- Because model judgements are framing-sensitive and diverge from human distributive preferences in both directions (more tolerant of inequality in one study, stricter in another), any model-proposed allocation should be labelled with the prompt, the model version and a sensitivity check across rephrasings, and never shown alone.
- The Habermas Machine's evidence concerns statements of common ground, not allocations of money; a statement everyone can sign may hide the distributive choice. In allocation, a model should surface the conflicts, not dissolve them.

#### Gaps
- No study was found of LLMs used inside a real public allocation process (PB, housing, grants) with measured effects on outcomes or legitimacy.
- The exact preference share and subgroup results of the Habermas Machine were not checked against the full text.
- No study was found comparing LLM-drafted with human-drafted reasons for allocation decisions on accuracy or acceptance.

### 6. Evaluation of Allocation Processes and Outcomes

#### Takeaway
After-the-fact evaluation combines process indicators (A4R-type conditions met, participation and its representativeness, appeals lodged and upheld, time to decision) with outcome indicators (who received what by group, inclusion and exclusion errors against an external benchmark, distributional summaries such as concentration indices or the equity–efficiency plane, satisfaction and legitimacy); the evidence that formal rules beat discretion is real where discretion is noisy or biased (grant review, kidney allocation, triage proxies) but formal rules have also caused the worst failures when they removed individual consideration (Robodebt, the Dutch benefits), so evaluation must test both.

#### Cited Findings
- Process–outcome frameworks: Sibbald et al. 2009 (ten elements, §1.4) doi:10.1186/1472-6963-9-43 [V]; Kapiriri & Martin doi:10.1007/s10728-009-0115-2 [V]; Barasa et al. 2015 doi:10.15171/ijhpm.2015.167 [V]; Gibson et al. 2004 doi:10.1186/1472-6963-4-25 [V].
- Distributional outcome measures: DCEA's equity impact (gains and opportunity costs by group) and equity–efficiency plane (Asaria et al., doi:10.1177/0272989X15583266 [V]; Cookson et al. 2017, doi:10.1016/j.jval.2016.11.027 [V]).
- Targeting outcome measures: inclusion and exclusion errors, coverage of the poorest quintile, and welfare-weighted measures; Aiken et al. 2022 compared regimes on exclusion errors, total social welfare and fairness across groups (doi:10.1038/s41586-022-04484-9 [V], content: abstract); Coady et al. 2004, doi:10.1093/wbro/lkh016 [V]; Kidd & Athias exclusion error figures (§2.5) [S].
- Discretion is noisy: Pier et al. 2018 (no agreement among NIH reviewers; [V], content: abstract); Kahneman, Sibony, Sunstein 2021, *Noise*, book record confirmed only through reviews, e.g. doi:10.1057/s11369-022-00251-w [S]; Kleinberg, Lakkaraju, Leskovec, Ludwig, Mullainathan 2018, "Human Decisions and Machine Predictions", *QJE*, doi:10.1093/qje/qjx032 [V] (content: memory: in bail decisions an algorithm could reduce crime or detention with no increase in racial disparities, relative to judges; it also documents selective labels); Lipsky 1980, *Street-Level Bureaucracy*, book record confirmed only through a 1980 review in *Politics & Society*, doi:10.1177/003232928001000113 [S]; Kaplow 1992, "Rules Versus Standards: An Economic Analysis", *Duke Law Journal*, doi:10.2307/1372840 [V].
- Formal rules with corrections improved fairness: KAS 2014 counting waiting time from dialysis start ([V] Israni et al.; effects content: memory); race-neutral eGFR waiting-time restoration ([S] HRSA; [S] JAMA IM 2026); Sönmez et al. reserve systems ([V] records; implementation [S]).
- Formal rules that removed individual consideration produced injustice at scale: Robodebt ([S]), Dutch childcare benefits ([S]), Ofqual 2020 ([S]).
- Contestation as an indicator: rates of appeal and of decisions overturned are used as signals of rule quality (e.g. high success rates of claimants at UK disability benefit tribunals, content: memory [M]); Robodebt shows that individually upheld appeals without systemic correction conceal rule failure ([M] content).
- Legitimacy measures: decision acceptance, trust and perceived fairness surveys (§3.7); caution that acceptance tracks outcome favourability (Esaiasson et al. 2019, doi:10.1017/S0007123416000508 [V] (online 2016)) and is higher among winners (HRC lottery survey, [V] content: abstract).
- Participation representativeness: PB turnout compared with population (Brno, [S]); sortition selection probabilities (Flanigan et al. 2021, [V]).

#### Inferences
- An honest evaluation report for an allocation round should contain at least: the eligible pool and who was missing from it; the declared criteria, weights and combination rule, with their provenance; the allocation by group (and by the groups affected parties name, not only administrative categories); exclusion errors against an external benchmark where one exists; the counterfactual allocation under one or two alternative standard rules; the appeals lodged, upheld and the revisions they caused; deviations from the rule and their reasons; satisfaction or acceptance with the caveat that it tracks outcomes.
- Rules versus discretion is not a binary: the evidence favours rules for consistency and auditability, with structured, recorded discretion for cases the rule cannot see (Alkhatib & Bernstein's gaps), and with every discretionary deviation logged and reviewable (the out-of-sequence organ lesson).

#### Gaps
- No comparative study was found that measured fairness outcomes before and after replacing discretion with formal rules in social housing or humanitarian allocation.
- No standard indicator set for "contestations upheld" across public allocation programmes was found.

### 7. What This Means for the Design

#### Takeaway
A tool that helps people allocate scarce public resources justly should be a record and a workbench of reasons, not an optimiser: it should make the pool, the criteria, the combination rule, the evidence and the dissent explicit and versioned; let every affected party see the reasons that bear on them and contest a specific reason; feed upheld contestations back into the rule; report distributional outcomes and exclusions honestly; and let a language model organise, articulate and draft under human authorship while never choosing, weighting or speaking for the absent.

#### Cited Findings
(Each design element is traced to the findings above.)

**7.1 Procedures and safeguards to build in**
- *Publicity of criteria and rules* (A4R publicity; CJEU C-203/22's "procedure and principles actually applied"; WHO SAGE transparency): every allocation round publishes its eligible pool definition, criteria, measures and proxies, weights, combination rule (weighted sum, lexical order, reserves, lottery among near-equals, points with committee) and budget, each with who set it and when — §1.1, §1.3, §1.5, §3.5.
- *Records of reasons*: for each unit and each decision, the reasons that bore on it (criterion values with their evidence and source, which rule step decided), written so a non-expert can understand and challenge them (C-203/22) — §3.5.
- *Proxy registry and audits*: each proxy (a severity score, an index of disadvantage, a predicted benefit) carries its validation and known group differences, as the SOFA, MELD and eGFR cases require — §1.5, §1.6.
- *Appeal and revision*: an affected party can contest a specific reason or datum; the response is recorded; an upheld contestation triggers review of the rule for everyone similarly placed, not only the individual case (Robodebt's unappealed losses) — §3.1, §6.
- *Individual consideration*: no all-or-nothing rules without a recorded hardship and circumstances step (Dutch benefits); the burden of proof stays with the allocator — §3.2.
- *Discretion logged*: any deviation from the declared rule is permitted only with a recorded reason and is reported (out-of-sequence organ allocation) — §1.6.
- *Participation of those affected*: representation of affected groups in setting criteria (sortition with equalised probabilities where panels are drawn; elicitation of stakeholders' own models as in WeBuildAI); report who participated against the population — §2.1, §4.3, §4.4.
- *Versioning and retroactive recomputation*: rules and data versioned so that a corrected rule can be re-run on past rounds to find who was wrongly passed over (eGFR restoration) — §1.6.
- *Lotteries as declared tie-breakers*: a lottery among options the procedure cannot distinguish, with the band defined by uncertainty (SNSF's credible intervals), seed published — §2.3.
- *Side-by-side rules*: show the allocation under the chosen rule and under standard alternatives (greedy, proportional/Equal Shares, maximin, utilitarian, reserves) so the distributive choice is visible — §2.1, §1.5.
- *MCDA hygiene*: explicit budget and opportunity cost, swing weights tied to scales, double-counting checks, sanity tests (Morton) — §4.1.

**7.2 What a language model should and should not do**
- Should (each output marked as proposed, with model, version and prompt kept): cluster and summarise contributions with links to each person's words (Pol.is/Talk to the City practice, §4.4, §5.1); restate each stakeholder's claim for that stakeholder's confirmation; identify what a disagreement turns on (an open term, a contested fact, a weight) as a proposal; propose candidate principles and rules to be examined beside standard ones; draft reasons for decisions people have taken, verified against declared criteria and evidence; translate and simplify explanations for affected people, checked by a human.
- Should not: decide or rank allocations; set or suggest numeric weights as defaults; act as a simulated stakeholder or stand in for absent groups (§5.2); grade the merit of claims; produce official reasons without human authorship; merge dissent into a consensus statement without keeping dissent visible (§5.1 critiques); persuade participants toward a position (§5.4).
- Must be accompanied by: provenance for every factual claim with citations checked (§5.4 hallucination studies); sensitivity checks across rephrasings (§5.3); disclosure to participants that text was machine-drafted; evaluation with actual users of the effect of model suggestions on human accuracy (§3.6).

**7.3 Evaluation the tool should report**
- The report outline in §6 Inferences: pool and missing; declared criteria, weights, rule and provenance; allocation by group (including groups named by affected parties); exclusion errors against an external benchmark, or a statement that none exists; counterfactual allocations under alternative rules; appeals lodged, upheld and the revisions they caused; logged deviations; participation representativeness; satisfaction or acceptance with the caveat that it tracks outcomes.

**7.4 Open datasets and published cases for honest demonstrations**
- Pabulib PB instances (2,210, mostly Polish cities, also Amsterdam and Budapest), `.pb` format — [pabulib.org](http://pabulib.org/) [V]; licence not stated: confirm terms before redistribution.
- Pol.is open data (vTaiwan Uber, Brexit consensus, Canadian electoral reform, Seattle $15 per hour, NZ biodiversity), CC BY 4.0 with attribution to The Computational Democracy Project — [compdemocracy/openData](https://github.com/compdemocracy/openData) [V].
- Equal Shares elections with ballots and explanations (Wieliczka 2023, Aarau 2023) — [equalshares.net](https://equalshares.net/elections) [S]; terms not checked.
- Published COVID-19 reserve allocations (Tennessee 85/10/5; Massachusetts 20% reserves) as worked cases of reserves — §1.5 [S].
- US organ allocation policy documents and OPTN/SRTR public data (for rules, not individual data; SRTR standard analysis files require an application and data use agreement, content: memory [M]).
- Research funding: HRC NZ Explorer Grants procedure and the SNSF Bayesian ranking (with Heyard et al.'s code and data if released, not checked) — §2.3.
- Togo Novissi targeting study's comparison of regimes (replication materials not checked) — §2.5.
- Health priority setting: NICE technology appraisal documents (public committee papers with reasons) and Norway's published criteria — §1.1, §1.3.
- Algorithmic failure cases as negative demonstrations (Robodebt, toeslagenaffaire, Ofqual 2020), from the official reports — §3.

**7.5 What a tool must never claim**
- That its allocation is optimal or just: it shows the consequences of declared criteria and rules; justice is the people's judgement on the record.
- That procedural safeguards will make those who lose accept the outcome (outcome favourability dominates acceptance, §3.7).
- That a model's proposal or drafted reasons reflect the model's actual reasoning (unfaithful explanations, §5.4) or the views of groups it simulates (§5.2).
- That human review makes a system safe (selective adherence, plateau effects, §3.6).
- That exclusion errors are low when the excluded are absent from its data (§2.5).
- That a proxy is neutral without a validation by group (§1.5, §1.6).
- That any threshold, weight or standard it ships is a jurisdiction's or institution's own unless a scheme states and sources it.

#### Inferences
- The infrastructure's existing pieces map onto these needs: claims problems and the Talmud rule, social choice and Equal Shares-type rules, matching and reserves, multi-criteria appraisal and cost–benefit analysis, decisions under ambiguity, argumentation frameworks and the reading of open terms under every reading (for "what the disagreement turns on"), evidence appraisal with the value of information, and an archive with sealed journals (for records of reasons, contestations and revisions). The gap the survey identifies is the loop from reasons to contestation to revision, and the honest reporting of exclusion.
- A model's place is upstream of judgement (organising and articulating) and downstream of it (drafting reasons for human signature), never at the point of choice.

#### Gaps
- No precedent was found of a deployed public allocation tool that implements the full loop (published reasons per unit, contestation of specific reasons, systemic revision, distributional reporting); the design will be novel and should be evaluated as such.
- Terms of use for several datasets (Pabulib, equalshares.net, Heyard et al.'s materials, Novissi replication data) were not verified.

## Part VI. What Was Built

*Written 2026-11-20, when the design this survey served was built ([ADR 0136](../../../development/decisions/0136-allocation-under-scarcity.md)). It reports the infrastructure's work, not the literature, and carries no verification markers: every claim in it is checked by the code's own tests, cited by their counts.*

### 1. The Design Taken from the Survey

| The survey's finding | What was built | Where |
|---|---|---|
| Allocation is a composition of declared rules, not one maximisation (§1.1) | a round: stages of seven kinds applied in turn to what remains, each naming its principle and who declared it, with its share of what remains | the allocation domain's `rounds` |
| Priority classes are the only admissible asymmetry under consistency and composition; reserves let several values govern without one score (§1.4) | priority classes within rounds; nomos's reserve systems with their three axioms checked and every precedence order's beneficiaries | `rounds`; nomos `allocation` (`reserves.hpp`) |
| What and how much: claims rules, equalisation, banded points, proportional voting, a floor under a ceiling (§1.5) | TAL, Piniles, adjusted proportional, duality, priority classes; sufficiency first, Porto Alegre's banded points, equalisation with its envelope; the Method of Equal Shares and Phragmén with EJR; the doughnut's floor and ceiling | nomos `allocation`; the domain's `incidence` |
| Principles are plural orders; their conflicts are shown, not resolved by a weighted sum (§1.2, Part I §7.2) | principles as orders (total, prioritarian, leximin, shortfall, opportunity); dominance; options compared under all at once with their undominated set, pivots and flips; a weighted aggregate only where weights are declared | nomos `welfare`; the domain's `verdicts` |
| Procedure confers legitimacy, not correctness; the checkable criteria; contestation must be able to change the outcome (§1.6) | a sealed record in archeion's journal; appeals on a datum, a parameter or a stage, heard for everyone; lotteries from committed seeds; Leventhal's rules and accountability for reasonableness decided, relevance and ethicality left to people | the domain's `procedure` |
| In the city the question is who finally gains (§1.7) | capitalisation with exact intervals over declared responses; re-spending and LM3; targeting | the domain's `incidence` |
| Generative justice's circulation has a counterpart in the Hodge decomposition (Part IV §6, the survey's proposal) | the return ratio and extraction share of each generator; the circulating share of a network's flows on the economics domain's decomposition, by kind | the domain's `incidence` |
| Language models may organise, articulate and draft, but must not choose (§1.9) | three roles at the edge: stages proposed without numbers, an objection restated as an appeal, a statement of reasons refused if it states a number the result does not hold; each a proposal of grade C | the API's `science.allocation` |
| Every allocation should report who receives, who is excluded, who pays and who finally gains, under which principles, how sensitively and by what procedure (§1.10) | the page of the Urban Studio's plug-in *Allocation and justice*: the round as a channel, the options' order and stakes, the record with its seals | the plug-in; the visualization layer's `allocation` |

### 2. The Checks

- **nomos:** `allocation` 37 and `welfare` 13, held to closed forms and published cases (the Talmud's estates, Pathak et al.'s hard reserve left idle, a majority and minority under utilitarian greedy and equal shares) and to axioms on random instances.
- **The domain:** 84, among them conservation and non-negativity on 200 random rounds, a lottery's first places at w_i/Σw over 4,000 seeds, and capitalisation's interval held to a grid over the box of responses.
- **The API:** 30; **the data library:** 10 (Birmingham's 41.7% income-deprived and Blackpool's 26.0% employment-deprived, as the IoD 2025 release's Table 7); **the visualization layer:** 19; **the Urban Studio:** the plug-in's checks in its suite.

### 3. What the First Cases Show

The amounts, claims, bands and responses are illustrative declarations; the data are real.

- **Warsaw's participatory budget, 2023** (Pabulib; 138 projects, 78,904 voters, PLN 30.3 m). Utilitarian greedy reproduces the city's selection of 22 projects exactly. Under it 7.9% of voters saw none of the projects they approved funded; under the Method of Equal Shares 4.7% (60 projects), under sequential Phragmén 4.4% (84 projects). By neighbourhood, Białołęka's share with nothing falls from 16.6% to 12.9% under equal shares, Ursynów's from 4.1% to 1.8%. With 78,904 voters, EJR cannot be checked by enumeration, and the tool says so.
- **England's districts** (IoD 2025, OGL). A fund of £1 bn whose first 85% answers claims of £80 for every income-deprived person (claims of £1.06 bn, so rationed) and whose rest goes by Porto Alegre's banded points. Read per income-deprived person under leximin, prioritarian (ε = 1) and the least for a kind of authority, proportional rationing is undominated among the six distinct options; equal awards to districts is dominated (its least £12.74 a person against £66.05). The banded points, which give a small district as many points as a large one in the same bands, favour the small per person (the City of London £359, Birmingham £66). A fund given to the most deprived decile of neighbourhoods by population reaches income-deprived people with 55% of its money, while 76% of England's 13.2 m income-deprived people live outside that decile; with three deciles, 43% reaches them and 44% are outside.
- **The City of Melbourne's areas** (the 2021 Census, CC BY 4.0). A care fund of A$2 m, three quarters served area by area from the highest share needing assistance (Kensington, Parkville, South Yarra West), the rest by people on low incomes. Against the order that gives by low income first, it leaves fewer people needing assistance in areas short of A$1,000 each (0.63 against 0.79 of them) but less to the least served per person (A$44 against A$177): the choice turns on which principle is given precedence, and the page says so.

### 4. What Remains

- **Time** (§1.4, Part I §4): rotation, queues, histories of who was passed over, and rules revised by monitoring (Ostrom), as a planned module `seasons`.
- **A repertoire searched** (Part IV §4): alternatives beyond rules and orders (stages' kinds, shares and thresholds) kept as a quality-diversity archive on heuresis, as input to deliberation.
- **Contestation as argument** (Part I §7.5, Part II §8.3): appeals as attacks in apodeixis's frameworks, the standing of each decision recomputed.
- **Incidence in equilibrium** (Part III §3): capitalisation through the urban-science domain's housing market rather than declared elasticities.
- **Deliberation** (Part V §4): Pol.is data, panels drawn with equal probabilities as quotas allow, and stakeholders' own models elicited.
- **Datasets' terms:** Pabulib's licence remains unstated; its file stays in the cache.
