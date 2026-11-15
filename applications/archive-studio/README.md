# Archive Studio

Status: **draft** (2026-11-15). A work of the infrastructure ([ADR 0119](../../development/decisions/0119-the-archive-studio.md)): a project-based studio for archives of social-science data, linked data and holdings of any kind. Port 9150.

## 1. Purpose

An archive holds things of many kinds and says what each is: where it came from, who made it, what is in it, how it stands to the rest. The Archive Studio works on such an archive as a project. Its holdings are kept with their digests, described and annotated as linked data, and drawn, queried, checked and reasoned over. Beside them stands a catalogue of studies from Europe's social-science data archives. Tables are analysed and charted, as in the other studios, and thought about on boards.

## 2. The Views

Three views are the studio's own, each drawn as its subject; the map, table, chart and board are the shared studio's.

- **Archive.** The holdings as a wall: an image as itself, a sound as the shape of its waves, a moving image by its first frame, a text by its first lines, a table by its first rows. A holding chosen opens in place into a reading room. There it is seen, heard or read, with:
  - its record: Dublin Core terms, each field a statement, every change with its undo;
  - its fixity: the SHA-256 kept when it came, digested again in the page to verify it;
  - its annotations, after the W3C Web Annotation model: a region of an image, a span of a sound or a moving image, a passage of a text, each with a note;
  - its links to other holdings.
- **Graph.** The project's linked data drawn as the page.
  - A node chosen opens where it stands, with what is said of it and what can be done: follow it, say something of it, take a statement back.
  - A desk beneath has four leaves: SPARQL queries whose bindings are marked on the graph; SHACL shapes whose failures are marked and listed; entailment by RDFS and part of OWL 2 RL, drawn dashed with each rule; and the text of each file, coloured, kept only when it reads.
- **Catalogue.** Studies of the CESSDA Data Catalogue, found by words. A study taken into the project is written in DISCO, the DDI Alliance's vocabulary for the discovery of studies.

## 3. How It Stands

- **The page** is the visualization layer's studio (`visualization/js/workspace/studio.js`, which gained views of a work's own for this) with `src/archive.js`, `src/graph.js` and `src/catalogue.js`; the graph and the annotations are drawn by `visualization/js/linked`.
- **The server** (`serve.py`) is the API's host for project-based works, configured with its own routes (`archive.py`): a project's linked data under `linked/`, its holdings under `holdings/`, the catalogue, and the example.
- **The engine** (`engine/main.cpp`, built by `engine.py`) is a program around the API's composition of symploke, `api/cpp/science/linked.hpp`, asked through `science.linked`.
- **Projects** are kept under `data/cache/archive-studio/projects/`.

## 4. The Example

*Papers of the reading room*, laid out by "Begin with an example archive", is invented throughout, and every piece says so:
- the minutes of a committee that never met;
- a plan of a reading room, drawn for the studio;
- an illustrative table of readers;
- a sound synthesised by the server.

It comes with the people, a small vocabulary and shapes. The shapes find two faults planted in it: a date written in words, and a table with no maker.

## 5. Checks

- `tests/run-tests.mjs` (19 checks, in the repository's run) holds the page and the server: digests computed apart, refusals with line and column, an edit and its undo, a query, the shapes' two faults, entailment, the catalogue, a write from another page refused.
- `tests/drive.mjs` drives the page in headless Chrome through fifteen steps, from the wall to the catalogue by night, and takes the gallery's pictures. It is not in the suite.

## 6. Not Yet

- IIIF manifests and deep zoom;
- the profession's formats (EAD, RiC-O mapped from arrangements, CIDOC CRM templates);
- archeion's arrangement, custody and packages for the holdings;
- the DDI codebooks of studies as datasets with archeion's harmonisation;
- the analyses of zetesis (coding the texts, QCA), tyche and nomos as tools;
- JSON-LD;
- several people on one archive.
