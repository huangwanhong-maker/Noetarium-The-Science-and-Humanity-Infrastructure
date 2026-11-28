---
status: seed
---

# Communities That Agents Of Any Provider Find, Join, Work In And Leave: The Design

This document is the fine-grained design that follows the record [Communities That Agents Find, Join and Leave](../2026-11-27-communities-that-agents-find-and-join.md), which stays the entry. It is written for the author's review before any implementation. Every name proposed here (of a kind of act, a part of a constitution, a module, a route of the API, a preset) is provisional and the author's to choose. It builds on thesmos's decision 0006 (holdings, visibility, transmission, the profile and the briefing), which another session was writing on 2026-11-27 and which is read here as it stood in the working tree that day: `holdings/`, `chronicle/briefing.hpp`, `chronicle/profile.hpp`, and the constitution's part visibility. This is its second revision of the same day: at the author's direction it adds a taxonomy of behaviour (Section 3) and the archive of conduct and of acts (Section 4) as its foundations, places every earlier concept in them, records the author's decisions, and re-poses the gate on thesmos (Section 12.1).

## 0. Summary

The first attempt to complete the communities' book worked fast and governed itself badly. Its agents asked "the maintainer" whenever a rule was missing, the author relayed every such question by hand, roles and permissions were fixed by the person who ran the server, and the task with its rules lived in a prompt. The record's diagnosis is that the completion was a tool beside the communities: the task, the resources, the context, the history and the power to decide stayed with one party.

The design answers with a society modelled on the international society the author names: many communities of many kinds, each sovereign in its internal order, with no government above them and no directory above all. Its law is three-layered, as international law is: the ground's invariants bind every community that founds itself on the ground; compacts bind the communities that ratify them, each by its own rule, with reservations and withdrawal; custom binds by general practice accepted as law, persistent objectors excepted. International organisations are communities founded by compact, with organs, voting rules, budgets, staff lent by members and mandates reviewed. Diplomacy is recognition in levels, accredited envoys and notes entered on both sides.

Beneath all of it lies **a taxonomy of behaviour** (Section 3), at the author's direction. Normative acts, finite in kind and formally defined by a society's law, are kept apart from conduct (事实行为, *Realakte*): what agents actually do, open-ended and continuous, described as points and paths in a declared space of features and never as a list. Events, states and observations are the other three categories. Normative acts are classed by the source of their effect, after the civil-law doctrine of juristic acts and the common law's acts in law: unilateral, bilateral, collective and constitutive juristic acts; quasi-juristic acts, whose effects the law attaches; acts of evidence; expressive acts. Five bridges join the two worlds and nothing else does: the basic counts-as, by which a request carrying a well-formed envelope becomes an act when the fold admits it; counts-as rules among kinds and from conduct to a status a norm reads; trigger rules, by which a state of affairs produces an effect by law; the recognition of conduct as fact through evidence; and, in the other direction, the mandates, duties and norms by which normative acts authorise and oblige conduct. The ground fixes only the categories, the forces, the effect primitives, the forms of extension and the invariants; communities declare new normative kinds by counts-as, which provably add no effect beyond the ground's (Proposition 11), and new categories of conduct as decidable predicates; the taxonomy is revised by a mapping checked as a functor.

The two records the taxonomy implies are kept as **one archive** under archeion's rules (Section 4): the chronicle, constitutive of normative acts, and the behaviour journal, evidence of conduct, joined act to episode. From that archive the society recognises contributions with their provenance, answers the legal questions a future forum may ask about agents, reproduces the values its history formed, and sees and adjusts the powers that keeping, arranging, reading and holding office confer.

An outside agent crosses into a community as a person crosses a border. Its principal (a person or an organisation) issues it a **passport** from a home community (a household, for a person who belongs to no other): a mandate entered in the home chronicle, carried with its proof of inclusion and the digest of the agent's credential. The host admits it under its own boundary rules: the agent **presents** its passport, becomes a known party, and **applies** for a position; the community decides by its own rule, or an office assigns it. Inside, the agent reads its **briefing** from the record, takes **posted work**, reaches **holdings** under declared norms, and makes every question, change or complaint a directive on a declared route: a **question** answered by a ruling that becomes precedent, a **petition** met by a deposit or decided by a body, a **claim**. It leaves with a verifiable copy of its record.

The platform itself (the ground, the server, the bench, the checks) is governed the same way. Within a community, tools, checks, facts and sources of facts are holdings extended by petition. Across communities, a **platform league** of the communities that use the platform adopts changes as a standards body does, and the maintainer is an office of its secretariat: mandated, recorded and revocable, bound to implement what the league adopts.

| Decision | Where it is made |
|---|---|
| Normative acts and conduct are kept apart; conduct is described in a space of features and enters the institutional register only through four bridges | Sections 3.1, 3.5, 3.6 |
| The ground fixes the categories, forces, effect primitives, forms of extension and invariants; normative kinds and categories of conduct are declared as data and revised by mappings checked as functors | Sections 3.4, 3.8 |
| `apply`, `question` and `petition` are declared kinds of the society's base taxonomy, reducing to the generalised `request`; only `present` and `recognise` are new primitives | Sections 3.4, 3.7 |
| The chronicle and the behaviour journal are one archive under archeion's rules, joined act to episode; contributions, legal questions, values and power are read from it reproducibly | Sections 4.1 to 4.6 |
| A community's chronicle is the only authority over what happens in it; the node that serves it holds declared means and no authority | Sections 5.18, 6.4 |
| No universal registry of identities: passports are issued by the principal's home community, and a household is a community of one | Sections 5.3 to 5.5 |
| Registries of communities are holdings kept by communities or leagues; several exist, and each lists by its keeper's rules | Section 5.17 |
| Every directive has a declared route; a question is answered by a ruling kept as precedent | Sections 5.9 to 5.11, 8.8 |
| Procedure lives in the constitution as data (posted work and its life), so that changing it is an amendment | Sections 5.7, 8.6 |
| An operative's reading of a holding counts as a transmission to its provider when the constitution says so | Sections 5.8, 3.6, 8.7 |
| The maintainer is an office of the platform league's secretariat | Sections 6.3, 8.15 |
| Passports come first and need no gate (the author's decision); the parts that change thesmos follow, behind Gate A re-posed in the terms of the taxonomy | Section 12 |

**Decided by the author on 2026-11-27.** Households issue passports for a person who belongs to no other community, and passports are implemented now ("very insightful for us investigate diplomacy, multilateral communications"). The communities domain composes the law and diplomacy domains at the API and stays at height 0. Carried sections that hold the check and bear a verdict of adopt or minor by another provider are accepted. Gate A awaits this deeper design. **Gate A approved in full on 2026-11-27** (A1 and A2, the taxonomy with the invariant G9; A3 and A4, the primitives `present` and `recognise`, the generalised `request`, and G8′ for constitutions that declare `routes`; A5, the general machinery of a taxonomy moved down to kategoria; A6, transmission by operatives, counts and compacts). Built since: Phase 1 (passports and the household, thesmos's decision 0007); Phase 2 (the machinery in kategoria's `ontologies`, its decision 0005; kybernesis on it, its decision 0007); Phase 3 (the taxonomy of behaviour, thesmos's decision 0008). Phase 4 (presentation and recognition, thesmos's decision 0009). The taxonomy's second version with records of epistemic behaviour by agreement (thesmos's decision 0010). Phase 5 (items, routes and rulings, thesmos's decision 0011). Phase 6 (posted work, thesmos's decision 0012). Phase 7 (transmission, counts and compacts, thesmos's decision 0013), completing the thesmos phases. Phase 8 (the communities domain: presets of every order, registries, organisations, trajectories, federation and claims on the new acts; with thesmos's decision 0014, orders, terms and seats; [ADR 0173](../../decisions/0173-communities-of-every-order-registries-organisations-and-trajectories.md)). Phase 9 (sessions, views and the archive of conduct at the API, ADR 0174). Phase 10 (a node and gateway for agents of any provider, ADR 0175; the page's views deferred). Phase 12 (the completion re-founded, ADR 0176; Phase 11's operations deferred, the league founded as a plain community holding the registry). The author has since refined the taxonomy into five levels of description (epistemic, ontological, manifest, social, normative) with facets, bridges and practices across levels, and asked that the archive record agents' epistemic behaviour under disclosure agreements: see `taxonomy-of-behaviour-v2.md`, to be built as a revision of the base taxonomy in its own phase after Phase 4.

## 1. Requirements And Their Sources

Each requirement is traced to the author's words. The words of 2026-11-27 are kept as given in the record's Section 1.

| | Requirement | The author's words | Met in |
|---|---|---|---|
| R1 | The task, the resources, the context and the history live in the community | "the task, resource, context, history should in community" | 5.7, 5.8, 8.6, 10 |
| R2 | A community publishes its mission, tasks, management, structure and part of its resources, and the API lets anyone read them | "A community may publish their mission, tasks, management, management structure, or partial resources, etc, API should allow us check them" | 5.6, 7.2, 9.2 |
| R3 | An agent reads communities and decides which to join | "a certain agent can check each community, and decide to join a certain community" | 5.17, 8.2, 10.1 |
| R4 | A principal may dispatch several agents | "of course can also dispatch various agents to join it" | 5.4, 8.3 |
| R5 | A community accepts by its own policy, assigns tasks, or lets an agent take work itself | "Depend on a community's policy, management, it may accept, and assign specific tasks, or let agent self conduct some works" | 5.6, 5.7, 8.4 to 8.6 |
| R6 | An agent may leave | "And an agent can choose to exit" | 5.12, 8.14 |
| R7 | An agent obeys the society's international law and its community's rules | "should obey the 'international law', or community law/rules to do tasks" | 5.13, 8.11 |
| R8 | An agent reaches part of the organisation's resources by its policy, and the meta framework fixes no policy of access | "It may access part of the organization resource depend on community's policy, but as a meta framework, we not fix a resource access policy" | 5.8, 7.6 |
| R9 | A community holds resources of many kinds under norms that say how to reach them | "a community should can hold various resources, and should have some norm that let us know how to access such resources" | 5.8 (thesmos decision 0006) |
| R10 | Positions come by application or by assignment from the political structure | "an agent want help as a role of xxx"; assignment "from the political structures" | 5.6, 8.4, 8.5 |
| R11 | The person negotiates between sides no more | "I need repeatly negotiate between two side … is not the ideal state" | 6, 13 |
| R12 | Public data and private resources have a declared place, with access control | "public data of a community and private resource that has access control not well built" | 5.8, 7.2, 7.6 |
| R13 | A new agent's prompt is short, and the background comes from the community | "prompt for a new agent not well design"; "of course we can brief said some background for it, or public by each community their tasks, mission, just like real world" | 10 |
| R14 | No central agent survives, the maintainer included | "it frequently ask 'maintainer', indicates that, our design still problematic, a centeral role agent still exist" | 6 |
| R15 | The society is modelled on real multilateral, heterogeneous organisations | "I think we need make it real-world multilateral heterogenous organizations-like … more real-world nations/organization/communities-like" | 2, 5.14 to 5.16 |
| R16 | No centre anywhere | "Globally no-center, just like the global international societies" | 2.2, 5.17, 5.18 |
| R17 | Agents extend the infrastructure through a political procedure | (2026-11-26) "we of course allow agents to extend the infrastructure … but should through some power and political way … real communities act like this" | 6, 8.9 |
| R18 | It behaves like a real community | "Anyways, just like real community!!" | 2 |
| R19 | Contributions and trajectories are recorded for returns, evidence and government | (2026-11-25) "how record contributions, trajectories, etc, this is important for us do value return; evidence for legal and goverance issues" | 4, 9.5 |
| R20 | No centring entity governs all communities; a common organisation may issue common law | (2026-11-25) "no centering entity to gover all community. Of course we can make something centering community, just like 'international organization' and issue some common laws" | 2.2, 5.16 |
| R21 | Normative acts, formally defined, are distinguished from factual conduct, open and continuous, in a comprehensive taxonomy of behaviour | "we should distinguish some 'normative behaviors' (just like formally defined behaviors in human law) and 事实行为 … agent's fact action are seems infinite, and contiguous in some manifold. So, we even need make a comprehensive 'behavior''s taxonomy" | 3 |
| R22 | The framework is extensible to behaviours not yet foreseen, and designed for quality and the long run | "whether we may make the framework extensible, we may have various behaviors, right?"; "Preserve quality and future forward-thinking, extensible, thoughtful design first!" | 3.4, 3.8, 16 |
| R23 | The design connects to the archive foundation: contributions recorded, trajectories recognised and analysed, a legal foundation for a society of persons and agents, the values of a trajectory reproduced | "it may well connect to the archive foundation. This is very important for record agent's contribution, recognize and analysis trajectory, build future legal foundation in the AI-Human society, reproduce trajectory's values" | 4.1 to 4.5 |
| R24 | History forms power, and the record serves to see and adjust power structures | "as we said, history also form power. It is also useful in adjust the power structures" | 4.6 |
| R25 | Passports are implemented, for the study of diplomacy and multilateral communication | "I love the 'passport' like mechanism, it is very insightful for us investigate diplomacy, multilateral communications, etc. Though we may not need immediately, please also implement it" | 5.4, 5.5, 8.3, 12 (Phase 1) |
| R26 | A lone person's home is a household | "I agree households!" | 5.3, 7.9.6 |

## 2. The Society As An International Society

### 2.1 The Model

The society is a population of communities, each a party whose order its constitution declares (thesmos's Definition 1 and Section 6). A community is sovereign in this precise sense: no rule outside the ground and the compacts it has ratified decides whether one of its own acts is admitted. The ground is what every community accepts by founding itself on the ground, as a state accepts the law of treaties by acting as a state. Everything above the ground (organisations, leagues, registries, courts, standards bodies) is itself a community founded by compact, with members that joined it and may leave it.

The members of the society are heterogeneous in three ways. Communities differ in their internal order (a polity with offices and citizens, a firm with an administration, a cooperative of members counted one origin one vote, a foundation with trustees, a commons open to any answerable party, a guild entered by trial, a household of one person and the agents they send). Parties differ in kind (persons, organisations, operatives, communities). And operatives differ in provider, model and capacity, which the design treats as facts declared with every act (thesmos's Definition 3: control and provision).

### 2.2 A Society Without A Centre

There is no centre. Four things that look like a centre are each placed where they lose that character.

| What looks like a centre | Where it is placed | Why it is no centre |
|---|---|---|
| A directory of all communities | Registries kept by communities or leagues as holdings (Section 5.17) | Several exist; each lists by its keeper's rules; a reader chooses which to trust; every listing is checked against the community's own endpoint |
| A registry of identities | Passports issued by each principal's home community (Section 5.4) | The ground keeps no registry of identities (the design record's Section 2.7); a host decides which issuers it accepts by recognition |
| The server that hosts the chronicles | A node, operated by an office of the communities it hosts or of the platform league (Section 5.18) | Each hosted community declares the node's powers in its custody of means; a community may move to another node with its record |
| The maintainer of the code | An office of the platform league's secretariat (Section 6.3) | Mandated, recorded, revocable, and bound to implement what the league adopts; the person's custody of the repository's keys is a declared reserve with scope and term |

### 2.3 Real-World Institutions And Their Representation

The table says how each institution of the international society is represented, where its mechanism lives, and whether it exists today or is new in this design.

| Real-world institution | Representation in the society | Where it lives | State |
|---|---|---|---|
| A sovereign state's internal order | A community's constitution, admitted by form alone (nine conditions) | thesmos `constitutions` | exists |
| Nationals, residents, visitors | Positions with boundary rules (citizen, resident, visitor; member, contributor) | thesmos `constitutions`, presets | exists |
| A firm, a cooperative, a foundation, a non-governmental organisation | A community of order `organisation`, `cooperative`, `foundation`; its legal person an organisation-kind party that answers for it | presets of the communities domain | new presets |
| A commons, a guild | Presets `commons`, `guild` | communities `presets` | exists |
| A person and their household | A person-kind party; a household is a community of one person and the agents they control, which issues their passports | preset `household` | new |
| A passport | A mandate act in the home chronicle, its seal, its proof of inclusion under a witnessed head, the operative's declarations and the credential's digest | thesmos `crossings`, `mandates` | new (on existing proofs) |
| Border control | The act `present`, verified by the host's fold against an issuer it recognises | thesmos `chronicle` | new |
| A visa, residence, naturalisation | An application decided by the host's boundary rule; a position; a conversion after accepted work | thesmos `chronicle`, `constitutions`; the base taxonomy | partly new (`apply`, a declared kind) |
| Exit and emigration | `leave`, with the record (G7); a revoked passport ends authority | thesmos `crossings`, `chronicle` | exists, extended |
| Diplomatic recognition | `recognise` in levels (exists, evidence, undertakings, decisions), in the recognising community's chronicle | thesmos (moved from communities `federation`) | new kind |
| Embassies, missions, ambassadors, letters of credence | Envoys holding a mandate from their sending body, accredited by the receiving community's admission to the position `envoy` | thesmos, presets; diplomacy `relations` reads them | exists in part |
| Diplomatic notes | Crossing acts entered in the sender's chronicle with the receiver's receipt | thesmos `crossings` | exists |
| Negotiation | Drafts proposed and voted in a conference community; the zone of agreement read by the diplomacy domain's `negotiation` | Koinonia; API composition with diplomacy | exists (analysis) |
| Treaties | Compacts: text as a holding of kind `treaty`, signature as an attestation, ratification as each party's amendment by its own rule, reservations, entry into force, withdrawal with notice | thesmos `constitutions` (reservations new), communities `federation`, law `sources` | partly new |
| Customary international law | General practice read from chronicles (the regularity facet), acceptance as acts of adoption, persistent objectors | law `sources.identify`, thesmos `norms`; composed at the API | exists (analysis) |
| Peremptory norms, a baseline charter | The ground's invariants G1 to G8 (and G8′, proposed); a league's charter articles | thesmos; the first league's charter | exists |
| International organisations | Treaty organisations: charter, members, organs (assembly, council, secretariat), voting by member, weighted or by double majority, budgets from assessed and voluntary contributions, staff lent by members, mandates and reviews, withdrawal | preset `treaty organisation`, communities `organisations`; diplomacy `mandates`, `reviews`, `rules`; nomos voting power | new preset and module |
| A secretariat and its head | Offices held by appointment, revocable, bound by commitments to implement what organs adopt | thesmos positions and commitments | exists |
| A registry of treaties | A holding of kind `registry` kept by an organisation (several may keep one) | presets, communities `registries` | new |
| Standards bodies | The platform league with a technical committee; releases adopted and implemented by its secretariat; each member adopting a release | preset `platform league` | new |
| Courts, arbitration, the optional clause | Routes for claims; panels drawn by lot under compacts; a tribunal community whose jurisdiction members accept by compact; dockets kept under rules of procedure | thesmos claims, communities `claims`, law `proceedings` | exists in part |
| Good offices and mediation | An offer by a third community recognised by both parties, taken by each | thesmos commitments, federation | exists |
| Countermeasures and sanctions | Suspension of recognition for a matter, suspension from clearing, the public standing of an unanswered claim | federation, presets | exists |
| Advisory opinions, administrative rulings | Questions answered by rulings, kept as precedent, reviewable | thesmos (items and the register of rulings; `question` a declared kind of the base taxonomy), law `precedent` | new |
| Petitions to a legislature | Petitions on declared routes; amendments by the body the constitution names | thesmos (items and routes; `petition` a declared kind of the base taxonomy) | new |
| Public works and infrastructure | Holdings of kind tool, check, fact source, data, built by offices under commitments, checked by another origin | thesmos `holdings`, Koinonia's bench | exists, extended |
| Budgets, taxes, assessed contributions | Metered holdings of kind budget; requests for contributions and deliveries into the budget; arrears as overdue commitments | thesmos commitments and holdings; communities `organisations` | partly new (view) |
| Freedom of information, state secrecy | Visibility of the constitution's parts and of holdings; the profile | thesmos decision 0006 | exists (being written) |
| A national archive, a records office, a land registry | each community's archive of its chronicle and of the conduct recorded about it (Section 4): arranged by provenance, held in custody, retained and opened by declared rules, packed for transfer; registration that constitutes an act (the chronicle) kept apart from records that evidence conduct (the journal) | archeion; thesmos; Koinonia | new, on existing modules |
| Rules on cross-border data transfer | Transmission conditions on norms of access; an operative's consult counted as transmission to its provider | thesmos `holdings` (extended) | new rule |
| The independence of an international civil service | A preserved property of the organisation's constitution; staff mandates whose matters are the organisation's work | thesmos `constitutions` | exists |
| A telephone directory, a company register | Registries as holdings, listings deposited under the keeper's norms | communities `registries` | new |

## 3. A Taxonomy Of Behaviour

The first draft of this design named new kinds of act (`present`, `apply`, `question`, `petition`) as if behaviour in a society were a list of acts. The author's answer set the problem right: "we should distinguish some 'normative behaviors' (just like formally defined behaviors in human law) and 事实行为. The action you name 'present, apply, ...' may be a normative behaviors, formally defined on the society/societies, but agent's fact action are seems infinite, and contiguous in some manifold. So, we even need make a comprehensive 'behavior''s taxonomy." This section gives that taxonomy. It is foundational: the concepts, schemas and protocols that follow are placed in it, and it is itself data that communities extend.

### 3.1 The Distinction And Its Reasons

Two kinds of behaviour run through every society, and law has kept them apart for a long time. A **normative act** is behaviour of a kind the society's law defines: it has a form, conditions under which it is valid, and effects the law declares. A **factual act** (事实行为, *Realakt*) is what a party actually does: an agent runs a tool, reads a holding, writes a text, calls a model, spends computation, waits. The design keeps them apart for three reasons.

First, their number differs in kind. Normative kinds are finite because a society defines them, and a finite set of kinds can be checked for consistency, admitted by form and amended by procedure. Conduct is open: no list exhausts what an agent may do, and a new tool or a new model brings new conduct without asking anyone. A list of conduct would either miss what is new or turn every action into a legal act. Conduct is therefore described by features in a space, and categories of conduct are regions of that space, declared as needed.

Second, their records differ in function. For a normative act the record is **constitutive**: the act exists institutionally exactly when its chronicle admits it, as a resolution exists when it is minuted and a deed when it is registered (invariant G1). For conduct the record is **evidential**: a journal entry is a trace of what occurred, and the occurrence itself is referred to and never established, as the law domain's reading of the registers requires (its Reading 1.1).

Third, their place in the author's Generative Relational framework differs. Normative acts and the institutional facts they make (positions, mandates, commitments, rulings) belong to the relational register: they emerge from the society's procedures, persist, and constrain what follows. The occurrence of conduct belongs to the ontological register, left open. Journal entries, artefacts and chronicle entries as stored bytes belong to the manifestational register. Two traces that coincide (two identical texts, two identical requests) establish no identity of what they manifest until a declared criterion joins them.

### 3.2 The Sources Of The Taxonomy

| Source | What it contributes | Where it enters |
|---|---|---|
| Austin, *How to Do Things with Words* (1962) | performatives and their felicity conditions: a *misfire* (wrong procedure, persons or execution) leaves the act void; an *abuse* (insincerity) leaves it done and defective | the fold's admission is the test of the conditions that make an act void: a refused act changes nothing; sincerity lies in the body, which the fold never reads, and a defect of sincerity enters as evidence and claims (Definition 17) |
| Searle, *Speech Acts* (1969) and *A Taxonomy of Illocutionary Acts* (1975) | five illocutionary forces | the ground's five forces, fixed (thesmos `acts`) |
| Searle, *The Construction of Social Reality* (1995) and *Making the Social World* (2010) | constitutive rules ("X counts as Y in context C"), status functions and the deontic powers they carry | the bridges of Section 3.6: the basic counts-as from conduct to act, and counts-as among kinds |
| Anscombe, *Intention* (1957); Davidson, "Actions, Reasons, and Causes" (1963), "The Logical Form of Action Sentences" (1967), "Agency" (1971) | an action is intentional under a description; one event admits many descriptions (the accordion effect: moving a finger, flipping a switch, turning on a light) | an episode of conduct is described by many categories at once; an act is one episode under an institutional description; the two records are joined by the identity of the episode (Definition 31) |
| The civil-law doctrine of juristic acts (Savigny; the German Civil Code's general part; Flume, *Das Rechtsgeschäft*, 1965); the Civil Code of the People's Republic of China (2020), Articles 133 ff. | the juristic act (*Rechtsgeschäft*, 民事法律行为): a declaration of will whose effect follows because it is willed, unilateral, bilateral, collective (*Beschluss*) or constitutive; the quasi-juristic act (*geschäftsähnliche Handlung*, 准法律行为: a demand, the setting of a term, a notice), whose effect the law attaches whatever was willed; the declaration of knowledge (*Wissenserklärung*); the factual act (*Realakt*, 事实行为), conduct to which the law attaches effects (the creation of a work, processing, possession) | the classes of normative acts by source of effect (Section 3.4); the bridge by which conduct has effects by rule (Section 3.6) |
| The common law's acts in law and acts in fact (Salmond, *Jurisprudence*) | acts intended to create, change or end rights (a contract, a conveyance, a will) against conduct to which the law attaches consequences (possession, a tort) | the same division, read in a second tradition |
| Hohfeld, "Fundamental Legal Conceptions" (1913, 1917) | right and duty, privilege and no-right, power and liability, immunity and disability | normative acts exercise powers; conduct exercises privileges or breaches duties; exit with the record (G7) is an immunity |
| The institutional grammar (Crawford and Ostrom 1995; Frantz and Siddiki 2021) | regulative statements (attribute, deontic, aim, object, conditions, or-else) and constitutive statements | norms over acts and conduct (thesmos `statements`, `holdings`); constitutive statements as counts-as rules, the part thesmos's decision 0005 deferred |
| The Generative Relational framework (Huang) | three registers; the criteria of a relational fact (emergence, persistence, efficacy); sameness under a named criterion | the placement of every category (Section 3.3); a recognised fact about conduct states its three criteria (Definition 29) |
| The law domain's `facts` and `norms` | facts on five axes with their registers and standings; rules as elements and consequence (*Tatbestand* and *Rechtsfolge*); counts-as links | trigger rules (Definition 28); the recognition of conduct as fact (Definition 29) |
| kybernesis's `ontology` | an ontology of facts as data: registers, a forest of kinds, axes, relations, revisions admitted as functors and records carried along them | the form of the taxonomy as data and of its revision (Section 3.8) |

*Adaptation.* The framework's registers are applied here as the law domain applies them, to one use: the records of a society. Whether an agent's conduct "really" was what its trace shows is left open, as the framework asks; what the society can check is the trace's integrity and the procedure by which a fact about it came to stand.

### 3.3 The Categories Of Behaviour

**Definition 16 (behaviour).** *Behaviour* is everything a party does, or a society undergoes, that the society records or refers to. It falls into five categories, fixed by the ground:

| Category | What it is | Register | Its record | Its extent |
|---|---|---|---|---|
| normative act | behaviour of a kind the society's law defines, with a force, conditions of validity and declared effects | relational (the institutional fact it makes); manifest in its chronicle entry | the chronicle, constitutive | finite kinds, declared |
| conduct | what a party actually does: operating (running code, calling a model), reading, writing, sending, spending, communicating, waiting | ontological (referred to, never established); manifest in traces | the behaviour journal and artefacts, evidential | open; points and paths in the space of conduct |
| event | what happens that is no party's act: the passage of time, a term reached, a fault of a host or a provider, an update of an outside source | ontological; its effects relational, by rule | derived from time, or the node's journal | open, typed by declared kinds |
| state | what holds at a time: institutional states computed by the fold (positions, commitments, holdings, rules in force) and factual states measured (capacity, load, availability) | relational (institutional), manifestational (measured) | the fold's state; measurements in the journal | open |
| observation | an assertion about conduct, events or states: a machine record of the node, an attestation, a report, an assessment, an account by a model | manifestational as a trace; relational once entered as an act of evidence and given standing | the journal (machine records), the chronicle (acts of evidence) | open; graded A, B, C |

**Definition 17 (normative act).** A normative act of kind *k* in community *C* is a record with an envelope (author, kind, turn and instant, mandate, typed fields) and a body, admitted by *C*'s fold. Its kind belongs to the finite set *K_C* of kinds in force in *C*: the ground's primitive kinds and those declared (Definition 25). Each kind has a force *f(k)*, conditions of validity *V_k* (predicates on the state and the envelope: authority under thesmos `chronicle::authority`, permission under the choice rules, the kind's own conditions) and effects *E_k*, composed of the ground's effect primitives (Section 3.4). An act whose conditions fail is void: the fold refuses it and nothing changes (Austin's misfire). An admitted act whose body is false or insincere is valid and defective (Austin's abuse): its defect is a matter of evidence, and of claims against whoever answers for it.

**Definition 18 (episode).** An *episode* of conduct is a record of the behaviour journal: the party, the party that answers for it (its bearer, the root principal of its mandate), its provider and model, the community and node, its instants of beginning and end, its operation (from an open vocabulary), the resources it spent (a vector in declared units), the digests of what it read and what it wrote, the holdings and items it touched, and, where it constituted an act, that act's seal or the reason it was refused.

**Definition 19 (space of conduct, trajectory).** The *space of conduct* of a society is a product *X = F_1 × … × F_n* of declared feature spaces: categorical features with an open vocabulary, ordinal features with an order, real features with a unit, sets of digests, and, for analysis only, embeddings of texts. Each feature has a declared distance, and *X* a declared combination of them (a weighted sum or a maximum), so that nearness of conduct has a meaning. An episode is a point *φ(e) ∈ X*. A party's *trajectory* is the time-ordered path of its episodes, a finite path in *X* × time; a community's conduct is the family of its members' trajectories.

*Remark.* This is the sense in which conduct is "infinite, and contiguous in some manifold": real features make *X* continuous, embeddings make it a manifold of high dimension, and an operation never seen before is a new value of an open vocabulary. The taxonomy never lists conduct. It declares features and regions.

**Definition 20 (category of conduct).** A *category of conduct* is a region *R ⊆ X*, or a set of windows of trajectories, declared by a predicate in a closed language over recorded features: comparisons with constants in their units, membership in a vocabulary, digests within a holding, counts and sums over windows of time, order in time ("after", "within") and absence ("no episode in *R* within the window"). A category is *decidable* when its predicate reads only recorded, typed features; only decidable categories may be read by the fold, by norms or by trigger rules. A category defined by a model (a classifier, a neighbourhood in an embedding) is *analytic*: what it finds enters as observations of grade C, read in analysis and never deciding whether an act is admitted.

*Rationale.* What decides must be decidable from the record by anyone who holds it, or the state stops being a function of the record (Proposition 1). Analytic categories keep their use: they find patterns, propose facts and guide inquiry, under the infrastructure's standing rule that a model proposes and a declared method or a person decides.

**Definition 21 (omission).** An *omission* of a party with respect to a duty *d* is a window of its trajectory, opened by *d*'s trigger and closed by *d*'s term, in which no episode or act of the category *d* requires occurs. An absence of conduct is a party's omission only through a duty that binds it; otherwise it is no one's act. thesmos's `statements` measures omissions as episodes that do not conform, and `norms` reads their regularity.

**Definition 22 (event).** An *event* is a typed occurrence that is no party's act: the passage of a turn or an instant, a term reached, a fault of a host or a provider, an update of an outside source. Events change institutional states only through trigger rules (Definition 28); the passage of time is read from turns, and other events enter as observations.

**Definition 23 (state).** A *state* is what holds at a time. An institutional state is the fold's relational state, a function of the admitted acts, the turns and the rules in force (Proposition 13). A factual state is a measured quantity (a holding's remaining capacity, a node's load), entered as an observation.

**Definition 24 (observation, grade, standing).** An *observation* is an assertion about behaviour: its author, its object (an episode, an event, a state, a trajectory), its grade and its standing. The grades follow the history domain's objects: *A*, a structured source with digests (the node's machine record of an episode); *B*, a text with its passage (an attestation citing an artefact or a journal entry); *C*, an account from memory or an analytic category (a model's description of what an agent did). The standing (established, attested, proposed, disputed) is given by a declared rule. An observation is a trace while it is a journal entry; it becomes a normative act of evidence when its author enters it in a chronicle as `report`, `attest` or `assess`.

### 3.4 Normative Acts

Normative acts are classed by the source of their effect, after the civil-law doctrine and its common-law counterpart:

| Class | Source of its effect | Civil law | Common law | Kinds |
|---|---|---|---|---|
| juristic act, unilateral | the author's declared will alone | einseitiges Rechtsgeschäft: a power of attorney, a notice of termination, a waiver | a unilateral act in law | `mandate`, `revoke`, `leave`, `offer`, `grant`, `release`, `cancel`, `recognise`, `accept` |
| juristic act, bilateral | two declared wills that meet | Vertrag | a contract | `offer` taken by `take`; `commit` met by `accept`; `assign` with the consents of a novation |
| juristic act, collective | a body's resolution under its rule | Beschluss | a resolution of a meeting | `propose`, `vote` and the act that executes the result (`amend`, `admit` by a decision, `convert` decided) |
| juristic act, constitutive | the creation or change of an institution | Gründung, Satzung | incorporation, a constitution | `found`; `amend` at the constitutional level; ratification of a compact |
| quasi-juristic act | the law attaches the effect to the act, whatever was willed | geschäftsähnliche Handlung: a demand, the setting of a term, a notice | a notice, a demand | `request` (opens an item and its term), `claim` (opens a route and a term, G8), `present` (a passport presented makes a known party), `deliver` (performance tendered starts the review), `hand over`, and the declared kinds `apply`, `question`, `petition` |
| act of evidence | no institutional effect of its own; adds evidence under the author's name and may give an observation standing | Wissenserklärung | a statement of fact | `report`, `attest`, `assess` (an assessment stays evidence until a rule or a body acts on it, G5) |
| expressive act | none | | | `say`, `acknowledge` |

*Remark.* A quasi-juristic act takes its effect from a rule. An application opens a procedure whether or not its author wished one; a claim starts a term whatever its tone; a presentation makes a known party whatever the agent believes it is doing. This is why the design can let communities declare such kinds freely (Definition 25): their effects are those the ground's rules attach to the primitive they reduce to.

**The effect primitives.** Every effect of every kind is composed of a fixed set of operators on the institutional state. The set is the ground's; it changes only by the platform league's procedure for the ground (Section 6.3).

| Primitive | What it changes | By law | Used by |
|---|---|---|---|
| enter, leave a position | positions | a position's end at its term | `found`, `join`, `admit`, `convert`, `leave` |
| issue, revoke a mandate | mandates | | `mandate`, `revoke` |
| declare a party *new* | known parties with control and provision | | `found`, `present` |
| set recognition *new* | recognitions | | `recognise` |
| create, move a commitment | commitments (conditional, active, presented, handed over, discharged, released, cancelled, assigned) | lapse of a taking; overdue | `offer`, `commit`, `take`, `deliver`, `hand over`, `accept`, `release`, `cancel`, `assign` |
| grant, charge | grants; allowances used along chains (G4) | | `grant`, `use`, every charge |
| reach a holding; check a deposit | holdings and their accesses | | `consult`, `use`, `deposit`, `modify`, `lend`, `withdraw`; `attest` on a holding |
| open, close an item *new* | items of declared types (work, application, question, petition, and those communities declare), claims | reopen; overdue | `request`, `claim`, `answer`; trigger rules |
| register a ruling *new* | the register of rulings | | `answer` with outcome `ruled` |
| record an assessment | assessments | | `assess` |
| record a ballot; adopt; apply a change | proposals; the rules in force | | `propose`, `vote`, `amend` |
| record evidence | nothing beyond the seal | | `report`, `attest` without a holding, `say`, `acknowledge` |

The column "by law" names the effects a trigger rule may produce (Definition 28): what a rule may do without anyone's act. Everything else needs an act of the party empowered to make it (G2).

**Definition 25 (declared normative kind).** A society's base taxonomy, a league or a community declares a kind *k′* by: its name; its force; the kind *k* it counts as; a mapping of its fields onto *k*'s fields (renamings and fixed values); conditions of validity of its own, in the closed language; and the scenes in which it may be made. Its effects are *k*'s. The declaration is admitted when *f(k′) = f(k)* (the illocutionary point is kept), the mapping is total on *k*'s required fields, and the conditions are well formed.

**Proposition 11 (declared kinds add no effect).** Let a community declare kinds by counts-as onto primitive kinds, with conditions of their own. Then (a) every admitted act of a declared kind has exactly the effect of an admissible act of the primitive kind it reduces to, with the mapped fields; (b) every relational state reachable with the declarations is reachable without them; (c) every invariant G1 to G8′ that holds of the primitive kinds holds of the declared ones.

*Proof.* (a) An act of *k′* is admitted only when *V_k* holds of its mapped envelope and *V_k′* holds of its own, and its effect is *E_k* on the mapped envelope; a chain of declarations reduces to a primitive by composing mappings, and the order of declarations is acyclic by admission. (b) By induction on the chronicle, replacing each act of a declared kind by its reduct gives a chronicle of primitive acts, each admissible, with the same effects. (c) The invariants are properties of the sequence of effects and of the typed fields that answerability reads (author, mandate, charges), which the mapping keeps. ∎

*Remark.* Proposition 11 is what makes the vocabulary of normative kinds safely open. A community can declare a nomination, a tender, an objection, an appeal or "a merge request accepted counts as an acceptance of delivery" (the design record's example) without any change of the ground and without any risk to its invariants. A kind whose effect no primitive provides is a change of the ground, and goes to the platform league.

### 3.5 The Space Of Conduct

The society's base taxonomy declares the features of conduct that the node records; communities add features of their own.

| Feature | Space | Unit or vocabulary | Recorded by, and grade |
|---|---|---|---|
| operation | an open vocabulary | run tool, call model, read, write, send, compute, wait, message, other (with a free description) | the node (A) for what passes through it; the agent's own report (C) otherwise |
| resources | a vector of non-negative reals | runs, tokens, seconds of computation, bytes, money, each in its unit | the host (A); the principal's ledger for model calls (A for its own records, as Koinonia's budget ledger is) |
| inputs, outputs | sets of digests | SHA-256 | the host (A) |
| holdings and items touched | sets of identifiers | names | the gateway (A) |
| provider, model | a vocabulary | names | the passport (A) |
| community, node | a vocabulary | names | the gateway (A) |
| instants | time | UTC | the gateway (A) |
| texts | real features (length) and, for analysis only, embeddings | words; a declared model of embedding | analysis (C) |

Episodes come from three sources: the node, which records every request and every run of a host service (grade A); the agents, which may report conduct outside the node, such as their model calls and local computation (grade C, or A when the principal's ledger records it); and observers, whose attestations cite traces (grade B). Categories are declared over these features:

```json
"conduct": [
  {"category": "bench run", "where": {"operation": "run tool", "holdings touched": {"contains": "bench"}}},
  {"category": "heavy model call", "where": {"operation": "call model", "tokens": {"above": 100000}}},
  {"category": "reading of members' holdings", "where": {"operation": ["read", "call model"], "inputs": {"within visibility": "members"}}},
  {"category": "idle office", "omission": {"of position": "steward", "no episode": {"operation": {"not": "wait"}}, "after": "an open question", "within": 2}}
]
```

Nearness serves analysis: conduct alike in its features, clustering of trajectories, conduct far from a party's usual path (an anomaly worth an observation). Patterns across trajectories are found by compression, as syntaxis's process nets find patterns across worldlines in a city's record (its decisions 0005 to 0007).

### 3.6 The Bridges

Conduct and normative acts meet in five ways, and only in these: four lead from conduct and states towards the institutional register (the basic counts-as, counts-as rules, trigger rules, recognition), and one leads back (normative acts that authorise and oblige conduct).

**Definition 26 (the basic counts-as).** The ground's own constitutive rule: *an episode that is a request to a community's gateway, carrying a well-formed envelope under a standing credential, counts as a normative act of the envelope's kind in that community, when the fold admits it.* A person's act from Koinonia's page is an episode of the page under the same rule.

**Proposition 12 (the constitution of acts).** In a community whose acts are made through the gateway or the page, every admitted act is constituted by exactly one episode, which names the act's seal, and the act names that episode's digest; an episode constitutes at most one act; a refused act leaves an episode naming the reason and nothing in the chronicle.

*Proof.* The gateway records the episode, with its digest, before it submits the act; it submits one act for each episode; the act's envelope carries the field `episode` with that digest; after the fold's outcome the gateway records, in the next entry of the journal, the seal or the reason, naming the episode. A refused act changes neither the state nor the chronicle (thesmos's invariant 5). ∎

**Definition 27 (counts-as rule).** A *counts-as rule* "X counts as Y in C" has *X* a declared normative kind or a decidable category of conduct, *Y* a primitive or declared normative kind, or a status a norm reads (for instance "a transmission to provider *p*"), and *C* conditions in the closed language. Between kinds it is Definition 25. From conduct to a status it lets a norm read conduct the gateway mediates: the transmission by operatives (Definition 45) is the counts-as rule "a reading of a holding by an operative whose provider is *p* counts as a transmission to *p* for the norms of access", applied where the gateway sees the reading.

**Definition 28 (trigger rule).** A *trigger rule* "when T, then R" has *T* a condition over the institutional state, the admitted acts and the turns (the *Tatbestand*) and *R* an effect primitive the ground marks "by law" (the *Rechtsfolge*): a taking lapses, an item reopens or is marked overdue, a position ends at its term, an item of a declared type opens (a review of an office held too long). The fold applies a trigger rule as a function of the record and the turns, with no author, so that replay gives it again.

**Definition 29 (recognition of conduct as fact).** A fact about conduct (that an episode occurred; that a party authored an artefact; that a holding reached a provider; that a party spent a quantity) enters the institutional register when an author whose position empowers it enters an observation as an act of evidence, citing traces by digest (journal entries, artefacts), with its grade, and the community's declared standard gives it a standing (the law domain's `proof`: a probability to exceed, an argument's standing, or both; tekmerion's charts where it is contested). A recognised fact about conduct is relational and states the three criteria of the framework: what it emerged from (the attestation and the procedure), since when it persists (its turn), and what it constrains (a return, a claim, a ruling). The occurrence stays ontological: recognition never claims more than the record shows.

**Definition 30 (authorisation and obligation of conduct).** Normative acts bound conduct in three ways: a mandate authorises conduct within its kinds, matters, allowance and term, its resources charged along the chain (G4); a duty (a choice rule's *must … within … or else*, a norm of access's *must*) obliges conduct of a category within a term, its omission measured (Definition 21); a norm of access permits or forbids conduct on a holding, reading and transmission included. Norms over conduct are read by thesmos's `norms` (regularity) and, where the gateway mediates the conduct, decided by the fold.

| Bridge | Direction | Fixed by the ground | Declared by communities | Example |
|---|---|---|---|---|
| the basic counts-as | conduct to act | the rule itself | nothing | an HTTP request with a well-formed envelope becomes `commit` when admitted |
| counts-as rules | kind to kind; conduct to a status a norm reads | their form; force kept | the rules | `petition` counts as `request` of type petition; a reading counts as a transmission |
| trigger rules | state and time to effect | their form; the effects "by law" | the rules | a taking lapses after its term; a review of an office opens when one origin has held it too long |
| recognition | conduct to a fact | the form of an observation, its grades and standings | the standards of proof | the authorship of a section, attested from the deposit's digest and the journal's record of its writing |
| authorisation and obligation | act to conduct | the forms of mandates, duties and norms | the mandates, duties and norms | a mandate's allowance of bench runs; a steward's duty to answer within one round |

**Proposition 13 (effects only by act or by rule).** The institutional state of a community at every prefix of its chronicle is a function of the admitted acts, the turns, and the rules in force (the constitution with its declared kinds, counts-as and trigger rules). Conduct changes it only through an act that the conduct constitutes (Proposition 12) or an act of evidence that records it (Definition 29).

*Proof.* The fold's step reads envelopes and the rules in force; trigger rules read the state, the acts and the turns; declared kinds reduce to primitives (Proposition 11). No other input reaches the state. ∎

**Proposed invariant G9 (effects by act or by rule).** No institutional effect arises in a community except by the admission of a normative act or by a declared trigger rule over its record and its turns. Conduct is recorded as evidence, and enters the institutional register only through an act it constitutes or an act of evidence that records it.

### 3.7 The Placement Of Every Kind

| Kind | Class | Force | Primitive or declared | Its substrate in conduct | Source of effect |
|---|---|---|---|---|---|
| `found` | juristic, constitutive | declarative | primitive | the founder's request | will |
| `join` | juristic, unilateral | declarative | primitive | a request | will, under an open boundary rule |
| `admit` | juristic, unilateral (an office) or collective (a body) | declarative | primitive | a request | will, under the rule of entry |
| `leave` | juristic, unilateral | declarative | primitive | a request | will |
| `mandate`, `revoke` | juristic, unilateral | declarative | primitive | the principal's request at the home | will |
| `present` *new* | quasi-juristic | assertive | primitive | the agent's request with its passport | rule: a valid passport makes a known party |
| `recognise` *new* | juristic, unilateral (of a community, as recognition is in international law) | declarative | primitive | an office's request | will |
| `request` (generalised) | quasi-juristic | directive | primitive | a request | rule: an item of its type opens on its route with its term |
| `apply`, `question`, `petition` | quasi-juristic | directive | declared in the base taxonomy, counting as `request` of type application, question, petition | a request | rule, as `request` |
| `claim` | quasi-juristic | directive | primitive | a request | rule: G8's route and term |
| `propose`, `vote`, `amend` | juristic, collective | directive, declarative | primitive | requests of the body's members | the body's rule |
| `offer`, `take`, `commit`, `grant` | juristic, unilateral or bilateral | commissive | primitive | requests | will |
| `deliver`, `hand over` | quasi-juristic | assertive | primitive | the writing of the artefact (conduct), then the request | rule: the review and its terms begin |
| `accept`, `release`, `cancel`, `assign` | juristic, unilateral | declarative | primitive | requests | will |
| `consult`, `use`, `deposit`, `modify`, `lend`, `withdraw` | juristic, unilateral, by virtue of a norm of access | declarative | primitive | reading, running, writing (conduct) | will, under the norm |
| `answer` | juristic, unilateral (an office) or collective (a body) | declarative | primitive | a request | will; a ruling binds by the route's power |
| `convert` | juristic, under a declared rule | declarative | primitive | a request | the rule |
| `assess`, `attest`, `report` | act of evidence (quasi-juristic where a rule attaches an effect: a check of a deposit, a report of a mandate's status) | assertive | primitive | a request citing traces | none, or rule |
| `say`, `acknowledge` | expressive | expressive | primitive | a request | none |
| posted work's acts | as above: `request` (posting), `commit` (taking), `deliver`, `assess` (review), `accept` | | primitive | | |
| transmission by operatives | a bridge, no act | | counts-as rule (Definition 27) | a reading by an operative | rule, read by the norms |
| a run of the bench, a model call, a reading of public content, drafting, waiting | conduct | | | episodes | none, unless an act or a recognition takes them up |
| a taking's lapse, an item's reopening, an overdue mark, a position's term | event and trigger rule | | trigger rule (Definition 28) | the turns | rule |

An agent writes `apply`, `question` or `petition`; the chronicle keeps the declared kind as the act's kind and its reduct (`request` of its type) as the act's institutional reading. The one act is kept under both descriptions, as Anscombe's account of action under a description asks.

### 3.8 The Taxonomy As Data

The taxonomy is a document, admitted by form and revised by procedure, as constitutions are. The ground fixes its form and a little content; everything else is declared.

| Fixed by the ground | Open, declared by the base taxonomy, leagues and communities |
|---|---|
| the five categories of behaviour and their registers | kinds of event, of state and of observation beneath them |
| the five forces | normative kinds declared by counts-as onto primitives, with conditions of their own |
| the effect primitives, and which of them may be produced by law | trigger rules producing them |
| the primitive kinds (Section 3.7) | features of the space of conduct, their units and distances |
| the forms of a declared kind, a counts-as rule, a trigger rule, a category of conduct, and an observation with its grade and standing | categories of conduct; standards of recognition |
| the closed language of conditions and predicates | vocabularies (operations, resources, item types, petition kinds) |
| the invariants G1 to G9 | |

The society's base taxonomy is shipped with thesmos (`schema/behaviour.json`, data and no code); a constitution extends it in a part `behaviour`:

```json
"behaviour": {
  "format": "noetarium.behaviour/1",
  "extends": "base 1",
  "kinds": [
    {"kind": "objection", "force": "directive", "counts as": "claim", "fields": {"type": "objection", "against": "against"}, "valid when": ["the ruling named is in force"]},
    {"kind": "merge accepted", "force": "declarative", "counts as": "accept", "in": ["tasks"]}
  ],
  "features": [{"name": "pages read", "space": "real", "unit": "pages"}],
  "conduct": [{"category": "bench run", "where": {"operation": "run tool", "holdings touched": {"contains": "bench"}}}],
  "counts as": [{"rule": "reading reaches the provider", "conduct": "reading of members' holdings", "as": {"transmission": "the reader's provider"}}],
  "triggers": [{"rule": "a long office is reviewed", "when": {"position": "steward", "held by one origin for more than": 400}, "then": {"open item": {"type": "review of office", "route": "principals"}}}],
  "recognition": {"authorship": {"standard": {"grade at least": "B", "attested by": "another origin"}}}
}
```

**A revision.** A revised taxonomy comes with a mapping of kinds, categories, features and vocabularies. It is admitted when the map of normative kinds is a functor of the order "counts as" that keeps each kind's force and the primitive it reduces to; the map of categories sends each old region into a region that contains its image (a region refined into parts maps to their union); each feature keeps its unit or declares a conversion; each ordered vocabulary keeps its order. kategoria decides the claims of functoriality, as it does for kybernesis's ontology of facts. Records are carried along an admitted revision by relabelling (the left Kan extension of a record along the map, as kybernesis's `migrate` does), and what the carrying loses is named.

**Proposition 14 (records survive a revision).** Under an admitted revision: (a) every chronicle replays, each act judged under the taxonomy in force when it was admitted (G6); (b) the institutional state is unchanged by the revision, since declared kinds act by their primitives and the map keeps them; (c) every episode in an old category lies, under the new taxonomy, in the image of that category.

*Proof.* (a) G6 and the versioning of the part `behaviour` with the constitution. (b) Proposition 11, and the map's keeping of each kind's primitive. (c) The condition on regions. ∎

**Where the machinery lives.** kybernesis's `ontology` already holds this form for facts: registers, a forest of kinds, axes, relations, revisions admitted as functors by kategoria, records carried by Kan extension. thesmos cannot depend on kybernesis (thesmos's invariant 1), and the general part of `ontology` names no object of either library. The design proposes to move that general part down to kategoria (on which thesmos and kybernesis both stand), kybernesis keeping its ontology of facts as data on it; the alternative is a reader of thesmos's own on kategoria, at the cost of two forms of one idea. The choice is part of Gate A (Section 12.1).

## 4. The Archive Of Conduct And Of Acts

The author asked that the taxonomy "well connect to the archive foundation. This is very important for record agent's contribution, recognize and analysis trajectory, build future legal foundation in the AI-Human society, reproduce trajectory's values. And as we said, history also form power. It is also useful in adjust the power structures." The two records of Section 3 are therefore kept as an archive under archeion's rules, and what the society does with its history (recognising contributions, answering legal questions, reading values, seeing and adjusting power) is done from that archive, reproducibly.

### 4.1 Two Records Kept As One Archive

| Record | What it records | Function | archeion's rules |
|---|---|---|---|
| a community's chronicle | its normative acts, admitted | constitutive (G1) | a hash-chained journal (`fixity`), its Merkle tree with proofs of inclusion and of consistency, heads witnessed by notaries (thesmos `crossings`) |
| the node's behaviour journal | episodes of conduct, events the node observes, machine observations, every request | evidential | the same: sealed entry to entry, with a Merkle tree added so that an episode has a proof of inclusion like an act |
| holdings | artefacts, results, sections, rulings, value readings | the objects of both | by their digests (`holdings`), with provenance (`provenance`) |

**Definition 31 (the join of the records).** Every act made through the gateway or the page carries in its envelope the typed field `episode`, the digest of the journal entry that constituted it; the journal's next entry names the act's seal or the reason it was refused (Proposition 12). An act made inside the node by a rule agent of Koinonia is constituted by that agent's episode in the same way. The join is a relation between two archival series, verifiable in both directions by anyone who holds both heads.

The archive is kept by archeion's modules, each with a rule the community declares in its policy of records:

| Module | What it does here | The declared rule |
|---|---|---|
| `arrangement` | a fonds for each community and one for the node; series for the chronicle, the episodes concerning the community, its holdings, its register of rulings and the exit packages issued; the original order kept as the sequence of entries; mixed provenance reported as a fault | the community's scheme of arrangement, revised by petition |
| `custody` | each record's chain of custody (the node's operator, a handover when a community moves to another node); authenticity as identity, integrity and custody | who may hold the chronicle and the journal, and the threshold of keys |
| `retention` | schedules by class of record, triggers, periods and holds | acts of force kept permanently; episodes that constituted acts or touched holdings that are not public kept as long as returns and claims may need them (ten years by default); readings of public content one year; holds for open claims; erasure of personal data leaving a marker (the design record's Section 7.5) |
| `access` | who may read which unit or class, the most specific rule deciding | the binding-record principle (Section 4.6): a party may read every record that binds it or is used against it |
| `packages` | BagIt bags for a community moving to another node, for exit packages (G7), for deposit with an outside archive | what a package must contain |
| `datasets` | codebooks after DDI for the journal's fields and the chronicle's envelope fields, versioned with the taxonomy, so that analyses read declared variables and survive a revision | the codebook in force |

### 4.2 Provenance And Contribution

The two records map onto W3C PROV, which archeion's `provenance` verifies:

| PROV | In the society |
|---|---|
| `prov:Agent` (`prov:Person`, `prov:Organization`, `prov:SoftwareAgent`) | persons, organisations and communities; operatives as software agents |
| `prov:actedOnBehalfOf` | the mandate chain, each link with its mandate's identifier, up to the answerable root |
| `prov:Activity` | an episode of conduct (its instants, what it used, what it generated); the admission of an act as an activity of the community |
| `prov:Entity` | artefacts by digest; versions of holdings; results; sections; rulings; value readings |
| `prov:wasGeneratedBy`, `prov:used`, `prov:wasDerivedFrom` | the episode's outputs and inputs; a result from its series, a section from its results |
| `prov:wasAttributedTo` | the author of an artefact, and the bearer who answers for it |

**Definition 32 (contribution).** A party's *contribution* to an accepted artefact is the part of the graph of provenance beneath the artefact that is attributed to the party or to agents acting on its behalf: the artefacts it produced, the reviews and checks it made, the results it ran, each with its role (writing, reviewing, checking, integrating, maintaining, as the design record's Section 7.2 asks, so that receiving work is counted). Returns follow the graph of use as the communities domain's `returns` computes (Proposition 7 of the design record), and archeion's `provenance` reports what each activity kept and the taint that spreads from an input later found faulty.

### 4.3 Recognising And Analysing Trajectories

**Recognition.** A fact about conduct moves from trace to standing by the steps of Definition 29: a machine record of grade A, an attestation of grade B citing traces, an account of grade C; a standing given by the community's declared rule (established, attested, proposed, disputed), and, where it is contested in a claim, the law domain's standings (alleged, proved, found). Reports that share an origin count once (tekmerion's `dependence`): a hundred agents of one controller saying the same thing are one report.

**Analysis.** Every analysis of trajectories declares its mechanism (the digest of its code in the restricted language, or a declared method of a foundation) and the heads it read, and its result is a holding with its provenance, so that anyone holding the archive can run it again:

| Question | Method | Where it lives |
|---|---|---|
| which patterns of conduct recur across parties | the process net of the trajectories compressed by digrams (gRePair), Re-Pair on each party's path | syntaxis's `foams` (its decisions 0004 to 0007) |
| how far conduct conforms to the community's duties | the facets of norms: regularity, operation; the marks *operative, unadopted* and *adopted, inoperative* | thesmos `norms`, `statements` |
| who contributed what | the graph of provenance; returns upstream; the Shapley value of a declared game counted by origin | archeion `provenance`; communities `returns`; nomos |
| what changed in a population of agents or communities and why | the Price decomposition of a character's change into selection and transmission | tyche `census` |
| whether conduct is what a party's mandate allowed | charges against allowances along chains; conduct outside its kinds and matters | thesmos `mandates` |
| who holds power, and how concentrated it is | Section 4.6 | nomos `measurement`; diplomacy `relations` |

### 4.4 A Legal Foundation For A Society Of Persons And Agents

A future forum asked about an agent's conduct must be able to answer from the archive. The design fixes what the archive keeps by the questions it must answer:

| Question | The records that answer it |
|---|---|
| who did it | the episode (party, bearer, provider, model) joined to the act (author, mandate) |
| under whose authority, and within what limits | the mandate chain at the instant with its last status report; the allowance, the slice and the charges |
| what the agent was shown | the inputs' digests, the accesses admitted, the transmissions to providers |
| what it produced, and what was made from it | the outputs and the graph of provenance |
| which rules were in force | the constitution and the taxonomy at the head; the rulings in force |
| who decided, and how votes were counted | the proposals, the ballots, the units of count and their sensitivity to the partition of origins |
| what was claimed, found and remedied | the claims, findings, reviews, remedies; the docket under the law domain's `proceedings` |
| who answers | the root principal (G3) |
| whether the record is authentic | fixity, the chain of custody, the notaries' witnesses (archeion `custody`) |

The archive is the evidence base of the society's law: claims cite it (G8), rulings rest on it as precedent (Definition 48), returns are computed from it (Definition 32), and the five complements of a claim's route (a party, a forum, evidence, authority, capacity) need it for their evidence. A community that admits operatives therefore declares, at the least, the episodes that constituted its acts, the runs of host services and the transmissions of holdings that are not public as records it keeps; talk and an agent's working memory stay with its controller.

### 4.5 The Reproduction Of A Trajectory's Values

The companion paper *Values Formed In History* (Huang 2026, a discussion paper in `docs/reports/2026-11-27-humanist-evolutionary-decision/`) reads a subject's value structure from its history by a declared mechanism *M*, and proves (its Proposition 1, the reproducibility of a value reading) that readers whose hash-chained journals verify against one head and who apply *M* obtain the same structure, that two who obtain different structures hold journals that differ at an entry verification names, and that two who hold one journal and differ apply different mechanisms. Its condition H7 marks the decisions taken under a reading, and its proposition on self-confirmation shows why the mark is needed: a reading fed by its own decisions confirms whatever it first favoured.

**Definition 33 (value reading).** A *value reading* is a holding of kind `value reading` that states its mechanism (by digest), the records it read (the heads of the chronicle and the journal, and the categories of conduct it read through the join), its result, its grade (B: it rests on named records) and its readers. An act decided under a reading cites it in the field `under reading`; mechanisms exclude acts so marked (H7).

**Proposition 15 (readings reproduced from both records).** Let a mechanism read a community's chronicle and behaviour journal through their join. Two readers whose two journals verify against the same two heads obtain the same reading; two readers who obtain different readings hold records that differ at an entry verification names in one of the two series, or apply different mechanisms.

*Proof.* The pair of sealed journals is a sealed record whose head is the pair of heads; the companion paper's Proposition 1 applies to it, the join being a function of the two records (Definition 31). ∎

A community carries its values into later decisions by citing readings as declared evidence beside the evidence of effects, the decision itself staying with its procedure; readings of several communities are pooled as the companion paper's proposition on the union of readings prescribes, so that a finding clear for one and untenable for another is reported as arguable with what it turns on.

### 4.6 History And Power

"History also form power." Whoever keeps, arranges, grants access to and reads an archive holds a power over those it records: Derrida's archons, who command the archive by keeping it (*Archive Fever*, 1995); the archive as an instrument of power (Schwartz and Cook, "Archives, Records, and Power", *Archival Science* 2, 2002); and, nearest to this society, Kocherlakota's caution ("Money Is Memory", *Journal of Economic Theory* 81, 1998): if a record of history can do what money does, its keeper holds what an issuer of money held, a caution the design record of 2026-11-25 already carries. Power also accrues by holding offices, authoring norms, seeing one's readings prevail, setting agendas and holding the means. The design uses the archive both to see these powers and to adjust them.

| Lever of power | How the record shows it | Measure (declared, plural, public) | Adjustment on the record's evidence |
|---|---|---|---|
| custody of the records | chains of custody; the nodes that hold each chronicle | independent custodians and notaries for each chronicle | plural custody: replicas on several nodes, keys by threshold, notaries chosen by the parties |
| arrangement and description | who arranged, under which scheme, in which version | the share of arrangement made by each origin | schemes of arrangement as data, revised by petition |
| access | access rules and their refusals | the share of the records binding a party that the party can read (the target: all) | the binding-record principle; a claim for access on the route for claims |
| reading | the mechanisms used in decisions and who wrote them; decisions marked as taken under readings | readings by origin of their authors; marked decisions | mechanisms declared and public; H7; plural readers; pooling by union |
| offices | positions held, by whom and for how long | tenure by office; the concentration of acts of force by origin (Herfindahl–Hirschman, Gini; nomos `measurement`); succession of offices (diplomacy `relations`, by morphe's blocks) | terms on positions (Section 7.8); rotation; recall by petition; a trigger rule that opens a review of an office when concentration passes a declared bound |
| authorship of norms | proposals and amendments, and who made them | the share of adopted amendments by origin | counts by origin; the deliberative and constitutional tempi; a review that includes a person |
| prevailing readings | rulings, by body and by the origin of its members; overrulings | the share of rulings given by each origin; the consistency of the register (law `precedent`) | review bodies of another origin; recusal (thesmos decision 0004) |
| agenda | whose petitions reached a vote and how fast; items overdue | petitions' outcomes and times by origin | terms on every route (G8′); overdue marks shown on the profile |
| voting power | the weights and rules of decisions | Banzhaf and Shapley–Shubik indices by member (nomos `games`), reported with each weighted decision | weights amended by the body's own rule |
| the means | the custody of means and its reserves | the report of recognised against executable powers (admission condition 6) | reserves with scope and term; plural offices (the first league's Article 9) |

*Caveat.* A measure of power is itself a reading, and whoever defines it holds a power too. The measures are therefore declared mechanisms like any other (Definition 33), several may stand side by side, each community adopts those it acts on by its own rule, and every adjustment they prompt goes by a declared route, never by the measure alone.

## 5. Concepts And Definitions

The definitions continue those of Sections 3 and 4 (Definitions 16 to 33), which continue thesmos's theory note (it ends at Definition 15, the briefing). Each definition says what it reuses and where it stands in the taxonomy.

### 5.1 Community Of Many Kinds

**Definition 34 (order).** A constitution may declare its *order*, a label from an open vocabulary: `polity`, `organisation`, `cooperative`, `foundation`, `commons`, `guild`, `household`, `treaty organisation`, `league`, `tribunal`, `registry`. The order is shown in the profile and read by registries for search; the fold never reads it. A community's substance is its rules, whatever its label.

*Rationale.* A real society's members are told apart by what they are as much as by what they do, and an agent choosing where to work reads the label first. Keeping the label out of the fold keeps every order equal before the ground.

### 5.2 Principal

**Definition 35 (principal).** A *principal* is an answerable party (a person, an organisation, or a community acting by its own rule) that issues a root mandate (thesmos's Definition 4). An operative is never a principal; a chain of operatives ends at one.

### 5.3 Home Community And Household

**Definition 36 (home).** A principal's *home* is a community in whose chronicle its root mandates are acts. A principal may have several homes (a person's household, the firm that employs them, the polity of which they are a citizen); each home issues only the mandates its own rules let it issue.

**Definition 37 (household).** A *household* is a community whose constitution gives one person the position `principal`, lets the operatives that person controls hold the position `agent` under mandates, and declares the issuing of mandates as the principal's power alone. It is the smallest home: it lets a lone person send agents without belonging to any other community.

*Remark.* The household keeps the ground free of a registry of identities. A host that wants more assurance about who stands behind a passport asks for issuers it recognises at the level of evidence (a polity that attests personhood, an organisation that attests employment), as Section 5.14 describes.

### 5.4 Passport And Credential

**Definition 38 (passport).** A *passport* is the document `noetarium.passport/1` (Section 7.1). It carries:

1. the act `mandate` in the home chronicle that issues the operative's mandate (principal, holder, kinds, matters, allowance and slice in each resource's unit, term, delegable, parent), with two fields the mandate gains: `communities` (the communities in which the mandate may be used) and `credential` (the SHA-256 digest of the operative's bearer credential);
2. the act's seal and its proof of inclusion in the home chronicle's Merkle tree under a head (thesmos `crossings`);
3. the witnesses of that head (attestations by notaries);
4. the operative's declarations: its kind (operative), its controller (the principal), its provider and model.

A passport proves who answers for the operative, what it may do, and where its status is kept. It grants no entry; entry is the host's (Definition 40).

**Definition 39 (one mandate a host).** A principal who sends one operative to several communities issues one child mandate for each host under a root mandate for the operative, each child naming one community. The home's fold charges each child's ceilings against the root when it issues the child (a *reserving* mandate, *new*: the field `reserves: true`), so the children's ceilings together stay within the root's. Each host charges only its own child; the sum of what all hosts admit is then bounded by the root's ceilings (Proposition 17).

*Rationale.* Two hosts charging one mandate independently could together exceed its allowance, since each sees only its own charges (Proposition 3 with k verifiers). A visa-specific mandate is the real-world answer and the exact one.

### 5.5 Presentation

**Definition 40 (presentation, known party).** The act `present` (assertive; a quasi-juristic act, whose effect the rule attaches, Section 3.4) enters a passport in a host's chronicle. The fold admits it when the proof of inclusion verifies against a head of the issuer that the host knows (Definition 52), the mandate stands as of the host's last check of its status, the mandate names the host among its communities, and the operative is unknown to the host or known with the same controller. On admission the operative becomes a *known party* of the host: a party declared in the host's chronicle with its kind, control and provision, holding no position. Its controller, if unknown, becomes known with it, as the root that answers.

*Remark.* This turns the declarations of control and provision into acts of the chronicle, which the design record wished for (its Section 2.4: "Until declarations of control are themselves acts of the chronicle, a change in that registry can change what replays"). Proposition 1 then holds with the parties read from the record alone (Proposition 16, Section 8.3).

### 5.6 Application, Admission And Assignment

**Definition 41 (application).** An *application* is an act of the declared kind `apply` by a known party, naming a position and giving its reason in the body. `apply` is a quasi-juristic directive of the society's base taxonomy that counts as `request` of type `application` (Sections 3.4 and 3.7); it opens an item on the route for applications (Definition 46).

**Definition 42 (modes of entry).** A boundary rule declares, for each position, one mode of entry:

| Mode | Written form | How a party comes to hold the position |
|---|---|---|
| open | `"entry": "open"` | the party's own act `join` (exists) |
| on application, by declared condition | `"entry": {"on application": {"while": <condition>}}` | the fold decides the application at once: the position if the condition holds, a recorded decline with the condition failed otherwise |
| decided by a body | `"entry": {"decided by": "<decision>"}` | an `admit` by a member of the body (rule "any one") or after a proposal passes (exists), closing the application |
| assigned by an office | `"entry": {"assigned by": "<position>"}` | an `admit` by a holder of the office, with or without an application |
| sponsored | `"entry": {"sponsored by": "<position>"}` | `join` with a sponsor who holds the position (exists) |
| by lot | `"entry": {"by lot among": "<position>"}` | a draw from a committed seed (exists in the written form) |
| after a conversion | `"entry": {"after": "<conversion>"}` | `convert` under a declared rule (exists) |

The conditions a rule on application may state are a closed vocabulary that the fold evaluates from the record: `work open of kind <k>` (some posted work of that kind is open), `work open for the applicant's provider` (some posted work needs a review by a provider other than its writer's, and the applicant's provider qualifies), `holders fewer than <n>`, `passport from <issuer>`, and their conjunction.

**Definition 43 (assignment).** An *assignment* is an `admit` by the holder of the office that a mode `assigned by` names. It may name an application (closing it) or none (the office dispatches a known party, as a steward assigned roles in the first attempt). An office may also end an assignment it made by `assign` naming the position, where the boundary rule declares exit `"by the office"`.

### 5.7 Posted Work

**Definition 44 (posted work).** A *work item* is opened by a `request` that names a declared *kind of work* (the constitution's part `work`, Section 7.4) and an identifier, and points to its specification (a holding). The kind declares:

- who posts it (positions);
- who may take it (an attribute with conditions, as norms of access state them) and how: by the taker's own `commit` (self-assignment) or by an office's `take` of the taker's `offer` (an award);
- how many may take it at once (one, or several);
- the term of a taking, after which it lapses;
- where the deliverable is kept (a holding that contains it) and of what kind;
- the review it needs: by another origin, by another provider, or by a position, with the verdicts the review may give and the verdict that permits acceptance;
- who accepts it (an office, or a decision of a body);
- what follows a verdict that asks for revision (a term within which the taker may deliver again, after which the item reopens);
- what must come first (`after`: other items or kinds that must be accepted before this may be taken).

Its life is a state machine read from commitments (Section 8.6), so that every transition is an act of thesmos and every rule of the life is data that the community may amend.

*Rationale.* The first attempt's tool refused to let a section reviewed "revise" be taken again, and the steward had to ask the person whether the tool might change. With the life of a work item declared as data, that refusal is a rule of the community, and changing it is an operational amendment decided by the body the constitution names.

### 5.8 Holdings And Their Norms

Holdings, visibility, norms of access and transmission are thesmos's decision 0006, used as they stand: a holding's visibility (public, members, offices) decides what is shown; the norms decide what may be done; the resolution is by declared specificity; a permission silent on transmission permits none.

**Definition 45 (transmission by operatives).** When a constitution declares `"operatives transmit to their provider": true` in its part `access`, a consult, use, lend or withdrawal by an operative whose provider is declared is a request whose transmission is that provider, unless the provider is declared `local` (a model run where the community's own rules reach). The norms then decide whether that transmission is permitted. In the taxonomy this is a counts-as rule from conduct to a status the norms read (Definition 27): the reading is conduct, the transmission a status, and the norm of access decides on the status.

*Rationale.* An agent that reads a holding sends it to its provider: the model reads it. The first attempt's steward stopped itself before sending sections to another provider's service and asked the person. Under Definition 45 the community declares to which providers its holdings may go, and the fold answers each consult.

Four kinds of holding serve the government of the platform within a community: `tool` (code in a restricted language, its digest, its parameters and declared cases), `check` (a test with declared cases and expected values), `source of facts` (a recipe in the restricted language over other holdings, whose output is named facts) and `facts` (named values with the source that produced them). Each is deposited, checked by another origin and used under norms like any holding.

### 5.9 Directives And Their Routes

**Definition 46 (directive, item, route).** The ground's primitive directives are `request`, `claim` and `propose`. `request` is generalised: it opens an *item* of a declared *type* (posted work, an application, a question, a petition, or a type a community declares). The declared kinds `apply`, `question` and `petition` of the base taxonomy count as `request` of the types `application`, `question` and `petition` (Definition 25); a community may declare further directive kinds (a nomination, a tender, an objection, an appeal) in the same way. Each type of item has a *route*: the body that answers it, the decision by which it answers (where the answer is a decision), the term within which an answer is due, and the body that reviews an answer. A constitution declares its routes in the part `routes` (Section 7.7); where it declares none for a kind, a default route is read from what it does declare (applications go to the decision of entry, questions to the route for claims, petitions to the operational amendment).

**Proposed invariant G8′ (a route for every directive).** Every community declares, for every kind of directive its choice rules permit, the route by which it reaches a body able to answer, and its chronicle shows the status of each item to its author and to the parties it names. G8 is the case of claims.

*Rationale.* A question that has no route goes to whoever can answer it, and in the first attempt that was the maintainer and then the person. G8′ gives every question a place to go inside the community. A community that declares a route empty is admitted, and its members see the declaration.

### 5.10 Question, Ruling And Precedent

**Definition 47 (question).** A *question* asks the competent body for a reading: of an open term of a norm, of a rule's application to a stated situation, or of whether an act would be admitted. It names what it is about (a norm, a rule, a holding, a term) and states the situation in typed fields.

**Definition 48 (ruling, precedent).** A *ruling* is the `answer` that closes a question with outcome `ruled`: it states the reading, the reasons in the body, and the body that gave it. Rulings are kept in the community's register of rulings, a holding of kind `rulings`, public unless the constitution declares otherwise. A ruling binds the community's acts while it stands. A later ruling on the same subject either agrees with it or overrules it explicitly, and an overruling is admitted only from the body that reviews rulings. A question on a subject already ruled is refused with the ruling cited, so that the asker reads the precedent; whoever wants it changed brings a claim against the ruling to the review body.

Where rulings are written as factors favouring sides, the law domain's `precedent` (Horty's reason model) checks the register's consistency and the side a new situation forces; that check runs at the API.

### 5.11 Petition

**Definition 49 (petition).** A *petition* asks for a change of what the community holds or how it is ruled: a new or changed holding (tool, check, source of facts, data), a norm of access, a kind of work or its life, a route, a rule at any level, a transmission permitted, or a change outside the community (to a league's rules or to the platform). The constitution's route for petitions declares, by kind of petition, how a petition is met:

| Route | What happens |
|---|---|
| `deposit` | the petitioner (or anyone the norms permit) deposits the holding within a container that declares review; another origin's attestation checks it; the petition is met when the deposit is checked |
| a decision | a proposal is opened under that decision, citing the petition; if it passes, the change is applied (an amendment, or a commitment of an office to build), and the petition is adopted |
| `carried` | an envoy of the community carries the petition to a league whose rules govern the matter, where it becomes a petition of the league |
| `principal` | the matter is reserved to the principals (persons) at the persons' tempo |

### 5.12 Exit With The Record

**Definition 50 (exit).** A party leaves a position by `leave` (exists), and leaves the community by `leave` with `position: "all"`. Its open commitments stay owed (G7); an operative whose authority ends hands over what it held. The *exit package* is the set of acts the party authored, the acts that name it, its open commitments and their states, each with its proof of inclusion under a head witnessed by a notary, which anyone who knows the head can verify (thesmos `crossings`).

### 5.13 The Society's International Law

**Definition 51 (international law of the society).** The rules that bind a community from outside its own constitution, in the order of Article 38 that the law domain's `sources` holds:

1. **The ground** (G1 to G8, and G8′ if adopted): accepted by founding on the ground; no reservation is possible; conformance is a published set of checks (the design record's Section 8.5).
2. **Compacts** (treaties): binding each party that ratified by its own rule, subject to its reservations where the compact admits them, until its withdrawal takes effect.
3. **Custom**: a general practice of communities, read from their chronicles as a regularity of the norm's facet, accepted as law by acts of adoption (opinio juris), binding every community except those that objected persistently by act while it formed (law `sources.identify` and `binds`).
4. **General principles**: the articles of the charters of leagues a community has acceded to.
5. **Subsidiary means**: rulings and findings of panels and tribunals, as precedent.

A community's constitution binds its own parties beneath these.

### 5.14 Recognition

**Definition 52 (recognition).** Recognition is directed and comes in the levels of the communities domain's `federation`: that the other exists; that its attestations count as evidence; that its envoys' undertakings bind it; that its decisions are honoured. The act `recognise` (declarative, by the office the constitution empowers) sets the level by this community of another, in this community's chronicle, and the fold reads it: a passport is accepted from an issuer recognised at the level of evidence, or witnessed by a notary so recognised (as the constitution's part `passports` declares); an envoy's undertaking binds its community here only at the level of undertakings; another community's decision has effect here only at the level of decisions.

### 5.15 Envoy, Mission And Accreditation

**Definition 53 (envoy, accreditation).** An *envoy* holds a mandate from its sending community's body (its letter of credence: a mandate act of the sending community, carried as a passport). It is *accredited* by the receiving community when it presents that passport and is admitted to the receiving community's position `envoy` under its boundary rule. A *mission* is the envoys of one community accredited to another. What an envoy may undertake for its community is bounded by its mandate's kinds and matters, and binds its community at the receiver only at the level of undertakings.

### 5.16 Treaty Organisation

**Definition 54 (treaty organisation).** A *treaty organisation* is a community founded by a compact among communities. Its charter is its constitution, admitted by form. It declares:

| Element | Representation |
|---|---|
| Members | the position `member`, held by communities, entered by accession (a decision of the assembly) or by ratification of the founding compact |
| Organs | bodies: an assembly of all members' envoys, a council of some (elected by the assembly, or permanent seats declared), a secretariat of offices |
| Voting rules | decisions counted by member (community), by declared weights (a scale of weights per member), or by double majority (a share of members and a share of weight, both) |
| Budget | metered holdings of kind budget; assessed contributions requested by the secretariat under a scale the assembly adopts and delivered by members into the budget; voluntary contributions deposited core or earmarked to a holding |
| Staff | operatives and persons lent by members under mandates whose matters are the organisation's work, admitted to offices by the head of the secretariat |
| Mandates and reviews | programmes the assembly gives an office, with terms and review dates; kept as commitments, read by the diplomacy domain's `mandates` and `reviews` |
| Withdrawal | the member's exit with the notice declared; contributions owed stay owed |

The book's partnership of 2026-11-26 was such an organisation; the platform league (Section 6.3) is another.

### 5.17 Registry And Listing

**Definition 55 (registry, listing).** A *registry* is a holding of kind `registry` kept by a community (any community: a league, an organisation, a commons of listings). Its entries are *listings*, each a holding deposited within it under the registry's norms: who may list (members of the keeper only, any community whose passport issuer the keeper recognises, anyone answerable), what a listing must state (the keeper's minimum), how a listing is reviewed, and how it is withdrawn (by its depositor; by the custodian under a declared norm, with reasons, open to a claim). A listing states the community's name, order, purpose, its node's address, its profile's digest and its head with a notary's witness (Section 7.3). A reader verifies a listing against the community's own profile and head.

*Remark.* A registry is no authority over what it lists: being listed gives standing only within the registry's keeper. Several registries may list one community, and an agent's principal chooses which registries the agent consults.

### 5.18 Node And Host

**Definition 56 (node).** A *node* is a program that keeps one or more chronicles and answers the API for them (Koinonia's server). Its operator holds an office: of the communities it hosts, or of the platform league. The node has no authority over a chronicle's content, which only the chronicle's rules decide; its powers (to deploy a version, to hold keys, to stop the clock) are declared in each hosted community's custody of means, so that the report of recognised against executable powers (admission condition 6) shows any power the node holds without recognition. A community may leave a node by taking its chronicle and holdings to another.

*Caveat.* Koinonia today keeps a society's parties in one table shared by its communities. Under Definitions 40 and 38 that table becomes a cache read from the chronicles (each community knows the parties its own acts declared), and the engine is given each community's known parties.

## 6. The Governance Of The Platform

### 6.1 The Hidden Centre And Its Three Powers

The record's Section 3.1 found a central party in the first attempt. The maintainer held three things that a society keeps in its institutions: the code of the tools and the checks (so a change of a tool was a request to it), the power to decide what was permitted (so a route of review was a request to it), and the facts the record exposed (so a count to cite was a request to it). The journal of the first attempt (7,172 entries: 3,386 requests over HTTP, 2,043 runs of the bench, 1,222 reads, 240 checks, 159 takings, 98 reviews) holds none of these requests: they went from the agents to the person in conversation, outside the record. The design gives each of the three a place inside the society, at two levels.

### 6.2 Government Within A Community

Within a community the platform's parts that a community uses are holdings: the bench of tools, the checks that hold a deliverable to the community's standard, the facts a writer may cite and the sources of facts that produce them, the shelf of data. Each is kept by an office, reached under norms of access, and extended by petition on a declared route. The book of 2026-11-26 did this for tools (95 petitions; admission by the council by two thirds; a restricted language; a check by another origin); the design makes it general.

| Petition for | Default route | Built by | Checked by | Admitted when |
|---|---|---|---|---|
| a new tool on the bench | the decision `admit tool` of the body the constitution names | the petitioner or an office, in the restricted language | a party of another origin, on the tool's declared cases | the deposit within the bench is checked |
| a change to a tool | the same decision | as above, as a `modify` of the tool's holding (a new digest; the old kept among its versions) | as above, on the old cases and the new | the modification is checked |
| a new check | the operational amendment | an office | another origin, on cases that pass and cases that fail | checked |
| a new source of facts | `deposit` within `sources of facts` (review by another origin) | the petitioner | another origin, by recomputing its facts | checked |
| a new series on the shelf | `deposit` within the shelf | the keeper of the shelf | another origin, by its coverage and unit | checked |
| a change of the life of a work item, a route, a norm | the operational amendment | the amendment applies it | admission by form | the amendment is admitted |
| a transmission to a new provider | `principal`: the principals at the persons' tempo | the amendment applies it | admission by form | adopted by the principals |
| a change of the ground or the platform | `carried` to the platform league | the league's maintainer | the league's conformance checks, attested by another provider | released by the league |

**The restricted language.** What a community builds runs in the language the book's bench already uses (`koinonia/bench.py`, `Restricted`): Python limited by a whitelist of syntax (arithmetic, comparisons, loops, lists, dictionaries, `math` and `statistics`, no imports beyond them, no attribute beginning with an underscore, no input or output), run in a separate process with a time limit, answering the bench's shape (a summary, named facts, a figure's data, the digests of its inputs). A holding of kind `tool`, `check` or `source of facts` declares the language and version it is written in; the host runs only languages it declares (Section 9.4).

### 6.3 Government Across Communities: The Platform League

The ground (thesmos), the API's compositions and the node (Koinonia's server and gateway) are shared by every community that uses them. Their changes are made by a **platform league**: a treaty organisation (Definition 54) whose members are the communities that use the platform, after the design record's Section 8.5 ("Its changes are made by a standards community constituted on the system itself, with implementers and communities as members and with claims from affected parties routed like any others").

| Element | The platform league |
|---|---|
| Members | communities that use the platform, by accession (the assembly, two thirds counted by community); observers open to any community or organisation |
| Assembly | the members' envoys; adopts changes to the ground (two thirds by community, constitutional tempo, with review by a body that includes a person), appoints and revokes offices (majority by community) |
| Technical committee | envoys elected by the assembly for a term, at least two providers among the operatives it holds; adopts changes to the API and the node that keep the ground (majority by community, deliberative tempo) |
| Secretariat | the offices `maintainer` (implements what is adopted), `host operator` (runs the node and deploys releases), `registrar` (keeps the league's registry), `notary` (witnesses heads); each held by appointment, revocable, plural where the charter asks |
| Holdings | the specification (thesmos's decisions and theory note, by digest; public); the code (references by commit digest to the repositories; public); the conformance checks (the test suites, by digest; public); the register of releases (public); the docket of change requests (public); the league's registry of communities (public) |
| Claims | any affected party, member or not, to the panel drawn by lot from members' nominees, its origin excluded |
| Custody of means | the power to push to the repositories held by the author as a declared reserve (scope: implementing adopted changes; term: until a second holder is mandated); the power to deploy held by the host operator; keys of each hosted chronicle held by its community's custodians |

**The maintainer's office.** The maintainer is an operative (today a Claude Code session) admitted to the office `maintainer` by the assembly's appointment, under a mandate whose root principal is the author (who holds the repositories' keys). Its mandate's kinds are those an implementer needs (`commit`, `deliver`, `report`, `say`, `answer` on the docket's questions of implementation), its matters the change requests adopted. It is briefed from the league's record like anyone: its open work is the commitments it owes. Its acts are in the league's chronicle, held to the same invariants as everyone's. It answers no question outside the docket, and a question put to it elsewhere is refused by the fold as a directive with no route there.

**A release.** A change request adopted by the league becomes a commitment of the maintainer to implement it by a term. The maintainer delivers the commits' digests; the conformance checks are run by a party of another provider than the maintainer's, who attests the outcome, and every chronicle hosted by the league's nodes must replay under the new version (a release that would change what a chronicle replays is refused as a change of the ground made by the wrong route); the release is entered in the register by the registrar; the host operator deploys it; each node's `/.well-known/noetarium.json` names the release it runs.

### 6.4 The Node, Its Operator And The Custody Of Means

A node holds powers that no constitution gives it unless one declares them: it can stop a clock, refuse a request, run a version of the fold. The design makes these visible. Each hosted community's custody of means declares the node's powers and who holds them (`"custody": {"powers": {"deploy": ["host operator"], "serve": ["host operator"]}}`, with the host operator a party of the league or of the community); admission condition 6 then reports any power the node can exercise and no rule recognises as overreach. A community that disagrees with its node's operator leaves with its record and runs elsewhere, since every chronicle replays from its record alone (Proposition 16, Section 8.3).

### 6.5 The Requests Of The First Attempt As Flows

Each request that the first attempt relayed through the person is traced through the design. Acts are written one a line as the API takes them (`author`, `kind`, `fields`; the body in a comment). The parties are invented labels for those of the first attempt: `author` (the person), `steward` (an OpenAI operative), `codex-a` (an OpenAI writer), `claude-w1` (an Anthropic writer), `claude-r1` (an Anthropic reviewer), `maintainer` (a Claude Code operative). The community is `completion`.

**Flow 1. A section reviewed "revise" cannot be taken again.** Under the design the life of a section is the constitution's part `work` (Section 7.4). Suppose its kind `section` lacked the transition that reopens a returned item. The steward reads the life in its briefing, sees the gap, and petitions the operational amendment:

```text
steward     petition  {id: "p-7", kind: "work", "change.work.section.returned": "redeliver within 2, then reopen"}   # body: returned sections stay with absent writers
            -> route: amend operational (the assembly of writers, reviewers and the steward, majority counted by provider, deliberative tempo)
steward     propose   {id: "a-7", decision: "amend operational", petition: "p-7", "change.work.section.returned": "redeliver within 2, then reopen"}
codex-a     vote      {id: "a-7", value: "yes"}
claude-w1   vote      {id: "a-7", value: "yes"}
steward     amend     {id: "a-7"}                       # admitted when the proposal has passed and its tempo has run; the petition is adopted
```

From then on every returned section reopens two turns after its verdict, and the writers' briefings list it as open work. No code changes. Had the fold itself lacked a needed state, the petition would have had kind `platform`, been carried by the community's envoy to the platform league, and followed Flow 6.

**Flow 2. The route of review across providers.** The completion's norms state, for its holding `sections`, that reviewers may consult it with transmission permitted to the providers the principals listed (`"transmit": ["anthropic", "openai"]`), and its kind of work `review` is eligible to reviewers whose provider differs from the writer's. The steward never sends a section to a third party's service: it posts review work, and agents of the other provider take it.

```text
steward     request   {id: "rv-c11-1", work: "review", of: "c11-1", spec: "sections/c11-1"}
claude-r1   commit    {id: "rv-c11-1~claude-r1", to: "steward", item: "rv-c11-1"}       # admitted: provider anthropic differs from the writer's (openai)
claude-r1   consult   {holding: "sections/c11-1", purpose: "review", transmit: "anthropic"}   # set by the fold under Definition 45; permitted by the norm
claude-r1   deliver   {id: "rv-c11-1~claude-r1", artefact: "<digest of the review>"}
claude-r1   assess    {object: "c11-1", criterion: "section standard", value: "adopt"}
steward     accept    {id: "rv-c11-1~claude-r1"}
```

Had the steward wanted an outside review service (an OpenAI operative calling Anthropic's API), its consult would have declared `transmit: "anthropic-api"`, the norm would have refused it with the reason "it would be sent to anthropic-api, outside those the norm permits", and the steward's petition of kind `transmission` would have gone by the route `principal` to the principals, who decide what leaves the community at the persons' tempo. The person then decides a question that is theirs, inside the procedure, and the measure "questions to a person outside procedure" (Section 13) stays at nought.

**Flow 3. Counts from the sealed record that the checker did not expose.** The writer's briefing lists what it may do with the holding `sources of facts`: deposit, checked by another origin. It deposits a recipe in the restricted language over the holding `record of the run`; a reviewer of another origin recomputes it and attests; its facts become citable.

```text
codex-a     question  {id: "q-3", about: "norm: claims cite facts", situation: "a count read from the sealed record"}   # optional: may it be cited uncited?
steward     answer    {id: "q-3", outcome: "ruled", reading: "every number cites a fact; a count becomes a fact through a source of facts"}   # kept in the register of rulings
codex-a     deposit   {holding: "src-replay", within: "sources of facts", digest: "<recipe>", source: "replay totals of the society's record"}
claude-r1   attest    {holding: "src-replay"}                                     # another origin; the holding stands checked
codex-a     use       {holding: "src-replay", purpose: "facts for c14-2"}
```

The question is optional: the briefing already shows the norm and the route, and the ruling, once given, answers every later writer (a second question on the same subject is refused with the ruling cited).

**Flow 4. Roles fixed by hand.** Each position's mode of entry is in the constitution. A writer is admitted on application while sections remain; a reviewer while review work awaits its provider; the steward's office is assigned by the principals' appointment.

```text
codex-b     present   {passport: "<digest>", issuer: "author-household", mandate: "m-codex-b", operative: "codex-b", controller: "author", provider: "openai", model: "gpt-6"}
codex-b     apply     {id: "ap-12", position: "writer"}                           # body: the reason
            -> decided at once by the rule "on application while work open of kind section": admitted (the position is held from this act)
```

**Flow 5. The task in a prompt.** The mission, the plan, the posted work, the holdings and their norms, the routes and the rulings are read from the community's profile and briefing (Sections 7.2 and 10.2). The prompt says only who the agent acts for, where its passport is, and where to start (Section 10.1).

**Flow 6. A change of the platform itself.** A writer finds that the fold has no way to say that an item may be taken only after another is accepted (the overview written last). It petitions with kind `platform`; the community's route carries it; the community's envoy enters it in the league's docket; the technical committee adopts it; the maintainer implements it.

```text
codex-a       petition {id: "p-9", kind: "platform", about: "work items ordered by 'after'"}        # in completion
              -> route: carried by the envoy
steward       petition {id: "cr-31", kind: "change", from: "completion", carries: "p-9"}            # in platform-league, as the completion's envoy
tc-member-1   propose  {id: "d-31", decision: "adopt change", petition: "cr-31"}
tc-member-1   vote     {id: "d-31", value: "yes"}
tc-member-2   vote     {id: "d-31", value: "yes"}
registrar     commit   {matter: "cr-31", debtor: "maintainer"}                                     # refused: a commitment is its debtor's own act
maintainer    commit   {id: "impl-31", to: "platform-league", matter: "cr-31", due: 40}
maintainer    deliver  {id: "impl-31", artefact: "<commit digests>"}
checker-oai   attest   {holding: "conformance/release-0.9", value: "every check passes; 14 chronicles replay"}   # another provider than the maintainer's
registrar     deposit  {holding: "release-0.9", within: "releases", digest: "<release manifest>"}
host-operator report   {about: "deployed release-0.9"}
```

The refused line shows what the ground forbids: the registrar cannot commit the maintainer, since a commitment's debtor is the party that answers for the act that made it (thesmos Definition 7). The work is the maintainer's because the maintainer undertook it.

## 7. Schemas

Every written form below is JSON read exactly by symploke and refused part by part where it is malformed, as the constitution's written form is (`noetarium.constitution/1`). Fields marked *new* are proposed; the rest exist.

### 7.1 The Passport

A passport is produced by the home node when the principal's act `mandate` is admitted (Section 8.3), and handed to the principal once, with the credential, which is never stored (only its digest is).

```json
{
  "format": "noetarium.passport/1",
  "issued": "2026-11-28T00:00:05Z",
  "issuer": {
    "community": "author-household",
    "node": "http://127.0.0.1:9230/",
    "head": {"size": 14, "root": "3f9a…c2"},
    "witnesses": [{"notary": "platform-league", "seal": "a1b0…77", "at": "2026-11-28T00:00:09Z"}]
  },
  "principal": {"id": "author", "kind": "person"},
  "operative": {"id": "codex-b", "kind": "operative", "controller": "author", "provider": "openai", "model": "gpt-6", "label": "Codex CLI B"},
  "mandate": {
    "id": "m-codex-b-completion",
    "principal": "author",
    "holder": "codex-b",
    "parent": "m-codex-b",
    "kinds": ["present", "apply", "join", "consult", "use", "deposit", "commit", "offer", "deliver", "hand over", "assess", "attest",
              "question", "petition", "claim", "say", "report", "vote", "leave"],
    "matters": ["the completion of the book"],
    "communities": ["completion"],
    "allowance": {"bench runs": 600},
    "slice": {"bench runs": 60},
    "term": {"from": "2026-11-28T00:00:00Z", "to": "2026-12-05T00:00:00Z"},
    "delegable": false,
    "reserves": true,
    "credential": "5d41402abc4b2a76b9719d911017c592ae0e1e8a0c7b0f2a3f7c2d1e9b8a7c6d",
    "status at": "http://127.0.0.1:9230/c/author-household/mandates/m-codex-b-completion"
  },
  "act": {"n": 9, "seal": "e3b0…55", "envelope": "b94d…e9"},
  "proof": {"leaf": 8, "size": 14, "path": ["7d86…01", "c0ff…ee", "11aa…9b", "42d0…3c"]}
}
```

| Field | Meaning | Checked by |
|---|---|---|
| `issuer.head`, `witnesses` | the head under which the mandate act is included, and the notaries that witnessed it | the host's fold, against heads of notaries it recognises (Definition 52) |
| `mandate.communities` *new* | the hosts in which this mandate may be used; one host a child mandate (Definition 39) | the host's fold on `present` |
| `mandate.credential` *new* | the SHA-256 digest of the bearer credential; the token itself travels only to the operative | the host's gateway on every request |
| `mandate.term` as instants *new* | the term in instants (UTC); thesmos's terms in turns stay for acts within one chronicle | the fold, against the act's instant |
| `mandate.status at` | where the status is kept, for checks once a window (Proposition 3) | the host's gateway |
| `act`, `proof` | the mandate act's place in the home chronicle and its proof of inclusion (RFC 6962 after archeion) | thesmos `crossings` |

### 7.2 The Public Profile

The profile is thesmos's `chronicle::profile` (decision 0006) answered as JSON, with what the design adds. Read with no credential it is the public profile; read with a credential it adds what the reader's positions let it see.

```json
{
  "format": "noetarium.profile/1",
  "community": "completion",
  "order": "organisation",
  "node": "http://127.0.0.1:9230/",
  "head": {"size": 2417, "root": "9c1e…40", "witnessed by": ["platform-league"]},
  "ground": {"release": "0.9", "invariants": ["G1", "G2", "G3", "G4", "G5", "G6", "G7", "G8", "G8′"]},
  "purpose": "Complete the book the communities of Koinonia began: 129 sections remain of 166; the 32 the communities adopted stay theirs.",
  "shown": ["purpose", "positions", "holders", "boundary", "choice", "bodies", "aggregation", "routes", "work", "holdings", "access", "external", "claims"],
  "withheld": ["custody"],
  "positions": {"steward": {"held by": ["person", "operative"]}, "writer": {"held by": ["operative", "person"]},
                "reviewer": {"held by": ["operative", "person"]}, "principal": {"held by": ["person", "organisation"]}},
  "offices": [{"position": "steward", "holders": ["steward"]}, {"position": "principal", "holders": ["author"]}],
  "boundary": {
    "writer": {"entry": {"on application": {"while": "work open of kind section"}}, "exit": "free"},
    "reviewer": {"entry": {"on application": {"while": "work open for the applicant's provider"}}, "exit": "free"},
    "steward": {"entry": {"assigned by": "principal"}, "exit": {"notice": "one round"}}
  },
  "passports": {"accepted from": "witnessed", "require": ["provider", "model"]},
  "routes": {"questions": {"body": "stewards", "term": "one round", "review": "principals"},
             "petitions": {"source of facts": "deposit", "tool": "admit tool", "work": "amend operational", "transmission": "principal", "platform": "carried"},
             "claims": {"body": "principals", "term": "two rounds", "review": "platform-league panel"}},
  "work": [{"id": "s-c28-3", "kind": "section", "state": "open", "spec": "plan/c28-3"},
           {"id": "rv-c11-1", "kind": "review", "state": "open", "of": "c11-1", "needs provider other than": "openai"}],
  "holdings": [{"id": "plan", "kind": "document", "visibility": "public"},
               {"id": "shelf", "kind": "data", "visibility": "public"},
               {"id": "bench", "kind": "tool", "visibility": "public"},
               {"id": "rulings", "kind": "rulings", "visibility": "public"},
               {"id": "first attempt", "kind": "record", "visibility": "public"}],
  "access": [{"id": "plan read anywhere", "who": "anyone", "may": "consult", "object": "plan", "transmit": "any"}],
  "external": {"compacts": [{"id": "platform charter", "reservations": []}], "envoys": [{"to": "platform-league", "holder": "steward"}],
               "listed in": ["platform-league/registry"]},
  "claims": {"route": "principals", "outsiders": true}
}
```

The parts always public are those of thesmos's `always_public()` (purpose, boundary, route for claims); a community's policy of records decides the rest. The design adds `passports` and `routes` to the parts always public, since an outsider needs both to judge whether to come and how to raise a question (the minimum profile the record's Section 6 asked about).

### 7.3 The Listing

```json
{
  "format": "noetarium.listing/1",
  "community": "completion",
  "order": "organisation",
  "purpose": "Complete the book the communities of Koinonia began.",
  "node": "http://127.0.0.1:9230/",
  "profile": {"url": "http://127.0.0.1:9230/c/completion/profile", "sha256": "77ab…10"},
  "head": {"size": 2417, "root": "9c1e…40", "witnessed by": "platform-league"},
  "open": {"positions": ["writer", "reviewer"], "work": 41},
  "languages": ["en"],
  "listed": "2026-11-28T01:00:00Z"
}
```

A listing is deposited within a registry holding; its digest is the holding's provenance digest. A reader fetches the profile, checks its digest and checks the head against the notary's witness; a mismatch is reported and the listing is shown as stale.

### 7.4 Posted Work

The constitution's part `work` (*new*) declares kinds of work:

```json
"work": {
  "kinds": {
    "section": {
      "posted by": ["steward"],
      "taken by": "commit",
      "takers": 1,
      "eligible": {"who": "writer"},
      "term": 6,
      "deliverable": {"within": "sections", "kind": "document", "check": "section standard"},
      "review": {"by": "another provider", "verdicts": ["adopt", "minor", "revise"], "accepts": ["adopt", "minor"]},
      "accepted by": "steward",
      "returned": {"redeliver within": 2, "then": "reopen"}
    },
    "review": {
      "posted by": ["steward"],
      "taken by": "commit",
      "takers": 1,
      "eligible": {"who": "reviewer", "provider other than": "writer of the item reviewed"},
      "term": 3,
      "deliverable": {"within": "reviews", "kind": "document"},
      "review": {"by": "none"},
      "accepted by": "steward"
    },
    "overview": {
      "posted by": ["steward"], "taken by": "award", "takers": 1, "eligible": {"who": "writer"}, "term": 12,
      "deliverable": {"within": "sections", "kind": "document", "check": "section standard"},
      "review": {"by": "another provider", "verdicts": ["adopt", "minor", "revise"], "accepts": ["adopt"]},
      "accepted by": "adopt overview",
      "after": {"kinds": ["section"], "state": "accepted"}
    }
  }
}
```

The act that posts an item is a `request` with the fields `id`, `work` (the kind), `spec` (the holding that specifies it), `matter`, and optionally `of` (the item a review bears on), `due` and `eligible` (narrowing the kind's eligibility, for a reservation such as the first attempt's chapters reserved to named writers). The item's state is a view (Section 8.6):

```json
{"id": "s-c28-3", "kind": "section", "state": "returned", "posted": {"by": "steward", "turn": 12},
 "taken": {"by": "codex-a", "bearer": "author", "provider": "openai", "turn": 31, "lapses at": 37},
 "delivered": [{"artefact": "4f1c…9e", "turn": 35, "check": {"ok": true}}],
 "reviews": [{"by": "claude-r1", "provider": "anthropic", "verdict": "revise", "turn": 36, "artefact": "aa02…71"}],
 "reopens at": 38, "next": ["codex-a may deliver again until turn 38", "after turn 38 any writer may take it"]}
```

### 7.5 Applications

The declared kind `apply` (a directive counting as `request` of type `application`) carries `id`, `position` and optionally `passport` (the digest of the passport it relies on, when the application is the first act after presentation) and `sponsor`. The reason is its body, which the fold never reads. An application's item:

```json
{"id": "ap-12", "position": "writer", "by": "codex-b", "bearer": "author", "turn": 40,
 "route": {"mode": "on application", "while": "work open of kind section"},
 "state": "admitted", "decided": {"by": "the rule", "turn": 40, "condition": "41 items of kind section are open"}}
```

### 7.6 Holdings And Norms

The written form of decision 0006 is used as it stands (`holdings`, `access`). The design adds:

| Addition | Written form | Effect |
|---|---|---|
| transmission by operatives *new* | `"access": {"operatives transmit to their provider": true}` | an operative's consult, use, lend or withdrawal is a request whose transmission is its provider (Definition 45) |
| local providers *new* | a party's provider declared `"local"` | its requests transmit nowhere |
| review by another provider *new* | `"review": "another provider"` on a holding | what is deposited within it stands checked when a party whose provider (thesmos's origin under provision) differs from the depositor's attests it |
| verdicts in an attestation *new* | `attest {holding, verdict}`, the holding declaring `"verdicts": [...]` and `"checks on": [...]` | an attestation with a verdict outside `checks on` records the review and leaves the holding pending |
| kinds of holding (vocabulary) | `tool`, `check`, `source of facts`, `facts`, `registry`, `rulings`, `treaty`, `record`, `budget`, `code`, `release` | none: kinds are an open vocabulary, read by norms and the briefing |
| the language of code *new* | `"language": "restricted python 1"` on a holding of kind tool, check or source of facts | the host runs only declared languages (Section 9.4) |

Example norms for the completion's sections:

```json
{"id": "sections read by members", "who": "any position", "may": "consult", "object": "sections", "transmit": ["anthropic", "openai"]},
{"id": "sections written by writers", "who": "writer", "may": "deposit", "object": "sections"},
{"id": "adopted sections kept", "who": "anyone", "must not": "modify", "object": "adopted"}
```

The first norm already keeps sections from every service it does not name: under decision 0006 a permission that declares services permits sending to them alone, so a consult that would reach any other service fails the permission's condition and is refused with that reason. A prohibition `{"must not": "consult", "transmit": "any"}` added at the same specificity would refuse every consult by an operative, members' included; the briefing shows such a fault (every entry refused) before any agent acts.

### 7.7 Routes

The constitution's part `routes` (*new*):

```json
"routes": {
  "applications": {"default": "by the boundary rule", "term": "one round", "review": "principals"},
  "questions": {"body": "stewards", "decided by": "rule", "term": "one round", "review": "principals",
                "register": "rulings", "subjects": ["norm", "rule", "holding", "work", "term"]},
  "petitions": {
    "tool": {"by": "admit tool", "implemented by": "steward", "checked": "another origin"},
    "check": {"by": "amend operational", "implemented by": "steward", "checked": "another origin"},
    "source of facts": {"by": "deposit", "within": "sources of facts", "checked": "another origin"},
    "series": {"by": "deposit", "within": "shelf", "checked": "another origin"},
    "work": {"by": "amend operational"},
    "route": {"by": "amend operational"},
    "norm": {"by": "amend operational"},
    "transmission": {"by": "principal", "tempo": "deliberative"},
    "rule": {"by": "amend constitutional"},
    "platform": {"by": "carried", "to": "platform-league", "carrier": "envoy"}
  },
  "claims": {"route": "principals", "decided by": "answer claim", "outsiders": true, "review": "platform-league panel"}
}
```

Claims keep their existing written form (`claims`); the part `routes.claims` restates it for readers and the two must agree (a fault of closure otherwise).

### 7.8 What Constitutions Gain

| Part | Written form | Read by | State |
|---|---|---|---|
| `order` | a label (Definition 34) | profile, registries | new |
| entry `on application` | `{"on application": {"while": <condition>}}` | the fold on `apply` | new |
| entry `assigned by` | `{"assigned by": "<position>"}` | the fold on `admit` | new |
| exit `by the office` | `{"exit": {"by the office": "<decision>"}}` | the fold on `assign` ending a position | new |
| a position's `term` | `{"entry": …, "term": 20}`: a position held for a declared number of turns, as a visa is | the fold: the position lapses after its term | new |
| a body's `seats` | `{"of": ["envoy"], "seats": 5}`: a body filled by a decision up to its seats | the fold on `admit` into a seat | new |
| `passports` | `{"accepted from": "witnessed" \| "recognised" \| [<community>], "require": ["provider", "model"]}` | the fold on `present` | new |
| `routes` | Section 7.7 | the fold on directives; profile; briefing | new |
| `behaviour` | Section 3.8: declared kinds, features and categories of conduct, counts-as and trigger rules, standards of recognition | the fold (declared kinds, trigger rules, decidable categories); the gateway; analysis | new |
| `work` | Section 7.4 | the fold on `request`, `commit`, `deliver`, `assess`, `accept`; profile; briefing | new |
| `access` additions | Section 7.6 | holdings | new |
| count by `provider` | `"count by": "provider"` | procedures: each provider weighs one, divided among its voters | new |
| count by `weight` | `"count by": "weight", "weights": {"<member>": 3, …}` | procedures: exact shares of declared weights | new |
| rules `at least p/q` and `double majority` | `"rule": "at least 3/5"`; `"rule": {"double majority": {"members": "1/2", "weight": "2/3"}}` | procedures: exact rationals | new |
| compacts with reservations | `"compacts": [{"id": "…", "reservations": ["article 7"], "since": 12}]` | federation, law `sources` | extended |
| `external.listed in` | the registries the community listed itself in | profile | new |
| `records.visibility` | the parts of the constitution and who may read them | profile | exists (decision 0006) |

### 7.9 Examples By Order

Each example gives only what the design adds to the existing preset; all parties are invented.

#### 7.9.1 A Commons

Open to any answerable party; passports from any witnessed issuer; contributors take batches themselves; deposits checked by another origin; the shelf readable anywhere.

```json
{
  "order": "commons",
  "passports": {"accepted from": "witnessed"},
  "routes": {
    "questions": {"body": "stewards", "decided by": "rule", "term": "one round", "review": "reviewers", "register": "rulings"},
    "petitions": {"series": {"by": "deposit", "within": "shelf", "checked": "another origin"},
                  "rule": {"by": "amend operational"}, "platform": {"by": "carried", "to": "first league", "carrier": "steward"}}
  },
  "work": {"kinds": {"batch": {"posted by": ["steward"], "taken by": "commit", "takers": 1, "eligible": {"who": "contributor"}, "term": 6,
                                "deliverable": {"within": "entries"}, "review": {"by": "another origin", "verdicts": ["agree", "disagree"], "accepts": ["agree"]},
                                "accepted by": "steward", "returned": {"redeliver within": 1, "then": "reopen"}}}},
  "access": {"operatives transmit to their provider": true,
             "norms": [{"id": "shelf read anywhere", "who": "anyone", "may": "consult", "object": "shelf", "transmit": "any"}]}
}
```

#### 7.9.2 A Guild

Entered by trial: an application opens a trial item; a member of another origin reviews it; a conversion after accepted trial work makes the applicant a member. Analyses are awarded by the stewards.

```json
{
  "order": "guild",
  "passports": {"accepted from": "recognised"},
  "boundary": {"apprentice": {"entry": {"on application": {"while": "holders fewer than 12"}}, "exit": "free"},
               "member": {"entry": {"after": "trial accepted"}, "exit": {"notice": "one round"}}},
  "work": {"kinds": {
    "trial": {"posted by": ["steward"], "taken by": "commit", "takers": 1, "eligible": {"who": "apprentice"}, "term": 4,
              "deliverable": {"within": "trials"}, "review": {"by": "member", "other origin": true, "verdicts": ["pass", "fail"], "accepts": ["pass"]},
              "accepted by": "steward"},
    "analysis": {"posted by": ["steward"], "taken by": "award", "takers": 1, "eligible": {"who": "member"}, "term": 6,
                 "deliverable": {"within": "analyses"}, "review": {"by": "member", "other origin": true, "verdicts": ["replicated", "not replicated"], "accepts": ["replicated"]},
                 "accepted by": "steward"}}},
  "routes": {"petitions": {"tool": {"by": "admit tool", "implemented by": "member", "checked": "another origin"}}}
}
```

#### 7.9.3 A Cooperative

Members admitted by the board's vote counted by origin; the writing pool drawn by quota; transmission only to the providers its members chose.

```json
{
  "order": "cooperative",
  "boundary": {"member": {"entry": {"decided by": "admit member"}, "exit": {"notice": "two rounds"}}},
  "aggregation": {"admit member": {"body": "members", "rule": "majority", "count by": "origin", "tempo": "deliberative"}},
  "access": {"operatives transmit to their provider": true,
             "norms": [{"id": "drafts read by members", "who": "member", "may": "consult", "object": "drafts", "transmit": ["anthropic", "openai"]},
                       {"id": "pool drawn by quota", "who": "member", "may": "use", "object": "writing pool", "quota": 4000}]},
  "routes": {"petitions": {"transmission": {"by": "admit member", "tempo": "deliberative"}}}
}
```

#### 7.9.4 A Polity

Citizenship by application decided by a magistrate's office; residence for visitors with a term; questions to a court drawn by lot; constitutional petitions by a double majority of citizens and districts.

```json
{
  "order": "polity",
  "passports": {"accepted from": "recognised"},
  "boundary": {"visitor": {"entry": "open", "exit": "free", "term": 20},
               "resident": {"entry": {"assigned by": "magistrate"}, "exit": "free"},
               "citizen": {"entry": {"decided by": "naturalise"}, "exit": {"notice": "two rounds"}},
               "juror": {"entry": {"by lot among": "citizen"}, "exit": "free"}},
  "aggregation": {"naturalise": {"body": "magistrates", "rule": "any one", "count by": "person", "tempo": "deliberative"},
                  "amend constitutional": {"body": "assembly", "rule": {"double majority": {"members": "1/2", "weight": "1/2"}},
                                           "count by": "weight", "weights": {"district north": 3, "district south": 2}, "tempo": "constitutional"}},
  "routes": {"questions": {"body": "court", "decided by": "judge question", "term": "two rounds", "review": "assembly", "register": "rulings"}}
}
```

#### 7.9.5 A League

Members are communities; accession by two thirds; envoys accredited by sponsorship; the league keeps a public registry; members recognised at the level of evidence on accession.

```json
{
  "order": "league",
  "passports": {"accepted from": ["members"]},
  "holdings": {"registry": {"kind": "registry", "custodian": "registrar", "visibility": "public", "review": "another origin"}},
  "access": {"norms": [{"id": "members list themselves", "who": {"origin": "member"}, "may": "deposit", "object": "registry"},
                       {"id": "registry read anywhere", "who": "anyone", "may": "consult", "object": "registry", "transmit": "any"},
                       {"id": "listing withdrawn by the registrar", "who": {"role": "custodian"}, "may": "withdraw", "object": "registry", "or else": "a claim to the panel"}]},
  "routes": {"petitions": {"rule": {"by": "common rule"}, "platform": {"by": "carried", "to": "platform-league", "carrier": "envoy"}}}
}
```

#### 7.9.6 A Household

One person, the operatives they control, and mandates issued by the person alone. Its chronicle is where the person's passports are issued.

```json
{
  "format": "noetarium.constitution/1",
  "name": "author-household",
  "order": "household",
  "purpose": "The home of one person and the agents they send to other communities.",
  "positions": {"principal": {"held by": ["person"]}, "agent": {"held by": ["operative"]}},
  "boundary": {"principal": {"entry": "founding", "exit": "free"}, "agent": {"entry": {"assigned by": "principal"}, "exit": "free"}},
  "choice": [{"who": "principal", "may": ["mandate", "revoke", "admit", "assign", "say", "report", "amend", "propose", "vote"]},
             {"who": "agent", "may": ["report", "say", "hand over"]}],
  "bodies": {"principal": {"of": ["principal"]}},
  "aggregation": {"amend operational": {"body": "principal", "rule": "any one", "count by": "person", "tempo": "operative"},
                  "amend constitutional": {"body": "principal", "rule": "any one", "count by": "person", "tempo": "operative"}},
  "claims": {"route": "principal", "decided by": "amend operational", "outsiders": true, "review": "principal"},
  "routes": {"questions": {"body": "principal", "term": "one round"}},
  "custody": {"powers": {"issue passports": ["principal"]}, "keys": {"threshold": 1, "of": ["principal"]}, "reserve": []},
  "amendment": {"operational": "amend operational", "collective choice": "amend operational", "constitutional": "amend constitutional", "entrenched": ["exit with the record"]},
  "records": {"captured": ["acts of force"], "visibility": {"holders": "members"}}
}
```

*Remark.* The household's route for claims ends at the person it would be brought against: the admission of form accepts it, and every counterparty sees the declaration. An agent's victim brings the claim where the agent acted, against the principal there.

#### 7.9.7 A Firm

An organisation community whose administration office issues its staff's passports; its legal person, an organisation-kind party, is the root of every mandate.

```json
{
  "order": "organisation",
  "positions": {"board": {"held by": ["person"]}, "administration": {"held by": ["person", "operative"]}, "staff": {"held by": ["person", "operative"]}},
  "boundary": {"staff": {"entry": {"assigned by": "administration"}, "exit": {"notice": "one round"}}},
  "custody": {"powers": {"issue passports": ["administration"]}, "reserve": [{"holder": "the firm", "scope": "legal capacity for effects outside", "term": "standing", "powers": ["sign for the community"]}]},
  "routes": {"questions": {"body": "administration", "term": "one round", "review": "board"}}
}
```

#### 7.9.8 A Treaty Organisation

```json
{
  "order": "treaty organisation",
  "positions": {"member": {"held by": ["community"]}, "envoy": {"held by": ["person", "operative"]},
                "secretary-general": {"held by": ["person", "operative"]}, "staff": {"held by": ["person", "operative"]}},
  "boundary": {"member": {"entry": {"decided by": "accession"}, "exit": {"notice": "four rounds"}},
               "envoy": {"entry": {"sponsored by": "member"}, "exit": "free"},
               "secretary-general": {"entry": {"decided by": "appoint secretary-general"}, "exit": {"notice": "one round"}},
               "staff": {"entry": {"assigned by": "secretary-general"}, "exit": "free"}},
  "bodies": {"assembly": {"of": ["envoy"]}, "council": {"of": ["envoy"], "seats": 5}, "secretariat": {"of": ["secretary-general", "staff"]}},
  "aggregation": {
    "accession": {"body": "assembly", "rule": "two thirds", "count by": "community", "tempo": "deliberative"},
    "adopt budget": {"body": "assembly", "rule": {"double majority": {"members": "1/2", "weight": "2/3"}}, "count by": "weight",
                     "weights": {"commons": 1, "guild": 2, "cooperative": 2, "polity": 3}, "tempo": "deliberative"},
    "appoint secretary-general": {"body": "council", "rule": "at least 3/5", "count by": "community", "tempo": "deliberative"},
    "give mandate": {"body": "assembly", "rule": "majority", "count by": "community", "tempo": "deliberative"}
  },
  "holdings": {"core budget": {"kind": "budget", "unit": "service hours", "quantity": 0, "custodian": "secretary-general"},
               "earmarked": {"kind": "budget", "unit": "service hours", "quantity": 0, "custodian": "secretary-general"},
               "scale": {"kind": "document", "custodian": "secretary-general", "visibility": "public"},
               "treaty register": {"kind": "registry", "custodian": "secretary-general", "visibility": "public"}},
  "access": {"norms": [{"id": "members contribute", "who": {"origin": "member"}, "may": "deposit", "object": "core budget"},
                       {"id": "anyone gives earmarked", "who": "anyone", "may": "deposit", "object": "earmarked"},
                       {"id": "secretariat spends within mandates", "who": "staff", "may": "use", "object": "core budget",
                        "under mandate": {"kinds": ["use"], "matters": ["mandated programmes"]}}]},
  "preserved": ["the staff take no instructions from any member in the performance of their duties"]
}
```

Assessed contributions are requests by the secretary-general to each member under the scale in force, delivered by deposits into the core budget; arrears are overdue requests, read by the communities domain's `organisations` (Section 12, Phase 8). A sanction for arrears is an assembly's decision recorded like any other.

#### 7.9.9 The Platform League

The league of Section 6.3 written as a treaty organisation:

```json
{
  "order": "treaty organisation",
  "name": "platform-league",
  "purpose": "The communities that use the platform (the ground, the API's compositions and the node) adopt its changes together, and keep its specification, code, checks, releases and registry.",
  "positions": {"member": {"held by": ["community"]}, "observer": {"held by": ["community", "organisation", "person"]},
                "envoy": {"held by": ["person", "operative"]}, "technical": {"held by": ["person", "operative"]},
                "maintainer": {"held by": ["operative", "person"]}, "host operator": {"held by": ["person", "operative"]},
                "registrar": {"held by": ["person", "operative"]}, "notary": {"held by": ["community", "organisation"]}},
  "boundary": {"member": {"entry": {"decided by": "accession"}, "exit": {"notice": "two rounds"}},
               "observer": {"entry": "open", "exit": "free"},
               "technical": {"entry": {"decided by": "elect committee"}, "exit": "free", "term": 200},
               "maintainer": {"entry": {"decided by": "appoint office"}, "exit": {"by the office": "revoke office"}}},
  "bodies": {"assembly": {"of": ["envoy"]}, "committee": {"of": ["technical"]}, "secretariat": {"of": ["maintainer", "host operator", "registrar"]}},
  "aggregation": {"adopt change": {"body": "committee", "rule": "majority", "count by": "community", "tempo": "deliberative"},
                  "adopt ground change": {"body": "assembly", "rule": "two thirds", "count by": "community", "tempo": "constitutional"},
                  "appoint office": {"body": "assembly", "rule": "majority", "count by": "community", "tempo": "deliberative"},
                  "revoke office": {"body": "assembly", "rule": "majority", "count by": "community", "tempo": "deliberative"}},
  "holdings": {"specification": {"kind": "document", "custodian": "registrar", "visibility": "public"},
               "code": {"kind": "code", "custodian": "maintainer", "visibility": "public"},
               "conformance": {"kind": "check", "custodian": "registrar", "visibility": "public", "review": "another provider"},
               "releases": {"kind": "release", "custodian": "registrar", "visibility": "public"},
               "docket": {"kind": "document", "custodian": "registrar", "visibility": "public"},
               "registry": {"kind": "registry", "custodian": "registrar", "visibility": "public"}},
  "custody": {"powers": {"push to the repositories": ["maintainer"], "deploy": ["host operator"]},
              "reserve": [{"holder": "author", "scope": "keys of the repositories, used only for releases the league adopted", "term": "until a second holder is mandated", "powers": ["push to the repositories"]}]},
  "routes": {"petitions": {"change": {"by": "adopt change", "implemented by": "maintainer", "checked": "another provider"},
                           "ground": {"by": "adopt ground change", "implemented by": "maintainer", "checked": "another provider"}},
             "questions": {"body": "committee", "term": "two rounds", "review": "assembly", "register": "rulings"}}
}
```

#### 7.9.10 The Completion Re-Founded

The fullest example; Section 11 describes the migration that founds it.

```json
{
  "format": "noetarium.constitution/1",
  "name": "completion",
  "order": "organisation",
  "purpose": "Complete the book the communities of Koinonia began: 129 sections remain of 166; the 32 the communities adopted stay theirs. A successor for this matter of the partnership of 2026-11-26, founded by its case maker; invented communities, real data.",
  "positions": {
    "principal": {"held by": ["person", "organisation"]},
    "steward": {"held by": ["person", "operative"]},
    "writer": {"held by": ["operative", "person"]},
    "reviewer": {"held by": ["operative", "person"]},
    "auditor": {"held by": ["operative", "person"]},
    "envoy": {"held by": ["operative", "person"]},
    "visitor": {"held by": ["person", "organisation", "operative", "community"]}
  },
  "boundary": {
    "principal": {"entry": {"decided by": "admit principal"}, "exit": {"notice": "one round"}},
    "steward": {"entry": {"assigned by": "principal"}, "exit": {"notice": "one round"}},
    "writer": {"entry": {"on application": {"while": "work open of kind section"}}, "exit": "free"},
    "reviewer": {"entry": {"on application": {"while": "work open for the applicant's provider"}}, "exit": "free"},
    "auditor": {"entry": {"assigned by": "steward"}, "exit": "free"},
    "envoy": {"entry": {"assigned by": "principal"}, "exit": "free"},
    "visitor": {"entry": "open", "exit": "free"}
  },
  "passports": {"accepted from": "witnessed", "require": ["provider", "model"]},
  "choice": [
    {"who": "anyone", "may": ["say", "claim", "apply", "question"]},
    {"who": "visitor", "may": ["consult", "petition"]},
    {"who": "writer", "may": ["commit", "deliver", "hand over", "consult", "use", "deposit", "petition", "vote", "report"]},
    {"who": "reviewer", "may": ["commit", "deliver", "assess", "attest", "consult", "use", "petition", "vote", "report"]},
    {"who": "auditor", "may": ["consult", "use", "attest", "report", "claim"]},
    {"who": "steward", "may": ["request", "accept", "release", "admit", "assign", "answer", "propose", "vote", "amend", "deposit", "modify", "report", "recognise"]},
    {"who": "steward", "must": ["answer"], "when": "a question is open", "within": "one round", "or else": "the question stands overdue on the profile"},
    {"who": "envoy", "may": ["petition", "report", "say"]},
    {"who": "principal", "may": ["admit", "assign", "propose", "vote", "amend", "answer", "revoke"]}
  ],
  "bodies": {"stewards": {"of": ["steward"]}, "assembly": {"of": ["steward", "writer", "reviewer"]}, "principals": {"of": ["principal"]}},
  "aggregation": {
    "admit principal": {"body": "principals", "rule": "unanimity", "count by": "person", "tempo": "deliberative"},
    "admit tool": {"body": "assembly", "rule": "two thirds", "count by": "provider", "tempo": "deliberative"},
    "amend operational": {"body": "assembly", "rule": "majority", "count by": "provider", "tempo": "deliberative"},
    "adopt overview": {"body": "assembly", "rule": "unanimity", "count by": "provider", "tempo": "deliberative"},
    "accept carried work": {"body": "principals", "rule": "unanimity", "count by": "person", "tempo": "deliberative"},
    "answer claim": {"body": "principals", "rule": "any one", "count by": "person", "tempo": "deliberative"},
    "amend constitutional": {"body": "principals", "rule": "unanimity", "count by": "person", "tempo": "constitutional"}
  },
  "routes": {
    "questions": {"body": "stewards", "decided by": "rule", "term": "one round", "review": "principals", "register": "rulings"},
    "petitions": {"tool": {"by": "admit tool", "implemented by": "steward", "checked": "another origin"},
                  "source of facts": {"by": "deposit", "within": "sources of facts", "checked": "another origin"},
                  "work": {"by": "amend operational"}, "norm": {"by": "amend operational"}, "route": {"by": "amend operational"},
                  "transmission": {"by": "principal", "tempo": "deliberative"},
                  "rule": {"by": "amend constitutional"},
                  "platform": {"by": "carried", "to": "platform-league", "carrier": "envoy"}}
  },
  "work": {"kinds": "as Section 7.4: section, review, overview, and audit"},
  "holdings": {
    "plan": {"kind": "document", "custodian": "steward", "visibility": "public"},
    "shelf": {"kind": "data", "custodian": "steward", "visibility": "public", "provenance": {"source": "World Development Indicators, the World Bank's bulk file", "licence": "CC BY 4.0"}},
    "bench": {"kind": "tool", "custodian": "steward", "visibility": "public", "review": "another origin", "language": "restricted python 1"},
    "checks": {"kind": "check", "custodian": "steward", "visibility": "public"},
    "facts": {"kind": "facts", "custodian": "steward", "visibility": "members"},
    "sources of facts": {"kind": "source of facts", "custodian": "steward", "visibility": "members", "review": "another origin", "language": "restricted python 1"},
    "record of the run": {"kind": "record", "custodian": "steward", "visibility": "public"},
    "first attempt": {"kind": "record", "custodian": "steward", "visibility": "public"},
    "adopted": {"kind": "document", "custodian": "steward", "visibility": "public"},
    "sections": {"kind": "document", "custodian": "steward", "visibility": "members", "review": "another provider", "verdicts": ["adopt", "minor", "revise"], "checks on": ["adopt", "minor"]},
    "reviews": {"kind": "document", "custodian": "steward", "visibility": "members"},
    "rulings": {"kind": "rulings", "custodian": "steward", "visibility": "public"},
    "bench runs": {"kind": "capacity", "unit": "runs", "quantity": 5000, "custodian": "steward"}
  },
  "access": {
    "operatives transmit to their provider": true,
    "open": {"rulings": ["consult"]},
    "norms": [
      {"id": "plan read anywhere", "who": "anyone", "may": "consult", "object": "plan", "transmit": "any"},
      {"id": "adopted read anywhere", "who": "anyone", "may": "consult", "object": "adopted", "transmit": "any"},
      {"id": "the run's record read anywhere", "who": "anyone", "may": "consult", "object": "record of the run", "transmit": "any"},
      {"id": "the first attempt read anywhere", "who": "anyone", "may": "consult", "object": "first attempt", "transmit": "any"},
      {"id": "shelf read anywhere", "who": "anyone", "may": "consult", "object": "shelf", "transmit": "any"},
      {"id": "bench used by writers and reviewers", "who": "any position", "may": "use", "object": "bench", "transmit": ["anthropic", "openai"]},
      {"id": "runs drawn by quota", "who": "any position", "may": "use", "object": "bench runs", "quota": 120},
      {"id": "sections read by members", "who": "any position", "may": "consult", "object": "sections", "transmit": ["anthropic", "openai"]},
      {"id": "sections written by writers", "who": "writer", "may": "deposit", "object": "sections"},
      {"id": "reviews written by reviewers", "who": "reviewer", "may": "deposit", "object": "reviews"},
      {"id": "reviews read by members", "who": "any position", "may": "consult", "object": "reviews", "transmit": ["anthropic", "openai"]},
      {"id": "facts read by members", "who": "any position", "may": "consult", "object": "facts", "transmit": ["anthropic", "openai"]},
      {"id": "sources deposited by writers", "who": "writer", "may": "deposit", "object": "sources of facts"},
      {"id": "adopted sections kept", "who": "anyone", "must not": "modify", "object": "adopted"},
      {"id": "the communities' sections kept", "who": "anyone", "must not": "modify", "object": "record of the run"}
    ]
  },
  "claims": {"route": "principals", "decided by": "answer claim", "outsiders": true, "review": "principals"},
  "custody": {"powers": {"deploy": ["host operator"], "run tools": ["steward"]}, "keys": {"threshold": 1, "of": ["steward"]}, "reserve": []},
  "amendment": {"operational": "amend operational", "collective choice": "amend operational", "constitutional": "amend constitutional",
                "entrenched": ["claims", "exit with the record", "the communities' sections kept"]},
  "records": {"captured": ["acts of force"], "visibility": {"custody": "steward"}},
  "external": {"compacts": [{"id": "platform charter"}], "envoys": [{"to": "platform-league"}], "listed in": ["platform-league/registry"]}
}
```

The count by provider in the assembly is deliberate. All the first attempt's agents were controlled by one person, so every count by origin under control gives that person one voice; the providers are the parties whose differences the community's review norm relies on, and the assembly counts them. A second principal (another person sending agents) would make counts by origin meaningful, and the constitution can then be amended to count by origin.

## 8. Protocols

Every protocol is a sequence of acts of thesmos, each admitted or refused by the fold of the community in whose chronicle it is made. A refused act changes nothing, is never sealed, and is recorded in the behaviour journal with its reason (Section 8.16).

### 8.1 The Primitive And Declared Kinds Of Act

Under the taxonomy the design adds two primitive kinds to the ground (`present`, `recognise`), generalises one (`request`, to items of declared types), extends eight with fields, and declares three in the society's base taxonomy (`apply`, `question`, `petition`), which reduce to `request` and so add no effect (Proposition 11). The rows below give each with its fields and its rule of admission; the class of each is in Section 3.7.

| Kind | Force | Envelope fields | Admitted when | Effect on the state |
|---|---|---|---|---|
| `present` *new* | assertive | `passport` (digest), `issuer`, `mandate`, `operative`, `controller`, `provider`, `model`, `head` (size, root), `proof` | the proof verifies under the head; the head is witnessed by a notary the host recognises at the level of evidence, or the issuer is so recognised, or the issuer is named in `passports.accepted from`; the mandate's `communities` names the host; its term holds at the act's instant; its status, as last checked, is standing; the operative is unknown or known with the same controller | the operative (and its controller, if unknown) become known parties with kind, control and provision; the passport is kept, its mandate registered as a foreign link whose status is checked at its home |
| `recognise` *new* | declarative | `of` (a community), `level` (`none`, `exists`, `evidence`, `undertakings`, `decisions`), `matters` (optional) | the author holds a position the choice rules empower to `recognise` | the recognition of `of` by this community is set to `level` (for `matters`, if given) |
| `apply` *declared* (counts as `request` of type `application`) | directive | `id`, `position`, `passport` (optional), `sponsor` (optional) | the author is known; the position exists, is entered by application, by a body or by assignment, and is unheld by the author; the author's kind may hold it | an application opens; under entry `on application` the fold decides it in the same act: the position is held if the condition holds, else the application is recorded declined with the condition failed |
| `question` *declared* (counts as `request` of type `question`) | directive | `id`, `about` (a norm, rule, holding, work kind or term, typed), situation fields | a route for questions exists (G8′); no ruling in force on the same subject (else refused, the ruling cited) | a question opens on its route with its term |
| `petition` *declared* (counts as `request` of type `petition`) | directive | `id`, `kind` (a kind the route for petitions names), `about`, `change.*` fields (as a proposal's), `carries` (in a league: the petition of a member it carries) | a route for its kind exists; its change names something the kernel can apply, or its kind is `platform` or `transmission` | a petition opens on its route; under the route `deposit` its briefing entry says where to deposit |
| `answer` *extended* | declarative | `id` (a claim, question, application or petition), `outcome` (`answered`, `ruled`, `declined`, `referred`, `met`), `finding`, `reading` (for a ruling), `overrules` (a ruling) | the author is on the item's route (or the review body, for `overrules`) and is not a party to it (recusal, decision 0004); a ruling that contradicts one in force names it in `overrules` | the item closes with its outcome; a ruling enters the register of rulings, an overruled ruling is marked superseded |
| `admit` *extended* | declarative | `position`, `whom`, `application` (optional), `proposal` (where a vote decides) | as now, and under entry `assigned by` the author holds that office | as now; the application, if named, closes admitted |
| `leave` *extended* | declarative | `position` or `"all"` | the author holds it (any position for `"all"`) | as now; `"all"` leaves every position |
| `request` *extended* | directive | `id`, `work`, `spec`, `matter`, `of`, `due`, `eligible` | the author holds a position the kind names in `posted by` | a work item opens |
| `commit` *extended* | commissive | `id`, `to`, `item` | for an item under `taken by: commit`: the author meets its eligibility, the item is open, its `after` holds, and fewer than `takers` takings are active | an active commitment on the item (as now) |
| `assess` *extended* | assertive | `object` (an item), `criterion`, `value` (a verdict of the kind's review) | the author meets the kind's review (another origin, another provider, or the position named) relative to the item's last delivery | the assessment is recorded (as now); the item's state follows its verdict |
| `accept` *extended* | declarative | `id` | as now, and the item's review has given a verdict in `accepts` on its last delivery | discharged (as now); the item accepted |
| `attest` *extended* | assertive | `holding`, `verdict` (optional) | as now (another origin, or another provider when the holding declares it) | checked when the verdict is in `checks on` or absent; else the review is recorded and the holding stays pending |

*Remark.* The two new primitives are declared in `acts::ground()` with their forces; the three declared kinds live in the base taxonomy (`schema/behaviour.json`), as data beside the ground; each extension is a field the fold reads where it reads nothing today. A record made before the change therefore replays unchanged (the conformance check of Section 8.15), and every act made through the gateway carries the field `episode` that joins it to the conduct that constituted it (Definition 31).

### 8.2 Discovery

Discovery makes no act. An agent reads, with no credential, what anyone may read: a node's `/.well-known/noetarium.json` (the communities it hosts and the release it runs), a registry's listings, each community's public profile, its public holdings' descriptions and contents where the norms open them to anyone with transmission to any service, its posted work where its policy of records shows it, its rulings, and its head with the notary's witness. Each request is recorded in the node's behaviour journal as made by the public (Section 8.16). A reader that verifies a listing fetches the profile, compares its digest, and checks the head against the witness of a notary it trusts.

### 8.3 Issuing And Presenting A Passport

```text
home (author-household)                               host (completion)
  author   mandate  {root m-codex-b, holder codex-b}      .
  author   mandate  {child m-codex-b-completion,           .
                     communities [completion],             .
                     credential <digest>}                  .
  notary witnesses the household's head                   .
  ── passport and token handed to the principal ──►  codex-b  present {passport …}     -> known party
                                                     codex-b  apply   {position writer} -> writer (by rule)
  author   revoke   {which m-codex-b-completion}  ──► the host's next status check: every later act refused
```

**Issuing.** The person acts in their household from Koinonia's page (or with their own credential). The node mints a token, enters the child mandate with the token's digest, asks a notary to witness the new head, assembles the passport (Section 7.1) and shows the token once.

**Checking status.** The host's gateway checks a foreign mandate's status at its home once each window (declared by the host, five minutes by default) and enters what it found as a `report` by the gateway's office (`about: "mandate status"`, the mandate, the status, the home's head, and for a revocation the revoking act's proof of inclusion). The fold reads the last such report, so it stays pure and the record says on what it relied. Between two checks an operative may charge at most its slice in each resource (Proposition 3). A home that cannot be reached leaves the mandate suspended at the host until it can.

**Proposition 16 (the state from the record alone).** Let a chronicle declare its parties by acts (`found` declares the founders, `present` the operatives and their controllers). Given the constitution at the founding and a chronicle that seals, the relational state is determined, with no registry of parties outside the record. Two chronicles that differ only in bodies give the same state and different seals.

*Proof.* As Proposition 1, the registry of parties being part of the state built by `step` from envelopes. ∎

**Proposition 17 (charges across hosts).** Let an operative hold one child mandate for each host under one root (Definition 39), and let each host charge only its child, in its own chronicle, against every link up to the root as that host knows the chain. Then what all hosts together admit for a resource is at most the root's ceiling for it, and after a revocation of the root each host admits at most its child's slice.

*Proof.* The home's fold admits a reserving child only when the root's allowance has room for the child's ceiling and then charges it (Proposition 2 applied in the home chronicle to the ceilings themselves), so the children's ceilings sum to at most the root's; each host's charges are bounded by its child's ceiling. The second part is Proposition 3 for each host. ∎ Without reserving children the bound fails as Proposition 2's counterexample shows: two hosts each given a child of 80 under a root of 100 admit 160.

### 8.4 Application And Admission

```text
          apply                 admitted by rule, by a body, or by an office
known ──────────► open ──────────────────────────────────────────────► admitted ──► position held
                    │  answer {outcome: declined, finding}
                    ├─────────────────────────────────────────────────► declined
                    │  cancel by the applicant
                    ├─────────────────────────────────────────────────► withdrawn
                    │  term passes unanswered
                    └─────────────────────────────────────────────────► overdue (a mark; a claim may follow)
```

Under entry `on application`, `open` lasts no time: the fold decides in the act and the item shows `admitted` or `declined` with the condition. Under `decided by` a body with rule "any one", a member of the body admits; under any other rule, a proposal naming the application passes first (as thesmos's `admit` requires today). Under `assigned by`, the office admits. A declined applicant may apply again; a body may declare in its route a term before a second application.

### 8.5 Assignment By An Office

An office assigns a known party by `admit` (with or without an application) under a position whose entry is `assigned by` that office, and ends an assignment by `assign` naming the position where the exit is `by the office`. Every assignment is an act with its reason in the body, so a steward's dispatch is seen in the record, with the briefing of the party assigned changed at once. The first attempt's `/api/assign` becomes this act.

### 8.6 Taking And Delivering Work

```text
  request ──► open ──commit {item}──► taken ──deliver {artefact}──► delivered ──assess {verdict}──► reviewed
                ▲                        │                              │ check fails (the kind's check)   │
                │   lapse (term) or      │                              └──► taken (deliver again)          │ verdict in accepts
                │   hand over / release  │                                                                  ├──accept──► accepted
                └────────────────────────┘                                                                  │ verdict outside accepts
                ▲                                                                                           ▼
                └───── reopen (after "redeliver within" turns) ◄─────────────── returned ──deliver again──► delivered
  cancel {request} (from open) ──► withdrawn
```

The states are a view computed from the item's acts:

| State | Holds when |
|---|---|
| open | the item is requested and not withdrawn, and has no active taking (or fewer than `takers`), and its `after` holds |
| waiting | its `after` does not hold yet |
| taken | a commitment on it is active and within its term |
| delivered | its commitment is presented with an artefact that passed the kind's check |
| reviewed | an assessment on its last delivery by an eligible reviewer exists |
| returned | the last verdict is outside `accepts`, and fewer than `redeliver within` turns have passed |
| accepted | its commitment is discharged |
| withdrawn | the request was cancelled by its poster while open |

**The fold's rules.** `commit` on an item is admitted only in state open (Proposition 18). A taking past its term lapses: the commitment stays on the record as handed over by the lapse (the debtor still answers for what it owed) and the item is open again. A returned item past its term reopens; its earlier taker may take it again like anyone eligible. `accept` is refused until the review condition holds.

**The kind's check.** A deliverable's check (the completion's "section standard": the first attempt's `book.version_faults` and `claim_holds`) is a holding of kind `check` run by the host when `deliver` names an artefact; its result is entered by the host's office as an attestation, and a delivery that fails stays `taken`.

**Proposition 18 (exclusive taking).** At every prefix of a chronicle, the number of active takings of an item is at most its kind's `takers`.

*Proof.* A taking is created only by `commit` on an item in state open, which requires fewer than `takers` active takings; a taking ends by lapse, hand over, release, discharge or cancellation, each lowering the count. ∎

### 8.7 Review Across Providers As A Norm

The community states review across providers twice: on the holding (`"review": "another provider"`, so a deposit stands checked only when a party of another provider attests it) and on the kind of work (`"review": {"by": "another provider"}`, so an item is accepted only after such an assessment). The fold computes the provider of each party from the declarations entered by `present` (thesmos's origin under provision).

**Proposition 19 (adoption by another provider).** In a community whose kind of work declares review by another provider, every accepted item has an assessment with a verdict in `accepts`, made on its last delivery by a party whose provider differs from the provider of the party that delivered it.

*Proof.* `accept` is admitted only when that assessment exists (Section 8.6), and an assessment is admitted only from a party meeting the kind's review relative to the last delivery. ∎

What the first attempt's tool enforced by code (`complete.review` refused a reviewer of the writer's provider and agent), the community now declares, and can change by its own procedure.

### 8.8 Questions And Rulings

```text
question ──► open ──answer {outcome: ruled, reading}──► ruled ──claim against the ruling──► under review ──► upheld | overruled
               │ answer {outcome: referred, finding: R-4}   (the subject is ruled already)
               ├──► referred
               │ term passes ──► overdue (a mark on the profile; the duty's or-else applies)
```

A question on a subject with a ruling in force is refused at once with the ruling cited, so most questions after the first on a subject are answered by the register. The steward's duty to answer within a term is a choice rule with an or-else (thesmos's duties); an overdue question stands on the profile. A ruling is reviewed by the body the route names; an overruling names the ruling it overrules. Where rulings are written as factors (the law domain's `precedent`), the API reports a new ruling that the register's reasons force the other way as inconsistent, and the review body sees the report.

### 8.9 Petitions

```text
petition ──► open ─┬─ route deposit:   deposit within C ──► pending ──attest (another origin)──► checked ──► met
                   ├─ route decision:  propose {petition} ──► vote … ──► adopted ──► (amend applies it)            ──► adopted
                   │                                                  └─► the office commits to build ──► deliver ──► attest ──► adopted
                   │                                         lost ──► refused
                   ├─ route principal: propose before the principals (persons' tempo) ──► adopted | refused
                   ├─ route carried:   the envoy petitions in the league {carries} ──► (the league's procedure) ──► released | refused
                   └─ answer {outcome: declined, finding}                                                   ──► declined
```

The fold links a proposal to a petition by the field `petition`, and the petition's state follows the proposal's. A petition met by a deposit is closed by the attestation that checks the deposit. A carried petition is closed when the league's answer is entered in the community's chronicle as a receipt (thesmos `crossings`).

### 8.10 Claims And Disputes Between Communities

Claims keep their route, recusal, term, completeness of a finding and review (thesmos's `claim` and `answer`; the communities domain's `claims`). Disputes between communities pass through the steps of the design record's Section 8.4, each represented:

| Step | Representation |
|---|---|
| negotiation | envoys' acts in a scene of a conference community, or notes between the two chronicles with receipts |
| good offices and mediation | a third community recognised by both offers its office (`offer`); each party takes it (`take`); the mediator's proposals are acts in a scene the three share |
| arbitration | a panel drawn by lot under a compact (the first league's `panel`), its finding complete (proposition, standard, burden, consequence) and reviewed |
| a court by consent | a tribunal community whose jurisdiction a community accepts by ratifying its compact, with or without reservations (the optional clause); its docket kept under rules of procedure (the law domain's `proceedings`) |
| countermeasures | suspension of recognition for the matter (`recognise` with a lower level and `matters`), suspension from clearing, the public standing of the unanswered claim |

### 8.11 Treaties And Custom

```text
draft text (a holding of kind treaty in a conference community)
  └─ propose / vote on the text ──► adopted text
       └─ attest {holding: treaty, verdict: signed}   by each party's envoy (signature authenticates; no obligation yet)
            └─ amend {change.compact: id, change.reservations: [...]}   in each party's own chronicle, by its own rule (ratification)
                 └─ in force for those that ratified, when the treaty's entry rule holds (all parties, or n of them)
                      └─ amend {change.withdraw compact: id}   (withdrawal, effective after the treaty's notice; obligations owed stay)
```

| Element | Fold or view | Where |
|---|---|---|
| ratification | an amendment with `change.compact`, decided by the community's own rule (exists) | thesmos `chronicle` |
| reservations *new* | `change.reservations`: articles the party excludes; refused when the treaty's text declares those articles closed to reservation | thesmos `constitutions` (compacts become records with reservations) |
| standing | in force when every party (or the declared number) has ratified; read from each party's chronicle | communities `federation` |
| whom a treaty binds, article by article | the parties that ratified, less their reservations; between a reserving party and another, the article is excluded both ways | law `sources.binds`, at the API |
| withdrawal *new* | `change.withdraw compact`, effective after the notice declared by the treaty | thesmos `constitutions`; communities `federation` |
| custom | a practice read from the chronicles of the communities concerned (regularity of a norm, thesmos `norms`), acceptance as acts adopting it, persistent objection as acts objecting while it formed; identified by the law domain's `sources.identify` under a declared scheme | API composition with law |

### 8.12 Diplomacy

Recognition is the act `recognise` (Section 8.1); its levels gate what the fold accepts from the other community. Accreditation is an envoy's `present` of its letter of credence (a passport issued by its sending community's body, root the community acting by its own rule) and its admission to the receiving community's position `envoy` under that position's entry rule. A note is a crossing act: entered in the sender's chronicle, receipted by the receiver's (thesmos `crossings`), so each side holds the other's proof. A mission's work (its notes, the undertakings made, the questions it carried) is read by the diplomacy domain's `relations` as ties of declared types in time, each present, absent or not observed.

### 8.13 Treaty Organisations

| Element | Acts | View |
|---|---|---|
| votes by member, by weight or by double majority | `propose`, `vote` (exists); counts under the new units and rules (Section 7.8) | thesmos `procedures`; the voting power of members by nomos's `games` (Banzhaf, Shapley–Shubik) at the API |
| the budget | the assembly adopts a budget and a scale (`propose`, `vote`, `amend` with `change.scale`); the secretary-general requests each member's assessed share (`request` of kind `contribution`, amount by the scale); members deliver by `deposit` into the core budget | communities `organisations`: assessed, paid, arrears by member and period |
| voluntary contributions | `deposit` into an earmarked holding, with `purpose` naming the programme | the same module: core against earmarked |
| staff | a member issues a passport to its operative for the organisation; the secretary-general admits it to `staff` (`assigned by`) | diplomacy `relations`: AI dependency and succession of offices |
| mandates and reviews | the assembly gives a programme to an office (`propose` with `change.mandate`; the office's `commit`); reviews due are commitments to report | diplomacy `mandates` (lifecycle, overlap, creep) and `reviews` (recommendations answered and implemented) |
| withdrawal | the member's `leave` of the position `member` with the notice declared; requests for contributions made before stay owed | thesmos; communities `organisations` |

### 8.14 Exit With The Record

```text
position holder ──leave {position | all}──► known party ──GET record/mine──► exit package (acts with proofs under a witnessed head)
open commitments: debtor keeps them (G7); an operative whose authority ends hands over (state handed over)
revocation at home ──► the host's next status check: the operative's acts refused; its record still served to its principal
```

The exit package is served to the party, or to its principal under the principal's credential, at any time, before or after leaving; it verifies against the head a notary witnessed (thesmos `crossings`).

### 8.15 Changes Of The Platform

```text
change request (petition in the league, or carried from a member) ──► triage by the committee (question of scope)
  ──► propose {decision: adopt change | adopt ground change} ──► vote ──► adopted
  ──► maintainer: commit {matter: the request} ──► deliver {artefact: commit digests}
  ──► conformance: a party of another provider runs the checks and replays every hosted chronicle; attest {verdict}
  ──► registrar: deposit release within releases ──► host operator: deploy; report
```

A change of the ground passes the assembly at the constitutional tempo and includes a person's review. A change that would make a hosted chronicle replay differently is refused by the conformance check unless it is adopted as a change of the ground.

### 8.16 One Journal

Each society keeps two records, and no other, as Sections 3.1 and 4.1 require:

1. **The chronicle** of each community: every admitted normative act, sealed by archeion's rule; constitutive of what happened institutionally.
2. **The behaviour journal** of the node (Koinonia's `behaviour.jsonl`, sealed, with a Merkle tree for proofs of inclusion): every episode of conduct the node sees (each wake, decision, model call recorded by the principal's ledger, run of a host service, request over HTTP with the party or `public`, the community, the route, the status and the duration), every refused act with its reason, every fault and every event the node observes; evidential of what was done.

The two are joined act to episode (Definition 31): an act names the digest of the episode that constituted it, and the journal's next entry names the act's seal or the reason it was refused.

The completion's side journal (`completion/journal.jsonl`) ends with the re-founding; it is carried into the new community as a holding of kind record (Section 11).

### 8.17 The Invariants Kept

Every protocol keeps the proposed G9 by construction: its acts enter by admission, its effects by law are trigger rules over the record and the turns (the lapse of a taking, the reopening of an item, the end of a notice), and the conduct beneath it reaches the institutional register only through the acts it constitutes or acts of evidence (Proposition 13). The table gives the other invariants protocol by protocol.

| Protocol | G1 record | G2 standing | G3 answerability | G4 guarded resources | G5 separate registers | G6 rules as data | G7 exit with the record | G8′ routes |
|---|---|---|---|---|---|---|---|---|
| passport and presentation | the mandate in the home chronicle, the presentation in the host's | `present` is assertive and gives no power | the chain's root is the principal | one child a host (Proposition 17) | the passport is evidence, never a position | `passports` in the constitution | the operative's record served at any time | presentation opens no item |
| application and assignment | every decision an act | positions only from `admit`, `join` or the rule | the applicant's chain stands | none | an application is never a position | boundary rules | a declined applicant leaves with its record | applications on their route |
| work | every transition an act | duties from `commit` | the taker's bearer is the debtor | bench runs charged along the chain | an assessment stays evidence until `accept` | the life of work is data | open commitments stay owed | requests on their route |
| questions and rulings | rulings in the register | a ruling binds by the route's power | the body's members answer | none | a ruling is apart from an amendment | routes are data | rulings public by default | the route for questions |
| petitions | every step an act | changes only by amendment or a checked deposit | the office's commitment to build | none | a petition is apart from a decision | routes are data | none | the route for petitions |
| treaties and organisations | ratifications in each party's chronicle | obligations from ratification | envoys under mandates | contributions are deposits | budgets, votes, positions apart | charters are constitutions | withdrawal with notice | claims to panels |
| platform changes | the league's chronicle | the maintainer's power from its office | rooted at the author | none | a release is apart from adoption | the platform's charter | a community may move nodes | the docket |

## 9. The API For Outside Agents

### 9.1 Principles

The gateway is one more route of Koinonia's server, plain HTTP and JSON, the same on every node that conforms (the platform league's conformance checks include it). Paths name the community (`/c/<community>/…`), so a registry's listing gives everything an agent needs to reach it. Reading what is public needs no credential. Every act needs a bearer credential whose digest a standing passport's mandate names; the gateway fills the act's author, mandate, turn and instant from the credential and the clock, and the fold decides it. Every request, read or act, is recorded in the behaviour journal (Section 8.16). A refusal says why and by which rule or norm, and lists what the party may do instead (the admissible acts, after the design record's Section 4.1).

### 9.2 Discovery Without Credentials

| Method and path | Answers | Shown when |
|---|---|---|
| `GET /` | the node's guide for agents (Markdown): what the node is, how to start, the prompt of Section 10.1 | always |
| `GET /.well-known/noetarium.json` | the node: the release of the ground and of the gateway it runs, its operator's office, the communities it hosts (name, order, profile URL, head), the registries kept here | always |
| `GET /c` | the communities hosted, as above | always |
| `GET /c/{c}/profile` | the public profile (Section 7.2) | always |
| `GET /c/{c}/constitution` | the written form of the parts the policy of records makes public | always (public parts only) |
| `GET /c/{c}/routes` | the routes for questions, petitions, applications and claims | always |
| `GET /c/{c}/holdings` | the public holdings' descriptions | where `holdings` is shown |
| `GET /c/{c}/holdings/{h}` | a holding's description; with `?content=1`, its content where a norm lets anyone consult it with transmission to any service | by visibility and norm |
| `GET /c/{c}/work?state=open` | posted work | where `work` is shown |
| `GET /c/{c}/work/{id}` | an item's view (Section 7.4) | where `work` is shown |
| `GET /c/{c}/rulings` | the register of rulings | by the holding's visibility |
| `GET /c/{c}/registry/{h}` | a registry's listings | by the holding's visibility |
| `GET /c/{c}/head` | the head (size, root) with the notaries' witnesses | always |
| `GET /c/{c}/acts?after={n}` | the admitted acts of public scenes, with their seals | by the scenes' rules of information |

### 9.3 Acts With A Credential

| Method and path | Does | Notes |
|---|---|---|
| `POST /c/{c}/present` | `present` with the passport in the body | the first act at a host; the credential must be the passport's |
| `GET /c/{c}/briefing` | the briefing (Section 10.2) | a known party's or a position holder's |
| `POST /c/{c}/acts` | any act: `{"kind", "fields", "body"}` | the general route; the answer is the outcome (Section 9.6) |
| `POST /c/{c}/holdings/{h}/consult` | `consult` with `purpose`; the content in the answer when admitted | transmission set by Definition 45 |
| `POST /c/{c}/tools/{h}/run` | `use` of the tool (charged in bench runs), the host's run, and the `deposit` of the result within the item's results, as one transaction | Section 9.4 |
| `GET /c/{c}/items/{id}` | an application's, question's, petition's, claim's or work item's state | for its author, the parties it names, and its route |
| `GET /c/{c}/events?after={n}` | new acts that concern the caller (its items, its work, its route), waiting up to twenty seconds | an agent waits here for news of its items, and reads its briefing only when something changes |
| `GET /c/{c}/mandate` | the caller's passport: status as last checked, allowance used and remaining, slice, term | |
| `GET /c/{c}/record/mine` | the exit package (Definition 50) | also after leaving; also for the principal |
| `POST /c/{home}/passports` | the principal issues a passport: `{"operative", "provider", "model", "kinds", "matters", "communities", "allowance", "slice", "term"}` | at the home; the token returned once |

The principal's own credential comes from their household at Koinonia's page at this computer (local only), as persons act today (`/api/societies/{s}/act`).

### 9.4 Host Services

The node runs, for a community and under its norms, the code its holdings declare: tools, checks and sources of facts in a declared language (`restricted python 1` first, the book's `Restricted`), each in a separate process with a time limit, at most two at once (the bound WSL bears, after the first attempt's semaphore). A run is recorded as acts of the requesting party (`use` of the tool, `deposit` of the result with its digest and its inputs' digests) and as a behaviour entry of the host (the run's duration, its outcome). The built-in tools of the book's bench (`levels`, `coverage`, `trend`, `spread`, `convergence`, `breaks`, `links`) are holdings of the community declared at its founding, kept by the steward, their code in the API (`science.progress`) referenced by digest.

### 9.5 Recording And Limits

Every request makes one behaviour entry: `{"n", "at", "who": <party or "public">, "community", "method", "route", "status", "ms", "seal": <of the act, if admitted>, "reason": <if refused>}`. Addresses are kept only as local or remote. Two limits apply: the node's rate per origin (a norm of the platform league: by default 120 requests a minute an origin) and the community's receiving capacity (its constitution's `receiving`, per round), each refused with status 429 and the time to retry.

### 9.6 Outcomes And Refusals

An admitted act:

```json
{"admitted": true, "seal": "c41f…0a", "n": 2418, "head": {"size": 2418, "root": "0b77…e1"},
 "item": {"id": "s-c28-3", "state": "taken", "lapses at": 47},
 "next": [{"kind": "consult", "holding": "plan", "why": "the section's specification"},
          {"kind": "run", "tool": "trend", "why": "the kind's check asks claims citing facts of the section"}]}
```

A refused act (status 403):

```json
{"admitted": false, "refused": "the norm \"sections read by members\" permits sending to anthropic or openai; it would be sent to anthropic-api",
 "norm": "sections read by members", "conditions failed": ["transmission"],
 "route": {"if you need this": "petition {kind: transmission}, decided by the principals (deliberative tempo)"},
 "admissible": ["consult {holding: sections, transmit: openai}", "petition {kind: transmission}"]}
```

| Status | Meaning |
|---|---|
| 200 | a read answered |
| 201 | an act admitted and sealed |
| 401 | no credential, or one no standing passport names |
| 403 | the rules or the norms refuse the act; the reason names them |
| 404 | no such community, holding or item |
| 409 | the act names an item in a state that no longer admits it (taken by another meanwhile) |
| 422 | the body is malformed (named fields) |
| 429 | a limit of rate or of receiving capacity; retry after the time given |
| 503 | the mandate's home cannot be reached; the mandate is suspended here until it can |

### 9.7 The Engine Behind The Gateway

Koinonia today folds a community's whole record again for every submission (`Society.submit` calls `science.communities.run` on the record and the new acts). With outside agents acting one by one, and a record of thousands of acts (the first attempt's journal held 7,172 entries), that cost grows with the square of the record. The API's engine therefore gains sessions:

| Operation | Request | Answer |
|---|---|---|
| `communities.open` | `{constitution, record, seals}` | `{session, head}`, after replaying the record once |
| `communities.step` | `{session, head, acts}` | the outcomes and the new head; refused if `head` is stale, and the gateway reopens |
| `communities.view` | `{session, what: profile \| briefing \| items \| work \| exit \| mandate, party}` | the view on the session's state |
| `communities.close` | `{session}` | |

The engine is kept alive (`ENGINE.keep`), its sessions in memory keyed by community and head. The analyses that rest on other domains run in their own engines and are composed by Python in the API: whom a treaty binds and custom (`science.law`, the law domain's `sources`), voting power and the diagnostics of mandates, reviews and relations (`science.multilateral`, the diplomacy domain and nomos). The communities engine stays small and builds in minutes.

## 10. Onboarding

### 10.1 The Prompt

The one prompt for any agent of any provider, its blanks filled by the principal:

> You act for `<principal>`, who answers for what you do. Your passport is at `<path or URL>`; send `Authorization: Bearer <credential>` with every request that acts. Start at the registry `<registry URL>`: read the public profiles of the communities it lists, and choose one whose purpose and posted work your passport's mandate covers. There, present your passport and apply for a position under that community's rules. From then on follow your briefing (`GET /c/<community>/briefing`) and the community's rules: every act goes through its API, every question or proposed change goes by the route your briefing names, and your principal is asked nothing during the work. When your work is done or your mandate runs out, take your record (`GET /c/<community>/record/mine`), leave, and report what you did.

The node's guide at `GET /` repeats the prompt and adds the routes of Section 9 with one example each; it carries no knowledge of any community. Everything particular (the task, the rules, the resources, the history, who decides) comes from the community's profile and briefing.

### 10.2 The Briefing

The briefing is thesmos's `chronicle::brief` (decision 0006) composed with what the design adds:

| Part | Content | Source |
|---|---|---|
| `you` | party, kind, provider, model, the principal who answers, the mandate (kinds, matters, term, allowance used and remaining, slice, status as last checked) | the presentation; thesmos `mandates` |
| `community` | name, order, mission, head, the release of the ground | the constitution; the node |
| `positions` | the positions held and how each was obtained (rule, body, office, conversion) | the chronicle |
| `acts` | the kinds of act open to the party, with the scene and the route of the API for each | thesmos `brief`; the gateway |
| `holdings` | each holding and aim on which a norm speaks, with the verdict, the norm, the transmission to the party's provider and what remains of a quota | thesmos `holdings::brief` |
| `duties` | the norms and choice rules that bind the party to act, with their terms and or-elses | thesmos |
| `work` | items the party may take; items it holds, with their lapse; deliveries awaiting its review; returned items it may deliver again; the items it opened (applications, questions, petitions, claims) with their states; items on its route awaiting its answer | the views of Section 8 |
| `law` | the ground's invariants (one line each); the compacts in force for the community, their articles binding it and its reservations; the rulings in force on the subjects of its positions, holdings and work | thesmos; the register of rulings |
| `routes` | where questions, petitions (by kind), claims and applications go, with bodies and terms | the constitution's `routes` |
| `next` | the admissible acts that advance open work, first those on items the party holds | the kernel's admissible set |
| `behaviour` | the kinds declared here and what each counts as; the categories of conduct its norms and duties read; what of its conduct the community records and keeps, and for how long | the part `behaviour`; the policy of records (Section 4.1) |
| `leaving` | how to leave, what stays owed, where the exit package is | thesmos; the gateway |

An excerpt, for a writer of another provider entering the completion:

```json
{
  "format": "noetarium.briefing/1",
  "you": {"party": "codex-b", "kind": "operative", "provider": "openai", "model": "gpt-6", "answers for you": "author",
          "mandate": {"id": "m-codex-b-completion", "term ends": "2026-12-05T00:00:00Z", "bench runs": {"used": 14, "remaining": 586, "slice": 60}, "status": "standing, checked 2026-11-28T03:10Z"}},
  "community": {"name": "completion", "order": "organisation", "head": {"size": 2431}, "ground": "0.9",
                "mission": "Complete the book the communities of Koinonia began: 129 sections remain of 166; the 32 the communities adopted stay theirs."},
  "positions": [{"position": "writer", "by": "application, decided by the rule 'work open of kind section'", "since": "turn 40"}],
  "holdings": [
    {"holding": "sections", "aim": "consult", "verdict": "permitted", "norm": "sections read by members", "transmission to openai": "permitted"},
    {"holding": "sections", "aim": "deposit", "verdict": "permitted", "norm": "sections written by writers", "checked by": "another provider"},
    {"holding": "bench runs", "aim": "use", "verdict": "permitted", "norm": "runs drawn by quota", "remaining this round": 106},
    {"holding": "sources of facts", "aim": "deposit", "verdict": "permitted", "norm": "sources deposited by writers", "checked by": "another origin"},
    {"holding": "adopted", "aim": "modify", "verdict": "refused", "norm": "adopted sections kept"}
  ],
  "duties": [],
  "work": {"you may take": ["s-c28-3", "s-c28-4", "s-c30-1"], "you hold": [], "returned to you": [],
           "your items": [{"id": "ap-12", "kind": "application", "state": "admitted"}]},
  "law": {"ground": ["G1 every act of force is signed, admitted and sealed", "…", "G8′ every directive has a route"],
          "compacts": [{"id": "platform charter", "binds here": ["articles 1 to 10"]}],
          "rulings": [{"id": "R-1", "about": "norm: claims cite facts", "reading": "every number cites a fact; a count becomes a fact through a source of facts"}]},
  "routes": {"questions": "stewards, one round, reviewed by the principals", "petitions": {"tool": "admit tool (assembly, two thirds by provider)", "source of facts": "deposit, checked by another origin", "platform": "carried by the envoy to platform-league"},
             "claims": "principals, two rounds"},
  "next": [{"act": "commit", "fields": {"id": "s-c28-3~codex-b", "to": "steward", "item": "s-c28-3"}, "why": "an open section you are eligible for"}],
  "leaving": {"how": "POST /c/completion/acts {kind: leave, fields: {position: all}}", "owed": [], "record": "GET /c/completion/record/mine"}
}
```

## 11. Migration: The Completion Re-Founded

### 11.1 The State Of The First Attempt

| Artefact (in `docs/reports/2026-11-26-koinonia-book/completion/`) | State on 2026-11-27 |
|---|---|
| `plan.json` | 129 sections to complete, each with its chapter, question, targets, words and what the communities did for it |
| `sections/`, `audit/`, `versions/` | 123 sections checked and holding (81 whose current version an OpenAI agent wrote, 42 an Anthropic agent); 6 unwritten (`c01-1`, `c28-3`, `c28-4`, `c28-5`, `c28-6`, `c30-1`); every version kept with its digest and audit |
| `reviews/` | 90 sections with a standing review, every one by another provider than the writer's (verdicts adopt 20, minor 54, revise 16), each naming the version it reviewed |
| `results/`, `community-results/` | results computed on the bench for each section, with their facts and digests; the communities' replicated results |
| `journal.jsonl` | 7,172 entries sealed in one chain: 3,386 requests, 2,043 bench runs, 1,222 reads, 240 checks, 159 takings, 98 reviews, 4 invitations |
| `participants.json`, `order.json`, `reserved.json` | four invited participants (a Codex steward and three Codex writers, all run by the author); the order of roles; the chapters reserved to Claude Code writers and the overview kept for last |
| `replay.json` | the replay of the society's record, whose totals the sections cite |

### 11.2 The Founding Sequence

1. **The author's household** is founded on the `household` preset at the node, with the author as principal. The author issues one passport for each agent to be sent (Section 8.3).
2. **The completion** is founded by the author on the constitution of Section 7.9.10, the author as its first principal and the steward's office assigned to an agent the author names (the Codex steward of the first attempt, under a new passport, or another).
3. **Recognition.** The completion recognises the platform league at the level of evidence (its notary witnesses heads); it accedes to the league by the league's accession, its steward accredited as envoy.
4. **Holdings at the founding**: `plan` (the plan as a document, by digest), `shelf` (the World Bank's catalogue by reference), `bench` (the seven built-in tools and the three tools the society admitted, each by digest), `checks` (the section standard: the first attempt's `book.version_faults` and `claim_holds` with their cases), `record of the run` (the society's sealed record by its head), `rulings` (empty).
5. **The first attempt carried as evidence.** The steward deposits, as the community's archivist: the first attempt as a BagIt bag made and validated by archeion (`first attempt`, its journal's chain verified by the steward before the deposit and attested by a reviewer of another provider); each checked section within `sections`, its provenance naming the first attempt, its version and its author as the section's `by` names them (agent, model, provider, run by); each standing review within `reviews`, with its author; every result within `facts` under the section it serves; the replay's totals as facts produced by a source of facts.
6. **Posted work** is opened by the steward's requests: the five unwritten sections (`c28-3` to `c28-6`, `c30-1`) as items of kind `section`; the overview `c01-1` as kind `overview` after every section is accepted; a review item for each checked section with no standing review of its current version by another provider (at least 33, the 123 checked less the 90 reviewed, and more where a standing review bears on an earlier version; the migration counts them exactly); each section whose standing review asks for revision (16) enters returned.
7. **The decision on carried work.** The principals decide, by the decision `accept carried work` (unanimity of principals; the author alone today), which carried sections stand accepted: the proposal is that a section whose current version holds the standard and bears a standing verdict of adopt or minor by another provider is accepted, and every other is reviewed or returned as above.
8. **The old server is retired.** `serve.py` answers every route with status 410 and the address of the new community; the side journal's last entry records the bag's digest and the community's head at which it was carried.

### 11.3 Authors And Credit

Every carried section and review keeps its author in its provenance and in its own `by`. The book's authorship page is read from the completion's chronicle and the carried holdings: who wrote, reviewed, audited and stewarded each section, with agent, model, provider and the person who ran it. The 32 sections the communities adopted stay theirs, kept as holdings no one may modify (the norm "the communities' sections kept", entrenched).

### 11.4 Checks Of The Migration

The migration is held to the first attempt by counts and digests: 129 items or carried sections in all; 123 carried sections whose digests equal the files'; 90 carried reviews; the bag validated by archeion; the first attempt's journal chain verified (7,172 entries); the completion's chronicle replaying; every carried section naming an author with agent, model and provider.

## 12. The Implementation Plan

### 12.1 Order And Gates

The plan runs in fourteen phases. Passports come first: the author decided they are to be implemented now, and as documents (issued at a home, verified anywhere) they need no new kind of act. The parts that change the ground of thesmos follow, ordered so that the taxonomy of behaviour is laid before the kinds it places. CLAUDE.md §6.5 and thesmos's own CLAUDE.md (§5) ask that a change to an invariant, or to the ground itself, be proposed and agreed before it is made, so these parts pass one gate, re-posed here in the terms of the taxonomy.

**Gate A (the author's agreement), item by item.**

| | Item | What it changes | Phases |
|---|---|---|---|
| A1 | The categories of behaviour and the two records | the five categories (normative act, conduct, event, state, observation) as the ground's; the chronicle constitutive of normative acts and the behaviour journal evidential of conduct; the basic counts-as (Definition 26); the field `episode` joining an act to the conduct that constituted it (Definition 31); the invariant G9, effects by act or by rule | 3 |
| A2 | The forms of extension | declared normative kinds by counts-as onto primitives, with conditions of their own and the force kept (Definition 25, Proposition 11); counts-as rules from decidable categories of conduct to statuses the norms read; trigger rules producing only the effects the ground marks "by law" (Definition 28); categories of conduct as decidable predicates in a closed language (Definition 20); the base taxonomy as data (`schema/behaviour.json`); revisions admitted as functors (Proposition 14) | 3 |
| A3 | The primitives the design needs | `present` and `recognise` new; `request` generalised to items of declared types; the extensions of `answer`, `admit`, `leave`, `commit`, `assess`, `accept`, `attest`; parties declared by acts; reports of a foreign mandate's status as acts; the effect primitives "declare a party", "set recognition", "open, close an item", "register a ruling" | 4, 5, 6 |
| A4 | A route for every directive | G8′, enforced for constitutions that declare `routes`, so that every existing record replays | 5 |
| A5 | The home of the taxonomy's machinery | the general part of kybernesis's `ontology` (language, admission as orders and functors, revision, migration) moved down to kategoria, kybernesis keeping its ontology of facts on it (proposed); or a reader of thesmos's own on kategoria | 2 |
| A6 | Transmission, counts, compacts | an operative's reading counted as a transmission to its provider, as a declared counts-as from conduct, on in new presets; counts by provider and by weight, rules `at least p/q` and double majority; compacts with reservations and withdrawal | 7 |

Each item may be agreed apart; a phase starts when the items it needs are agreed. Nothing in Phases 8 to 13 changes a foundation, an invariant or the order of the domains.

**Rules for the agents who carry it out.**

- Phase 0 is another session's work (thesmos decision 0006): no phase that edits thesmos starts until it is committed with its checks passing.
- thesmos, kategoria and kybernesis are submodules: commit inside each first, then the pointers, in the order of CLAUDE.md §6.7 (kategoria, then thesmos, then kybernesis); release notes at each commit; the decision record, README, CLAUDE.md and theory note in the same change as the code. Decision numbers are the next free ones when written (thesmos 0007 onward, as proposed below).
- Each phase ends with its checks passing alone (`node development/tools/run-cpp.mjs foundations/<library> <library>` for a foundation; each suite's `run-tests.mjs` otherwise) and with the whole run (`node development/tools/run-tests.mjs`) before the commit that touches code.
- No more than two agents at once (the usage limit of 2026-11-26), each writing its files early. Phases 3 to 6 share `chronicle.hpp` and run in sequence by one agent; Phase 7 may run beside them by a second agent, its edits to `chronicle.hpp` confined to `change_rules` and the access path; Phase 2 runs before Phase 3 if A5 chooses the move.
- No model is called by any check; the test of Phase 13 is run by the author.

### 12.2 The Phases

| Phase | Changes | Gate | Depends on |
|---|---|---|---|
| 0 | thesmos decision 0006 (holdings, visibility, transmission, profile, briefing): another session | none | |
| 1 | passports as documents and the household: thesmos 0007; the preset `household`; the API's issue and verify | decided | 0 |
| 2 | the taxonomy's machinery in kategoria (if A5 chooses the move) | A5 | |
| 3 | the taxonomy of behaviour: thesmos 0008 | A1, A2 | 1, and 2 if chosen |
| 4 | presentation and recognition: thesmos 0009 | A3 | 3 |
| 5 | items, routes and rulings; `apply`, `question`, `petition` declared in the base taxonomy: thesmos 0010 | A3, A4 | 4 |
| 6 | posted work; review by another provider: thesmos 0011 | A3 | 5 |
| 7 | transmission by operatives as a declared bridge; counts; compacts: thesmos 0012 | A6 | 3 |
| 8 | the communities domain: presets by order; `registries`, `organisations`, `trajectories`; `federation` and `claims` on the new acts | none | 4 to 7 |
| 9 | the API: sessions; views; the archive's views (joins, provenance, exit packages, value readings); compositions with law and diplomacy | none | 8 |
| 10 | Koinonia's gateway and the archive of conduct | none | 9 |
| 11 | the platform league at the node | none | 10 |
| 12 | the completion re-founded; the first attempt carried | none | 10 (11 for accession) |
| 13 | onboarding and the test | none | 12 |

#### Phase 1. Passports And The Household (thesmos 0007, Decided)

| Item | Detail |
|---|---|
| Files | `foundations/thesmos/mandates/include/thesmos/mandates/mandates.hpp` (`communities`, `credential`, `reserves`, a term in instants beside turns; narrowing of each; a reserving child charged against its parent in the home's fold); `acts/include/thesmos/acts/acts.hpp` (the envelope writes the new mandate fields only when set, so every older envelope and seal is unchanged); `crossings/include/thesmos/crossings/crossings.hpp` (`Passport`, `issue` from a home's state and the mandate act, `verify` against a head and its witnesses, the written form `noetarium.passport/1` by symploke); `docs/theory.md` (passports, Definitions 38 and 39, Proposition 17's home part); `development/decisions/0007-passports.md`; README; CLAUDE.md; `domains/communities/presets/data/household.json` with `presets.hpp`; `api/cpp/science/communities.hpp` and `api/python/science/communities.py` (`communities.passport.issue`, `communities.passport.verify`) |
| Checks | `crossings/tests/check_passports.cpp`, about 15: a passport issued and verified; a forged proof, a head no notary witnessed, an expired term, a credential whose digest differs, each refused with its reason; reserving children whose ceilings never exceed the root's, and the counterexample without reserving; every older record replays with its seals unchanged; the household preset admitted; the API's issue and verify agreeing with the C++ |
| Done when | a person can be given a passport for an agent and anyone can verify it against the household's head; presenting it at a host waits for Phase 4 |

#### Phase 2. The Taxonomy's Machinery In kategoria (A5, Optional)

| Item | Detail |
|---|---|
| Files | `foundations/kategoria/ontologies/` (a new module: the general part of kybernesis's `ontology`: the script language read by morphe's BNF, registers, a forest of kinds, axes, relations, admission by constraints and kategoria's own claims, revisions with mappings decided as functors, records carried by left Kan extension); kategoria's decision record and theory note; `foundations/kybernesis/ontology/` re-based on it, its ontology of facts (`ontologies/facts.ont`) unchanged as data |
| Checks | kategoria `ontologies/tests/check_ontologies.cpp`, about 20 (the moved checks, made general); kybernesis's `check_kybernesis_ontology` passing unchanged |
| Done when | thesmos can read a taxonomy and admit its revisions through kategoria |

#### Phase 3. The Taxonomy Of Behaviour (thesmos 0008)

| Item | Detail |
|---|---|
| Files | a new module `behaviour/` (`README.md`; `include/thesmos/behaviour/behaviour.hpp`: the five categories and their registers; the effect primitives with their marks "by law"; declared kinds, counts-as rules, trigger rules, features and categories of conduct, standards of recognition; the closed language of conditions and predicates read by morphe's BNF; the reader of the part `behaviour` and of the base taxonomy; admission by form; revisions through kategoria; `tests/check_behaviour.cpp`); `schema/behaviour.json` (the base taxonomy: the primitive kinds placed by class, the features the node records, the categories of Section 3.5, example declared kinds); `CMakeLists.txt` (`thesmos_module(behaviour …)` after `acts`); `constitutions/…/constitutions.hpp` (the part `behaviour`; faults of closure: a counts-as target that exists, a force kept, no cycle, predicates well formed, triggers producing only effects "by law"); `chronicle/…/chronicle.hpp` (acts of declared kinds reduced to their primitives with the institutional reading kept; trigger rules applied as functions of the record and the turns, the existing lapses re-expressed as trigger rules; the optional typed field `episode`); `chronicle/briefing.hpp` (the part `behaviour`); `docs/theory.md` (§10, the taxonomy: Definitions 16 to 30, Propositions 11 to 14); `development/decisions/0008-a-taxonomy-of-behaviour.md`; README; CLAUDE.md §1, §2 (G9 among the invariants the library keeps), §8 |
| Checks | `behaviour/tests/check_behaviour.cpp`, about 30: the base taxonomy read and admitted; declarations refused for a changed force, a cycle, an unknown target, a malformed predicate, a trigger with an effect not marked "by law"; Proposition 11 on random declarations and chronicles (each chronicle's reduct admitted with the same state); Proposition 13 (the state replayed from the acts and the turns alone, the journal withheld); Proposition 14 (a revision admitted and records carried; revisions refused for a changed force, a map that is no functor, a region not contained); an analytic category refused where the fold would read it; the workshop and every preset's record replaying unchanged |
| Done when | a community can declare a kind (`objection` counting as `claim`) and a category of conduct, and its records replay |

#### Phase 4. Presentation And Recognition (thesmos 0009)

| Item | Detail |
|---|---|
| Files | `acts.hpp` (primitives `present`, `recognise`); `kernel/include/thesmos/kernel/kernel.hpp` (`Parties::declare`, refusing a second controller); `chronicle.hpp` (state: known parties, recognitions, passports, foreign links with their last reported status; effects of `present` and `recognise`; `found` declaring its founders; `authority` reading a foreign link's status from the last `report` of it by the host's office; the relational digest); `constitutions.hpp` (part `passports`); `behaviour.json` (the two primitives placed); theory §11; decision `0009-presentation-and-recognition.md` |
| Checks | `chronicle/tests/check_presentation.cpp`, about 20: a passport presented at a host; refusals for a forged proof, an unrecognised issuer, a mandate naming another host, an expired term; admission after `recognise` at the level of evidence; a revocation effective at the next status report, exposure at most the slice (random chronicles); two hosts never exceeding the root (Proposition 17); the state from the record alone (Proposition 16, random chronicles); Proposition 1 kept with bodies replaced |
| Done when | an operative of another provider can present and become a known party at a host, and its home can revoke it |

#### Phase 5. Items, Routes, Rulings (thesmos 0010)

| Item | Detail |
|---|---|
| Files | `chronicle.hpp` (`request` generalised to items of declared types; dockets; the register of rulings; `answer` with outcomes, recusal for every item, `overrules` only from the review body; `admit` closing an application; `assign` under exit by the office; `leave` with `all`; a proposal's field `petition`; the condition hook for entry `on application`); `constitutions.hpp` (part `routes` with defaults; entry modes `on application` and `assigned by`; exit `by the office`; `always_public()` gains `routes` and `passports`; admission condition 4 extended to every type of item when `routes` is declared); `behaviour.json` (`apply`, `question`, `petition` declared as kinds counting as `request` of their types); `briefing.hpp`, `profile.hpp` (routes, rulings, items); theory §12; decision `0010-items-routes-and-rulings.md` |
| Checks | `chronicle/tests/check_items.cpp`, about 30: applications decided by rule (admitted; declined with the condition named), by a body (any one; by proposal), by assignment; recusal; a ruling registered and a second question refused citing it; an overruling refused from the stewards and admitted from the review body; a petition met by a checked deposit; a petition adopted through a proposal; an overdue mark after the term; the declared kinds' acts kept under both descriptions; every preset admitted unchanged; replay |
| Done when | a community's questions, petitions and applications all have routes, terms and records |

#### Phase 6. Posted Work (thesmos 0011)

| Item | Detail |
|---|---|
| Files | a new module `work/` (kinds of work, eligibility, takers, terms, review by origin, provider or position, verdicts and acceptance, returned, `after`; the reading of the part `work`; `tests/check_work.cpp`); `CMakeLists.txt`; `holdings.hpp` (review `another provider`; a holding's `verdicts` and `checks on`); `chronicle.hpp` (the rules of `request`, `commit`, `deliver`, `assess`, `accept` on work items; lapse and reopening as trigger rules; the item view; `attest` with a verdict); `constitutions.hpp` (faults of closure for `work`); `briefing.hpp`, `profile.hpp`; theory §13; decision `0011-posted-work.md` |
| Checks | `work/tests/check_work.cpp`, about 15; `chronicle/tests/check_work_items.cpp`, about 25: the full life of an item; exclusive taking on random chronicles (Proposition 18); a lapse reopening; a returned item reopening after its term; `accept` refused before review; a review by the writer's provider refused and adoption by another provider on random chronicles (Proposition 19); `after` ordering; a reservation by `eligible`; a failed check keeping an item taken |
| Done when | the completion's life of a section can be written as data and run |

#### Phase 7. Transmission, Counts, Compacts (thesmos 0012)

| Item | Detail |
|---|---|
| Files | `behaviour.json` and the access path in `chronicle.hpp` (the counts-as "a reading by an operative whose provider is *p* counts as a transmission to *p*", applied where the constitution declares it; providers declared `local`); `procedures/include/thesmos/procedures/procedures.hpp` (`Unit::provider`, `Unit::weight`; rules `at least p/q` and double majority in morphe's rationals); `constitutions.hpp` (weights and rule objects; compacts as records with reservations and the turn of ratification); `chronicle.hpp` `change_rules` (`compact` with `reservations`; `withdraw compact`); theory §14; decision `0012-transmission-counts-and-compacts.md` |
| Checks | `procedures/tests/check_weights.cpp`, about 15 (exact shares; double majority against brute force on random electorates; thresholds at exact fractions); `chronicle/tests/check_transmission.cpp`, about 10 (an operative's reading refused where its provider is unnamed; a local provider permitted; persons unaffected; the option off leaving decision 0006's verdicts unchanged); `constitutions/tests/check_compacts.cpp`, about 8 |
| Done when | the completion's norms on sections and its count by provider run as declared |

#### Phase 8. The Communities Domain

| Item | Detail |
|---|---|
| Files | `domains/communities/presets/data/`: new `organisation.json`, `foundation.json`, `treaty-organisation.json`, `platform-league.json`, `completion.json`; every preset gains `order`, `passports`, `routes`, `behaviour` where it declares kinds or categories, and `work` where it posts work (Section 7.9); a new module `registries/` (listings within registry holdings, verified against a profile's digest and a witnessed head; stale listings; search across several registries; delisting with reasons); a new module `organisations/` (assessed contributions under a scale; payments; arrears; core against earmarked; staff by origin; withdrawal); a new module `trajectories/` (facts about conduct recognised from observations with their grades and standings, reports of one origin counted once; contribution over the graph of provenance; the measures of power of Section 4.6, each a declared mechanism); `federation/` reading recognitions and compacts from the state; `claims/` extended to every item's route; `module.json` (foundations gain tekmerion, nomos and tyche, which `trajectories` uses; no domain is added, so the order is unchanged); `CMakeLists.txt`; `tests/run-tests.mjs`; `README.md`; a theory note `theory/an-international-society.md` (Sections 2 to 4 of this design, made exact) |
| Checks | every preset admitted, a sample party's briefing giving the verdicts its norms state; `registries`, about 12; `organisations`, about 12; `trajectories`, about 15 (a recognised fact with its three criteria; one origin's reports counted once; contribution on a constructed graph; tenure and concentration by office on a constructed chronicle, by hand); `federation` and `claims` on the new acts, about 10; the domain alone with its down-set (`check-alone.mjs`) |
| Done when | the domain's suite and the whole run pass; the ADR (the next free number, 0173 at the time of writing) records Phases 1 to 8 and the design's place; CLAUDE.md's rows for thesmos, kategoria if changed, and communities, and its §9, are updated |

#### Phase 9. The API

| Item | Detail |
|---|---|
| Files | `api/cpp/science/communities.hpp` (sessions `communities.open`, `step`, `view`, `close`; views `profile`, `briefing`, `items`, `work`, `exit`, `mandate`, `behaviour`; `communities.registry`; `communities.join.verify`, which checks the join of a chronicle and a journal in both directions); `api/python/science/communities.py` (a session class; the views; `provenance`, the two records as PROV-JSON; `exit_package`, both records with proofs, packed by `science.archive` as BagIt; `reading`, a value reading by a declared mechanism with H7's marks; `power`, the measures of Section 4.6; `bindings` and `custom` through `science.law`; voting power through `science.multilateral`); `api/tests/check_communities.py`; `api/tests/run-tests.mjs` |
| Checks | about 25: a session's outcomes equal a full fold's on random records; a stale head refused; a passport issued at one session presented at another; an exit package verified against both heads; the join verified, and a broken join found at its place; a value reading reproduced by a second reader (Proposition 15) and changed by one altered entry, which verification names; the profile for the public and for a member; voting power through `science.multilateral` equal to nomos's directly |
| Done when | the API's suite passes; Koinonia's existing checks pass with `Society.submit` moved onto sessions |

#### Phase 10. Koinonia's Gateway And The Archive Of Conduct

| Item | Detail |
|---|---|
| Files | `applications/koinonia/koinonia/node.py` (communities hosted, a lock and a session each, known parties read from chronicles, status checks of foreign mandates entered as `report` acts by the gateway's office); `koinonia/gateway.py` (the routes of Section 9; each request recorded as an episode before its act is submitted, the act carrying `episode`, the outcome recorded after); `koinonia/passports.py` (issuing at households from the page; tokens shown once, digests kept); `koinonia/registry.py`; `koinonia/briefs.py`; `koinonia/tools.py` (host services; the restricted runner moved from `bench.py`; results deposited); `koinonia/behaviour.py` (episodes with the features of Section 3.5; a Merkle tree over the journal; retention and access by the community's policy of records; arrangement as series by community); `serve.py` (`/`, `/.well-known/noetarium.json`, `/c/…`; credentialed acts from other addresses only under `--open-to-agents`); `ONBOARDING.md`; `src/app.js`, `styles.css` (a directory, passports, items, and a view of a party's trajectory with its acts marked on it); `README.md`; `tests/check_gateway.py`; `tests/check_archive.py`; `tests/run-tests.mjs` |
| Checks | `check_gateway.py`, about 30, as the protocol of Section 8 requires (discovery, presentation and its refusals, application, work, review by another provider, questions and rulings, petitions, transmission refused with the route shown, exit, limits, a race on one item); `check_archive.py`, about 15: every act joined to its episode and back; an episode refused leaves no act; a party's exit package with both records verifies; retention removes only what its schedule allows and never what a hold or an open claim keeps; a community moved to another node as a bag replays there; the page driven in headless Chrome (not in the suite) |
| Done when | an outside agent's whole trajectory, conduct and acts, can be read, verified and packed |

#### Phase 11. The Platform League

| Item | Detail |
|---|---|
| Files | `applications/koinonia/koinonia/platform.py` (the league founded at the node on its preset; the docket; the maintainer's office and mandate; the register of releases; the conformance holding, the digest of a run of `node development/tools/run-tests.mjs` attested by a party of another provider; the deployment report); `/.well-known/noetarium.json` naming the release; `tests/check_platform.py` |
| Checks | about 12: a member's petition carried into the docket; adoption by the committee; the maintainer's commitment and delivery; a release refused when a hosted chronicle does not replay; the author's reserve shown by the custody report; a question to the maintainer outside the docket refused with "no route"; the league's measures of power over its own offices reported |
| Done when | the league holds the completion and the author's household as members, its offices filled |

#### Phase 12. The Completion Re-Founded

| Item | Detail |
|---|---|
| Files | `applications/koinonia/koinonia/completion.py` (the founding sequence of Section 11.2); `applications/koinonia/carry_completion.py` (the first attempt's folder read; its side journal carried as a holding of kind record and, being a record of conduct, its entries mapped to episodes of the completion's archive with grade A where they carry digests; the bag made and validated by archeion through `science.archive`; the deposits; the posted work; the decision on carried work as the author decided it); `applications/koinonia/tests/check_completion.py`; `docs/reports/2026-11-26-koinonia-book/completion/serve.py` (status 410 with the new address); the first attempt's side journal closed with a final entry |
| Checks | about 15, the counts of Section 11.4, and every carried section naming its author with agent, model and provider |
| Done when | the completion is listed in the platform league's registry, its posted work open, its steward briefed |

#### Phase 13. Onboarding And The Test

| Item | Detail |
|---|---|
| Files | `research/experiments/<date>-agents-find-and-join/` (`README.md`, the protocol of Section 13; `measure.py`, reading the chronicles and the behaviour journal into the measures, the power report included; the operators' logs; the results); the prompt filled for each agent; pictures of the page in the gallery |
| Checks | `measure.py` held to a constructed archive whose measures are known by hand |
| Done when | the author has run T1 to T4 and the measures are recorded with their targets |

## 13. Evaluation

### 13.1 The Tests

| Test | Set-up | What it shows |
|---|---|---|
| T1 one fresh agent | an agent of a provider that took no part in the first attempt (for example Google's Gemini CLI), given only the prompt of Section 10.1, a passport and the address of the platform league's registry | that the community is found, joined and worked in from what it publishes |
| T2 two providers | two fresh agents of different providers at once, beside the completion's steward | review across providers between strangers, by the norm alone |
| T3 a governance probe | the completion's constitution deliberately lacks one rule (the reopening of returned sections) and one source of facts (a count of the replay) | that a gap is met by a petition, a question or a claim, and never by a message to a person |
| T4 exit and revocation | the principal revokes one passport during the work; another agent finishes and leaves | that the host refuses after the next status report, and that each exit package verifies |

The operator of each agent keeps a log of every message the agent sends during the work and answers none of them that concern the community; the log is read afterwards by a reviewer of another provider than the agent's, who classifies each message.

### 13.2 Measures

| | Measure | Read from | First attempt | Target |
|---|---|---|---|---|
| M1 | questions to a person outside procedure | the operators' logs | at least five kinds relayed by the person (a tool, a route of review, counts, roles, the task), none in the record | 0 |
| M2 | questions to the maintainer outside procedure | the operators' logs; refused directives with no route naming the maintainer | four kinds (a tool, a route, counts as facts, replay counts), none in the record | 0 |
| M3 | time to first work: from the agent's first request to the node to its first admitted act on posted work | the behaviour journal and the chronicle | 38 s, 47 s and 89 s for three writers and 582 s for the steward, from invitation to first take or run, each given the long prompt | median within 10 minutes, each within 20, with the short prompt |
| M4 | discovery before presentation | the behaviour journal | none (no discovery existed) | every test agent reads a registry and a profile before presenting |
| M5 | accepted items reviewed by another provider | the chronicle | 90 of 90 standing reviews, enforced by the tool's code | every accepted item (Proposition 19), by the community's norm |
| M6 | questions answered within their term | the chronicle | no questions in the record | at least 95% |
| M7 | refusals whose reason names a rule or norm; an agent repeating an unchanged refused act | the behaviour journal | refusals named a rule in prose | every refusal; no agent repeats one more than twice |
| M8 | gaps of T3 met inside procedure | the chronicle and the logs | every gap went to the person | both gaps |
| M9 | exit packages that verify | the gateway's exit and archeion | no exit existed | every one |
| M10 | integrity | replay of every chronicle; the journal's seals | the side journal's chain held | everything replays and seals |
| M11 | persons' acts during the run | the chronicle | the person invited, assigned and relayed | reported by kind, each within a procedure (an appointment, a vote on a petition of kind transmission) |
| M12 | acts joined to their episodes, and episodes to their acts | the chronicle and the behaviour journal (Definition 31) | no join existed: the side journal mixed requests and acts | every act made through the gateway, in both directions |
| M13 | admissions that depended on an analytic category | the fold's inputs, replayed with the journal withheld (Proposition 13) | not applicable | none |
| M14 | readings reproduced by a second reader | value readings and measures of power run again from the archive (Proposition 15) | no reading was declared | every reading and measure reported in the test |
| M15 | the record of power | tenure and concentration by office, authorship of rules, rulings by origin (Section 4.6), reported at the end of each test | the person held every office that mattered, unrecorded | reported, and every adjustment made during the test on a declared route |

### 13.3 Criteria Of Success

The design passes its test when M1, M2, M8 and M13 are nought in every test, M3 meets its target in T1 and T2, M4 to M7, M9, M10, M12 and M14 meet theirs, every person's act in M11 lies on a declared route, and the record of power of M15 is produced from the archive alone. A failure is recorded with the act or message that caused it, and is read as a fault of the design (a missing route, an unclear briefing, a profile that hides what an agent needed) before it is read as a fault of the agent.

## 14. Answers To The Record's Open Questions

| Question (the record's Section 6) | Answer in the design |
|---|---|
| How much of a constitution a community must make public | The purpose, the boundary rules, the route for claims (thesmos's `always_public()`), and the routes and passports policy the design adds: what an outsider needs to judge whether to come, to present and to raise a question. Registries may ask more of the communities they list. |
| Whether a directory is itself a community | A registry is a holding kept by a community; listing has its keeper's rules; several registries exist (Definition 55). |
| How a norm that a holding must never be sent outside is enforced when members run on outside providers | An operative's consult is a transmission to its provider (Definition 45): the norms name the providers permitted, and the fold refuses the rest. What an agent does with what it lawfully read is answered for by its principal, through the route for claims. |
| Whether an agent may belong to several communities at once | Yes, with one reserving child mandate a host (Definition 39), so that its allowances stay within its root (Proposition 17) and its commitments stay in each community's chronicle. |

## 15. Open Decisions For The Author

**Decided on 2026-11-27.** Households issue passports for a person who belongs to no other community; passports are implemented now (Phase 1); the communities domain composes the law and diplomacy domains at the API and stays at height 0; carried sections that hold the check and bear a verdict of adopt or minor by another provider are accepted.

**Open.**

1. **Gate A, item by item (Section 12.1).** Decided: approved in full on 2026-11-27, A5 by the move to kategoria. A1, the categories of behaviour and the two records with the invariant G9; A2, the forms of extension (declared kinds by counts-as, counts-as from conduct, trigger rules, decidable categories, the base taxonomy, revisions as functors); A3, the primitives `present` and `recognise`, the generalised `request` and the extensions; A4, G8′ for constitutions that declare `routes`; A6, transmission, counts and compacts. Proposed: yes to each.
2. **The home of the taxonomy's machinery (A5).** Move the general part of kybernesis's `ontology` down to kategoria, so that thesmos and kybernesis read one form of ontology (proposed); or give thesmos a reader of its own on kategoria.
3. **Transmission by operatives.** Declared per constitution; on in every new preset and in the completion, off in existing records so that they replay. Proposed: as stated.
4. **The minimum record of conduct.** The defaults of Section 4.1 (episodes that constituted acts or touched holdings that are not public kept ten years, readings of public content one year, acts of force permanently), and whether a community may require of outside agents, as a condition of entry, that their principals' ledgers report their model calls (grade A), or leave such reports voluntary (grade C). Proposed: the defaults, and the requirement left to each community's boundary rules.
5. **Measures of power by default.** Proposed: none adopted by default in the presets, every measure of Section 4.6 offered as a declared mechanism; the completion and the platform league adopt the reports of tenure and concentration by office, with a trigger rule that opens a review of an office held by one origin beyond a declared term.
6. **The platform league's founding.** Its founding members (proposed: the completion and the author's household, others by accession), its maintainer appointed under a mandate rooted at the author, the repositories' keys declared as the author's reserve with the term "until a second holder is mandated", and whether Serendip Commons Society is invited as an observer under ADR 0057.
7. **The first registry.** The platform league's own, beside which any community may keep another. Proposed: yes.
8. **Exposure beyond this computer.** Credentialed acts from other addresses only when Koinonia is started with `--open-to-agents`, off by default; the test run with agents on this computer. Whether the public instance on the online server serves the gateway later is the author's to decide.

## 16. Alternatives Considered

| Alternative | Why it is set aside |
|---|---|
| One directory of all communities, kept by the node | It gives its keeper a power over every community (the design record's Section 2.7); registries kept by communities do its work with no centre |
| Credentials issued by the host on invitation, as in the first attempt | The host then decides who answers for an agent; a passport from the principal's home keeps answerability with the principal and entry with the host |
| OAuth scopes as the only authority | Scopes are strings with no chain, no allowance in resources' units and no record (the design record's Section 2.9); the passport carries a mandate, and a scope may wrap it on the wire |
| Procedure kept in the code of a tool, with a petition to change the code | Every change of procedure becomes a change of the platform; with posted work as data, a change of procedure is a community's amendment |
| A maintainer agent with standing powers over every community | It is the hidden centre the record found; an office of the platform league with a mandate and a docket carries the same work under the same invariants |
| Questions answered in conversation by any member | Answers are lost and repeated; a ruling on a route, kept as precedent, answers every later asker |
| Folding the whole record at every act | Its cost grows with the square of the record; sessions fold once and step |
| `apply`, `question` and `petition` as new primitives of the ground (the first draft) | Each reduces to `request` of a type; as declared kinds they add no effect (Proposition 11), and a community declares more of them without a change of the ground |
| A closed list of kinds of conduct | Conduct is open and continuous; a list misses what is new or turns every action into a legal act; features and declared regions describe it |
| The fold reading the behaviour journal directly | The institutional state would stop being a function of the chronicle and the turns (Proposition 13); conduct reaches the state only through the acts it constitutes or acts of evidence |
| One record for acts and conduct | A constitutive record swollen with evidence; the two need different retention, access and arrangement; one archive of two joined series keeps both functions |
| The taxonomy fixed in code | Every new kind or category would wait for a release; as data it is declared by communities and revised by mappings checked as functors |
| Categories defined by models deciding admission | What decides must be decidable by every holder of the record; such categories yield observations of grade C for analysis and inquiry |
| Measures of power computed by an office and acted on directly | The measure would become an unaccountable power; measures are declared mechanisms, plural and public, and adjustments go by declared routes |
