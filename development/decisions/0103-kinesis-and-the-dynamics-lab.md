# 0103. kinesis, a Foundation for Dynamical Systems, and the Dynamics Lab

- **Status:** Accepted, provisionally (2026-11-13).
- **Date:** 2026-11-13
- **Decided by:**
  - **The request.** The user: "We will begin the next ambitious project. Dynamical System one. For application, make be dynamics lab?", to "analyze, visualize various dynamical system (both classical, and (open) quantum)", to "from data, induce structured dynamical representation", for systems "structured, and non-structure", with "topology, algebra, and geometry analysis, like braid group theory", "project-based".
  - **In the discussion.** An eleventh foundation ("yes"); the work named "Lab"; "braid group is sehr important", built early on the user's own draft; "we even need create a 'domain specific langauge' that can define dynamical systems", which must not be "mainly for PDE" and must take "field theory one, like action S's representation"; the user's four primitives ("change, structure, constaint, and difference can be 4 primitives that construct a dynamical system … we need consider such 4 primitive/elements in design"); operators "in general gravity sense. something be lift into operator, and non-commutative geometric sense"; the first test of the language ("a generative grammar … background-independent dynamics, like utilize GFT … a random model, like diffusion model"); a survey first, to gather the models of many fields "as predefined models" for matching; torch allowed, on the CPU only; the name kinesis, whose repository the user creates; the next free port, then 9090.
  - **To build.** "make it professional, delicate, and elegant, extenable. Even visualization, i.e., topological features, criticality detection (need survey papers), dynamics landscape. simulations, etc. should be done. And even heterogenous dynamics system can connected and simulate, co-evaluation, etc. And from data to train models, match model, etc."
  - **Case studies, the next morning.** "Can you also try a fluid dynamics and investigate turbalence in the lab?"; "several integrated heteogenous dynamics examples, this will important for us to investigate co-existence and ecology, ocean science … system 1 has fluid dynamics, system 2 has chemistry dynamics, system 3 has some dynamics produced by a spiece (like a fish) they combined. They co-evoluated"; and whether "a structured field theory based method" with an action "S_{int} will be easier? I not sure".
  - **The design.** Claude, here, from the idea of 2026-11-12 ([*A language of dynamical systems*](../ideas/2026-11-12-a-language-of-dynamical-systems.md)) and the survey (`research/literature/reports/Dynamical systems models and braids.md`).

## 1. Context

Dynamical systems were in the infrastructure in pieces: calx's integrators, physis's spectra of exponents and its laws of physics, morphe's grammars, tyche's time series, and each domain's own dynamics (spiking networks, walkers, reaction networks, substrates that compute). Nothing said what a dynamical system is in a way that serves a flow, a grammar, a field theory, a stochastic model and an open quantum system alike; and nothing went from a record to a law.

The survey found that the tools of the field divide along the kinds of system: one for differential equations, another for stochastic ones, another for quantum ones, others for grammars and for data. None has one declaration for all, and in the literature on braids of trajectories it found no statement of what a sampled record certifies, apart from one recent treatment (Guillemot and Lairez 2026) and the user's own draft.

Four questions had to be answered.

- **A foundation?** Yes. What a law does over time, and the recovery of a law from a record, name no object in nature (CLAUDE.md §1), and every domain whose objects change may use them.
- **Where against physis?** physis writes generators, the laws of physics; the new library studies a generator of any kind and recovers generators from records. A grammar is not physics, and a pendulum's law is not the new library's.
- **What is a system, so that one notation serves every kind?** The user's four primitives: Change, Structure, Constraint and Difference.
- **What may be said from data?** What the user's draft says of braids: the set of values over every history a record admits under stated assumptions, certified only when it is one value.

## 2. Decision

### 2.1 Four Places

| What | Where |
|---|---|
| The mathematics: the language, what a declared law does over time, what a record identifies, laws from records | an eleventh foundation, **kinesis** (`foundations/kinesis`, a repository of its own), on calx, morphe, physis and tyche |
| What is algebra and what is physics beneath it | morphe's braid groups and Laurent polynomials (its decision 0014); physis's open quantum systems of finite dimension (its decision 0028) |
| The drawings: charts, maps of basins and landscapes, eigenvalues, braids, the forms of rules, paths and strands in the round | the visualization layer, `visualization/js/dynamics` |
| The composition for any work's engine | the API, `api/cpp/science/dynamics.hpp` |
| The place where a person writes, runs and looks | a work, the **Dynamics Lab** (`applications/dynamics-lab`, port 9110) |

### 2.2 The Language

A system is declared by its carrier (`state`, `param`, `noise`, `unknown`, `input`) and the four primitives: `change` (equations, an action, a Hamiltonian and jumps, rules, events), `structure` (the time, derivations, stationary or weighted histories, the semiring of weights, the stochastic calculus), `constraint` (a background geometry, an invariance, a conserved quantity) and `difference` (the observations). A system may be made of parts wired at their inputs, or derived from another (`system B = vary A`).

The grammar is a text in morphe's BNF, read by Earley's parser: a script that is wrong is answered with its line, its column and what may follow. Every expression is a term with exact symbolic derivatives, so that a Jacobian, the Euler–Lagrange equations of an action and the conversion between stochastic calculi are derived and not approximated. A term may hold an unknown; a system with unknowns is a template.

### 2.3 kinesis

Nine modules (its decisions 0002 to 0004; its theory note `docs/theory.md`):

- `kernel`: the levels at which a system is known (a presentation, an oracle, a record); a refusal with its reason; the values a record identifies, and how a certificate stands to the one before it.
- `terms`, `language`: terms and their derivatives; the grammar, the reader and the writer.
- `systems`: flows and maps with events; stochastic differentials in Itô's or Stratonovich's calculus, by Euler–Maruyama and Milstein; actions varied into flows; rewriting systems and the histories of their words; open quantum systems.
- `analysis`: Lyapunov's exponents with the error of a finite time, equilibria, sections, bifurcation diagrams, basins; the averages of what a system observes, with their errors, and the power law by which they scale; signs of criticality (slowing down against red-noise surrogates, fluctuation analysis, avalanches, the branching ratio); landscapes (a potential, the stationary landscape, Kramers's time); the transfer operator on a partition.
- `braids`: the braid of a record of planar trajectories; a certificate under a bound on speed and an error of observation; the windings a record admits; topological entropy between Burau's bound and an estimate.
- `induction`: templates filled from records, unknowns inside actions among them; sparse discovery and its weak form; stochastic templates; candidates ranked on a part kept aside; linear operators; a forecaster with no law.
- `coupling`: flows merged into one declaration, exactly; parts given by actions joined by an interaction action; parts of different forms co-simulated.
- `catalogue`: thirty models from the literature as declarations, a shell model of turbulence among them, each with its source; plain ones as templates for a record, in every order of its components.

### 2.4 The Lift to Operators, and the Braids

The lift the user means is stated and proved for finite sets in kinesis's theory note (§2): a change becomes the operator $f \mapsto f \circ \alpha(m)$ on the algebra of observables, Constraint becomes an invariant subalgebra, and Difference is empty exactly when the operator maps that subalgebra onto itself. Three lifts are computed: Ulam's matrix of a map, the generator of an open quantum system, and a section of Koopman's operator fitted to a record. A braid lifted to an operator is its Burau matrix, whose spectrum bounds the braid's entropy.

The braids are built on the user's draft, *Rethinking trajectory-based credibility under symbol inflation*: its identified sets, its dispositions of a certificate and the numbers of its worked example are reproduced. One result was derived for the library: a certificate by interval and by pair, $|r_k| + |r_{k+1}| - (V_i + V_j)h - 8\eta > 0$, which the draft's corollary implies and which is sharp for two points (the theory note, Proposition 7.1).

### 2.5 The Other Layers

- **morphe.** `algebra/braid.hpp`: the word problem by Dehornoy's handle reduction, checked against Artin's faithful action on the free group; Dynnikov's coordinates; Burau's representation and Alexander polynomials. `arithmetic/laurent.hpp`.
- **physis.** `quantum/lindblad.hpp`: the generator of Gorini, Kossakowski, Sudarshan and Lindblad for any finite dimension as a real matrix, its stationary state, spectrum, gap and propagator, and quantum jumps.
- **The API.** `science/dynamics.hpp`: a request holds a whole script and names an operation; nothing is read from a file or kept between requests.
- **The visualization layer.** `dynamics/`: charts as markup that needs no page, so the checks read them in Node; views in the round that take three.js as an argument. Its ten inks for lines were chosen by a search so that the first few are apart in every colour vision and each is legible on paper and by night.
- **The work.** A project is a script, its records and its studies. The script is at the left with what it declares beneath it; the studies are in the middle, each a tool that was run with what it was asked, marked when the script has changed since; the system selected and its tools are at the right. Nineteen tools, each declared, so that a tool joins by being added to a list. Sixteen examples, and the catalogue's models to add to a script. Its engine is kept alive by its server.

### 2.6 Checks

kinesis 433, morphe's braids 39 and physis's open quantum systems 23, against closed forms, published values, exact solutions and laws that the checks declare and then recover; the visualization layer's dynamics 28; the Dynamics Lab 80, every example read and every study of it carried out through its server and answered as it is known to be. The page was driven in headless Chrome (not in the suite).

### 2.7 Four Case Studies

At the user's request, the morning after ([the record](../../research/experiments/2026-11-13-turbulence-and-co-evolution/README.md)); each is an example of the Lab, and each number is asked of its engine.

- **Turbulence.** A shell model of a turbulent fluid (Sabra), declared shell by shell by the catalogue: a cascade, not a discretised fluid. What is exact of it is held: its two invariants as identities of its field, and in the steady state the power put in, the power dissipated and the flux through every shell of the cascade equal (4.96, 4.95, 4.96, 4.95). What is measured comes with its error: the energy of the shells falls as k^−0.78 ± 0.02 over eight octaves, steeper than Kolmogorov's 2/3, and the flatness grows down the cascade, which is intermittency.
- **Coupling by an action.** The user's question, answered: an interaction action is the form of a coupling that is reciprocal, a force. Two pendulums joined so are one system whose energy is kept and passes between them. A coupling that is one-way cannot be a term of an action of the two, and is a wire: the first keeps its energy and the second is driven. Couplings between living things are mostly wires. The field-theoretic form that holds both, the response-field action of Martin, Siggia and Rose, is recorded in kinesis and not built.
- **A fluid, its chemistry and a species.** Convection, nutrient and plankton, and fish, each with a law of its own, joined by four wires into one flow. With fish the plankton stands at exactly death/gain; without them there is 4.4 times the plankton, more heating and half as much overturning again: in this model the fish calm the convection. The steady state is lost where the heating passes the fluid's own threshold, though a sweep shows chaos only later. A conceptual model, and said to be.
- **Behaviours.** Two callers coupled unequally keep one pace, a mean of their own weighted by how much the other listens: the one that listens less leads, in closed form and to rounding. A third animal, excitable, answers more when they are in step, and the pace they keep is exactly theirs and its.

## 3. Alternatives Considered

- **A module of physis.** Its `dynamics` holds flows. Set aside with the user: grammars, stochastic models and inference from data are not physics, and a language with its reader is not a module of laws.
- **A language of equations.** The first sketch, turned away by the user as "mainly for PDE": a field theory is given by its action, a grammar by its rules.
- **An existing language or library.** The survey found none with one declaration for a grammar, an action and a stochastic differential, and none whose answers from data carry their assumptions.
- **The Lab on the workspace of Market Studio** (ADR 0093), as first planned. A project here is a script and studies of it, not tables bound to roles and slices of space and time; the workspace's document does not fit, and a document of the Lab's own (`noetarium.dynamics/1`) was written. A record of a project may still be given to a workspace as a table later.
- **Automatic differentiation in place of terms.** It gives numbers, not an equation to show, to write out or to vary.
- **The braid groups in kinesis.** They are algebra, and serve knots and mapping classes as well.
- **A studio in the manner of the Category Studio**, a script with its answers in order. A system is studied by many tools with many parameters, and a study is kept and repeated; a bench of declared tools serves that better than statements of a script.

## 4. Consequences

- **The name.** The work is the *Dynamics Lab*, the name the user chose. CLAUDE.md's rule against the word "laboratory" keeps its sense (no part of the infrastructure is called one); the work's proper name is the exception, written there.
- **The three tests of the language.** A generative grammar is declared and run. A random model is declared and run; its reversal, which is the generation of a diffusion model, is read and not carried out. A group field theory is declared with no background and read; it is not run. The language passes all three as a notation, and the first as a computation.
- **Read and not run.** The derivations `expand`, `reverse`, `lift`, `legendre` and `unravel`; statistical fields; unknown functions and their learning, for which torch was allowed and is not yet used. Each is answered with a note that says so.
- **Not built:** oracles as declarations (a physical or simulated system driven as a black box); samplers of laws, agreed to go to tyche; the weighted forms of Constraint and Difference, and a check that a declared invariance holds; Garside's normal form and conjugacy; braids in which strands arrive and leave, beyond cutting a record into segments; physical dimensions of terms; projects with several people; plug-ins of the Lab.
- **Deferred, and to be proposed as a change of its own.** The move of physis's general analysis (its spectra of exponents, Floquet multipliers and continuation) into kinesis, which the user agreed to: it moves code and checks between two repositories, and kinesis calls physis's meanwhile.
- **No fluid is discretised.** The turbulence studied is a shell model's: a cascade with no space. Turbulence in a discretised fluid, and the coupling of fields (a concentration carried by a flow, animals moving through both), wait for fields that run.
- **Systems are small.** Terms are evaluated by a stack program; a quantum state has dimension at most 64; Euler–Lagrange equations are solved as terms for at most three coordinates; no partial differential equation is discretised here.
- **Found while building.** A crossing that falls exactly on a sample needs one rule at both ends of every interval, or an exchange is lost or invented. Milstein's condition for diagonal noise is on every driven component, whichever source is declared first. A forecaster with delays has modes of its own, which too small a ridge lets grow: the ridge is to be chosen by the horizon on a part kept aside. Points of a grid that lie on the boundary between two basins end at a saddle or creep toward one, and are named so, not counted as attractors.
- **The port.** The user agreed to 9090, the next free port when it was proposed. The Urban Studio (ADR 0102) took 9090 before this record was written, so the Lab is on 9110, the next in the series.
- **Provisional:** the Lab's colour (an iris with a rose minor), until the user chooses; kinesis's repository was created by the user on 2026-11-13, and it is attached as a submodule.
- **The domains keep their dynamics.** Where a domain's method is general, it is proposed to kinesis; where a domain wants its models read, run and fitted in one notation, it declares them in the language.
