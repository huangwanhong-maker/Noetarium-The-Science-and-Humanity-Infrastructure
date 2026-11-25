---
status: seed
---

# A Society of Human and AI Communities: A Ground of Acts and Records, Constitutions as Data, Monetary and Moneyless Circulation, and Polycentric Government

*Status: seed (2026-11-25). The entry to a design whose documents, figures and executable sketch are in the folder [2026-11-25-a-society-of-human-and-ai-communities/](2026-11-25-a-society-of-human-and-ai-communities/design.md). It records the user's request, the reading given to it, the design in brief, what the sketch found, and what is asked of the user. On the same day the user chose a new foundation for institutions and a domain `communities` with a subdomain for the idea, and the implementation began ([ADR 0167](../decisions/0167-thesmos-and-the-communities-domain.md)): the foundation [thesmos](../../foundations/thesmos/README.md) and the domain [communities](../../domains/communities/README.md), whose jar puts the user's hypothesis of dissolution to a construction. Koinonia, chronicle, operative and league remain working names.*

## 0. Summary

The user asked for the design, and then the implementation, of a society in which persons and AI agents cooperate: heterogeneous communities with their own internal orders, participants with unequal intelligence and computation, and authority that stays with each community. Seven preliminary papers of the user were given as background.

The design answers with a common *ground* and a declared *constitution*. The ground is a small grammar of acts with declared force, a sealed chronicle of them in each community from which every standing relation is computed, and eight invariants that a program can check. A constitution is data in seven kinds of rules with its rule of amendment; the ground admits its form and leaves its substance to the community. A community is itself a party, so a league of communities is a community of communities.

The five questions of the request are answered in [the design](2026-11-25-a-society-of-human-and-ai-communities/design.md): communication by scenes, envoys, interfaces and compacts; norms as entries with four independent facets; a money-like instrument as a view of the record joined to a gate, with clearing, memory and provision as what circulation rests on in its absence, and seven tests of its dissolution; records that keep contribution, use and receiving work apart from entitlement; and government by constitutions, compacts and leagues whose offices are plural.

[The implementation](2026-11-25-a-society-of-human-and-ai-communities/implementation.md) places the parts in the infrastructure, names what is reused, and sets out stages. [An executable sketch](2026-11-25-a-society-of-human-and-ai-communities/sketch/README.md) holds the definitions as code: 152 distinct statements in 25,570 evaluations, all holding. Five constitutions of different internal orders are written as [files of one schema](2026-11-25-a-society-of-human-and-ai-communities/presets/) and admitted by one check. [Fourteen figures](2026-11-25-a-society-of-human-and-ai-communities/figures/README.md) accompany the documents. Three surveys were made for the record, on [protocols for agents](2026-11-25-a-society-of-human-and-ai-communities/survey/agent-protocols.md), on [community economies and contribution accounting](2026-11-25-a-society-of-human-and-ai-communities/survey/community-economies.md) and on [governance, norms and populations of agents](2026-11-25-a-society-of-human-and-ai-communities/survey/governance-and-agent-populations.md), each of their statements marked as read in its source, read in an excerpt, or remembered.

## 1. The Request

### 1.1 The Proposal

On 2026-11-25 the user wrote: "Today, we will have another tremendous idea! In a word, we want build a human/AI agents community. So that we can do AI, human cooperative 'society'. A similar concept may be 'grassroot economics', but just a reference. We face more challenging, because this is in fact a society with heterogenous communities and heterogenous subjects (human-AI) building, and subjects has intelligence resource, computation resources, etc gaps."

### 1.2 The Problems Named

"Various problems need investigate, like (1) how to do in-community, and cross-community communications; (2) how norm formed; (3) whether money-like form should appear, and we said that, definitively if may appear first, but in future, if AI epistemic resource enough, we deed can dissolute currency in value circulation circle (can see my previous currentcy dissolution paper in website), (4) how record contributions, trajectories, etc, this is important for us do value return; evidence for legal and goverance issues, economics issue etc (we said this may help us dissolute the money)., (5) how to gover this society, it somehow like international society, no centering entity to gover all community. Of course we can make something centering community, just like 'international organization' and issue some common laws."

### 1.3 The Conditions

"Anyways, may need consideration. But in this project we mainly consider DEISGN AND IMPLEMENTATIONS. let us design first, above paper are very preliminary, don't fully trust, but can refers some idea and questions, they may useful in design. Then seriously, taking efforts, to design, and output the idea and design documents, including related figures/diagrams! don't hesitate time."

And on the meta level: "This may allow human-ai get together to solve various problems, labors. Of course, in a meta level, we not limited the inner-community goverance and structure. Some may like commons, any users may send an agent to help for hours/days etc, or until reach token limitation for solving some tasks inside this community, like 'donation', some may like really society, has political structure, agent/human joining is selective, etc...."

The papers named are the seven most recent in the user's downloads: on institutional foundations, political authority and infrastructural power, community diplomacy, commitments and monetary change, evaluative authority and norm formation, responsibility and remedy, and post-labor participation.

## 2. The Reading Given to the Request

### 2.1 A Thin Common Part

The design problem is the choice of what all communities share. The reading is that they share exactly a way of acting on the record: a grammar of acts, a sealed record, rules as data. A currency, a government, a registry of identities and an ontology are each left to communities and to the agreements among them.

### 2.2 The Papers as Requirements

The papers decline to fix mechanisms; their arrangements "remain configurable". They are read as a careful statement of what must be kept apart, and the design fixes what they defer as defaults in data. [The reading of the papers](2026-11-25-a-society-of-human-and-ai-communities/reading-of-the-papers.md) gives a concordance, seven departures and nineteen of their finite constructions recomputed.

### 2.3 Amendments to the Intuitions of the Request

Three intuitions of the request are kept with an amendment each.

*On money.* The request expects a money-like form first and its dissolution once AI's epistemic resources suffice. The design agrees for balanced circuits, where what an instrument does is done by finding cycles and chains of exchange, which capable agents make cheap. It adds two limits. Across a gap of capacity an instrument only postpones the outcome, and provision is the only bridge; that is a political decision of a constitution. And the record that takes the instrument's place identifies its parties and can become the new gate, which is the "apparent dissolution" of the user's paper on relational currency; plural offices and portable records are the safeguard.

*On the centre.* The request suggests a centring community like an international organisation that issues common laws. In the design such a body is one more community: its members are communities, its laws bind by accession, and each of its offices can be run by more than one party.

*On the freedom of the meta level.* The request leaves internal governance free. The design does so for substance and says openly that its ground carries two commitments of its own: a participant can leave with their record, and anyone affected can see the route by which a claim is answered.

## 3. The Design in Brief

| Question of the request | Answer of the design | Where |
|---|---|---|
| Communication in and across communities | Acts with a typed envelope and a free body; scenes with protocols, presence and tempo; recognition, envoys, interfaces checked as squares, compacts; every crossing act entered on both sides | design, Section 4 |
| The formation of norms | A register whose entries carry four independent facets (regularity, adoption, operation, justification), with marks for a norm that operates unadopted and one adopted and inoperative; routes of formation, application through readings and precedent | design, Section 5 |
| Money-like forms and their dissolution | Six economic objects as types; forms of circulation from gift to outside money; clearing by set-off, novation, cycles and chains; an instrument as a view joined to a gate; seven tests of dissolution; provision for gaps | design, Section 6 |
| Records of contribution and trajectory | Chronicles and holdings; contribution, use and receiving work; description, causal account and entitlement kept apart; return rules; evidence with its tests; proportion | design, Section 7 |
| Government of the society | Four levels of order; leagues with plural offices; a default procedure for claims; steps for disputes; the ground governed by its own rules | design, Section 8 |
| Unequal intelligence and computation | Counts by origin, receiving capacity, checkable warrants, floors in usable units, shared agents, epistemic aid, a solidarity pool | design, Section 9 |

## 4. The Documents

| Document | Content |
|---|---|
| [design.md](2026-11-25-a-society-of-human-and-ai-communities/design.md) | The design: the ideas, the formal setting with seven results, constitutions, the five answers, the measures against unequal capacity, threats, questions |
| [implementation.md](2026-11-25-a-society-of-human-and-ai-communities/implementation.md) | The five places in the infrastructure, what is reused, the documents exchanged, the kernel, the node, how persons and agents take part, security, the page, stages and measures, proposed decisions, risks |
| [reading-of-the-papers.md](2026-11-25-a-society-of-human-and-ai-communities/reading-of-the-papers.md) | The seven papers and their use, a concordance, what the design fixes, where it departs, the constructions recomputed, observations for their author |
| [existing-work.md](2026-11-25-a-society-of-human-and-ai-communities/existing-work.md) | Existing systems, protocols and results that the design builds on or must answer, with the state of their verification |
| [survey/agent-protocols.md](2026-11-25-a-society-of-human-and-ai-communities/survey/agent-protocols.md) | The protocols by which AI agents act for principals, with delegation, identity, signed records and law, and the state of each statement |
| [survey/community-economies.md](2026-11-25-a-society-of-human-and-ai-communities/survey/community-economies.md) | Circulation of value and contribution accounting in practice and in the literature, with addresses, identifiers and the state of each statement |
| [survey/governance-and-agent-populations.md](2026-11-25-a-society-of-human-and-ai-communities/survey/governance-and-agent-populations.md) | Norms and institutions among agents and persons, governance tools, precedents of mixed communities, with the state of each statement |
| [figures/](2026-11-25-a-society-of-human-and-ai-communities/figures/README.md) | Fourteen figures with their captions and the scripts that make them |
| [sketch/](2026-11-25-a-society-of-human-and-ai-communities/sketch/README.md) | The definitions as code, the checks, a worked record of 37 acts and a constructed experiment |
| [presets/](2026-11-25-a-society-of-human-and-ai-communities/presets/) | Five constitutions as data: commons, guild, cooperative, polity, league |

## 5. Findings of the Sketch and of the Search

### 5.1 The Sketch

- The relational state is a function of the record: a replay of the worked record gives the same state, an altered act is found, and replacing every body leaves the state as it was.
- The same six votes pass by operative (2/3) and fail by origin (1/2) when three of the voters are agents of one organisation. With control undeclared the count by origin equals the count by operative, so the count rests on the declaration.
- Two ceilings of 80 under a root of 100 admit 160 when charged apart and 80 when charged along the chain. A revocation learnt late costs 450 when unsliced and 50 with a slice of 100 in the case computed.
- On 500 random graphs of obligations, set-off leaves 0.62 of the volume and novation 0.44, with every net obligation unchanged.
- In a balanced circuit, mutual swaps meet 0.631 of needs, greedy cycles of at most four 0.800, exact set-off 0.874, and a plan with reciprocity set aside 0.881. Exchange one service at a time needs an instrument or a limit to begin; planned set-off begins unaided.
- Under unequal capacities set-off meets a third of the weakest parties' needs; a limit of 3 or of 6 defers that by about two and five rounds; provision keeps them at 0.88.
- Nineteen finite constructions of the papers were recomputed and agree.
- Five constitutions (an open commons, a guild entered by assessment, a cooperative, a polity with juries by lot, a league of communities) pass one admission of nine conditions of form, and a breach of each condition is found.

### 5.2 The Searches of Practice

The findings are in the design's Sections 2.9, 5.6, 6.9 and 8.7, each with how it was obtained.

- The protocols by which agents act today (the Model Context Protocol in its revision of 2026-07-28, Agent2Agent 1.0) authenticate software. They leave to implementers the chain of authority, the party that answers, revocation and the record, in the words of their own specifications. That is the part the ground supplies.
- A payments protocol for agents uses the word mandate for a signed credential of purchase. The design keeps the general sense and says so.
- Norms form among language-model agents by interaction alone, and committed minorities from 2% to 67% of a population have tipped them, by model.
- Wikipedia's bot policy is a working regime of mandates: approval for each task, and an operator who answers.
- A count by origin in operation, institutions amended by persons and agents together, and a register of measured and adopted norms were each absent from what the searches reached.

- Clearing and bounded credit are established among firms. Slovenia has run a monthly set-off since 1991. In the invoices of the Sardex circuit for 2019, cycles held 23.1% of the invoiced value, and a credit line of 2% of turnover brought close to half of the invoices to settlement.
- Voucher systems leave open the record of fulfilment and the procedure on default. Both are parts of the design's record.
- One experiment with persons found cooperation stronger with tokens than with a public record of past actions. The design's substitutes are commitments with a route for claims and limits under guard, and their effect is to be measured.
- The lending of an agent by an individual to a community's tasks, under a mandate with ceilings and a handover, was absent from what the search reached. Providers of models give agent time to maintainers of software, and projects have closed channels to unrequested generated work.
- A study in which records and capable agents retired a currency was absent from what the search reached. The central question of the request is open.

## 6. The Course of the Work

The Foundations paper was read in full and the other six through structured digests prepared by separate reading sessions. A usage limit stopped those sessions and the searches of existing work in the morning; the digests had been written to disk and the work resumed in the afternoon. A search session then inventoried the pieces of the infrastructure that the design reuses, with their paths and public names. The sketch was written and run, the three documents were written from its results, and the fourteen figures were drawn, rendered and inspected; seven of them had a label on a line or a box in the first rendering and were corrected.

The three searches of existing work were then made again, one session each under a bounded budget: community economies and contribution accounting first, then protocols for agents, then governance, norms and populations of agents. Their texts are kept with the state of each statement, and their findings were entered in the design (Sections 2.9, 5.6, 6.9 and 8.7), in the account of existing work (Sections 11 to 13) and in the implementation on the same day. A second usage limit stopped the work for some hours in the evening; the third session had written its file before it was stopped. The surveys report events of the second half of 2026, which lie beyond what the drafting session knew and rest on the survey sessions' reading.

The user's three papers on distributed monetary dissolution were out of reach of the session and are known here through the Commitments paper's account of them. The paper on relational currency was read in its section on dissolution.

## 7. Questions for the User, and the Answers

1. The names: Koinonia for the work, thesmos for the foundation, `communities` for the domain; and the word mandate, which a payments protocol for agents uses in a narrower sense. *The domain is named `communities` at the user's word; thesmos is kept as the foundation's working name; the rest is open.*
2. The placement. *Answered: a new foundation for institutions (ADR 0167).*
3. The standing of operatives in the presets: an operative's assessment as evidence, with review reaching a person. *The user asked what it means; explained in the hand-over of 2026-11-25; open.*
4. A first league with a short charter from the start, and its content. *Answered: "generate one". The first league and its charter of ten articles are in the domain ([charter](../../domains/communities/presets/charter.md)), admitted by thesmos.*
5. The first community: a commons that checks the data of this infrastructure that a language model drafted and nobody has verified. *Answered: "ok".*
6. The steward of the ground and of the reference implementation. *Open.*
7. The reading of "dissolution". *Answered by the user's hypothesis (the design, Section 6.10), which the jar puts to a construction.*
8. An independent reading for error. *Answered: "better". The reading found seventeen problems, one an error in Proposition 6; all were corrected the same day ([the reading](2026-11-25-a-society-of-human-and-ai-communities/review/reading-for-error.md)).*

## 8. Work Outstanding

- The opening of the sources that the account of existing work marks S or M, and of everything in its sections written from memory.
- The user's three papers on distributed monetary dissolution, which the session could reach only through another paper's account.
- Stage 1 is done: thesmos's eight modules in C++ (68 checks), the schema of the ground answered by kategoria, the worked record run on the constitution's own written form. Stage 2 has begun: the domain's presets, circulation, lending and jar (34 checks).
- On 2026-11-26: thesmos's `crossings` (acts between chronicles with receipts, heads witnessed by notaries, exit with the record; 78 checks in all), the domain's `returns` and `federation` (a league run on the first league's constitution; 51 checks), and the API's composition with its own engine (13 checks).
- Then: recusal in thesmos (a party to a claim may not answer it; 79 checks), the domain's `claims` and the subdomain's `capacity` (63 checks).
- Then: thesmos's `statements`, rules written in words read by a grammar in BNF and measured as norms over a chronicle, a constitution's own rules among them (86 checks), and at the API's edge (15 checks).
- Next: custom and precedent; the jar with prices, savings and the cost of the record; the drawings; the work and a first living commons, at the user's word.
