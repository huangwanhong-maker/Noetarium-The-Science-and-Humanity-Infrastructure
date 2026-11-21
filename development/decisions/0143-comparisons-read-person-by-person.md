# 0143. Comparisons Read Person by Person

- **Status:** Accepted, provisionally
- **Date:** 2026-11-21
- **Decided by:** the user's "Continuer", taken as leave to build what the design of evolutionary allocation left for later: "people's comparisons kept per person, with each person's own audit of consistency" (`development/ideas/2026-11-20-evolutionary-allocation-of-heterogeneous-resources.md`, *What Is Not Built*).

## 1. Context

ADR 0138 let people compare kept allocations in pairs on the Urban Studio's page and read their judgements as Bradley–Terry strengths, beside the search and never in it. Each judgement already named who made it, but the strengths were read for everyone together. A model's judgements were audited for consistency (each pair shown both ways, a share asked again) and a person's were not, though the page shows a pair again in the other order.

## 2. Decision

1. **The audit** (the visualization layer's `allocation/evolution.js`, `auditJudgements`). Each person's judgements are counted, with the pairs they judged and the pairs they judged more than once. A pair is consistent when every judgement of it prefers the same allocation, wherever it stood.
2. **The page** (*Allocation and justice*'s view). Everyone's judgements are read together as before, then each person's apart, each row labelled with that person's judgements and consistency. With one person, the consistency joins the common row.

Nothing of it enters the search, and no person's strengths are added to another's or to a model's.

## 3. Alternatives Considered

- **Weighting people by their consistency.** That would rank people, which the page has no warrant to do. Each person's consistency is shown, and the reader weighs it.
- **Asking a person to repeat pairs on purpose.** The page already swaps the order on the second showing; asking for more is left to the people.

## 4. Consequences

- **Checks:** the visualization layer 1 (933). Ada judges one pair twice, in both orders, and alike; Ben judges a pair twice and chooses otherwise; an unnamed judgement is a person's.
- **Not built:** several people's judgements joined by a model with each person's own order effect.
