# The Theory of the Philosophy Domain: Arguments, their Frameworks and their Maps

*Status: draft (2026-11-13). The results the module `arguments` relies on, with their hypotheses and proofs or citations. §1–§6, on abstract frameworks, are now apodeixis's (below); §7 concerns argument maps (`maps.hpp`). Every framework is finite. Where a result is Dung's (1995) it is cited as his, without a number; the proofs given are for the finite case and are short enough to be stated in full.*

## 1–6. Frameworks (moved)

The sections on abstract frameworks (Definitions 1.1–6.2 and the results between them: the characteristic function, the grounded extension as a least fixed point, the inclusions among the semantics, the enumeration of extensions, supports and the attacks they induce) moved with the frameworks to the foundation apodeixis, numbered as they were, in its note [docs/argumentation.md](https://github.com/huangwanhong-maker/apodeixis/blob/main/docs/argumentation.md) ([ADR 0112](../../../development/decisions/0112-apodeixis-and-tekmerion.md)). A reference in §7 to a definition or result numbered 1.x–6.x is to that note.

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
