# Philosophy

*Status: draft (2026-11-13; [ADR 0111](../../development/decisions/0111-musicology-and-philosophy-founded.md)).* The domain of questions, the positions taken on them and the arguments for and against. It is a field at height 0, standing on the foundations alone, C++ from the start. Its first module, `arguments`, is written for Histos Harmonia, the history archive and learning work in which every historical object is asked what question it answers, why that question arose, what made it answerable and to whom it was worth answering.

## 1. Purpose

A philosophical text is read here as an answer. The domain holds the objects that make the reading explicit:

- a **question**, which may be raised by an answer to an earlier one;
- the **positions** taken on a question;
- the **arguments** that speak for or against a position, or for or against another argument;
- the **standing** of each position once every argument has been heard: accepted, rejected or undecided, with the arguments that decide it;
- the **open questions**, on which no position is accepted.

Two layers are kept apart.

1. *The map* names objects of philosophy: questions, positions and arguments with their texts, after the issue-based information systems of Kunz and Rittel (1970). It is what a person writes and reads.
2. *The framework* names none: a finite directed graph of attacks, after Dung (1995), whose semantics are fixed points of a monotone function on its subsets. It is what is computed.

A map induces a framework, and the standing of a position is the label the framework's grounded semantics gives it. The domain evaluates the structure of a debate, not its content: whether an argument's premises entail its conclusion is for the planned module `logic`, and until then an argument stands unless another argument in the map defeats it.

*Remark.* The verdict is therefore relative to the map. "Accepted" means: defended, within what has been written down, from what nothing written down attacks. A map of a doctrine drawn by its opponents and one drawn by its adherents give different standings, and each is a true statement about its map.

Its theory note, [theory/arguments.md](theory/arguments.md), states and proves what the code relies on.

## 2. Modules

| Module | Holds | Status |
|---|---|---|
| `arguments` | **Frameworks** (`frameworks.hpp`): abstract argumentation after Dung (1995). Conflict-free and admissible sets; the characteristic function; the grounded extension as its least fixed point, with the round at which each argument is accepted or defeated; the three-valued labelling (in, out, undecided); complete, preferred and stable extensions by enumeration over the arguments the grounded semantics leaves undecided, refused with the reason above a stated number of them; credulous and sceptical acceptance; supports by the attacks they induce (the supported attack, after Cayrol and Lagasquie-Schiex 2005). **Maps** (`maps.hpp`): argument maps after IBIS. Questions, positions, arguments and what raises a question; every fault of a map with where it is and why; the framework a map induces; each position's and argument's standing with the nodes that decide it; the open questions; a written form of lines, read and written | draft: 90 checks |
| `logic` | The formal validity of arguments: an argument's premises and conclusion as formulas, entailment and consistency, the attacks that follow from the logic (rebuttal and undercutting, after ASPIC+). On morphe's planned `logic` | planned |
| `concepts` | Concepts and their relations: definition, subsumption, opposition, the senses of a term across texts | planned |
| `positions` | Doctrines and the questions they answer across texts: a position held by several authors, its versions, the questions a tradition inherits | planned |
| `ethics` | Normative questions: values, their orderings by audience, and arguments that promote them | planned |

*Remark (general methods held here for now).* The semantics of frameworks are general mathematics: the fixed points of a monotone function on the subsets of a directed graph. They name nothing of philosophy, and other sciences use them (non-monotonic reasoning and logic programming, the stable sets of games, kernels of digraphs). They are written here because this domain is the first to need them, and wait to move to morphe's planned `logic` when a second domain does; `frameworks.hpp` uses nothing of `maps.hpp`, so that the move is a change of namespace. The directed graph itself and the shortest way round a cycle are already morphe's (`combinatorics`), and are used, not written again.

## 3. Conventions

### 3.1 Frameworks

- The arguments of a framework are the numbers $0, \dots, n - 1$; an attack is an edge (attacker, attacked) of morphe's `Multigraph`. An attack stated twice counts once. A loop is an argument that attacks itself; it is allowed, and is never accepted.
- A set of arguments is a vector in ascending order without repeats (`Set`). Extensions are listed in lexicographic order.
- $S^+$ is the set of arguments attacked by a member of $S$.
- Labels follow Caminada (2006): `in`, `out`, `undecided`. The round of an accepted argument is the least $k \ge 1$ with the argument in $F^k(\emptyset)$; of a defeated one, the least $k$ such that an argument of $F^k(\emptyset)$ attacks it; of an undecided one, 0.
- Sceptical acceptance is membership of every extension, literally: under a semantics with no extension (the stable one may have none) every argument is sceptically accepted, vacuously, and the count of extensions is returned beside the answer.
- An enumeration is refused, never truncated. The refusal carries its reason; the default limit is 20 arguments left undecided by the grounded semantics, and no caller may raise it above 30.

### 3.2 Maps

- Every node has an id and a text. An id is one word: not empty, without blanks or colons. Ids are unique across the three kinds.
- A position answers exactly one question. An argument bears on one or more positions or arguments, on each for it (`pro`) or against it (`con`). A question may be raised by one position or argument.
- A question is **exclusive** when its positions cannot hold together. Only then do its positions attack one another. The default is not exclusive: a question such as "what moved the author to write" may have several true answers.
- **Support is read by the supported attack alone** (a stated choice; the theory note, §6): an argument for $b$, where $b$ attacks $c$, attacks $c$. So an argument for a position counts against its exclusive rivals, and an argument for an argument against $p$ counts against $p$. An attack on an argument is *not* an attack on what that argument supports: a claim may stand though one argument for it fails. `Induction{.supported_attacks = false}` reads a map without them.
- Two arguments against each other are no fault of a map; nor is an argument against itself (a self-defeating argument, left undecided). An argument *for* itself is a fault.
- Standings are those of the grounded semantics, the sceptical one. It settles no tie: two exclusive positions with nothing said on them are both undecided. The other semantics are reached through the induced framework (`induce`, then `preferred_extensions` or `acceptance`).

*Caveat.* If two exclusive positions each have an unanswered argument for them, each is attacked by an accepted argument and **both are rejected**, not undecided. The question is open either way, but the two verdicts differ in what they say; see §6.1.

### 3.3 The Written Form of a Map

No reader of JSON is shared below the domains (the history domain's README, §6.2, where a second copy of one was made and its shared place left open). This domain does not make a third: a map is written as lines of text, read by `read` and written by `write`.

```
# The Euthyphro, in outline (Plato, Euthyphro 10a–13c)
exclusive question q1: Is the pious loved by the gods because it is pious, or pious because they love it?
question q2 <- p1: What is the pious, if it is not what the gods love?

position p1 -> q1: It is loved because it is pious.
position p2 -> q1: It is pious because it is loved.
position p3 -> q2: The pious is the part of the just that cares for the gods.

con a1 -> p2: The gods love the pious for being pious, so their love cannot be what makes it pious.
premise a1: The gods love the pious because it is pious.
premise a1: What is loved for being so is not made so by being loved.
conclusion a1: The pious is not pious because the gods love it.
con a2 -> a1: A standard of piety apart from the gods' love would stand above the gods.
con a3 -> a2: A love without a reason is arbitrary, and gives no one a reason to be pious.
con a4 -> p3: Care betters what it cares for, and the gods are not bettered by us.
```

One statement a line. A line that is empty, or begins with `#`, is passed over. Otherwise a line is a head, then optionally a colon and a text that runs to the end of the line:

| Line | States |
|---|---|
| `question <id>: <text>` | a question |
| `exclusive question <id>: <text>` | a question whose positions cannot hold together |
| `question <id> <- <id>: <text>` | a question raised by the position or argument after the arrow |
| `position <id> -> <id>: <text>` | a position and the question it answers |
| `pro <id> -> <id>: <text>` | an argument for a position or an argument |
| `con <id> -> <id>: <text>` | an argument against one |
| `pro <id> -> <id>`, `con <id> -> <id>` | a further bearing, without text, of an argument stated above |
| `argument <id>: <text>` | an argument that bears on nothing yet |
| `premise <id>: <text>` | a premise of the argument stated above |
| `conclusion <id>: <text>` | its conclusion |

- The words of a head are parted by blanks, the arrows among them. Statements may come in any order, except that a premise, a conclusion or a further bearing follows the line that states its argument.
- In a text, `\\` is a backslash, `\n` a new line, `\r` a carriage return, `\t` a tab and `\s` a space; `\s` is needed only where a text begins or ends with a space, since the blanks about a text are dropped. A text may hold colons, `#` and arrows.
- The reader reads syntax only, and reports each line it cannot read with its number and the reason, reading the rest. It refuses an id stated twice. What the references mean (an id that is not there, the wrong kind, a cycle) is for `faults`.
- `read(write(m)).map == m` for every map whose ids and references are each one word and whose ids are distinct: in particular for every well-formed map.

On this map the domain answers: `a3` is accepted at round 1 and defeats `a2`; `a1` is reinstated at round 2 and defeats `p2`; `p1` is accepted at round 3, defended by `a1`; `p3` is rejected by `a4`; and the open question is `q2`.

## 4. Dependencies

morphe (`combinatorics`: the directed graph, its incidence, shortest paths) and calx (`verification`, and `random` in the checks). No domain.

## 5. Code and Checks

Header-only C++20, namespace `philosophy::arguments`, the CMake target `philosophy::arguments`.

| Header | Functions |
|---|---|
| `philosophy/arguments/frameworks.hpp` | `Framework`; `attacked_by`, `conflict_free`, `defends`, `characteristic`, `admissible`, `complete`, `stable`; `grounded`; `labelling`, `complete_labelling`; `complete_extensions`, `preferred_extensions`, `stable_extensions`; `acceptance`; `Bipolar`, `supported_attacks` |
| `philosophy/arguments/maps.hpp` | `Map` (`Question`, `Position`, `Argument`, `Bearing`); `faults`, `well_formed`; `induce`; `evaluate`, `standings`, `open_questions`; `write`, `read` |

`node domains/philosophy/tests/run-tests.mjs` builds and runs 90 checks:

- **Frameworks worked by hand:** the Nixon diamond (grounded empty, two preferred, two stable); a chain $a \to b \to c$ (grounded $\{a, c\}$, with its rounds) and chains of five and of 1001; an odd cycle of three (no stable extension; the empty set its one preferred extension); self-attackers; an even cycle; floating acceptance; a preferred extension that is not stable; the refusal of an enumeration above its limit.
- **Brute force on every framework of at most four arguments** (66,067 attack relations, 1,052,741 subsets), by code on bit masks that shares nothing with the library. Every definition agrees on every subset, and each theorem the library relies on is verified there, one check each: the characteristic function is monotone; the Fundamental Lemma; the grounded extension is the least fixed point, is complete, lies in every complete extension and is their intersection; every stable extension is preferred; every preferred extension is complete; a preferred extension exists; complete labellings correspond to complete extensions; sceptical acceptance under the complete semantics is the grounded extension; a well-founded framework has one complete extension, which is stable; a framework without odd cycles is coherent.
- **300 random frameworks of 12 arguments** against brute force over their 4,096 subsets.
- **Supports:** the supported attack and its chains; what is not derived; a support that changes what is accepted.
- **Maps:** each fault found where it is (a position that answers no question, a reference to an id that is not there or of the wrong kind, an argument for itself, an id used twice, cycles of "raised by" with the nodes round them); the framework a hand-made map induces, attack by attack with its origin; standings with their rounds and deciding nodes; open and moot questions; the caveat of §3.2.
- **The written form:** a hand-made map written as the lines expected and read back; a text written by hand; texts with blanks, new lines, backslashes and letters beyond ASCII; fourteen lines that cannot be read, each reported with its number and reason; 400 random maps written and read back, and the well-formed among them evaluated consistently.

## 6. Open Questions

1. **The reading of support.** The supported attack alone gives the caveat of §3.2: exclusive rivals with an unanswered argument each are both rejected. Under a deductive reading of support (Boella, Gabbay, van der Torre and Villata 2010; Cayrol and Lagasquie-Schiex 2013), an attack on a position is also an attack on the arguments for it, the two arguments rebut each other and both positions are left undecided. Which reading a map of a philosophical debate wants is open; it may differ by the kind of argument (a proof supports deductively, an example does not).
2. **A shared reader of JSON.** The same question as the history domain's (its README, §6.2). When its place is decided the map gains a JSON form beside the lines, with no change to `Map`.
3. **The move to morphe.** The semantics of frameworks move to morphe's planned `logic` when a second domain needs them (§2).
4. **Structure inside an argument.** Premises and conclusions are kept as texts. With `logic` they become formulas, and attacks follow from them (rebuttal, undercutting and undermining, as in ASPIC+: Modgil and Prakken 2014) instead of being stated by hand.
5. **To whom a question was worth answering.** Value-based frameworks (Bench-Capon 2003) let an attack succeed or fail by an audience's ordering of the values the arguments promote: the same map then has a standing for each audience. This is the formal counterpart of the fourth question Histos Harmonia asks, and belongs with `ethics`.
6. **Standings beyond the grounded semantics.** The preferred extensions of a map are its coherent overall views; showing them as such, and a position's credulous acceptance, is a matter of presentation for the work, on functions already here.
7. **A question raised by several nodes,** and a position that answers several questions, are not expressible; neither has been needed yet.

## 7. References

- Baroni, P., Caminada, M. and Giacomin, M. (2011), An introduction to argumentation semantics, *The Knowledge Engineering Review* 26(4), 365–410.
- Bench-Capon, T. J. M. (2003), Persuasion in practical argument using value-based argumentation frameworks, *Journal of Logic and Computation* 13(3), 429–448.
- Boella, G., Gabbay, D. M., van der Torre, L. and Villata, S. (2010), Support in abstract argumentation, in *Computational Models of Argument: Proceedings of COMMA 2010*, IOS Press.
- Caminada, M. (2006), On the issue of reinstatement in argumentation, in *Logics in Artificial Intelligence: JELIA 2006*, LNCS 4160, Springer.
- Cayrol, C. and Lagasquie-Schiex, M.-C. (2005), On the acceptability of arguments in bipolar argumentation frameworks, in *Symbolic and Quantitative Approaches to Reasoning with Uncertainty: ECSQARU 2005*, LNCS 3571, Springer.
- Cayrol, C. and Lagasquie-Schiex, M.-C. (2013), Bipolarity in argumentation graphs: towards a better understanding, *International Journal of Approximate Reasoning* 54(7), 876–899.
- Conklin, J. and Begeman, M. L. (1988), gIBIS: a hypertext tool for exploratory policy discussion, *ACM Transactions on Office Information Systems* 6(4), 303–331.
- Dimopoulos, Y. and Torres, A. (1996), Graph theoretical structures in logic programs and default theories, *Theoretical Computer Science* 170, 209–244.
- Dung, P. M. (1995), On the acceptability of arguments and its fundamental role in nonmonotonic reasoning, logic programming and n-person games, *Artificial Intelligence* 77(2), 321–357.
- Dunne, P. E. and Bench-Capon, T. J. M. (2002), Coherence in finite argument systems, *Artificial Intelligence* 141, 187–203.
- Knaster, B. (1928), Un théorème sur les fonctions d'ensembles, *Annales de la Société Polonaise de Mathématique* 6, 133–134.
- Kunz, W. and Rittel, H. W. J. (1970), *Issues as Elements of Information Systems*, Working Paper 131, Institute of Urban and Regional Development, University of California, Berkeley.
- Modgil, S. and Prakken, H. (2014), The ASPIC+ framework for structured argumentation: a tutorial, *Argument & Computation* 5(1), 31–62.
- Plato, *Euthyphro*, 10a–13c (the hand-made map of the checks and of §3.3, in outline).
- Reiter, R. and Criscuolo, G. (1981), On interacting defaults, in *Proceedings of the 7th International Joint Conference on Artificial Intelligence*, 270–276 (the Nixon diamond).
- Richardson, M. (1953), Solutions of irreflexive relations, *Annals of Mathematics* 58(3), 573–590.
- Tarski, A. (1955), A lattice-theoretical fixpoint theorem and its applications, *Pacific Journal of Mathematics* 5(2), 285–309.
