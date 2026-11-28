# A Book Written by Communities

- **Status:** seed (being built as Koinonia's case `book`)
- **Date:** 2026-11-26
- **Asked by the user:** "do some more meaningful example, like multiple community cooperate finish a complex task like writing a UN SDGs global analysis and proposals … even political structure, i.e., who evaluate, how evaluate … multilateral cooperation. From initialize questions, to collect data/reports … analysis (you can utilize any tools in this project), report writing, evaluation, revision (evolution)"; "not done by you. But utilize the communities application, build communities, and send agents to do"; "a book-length final report (more than 500 pages in 2-columns version …) … If we can show this experiment well done, we can know that, at least, this community framework preliminary work"; "I will see whether such communities can autonomously do well. And how they cooperate, the trajectory … a full well-formed case study, not a simple prototype or demo"; "we of course allow agents to extend the infrastructure … but should through some power and political way … real communities act like this, we can implement."

## 1. The Experiment

Five communities of different internal orders, joined in a league, write a global analysis of the Sustainable Development Goals with proposals, as a book of more than five hundred two-column pages. Nobody outside the society writes, chooses or corrects any of it. The infrastructure supplies instruments (a shelf of data, a bench of tools, a typesetter) and the society's own rules; everything else is decided and done by agents acting under mandates in communities, and is recorded. The question the case answers is whether such a society can do a large, divided task well, and what its order costs: who decided what, how evaluation was organised and changed, what the audits found, what the deliberation cost in rounds and effort.

## 2. The Communities

| Community | Order (preset) | What it holds | Who acts |
|---|---|---|---|
| The commons of data | commons: open, anyone answerable may lend an agent | the shelf: which series a section rests on, and the derived series | gatherers lent by many origins; a keeper accepting by the stewards' standard (two gatherers of different origins agree) |
| The guild of analysts | guild: selective, entered by trial | analyses: which tools are run on which series, replicated | analysts (members' aides) and the guild's reviewers |
| The writers' cooperative | cooperative: members govern, one member one vote | drafts and revisions | writers lent by its members |
| The forum of the affected | polity of persons: delegates | what matters and to whom: questions, and review as those the goals concern | delegates' agents |
| The partnership | league on the first league's charter | the outline, the page budget, the evaluation regime, the schemes, the bench's admitted tools, the adoption of the book | envoys of the four communities in its council; the secretariat (an organisation) as steward, with a secretary and an editor |

All parties are invented and say so. Circulation between the communities is the league's declared arrangement (ADR 0169–0170); it is not the case's subject and stays as declared unless the council changes it.

## 3. The Stages

1. **The call.** The secretariat declares the task: a global analysis of the seventeen goals with proposals, a page budget (520 to 600 two-column pages, of which the statistical annex is at most a fifth), the base year 2015 and the target year 2030.
2. **The outline.** Envoys propose chapters and sections (goal chapters, regional chapters, chapters across goals, proposals), each with its question and the targets it answers; the council adopts the outline by the league's rule and allots the page budget among the chapters by the Method of Equal Shares over the member communities' approvals (nomos's participatory budgeting through `science.allocation`). The title is the council's.
3. **Data.** For each section the commons' gatherers choose series from the World Bank's catalogue of 1,498 indicators (`data/products/sdg_shelf.py`; fetched on first use) or derive them; a second gatherer of another origin checks the choice (relevance, coverage, unit); the keeper accepts when two agree.
4. **Analysis.** Guild analysts plan the tools run on a section's series (the bench: trend to target after the United Nations Statistics Division's method, spread and its decomposition between and within regions, convergence, breaks, links between series, and whatever the society admits later), the work runs them through the API's engines, and a second analyst of another origin replicates the plan and judges it; results carry the digests of their inputs.
5. **Drafting.** Writers draft each section from its accepted analyses: prose, claims that cite results by identifier, figures named from results, proposals that cite findings, and what the data do not cover.
6. **Evaluation.** The regime in force (a compact of the league) says who reviews and how: the secretariat's editor alone; peers (two analysts of different origins); a panel (one reviewer from each member community); or the forum's delegates. Reviewers score a declared scorecard (kybernesis's `scorecards` through `science.programmes`, aggregated by the weakest link), and the section's adoption is a proposal decided by the league's decision rule in force (its rule and unit of count amendable by the council).
7. **Revision.** A section not adopted returns to its writer with its reviews; each version keeps its parent.
8. **Audit.** The secretariat's auditor checks a declared share of the numbers in adopted sections against the results they cite and attests what it finds; a section found wrong is reopened.
9. **Proposals.** Each community's envoy drafts proposals from the adopted findings; the council adopts the chapter of proposals by its rule, objections recorded.
10. **Adoption and typesetting.** The council adopts the book; the secretariat's typesetter sets it in two columns from the adopted versions, with the statistical annex from the accepted series, figures from the analyses' results, and an annex on how the book was made (the record).

## 4. Extending the Infrastructure Through the Society's Politics

An agent that finds the bench or the shelf wanting may petition for an extension. Each kind of extension is decided where its effects fall:

| Kind | Decided by | Built and checked |
|---|---|---|
| a series added to the shelf (any WDI code) | the commons' keeper (operative) | fetched by the data library |
| a derived series (a formula over shelf series) | the commons' contributors, majority | the formula read by a small grammar of arithmetic; two gatherers recompute |
| a tool (a recipe of existing operations, or a pure function in a restricted language over numbers) | the petitioner's community forwards it; the league's council decides, majority of members | written by the petitioner, checked by two agents of different origins on declared cases, then admitted to the bench |
| a scheme (a target's reading, the thresholds of the trend) | the league's council, two thirds | declared data |

A pure function is Python restricted by a whitelist of syntax (arithmetic, comparisons, loops, lists, dicts, `math` and `statistics`, no imports beyond them, no attributes beginning with an underscore, no I/O), run in a separate process with a time limit. The overhead of this order is measured: rounds from petition to admission, acts and model calls spent deliberating against acts spent working.

## 5. Agents and Models

Rule agents keep the procedures (keepers, secretary, auditor, typesetter, clerks). Model agents decide what needs judgement: envoys (Claude Opus 5.5), writers and analysts (Claude Sonnet 5.5), gatherers and some reviewers (Claude Haiku 5.5), the editor (Sonnet). What a model says enters as a proposal of grade C, admitted or refused by the community's rules. Agents decide in parallel within a round. The ledger's stop (45 of 50 dollars) holds; the case's estimate is 25 to 35 dollars.

## 6. What Is Kept

The society's record (every community's chronicle, sealed), the behaviour journal (every wake, decision, model call and act), the bodies (outline, series choices, analyses, drafts, reviews, audits, petitions, admitted tools), the book's sources (LaTeX) and PDF, its lineage (every number to its result, every result to its inputs' digests, every series to the World Bank's file), and the measures of the case: rounds, cost, acceptance, reversals, audit findings, regime changes, petitions and their fate. Kept as a BagIt bag by archeion in `research/experiments/2026-11-26-koinonia-book/`.

## 7. What the Book Is Not

It is no institution's report. It names no real organisation as its author, and its title page says that it was written by invented communities of agents, every statement a proposal of grade C, its numbers traceable to the World Bank's data and its analyses to the infrastructure's tools.
