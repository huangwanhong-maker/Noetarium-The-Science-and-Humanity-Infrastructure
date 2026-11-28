# The Holdings of Communities and the Norms of Access

*Status: draft (2026-11-27). The presets' holdings and norms ([presets/data/](../presets/data/)), their checks ([check_presets.cpp](../presets/tests/check_presets.cpp), [check_lending.cpp](../human-ai/lending/tests/check_lending.cpp)), on thesmos's module `holdings` and its fold ([decision 0006](../../../foundations/thesmos/development/decisions/0006-holdings-and-the-norms-of-access.md); the theory note's §9). Every holding and party named here is invented.*

## 1. The Request

The author asked that a community can hold various resources, and have norms that say how they are reached. From the same discussion: an agent that enters a community should learn from the community what it should do and the context of the task; the context lives in the community's record, never in an agent; a leader is an office that reads the same record.

## 2. What the Ground Gives

thesmos holds what is general (Definitions 10 to 15 of its theory note). A holding has a kind, a custodian (an office or a party), a provenance by archeion's digest, a container if it is kept within another holding, and, when metered, a unit and a quantity. A norm of access has an attribute, a deontic, an aim, an object, conditions and an or-else. The most specific norms decide, by object first and then by attribute; a prohibition prevails at equal specificity; what no norm permits is refused unless the constitution opens its kind. The fold admits an act of access only under that decision, takes a draw from what is available, charges it along the mandate's chain in the holding's unit, and records who did what, how much, for what purpose and by which norm. A holding is public, members' or an office's, and a norm says whether what is reached may be sent to a service outside the community (a permission silent on it sends nothing outside). The briefing reads the record for a party: the mission, what it may do with each holding it may know of and by which norm, its duties, the open work. The profile gives what an outsider may know of a community without joining: its mission, admission policy and route for claims always, and what its policy of records makes public.

The domain declares what each order holds and how it is reached. Nothing in a preset is any real community's rule.

## 3. The Presets

| Preset | Holdings | Norms of access, in words | What the order means by them |
|---|---|---|---|
| commons | a shelf of data, kept by the steward, under review by another origin, public; a guide, of kind knowledge, public | anyone who holds a position may consult the shelf, and send it anywhere outside; contributor may deposit to the shelf; the depositor may modify the shelf; the custodian may withdraw from the shelf; knowledge is open to consult | an open commons: whoever takes part reads what is shared, contributors add to it, and no deposit stands until someone of another origin has checked it |
| guild | a bench of tools of its method, kept by the steward, under review, its members'; who holds its positions shown to members | member may use the bench for analysis or replication; aide may use the bench for replication; anyone who holds a position may consult the bench; member may deposit to the bench; the depositor may modify it; the custodian may withdraw from it | a selective community of a method: members work and replicate on the bench, their agents only replicate, applicants learn the method by reading it; no norm lets the bench be sent outside, so its method never leaves the guild |
| cooperative | a writing pool, of the members' writing budgets, in words, its members'; its custody of means shown to its board, its posted work to its members | member may deposit to the writing pool; member may use it within 2000 a round; hand may use it within 1000 a round under a mandate of use on writing; candidate may use it within 500 a round by need under pressing need; the custodian may lend it | a pool by quota: members put in and draw a share a round, their hands a narrower share under a mandate, candidates only when another origin attests their need |
| polity | its records, kept by the magistrates, public | anyone may consult the records, and send them anywhere outside; magistrate may deposit to and modify them; juror may deposit a finding to them; anyone must not withdraw them | public records: open to all, kept and amended by its offices, never withdrawn |
| league, first league | a plan and a briefing, kept by an office (the secretary, the notary), its members' | a party whose origin holds member may consult the plan and the briefing, and send them to its provider; envoy may consult the plan; anyone who holds a position may consult the briefing; the office may modify the plan, and must keep the briefing current | a community of communities: every member's agents read what the league plans and asks, and an office answers for keeping it current |

*Remark.* In the cooperative a member's hand holds the position hand itself and reaches the position member only through the member who answers for it; the norm addressed to the hand is the more specific (thesmos's Definition 12), so a hand draws within its own quota of 1000 and never within its member's 2000. Both quotas are kept by the member who answers, so a member and its hands share them.

*Remark.* Transmission matters because members run on outside providers. In the commons, the polity and the leagues, a norm says what may be sent outside, and an agent of a provider reads by it; in the guild no norm says so, and an agent that would send the bench to its provider is refused, the norm named.

## 4. The Briefing of an Agent That Enters

A lent agent that joins the commons as a contributor is briefed from the record before it acts ([check_lending.cpp](../human-ai/lending/tests/check_lending.cpp)): the commons' mission, the shelf it may consult by the norm "the shelf consulted", the guide open to all, and the deposit the commons would permit a contributor, marked as uncovered by its mandate, which lends it for reading and checking and not for depositing. It reads the shelf; the access is recorded with the sponsor as the party that answers for it. A deposit by a contributor waits on the shelf until a reviewer of another origin checks it.

## 5. The Profile and the Directory

An agent that has not joined reads a community's public profile: the commons shows its mission, how each position is entered, its route for claims, its offices and holders, and its shelf and guide with what is deposited on the shelf; the guild keeps its holders and its bench to its members; the cooperative keeps its custody of means to its board and its pool and posted work to its members. Koinonia lists the profiles of a society's communities as a directory (`Society.directory`, the route `/api/societies/<name>/directory`), the first step of the author's design *Communities That Agents Find, Join and Leave*.

## 6. What Is Not Built

Joining by outside agents through the directory; a directory that is itself a community; norms of access read in words by the grammar of `statements`; amendments that add a holding or a norm; a loan returned by an act; holdings carried between communities by crossings; retention of holdings by archeion's schedules; custom, the routes by which a norm of access forms from practice (the planned module `custom`).
