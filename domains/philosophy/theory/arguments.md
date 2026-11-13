# The Theory of the Philosophy Domain: Arguments, their Frameworks and their Maps

*Status: draft (2026-11-13). The results the module `arguments` relies on, with their hypotheses and proofs or citations. §1–§6 concern abstract frameworks (`frameworks.hpp`) and name nothing of philosophy; §7 concerns argument maps (`maps.hpp`). Every framework is finite. Where a result is Dung's (1995) it is cited as his, without a number; the proofs given are for the finite case and are short enough to be stated in full.*

## 1. Argumentation Frameworks

**Definition 1.1 (Dung 1995).** An *argumentation framework* is a pair $(A, R)$ of a finite set $A$ of *arguments* and a relation $R \subseteq A \times A$ of *attack*. Write $a \to b$ for $(a, b) \in R$. For $a \in A$ and $S \subseteq A$ let
$$a^- = \{b : b \to a\}, \qquad S^+ = \{b : a \to b \text{ for some } a \in S\}.$$
A framework is a directed graph in which loops are allowed; what an argument says is no part of it.

**Definition 1.2 (Dung 1995).** Let $S \subseteq A$.
1. $S$ is *conflict-free* when $S \cap S^+ = \emptyset$.
2. $S$ *defends* $a$ (and $a$ is *acceptable with respect to* $S$) when $a^- \subseteq S^+$: every attacker of $a$ is attacked by a member of $S$.
3. The *characteristic function* $F : 2^A \to 2^A$ is $F(S) = \{a \in A : S \text{ defends } a\}$.
4. $S$ is *admissible* when it is conflict-free and $S \subseteq F(S)$.
5. $S$ is a *complete extension* when it is conflict-free and $S = F(S)$.
6. $S$ is a *preferred extension* when it is admissible and maximal for inclusion among the admissible sets.
7. $S$ is a *stable extension* when it is conflict-free and $S \cup S^+ = A$.
8. The *grounded extension* $G$ is the least fixed point of $F$ (which exists: Theorem 3.2).

*Remark.* An argument that attacks itself belongs to no conflict-free set, so to no extension of any kind. An argument that nothing attacks is defended by every set, the empty one included: $F(\emptyset)$ is the set of unattacked arguments.

## 2. The Characteristic Function

**Proposition 2.1.** $a \in F(S)$ exactly when $a^- \subseteq S^+$; in particular $F(S)$ depends on $S$ only through $S^+$.

*Proof.* Definition 1.2 (2, 3). ∎

**Proposition 2.2 (monotonicity; Dung 1995).** If $S \subseteq T$ then $F(S) \subseteq F(T)$.

*Proof.* $S \subseteq T$ gives $S^+ \subseteq T^+$. If $a \in F(S)$ then $a^- \subseteq S^+ \subseteq T^+$, so $a \in F(T)$. ∎

**Lemma 2.3.** If $S$ is admissible then $F(S)$ is admissible.

*Proof.* *Conflict-free.* Suppose $a, b \in F(S)$ and $a \to b$. Since $S$ defends $b$, some $c \in S$ attacks $a$. Since $S$ defends $a$, some $d \in S$ attacks $c$. Then $d \to c$ with $c, d \in S$, against the conflict-freeness of $S$. *Self-defending.* $S \subseteq F(S)$ since $S$ is admissible, so $F(S) \subseteq F(F(S))$ by Proposition 2.2. ∎

**Lemma 2.4 (the Fundamental Lemma; Dung 1995).** Let $S$ be admissible and let $a, a'$ be acceptable with respect to $S$. Then $S \cup \{a\}$ is admissible, and $a'$ is acceptable with respect to $S \cup \{a\}$.

*Proof.* $S \cup \{a\} \subseteq F(S)$, which is conflict-free by Lemma 2.3; so $S \cup \{a\}$ is conflict-free. By Proposition 2.2, $F(S) \subseteq F(S \cup \{a\})$; hence $S \cup \{a\} \subseteq F(S) \subseteq F(S \cup \{a\})$, and $a' \in F(S) \subseteq F(S \cup \{a\})$. ∎

*Checked:* Proposition 2.2 on every pair $S \subseteq T$, and Lemma 2.4 on every admissible $S$ and every $a, a'$, of every framework of at most four arguments.

## 3. The Grounded Extension as a Least Fixed Point

**Theorem 3.1 (Knaster 1928; Tarski 1955).** A monotone function on a complete lattice has a least fixed point, which is the meet of its pre-fixed points $\{x : f(x) \le x\}$.

The lattice here is $(2^A, \subseteq)$, complete and finite, and $F$ is monotone (Proposition 2.2). In the finite case the least fixed point is reached by iteration, and the proof is elementary.

**Theorem 3.2 (the finite case).** Let $F$ be a monotone function on the subsets of a set $A$ of $n$ elements, and put $G_0 = \emptyset$, $G_{k+1} = F(G_k)$. Then:
1. $G_0 \subseteq G_1 \subseteq G_2 \subseteq \cdots$;
2. there is $K \le n$ with $G_K = G_{K+1}$, and then $G_k = G_K$ for every $k \ge K$;
3. $G = G_K = \bigcup_k G_k$ is a fixed point of $F$;
4. if $F(X) \subseteq X$ then $G \subseteq X$. In particular $G$ is the least fixed point of $F$.

*Proof.* (1) $G_0 = \emptyset \subseteq G_1$. If $G_{k-1} \subseteq G_k$ then $G_k = F(G_{k-1}) \subseteq F(G_k) = G_{k+1}$ by monotonicity. (2) A strictly increasing chain of subsets of $A$ that starts at $\emptyset$ has at most $n$ strict steps, so $G_K = G_{K+1}$ for some $K \le n$; then $G_{K+2} = F(G_{K+1}) = F(G_K) = G_{K+1}$, and so on. (3) $F(G_K) = G_{K+1} = G_K$. (4) $G_0 = \emptyset \subseteq X$; if $G_k \subseteq X$ then $G_{k+1} = F(G_k) \subseteq F(X) \subseteq X$. So every $G_k \subseteq X$. A fixed point satisfies $F(X) \subseteq X$. ∎

**Proposition 3.3 (Dung 1995).** The grounded extension is admissible, hence conflict-free, and is a complete extension.

*Proof.* $G_0 = \emptyset$ is admissible. If $G_k$ is admissible so is $G_{k+1} = F(G_k)$, by Lemma 2.3. So $G = G_K$ is admissible, and $F(G) = G$ makes it complete. ∎

**Definition 3.4 (rounds).** The *round* of $a \in G$ is the least $k \ge 1$ with $a \in G_k$. The round of $a \in G^+$ is the least $k$ with $a \in G_k^+$. An argument of neither has round 0. The number of *rounds* of the framework is the least $K$ with $G_K = G_{K+1}$.

The round of an accepted argument is the length of the shortest justification of it from what is unattacked: at round 1 it is unattacked; at round $k + 1$ each of its attackers is defeated at a round at most $k$, and one of them at round $k$ exactly. An argument defeated at round $k$ has an attacker accepted at round $k$ exactly.

**Proposition 3.5 (the computation).** The following procedure labels $G$ `in`, $G^+$ `out` and the rest `undecided`, with the rounds of Definition 3.4, in $O(|A| + |R|)$ operations. Keep for each argument the number of attacks on it from arguments not yet `out`. At round 1 take the arguments with no attacker. At round $k$, label `in` the arguments taken; label `out`, at round $k$, each argument they attack that is not yet `out`; for each argument so labelled, lower the count of each argument it attacks, and take for round $k + 1$ those whose count reaches zero.

*Proof.* By induction on $k$, the arguments labelled `in` up to round $k$ are $G_k$, and those labelled `out` up to round $k$ are $G_k^+$. For $k = 1$: $G_1 = F(\emptyset)$ is the set of unattacked arguments. If the claim holds at $k$, an argument's count reaches zero during round $k$ exactly when its last attacker entered $G_k^+$ at round $k$, that is when $a^- \subseteq G_k^+$ and $a^- \not\subseteq G_{k-1}^+$: when $a \in G_{k+1} \setminus G_k$ (Proposition 2.1). Such an argument is not `out`, since $G$ is conflict-free. Each attack is looked at once when its tail is labelled `in` and once when its tail is labelled `out`. ∎

*Checked:* extension, labels, rounds and their number against the iteration of the definition, on every framework of at most four arguments and on 300 random ones of twelve.

## 4. The Inclusions among the Semantics

**Proposition 4.1 (Dung 1995).** The complete extensions are the conflict-free fixed points of $F$. The grounded extension is a complete extension and is contained in every other: it is the least of them, and their intersection.

*Proof.* The first statement is Definition 1.2 (5). $G$ is complete by Proposition 3.3. A complete extension $E$ is a fixed point, so $G \subseteq E$ by Theorem 3.2 (4). The intersection of a family that contains $G$ and whose members all contain $G$ is $G$. ∎

**Proposition 4.2.** Every admissible set is contained in a preferred extension. In particular a preferred extension exists.

*Proof.* The admissible sets that contain a given one are finitely many and not none; one of them is maximal. The empty set is admissible. ∎ (For infinite frameworks Dung (1995) proves it by Zorn's lemma.)

**Proposition 4.3 (Dung 1995).** Every preferred extension is complete; and the preferred extensions are exactly the maximal complete extensions.

*Proof.* Let $E$ be preferred. It is admissible, so $E \subseteq F(E)$, and $F(E)$ is admissible by Lemma 2.3. By maximality $F(E) = E$. So $E$ is complete, and maximal among the complete extensions since these are admissible. Conversely let $C$ be a maximal complete extension. By Proposition 4.2 it lies in a preferred extension $E$, which is complete; so $C = E$. ∎

**Proposition 4.4 (Dung 1995).** $S$ is stable exactly when $S^+ = A \setminus S$. Every stable extension is preferred, hence complete; and the stable extensions are exactly the complete extensions $E$ with $E \cup E^+ = A$.

*Proof.* $S \cap S^+ = \emptyset$ and $S \cup S^+ = A$ together say $S^+ = A \setminus S$. Let $E$ be stable. If $b \to a$ with $a \in E$ then $b \notin E$, as $E$ is conflict-free, so $b \in E^+$: $E$ defends each of its members and is admissible. If $a \notin E$ then $a \in E^+$, so no conflict-free set contains $E \cup \{a\}$: $E$ is maximal among the conflict-free sets, all the more among the admissible ones. So $E$ is preferred, and complete by Proposition 4.3. The last statement follows, a complete extension being conflict-free. ∎

**Corollary 4.5.** For every finite framework,
$$\{\text{stable}\} \subseteq \{\text{preferred}\} \subseteq \{\text{complete}\} \subseteq \{\text{admissible}\}, \qquad G = \bigcap \{\text{complete}\}.$$
Each inclusion can be strict. In the cycle $a \to b \to c \to a$ the empty set is preferred and not stable, and there is no stable extension. In the Nixon diamond $a \rightleftarrows b$ (Reiter and Criscuolo 1981) the empty set is complete and not preferred, and $G = \emptyset$ while $\{a\}$ and $\{b\}$ are preferred and stable. In the chain $a \to b \to c$ the set $\{a\}$ is admissible and not complete.

**Corollary 4.6 (acceptance).** Call an argument *credulously accepted* under a semantics when some extension holds it, and *sceptically accepted* when every extension does. Then:
1. sceptical acceptance under the complete semantics is membership of $G$;
2. credulous acceptance under the complete semantics, under the preferred semantics, and membership of some admissible set, are the same;
3. under the grounded semantics the two notions coincide.

*Proof.* (1) Proposition 4.1. (2) A complete or preferred extension is admissible; an admissible set lies in a preferred extension (Proposition 4.2), which is complete (Proposition 4.3). (3) There is one grounded extension. ∎

*Remark.* Sceptical acceptance under the preferred semantics may exceed $G$: with $a \rightleftarrows b$, $a \to c$, $b \to c$, $c \to d$, the preferred extensions are $\{a, d\}$ and $\{b, d\}$, and $G = \emptyset$ ("floating" acceptance). Under the stable semantics, with no extension, every argument is sceptically accepted vacuously; the code returns the number of extensions beside the answer.

**Proposition 4.7 (labellings; Caminada 2006).** A *labelling* assigns to each argument one of `in`, `out`, `undecided`. It is *complete* when an argument is `in` exactly when all its attackers are `out`, and `out` exactly when some attacker is `in`. The map $E \mapsto (E, E^+, A \setminus (E \cup E^+))$ is a bijection from the complete extensions to the complete labellings.

*Proof.* Let $E$ be complete and label by the map; the three sets are disjoint since $E$ is conflict-free. An argument is `out` iff it lies in $E^+$ iff some attacker lies in $E$. An argument is `in` iff it lies in $E = F(E)$ iff $a^- \subseteq E^+$ iff all its attackers are `out`. Conversely let a complete labelling have `in`-set $I$ and `out`-set $O$. The second condition says $O = I^+$; the first says $I = \{a : a^- \subseteq I^+\} = F(I)$. $I$ is conflict-free, as $I \cap I^+ = I \cap O = \emptyset$. So $I$ is complete and the labelling is its image. The map is injective, $E$ being the `in`-set. ∎

*Remark 4.8 (two classes with one answer).* A framework is *well-founded* when there is no infinite chain $a_1 \leftarrow a_2 \leftarrow \cdots$, which for a finite one means no cycle of attacks; it then has exactly one complete extension, which is grounded, preferred and stable (Dung 1995). A finite directed graph without odd cycles has a kernel (Richardson 1953), and a stable extension is a kernel of the graph of attacks reversed; so a framework without odd cycles has a stable extension, and indeed its preferred extensions are its stable ones (it is *coherent*; Dung 1995, on limited controversial frameworks). Neither result is used by the code. *Checked:* both, on the 573 well-founded frameworks and the 1,754 frameworks without odd cycles among those of at most four arguments.

*Checked:* Propositions 4.1–4.4 and 4.7 and Corollary 4.6 on every framework of at most four arguments (66,067 attack relations), by code that evaluates Definition 1.2 literally on bit masks and shares nothing with the library; 39,481 of these frameworks have no stable extension.

## 5. The Enumeration of Extensions

The complete, preferred and stable extensions are found by trying sets. The grounded semantics first removes everything it settles.

**Proposition 5.1 (reduction to the undecided part).** Let $G$ be the grounded extension of $(A, R)$, let $U = A \setminus (G \cup G^+)$ be the arguments it leaves undecided, and let $(U, R|_U)$ be the framework restricted to $U$, with characteristic function $F_U$. Then:
1. no argument of $U$ is attacked by $G$, and none attacks $G$;
2. for every conflict-free $T \subseteq U$ of the restricted framework, $G \cup T$ is conflict-free and $F(G \cup T) = G \cup F_U(T)$;
3. the complete extensions of $(A, R)$ are exactly the sets $G \cup T$ with $T$ a complete extension of $(U, R|_U)$;
4. $G \cup T$ is preferred exactly when $T$ is, and stable exactly when $T$ is.

*Proof.* (1) An argument attacked by $G$ lies in $G^+$. If $u \to g$ with $g \in G$, then $G$ defends $g$ (as $G = F(G)$), so $G$ attacks $u$ and $u \in G^+$.

(2) $G$ and $T$ are conflict-free, and by (1) there is no attack between them. Now let $a \in A$.
- If $a \in G$: $G$ defends $a$, so $G \cup T$ does.
- If $a \in G^+$: some $g \in G$ attacks $a$, and $G \cup T$, being conflict-free, attacks no $g \in G$. So $a \notin F(G \cup T)$.
- If $a \in U$: by (1) no attacker of $a$ lies in $G$. An attacker in $G^+$ is attacked by $G$. An attacker in $U$ is attacked by $G \cup T$ exactly when it is attacked by $T$, since $G$ attacks nothing in $U$. So $G \cup T$ defends $a$ exactly when $T$ defends $a$ in the restricted framework.

(3) Let $E$ be complete. Then $G \subseteq E$ (Proposition 4.1), and $E \cap G^+ = \emptyset$ since $E$ is conflict-free; so $E = G \cup T$ with $T = E \setminus G \subseteq U$ conflict-free. By (2), $G \cup T = F(G \cup T) = G \cup F_U(T)$, so $T = F_U(T)$: $T$ is complete in the restricted framework. Conversely if $T$ is complete there, $G \cup T$ is conflict-free and $F(G \cup T) = G \cup T$ by (2).

(4) By (3) and Proposition 4.3, inclusion among the sets $G \cup T$ being inclusion among the sets $T$. For stability, $A \setminus (G \cup T) = G^+ \cup (U \setminus T)$; $G$ attacks all of $G^+$, and an argument of $U \setminus T$ is attacked by $G \cup T$ exactly when it is attacked by $T$. Apply Proposition 4.4. ∎

*Remark (the cost, and the refusal).* With $u = |U|$, the code tries the $2^u$ subsets of $U$ as bit masks, each in $O(u)$ word operations, and refuses when $u$ exceeds a stated limit (20 by default, never above 30), giving the reason. A framework in which the grounded semantics settles everything, as every well-founded one, costs $O(|A| + |R|)$ whatever its size. Nothing much better is to be expected in the worst case: credulous acceptance under the preferred semantics is NP-complete (Dimopoulos and Torres 1996), and sceptical acceptance $\Pi_2^p$-complete (Dunne and Bench-Capon 2002).

*Checked:* the three enumerations against the definitions evaluated on all $2^{|A|}$ subsets, on every framework of at most four arguments and on 300 random ones of twelve.

## 6. Supports and the Attacks they Induce

**Definition 6.1 (Cayrol and Lagasquie-Schiex 2005).** A *bipolar framework* is a triple $(A, R, R_{\mathrm{s}})$: a framework $(A, R)$ with a second relation $R_{\mathrm{s}} \subseteq A \times A$ of *support*. Write $a \Rightarrow b$ for $(a, b) \in R_{\mathrm{s}}$.

Support has no semantics of its own here. It is used in one way, stated as a choice.

**Definition 6.2 (the supported attack; after Cayrol and Lagasquie-Schiex 2005).** Let $\Rightarrow^+$ be the transitive closure of support. The framework *induced* by $(A, R, R_{\mathrm{s}})$ is $(A, R')$ with
$$R' = R \cup \{(a, c) : a \Rightarrow^+ b \text{ and } b \to c \text{ for some } b\}.$$
An attack of $R' \setminus R$ is a *supported attack*; the code records with each a shortest chain $a = c_0 \Rightarrow c_1 \Rightarrow \cdots \Rightarrow c_k \to c$.

*Rationale.* Whoever gives a reason for $b$ is committed to what $b$ is against. No other derived attack is taken. In particular $a \to b$ and $b \Rightarrow c$ do not give $a \to c$: to defeat an argument for a claim is not to defeat the claim, which may have other arguments or need none.

*Remark.* The attack of $b$ on $c$ is the direct one, so supported attacks are not chained through other supported attacks; and an argument that supports its own attacker attacks itself, and is never accepted. *Caveat.* The definition is cited as the work's, not by its number there; the reading of support adopted is this note's, and Remark 7.8 says what it costs.

## 7. Argument Maps

**Definition 7.1 (after Kunz and Rittel 1970).** A *map* is a triple of finite lists:
- *questions* $q$ (the issues of IBIS), each with a flag *exclusive* and optionally a node $\mathrm{raised}(q)$;
- *positions* $p$, each with a node $\mathrm{answers}(p)$;
- *arguments* $a$, each with a list of *bearings* $(t, \pm)$, a target $t$ taken *for* ($+$) or *against* ($-$); and, uninterpreted, a list of premises and a conclusion.

Each node has an *id* and a *text*.

**Definition 7.2 (well-formedness).** A map is *well formed* when:
1. every id is one word (not empty, without blanks or colons) and no two nodes share an id;
2. every reference names a node of the map;
3. $\mathrm{answers}(p)$ is a question, for every position $p$ (so every position answers a question);
4. every argument has at least one bearing, its targets are positions or arguments, and no target occurs twice;
5. no argument bears on itself *for*;
6. $\mathrm{raised}(q)$, where given, is a position or an argument;
7. there is no cycle of "raised by" (Definition 7.3).

The code reports every violation with the node at fault and the reason, not the first alone.

**Definition 7.3 (what a node hangs from).** Let $H$ be the directed graph on the nodes with an edge from each question to what raises it, from each position to the question it answers, and from each argument to each of its targets other than itself. A *cycle of "raised by"* is a directed cycle of $H$ through a question.

*Remark.* A position has one edge, to a question; a question has at most one, to a position or an argument. So a cycle of $H$ either passes through a question, or lies among the arguments alone. The first kind says that a question arises only from an answer to itself, and is a fault. The second is not: two arguments against each other are a mutual attack, and the Nixon diamond is a well-formed map. An argument against itself is admitted as well (a self-defeating argument); the semantics leaves it undecided.

**Proposition 7.4 (nesting).** In a well-formed map, put $q' \prec q$ when $q$ is reached from $q'$ in $H$ by a path of at least one edge. Then $\prec$ is a strict partial order on the questions: the map nests.

*Proof.* Transitivity is that of reachability. If $q \prec q$ there would be a closed walk of $H$ from $q$ to $q$; a shortest one repeats no node, so it is a cycle through $q$, excluded by Definition 7.2 (7). ∎

The code finds a cycle through a question $q$ as the edge from $q$ to what raises it followed by a shortest path back to $q$ (morphe's shortest paths), and reports each cycle once, at its first question.

**Definition 7.5 (the induced framework).** Let a well-formed map have positions $P$ and arguments $N$. Its *induced framework* is the framework induced (Definition 6.2) by the bipolar framework $(P \cup N, R, R_{\mathrm{s}})$ with
- $a \to t$ for each bearing $(t, -)$ of an argument $a$;
- $p \to p'$ and $p' \to p$ for each two positions $p \ne p'$ that answer one exclusive question;
- $a \Rightarrow t$ for each bearing $(t, +)$ of an argument $a$.

The questions are not arguments of the framework. Each attack keeps its origin: an argument against, the exclusivity of a named question, or a chain of supports.

**Definition 7.6 (standing).** The *standing* of a position or an argument $v$ of a well-formed map is its label in the grounded labelling of the induced framework: *accepted* (`in`), *rejected* (`out`) or *undecided*, with its round (Definition 3.4). Its *deciding* nodes are:
- if $v$ is rejected at round $k$: its attackers accepted at round $k$;
- if $v$ is accepted at round $k$: the nodes accepted before round $k$ that attack an attacker of $v$;
- if $v$ is undecided: its undecided attackers.

A question is *open* when none of its positions is accepted, and *moot* when what raises it is rejected.

**Proposition 7.7.** In a well-formed map:
1. every position and every argument has exactly one standing;
2. at most one position of an exclusive question is accepted;
3. a rejected node has a deciding node; an undecided node has one; the deciding nodes of an accepted node attack every one of its attackers, and are none exactly when it has no attacker;
4. a position on a question that is not exclusive, with no argument against it, is accepted at round 1.

*Proof.* (1) The three labels partition the arguments, $G$ being conflict-free (Proposition 3.3). (2) Two such positions attack each other, and $G$ is conflict-free. (3) By the remarks after Definition 3.4: a node defeated at round $k$ has an attacker accepted at round $k$; a node accepted at round $k$ has every attacker in $G_{k-1}^+$, attacked by a node accepted before round $k$. An undecided node has no accepted attacker, else it would be rejected, and not all its attackers are rejected, else it would be accepted (Proposition 4.7); so one is undecided. (4) No direct attack reaches it, so no supported one does (Definition 6.2): it is unattacked. ∎

*Remark 7.8 (what the grounded semantics does and does not settle).* It is the sceptical semantics: it accepts only what is defended from what nothing attacks. Two exclusive positions with nothing said on them are a Nixon diamond and are both undecided. *Caveat.* If each has an unanswered argument for it, $a_1$ for $p_1$ and $a_2$ for $p_2$, then by Definition 6.2 $a_1 \to p_2$ and $a_2 \to p_1$; $a_1$ and $a_2$ are unattacked, and **both positions are rejected** at round 1. The question is open on either verdict, and the verdicts coincide again once one of the arguments is answered. Under a deductive reading of support (Boella, Gabbay, van der Torre and Villata 2010) an attack on $p_1$ would also attack $a_1$; the two arguments would then attack each other and both positions be undecided. That reading is not taken (the README, §6.1). A map may also be read without supported attacks, in which case arguments *for* play no part in the standings.

*Checked:* each fault of Definition 7.2 found at its node; the induced framework of a hand-made map (the *Euthyphro*, in outline), attack by attack with its origin; its standings, rounds and deciding nodes; Proposition 7.7 (1, 3) and the open questions on the well-formed among 400 random maps; the caveat of Remark 7.8, both ways.

## 8. References

Baroni, Caminada and Giacomin (2011) survey the semantics of §1–§5 and their labellings. The full list is in the domain's [README](../README.md), §7: Dung (1995); Knaster (1928); Tarski (1955); Caminada (2006); Richardson (1953); Reiter and Criscuolo (1981); Dimopoulos and Torres (1996); Dunne and Bench-Capon (2002); Cayrol and Lagasquie-Schiex (2005); Boella, Gabbay, van der Torre and Villata (2010); Kunz and Rittel (1970).
