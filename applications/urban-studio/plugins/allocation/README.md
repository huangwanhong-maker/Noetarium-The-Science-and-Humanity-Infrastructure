# Allocation and justice

*Status: draft (2026-11-20; [ADR 0136](../../../../development/decisions/0136-allocation-under-scarcity.md)). A plug-in of the Urban Studio.* When there is not enough: who is served first, what each receives and how much, and by what procedure, as a project. The mathematics is the [allocation domain](../../../../domains/allocation/README.md)'s, on nomos's rules and welfare orders, archeion's journal and the economics domain's circulation, composed by the API's `science/allocation.hpp`.

## 1. What It Does

- **A round as the page.** The view *Allocation* draws a round as a channel: the resource enters at the left, each declared stage is a sluice that lets water off to the plots it serves, and the plots stand in the order the round reached them. A plot opens where it stands, with what each stage gave it and why, what is at stake for it among the undominated options, and a form to file an appeal on a datum.
- **The rounds it could have been.** Beneath the channel, every rationing rule a stage could have used and every order of the stages, compared under the case's principles at once, drawn as the Hasse diagram of dominance; the pivots say where the choice turns on which principle is given precedence. Principles are never summed.
- **The record.** Publish the round, consult groups, apply it, file and review appeals: each act is sealed in archeion's journal, an appeal is heard for everyone, and the rules of procedural justice are drawn as seals, held, failed or left to people.
- **A participatory budget counted again.** For a Pabulib case, the ballots under utilitarian greedy, the Method of Equal Shares and Phragmén, beside the city's selection, with the voters left with nothing in each neighbourhood.
- **A model proposes.** Stages proposed for a situation (no numbers), taken into the round or set aside; asked only from the work's own page at this computer.

## 2. Tools

| Tool | What it answers |
|---|---|
| `round.py` | The declared round on a case: what each entity receives, stage by stage, with who was reached first and the stages' totals |
| `judge.py` | The round against its alternatives: each option's value under each principle, the undominated, what is at stake |
| `budget.py` | A participatory budget under three rules and the city's own count, overall and by neighbourhood |
| `targeting.py` | A fund to England's most deprived neighbourhoods against the people it is meant for; re-spending and capitalisation under declared responses |

## 3. Templates

- **England: a fund for districts by deprivation** (`england-councils`): the 296 districts of the IoD 2025 (OGL v3.0); an illustrative £1 bn, 85% as a claim of £80 for every income-deprived person, the rest by Porto Alegre's banded points.
- **Melbourne: a care fund, who first** (`melbourne-care`): the City of Melbourne's areas at the 2021 Census (CC BY 4.0); an illustrative A$2 m, three quarters served area by area from the highest share needing assistance.
- **Warsaw's participatory budget, counted again** (`warsaw-budget`): Pabulib's Warszawa 2023, 78,904 voters. Pabulib states no licence: the file stays in the cache, and only derived results are shown.

Every amount, claim, weight, band and response in them is a declaration of the example, illustrative and no authority's.

## 4. Server Side and Storage

`routes.py` answers under `/api/projects/<project>/allocation/`: the study, computing a round with its judgement and record, a budget, and a model's proposals. A project's study is kept as `allocation/study.json` (the case, the settings, any stages taken, the acts of the record).

## 5. Checks

In the Urban Studio's suite: the plug-in's parts; England's fund divided to the pound and judged (proportional claims undominated, equal awards to districts dominated); its record replayed with an appeal heard for everyone; a fund to the most deprived decile against the people it is meant for; Melbourne's pivot; Warsaw's ballots counted again, utilitarian greedy reproducing the city's selection.
