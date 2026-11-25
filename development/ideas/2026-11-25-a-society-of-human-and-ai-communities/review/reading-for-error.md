---
status: seed
---

# An Independent Reading for Error, with What Was Corrected

*This reading was made on 2026-11-25 by a separate session that had not written the design, with no access to the web and a bounded budget; its text follows as that session wrote it, after its first heading. On the same day every finding was answered: findings 1, 2, 3, 4, 7, 8, 9, 11, 13, 14, 15, 16 and 17 and the question by corrections to [the design](../design.md) (each correction says so where it stands, or rewrites the sentence named); findings 5, 10 and 12 by the C++ of the foundation thesmos, which enforces the slice per resource, refuses charges under another's mandate, counts by person and by community, and refuses an amendment that would leave the constitution inadmissible (its decision 0003); finding 6 by the communities domain's jar, which models instruments that issue units every round and provision constrained by behaviour and by verification (the design, Section 6.10). The sketch in Python keeps its first statements; its results stand as written, under the corrected names.*

Independent reading of design.md, kernel.py, checks.py, constitution.py, results.json. Kernel claims were probed with a script outside the folder (scratchpad/review/probe.py); no design file was changed. Every number quoted in design.md was compared with results.json; apart from finding 8, all agree within rounding.

**1. Error. §2.8 Prop. 6, "replaces its voters by any number of voters that vote as it did".** This is false when an origin's voters split, because "vote as it did" is then undefined. Counterexample run on `kernel.tally`: X's operatives vote yes and no, and Y votes yes, giving 3/4. X replaces them with yes, yes, no, giving 5/6. The proof ("weight is one") covers only the unanimous case. `check_counts` tests only origins whose copies all vote True. Missing hypothesis: the origin votes unanimously, or the replacement keeps its yes-fraction. Confidence high.

**2. Gap and inconsistency. Def. 3, "the first party that is not an operative reached along controls".** The definition is undefined in three cases: joint control (two controllers), cycles among operatives, and an operative with no declared controller. The kernel's `origin` docstring says "An operative with no declared controller is its own origin". It returns an operative under cyclic control: a probe gives a→a and b→b for a pair that control each other. Prop. 6's remark ("seven copies count as seven origins") relies on this convention, which contradicts the definition's text. Confidence high.

**3. Gap. Def. 4 and Prop. 3, "a slice: the most that may be charged between two checks".** Allowances are per resource, each in its own unit, and §6.2 and G5 forbid undeclared conversions. Yet the slice is a single number with no unit (`slice: int` in the kernel), so min(B, σ) mixes units. The proof also assumes two things it does not state: (a) one verifier, whereas G4 and §6.2 have every keeper of a resource enforce its own guard, so k keepers expose k·σ; (b) the revocation is visible at the status source the moment it is made, whereas a delay d gives up to (⌈d/window⌉+1)·σ. The claim "turns that impossibility into a bound" (the Diplomacy paper's delayed revocation) needs (b). Confidence high on the units, medium on (a) and (b).

**4. Gap. Prop. 1 and §2.4, "recomputed by anyone who holds the record".** The fold also depends on the declared control relation and on the constitution. The sketch's `replay(parties, rules, journal)` takes both from outside the chronicle, and the founding act carries no constitution, contrary to Def. 6. Example: in the worked record the first `amend` fails at 2/4 by origin. Against a registry in which lab-k's agents have no declared controller it passes at 4/6, so the same journal no longer replays. Unless declarations of control are themselves acts, the state is not a view of the record alone. Admission after a revocation also depends on an external check of status (Prop. 3). Confidence medium-high.

**5. Inconsistency (code against G3 and G4).** `kernel.step` never enforces `slice`: one act charged 500 under a slice of 10 and was admitted. The `charges` loop also never checks that the charged mandate is the act's own, held by its author, or unrevoked. In the probe, a third party, bob, charged 400 against the revoked m1 and was admitted. Prop. 3 is checked only on a separate simulation (`exposure`), and that check asserts `lost <= min(budget, slice)` with the total budget, which is weaker than the stated B, the allowance remaining at revocation. Confidence high.

**6. Overstatement. §0 and §6.8, "an instrument only postpones it".** The instruments modelled are a fixed debit limit and a fixed initial stock of tokens. A party with capacity 1 and need 3 runs a net debit of about 2 a round, so any fixed bound L binds after about L/2 rounds; that is arithmetic, not a finding. An instrument with periodic issuance is not modelled. "Provision" is `exact(inst, None)`, the planner with no constraint, so the weakest third's 0.88 equals the overall optimum by construction. No pool, rule of access or incentive is represented. Confidence high.

**7. Inconsistency: "set-off" in two senses.** In Prop. 5 and §6.4 it means cancelling cycles of existing obligations of one class. In §0, §6.7 and §6.8, "exact set-off, limit zero" and "planned set-off" mean the linear-programme optimum of a balanced exchange (`exact(inst, 0)`), with no prior obligations. That is §6.4's "cycles of performance" solved exactly. Prop. 5's own remark says that cancelling cycles can fall short of the maximum circulation, so 0.874 is credited to an operation that does not produce it. Confidence high.

**8. Inconsistency. §6.8, "after two rounds for a limit of 3 and after five for a limit of 6".** results.json gives the weakest third 0.871, 0.726, 0.385, 0.339 under limit 3 and 0.871, 0.871, 0.864, 0.625, 0.393, 0.343 under limit 6. Limit 6's round 5 (0.393) corresponds to limit 3's round 3 (0.385), so a single way of counting gives 2 and 4 (or 3 and 5), never 2 and 5. Confidence high.

**9. Gap. Prop. 5(4) proof, "only decrease in number, each is cleared ... and the process ends".** A non-increasing count does not prove termination. Each step removes one obligation but may create a new one (i,k). The claim is true: fix a topological order and use Φ = Σ_e M^(n − span(e)) with M > 1. Each step deletes an edge of shorter span and adds at most one of longer span, so Φ strictly decreases over finitely many edge sets. The proof as written does not give this. Confidence high.

**10. Inconsistency (code against the presets).** `tally` treats every unit of count other than "operative" as "origin". The presets declare "person" (commons, cooperative, guild, polity) and "community" (league). In a probe, a count by person over one person voting yes and one operative voting no returned 1/2. Confidence high.

**11. Overstatement. §2.8, "each is checked in the sketch on random instances".** Props. 1, 4 and 6 are checked on fixed instances only: the worked record (`check_record`), three histories and three records (`check_sufficiency`), and 1, 2, 7 or 50 copies (`check_counts`). Confidence high.

**12. Inconsistency. §3.2, "The kernel then checks each amendment for that property".** The sketch's condition 7 checks only that `preserved` is a list (constitution.py l. 121). `kernel` `amend` applies changes by `setattr` without checking any property or level. `check_registers` declares a conversion by mutating `state.rules` directly (checks.py l. 406), outside any act, contrary to G6. Confidence high.

**13. Inconsistency. Def. 1, "a community (Definition 8)".** Definition 8 is commitment. Community has no numbered definition. Confidence high.

**14. Inconsistency. Def. 8, "Its debtor is the party that answers" against "handed over (the debtor's authority ended)".** In the worked record the debtor of c2 is mei, as `check_record` asserts: "the debtor of both is the sponsor". Mei's authority did not end; the operative's allowance did. Confidence medium-high.

**15. Overstatement. §2.9, "left to implementers by each of them, in the words of their own specifications".** A2A was read only in part, and AP2 is known only from excerpts. The same table says AP2 and Verifiable Intent make authority a signed credential that third parties can verify, and that capability tokens give chains that only narrow. §0 repeats the claim. Confidence medium.

**16. Overstatement. §6.5, "the seventh test of Section 6.6 puts it to each service".** Bigoni et al. concern behaviour (free riding). Test 7 is run "as a drill in simulation" with "duties held as they were", which cannot measure that effect. Only the bounded trial could. Confidence medium.

**17. Overstatement. §2.8, "no rule that reads balances alone can return value to whoever provided it".** Prop. 4 yields less: no such rule does so in every history of a fibre in which the providers differ. Confidence medium.

**Question (unchecked).** Def. 11 and Test 1 let a standing gate pass: a threshold on accumulated reputation or on a count of contributions. Yet §6.5, row 6, calls personal accumulation "what a personal gate is". Test 1's second clause names only "a balance", so the verdict may turn on what the quantity is called.

Counts: error 1, gap 4, inconsistency 7, overstatement 5; one question.
