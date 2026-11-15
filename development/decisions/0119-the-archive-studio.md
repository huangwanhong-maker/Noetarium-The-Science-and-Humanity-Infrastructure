# 0119. The Archive Studio

- **Status:** Accepted, provisionally (2026-11-15).
- **Date:** 2026-11-15
- **Decided by:**
  - **The request.** The user asked for "the frontend of utilize social science data infrastructure. I think it is a basically archieve like things. With visualization features, and various algorithms can be equiped. I think for analysis and visualization, a studio and project-based design is OK". They added: "please support RDF edition, store, and visualization, etc. Because this is important for digital humanity".
  - **The choices.** Asked, the user chose one studio for social data and linked data together. It should "properly edit, visualize, analysis the (maybe multimodal) archieve data". RDF went to a new foundation, symploke (ADR 0118).
  - **The design.** Claude, here.

## 1. Context

The layers beneath social analysis are built (ADR 0116):
- an archive of data (archeion's datasets, peira's disclosure control, a catalogue of studies harvested into the data cache);
- an archive of evidence;
- case-oriented inquiry (zetesis, ADR 0117);
- the quantitative gaps.

symploke holds linked data (ADR 0118). Nothing yet let a person work on an archive: keep its holdings, say what they are, see and annotate them, and reason over what is said.

## 2. Decision

1. **A work, the Archive Studio** (`applications/archive-studio`, port 9150; name, colour and port provisional). It stands on the studios' shared host and page.
2. **Two extension points, made general rather than written for this work:**
   - the shared studio page (`visualization/js/workspace/studio.js`) takes views of a work's own, each a tab and a pane drawn by the work;
   - the API's host (`science.studio_host`) takes routes of a work's own.

   Market Studio and the Urban Studio are unchanged by them.
3. **Three views, each drawn as its subject:**
   - **the archive:** a wall of holdings drawn by their kinds, opening in place into a reading room where each is seen, heard or read, described in Dublin Core terms, annotated after the W3C Web Annotation model, and its fixity verified in the page;
   - **the graph:** the linked data as the page, nodes opening where they stand, and a desk for SPARQL, SHACL, entailment and each file's text;
   - **the catalogue:** studies taken into the project in DISCO.
4. **Holdings and their descriptions are linked data.** A holding is kept under its name with its SHA-256. Its description, its annotations and its links are statements in the project's files of linked data, so every view, query and shape reaches them alike.
5. **The visualization layer's `linked`** draws graphs, colours Turtle and SPARQL, and writes and reads Web Annotations, for any work: Histos Harmonia among the first.
6. **The API's `science/linked.hpp` and `science.linked`** compose symploke for any work's engine and any script.

## 3. Alternatives Considered

- **A page of its own, apart from the shared studio.** Rejected: the project, its tables, tools and boards would be written again, and the user asked that new studios keep them.
- **Annotations in their own store.** Rejected: as linked data they are queried, shaped and reasoned over like every other statement.

## 4. Consequences

- **Checks:** the studio's suite has 19; the visualization layer's `linked` 11 (615 there); its drive in headless Chrome 15 steps, not in the suite.
- **The gallery** gains ten pictures.
- **Not yet:**
  - IIIF and deep zoom;
  - EAD, RiC-O and CIDOC CRM as templates;
  - archeion's arrangement, custody and packages for the holdings;
  - studies' DDI codebooks as datasets, with harmonisation;
  - zetesis, tyche and nomos as the studio's tools;
  - JSON-LD;
  - several people on one archive.
