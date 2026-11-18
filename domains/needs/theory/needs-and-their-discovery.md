# Needs and Their Discovery

*Status: draft (2026-11-18; ADR 0125).* The theory note of the needs domain: what a need is, how a statement of one is judged against data, and how needs are searched for, by evolution, without the search deceiving itself.

## 1. A Need Is a Gap Judged by a Standard

The concept of need in social policy has one shape under its many names. A need is a gap between the state of some people and a standard, with respect to something that would close it (Witkin and Altschuld 1995, ch. 1; Kaufman's discrepancy model). The standard says whose judgement makes the gap a need. Bradshaw (1972) named four kinds of standard, and they remain the working taxonomy of needs assessment:

- **Normative need:** the state falls below a level that an authority or an expert declares (a care visit a week for those who cannot dress alone; rent below 30% of income).
- **Felt need:** the people say they want it (a survey's answer).
- **Expressed need:** the want is turned into demand (a waiting list, a request, a visit), and demand exceeds what is provided.
- **Comparative need:** the people lack what others like them, elsewhere or otherwise, have.

What is needed is kept apart from the means of meeting it. Max-Neef (1991) distinguished needs from *satisfiers*. Doyal and Gough (1991) set basic needs (health, autonomy) above intermediate ones (food, housing, care, safety, education), which are universal; how a society meets them varies. A need is stated about a *domain* of satisfaction (care, health care, housing, mobility, food, green space, safety, income), never about one provider.

*Remark.* A need is therefore not observed. It is a judgement of observed states under a declared standard. Two analysts with different standards find different needs in the same data, and both are right about their standards. The domain never decides a standard; it makes each one explicit, and computes what follows from it.

## 2. A Statement of Need

**Definition 1** (statement). A statement of need is a sextuple $N = (G, D, I, \sigma, w, P)$:

- $G$, the **group**: a predicate on units (people, households, places), a conjunction of conditions on their attributes ($\text{age} \ge 75$, $\text{tenure} = \text{renting}$, $\text{district} \in \{\dots\}$);
- $D$, the **domain** of satisfaction, from a declared vocabulary;
- $I$, the **indicator**: a column of the data measuring the state, with the direction in which it is worse;
- $\sigma$, the **standard**, one of the four kinds:
  - normative, a level $\tau$;
  - comparative, a reference group $G'$ (its complement within a frame, by default);
  - expressed, a demand column against a provision column;
  - felt, a column of stated want and a level;
- $w$, the **weight** of a unit (the people a row stands for);
- $P$, optionally, the **provision**: a column of what is provided, and the levers on it.

The statement is written in the domain's language (§6), so that a person, a program and a language model write needs in one form, and the domain reads every one of them, or refuses it with the place and the reason.

**Definition 2** (gap). An indicator is a ratio of two sums over a group's units, $R_G = \sum_G w y / \sum_G w x$, with $w$ the table's weight. For a value $v$ standing for $t$ people, $y = t v$ and $x = t$. For a share counted as $k$ of $n$, $y = k$ and $x = n$. The gap of $N$ is

- normative: $\Delta = R_G - \tau$;
- comparative: $\Delta = R_G - R_{G'}$;
- expressed: $\Delta = \sum_G w\,(\text{demand} - \text{provision}) / \sum_G w\,\text{demand}$, the share of the demand that goes unmet;
- felt: $\Delta = R_G - \tau$, as normative, on the stated column.

Each is signed so that a positive gap is worse, and a statement's direction must agree with its indicator's. Its **coverage** is $\sum_G w x$, the people it concerns. Its **size** is $\Delta$ times coverage, people-units of unmet need.

**Inference.** By design (the default), the clusters are the primary sampling units. The variance is the ultimate-cluster estimator of the ratios linearised (Wolter 2007, §6.5; peira's surveys), on clusters less one degrees of freedom. For a comparative gap it is the difference of the two ratios, $u = (y_G - R_G x_G)/X_G - (y_{G'} - R_{G'} x_{G'})/X_{G'}$, so that the covariance of a group and its reference living in the same areas is kept. By binomial reading, the units are independent: tyche's score test of a share against a level, Newcombe's interval for a difference of shares, Welch's test for values. The test is one-sided, for the statement claims a positive gap.

## 3. Evidence, and the Standing of a Need

A statement is supported or not by tests. Each test is a function of the statement and the data that answers with a verdict (holds, fails, undecided) and its numbers.

1. **The gap and its interval.** $\Delta$ with a confidence interval: Welch's for a difference of means, Wilson's for a share, a weighted bootstrap otherwise.
2. **Replication on data kept aside.** The data are split once, by a declared key, into parts for discovery, validation and confirmation (§5). The statement is judged on the confirmation part as on the first, once, by an online procedure in the order statements are admitted.
3. **Stability.** The share of bootstrap resamples (of units, or of the declared clusters) in which the gap keeps its sign and its significance.
4. **Causal support,** when a provision is declared. Within $G$, the effect of provision on $I$ is estimated with the adjustment set that a declared causal graph gives (the back-door criterion), or within units over time (fixed effects). A need that provision would meet has an effect of the expected sign. One that provision does not touch is a gap of another kind.
5. **Simulation,** when a model of seeking is declared. Agents of $G$ seek satisfiers within reach of capacity (a model of their behaviour, with its parameters declared). The need is supported when the unmet demand the model leaves in $G$ exceeds its standard. The model is a mechanism offered for the gap, and the test says whether the mechanism can produce it.
6. **Rules.** Declared normative rules (when a person is entitled to what) derive needs by forward chaining. A statement consistent with the rules is supported by them; one that contradicts them is flagged.

**Definition 3** (standing). The claim that the group has the need and the objections the tests raise are the arguments of a Dung framework, and the standing is read from its grounded extension (apodeixis). The arguments are:

- N, the need;
- D, no gap on discovery, attacking N;
- V, the validation part not upholding it, attacking N;
- S, instability, attacking N;
- C, confirmed on the part kept aside, attacking V and S, for fresh data outweigh screens;
- X, the opposite gap there, attacking N and C.

The standings follow:

- *robust*: N and C are in;
- *supported*: N is in without C (not yet confirmed, or not confirmed);
- *unsupported*: N is out;
- *refuted*: X is in.

Causal support, the simulation and the rules qualify a standing and do not attack it: a need may exist that provision would not meet, that no mechanism offered explains, or that no rule recognises.

A need's standing is always stated with the standard it was judged by.

## 4. Searching for Needs

The space of statements is large: a group is any conjunction of conditions, and each comes with its domains, indicators and standards. Needs are therefore *searched* for. A search keeps a **population** of statements and improves it generation by generation:

- **Generation.** New statements come from generators of several kinds:
  - enumeration of the language, breadth-first or depth-first to a size;
  - sampling from it;
  - subgroup discovery by beam search, where the quality of a group is its gap times a power of its coverage (Klösgen 1996; Wrobel 1997);
  - seeds from the data: the strongest single conditions, the splits of a regression tree, clusters of the worst-off;
  - derivation from declared rules;
  - places a simulation leaves unmet;
  - a language model's proposals, read by the domain like any other statement.
- **Variation.** A statement is changed: a condition added, dropped, widened, narrowed or moved to another attribute; the standard, indicator or domain changed; two statements' groups joined or intersected. A model can be asked to refine one in the light of its evidence.
- **Evaluation** on the discovery part, by objectives: the gap, standardised; the coverage; the stability; simplicity (fewer conditions); novelty (the distance of the group's members from those of statements already kept); causal support where declared.
- **Selection, survival and diversity:** strategies interchangeable at every stage (heuresis).
- **The archive:** statements that reach a threshold are kept with their evidence. A kept statement takes part in later generations as a parent. It is *retired*, recorded with the reason, when it has produced nothing better for a declared number of generations, or when its evidence no longer holds on new data.

*Remark.* Evolution over hypotheses is old. Genetic algorithms have been used for rule and subgroup discovery for decades (Freitas 2002; del Jesus, González, Herrera and Mesonero 2007). What is new here is only the composition: needs as statements in a language, tested by several kinds of evidence and by data kept aside, with generators of every kind in one loop.

## 5. A Search That Does Not Deceive Itself

A search that tries ten thousand statements will find some with large gaps by chance alone, and evolution finds them faster than chance (the garden of forking paths; Gelman and Loken 2014). The safeguards below are part of the method, not options. The first draft of this note had two of them wrong, as the survey of the literature (`research/literature/reports/Evolutionary exploration and validation of needs.md`, §12.2) showed; they are stated here as built.

1. **Three parts, apart.** The data are cut once into three parts of whole clusters, by a keyed hash of each cluster's name, never by its data (Cox 1975; Rinaldo, Wasserman and G'Sell 2019):
   - discovery (half), which the search sees freely;
   - validation (a quarter), which it may consult only through a reusable holdout;
   - confirmation (a quarter), never seen by the search.

   The first draft had two parts, and kept statements confirmed on the second part as parents of later generations. Then confirmation outcomes steer the search, the adaptive reuse that Dwork et al. (2015) show invalidates held-out tests.
2. **Admission through a reusable holdout.** A statement is admitted to the archive, and so becomes a parent, only after Thresholdout (Dwork et al. 2015) answers it:
   - the validation part's effect is answered only when it differs from discovery's by more than a noisy threshold, and then with noise, at a unit of a declared budget;
   - otherwise the discovery value is answered;
   - each statement is asked once, and the budget's use is recorded.

   *Caveat.* Thresholdout's guarantee is for bounded statistical queries. An effect clipped to [−1, 1] spreads is close to one, not one, so the guarantee is approximate.
3. **Confirmation in order, decided online.** Each statement admitted is tested once on the confirmation part, in the order of admission, and the decision is made then and never revised. The first draft re-ran Benjamini and Hochberg's procedure over every statement ever admitted. That can withdraw labels already given, and needs independence or positive dependence, which tests of overlapping groups on one part do not have. The procedures are tyche's online ones:
   - e-LOND (Xu and Ramdas 2024) by default, which controls the false discovery rate under any dependence, on e-values calibrated from the p-values;
   - or LOND, LORD++, SAFFRON and α-investing, each with what it assumes stated.

   Each test is registered in a journal sealed by archeion's hash chain before the part is read, and its outcome written after, so that whoever keeps the journal's last seal can verify the order.
4. **Nothing of the confirmation reaches the search.** The judge, the operators' credit and retirement read discovery and the holdout alone. A check holds this exactly: with the confirmation part's outcomes permuted, the population and the archive are the same, generation by generation.
5. **Stability under resampling.** The share of resamples of clusters in which the gap keeps its sign and clears z = 2 (after Meinshausen and Bühlmann 2010). *Caveat.* Without a calibrated threshold and stability selection's conditions it is a screen, not a control of error; the control of error is the confirmation's.
6. **Productive and distinct statements.** Two more screens, found needed when the archive filled with variants of one need:
   - every condition of a group (and of its frame) must carry a gap of its own within the rest of the group, after Webb's productive rules (2007), else it says the same need in more words;
   - a statement within a least distance of a kept one (weighted Jaccard over the people it concerns, with what it says) is kept only if it is stronger, and the weaker is then retired as superseded.

   *Caveat.* At z = 2 the productivity screen still lets through some conditions that carry no gap, when many cut points of a noisy attribute are tried. The confirmation then judges a true but wordier statement.
7. **Testable by design.** A group must reach people in a least number of clusters of discovery (five by default). One that lives in a single cluster of a part has no standard error there by the clustered design, and cannot be confirmed.

*Caveat.* The confirmation part protects against the search; it does not protect against the data. A need found in a census is a need of the population as its questions measure it. A full count read as a sample is a sample of the process that produced it, with the areas as its clusters. A gap is not a cause: a group's unmet need may follow from something the data do not hold. Causal support (§3.4) is a test of the declared graph, not of the world. And the archive's false discovery rate counts statements, not distinct needs.

## 6. The Language

Statements are written in a small language, read by morphe's BNF reader, so that a reading that fails says where and why. In outline:

```
need ::= "need" domain "for" group [ "within" group ] "by" standard
group ::= "everyone" | condition { "and" condition }
condition ::= attribute ( "=" | "!=" | "<" | "<=" | ">" | ">=" ) value | attribute "in" "{" value { "," value } "}"
standard ::= "normative" indicator ( "above" | "below" ) number
           | "comparative" indicator ( "higher" | "lower" ) [ "than" group ]
           | "expressed" demand "over" provision
           | "felt" indicator ( "above" | "below" ) number
```

`within` sets the frame: the comparison group of a comparative standard is the frame less the group.

## 7. References

Bradshaw, J. (1972), The concept of social need, *New Society* 30, 640–643. Dwork, C., Feldman, V., Hardt, M., Pitassi, T., Reingold, O. and Roth, A. (2015), The reusable holdout, *Science* 349, 636–638. Javanmard, A. and Montanari, A. (2018), Online rules for control of false discovery rate and false discovery exceedance, *Annals of Statistics* 46, 526–554. Rinaldo, A., Wasserman, L. and G'Sell, M. (2019), Bootstrapping and sample splitting for high-dimensional, assumption-lean inference, *Annals of Statistics* 47, 3438–3469. Webb, G. I. (2007), Discovering significant patterns, *Machine Learning* 68, 1–33. Wolter, K. M. (2007), *Introduction to Variance Estimation*, 2nd ed., Springer. Xu, Z. and Ramdas, A. (2024), Online multiple testing with e-values, *AISTATS*, PMLR 238, 3997–4005. Cox, D. R. (1975), A note on data-splitting for the evaluation of significance levels, *Biometrika* 62, 441–444. del Jesus, M. J., González, P., Herrera, F. and Mesonero, M. (2007), Evolutionary fuzzy rule induction process for subgroup discovery, *IEEE Trans. Fuzzy Systems* 15, 578–592. Doyal, L. and Gough, I. (1991), *A Theory of Human Need*, Macmillan. Freitas, A. A. (2002), *Data Mining and Knowledge Discovery with Evolutionary Algorithms*, Springer. Gelman, A. and Loken, E. (2014), The statistical crisis in science, *American Scientist* 102, 460–465. Klösgen, W. (1996), Explora: a multipattern and multistrategy discovery assistant, in *Advances in Knowledge Discovery and Data Mining*, AAAI/MIT Press. Max-Neef, M. (1991), *Human Scale Development*, Apex. Meinshausen, N. and Bühlmann, P. (2010), Stability selection, *J. R. Stat. Soc. B* 72, 417–473. Witkin, B. R. and Altschuld, J. W. (1995), *Planning and Conducting Needs Assessments*, Sage. Wrobel, S. (1997), An algorithm for multi-relational discovery of subgroups, *PKDD*, 78–87.
