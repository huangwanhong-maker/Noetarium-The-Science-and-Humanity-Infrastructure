# 0099. The Market Foam: The Generative–Relational Formalism in the Markets Domain, and a Plug-In of Market Studio

- **Status:** Accepted, provisionally (2026-11-12).
- **Date:** 2026-11-12
- **Decided by:**
  - **The request.** The user, handing in three drafts of their formalism (the Unified Design Draft v0.9, *Market Foam* v0.9a, and the case studies on inflation and justice v0.9b): "Now implement these formalism. I think can as a plug-in?"
  - **The design.** Claude, here.

## 1. Context

The drafts design one object for market analysis in five layers, a market history as a labelled two-complex (the market foam), and two cases read on it. Their proofs concern explicitly specified models and their checks are synthetic; the scripts were not handed in.

The question was where such a formalism lives. A plug-in was proposed. The infrastructure already holds much of its mathematics: morphe's worldline complex (on which the history domain and economics' spacetime of transactions stand), physis's Hodge decomposition, Kirchhoff networks and master equations, calx's matrix exponentials, and in economics the holonomies of transfer networks.

## 2. Decision

### 2.1 Three Places

- **What a person does with the formalism is a plug-in of Market Studio:** tools that read a record of movements.
- **Its mathematics is the markets domain's,** in C++ with checks, as for every formalism of the user's made exact here (ADR 0047, ADR 0051, ADR 0062).
- **What names no market is the foundations',** written there first.

The apparatus of the drafts (propositions, labels, amendments) stays in the domain's theory note. The plug-in says results plainly, and a check asserts that its tools name no proposition.

### 2.2 The Foundations

- **tyche** (its decision 0016): `probability/information.hpp`, the chain rule of relative entropy through a record: what a record shows of the difference between two laws and what it hides; and the rate at which two jump processes part. 22 checks.
- **nomos** (its decision 0011): `games/claims.hpp`, the Shapley value and the division of an estate short of its claims (priority and its range over orders, random arrival, the proportional rule, equal awards and losses, the Talmud rule, payment by turns). 28 checks.

Nothing was written again: relative entropy and stationary laws are physis's, the potential of a ratio cochain is physis's Hodge decomposition, effective resistance its Kirchhoff networks, matrix exponentials calx's.

### 2.3 The Markets Domain

Five modules, in the fragment where events change quantities only (as the drafts' proofs are). 138 checks.

- **`system`:** a grammar of events as vectors on places; its invariants as a kernel; the classical, Gaussian and path regimes; simulation with each type's clock; the distance a tracer's rates induce; and a market from rules, a record whose truth is known.
- **`foam`:** a record of movements as a history, booked as bearer holdings or at ledgers; the history complex with its two ends; slices; the worldline order with its density and longest chain; proper time; the balance at every event and the Gauss law at a ledger; the area law.
- **`valuation`:** the reference valuation of a record's exchanges with each exchange's residual and each subject's advantage; loops through time in their geometric and dynamical parts; arbitrage on triangles; cycle maps.
- **`money`:** velocity; Fisher's loop; an injection of money followed in full and to first order.
- **`procedure`:** order sensitivity; the division of a short estate by every rule; procedures as laws on histories (the weight of admissible histories, reliability, what outcomes hide, asymmetry under an exchange of parties); the hearing condition on a history.

Between events the history complex is morphe's worldline complex of the record of touches (the theory note's Proposition 3.3). The construction of the history domain, of economics' spacetime and of this one is the same.

### 2.4 Five Amendments

The theory note, `domains/markets/theory/generative-relational-formalism.md`, states what holds and what the drafts should revise. Each amendment is held by a check.

- **A1.** Under arbitrage on triangles a cycle outside their span does not keep its holonomy when it shares an edge with a triangle. What persists is the harmonic part of the ratios.
- **A2.** The concurrent procedure is not one rule: fine interleaving removes the order and leaves the rates. At rates in proportion to the claims it is the proportional rule, at equal rates the constrained equal awards.
- **A3.** The presentation of a claim between holder and issuer is not the one the existence proof constructs from the minimal schema.
- **A4.** In the injection model the receipts are $(W^{\top}m)$. Conservation of consumption then needs no symmetry, the rest state is the stationary law of the shares, and the first-order gains have a general form.
- **A5.** With one kind at one ledger the causal order is total; the draft's 0.957 needs its setup stated.

The drafts' numbers that follow from their text are reproduced, among them the gains and the half-rise times of the ring of twelve.

### 2.5 The API

`api/cpp/science/market_record.hpp`, beside `markets.hpp`: a record stated in the request as movements, and seven operations (`generate`, `foam`, `reciprocity`, `circulation`, `injection`, `division`, `discrepancy`). Market Studio's engine answers both headers.

### 2.6 Plug-Ins of Market Studio

Market Studio gains a seam. A plug-in is a directory under `plugins/`: `plugin.json`, `tools/*.py` (a group of tools under its name), `templates/*.json`, and `templates/data/` (small files a template's project is given). Nothing else of the work changes when one is added.

The first is `market-foam`, *A record as a foam*, with eight tools on a table of movements whose columns the person names:

- *A market from rules*; *The record's spacetime* (with a figure of its first events as a complex);
- *Who has the better of whom*; *Loops through time*;
- *Money's round*; *New money, and who gains*;
- *Dividing what is short*; *Two mechanisms, one record*.

Its template runs them on a record of some two thousand exchanges made by rules.

### 2.7 Checks

- **Foundations:** tyche 843 (22 new), nomos 339 (28 new).
- **The markets domain:** 236 (138 new).
- **Market Studio:** 68 (19 new): the seam, the plug-in's template, and its plan run through the server with every answer held to the rules' truth or to a closed form.
- **The page** was driven in headless Chrome (10 assertions, not in the suite). Five pictures are in the gallery.

## 3. Alternatives Considered

- **Only a plug-in, its mathematics in Python.** The computations would then live in a work, unchecked against the domain's standard and unusable by other works.
- **A new domain.** The formalism is of markets, and the markets domain is where a market's objects are. Its valuation part meets economics' transfer networks, which the bridge may use.
- **The economics domain.** Economics holds value carried between positions and the companion drafts' loop deficits. This formalism is wider: events, holdings, procedures. Holonomy here and the loop deficit there are one quantity with opposite signs, and the note says so.
- **Implementing the categorical layer first** (schemas as categories, attributed rewriting). The drafts' own proofs do not use it, and nothing computed here needs it yet.

## 4. Consequences

- **Easier:** a record of payments or trades from anywhere is read by the eight tools once its columns are named. A further reading is a file in the plug-in.
- **Not built:** the categorical layer; observers as schema views and the reflexive extension; the classical cases of the Unified Draft beyond those above; rule change and kind creation on the complex; the three hypotheses. Field data: every check is synthetic, as in the drafts.
- **For the drafts:** the five amendments, and the remark that a first-receiver advantage reaches only as far as spending is concentrated: where each spends evenly with all, the injected subject alone gains.
