# The Schema of Historical Objects

*Status: draft ([ADR 0110](../../../development/decisions/0110-historical-objects-and-histos-harmonia.md)).* What a record of historical objects is, stated as categories in kategoria's language of scripts, so that its well-formedness is decided by a verifier and not asserted in prose. The mathematics is in the [theory note](../theory/historical-objects.md).

| File | What it is |
|---|---|
| [`historical-objects.cat`](historical-objects.cat) | The script: six parts, 41 statements. A statement with `# expect fails` on the line above it is meant to fail, and says where; every other holds |
| [`figures/`](figures/README.md) | kategoria's answer to each statement, and eleven diagrams drawn from its views by the visualization layer's `categories` |

## 1. What the Script States

1. **The kinds** are a thin category with a terminal object, the historical object. One equation is needed: a school is a tradition and a collective, and must be an object in one way; without it the kinds are not an order (shown failing).
2. **A discipline's kinds** extend the core's by a functor that is full and faithful: nothing among the core's kinds is added or merged.
3. **Layers.** Exemplifies, specialises and part are presented with three equations, completed to a confluent rewriting system; a part of what exemplifies a class does not exemplify it (shown failing); taxonomy and chronology are isomorphic.
4. **The record** is a finitely presented category whose four equations are the typing of relations, with the witness of "is a kind of" in a table of its own. A sample record is a functor on it; a record in which a person draws on a work is not (shown failing, at the equation).
5. **What the desk, the study and a discipline add** each stand on one base, an object and its kind, by a faithful functor; what the core reads of a discipline's record is its pullback to the base.
6. **Time.** Dating is a functor from dependence to the order of years; an arrow against time has nowhere to go (shown failing).

## 2. How It Is Checked

- The domain's check `check_history_schema` carries the script out by kategoria's session and requires every statement answered as expected; it holds the C++ order of kinds to the script's, pair by pair; and it writes every record of the domain as a set-valued functor on the script's category `Record` and requires kategoria to find it one.
- `node applications/histos-harmonia/tools/draw-schema.mjs` does the first of these through the Category Studio's engine and saves the diagrams.

## 3. Open Questions

1. The diagram of the record is crowded: thirteen tables and twenty-three arrows on one plane. A drawing by parts would read better.
2. Acyclicity, the rule of grades and that a premise precedes what it is a premise of are not equational, and are checked by the reader alone.
