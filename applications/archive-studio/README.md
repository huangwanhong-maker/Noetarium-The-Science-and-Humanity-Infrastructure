# Archive Studio

Status: **draft** (2026-11-15). A work of the infrastructure ([ADR 0119](../../development/decisions/0119-the-archive-studio.md), [ADR 0120](../../development/decisions/0120-the-archive-studio-continued.md)): a project-based studio for archives of social-science data, linked data and holdings of any kind, for one person or for several. Port 9150.

## 1. Purpose

An archive holds things of many kinds and says what each is: where it came from, who made it, what is in it, how it stands to the rest, whose hands it passed through. The Archive Studio works on such an archive as a project:
- its holdings are kept with their digests and seen, heard or read, an image in deep zoom;
- what is said of them is linked data, drawn, queried, checked and reasoned over: descriptions in Dublin Core, Records in Contexts or CIDOC CRM, annotations, coded passages, an arrangement, chains of custody;
- its tables have codebooks, as data archives keep them;
- its cases, texts, scales and models are analysed by the studio's tools;
- several people may work on it together, each change written in a sealed journal.

## 2. The Views

Five views are the studio's own, each drawn as its subject. The map, table, chart and board are the shared studio's.

- **Archive.** The holdings as a wall: an image as itself, a sound as the shape of its waves, a moving image by its first frame, a text by its first lines, a table by its first rows. A holding chosen opens in place into a reading room:
  - an image is read in tiles from the server's own IIIF service, as deep as its pixels go;
  - its record is described in Dublin Core, or in the forms of RiC-O or CIDOC CRM (SHACL shapes read as forms, a CRM path such as "produced by, carried out by" one field), every change with its undo;
  - its fixity is the SHA-256 kept when it came, digested again in the page to verify it;
  - its chain of custody is followed by archeion, with a verdict of authenticity;
  - its annotations, after the W3C Web Annotation model, mark a region, a span or a passage;
  - in a text, passages are coded, each code with who coded it;
  - a table may be taken as a table of the project, for the tools.

  The archive is given as a IIIF manifest, and a manifest from elsewhere is taken in, its images and annotations with it.
- **Arrangement.** The finding aid, a tree from the fonds to its items, arranged in place by a keeper. archeion's faults are marked on their units. It is kept in RiC-O, written and read as EAD3, and a unit or the whole archive is packed as a BagIt bag for transfer. Beneath it is the project's journal, verified by archeion.
- **Graph.** The project's linked data drawn as the page. A node chosen opens where it stands, with what is said of it and what can be done. A desk beneath has four leaves: SPARQL queries, SHACL shapes, entailment, and the text of each file. The files may be Turtle, TriG, N-Triples, N-Quads or JSON-LD. Another person's change is merged with one's own by statements. The whole is taken away as TriG, N-Quads or JSON-LD.
- **Datasets.** The tables and their codebooks: drafted from a table, read from DDI-Codebook 2.5, edited, written back. A file is held to its codebook cell by cell, with each variable's frequencies. Two versions are compared. Two variables of two studies are harmonised onto their finest common target.
- **Catalogue.** Studies of the CESSDA Data Catalogue, found by words and taken into the project in DISCO.

## 3. The Tools

The studio's tools run on the project's tables. Each is a module of the board's contract, on the API's `science.inquiry`:
- **Cases and sets** (zetesis): calibrate a set; qualitative comparative analysis with every minimal model; concepts of cases and attributes, with their lattice drawn; Mill's methods; rival explanations weighed clue by clue.
- **Coded texts** (zetesis, nomos): the reading rooms' coded passages, with the agreement of coders, codes by case, codes together and saturation.
- **Scales and models** (nomos, tyche, peira): reliability of a scale, item response theory, factor analysis, latent classes, a linear mixed model, and k-anonymity before a table is shared.

## 4. Several People

Started with `--people --keeper "a name"`, the server knows people by name. Each has a role:
- a reader sees;
- a contributor also describes, annotates, codes and adds;
- a keeper also arranges, hands holdings over and invites.

The keeper invites from the people's dialog and gives the link shown once. Each page says who is here and where. What one does is told to the others at once. Every change is written in the project's journal, sealed to the entry before.

## 5. How It Stands

- **The page** is the visualization layer's studio (`visualization/js/workspace/studio.js`, with views of a work's own and several people), with `src/archive.js`, `src/arrangement.js`, `src/graph.js`, `src/datasets.js` and `src/catalogue.js`. Graphs, forms and annotations are drawn by `visualization/js/linked`, deep zoom by `visualization/js/iiif`.
- **The server** (`serve.py`) is the API's host for project-based works with the studio's own routes:
  - `archive.py`: linked data, holdings, the catalogue, the example, exports, coded passages, forms;
  - `arrangement.py`: arrangement, EAD, BagIt, custody, the journal;
  - `datasets.py`: codebooks;
  - `presentation.py`: IIIF.
- **Three engines**, built by `engine.py`, are programs around the API's compositions, each asked one request a process:
  - `linked` (symploke, `science/linked.hpp`);
  - `archive` (archeion, `science/archive.hpp`);
  - `inquiry` (zetesis, tyche, nomos, peira, `science/inquiry.hpp`).
- **The forms** of description are SHACL shapes under `forms/`, data rather than code.
- **Projects** are kept under `data/cache/archive-studio/projects/`. JSON-LD's remote contexts are fetched once into `data/cache/archive-studio/contexts/`.

## 6. The Example

*Papers of the reading room*, laid out by "Begin with an example archive", is invented throughout, and every piece says so. It holds:
- the minutes of a committee that never met, coded by two people who differ on two passages;
- a plan of the reading room;
- a register's facsimile drawn by the server, 4800 × 3400 pixels, for deep zoom;
- a table of readers, two waves of a survey with their codebooks, and reading rooms compared;
- a sound synthesised by the server.

It comes with people, a small vocabulary, codes, shapes, a visit written in JSON-LD, an arrangement and chains of custody. Faults are planted for the checks to find:
- the shapes find a date written in words and a table with no maker;
- archeion finds a file that ends after its series, and a handover by someone who never held the holding;
- the codebook finds an answer no code declares.

## 7. Checks

- `tests/run-tests.mjs` (in the repository's run) holds the page and the server, started with several people:
  - every planted fault found, and nothing else;
  - the journal verified, and a copy with one entry changed refused at that entry;
  - coded passages in bytes that are their quotes;
  - a IIIF tile at the size asked, EAD read back, a BagIt package, JSON-LD;
  - roles kept, two people's changes merged, events told;
  - the tools held to answers known by construction (`tests/tools_check.py`).
- `tests/drive.mjs` drives the page in headless Chrome, as the keeper and as a contributor, and takes the gallery's pictures. It is not in the suite.

## 8. Not Yet

- JSON-LD framing, and compaction against any context;
- IIIF search and authentication;
- EAC-CPF for agents;
- access rules and retention applied to holdings;
- accounts, and locking a unit while it is arranged.
