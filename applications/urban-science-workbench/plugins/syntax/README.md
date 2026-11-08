# Geometric syntax

*The grammars streets and buildings are written in.* Status: **draft**. Activity: research. [ADR 0083](../../../../development/decisions/0083-syntaxis-grammars-whose-words-are-geometric.md); the foundation [syntaxis](https://github.com/huangwanhong-maker/syntaxis), whose theory note states every construction used here.

## 1. Layers

- **The grammar of the streets** (anywhere). Pick a place. Its streets within 250 m, 500 m or 1 km become a geometric graph of junctions and bends, and a grammar is learned from it by compression: what repeats, up to rotation, scale and position, becomes a rule. Then:
  - **The derivation plays on the map.** Each step lights what it replaces, the steps done keep their colours from orchid to gold, and a column stands where each rule's instance was made. A slider holds any step.
  - **The card.** It gives the junctions and segments, the rules (drawn small, with their uses), the description against the input's, the axiom, and whether the replay gives back every segment. The derivation tree is drawn by its nonterminals when its leaves are too many to draw.
  - **Parse another place.** The next place picked is parsed by this grammar: what it covers, and whether it is derived.
  - **Sample (PCFG) and sample (PCSG).** Streets that were never drawn: the rules merged by shape into categories and weighted by their use, or, for the PCSG, conditioned on their parent's category and refused where they would cross what is drawn. The sample's degrees are compared with the streets'.
  - **Save.** The grammar (`noetarium.grammar/1`), the derivation and the parse (`noetarium.derivation/1`).
- **Blocks and buildings as words of triangles** (anywhere). Pick a place. Its blocks (the faces of its street graph) or its buildings' footprints are cut by their constrained Delaunay triangulation, and each triangle is coloured by its letter, its angles to 15°. The card gives:
  - the alphabet and its shares;
  - the letters' entropy, and what a neighbour tells about a letter;
  - the mean number of triangles glued to one, below one.

  A patch grown by the grammar of their gluing stands to the east. Since the grammar's growth is subcritical, the patch is drawn conditioned on its size, and the card says how many draws that took and whether the size was reached.

## 2. Tool

**The syntax analyser** (anywhere):
- **Grammars of structures.** Grammars are learned from self-similar structures (triangles bridged into copies of themselves, Sierpinski's gasket) and a grid. Their derivations play step by step, and a copy turned by 37°, scaled by 2.3 and moved is parsed by each, with the derivation tree and the parse forest drawn and saved.
- **Triangles rewritten.** A triangle is rewritten by Pachner's 1–3 move, the midpoint and Sierpinski's corners. Its derivation is then recovered from the final triangles alone, pass by pass, and saved.

## 3. Engine

`engine/main.cpp`, on urban-science's and architecture's `syntax` and syntaxis's `induction`, `derivation`, `parsing`, `stochastic` and `simplicial`. Its operations are `learn`, `parse`, `sample`, `words`, `replace` and `status`. The grammar and the input last learned are kept for `parse` and `sample`.

## 4. Limits

- **Tolerance.** Keys are rounded at a tolerance, so learning is sensitive to it; the layer offers three.
- **Greedy compression.** The grammar is a good compression, not the shortest.
- **Not a membership test.** The analyser is greedy: a structure it does not derive may still belong to the grammar's language.
- **Shared corners.** Node replacement cannot share corners between copies, so Sierpinski's gasket keeps a large axiom; hyperedge replacement is planned in syntaxis.
- **The streets read.** They are the world's tiles' ways near the place, cut at the radius; where a city model stands, its own streets are drawn but not read.
- **Footprints.** They are read without their courtyards. Rings that cross themselves are left out and counted.
- **Entropies** are estimated from counts, and biased low where contexts are many and rare.
