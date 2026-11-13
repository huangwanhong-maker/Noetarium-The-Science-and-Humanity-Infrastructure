---
title: "A Language of Dynamical Systems: Change, Structure, Constraint and Difference"
status: promoted
date: 2026-11-12
related: [physis `dynamics`, `mechanics`, `field_theory`, `quantum_gravity`, `quantum` and `fields` (ADR 0035, ADR 0039, ADR 0040, ADR 0053, ADR 0054), morphe `formal` (grammars over semirings, the BNF reader; ADR 0047, ADR 0064), tyche `probability` (ADR 0048), heuresis (ADR 0073), kategoria and its script language (ADR 0100), physical computing (ADR 0076), the survey research/literature/reports/Dynamical systems models and braids.md (in progress)]
---

# A Language of Dynamical Systems: Change, Structure, Constraint and Difference

*Promoted on 2026-11-13 by [ADR 0103](../decisions/0103-kinesis-and-the-dynamics-lab.md): the library is kinesis, whose theory note (`foundations/kinesis/docs/theory.md`) holds §3, §4 and §10 as they were built, and whose `language/grammar.hpp` holds the grammar as it now is (with events, inputs, parts and named observations, which §9 lacks). What follows is the note as it was written, kept as the record of the discussion.*

*The record of a discussion with the user on 2026-11-12, at their opening: "We will begin the next ambitious project. Dynamical System one", an application to "analyze, visualize various dynamical system (both classical, and (open) quantum)" and to induce structured representations from data, for which "we even need create a 'domain specific langauge' that can define dynamical systems". Nothing here is built. The grammar of §9 is a first draft, checked only as syntax. The note is for the user to mark up; the survey may change it.*

## 1. The Question

A dynamical system may be known in several ways: by a law one can write down, deterministic or stochastic, continuous or not; by a device one can only drive and read; or by data alone. The work planned is to analyse and draw systems of all these kinds, and to go from data to a written law.

Every tool of that work needs one thing first: a way to say what a system is. The question of this note is what a declaration of a dynamical system must contain, so that one notation serves a flow, a field theory given by its action, a stochastic model, an open quantum system and a grammar.

The user set the first test: "Whether our language now can define a generative grammar"; "Whether we can define background-independent dynamics, like utilize GFT"; "Whether it can define a random model, like diffusion model, contains random term that obey some distribution. If these 3 ones can represent, I think it is at least a sucess language at the first step."

## 2. The Turns of the Discussion

The design first sketched was a language of equations of motion: `x' = f(x)`, `u_t = F(u)`. The user turned it four times.

1. **A law is not only an equation.** "I don't think this is a good language. as it is mainly for PDE. We even allow field theory one, like action S's representation." A system may be given by its action, and the equation of motion is then derived, not written.
2. **Random variables need their own treatment.** "how represent random variable, is difficult. I think we need now need random lib. include sampler?" This became §7, and the library goes to tyche.
3. **Four primitives.** From the user's paper on generative relations: "change, structure, constaint, and difference can be 4 primitives that construct a dynamical system. Of course we can not utilize the specific equation, as they are philosophical, not indicate is engineering efficient. But we need consider such 4 primitive/elements in design." And of a clause in a sketch: "you 'on lattice(L, L)' define the background geometry!" The primitives became the parts of a declaration (§5).
4. **Operators.** Asked what "operator algebra" should mean for trajectories and braids: "general gravity sense. something be lift into operator, and non-commutative geometric sense." This became §4.

The user also holds that "each generetive grammar is also a dynamical system", which the first test makes concrete.

## 3. The Four Primitives

The user's primitives, as given:

$$
\begin{aligned}
\textbf{(Change)}\quad & \alpha : \mathcal{M} \times \mathcal{S} \to \mathcal{S}, \qquad (m, x) \mapsto m(x) \\
\textbf{(Structure)}\quad & \alpha(m_1 m_2,\, x) = \alpha(m_1,\, \alpha(m_2, x)) \\
\textbf{(Constraint)}\quad & x \sim y \;\Longrightarrow\; \alpha(m, x) \sim \alpha(m, y) \\
\textbf{(Difference)}\quad & \Delta(m) = \{\, (x, y) : x \not\sim y \ \wedge\ \alpha(m, x) \sim \alpha(m, y) \,\}
\end{aligned}
$$

### 3.1 What They Say

- **Change and Structure** together say that $\mathcal{M}$ acts on $\mathcal{S}$: a dynamical system is a representation of a monoid of changes. Time is one such monoid ($\mathbb{N}$, $\mathbb{R}_{\ge 0}$); the words over a set of rules are another.
- **Constraint** says that $\sim$ is a congruence of the action. The action then descends to the quotient, $\bar\alpha(m) : \mathcal{S}/{\sim} \to \mathcal{S}/{\sim}$.
- **Difference** collects what a change stops telling apart. Under Constraint, $\Delta(m) = \emptyset$ exactly when $\bar\alpha(m)$ is injective.

*Remark.* If $\mathcal{M}$ is a group and Constraint holds for every element, each $\bar\alpha(m)$ is a bijection and $\Delta(m)$ is empty. A non-empty Difference is therefore the mark of irreversibility relative to $\sim$: dissipation, merging, the forgetting of a coarse description.

### 3.2 Three Generalisations for Engineering

The equations are philosophical, as the user says. Three generalisations make them cover the systems of §1.

1. **A change carries a weight.** Let $K$ be a semiring. A change sends a state to a $K$-weighted family of states, $\alpha(m) : \mathcal{S} \times \mathcal{S} \to K$, and Structure becomes the sum over intermediate states:
   $$\alpha(m_1 m_2)(x, z) = \sum_{y} \alpha(m_2)(x, y)\, \alpha(m_1)(y, z).$$
   The choice of $K$ is the kind of system (§6). A deterministic system is the case in which every $\alpha(m)(x, \cdot)$ is a single state of weight one.
2. **Changes may be typed.** A rule applies only where it matches; an event may add or remove a participant. $\mathcal{M}$ is then a category whose objects are the types of state, and $\alpha$ a functor on it. The user's draft on braids already writes its event operators so, $T_e : S_L \rightharpoonup S_{L'}$.
3. **A constraint is checked, not assumed.** A declared $\sim$ is a claim about the law. A tool answers that every change respects it, or shows a pair where it fails, as kategoria answers a claim (ADR 0100).

*Caveat.* Constraint and Difference are stated above for unweighted changes. Their weighted forms are not yet written. For probabilities the weighted Constraint should be lumpability, which physis holds with its coupling bound (its decision 0018); this is to be shown, not assumed.

## 4. The Lift to Operators

The lift the user means: the space is replaced by its algebra of observables, and a change becomes an operator on that algebra.

Let $A$ be the algebra of all functions $\mathcal{S} \to k$ into a field $k$, and let $A_\sim \subseteq A$ be the functions constant on the classes of $\sim$. Define $\alpha^*(m) f = f \circ \alpha(m, \cdot)$.

**Proposition.** *For an unweighted system:*

1. *(Change) each $\alpha^*(m)$ is an endomorphism of the algebra $A$;*
2. *(Structure) $\alpha^*(m_1 m_2) = \alpha^*(m_2)\, \alpha^*(m_1)$;*
3. *(Constraint) $\sim$ is respected by $m$ if and only if $\alpha^*(m) A_\sim \subseteq A_\sim$;*
4. *(Difference) under Constraint, $\Delta(m) = \emptyset$ if and only if $\alpha^*(m)$ maps $A_\sim$ onto $A_\sim$.*

*Proof.* (1) and (2) are immediate: $\alpha^*(m_1 m_2) f = f \circ \alpha(m_1) \circ \alpha(m_2) = \alpha^*(m_2)(\alpha^*(m_1) f)$. (3) If $\sim$ is respected and $f \in A_\sim$, then $x \sim y$ gives $\alpha(m, x) \sim \alpha(m, y)$ and so $f(\alpha(m,x)) = f(\alpha(m,y))$. Conversely, if $x \sim y$ but $\alpha(m,x) \not\sim \alpha(m,y)$, the indicator of the class of $\alpha(m,x)$ lies in $A_\sim$ and its image does not. (4) $A_\sim$ is the algebra of functions on $Q = \mathcal{S}/{\sim}$ and $\alpha^*(m)$ acts on it as composition with $\varphi = \bar\alpha(m)$. If $\varphi$ is injective, any $g$ on $Q$ is $f \circ \varphi$ for $f(\varphi(q)) = g(q)$, extended by zero. If $\varphi(q_1) = \varphi(q_2)$ with $q_1 \ne q_2$, every $f \circ \varphi$ agrees at $q_1$ and $q_2$, and the indicator of $q_1$ is not reached. $\square$

*Caveat.* The proposition is for the algebra of all functions. For continuous or measurable observables, (4) needs hypotheses on the spaces and maps that are not stated here.

What the lift gives:

- **Classical systems.** $A$ is commutative, and $\alpha^*$ is Koopman's operator.
- **Weighted systems.** $\alpha^*(m) f(x) = \sum_y \alpha(m)(x, y) f(y)$. For probabilities it is a positive map that keeps the constants, and it is an algebra homomorphism exactly when the change is deterministic.
- **Quantum systems.** $A$ is noncommutative and $\alpha^*(m)$ is a completely positive map that keeps the unit. Open quantum dynamics is the same four lines.
- **Braids.** A braid becomes an operator through a representation of the braid group, and its invariants become traces and spectra.

In the language, `lift` is a derivation available for every system (§5.3). The survey's part on operators and noncommutative geometry is to supply the citations and the cases beyond finite sets.

## 5. A System as a Declaration

### 5.1 The Parts

A declaration names its carrier and then gives the four primitives.

| Part | What it declares | Examples |
|---|---|---|
| `state`, `param`, `noise`, `unknown` | the carrier $\mathcal{S}$, its parameters, its sources of noise, and what is not yet known | variables, a path, a field over arguments, words over an alphabet, a density matrix |
| `change` | the generators and how each acts | a vector field, a stochastic differential, an action, a Hamiltonian and its jumps, rules |
| `structure` | how changes compose into histories, and what a history weighs | real or integer time, derivations, histories weighted by $e^{-S}$, the semiring of weights, the stochastic calculus |
| `constraint` | what every change respects | a background geometry, an invariance, a conserved quantity, an identity |
| `difference` | what is told apart | the observations, or the group up to which states are the same |

*Remark.* The background is a constraint, as the user read it. `x on lattice(L, L)` says that the argument `x` ranges over a fixed geometry and that the law is local on it. A system with no `on` clause declares no background.

### 5.2 The Forms of Change

| Form | Written | What computes it today |
|---|---|---|
| a flow | `x' = f`, with real time | calx `integrators`; physis `dynamics` |
| a map | `x' = f`, with integer time | physis `dynamics` (planned there) |
| a stochastic differential | `dX = a * dt + b * dW` | not found in the foundations: steppers for stochastic differential equations are to be written |
| an action | `action S = ...` | physis `mechanics` (motion from a discrete action; Euler–Lagrange equations by jets), `field_theory` (lattice fields), `quantum_gravity` (group field theories) |
| a Hamiltonian with jumps | `hamiltonian H = ...`, `jump L = ...` | physis `quantum`, for the two-level system only |
| rules | `name : left -> right @ weight` | morphe `formal` (grammars and parsers over semirings, graph rewriting); syntaxis |

The prime is read by the time declared: a derivative when time is real, the next value when it is an integer.

### 5.3 Derivations between Forms

One system has several presentations. A derivation is a named transformation from one to another, written `system B = derivation A`, and each is a statement that can be checked.

| Derivation | From | To | In the foundations |
|---|---|---|---|
| `vary` | an action | its Euler–Lagrange equations | physis `mechanics` |
| `legendre` | a Lagrangian | a Hamiltonian | not written |
| `expand` | an action weighted by $e^{-S}$ | rules that glue its vertices, weighted by amplitudes | physis `field_theory` and `quantum_gravity` (Feynman graphs, exact series) |
| `reverse` | a stochastic differential | its time reversal, with the score unknown | not written |
| `unravel` | a Hamiltonian with jumps | quantum trajectories | the neuroscience domain's quantum emitter, in one case |
| `lift` | any system | its operator form (§4) | not written |

*Remark.* `expand` joins two of the user's tests: a group field theory is, through its expansion, a weighted generative grammar of complexes (ADR 0040, ADR 0042).

### 5.4 Templates

A term may hold an unknown, written `?name`: a number, a function, or a sparse combination from a library of terms. A system with unknowns is a template, and going from data to a model is filling them. A model of a catalogue is a term whose only unknowns are its parameters. An unknown may stand in any form of change: an unknown potential inside an action is learning an action.

## 6. The Weights of Histories

| Semiring $K$ | A history is | Systems |
|---|---|---|
| Boolean | possible or not | nondeterministic systems; a grammar's language as the reachable set |
| probabilities | more or less likely | stochastic processes, stochastic grammars |
| non-negative reals | weighted by $e^{-S}$ | statistical field theories; the weight is normalised only by a sum that is not computed |
| complex numbers | weighted by an amplitude | closed quantum systems, Feynman amplitudes |
| operators | composed, not summed commutatively | open quantum systems |

morphe's parsers and alignments already compute sums over histories in a semiring (ADR 0043, ADR 0047).

*Caveat.* The last row is not a semiring of numbers. Whether open quantum systems fit this table, or only the lift of §4, is open.

## 7. Noise and Laws

### 7.1 Noise as a Declared Source

A random variable is a term over declared sources of noise: a Wiener process, a Poisson process, an initial law, a draw at each step. The system is then a deterministic map of state and noise. This is the definition of a random dynamical system, and it keeps runs reproducible, as heuresis's keyed draws and nomos's draws by identity do.

A stochastic differential states its calculus, Itô or Stratonovich, in `structure`.

### 7.2 A Law Known at a Level of Access

| Level | What is known | Example |
|---|---|---|
| closed form | density, distribution function, quantiles | tyche's eight distributions |
| up to normalisation | a weight | $e^{-S}$ |
| by its score | $\nabla \log p$ | a diffusion model |
| by a sampler | a device that draws | a quantum device, a simulator |
| by samples | data | `X(0) ~ data` |

Samplers convert between levels: Metropolis, Hamiltonian Monte Carlo and Langevin dynamics go from a weight to samples; a reversed stochastic differential goes from a score to samples; score matching goes from samples to a score. Each sampler is itself a dynamical system, a chain whose stationary law is the target, so the tools of the work apply to its convergence.

### 7.3 Placement

The user agreed that this library goes in tyche, not in a foundation of its own. tyche's `probability` holds distributions as objects with no sampling (its open decision 3), its planned `bayes` names the Markov chain methods, and calx holds the streams. heuresis's separation of the family, the sampler and the rule (its decision 0003) is the precedent.

## 8. The Three Tests

Each system below is written in the grammar of §9 and was parsed by it.

### 8.1 A Generative Grammar

```
system anbn {
  state w : words over { S, a, b }, start S
  change {
    grow : S -> a S b
    stop : S -> a b
  }
  structure { derivations ; weights boolean }
  difference { observe terminal }
}
```

- **Change:** the two rules. **Structure:** derivations, which compose freely; a rule applies where its left side matches, so the changes are typed and partial (§3.2).
- **Weights.** With Boolean weights the language $\{a^n b^n\}$ is the set of observed states reachable from the start. With `@ 0.7` and `@ 0.3` on the rules and `weights probability` it is a stochastic grammar.
- **Constraint.** The user's group grammars (ADR 0047) are this primitive: with $x \sim g \cdot x$ and equivariant rules, the group action is a respected $\sim$.
- **Reconstruction.** A derivation is a trajectory, and the parse forest of a word is the set of histories that could have produced it. An ambiguous word is one whose history is not identified.
- **What computes it:** morphe's grammars and parsers. Graphs or complexes in place of words give morphe's graph rewriting and syntaxis's grammars.

### 8.2 A Background-Independent Dynamics

```
system boulatov {
  state phi(g1, g2, g3) : real, g1 g2 g3 in group G
  param lambda = 0.1
  constraint {
    phi(g1 * h, g2 * h, g3 * h) = phi(g1, g2, g3)
  }
  change {
    action S = 1/2 * integral(phi(g1, g2, g3)^2)
             + lambda/24 * integral(phi(g1, g2, g3) * phi(g1, g4, g5) * phi(g2, g5, g6) * phi(g3, g6, g4))
  }
  structure { histories weight exp(-S) ; weights real }
}

system tetrahedra = expand boulatov
```

- **No background.** There is no `on` clause and no time. The group is the domain of the field, not a spacetime. The interaction joins four fields by the pattern of a tetrahedron's six edges, each argument shared by two fields.
- **Spacetime is generated.** `expand` gives the rules that glue tetrahedra, weighted by amplitudes: the Feynman graphs of the theory, which are complexes.
- **For contrast,** a field on a background declares it: `constraint { x on lattice(L, L) }`.
- **What computes it:** physis's group field theories over finite groups, exactly (its decisions 0006, 0011).

*Caveat.* The language does not certify background independence. It lets a system declare no spacetime; the group remains fixed data. The other route, geometry as the state itself (complexes rewritten by Pachner moves), is the rules form of §8.1 on complexes.

### 8.3 A Random Model

```
system forward {
  state X(t) : real^d
  param d = 2, T = 1, beta(t) = 0.1 + 19.9 * t
  noise W : wiener(d)
  noise X(0) ~ data
  change { dX = -beta(t)/2 * X * dt + sqrt(beta(t)) * dW }
  structure { time t in [0, T] ; weights probability ; calculus ito }
}

system generate = reverse forward
```

- **Noise with a stated law.** `W` is a Wiener process; the initial law is known only by samples. A source may have any stated law: `noise xi : levy(1.5)` drives a map in discrete time.
- **The diffusion model.** `reverse` gives the reversed process, in which one term is unknown: the score of the law at each time. Learning it is filling that unknown.
- **What computes it:** nothing yet. Steppers for stochastic differentials, the reversal and the learning of a score are to be written; the last may use torch on the CPU, in low dimension.

## 9. A First Grammar

### 9.1 Choices

- **Blocks in braces,** as kategoria's scripts: a grammar over bytes read by a context-free parser cannot read indentation.
- **Products are written with `*`.** Juxtaposition would be ambiguous with application.
- **ASCII only** in this draft: `phi`, `lambda`, `integral`. Unicode spellings can be added as alternatives.
- **`integral`, `sum`, `grad`, `exp` are ordinary applications.** The variables integrated over are implicit; this is to be made explicit.
- **A line may continue** before a binary operator. The left side of an equation is a name, a primed name or an application, so no item begins with an operator.

### 9.2 The Grammar

In morphe's dialect (`formal/bnf.hpp`): the first rule is the start, names beginning with `_` are hidden, and whitespace is written out.

```
script    ::= _ ( system ( _sep system )* _ )? ;

_    ::= ( [ \t\n\r] | _c )* ;
_c   ::= "#" [^\n]* ;
_s   ::= [ \t]* ;
_sp  ::= [ \t]+ ;
_sep ::= _s _c? ( [\n\r;] _s _c? )+ ;

name   ::= [A-Za-z_] [A-Za-z0-9_]* ;
number ::= [0-9]+ ( "." [0-9]+ )? ;
names  ::= name ( _s "," _s name )* ;

# ---- a system: a declaration in four parts, or a derivation from another system ----
system     ::= "system" _sp name _s ( body | "=" _s derivation ) ;
body       ::= "{" _ ( _item ( _sep _item )* _ )? "}" ;
_item      ::= state | param | noise | unknown | change | structure | constraint | difference ;
derivation ::= dop _sp name ;
dop        ::= "expand" | "reverse" | "lift" | "vary" | "legendre" | "unravel" ;

# ---- the carrier, its parameters, its noise and what is not yet known ----
state     ::= "state" _sp decls _s ":" _s space ( _s "," _s qualifier )* ;
decls     ::= decl ( _s "," _s decl )* ;
decl      ::= name ( "(" _s names _s ")" )? ;
space     ::= "words" _sp "over" _s set | name ( "(" _s args _s ")" )? ( "^" ( number | name ) )? ;
set       ::= "{" _s names _s "}" ;
qualifier ::= name ( _sp name )* _sp "in" _sp domain | "start" _sp word ;
domain    ::= interval | "group" _sp name | eapp | name ;
interval  ::= "[" _s expr _s "," _s expr _s "]" ;

param   ::= "param" _sp binding ( _s "," _s binding )* ;
binding ::= decl _s "=" _s expr ;
noise   ::= "noise" _sp target _s ( ":" | "~" ) _s expr ;
target  ::= name ( "(" _s args _s ")" )? ;
unknown ::= "unknown" _sp names _s ":" _s ( "number" | "function" | "sparse" _sp expr ) ;

# ---- Change: the generators and how each acts ----
change      ::= "change" _s "{" _ ( _citem ( _sep _citem )* _ )? "}" ;
_citem      ::= action | hamiltonian | jump | rule | equation ;
action      ::= "action" _sp name _s "=" _s expr ;
hamiltonian ::= "hamiltonian" _sp name _s "=" _s expr ;
jump        ::= "jump" _sp name _s "=" _s expr ;
rule        ::= name _s ":" _s word _s "->" _s word ( _s "@" _s expr )? ;
word        ::= name ( _sp name )* ;
equation    ::= lhs _s "=" _s expr ;
lhs         ::= name ( "'"+ | "(" _s args _s ")" )? ;

# ---- Structure: how changes compose into histories, and what a history weighs ----
structure   ::= "structure" _s "{" _ ( _sitem ( _sep _sitem )* _ )? "}" ;
_sitem      ::= time | derivations | histories | weights | calculus ;
time        ::= "time" _sp name _sp "in" _sp domain ;
derivations ::= "derivations" ;
histories   ::= "histories" _sp ( "stationary" _sp name | "weight" _sp expr ) ;
weights     ::= "weights" _sp ( "boolean" | "probability" | "real" | "complex" | "operators" ) ;
calculus    ::= "calculus" _sp ( "ito" | "stratonovich" ) ;

# ---- Constraint: what every change respects ----
constraint ::= "constraint" _s "{" _ ( _kitem ( _sep _kitem )* _ )? "}" ;
_kitem     ::= background | invariance | conserved | equation ;
background ::= name ( _sp name )* _sp "on" _sp domain ;
invariance ::= "invariant" _sp "under" _sp expr ;
conserved  ::= "conserves" _sp expr ;

# ---- Difference: what is told apart ----
difference ::= "difference" _s "{" _ ( _ditem ( _sep _ditem )* _ )? "}" ;
_ditem     ::= observe | upto ;
observe    ::= "observe" _sp args ;
upto       ::= "up" _sp "to" _sp expr ;

# ---- terms: products are written with '*'; a line may continue before an operator ----
expr    ::= _eadd ;
_eadd   ::= _emul ( _ addop _s _emul )* ;
addop   ::= "+" | "-" ;
_emul   ::= _eunary ( _ mulop _s _eunary )* ;
mulop   ::= "*" | "/" ;
_eunary ::= neg | _epow ;
neg     ::= "-" _s _eunary ;
_epow   ::= power | _eatom ;
power   ::= _eatom "^" _eunary ;
_eatom  ::= number | hole | eapp | evar | paren ;
paren   ::= "(" _ expr _ ")" ;
eapp    ::= name "(" _s ( args _s )? ")" ;
evar    ::= name "'"* ;
hole    ::= "?" name ( "(" _s args _s ")" )? ;
args    ::= expr ( _s "," _s expr )* ;
```

### 9.3 What Was Checked

The grammar was compiled by morphe's reader on 2026-11-12 (67 rules, 52 classes of bytes) and given five scripts, with a small program outside the repository.

- **Accepted:** thirteen systems in four scripts. The three tests of §8 with their variants (the stochastic grammar, the expansion, a field on a lattice, the reversal, a map with Lévy noise); and five ordinary cases: Lorenz's flow with an invariance and an observation, a pendulum by its action with a conserved energy, a pendulum with an unknown potential, a damped qubit, and a lift.
- **Refused:** a product written by juxtaposition, `sigma (y - x)`, at the column where it goes wrong, with what may follow.

*Caveat.* This checks syntax and nothing else. No system here has a meaning yet: no term is typed, no dimension checked, no derivation carried out. Earley's parser accepts ambiguous grammars, and the number of parses was not counted.

## 10. Records, Access and Certificates

A system is known at a level of access: a presentation (a declaration of §5), an oracle (something one can drive and read), or a record (finite data through an observation). Each tool states the access it needs, and a system that cannot give it refuses with a reason, as physis's `fields` do.

An answer drawn from a record is not a value but the set of values over every reconstruction the record admits. The user's draft on braids states it: for a feature $\Phi$, a record $D$ and assumptions $A$,

$$\mathcal{I}_\Phi(D; A) = \{\, \Phi(\Gamma) : \Gamma \in \mathcal{R}(D; A) \,\},$$

certified when it is a single value, and otherwise reported as it is. A certificate keeps its assumptions and its lineage: retained by refinement, transported, withdrawn, certified again. This is proposed as the form of every answer from a record in the work, not only of braids. The parse forest of §8.1 is the same object.

## 11. What Is Agreed

- **The foundation.** An eleventh, on calx, morphe, physis and tyche, named `kinesis`; the user creates its repository. physis's generic analysis (Lyapunov spectra, Floquet multipliers, continuation) moves up into it, as panels moved from nomos to tyche. The rule: physis writes generators; kinesis studies what a generator does over time and recovers generators from records.
- **Randomness** in tyche (§7.3).
- **Braids.** The exact structures in morphe; their reading from trajectories, on the user's draft, in kinesis; lifted to operators as in §4.
- **The work.** *Dynamics Lab*, the name the user chose, project-based on the visualization layer's `workspace` (ADR 0093), on port 9090. CLAUDE.md's rule against the word "laboratory" needs this exception written when the work is founded.
- **Learning.** The cheap methods first, in C++; torch on the CPU only for unknown functions.
- **The order of building** waits for the survey.

## 12. Open Questions

1. **The weighted Constraint and Difference** (§3.2), and whether open quantum systems fit the table of weights or only the lift (§6).
2. **The home of `histories`.** Whether the reading of an action, stationary or as a weight, belongs to Structure, as written here, or to Change.
3. **Not yet in the grammar:**
   - events with guards and resets (hybrid systems: `when v >= 1 : v -> 0`);
   - changes that add or remove participants;
   - the composition of systems (coupling, driving, copies over a graph);
   - physical dimensions;
   - the variables an integral is over;
   - the library of a sparse unknown;
   - oracles and records as declarations.
4. **Locality.** How the check that a law is local on its background is carried out on a term.
5. **The meaning of a term.** One term is to be read on numbers, on calx's jets, as a formula, and for torch. The intermediate representation that serves these readings is not designed; calx's stack and physical computing's optional representation (ADR 0076) are its neighbours.
6. **The relation to kategoria's scripts.** Whether claims about a system ("this has a Hopf point near $r = 24.74$") are statements of this language, answered with verdicts, or tools of the work.
7. **Kept for another note:** the braid design on the user's draft, with three sharpenings offered in the discussion (a certificate by interval and by pair, $|r_j| + |r_{j+1}| > 2Vh$; the endpoints under noise; Fox and Neuwirth's cells as the convention for endpoints), each to be proved and checked there.
