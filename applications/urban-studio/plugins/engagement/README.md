# From evidence to action

*Status: draft. Name and colour provisional. [ADR 0131](../../../../development/decisions/0131-an-engagement-with-a-city.md); the design is [the idea of 2026-11-19](../../../../development/ideas/2026-11-19-an-engagement-with-a-city.md).*

## 1. Concepts

An **engagement** is a project's work with one city, kept as a trajectory: the loop from evidence to action, turn after turn, every value of every block with how it came to be. It is the Urban Studio's plug-in for the practice it was drawn from, TDLC's practitioner programmes (needs identification, collaborative learning, a technical deep dive, advisory support, community engagement, results capture), and it serves any engagement with a city.

- **Evidence comes from the other plug-ins.** A tool that answers evidence (`board.evidence`) adds to the project's ledger. The walking tool of *Lifelines and evacuation* does, and so does this plug-in's *Evidence from a table* for any survey, scorecard or diagnostic. Gathering observes the ledger at Collection: what the world does enters the loop only by observation.
- **Every block is filled by a method or by a person.** The predefined methods (kybernesis's record, through `science/steering.hpp`) go as follows:
  - **Evidence:** keeps the traces;
  - **Interpretation:** gathers the open terms with their readings and sources;
  - **Estimation:** for each quantity of each place, says what the evidence allows: the intersection of the items' intervals, or their hull with the conflict said;
  - **Evaluation:** judges each declared operator under every reading against its standard and the standard's holder, with a standing (clear, arguable, untenable) and a measure;
  - **PolicyMaking:** sets the needs taken up beside the practices that answer them;
  - **Policy:** carries the advice as chosen;
  - **Decision:** turns the advice into the intervention, and what is open into the inquiry.

  A person may fill a block, amend a value (the computed one is kept beside it) or review it. A role the rules do not empower enters a proposal, which waits for a review.
- **A block may have several methods.** Estimation by what the evidence allows (the default), pooled by fixed or random effects (peira's synthesis, each interval read as a 95% interval), or the latest item; interpretation by declared readings or the constitution's alone. Which one fills a block from a turn on is chosen, in the opened block, by a sealed act of a role the rules empower (the lead by default); on a branch, it reads old evidence again by another method. Running a turn carries the constitution, proposals and practices from the turn before, as acts by whoever runs it.
- **Nothing is lost.** Every act is sealed in a journal. The trajectory is verified whenever it is read again. A turn run again recomputes what methods made and reports persons' entries stale. A branch re-reads a turn under another reading without touching the main line.
- **Revisiting** asks what was believed at a turn, what changed, and what rests on an entry, a file or a person. It gives the lineage as W3C PROV.
- **Transfer.** A practice from another city is carried by a lens. The plug-in says where the mechanisms agree, which support factors that agreement needs, and whether "it will work here" stands over the readings of the target's terms.

## 2. Content

| Part | Holds |
|---|---|
| `routes.py` | the engagement in the project (`engagement/trajectory.json`, `engagement/transfers.json`); open, start from an example, act, turn, gather, files, transfer; verify, replay, prov, diff, resting |
| `tools/evidence_table.py` | *Evidence from a table*: the rows of any table answered as evidence |
| `engine/main.cpp` | the program around `api/cpp/science/steering.hpp` |
| `examples/freetown-from-miyako.json` | a constitution (open terms, operators, a standard), a practice and a transfer, every probability and standard declared for the example |
| `templates/freetown-from-miyako.json` | Freetown and Miyako walked out of their hazards' reach, leaving as in 2011 and at once, each run answering evidence |
| `views/` | the trajectory as the page, opened in place; the transfer as two systems facing each other |

## 3. Using It

1. Begin a project from the template *Freetown learning from Miyako* and run its plan.
2. In the view *Evidence to action*, start from the example. Gather the ledger at turn 0, and run the turn.
3. Open a block where it stands: read its findings, fill, amend or review it, attach a file.
4. Take a branch to re-read a term. Ask what rests on an entry. Verify, replay, compare.
5. In *Transfer*, assess the example's transfer, or one of your own.

## 4. Checks

The Urban Studio's suite (`applications/urban-studio/tests/run-tests.mjs`):

- a table turned into evidence, its run kept in the archive and verified against the journal;
- an engagement laid out from the example, gathered and turned: Freetown's need arguable, turning on leaving; Miyako, unmeasured, undecided;
- a stale act refused with the current state;
- a proposal by a role not empowered;
- the record verified;
- the transfer arguable, with probability in [0.5, 0.8], whether people leave when warned being its support factor.

The API's `check_steering.py` holds the composition, and kybernesis's `check_kybernesis_record` and `check_kybernesis_transfer` the mathematics.

## 5. Caveat

The example's evidence is computed from public data, but its laws, standard and readings are declared for it. Nothing in it is an official plan of Freetown or of Miyako.
