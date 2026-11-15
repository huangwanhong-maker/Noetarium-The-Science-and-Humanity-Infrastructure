# 0122. The Policy Studio: A Question of Policy Worked from Its Theory of Change to the Decision

- **Status:** Accepted, provisionally (2026-11-16). The work's name, colour and port are provisional.
- **Date:** 2026-11-16
- **Decided by:**
  - **The request.** The user had named two works as important (ADR 0116): a workbench for law and one for "social analysis and policy making". When the Law Studio was finished (ADR 0121) they asked: "EBPM one finished? If not finish, please finish it."
  - **The design.** Claude, here, on the policy domain of ADR 0114 and the pattern of the Law Studio, as ADR 0121 had settled it with the user: two works sharing an evidence layer; for practice; models extensible; a language model allowed, which proposes and never decides.

## 1. Context

The policy domain (ADR 0114) held the methods of evidence-based policy making in five modules: theories of change, the evidence for a claim, the argument for a place, options, and the decision to adopt, reject or study first. Three things stood between it and a person with a question.

- **No composition.** Each module answered its own call. Nothing took a whole question and worked it through, so that a change to one study could be followed to the decision.
- **No drawing.** A theory of change is a graph whose links differ in how far they are shown. Nothing drew it, nor a forest of studies, nor the worth of a study over its size.
- **No work.** ADR 0114 listed "a work for law or policy" and "the policy domain's composition in the API" as not built.

Two findings of ADR 0114 shape the work. The decision is decision theory's, but an argument stands between the studies and the decision; and no scheme, threshold or worth is any institution's until a caller states it.

## 2. Decision

A work, the **Policy Studio**, on the shared studio host and page, with what it needs placed in the layers beneath it. The policy domain is not changed: the work adds no method.

1. **The API composes the domain** in `science/policy.hpp`, with two operations.
   - `theory` reads a theory of change from the domain's written form, or answers every line that does not read.
   - `appraise` takes the whole question (`noetarium.policy/1`) and answers its parts, each answered or said why it cannot be, so that one part's fault does not stop the others:
     - each body of evidence pooled by peira and graded by the declared scheme, every step said, with each study's interval and weight, the pool with each study left out, and each stratum pooled;
     - the theory with each body's evidence set on its claim, the strongest path, its weakest link, the gaps on every path, and Wright's rule where every claim states an effect;
     - the argument for the place, with each support factor of the strongest path by its state there, and the effect to expect, carried over or transported through strata;
     - the options appraised and compared;
     - the decision, with the worth of a study over its size and robustness over bounds on a unit's worth.

   In Python, `science.policy` asks the engine, reads a pasted table of studies, and asks a model for a draft.
2. **A question holds everything the engine is asked.** The theory, the scheme and its rules, the studies, the place, the costs, the rates and the worths are in one document. Nothing is read from a file, and a question can be appraised as it would be without being kept.
3. **The page writes the theory and never reads it.** A theory is kept in the domain's written form. The page changes the structure the domain's reader answered and writes it back; the reader reads it again. A change that makes no theory is refused by the domain and not by the page.
4. **The visualization layer** gains `policy`:
   - a theory of change laid out in layers from the intervention to the outcome, a claim that passes over a layer drawn around what stands there, each claim's standing told by words and pips as well as by its line;
   - a forest plot, intervals compared, the worth of a study over its size, the decision over the worth of a unit;
   - a theory as a document: written back, and changed by operations that leave the old one as it was.
5. **A model proposes and never decides** (ADR 0121). A model may draft a theory of change from a description. It is asked for states and causal claims only, never an effect or a grade, since a model is not a source. Its draft is written in the domain's form and read by the domain's reader; one that does not read is refused, never repaired. Every claim of a draft stands as not shown, and a draft is the theory only when a person takes it.
6. **The work** has five views, each drawn as its subject:
   - *Theory:* the question as a sentence, and the theory drawn beneath it; a state or a claim opens where it stands; the claims one by one, the strongest path first; the written form with its lines that do not read.
   - *Evidence:* a body as a forest of its studies, with the ladder of its grade step by step, what a rule read told from what a reader assessed; studies pasted as a table.
   - *Here:* the crossing from there to here, premise by premise; the strata and the place's mix of them.
   - *Options:* each option a ledger; net present values as intervals.
   - *Decision:* the three courses with what each is worth; the study that could be run; the worth of a unit and whether the decision holds over its bounds.
7. **A subject is a plug-in** bringing templates and grading schemes.
   - *health:* the thirteen trials of the BCG vaccine gathered by Colditz et al. (1994), with their published counts, read for an invented district.
   - *schooling:* free school meals, a theory of two paths with a body on every claim. Every study in it is invented and says so.
   - Two schemes: four levels written after GRADE, and a plainer one of three levels. Each says that it is an example and no institution's own text.

## 3. Alternatives Considered

- **A plug-in of the Law Studio.** Rejected with the user in ADR 0121: a case and a theory of change want different pages.
- **A plug-in of the Urban Studio**, which already holds *Appraisal and futures* (ADR 0108). Rejected: that plug-in runs tools on tables. A question of policy is one document worked as a whole, and wants views of its own.
- **Reading the written form in the page**, to edit a theory at once. Rejected: two readers of one form drift apart. The round through the domain's reader takes a fifth of a second.
- **A model that proposes effects or grades.** Rejected: it would enter the theory as evidence with no study behind it.
- **A model that finds studies.** Not built. A study it named would have to be found in a register, as a passage is found in its document, and no register is reached yet.
- **An example on a current policy question.** Rejected for the first one: the BCG trials are public, small, long settled as data, and already a fixture of peira. The place they are read for is invented, so that nothing here is taken for advice.

## 4. Consequences

- **Checks:** the API's 38 for the composition (a stand-in for the model); the visualization layer's `policy` (36 of its 678); the studio's 51. The page was driven in headless Chrome through three questions (26 steps, not in the suite), once more with a real model.
- **What the BCG question shows** (real trials, an invented place and invented costs):
  - the trials pool to a risk ratio of 0.489 [0.344, 0.696], as metafor reports them, and are graded low under the example scheme: inconsistency lowers them two steps, the size of the effect raises them one, and the reader's judgment of allocation lowers them one;
  - the question, whether fewer cases follow from the *programme*, is not shown: the trials are of vaccination, and whether the programme reaches the newborn has no evidence;
  - carried to a place at low latitude through the five trials below 30°, the ratio to expect is 0.688, not 0.489;
  - on the example's costs the decision is to adopt, and no study of the sizes considered is worth its cost and its delay; the decision turns on the worth of a case averted, which switches it within the bounds declared.
- **The rule is expected worth, not significance.** A claim graded low may be adopted, and a well-shown one may be worth studying first. The page says so where the decision is drawn.
- **A scheme is a declaration.** Graded by the plainer scheme, the same studies stand on other levels, and a question shown under one is not shown under the other.
- **Not built:**
  - the systematic review that finds and screens the studies (tekmerion's `reviews`, ADR 0116) joined to a body of evidence;
  - several outcomes or interventions at once, with a budget (the domain's planned `programmes`);
  - the evaluation designed with the adoption (the domain's planned `monitoring`);
  - meta-regression on a study's covariate, in place of strata;
  - qualitative evidence on a claim (zetesis's coding and process tracing);
  - a study's records read from a trial register;
  - a schedule of discount rates by bands of years edited on the page (the engine takes one);
  - the board, the map and the tools of the studio's shared page used by a plug-in of this work.
- **Provisional:** the work's name, its colour (a mallow and a verdigris) and its port, 9190.
