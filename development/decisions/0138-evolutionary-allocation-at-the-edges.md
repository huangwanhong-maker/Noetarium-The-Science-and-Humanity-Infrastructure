# 0138. Evolutionary Allocation at the Edges

- **Status:** Accepted, provisionally
- **Date:** 2026-11-20
- **Decided by:** the user, after ADR 0137: "push, then continue". The design's next stage is its Stage 6, the edges.

## 1. Context

ADR 0137 composed evolutionary allocation of heterogeneous resources in the allocation domain, reachable only from C++. The design's Stage 6 asked for the API's composition, a language model's pairwise judgements with the order swapped, several samples and an audit share, people's comparisons through a page, drawings, and the Urban Studio's tool and view. Building the demonstration on Melbourne's areas showed two things the composition lacked.

## 2. Decision

1. **Partial grants** in the allocation domain's `evolution`: a claim or a fixed demand may be declared partial, met up to what remains when the pool can no longer meet it, instead of refusing the grant; the order then decides who is met in full and who in part. Shares, units and items stay exact.
2. **Kept allocations distinct for the people:** two allocations are kept apart only when some recipient receives something different at the resources' grains (kategoria's exact parts), so variants differing in an attribute or below a grain are one, kept by its best; and only when they stand a declared distance apart in the genotype's own metric (by default half a typical distance), a niche each after Pétrowski's clearing, so that what people are asked to compare are alternatives, not neighbours of one (the first pictures showed two allocations a thousand dollars apart, which no one could tell apart).
3. **The API's `science/evolve.hpp`**, answered through the allocation engine: `evolve.verify`, `evolve.compose`, `evolve.search`, `evolve.strengths`. A problem, its resource types (components, quantities, kinds, conversions, laws) and its evaluators (declared closed forms: each recipient's value a sum of terms over amounts, works a price bought, items aligned with a direction, or being granted) are JSON; a declared price that rises after each grant drawing on a resource.
4. **`science.allocation`** declares them in Python and asks the engine; `judge_pairs` asks a model to compare kept allocations in pairs against a stated purpose, each pair shown both ways, a share asked again to audit its consistency; the model is shown the allocations in words and never the search's metrics, under a role of its own, and its judgements are grade C, read by Bradley–Terry with the order effect and never entering the search.
5. **The visualization layer's `allocation/evolution.js`:** the field of the kept (total value against the least served recipient), an allocation on its recipients with the thread of grants (places spread where they crowd), which grants commute, the resource types' laws as seals, pairwise strengths with a model's marked as proposed.
6. **The Urban Studio's plug-in *Allocation and justice*:** the tool `evolve.py`, the routes `evolve`, `strengths` and `judge-pairs` (a model only from the work's own page at this computer), the view drawing the evolved allocation as the page with people's pairwise comparison beside it, and the template *Melbourne: resources of many kinds, by evolution* on the City of Melbourne's areas (a declared care fund, crews, duties that may not be thrown away, three sites with a canopy's direction, a price that rises, a precedence; every number illustrative).

## 3. Alternatives Considered

- **Evaluators as code sent to the engine.** Declared terms cover the case and stay data a person can read and audit; code would make the engine run what a page sends.
- **The general role for the model's comparisons.** It forbids ranking options, and the model then declined every pair (26 of 26); a comparison is one judgement between two, recorded as a proposal, and has a role saying so.

## 4. Consequences

- **Checks:** the allocation domain's `evolution` 16 (100 in the domain); the API's allocation checks 48; the visualization layer's allocation drawings 29 (the layer 932); the Urban Studio's suite 145; the page driven in headless Chrome (9 steps, not in the suite), pictures in the gallery's §50.1.
- **What the Melbourne case shows** (declared, illustrative): met in part, the fund reaches 12 or 13 of the 13 areas, against 5 when a claim that cannot be met is refused; the declared price, rising 5% with each funded job, concentrates the fund on four or five areas, since funding fewer buys more works; seeds differ by up to 5% in the total found. A model asked to compare the kept allocations told 2 of 26 pairs apart and judged the rest too alike to say.
- **Not built:** Stage 7 (the economic evaluators), Warsaw's budget as the order of a sequential mechanism, the OR-Library's files read by the data library, people's comparisons kept per person with their own audit.
