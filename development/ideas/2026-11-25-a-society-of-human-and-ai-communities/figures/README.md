# Figures of the Design

Fourteen figures, each drawn at print size (720 units wide for a text width of 162 mm, lettering of 11 units or more) with formal labels only. The captions below hold what explains them. Each figure exists as SVG (the source of record), PDF (vector) and PNG (three times the size).

## Making the Figures

```
node figures.mjs            every figure, as SVG
node figures.mjs 3 9        figures 3 and 9
node render.mjs             PDF and PNG of every SVG, with headless Chrome and Source Serif 4
```

`lib.mjs` holds the shared drawing helpers and `render.mjs` the renderer, both taken unchanged from the figures of the design of 2026-11-20. Figures 10, 11 and 14 read `../sketch/results.json`, so the sketch's checks are run first. Every figure was rendered and inspected, and every label stands clear of lines, boxes and other labels.

## Captions

**Figure 1. The levels of order.** Four levels, with what is shared at each and the ground on which each binds. Practices live in scenes and bind by reliance. A constitution is declared by one community in seven kinds of rules with a rule of amendment, and binds those who enter its positions. Compacts and leagues are made among communities and bind the parties that ratify them. The ground (acts, the chronicle, the fold and eight invariants) is common to all and binds by conformance.

**Figure 2. Parties, positions, mandates and origins.** A person and an organisation issue mandates to operatives; a mandate issued under another is narrower. Operatives hold positions that a community's constitution opens to them. A commitment made by an operative is answered for by the root of its chain (dashed line). Operatives that one party controls form one origin: six operatives here, two origins. The provider of a model is a second relation, under which origins can also be grouped.

**Figure 3. An act, the chronicle and the fold.** An act has an envelope of typed fields and a free body. Admitted acts are sealed in a chronicle. The fold reads typed fields alone and gives the relational state, or refuses an act with its reason. Bodies are for readers. Balances, lists of members and reports are views over the state.

**Figure 4. The life of a commitment.** States and the acts that move between them, coloured by the side that makes each. An offer is conditional until taken; performance is presented by the debtor's side and accepted by the creditor's. A cancellation by the debtor and a handover at the end of its authority stay on the record until the creditor releases them; the creditor may release an active commitment as well. A commitment is disputed while a claim about it is open.

**Figure 5. A constitution as data.** The seven kinds of rules of Ostrom's analysis, each acting on one element of an action situation: positions, the acts open to them, the information available, the way decisions are reached, the returns, and the outcomes that may be affected. Three levels of rules, each changing the next. Five declarations stand beside the rules.

**Figure 6. Eight presets as profiles.** For each preset, the options it takes for entry, decisions, returns and circulation. The presets are starting files of one schema. The garden's returns come from what other lineages carry, which lies outside the options listed.

**Figure 7. Communication within and across communities.** Scenes inside each community. Between two communities: recognition in each direction, an act carried by an envoy and answered by a receipt so that it is entered in both chronicles, an interface that relates the two vocabularies, and a compact that each ratifies. A league is a community whose members are communities, with offices that serve them.

**Figure 8. A norm as an entry with four facets.** Six routes by which entries arise. An entry holds a statement, its purpose and its open terms, and four facets: regularity and operation are measured from the chronicle; adoption and justification are made by acts. Adoption and operation together give four cases, two of which the system marks. Entries are revised, retired or carried to another community.

**Figure 9. Clearing.** (a) Set-off and novation on the example of the Commitments paper: A owes B 6, B owes C 5, C owes A 4. Set-off cancels the common amount of the cycle and novation replaces a chain by a direct obligation; gross obligations fall from 15 to 3 to 2, and net obligations stay 2, −1 and −1. (b) A cycle of performance among four parties, each in its own unit, and a chain opened by a gift.

**Figure 10. The constructed experiment.** (a) A balanced circuit of 18 parties and six kinds of service, 200 economies: the share of needs met by mutual swaps, by cycles of bounded length found greedily, by exact set-off, with reciprocity set aside (feasible), and by exact set-off with chains from two and four donors. (b, c) Unequal capacities, 100 runs of ten rounds: the share of needs met for the weakest third and for all parties under set-off alone, under a debit limit of 3 and of 6 over the whole history, and under provision free of a personal gate. The construction and its limits are in Section 6.7 of the design.

**Figure 11. Records and returns.** Left: a graph of use with the share that each artefact passes to its inputs; 100 arriving at the service is kept as 50, 30, 10 and 10. Right: description, causal account and entitlement, each with its own source, kept apart.

**Figure 12. A claim: complements, procedure and forums.** A claim has a route when a responsible party, a competent forum, usable evidence, authority for the response and capacity to perform it are all present. The default procedure runs from intake to review, with provisional protection where needed and the remedies available. Forums across the levels of order, joined by referral.

**Figure 13. The five places and the node.** Left: where the parts would live in the infrastructure, with what is proposed marked; arrows point from a library to what stands on it. Right: a node, with its store, the kernel as an engine, scenes, the exchange between nodes and three adapters.

**Figure 14. A bounded contribution, act by act.** Fourteen of the 37 acts of the worked record of the sketch, in order, between a sponsor, the operative it lends, a steward and a reviewer, coloured by force. Two acts are refused: a delivery that would pass the ceiling of two million tokens, and a remark after the mandate is revoked. The column at the right shows the tokens charged along the way.
