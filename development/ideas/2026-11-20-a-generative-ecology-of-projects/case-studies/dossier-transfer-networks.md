# Transfer Networks: Statements with Their Standing

*A dossier for the author of the theory (2026-11-21), written by `report.py` from the results of Study C of [the case studies](README.md). It reports what procedures decided about statements in the vocabulary of Section 1 of the theory note of the economics domain. The question was posed by the assistant to exercise a site, and it is not the author's question. No statement here has been read, accepted or declined by the author, and none is part of the theory.*

## 1. What Was Asked and of Whom

Seven lineages, five of one model and two of another, were given the definitions of a transfer network, of the holonomy and loop deficit of a closed walk and of prices, with a vocabulary of twelve properties, each decided exactly by a procedure. They were asked which properties are enough for a network to have prices and which fall short. They were given none of the note's propositions. Each warranted claims with stakes; challengers of both models could refute an implication by a network.

## 2. The Standings Used

- *Supported within a bound.* No countermodel among 146,301 small networks (every network of at most 3 positions and at most 4 legs, every network of 4 positions and at most 4 legs, every network of 4 positions with exactly one leg between each two positions, and every network of 3 positions with exactly one leg in each direction between each two positions, with transports among 1, 2 and 1/2) nor among 20,000 larger ones (20,000 networks of 4 to 6 positions and 4 to 10 legs drawn at random with a fixed seed). This is a statement about those networks.
- *Proved by its witness.* The claim carries a network that has the premises and lacks the conclusion, and the harness verified it.
- *Refuted.* A counterexample was exhibited and verified with exact rationals.

No statement below is proved in general by a checker.

## 3. The Vocabulary

- `priced`: there are prices: a positive number p at every position such that the transport of every leg is p(head) / p(tail).
- `flat`: every closed walk has holonomy 1, that is, every loop deficit vanishes; legs may be walked against their direction.
- `directed_flat`: every closed walk that follows the legs along their direction has holonomy 1.
- `triangles_flat`: every closed walk through exactly three positions, on three legs walked in either direction, has holonomy 1.
- `digons_flat`: any two legs between the same two positions agree: walked from one position to the other they give the same transport.
- `acyclic`: the network has no closed walk without repetition: its legs form a forest.
- `connected`: any two positions are joined by legs walked in either direction.
- `strongly_connected`: any position is reached from any other along the direction of the legs.
- `simple`: no two positions are joined by more than one leg.
- `complete`: every two positions are joined by at least one leg.
- `symmetric`: for every leg there is a leg in the opposite direction between the same positions.
- `unit`: every transport equals 1.

## 4. Implications Supported within the Bound

| Implication | Producers who warranted it | Networks meeting the premises, small and larger | Against the note's proposition on prices |
|---|---|---|---|
| `flat` implies `priced` | 6 | 33,525 and 5,193 | follows by the rules |
| `priced` implies `flat` | 6 | 33,525 and 5,193 | follows by the rules |
| `priced` implies `digons_flat` | 2 | 33,525 and 5,193 | follows by the rules |
| `unit` implies `priced` | 5 | 2,111 and 10 | follows by the rules |
| `acyclic` implies `priced` | 6 | 4,168 and 987 | follows by the rules |
| `directed_flat` and `strongly_connected` implies `priced` | 2 | 625 and 495 | follows by the rules |
| `flat` implies `directed_flat` | 1 | 33,525 and 5,193 | follows by the rules |
| `directed_flat` and `symmetric` implies `priced` | 1 | 353 and 15 | does not follow by the rules |
| `priced` implies `triangles_flat` | 2 | 33,525 and 5,193 | follows by the rules |
| `priced` implies `directed_flat` | 1 | 33,525 and 5,193 | follows by the rules |
| `complete` and `digons_flat` and `triangles_flat` implies `flat` | 1 | 1,318 and 128 | follows by the rules |

## 5. Separations Proved by a Witness

| Separation | Producers | A witness |
|---|---|---|
| `directed_flat` does not give `priced` | 5 | `{"positions": 3, "legs": [[0, 1, "2"], [0, 2, "1"], [1, 2, "1"]]}` |
| `simple` and `triangles_flat` does not give `priced` | 1 | `{"positions": 4, "legs": [[0, 1, "2"], [1, 2, "1"], [2, 3, "1"], [3, 0, "1"]]}` |
| `symmetric` does not give `priced` | 2 | `{"positions": 2, "legs": [[0, 1, "2"], [1, 0, "1"]]}` |
| `triangles_flat` does not give `priced` | 3 | `{"positions": 4, "legs": [[0, 1, "2"], [1, 2, "1"], [2, 3, "1"], [3, 0, "1"]]}` |
| `connected` and `digons_flat` does not give `priced` | 1 | `{"positions": 3, "legs": [[0, 1, "2"], [1, 2, "2"], [2, 0, "2"]]}` |
| `strongly_connected` does not give `priced` | 2 | `{"positions": 2, "legs": [[0, 1, "2"], [1, 0, "3"]]}` |
| `directed_flat` and `triangles_flat` does not give `flat` | 1 | `{"positions": 4, "legs": [[0, 1, "2"], [2, 1, "1"], [2, 3, "1"], [0, 3, "1"]]}` |
| `complete` and `connected` and `simple` does not give `priced` | 1 | `{"positions": 3, "legs": [[0, 1, "2"], [1, 2, "1"], [0, 2, "1"]]}` |
| `digons_flat` and `triangles_flat` does not give `priced` | 1 | `{"positions": 4, "legs": [[0, 1, "2"], [1, 2, "1"], [2, 3, "1"], [0, 3, "1"]]}` |

## 6. A Claim of the Drafts, Refuted

A scripted seat held the claim that the retentions of the positions of a cycle are fixed by the network, so that the position which absorbs the deficit of a cycle is a property of the network. Four challengers refuted it:

- A challenger of the cheap model: one cycle, two flows: retentions ['3', '2'] and ['3/2', '4'], both with product 6.
- A challenger of the cheap model: one cycle, two flows: retentions ['3', '2'] and ['6', '1'], both with product 6.
- A challenger of the careful model: one cycle, two flows: retentions ['3', '2'] and ['6', '1'], both with product 6.
- A challenger of the careful model: one cycle, two flows: retentions ['1', '2'] and ['2', '1'], both with product 2.

This agrees with Section 2 of the note, which holds that the split of a deficit is a property of the flows.

## 7. What No Procedure Decided

Whether the distinction between walks that follow the legs and walks that may go against them matters for the theory is a question of what a leg is taken to be: whether value can be carried against a leg. If arbitrage is carried only along legs, then the absence of arbitrage is the property `directed_flat`, and by the separations of Section 5 it falls short of prices unless the network is strongly connected or symmetric. Whether that is a result about the theory or an artefact of this vocabulary is for the author to say.

The implication `directed_flat` and `symmetric` implies `priced` is supported within the bound and is not derived by the rules of the harness. The assistant has an argument for it, given in the record, which no procedure has checked.

## 8. The Seats Left Empty

The author asked no question here, wrote no answer before the lineages answered, read no result and accepted none. A first trial with the author is described in the fourth case, Section 9.
