---
status: seed
---

# Evolutionary Allocation of Heterogeneous Resources: A Design and Its Plan

*Status: seed (2026-11-20). A design written for the user's review, with a staged plan of implementation. Stages 1 to 5 are built (2026-11-20; heuresis's decisions 0008–0010, morphe's 0021, kategoria's 0004, ADR 0137; the survey's Part X says what they found); Stages 6–8 are not; each stage, when built, is recorded in an ADR and in the decisions of the libraries it touches. Its survey is [Evolutionary heterogeneous resource allocation](../../research/literature/reports/Evolutionary%20heterogeneous%20resource%20allocation.md), nine Parts, cited below as "Part III §2" and so on.*

## 1. The Occasion

After the allocation domain (ADR 0136), the user asked whether a classical evolutionary algorithm for resource allocation could be built, holding that:

- allocation is a combinatorial problem in which the trajectory matters: "allocate A first then B not equal to B then A";
- the fitness function is the hardest part and can be many things: "a economics value circulation simulation or even ABM, a LLM evaluator, a machine learning model, etc.";
- the aim is "a reusable/extensible universal envoluntionary resource allocation framework", in which "generality is the most important thing";
- resource categories should be definable and "formal verificable (by category theory foundation)", each with "the representative algebraic space of the resource (like some resource can represent on R^N, some are represent on a set, some may be some other uncommon manifolds, etc)".

They asked for a survey, and widened it twice: to how agent simulations give insight into economic circulation (agents' relations, states and behaviours under learned dynamics, agents embedded in manifolds or dynamical systems), and to how classical and quantum economic dynamical models are learned from observed data. They asked for care over speed, and for a well-organised implementation plan.

## 2. What the Survey Settles

1. **A space-independent definition of variation exists, given a metric.** Mutation is a draw from a ball of the metric, crossover a draw from the segment between parents (Moraglio). On a product of typed parts, crossing each part geometrically is geometric for the summed metric, even when the parts are coupled. Each resource's space can therefore bring its own operators while the whole stays principled; parts must be scaled, or a wide one dominates (Part III §1, §5).
2. **Encodings of two kinds should both be offered and compared.** A direct encoding with repair and local improvement came closest to the bound on the multidimensional knapsack (0.33% on average), keys with a decoder are the generic route (0.49–0.63%), and the difference is the price of generality, which a framework can measure (Part I §2). Duplicates by phenotype should be removed by default (Part I §2).
3. **Order is a word of grants acting on a state.** Order-independence is exactly the commutativity of the grants' actions; grants that commute only partly give a trace monoid, whose normal forms remove the redundancy of many orders giving one allocation and let each distinct allocation be evaluated once (Part II, Part IV §3). Which feature of an order matters (precedence, adjacency, position) decides which operators work: on an allocation that depletes stock, operators that keep relative order won, and the travelling salesman's best operator was among the worst (Part II §2).
4. **A resource type is a symmetric monoidal preorder.** Its objects are resources, its morphisms the conversions allowed for free; its shadow is an ordered commutative monoid in which a ≽ b when a converts to b (Coecke, Fritz and Spekkens; Fritz). Divisibility is the action of a semiring (ℕ, granular, ℚ≥0, ℝ≥0, ℤ for balances, max or ∨ for qualities); distinct items need a validity predicate that makes combination partial (Iris); chores fit because positivity is not assumed (Part IV §1).
5. **Verdicts must be scoped.** Laws hold by construction from verified primitives (products preserve them), exhaustively on a finite model (for that model only), on generators for homomorphisms (prices, conservation), by bounded search with a witness or a violated monotone for convertibility, and on samples only as evidence. Refutations are definitive (Part IV §4).
6. **An evaluation is a record, not a number.** Uncertainty, replicates, cost, fidelity, the random stream, the evaluator and its version, and a standing (verified, computed, estimated, proposed) must travel with every score; language models generate well and judge poorly, so their scores enter only as tie-breakers or descriptors, pairwise with the order swapped and audited; costly agent-based evaluators need resampling where selection is in doubt and surrogates checked against the true model; equity stays its own objective (Part V).
7. **Agent simulations show what accounting cannot, and must conserve.** A production-network model forecast the UK's 2020 fall in output (21.5% against 22.1% measured); circulation lives in cycles (Sarafu, Sardex, Fedwire); stock–flow consistency must be built in; learned agents game rules and are more homogeneous than people (Part VI). No agent model has been validated against measured circulation statistics.
8. **Learned and geometric representations help, within limits.** Latent positions are identified only up to rotations; density models (Achdou et al.) suit many interchangeable agents; learned simulators are faithful to their simulators but untested on counterfactuals (Part VII). No credible economic law has yet been discovered from real aggregate data by sparse discovery or Koopman methods; nonlinearity is real, low-dimensional chaos is not established; nonlinear drifts can be artefacts of finite samples (Part VIII). Quantum-like models have one genuine, not exclusive, success (question-order effects); order effects in allocation are classical, and quantum computing shows no advantage for finance on real data (Part IX).

## 3. The Design

### 3.1 Four Layers, and Where Each Lives

| Layer | What it is | Where it lives | Why there |
|---|---|---|---|
| Spaces | Typed representation spaces with their operators, composed by constructors, held to laws | heuresis, a module `spaces` | General search; names no object. heuresis may use physis's Lie groups and tyche's Aitchison geometry, which its dependencies allow |
| Order | Words of grants acting on states; independence; trace normal forms; decoders; operators by the feature they keep; sequences or policies | heuresis, a module `sequential`, on morphe's `traces` (extended with canonical forms) | General; morphe holds traces already |
| Resources | Resource types as symmetric monoidal preorders; their laws verified with scoped verdicts; allocations and conversions as morphisms; the order-dependence of a finite system decided | kategoria, a module `resources` | Categorical verification is kategoria's; it depends on morphe and calx only |
| Evaluation | The assessment record; replicates, races, cascades, caching; tiers before Pareto | heuresis, its planned module `noise`, renamed `assessment` | Selection under uncertainty is search's |
| Composition | Heterogeneous allocation problems: resource types, recipients, a grant system, a genotype assembled from the resources' spaces, evaluators; the search; its verdicts and sealed record | the allocation domain, a module `evolution` | It names what is allocated; it may declare heuresis and kategoria beside its present foundations |
| Economic evaluators | A ledger that conserves; circulation read from a run; agents; density laws; learned laws with baselines | the economics domain (`ledger`), nomos (`agents`), kinesis (`balance`, learned laws) | Each where its subject is |
| Edges | The API's composition with ask–tell over a saved state; evaluators that call models and people; drawings; a work | `api/`, `visualization/`, the Urban Studio | They touch the world |

No foundation's rule of dependence changes, and no new foundation or domain is needed. The allocation domain's `module.json` gains heuresis and kategoria among its foundations.

### 3.2 Spaces

**The contract of a space** (Part III, design §1). A space over values of a type T supplies:

- *required:* `sample(stream, size)`; `mutate(x, scale, stream)` within a ball of the space's metric; `distance(x, y)` with a declared natural unit; `valid(x)`;
- *optional:* `recombine(parents, stream)` with its arity and whether it is geometric, pure and respectful; `repair(x)`, idempotent and the identity on valid values; a `chart` for continuous and curved spaces (`log`, `retract`, `transport`, `basis`, `dim`, `trust_radius`); `embed`/`unembed` to a standardised ℝᵏ (bijective, surjective with a decoder, or stochastic, flagged); a floor on the probability of leaving a discrete value; `size` and `shrink`.

An algorithm that needs a missing capability refuses the space by name.

**Values.** A genotype is a tree of values: a real vector, an integer vector, a category, a permutation, a set or multiset of indices, a sequence of values, a record of named values, a choice (a tag and its payload), a point of a manifold in its representation. One value type serves every space, so that heuresis's evolutionary loop, which takes any copyable genotype, runs on all of them.

**Base spaces** (Part III, design §3): real boxes; ordinal integers; categories; ordered categories; the simplex in Aitchison's geometry (ilr coordinates, tyche's `compositions`); spheres and SO(3) (physis's rotations), by tangent steps and retraction within the trust radius; permutations under a stated metric (swap, Hamming, Kendall); sets and multisets under the symmetric difference; variable-length sequences under the edit distance. Graphs are heuresis's planned `graphs`, later.

**Constructors** (Part III, design §4): product (named; distance the weighted sum of scaled parts; recombination part by part, geometric by Moraglio's product theorem); choice (a tagged union; a metric across branches; a declared policy for inactive payloads); sequence; set and multiset; constrained subspace (a predicate with a repair, else rejection counted); dependent product (shape of one part depends on another; joint recombination required). Recursive types are heuresis's `programs`, which exist.

**Derived generically:** first populations by Boltzmann samplers from the constructor tree, uniform among values of a size; product and choice operators; the standardised embedding; per-part step sizes; shrinkers; the laws below. Specialised spaces may override any of these.

**Laws, held for every space** (Part III, design §2): purity (`recombine(x, x) = x`); children on the segment (d(x, c) + d(c, y) = d(x, y)); a mutation within its ball; repair idempotent and the identity on valid values; `retract_x(0) = x` and `log_x ∘ retract_x ≈ id` within the trust radius; `unembed ∘ embed = id` when bijective; the metric axioms on sampled triples. The checks are written once and run on every space, built-in or a person's; a user's operator that fails one is reported, not refused, since non-geometric operators can still work.

**Diagnostics of a representation** (Part I §1, §5): Raidl and Gottlieb's mutation innovation, crossover innovation and crossover loss, the share of mutations that leave the phenotype unchanged, and the fitness–distance correlation, measured on samples through any decoder. They are run before a search commits to an encoding.

### 3.3 Resource Types

**A resource type** (Part IV, design (a)) is a finite product of components, combined componentwise. Each component declares:

- its *kinds* (the labels of goods, or the places of a Petri net);
- its *quantity semiring*, which fixes divisibility: ℕ (identical units), (1/q)ℕ (granular), ℚ≥0 or ℝ≥0 (divisible), ℤ or ℚ (signed balances and debts), Boolean ∨ or max (qualities, copyable);
- its *combination*: addition for quantities, a join for qualities;
- its *order*: generated by declared conversions, with free disposal declared or refused (refused for chores and waste);
- its *validity*: an optional predicate closed under taking part of a bundle, for capacities, exclusivity and distinct items (a labelled item held at most once);
- its *declared properties*: cancellative, catalysis-free, splittable (Riesz decomposition), quantity-like or quality-like, waste-free, conservation laws (homomorphisms to ℤ preserved by every conversion);
- its *carrier space*: the heuresis space in which amounts of it are represented and varied (ℝᴺ, an integer lattice, a simplex scaled by the pool, sets of items, a manifold for attributes such as an orientation).

The algebra and the space are declared apart: the algebra says how amounts combine and compare, the space how a search moves among them. Attributes that do not combine (a direction, a time slot) are spaces without an algebra.

**Verification** (Part IV, design (c)). Each law is assigned a method, and each verdict says what it means:

| Law | Method | The verdict means |
|---|---|---|
| Monoid laws of the primitive semirings | By construction: primitives verified once, products preserve the laws (Birkhoff) | Proof for every type built this way, naming the closure theorem |
| The same laws on a finite carrier or a truncation (with partial addition) | Exhaustively, the carrier's preorder as a category and combination as a functor P×P → P checked by kategoria | Proof for that finite model only |
| Order compatible with combination | By construction for generated orders; by the functor check on finite carriers | Proof |
| Conservation laws, prices, monotones | On generators and on each declared conversion | Proof for every element |
| Splittability, refinement | On finite fragments; by bounded search | A fragment's verdict; a counterexample refutes |
| Convertibility a ≽ b | Bounded search for a witness; a monotone or invariant for impossibility | Holds with its witness, fails with its monotone, or undecided (the problem is Ackermann-complete in general) |
| Validity closed under parts; frame-preserving updates | Exhaustively on a finite resource algebra | Proof for that model |
| Laws over ℝ | Exact rationals or certificates where possible; else samples | Samples are evidence, never proof; no law is checked in floating point |
| Equivariance of an operator under renaming items or recipients | Naturality squares on finite instances | Proof for the instance; samples at scale |

**Allocations and conversions as morphisms** (Part IV, design (b)). An allocation of a pool p to recipients 1..n is a morphism p → r₁ ⊗ … ⊗ rₙ, lawful when it conserves (Σ rᵢ ≃ p, or ≤ p where disposal is free), each rᵢ and the whole are valid, and regrouping is coherent. A conversion is a morphism built from the declared ones.

### 3.4 Order

**A grant system** (Part II, design §1) is a state type X, an alphabet of grants G, a partial action δ : G × X ⇀ X (or a random kernel), an initial state, and a value read from the final state or along the path. An allocation is a word in G*. A set-valued allocation is the special case in which all grants commute.

**Independence as data.** Each grant declares a footprint: the parts of the state it reads and writes (morphe's `traces`). Two grants are independent when neither writes what the other reads or writes. Independence may also be estimated by commutator tests (δ_g δ_h x against δ_h δ_g x) on sampled reachable states, and, for a finite system, decided by kategoria. From it the framework derives:

- whether order matters at all (all grants independent: no permutation search; sets and counts suffice);
- the trace of each word, its causal order and its canonical form (the lexicographically least linearisation, to be added to morphe's `traces`);
- caching by canonical form, so that orders equal up to independent exchanges are evaluated once, and the redundancy of a search measured.

**Genotypes and decoders** (Part II, design §3; Part I, design §2–4): words or permutations with a greedy feasible decoder (the serial scheme: walk the word, grant each item when the state allows it, else defer or skip by a declared rule) and a time-stepped variant, each stating which allocations it can reach; random keys (and the biased random-key GA) on the same decoders; direct words with repair for small spaces. A decoder is pluggable and declares its reach, since a decoder can exclude every optimum.

**Operators by the feature they keep** (Part II, design §4): precedence (order-based crossover, position-based crossover, precedence-preserving crossover, insertion and scramble mutations), adjacency (edge recombination, order crossover, reversal), position (cycle crossover, PMX, swap). heuresis holds six of these; the others are added. A feature probe correlates differences of fitness with each feature's distance on sampled pairs, and adaptive operator selection, which heuresis's loop already has, credits the portfolio.

**Search distributions** behind one interface: Plackett–Luce (which heuresis has; the natural law of "who first"), Mallows under Kendall, Cayley and Hamming, random-key Gaussians.

**A sequence, or a policy** (Part II, design §7). A sequence by default when the system is deterministic and known; a policy when outcomes are random and observed, claims arrive online, or a published rule is wanted. Policies are priority functions over declared features of the state, evolved as heuresis's programs, or proposed by a language model and filtered by the evaluator. An estimate of the adaptivity gap (the best common sequence against the best sequence for each scenario) says which is warranted.

**Mechanisms' orders as genotypes** (Part II, design §9): picking sequences, serial dictatorships and the precedence of reserve categories are searched as genotypes whose evaluation runs the mechanism (the allocation domain's rounds, nomos's reserves) on a distribution of claims or preferences.

### 3.5 Evaluators

**The assessment record** (Part V, design §1), beside heuresis's present `Evaluation`, which it maps to for selection: metrics, each with a name, a role (objective, constraint, descriptor, report), a value, its uncertainty (0 deterministic, absent unknown), the number of replicates and optionally the replicates and per-unit scores; cost; status (ok, failed, refused, abstained) with a reason; and provenance: the evaluator and its version, its kind (closed form, simulation, agent model, surrogate, language model, person), the fidelity, the random stream's key, the digest of the input, and a standing (verified, computed, estimated, proposed).

**Policies** (Part V, design §2–4): replicates on demand, beginning from a power calculation for a declared smallest difference and spent only where selection is in doubt; races with Hoeffding or empirical Bernstein bounds where a guarantee is wanted, each run stating which guarantee its selections carry; common random numbers by heuresis's keyed draws, kept only where they lower the variance of differences; cascades from cheap to costly evaluators; surrogates under model management, their agreement with the true evaluator tracked on every re-evaluated point; caching by (input digest, evaluator, version, fidelity, stream), append-only and sealable in archeion's journal.

**Combination without one score** (Part V, design §7): failures last; then hard constraints from verified evaluators by Deb's rule; then Pareto dominance over verified and computed objectives; estimated and proposed metrics only as tie-breakers, descriptors, or a second front shown beside the first; equity as objectives or declared constraints read from per-unit outcomes (nomos's `welfare` principles); quality-diversity archives over descriptors from any evaluator; every winner re-evaluated at the highest fidelity on fresh streams before it is reported.

**Kinds of evaluator, and what each can establish:**

| Evaluator | Built on | Establishes | Cannot establish |
|---|---|---|---|
| Closed forms (welfare principles, incidence, rationing outcomes) | nomos, the allocation domain | The declared quantity exactly | That the declaration is apt |
| A ledger that conserves: balance sheets, postings as the only change, money made or destroyed by named acts, an outside node for leakage; circulation read from a run (velocity and holding times, retention by rounds, leakage by where money entered, LM3 as its three-round truncation, the circulating share by Hodge decomposition, cycles) | the economics domain (a new `ledger`), its `circulation` | That a mechanism is sufficient for a pattern; a ranking of allocations under one model | The actual mechanism; behaviour not validated |
| Agents with written, learned or proposed behaviour | nomos's `agents` | The same, with heterogeneity and timing; a conservation verdict every step | That learned or language-model agents stand for people |
| Density laws (a value equation with its density; exchange models) | kinesis's `balance` | The exact effect of order under the law, by composing its maps in both orders | Anything outside its assumptions |
| Learned laws, as an ensemble with the linear benchmark | kinesis's discovery and templates, tyche's filters | Whether a ranking of orders is stable across the laws the record allows; the terms an order effect rests on and how well they are identified | Effects of allocations outside the record's variation (flagged out of support: the Lucas critique made a test) |
| Surrogates | heuresis's `models` | Fast rankings | Anything unchecked against the true evaluator |
| Language models | the API's `science.models` | Proposals; pairwise judgements as descriptors, audited | A verdict; their score is never summed into fitness |
| People | the Urban Studio's page | Preferences and value judgements they are entitled to make, as pairwise comparisons within a fatigue budget (Bradley–Terry) | Factual claims a model or a check can verify |

### 3.6 The Composition

An *allocation problem* in the allocation domain's `evolution` holds: resource types (verified by kategoria); recipients with their attributes; a grant system (grants of each resource to each recipient, their footprints, the state's laws: conservation, validity, capacities, eligibility, precedence); the genotype, assembled from the resources' spaces (shares of a divisible pool on a scaled simplex, units as integer compositions, distinct items as assignments, order as a word, attributes on their manifolds); evaluators; and the search's strategy (heuresis's loop, its archive and judge). It answers: the front and the archive; each kept allocation as a round of grants, drawn as the allocation domain draws rounds; the verdicts under the declared principles (`verdicts`); whether order mattered, and where; the redundancy removed; every law verified and how; the lineage; and the search's record sealed (`procedure`).

### 3.7 What the Framework Must Never Claim

- That an evolved allocation is optimal or just: it is the best found under the declared evaluators and principles, with its standing.
- That a verdict reaches beyond its scope: a finite model's proof is that model's; a sample is evidence.
- That a language model's or an unvalidated agent's score is evidence of quality.
- That a simulation identifies the real mechanism, or that a learned law holds outside the record's variation.
- That a quantum-like model is warranted without the experiment that separates it from classical rivals.

## 4. The Plan

Stages run in dependency order; each is finished, checked, documented and committed on its own, so that the user can review and redirect between stages. Library decisions are numbered when written; the infrastructure's ADR for the framework is 0137, amended by later stages.

### Stage 1. Spaces (heuresis `spaces`)

*Built 2026-11-20 (heuresis's decision 0008; 83 checks). Not built of what is listed: the dependent product, transport and bases on charts, shrinkers.*

- **Build:** the value tree and the space contract; base spaces (real boxes, integers, categories, ordered categories, simplex in ilr coordinates, permutations under stated metrics, sets, multisets, sequences, sphere and SO(3)); constructors (product, choice, sequence, set, constrained subspace); Boltzmann first populations; derived operators; the laws as checks for any space; adapters to `evolution::Operators`; diagnostics of a representation.
- **Checks:** the laws on every built-in space; the product theorem (a product's component-wise crossover geometric under the summed metric) on random products; a choice's metric a metric; Boltzmann uniformity given size, counted exactly on small sizes; retraction identities on SO(3) and the sphere; the simplex's ilr chart bijective; an evolutionary run on a mixed product reaching a known optimum.
- **Documents:** heuresis decision 0008, its README and CLAUDE.md; theory note section.
- **Not yet:** graphs; joint covariance across spaces with no common embedding (an open problem the survey names).

### Stage 2. Order (morphe `traces`, heuresis `sequential`)

*Built 2026-11-20 (morphe's decision 0021, 11 checks; heuresis's decision 0009, 48 checks). Amended while building: two grants commute at a state when either order, each grant skipped where not allowed, reaches the same state and both orders grant both or neither (not when both orders are merely infeasible); the trace is taken of the word granted. Not built: evolved policies, the biased random-key GA; mechanisms run on words are the evaluator's.*

- **Build:** in morphe, the canonical (lexicographically least) linearisation of a trace and equality of words up to independence; in heuresis, grant systems with footprints, commutator tests and estimated independence, canonicalisation and caching, serial and time-stepped decoders with their reach declared, the missing permutation operators (order-based, position-based, precedence-preserving, edge recombination, non-wrapping order crossover), a feature probe, a Mallows sampler, the adaptivity-gap estimator, mechanisms' orders as genotypes.
- **Checks:** the canonical form constant on each trace and distinct between traces, against brute force; the number of words per trace against morphe's `linearisations`; each operator preserving its feature exactly (precedence, adjacency, position) on random parents; Starkweather's warehouse effect reproduced on a declared depletion system (relative-order operators ahead of edge recombination); the serial decoder's allocations feasible and maximal; the adaptivity gap of a stochastic knapsack within its known bound.
- **Documents:** morphe decision 0021, heuresis decision 0009.

### Stage 3. Resources (kategoria `resources`)

*Built 2026-11-20 (kategoria's decision 0004; 39 checks).*

- **Build:** components and products; the primitive semirings, verified once; finite carriers and truncations with partial addition; the preorder as a category and combination as a functor, checked; generated orders from conversions with free disposal declared; validity predicates; conservation laws and monotones checked on generators; convertibility by bounded search with witnesses and monotones, else undecided; allocation morphisms checked (conservation, validity, coherence); the order-dependence of a finite grant system decided (the action's commutation on reachable states, the independence relation derived); equivariance of an operator on finite instances; the verification table as data, each verdict with its method and scope.
- **Checks:** a truncation without partial addition refused as unlawful, with partial addition accepted; a declared order incompatible with combination refused, its failing pair named; conservation proved on generators and a violating conversion refuted; convertibility found with its witness on a small Petri net and refuted by a monotone on another; a finite system with two commuting and one non-commuting grant decided so; known resource theories as cases (CFS's examples).
- **Documents:** kategoria decision 0004; its theory note section.

### Stage 4. Assessment (heuresis `assessment`)

*Built 2026-11-20 (heuresis's decision 0010; 30 checks), in the planned `noise`'s place. Amended while building: constraints of computed standing bind by default, beside verified ones (a feasibility computed exactly is as hard as a verified one); the floor is declared.*

- **Build:** the assessment record and its mapping to `Evaluation`; standings and tiers before Pareto; replicates on demand, races with their guarantees, cascades by fidelity; the cache by input, evaluator, version, fidelity and stream; surrogate management on `models`, its agreement tracked; a Bradley–Terry aggregator for pairwise judgements (with tyche).
- **Checks:** a race's error rate within its bound on simulated noisy pairs; a cascade spending less for equal selections; tiers never letting a proposed metric outrank a verified one; Bradley–Terry recovering known strengths; caching returning recorded evaluations exactly.
- **Documents:** heuresis decision 0010 (the planned `noise` becomes `assessment`).

### Stage 5. Composition (the allocation domain `evolution`)

*Built 2026-11-20 (ADR 0137; 12 checks). The knapsack's instances are generated as Chu and Beasley's in the checks; the OR-Library's files and Warsaw's sequential mechanism are not yet taken up. Found while building: age layers (ALPS) as the default strategy, since survival of the best settled in a deceptive basin of the city case.*

- **Build:** allocation problems as above; the search; results as rounds with their verdicts; order-dependence reported; the sealed record; the two encoding routes on one problem with the price of generality reported.
- **Cases:** the OR-Library's multidimensional knapsack (Chu–Beasley sets, fetched by the data library under their terms), reproducing the ordering of Raidl and Gottlieb's study; a declared heterogeneous city case (money divisible, crews in units, sites distinct, an attribute on a manifold, order mattering through depletion and precedence) with known answers on a small instance found by enumeration; Warsaw's participatory budget, searching the order of a sequential mechanism.
- **Checks:** every evolved allocation lawful by kategoria's verdict; the enumerated optimum of the small city case found; the knapsack's ordering of encodings; caching by canonical form evaluating each trace once.
- **Documents:** ADR 0137; the domain's README and theory note.

### Stage 6. Edges

- **Build:** the API's composition (`science/evolve.hpp`: declare and verify a resource type, assemble a problem, run a few generations, ask and tell over the state saved as JSON with `evolution_state.hpp`); `science.allocation`'s evaluators at the edge (a language model's pairwise judgement with order swapped, k samples and an audit share; a person's pairwise comparisons through the page); the visualization layer's drawings (the archive as a field, the lineage, the commutation of grants, a resource type with its verdicts as seals); the Urban Studio plug-in's tool and view for evolving an allocation, with a page for people's comparisons.
- **Checks:** the API's, with a stand-in model; the drawings'; the Urban Studio's suite; the page driven in headless Chrome, pictures in the gallery.

### Stage 7. Economic evaluators

- **Build:** the economics domain's `ledger` (balance sheets and postings, conservation checked every step, an outside node, circulation readings, the steady state shared among agents first); an agent evaluator on nomos's `agents` with rules, learned and proposed behaviours kept apart; density laws on kinesis's `balance` (a value equation with its density, checked against published solutions); learned laws as evaluators (kinesis: a measurement layer on tyche's filters, regime-switching laws, declared baselines with forecast tests, an ensemble of laws, out-of-support flags).
- **Checks:** conservation exact over long runs; LM3 equal to the three-round truncation of retention; a known stationary distribution reproduced; a learned law beaten by its baseline reported so; an allocation outside the record's variation flagged.
- **Documents:** the economics domain's README; kinesis decision 0009; nomos decision 0016.

### Stage 8. Research tracks (as the user chooses)

- latent-space network models with their identifiability stated (tyche);
- surrogate stand-ins for agent evaluators with checked coverage;
- quantum-like sequential judgement models nested over their classical rivals, with the parameter-free tests (tyche, kinesis), and the counterbalanced experiment on allocation choices designed (peira);
- policies evolved as programs, and the comparison of sequences against policies on one problem, which the survey found nowhere.

### The Order of Stages 1–3

Stages 1, 2 and 3 do not depend on one another; Stage 5 needs all three, and Stage 4. The survey's own order of building (its §1.15, point 11) begins with resource types and their verdicts, since every later layer stands on what a resource is; spaces and order follow. Built together, the three meet first in Stage 5's composition.

### What Needs the User's Word

- The names of the modules (`spaces`, `sequential`, `resources`, `assessment`, `evolution`, `ledger`), all provisional.
- The cadence: a review after each stage, or after Stages 1–3 together.
- Which demonstration cases matter most after the knapsack benchmark.
- Stage 7's placement follows the survey (§1.15, point 6): the agent machinery in nomos's `agents`, the accounting of money and the circulation readings in the economics domain beside its `circulation`.

## 5. Open Questions

- A joint covariance across parts with no common embedding (a manifold beside a graph) has no method in the literature (Part III, gaps).
- A provably geometric crossover for dependent products and recursive types is open (Part III, gaps).
- The decidability of conversion rates and catalysis for finitely presented resource theories is open in what was found (Part IV, gaps).
- No study compares evolved sequences against evolved policies on one allocation problem, nor encodings for genuinely heterogeneous allocation (Part I, Part II, gaps); the framework can produce that evidence.
- No agent model has been validated against measured circulation statistics (Part VI); the economics domain's circulation readings make the test possible.
