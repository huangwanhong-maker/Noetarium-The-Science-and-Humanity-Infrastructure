# Circulation Under Scarcity: The User's Hypothesis Put to a Construction

*Status: draft (2026-11-25). The jar of the subdomain human-ai ([jar.hpp](../human-ai/jar/include/communities/jar/jar.hpp), its check [check_jar.cpp](../human-ai/jar/tests/check_jar.cpp)). Every number below is printed by that check program, the mean of 40 runs of 20 rounds; a construction measures, and proves nothing about real communities.*

## 1. The Hypothesis

The user stated it on 2026-11-25: under resource scarcity, a currency-like object may need to appear for value to circulate fairly; it can be made to disappear, with no currency at all, if the circulation of value is epistemically good enough and justice is ensured. The user called it a hypothesis.

The design record made three claims that a construction can examine.

1. A currency enforces reciprocity without knowing who can give.
2. It pays for that with the weak.
3. Two things together can take its place: knowing who can give, and provision by need with duties by capacity.

## 2. The Construction

Thirty-six parties in three tiers of capacity: 5, 3 and 1 units of service a round, each of one of six kinds. Each party needs units of other kinds each round: 3, so that capacity and need balance in aggregate, or 4, so that capacity is three quarters of need. A share of the parties are free riders. They declare the weakest capacity and offer the least that keeps their access. Under provision they also claim a unit more than they need when they expect not to be found.

Five arrangements:

| Arrangement | How a round's offers reach needs | What keeps a party contributing |
|---|---|---|
| Currency | a market one unit at a time, price one token, a stock of three at the start | it must earn what it spends |
| Currency with income | the same, holdings lapsing by a tenth each round, two tokens issued to everyone | it must earn what the income leaves |
| Currency with targeted income | the same, each party topped up to its need by the community's estimate of its capacity | as above |
| Provision, open | handed out by need, the least served first | nothing |
| Provision with duty | the same; each party owes its capacity as the community knows it, and a party found offering less, or claiming more than it needs, is suspended for the next round | the duty, and the chance of being found |

The **epistemic resource** is the probability that the community verifies a party's capacity and its claim of need in a round, by an audit, a test or a history read. A verified capacity stays known. A free rider under duty shirks when the effort it saves is worth more than the provision it expects to lose.

## 3. What the Jar Finds

### 3.1 With Every Party Contributing

With no free riders, both kinds of provision and the currency with income meet 0.92 of needs when capacity balances need, and 0.74 when it is scarce; under scarcity 0.75 is the bound. Their weakest tier is met as well as the rest. The currency without income meets 0.72 and 0.64, and its weakest tier 0.36 and 0.28. Every need it refuses rests on a balance, while what the strong offer beyond their own needs waits for buyers who hold no tokens.

### 3.2 With Free Riders

Shares of needs met, all parties and the weakest tier (balanced capacity and need above; capacity three quarters of need below).

**Half the parties free riders, capacity equal to need**

| Arrangement | Epistemic resource 0 | 0.1 | 0.25 | 0.5 | 1 |
|---|---|---|---|---|---|
| Currency | 0.62 / 0.35 | 0.62 / 0.35 | 0.62 / 0.35 | 0.62 / 0.35 | 0.62 / 0.35 |
| Currency with income | 0.49 / 0.49 | 0.49 / 0.49 | 0.49 / 0.49 | 0.49 / 0.49 | 0.49 / 0.49 |
| Currency with targeted income | 0.49 / 0.49 | 0.60 / 0.62 | 0.66 / 0.69 | 0.68 / 0.72 | 0.69 / 0.73 |
| Provision, open | 0.50 / 0.49 | 0.50 / 0.49 | 0.50 / 0.49 | 0.50 / 0.49 | 0.50 / 0.49 |
| Provision with duty | 0.66 / 0.66 | 0.80 / 0.80 | 0.81 / 0.81 | 0.89 / 0.90 | 0.92 / 0.92 |

**Half the parties free riders, capacity three quarters of need**

| Arrangement | Epistemic resource 0 | 0.1 | 0.25 | 0.5 | 1 |
|---|---|---|---|---|---|
| Currency | 0.57 / 0.28 | 0.57 / 0.28 | 0.57 / 0.28 | 0.57 / 0.28 | 0.57 / 0.28 |
| Currency with income | 0.39 / 0.39 | 0.39 / 0.39 | 0.39 / 0.39 | 0.39 / 0.39 | 0.39 / 0.39 |
| Currency with targeted income | 0.38 / 0.37 | 0.44 / 0.44 | 0.48 / 0.49 | 0.50 / 0.50 | 0.51 / 0.51 |
| Provision, open | 0.38 / 0.38 | 0.38 / 0.38 | 0.38 / 0.38 | 0.38 / 0.38 | 0.38 / 0.38 |
| Provision with duty | 0.49 / 0.49 | 0.63 / 0.63 | 0.69 / 0.70 | 0.72 / 0.73 | 0.74 / 0.74 |

Five readings follow.

1. **The currency works without knowledge and fails the weak.** Its outcome is the same at every epistemic resource. It makes free riders earn what they spend, so they receive less than cooperators. Its weakest tier receives about a third of what it needs, or less.
2. **Income without knowledge invites withdrawal.** An income issued to everyone closes the gap while everyone contributes. With free riders it falls below the plain currency (0.49 against 0.62; 0.39 against 0.57), since they live on it and withhold what they could offer.
3. **Justice without knowledge is exploited.** Open provision equals the best when everyone contributes. With free riders it falls to 0.50 and 0.38, and the free riders, who claim more and give nothing, are better served than the cooperators.
4. **Knowledge inside a currency helps, and the instrument keeps its own limit.** Targeted income rises with the epistemic resource and stays below provision with duty at every level. A currency asks each party to earn what it needs, and a duty asks it to give what it can, so under the currency the strong withhold their surplus.
5. **Knowledge with justice takes the currency's place.** Provision with duty rises with the epistemic resource and reaches the bound when capacities are known. It meets more needs than the currency from no epistemic resource at all in three of the four settings, and from 0.1 in the hardest (scarce, half free riders). It serves the weakest tier better at every level tried.

### 3.3 The Price of the Epistemic Resource

In the tables above verification is free. When each audit consumes service, taken from what was offered, provision with duty in the hardest setting meets these shares (the currency meets 0.57 / 0.28):

| Cost of an audit, in units of service | 0 | 0.1 | 0.25 | 0.5 | 0.75 | 1 |
|---|---|---|---|---|---|---|
| 0 | 0.49 / 0.49 | 0.63 / 0.63 | 0.69 / 0.70 | 0.72 / 0.73 | 0.74 / 0.74 | 0.74 / 0.74 |
| 0.25 | 0.49 / 0.49 | 0.63 / 0.63 | 0.68 / 0.69 | 0.69 / 0.70 | 0.70 / 0.70 | 0.68 / 0.68 |
| 0.5 | 0.49 / 0.49 | 0.62 / 0.62 | 0.66 / 0.67 | 0.66 / 0.67 | 0.65 / 0.65 | 0.62 / 0.62 |
| 1 | 0.49 / 0.49 | 0.61 / 0.60 | 0.61 / 0.64 | 0.58 / 0.60 | 0.55 / 0.55 | 0.45 / 0.47 |
| 2 | 0.49 / 0.49 | 0.58 / 0.58 | 0.52 / 0.56 | 0.35 / 0.41 | 0.19 / 0.23 | 0.03 / 0.04 |

The columns run over the epistemic resource. The best level of verification falls as its price rises: three quarters at a quarter of a unit, a tenth to a quarter at a unit, a tenth at two units. At a price of one unit of service an audit costs what a party of the weakest tier produces in a round, and provision with duty still meets slightly more needs in total than the currency (0.61 against 0.57). At two units it meets as many in total (0.58) and twice as many of the weakest tier's (0.58 against 0.28). Beyond that, verification consumes the economy it serves.

## 4. The Reading

Within this construction the hypothesis holds, with its conditions stated.

- Under scarcity a currency does one thing that nothing else in the jar does without knowledge: it holds each party to reciprocity whether or not anyone knows what that party could give. Its price is that whoever can give little receives little.
- The currency becomes unnecessary when two things are present together. The first is knowledge of who can give and who needs: verification of capacities and of claims, cheap enough to be done often enough. The second is justice: provision by need, with duties by capacity, enforced by a sanction that rests on conduct. Either alone fails: knowledge inside a currency leaves surplus idle, and justice without knowledge is exploited.
- The condition "if AI epistemic resource enough" becomes a measurable one: the price of a verification relative to a unit of service. In the jar provision with duty dominates the currency in total while an audit costs less than about one unit of service, and for the weakest at every price tried.
- The access rules of provision with duty read conduct in the round, never an accumulated quantity. By Definition 11 the gate is one of membership by conduct, and the first test of dissolution passes. Every refusal under the currency rests on a balance.

## 5. What Lies Outside, and What Would Overturn the Reading

The construction favours provision in ways that a real community may not.

- Free riders honour their declarations.
- The sanction is credible and costs nothing to impose.
- Persons are not asked whether the outcome is fair.
- The planner hands out units exactly, while the market is myopic: with half the parties free riders, a fifth of what is offered under the currency finds no buyer when capacity balances need, and a ninth when it is scarce.

The construction also leaves out:

- prices other than one, savings, debt and credit;
- learning, collusion and quality;
- misreporting by cooperators;
- the cost of keeping the record.

Each of these is a parameter or a module to add. The bounded trial of the design (its stage 3) is where the behavioural question, whether persons cooperate as well under duties read from a record as under tokens, can be measured. One experiment with persons found tokens better than a public record ([the design](../../../development/ideas/2026-11-25-a-society-of-human-and-ai-communities/design.md), Section 6.5).
