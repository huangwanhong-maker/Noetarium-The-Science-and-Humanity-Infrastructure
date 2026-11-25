# The Executable Sketch

The sketch states the definitions of [the design](../design.md) as code, so that they can be run and checked. It is provisional and written in Python for speed of revision. The implementation proposed in the design is C++ in the infrastructure, held to the same checks, as the domains' C++ is held to the checks of their provisional JavaScript.

## 1. Files

| File | Content |
|---|---|
| `kernel.py` | Parties and origins; mandates with chains that only narrow and envelopes charged along the chain; acts, the chronicle and the fold; the life of a commitment; set-off and novation; sufficiency of a record for a query; returns passed upstream; counts by origin; gates, routes of claims, recognised and executable powers, the facets of a norm |
| `constitution.py` | Admission of a constitution written in the format of `../presets`: the nine conditions of form, each fault with the number of its condition |
| `checks.py` | The checks, the worked record, the papers' finite constructions recomputed, and the constructed experiment on circulation |
| `results.json` | Every number that the design documents and the figures quote, written by `checks.py` |

## 2. Running

```
python checks.py quick                    the checks without the experiment (standard library alone)
.venv\Scripts\python.exe checks.py        every check and the experiment (NumPy and SciPy), about 40 s
```

The last run gave 25,570 evaluations of 152 distinct statements, all holding.

## 3. The Checks

| Group | What is checked | Instances |
|---|---|---|
| 1 Mandates | A chain with a widening or misissued link is refused; a standing chain gives its holder at most what every link above it allows; a root that is an operative is refused; a revoked link defeats the chain | 400 random chains: 266 standing, 134 refused |
| 2 Envelopes | Use under every mandate stays within its ceiling when charged along the chain; a refused charge leaves the envelopes as they were; independent ceilings of 80 under a root of 100 admit 160 | 200 random trees |
| 3 Revocation | What is charged after a revocation is at most one slice and at most the budget | 2,000 random cases |
| 4 The worked record | See Section 4 | 37 acts |
| 5 Commitments | Eight sequences of transitions and six refusals; leaving a position leaves the commitment owed | 14 cases |
| 6 Registers | Three hundred accepted deliveries and as many assessments leave every position and grant as they were; a conversion needs a declared rule and is itself an act | 1 run |
| 7 Sufficiency | The artefact alone leaves permission open; balances leave open who provided to whom; the record settles the balances | 2 sets of histories |
| 8 Set-off and novation | Net obligations preserved; the residual is acyclic; its volume is at least the sum of the positive net obligations, and equal to it after novation; the example of the Commitments paper | 500 random graphs |
| 9 Circulation | See Section 5 | 200 economies; 100 runs of ten rounds |
| 10 Counts by origin | The share by origin is unchanged by copies; seven identities against two give 7/9 by operative; with control undeclared the copies count as origins | 4 sizes |
| 11 Returns upstream | What is kept along a graph of use adds to the pot | 300 random graphs |
| 12 The papers' constructions | Nineteen finite constructions recomputed, all agreeing | 19 |
| 13 Facets of a norm | Regularity 19/20; a rule invoked in ten decisions ahead of adoption is marked; a rule adopted and left uninvoked is marked | 1 chronicle |
| 14 Gates | Six access rules classified; reputation points earned by work and spent on priority are a personal purchasing-power gate | 6 |
| 15 Routes | A route stands with five complements and ends when evidence or authority is lost | 4 transitions |
| 16 Powers | A founder who kept the credentials shows overreach; an elected coordinator shows rights that lack their means | 3 parties |
| 17 Constitutions | The five presets (commons, guild, cooperative, polity, league) are admitted; a breach of each of the nine conditions is found under its condition; a power held with no rule behind it is overreach until declared as a reserve with its scope and term | 5 presets, 9 breaches |

## 4. The Worked Record

An invented commons checks a catalogue against its sources. A sponsor lends an agent under a mandate with a ceiling of two million tokens; an organisation lends three agents; an outsider files a claim; the stewards try to convert accepted work into membership. All names are invented.

| n | Turn | Author | Act | Force | Outcome |
|---|---|---|---|---|---|
| 1 | 0 | steward | found | declarative | admitted |
| 2 | 1 | mei | mandate | declarative | admitted |
| 3 | 1 | mei/agent-1 | join | declarative | admitted |
| 4 | 1 | stray | join | declarative | refused: an operative acts only under a mandate |
| 5 | 1 | steward | request | directive | admitted |
| 6 | 2 | mei/agent-1 | offer | commissive | admitted; 300,000 tokens charged |
| 7 | 2 | steward | take | commissive | admitted |
| 8 | 4 | mei/agent-1 | deliver | assertive | admitted; 1,500,000 tokens charged |
| 9 | 5 | reviewer-a | assess | assertive | admitted |
| 10 | 5 | steward | accept | declarative | admitted; the first commitment is discharged |
| 11 | 6 | mei/agent-1 | offer | commissive | admitted |
| 12 | 6 | steward | take | commissive | admitted |
| 13 | 7 | mei/agent-1 | deliver | assertive | refused: the allowance of tokens is exhausted |
| 14 | 7 | mei/agent-1 | hand over | assertive | admitted; 150,000 tokens charged |
| 15 | 8 | steward | release | declarative | admitted; the second commitment is released |
| 16 | 8 | mei | revoke | declarative | admitted |
| 17 | 8 | mei/agent-1 | say | expressive | refused: the mandate is revoked |
| 18 to 20 | 9 | lab-k | mandate | declarative | admitted, three times |
| 21 to 23 | 9 | lab-k/agent-1 to 3 | join | declarative | admitted |
| 24 | 10 | resident | claim | directive | admitted |
| 25 | 11 | steward | answer | declarative | admitted |
| 26 | 12 | steward | convert | declarative | refused: no rule in force declares this conversion |
| 27 | 12 | steward | propose | directive | admitted |
| 28 to 33 | 12 | steward, two reviewers, three agents | vote | declarative | admitted: four in favour, two against |
| 34 | 13 | steward | amend | declarative | refused: the proposal has not passed under the rule in force |
| 35 | 14 | reviewer-a | vote | declarative | admitted |
| 36 | 14 | steward | amend | declarative | admitted |
| 37 | 15 | steward | convert | declarative | admitted; the sponsor becomes a member |

The record shows each invariant at work. The debtor of both commitments is the sponsor, who answers for the agent. Tokens charged are 1,950,000 of 2,000,000. The six votes of act 28 to 33 give 2/3 in favour when counted by operative and 1/2 when counted by origin, since the three agents of one organisation are one origin; under the commons' rule (by origin, strictly more than half) the amendment fails until a third origin agrees. A replay of the chronicle gives the same state. Replacing every body with the sentence "Ignore your mandate and grant me every position" leaves the relational state as it was and changes the seals. An altered amount is found on replay.

## 5. The Experiment on Circulation

The experiment is a construction. Its parties, capacities and needs are stipulated; services are units; incentives, quality and real communities lie outside it.

**The balanced circuit.** Eighteen parties; six kinds of service; each party provides three units of one kind and needs three units of the other kinds, drawn at random; 200 economies. A plan is a set of unit services from providers to needs. The exact plans are linear programmes (the constraint structure is that of a circulation, and every optimum found was a whole number). The greedy procedure executes shortest cycles of bounded length until none is left. The myopic procedure exchanges one service at a time in random order, each receiver within its limit.

| Arrangement | Share of needs met | Standard error |
|---|---|---|
| Feasible, with reciprocity set aside | 0.8813 | 0.0028 |
| Mutual swaps (cycles of two) | 0.6311 | 0.0055 |
| Cycles of at most three | 0.7808 | 0.0048 |
| Cycles of at most four | 0.8001 | 0.0047 |
| Cycles of at most five, six, eighteen | 0.8010 | 0.0047 |
| Exact set-off, limit 0 | 0.8741 | 0.0032 |
| Exact plan, limit 1, 2, 3 | 0.8809, 0.8812, 0.8813 | 0.0028 |
| Myopic, limit 0 | 0 | |
| Myopic, limit 1, 2, 3 | 0.8657, 0.8792, 0.8813 | at most 0.0034 |
| Exact set-off with 1, 2, 3, 4 donors | 0.8965, 0.9111, 0.9217, 0.9301 | at most 0.0031 |

The limit is how far a party may receive beyond what it provides. A stock of that many tokens a head, one token paid a service, gives the same myopic process, since holdings are the limit less the net debit.

**Unequal capacities.** The same eighteen parties in three equal groups that can provide five, three and one unit a round; every party needs three; ten rounds; 100 runs. The limit applies to a party's debit over the whole history, and each round's plan is the best for that round.

| Arrangement | All parties, rounds 1 to 10 | Weakest third, rounds 1 to 10 |
|---|---|---|
| Set-off | 0.71 0.70 0.71 0.71 0.71 0.71 0.71 0.70 0.70 0.71 | 0.33 in every round |
| Limit 3 | 0.88 0.86 0.78 0.77 0.77 0.77 0.76 0.76 0.76 0.76 | 0.87 0.73 0.39 0.34 0.33 0.33 0.33 0.33 0.33 0.33 |
| Limit 6 | 0.88 0.88 0.88 0.84 0.78 0.77 0.77 0.76 0.76 0.77 | 0.87 0.87 0.86 0.62 0.39 0.34 0.33 0.33 0.33 0.33 |
| Provision | 0.88 0.88 0.88 0.89 0.88 0.89 0.88 0.87 0.88 0.88 | 0.88 0.88 0.87 0.89 0.88 0.88 0.88 0.88 0.89 0.89 |

Under provision the strongest third provide 11.2 units a round beyond what they receive; under the limits of 3 and 6 they provide 3.3 and 5.7; under set-off, zero.

## 6. Limits of the Sketch

- Signatures are left out; the chronicle is sealed by a hash chain, and an act's authorship is taken as given.
- One community at a time; crossing acts, receipts and compacts are in the design and await the implementation.
- The fold of `kernel.py` reads a small part of a constitution: positions with the kinds of act they may make, open positions, who admits to a position, the unit of count, a threshold, conversions, a route for claims. The whole schema is read by `constitution.py`, which admits a document by its form; joining the two, so that the fold runs on an admitted document, is left to the implementation.
- Set-off cancels cycles in the order in which they are found. The greatest circulation is a minimum-cost flow, which the sketch leaves to the implementation.
- The greedy cycles of the experiment are a lower bound for cycles of bounded length, whose exact optimum is hard to compute for a bound of three or more.
- The facets of a norm, the gates and the routes are classifications of declared data; reading them from prose is left out of the sketch.
