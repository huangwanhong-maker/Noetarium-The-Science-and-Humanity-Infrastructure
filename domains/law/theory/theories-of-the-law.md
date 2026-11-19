# Theories of the Law, Searched For, Judged by Fit and Justification and Tested

*Status: draft (2026-11-19; [ADR 0133](../../../development/decisions/0133-theories-of-the-law.md)). The law domain's module `doctrine`, on its modules `norms`, `precedent` and `authority`, apodeixis's interpretation, heuresis's `evolution` and archeion's journal.* This note is for **learning** and for **use**: it says what a doctrine is here, how it is judged, what the search for doctrines does and does not establish, and what is exact.

## 1. The Problem

Theories of the case ask what happened ([theories-of-the-case.md](theories-of-the-case.md)). A court must also decide how the law is to be read, and two difficulties make that a search, not a lookup.

1. **Open terms.** A rule's words admit several readings ("vehicle", broad or narrow), and the readings a court may take are bounded by the holdings that bind it and the arguments before it ([schemes-authority-and-the-plan-of-proof.md](schemes-authority-and-the-plan-of-proof.md)). Which admissible reading is best is not settled by admissibility.
2. **Unstated reasons.** A precedent decided on a fact (whether there was an emergency) records the factors present and the outcome, and often not which factors decided it. How it constrains a later case depends on the reason it is read as decided on (Horty 2011; Horty and Bench-Capon 2012), and a reading of the reasons of several precedents may make them inconsistent.

Dworkin (1986, ch. 7) asks of an interpretation of the law that it *fit* the record it must explain and, among those that fit, that it *justify* it best. Both are made computable here from what a scheme and a matter declare: the decided cases with their courts and what they held, the precedents with their factors, and the purposes the matter states with the worth of each reading. Nothing is weighed that is not declared, and every weight is the matter's.

## 2. Definitions

**2.1 Problem.** A problem is $(M, Q, \mathcal{D}, \mathcal{B}, P, W, \tau)$:
- $M$, the matter as apodeixis reads it: rules, open points $\pi_1, \dots, \pi_n$ with readings $R_k$, the record of facts, the arguments and binding holdings, and so the admissible interpretations $\mathcal{A} \subseteq \prod_k R_k$;
- $Q$, its questions;
- $\mathcal{D}$, decided cases $d = (X_d, q_d, h_d, c_d, t_d)$: facts of the vocabulary, a question, whether it was held to follow, a court and a date;
- $\mathcal{B}$, case bases $B = (f_B, s, \bar s, F^s, F^{\bar s}, \Gamma_B, X_B)$: the fact it decides, its two sides (the first that the fact holds), the factors favouring each, its precedents $\Gamma_B$ (each with facts $X_c$, outcome $o_c$ and perhaps a stated reason), and the matter's own factors $X_B$;
- $P$, purposes $p$ with weights $w_p \ge 0$ and worths $v_p(\pi_k, r) \in [0,1]$;
- $W = (W_a, W_c, W_h, W_j, \omega)$, declared weights, $\omega$ the weight of a persuasive court; $\tau \ge 0$, the tolerance.

A decision (case or precedent) dated after the matter, or listed by the matter, is *held back*: removed from $\mathcal{D}$ and from each $\Gamma_B$ for everything below except §5.

**2.2 Doctrine.** A doctrine is $\delta = (\iota, \rho)$ with $\iota \in \prod_k R_k$ a reading of every open point and $\rho$ a ratio for every precedent $c$ whose reason is unstated: a non-empty $\rho_c \subseteq X_c^{o_c}$, the factors it had for its outcome. Stated reasons are kept. The doctrines are finite in number:
$$N = \prod_k |R_k| \cdot \prod_{c \text{ open}} \left(2^{|X_c^{o_c}|} - 1\right).$$

**2.3 Fit.** The force of $d$ for the forum is binding when the forum is bound by $d$'s court's order and the court has authority over $d$'s instrument, persuasive otherwise; its weight $\varphi_d$ is 1 or $\omega$ (0 when held back). Under $\iota$, $q_d$ follows from $X_d$ or not (apodeixis's entailment on $M$'s rules), so
$$\mathrm{misfit}(\iota) = \sum_{d \in \mathcal{D}} \varphi_d \, [\, q_d \text{ follows from } X_d \text{ under } \iota \;\ne\; h_d \,].$$
Authority: $\iota \in \mathcal{A}$ or not. Coherence: under $\rho$, the pairs $(c, c')$ of precedents for opposite sides with $\rho_c \subseteq X_{c'}^{o_c}$ and $\rho_{c'} \subseteq X_c^{o_{c'}}$, each holding the other's reason outweighed (Horty's inconsistency); $\mathrm{conf}(\rho)$ is their number.

**2.4 Justification.** $J(\iota) = \sum_p w_p \sum_k v_p(\pi_k, \iota_k) \big/ \sum_p w_p \sum_k \max_r v_p(\pi_k, r) \in [0, 1]$, the declared purposes' share of their best.

**2.5 Score.** $\mathrm{score}(\delta) = \underbrace{W_a [\iota \notin \mathcal{A}] + W_c\,\mathrm{misfit}(\iota) + W_h\,\mathrm{conf}(\rho)}_{\mathrm{unfit}(\delta)} \;-\; \underbrace{W_j\,J(\iota)}_{\mathrm{worth}(\delta)}$, minimised.

**2.6 What a doctrine makes of the matter.** For each base, the precedents read with $\rho$ force a side for $X_B$ when some precedent $c$ for that side has $\rho_c \subseteq X_B^{o_c}$ and $X_B^{\bar o_c} \subseteq X_c^{\bar o_c}$ (Horty's constraint), unless precedents force both, when the fact is left open. The fact $f_B$ is then held, denied or open, and each question $q \in Q$ *follows*, *fails*, or *turns* (follows with the open fact and not without, or the reverse). The tuple of these outcomes, with each base's forced side, is the doctrine's **kind** $\kappa(\delta)$.

**2.7 Standing.** Over a set $\Delta$ of doctrines, the acceptable ones are $\Delta_\tau = \{\delta : \mathrm{score}(\delta) \le \min_\Delta \mathrm{score} + \tau\}$. A question is *clear* when it follows under every acceptable doctrine, *untenable* when it follows under none and turns under none, *arguable* otherwise.

## 3. Results

**Proposition 1 (the field).** Every doctrine lies on the line $\mathrm{worth} = \mathrm{unfit} - \mathrm{score}$; the acceptable ones are exactly those in the half-plane $\mathrm{worth} \ge \mathrm{unfit} - (\min \mathrm{score} + \tau)$.

*Proof.* Immediate from 2.5 and 2.7. ∎

*Remark.* This is why the Law Studio draws the field with misfit to the right and worth upward: Dworkin's two dimensions are kept apart, and the declared weights are what turn them into one order. Changing $W_j$ tilts the line; nothing else moves.

**Proposition 2 (the reach of a ratio).** For a precedent $c$ and ratios $\rho_c \subseteq \rho'_c$, every situation the precedent forces under $\rho'_c$ it forces under $\rho_c$; and every pair inconsistent under $\rho'$ is inconsistent under $\rho$.

*Proof.* Forcing asks $\rho_c \subseteq X^{o_c}$ and a condition on $X^{\bar o_c}$ independent of $\rho$; the first is implied by $\rho'_c \subseteq X^{o_c}$. Inconsistency asks $\rho_c \subseteq X_{c'}^{o_c}$ and the same of $c'$; again implied. ∎

*Remark.* A narrow ratio (one factor) makes a precedent reach further and conflict more; a wide one (every factor it had) reaches least and conflicts least. Neither is right in general: the coherence term and the matter's questions decide between them, and the search keeps both kinds when they differ in what they make of the matter.

**Proposition 3 (standing and tolerance).** As $\tau$ grows, a clear or untenable question can only become arguable, and an arguable one stays arguable.

*Proof.* $\Delta_\tau$ grows with $\tau$; "follows under every" and "under none" are preserved under shrinking and may fail under growth, and "under some and not all" is preserved under growth. ∎

**Proposition 4 (what is held back is not seen).** The score and the kind of a doctrine, and so every choice of the search (its parents, survivors, archive and the credit of its operators), are functions of the problem without its held-back decisions.

*Proof.* Held-back cases weigh 0 in misfit and are removed from each base before coherence and forcing are computed (`detail::as_read`); the test (§5) runs after a generation's survivors are chosen and writes only to the journal. ∎

The checks hold Proposition 4 by changing what the held-back decisions held (and the facts of one) and finding the same archive, generation by generation for twelve generations.

## 4. The Search

The doctrines are made, varied and crossed by heuresis's evolution (its decision 0006), each child assessed (2.5) and given its kind (2.6) as its descriptor.

- **Makers:** the result model with authority's readings; each reading of a term with the narrowest ratios; at random.
- **Mutations:** a term read otherwise; a factor taken into or out of a ratio; a ratio made the case's whole outcome, or one factor of it.
- **Crossovers:** term by term and ratio by ratio; the readings of one with the ratios of another.
- **The archive** keeps, of each kind, the best doctrines (two by default), one within a least distance of a better one of its kind not kept (Mouret and Clune 2015). So the best reading of the law under which the offence follows is sought as actively as the best under which it does not.
- **From outside:** a party, a person or a language model proposes doctrines in the written form; each is read (refused line by line when it does not read, never repaired) and enters under its source. A model has the four roles of the other searches (seed, mutate, cross, reflect), each role's share of a budget its proposals' survival; it is shown the law as the search sees it, never a held-back decision, and its doctrines are grade C.

**Exactness.** When $N$ is small (the checks' and the example's 162), every doctrine is visited apart from the search, and the best of each kind there is is reported beside what the search kept. The checks hold that the search finds, for every kind it keeps, the best of that kind, and the best doctrine there is.

## 5. The Test

Each doctrine kept is tested once on the held-back decisions. The test is registered in archeion's hash-chained journal before they are read, and its outcome written after:
- a held-back decided case is *predicted* when $q_d$ follows from $X_d$ under $\iota$ exactly when the court held it did, *contradicted* otherwise;
- a held-back precedent is *predicted* when the precedents read with $\rho$ force its outcome, *contradicted* when they force the other, *open* when they force neither.

A doctrine is *upheld* when nothing is contradicted. A test done is never done again, and a journal altered is refused before a search goes on.

*Caveat.* A later decision is not ground truth: a court may depart from the best reading of the law as it stood. The test says whether a doctrine anticipated what came, which is evidence of its fit to the law's direction, not proof of its correctness.

## 6. What Is Not Claimed

- **Dworkin's justification is not computed.** What is computed is the declared purposes' worth of the readings, and the worths are the matter's. A purpose left undeclared weighs nothing; the page says so.
- **No weight is any jurisdiction's.** $W$, $\omega$ and $\tau$ are declared by the matter, as burdens and standards are elsewhere in the domain.
- **The doctrines found rank what was found.** A question clear over the acceptable doctrines found may be arguable over doctrines not found. Where $N$ is small every doctrine is visited and this does not arise.
- **Factors and facts are declared.** Whether a situation had a factor is the matter's to say; reading factors from documents is a separate task (tekmerion's recognition).

## 7. References

- Dworkin, R. (1986), *Law's Empire*, ch. 7. Raz, J. (1979), *The Authority of Law*, ch. 10 (on ratio decidendi).
- Horty, J. F. (2011), Rules and reasons in the theory of precedent, *Legal Theory* 17. Horty, J. F. and Bench-Capon, T. J. M. (2012), A factor-based definition of precedential constraint, *Artificial Intelligence and Law* 20.
- Bench-Capon, T. and Sartor, G. (2003), A model of legal reasoning with cases incorporating theories and values, *Artificial Intelligence* 150 (theories of a case law constructed and compared by explanatory power and values).
- Mouret, J.-B. and Clune, J. (2015), Illuminating search spaces by mapping elites, arXiv:1504.04909.
