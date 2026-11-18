---
status: promoted in part
---

# An Engagement with a City, from Evidence to Action: A Design for the Urban Studio

*Status: seed (2026-11-19); promoted in part the same night to [ADR 0131](../decisions/0131-an-engagement-with-a-city.md), built at the user's word before sleeping ("Not need hesitate time, and make things extensible, flexible, revisable, and high-quality") and awaiting their review. It applies the framework of [From evidence to action](2026-11-18-from-evidence-to-action.md), now the foundation kybernesis ([ADR 0130](../decisions/0130-kybernesis-steering-by-evidence.md)), to the practice it was drawn from: the practitioner programmes of the Tokyo Development Learning Center (TDLC). Its survey, block by block, of the methods, evidence and actions the mission needs is [Evidence, methods and actions for steering cities](../../research/literature/reports/Evidence,%20methods%20and%20actions%20for%20steering%20cities.md). Every name in it is provisional.*

## 1. The Occasion

The user asked to "prepare practical implementation" for the mission of an urban analyst at TDLC: cities that are green, inclusive, resilient and productive; disaster risk management (assessment and mapping, reduction, preparedness, financing); territorial development and rural–urban linkages; post-conflict recovery; land tenure and land information; spatial data infrastructure; tourism; and, above all, the *learning-to-action journey* of TDLC's practitioner programmes, through which Japanese and global practice is carried into World Bank-financed projects: needs identification, collaborative learning, a technical deep dive in Japan, advisory support, community engagement and results capture.

They proposed that it be a plug-in of the Urban Studio rather than another application, asked for a survey of the indexes, methods and evidence that serve the mission, "so that can integratively analysis, and utilize our framework", and asked for a design first. When agreed with, they asked not to be agreed with but to be told what Claude thinks. §2 is that answer.

They then said that the four qualities are only examples. The other missions need their methods too: how risk is estimated (the criticality of dynamical systems among them), strategies for allocating finance, the methods of economic evaluation. A survey of all of it is very difficult. Instead they proposed that every block admit a person's intervention and keep files, and that the trajectory be archived. §4 takes that up. It is the answer to breadth: no survey can be complete, and the framework does not need it to be.

## 2. The Position Taken

### 2.1 A Plug-in of the Urban Studio

The work is a plug-in of the Urban Studio. The reason that weighs most is not the number of applications: the studios already share one host, one page and one builder of engines, so a new studio costs a port, a colour and a recipe, not a codebase. The reasons that weigh most concern the data and the work.

1. **The evidence is already there.** A city's state is estimated from what the Urban Studio's plug-ins compute: lifelines and evacuation, needs, population, indexes across scales, evidence in space and time, time and care, materials and flows, ecology, appraisal. A separate work would copy them or fetch them again.
2. **The unit is the same.** An engagement with one client city is one project: one place, one folder, one journal, the same people.
3. **Integration happens within a project.** Integrative analysis is several plug-ins run on one place and feeding one belief about it. That is possible only where their results meet.

### 2.2 The General Part Is Built Below the Work

kybernesis is general: a city, a river basin, a court, a clinic. What is general about steering is built in the shared layers, so that the Policy Studio and the Law Studio can take it later without copying it:

- in kybernesis, the lens of transfer and, later, policy making as evolution and the constitution (§7.1);
- in the API, a composition of kybernesis (`science/steering.hpp`, `science.steering`) that admits an architecture and its methods and runs a turn (§7.2);
- in the studio host, a contract by which any plug-in's results become evidence, and an archive of every project's trajectory, kept every time (§4.6, §7.3);
- in the visualization layer, the computational graph that already exists (`steering/`), mounted as a view any studio may load (§7.4).

Only what is urban is the plug-in: the four qualities as families of evaluation operators, the indexes, Japanese practices as sources to transfer from, the journey and its first case.

### 2.3 Plug-ins Meet Through Evidence, Never Through Each Other's Files

A plug-in that read nine others' internal files would couple itself to all of them, and that coupling, more than the number of works, is the long-term risk the user named. Plug-ins therefore meet through one contract: a tool may answer *evidence* beside its tables, notes and figures (§7.3). The engagement plug-in reads only evidence, whichever plug-in produced it, and each existing plug-in opts in tool by tool.

### 2.4 The Policy Domain Is Called, Not Duplicated

The Policy Studio already holds theories of change, bodies of evidence and their grading, transfer after Cartwright and Hardie, options and the decision to adopt, reject or study first. The plug-in calls the policy domain through the API (`science/policy.hpp`) and reuses the visualization layer's `policy/` drawings. The Urban Studio remains the home because the Policy Studio frames one question and one decision, while an engagement is one city across many decisions and many qualities.

### 2.5 What Is Left Out

Programme logistics (agendas, lists of participants, travel, schedules) are office tooling, not science, and are left out. In scope are what the journey knows and decides:

- needs identified, as evidence and evaluation;
- a deep dive, as a transfer whose support factors are checked;
- advice, as policy making;
- community engagement, as readings, standards and coded evidence;
- results, as evidence of effects with their standing;
- knowledge products, drafted from the record, in English and in Japanese.

Two things are refused by design: ranking cities by one composite score by default (a composite is made only when asked, with its weights' stability shown), and any statement presented as an official plan of a city or an agency.

### 2.6 When It Would Become a Work of Its Own

Any one of these would justify a separate work:

- its people differ from the analysts' (city partners and task teams reading, commenting and approving, with roles the studio does not have);
- its object stops being a place (a programme across many cities, a portfolio, a comparison of engagements);
- its public deployment needs a different audience from the Urban Studio's.

Until one of these holds, it stays a plug-in.

*Remark (asked again, with the features of §4).* When interventions at every block, files and an archive of the trajectory were added, the user asked again whether the work should now be an application of its own, or the foundations of evidence and archives integrated into the Urban Studio. The answer is neither, as asked.

- **The archive is general.** The features of §4 are not urban: the Law Studio's theories, the Policy Studio's questions and the Archive Studio's projects need them as much. They belong in kybernesis (`record`) and in the studio host, beneath every studio. The Urban Studio receives them with the others, and a new work would only be one more user of them.
- **The urban content needs the Urban Studio's evidence.**
- **The criterion that may soon hold is the second of the list:** a practitioner programme usually brings several client cities together with Japanese ones, which makes its object a programme across places. A programme can first be a project that reads the ledgers and trajectories of several engagements. When the programme level grows its own views and people (a portfolio, partners, a calendar of turns across cities), that is the time for a work of knowledge programmes, standing on the same host and reading the engagements kept in the Urban Studio.

## 3. The Object: An Engagement

An **engagement** is one project of the Urban Studio: a client city, the people who work on it, and the record of one or more turns of the loop. Its record holds, each item with its provenance:

| The record | Its kind in kybernesis | Kept as |
|---|---|---|
| the city's estimated state: quantities by unit and by period, each with its measure and source | Estimate (Belief) | evidence items gathered from steps |
| the readings of open terms, each with its source | Construal (Readings) | readings declared by persons, by schemes, by data, or proposed by a model |
| evaluations by quality: features, needs, problems, risks, assets, each with its standing and measure | Evaluation | findings of declared operators |
| needs taken up, by whom and why | Uptake | journal entries |
| practices considered, each with its mechanism, context and sources | Practice (boundary) | records of practice (§6.2) |
| transfers: a practice read in the city's terms, its support factors checked | Finding (transfer) | lenses (§7.1) |
| options appraised and the decision | Decision, Policy | the policy domain's question |
| outcomes harvested after the advice | Trace, Estimate | evidence items with their contribution |
| knowledge products | Finding, retained | documents drafted from the record |

## 4. Every Block Open to a Person, and the Trajectory Archived

### 4.1 Why

Most of what a practitioner programme knows is not computed:

- a workshop's list of needs;
- an engineer's estimate of a quantity with its range;
- officials' levels on a scorecard;
- a task team's judgement of what is feasible;
- minutes of a meeting in Japanese.

A framework that let only admitted methods fill its blocks would leave most of the work outside its record. One that let anyone overwrite anything would keep no record worth the name. The design takes the course between: every block may be filled by a method or by a person, and the record says, for every value, which, by whom, on what, and why.

This also answers the breadth of the survey. A block with no method yet is filled by a person. A method added later is admitted when it keeps the block's laws, and replaces or stands beside the person's entry. The record shows which blocks were computed and which were judged, and that honesty is worth more than a complete catalogue of methods.

### 4.2 The Interventions

A person acts at a block in one of three ways, and may attach files to any of them.

| Intervention | What the person does | What the record keeps |
|---|---|---|
| **fill** | enters the block's output where no method runs | the entry, its author and role, its reason, its files |
| **amend** | changes a method's output | both the computed and the amended value, the difference, the reason; what follows is marked as resting on the amendment |
| **review** | accepts or refuses a method's output before it passes on | the decision and its reason; a model's proposal passes only through a review |

What is entered is held to what can be held:

- **Its type.** An entry is a value of the block's output type over the declared instance: a belief over the declared states, readings of declared open terms, findings on declared scales. An entry that is not is refused with the reason.
- **Its laws, where the record can check them.** A belief that excludes a state the evidence shows is refused as unsound, whoever entered it. Totality and determinism are a method's laws and do not apply to one entry.
- **Its grade and family.** An entry is of the family *declared* (kybernesis's `Family::declared`). Its grade follows what it rests on: A a structured source, B a text with its passage, C a word from memory.
- **Its authority.** An entry counts as the record's only where the engagement's rules empower that person's role at that block (the framework's rule that a change is made only by one whom the authorising rule empowers). Otherwise it stands as a proposal.

### 4.3 Files

Files of every kind (documents, tables, photographs, recordings, maps, minutes) are kept as holdings, under the SHA-256 of their bytes, with their custody, by archeion's rules, as the evidence plug-in and the Archive Studio already keep theirs. An entry names its files by digest. A file changed on disk is found when it is next used, and the entries resting on it are marked. A passage cited from a document is checked to stand in it, as the Law Studio checks quotations.

### 4.4 The Trajectory

The **trajectory** of an engagement is the record of its loop: turn after turn, each block's entry with its inputs (by digest), its method or person, its intervention, its outputs, their standing and measure, and its files. It also records the version of the architecture each turn ran under, so that revisions of the framework itself are part of it. Every entry is sealed in archeion's hash-chained journal.

What can be done with a trajectory:

- **replay:** the computed entries recomputed from the recorded inputs and the persons' entries give the same values (the law of determinism); a difference names the method whose version changed;
- **branch:** an alternative taken from any turn (another option adopted, another reading of a term, another standard) is kept as a branch beside the main line, never over it, and branches are compared;
- **lineage:** any finding traced back to the evidence, the persons and the files it rests on, and given as W3C PROV;
- **audit:** the journal verified, and the first entry that does not seal named.

The trajectory is itself evidence:

- for the next turn of policy making: what was advised, and what followed;
- for knowledge products: a case study is a trajectory told;
- for transfer: a source city's trajectory is the evidence of its practice.

In the user's words of ADR 0127, the archive is kept "for recognizing the trajectory generativity of a system".

### 4.5 Revisiting the Archive

An archive is kept so that it can be read again, and reading it again is how a person intervenes better the next time and finds what was not seen. Revisiting is an operation, not a search through old files:

- **the belief at a turn:** what was believed, on what, when a decision was taken, and what was not yet known;
- **difference between turns:** what changed in the evidence, the readings, the methods and the conclusions, each change named;
- **re-reading:** old evidence read under new readings of its terms, or by a method admitted since, its conclusions compared with what was concluded then, without rewriting the past (the result is a branch);
- **resting on:** every entry that rested on a file, a reading, a method or a person, so that a doubt about one reaches all that it touched;
- **proposals from the archive:** a model may propose insights from a trajectory, of grade C, entering only through a person's review.

### 4.6 Archived Every Time, in the Studio Host

The user asked that evidence and the trajectory be archived every time, reusing the archive system. The archive system already has three parts, and all three are reused:

- **archeion** (the foundation): holdings by SHA-256, the hash-chained journal, Merkle proofs, provenance after W3C PROV, custody, retention with holds, access rules, BagIt packages;
- **the studio host's journal** (ADR 0120), already sealed by archeion's rule, which today records who changed what but not the content;
- **the evidence plug-in's holdings**, already kept by digest with their custody.

What is missing is the content. Today a step run again replaces its earlier results, so the past is lost. The proposal is a **project archive** in the studio host, for every studio:

- every run of a tool keeps its inputs and outputs under their digests;
- every evidence item, every person's entry and every file is a holding;
- the journal seals each, so that nothing is overwritten and every earlier state can be read again.

The rules that make it bearable:

- **Content addressing.** The same output is stored once, however often it recurs.
- **Retention.** Large derived tables may be declared recomputable from their recorded inputs instead of kept: replay reproduces them. Evidence items, persons' entries and files are always kept.
- **Access.** What residents said in community engagement, and any personal data, is held under archeion's access rules and peira's disclosure control, since an archive that keeps everything keeps the sensitive too.
- **Packages.** An engagement's archive, or a part of it, can be packed as a BagIt bag to hand to a client city or an agency, with its digests verifiable by them.

The Archive Studio could later open a project's archive for archival description and arrangement, read only. This links the two works without merging them.

### 4.7 On the Page

In the ring of the journey and in the computational graph, each block shows its state: computed, entered, amended, reviewed, a proposal awaiting review, or stale. Opening a block shows its entries, their files and their lineage, and offers the interventions the person's role allows. A branch is drawn as a second line beside the first, from the turn where it parted.

## 5. Methods by Block, the Qualities as Examples

The survey is organised by block, because a method, a kind of evidence or an action attaches to a block, not to a sector. For every mission it asks the same eight questions:

1. **Collection:** by what designs and mechanisms evidence is gathered;
2. **Evidence:** what traces exist, with their provenance, licence, resolution and biases;
3. **Interpretation:** which open terms need readings, and from whom;
4. **Estimation:** which states matter, and which are better estimated than observed, by what method and with what uncertainty (risk among them, with the criticality of dynamical systems);
5. **Evaluation:** which operators, on which scales, against whose standards (economic evaluation among them);
6. **Policy making:** how options are made, appraised, allocated and selected (finance among them), and how a practice from elsewhere is judged transferable;
7. **Actions:** the repertoire of interventions and of inquiries, each with its mechanism, the states it acts on, the evidence that would show it worked, its typical risks and the support factors a transfer needs;
8. **Results capture:** how what an action did is shown.

The missions are then examples of what an engagement may declare:

- the four qualities;
- disaster risk management;
- territorial development and rural–urban linkages;
- post-conflict recovery;
- land;
- spatial data infrastructure;
- the promotion and management of tourism;
- the knowledge programme itself.

Economic evaluation and allocation run across all of them. The survey cannot be complete. §4 is what makes that acceptable.

### 5.1 No Quality Is a Number

"Green", "inclusive", "resilient" and "productive" are thick concepts. Each is a family of evaluation operators $J_k(b;\iota,\sigma)$ applied to the belief $b$ about the city, under readings $\iota$ of its open terms and standards $\sigma$ held by someone (framework §3.4). An operator states:

- the quantity it reads from the belief (an index of the catalogue, by its id, `domains/urban-science/indicators/`);
- its open terms, each with its admissible readings and their sources ("within walking distance" as 400 m, 500 m or 10 minutes; "affected" by a hazard as Sendai's or a national definition);
- its scale type (nominal, ordinal, interval, ratio, absolute), which decides what comparisons are meaningful (a scorecard's levels are ordinal: they may be ordered, not averaged);
- the standards it judges against, each held by someone: a normative standard (WHO's guideline for PM2.5, a Sendai target), a comparative one (peer cities), the city's own target, or a felt and expressed standard from community engagement;
- the kind of finding it yields: a feature, a need, a problem, a risk or an asset.

Its answer is a finding with two parts: a **standing** (clear, arguable or untenable over the admissible readings, with the readings it turns on) and a **measure** (an estimate with its interval). A need that is clear under every reading of its terms and arguable only under one standard says so.

### 5.2 The Families

The survey (its §1 and chapters A–H) lists, for every mission, the operators worth declaring first. Their open terms are where readings are needed, and their scale types say what may be compared. The pattern, mission by mission:

| Mission | First operators | Where the readings are | Scale |
|---|---|---|---|
| green | PM2.5 weighted by population (SDG 11.6.2); waste in controlled facilities (11.6.1); open public space within reach (11.7.1); heat exposure in person-days | the city's boundary, modelled or monitored; "controlled" on a ladder; 400 m on the network or 3-30-300; surface or air temperature | ratio; a ladder ordinal |
| inclusive | slum and informal households (11.1.1); housing cost burden; convenient access to transit (11.2.1); tenure security, documented and perceived (1.4.2, Prindex) | each of the five deprivations; the income concept; 500 m or 1 km, a "recognised stop"; which documents count | nominal per household, ratio over persons; ordinal for perception |
| resilient | expected annual loss and probable maximum loss as a band over admissible models; well-being loss; the share able to reach safety within the warning; early-warning coverage | return period, damage functions kept as a set; utility's elasticity; walking speed, departure, "safe site"; "reachable" or "covered" | ratio; Scorecard levels ordinal |
| productive | debt service ratio; own-source revenue; jobs reachable within a time; land value captured as a share of uplift | which liabilities and revenues count; 30, 45 or 60 minutes; the counterfactual uplift | ratio, judged against thresholds (Japan's 18/25/35 %) |
| territorial | Degree of Urbanisation; functional urban areas; rural market access | the version and thresholds; 15 % commuting, or estimated where no commuting data exist | nominal from ratio thresholds |
| land and spatial data | cadastral coverage; LGAF and IGIF levels; the roll's coverage and collection efficiency | parcels or area, surveyed or registered; the edition and panel | ratio; ordinal |
| post-conflict | damage, loss and needs; returns and services restored; recovery time | replacement cost or changed flows; "displaced", "returned", "safe to return" | ratio; survival |
| tourism | intensity and density; residents' satisfaction; direct value added kept apart from the total contribution | registered or short-term lodging, peak or mean; the items of the survey | ratio as a band; ordinal (latent) |
| knowledge | uptake in operations; a contribution claim; persistence of action-plan items | dialogue, design, financed component, indicator moved; contribution or linkage | ordinal; standing with a posterior; survival |

Every one of these has open terms whose readings are someone's, and several are ordinal. Both points confirm the framework's choice: a standing over readings, a measure beside it, no averaging of ordinal levels.

### 5.3 Where the Qualities Meet

The survey's §2 finds eight places where the missions meet. The five that matter most for a first engagement:

1. **Density is productive and green, and raises exposure.** The productive, green and resilient operators are to be applied side by side, never one alone. Japan's 2020 amendment, which excludes red zones from areas where residence is induced, already joins their readings.
2. **The fiscal loop runs against tenure.** The uplift that value capture recovers also displaces tenants. Readjustment's consent counts landowners and leaseholders of land but not building tenants, the support factor that matters most when it is transferred. Residents retained are to be tracked beside revenue captured.
3. **Tourism runs against housing and resilience.** Visitors are an exposed population that no register holds, and short-term lodging raises rents.
4. **Asset losses and well-being losses rank investments oppositely.** The poorest fifth bear a small share of asset losses and nearly half of well-being losses (Hallegatte et al.), so the two are held as parallel evaluation spaces, with distributional weights reported as a standing.
5. **Ordinal scorecards cannot be combined into one quality.** The Scorecard, the City Resilience Index, LGAF, PEFA, IGIF and residents' satisfaction are ordinal, and the Scorecard itself forbids comparison across cities. There is no default composite: counts at each level, non-compensatory rules, or robust classes.

*Remark.* Informality escapes every mission's evidence in the same places. So one enumeration of informal settlements serves several missions at once, and its worth as an inquiry is to be judged across them.

### 5.4 The Repertoire of Actions

The policy block maps a belief to an action, so the space of actions must be declared as carefully as the space of states. An **action** is a record: its kind (an investment, a regulation, a price or tax, a service, a programme, a transfer of practice, an inquiry), its mechanism as a theory of change, the states it acts on, the evidence that would show it worked, its typical risks, and the support factors it needs. An inquiry is an action whose effect is on the belief, not on the city; its worth is judged before it is made by the value of the information it would bring (the policy domain's EVSI, tyche's tests worth running within a budget).

The survey's §3 holds a repertoire of sixty actions across the missions, each with its mechanism, the states it acts on and its support factors. Those most useful for a first engagement are these:

- **Inquiries:**
  - diagnostics (a municipal finance self-assessment, a Scorecard workshop with its justifications recorded, City Scan, LGAF, IGIF);
  - surveys and enumerations of those whom the records miss;
  - pilots with a registered protocol.
- **Knowledge actions:** a technical deep dive with an action plan and follow-up funded in advance; expert dispatch within a project's preparation.
- **Interventions:**
  - early warning, heat alerts among them;
  - evacuation planning with drills timed against the model's bound;
  - land-use regulation in hazard zones;
  - risk layering;
  - property tax reform;
  - land value capture tied to transit;
  - land readjustment with an explicit rule for tenants;
  - in-situ upgrading with tenure;
  - visitor levies earmarked, with residents as judges.

Each is a record of the policy block's action space. Its support factors are what a transfer checks.

## 6. The Journey Mapped onto the Loop

### 6.1 Stage by Stage

| The journey | What a person does | What the plug-in computes | Block of the loop | Existing parts |
|---|---|---|---|---|
| needs identification | states the task team's knowledge gaps; chooses operators and readings; runs the plug-ins on the city | the city's belief from the evidence items; each operator's finding with standing and measure; needs proposed by search | Collection, Evidence, Estimation, Evaluation; Uptake in PolicyMaking | the plug-ins' tools; needs (search, standings); scales (estimates withheld where not shown) |
| collaborative learning | gathers candidate practices with peers and partner cities | practices whose mechanism answers a need taken up, ranked by how many of their support factors the city's belief shows | the boundary Practice; variation in PolicyMaking | records of practice (new); the policy domain's theories of change |
| the technical deep dive in Japan | studies a practice where it works; meets those who run it | the practice's source city read in the same terms as the client; the lens $(f, g)$; each support factor checked against both beliefs; "will it work here" with its standing | transfer (new in kybernesis), Evaluation | the policy domain's `transfer`; peira's transport; tekmerion's warrant |
| advisory support | frames options with the task team | options appraised on several qualities at once; the decision to adopt, reject or study first; risk financing layers where relevant | PolicyMaking, Policy, Decision | the policy domain's `options` and `adoption`; nomos's `appraisal`; disaster risk's financing |
| community engagement | holds sessions; records what residents say | coded passages as evidence; felt and expressed standards; readings proposed by residents | Interpretation (Words to Readings), standards (Constitution) | zetesis's coding with the agreement of coders; tekmerion's construal |
| results capture | gathers outcomes after the advice | outcomes as evidence items; their contribution to the advice with its standing; the practice retained with its lineage | Collection after the decision; PolicyMaking's retention | peira's quasi-experiments and interference; tekmerion's reviews |
| knowledge products | writes the brief, the case study | a draft from the record in English and in Japanese, every claim tied to its finding; a model's text marked as a proposal | (outside the loop: a view of the record) | the visualization layer's documents; `science.models` |

### 6.2 Records of Practice

A **practice** is a record: its mechanism as a theory of change in the policy domain's written form, the context it worked in (its source city's state, in the catalogue's terms), the support factors it needs, the actions that realise it, its evidence graded A (a structured source), B (a text with its passage) or C (a language model from memory), and its sources with their licences. The format is general and belongs to the policy domain; the urban records are data and belong to urban science, as the catalogue of indexes does. Japanese practices come first because the mission asks for them; global ones follow in the same form.

### 6.3 The Page

The page is the city, not a list (the user's rule for studios). The four qualities are lenses over the city's map, each drawing its findings at their places with their standing told by a mark and a word. The loop is a ring around the city whose stages light as the engagement advances, opening in place.

A transfer is drawn as two cities facing each other: the source city at the left in its own terms, the client city at the right, the interpretation $f$ and the realisation $g$ as threads between their places, each support factor a plank laid or missing between them (the Policy Studio's crossing to a place, extended to two maps). The computational graph of the turn is one view among them.

## 7. The Layers

### 7.1 kybernesis: Transfer, the Record, Then Evolution and the Constitution

- `record` (first): the trajectory of §4: entries by block with their method or person, the three interventions with their checks and authority, files by digest, turns under the version of the architecture they ran under, replay, branches, lineage as W3C PROV; sealed by archeion's journal, which kybernesis would then declare.
- `transfer` (first): a lens $(f, g)$ between two declared systems; the commuting square checked within a declared tolerance on finite instances; support factors as the conditions under which it commutes; the standing of "it will work here" over the readings of both systems' terms.
- `evolution` (second): policies varied, selected on estimated effects against what would have been, retained with their lineage, on heuresis's `evolution` with the needs protocol.
- `constitution` (later): versions of an architecture, the rules of who may change what, records migrated along a change.

### 7.2 The API: A Composition of kybernesis

`science/steering.hpp` and `science.steering`: an architecture read and admitted, methods admitted to its blocks, one turn run on an instance, the record as JSON; and an engagement's turn composed from evidence items, operators, readings and standards, the policy domain's question, and a lens.

### 7.3 The Studio Host: The Project Archive, and Evidence as a Kind of Result

The project archive of §4.6, for every studio: runs kept by digest and never replaced, holdings shared by all of a project's plug-ins (the evidence plug-in's moved down to it), the journal sealing content as well as changes, retention and access by archeion's rules, bags for handover.

A tool may answer results of a fourth kind, `evidence`, beside tables, notes and figures: items each naming a quantity (an index of the catalogue by id), its value and interval or its distribution, its unit and scale type, its support (a place or a unit, and a period), the readings it was computed under, and its provenance (the step, the tool, its sources). The host keeps them with the step as it keeps every result, marks them stale when the step is, and gathers a project's items into a ledger any plug-in may read. Each existing plug-in opts in tool by tool, starting with lifelines and evacuation, needs, population and the indexes across scales.

### 7.4 The Visualization Layer

The computational graph (`steering/`) mounted as a studio view; the two cities facing each other; the ring of the journey; the findings of a quality on a map with standing told apart from measure.

### 7.5 The Plug-in

`applications/urban-studio/plugins/engagement` (name provisional): routes for the engagement's record; views (the city under its qualities, the transfer, the ring, the graph); tools (gather the belief; evaluate under the declared operators; rank practices; check a transfer; appraise options; draft a brief); templates, one per case.

## 8. The First Case

**Freetown learning from Miyako: leaving a hazard's reach in time.** Both places are already templates of the lifelines and evacuation plug-in, with their hazards, people, roads and refuges.

- **The practice:** Japan's tsunami preparedness as Miyako lives it: hazard maps, designated evacuation sites, warnings, drills, and leaving at once.
- **The client city:** Freetown, with its floods and landslides.
- **The lens:** $f$ reads Freetown in Miyako's terms (a flood's onset in place of a tsunami's arrival, the warning's lead time, the reach of dry ground and of refuges); $g$ realises Miyako's actions in Freetown (designated sites as Freetown's schools, places of worship and high ground; drills; warnings by the means Freetown has).
- **Its support factors**, each checked or declared:
  - refuges above the flood within walking reach (computed);
  - a warning longer than the walk (computed against a declared lead time);
  - people who leave when warned (declared, from 2011's departures, until evidence of Freetown's own is given);
  - routes the hazard does not cut (computed).

Some factors are shown by the belief and some only declared, so "it will work here" is expected to stand as arguable, turning on whether people leave when warned, which tells the task team what to find out first. That is the use of the framework. As with the Miyako study, the case is an analysis of public data, not an official plan of either city.

A second case, for the productive and green qualities, would follow, chosen from the survey (§5).

## 9. The Order of Work

1. This design and its survey, discussed with the user (now).
2. kybernesis's `record` and `transfer`, with checks; then the API's composition.
3. The project archive and the evidence contract in the studio host, and the first plug-ins opting in.
4. The plug-in on the first case, driven in headless Chrome, with pictures in the gallery.
5. Knowledge products in two languages.
6. Policy making as evolution, and results capture over several turns.

## 10. Questions for the User

1. **The plug-in's name.** `engagement` is provisional; the user names works.
2. **Where records of practice live.** Proposed: the format in the policy domain, the urban records in urban science beside the catalogue of indexes.
3. **The first case.** Freetown learning from Miyako, or another pair.
4. **Languages.** Knowledge products in English and Japanese from the start, or English first.
5. **Our online server.** Whether the plug-in should run there when built, with the Urban Studio on 9204.
