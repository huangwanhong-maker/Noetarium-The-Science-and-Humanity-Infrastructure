# Schemes, Interpretive Authority and the Plan of Proof

*Status: draft (2026-11-15; [ADR 0121](../../../development/decisions/0121-the-law-studio.md)).* This note states what the modules `schemes`, `authority` and `issues` compute, and proves what they rely on. The facts they work on are those of [facts-and-their-registers.md](facts-and-their-registers.md); the logic is apodeixis's, whose notes on [interpretation](../../../foundations/apodeixis/docs/interpretation.md) define $\Gamma \vdash_\iota \varphi$, $D(\varphi)$ and labels.

## 1. A Body of Law Stated Clause by Clause

### 1.1 Definition

A **scheme** is a tuple $S = (I, P, R, X, T, K, V, C, H)$:

- $I$, instruments, each with its provisions $P_i \subseteq P$. A provision has an identifier, a label, its words and at most one parent in $P$.
- $R$, rules $r : e_1, \dots, e_n \Rightarrow c$; $X$, exceptions $x : e_1, \dots, e_n \Rightarrow \neg\,\mathrm{applies}(r)$; $K$, counts-as links $k : f, [t = \rho] \to a$ (the module `norms`).
- $T$, open terms, each with at least two readings.
- $V$, a vocabulary: atoms declared as facts or as consequences.
- $C$, courts, each in a legal order, with a level and the instruments it interprets with authority; $H$, holdings, each of a court, reading a term $t$ as $\rho$, with the passage that says so.
- A citation map $\kappa : R \cup X \cup K \to \mathcal{P}(P)$, each citation with an optional quotation $q$.

### 1.2 What Is Checked

`schemes::check` reports every violation of the following; a scheme with none is called **whole**.

1. Provision identifiers are distinct, and a parent named exists.
2. $\kappa(y) \neq \emptyset$ for every $y \in R \cup X \cup K$, and $\kappa(y) \subseteq P$.
3. A quotation $q$ of a citation occurs in the words of some $p \in \kappa(y)$, up to runs of whitespace.
4. Every element of a rule or exception is a declared fact, the consequence of a rule or link, or a reading $[t = \rho]$ with $t \in T$ and $\rho$ a reading of $t$. Every consequence of a rule is declared as a consequence.
5. Every link names a term of $T$ and one of its readings.
6. Every holding names a court of $C$, a term of $T$ and one of its readings; where the text of its judgment is given, its passage occurs in it.

*Caveat.* Wholeness is a property of the scheme as a text. It does not say that a rule renders its clause rightly: that is the judgment of whoever wrote the scheme, which condition 3 makes checkable by a reader, clause by clause. A scheme is a reading of the law, dated and cited; it is no authority.

### 1.3 Words Come from the Official Text

A scheme may list a provision by its identifier alone. Its words are then filled from the official text as the data library fetched it (`data/products/legal_texts.py`), together with the provisions it stands under, so that conditions 3 and 6 are checked against the publication and never against memory. The words of an official text are kept in the data cache, not in the repository.

## 2. Whose Reading Binds

### 2.1 Force

A **forum** $F$ declares the set $B_F$ of legal orders whose courts bind it. A holding $h$ of court $c$ on instrument $i$ is, for $F$:

- **binding** when $\mathrm{order}(c) \in B_F$ and $c$ interprets $i$ with authority;
- **persuasive** otherwise;
- **left out** when it was given after the date of the question, or reads a term the case does not turn on.

Nothing here says which orders bind which forum: a matter declares it.

### 2.2 The Order on Kinds

Each holding that counts becomes an interpretive argument of apodeixis for its reading, of a kind of its own. Kinds are ordered ($\prec$, "is below"):

1. persuasive $\prec$ binding;
2. among binding holdings of one order, a lower court's $\prec$ a higher court's;
3. of one court on one term, an earlier holding $\prec$ a later one (a court may depart from itself), unless the caller says otherwise;
4. every canon of the parties (text, system, purpose, history) $\prec$ every binding holding; against persuasive holdings the canons stand below, above or unranked, as declared.

In apodeixis, an argument $a$ for reading $\rho$ of term $t$ attacks an argument $b$ for another reading of $t$ exactly when $\mathrm{kind}(a) \not\prec \mathrm{kind}(b)$, and the readings left at $t$ are those of the arguments accepted under the grounded semantics (all readings, if none is accepted and none has all its arguments defeated).

### 2.3 A Dominant Holding Fixes Its Reading

**Proposition 1.** Let $h$ be an argument for reading $\rho$ of term $t$ such that $\mathrm{kind}(b) \prec \mathrm{kind}(h)$ for every argument $b$ for another reading of $t$, and let no argument undercut $h$. Then $\rho$ is the only reading left at $t$.

*Proof.* An attack on $h$ comes from an argument $b$ for another reading of $t$ with $\mathrm{kind}(b) \not\prec \mathrm{kind}(h)$; by hypothesis there is none, and nothing undercuts $h$. So $h$ is unattacked and belongs to the grounded extension. Since $\prec$ is a strict order, $\mathrm{kind}(h) \not\prec \mathrm{kind}(b)$, so $h$ attacks every such $b$, and each $b$ is out. The accepted arguments at $t$ are therefore all for $\rho$, and the readings left are $\{\rho\}$. $\blacksquare$

*Remark.* A single binding holding of the highest court of an order that binds the forum satisfies the hypothesis against every canon and every persuasive or lower holding. Two holdings of one court on one term satisfy it for the later one. Two binding holdings of courts of different orders, for different readings, satisfy it for neither: both stay, and the term is open. That is the right answer, since the scheme has not said which order prevails.

## 3. The Plan of Proof

### 3.1 Definition

Let $\varphi$ be the question of an issue, $\mathcal{N}$ the norms and $\mathcal{F}$ the record. The facts $\varphi$ **turns on** are the atoms reached from $\varphi$ backwards through the rules that give it, the exceptions to those rules and the counts-as links, that no rule gives. Each is:

- **given**, if the record establishes it;
- **decided against**, if the record has it not proved or rebutted;
- **open**, if the record has it alleged;
- **absent**, if the record does not mention it.

Extend the case's open points (the terms, each with the readings the interpretive arguments leave it) by one point per open fact $f$, with readings *holds* (adding $f$) and *fails* (adding $\neg f$). Let $\Omega$ be the resulting finite space, $A \subseteq \Omega$ the admissible interpretations and $D \subseteq \Omega$ those under which $\varphi$ follows.

A **way to establish** $\varphi$ is a minimal partial interpretation $w$ every completion of which lies in $D \cup (\Omega \setminus A)$, and which has a completion in $D \cap A$. A **way to resist** it is the same with $\Omega \setminus D$ in place of $D$. A way is read as: facts to make out ($f$ = holds), facts to show do not hold ($f$ = fails), readings to argue for.

### 3.2 What the Ways Say

**Proposition 2.** (a) Every admissible completion of a way to establish $\varphi$ makes $\varphi$ follow. (b) Every admissible interpretation under which $\varphi$ follows completes some way to establish it. (c) No step of a way can be dropped without losing (a).

*Proof.* (a) A completion of $w$ lies in $D \cup (\Omega \setminus A)$; if it is admissible it lies in $D$. (b) The label of a set $U \subseteq \Omega$, as apodeixis computes it, is the set of minimal partial interpretations all of whose completions lie in $U$, and every member of $U$ completes one of them (a total interpretation in $U$ is such a partial interpretation, and some minimal one lies beneath it). Take $U = D \cup (\Omega \setminus A)$: an $x \in D \cap A$ completes a member $w$ of the label, and $w$ then has a completion in $D \cap A$, so it is a way. (c) Minimality in the label. $\blacksquare$

**Proposition 3.** If a fact $f$ is to be made out in every way to establish $\varphi$, then $f$ holds under every admissible interpretation under which $\varphi$ follows: $f$ is necessary for $\varphi$ over the open facts. Likewise for facts to be shown not to hold.

*Proof.* By Proposition 2(b) an $x \in D \cap A$ completes a way $w$; $w$ fixes $f$ = holds; so does $x$. $\blacksquare$

### 3.3 Absent Facts

A body of law offers several routes to a consequence (an adequacy decision, safeguards, a derogation), and most of the facts of most routes are never in play in a given matter. A plan over all of them would be large and would say little. So an absent fact is left as the case leaves it, neither holding nor failing, and is listed; alleging it brings it into the plan. `plan` takes an option that opens the absent facts too.

### 3.4 Cost and Refusal

$\Omega$ is enumerated, never sampled: $D$ costs $|\Omega|$ evaluations of the case, and the label at most $\prod_k (r_k + 1) \cdot |\Omega|$ steps for points with $r_k$ readings. A term whose reading is fixed has one reading left and costs nothing. Above a limit on $|\Omega|$ (1024) the plan is refused with the numbers, and says to establish or decide some facts first.

*Rationale.* The limit keeps the answer exact. With the readings the courts have fixed removed, the first privacy matter has two open terms and three alleged facts: 32 points.

## 4. Evidence Put Forward by a Model

A language model that reads a document proposes candidates of evidence. tekmerion's grades apply (its recognition module): what a model says from memory is grade C. Here a model must copy the passage it relies on; the passage is looked for in the document, and a candidate whose passage is not there is refused. A candidate that passes has **status** *proposed* until a person reads it.

**Invariant.** A proposed candidate is read at grade C whatever it cites, and enters no chart of evidence. So it changes the standing of no fact and the answer to no question. Only a person's acceptance makes it a grade-B candidate whose source is the document.

*Rationale.* A quotation that is almost right is the failure that matters most in practice. Finding the passage removes invented quotations; it does not show that the passage means what the model took it to mean, which is why acceptance is a person's.

## 5. Open Questions

1. **Negative elements.** A rule that asks for $\neg f$ is met only by a fact shown not to hold. Negation as failure, where the law means it ("in the absence of a decision"), is modelled by exceptions for now.
2. **Burdens in the plan.** The plan gives the ways; it does not yet split them by who bears the burden of each fact.
3. **Dimensions of precedent** (magnitudes, after Horty 2019) and the binding force of a holding's reasons, as against its reading of a term.
4. **Larger spaces.** Labels computed from the rules' structure (assumption-based truth maintenance proper) instead of by enumeration.

## 6. References

- Hart, H. L. A. (1958), Positivism and the separation of law and morals, *Harvard Law Review* 71.
- MacCormick, D. N. and Summers, R. S., eds (1991), *Interpreting Statutes: A Comparative Study*, ch. 13.
- Sartor, G., Walton, D., Macagno, F. and Rotolo, A. (2014), Argumentation schemes for statutory interpretation: a logical analysis, *JURIX 2014*.
- de Kleer, J. (1986), An assumption-based TMS, *Artificial Intelligence* 28.
- Gordon, T. F., Prakken, H. and Walton, D. (2007), The Carneades model of argument and burden of proof, *Artificial Intelligence* 171.
- Magesh, V. et al. (2025), Hallucination-free? Assessing the reliability of leading AI legal research tools, *Journal of Empirical Legal Studies*.
