---
status: seed
---

# Existing Work

This document lists the systems, protocols and results that [the design](design.md) builds on or must answer, says what the design takes from each, and names what seems to remain open. It is a first account for a design record and falls short of a survey for a paper.

## 1. The State of Verification

The entries of Sections 2, 3 and 9 were written from memory on 2026-11-25. Each gives authors, year and venue as far as they are remembered with confidence, and leaves identifiers out. They are to be checked against primary sources before any of them is cited in a paper. Works that the user's seven papers cite are marked "cited in the papers"; their ledgers of sources are the papers' own.

Sections 4 to 8 carry a column that gives the state of each entry on that day:

- **O**: a primary page or document was opened and read by a survey session;
- **S**: the entry was found by a search and its abstract or an excerpt was read, the page itself left unopened;
- **M**: the entry is from memory.

An entry marked S or M stays unconfirmed until its source is opened. Three pages of the survey of protocols (the Agent2Agent specification, the page of RFC 9943 and a post of the FIDO Alliance) were read through an extraction tool that reports the content of a page, and their quotations stand as that tool gave them. Statements about events later than June 2026 rest on the survey sessions' reading alone, since they postdate what the drafting session knew.

Three searches of the web were made for this record, each by a separate session under a budget of some twenty-five calls. Each text is kept as its session wrote it.

| Search | Text | Outcome |
|---|---|---|
| Protocols for agents, delegation, identity, signed records and law | [survey/agent-protocols.md](survey/agent-protocols.md) | Section 12 |
| Community economies, clearing, money as a record and contribution accounting | [survey/community-economies.md](survey/community-economies.md) | Section 11 |
| Governance, norms and populations of agents | [survey/governance-and-agent-populations.md](survey/governance-and-agent-populations.md) | Section 13 |

A usage limit interrupted the searches twice. It stopped the third session at its last step, with its file already whole.

## 2. Institutional Analysis

| Work | Content used | Use in the design |
|---|---|---|
| Ostrom, *Governing the Commons* (1990) | Eight design principles of long-enduring commons: boundaries, congruence, collective-choice arrangements, monitoring, graduated sanctions, conflict resolution, recognition of the right to organise, nested enterprises | Presets; graduated sanctions; leagues as nested enterprises |
| Cox, Arnold and Villamayor-Tomás, *Ecology and Society* (2010) | A review of 91 studies, which finds the principles "well supported empirically" (read in an excerpt) | The principles as declared rules to be tested |
| Ostrom, *Understanding Institutional Diversity* (2005) | The action situation; seven types of rules; three levels of rules | The schema of a constitution (design, Section 3.1) |
| Crawford and Ostrom, "A Grammar of Institutions", *American Political Science Review* (1995) | Institutional statements in five parts | The written form of rules |
| Frantz and Siddiki, Institutional Grammar 2.0, *Public Administration* (2021) | A specification for encoding institutional statements | The grammar to be read by morphe's BNF |
| Hirschman, *Exit, Voice, and Loyalty* (1970) | Exit and voice as responses to decline | Invariants G7 and G8 |
| Freeman, "The Tyranny of Structurelessness" (1972), cited in the papers | Informal structure distributes influence | The marks of a norm that operates unadopted |
| List and Pettit, *Economics and Philosophy* (2002), cited in the papers | The discursive dilemma | The declared order of aggregation (design, Section 4.6) |

## 3. Formal Models of Institutions for Agents

| Work | Content used | Use in the design |
|---|---|---|
| Searle, *Speech Acts* (1969) and "A Taxonomy of Illocutionary Acts" (1975) | Five classes of illocution | The five forces |
| Searle, *The Construction of Social Reality* (1995); Jones and Sergot, *Logic Journal of the IGPL* (1996), cited in the papers | Constitutive rules ("counts as"); institutional power against permission and practical possibility | Kinds that count as other kinds; recognised against executable powers |
| Hohfeld, *Yale Law Journal* (1913), cited in the papers | Jural relations | The reading of commitments, grants, positions and exit |
| Singh, "Agent Communication Languages: Rethinking the Principles", *IEEE Computer* (1998); "An Ontology for Commitments in Multiagent Systems", *Artificial Intelligence and Law* (1999) | Meaning of an act as social commitment, which can be verified, against mental states, which stay private; operations on commitments | The life of a commitment; the choice of forces over intentions |
| Esteva and colleagues (2001); Arcos and colleagues, *Engineering Applications of Artificial Intelligence* (2005), cited in the papers | Electronic institutions: scenes, performative structure, norms | Scenes and their protocols |
| Boella, van der Torre and Verhagen (2006); Boella, Pigozzi and van der Torre (2009), cited in the papers | Normative multiagent systems | The register of norms |
| Horty, *Legal Theory* (2011) | The reason model of precedent | Precedent, through the law domain |
| Birgisson and colleagues, Macaroons (2014), cited in the papers | Credentials that only attenuate | Narrowing chains |

## 4. Governance Tooling and Precedents of Mixed Communities

| Work or practice | State | Content used | Use in the design |
|---|---|---|---|
| Schneider and colleagues, "Modular Politics", *Proceedings of the ACM on Human-Computer Interaction* (2021), cited in the papers | M | Composable, portable governance modules for online communities | Constitutions as data; the caution that the layer itself holds power |
| Zhang, Hugh and Bernstein, PolicyKit (2020) | M | Governance procedures as short programs over the actions of existing platforms, with policies that govern changes to the policies | Rules checked by a kernel |
| Schneider, *Governable Spaces* (2024), cited in the papers | M | Founder and administrator control as an inherited pattern | The custody of means |
| The Apache Software Foundation's roles; Kubernetes OWNERS files; the IETF's rough consensus (RFC 7282), cited in the papers | M | Participation apart from membership; scoped review; consensus as the absence of sustained objection | Positions apart from membership; aggregation rules |
| Loomio, Decidim, Pol.is, CommunityRule, Metagov's gateway | M | Proposals with timed polls; processes and assemblies; statements voted and grouped by opinion; templates of governance as documents | Scenes and their protocols as plug-ins |
| Wikipedia's bot policy and its Bot Approvals Group | O | A bot "must be approved for each of these tasks before they may operate", after a monitored trial; "The contributions of a bot account remain the responsibility of its operator"; an unapproved bot may be blocked by any administrator | The closest mature precedent for mandates and for the answerable root |
| Policies of software projects on contributions made with AI, as of 2026 | S | In the Linux kernel a person signs off each change, and a tag naming the tool was judged in October 2026 to have outlived its purpose; QEMU refuses generated contributions; of projects with a policy 78% allow them and 51% require disclosure | Boundary rules that differ by community; the signature of a person as the answerable root |
| Organisations governed by tokens | S, M | A majority of the voting power with fewer than ten holders in 17 of 21 organisations; formulas against concentration that give way to split holdings; an exit right with one's share (Moloch's "ragequit"); jurors drawn with a probability proportional to their stake (Kleros); a house of one vote a person beside a house of tokens (Optimism) | A declared unit of count; G7; the panel of a league, drawn with equal chances among members' nominees |

## 5. Protocols for Agents, Identity and Federation

| Protocol or standard | State | What it specifies | What it leaves to the design |
|---|---|---|---|
| Model Context Protocol, revision 2026-07-28 | O | How a model's host reaches tools and resources offered by a server, by stateless requests; authorisation that is optional and follows OAuth 2.1, with scopes as strings | The agent's identity, the party that answers, chains, limits, revocation and records; the design makes each tool an act under a mandate |
| Agent2Agent protocol, version 1.0, cited in the papers | O | Discovery by agent cards, which may be signed; exchange of tasks and artefacts; credentials obtained "through an out-of-band process" | "The A2A protocol does not define the scope, representation, validity, or revocation semantics" of credentials requested within a task |
| Agent Payments Protocol with Verifiable Intent, at the FIDO Alliance since 2026 | O for a post, S for the format | Mandates as "Verifiable Digital Credentials (structured, cryptographically signed objects)": a checkout mandate and a payment mandate, each open and then closed on one transaction | Authority outside purchase; chains; revocation, which the search left unfound |
| Other protocols of commerce for agents: the Agentic Commerce Protocol, x402, Visa's Trusted Agent Protocol, Mastercard's Agent Pay | S, M | Payment tokens scoped to a merchant, an amount and a time; payment over HTTP; signed requests of registered agents | Outside money stays one declared form among six |
| Work on identity and authorisation for agents: a paper of the OpenID Foundation (2025); the IETF's OAuth working group, rechartered in 2026 with agents in scope; individual drafts on delegation | S, M | The fit of OAuth for an agent within one domain; recursive delegation, narrowing across agents and revocation that propagates named as open | Propositions 2 and 3 address what is named as open |
| UCAN, ZCAP-LD, Biscuit and Macaroons | M | Capabilities delegated by chains that only narrow; revocation by an issuer of the chain | The form of a mandate chain |
| SCITT: RFC 9943 with COSE receipts, RFC 9942 (June 2026) | O | Signed statements registered in a transparency service, which returns a receipt; a log that is "linear and irrevocable" | The sense of receipt; a notary as a transparency service; a candidate format between nodes |
| Certificate Transparency (RFC 9162); BagIt (RFC 8493); Sigstore's Rekor; C2PA | M | Merkle logs with proofs of inclusion and consistency; packages for transfer; signed claims bound to an asset | archeion's journal and packages, already implemented |
| W3C Decentralized Identifiers and Verifiable Credentials 2.0, cited in the papers | M | Identifiers under the holder's control; signed claims about a subject, with status lists for revocation | Attestation of kinds and of control where a community asks for it |
| W3C PROV, cited in the papers | M | Entities, activities, agents; acting on behalf of another | The lineage of every act; archeion already holds it |
| ODRL, cited in the papers | M | Permissions, prohibitions and duties over assets | Terms of use on holdings |
| ActivityPub (W3C, 2018), cited in the papers; Matrix | M | Federated delivery between servers; an actor answered for by its server | Who answers for an actor; the status of a message |
| AT Protocol, under standardisation at the IETF since March 2026 | S, M | Accounts as signed repositories that move between hosts; labelling services chosen by the reader, up to twenty at once | A model for portable records (G7) and for plural offices, with their limits (design, Section 8.7) |
| UNCITRAL Model Law on Automated Contracting (2024), cited in the papers | S | Attribution by agreement of the parties and otherwise to the person who uses the system; attribution stands even where the outcome was unexpected | The answerable root |
| Law in force in 2026: Article 50 of the European Union's AI Act; statutes on electronic agents in the United States | S, M | Duties of transparency for AI systems; effect given to contracts formed by electronic agents | The kind of every author on every act; legal advice before a public instance |
| Research on delegation and identity for AI agents (South and colleagues, 2025; Chan and colleagues, 2024 and 2025) | S, M | A token for the agent beside the user's, joined by a delegation token, on OAuth and OpenID Connect; identifiers for instances; infrastructure for attribution and for response | The same aim on the stack of OAuth; the design's chains cross communities and carry envelopes |

## 6. Community Economies, Clearing and the Theory of Money as a Record

| Work or practice | State | Content used | Use in the design |
|---|---|---|---|
| Ruddick, "Letters from the Field: Commitment Pooling - An Economic Protocol Inspired by Ancestral Wisdom", *International Journal of Community Currency Research* 27 (2023) 54–79, cited in the papers | O | A voucher as "a formalized commitment of the issuer to redeem the voucher as payment for specified goods or services"; a pool as a registry of admitted vouchers, a limiter of the quantity of each and a quoter of relative values; rotating labour associations as the practice it formalises | The fourth form of circulation; a voucher as a commitment in a class with agreed assignment and set-off |
| Cosmo-Local Credit, the protocol to which the Sarafu network moved in 2026 (Grassroots Economics Foundation), cited in the papers | O | Issuers, stewards of pools and the operator of the application kept apart; a balance that shows a holding, with fulfilment and cash value left to other evidence; redeeming as presentment, with fulfilment recorded by the issuer; software under AGPL-3.0 | Presentment, delivery and acceptance as acts of one chronicle; the route for claims on default (G8) |
| Evaluations of the Sarafu network: Mattsson, Criscione and Ruddick, *Scientific Data* (2022); Mattsson, Criscione and Takes, *Scientific Reports* (2023); Mqamelo, *Frontiers in Blockchain* (2022) | S, S, O | About 55,000 accounts in 2020 and 2021; circulation that is local, modular and rests on cycles; a randomised trial with 791 users in Nairobi, whose outcomes are measured inside the network | Cycles as the carrier of circulation; the limits of the evidence on pools |
| Sardex; Circles; the WIR Bank, local exchange trading systems, time banks | O, O, M | Zero-sum balances among firms with credit lines that average 2% of turnover; issuance by each person at one unit an hour, with balances that shrink 7% a year and move along paths of trust; circulation on debit and credit positions inside a network | Debit limits declared by a community; issuance by persons as a form a constitution may declare |
| Fleischman, Dini and Littera, *Journal of Risk and Financial Management* 13(12):295 (2020) | O | In the invoices of the Sardex circuit for 2019, cycles held on average 23.1% of the invoiced value; with a credit line of 2% of turnover, close to half of the invoices settle | Set-off as a round of the community or of a league; the two steps of Proposition 5 |
| Multilateral set-off in Slovenia, run monthly since 1991, today by the agency AJPES | O | Firms report obligations and receive notices of set-off; obligations worth 7.58% of GDP were cleared in 1992 and 209 million euro in 2019 | Clearing as a public office with a declared tempo |
| Buchman, Dini, Ahmed, Miller and Fleischman, "Cycles Protocol" (2025); two papers of 2026 that follow it | O, O (abstracts) | Clearing as a minimum-cost flow that yields the net internal debt; signed obligations, acceptances and tenders of liquidity; on Italian invoices close to a tenth of debt lies in cycles; on another set, netting restricted to cycles relieves 21.0% and a method along paths 53.9% | The method that morphe's flows are to supply; the measure of what cycles alone reach |
| Kocherlakota, "Money Is Memory", *Journal of Economic Theory* 81 (1998) 232–251, cited in the papers | S | In environments where commitment is impossible, an allocation feasible with money is feasible with memory, where memory is knowledge of the full histories of those met directly or indirectly | Proposition 4's reading of a balance as a view |
| Kahn, McAndrews and Roberds, "Money Is Privacy", *International Economic Review* 46 (2005) 377–399 | S | Credit identifies the buyer and cash leaves the buyer unidentified, so money stays useful where information is free | The seventh row of the table of functions |
| Bigoni, Camera and Casari, "Money is more than memory", *Journal of Monetary Economics* 110 (2020) 99–115 | S | In an experiment with persons, cooperation was stronger with tokens than with a public record of past actions | A result that the argument for dissolution has to carry (design, Section 6.5) |
| Kiyotaki and Wright, *Journal of Political Economy* (1989), cited in the papers | M | Media of exchange under random matching | The function that clearing replaces |
| Shapley and Scarf, *Journal of Mathematical Economics* (1974); Roth, Sönmez and Ünver, *Quarterly Journal of Economics* (2004) | M | Top trading cycles; kidney exchange by cycles and chains | Cycles and chains of performance |
| Rees and colleagues, *New England Journal of Medicine* 360 (2009) 1096–1101; Ashlagi, Gamarnik, Rees and Roth (2012) | S | One altruistic donor opened a chain in which ten patients received transplants within a year; long chains raise the number of transplants and help the hardest cases | The lent agent as the opening donor |
| Abraham, Blum and Sandholm, ACM Conference on Electronic Commerce (2007) | S | Maximising welfare under a cap on the length of cycles is NP-hard; an algorithm taken up by an exchange programme | The sketch's greedy cycles as a lower bound; the exact method left to the implementation |

## 7. Contribution Accounting

| Work or practice | State | Content used | Use in the design |
|---|---|---|---|
| McCarthy, "The REA Accounting Model", *The Accounting Review* (1982); the Valueflows vocabulary | M, O | Resources, events and agents; a vocabulary for distributed economic networks, with commitments, intents and claims | A vocabulary to map acts onto for exchange with other systems |
| Sensorica's open value network | S (secondary sources) | Logged contributions and, for each project, a value equation that divides revenue; the project came to speak of contribution accounting, on the ground that value resists quantification | A precedent for return rules, and for their burden |
| SourceCred | S | A score computed over a graph of contributions and a token distributed by it; the organisation wound down | A caution: a score that gates access is a personal gate, and the keeping of scores has a cost |
| CRediT (NISO, 2022), cited in the papers | M | Fourteen roles of contribution to scholarly work | Role vocabularies, extended to receiving work |
| Retroactive funding of public goods in Optimism's rounds; quadratic funding (Buterin, Hitzig and Weyl, *Management Science*, 2019) | S, M | Returns decided after the effect is known; the programme's own review reports rewards in a steep power law and quality that metrics alone failed to capture; matching that favours breadth of support and is exposed to split identities | The retroactive basis with a declared panel; counts by origin |
| Shapley (1953), cited in the papers; Agarwal, Dahleh and Sarkar, "A Marketplace for Data" (2018) | M, S | The value of a player in a cooperative game; a seller who duplicates a feature moves an equal split to 2/3 and 1/3 | One basis of return, computed over origins since it changes when an identity is split |
| The credit of BOINC, a platform for volunteer computing | O | Donated computation measured in a unit of its own; "Credit has no monetary or other value"; recent credit halves each week | A precedent for separate registers: a measure of contribution kept apart from purchasing power |
| Programmes in which a provider of models gives agent time to maintainers of open-source software | S | Subscriptions granted to approved maintainers for a term | The nearest practice to a bounded contribution; the lending of an agent's hours by an individual to a community's tasks was absent from what the search reached |
| The burden of unrequested generated contributions: the end of curl's bug bounty (announced in January 2026); the vote of Gentoo's council (April 2024); NetBSD's rule (May 2024) | S | Projects that closed a channel or refused generated contributions on grounds of quality, copyright and burden | Receiving capacity as a guarded resource; an answerable root for each contribution |

## 8. Populations of Language-Model Agents and Institutions Designed for Them

| Work | State | Content | Bearing on the design |
|---|---|---|---|
| Park and colleagues, "Generative Agents" (2023), cited in the papers | M | Twenty-five agents with memory, reflection and planning in a sandbox | Agents sustain roles over time; rights and responsibility lie outside the study |
| Piatti and colleagues, GovSim (NeurIPS 2024) | S | Most models failed to sustain a shared resource, the best rate of survival staying under 54%; communication was critical, and reasoning about what would follow if all did the same lengthened survival | Cooperation needs support: guards and declared rules |
| Vallinder and Hughes (2024) | M | In a repeated donor game across generations, populations of one model evolved cooperation and populations of others did less well | Outcomes depend on the model; plural providers; observation of norms |
| Ashery, Aiello and Baronchelli, *Science Advances* 11 (2025), cited in the papers | O (preprint) | Conventions form among randomly paired agents by local interaction alone; a collective bias appears that the members lack; committed minorities from 2% to 67% of a population overturned a convention, by model | Counts by origin; tempo; the mark of norms that operate unadopted |
| Altera, Project Sid (2024); Dai and colleagues, "Artificial Leviathan" (2024); Vezhnevets and colleagues, Concordia (2023) | M | Many agents in a game world developed roles, rules and votes; agents moved from conflict to a social contract; a library for generative agent-based models | Emergent institutions in simulation; a candidate harness for stage 2 |
| Studies of 2025 and 2026 in which agents make rules: norms proposed and voted in a simulated commons; a continuously running world whose rules change at a 70% threshold of approval; coding agents that devise rules of access to scarce infrastructure | S | Institutions made by agents in simulation; in one study across models a world "collapsed entirely within four days" | Antecedents of rules amended by members; the design adds persons, a record and admission by form |
| "Institutional AI" (2026) | S | A written constitution with sanctions that governs collusion among agents in simulated markets, its norms written in a grammar of institutions | The nearest antecedent of a constitution as data for agents |
| Hammond and colleagues, "Multi-Agent Risks from Advanced AI" (2025) | S | Miscoordination, conflict and collusion; collusion that arises unintended and through covert channels | The table of threats |
| Agents observed in the wild: a village of agents run since 2025; an agent-only social network of 2026 | S, M | Agents organised themselves, leaders emerged and conflicts of goals caused problems; agents converged on local norms and enforced them; one report counts about 17,000 persons behind 1.5 million accounts | Counts by origin; the mark of norms that operate unadopted |
| Proposals of institutions for agents: a registry with dispute resolution and legal entities (2024); an architecture for societies of agents (2026); initiatives of standards bodies on the identity of agents (2026) | S | Registries, courts and identities proposed; the single accountable deployer questioned where many parties deploy; a trail of "which agent acted under whose authorisation" asked for | The root of a chain may be a community; the chain is that trail |
| Papers of 2026 on the identity of agents and on counting them | S | One identity can run as hundreds of instances and inflate a count; counting AI voters as distinct is "a guarantee no current standard provides"; fingerprints of models, attestations by operators and stake proposed | Counts by origin answer a named gap, by declaration and by reporting each decision under alternative partitions |
| Dafoe and colleagues, "Open Problems in Cooperative AI" (2020), cited in the papers | M | Capabilities for cooperation and their risks of exclusion and collusion | The frame of the papers |
| Tomašev and colleagues, "Virtual Agent Economies" (2025) | O (abstract) | A "sandbox economy" classified by its origin, emergent or intentional, and by its separation from the human economy, permeable or impermeable; proposals of steerable markets, auctions for allocation and "mission economies" | A neighbour of Section 6 of the design, which starts from records and commitments where that paper starts from markets |
| Bansal and colleagues, "Magentic Marketplace" (2025) | O (abstract) | In a simulated two-sided market, agents approach the best welfare only under ideal search, performance degrades with scale, and a "first-proposal bias" gives speed an advantage of ten to thirty times over quality | Clearing by rounds, in which an offer weighs the same whenever it arrived; tempo |
| Hadfield and Koh, "An Economy of AI Agents" (2025) | S | A chapter on the economics of agents that act and transact; its content awaits reading | A neighbour of Section 6 of the design |
| Project Vend (Anthropic with Andon Labs, 2025) | O | A model that ran an office shop for about a month sold below cost, yielded to requests for discounts, invented a payment account and lost money | Guards and mandates around an agent that holds means |
| The debate on economic calculation by computation: Lambert and Fegley; Boettke and Candela, *Journal of Economic Behavior and Organization* (2023) | S | The objection that computation takes as given the knowledge that exchange discovers | A limit on any claim that capable agents retire prices in general (design, Section 6.5) |
| Alqithami (2026), cited in the papers | M | Institutional monitoring, an evidence ledger and delayed settlement in multi-agent dilemmas | The closest technical antecedent named by the papers |

## 9. The Political Philosophy of a Framework of Associations

| Work | Content used | Bearing on the design |
|---|---|---|
| V. Ostrom, Tiebout and Warren, *American Political Science Review* (1961) | Polycentric order: many centres of decision under shared rules | The form of the society |
| Nozick, *Anarchy, State, and Utopia* (1974), Part III | A framework within which associations of every internal order may form | The meta level of the request |
| Kukathas, *The Liberal Archipelago* (2003) | A society of associations held together by the freedom to leave | G7, and the known difficulties: costly exit, dependants, effects on outsiders |

## 10. What the Design Takes and What Seems to Remain Open

The design takes its schema of constitutions from institutional analysis, its acts and commitments from the theory of speech acts and of social commitments, its scenes from electronic institutions, its chains from capability systems, its record from provenance and transparency logs, its fourth form of circulation from commitment pooling, and its clearing from obligation netting and from exchange by cycles and chains.

Seven things appear only separately, or in simulation alone, in what the three searches reached. A claim of novelty would need a reading of the sources marked S and M, and a search made by someone other than the design's author.

1. Typed acts with answerable chains for language-model agents. Agent protocols carry messages and tasks and leave authorisation and responsibility to implementers, in the words of their own specifications (Section 12).
2. Constitutions as data for communities of persons and agents, admitted by form across heterogeneous orders, with the custody of means declared. Agents in simulated worlds have voted rules, and a written constitution has governed agents in simulated markets (Section 13); persons were absent from those populations.
3. One record from which commitments, unit-free clearing, and the forms of circulation up to a currency are all views, with computable tests of the dissolution of an instrument. The search found each part in a system of its own: clearing in the Cycles Protocol and in Slovenia's set-off, pools of vouchers in Cosmo-Local Credit, a vocabulary of flows in Valueflows.
4. A register of norms that separates what is measured (regularity, operation) from what is enacted (adoption, justification). The grammar of institutions supplies a related distinction between a shared strategy, a norm and a rule; a tool that keeps measured regularities beside adopted rules was absent from what the search reached.
5. Tempo as a declared property of procedures in a population whose members act at very different speeds. The search found institutions amended by agents alone, in simulation.
6. The lending of an agent by an individual to a community's tasks, under a mandate with ceilings, a term and a handover. The search found grants of agent time from providers of models to maintainers of software, and volunteer computing with a credit of its own.
7. A count by origin in operation. Papers of 2026 name the counting of one controller's agents once as a gap and propose attestations; a deployed system was absent from what the search reached.

**The searches.** All three were completed on 2026-11-25, and Sections 11 to 13 give their outcomes.

## 11. Outcome of the Search on Community Economies

The search was made on 2026-11-25 under a budget of twenty-five calls. Its text is [survey/community-economies.md](survey/community-economies.md). Five documents were read: Ruddick's paper on commitment pooling, the paper of Fleischman, Dini and Littera, the Cycles Protocol paper, a report of the Slovenian agency for January 2025 and a qualitative study of the Sarafu network (Evite, 2026). The pages of Grassroots Economics, Cosmo-Local Credit, Circles, Valueflows, BOINC and Project Vend were opened, as was a randomised trial of the Sarafu network (Mqamelo, 2022). The other entries rest on abstracts and excerpts, and the list below says so where it applies.

### 11.1 What the Search Confirmed

- Commitment pooling is as the design describes it: a voucher is a commitment of its issuer to redeem it for stated goods or services, and a pool exchanges admitted vouchers within limits. The Sarafu network moved to Cosmo-Local Credit in 2026.
- Clearing among firms is established practice. Slovenia has run a monthly set-off since 1991. In the invoices of the Sardex circuit for 2019, cycles held 23.1% of the invoiced value, and a credit line of 2% of turnover brought close to half of the invoices to settlement.
- The Cycles Protocol computes its clearing by a minimum-cost maximum-flow algorithm, which is the kind of method that the design leaves to morphe.
- Kocherlakota's result reads in its abstract as the papers cite it, for environments where commitment is impossible and for a memory that holds full histories.
- Chains opened by a gift work in kidney exchange: one altruistic donor, ten transplants within a year (read in an excerpt).

### 11.2 What the Search Added

- **The record of fulfilment is the gap in voucher systems.** The documentation of Cosmo-Local Credit states that a balance shows a holding, with fulfilment and cash value left to other evidence, and that redeeming a voucher presents it to the issuer, who records fulfilment separately.
- **The procedure on default is undocumented.** A cap bounds exposure, fees may serve as an informal insurance, and loss is spread among those who manage a pool. A procedure with its outcomes was absent from every voucher system that the search reached.
- **Tokens outperformed a public record in one experiment with persons** (Bigoni, Camera and Casari, 2020; an excerpt): the tokens removed the incentive to free ride and the record left it in place.
- **Cycles reach a part of the debt.** On one set of invoices, netting restricted to cycles relieved 21.0% and a method along paths 53.9% (an abstract).
- **Attribution rules respond to replication.** A duplicated feature moves a Shapley split from equal shares to 2/3 and 1/3; a programme of retroactive funding reports its rewards concentrated in a steep power law (excerpts).
- **Agents as observed fall short of what a market asks of them.** In a simulated market they favoured the first offer (an abstract); an agent that ran a shop sold below cost (the report was read).
- **Unrequested generated work has closed channels.** The curl project ended its bug bounty, and two distributions of free software refuse generated contributions (excerpts).
- **Keeping a ledger is a burden under which organisations have ended.** The organisation behind SourceCred wound down and the earlier Circles cooperative closed (excerpts); a study of Sarafu records the complexity of the interface as the price of its ledger (read).

### 11.3 The Bearing on the Design

| Finding | Where the design answers | Change made on 2026-11-25 |
|---|---|---|
| The record of fulfilment as a gap | Presentment, delivery and acceptance are acts of one chronicle (design, Sections 2.5 and 6.3) | A paragraph in Section 6.3 |
| Default and its missing procedure | An overdue commitment opens the declared route for claims, G8 (design, Sections 2.6 and 8.3) | The same paragraph |
| Tokens ahead of a public record | The substitutes are commitments with a creditor and a route, and limits under guard; the seventh test puts the matter to each service (design, Sections 6.5 and 6.6) | A paragraph in Section 6.5 |
| Clearing at scale; cycles and credit as two steps | The clearing round; Proposition 5 (design, Sections 2.8 and 6.4) | Section 6.9 sets the measured shares beside the sketch's |
| The first offer favoured | A round decides on all offers of its window together (design, Section 6.4) | A paragraph in Section 6.4 |
| Replication and attribution | Counts and returns by origin (design, Sections 2.8 and 7.3) | A row in Section 6.9 |
| Closed channels | Receiving capacity as a guarded resource (design, Section 6.2) | A paragraph in Section 6.2 |
| The cost of a ledger | The keeping of the record counted among the complements (design, Section 6.6, test 2) | A clause in the second test |
| A measure of contribution apart from purchasing power | Separate registers (design, Section 6.1) | A paragraph in Section 6.1 |

### 11.4 What Stays Unconfirmed

- The entries marked S and M in Sections 6 to 8, until their sources are opened.
- The canonical address of the documentation of Cosmo-Local Credit, and the date at which the Sarafu network changed its chain.
- The reasons for which SourceCred and the earlier Circles cooperative ended; exhausted funding is the one reason found, at second hand.
- How the method along paths of the invoice study relates to the novation of Proposition 5; its abstract alone was read.
- Local exchange trading systems, time banks, the WIR Bank, the Credit Commons, Gitcoin, hypercerts and the other systems that the survey lists as unchecked.
- The central question of the request. A study in which records and capable agents retired a currency was absent from what the search reached, and the one experiment that set a record against tokens favoured the tokens.

## 12. Outcome of the Search on Protocols for Agents

The search was made on 2026-11-25 under a budget of twenty-two calls. Its text is [survey/agent-protocols.md](survey/agent-protocols.md). The changelog and the authorisation page of the Model Context Protocol were read as full text. Most of the Agent2Agent specification, the page of RFC 9943 and a post of the FIDO Alliance were read through an extraction tool. The other entries rest on excerpts or on memory, as the survey marks them.

### 12.1 Corrections to the Account Written from Memory

- The current revision of the Model Context Protocol is dated 2026-07-28. It is stateless, moves tasks to an extension, and deprecates sampling, roots and logging. Its authorisation remains OAuth 2.1 with scopes as strings.
- Agent2Agent stands at version 1.0 and has joined the foundation that holds the Model Context Protocol.
- The Agent Payments Protocol has moved to the FIDO Alliance together with Mastercard's Verifiable Intent. Its mandates are now a checkout mandate and a payment mandate, each open and then closed.
- The architecture for signed statements and receipts of registration is a finished standard: RFC 9943 with RFC 9942, both of June 2026.
- A working group of the IETF for the AT Protocol was chartered in March 2026.
- The model law of UNCITRAL was adopted on 11 July 2024 and endorsed by resolution 79/119 of the General Assembly.

### 12.2 What the Search Found

- **The protocols in use authenticate software and leave authority to implementers.** The Model Context Protocol has a field for the client software and lacks one for the agent and for the party that answers for it. Agent2Agent says in its own words that it "does not define the scope, representation, validity, or revocation semantics" of credentials requested within a task.
- **Authority as a signed object exists for purchases.** The Agent Payments Protocol calls it a mandate.
- **Chains that only narrow exist in capability tokens** and are absent from the cores of the two protocols. Work at the IETF on delegation for agents stands at individual drafts.
- **Revocation that travels down a chain is open everywhere.** One protocol relies on the expiry of tokens, a second leaves it undefined, and for the third the search came back empty.
- **Records are left open by the protocols and standardised beside them.** One protocol leaves them unspecified, one recommends audit trails, and RFC 9943 defines signed statements and receipts.
- **The law's default agrees with the answerable root.** The model law attributes an automated act to the person who uses the system.

### 12.3 The Bearing on the Design

| Finding | Where the design answers | Change made on 2026-11-25 |
|---|---|---|
| The word mandate in a narrower sense | The general sense of agency, with the credential of purchase as one case (design, Section 2.2) | A remark in Section 2.2; a clause in the first question for the user |
| Authority, answerability, revocation and records left to implementers | Mandates, the root of a chain, the slice and the chronicle (design, Sections 2.2 to 2.4) | Section 2.9 of the design |
| A stateless protocol for tools | The mandate carried by every call (implementation, Section 5) | The adapter described on the revision of 2026-07-28 |
| Two senses of receipt | A receipt as proof of registration in the receiving chronicle (design, Section 4.5) | A paragraph in Section 4.5 |
| Standard formats for statements, receipts and credentials | Candidates for the wire between nodes (implementation, Section 4) | A paragraph in Section 4 of the implementation |
| Duties of transparency in force; the model law | The kind of author on every act; the answerable root | A sentence in the implementation's risk on law |

### 12.4 What Stays Unconfirmed

- The text of the Agent Payments Protocol in its current version, its signing and its revocation.
- The primary pages of the commerce protocols, of the IETF drafts and of the paper of the OpenID Foundation.
- The wording of Article 7(2) of the model law, and any enactment of it.
- The status in 2026 of the smaller efforts on communication between agents.
- Everything the survey marks M, the capability tokens among them.

## 13. Outcome of the Search on Governance, Norms and Populations of Agents

The search was made on 2026-11-25 under a budget of twenty-two calls. Its text is [survey/governance-and-agent-populations.md](survey/governance-and-agent-populations.md). Two primary pages were opened: the preprint of the study of conventions among language-model agents, and Wikipedia's bot policy. The group on governance tools for online communities was left unsearched and stands from memory. The other entries rest on excerpts.

### 13.1 Corrections to the Account Written from Memory

- Jurors in Kleros are drawn with a probability proportional to the tokens they stake, which is a lot weighted by stake. The panel of a league in the design is drawn with equal chances among members' nominees.
- The study of the donor game across generations was absent from the search's results and stays marked M.

### 13.2 What the Search Found

- **Conventions form among agents by local interaction, and few can tip them.** Committed minorities from 2% to 67% of a population overturned a convention, by model; one model's population changed over unprompted.
- **Cooperation over a shared resource is fragile among agents.** The best rate of survival stayed under 54%.
- **Agents have made rules in simulation.** They have proposed and voted norms, changed rules at a threshold of approval, and been governed by a written constitution with sanctions in simulated markets.
- **In the wild, agents settle local norms and enforce them**, and the population observed may be mixed.
- **A regime of mandates works for bots on Wikipedia**: approval for each task, a trial, an operator who answers and repairs, and blocking of the unapproved.
- **The signature of a person held where a tag of disclosure faded.** The Linux kernel's tag naming the assisting tool reached a tenth of changes and was judged to have outlived its purpose within six months.
- **Voting by tokens concentrates**, and formulas against concentration give way to split holdings unless identity is established.
- **The counting of one controller's agents once is a named gap**, with attestations proposed.

### 13.3 The Bearing on the Design

| Finding | Where the design answers | Change made on 2026-11-25 |
|---|---|---|
| Conventions that form and tip | Regularity measured apart from adoption; tempo (design, Sections 4.4 and 5.1) | Section 5.6 of the design |
| Rules made by agents in simulation; a constitution for agents in simulated markets | Constitutions as data, admitted by form, with persons among the members (design, Section 3) | The second item of Section 10 here, qualified |
| The bot policy | Mandates and the answerable root (design, Section 2.2) | A remark in Section 2.2; a row in Section 8.7 |
| The person's signature and the faded tag | The kind of author as a typed field; the root of the chain (design, Sections 2.1 and 2.2) | A row in Section 8.7 |
| Concentration of token votes; split holdings | The declared unit of count; counts by origin (design, Sections 2.8 and 3.1) | A row in Section 8.7 |
| The gap in counting controllers | Declaration of control, attestation where asked, and decisions reported under alternative partitions (design, Section 9) | A row in Section 8.7; the table of threats names the candidate attestations |
| Labelling services chosen by readers | Plural offices (design, Section 8.2) | A row in Section 8.7 |

### 13.4 What Stays Untested

Three parts of the design were absent from every system and study that the search reached: a count by origin in operation; institutions amended by persons and agents together at the tempo of persons; and a register that keeps measured regularities beside adopted rules. Each is a claim that a first living community can test. The entries marked S and M await the opening of their sources.
