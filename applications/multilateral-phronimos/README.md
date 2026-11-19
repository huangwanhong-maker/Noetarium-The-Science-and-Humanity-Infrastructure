# Multilateral Phronimos

Status: **draft** (2026-11-19). A work of the infrastructure ([ADR 0132](../../development/decisions/0132-diplomacy-and-multilateral-phronimos.md)). It is a project-based studio for heterogeneous multilateral practice: undertakings among states, international organisations, cities, firms, communities, persons and artificial systems, and the partnerships they make. A project is one undertaking and its partnership, read on a date. Port 9210. Its colour and port are provisional.

Its name and concepts are the user's monograph's: Huang, W. (2026), *Future Talent Strategy: The Possibility of the Human–AI Multilateral Phronimos*, Knowledge Commons, DOI [10.17613/s9xqh-xm397](https://doi.org/10.17613/s9xqh-xm397). It serves **use** first (keeping and reading the record of an undertaking) and **research** (comparing undertakings by what their records show). Its documents for readers outside the infrastructure (what practical wisdom asks of it, how to use it, and how its concepts correspond to the monograph's) are in the companion repository [Phronesis: The Practical Wisdom Infrastructure](https://github.com/huangwanhong-maker/Phronesis-The-Practical-Wisdom-Infrastructure), under CC BY-NC 4.0.

*Caveat.* The work computes what follows from what is declared: participants, ties, designs of observation, decisions, commitments, values, criteria, rules and thresholds. Its diagnostics and signs are properties of the record, evidence about an arrangement, never a verdict on it. Nothing here is any institution's scheme or advice on any policy.

## 1. Concepts

**An undertaking is a structure of relations, not a list of parties.** Following the monograph (§10.4.2), an undertaking is R = (V, {E_k}, D, Γ):

- participants of declared kinds;
- ties of declared types in time (authority, reliance, challenge, communication, contribution, or a type of one's own);
- decisions, each with its authority, its evidence, those it affects and the venues where it may be contested;
- constraints, each held by someone.

**Not observed is not absent.** A tie is present when recorded. It is absent when recorded absent, or when a declared design of observation covered both ends and found none. Every other pair is *not observed*, and the page draws it as a gap with a mark, never as nothing.

**The record diagnoses itself.** The monograph's Table 10.2 names the failures of a coordinating function: a decision with no evidence; one made by someone with no authority recorded (a recommendation recorded as a decision); one with no venue, or affecting someone with no route to one; one resting on artificial systems alone, or counting one source twice; a participant without whom the relations of authority fall apart; a participant on whom most reliance falls. The diplomacy domain computes each, and names the place where it shows.

**Mutual benefit is never one number.** Each party's benefit is read under its own criteria, in its own words, with each reading's source. It is judged as a dual verdict: a standing (clear, arguable, untenable, undecided) over what the evidence allows, and a probability bounded without assuming how the criteria depend on one another. The partnership benefits every party only when each one's benefit holds, by a standard the record declares. Values on different dimensions are compared as their scales permit, and summed only by a rule someone declares.

**A commitment is screened, and its state read at a date.** A commitment is screened by the SMART criteria of UN DESA's SDG Actions Platform; whether it is achievable stays undecided until someone is recorded as having judged it. Its state is honoured, honoured late, in progress, overdue or unreported, never failed for want of a report. An amendment counts from its date and never reaches back.

**A rule's power is the rule's alone.** The a priori power of the rules a body decides by (Penrose, Banzhaf, Shapley–Shubik, Coleman, in nomos) is reported beside the evidence of influence, never instead of it.

## 2. The Page

Four views of its own, beside the studios' map, table, chart and board.

| View | Shows |
|---|---|
| Round | The undertaking as itself: the participants on a ring with no head, grouped by kind; the relations as threads, a layer for each type, each switched at its key; the date the record is read on, from every date it holds. A seat chosen reads the round from it, every other pair marked present, absent or not observed, and opens its card (its position at the date, what the record shows of it, its place in the partnership). A thread opens its tie and source. Beneath, the trajectory as strands (relations, observation, decisions, commitments, contributions), each change a knot that opens and moves the date to it |
| Partnership | Whether it benefits every party, by the declared standard, party by party; the commitments with their screens, progress and states, one opened with each criterion's reason, its reports and its amendments; each party's benefit by its own criteria, one opened with its readings and values; what was given, by party, kind and unit; the signs of failure the record shows |
| Governance | Each decision as its record, with the diagnostics that name it; the arrangement's other diagnostics, grouped as the monograph names its failures; the rules its bodies decide by, with their text, source and reading, and their a priori power member by member |
| Negotiation | Where members stand, as ideal points from the record's votes with intervals by a parametric bootstrap; a bargain between two parties, with the Nash, Kalai–Smorodinsky and egalitarian solutions and what each keeps and gives up; issues divided by Adjusted Winner |

Every change is a step kept in the record with what was done. Several people may work on one record (`--people`): roles, keys, a sealed journal of every change, and what the others did told to each page (the API's studio host, ADR 0120).

## 3. Plug-ins

A plug-in is a directory under `plugins/` with a `plugin.json`, templates under `templates/<name>/` (`template.json`, and a `record.json` or the name of a product of the data library) and rules under `rules/`.

| Plug-in | Brings |
|---|---|
| `migration` | **The Migration Multi-Partner Trust Fund, as IATI records it.** The Start-Up Fund for Safe, Orderly and Regular Migration, as the UN Multi-Partner Trust Fund Office publishes it: 14 contributors, the administrative agent, 12 participating UN organisations, 15 joint programmes and the 14 countries they serve, with US$28.2 million of contributions. The records are read by the data library (`data/products/multilateral_cases.py migration-fund`, ODC-PDDL) into its cache and never committed. The publication holds money and participation, not governance or results, and the work says so where it matters |
| `charter` | **The Charter's rules, and an invented basin partnership.** The Security Council's rule on matters other than procedure (Article 27(3)) and the General Assembly's on important questions (Article 18(2)), each with the reading under which it is computed. And the Arda Basin Partnership, every party, figure, decision and vote invented and said to be: three riparian states, a commission, a bank, two cities and the delta's fishers |

### 3.1 The Invented Partnership

What the invented partnership shows, read on 2025-12-31:

- the model's run was entered as a decision: no authority, no venue, and resting on itself twice;
- the commission and the secretariat each hold the relations of authority together;
- the partnership does not benefit every party: Porthaven's 75 bathing days are left open by the evidence (P = 0.16), and the fishers' catch is untenable;
- in the steering committee's double majority, the Upland Republic's 25 votes give it no more power than the chair's 5 (0.15 each), and the bank's 40 give it 0.4.

### 3.2 The Migration MPTF

What the Migration MPTF shows: its authority runs through the fund alone, the one participant without whom it falls apart. None of its fifteen programmes has an indicator in the publication, so no one can tell from it whether they are honoured.

## 4. Running It

```
python applications/multilateral-phronimos/engine.py build          # the multilateral engine, with g++ (natively or in WSL); a few minutes
python data/products/multilateral_cases.py migration-fund           # the Migration MPTF from IATI, into the data library's cache
python applications/multilateral-phronimos/serve.py --open          # add --people --keeper "your name" to work with others
```

The votes of the UN General Assembly come from the file the Dag Hammarskjöld Library publishes (UN Digital Library, record 4060887). Its terms ask that it not be downloaded automatically without the Library's approval, so the data library never fetches it. A person downloads it, and it is read a session at a time:

```
python data/products/multilateral_cases.py general-assembly --votes 2026_10_15_votes_ga.csv --sessions 73
```

From a script, the same record is read without the page:

```python
from science import multilateral
multilateral.ask("undertaking", undertaking=record["undertaking"], at="2025-12-31")
multilateral.ask("partnership", partnership=record["partnership"], at="2025-12-31", standard={"kind": "standing", "least": "clear"})
multilateral.ask("rule", **multilateral.rule_request("security-council"))
```

## 5. Checks

`node applications/multilateral-phronimos/tests/run-tests.mjs` (31 checks): the page, the plug-ins, and the server on a store of its own with several people:

- the invented partnership read on its date, its rules' power and its votes read as ideal points;
- a record kept over another's change refused with the current one;
- a reader who may read and not keep;
- bargaining and a division asked of no project;
- the Migration MPTF, where the data library has read it.

`tests/drive.mjs` drives the page in headless Chrome through both records (21 steps, not in the suite).

## 6. Dependencies

- the API's host for project-based works and the visualization layer's studio page;
- the diplomacy domain (`relations`, `partnerships`), with nomos (power, bargaining, the nucleolus) and tyche (ideal points), through the API's `science/multilateral.hpp`, composed in a C++ engine kept running;
- the visualization layer's `multilateral` (the round, the strands, the ledger, power, the line, bargaining);
- the data library's `formats/iati.py`, `formats/unvotes.py` and `products/multilateral_cases.py`.

## 7. Not Yet

- The planned modules of the domain: rules as simple games declared in a body's own words; institutions and regime complexes; events as reports; negotiation with win-sets; mandates; reviews.
- Transfer between undertakings as two systems facing each other.
- A model's proposals (readings, ties read from documents), each of grade C until a person takes it.
- The record's archive in archeion's journal, as the Urban Studio's engagement keeps its trajectory.
