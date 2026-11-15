# 0120. The Archive Studio Continued: Images, Standards, Arrangement, Datasets, Inquiry, JSON-LD and Several People

- **Status:** Accepted, provisionally (2026-11-15).
- **Date:** 2026-11-15
- **Decided by:**
  - **The request.** The user asked that everything ADR 0119 left undone be built: "IIIF images and deep zoom; the archival standards (EAD, RiC-O, CIDOC CRM) as templates; the archives foundation's arrangement and chain of custody applied to the holdings; the studies' DDI codebooks as datasets; the analysis libraries (zetesis, tyche, nomos) as tools inside the studio; JSON-LD; several people working on one archive".
  - **The design.** Claude, here; symploke's part in its decision 0004.

## 1. Context

ADR 0119 built the Archive Studio: holdings on a wall, reading rooms, linked data drawn and queried, a catalogue of studies. It listed seven things not yet done. Each belongs in a different layer, and each layer already held most of what it needed:
- archeion has arrangement, custody, packages, a sealed journal and codebooks;
- zetesis, tyche, nomos and peira have the analyses;
- symploke planned JSON-LD.

## 2. Decision

Each of the seven is built in the layer it belongs to, and the work composes them.

1. **IIIF.**
   - The data library's `formats/iiif.py` holds the Image API 3.0 (regions, sizes, rotations, qualities, the information document, a request served by Pillow) and the Presentation API 3.0 (manifests written, manifests of version 3 and 2 read).
   - The visualization layer's `iiif` holds the arithmetic of tiles, pure and checked, and a deep-zoom viewer.
   - The work serves every image as a IIIF service and the archive as a manifest, and takes a manifest from elsewhere in.
2. **Standards as templates.**
   - A form of description is a SHACL node shape. The visualization layer's `linked/forms.js` reads it as fields and writes what is filled in as statements, along sequence paths through the nodes they pass through. So a CIDOC CRM path such as "produced by, carried out by" is one field.
   - The work offers Dublin Core, Records in Contexts (RiC-O 1.1, every property checked against the published ontology) and CIDOC CRM 7.1, as files under `forms/`. The same shapes check what the forms wrote.
   - EAD3 and EAD 2002 are read and written by the data library's `formats/ead.py`.
3. **Arrangement and custody.**
   - The arrangement is kept as RiC-O in the project's linked data, checked by archeion, written and read as EAD, and packed as a BagIt bag by archeion.
   - Chains of custody are PROV-O activities in the linked data. archeion follows each and judges authenticity against the holding's digest now.
   - The API composes archeion as `science/archive.hpp` and `science.archive`.
4. **Datasets.**
   - A table's codebook is kept in archeion's form, read from and written to DDI-Codebook 2.5 by the data library, and described in DISCO.
   - archeion holds a file to its codebook cell by cell, compares versions, and finds the finest common target of two variables.
5. **Tools.** Three groups of the studio's tools stand on `science/inquiry.hpp` and `science.inquiry`:
   - cases and sets: calibration, QCA with every minimal model, concept lattices, Mill's methods, rival explanations weighed;
   - coded texts: passages coded in the reading rooms, with the agreement of coders, codes by case and saturation;
   - scales and models: reliability, IRT, factor analysis, latent classes, mixed models, k-anonymity.
6. **JSON-LD** is a symploke module (its decision 0004), held to pyld. Remote contexts are given to the engine, never fetched by it; the work fetches and caches them at its edge.
7. **Several people** is the API host's, opt-in by `--people`, so every studio may use it:
   - people have names, roles (reader, contributor, keeper) and keys, of which only digests are kept;
   - every change is written in the project's journal, sealed to the entry before by archeion's rule, and told to the others' pages as an event;
   - each page says where its person is;
   - two people's changes to one file of linked data merge by statements, since statements are sets and neither can contradict the other;
   - a key reaches what a page fetches without its scripts by its address, for reading only.

## 3. Alternatives Considered

- **Forms written in code for each standard.** Rejected: a standard would be code to maintain. As shapes, a form is data, and it checks what it wrote.
- **Custody and arrangement in files of their own.** Rejected for the reason ADR 0119 gave for annotations: as linked data they are queried, shaped and reasoned over like the rest.
- **Several people by accounts and sessions.** Rejected for now: keys given as links, roles and a sealed journal are what one archive with a few people needs. Accounts would be a server's to keep.
- **Locking files while one person edits them.** Rejected: merging by statements loses nothing and blocks no one. It is said that a merged file loses its comments and layout.

## 4. Consequences

- **Checks:**
  - symploke's 37 for JSON-LD (108 there);
  - the data library's 18 for IIIF and EAD;
  - the visualization layer's 11 for tiles and forms (626 there);
  - the studio's suite, its tools held to answers known by construction among its checks;
  - the drive in headless Chrome, not in the suite.
- **The engine builder** runs at most three engines in WSL at once (`NOETARIUM_WSL_ENGINES`), and reads what WSL says when it fails. Without this, a page that asked several things together could make WSL stop answering. A server may also keep an engine's processes running (`Engine.keep(n)`), each started with `--serve` and answering a request a line. A process that dies or stalls is replaced, and a rebuilt engine replaces them all. The Archive Studio keeps its three engines so, since a reading room asks several things at once.
- **Not yet:**
  - JSON-LD framing and compaction against any context;
  - IIIF search and authentication;
  - EAC-CPF for agents;
  - access rules and retention applied to holdings;
  - accounts;
  - locking a unit while it is arranged.
