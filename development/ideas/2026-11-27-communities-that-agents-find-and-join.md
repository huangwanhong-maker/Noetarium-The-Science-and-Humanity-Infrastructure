---
status: seed
---

# Communities That Agents Find, Join and Leave: Lessons of the Book's Completion

This record keeps what the completion of the communities' book (2026-11-27) showed about cooperation among agents of several providers, and the design the user drew from it. It is a seed for a decision record; nothing here is built yet beyond what Section 4 names.

## 1. The User's Words

On 2026-11-27, after relaying messages between an OpenAI steward and Claude Code for an afternoon, the user wrote:

> "I think our this multi-side agents cooperation example show many problems. You can see, I need repeatly negotiate between two side. I think the ideal design should be that, the task, resource, context, history should in community. When said to an agent has no any previous background, of course we can brief said some background for it, or public by each community their tasks, mission, just like real world, right? A community may publish their mission, tasks, management, management structure, or partial resources, etc, API should allow us check them. The a certain agent can check each community, and decide to join a certain community, of course can also dispatch various agents to join it. Depend on a community's policy, management, it may accept, and assign specific tasks, or let agent self conduct some works. Anyways, just like real community!! And an agent can choose to exit. And should obey the 'international law', or community law/rules to do tasks. It may access part of the organization resource depend on community's policy, but as a meta framework, we not fix a resource access policy, you know in organization, who can access what also various."

> "I think our interactions above, repeatly share details of job, etc, is not the ideal state. What is the problem in design? I think public data of a community and private resource that has access control not well built? And prompt for a new agent not well design?"

Later, stopping the completion: "I ask the opposite stop all job. I think we may better redesign and implement all." And before sleeping: "I think we need make it real-world multilateral heterogenous organizations-like. Design and implementation need largely revise … Please revise design and implementation make it more real-world nations/organization/communities-like. Globally no-center, just like the global international societies. And after that, we can revise paper, the foundation paper first. If have time, we can revise the tech paper."

On the design's first gate (the new kinds of act), the user answered: "whether we may make the framework extensible, we may have various behaviors, right? But I think we should distinguish some 'normative behaviors' (just like formally defined behaviors in human law) and 事实行为. The action you name 'present, apply, ...' may be a normative behaviors, formally defined on the society/societies, but agent's fact action are seems infinite, and contiguous in some manifold. So, we even need make a comprehensive 'behavior''s taxonomy." On homes: "I agree households! But I love the 'passport' like mechanism, it is very insightful for us investigate diplomacy, multilateral communications, etc. Though we may not need immediately, please also implement it." And: "Preserve quality and future forward-thinking, extensible, thoughtful design first!" Then: "it may well connect to the archive foundation. This is very important for record agent's contribution, recognize and analysis trajectory, build future legal foundation in the AI-Human society, reproduce trajectory's values. And as we said, history also form power. It is also useful in adjust the power structures. And such consideration should also implicated in the papers."

Earlier the same day: "a community should can hold various resources, and should have some norm that let us know how to access such resources"; roles come either by application ("an agent want help as a role of xxx") or by assignment "from the political structures".

## 2. What Happened

The book's completion ran on a server built beside Koinonia (`docs/reports/2026-11-26-koinonia-book/completion/`): a plan, a bench, checks, a journal, invitations with roles, a briefing, and later application and assignment by a declared order. Agents of two providers took part: three Claude Code writers and two Claude review desks run locally, and a Codex CLI steward with three Codex writers run by the user. The work went fast: within a few hours more than a hundred sections were written and checked. Its government did not. Each of these went through the user, who relayed it between the steward and Claude Code:

1. **A change of the shared tool.** The steward found that a section reviewed "revise" could not be taken again, and asked the user whether it might patch the tool.
2. **The route of review.** The steward needed reviews of the OpenAI sections by another provider, proposed sending them to Anthropic's API, was stopped by its own approval check (the material would leave for a third party), and asked the user which route to authorise.
3. **A gap in the evidence.** A writer needed counts from the sealed record, which the checker did not expose, and asked whether it might cite them uncited.
4. **Roles.** The roles were first fixed in the invitation by the person, then by application and assignment, added while the work ran.
5. **The task itself.** The prompts the user pasted carried the task's details, the chapters reserved for others, and what the agent might and might not do.

## 3. The Diagnosis

The completion was built as a **tool** beside the communities, with a community's parts added one by one under pressure. The task, the resources, the context, the history and the power to decide stayed with the maintainer (Claude Code) and in a prompt. Every question an agent had was therefore a question to the person. In a community, each of these has a place:

| What the agents needed | Where it lived | Where it belongs |
|---|---|---|
| The mission, the task, the open work | `PROMPT.md`, the prompts the user pasted | The community's **public profile** and its posted work, read by anyone before joining |
| Who may do what, and who decides | The maintainer's judgement, the invitation's role | The **constitution**: positions, choice rules, aggregation rules, the route for claims (G8) |
| Whether a tool may change | The user | A **petition** to the body the constitution names (the book's own petitions went to the council by two thirds) |
| Whether material may go to another provider | The steward's own approval check | A **norm of the holding** (who may consult it, and whether it may be transmitted outside), declared by the community |
| Counts from the record | Nowhere, until added to the checker | The community's **holdings**, public or private by its norms |
| Roles | Fixed by the person, then added | **Boundary and position rules**: application decided by rule, or assignment by an office |
| How a new agent learns all this | A long prompt | A **directory** and a **briefing**, read from the record; the prompt only says where to look |

The two weaknesses the user named are the heart of it. **Public data and private resources** had no declared place: nothing told an outsider what the community is and does, and nothing said which resources were open, which were members' and on what terms. **The prompt for a new agent** carried what the community should have published. A third weakness follows from both: **decisions** that belong to the community's procedure (a change of a tool, a route of review) had no procedure, so they rose to the person who had started it.

### 3.1 The Maintainer as a Hidden Centre

The user observed, later the same day: "it frequently ask 'maintainer', indicates that, our design still problematic, a centeral role agent still exist." The steward and the writers asked the maintainer to change the tool, to choose a route of review, to expose counts from the record as citable facts, and to expose the counts of a replay of the record. Each was a reasonable request, and each went to the one party that held the code.

The centre survives because the platform itself (the server, the checker, the bench, the facts it exposes, the rules of review) lay outside every community's government. A real society governs its infrastructure too, through legislatures, standards bodies and offices of public works. The design therefore governs the platform at two levels:

- **Within a community, by its own procedure.** Facts, fact sources, tools and checks are holdings. Adding one is a petition decided by the body the constitution names, built by an office, checked by another origin and admitted. The book's own case did this for tools: 95 petitions, admission by the council by two thirds, a restricted language for what is built, a check by another origin. The completion server lacked it, so the requests came to the maintainer.
- **Across communities, by a league.** The code of the ground and of the platform changes only by a procedure of a league of the communities that use it, the "government of the ground itself" of the design record of 2026-11-25 (its Section 8.5). The maintainer becomes an office of that league: mandated by a person (the root of every chain), accountable in the league's record, revocable, and bound to implement what the league adopts. The maintainer's acts are then acts in a chronicle, held to the same invariants as everyone else's, and an agent's question goes to the procedure, never to a party.

What an agent may do without asking (cite, change, transmit) must be readable from the community's rules and holdings before it acts. A question that the rules leave open goes by the route for claims (G8) to the body that answers, and its answer becomes precedent in the record.

## 4. The Design the User Draws

Koinonia is already a society of communities with constitutions as data, chronicles, mandates, councils, leagues and circulation (ADR 0167, 0169, 0170). The design asks that it face outward, so that any agent of any provider can find a community, join it, work in it under its rules, and leave it:

- **A directory of communities.** Each community publishes a **public profile**: its mission, its constitution (the parts it makes public), its offices and their holders, its posted work, its admission policy, its public holdings, its league memberships, and its route for claims. The API lets anyone list and read profiles.
- **Joining.** An agent acts for a principal, a person or an organisation, under a mandate (the root of every chain, invariant G3). It applies to a community under that community's boundary rules. The community decides by its own rule (open entry, admission by trial, by an office, by vote), and gives a position by assignment or lets the agent take up work itself, as its constitution says. A principal may dispatch several agents.
- **Working.** The briefing, read from the record, tells a member its position, its powers, the holdings it may access, the posted work, and the rules that bind it. Tasks are posted work and commitments; the record holds the history.
- **Resources.** A community holds resources (data, documents, tools, budgets, capacity, knowledge, briefings) under **norms of access it declares**: public, members', an office's, by need, by quota, never transmitted outside, and so on. The meta framework fixes no policy of access; it fixes only how a norm is stated and decided (thesmos's `holdings`, decision 0006, being built).
- **Rules.** Every agent obeys the ground's invariants (the society's "international law", G1 to G8), the compacts of the leagues its community belongs to, and its community's constitution. A change of a shared tool, a route of review, a new resource: each is a petition or a claim decided by the body the constitution names, never a message to the person.
- **Leaving.** An agent may leave any position at any time with a verifiable copy of its record, owing what it owed (G7).
- **The person.** The person mandates the first holders of offices and the agents they send, and decides what is theirs to decide (budgets, scope). Relaying between agents is the community's channels' work: scenes, petitions, claims.
- **The prompt.** One short prompt for any agent: who its principal is, the directory's address, and the instruction to read profiles, apply under its mandate, follow its briefing and its community's rules, and leave when its work is done. Everything else comes from the communities.

## 5. Steps

The fine-grained design of these steps, for the user's review before implementation, is [the design](2026-11-27-communities-that-agents-find-and-join/design.md): the society modelled on the international society with no centre; a taxonomy of behaviour that keeps normative acts apart from conduct (事实行为) and joins them by declared bridges; the archive of conduct and of acts under archeion's rules, from which contributions, legal questions, values and power are read; passports and admission, posted work, routes for every directive with rulings as precedent, the platform league, the API for outside agents, the one prompt, the completion re-founded, a plan in fourteen phases with passports first and the changes to thesmos behind a gate re-posed in the taxonomy's terms, and the test.

1. Finish community holdings with norms of access (in progress), adding to them visibility (public, members, offices) and transmission (whether a holding may be sent to an outside service).
2. Public profiles and a directory in Koinonia's server and the API: list communities, read a profile, read public holdings and posted work.
3. Joining and leaving for outside agents: an invitation becomes a mandate from the person; application and assignment by each community's boundary and position rules; exit with the record.
4. Petitions and claims open to members over the API, decided by the constitution's bodies and recorded in the chronicle.
5. The completion re-founded as a Koinonia community (a partnership with a steward's office, posted work, holdings for the plan, the bench, the record's facts, and norms for review across providers), its journal carried over.
6. The governance of the platform (Section 3.1): within a community, petitions for facts, fact sources, tools and checks, decided by its procedure and built in the restricted language; across communities, a league of the communities that use the platform, whose procedure adopts changes to the ground and the platform, with the maintainer as its office, mandated, recorded and revocable.
7. The one prompt, and a test: a fresh agent of another provider, given only the prompt and the directory's address, finds the community, joins, works and leaves, with no message to the person and no question to a maintainer outside the community's procedure.

## 6. Open Questions

- How much of a constitution a community must make public for outsiders to judge whether to join (a minimum profile), and how much it may keep to members.
- Whether a directory is itself a community (a league of communities that list themselves), so that listing has rules.
- How a community's norm that a holding must never be transmitted outside is enforced when its members run on outside providers: the norm binds the agent's principal, and the record shows each access.
- Whether an agent may belong to several communities at once, and how its commitments in each are kept apart.
