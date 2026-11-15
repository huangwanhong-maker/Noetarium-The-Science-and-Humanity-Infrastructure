# 0115. Precedent and Proceedings in the Law Domain, Timelines of Evidence, and the Archives Continued

- **Status:** Accepted, provisionally (2026-11-15). Continues [0113](0113-archeion-and-the-law-domain.md).
- **Date:** 2026-11-15
- **Decided by:** the user, who asked that the work go on overnight ("the archieve foundation, the facts category, record, and law domains, reasoning foundation, etc. … continue works remain, be high quality, delicate, elegant, extensable, revisable"); and Claude, who chose these as the next modules ADR 0113 had planned.

## 1. Context

ADR 0113 left the law domain's `precedent` and `proceedings` planned, and archeion's packages. A docket is a record whose steps must not change once taken, which is what archeion's journal is for. Precedent is the constraint decided cases put on later ones, the other half of legal reasoning beside the application of rules (`norms`). It is also the half most often modelled with factors since HYPO and CATO.

## 2. Decision

1. **`law::precedent`**, Horty's reason model. Factors favour two declared sides, and a case is decided on a reason. The model gives a case base's consistency, with the pair of cases that breaks it, and a new decision's consistency with the base on a given reason. A situation is forced a fortiori when no reason for the other side keeps the base consistent, and the precedents not forcing it are distinguished by the factors that set them apart. The result model reads a case with no stated reason. A written form is included. The closed forms are proved in the theory note and held to the definitions by enumeration.
2. **`law::proceedings`.** A docket is archeion's journal. Each kind of step is declared with its order, roles, once-only rule, period in days and the standing it gives facts. `review` holds a docket to the rules and gives each fault at its place, `state` says what is open, due and overdue, and `apply` moves the record's facts by `can_become` alone. A finding is given the criteria of a relational fact.
3. **Records between versions of the schema.** `schema/facts.cat` gains a later version of the record with a proceeding's steps and findings, the inclusion of the earlier (full and faithful), and records moved along it by kategoria's left Kan extension and pullback. That is functorial data migration, and the schema's figures gain the later record.
4. **tekmerion's timelines** (its decision 0005): statements of time as a simple temporal network, the conflict named with its sources, the tightest bounds; on morphe's shortest paths of either sign (its decision 0017).
5. **archeion** (its decision 0004):
   - RFC 6962 proofs of consistency, with the caveat that they bind roots, not sizes, found by a check that passed when it should have failed;
   - calendar dates with periods of days;
   - `packages`, BagIt bags made and validated.

## 3. Alternatives Considered

- **Precedent by argumentation.** A case base can be read as an ASPIC+ theory, its preferences the cases' (Prakken, Wyner, Bench-Capon and Atkinson 2015). The reason model is the smaller structure, and its decisions are exact set conditions. The argumentation reading is the natural next step, on apodeixis.
- **Rules of procedure built in.** Rejected: no jurisdiction's rules are the library's. The example scheme in the checks says it is one.

## 4. Consequences

- **Checks:** law 107 (precedent 14, proceedings 14); archeion 116; tekmerion 149 (timelines 11); morphe 812 (four new).
- **Not built:** precedent by dimensions (Horty 2019); the hierarchy of courts and overruling; issues and pleadings; a written form of rules of procedure; the docket in a work.
