# Diplomacy: Multilateral Relations Among Parties of Every Kind

*Status: draft (2026-11-19).* A bridge at height 1 over law and policy, C++ from the start ([ADR 0132](../../development/decisions/0132-diplomacy-and-multilateral-phronimos.md)). It serves **use** and **research**: how an undertaking among many parties holds together, who may decide what in it, what each party commits, gives and finds in it, and where it fails; and, since [ADR 0134](../../development/decisions/0134-the-affairs-of-an-undertaking.md), the affairs by which it changes: decisions taken by rules, negotiations, events, mandates, reviews, and the institutions it sits among.

## 1. Concepts

**Multilateral practice is a structure of relations, not a list of parties.** The parties are states, international organisations, cities, firms, civil society, communities, persons and, increasingly, artificial systems and datasets. What makes them an undertaking is the relations among them: who authorises whom, who relies on whose claims, who may contest whose decisions, who tells whom. The user's monograph (Huang 2026, §10.4.2) writes an undertaking as R = (V, {E_k}, D, Γ): participants, typed relations, decision points and constraints. The domain holds that structure, with three distinctions kept throughout.

1. **Not observed is not absent.** A tie is present when recorded, and absent when recorded absent or when a declared design of observation covered both ends and found none. Every other pair is *not observed*. A complete register of communication may stand beside an ambiguous record of authority, and the domain says which.
2. **A decision is a record with its authority, evidence, those affected and its venues of challenge** (the monograph's Appendix D asks for this minimum). The undertaking diagnoses itself from these records (its Table 10.2): a decision with no evidence; one made by a participant holding no authority recorded (a recommendation recorded as a decision); one with no venue, or affecting someone with no route to one; one resting on artificial systems alone, or counting one source twice; a participant without whom the relations of authority fall apart; a participant on whom most reliance falls.
3. **Mutual benefit is never one number.** Each party's benefit is read under its own criterion, taken from its own reading of the term with that reading's source. The answer is a dual verdict (kybernesis): a standing over what the evidence allows, and a probability bounded without assuming how the criteria depend on one another. Values on different dimensions are compared only as their scales permit. They are summed only by a rule someone declares, and the rule is refused where its scale forbids it (Proposition 12.1 of the monograph: alternatives that cross have no unique ranking without such a rule).

**Partnerships are multilateral relations with commitments.** A commitment is screened by declared criteria (by default the SMART criteria of UN DESA's SDG Actions Platform). Its state at a date is honoured, honoured late, in progress, overdue or *unreported*, never failed for want of a report. The modes of failure that the survey finds detectable from a structured record are signs named with where they show: a paper partnership, drift, resource fragility, capture, collaborative inertia, accountability gaps and succession loss.

**An undertaking changes through its affairs** (ADR 0134). Each affair is a record of acts in time, and what it generates (an outcome, a narrower zone, a lineage, a repeat) is read from it, never assumed:

- a decision is taken by a rule stated in the body's words, whose open terms ("concurring votes", "present and voting", "formal objection") are read under every reading with its source, so that an outcome is clear, arguable or untenable and says what it turns on;
- a negotiation narrows draft by draft, its zone of agreement the packages every party's negotiator and ratifiers can accept, each value an interval;
- an event rests on its reports, each origin counted once;
- a mandate is given, renewed, amended, superseded and reviewed;
- a review's recommendation is answered, and its implementation claimed, evidenced or contested;
- institutions overlap, nest and carry reciprocity specific or diffuse.

**Nothing here is any institution's scheme until a caller declares it.** Relation types, observation designs, criteria of a screen, thresholds of a diagnostic and rules of aggregation are all declared; the checks state theirs as examples, on invented cases.

**What diplomacy is not.** Treaties, sources of law and proceedings are the law domain's; theories of change and transfer to a place are the policy domain's. Voting rules, power indices and bargaining are nomos's; inference is tyche's; the loop from evidence to action and its dual verdicts are kybernesis's; archives are archeion's. The domain composes them for multilateral practice and names what only that practice names: participants of many kinds, undertakings, commitments and their partners.

## 2. Modules

| Module | Holds | Status |
|---|---|---|
| [`relations`](relations/include/diplomacy/relations/relations.hpp) | An undertaking as R = (V, {E_k}, D, Γ): participants of declared kinds, ties of declared types in time, each present, absent or not observed under declared designs; decisions with authorities, evidence, those affected and venues; constraints; a participant's relational position at a date; the undertaking's diagnostics (evidence, authorisation, participation, AI dependency, succession by morphe's blocks, concentration, what is not observed) | draft: 29 checks |
| [`partnerships`](partnerships/include/diplomacy/partnerships/partnerships.hpp) | Members, goals, commitments screened by declared criteria (SMART by default), their state at a date as amended or transferred, contributions; a ledger of values by party, dimension and scale; comparison as scales permit, the undominated alternatives, aggregation only by a declared rule; each party's benefit as a dual verdict, mutual benefit party by party; the modes of failure | draft: 52 checks |
| [`rules`](rules/include/diplomacy/rules/rules.hpp) | A body's decision rule as its instrument states it: thresholds as exact fractions or numbers of members or weights, blocking minorities, concurrences, consensus as no formal objection among those present, quorums, fallbacks; its open terms with readings and sources, read by tekmerion's construal; a question's outcome on a record of acts, the members not recorded kept apart; the tally in words, the members decisive ex post, a recorded outcome the rule does not carry; the simple game the rule makes (ADR 0134) | draft: 28 checks |
| [`negotiation`](negotiation/include/diplomacy/negotiation/negotiation.hpp) | Issues with options in brackets, packages, values and reservations as intervals, ratifiers at home deciding by a `rules` rule (Putnam's win-sets); the zone of agreement clear, arguable or untenable package by package; drafts in order, what each settles and reopens and when the zone empties; linkage; value claimed by nomos's bargaining; how a change of a ratification rule moves the zone (ADR 0134) | draft: 17 checks |
| [`events`](events/include/diplomacy/events/events.hpp) | Events in a declared vocabulary (PLOVER's by default) as the reports they rest on, each origin counted once by tekmerion's dependence, with what counting every report would give; coincident records reported and never merged; a pair's series by class; the signs of thin evidence (ADR 0134) | draft: 12 checks |
| [`mandates`](mandates/include/diplomacy/mandates/mandates.hpp) | Mandates and their lifecycles: state at a date (pending, in force, expired, renewed, superseded, consolidated, terminated); renewals and amendments as an acyclic relation and a mandate's lineage; reviews due and overdue; overlaps; each addressee's load against what is resourced; creep; authority read against an undertaking (ADR 0134) | draft: 20 checks |
| [`reviews`](reviews/include/diplomacy/reviews/reviews.hpp) | Review cycles: recommendations, responses (supported, noted, rejected), implementation claimed by the party reviewed, evidenced by another or contested; repeats across cycles; measures by response and state; the signs of a loop not closing, and pairs that recommend each other only mildly (ADR 0134) | draft: 12 checks |
| [`institutions`](institutions/include/diplomacy/institutions/institutions.hpp) | Memberships at a date as a formal context: the lattice of groups and the canonical basis of regularities of membership (morphe's concepts); a regime complex on an issue (nested, overlapping, apart) and the parties that can choose their forum; declared design; reciprocity specific and diffuse over windows (ADR 0134) | draft: 13 checks |

## 3. Checks

`node domains/diplomacy/tests/run-tests.mjs` builds the C++ and runs its eight check programs (183 checks). They are held to answers known by construction, in closed form or by another route:

- each diagnostic raised on an invented case built to raise it, and none on a case built to raise none;
- the participants and ties named in succession held to removal on 300 random undertakings, with ties ended and recorded absent among them;
- the refusal to weigh an ordinal dimension held to kybernesis's test of meaningfulness;
- Fréchet's bounds on mutual benefit held to three joint laws with the same margins, which reach both ends;
- the Council's Shapley–Shubik indices under Article 27(3) exactly 421/2145 and 4/2145, a weighted rule's power equal to nomos's weighted game, and monotonicity on 400 random records;
- linkage creating a zone at different rates and none on 200 random negotiations at one rate; a tighter ratification rule never enlarging a win-set on 100 random negotiations; the Nash point moving with a smaller win-set;
- the force of reports relaying one origin rising toward the origin's and never passing it;
- the lattice of memberships equal to the closed sets found by brute force on 100 random landscapes, its basis sound and complete; the group's imbalance never above the pairs' on 200 random sets of transfers.

## 4. Dependencies

The domain is a bridge over law and policy (module.json). Its modules stand on the foundations alone:

- kybernesis (`kernel`: dual verdicts, scales);
- tekmerion (`construal`: a rule's open terms read under every reading; `evidence`: reports that share an origin);
- archeion (`retention`: calendar dates);
- nomos (`measurement`: concentration; `games`: simple games and bargaining);
- tyche (`probability`);
- apodeixis (`kernel`: verdicts);
- morphe (`combinatorics`: blocks of a multigraph, acyclic relations, formal concepts);
- calx (`verification`, `random`);
- physis, as the siblings' order requires.

Its work is Multilateral Phronimos (`applications/multilateral-phronimos`).

## 5. Theory

[theory/undertakings-and-partnerships.md](theory/undertakings-and-partnerships.md) states and proves what the modules rely on.

## 6. References

- Huang, W. (2026), *Future Talent Strategy: The Possibility of the Human–AI Multilateral Phronimos*, Knowledge Commons, DOI [10.17613/s9xqh-xm397](https://doi.org/10.17613/s9xqh-xm397).
- Hopcroft, J. and Tarjan, R. (1973), Algorithm 447: efficient algorithms for graph manipulation, *Communications of the ACM* 16(6).
- Fréchet, M. (1935), Généralisations du théorème des probabilités totales, *Fundamenta Mathematicae* 25.
- Stevens, S. S. (1946), On the theory of scales of measurement, *Science* 103.
- UN DESA, SDG Actions Platform: About (the SMART criteria), https://sdgs.un.org/partnerships/about.
- Pattberg, P. and Widerberg, O. (2016), Transnational multistakeholder partnerships for sustainable development: conditions for success, *Ambio* 45.
- Huxham, C. and Vangen, S. (2005), *Managing to Collaborate*, Routledge.
- Putnam, R. D. (1988), Diplomacy and domestic politics: the logic of two-level games, *International Organization* 42.
- Felsenthal, D. S. and Machover, M. (1998), *The Measurement of Voting Power*, Edward Elgar.
- Halterman, A., Schrodt, P. A., Beger, A., Bagozzi, B. E. and Scarborough, G. I. (2023), on POLECAT and the PLOVER ontology; the PLOVER specification (CC BY-SA 4.0).
- United Nations (2025), *Mandate Implementation Review: Report of the Secretary-General*, A/80/318.
- Keohane, R. O. (1986), Reciprocity in international relations, *International Organization* 40.
- Raustiala, K. and Victor, D. G. (2004), The regime complex for plant genetic resources, *International Organization* 58.
- Stibbe, D. and Prescott, D. (2020), *The SDG Partnership Guidebook*, The Partnering Initiative and UN DESA.
- Ruggie, J. G. (1992), Multilateralism: the anatomy of an institution, *International Organization* 46(3).

The survey behind the domain is [Multilateral practice, diplomacy and partnerships](../../research/literature/reports/Multilateral%20practice,%20diplomacy%20and%20partnerships.md).

## 7. Open Questions

- Relations among more than two parties (a statement on behalf of a group, a joint sponsorship) are not yet held: ties are binary. A hypergraph, or ties to a declared group, would hold them.
- Succession reads the ties of authority as undirected. Whether an undertaking can continue without a participant may depend on the direction of authority (dominators of a directed graph), which morphe does not yet hold.
- A negotiation's values add across issues (separability). Issues that interact (a share worth more with a mechanism of verification) would need values of combinations, or a model of them.
- A rule's open terms are read independently of one another; readings that depend on one another are not yet held.
- An event's channels are declared by the caller. Estimating them from a record of corrections and retractions is left open, as is a chain of copying among sources (one outlet copying another, not the wire).
- Reciprocity is read on flows of one unit; flows of different kinds (money, expertise, votes) are not yet compared.
- The modes of failure are signs read from a record by declared thresholds, not findings. Whether they predict failure is an empirical question for a comparison of cases (zetesis's QCA on Pattberg and Widerberg's conditions).
