# The schema of historical objects, as kategoria answers it

Made by `applications/histos-harmonia/tools/draw-schema.mjs` from [historical-objects.cat](../historical-objects.cat).

| Line | Verdict | Claim | Reason |
|---|---|---|---|
| 8 | holds | Kinds is a category | from 20 generators and 1 equation, completed into 1 rule: 20 objects, 52 arrows; the identities' laws and associativity hold at every instance |
| 32 | holds | Kinds is thin | no two arrows are parallel |
| 33 | holds | Object is a terminal object of Kinds | it has exactly one arrow from every object |
| 36 | holds | Unreconciled is a category | from 5 generators and 0 equations, completed into 0 rules: 5 objects, 14 arrows; the identities' laws and associativity hold at every instance |
| 44 | fails | Unreconciled is thin | two arrows are parallel |
| 47 | holds | Disciplines is a category | from 26 generators and 1 equation, completed into 1 rule: 26 objects, 72 arrows; the identities' laws and associativity hold at every instance |
| 76 | holds | Disciplines is thin | no two arrows are parallel |
| 77 | holds | Object is a terminal object of Disciplines | it has exactly one arrow from every object |
| 80 | holds | Core is a functor Kinds → Disciplines | it keeps the ends of every arrow, every identity and every composite |
| 86 | holds | Core is faithful | it tells apart every pair of parallel arrows |
| 87 | holds | Core is full | every arrow between images is an image |
| 91 | holds | Taxonomy is a category | from 3 generators and 3 equations, completed into 3 rules: 2 objects, 6 arrows; the identities' laws and associativity hold at every instance |
| 99 | holds | the rules of Taxonomy are a complete rewriting system for it | 3 rules, each decreasing and derived from the equations; every critical pair is joinable |
| 100 | holds | the table of composition of Taxonomy | row g, column f: g ∘ f |
| 101 | holds | specialises ∘ specialises ∘ exemplifies = exemplifies in Taxonomy | a derivation of 2 steps by the equations leads from one to the other |
| 104 | fails | exemplifies ∘ part = exemplifies in Taxonomy | their normal forms differ, and the rules are confluent |
| 106 | holds | Chronology is a category | from 3 generators and 3 equations, completed into 3 rules: 2 objects, 6 arrows; the identities' laws and associativity hold at every instance |
| 114 | holds | Taxonomy ≅ Chronology | a functor that is a bijection on objects and on arrows was found |
| 119 | holds | Record is a category | from 23 generators and 4 equations, completed into 4 rules: 13 objects, 56 arrows; the identities' laws and associativity hold at every instance |
| 150 | holds | upper ∘ from_fits = domain ∘ type in Record | a derivation of 1 step by the equations leads from one to the other |
| 154 | holds | Sample is a functor Record → FinSet | it keeps every identity and every composite, element by element |
| 195 | fails | Mistyped is a functor Record → FinSet | it does not keep the composite lower ∘ from_fits = kind ∘ from: the two ways act differently |
| 225 | holds | Base is a category | from 1 generator and 0 equations, completed into 0 rules: 2 objects, 3 arrows; the identities' laws and associativity hold at every instance |
| 228 | holds | InRecord is a functor Base → Record | it keeps the ends of every arrow, every identity and every composite |
| 229 | holds | InRecord is faithful | it tells apart every pair of parallel arrows |
| 231 | holds | Desk is a category | from 7 generators and 0 equations, completed into 0 rules: 8 objects, 23 arrows; the identities' laws and associativity hold at every instance |
| 240 | holds | InDesk is a functor Base → Desk | it keeps the ends of every arrow, every identity and every composite |
| 241 | holds | InDesk is faithful | it tells apart every pair of parallel arrows |
| 243 | holds | Study is a category | from 9 generators and 2 equations, completed into 2 rules: 7 objects, 23 arrows; the identities' laws and associativity hold at every instance |
| 256 | holds | InStudy is a functor Base → Study | it keeps the ends of every arrow, every identity and every composite |
| 257 | holds | InStudy is faithful | it tells apart every pair of parallel arrows |
| 259 | holds | Music is a category | from 3 generators and 0 equations, completed into 0 rules: 4 objects, 10 arrows; the identities' laws and associativity hold at every instance |
| 264 | holds | InMusic is a functor Base → Music | it keeps the ends of every arrow, every identity and every composite |
| 265 | holds | InMusic is faithful | it tells apart every pair of parallel arrows |
| 268 | holds | Tunings is a functor Music → FinSet | it keeps every identity and every composite, element by element |
| 277 | holds | Read is a functor Base → FinSet | it keeps every identity and every composite, element by element |
| 282 | holds | Thread is a category | from 2 generators and 0 equations, completed into 0 rules: 4 objects, 6 arrows; the identities' laws and associativity hold at every instance |
| 286 | holds | Years is a category | 4 objects, 10 arrows; the identities' laws and associativity hold at every instance |
| 287 | holds | Dated is a functor Thread → Years | it keeps the ends of every arrow, every identity and every composite |
| 290 | holds | Anachronism is a category | from 2 generators and 0 equations, completed into 0 rules: 4 objects, 6 arrows; the identities' laws and associativity hold at every instance |
| 295 | fails | Misdated is a functor Anachronism → Years | no functor sends these objects and arrows as asked: what is given contradicts a composite |

## Diagrams

- [kinds.svg](kinds.svg)
- [disciplines.svg](disciplines.svg)
- [taxonomy.svg](taxonomy.svg)
- [chronology.svg](chronology.svg)
- [record.svg](record.svg)
- [sample.svg](sample.svg)
- [base.svg](base.svg)
- [desk.svg](desk.svg)
- [study.svg](study.svg)
- [music.svg](music.svg)
- [thread.svg](thread.svg)
