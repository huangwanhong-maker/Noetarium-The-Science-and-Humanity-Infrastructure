# 0131. An Engagement with a City: The Record, Transfer, the Project Archive and the Plug-in *From Evidence to Action*

- **Status:** Accepted, provisionally (built overnight at the user's word, awaiting their review)
- **Date:** 2026-11-19
- **Decided by:** the user, in discussion (a plug-in of the Urban Studio; the survey by block; every block open to a person, with files; the trajectory archived every time, reusing the archive system; tourism among the missions), and Claude, for the design ([the idea](../ideas/2026-11-19-an-engagement-with-a-city.md)), at the user's word before sleeping: "Not need hesitate time, and make things extensible, flexible, revisable, and high-quality"

## 1. Context

With kybernesis founded ([ADR 0130](0130-kybernesis-steering-by-evidence.md)), the user asked to "prepare practical implementation" for the mission of an urban analyst at the Tokyo Development Learning Center (TDLC, the Government of Japan and the World Bank Group). The mission covers:

- cities that are green, inclusive, resilient and productive;
- disaster risk management;
- territorial development, land, spatial data infrastructure, post-conflict recovery and tourism;
- above all, TDLC's practitioner programmes: needs identification, collaborative learning, a technical deep dive in Japan, advisory support, community engagement and results capture.

In discussion the user proposed:

- that it be a plug-in of the Urban Studio rather than another application;
- that the indexes, methods and evidence be surveyed, then everything each block needs, including actions;
- since such a survey can never be complete, that every block admit a person's intervention and keep files;
- that the trajectory of evidence and information be archived every time, reusing the archive system, so that people can intervene better and revisit it for new insights.

Asked whether so many features now called for an application of its own, Claude answered no: the features are general, and belong beneath every studio (the idea, §2.6).

## 2. Decision

Each part is built in the layer where it belongs.

1. **kybernesis gains `record` and `transfer`** (its decision 0004, which declares archeion a dependency).
   - **The record.** It keeps every value of every block at every turn with how it came to be: a method, an observation (the world is observed where it feeds the loop, never computed), a person's entry, an amendment keeping the computed value, or a review.
     - Each entry is held to its type, the block's bound and the person's authority; an entry outside it is a proposal.
     - Files are named by their digests.
     - Branches are overlays on their parent as it stood when taken.
     - Every act is sealed in archeion's journal.
     - Turns recompute what methods made, and report persons' entries stale rather than replacing them.
     - The record can be verified, replayed and compared, searched for what rests on an entry, a file or an agent, and given as lineage in W3C PROV.
   - **Transfer.** A practice is carried by a lens (f, g). The square is decided in relations by kategoria and measured in laws within a tolerance. Support factors are found sufficient or necessary, and "it will work here" is a dual verdict over the readings of the target's terms.
2. **The API composes them:** `api/cpp/science/steering.hpp` and `science.steering`.
   - An engagement's trajectory travels as JSON (`noetarium.trajectory/1`) and is refused when it does not verify.
   - The engagement's predefined methods fill the standard architecture on JSON values:
     - evidence kept as traces;
     - readings with their sources;
     - for each quantity of each place, what the evidence allows: the intersection of sound intervals, or their hull with the conflict said;
     - declared operators judged under every reading, against standards with their holders;
     - the needs taken up meeting the practices that answer them;
     - the advice as chosen;
     - what is open, as the next inquiry.
   - A transfer may be declared by components.
3. **The studio host keeps a project archive, and evidence becomes a kind of result**, for every studio.
   - Every run of a tool is kept under the digest of its canonical JSON and never replaced, and sealed in the project's journal. Files people bring are kept by their digests (`science.project_archive`).
   - A tool may answer `evidence` beside tables, notes and figures (`board.evidence`). The items are checked where they are made, and gathered into the project's ledger.
   - Plug-ins meet through evidence, never through each other's files. The lifelines and evacuation plug-in's walking tool is the first to opt in.
4. **The Urban Studio's plug-in *From evidence to action*** (`plugins/engagement`). It brings:
   - a server side: the trajectory in the project, every act with the last digest read (a stale write is refused with the current state), the ledger gathered at Collection, files, transfers, and revisiting;
   - a tool, *Evidence from a table*;
   - an engine;
   - an example: Freetown learning from Miyako, every probability and standard declared for it;
   - a template;
   - views: the trajectory as the page, and the transfer as two systems facing each other.
5. **The visualization layer draws them:** the trajectory as a lattice of turns and blocks, and a transfer as two systems facing each other with the support factors as planks.
6. **The survey** (`research/literature/reports/Evidence, methods and actions for steering cities.md`) is organised by block. It has eight chapters (green, inclusive, resilient, productive, knowledge programmes and transfer, the estimation of risk, economic evaluation and allocation, tourism) with 448 listed references, 353 verified against a primary record. It is the catalogue from which an engagement declares its operators, evidence and actions.

## 3. Alternatives Considered

- **An application of its own.** Rejected for now. The studios share a host, a page and an engine builder, so a new work saves nothing. The evidence lives in the Urban Studio's plug-ins, and the archive and the record are general. The criterion for a separate work is recorded in the idea (§2.6): a programme across many cities, with its own views and people.
- **The evidence and archive foundations integrated into the Urban Studio alone.** Rejected: the Law Studio, the Policy Studio and the Archive Studio need the same archive. It is in the studio host.
- **A plug-in reading the other plug-ins' files.** Rejected: it couples to every one of them. Evidence is a kind of result instead.
- **A composite score of a city's qualities by default.** Rejected: the survey shows most scorecards are ordinal, and the Scorecard forbids comparison across cities. Composites are made only when asked, with their weights' stability shown.
- **Programme logistics** (agendas, participants, travel). Left out as office tooling, not science.

## 4. Consequences

- **Checks:**
  - kybernesis: 137 in eight programs (record 33, transfer 18);
  - the API: 25 more (the project archive 10, steering 15);
  - the Urban Studio: 130, the engagement's 10 among them; its page driven in headless Chrome on the template (28 steps, not in the suite), four pictures in the gallery's §48;
  - the visualization layer: `steering/` 59 checks, 815 in the layer;
  - the dependency rules hold, kybernesis now on archeion.
- **What the example shows** (Freetown learning from Miyako; its evidence computed from public data, its laws, standard and readings declared for it):
  - Freetown's need to bring more people out in time is arguable, turning on how leaving is read;
  - "leaving at once" carried from Miyako stands arguable, with probability in [0.5, 0.8], and turns on whether people leave when warned. That is what a deep dive should study first.
- **Not built:**
  - the views' map of the city under its qualities;
  - ordinal evaluation spaces and declared-scheme scorecards in kybernesis;
  - policy making as evolution;
  - learned lenses;
  - the modules the survey finds missing (its §4): hazard event sets, vulnerability, well-being losses, municipal finance, SDG 11.1.1 and 11.2.1 operators, a visitor-stock estimator, a schema of knowledge programmes;
  - knowledge products in two languages;
  - the Archive Studio opening a project's archive.
- **Provisional:** the plug-in's name and colour; kybernesis's dependency on archeion, pending the user's confirmation.
