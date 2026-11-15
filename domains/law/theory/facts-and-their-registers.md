# Facts and their Registers

*Status: draft (2026-11-14). The domain's account of facts in law: the axes on which a fact has a kind, the rules a record of facts keeps, and what is computed from it. §1 follows the three registers of Wanhong Huang's Generative Relational framework as the author summarised them to this project; it has not been checked against the papers, and says where it adapts.*

## 1. Three Registers

The framework divides reality into three analytical registers. The *ontological* register concerns what obtains, and its constitution is left open. The *relational* register holds what emerges from couplings within a system; a configuration belongs to it when it emerges through ongoing relational organisation, persists enough to take part in the system's course, and has causal efficacy on it. The *manifestational* register is how relational structures become available to observational, epistemic or symbolic systems; two phenomena may share manifestations and differ in what they manifest.

**Reading 1.1 (law).** The domain takes each register as follows.

1. *Ontological: the occurrence.* What took place on a day is referred to, so that facts can be said to be about it, and is never established. No standing beyond "alleged" may be given to an occurrence (a rule of `faults`).
2. *Relational: what stands.* A state of mind as the law attributes it, a social fact, an institutional fact (a marriage, a company, ownership), a norm in force, a custom, a convention. A finding of fact belongs here: it emerges through a procedure, persists as res judicata, and binds the parties. A relational fact that stands in a record states the three criteria: what it emerged from, since when it persists, what it constrains.
3. *Manifestational: the trace.* A document, a recording, an object, testimony, data: what an archive holds (archeion). A trace is of a fact; one holding may be a trace of several facts, and a fact has many traces. Neither identifies the other.

*Adaptation.* The framework's registers are lenses on any system; the domain fixes one use of them, for a record of facts in a proceeding. It reads "left open" strictly: the record has no way to assert an occurrence. Whether a finding is true of what occurred is a question the record cannot state, and the domain does not pretend to answer it.

*Remark (why this is not scepticism).* A court does decide what happened. Read through the registers, what it produces is a relational fact with real effect, reached from manifestations by inference under rules of proof. The reading says what kind of thing a finding is, and so what can be checked about it: its emergence (the procedure), its persistence (finality), its efficacy (what it binds), and its traces.

## 2. The Axes

**Definition 2.1.** A *fact* in a record has:

1. a *kind*, one of the order Kinds (schema/facts.cat): Occurrence, or a relational kind (Mental; Social, Institutional, Normative, Custom, Convention);
2. a *standing*: alleged, admitted, noticed, presumed, deemed, proved, not proved, rebutted, found, final;
3. a *role*, evidentiary (what a trace shows), intermediate or ultimate (an element a rule requires), and a *scope*, adjudicative or legislative (Davis 1942);
4. a *form*: positive or negative; actual, counterfactual or dispositional; past, present or future; particular, general or statistical; and whether it is evaluative;
5. *traces*, each a holding of an archive with a kind of Trace.

*Remark (the kinds a flat list leaves out).* Mental facts (never observed, always inferred); institutional facts as distinct from social ones (they exist by a constitutive rule: Searle 1995); admitted, noticed, presumed and deemed facts (facts by standing, not by kind: a fiction is true by rule whatever happened); ultimate facts (a rule's elements) as distinct from evidentiary ones (Wigmore's probandum and probans); legislative facts (general facts that inform the making and reading of a rule, which evidence-based policy supplies); negative, counterfactual, dispositional, predictive and statistical facts (forms that cannot be observed and are reached by inference); evaluative facts (negligence, good faith: fact and norm together); facts about norms (that a rule is in force; custom, which is two facts, a practice and its acceptance; foreign law proved as fact); and procedural facts (that a document was served or sworn), which are facts about the record itself.

**Definition 2.2 (standing as a category).** The standings and the thirteen steps between them generate a free category, Standing. It is not thin: from Alleged to Found there are five arrows, by admission, notice, presumption, fiction and proof. Identifying them gives the order Reached, and the functor Standing → Reached is full and not faithful.

*Remark.* The non-thinness is the point: how a fact came to be found is part of what it is in the record. A fact found on an admission binds differently from one found on contested proof, and an appeal may reach one and not the other.

## 3. The Rules of a Record

A record is well formed (`faults` finds none) when:

1. ids are distinct and every reference resolves;
2. an occurrence is only alleged (Reading 1.1.1);
3. a relational fact that stands states its emergence, its persistence and its efficacy (Reading 1.1.2);
4. a presumed fact names a basic fact of the record that itself stands; a deemed fact names its rule; a noticed fact says why it needs no proof; a found or final fact names its decision;
5. an evidentiary fact is observable (not mental, negative, counterfactual, dispositional, future or general), and where it stands by proof it has a trace;
6. an unobservable fact that stands by proof has an inference leading to it;
7. inferences rest on generalisations, run from the evidence toward the ultimate facts, and have no cycle;
8. a legislative fact is general or statistical;
9. a trace has a kind of Trace and names a holding.

*Caveat.* These are the domain's conventions for a record, not rules of any jurisdiction's procedure. Rules 5 and 6 encode one claim, that some forms of fact are never shown by a trace; a tradition that treats, say, a confession as showing a state of mind directly would relax them.

**Proposition 3.1 (the record as a functor).** A record written by `as_sets` is a functor from the schema's category Record to finite sets exactly when every trace's kind lies in the manifestational register, every inference joins facts of one proceeding and every presumption joins facts of one proceeding.

*Proof.* The functor laws for a free category with equations reduce to the equations; Record has three, which say those three things. ∎ kategoria verifies this for records of the domain, by code that shares none of `faults`.

## 4. Proof, Sources and Norms

**Proof.** A standard is a declared test in two measures tekmerion supplies: a probability to be exceeded and a standing the fact's argument must reach. A fact that fails the test is not proved, and the point goes against the party that bore the burden. A measure the standard asks for and the assessment lacks leaves the verdict undecided and the fact alleged.

**Sources.** A treaty binds its parties while in force. Between two sources, the first maxim in the declared order that distinguishes them decides. A custom exists when both elements meet the declared scheme: generality (the share of the States concerned, less the persistent objectors, that have practised), consistency (the share of instances that conform), and acceptance (the share of practising States whose practice is accompanied by evidence of acceptance as law). A State all of whose instances before the rule formed are objections is a persistent objector, and the custom does not bind it. The law states no shares; the measures are given beside every verdict so that another scheme can be applied.

**Norms.** `subsume` builds, from a record and a set of rules, the case that apodeixis decides. Established facts enter as axioms (final, found, admitted, noticed, deemed) or as ordinary premises open to attack (proved, presumed); a rule is a defeasible rule, an exception an undercutter of it; a counts-as link is a strict rule conditioned on a reading of an open term; rules of ranked sources are preferred by the maxims. Each consequence is then clear, arguable or untenable over the admissible readings, with its label (apodeixis's interpretation note).

**Exhibits.** An exhibit is a trace with custody. archeion says whether it is authentic; if not, the finding enters the chart of evidence as an item from the archive that undercuts the exhibit. Authenticity is of the manifestation; what it shows is the chart's question; what occurred stays open.

## 5. Precedent

Let F be a finite set of factors, each favouring one of two sides s and s̄; for a situation X ⊆ F, X^s are its factors that favour s. A precedent c = (X_c, s_c, R_c) was decided for s_c on a reason R_c ⊆ X_c^{s_c} (the result model takes R_c = X_c^{s_c}). For reasons U for s̄ and V for s, c decided for s gives U <_c V iff U ⊆ X_c^{s̄} and R_c ⊆ V: the reason outweighs what the case had against it, and so anything weaker, and anything containing the reason outweighs it as well. Γ is inconsistent iff U <_Γ V and V <_Γ U for some U, V (Horty and Bench-Capon 2012).

**Proposition 1.** Γ is inconsistent iff there are c for s and d for s̄ with R_c ⊆ X_d^s and R_d ⊆ X_c^{s̄}.

*Proof.* If U <_c V and V <_d U then R_c ⊆ V ⊆ X_d^s and R_d ⊆ U ⊆ X_c^{s̄}. Conversely take V = R_c and U = R_d: then U = R_d ⊆ X_c^{s̄} and R_c ⊆ V give U <_c V, and V = R_c ⊆ X_d^s and R_d ⊆ U give V <_d U. Two cases for the same side induce relations between the same pairs in the same direction, so they cannot conflict. ∎

**Proposition 2.** For a consistent Γ, deciding X for s on R ⊆ X^s keeps it consistent iff no d for s̄ has R ⊆ X_d^s and R_d ⊆ X^{s̄}. Deciding X for s̄ is possible on some reason iff it is possible on X^{s̄}. So s is forced iff some d for s has R_d ⊆ X^s and X^{s̄} ⊆ X_d^{s̄}: the a fortiori constraint.

*Proof.* The first part is Proposition 1 for the new case against each precedent of the other side. For the second, the condition that forbids s̄ on R, namely R ⊆ X_d^{s̄} with R_d ⊆ X^s for some d for s, weakens as R grows, so if any reason is permitted the largest, X^{s̄}, is. ∎ The checks hold both propositions to the definition on 400 random case bases, every pair of reasons enumerated; 119 were inconsistent, and 166 situations forced.

*Remark.* When nothing forces the decision, a court is free, and the precedents whose reasons are present are *distinguished* by the factors against them that the situation has and they lacked. Freedom here is freedom left by the precedents, not by the law: whether a new factor should count is an interpretation, which apodeixis's arguments about readings can carry.

## 6. Proceedings

A docket is a journal sealed entry to entry (archeion), so the steps of a proceeding are kept as an archive keeps a record: changes found at their place, a cut end found against a seal kept elsewhere. Rules of procedure are declared: the kinds before a step, the roles that may take it, once only or not, a period in days from an earlier kind, and the standing it gives the facts it concerns. A finding moves facts by the steps of `can_become` and no other: an allegation is admitted or proved before it is found. The fact found then has the three criteria of a relational fact. It emerged from the proceeding by that step, it persists from its date, and what it constrains is the decision that rests on it.

## 7. Records between Versions of the Schema

A record is a set-valued functor on the schema's category Record. A later version of the schema, RecordSteps, adds what proceedings make: Step, with during : Step → Proceeding, and Finding, with taken : Finding → Step and finds : Finding → Fact, under the equation within ∘ finds = during ∘ taken (a finding is made by a step of the fact's own proceeding). The inclusion Grow : Record → RecordSteps is full and faithful, as kategoria finds. Records move along it as categorical data migration does (Spivak 2012, Functorial data migration, Information and Computation 217):

- the pullback Δ_Grow takes a record of the later schema to the earlier, its steps and findings set aside and nothing else lost;
- the left Kan extension Σ_Grow takes a record of the earlier schema to the later, with no step and no finding. No object of Record maps into Step or Finding, so the colimits that compute them are empty: nothing in an earlier record says there were steps.

The checks answer both, the left Kan extension held universal against the representables and a constant. That a later record pulled back and pushed forward again loses its findings is the counit of the adjunction Σ ⊣ Δ, and it is the honest answer: the earlier schema has nowhere to keep them.

## 8. References

Those of the README.
