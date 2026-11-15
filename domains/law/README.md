# Law

*Status: draft (2026-11-14; [ADR 0113](../../development/decisions/0113-archeion-and-the-law-domain.md); precedent and proceedings 2026-11-15, [ADR 0115](../../development/decisions/0115-precedent-proceedings-and-the-archives-continued.md)).* The domain of legal systems: their facts, their sources, their rules and their reasoning. It is a field at height 0, standing on the foundations alone (apodeixis for logic and interpretation, tekmerion for evidence, archeion for archives, kategoria for its schema), C++ from the start. Its first modules are an archive of facts and what stands on it; the work that will use them is to be discussed.

## 1. Purpose

Law reasons from facts under norms, and both need care before any reasoning starts.

**"Fact" is said of many things.** A finding of a court, a common understanding, what happened on a day, a custom among States, a treaty in force are all called facts, and a flat list of them mixes distinctions that do not depend on one another. Here a fact has a kind on each of five axes: its register and kind; its standing in a proceeding; its role and scope in reasoning; its logical form; and the traces by which it is preserved. The theory note, [theory/facts-and-their-registers.md](theory/facts-and-their-registers.md), states them.

**What happened is left open.** After the three registers of Wanhong Huang's Generative Relational framework, the ontological register (what occurred) is suspended: an occurrence is referred to and never established. What a court finds is a relational fact: it emerges through a procedure, persists as res judicata, and binds. What an archive keeps is manifestational: traces, which the foundation archeion holds, and no trace identifies what it is a trace of.

**Whether a consequence follows can turn on a reading.** A rule applied to facts is a case for apodeixis: each consequence is clear, arguable or untenable over the admissible readings of the rule's open terms.

## 2. Modules

| Module | Holds | Status |
|---|---|---|
| `facts` | The kinds of fact in their order and registers; standings and the thirteen steps between them; roles, scope and logical form; traces; a record of facts and its faults; the record as a set-valued functor on the schema, for kategoria | draft: 32 checks |
| `proof` | Burdens, and standards of proof declared as a probability to exceed, a standing an argument must reach, or both; a fact not proved taken against the party that bore the burden | draft: 9 checks |
| `sources` | Article 38's sources and a domestic ranking; whom a treaty binds and when; which source prevails, by maxims in a declared order; custom by its two elements under a declared scheme, with persistent objectors | draft: 19 checks |
| `norms` | Rules with elements and consequences, exceptions, open terms and counts-as links, applied to a record of facts as a case for apodeixis; rules of ranked sources preferred | draft: 10 checks |
| `exhibits` | Exhibits as holdings with custody, authenticated by archeion and offered into a chart of evidence of tekmerion; an exhibit that fails is undercut by the archive's finding | draft: 9 checks |
| `precedent` | Cases as constraints on later cases by Horty's reason model: factors favouring two sides, a case's reason; a case base's consistency, with the pair that breaks it; the decisions a situation permits on a reason, the side it forces a fortiori, and the precedents distinguished; the result model; a written form | draft: 14 checks |
| `proceedings` | A docket kept as archeion's sealed journal; its steps held to declared rules of procedure (the kinds before, the roles that may take a step, once only, a period in days); what may be taken next, what is due and overdue; the standing of facts moved by the steps, a finding standing as a relational fact | draft: 14 checks |
| `issues` | Pleadings and the issues they join; decisions and their finality | planned |
| `drafting` | Legislative facts and the making of a rule: evidence for policy, on tekmerion | planned |

## 3. The Schema, in Categories

[schema/facts.cat](schema/facts.cat) states the taxonomy as categories, in kategoria's language, and kategoria answers each of its 31 statements as the script expects: the registers and the kinds are orders ending in one object; every kind lies in a register, functorially; standing is not an order (a fact found on an admission and one found on proof came to be found differently), and forgetting the ways leaves one; custom is the product of a practice and its acceptance; a record of facts is a set-valued functor on the category Record, and a record that keeps a state of mind as a trace, or infers across proceedings, is refused. When the schema changes, records move with it: a later version adds a proceeding's steps and findings, the earlier schema sits within it fully and faithfully, an earlier record is pushed forward by the left Kan extension (with no step or finding, since nothing in it says there were any), and a later record is pulled back to what earlier readers can read. The diagrams are in [schema/figures](schema/figures/README.md), made by `node applications/category-studio/tools/draw-schema.mjs domains/law/schema/facts.cat domains/law/schema/figures`.

## 4. Conventions

- A fact's kind is one of `facts::kinds()`, not a kind of trace; a trace's kind is a kind of `Trace`.
- An occurrence is never established: what is admitted, proved or found is a relational fact `about` it.
- A relational fact that stands states what it emerged from, since when it persists and what it constrains.
- Standards of proof, the shares that make a practice "general", retention periods and the order of the maxims lex superior, specialis and posterior are declared by the caller. None is any jurisdiction's until a scheme states it.
- Dates are ISO 8601, which sort as text.

## 5. Dependencies

apodeixis (`kernel`, `argumentation`, `interpretation`), tekmerion (`evidence`), archeion (`custody`, `fixity`, `retention`'s dates), kategoria (`script`, in the checks), morphe (`combinatorics`) and calx (`verification`). No domain.

## 6. Code and Checks

Header-only C++20, namespaces `law::<module>`. `node domains/law/tests/run-tests.mjs` builds and runs 107 checks: the order of kinds and the steps of standing; a record of a sale well formed, and each of its rules broken one at a time; kategoria's answers to the schema, the C++'s order of kinds held to the schema's pair by pair (225 pairs), and a record verified as a functor; standards met and not; a treaty's parties, the maxims in two orders, a custom, a usage and a persistent objector with their measures; a bylaw applied to a bicycle and to a car, an exception, a statute over a bylaw; a deed authenticated, its custody broken and the sale no longer established, a second exhibit carrying it again; a case base of trade secrets, a situation forced a fortiori, a precedent distinguished, a decision forbidden on one reason and permitted on another, an inconsistent base with its pair, and the closed forms held to the definitions on 400 random case bases with every pair of reasons enumerated; a docket of eight steps kept, what is due and overdue, seven kinds of fault each at its place, the seal broken, and the record after the steps.

## 7. Open Questions

1. **A written form of a record of facts,** and of norms, as lines, for a work to read.
2. **Evaluative facts.** They are flagged; their treatment as open terms of the rule they belong to is the natural next step.
3. **Standing by argument and by probability together.** `proof` takes both measures as given; which a tradition means is the scheme's to say.
4. **Issues and drafting** are planned (§2); precedent by dimensions (magnitudes rather than factors, after Horty 2019) and the hierarchy of courts are not yet written.
5. **The profession's formats:** Akoma Ntoso and LegalRuleML for norms, and the archive's (archeion's open questions).

## 8. References

- Statute of the International Court of Justice (1945), Article 38. Vienna Convention on the Law of Treaties (1969), Article 34.
- International Law Commission (2018), *Draft conclusions on identification of customary international law*.
- Hart, H. L. A. (1961), *The Concept of Law*. Searle, J. R. (1995), *The Construction of Social Reality*.
- Davis, K. C. (1942), An approach to problems of evidence in the administrative process, *Harvard Law Review* 55.
- Wigmore, J. H. (1913), *The Principles of Judicial Proof*. Anderson, T., Schum, D. and Twining, W. (2005), *Analysis of Evidence*.
- Gordon, T. F., Prakken, H. and Walton, D. (2007), The Carneades model of argument and burden of proof, *Artificial Intelligence* 171.
- Spivak, D. I. (2012), Functorial data migration, *Information and Computation* 217.
- Horty, J. F. (2011), Rules and reasons in the theory of precedent, *Legal Theory* 17. Horty, J. F. and Bench-Capon, T. J. M. (2012), A factor-based definition of precedential constraint, *Artificial Intelligence and Law* 20. Aleven, V. (2003), Using background knowledge in case-based legal reasoning, *Artificial Intelligence* 150 (the factors of trade secrets, after CATO).
- Huang, W., *Rethinking Relation in a Generative Relational Framework*; *A Preliminary Taxonomy of Preservation in Generative Relational Theory* (read here from the author's summary).
