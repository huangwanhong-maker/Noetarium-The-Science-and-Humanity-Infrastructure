# Needs: a Plug-In of the Urban Studio

*Status: draft (2026-11-18; [ADR 0125](../../../../development/decisions/0125-the-evolutionary-exploration-and-validation-of-needs.md)). Its name and colour are provisional.*

## 1. Concepts

A need is a gap between the state of some people and a standard, in a domain of satisfaction, judged by one of Bradshaw's four standards. It is written in the needs domain's language:

    need care for age >= 75 and tenure = renting by comparative need_assist higher

The plug-in searches for needs by evolution and tests them without the search deceiving itself:
1. **A population of statements evolves** on half of a place's areas.
   - Generators make statements without a language model: the language walked, subgroup discovery, a regression tree's leaves, the declared rules, and where a model of seeking leaves demand unmet.
   - Mutations and crossovers vary them, and a person or a language model may propose them.
   - Every stage's strategy is chosen on the loop: parents, survival, the choice of operators and their credit, novelty, the archive's gates, the online rule.
2. **A statement is admitted** to the archive when its gap clears its bounds there, stably and productively, and a quarter of the areas, asked through a reusable holdout, does not contradict it. Kept statements breed, and are retired with their reasons.
3. **Each admitted statement is confirmed once** on the last quarter of the areas, which the search never sees, in the order admitted, by an online procedure that controls false discoveries. Its test is sealed in a journal before it runs.

A language model only proposes. It is shown no validation, confirmation or standing, and what it writes is read like anyone's.

**Statements read under their terms** ([ADR 0126](../../../../development/decisions/0126-needs-read-under-their-terms-and-a-model-in-every-part.md)). Every statement is read under every reading of its open terms the data declare (an income line, a reference group), on tekmerion's construal: clear, arguable or untenable, with what it turns on, causal support under each graph, and its rivals within their strata. A model may take part in every role (seeding, varying, joining, refining), interpret a statement in words with other readings and rivals, and propose causal graphs; all of it is grade C and shown as proposed.

## 2. What It Brings

- **The view *Needs*** (`views/needs.js`, `views/needs.css`):
  - the field of statements, every statement made set by the people it concerns and its gap, the needs kept drawn by domain, standard and standing;
  - a need opened where it stands, with its tests in words, its standing from their argument, and the line of operators that made it;
  - the loop of the search, each stage opened in place to choose its strategy;
  - the areas, coloured by their part of the data or by where a need lies;
  - the ledger of confirmations against the rule's levels, with the journal's last seal;
  - the generations, each with who made its children;
  - statements of one's own, read, tested and added; and a language model's proposals.
- **Three tools:** *Test statements of need* (`tools/test.py`), *Search for needs by evolution* (`tools/evolve.py`), *A need, area by area* (`tools/areas.py`, a table placed on the map).
- **One template:** *Needs in the City of Melbourne* (`templates/melbourne-needs.json`), the 2021 Census's need for assistance, low income and unpaid care by area, sex and age (`data/products/needs_tables.py`).
- **Its engine** (`engine/main.cpp`), around the API's `science/needs.hpp`.
- **Its readings** (ADR 0126): a need's card holds its readings, the grid of its interpretations with the stated one ringed, a model's readings dashed and those set aside faded, what it turns on, provision under each graph, its rivals and a model's words; the loop's Generate stage takes a model's roles and a budget, its Admit stage the gates of readings and rivals; the composer says how statements are read and what a model proposed, each to be set aside; the head gives the lineage as PROV.
- **Its server side** (`routes.py`): the project's search kept as `needs/study.json`, `needs/state.json`, `needs/report.json`, `needs/proposals.json` and `needs/readings.json` (a model's interpretations and structures); a search run in a thread a few generations a step and followed from the page; a model asked only from the work's own page at this computer's own address.

## 3. Checks

In the Urban Studio's suite (`applications/urban-studio/tests/run-tests.mjs`): the plug-in's parts; refusals (a dataset that is not a name, a search started by another site's page); and, where the engine and the Melbourne dataset are present: statements read with one refused, a statement tested, a search run in steps, followed and continued with every admission confirmed once and sealed, a need area by area, and the three tools. The page is driven in headless Chrome by `applications/urban-studio/tests/drive-needs.mjs` (not in the suite; with `NEEDS_MODEL` set it asks a language model to propose, to interpret and for causal structure, and runs a search with it in four roles). The suite also asks a statement's readings, the lineage as PROV, and refuses a model asked from elsewhere.

## 4. Caveats

- A census is a full count, read here as a sample of the process that produced it, with its areas as clusters.
- The suppliers' capacity (one person served per 40 m² of care floor space), the catchment (800 m), the causal graph and the rules are declared, not measured; the rules are no institution's.
- A gap is not a cause.
