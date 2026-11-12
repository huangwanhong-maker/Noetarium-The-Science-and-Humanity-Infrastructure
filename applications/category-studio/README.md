# Category Studio

Status: **draft** (2026-11-12). Serves **learning**, **research** and **use**. [ADR 0100](../../development/decisions/0100-kategoria-and-the-category-studio.md).

## 1. Concepts

A studio for experiments in category theory. One writes a script; each of its statements is answered beside it.

- **A script.** It declares categories (by generators and equations, or from families: ordinals, lattices, groups, finite sets, the simplex category), functors, natural transformations, set-valued functors, and monoidal theories with their models; and it states claims (`check`) and asks questions (`find`).
- **Answers with evidence.** A claim *holds*, with what makes it so: a derivation step by step, a limit with its legs, an adjoint with its unit and counit. It *fails*, with the place where it fails: the arrows a functor does not keep, the cone that does not factor, the entry at which two sides of a law differ. Or it is *not decided*, and says why: a completion that did not end, a category too large to build.
- **Drawings.** Each answer is drawn: a category by arrows that generate it; a functor as two diagrams, each object's hue shown again beneath its image; a cone with its diagram and its apex marked; data on a schema as slips of elements joined by lines; a derivation with the part each step replaces marked; string diagrams.
- **What it is not.** It decides claims about categories given whole or finitely presented. It is not a proof assistant: it does not check proofs about all categories, and a law that holds in one model says nothing of the theory.

The mathematics is the foundation [kategoria](https://github.com/huangwanhong-maker/kategoria)'s; the drawings are the visualization layer's `categories`; the page computes nothing.

## 2. Using It

```
python applications/category-studio/serve.py --open
```

The first time, the page offers to build the engine (g++ 11 or later, on this computer or in WSL; about two minutes), or run `python applications/category-studio/engine.py build`.

- **The desk** (left). The script, with a mark beside each statement: ✓ holds, ✗ fails, ? not decided, ! cannot be carried out. It is carried out as one types, or with **Run** (Ctrl+Enter). A remark (`# …`) on a line of its own is shown on the sheet in its place. **Keep** keeps the script on this computer under its name.
- **The sheet** (right). Each statement's answer in order: its claim, its verdict in a glyph and a word, its reason, its evidence and its drawing. A category's card can show every arrow and its table of composition. Clicking a card's head marks its lines in the script; clicking a mark in the margin goes to its card.
- **Examples.** Eleven, from a commutative square to bialgebras and Frobenius algebras, and one of claims that are refused.
- **What one may write.** The guide on the page; kategoria's `script/README.md`; and `GET /api/grammar` for the grammar in BNF.

```
category Sq {
  objects A B C D
  f : A -> B
  g : B -> D
  h : A -> C
  k : C -> D
  g . f = k . h
}
check g . f = k . h in Sq
find pullback g k in Sq
```

A script can also be carried out without the page: `python applications/category-studio/engine.py run <file>`.

## 3. Structure

| Part | Holds |
|---|---|
| `index.html`, `styles.css`, `src/app.js`, `src/guide.js` | The page: the desk, the sheet, the guide |
| `serve.py` | The server: the page, the shared code, the engine kept alive (a request a line), the examples, scripts kept under `data/cache/category-studio/scripts/` |
| `engine.py`, `engine/main.cpp` | The engine: a program around the API's `science/categories.hpp`, built into `data/cache/engines/category-studio/` |
| `examples/*.cat` | The examples: a file's first comment line is its title, its second what it is about |
| `recipe.json` | The one step of setting up: the engine |
| `tests/run-tests.mjs` | Its checks |

## 4. Checks

`node applications/category-studio/tests/run-tests.mjs`: 23. The page's elements and files, the guide against the grammar, the recipe, the examples; the server on a directory of its own (a script kept, read back and put away; a name that is no name and another page refused); and, where the engine is built from the present sources, every example carried out through the server and answered as it is known to be (so many statements hold, fail, are not decided or cannot be carried out), with a derivation, a cone and a string diagram among the views.

The page is driven in headless Chrome when it changes (not in the suite).

## 5. Dependencies

kategoria (through the API's `science/categories.hpp`), morphe; the visualization layer's `categories` and styles. Python's standard library; g++ for the engine. No data.

## 6. Limits and Open Questions

- Sizes are those of a table: a statement refuses to build a category of more than 3,000 arrows, and a script that takes more than a minute is stopped.
- Diagrams of more than 26 objects or 84 arrows are described and not drawn.
- Not here yet: equality of string diagrams modulo a theory's laws; a view of a functor category's objects as the functors they are; drawing by hand (a category drawn and then read as a script).
- The colour, an amethyst with a rose-quartz minor, is provisional until the user chooses.
