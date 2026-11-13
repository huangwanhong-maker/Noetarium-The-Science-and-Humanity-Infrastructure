# Histos Harmonia

*Status: draft. Serves learning, use and research ([ADR 0110](../../development/decisions/0110-historical-objects-and-histos-harmonia.md)).* An archive of historical objects and a place to learn from it. ἱστός is the loom and the web woven on it; ἁρμονία is a fitting-together, and the musical scale. The name is the user's.

## 1. Concepts

- **A historical object is anything in history that can be inquired into**: an event, a period, a person, a work, a theory, a kind of music, a problem. An object appears in dependence on others, elsewhere in space and time, and objects of different kinds bear on one another.
- **Every object is asked four questions**: what problem it answers; why that problem arose then and there; what made it answerable; to whom it mattered and what it made possible. The work asks them before it shows what the record says. Answers are cheap now; the judgement of why a question arose, and why it was worth asking, is what is learnt here.
- **The method is in the questions.** Conditions (an instrument, a press, a practice, an institution) are recorded beside ideas and works, and asked for of every object. The work asserts no thesis of why things happen.
- **Every statement says who states it and how it stands**: established, attested, proposed or disputed. What a language model proposes from memory is marked so and is in the default layer of no reader. Where the record is silent the page says that the record is silent.
- **One choice, three ways of seeing.** The loom: time as the warp, strands side by side, the dependences as threads between them. The map: where things were, by a clock. The cone: what the appearing of one object rested on, and what rests on it.
- **Activities are separate tasks, brought by plug-ins**, each for the kinds of object it suits, and what is written for an activity on an object may be a document or a program.
- **An archive is its institution's own folder.** Materials are kept by the hash of their content and never altered, each with its rights; what was done is logged and the log is only added to.

## 2. What It Does

| Part | What one can do |
|---|---|
| The layer | Read the established alone, with the attested, or with the proposed; the counts of each are on the keys |
| The loom | See every object by the year it appeared, in strands by discipline, tradition or kind; the threads are premises, coloured by role and drawn as they stand; choosing an object lights its cone |
| The map | See where things were by the year of the clock, on present coasts with no names and no borders of the time; the cone of the object chosen as arcs between places |
| The cone | Read the premises of an object by steps and by role, with what it inherits from what it stands under marked as inherited, and what rests on it |
| The clock | Move through the years, or let them run; beneath it stand the questions of that year: the problems posed, open or answered |
| Inquire | The four questions, each asked before its answer is shown; who else answered the same problem, and whether independently; what in the record could not have appeared without the object |
| Present, Trace, Examine | What the object is and stands under; its past and future by steps; what is said of it by whom, and what the archive holds of it |
| Practise (music) | Hear a tuning's scale, its fifths and its thirds, and read each one's departure from the pure interval; set it beside another tuning |
| Dispute (philosophy) | Follow an argument map: the question, the positions, the arguments for and against, and what stands, falls or is left open |
| Write | For an activity on an object: a document (a paragraph `? question :: answer` is asked before it is shown) or a program (the body of a function of the object's data, run apart from the page); it also serves the objects that stand under this one |
| A path | Walk a declared path through a record, a sentence at each stop; with `?exhibit=<path>` the page becomes an exhibit: no finder, no desk, larger type |
| The desk | Add what a source says of a statement, with its passage looked for in a holding; take in materials with their rights and access; add sources; read the log; have kategoria verify the record; take the record with its holdings as a bag |

## 3. Structure

- `serve.py`: the server (port 9130). It asks the engine, keeps the archive, and takes changes only from its own page at this computer's own address. Its docstring lists what it answers.
- `engine/main.cpp`, `engine.py`: the engine, a program around the API's `science/histos.hpp` (the history domain, with kategoria), `science/sounding.hpp` (musicology) and `science/arguments.hpp` (philosophy), built by `science.engines` into `data/cache/engines/histos-harmonia/` and asked one request a process.
- `index.html`, `styles.css`, `src/app.js`, `src/desk.js`: the page, on the visualization layer's `history` (the cone, the loom) and `map`.
- `plugins/<name>/plugin.json` and its module: the activities a plug-in declares (`id`, `name`, `kinds`, `needs`, `about`), the kinds it adds, and paths (`tours`). The module exports `render`, a function for each activity from a context (the object, what the engine answered, the record, the page's helpers) to a node.
- `tools/draw-schema.mjs`: the schema of historical objects verified by kategoria and its diagrams saved (`domains/history/schema/figures/`).
- The archive: `data/cache/histos-harmonia/archive/` unless `--archive` names another: `records/`, `holdings/`, `holdings.jsonl`, `content/`, `log.jsonl`.

## 4. Running It

```
python applications/histos-harmonia/engine.py build     (once, about three minutes; or from the page)
python applications/histos-harmonia/serve.py --open
```

Links open the page as wanted: `?record=…&object=…&view=loom|map|cone|desk&layer=established|attested|proposed&activity=…&theme=day|night&answers=shown&exhibit=<path>`.

## 5. Checks

`node applications/histos-harmonia/tests/run-tests.mjs`: 59 checks. The page's elements and files; each plug-in's activities, kinds and paths; and, where the engine is built, the server on a small record and a throwaway archive: the record, an object, the frontier, kategoria's verdict, a tuning, an argument map; a holding taken in by its hash; a passage not found adding nothing and one found establishing its statement, in the archive's own copy; what is written kept and inherited; an altered holding found; a dark one withheld; the package; the log; another page refused. Then the domain's own record, whole. The page was driven in headless Chrome for the gallery's pictures (not in the suite).

## 6. Visual Style

Provisional, until the user chooses: undyed linen (#FAF6F1) by day and indigo (#1F1A2C) by night; madder rose (#B5566B) only on what is chosen or in the hand; an indigo violet (#6B5FA8) for what passes. Hairlines, serif type, generous air. A standing is told by a word as well as by its line; a role by its words beside its hue, one of the visualization layer's validated ten.

## 7. What Is Not Built, and What Must Not Be Claimed

- **The first record is a language model's draft.** *Tuning and temperament* has 245 statements, all of grade C; none is established. It shows how the work behaves. It is not history confirmed, and must not be presented as such until a keeper has held its statements against sources.
- **One institution, one machine.** No accounts, no roles; a server opened to others changes nothing.
- **Preservation-compatible, not a certified repository.** The archive follows the ideas of OAIS, packages as a bag, dates in EDTF. EAD, RiC, CIDOC CRM, LIDO, IIIF, MEI and TEI are not yet read or written, and it replaces no collection system.
- **Not built:** a model reading a text for relations with each passage checked (the desk checks a passage a person gives); the audit of a grade's precision; spacetime in three dimensions; the board; a record of objects read as a history complex; scores and texts shown as materials.
