# 0173. Communities of Every Order, Registries, Organisations and Trajectories

- **Status:** Accepted, provisionally; amended by [ADR 0174](0174-sessions-views-and-the-archive-of-conduct-at-the-api.md)
- **Date:** 2026-11-27
- **Amends:** [ADR 0167](0167-thesmos-and-the-communities-domain.md)
- **Decided by:** the user, who approved in full on 2026-11-27 the gate of the design *Communities That Agents Of Any Provider Find, Join, Work In And Leave* ([its record](../ideas/2026-11-27-communities-that-agents-find-and-join/design.md), with [the taxonomy's second version](../ideas/2026-11-27-communities-that-agents-find-and-join/taxonomy-of-behaviour-v2.md)), built since in thesmos's decisions 0007 to 0013; Claude, carrying out its Phase 8 and the four rows of its Section 7.8 left open, by the rule its maintainer set (what has an effect in the fold goes into thesmos; a label with no effect stays data the profile carries and registries read), recorded as thesmos's decision 0014

## 1. Context

On 2026-11-27 the user asked that agents of any provider find communities, join them, work under their rules and leave, in a society "real-world multilateral heterogenous organizations-like", "globally no-center, just like the global international societies"; that normative acts be kept apart from conduct in a taxonomy of behaviour; that the archive record contributions, trajectories and power ("history also form power"); and that passports be built ("I love the 'passport' like mechanism … please also implement it"), a lone person's home a household ("I agree households!"). The design answering them runs in fourteen phases. Phases 1 to 7 change thesmos and kategoria. Phase 8 is the communities domain. Phases 9 to 13 are the API, Koinonia's gateway, the platform league, the completion re-founded, and the test. This record states the place of the design and records Phases 1 to 8.

## 2. Decision

1. **The design's place.** The design is the record of a society on the model of the international society. Communities of many orders are each sovereign in their own record. Registries are kept by communities, and no directory stands above all. Passports are issued by a principal's home. Recognition comes in levels, compacts are ratified by each party's own rule, treaty organisations are founded by compact, and the platform is governed by a league of the communities that use it. Every directive has a route. Behaviour is kept in two joined records, the chronicle constitutive of acts and the behaviour journal evidential of conduct. Its Sections 2 to 4 are made exact in the domain's theory note [an-international-society.md](../../domains/communities/theory/an-international-society.md). Thesmos's theory note (§10 to §17) holds the ground.
2. **Phases 1 to 7, in the foundations** (built and recorded in each library's own decisions):
   - Phase 1, passports and the household (thesmos's decision 0007);
   - Phase 2, the machinery of a taxonomy moved down to kategoria's `ontologies` (its decision 0005; kybernesis's 0007);
   - Phase 3, the taxonomy of behaviour with the invariant G9 (thesmos's 0008);
   - Phase 4, presentation and recognition (its 0009);
   - levels of description and epistemic behaviour by agreement, with G10 (its 0010);
   - Phase 5, items, routes and rulings, with G8′ (its 0011);
   - Phase 6, posted work (its 0012);
   - Phase 7, transmission by operatives, counts by provider and by weight, and compacts (its 0013).
3. **Orders, terms and seats, in thesmos** (its decision 0014). A community's order is a label: the profile shows it and registries read it, and no fold reads it. The registries a community is listed in and its envoys as objects are data of its part `external`. A position's term is shorthand for the trigger rule that ends a position held too long. A body's seats are read by the fold, which refuses every act that would pass them. A step with trigger rules no longer copies the whole state.
4. **Presets of every order.** Five presets join the seven, each declaring that operatives transmit to their provider:
   - `organisation`: a legal person roots its staff's mandates, a board of five seats sits for a term, and staff are assigned and their tasks posted;
   - `foundation`: trustees hold an endowment and award grants as posted work, with applicants admitted while a grant is open;
   - `treaty-organisation`: founded by an open compact. An assembly adopts the budget by a double majority of members and weight, and a council of five elected seats appoints the secretary-general. Calls for contributions and their payments are declared kinds, and staff are lent by members;
   - `platform-league`: the design's Section 6.3, with a committee of seven seats and a maintainer held for a term. Implementation is posted work reviewed by another provider, and listings are deposited in its registry;
   - `completion`: the design's Section 7.9.10, with its sections, reviews, overview and audit as kinds of work.

   The seven of before gain their order, from whom they accept passports with an office of status that may report and recognise, and a route for every directive. The commons and the guild gain kinds of work, the polity a visitor's position held for a term, and the league a registry. Their transmission by operatives stays off, so that their records replay as they were.
5. **New modules of the domain:**
   - `registries`: listings `noetarium.listing/1` made from a community's record and verified against its own profile document and a trusted notary's witness of its head (current, stale, unverified, faulty); a registry's entries in its keeper's chronicle, delisting with reasons; search across several registries;
   - `organisations`: a treaty organisation's accounts read from its record. They hold the budget adopted, calls against a scale with each quota exact, payments credited to the call named and then in order with a credit carried, and arrears. Members in arrears beyond declared periods are reported after the Charter's Article 19 as a declared mechanism. Core is set against earmarked, staff are counted by origin, and a withdrawal leaves its contributions owed;
   - `trajectories`: facts about conduct recognised under a standard with their three criteria, traces checked, one origin's reports counted once and weighed by tekmerion. Contribution is read over archeion's graph of provenance with exact shares and taint. The measures of power are seven declared mechanisms with their digests, none adopted by default.
6. **Extended modules.** `federation` reads recognitions from each community's chronicle, generally or for a matter, and finds the parties of a compact open to any community in the records. `claims` reads every item's route with the five complements of a claim's, and names the items no one may answer.
7. **Foundations.** The domain declares tekmerion, nomos and tyche beside its seven. No domain is added, and its place in the order is unchanged.

## 3. Alternatives Considered

- **A directory of all communities kept by the node.** Set aside, as the design sets it aside: its keeper would hold a power over every community. A registry is a holding a community keeps. A listing is verified against the community itself (the theory note's Proposition 2).
- **A profile document that carries the head.** Set aside: every act would make every listing stale. The document holds what changes only by amendment or by public holdings, and the head is checked apart against a notary's witness (Proposition 4).
- **Assessed contributions as posted work, or as items on a route.** Set aside: a call is a request whose matter the declared kind fixes, and a payment is a deposit. Arrears are a view of the record, and no route or taking is needed for a member to pay.
- **A sanction for arrears applied by the module.** Set aside: the Charter's Article 19 is a declared mechanism the module reports. A sanction is the assembly's decision.
- **A council of envoys as a body of the assembly's position with seats.** Set aside: a body is the set of holders of its positions, so a council of the assembly's position would be the assembly. A council elected from the envoys is a position of its own (`councillor`).
- **Ending the maintainer's office by the assembly's decision** (the design's "revoke office"). Set aside: no effect primitive of the ground ends a position by decision. The maintainer is held for a term.
- **A trigger rule that opens a review of an office held too long** (the design's Section 4.6 and its open decision 5). Set aside: opening an item is not an effect the ground produces by law. The measure "long offices" reports such holdings, and a community acts on it by a declared route.
- **Measures of power adopted by default.** Set aside, as the design proposes: each is a declared mechanism identified by its digest, and a community adopts those it acts on by its own rule.

## 4. Consequences

- **Checks.**
  - thesmos: 345 (15 new);
  - the communities domain: 130 (53 new): the presets 9 (every preset admitted, what each declares, a sample party's briefing in each new preset giving the verdicts its norms state), `registries` 11, `organisations` 11, `trajectories` 15, `federation` 2, `claims` 5; each by hand or against a count made apart;
  - the domain alone with its down-set (130 of 130), the API's checks of the domain (42) and Koinonia's (78) on the twelve presets, and the whole run on 2026-11-27: 21,195 checks in 85 suites, all passing, and every domain alone with its down-set.
- **Parts of the design adjusted on building**, each recorded in the presets, the theory note and thesmos's decision 0014:
  - the completion's kinds of work name the office that enters their check;
  - its route for transmission is a decision of the principals, since a route's `by` names a decision;
  - the host operator's power of deployment is left out, since a power is held by a position of the community;
  - its route terms are numbers of turns;
  - its conversions, receiving capacity, circulation and preserved properties are declared, as admission asks;
  - where operatives transmit to their provider, a draw on a metered holding is a reading that reaches the drawer's provider (thesmos's Definition 61), so a norm silent on transmission refuses every agent's draw: the design's "runs drawn by quota" would have refused every writer its bench runs, and the new presets' draws on budgets and capacities name `"transmit": "any"`.
- **Not built.** These wait for Phases 9 to 13: the API's sessions and views; Koinonia's gateway, its archive of conduct and its platform league; the completion re-founded with the first attempt carried; and the test with agents of other providers. A trigger rule that opens an item, a decision that ends a position, a body that is a subset of another, and listings fetched over the network all stay outside the domain. Fetching is a work's.
