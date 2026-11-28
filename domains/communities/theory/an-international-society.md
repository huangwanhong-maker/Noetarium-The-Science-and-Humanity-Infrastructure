# An International Society

*Status: draft (2026-11-27). The presets of every order ([presets/data/](../presets/data/)), and the modules [registries](../registries/include/communities/registries/registries.hpp), [organisations](../organisations/include/communities/organisations/organisations.hpp), [trajectories](../trajectories/include/communities/trajectories/trajectories.hpp), [federation](../federation/include/communities/federation/federation.hpp) and [claims](../claims/include/communities/claims/claims.hpp), with their checks; on thesmos's decisions 0006 to 0014 and its theory note. Phase 8 of the design* [Communities That Agents Of Any Provider Find, Join, Work In And Leave](../../../development/ideas/2026-11-27-communities-that-agents-find-and-join/design.md), *whose Sections 2 to 4 this note makes exact; ADR 0173. Every community and party named here is invented.*

## 1. The Request

The author asked for a society "real-world multilateral heterogenous organizations-like … more real-world nations/organization/communities-like", "globally no-center, just like the global international societies", in which "no centering entity" governs all communities, though "we can make something centering community, just like 'international organization' and issue some common laws". Its records are to serve "value return; evidence for legal and goverance issues", and since "history also form power", the record is to show power and serve to adjust it. The design answered with a society of communities on the model of the international society. This note states what of that design the domain computes, with definitions kept apart from results and each result proved or cited.

## 2. The Society and Its Absence of a Centre

**Definition 1 (society, community).** A *society* is a finite set $\mathcal{S}$ of communities. A community $C$ is a pair $(K_C, R_C)$ of a constitution $K_C$ admitted by thesmos's nine conditions of form and a chronicle $R_C$, a sequence of acts sealed in archeion's journal. Its *state* $\sigma_C = \mathrm{fold}(K_C, R_C)$ is what thesmos's fold computes; the compacts $C$ has ratified are records of $K_C$ (thesmos's Definition 65).

**Definition 2 (sovereignty).** $C$ is *sovereign* when whether an act $a$ is admitted in $C$ is a function of $\sigma_C$ and the envelope of $a$ alone.

**Proposition 1 (every community on the ground is sovereign).** Every community of a society is sovereign; in particular no act of another community, no registry and no node enters the admission of an act in $C$, except through an act $C$ has admitted (a recognition, a report of its office of status, a ratification).

*Proof.* The fold's step reads the state and the act's envelope (thesmos's Proposition 1), and the state is a function of the constitution at the founding and the admitted acts, its parties declared by acts (thesmos's Proposition 21). What $C$ knows of another community (its heads, its recognitions, the status of its mandates) is part of $\sigma_C$ only as the acts of $C$ that entered it (thesmos's Definitions 39 and 40). ∎

**Definition 3 (order).** A constitution may declare its *order*, a label from an open vocabulary (thesmos's Definition 67). The presets declare nine: commons, guild, cooperative, polity, league, household, organisation, foundation, treaty organisation.

*Remark.* By thesmos's Proposition 39 the order changes no verdict and no digest: every order is equal before the ground, and a community's substance is its rules. An agent choosing where to go reads the label first; a registry reads it for search.

**Definition 4 (registry, listing, profile document).** A *registry* is a holding of kind `registry` that some community keeps; its *listings* are holdings deposited within it under its keeper's norms of access, each naming by its SHA-256 digest a document $\ell$ (`noetarium.listing/1`) that states a community $c$, its order, purpose and node, the digest $\pi(\ell)$ of $c$'s profile document, a head $h(\ell) = (c, n, r)$ and the notary that witnessed it. The *profile document* $P(\sigma_c)$ of $c$ is the public profile written in symploke's canonical form: its order, purpose, boundary (each position's mode of entry and term), route for claims, passports policy, routes, the registries it says it is listed in, its public holdings and the parts it shows.

**Definition 5 (verification).** A reader holds evidence $e$: a profile document $p$ fetched from $c$'s node; possibly $c$'s present head $h'$ with a proof of consistency from $h(\ell)$; witnesses of $h(\ell)$; and the heads $T$ of the notaries it trusts. The listing is *faulty* when $p$ is not $c$'s, or the proof of consistency fails; else *unverified* when no witness of a notary whose head lies in $T$ attests $h(\ell)$; else *stale* when $\mathrm{SHA\text{-}256}(p) \neq \pi(\ell)$; else *current*.

**Proposition 2 (no registry is an authority).** The status of a listing is a function of the listing and the reader's evidence; the registry that holds it enters only whether the community says it is listed there. Two registries that hold the same listing give it the same status for the same reader.

*Proof.* Definition 5 reads $\ell$ and $e$ and nothing of the keeper; the module's `verify` uses the registry's name only to compare it with the profile's "listed in". ∎

**Proposition 3 (a rewritten record is found).** If a reader trusts a notary's head under which $h(\ell)$ is witnessed and holds $c$'s present head $h'$, then a listing verifies current or stale only if $c$'s present chronicle begins with the chronicle whose head was listed, unless a collision of SHA-256 is exhibited.

*Proof.* A proof of consistency between two heads of Merkle trees built as RFC 6962 builds them (archeion's `fixity`) verifies only when the first tree's leaves are the first leaves of the second, unless a collision is exhibited (RFC 6962, §2.1.2; archeion's decision 0004). Definition 5 makes a failed proof faulty. ∎

**Proposition 4 (stale exactly when what is published changed).** For a reader who trusts a notary that witnessed $h(\ell)$ and holds a proof of consistency from it, a listing made from $\sigma_c$ and verified against $p = P(\sigma'_c)$ for a later state of the same chronicle is stale or current; it is stale exactly when $P(\sigma'_c) \neq P(\sigma_c)$, unless a collision is exhibited. Acts that change neither the constitution in force nor the public holdings (a word, a vote, a work item) leave it current.

*Proof.* $\pi(\ell) = \mathrm{SHA\text{-}256}(P(\sigma_c))$ by construction of the listing; $P$ reads the constitution in force, the public holdings and the parts shown, which only amendments and accesses to public holdings change. ∎

*Remark.* A search across registries gathers each community once and reports where listings disagree (two heads of one length with different roots, two orders), never resolving it: by Proposition 3 at most one of two disagreeing heads is the community's. A delisting by another than the depositor that gives no reason is reported as a fault of the registry's record, open to the claim the keeper's norm names (the design's Definition 55).

## 3. Organisations and Their Accounts

**Definition 6 (treaty organisation).** A *treaty organisation* is a community whose constitution gives the position `member` to communities, entered by accession, declares a founding compact among its records (thesmos's Definition 64) and its organs as bodies: an assembly of envoys, a council of a bounded number of seats (thesmos's Definition 69) and a secretariat. A body elected from among another's members is a position of its own with a body of its own, since a body is the set of holders of its positions.

**Definition 7 (calls, payments, accounts).** A *call* $q = (m, a_q, \rho_q, d_q)$ is an act of the declared kind "call for contributions", read as a request whose matter is an assessed contribution: its member $m$, amount $a_q \in \mathbb{N}$, period $\rho_q$ and due turn $d_q$. A *payment* $(m, x)$ is an admitted deposit of $x$ into the core budget, credited to the party that answers for it. Calls and payments of $m$ are taken in the order of the record; a payment is credited to the call it names, then to $m$'s calls in the order they were made, and what is left is $m$'s credit, applied to its next call (after the United Nations' Financial Regulations, which credit a payment to contributions in the order assessed). At a turn $t$, $m$'s *owed* amount $o_m(t)$ is what remains on its calls, its *arrears* what remains on calls with $d_q < t$, and $k_m(t)$ its credit.

**Proposition 5 (the totals do not depend on the order of crediting).** For every member $m$ and turn $t$, with $A_m(t)$ the sum of its calls and $X_m(t)$ of its payments up to $t$: $o_m(t) - k_m(t) = A_m(t) - X_m(t)$ and $\min(o_m(t), k_m(t)) = 0$. Hence $o_m(t) = \max(0, A_m - X_m)$ and $k_m(t) = \max(0, X_m - A_m)$.

*Proof.* By induction on the calls and payments taken in order. Initially all are nought. A payment of $x$ lowers the outstanding calls by $\min(x, o_m)$ and raises the credit by the rest, so $o_m - k_m$ falls by $x$ and the credit is positive only if nothing remains owed. A call of $a$ raises $o_m$ by $a$ and then applies the credit, lowering both $o_m$ and $k_m$ by $\min(a, k_m)$, since $o_m = 0$ before whenever $k_m > 0$; the difference rises by $a$, and afterwards either the call is met and the credit may remain, or the credit is spent. ∎ Only arrears, which calls a payment met, depend on the order; the checks hold the identity on random sequences of calls and payments run through the fold.

**Definition 8 (scale, quota).** A *scale* is a family of weights $w_m \geq 0$ with $W = \sum_m w_m > 0$; by default the budget decision's own weights. Given an adopted budget $B$ for a period (the amount named by the last proposal under the budget's decision that passes under its rule, unit and weights), $m$'s *quota* is $\kappa_m = B w_m / W$. The calls of the period *follow the scale* when $|a_m - \kappa_m| < 1$ for every $m$ and $\sum_m a_m = B$.

*Remark.* Calls in whole units that follow the scale exist for every $B$ and scale: Hamilton's method of largest remainders gives them (Balinski and Young, *Fair Representation*, 1982, ch. 2, on the quota property). The module checks calls against the scale; it fixes no method of apportionment, which is the assembly's to decide.

**Definition 9 (arrears beyond k periods).** In period $\rho$, $m$ is *in arrears beyond k periods* at $t$ when its arrears at $t$ equal or exceed $\sum \{ a_q : q \text{ a call of } m,\ \rho - k \leq \rho_q < \rho \} > 0$. With $k = 2$ this is the condition of the Charter of the United Nations, Article 19, for the loss of a vote in the General Assembly. A declared mechanism: reported, never applied; a sanction is the assembly's decision.

*Remark.* The other readings of an organisation's record are direct: what each member paid into the core budget and into earmarked holdings and the share earmarked; the holders of the staff's positions by origin under control, with each origin's share and the Herfindahl–Hirschman index $\sum_o s_o^2$ (nomos's measurement); and a member's withdrawal, the turn its notice takes effect and what it owed when it left and owes now, contributions called before staying owed (Proposition 5 continues to apply to a member that has left).

## 4. The Archive of Acts and Conduct

### 4.1 Facts About Conduct

**Definition 10 (observation, traced grade).** An *observation* is an act of evidence (a report, an attestation or an assessment) naming a fact $f$, a subject $s$, a value $v$, a grade $g \in \{A, B, C\}$ and traces $\tau$ by digest. Its *traced grade* is $g$ when $g = C$, or when every trace in $\tau$ (non-empty) is a seal of the chronicle, a holding's digest or one of its versions, or a trace the reader holds beside the record (the behaviour journal's seals); otherwise $C$.

**Definition 11 (recognised fact).** Under a standard $(f, g^*, \omega)$ (thesmos's Definition 35: the least grade, and whether the author must be of another origin than the subject's), the *standing* of $f$ about $s$ is thesmos's rule of recognition applied to the observations with their traced grades. A *recognised fact* states the three criteria of the Generative Relational framework: what it *emerged from* (the observations and the standard), *since* when it persists (the turn $t_0$ such that the standing has been established at every prefix of the observations from $t_0$ on, undefined when it is not established now), and what it *constrains* (the acts whose field "rests on" names it).

**Proposition 6 (since is well defined).** If the standing is established, $t_0$ exists and is unique: it is the turn of the observation after which no prefix's standing fails to be established.

*Proof.* The standing of each prefix is computed from finitely many observations; the set of prefixes whose standing is established contains the last; the earliest prefix from which all later prefixes are established exists and is unique, and $t_0$ is its last observation's turn. ∎

**Definition 12 (corroboration by origin).** The *corroboration* of an established value is the number of distinct origins under control among the observations that meet the standard and agree with it; the *reports* are those observations. Reports of one origin count once.

**Proposition 7 (one origin's reports are worth its channel).** Let the observations of an origin $o$ be read as one group of reports sharing $o$ (tekmerion's decision 0007): $o$ registers the fact through the channel $(h_o, f_o)$ its best traced grade declares, $0 < f_o < h_o$, and its $n$ observations relay what it registered through channels $(h_i, f_i)$ with $h_i \geq f_i$, each saying the value. Then the group's likelihood ratio for the value is at most $h_o / f_o$, with equality when the relays are perfect ($h_i = 1, f_i = 0$); counted as $n$ independent reports through the composed channels, the ratio would be $\prod_i h'_i / f'_i$, which for perfect relays is $(h_o / f_o)^n$.

*Proof.* tekmerion's theory note, §2.1: with $Y = \prod_i h_i$ and $N = \prod_i f_i \leq Y$, the ratio is $(h_o Y + (1 - h_o) N) / (f_o Y + (1 - f_o) N)$; as a function of $r = Y/N \geq 1$ it equals one at $r = 1$, its derivative has the sign of $h_o(1 - f_o) - f_o(1 - h_o) = h_o - f_o > 0$, and it tends to $h_o/f_o$, which perfect relays ($Y = 1$, $N = 0$) attain. The composed channel of a perfect relay behind $o$ is $(h_o, f_o)$, and independent reports multiply. ∎ The checks find 9 for three reports of one origin through $(0.9, 0.1)$, 729 counted apart, and 81 with a second origin.

### 4.2 Contribution

**Definition 13 (contribution).** Let $G$ be a graph of provenance (archeion's PROV: entities, and activities that use and generate them, each carried out by an agent) and $x$ an artefact. The activities *beneath* $x$ are those that generated $x$ or an entity of its lineage. A party's *contribution* to $x$ is the set of activities beneath $x$ whose agent's origin under control is that party, grouped by role (the activity's kind), with the entities they generated. Under declared weights $\lambda$ of the roles (one where none is declared), the party's *share* is $\sum_{\alpha} \lambda(\alpha) / \sum_{\beta} \lambda(\beta)$, $\alpha$ over its activities beneath $x$ and $\beta$ over all activities beneath $x$.

**Proposition 8 (shares).** The shares of the parties sum to one whenever some activity of positive weight lies beneath $x$; an activity that is not beneath $x$ changes no share; a party's activities through its operatives count as its own.

*Proof.* The shares are the parts of one finite sum over the activities beneath $x$, partitioned by the origin of their agents; an activity off the lineage is in no part. ∎ Receiving work (reviewing, checking, integrating, maintaining) is counted by its role, as the design's Definition 32 asks. What a faulty input taints of a party's contribution is the entities its activities beneath $x$ generated that were derived from it (archeion's taint).

### 4.3 History and Power

**Definition 14 (mechanism, reading).** A *mechanism* is a name with parameters, identified by the SHA-256 digest of its canonical text. A *reading* of a record by a mechanism states the mechanism and its digest, the head of the record it read, and its result. The domain declares seven mechanisms, none adopted by default:

| Mechanism | Parameters | Result |
|---|---|---|
| tenure by office | a position | the turns each origin held it, their shares, $\sum s^2$ |
| long offices | a position, a bound | each origin's longest continuous holding, noted beyond the bound |
| acts of force by origin | a force (declarative unless named) | the admitted acts of that force by the origin of the party that answers for them |
| authorship of amendments | | adopted amendments by the origin of their proposer |
| rulings by origin | | rulings in the register by the origin of their author |
| petitions by origin | | petitions opened by origin, with those met, adopted or carried |
| voting power | a decision | each member's Banzhaf and Shapley–Shubik indices |

Tenure is read by replaying the record act by act: an interval of a holding begins at its turn of entry and ends at the turn of the act at which it was found ended (by leaving, by an office, by a term settled at that act), or, still held, at the last act's turn.

**Proposition 9 (a reading is reproduced).** Two readers who hold records sealing to the same head and apply mechanisms of the same digest obtain the same reading.

*Proof.* Records that seal to the same head are the same sequence of acts, unless a collision is exhibited; the replayed state is then the same (thesmos's Proposition 21); every mechanism is a function of the replayed state, the replay's intervals and its parameters, which the digest fixes. ∎ This is the design's Proposition 15 for readings of the chronicle alone; a reading of the behaviour journal joined to it is reproduced the same way, the pair of heads standing for the head.

**Proposition 10 (voting power is the rule's).** For a decision with a known rule, unit of count and weights, the game whose coalitions win when their members voting for and the others against pass under the decision is a simple game, and its indices are defined.

*Proof.* Every rule the kernel knows is monotone (thesmos's Proposition 33), so a superset of a winning coalition wins; with every member voting against no rule passes, and with every member for every rule passes, so the empty coalition loses and the grand coalition wins. nomos's games compute Penrose, Banzhaf and Shapley–Shubik indices of any such game on at most 24 players exactly (Banzhaf 1965; Shapley and Shubik 1954; Felsenthal and Machover 1998). ∎ The checks find Banzhaf $(3/5, 1/5, 1/5)$ and Shapley–Shubik $(2/3, 1/6, 1/6)$ for at least $2/3$ of weights $2, 1, 1$, and the treaty organisation's double majority equal to a count of swings made apart.

*Caveat.* A measure of power is itself a reading, and whoever defines it holds a power too (the design's Section 4.6). The measures are declared mechanisms, several may stand side by side, each community adopts those it acts on by its own rule, and an adjustment they prompt goes by a declared route: a term on a position, seats on a body, recall by petition, rotation. A trigger rule that opens a review of an office held too long, which the design proposed, is not an effect the ground produces by law (thesmos's Definition 34 limits trigger rules to ending positions, lapses and overdue marks); a community writes a term instead, or reads the measure and petitions.

## 5. Items and Their Routes

Every item a constitution's routes open (an application, a question, a petition, or a type it declares) is read with the five complements of a claim's route (the design record of 2026-11-25, Section 8.3): its author a known party, a forum (the body or office its route names), evidence (what it cites is in the chronicle), authority (the route reaches the forum), and capacity (a holder of the forum who is not a party to it, thesmos's Definition 55). An item that lacks capacity is referred to its route's review body or beyond the community. By thesmos's Proposition 27, in a community that declares its routes every item either closes in the act that opens it or stands open on a route on which some party may answer it, unless the route's body has no holder or every holder is a party to it; the domain names those items (`unanswerable`), a household's own question to its only person among them.

## 6. What the Domain Holds

| Concept of the design | Where it is computed |
|---|---|
| communities of many orders (Sections 2.1, 7.9) | the twelve presets, each admitted, each declaring its order, passports, routes, and work where it posts work; the five of Phase 8 declaring transmission by operatives |
| recognition in levels, read from the record (Definition 52) | `federation::Recognitions::read` on thesmos's recognitions |
| compacts, open to any community (Section 8.11) | `federation::standing`, its parties found from the records |
| registries and listings (Definition 55, Section 7.3) | `registries` |
| treaty organisations' budgets, staff and withdrawal (Definition 54, Section 8.13) | `organisations`, on the preset `treaty-organisation` |
| facts about conduct, contribution, power (Section 4) | `trajectories`, with tekmerion's dependence, archeion's provenance and nomos's measures |
| every directive's route (Definition 46, G8′) | `claims::read_item`, `claims::unanswerable` |
| terms and seats (Section 7.8) | thesmos's decision 0014 |
