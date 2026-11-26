# Koinonia

- **Status:** draft
- **Activities:** use, research
- **Decision:** [ADR 0169](../../development/decisions/0169-koinonia.md), on [ADR 0167](../../development/decisions/0167-thesmos-and-the-communities-domain.md)
- **Port:** 9230

Koinonia (κοινωνία, fellowship, a sharing in common) is a work in which communities are founded on constitutions, and agents lent to them act on their own initiative for their missions. Every act is put to the community's own rules by thesmos's fold through the API (`science.communities`); an act refused changes nothing and is kept with its reason.

## 1. Concepts

- **A community** is a constitution (a preset of the communities domain), a mission and a record: the acts admitted, in order, sealed by thesmos's chronicle on archeion's journal.
- **An agent** is an operative, lent by a person, an organisation or a community under a mandate that names its kinds of act, matters, ceilings and term. It is not asked; it wakes. It wakes when its tempo has run or when something it can act on has appeared in a community it belongs to, and it decides from the community's state. An agent with nothing new in its communities asks nothing of its model.
- **Two origins of behaviour.** An agent acts by rule (a declared procedure) or by model (Claude, at low effort). A model agent asks its model for the work itself (a check, an adjudication). After something unusual, or every few rounds when something has changed, it also asks for its own initiative: which acts to take, and a note to its community if one would help. What a model says is a proposal of grade C, the body of an act that the rules admit or refuse.
- **Roles.**
  - checker (a commons' contributor, a cooperative's hand, a guild's aide);
  - keeper (a steward's operative, accepting by the steward's declared standard);
  - assessor (a reviewer's operative);
  - adjudicator (a guild's aide, or an applicant's agent on trial);
  - clerk (a community's book-keeper in a league);
  - envoy (a community's voice in a league's council).
- **Circulation between communities.** Accepted work by an agent outside its home community is a service. A league declares by a compact how services are kept: gift, credit, transferable credit, cleared credit or provision (`koinonia/circulation.py`). Its envoys may propose another arrangement and vote.
- **The guard on spending.** The key is read from the person's settings folder (`models.json`, `keys.koinonia`), never from the repository. Each call is reserved against `koinonia-budget.json` in the same folder at its largest cost, and refused before it is made if it could pass the stop. Model agents ask a stand-in until Claude is switched on.

## 2. Use

```
python applications/koinonia/serve.py [--port 9230] [--open]                 # the page; changes only from this computer
python applications/koinonia/run_case.py two --rounds 45 [--models]          # a case without the page, its trajectory kept
python applications/koinonia/run_case.py three --regime credit --envoys --models --models-for envoys --record <folder>
python applications/koinonia/record.py research/experiments/2026-11-26-koinonia-cases   # journals, PROV, BagIt bags, figures
node applications/koinonia/tests/run-tests.mjs                                # 23 checks; no model is asked
node applications/koinonia/tests/drive.mjs [url] [society] [folder]           # the page in headless Chrome (not in the suite)
```

The page draws a society as rounds, one for each community. Each round shows its seats (persons as circles, organisations as squares, agents as diamonds beside those who lent them, violet by model and slate by rule) and its commitments as a spiral, with the threads between communities. The mission's work is woven beneath, a cell an entry, told by glyph and tone (✓ holds, ✕ at fault, ? open), and the chronicle flows below. Anything chosen opens where it stands. A society's clock plays a round every few seconds whether or not a page is open, and stops itself after six rounds in which no one acted.

## 3. Layout

| Path | Holds |
|---|---|
| `koinonia/society.py` | societies, communities, records put to the fold a round at a time, persistence |
| `koinonia/agents.py` | lending, waking, the roles' behaviours, the guard's asker, the mission's bookkeeping |
| `koinonia/missions.py` | the commons' mission: the catalogue's entries, declared rules of aggregation, the stewards' standard, findings |
| `koinonia/circulation.py` | services between communities, the league's arrangements, clerks and envoys |
| `koinonia/budget.py` | the ledger and prices |
| `koinonia/cases.py` | the case studies (`two`, `three`) and the stand-in |
| `serve.py`, `index.html`, `styles.css`, `src/app.js` | the server and the page |
| `run_case.py`, `record.py` | case studies without the page, and their records as archives |

## 4. Status

The cases and what they showed are in [the record](../../research/experiments/2026-11-26-koinonia-cases/README.md). Several parts are not built:

- persons acting from the page;
- crossings with receipts between chronicles;
- free riders and the verification of declared capacity;
- a second provider of models;
- accession of a fourth community.

The name, the colour (a wild rose with a lichen, on warm paper and a dusk plum) and the port are provisional.
