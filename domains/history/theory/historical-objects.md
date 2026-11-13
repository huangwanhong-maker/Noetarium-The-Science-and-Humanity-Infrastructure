# Historical Objects and Their Dependence

*Status: draft (2026-11-13, ADR 0110).* What a record of historical objects is, as mathematics: the order of kinds, the record as a functor on a schema, the dependence of objects as an order, and what is computed from it. The code is `objects/` and `dependence/`; the schema, verified by kategoria, is [`schema/historical-objects.cat`](../schema/historical-objects.cat), with its diagrams in [`schema/figures/`](../schema/figures/). How a record is written is the [codebook of objects](../codebook-objects.md).

## 1. Kinds

**Definition 1.1.** The *kinds* are a finite poset $(K, \le)$ with a greatest element $\top$, the historical object. $k \le k'$ is read "$k$ is a kind of $k'$".

As a category, $K$ is thin (at most one arrow between two objects) and $\top$ is terminal. Both are verified for the core's twenty kinds; a presentation by generators needs one equation, for the school, which is a tradition and a collective and must be an object in one way.

**Definition 1.2.** An *extension* of the kinds by a discipline is an order embedding $K \hookrightarrow K'$: as a functor, full and faithful. A discipline may add kinds beneath the core's, and may neither merge two of the core's kinds nor relate two that were unrelated.

*Remark.* The reader enforces 1.2 by construction, since a new kind names only kinds already there and is new. The schema's script checks it for the kinds music and philosophy add.

## 2. The Record as a Functor

**Definition 2.1.** The *schema* is the category $\mathcal{S}$ presented by a generator for each reference of a record (`kind : Object → Kind`, `from, to : Relation → Object`, `type : Relation → RelationType`, `domain, codomain : RelationType → Kind`, `lower, upper : Subkind → Kind`, `from_fits, to_fits : Relation → Subkind`, and those of emergences, premises, statements and assertions) and by the four equations of typing:

$$\mathrm{lower} \circ \mathrm{from\_fits} = \mathrm{kind} \circ \mathrm{from}, \qquad \mathrm{upper} \circ \mathrm{from\_fits} = \mathrm{domain} \circ \mathrm{type},$$

and the same for `to` with `codomain`.

**Definition 2.2.** A *record* is a functor $R : \mathcal{S} \to \mathbf{FinSet}$ in which $R(\mathrm{Subkind}) = \{(k, k') : k \le k'\}$ with `lower` and `upper` its projections.

**Proposition 2.3.** A record in the sense of 2.2 is exactly an assignment of tables and references in which every relation is well typed: the kind of its source is a kind of its type's domain, and the kind of its target a kind of its type's codomain.

*Proof.* Functoriality on generators says every reference lands in its table. The two equations for `from` say that the pair $\mathrm{from\_fits}(r)$ has first member the kind of $r$'s source and second member the domain of $r$'s type; by 2.2 that pair is in the order. Conversely a well-typed relation has exactly one such pair. $\square$

*Remark.* The typing is an inequality and equations state equalities; the table of witnesses is what carries one into the other. The domain writes any record as such a functor (`as_sets`) and kategoria decides whether it is one. An ill-typed relation has no witness and is written with the pair of its own kind with itself, so that the functor fails at an equation of typing and nowhere else (checked).

**What is not equational,** and is checked by the reader alone: that the layering relations and the dependence have no cycle; that what marks an emergence is an event; that a premise begins before what it is a premise of appears; the rule of grades.

## 3. Layers

**Definition 3.1.** A *layering* is the category presented by $\mathrm{member} : T \to C$, $\mathrm{finer} : C \to C$, $\mathrm{part} : T \to T$ with $\mathrm{finer}^2 = \mathrm{finer}$, $\mathrm{part}^2 = \mathrm{part}$ and $\mathrm{finer} \circ \mathrm{member} = \mathrm{member}$.

It has six arrows, and its three equations complete to a confluent rewriting system. Taxonomy (exemplifies, specialises, part of) and chronology (falls, within, episode) are two layerings and are isomorphic: one notion of zoom serves both.

**Proposition 3.2.** In a layering, $\mathrm{member} \circ \mathrm{part} \ne \mathrm{member}$.

*Proof.* Their normal forms differ and the system is confluent (kategoria's answer, `expect fails` in the script). $\square$

So a part of what exemplifies a class is not thereby an example of it: an aria is no opera. The relation $\mathrm{member} \circ \mathrm{part}$ is a sixth, derived one.

## 4. Dependence

**Definition 4.1.** An *emergence* of an object $x$ is a set $P$ of objects, its premises, each with a role in $\{\text{problem}, \text{condition}, \text{source}, \text{against}\}$. The *dependence* of a record at a layer is the relation $p \to x$ for $p$ a premise, in the layer, of an emergence of $x$.

**Proposition 4.2.** The dependence of a record that reads without fault is acyclic, so its reflexive–transitive closure is a partial order $\preceq$, and dating is monotone in this sense: if $p \to x$ and both are dated, the earliest day $p$ may have begun is not after the latest day $x$ may have appeared.

*Proof.* The reader refuses a cycle (morphe's `topological_order`, which names it) and an anachronism. $\square$

*Remark.* With dates as intervals the order of time is an interval order, and 4.2 is the weak statement that holds of it. Where dates are points, dating is a functor from $(O, \preceq)$ to a chain; the schema's script shows it, and shows a dating against time refused.

**Definition 4.3.** For an object $x$ at a layer:
- its *past cone* is $\{y : y \prec x\}$ with the least number of steps to each, and its *future cone* the same upward;
- its *lineage* is the objects reached by one step of exemplifies or specialises and then steps of specialises;
- its *inherited premises* are the past cones of its lineage, marked with the ancestor each comes through.

**Definition 4.4 (the frontier).** At a day $t$, a problem is *posed* if it has appeared by $t$; a problem with no date is posed once the premises of one of its emergences have all appeared, or else when its first answer does. It is *open* if no object that answers it has appeared by $t$.

**Definition 4.5 (support).** Read each emergence as a rule $P \Rightarrow x$, and let the givens be the objects with no emergence in the layer. An object is *derivable* if it lies in the least set containing the givens and closed under the rules. $y$ *rests on* $c$ if $y$ is derivable and is not once $c$ is withheld.

**Proposition 4.6.** $y$ rests on $c$ exactly when every derivation of $y$ uses $c$. In particular an object with two emergences, of which one does not pass through $c$, does not rest on $c$.

*Proof.* A derivation that avoids $c$ survives the withholding; and if $y$ is derivable without $c$, the derivation found avoids $c$. $\square$

*Caveat.* This is a property of the record: its premises are taken as jointly needed and its emergences as the only ways. It is no counterfactual of history, and the work says so where it shows it.

**Definition 4.7 (comparison cases).** $y$ is a *comparison case* of $x$ if both answer a problem (their own or inherited), neither stands under the other, and $y \ne x$. It is *independent* if $x$ and $y$ are incomparable in $\preceq$.

*Remark.* Independence is the record's: no path of dependence is known. Two answers to one problem with no path between them are where judgement is trained, since what they share is the problem and its conditions and not each other.

**Definition 4.8 (gathering).** Given classifying objects $c_1, \dots, c_m$, place each object under the nearest of them in its lineage. The *derived* dependence from $c_i$ to $c_j$ is the number of premises $p \to x$ with $p$ under $c_i$ and $x$ under $c_j$, $i \ne j$. A premise directly between two of the $c$ is *asserted*, and is kept apart.

## 5. Standing

**Definition 5.1.** An assertion has a source, a grade in $\{A, B, C\}$, and may be documentary, confirmed, or a denial. The standing of a statement with assertions $\Sigma$ is

$$\sigma(\Sigma) = \begin{cases} \text{disputed} & \text{some } a \in \Sigma \text{ of grade } A \text{ or } B \text{ denies it}, \\ \text{established} & \text{else, if a confirmed documentary } a \text{ of grade } A \text{ or } B \text{ exists, or the confirmed ones of grade } A, B \text{ have } \ge n \text{ sources}, \\ \text{attested} & \text{else, if some } a \text{ has grade } A \text{ or } B, \\ \text{proposed} & \text{otherwise}, \end{cases}$$

with $n = 2$ unless the record says otherwise.

**Proposition 5.2.** The layers are nested: the established lie in the attested layer, and that in the proposed. Adding an assertion of grade C never changes a standing; adding a denial of grade A or B makes any statement disputed.

*Proof.* By cases of 5.1; grade C enters no clause but the last. $\square$

So nothing a model writes can establish or dispute anything, whatever it writes.

## 6. What Is Not Proved or Not Built

- The map between this record and the codebook of occasions and beings (an emergence as an occasion, an object as a being), under which a record of objects is a history complex.
- Dating as a functor to an interval order, stated properly.
- The agreement of two keepers on roles and types.
