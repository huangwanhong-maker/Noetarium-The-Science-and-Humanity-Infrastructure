# 0118. symploke, a Foundation of Linked Data

- **Status:** Accepted, provisionally (2026-11-15).
- **Date:** 2026-11-15
- **Decided by:**
  - **The request.** The user asked for the frontend of the social-science data infrastructure, "basically archieve like things", and added: "please support RDF edition, store, and visualization, etc. Because this is important for digital humanity."
  - **The choices.** Asked, the user chose:
    - a new foundation for RDF, named symploke;
    - one studio for social data and linked data together, which should also "properly edit, visualize, analysis the (maybe multimodal) archieve data".
  - **The design.** Claude, here and in symploke's decisions 0002–0003. The studio follows in a record of its own.

## 1. Context

Archives, museums and the digital humanities describe their holdings as linked data: CIDOC CRM, Records in Contexts (RiC-O), SKOS, PROV, Dublin Core, the Web Annotation model. The infrastructure had no RDF:
- Histos Harmonia keeps its record in its own JSON;
- archeion's description is in its own structures;
- the profession's formats were listed as not built in both.

## 2. Decision

1. **A sixteenth foundation, symploke** (συμπλοκή, the interweaving of Plato's Sophist), on morphe, apodeixis and calx.
2. **Seven modules** (symploke's decision 0003; 71 checks):
   - `terms`;
   - `syntax`: N-Triples, N-Quads, Turtle and TriG, read exactly and written back;
   - `store`: indexed three ways per graph, edits with their undo;
   - `query`: SPARQL's core with property paths and aggregates;
   - `entailment`: RDFS and part of OWL 2 RL, each conclusion with its derivation;
   - `shapes`: SHACL Core, every failure named;
   - `canonical`: isomorphism up to blank nodes, on morphe's canonical forms.
3. **Vocabularies are data.** No ontology is written into the library. A work loads CIDOC CRM or RiC-O as graphs.
4. **symploke stands beside archeion, not on it.** A work joins them: a holding's description in RDF, its fixity and custody in archeion.

## 3. Alternatives Considered

- **A module of archeion.** Offered: other sciences would reach RDF only through the archive foundation.
- **The data library alone.** Offered: no checked core, and nothing for the C++ works.

## 4. Consequences

- **Checks:** symploke 71.
- **Next:** the studio of social and linked data, on the shared studio host. It brings the API's composition of symploke and the visualization layer's views of graphs, which Histos Harmonia and other works can use.
- **Not yet:**
  - JSON-LD, RDF/XML and SPARQL Update;
  - FROM and FROM NAMED;
  - RDFC-1.0's exact canonical labels;
  - SHACL-SPARQL.
