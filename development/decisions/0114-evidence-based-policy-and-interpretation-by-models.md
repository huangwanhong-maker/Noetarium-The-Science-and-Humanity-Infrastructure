# 0114. Evidence-Based Policy as a Domain, Decisions in One Step, and Interpretation by Models

- **Status:** Accepted, provisionally (2026-11-15). Amends [0112](0112-apodeixis-and-tekmerion.md) (tekmerion's `policy` renamed `warrant`; its value of information moved to tyche).
- **Date:** 2026-11-15
- **Decided by:**
  - **The request.** The user, before a night's work: "we said, sometime, derivation need 'interpretation', interpretation may does by some model, please also implement. And EBPM as a decision theory's domains? Also decide implement. I not sure, anyways, please try your best this night." Earlier (ADR 0112) they had held that a norm "can be abstract, we can interpret in different method, like a classical statistics model, e.g., beyesian, or a POMDP based model, Even an LLM".
  - **The design.** Claude, here, deciding as asked; the user had not yet seen it.

## 1. Context

ADR 0112 founded apodeixis (derivability under an interpretation, Γ ⊢_ι φ) and tekmerion (evidence and its interpretation). Three things were left open. Interpreters were six functions with no common interface, so a method written elsewhere (a POMDP, a model at the edge, a person) could not join them, and they ignored the interpretations that arguments leave admissible. tekmerion's `policy` module held grading, "it will work here" and the value of information for a normal net benefit, while evidence-based policy as a whole had no home. And tyche, whose scope names statistical decision theory, held POMDPs but not the single decision beneath them.

The user's question, whether evidence-based policy is one of decision theory's domains, has a precise answer. Its decision is decision theory's: adopt, reject or study first, under uncertainty about an effect, with the value of information. But between the studies and the decision stands an argument, not a calculation: a theory of how the intervention works, the evidence for each link, and whether it will work in a place. And its objects (interventions, theories of change, places, programmes) name things in society, which the infrastructure's test of placement (CLAUDE.md §3.7) gives to a domain.

## 2. Decision

1. **Decisions in one step, in tyche** (tyche decision 0024). `decision` gains decision tables chosen among under risk, ignorance and ambiguity (expected utility; maximin, maximax, Hurwicz, Laplace, minimax regret; credal sets with Γ-maximin, α-maxmin, interval dominance, maximality, E-admissibility), decision trees folded back, the value of perfect and sample information by preposterior analysis and for a normal net benefit, and stochastic dominance with certainty equivalents.
2. **Interpretation by any method, in tekmerion** (tekmerion decision 0004). An interpreter answers with a *reading* of the norm (a law over the interpretations, a credal set, a choice, a proposal, or a deferral), which composes with apodeixis's D(φ) and the admissible set. A *panel* of interpreters is one credal set: its bounds are attributed to the interpreters at each, and nothing is averaged unless weights are stated (the opinion pool). `weigh_case` is Γ ⊢_ι φ weighed. New methods: a decision over purposes believed only within a credal set, and a POMDP over the sequence of cases. In the POMDP the purposes are hidden, the readings are acts, the responses to them are observations and drift renews the purpose; it is solved by tyche's HSVI. `policy` becomes `warrant` (grading and transfer, general to any field), and a grading scheme's factors can now be read off a synthesis by declared rules.
3. **A model at the edge.** The API composes apodeixis and tekmerion (`science::interpretation`, with an engine of the API's own) and asks a language model for readings (`science.interpret`). The model is given the case, its points and what the question states. It answers under a JSON schema of the declared readings, and the answer is checked again and accepted as a *proposal* of grade C by the model that answered, or refused, never repaired. A proposal weighs nothing in a panel; its readings are fixed only by arguments. Claude is asked through the Anthropic SDK (Opus 5.5, adaptive thinking, structured output, the API's server-side fallback on a declined request); a stand-in answers in the checks, which call no model.
4. **The policy domain.** A field at height 0 on the foundations, C++ from the start:
   - `change`: theories of change, with the weakest link, the strongest path, gaps on every path, support factors, and Wright's rule with its enclosure;
   - `evidence`: studies pooled by peira and graded by tekmerion;
   - `transfer`: Cartwright and Hardie's argument with the support factors of the theory's path, and peira's transport;
   - `options`: a net present value normal in the effect, by nomos's discounting, and the value of knowing which option is best;
   - `adoption`: adopt, reject or study first, with the study worth doing and robustness over the worth of a unit of the outcome.

## 3. Alternatives Considered

- **Evidence-based policy within tekmerion.** It was there, as `policy`. Rejected: interventions, theories of change and places name objects of society, and the decision is tyche's; tekmerion keeps what is general to evidence (`warrant`).
- **Within economics or political science.** Economics' appraisal is nomos's and would be reused, not owned. Political science (states, elections, conflict) is about how policy is made, not whether an intervention is warranted. A domain of its own lets either compose it.
- **A foundation of decision theory.** Rejected in tyche's decision 0024: one-step decisions are the theory beneath its POMDPs.
- **A model called from the foundations.** Rejected: the foundations are pure (CLAUDE.md §3.3), and a model's answer must enter as data with its grade.
- **The panel averaged.** Rejected as the default: an average presumes weights no one stated.

## 4. Consequences

- **Checks:**
  - tyche `check_tyche_choice` 24 and `check_tyche_trees` 24;
  - tekmerion 130 at the decision, 138 with derived grading in `warrant` since;
  - the API's `check_interpret.py` 23;
  - the policy domain 53.
- **Name.** The domain's name, `policy`, is provisional; economics keeps a module of the same name (decisions on a belief about the business cycle, ADR 0074), which is unrelated.
- **Not built:**
  - `monitoring` and `programmes` in the domain;
  - adopting with research, and adopting only in research;
  - correlated options;
  - a theory of change stated as a case of apodeixis;
  - a work, and the domain's composition in the API;
  - a model reading texts for readings with each passage checked;
  - interpreters that read a policy's own norms.
