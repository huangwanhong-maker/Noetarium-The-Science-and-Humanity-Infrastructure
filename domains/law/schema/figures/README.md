# The schema of facts in law, as kategoria answers it

Made by `applications/category-studio/tools/draw-schema.mjs` from [facts.cat](../facts.cat).

| Line | Verdict | Claim | Reason |
|---|---|---|---|
| 9 | holds | Registers is a category | from 2 generators and 0 equations, completed into 0 rules: 3 objects, 6 arrows; the identities' laws and associativity hold at every instance |
| 13 | holds | Registers is thin | no two arrows are parallel |
| 14 | holds | Ontological is a terminal object of Registers | it has exactly one arrow from every object |
| 20 | holds | Kinds is a category | from 14 generators and 0 equations, completed into 0 rules: 15 objects, 49 arrows; the identities' laws and associativity hold at every instance |
| 36 | holds | Kinds is thin | no two arrows are parallel |
| 37 | holds | Fact is a terminal object of Kinds | it has exactly one arrow from every object |
| 40 | holds | InRegister is a functor Kinds → Registers | it keeps the ends of every arrow, every identity and every composite |
| 51 | holds | Standing is a category | from 13 generators and 0 equations, completed into 0 rules: 10 objects, 39 arrows; the identities' laws and associativity hold at every instance |
| 68 | fails | Standing is thin | two arrows are parallel |
| 72 | holds | Reached is a category | from 13 generators and 4 equations, completed into 4 rules: 10 objects, 31 arrows; the identities' laws and associativity hold at every instance |
| 91 | holds | Reached is thin | no two arrows are parallel |
| 92 | holds | Forget is a functor Standing → Reached | it keeps the ends of every arrow, every identity and every composite |
| 99 | holds | Forget is full | every arrow between images is an image |
| 102 | fails | Forget is faithful | it sends on_admission ∘ admit and on_notice ∘ notice to on_admission ∘ admit |
| 106 | holds | Roles is a category | from 2 generators and 0 equations, completed into 0 rules: 3 objects, 6 arrows; the identities' laws and associativity hold at every instance |
| 110 | holds | Roles is thin | no two arrows are parallel |
| 111 | holds | Ultimate is a terminal object of Roles | it has exactly one arrow from every object |
| 116 | holds | Elements is a category | from 3 generators and 0 equations, completed into 0 rules: 4 objects, 7 arrows; the identities' laws and associativity hold at every instance |
| 121 | holds | Custom with practised, accepted is a limit of the objects Practice, Acceptance | the one cone factors through it by exactly one arrow |
| 125 | holds | Record is a category | from 13 generators and 3 equations, completed into 3 rules: 10 objects, 43 arrows; the identities' laws and associativity hold at every instance |
| 145 | holds | within ∘ source = within ∘ target in Record | a derivation of 1 step by the equations leads from one to the other |
| 150 | holds | Sample is a functor Record → FinSet | it keeps every identity and every composite, element by element |
| 178 | fails | MindAsTrace is a functor Record → FinSet | it does not keep the composite as ∘ marks = register ∘ shape: the two ways act differently |
| 202 | fails | AcrossProceedings is a functor Record → FinSet | it does not keep the composite within ∘ target = within ∘ source: the two ways act differently |
| 228 | holds | RecordSteps is a category | from 16 generators and 4 equations, completed into 4 rules: 12 objects, 52 arrows; the identities' laws and associativity hold at every instance |
| 251 | holds | Grow is a functor Record → RecordSteps | it keeps the ends of every arrow, every identity and every composite |
| 257 | holds | Grow is full | every arrow between images is an image |
| 258 | holds | Grow is faithful | it tells apart every pair of parallel arrows |
| 260 | holds | Pushed is the left Kan extension of Sample along Grow | computed pointwise, it is a functor, and it is universal against each of 13 functors tried (the representables and the constant 2) |
| 262 | holds | Kept is a functor RecordSteps → FinSet | it keeps every identity and every composite, element by element |
| 293 | holds | Back is a functor Record → FinSet | it keeps every identity and every composite, element by element |

## Diagrams

- [The three registers of reality](registers.svg)
- [The kinds of fact](kinds.svg)
- [How a fact comes to stand](standing.svg)
- [Standing, the ways forgotten](reached.svg)
- [Roles in reasoning](roles.svg)
- [Custom, its two elements](elements.svg)
- [A record of facts](record.svg)
- [A small record on the schema](sample.svg)
- [A record of facts with its proceeding's steps and findings](recordsteps.svg)
