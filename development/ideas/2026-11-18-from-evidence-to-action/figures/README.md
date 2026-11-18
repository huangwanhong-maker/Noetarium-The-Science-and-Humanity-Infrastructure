# The framework from evidence to action, stated as categories: a first schema (seed, 2026-11-18, third round), as kategoria answers it

Made by `applications/category-studio/tools/draw-schema.mjs` from [schema.cat](../schema.cat).

| Line | Verdict | Claim | Reason |
|---|---|---|---|
| 23 | holds | Registers is a category | from 2 generators and 0 equations, completed into 0 rules: 3 objects, 6 arrows; the identities' laws and associativity hold at every instance |
| 29 | holds | LawKinds is a category | from 14 generators and 0 equations, completed into 0 rules: 15 objects, 49 arrows; the identities' laws and associativity hold at every instance |
| 45 | holds | LawInRegister is a functor LawKinds → Registers | it keeps the ends of every arrow, every identity and every composite |
| 65 | holds | Kinds is a category | from 25 generators and 0 equations, completed into 0 rules: 26 objects, 99 arrows; the identities' laws and associativity hold at every instance |
| 92 | holds | Kinds is thin | no two arrows are parallel |
| 93 | holds | Fact is a terminal object of Kinds | it has exactly one arrow from every object |
| 95 | holds | InRegister is a functor Kinds → Registers | it keeps the ends of every arrow, every identity and every composite |
| 106 | holds | FromLaw is a functor LawKinds → Kinds | it keeps the ends of every arrow, every identity and every composite |
| 111 | holds | FromLaw is full | every arrow between images is an image |
| 112 | holds | FromLaw is faithful | it tells apart every pair of parallel arrows |
| 115 | holds | Via is a functor LawKinds → Registers | it keeps the ends of every arrow, every identity and every composite |
| 116 | holds | Via ≅ LawInRegister | a natural isomorphism was found |
| 121 | holds | InLaw ⊣ FromLaw | every object has a universal arrow; the adjoint built from them verifies, with both triangle identities |
| 122 | holds | InLaw ⊣ FromLaw | the unit and the counit are natural and both triangle identities hold at every object |
| 128 | holds | Examples is a category | from 30 generators and 0 equations, completed into 0 rules: 31 objects, 124 arrows; the identities' laws and associativity hold at every instance |
| 160 | holds | Examples is thin | no two arrows are parallel |
| 163 | holds | Declare is a functor Kinds → Examples | it keeps the ends of every arrow, every identity and every composite |
| 170 | holds | Declare is full | every arrow between images is an image |
| 171 | holds | Declare is faithful | it tells apart every pair of parallel arrows |
| 175 | holds | AsFramework ⊣ Declare | every object has a universal arrow; the adjoint built from them verifies, with both triangle identities |
| 176 | holds | AsFramework ⊣ Declare | the unit and the counit are natural and both triangle identities hold at every object |
| 187 | holds | Steering is a theory | 6 types, 16 generators, 19 laws |
| 231 | holds | (S ⊗ cB) ; (S ⊗ B ⊗ policy) ; (S ⊗ B ⊗ cA) ; (S ⊗ σ(B,A) ⊗ A) ; (dynamics ⊗ B ⊗ A) ; (cS ⊗ B ⊗ A) ; (S ⊗ observe ⊗ B ⊗ A) ; (S ⊗ σ(O,B) ⊗ A) ; (S ⊗ B ⊗ σ(O,A)) ; (S ⊗ estimate) is well formed | S ⊗ B → S ⊗ B |
| 239 | holds | Sure is a model of Steering | each of its 19 laws holds, entry by entry |
| 257 | holds | Possible is a model of Steering | each of its 19 laws holds, entry by entry |
| 276 | fails | NoisyFeature is a model of Steering | the law a feature is deterministic fails: the two sides differ at output 1, input 0: 0 and 1 |
| 295 | fails | NoisyEvaluation is a model of Steering | the law an evaluation is deterministic fails: the two sides differ at output 1, input 2: 0 and 1 |
| 313 | fails | dynamics ; cS = (cS ⊗ cA) ; (S ⊗ σ(S,A) ⊗ A) ; (dynamics ⊗ dynamics) holds in Possible | the two sides differ at output 1, input 0: 0 and 1 |
| 318 | holds | Watched is a theory | 4 types, 8 generators, 0 laws |
| 331 | holds | Kept is a model of Watched | the theory has no laws |
| 342 | holds | member ; (S ⊗ cB) ; (S ⊗ B ⊗ policy) ; (S ⊗ B ⊗ cA) ; (S ⊗ σ(B,A) ⊗ A) ; (dynamics ⊗ B ⊗ A) ; (cS ⊗ B ⊗ A) ; (S ⊗ observe ⊗ B ⊗ A) ; (S ⊗ σ(O,B) ⊗ A) ; (S ⊗ B ⊗ σ(O,A)) ; (S ⊗ estimate) = member ; (S ⊗ cB) ; (S ⊗ B ⊗ policy) ; (S ⊗ B ⊗ cA) ; (S ⊗ σ(B,A) ⊗ A) ; (dynamics ⊗ B ⊗ A) ; (cS ⊗ B ⊗ A) ; (S ⊗ observe ⊗ B ⊗ A) ; (S ⊗ σ(O,B) ⊗ A) ; (S ⊗ B ⊗ σ(O,A)) ; (S ⊗ estimate) ; member holds in Kept | both sides are one matrix of 8 × 8 |
| 346 | holds | Guessed is a model of Watched | the theory has no laws |
| 358 | fails | member ; (S ⊗ cB) ; (S ⊗ B ⊗ policy) ; (S ⊗ B ⊗ cA) ; (S ⊗ σ(B,A) ⊗ A) ; (dynamics ⊗ B ⊗ A) ; (cS ⊗ B ⊗ A) ; (S ⊗ observe ⊗ B ⊗ A) ; (S ⊗ σ(O,B) ⊗ A) ; (S ⊗ B ⊗ σ(O,A)) ; (S ⊗ estimate) = member ; (S ⊗ cB) ; (S ⊗ B ⊗ policy) ; (S ⊗ B ⊗ cA) ; (S ⊗ σ(B,A) ⊗ A) ; (dynamics ⊗ B ⊗ A) ; (cS ⊗ B ⊗ A) ; (S ⊗ observe ⊗ B ⊗ A) ; (S ⊗ σ(O,B) ⊗ A) ; (S ⊗ B ⊗ σ(O,A)) ; (S ⊗ estimate) ; member holds in Guessed | the two sides differ at output 2, input 1: 1 and 0 |
| 374 | holds | Record is a category | from 36 generators and 9 equations, completed into 9 rules: 22 objects, 100 arrows; the identities' laws and associativity hold at every instance |
| 437 | holds | Sample is a functor Record → FinSet | it keeps every identity and every composite, element by element |
| 500 | fails | Borrowed is a functor Record → FinSet | it does not keep the composite of ∘ rests = of ∘ rests: the two ways act differently |
| 532 | fails | Misread is a functor Record → FinSet | it does not keep the composite point_term ∘ settles = point_term ∘ settles: the two ways act differently |
| 571 | fails | ForeignPoint is a functor Record → FinSet | it does not keep the composite by_operator ∘ in_evaluation = by_operator ∘ in_evaluation: the two ways act differently |
| 610 | fails | CrossedTerms is a functor Record → FinSet | it does not keep the composite evaluates ∘ by_operator ∘ judged = standard_of ∘ standard: the two ways act differently |
| 646 | fails | Hindsight is a functor Record → FinSet | it does not keep the composite at ∘ decided_on = when: the two ways act differently |
| 680 | fails | Unauthorised is a functor Record → FinSet | it does not keep the composite empowers ∘ authorised = made_by: the two ways act differently |
| 723 | fails | AnsweringAnother is a functor Record → FinSet | it does not keep the composite holder ∘ after = holder ∘ after: the two ways act differently |

## Diagrams

- [The three registers of reality](registers.svg)
- [The kinds of fact in the framework](kinds.svg)
- [A record of the framework](record.svg)
- [A small record on the schema](sample.svg)
