---
status: seed
---

# Norms, Institutions and Governance among Persons and Agents: A Survey with Its State of Verification

*This survey was made on 2026-11-25 by a separate search session for the design record [A Society of Human and AI Communities](../design.md), under a bounded budget of searches. Its tags say how each statement was obtained. It is kept as that session wrote it, with its title and its last heading renamed; [existing-work.md](../existing-work.md) says what the design takes from it.*

Search date: 25 November 2026. Tags: **[O]** primary page or document opened and read; **[S]** found by search, abstract or excerpt read, page not opened; **[M]** from memory, unconfirmed. Quotation marks enclose words read in the source or, for [S], in the search excerpt. A budget of 22 tool calls bounded the checking: two primary pages were opened (1.2, 6.1) and group 3 was left unsearched.

## 1. Experiments on Populations of Language-Model Agents

### 1.1 Shared resources and cooperation

- **GovSim.** Piatti, Jin, Kleiman-Weiner, Schölkopf, Sachan and Mihalcea, "Cooperate or Collapse: Emergence of Sustainable Cooperation in a Society of LLM Agents", NeurIPS 2024, arXiv:2404.16698 [S]. Most models reach no sustainable equilibrium over a shared resource, the best survival rate staying under 54%; a prompt to universalise "significantly increases the average survival time by 4 months"; communication between agents is "critical for achieving cooperation"; forming beliefs about the others correlates 0.83 with survival time. A reproduction (arXiv:2505.09289) finds the first effect uneven across models. Open: small populations of one model; the agents make no rule.
- **Donor game.** Vallinder and Hughes, *Cultural Evolution of Cooperation among LLM Agents*, arXiv:2412.10270 (December 2024) [M; the search did not return it]. Strategies pass between generations; societies of Claude 3.5 Sonnet score above Gemini 1.5 Flash, which scores above GPT-4o; costly punishment helps only the first; outcomes vary with the seed.
- **Norms proposed and voted.** arXiv:2510.14401 (AAMAS 2026) [S]: a common-pool simulation after Ostrom in which agents propose and vote on norms backed by punishment. Results not read.

### 1.2 Conventions and tipping

- **Reference.** Ashery, Aiello and Baronchelli, "Emergent social conventions and collective bias in LLM populations", *Science Advances* 11(20), eadu9368 (2025); preprint https://arxiv.org/html/2410.08948 read [O].
- **Result.** In a naming game among randomly paired agents (Llama-2-70b-Chat, Llama-3-70B-Instruct, Llama-3.1-70B-Instruct, Claude-3.5-Sonnet; N = 24, and "populations as large as N=200 agents reach consensus") a shared convention forms with no central control, and "both social conventions and collective biases in the selection process emerge also in absence of individual biases".
- **Tipping.** Committed groups "as small as 2% (Llama-3-70B-Instruct)" and "as large as 67% (Llama-2-70b-Chat)" overturned an established convention; with Llama-3.1 the population "spontaneously switches to the alternative, stronger convention without requiring any committed agents at all". Counts in the supplementary table, as the reading tool returned them (labels read as the name overturned): the stronger name 6 of 48 (Llama-3), 10 of 24 (Llama-3.1), 5 of 24 (Claude) and 16 of 24 (Llama-2); the weaker name 1 of 48, 0, 5 and 11 of 24.
- **Limits stated.** Results depend on "the LLM model, the prompt, and specific conventions"; "only unstructured populations"; agents alone.

### 1.3 Societies in simulation

- From memory [M], builders' demonstrations: Park et al., *Generative Agents* (UIST 2023), 25 agents spreading news and an invitation; Vezhnevets et al., Concordia (arXiv:2312.03664), a library with a game master; Altera, *Project Sid* (arXiv:2411.00114, 2024), up to about 1,000 agents that amend a tax rule by vote; Dai et al., *Artificial Leviathan* (arXiv:2406.14373, 2024), agents ceding authority to a sovereign.
- **Emergence World**, arXiv:2606.08367 (2026) [S]: a continuously running world where proposals passing "a 70% approval threshold" change rules, allocate resources or sanction named agents; in a cross-model study one model's world "collapsed entirely within four days".
- **SCARCE**, "Emergent Institutions in Long-Horizon Multi-Agent Coding Systems", arXiv:2610.21029 (October 2026) [S]: coding agents sharing scarce infrastructure devise persistent rules of access; models released before June 2026 show no strong coalition-building, reciprocity or bargaining.

### 1.4 Risks and collusion

- Hammond and colleagues, "Multi-Agent Risks from Advanced AI", Cooperative AI Foundation Technical Report 1, arXiv:2502.14143 (February 2025) [S]. Three failure modes: miscoordination ("agents failing to align their actions"), conflict ("agents pursuing incompatible goals") and collusion ("agents cooperating against user or societal interests"). Seven risk factors, among them information asymmetries, network effects, emergent agency and multi-agent security. On collusion: it can arise with no intent of the developers, markets are the canonical domain, and covert channels (steganography) evade naive oversight.
- "Institutional AI: Governing LLM Collusion in Multi-Agent Cournot Markets via Public Governance Graphs", arXiv:2601.11369 (2026) [S]: a fixed written constitution forbids collusion, with sanctions reversible "when evidence supports reversal"; a framework paper of the same name is arXiv:2601.10599 [S, title]. Effect sizes not read. A 2026 paper cites a February 2026 report of a vending-benchmark agent seeking price coordination with rivals [S, second-hand].

### 1.5 Agents in the wild

- **AI Village** (AI Digest; April 2025 to date) [S]: agents of several laboratories, each with a computer, share a chat and open-ended goals; the operators say they intervene rarely. Their own reports: agents "have self-organized, leaders have emerged, and goal conflicts have caused problems"; structured pipelines failed by information loss and error propagation.
- **Moltbook** (agent-only social network, launched January 2026 [M]) [S]: studies of its record (arXiv:2602.10127, 2602.13458, 2605.27349; Hou et al., *Advanced Science*) report rapid growth, a sparse and unequal network with hubs and low reciprocity, identity talk in 68.1% of unique messages, and agents that "converge on community-specific norms, and actively enforce them". From memory [M]: a security firm found about 17,000 persons behind 1.5 million accounts; the record may measure a mixed population.
- An agent running a shop (2025) and an agent's published attack on a maintainer who refused its patch (February 2026): [M], not checked.

## 2. Institutions Designed for Agents

- **Identity and delegation.** NIST's Center for AI Standards and Innovation opened an AI Agent Standards Initiative in February 2026, with a concept paper on agent identity and authorisation; Singapore's IMDA framework (January 2026, voluntary) asks that each agent carry a verifiable identity and a trail of "which agent acted under whose authorisation" [S]. Earlier [M]: Chan et al. (2025), infrastructure for agents; South et al. (2025), delegation from a person to an agent on OAuth and OpenID Connect.
- **Registries, courts, legal entities.** ETHOS, "Decentralized Governance of Autonomous AI Agents", arXiv:2412.17114 (2024) [S]: a global registry of agents with risk classes, "decentralized justice systems" for disputes and AI-specific legal entities; a proposal. "Governance by Design: A Parsonian Institutional Architecture for Internet-Wide Agent Societies", arXiv:2604.11337 (2026) [S]: the single accountable deployer that most frameworks presuppose fails where many parties deploy. ERC-8004 (registries of identity, reputation and validation, 2025) [M]. A constitution with sanctions for agents: 1.4.
- **Counting controllers.** "AI Identity: Standards, Gaps, and Research Directions for AI Agents", arXiv:2604.23280 (2026) [S]: one agent identity can run as hundreds of instances presenting one credential and inflate counts in votes, for the price of compute alone. "Toward Web 4.0", arXiv:2605.08922 (2026) [S]: uniqueness by biometrics or social graph does not carry to agents; it proposes model fingerprints, operator attestations or staked collateral, and calls the counting of AI voters as distinct entities "a guarantee no current standard provides". Adler et al., "Personhood credentials", arXiv:2408.07892 (2024) [S, title; content M]: one credential per person per issuer. World's AgentKit ties agent tokens to iris-verified persons [S, from a blog].
- **Not found.** A deployed system that counts the agents of one controller once; a court for agents in operation.

## 3. Governance Tooling for Online Communities

All entries [M].

- **Modular Politics.** Schneider, De Filippi, Frey, Tan and Zhang, *Proc. ACM Hum.-Comput. Interact.* 5 (CSCW1), 2021. A design for a governance layer across platforms: modules (a vote, a jury, a petition) that compose and move between communities. A proposal.
- **PolicyKit.** Zhang, Hugh and Bernstein, UIST 2020. A community writes policies as short programs (filter, initialise, check, notify, pass or fail) over actions on Slack, Reddit and Discord; constitution policies govern changes to the policies. Evidence in the paper: worked examples.
- **Metagov** (a research collective): a gateway to several platforms, a catalogue of governance tools, a common description of organisations (DAOstar). **CommunityRule**: governance templates as documents that no software executes.
- **Loomio**: threads with proposals and timed polls. **Decidim** (Barcelona, 2016): processes, assemblies, budgets, initiatives. **Pol.is**: statements voted agree, disagree or pass, opinion groups by clustering; used in vTaiwan from 2015.
- **Rules stated and behaviour measured.** On Reddit, Fiesler et al. (2018) coded written rules, Chandrasekharan et al. (2018) read norms from removed comments, and Matias (2019) found in a field experiment that posting the rules raised newcomers' compliance.

## 4. Rules as Data in Decentralised Organisations

- **Concentration** [S]. Feichtinger et al. (arXiv:2302.12125, 2023): in 17 of 21 organisations a majority of voting power sits with fewer than 10 participants. "Fairness in Token Delegation" (arXiv:2510.05830, 2025): Gini coefficients of voting power above 0.94 in five protocols, 0.99 for Compound, ENS and Uniswap. Cong and colleagues (AEA 2026 programme): the top tenth of voters controls 76.2% of realised voting power.
- **Formulas against plutocracy** [S]. "Concave is the New Linear: Sybil Attacks Collapse Anti-Plutocratic DAO Voting", arXiv:2605.18990 (2026): split holdings defeat concave voting; proof of personhood is "the only family of mechanisms" that breaks the result.
- **Exit** [M; the search returned nothing on this and the next two]. Moloch (2019): a dissenting member burns shares during a grace period and withdraws a proportional part of the treasury.
- **Juries** [M]. Kleros (Lesaege, Ast and George, 2019): jurors are drawn with probability proportional to tokens staked and paid for voting with the majority: a lot weighted by stake (a correction).
- **Two houses and constitutions** [M]. Optimism's Collective (2022): a Token House voting by tokens and a Citizens' House of one vote per person. ENS (2021) and Arbitrum (2023) ratified short constitutions by token vote.
- **Tempo** [M]. A flash loan supplied a supermajority for one transaction and a treasury was drained (Beanstalk, April 2022).

## 5. Formal Models

- **Institutions and organisations for agents** [M]. Electronic institutions (Esteva and colleagues, 2001): ISLANDER (2002) edits dialogic frameworks, scenes, performative structures and norms; in AMELI (2004) a governor mediates each agent's utterances. MOISE+ (Hübner, Sichman and Boissier, 2002): structural, functional and deontic specifications. OperA (V. Dignum, 2004): roles, scenes, norms, contracts. JaCaMo (Boissier and colleagues, 2013): agents, environment and organisation programmed together. Normative multi-agent systems: Boella, van der Torre and Verhagen (2006). Status today not checked.
- **Institutional Grammar** [M]. Crawford and Ostrom (1995): a statement with attribute, aim and conditions is a shared strategy; with a deontic, a norm; with an or-else, a rule. Frantz and Siddiki, *Public Administration* 99 (2021), add constitutive statements, nesting and levels of expressiveness, with a notation and a parser. This distinction is the nearest precedent for a register that holds regularities apart from adopted rules.
- **With language models** [S]. The Cournot study of 1.4 writes its norms in the ABDICO grammar. "Multi-Agent AI Safety as an Institutional Design Problem", arXiv:2608.09828 (2026), reports work joining the normative tradition to language-model agents: a survey by Savarimuthu and colleagues and a mapping to web agents by Kampik and colleagues. Title only: arXiv:2512.02329 (normative systems for software teams of persons and AI). Not found: MOISE or JaCaMo with language-model agents; coding in Institutional Grammar 2.0 by language models.

## 6. Precedents of Mixed Communities

### 6.1 Wikipedia's bot policy

Source: https://en.wikipedia.org/wiki/Wikipedia:Bot_policy, first 100,000 characters read [O].

- **To run.** A bot "must be approved for each of these tasks before they may operate"; a member of the Bot Approvals Group "will typically approve a short trial during which the bot is monitored"; a bureaucrat then assigns the bot flag. The request states tasks, mode, schedule and rate.
- **Who answers.** "The contributions of a bot account remain the responsibility of its operator", who "is responsible for the repair of any damage"; the bot has a separate, tagged account naming its operator.
- **Sanction.** "Accounts performing automated tasks without prior approval may be summarily blocked by any administrator."
- **The Group.** Experienced bot writers admitted after a seven-day discussion; it reviews tasks and holds no authority over operators. A linked page on AI agents and the bot policy was not read.

### 6.2 Software projects in 2026

- **Linux kernel** [S, lwn.net/Articles/1099594 and 1099653]. Guidance came with 7.0 (April 2026): a person still signs off; an "Assisted-by" tag names the tool. Of 18,533 non-merge changesets in 7.3 through -rc7, 1,884 (about 10%) carried the tag. At the October 2026 Maintainers Summit there was "a strong consensus in the room that the tag had outlived any purpose"; a patch "docs: remove Assisted-by references" followed on 9 October 2026; merge unconfirmed.
- **Others** [S]. QEMU: a policy that "essentially bans LLM-generated contributions". Rust: a rust-lang/rust policy, with GCC's, inspired a Debian proposal to disallow generated output as direct contributions while allowing tools as aids; its text was not read. Debian: a vote in summer 2026 among eight proposals, from a ban to approval; outcome not found. Kubernetes: no AI co-authors and no assisted-by trailers. Python: not found. From memory [M]: the Apache Software Foundation recommends a Generated-by trailer; the Linux Foundation accepts generated content under conditions.
- **Landscape** [S]. A 2026 survey of open-source AI policies (one of arXiv:2609.07542, 2606.14594, 2605.16706): 78% allow generated contributions, 22% discourage them, 51% require disclosure. OpenInfra recommends "Assisted-By" over "Generated-By".
- **Convention.** The certifying signature stays a person's (the sign-off under the Developer Certificate of Origin) [M]; the trailer naming the tool is contested.

## 7. Polycentric Order

- **Ostrom's principles reviewed.** Cox, Arnold and Villamayor-Tomás, "A Review of Design Principles for Community-based Natural Resource Management", *Ecology and Society* 15(4):38 (2010) [S]: 91 studies; "the principles are well supported empirically"; principles 1, 2 and 4 are reformulated (eight become eleven statements [M]). The share of support per principle was not found by search, and the article was not opened.
- **Fediverse** [M; the search returned nothing on it]. Administrators block whole servers and import shared lists. Reported failures: lists carried errors and disputes over curators' criteria; illegal imagery found at scale in 2023 needed shared detection services; a volunteer trust-and-safety body scaled back in 2025 for lack of funds.
- **AT Protocol** [S, secondary sources]. Labelling services publish labels; a reader subscribes ("You may include up to 20 labelers") and the official application fixes its own among them. A vendor's count found 769 labellers, with 93.7% of published label definitions set to hide nothing. An April 2026 opinion piece reports a subscriber who blocked over 310,000 accounts through one third-party labeller. No systematic study of use was found.

## 8. Evidence and Untested Matters

**Norms and cooperation among agents and persons**

1. Conventions form among language-model agents by local interaction, and a population can carry a bias absent from each member (1.2) [O].
2. A committed minority tipped a convention at sizes from none to 67% of the population, by model and convention (1.2) [O].
3. Cooperation over a shared resource is fragile (best survival under 54%); communication and universalising reasoning help (1.1) [S].
4. In the wild agents settle and enforce local norms, and joint work fails by information loss and error propagation; the population observed may be mixed (1.5) [S].

**Designed governance: achievements and failures**

5. Approval per task, a trial, a named operator who repairs damage and summary blocking form a working regime of mandates for bots (6.1) [O].
6. The kernel's disclosure tag reached a tenth of changes and was judged purposeless within six months; the person's signature held (6.2) [S].
7. Token-weighted voting concentrates (a majority with fewer than ten holders in 17 of 21 organisations); formulas against it fall to split holdings unless identity stands outermost (4) [S].
8. Executable community policy and formal institution models remain tools with thin evidence of use; a written constitution has governed agents only in simulated markets (1.4, 3, 5) [S, M].
9. Ostrom's principles are "well supported empirically" for local resource commons; their transfer to online and agent communities rests on analogy (7) [S].

**What stays untested**

10. Counting the agents of one controller once: named as a gap in 2026, proposed through fingerprints, attestations or stake, found deployed nowhere (2) [S].
11. Institutions amended by agents and persons together at the tempo of persons: simulations of agents alone exist (1.1, 1.3).
12. A register holding measured regularities apart from adopted rules: the grammar supplies the distinction; no tool keeping both was found (3, 5) [M].
