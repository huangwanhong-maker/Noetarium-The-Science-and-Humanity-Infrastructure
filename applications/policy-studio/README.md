# Policy Studio

Status: **draft** (2026-11-16). A work of the infrastructure ([ADR 0122](../../development/decisions/0122-the-policy-studio.md)): a project-based studio for evidence-based policy making. A project is a question: whether an outcome follows from an intervention, worked from the theory of change through the evidence for each claim and the argument for a place, to the options and the decision. Port 9190. Its name, colour and port are provisional.

It serves **use** first (working a question through) and **learning** (seeing how a decision depends on one study, one premise or one worth).

*Caveat.* The studio computes what follows from what is declared: a theory, studies, a grading scheme, a place, costs, rates and worths. A scheme here is an example and no institution's own text. Nothing here is advice on any policy.

## 1. Concepts

**A question asks whether an outcome follows from an intervention.** It is asked on a theory of change: states of affairs from the intervention to the outcome, joined by causal claims. Each claim rests on an assumption and needs its support factors, what else must be present in a place for the cause to work there (the policy domain's `change`).

**A chain is no better shown than its least shown claim.** A claim is shown, at a grade, when the interval of its evidence lies on the side the theory expects. The studio finds the strongest path, names its weakest link, and names the gaps that every path passes through, where evidence would help most.

**The evidence for a claim is a body of studies,** of one design and one measure. It is pooled (random effects by REML, in peira) and graded by a scheme the question declares (tekmerion's `warrant`). The factors that are properties of the synthesis are read off it by declared rules; those only a reader can judge are assessed by a reader, with a reason. Every step is said.

**That it worked there is one premise of three.** The other two are about the place: that the same causal principle holds there, and that the support factors are present (Cartwright and Hardie 2012). A premise nothing is stated for is named. Where the studies name strata and the place states its own mix of them, the effect is carried through the strata.

**The decision is by expected worth.** An option's net present value is uncertain as its effect is. Adopting, rejecting and studying first are each given what they are worth in expectation; a study is worth running when what it would teach is worth more than its cost and its delay (the domain's `adoption`). No threshold of significance is consulted.

**A model proposes and never decides.** A language model may draft the states and claims of a theory from a description. It is asked for no effect and no grade. The draft is read by the domain's reader or refused, carries no evidence, and is the theory only when a person takes it.

## 2. The Page

Five views of its own, beside the studios' map, table, chart and board.

| View | Shows |
|---|---|
| Theory | The question as a sentence whose parts can be changed, with its answer; the theory drawn from the intervention to the outcome, each claim a full line with its grade in words and pips, or a dashed one, the strongest path heavier. A state or a claim opens where it stands: its words, what it rests on, what it needs in the place, its evidence. Beneath, the claims one by one. The written form, with every line that does not read |
| Evidence | Each body of evidence: its studies as a forest with the pooled effect and the interval predicted for a new setting; the ladder of its grade, step by step; what a reader assesses; a study opened with the pool without it; the strata pooled; the studies as a table to paste over |
| Here | The crossing from there to here: the effect found there, the premises between as planks (laid, missing, or against), the effect to expect here; the strata with the place's share of each |
| Options | Each option as a ledger: its effect, the worth of a unit, whom it reaches, its costs year by year; its net present value with how often it would be best; the options' values as intervals |
| Decision | Adopt, reject and study first, each with its expected worth; the study that could be run and its worth over every size; the worth of a unit, and whether the decision holds over its bounds |

Several people may work on one question (`--people`): roles, keys, a sealed journal of every change, and what the others did told to each page (the API's studio host, ADR 0120).

## 3. Plug-ins: Subjects

A plug-in is a directory under `plugins/` with a `plugin.json`, grading schemes under `schemes/` and templates under `templates/<name>/question.json` (`noetarium.policy/1`).

| Plug-in | Brings |
|---|---|
| `health` | **The BCG trials read for a place.** The thirteen trials of the BCG vaccine against tuberculosis gathered by Colditz et al. (1994), with their published counts and latitudes, on a theory from a district's programme to fewer cases. The district, its baseline risk, the costs, the worths and the study that could be run are invented and say so. A scheme of four levels written after GRADE |
| `schooling` | **A theory of two paths.** Free school meals and learning, through attendance and through nourishment, a body on every claim and trials of the whole. Every study is invented and says so. A plainer scheme of three levels |

What the first shows is in the decision record's §4.

## 4. Running It

```
python applications/policy-studio/engine.py build      # the policy engine, with g++ (natively or in WSL); some minutes
python applications/policy-studio/serve.py --open      # add --people --keeper "your name" to work with others
```

A language model needs a key, kept for the person at this computer in `models.json` in their settings folder (`%APPDATA%\noetarium`, or `~/.config/noetarium`) as `{"keys": {"anthropic": "..."}}`, never in the repository and never given to a page. A model is asked only from the work's own page at this computer's own address.

From a script, the same question is asked without the page:

```python
from science import policy
answer = policy.ask("appraise", question=question)     # noetarium.policy/1, as api/cpp/science/policy.hpp documents it
```

## 5. Structure

| Part | Holds |
|---|---|
| `serve.py` | The server: the API's host for project-based works, with the work's routes and its engine kept running |
| `questions.py` | The routes: a project's question kept and appraised, a theory and a pasted table read, a template laid out, a model's draft |
| `engine.py`, `engine/main.cpp` | The engine: a program around `science::policy`, built into `data/cache/engines/policy-studio/policy/` |
| `src/` | The page's five views and what they share |
| `plugins/` | The subjects: templates and grading schemes |
| `tests/` | The suite (`run-tests.mjs`, `check_draft.py`) and the page driven in headless Chrome (`drive.mjs`, not in the suite) |

Projects are kept under `data/cache/policy-studio/projects/<id>/`: `question.json`, and `drafts.json` for what a model drafted.

## 6. Dependencies

The policy domain with peira, tekmerion, nomos and tyche, through the API's `science/policy.hpp`; the API's studio host, `science.engines` and `science.models`; the visualization layer's studio page and its `policy` drawings. Nothing depends on this work.

## 7. Open Questions

- How a systematic review (tekmerion's `reviews`) should hand its included studies to a body of evidence.
- Whether strata are enough for carrying an effect to a place, or a meta-regression on a covariate is needed first.
- How several outcomes of one intervention are decided on together.
- Whether a question should carry its units as declared quantities of physis, not as words.

## 8. References

- Cartwright, N. and Hardie, J. (2012), *Evidence-Based Policy: A Practical Guide to Doing It Better*, Oxford University Press.
- Weiss, C. H. (1995), Nothing as practical as good theory, in Connell et al., eds., *New Approaches to Evaluating Community Initiatives*, Aspen Institute.
- Colditz, G. A., Brewer, T. F., Berkey, C. S. et al. (1994), Efficacy of BCG vaccine in the prevention of tuberculosis: meta-analysis of the published literature, *JAMA* 271, 698–702.
- Guyatt, G. H. et al. (2008), GRADE: an emerging consensus on rating quality of evidence and strength of recommendations, *BMJ* 336, 924–926.
- Claxton, K. (1999), The irrelevance of inference, *Journal of Health Economics* 18.
- Wright, S. (1934), The method of path coefficients, *Annals of Mathematical Statistics* 5.
- The policy domain's theory note, [`domains/policy/theory/evidence-based-policy.md`](../../domains/policy/theory/evidence-based-policy.md).
