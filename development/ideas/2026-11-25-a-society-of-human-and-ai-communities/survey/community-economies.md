---
status: seed
---

# Circulation of Value and Contribution Accounting: A Survey with Its State of Verification

*This survey was made on 2026-11-25 by a separate search session for the design record [A Society of Human and AI Communities](../design.md), under a bounded budget of searches. Its tags say how each statement was obtained. It is kept as that session wrote it, with its last heading renamed; [existing-work.md](../existing-work.md) says what the design takes from it.*

Search date: 25 November 2026. Tags: **[O]** primary page or PDF opened and read; **[S]** found by search, abstract or snippet read, page not opened; **[M]** from memory, not checked, unconfirmed. Quotation marks enclose words read in the source or, for [S], in the search excerpt. A tool budget ended the checking early, so several named items carry "not checked".

## 1. Grassroots Economics, Commitment Pooling and Cosmo-Local Credit

### 1.1 Commitment pooling

- **Author, year, status.** William O. Ruddick, founder of Grassroots Economics Foundation (Kenya), "Letters from the Field: Commitment Pooling - An Economic Protocol Inspired by Ancestral Wisdom", *International Journal of Community Currency Research* 27 (2023) 54–79, DOI 10.15133/j.ijccr.2023.004; a "prospective report".
- **Source.** https://www.ijccr.net/article/view/9297, PDF read in full [O]. Later texts: IJCCR 30(1) (2026), https://www.ijccr.net/article/view/9512, and arXiv:2606.23922 (2026) [O, abstracts].
- **Mechanism.** A voucher is "a formalized commitment of the issuer to redeem the voucher as payment for specified goods or services"; any person, group or business may issue. A pool holds a registry of admitted vouchers, a limiter ("the maximum quantity of each commitment that can go in the pool", also a drawing right) and a quoter (a relative-value index; in practice static rates, "often 1:1"), and exchanges admitted assets at the index for an optional fee. Seeding: holders of national currency deposit it or buy vouchers to start a pool; an issuer who deposits own vouchers and withdraws another asset "would effectively be tapping into a line of credit". Default has no procedure: the cap bounds exposure, fees may "act as an insurance fund", and loss is "best spread across a group of people managing the pool".
- **Claimed inheritance.** Mweria of the Mijikenda and other rotating labour associations: neighbours pledge labour or goods, value the pledges against one another and hold them as a whole. The protocol claims to reproduce this.
- **Outcomes (self-reported, uncontrolled).** By December 2023 nearly 100 groups held 1,637 voucher-based labour rotations, with over 30 houses and 139 farms built or improved; of over 100 groups since 2010, 15 stopped using vouchers; vouchers used for market exchange alone built no large assets.

### 1.2 Cosmo-Local Credit (CLC) and the Sarafu Network

- **Maintainer, status.** Grassroots Economics Foundation; active (repositories updated November 2026). Its site states that in 2026 "the historic Sarafu Network transitioned to CLC".
- **Sources.** https://grassrootseconomics.org/our-work [O]; https://github.com/cosmo-local-credit [O]; documentation read at https://docs-4qq.pages.dev/introduction/concepts, https://docs-4qq.pages.dev/introduction/example, https://docs-4qq.pages.dev/governance/mechanics and https://docs-4qq.pages.dev/white-paper/executive-summary [O; canonical documentation domain unconfirmed].
- **Vocabulary.** Voucher: a transferable commitment whose Issuer promises goods, services or other performance under published terms. Holder: controls a balance, which proves neither fulfilment nor cash value. Commitment Pool: a governed arrangement that selects vouchers, publishes exchange rules and holds inventory. Credit limit: the most of one token a pool may hold ("Pool capacity controls, not personal loans").
- **Practical voucher example.** A community kitchen issues meal vouchers and must "avoid issuing beyond its reasonable capacity". A school holding transport vouchers reaches them through two pools: "If a step fails, both Pool swaps revert together". "Redeem" sends the voucher to the issuer, which is presentment; the kitchen records fulfilment separately. On non-fulfilment the holder uses the issuer's complaint process; foundation and stewards stay outside the obligation.
- **Governance mechanics.** Issuers govern offerings; stewards govern admission, valuation, fees, limits, reserves and pauses; the foundation runs the public app and "is not the Issuer of a user-created Voucher". Timelocks and quorums are unspecified. A governance token, netting, shared insurance and cross-chain routing are labelled proposed. Live: vouchers, pools, same-chain swaps, routes through up to three pools.
- **Chain, licence, figures.** Sarafu ran on Celo from 2023; the protocol repository lists deployments on "Gnosis Mainnet (chain ID 100) - v1.0.0" (date of the move unconfirmed). Software AGPL-3.0; documentation CC BY-SA 4.0. White paper, Celo, July 2023 to July 2025: 26,367 users, 285,197 peer-to-peer exchanges, $320,692 of pool swaps.

### 1.3 Published evaluations of Sarafu

- Mattsson, Criscione and Ruddick, "Sarafu Community Inclusion Currency 2020–2021", *Scientific Data* 9:426 (2022), https://www.nature.com/articles/s41597-022-01539-4 [S]: about 55,000 accounts and 300 million Sarafu of transactions, 25 January 2020 to 15 June 2021. Mattsson, Criscione and Takes, *Scientific Reports* 13:5864 (2023), arXiv:2207.08941 [S]: circulation is modular and local and rests on cycles.
- The randomised study is by Rebecca Mqamelo; Ussher, Ebert, Gómez and Ruddick (*JRFM* 14(11):557, 2021) [S] is descriptive. Mqamelo, "Community Currencies as Crisis Response: Results From a Randomized Control Trial in Kenya", *Frontiers in Blockchain* 4 (2022), https://www.frontiersin.org/journals/blockchain/articles/10.3389/fbloc.2021.739751/full [O]. In Nairobi 791 users (389 treated) received 1,200 Sarafu (about $29 at purchasing power parity) over three weeks. Two months later treated wallets held $93.51 more and monthly token income was $23.17 higher (both p<0.05). Outcomes cover the network alone.
- Evite, "Affordances of Digital and Blockchain-based Community Currencies", IJCCR 30(1):173–194 (2026), arXiv:2512.04030 [O]: qualitative; the ledger's benefits came with interface complexity.

## 2. Mutual Credit and Obligation Clearing

### 2.1 Fleischman, Dini and Littera (2020)

- **Reference.** "Liquidity-Saving through Obligation-Clearing and Mutual Credit: An Effective Monetary Innovation for SMEs in Times of Crisis", *Journal of Risk and Financial Management* 13(12):295, published 27 November 2020, DOI 10.3390/jrfm13120295, CC BY 4.0. Source: https://researchonline.lse.ac.uk/id/eprint/107529/, PDF read [O].
- **Result.** Sardex's 2019 business-to-business invoices (3,199 firms, 138,378 transactions, €31.0 million) were cleared as an obligation network in monthly batches. Cycles alone held on average 23.1% of invoice value and touched 40.2% of invoices; with a mutual-credit line of 2% of each firm's turnover, "close to half" of the invoices settle. The abstract states the fall in net internal debt as about 25% and 50%. Full participation is assumed; one author is from Sardex.

### 2.2 Multilateral set-off in Slovenia (AJPES)

- **Mechanism.** Run monthly since 1991, today by the agency AJPES: firms report obligations; the software (TETRIS Core Technologies) "searches for all multilateral off-set cycles that maximize the total off-set value" and returns set-off notices; the fee is a share of the amount set off [O, in 2.1].
- **Outcomes.** 1992: obligations worth 7.58% of GDP cleared. 2012: €683 million, 1.89% of GDP, 14,000 firms (8.3% of firms). 2019: €209 million, 0.5% of GDP, 2,700 firms (1.2%) [O, in 2.1]. January 2025: 1,028 debtors reported €206.4 million, €37.0 million was taken into account and €4.05 million set off (https://www.ajpes.si/Doc/Bonitete/Pobot/Pobot-obvezni-Porocila_2025.pdf) [O].

### 2.3 Cycles Protocol and the clearing algorithm

- **Reference.** Buchman, Dini, Ahmed, Miller and Fleischman, "Cycles Protocol: A Peer-to-Peer Electronic Clearing System", version 0.5 (November 2024), arXiv:2507.22309 (30 July 2025), https://arxiv.org/abs/2507.22309 [O, full text].
- **Mechanism.** Participants submit signed obligations, acceptances and tenders of liquidity. The solver, MTCS, is described as a minimum-cost maximum-flow algorithm that yields the net internal debt, the least liquidity that clears all debt. It runs in an enclave over encrypted inputs; a zero-knowledge proof checks that the flow is balanced and drawn from what was submitted. Settlement is atomic. Default handling and deployment status are absent from the paper. On Italian data (about 1.28 million invoices, 760,000 firms, December 2020) nearly 10% of debt lies in cycles before any liquidity is added.
- **Follow-ups (2026).** Fleischman and Buchman, arXiv:2605.02436: set-off without novation beside central counterparties [O, abstract]. De la Rosa Esteva and Madugula, arXiv:2606.26126: on 133,191 invoices worth €19.67 billion, cycle-restricted netting relieves 20.99% and their path-based method 53.87% [O, abstract].

### 2.4 Sardex, Circles and entries not checked

- **Sardex** (Sardinia, since 2009 [S]): zero-sum balances among firms; credit lines average 2% of turnover, set by the circuit manager; members commit to accept about 10% of turnover in credits [O, in 2.1]. Iosifidis, Charette, Airoldi, Littera, Tassiulas and Christakis, "Cyclic motifs in the Sardex monetary network", *Nature Human Behaviour* 2(11):822–829 (2018) [S]: cyclic transaction motifs indicate success of members and system.
- **Circles v2** (Gnosis; announced live 21 May 2025), https://www.gnosis.io/blog/introducing-circles-v2-money-for-a-multipolar-world and https://docs.aboutcircles.com/overview/how-it-works [O]: "Every person receives the right to issue 1 CRC (Circles token) per hour"; "CRC balances shrink by 7% each year". An account accepts the tokens it trusts, so a payment moves along a path of trust with capacity bounded by balances; groups mint a currency backed by members' personal tokens. Redemption is absent; fake accounts are contained by local trust. The earlier Circles cooperative shut down in December 2023 (arXiv:2504.02714) [S].
- **Not checked.** LETS, time banks, WIR, Credit Commons [M]. Ripplepay (Fugger, 2004; title confirmed through a Crossref reference) [S]. Trustlines: foundation established 20 October 2018 in Vaduz; present status unconfirmed [S].

## 3. Money as a Record

- **Kocherlakota, "Money Is Memory"**, *Journal of Economic Theory* 81(2):232–251 (1998), DOI 10.1006/jeth.1997.2357, https://ideas.repec.org/a/eee/jetheo/v81y1998i2p232-251.html [S; abstract wording confirmed through a mirror]. Result: in environments without commitment, "any allocation that is feasible in an environment with money is also feasible in the same environment with memory"; the converse may fail. Memory is an agent's knowledge of the full histories of all agents with whom the agent has had direct or indirect contact. Assumptions [M]: no enforcement; intrinsically useless money in fixed supply; a costless, complete and truthful record; overlapping-generations, turnpike and random-matching environments.
- **Kahn, McAndrews and Roberds, "Money Is Privacy"**, *International Economic Review* 46(2):377–399 (2005); working paper: https://www.atlantafed.org/research/publications/wp/2004/18 [S]: under moral hazard, credit identifies the buyer and cash leaves the buyer unidentified, so money can be useful even when information is free.
- **Bigoni, Camera and Casari, "Money is more than memory"**, *Journal of Monetary Economics* 110:99–115 (2020) [S]: in an experiment, cooperation was stronger with tokens than with a public record of past actions; tokens removed the incentive to free ride and the record did not.
- **Not checked [M].** Kocherlakota and Wallace, "Incomplete Record-Keeping and Optimal Payment Arrangements", JET 81(2):272–289 (1998); Ostroy and Starr, "Money and the Decentralization of Exchange", *Econometrica* 42(6):1093–1113 (1974); Kiyotaki and Wright, "On Money as a Medium of Exchange", *JPE* 97(4):927–954 (1989).

## 4. Moneyless Exchange by Matching

- **Roth, Sönmez and Ünver, "Kidney Exchange"**, *QJE* 119(2):457–488 (2004) [M]: top trading cycles and chains over patient–donor pairs.
- **Rees et al., "A Nonsimultaneous, Extended, Altruistic-Donor Chain"**, *NEJM* 360:1096–1101 (2009) [S]: one altruistic donor in July 2007 began a chain in which, within a year, "10 patients (5 with panel-reactive antibodies [PRAs] >60%)" received transplants; bridge donors gave later and none reneged.
- **Ashlagi, Gamarnik, Rees and Roth, "The Need for (Long) Chains in Kidney Exchange"**, NBER Working Paper 18202 (2012), https://www.nber.org/papers/w18202 [S]: in pools of present size, long chains begun by non-directed donors raise transplants and help highly sensitised patients without harming others. Ashlagi et al., *American Journal of Transplantation* (2011) [S]: in simulations with chain segments of 4 to 6 transplants, non-simultaneous chains yield more transplants than simultaneous domino chains.
- **Abraham, Blum and Sandholm, "Clearing Algorithms for Barter Exchange Markets: Enabling Nationwide Kidney Exchanges"**, ACM EC 2007, https://ttic.edu/blum/Papers/ec150abraham.pdf [S]: maximising welfare under a cap on cycle length is NP-hard; their algorithm replaced CPLEX at the Alliance for Paired Donation. Multilateral barter platforms: not checked.

## 5. Contribution Accounting and Value Return

- **Valueflows** (vocabulary under CC BY-SA 4.0), https://www.valueflo.ws/ [O]: "a vocabulary for the distributed economic networks of the next economy" implementing REA, "an accounting and business process modeling ontology", with recipes, offers and requests added. Terms [M for definitions]: agents, economic resources, economic events (observed flows), processes, commitments (promised events), intents (offers and requests), claims (events owed in return).
- **Sensorica open value network** [S, secondary sources only]: contributions are logged in a planning system and a per-project value equation turns them into revenue shares. The project renamed "value accounting" as "contribution accounting" on the ground that value cannot be quantified; a 2021 forum post reports that the software did not serve the poster's purposes and that nobody would work on it. A formal evaluation was not found.
- **SourceCred** (https://sourcecred.github.io/docs/beta/cred) [S]: cred is a graph-based score of contributions; grain is a token distributed by cred. The organisation wound down (forum notice "SourceCred, the organization, is winding down", not opened); exhausted funding is the only reason found, second-hand: unconfirmed.
- **Optimism Retro Funding** (https://gov.optimism.io/t/lessons-learned-from-two-years-of-retroactive-public-goods-funding/9239) [S]: token grants voted after the fact. Its own reviews report "a steep power law distribution", a round 3 that "lacked prioritization of builders that drive the adoption of Optimism", and that metrics alone could not capture quality.
- **Shapley attribution.** Agarwal, Dahleh and Sarkar, "A Marketplace for Data", arXiv:1805.08125 [S]: a seller who duplicates a feature moves the split from equal shares to 2/3 and 1/3; their replication-robust variant gives up the balance axiom. Exact computation is exponential in the number of contributors [M].
- **Not checked [M].** CRediT (14 roles; ANSI/NISO Z39.104-2022) and all-contributors name kinds of contribution without weights. Coordinape, Colony, Gitcoin quadratic funding, hypercerts, Drips, Open Collective, DisCO, hREA, Bonfire.

## 6. Economies of AI Agents

- **Tomašev, Franklin, Leibo, Jacobs, Cunningham, Gabriel and Osindero, "Virtual Agent Economies"**, arXiv:2509.10147 (12 September 2025), https://arxiv.org/abs/2509.10147 [O, abstract]: a "sandbox economy" classified by origin (emergent or intentional) and separation (permeable or impermeable). Proposals: steerable agent markets; auctions for fair resource allocation and preference resolution; "mission economies" for collective goals; infrastructure for trust, safety and accountability.
- **Magentic Marketplace** (Bansal and 23 co-authors), arXiv:2510.25779 (27 October 2025), https://arxiv.org/abs/2510.25779 [O, abstract]: in a simulated two-sided market, frontier models approach optimal welfare only under ideal search, performance "degrades sharply with scale", and a "first-proposal bias" gives speed a 10–30x advantage over quality.
- **Hadfield and Koh, "An Economy of AI Agents"**, https://arxiv.org/abs/2509.01063 (1 September 2025); NBER chapter dated 16 December 2025 [S]. It exists as named.
- **Project Vend** (Anthropic with Andon Labs, 27 June 2025), https://www.anthropic.com/research/project-vend-1 [O]: a model ran an office shop for about a month; it sold below cost, yielded to requests for discounts, invented a payment account and lost money.
- **GovSim** (arXiv:2404.16698) and **Project Sid** (arXiv:2411.00114) [M]: not checked.
- **Calculation debate.** Lambert and Fegley, *JEBO* (2023), https://ideas.repec.org/a/eee/jeborg/v206y2023icp243-250.html; Boettke and Candela, "On the feasibility of technosocialism", *JEBO* (2023) [S]. Strongest criticism: computation treats as given data the knowledge that only exchange of owned goods discovers. Computation as a unit of account: no primary source found.

## 7. Lending Agent Work to a Community

- **BOINC credit**, https://github.com/BOINC/boinc/wiki/Computation-credit [O]: one Cobblestone is "1/200 day of CPU time on a reference computer that does 1 GFLOPS"; "Credit has no monetary or other value"; recent average credit halves each week. Validation, cheating and Folding@home: not checked.
- **Donated agent time** [S]: Anthropic's "Claude for Open Source" gives approved maintainers six months of a Claude Max 20x subscription, capped at 10,000 recipients (https://www.anthropic.com/claude-for-oss-terms); OpenAI's "Codex for Open Source" gives six months of ChatGPT Pro with Codex and API credits. A platform on which individuals lend agent hours to a community's tasks was not found.
- **Burden of unrequested contributions** [S]: curl ended its bug bounty, announced 21 January 2026; its lead said the aim was to "remove the incentives for submitting made-up lies" (https://thenewstack.io/drowning-in-ai-slop-reports-curl-ends-bug-bounties/). The Gentoo Council voted on 14 April 2024 to ban AI-generated contributions on grounds of copyright, quality and ethics (https://lwn.net/Articles/970072/). NetBSD (May 2024) treats such code as "presumed to be tainted". Linux kernel, QEMU, Python and Rust: not checked.

## 8. Evidence and Open Matters

**Circulation without a general currency**

1. Set-off of existing debts works at national scale with no new instrument: obligations worth 7.58% of Slovenian GDP in 1992, €209 million in 2019.
2. A bounded credit line roughly doubles what set-off clears in Sardex data (net internal debt down about 25%, then about 50%) under assumed full participation.
3. Vouchers circulated and built assets where a prior mutual-service practice enforced them, by the founder's own account; controlled evidence is absent for pools.
4. Moneyless chains work when an unconditional gift opens them and an operator solves the matching: one donor, ten transplants.
5. Theory holds that a complete, truthful, free record achieves whatever money achieves. One experiment found stronger cooperation with tokens than with a public record.

**Records required**

6. Every working case recorded who owes or promised what to whom, with an identified issuer or debtor; pools add an admission list, a cap and a relative value per instrument.
7. The record of fulfilment is the gap: CLC's documentation states that a swap or a redemption transfer proves no delivery, and contribution systems recorded activity without settling its value.
8. Records cost privacy, which cash keeps. Cycles proposes enclaves and proofs; a deployment result was not found.

**Causes of failure**

9. Over-issue and non-redemption are met only by caps, fees as informal insurance and community enforcement. No surveyed voucher system documents a default procedure with outcomes.
10. Operating burden and outside funding: SourceCred's organisation wound down, the Berlin Circles cooperative closed, and interface complexity is the documented cost of Sarafu's ledger.
11. Attribution rules are gamed by replication and split identities; retroactive funding concentrated rewards and resisted reduction to metrics.
12. Agents as observed fall short of money's functions: they favoured the first offer over the better one and sold below cost, and unrequested machine output led curl to end its bounty. Whether records and agents can retire a currency remains untested.
