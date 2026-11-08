---
title: What Urban Science Should Take Up Next, and in What Order
status: seed
date: 2026-11-08
related: [the urban-science and architecture domains, the Urban Science Workbench, peira (ADR 0082), heuresis (ADR 0073), the user's temporary note on modern methods in architecture, urban design and spatial demand (docs/, not committed)]
---

# What Urban Science Should Take Up Next, and in What Order

*A temporary record of priorities, at the user's request (2026-11-08), after reading the user's note on modern methods in architecture, urban design and spatial demand. It will be revised as the work proceeds; each item becomes an ADR when it is built.*

## 1. What Is Already Held

- Agent-based models of walkers, households and occupancy (ADR 0075).
- Reach from a place picked (urban-science `access`, ADR 0078).
- Exposure to hazards.
- Moran's I (ADR 0080).
- The heat of buildings under the Sun (architecture `environment`).
- A district redesigned under five objectives (heuresis, ADR 0073).
- The Studio (ADR 0081).
- Designs, surveys, survival and causal inference (peira, ADR 0082).

## 2. The Gaps, Checked Against the Code

- No network centrality at all: no closeness or betweenness, so no Space Syntax.
- Accessibility is computed from one place only. Neither $A_i = \sum_j O_j f(t_{ij})$ for every origin nor the gaps between supply and demand are computed.
- Choice probabilities exist (nomos `agents`), but no conditional, nested or mixed logit can be estimated.
- No spatial econometrics beyond Moran's I: no spatial-lag or spatial-error models for hedonic prices.
- No evaluation of street interventions before and after; it waits on peira's second stage.

## 3. The Order

| Stage | What | Where | Why in this place |
|---|---|---|---|
| 0 | **Geometric syntax** (G³ brought in): geometric graph grammars learned from street networks and buildings. Deterministic, probabilistic and context-sensitive forms; grammars whose words are 2-simplices; a syntax analyser whose parsing is seen step by step and saved | syntaxis, a ninth foundation (the general engine, at the user's agreement; physis for triangulations); urban-science and architecture `syntax` (streets, buildings); a workbench plug-in | The user's choice to begin (2026-11-08): a preliminary G³ exists, and the benefit to the infrastructure is high. **Built:** [ADR 0083](../decisions/0083-syntaxis-grammars-whose-words-are-geometric.md) (2026-11-08) |
| 1 | **Street configuration.** Space Syntax as graph centralities (closeness as integration, betweenness as choice; metric, angular and topological costs; radii) | morphe (centralities); urban-science `syntax` (the segment map) | A real gap with ground truth: Melbourne's counters test configuration against our destination-based walker model (R² 0.72). **Built:** [ADR 0084](../decisions/0084-the-configuration-of-streets.md) (2026-11-08): configuration explains almost none of where walkers are counted (ρ ≤ 0.22 on the primary tests), and the walkers' model no more between places; what stands at a counter matters most |
| 2 | **Accessibility and unmet need, anywhere.** Gravity and cumulative opportunities; two-step floating catchments; the Rural Access Index | urban-science `access`; Overture's places and roads; population grids | Any city or rural area the workbench opens; measures need, not use |
| 3 | **Causal evaluation of street changes.** Differences in differences, event studies and synthetic control on the counters | peira stage 2 | Effects of real interventions; how latent and induced demand are identified |
| 4 | **Demand estimation.** Discrete choice estimated; stated-choice designs; spatial-lag and spatial-error models; hedonic rents | tyche or nomos (logit), peira (designs), tyche `spatial` | Preferences and willingness to pay, revealed and stated |
| 5 | **The generative design loop.** Massing and frontage rules, sun and shade on streets and façades, heuresis's Pareto alternatives in the Studio | architecture `form`, `frontage`, `environment`; heuresis | "What could be designed", scored by configuration and access as well as energy |
| 6 | **Land use and transport interacting.** Housing, movement and access fed back over years | urban-science | Needs stages 2 and 4 |

**Left aside for now:**
- Street-view computer vision: imagery licences, heavy models, and the reproducibility problems the note itself cites.
- Fluid dynamics for microclimate: sun and shade give most of the value.
- BIM, life-cycle assessment and post-occupancy evaluation: they need building data we lack.
- Generative AI for layouts.
- Participatory geodesign: in the Studio, after the user's feedback.

## 4. A Remark on Demand

Observed use is demand passed through constraints: access, supply, environment. Latent demand is a counterfactual quantity. It is identified only where the constraints vary for reasons of their own (a closure, a pilot, a new route) or by a stated-choice experiment, which makes it an identification problem in peira's sense.
