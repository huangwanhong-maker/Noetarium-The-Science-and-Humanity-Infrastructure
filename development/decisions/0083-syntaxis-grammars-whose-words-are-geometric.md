# 0083. syntaxis: Grammars Whose Words Are Geometric, and the Syntax of Streets and Buildings

- **Status:** Accepted, provisionally (2026-11-08).
- **Date:** 2026-11-08
- **Decided by:**
  - **The request.** The user chose geometric syntax as the first of the urban-science priorities (`development/ideas/2026-11-08-urban-science-priorities.md`):
    - begin with G³, their geometric graph grammar, whose preliminary two-dimensional version in Python is presented at https://serendip.ngo/g3/;
    - develop its deterministic context-free grammar into PCFGs and (P)CSGs, and into grammars whose words are 2-simplices, "and visualize them", preferably as a plug-in on the workbench's main page;
    - add a "correspondent geometry syntax analyzer" that shows its process and results and can "save the parsing graph or trees";
    - "This belong to the city/architecture syntax domain(s)"; "Don't fully utilize the original code, migrate them to fix our project style";
    - "we need a geometry and topology grammars domain", since such grammars "can also parse other structures, like chemistry".
  - **The answers.** The user agreed to a foundation rather than a domain, to the name *syntaxis*, and to `syntax` modules in urban science and architecture, and created the repository.
  - **The design.** Claude.

## 1. Context

G³ follows Fiser et al. (2016), "Learning geometric graph grammars": what repeats in a geometric graph, up to rotation, scale and position, is replaced by a nonterminal, level by level. The grammar so found is context-free and deterministic. Each nonterminal has one production, so its language is the input alone. It is a compression of one structure, not yet a language of structures.

The user asked for the steps beyond:
- a probabilistic grammar;
- a context-sensitive one;
- grammars whose letters are 2-simplices;
- for each, an analyser that parses a structure and keeps the parse.

Streets are the first objects, then buildings, and the user named chemistry. The mathematics names no object in nature, so it is a foundation's (CLAUDE.md §2.1, the rule of reuse). What is a street's or a building's stays with the domain it belongs to.

## 2. Decision

### 2.1 A Ninth Foundation

`foundations/syntaxis` (https://github.com/huangwanhong-maker/syntaxis), header-only C++20 on calx, morphe and physis. Its scope is grammars whose words are geometric or topological objects, and their learning, derivation and analysis (its decision 0002). Seven modules, 70 checks (its decision 0003):

| Module | Holds |
|---|---|
| `shape` | similarities of the plane, Procrustes alignment, canonical keys up to similarity at a tolerance |
| `graphs` | geometric graphs, induced patterns, crossings; Sierpinski's gasket, bridged triangles, grids |
| `induction` | a straight-line grammar learned by compression, with its derivation recorded for exact replay |
| `derivation` | replay, and expansion of any symbol by the relaxed embedding |
| `parsing` | the syntax analyser: a parse forest, its coverage, and whether the input is derived |
| `stochastic` | rules merged into categories by shape; the PCFG by relative frequency; the PCSG by parent annotation and a non-crossing condition; sampling and its statistics |
| `simplicial` | 2-complexes from polygons and plane faces; letters (angle classes); the gluing grammar with its entropies and size-conditioned growth; the replacement grammar (Pachner 1–3, midpoint, Sierpinski) and its analyser |

Its theory note (`foundations/syntaxis/docs/theory.md`) states each construction with its hypotheses and proofs. It also records what was found while checking them:
- **Node replacement cannot share nodes.** Sierpinski's gasket keeps an axiom of many nodes, because adjacent copies share corners. The same family with the copies joined by an edge reduces to three nodes, one rule per depth. Hyperedge replacement is planned for the gasket.
- **Keys are sensitive to the tolerance.** Rounding is not continuous: the bridged family is found exactly at τ = 0.06 and broken at 0.08.
- **Analysis is greedy.** The analyser is invariant under similarity but not a membership test, since membership is NP-complete already for regular DNLC grammars.
- **Growth is subcritical.** Relative frequencies give a mean offspring of (T − r)/T < 1, so a patch of a given size is drawn by rejection, and the output says whether one was reached.
- **Plug-in entropies are biased low** where contexts are sparse.

### 2.2 physis

physis gains the triangulation of polygons (its decision 0024): ear clipping, and the polygon's constrained Delaunay triangulation by Lawson's flips. That triangulation maximises the smallest angle and is unique up to cocircular ties, so a polygon's letters are a function of its shape.

### 2.3 The Domains' Syntax

- **urban science `syntax`:** `street_graph` (ways snapped into a geometric graph at junctions and bends) and `blocks` (the faces of that graph as a 2-complex).
- **architecture `syntax`:** `footprints` (a district's footprints as one 2-complex, shared walls sharing vertices) and `outlines` (footprints as a geometric graph).

Both declare syntaxis in `module.json`, and the dependency tools know it.

### 2.4 The Plug-in *Geometric Syntax*

The Urban Science Workbench gains a plug-in under the lens Fabric, with its engine on the domains and syntaxis:
- **The grammar of the streets.** A layer for any place: its streets learned within 250 m, 500 m or 1 km at a chosen tolerance. The derivation is played on the map step by step, with ribbons coloured by step and a column at each instance made. Another place can be parsed by the grammar, and streets can be sampled from it as a PCFG or a PCSG. The grammar is saved as `noetarium.grammar/1`, the derivation and the parse as `noetarium.derivation/1`.
- **Blocks and buildings as words of triangles.** A layer that colours each triangle by its letter, with the alphabet, the entropies and what a neighbour tells, and a patch grown beside the place.
- **The syntax analyser.** A tool: grammars learned from self-similar structures and grids, played step by step, a turned, scaled and moved copy parsed by them, and the derivation tree and parse forest drawn and saved. Triangles are rewritten by the three moves and their derivation recovered from the final triangles, pass by pass.

The user's preliminary code was read, not reused: its ideas are written again in the foundation's terms and checked.

## 3. Alternatives Considered

- **A domain of geometric and topological grammars, as the user first proposed.** Rejected, with the user's agreement: a domain holds one science's objects, and these grammars name none. The user's "city/architecture syntax domain(s)" are the two domains' `syntax` modules.
- **morphe.** Its formal languages and graph rewriting are exact and combinatorial. Grammars compared by geometry within a tolerance and learned statistically would stretch its invariants.
- **A script in the Studio.** The user preferred a plug-in on the main page, as the feature matters to the workbench.

## 4. Consequences

- **Order.** The foundations are nine. The order of commits is calx and morphe, physis, tyche, peira, nomos, eikon, heuresis, syntaxis, then the domains' submodules and this repository.
- **Next for syntaxis.** Hyperedge replacement, an exact parser for small inputs, keys continuous in the shape space, molecular graphs (for a chemistry domain), and grammars on 3-complexes (architecture's districts).
- **Next among the urban priorities.** Configuration (Space Syntax's integration and choice), which can read the same street graph.
- **The gallery's §20** holds the plug-in's pictures.
