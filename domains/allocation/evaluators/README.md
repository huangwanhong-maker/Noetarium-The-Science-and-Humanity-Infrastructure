# evaluators: Economic Evaluators of Allocations Whose Order Matters

*Status: draft (2026-11-21; [ADR 0139](../../../development/decisions/0139-economic-evaluators-of-allocations.md)).*

## 1. Concepts

An evaluator of the allocation domain's `evolution` reads a decoded allocation (what each recipient was granted, and the order of the grants) and answers an assessment: metrics with their roles and standings. The three here are economic, and each makes the order of grants matter for a reason of its own, which it says.

- **An economy of places.** Each recipient is a place of the economics domain's regional economy: households and firms trading with one another and with outside, every payment posted in balanced accounts. Grants reach each place's households in the allocation's order, a set number of periods apart, so that money given first is spent before the next grant arrives. Households short of money spend differently from those with money; through that state-dependence, money given to one place first changes how the next place will spend its own grant. The reading follows each grant from where it entered: the money re-spent in its place (minimised as a loss, with each place's own for the comparison under principles), the grants' LM3, the share that left, the payments that waited. The accounts' audit is a verified constraint: a candidate whose run does not balance is infeasible, not good.
- **A declared law of holdings.** Each recipient is a density of its units' holdings, moved by Bouchaud and Mézard's law toward the mean of all the recipients' units, so that a grant to one raises the mean that pulls the others. A grant fills the gap of its units below a line, in proportion to each unit's gap, and what remains is shared by all; the reading is the share of all units below the line. Two orders end differently because a transfer to those below a line depends on the density at the moment and the mean field on everything given before: what the declared law implies, not that anything behaves so.
- **Laws learned from a record.** An ensemble of laws, each a map from a state of one value per recipient and an input to the next state; the allocation's worth is their weighted mean of the final state, an estimate. A grant that drives an input or a state beyond the record's range is a verified violation of the record's support: the laws do not speak there. Two allocations are compared across the laws with a standing (holds, fails, arguable, undecided).

## 2. Content

- `economy.hpp`: `EconomySpec{resource, unit, places, habits, trade, spacing, periods, rounds}`, `EconomyReading{assessment, traces, outcome}`, `read_economy(spec, allocation)`, `economy_evaluator(composition, spec, name)`;
- `density.hpp`: `DensitySpec{resource, unit, population, mean, grid, pull, spread, line, dt, between, after}`, `DensityReading{assessment, densities, below, mean, start, beyond}`, `read_density`, `density_evaluator`;
- `laws.hpp`: `LawsSpec{resource, unit, laws, start, between, after, states, inputs, margin}`, `inputs_of`, `read_laws`, `laws_evaluator`, `rank(spec, first, second)`;
- `evaluators.hpp`: all three.

*Caveat.* Every habit, law and spacing in a specification is a declaration. The readings rank allocations under one model each; they do not say how places, households or a city behave, and a learned law's ranking holds only within the record it was learned from.

## 3. Checks

`check_allocation_evaluators` (18): see the domain's README, §4.

## 4. Dependencies

`allocation::evolution`; the economics domain's `ledger`; kinesis's `densities` and `induction`.
