# ADR 0130: kybernesis, Steering a System Known through Evidence: the Architecture as Data, Methods Admitted to Its Blocks

- **Status:** Accepted, provisionally
- **Date:** 2026-11-18
- **Decided by:** the user, who proposed the framework from evidence to action and asked, after four rounds of discussion, that it be "a foundation … highly abstract, but define interfaces and there relations", verified by category theory, with "various predefine implementation for each blocks … like law's one combine formal reasoning + statistical methods"; who asked that it be "easily to revise and extend … if formal verifications in category and other constraints aspect pass, we should allow revise our framework architecture", that a method "satisfy the constraints of a block and the relations with other blocks" be admitted through "a wrapper of adapter", and for "a 'computational graph' visualizer"; and who said "I will found 'kybernesis'"; and Claude.

## 1. Context

The idea of 2026-11-18, *From evidence to action* ([development/ideas/2026-11-18-from-evidence-to-action.md](../ideas/2026-11-18-from-evidence-to-action.md)), records a framework for steering a system known only through evidence, and four rounds of discussion that revised it:

- interpretation is kept apart from estimation;
- evaluation is a family of operators on the estimated state, with features, needs, problems, risks and assets as examples;
- three nested loops carry knowledge production, governance and transition;
- no space, time or discipline is presupposed.

Its first schema was verified by kategoria, and seven figures were drawn for publication. The framework names no object in nature, so it belongs among the foundations.

## 2. Decision

1. **A seventeenth foundation, kybernesis** (κυβέρνησις, steering, government), the highest of the foundations, on kategoria, tekmerion, tyche, apodeixis and morphe, and through them peira, nomos, heuresis, kinesis, physis and calx. Its decisions 0001–0003 record its scope and its first modules: `kernel`, `architecture`, `blocks`, `methods`, `loop`.
2. **The architecture is data.** The loop is declared in a script: registers, stages, kinds, types, blocks with ports and laws, boundaries, and wires, some carried to the next turn. A grammar in BNF on morphe reads it, and it is printed back. The code that verifies, admits and runs knows no block by name. A revision is admitted exactly when two things hold:
   - its constraints, among them that a wire going forward within a turn reaches only the next stage or a later stage of its register, so that the record sees the system only through what is observed;
   - its categorical claims, answered by kategoria: thin orders, a terminal kind, each kind in its register functorially, and each block's stage as a functor from the step category to the stages.
3. **Admission is decided.** A method fills a block when, on a finite instance, its ports are the block's and the block's laws hold. kategoria decides the laws in a model in relations of the method's tabulation: total; deterministic; sound for an estimator in a world given as relations; referred for the world's own blocks. Methods are re-admitted after a revision.
4. **Predefined methods,** adapted from the siblings:
   - declared readings, construal and precedents for interpretation;
   - the possibilistic filter and Bayes's rule for estimation, with the face-value estimator kept as a refused counterexample;
   - exceedance under readings for evaluation;
   - a threshold rule and HSVI for policy.

   An invented riverside example fills every block of the standard architecture.
5. **Dual verdicts.** Every conclusion carries a standing and a measure, combined by a declared standard. A proposal is never the record's.
6. **The computational graph** is drawn by the visualization layer's `steering/`. It reads architecture scripts and the foundation's report, and shows the stages as columns, the blocks with their methods and verdicts, and the wires carried to the next turn.
7. **The schema** of the idea moves into the foundation (`schema/framework.cat`, with the kind Construal added), checked with the architecture's kinds held to it.

## 3. Alternatives Considered

- **A domain beside policy.** Rejected: the framework names no object in nature.
- **The architecture as classes.** Rejected: it would make every revision a restructuring.
- **Laws declared by methods.** Rejected: the face-value estimator shows a law claimed is not a law shown.
- **Path properties as functors.** Rejected: a functor into the stages cannot forbid a wire that skips a stage, since the composite exists. That property is a constraint, and the checks show the difference.

## 4. Consequences

- kybernesis: 86 checks in six programs; the schema's 42 statements answered by kategoria. The visualization layer: 33 more checks (789).
- Its repository is `https://github.com/huangwanhong-maker/kybernesis`, attached as the submodule `foundations/kybernesis` on 2026-11-19.
- Planned:
  - policy making as evolution with an honest protocol (on heuresis);
  - transfer between relational systems as a lens;
  - the constitution, with versions, rules of change and records migrated by kategoria;
  - the API's composition and model proposals at the edge;
  - an Urban Studio plug-in on a real case.
