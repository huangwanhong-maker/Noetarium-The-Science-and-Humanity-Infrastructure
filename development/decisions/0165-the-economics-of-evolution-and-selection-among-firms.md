# 0165. The Economics of Evolution, and Selection Among Firms

- **Status:** Accepted, provisionally
- **Date:** 2026-11-23
- **Decided by:** the user, who asked whether an economics instantiation of the infrastructure's evolutionary meta-framework exists ("if not exist we can implement one"), with evolutionary economics surveyed first; then chose the model side first, a sub-domain inside the economics domain, and connections to the infrastructure's other frameworks ("this is why we identify this repository is a 'integrative' infrastructure"). After the survey *Evolutionary economics models and methods* they chose, among the options it raised: its modules as modules of the economics domain, the domain declaring heuresis when a module first needs it, and the Price decomposition in tyche as the first piece.

## 1. Context

The survey found one exact core shared by evolution as an object of economics and evolution as a method of search: Price's equation, of which the productivity decompositions of firms, Fisher's principle and the replicator are instances. It found a handful of analytic models with closed forms, many simulation models whose validation is weak, and measures of relatedness and complexity that predict without identifying causes. Its §7.5 orders the building: the exact core in the foundations, then the domain's analytic models and measures, then simulation models with published targets, then estimation, then connections through the API, then works.

## 2. Decision

1. **The economics of evolution is a group of modules of the economics domain**, introduced together in its README: `selection` (populations of firms and the decompositions of their productivity), `industry` (industry dynamics as populations and agents, later the Keynes + Schumpeter family), `diversification` (relatedness, complexity, branching) and `adaptive` (economies of learning agents). The names are provisional; `evolution` is the allocation domain's.
2. **The economics domain will declare heuresis** with the first module whose routines, rules or technologies vary by evolutionary operators (genetic-algorithm learning, the K+S search, NK landscapes). It closes no cycle.
3. **General methods go to the foundations first.** The census record and the Price decomposition are tyche's `census` (its decision 0035), on which `selection` composes; revision protocols and the replicator family will be nomos's; urns and growth processes tyche's; NK landscapes heuresis's.
4. **The first module, `selection`** (theory note `theory/selection-among-firms.md`, 18 checks): a panel of firms at two dates read as tyche's census record, and aggregate productivity growth decomposed by Baily, Hulten and Campbell's, Griliches and Regev's, Foster, Haltiwanger and Krizan's, Melitz and Polanec's dynamic Olley–Pakes and Price's decompositions, the last with selection between industries and within them; Olley and Pakes's static decomposition; logs of a panel in levels.
5. **Connections go through the API**, never as dependencies of domains: the allocation domain stands on economics, and joining economics with policy would make a bridge the rule of two occurrences argues against.

## 3. Alternatives Considered

- **A nested sub-domain folder** (`domains/economics/evolutionary/`). It would be a new kind of structure, a domain within a domain, and the present rules hold the group as modules; the user chose modules.
- **The productivity decompositions without tyche's record.** Each is an instance of Price's equation with a reference for entry and exit; writing them on the record makes their relation exact and checkable, and lets the same record read a search's runs.
- **One decomposition only.** The methods differ in what they call entry (Melitz and Polanec 2015, §2) and in their invariance to the level of productivity; each is offered, named, and checked against the others.

## 4. Consequences

- **Checked:** a panel of three firms worked by hand in every method; Melitz and Polanec's closed form for a representative firm (entry credited 0, s_E γ/2 and s_E γ); a constant added to every productivity moving Baily, Hulten and Campbell's entry, exit and between and no other method's terms; Olley and Pakes's identity; every method summing exactly on 148 random panels.
- **Not built:** a reader of firm panels (no open firm-level panel was chosen; the survey's §7.4 names candidates); the API's composition and a tool; the remaining modules in the survey's order (nomos's revision protocols next).
- **Provisional:** the module names and their grouping.
