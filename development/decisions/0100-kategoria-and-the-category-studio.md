# 0100. kategoria, a Foundation for Categories, and the Category Studio

- **Status:** Accepted, provisionally (2026-11-12).
- **Date:** 2026-11-12
- **Decided by:**
  - **The request.** The user: "Could you implment the cateogry theory experiment studio, with formal verifier and visualizer? As an independent foundation (I think it is a foundation?) And please also update gallery."
  - **The design.** Claude, here.

## 1. Context

morphe, the algebraic foundation, planned a module `categories` from its first day (ADR 0035) and named category theory its spine. Nothing of it was written. The market foam (ADR 0099) left its categorical layer unbuilt for want of it: schemas as categories, data as functors.

The request has three parts, and each raised a question.

- **A foundation?** Yes. Categories, functors and their universal constructions name no object in nature (§1 of the infrastructure's CLAUDE.md), and every domain whose structures compose may use them. The *studio* is not a foundation: a place where a person writes and looks is a work, and what it draws belongs to the visualization layer.
- **Independent of morphe?** The user asked for it. Three reasons hold on their own: the body is large at its founding; it needs morphe's graphs, canonical forms, grammars and groups beneath it, as syntaxis needed morphe's and physis's (ADR 0083); and a language of claims with its session is a different kind of thing from a module of algebra.
- **What is a "formal verifier" in a library that computes?** Not a proof assistant: those check proofs of theorems about all categories, and good ones exist. What is missing is a place to try a finite case and be told exactly where a claim fails.

## 2. Decision

### 2.1 Three Places

| What | Where |
|---|---|
| The mathematics: categories computed with, claims decided, the language of scripts | a tenth foundation, **kategoria** (`foundations/kategoria`, a repository of its own), on morphe and calx |
| The drawings: diagrams of categories, cones, set-valued functors, derivations, string diagrams | the visualization layer, `visualization/js/categories` |
| The composition for any work's engine | the API, `api/cpp/science/categories.hpp` |
| The place where a person writes and looks | a work, the **Category Studio** (`applications/category-studio`, port 9070) |

The name *kategoria* (κατηγορία, the word of Aristotle's *Categories*, from which Eilenberg and Mac Lane took "category") is provisional until the user confirms it.

### 2.2 The Verifier

Every claim is answered with a verdict: it *holds*, with what makes it so; it *fails*, with the place where it fails; or it is *undecided*, with the reason (kategoria's `kernel`).

- A claim about every instance of a finite structure is decided by examining every instance, and a failure is answered with a counterexample by name.
- A claim that something exists is decided by a search, and what the search finds is checked by a function that does not share the search's code: a limit by the definition of a limit, an adjoint by the triangle identities, an equality of paths by a derivation that a checker knowing only the equations replays.
- Where a bound stops a search or a completion, the answer is *undecided* and says so.

### 2.3 kategoria

Eight modules at its founding (its decisions 0002 and 0003; its theory note `docs/theory.md`):

- `kernel`: verdicts.
- `finite`: finite categories as tables; families; opposites, products, sums, slices; monos, epis, isomorphisms, skeletons, generators.
- `functors`: functors and natural transformations verified and found by a search that visits exactly the functors there are; categories of functors; equivalences; comma categories; adjoints from universal arrows; monads, their algebras and Kleisli categories.
- `limits`: cones; limits and colimits decided and found; completeness; preservation; exponentials and cartesian closure.
- `sets`: set-valued functors; the Yoneda lemma; elements; the pullback and both Kan extensions along a functor with their universal properties; the classifier of subobjects.
- `presentations`: quivers with equations; Knuth–Bendix completion on paths; derivations and their checker; the finite category built from normal forms.
- `monoidal`: symmetric monoidal signatures and terms; string diagrams; equality in the free symmetric monoidal category by morphe's canonical forms; models in matrices; laws decided in a model.
- `script`: a language of declarations, claims and questions, its grammar in BNF read by morphe's reader (as genomics' notations are, ADR 0064); a session that answers each statement with a verdict and a view of what there is to draw.

### 2.4 morphe

Categories as objects of study are kategoria's; morphe's planned `categories` is narrowed to the constructions its own modules share, and its row points to kategoria for the subject (its decision 0013). morphe keeps the structures and their algorithms; kategoria reads them as categories where it needs them. The order has no cycle: kategoria stands on morphe, and nothing of morphe needs it.

### 2.5 The Other Layers

- **The API.** `science/categories.hpp`: a request holds a whole script; the answer is its outcomes. Nothing is read from a file, and nothing is kept between requests.
- **The visualization layer.** `categories/`: a category laid out in layers by arrows that generate it; a functor as two diagrams with each object's hue beneath its image; a cone with its diagram, apex and legs marked; a set-valued functor as slips of elements joined by the generators' actions; a derivation path by path; string diagrams. Markup that needs no page, so the checks read it in Node.
- **The work.** The script at the left, with a mark beside each statement; at the right each statement's answer in order, with its verdict (a glyph and a word as well as a colour), its reason, its evidence and its drawing; the script's remarks set between them. Eleven examples. Scripts of one's own kept on this computer. Its engine is kept alive by its server.

### 2.6 Checks

kategoria 190, against closed forms, brute force and structures built another way; the visualization layer's categories 29; the Category Studio 23, every example carried out through its server and answered as it is known to be. The page was driven in headless Chrome (not in the suite).

## 3. Alternatives Considered

- **morphe's `categories` module, as planned.** One algebraic foundation fewer to keep. Set aside at the user's word and for the reasons of §1.
- **A foundation beneath morphe**, so that morphe could build on it. morphe's structures do not need categories to be computed with, and kategoria needs morphe's algorithms.
- **A domain.** Category theory names no object in nature.
- **A proof assistant's kernel.** Another question (§1).
- **The studio as a plug-in of Tabula.** A script with its answers is a document of its own kind, and the board's modules are Python. A card of the board that runs a script through the API is possible later.
- **Drawings in the work.** Any work that holds a schema or a process will want to draw it.

## 4. Consequences

- **What is decided is finite.** Categories given whole or finitely presented; statements about all categories are outside the library, and are said to be.
- **Sizes are those of a table.** Some thousands of arrows; a statement refuses to build more.
- **Found while building.** In a category with retractions every arrow is a composite of two non-identities, so generators are given by constructions and not derived; the faces and degeneracies of the simplex category up to [2] with the cosimplicial identities among them do present it, in 31 arrows; and they are not the fewest arrows that generate it.
- **Not built:** equality of string diagrams modulo a theory's laws (rewriting with convex matches); enriched and higher categories; the internal language; categories as abstract interfaces for infinite cases; the market foam's categorical layer on this foundation; a card of the board.
- **A repository to create.** kategoria needs a repository on GitHub, as each foundation has; until then the infrastructure's commit that points to it cannot be cloned whole.
- **Provisional:** the name, and the studio's colour (an amethyst with a rose-quartz minor), until the user chooses.
