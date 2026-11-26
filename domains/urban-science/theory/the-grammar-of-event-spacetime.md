# The Grammar of Event Spacetime

- **Status:** draft (2026-11-26)
- **Activity:** research
- **Code:** `events/include/urban/events/grammar.hpp` and `network.hpp` (`street_between`), on syntaxis's `foams` (its decisions 0004 to 0007) and morphe's `repair` (its decision 0022)
- **Decisions:** [ADR 0168](../../../development/decisions/0168-the-grammar-of-event-spacetime.md), [ADR 0171](../../../development/decisions/0171-patterns-grown-histories-and-new-york.md)

A city's record of events makes a 2-complex, the event complex of [ADR 0067](../../../development/decisions/0067-event-spacetime.md). This note reads that complex as generated: each event is a production that attaches one vertex with its neighbourhood, and the complex is what the productions glue together in time. The productions are learned from the record, and the grammar they form is judged on events it has not yet seen. The general constructions are syntaxis's (`docs/theory.md`, §7); what follows is what they mean for a city, and what this record can and cannot show.

## 1. The Record as a Foam

**1.1 The event complex.** A record holds couplings (volumes: buildings, street spaces, junction spaces, malls and laneways), primitives (interfaces between two couplings: party walls, frontages, cross-sections) and events, each touching some couplings and meeting some primitives at a time. Its event complex $K$ has the events as vertices, with the window's two ends $\partial_0$ and $\partial_1$; each coupling's life between two consecutive events it attends as an edge (a segment of its worldline); and each primitive's life between two consecutive events that meet it as a face (a lens), whose boundary runs up one side's worldline and down the other's. $K$ is morphe's worldline complex of the record's encounters.

**1.2 Labels.** $K$ is a foam in syntaxis's sense: its vertices are labelled by their events' types (a regime's end apart from its start; with detail, by type and kind), its segments by their couplings' types and its lenses by their primitives' types.

**1.3 States.** A coupling may carry a state into each event it attends. Two are used here, each computed from the record *before* the event and never from the event itself:

- a building's **use**, its family as its last census saw it;
- a space **under a regime** begun and not yet ended.

*Remark.* The second is an attribute that the productions set themselves: a regime's start sets it on every space it touches, and its end clears it. A grammar read with it is a grammar of attributed symbols (Knuth 1968). The first is an attribute set by an observation.

*Caveat.* A state given at an event's own vertex is seen by that event's context. Were the state set by the event (a census's sight of the use, a regime's start), the context would hold the event's outcome and the test of §4 would be void. The states are therefore keyed at every event with the value carried into it.

## 2. Events as Productions

**2.1 The star of an event.** The star of a vertex $v$ is everything in $K$ incident to $v$. Each coupling present has a role: *through* (it stood before $v$ and goes on), *begins* (its life starts at $v$), *ends*, or *instant*. Each lens incident to $v$ has a corner there: it *opens* (its first meeting), *closes* (its last), or *passes* (the primitive's life continues through $v$ on one side's worldline).

**2.2 Four keys.** A star is reduced to four labelled graphs, each keyed up to isomorphism:

| Key | What it holds | Its city reading |
|---|---|---|
| star | everything, with states | the event as it happened |
| rule | the event's kind, the couplings' kinds and roles, the corners opened and closed | what the event does (the production) |
| match | the couplings it consumes (those with an arriving segment) and the lenses it closes among them | what the event needs in order to occur |
| context | the match with its couplings' states and the lenses that pass beside them | the match in its surroundings |

**2.3 Gluing.** The complex is recovered from its stars: the cells counted from the stars agree with $K$'s (syntaxis's Proposition 7.1), and events consecutive in time that share no coupling commute (its Proposition 7.2). The record is therefore a trace, and its Foata normal form gives the steps in which events could have happened together.

## 3. Curvature and the Generation of χ

**3.1 Definition.** For a vertex $v$ with $e(v)$ ends of segments and $c(v)$ corners that open or close,

$$\kappa(v) = 1 - \tfrac{1}{2}\,e(v) + \tfrac{1}{2}\,c(v),$$

and $\chi(K) = \sum_v \kappa(v)$ (syntaxis's Proposition 7.1): each segment has two ends and each lens one opening and one closing corner. A rule's share of $\chi$ is its count times its star's $\kappa$, and the shares of the rules with the window's ends make up $\chi$ exactly.

**3.2 Proposition (the city's kinds of curvature).** In a record whose events are observations, alterations, registrations, regimes and births:

1. an observation, an alteration or a registration touches one building, which passes through, and meets no interface: $\kappa = 0$;
2. a regime touching $n$ spaces that all pass through, meeting no interface, pinches their worldlines: $\kappa = 1 - n$, and with its end the regime contributes $2(1 - n)$;
3. a birth of one building touching $m$ standing couplings that pass through, and opening $o$ interfaces, has $\kappa = \tfrac{1}{2} - m + \tfrac{o}{2}$; when every neighbour receives exactly one interface ($o = m$), $\kappa = (1 - m)/2$.

*Proof.* (1) One coupling through gives $e = 2$ and $c = 0$. (2) $n$ couplings through give $e = 2n$, $c = 0$. (3) The new building contributes one end, each neighbour two; each opened interface one corner. ∎

*Remark.* Hence the observations, which are most of a record, change nothing of its topology. The record without them has the same $\chi$ (each only cuts one worldline), which the reading reports as a check. A city's $\chi$ is written by its regimes, which hold many spaces together for a time, and by its births, which attach a building to its neighbours.

## 4. The Grammar and Its Judgement

**4.1 Three levels.** The rule of an event is predicted at three levels: alone ($P_0$); given what the event consumes ($P_1$, context-free); and given its context ($P_2$, context-sensitive). Each level backs off to the one below by a Dirichlet prior (MacKay and Peto 1995):

$$P_1(r \mid m) = \frac{N(m, r) + \alpha_1 P_0(r)}{N(m) + \alpha_1}, \qquad P_2(r \mid c) = \frac{N(c, r) + \alpha_2 P_1(r \mid m(c))}{N(c) + \alpha_2}.$$

$P_0$ keeps a share $\gamma/(N + \gamma)$ for a rule never seen. $\gamma$ is chosen by leave-one-out in closed form, and $\alpha_1$, $\alpha_2$ by leave-one-out on a grid.

*Caveat.* $P_0$ and $P_1$ are not comparable as codes for the same thing: a rule determines its match, so $I(R; M) = H(M)$. $P_1$ against $P_2$ is the comparison of context-free with context-sensitive.

**4.2 Prequential judgement.** Each event is priced by the grammar of every event before it, the escape and concentrations chosen again at the start of each year (Dawid 1984). The total is the length of a code for the record's rules as the record unfolds (Grünwald 2007). A rule is priced once as new when it first appears and is known thereafter. A source that begins partway through the window is therefore charged in the years it arrives and then learned, and its arrival cannot be mistaken for a failure of the grammar on later years.

**4.3 Grammars compared.** Each grammar is measured by its bits per event before a date and after it, and by the pairs of a context and a rule it counts. One stands below another only when it is no better on all three: the order is partial, and several grammars may be undominated, after the author's inquiry into the formation of evidence-based partial orders (Huang 2026).

## 5. Lives

**5.1 Lives as words.** A coupling's events read in time form a word, its life, with each event written as its rule and the coupling's place in it. The observations are left out, so that the lives are of changes alone. The words of each kind of coupling are compressed together by Re-Pair (morphe's `repair`). A rule of the straight-line grammar is a run of events that recurs in lives, nested, and its uses count how often the derivation spends it.

**5.2 Remark (histories given together).** A run that several couplings share is a history they were given together. The segments of one named street share the permits that name the street: the street, which the record never declares as a unit, appears as a rule of their lives.

## 6. Patterns across the Volumes

**6.1 The record as a process.** The record's changes, the censuses left out, are read as a process net (syntaxis's §7.6). Each event is a box, with a port for each volume it consumes or leaves, and each piece of a volume's worldline between two events is a wire. The net is compressed by digrams. The commonest pair of boxes joined by wires, counted where no two share a box, becomes a pattern, and again until no pair recurs.

**6.2 What a pattern is in a city.** A pattern's instances bind the volumes that run through them: a permit and its end on the same street spaces, two permits in a row on a named street, a building's alterations in a row. A pattern of several volumes is a part of the city's history that recurs across them, found from the complex and from no single volume's history. Its instances' sets of volumes are the units the record treats together. The record never declares those units: they are named streets, routes of parades, blocks taken for filming.

## 7. Histories Grown from the Grammar

**7.1 The growth.** The district's history is grown again from its grammar (syntaxis's §7.7), its censuses left out. Each place where events happen is an automaton that remembers its last event: any building, for a change to one building; the very street spaces of a permit; a building's birth against its neighbours, once. At each step a place is chosen by its rate given its last event and its era (the eras of the record's sources). The grammar then chooses the event among the rules that followed that last event at such a place in the record, in the context it would have, each space carrying the number of regimes in force on it. A permit's end follows its own start because the record shows only an end after a start at that place. A state a place never stood in during an era takes the window's rate there, so that a permit grown across an era's start ends even where the record held no permit in force in that era.

*Caveat.* The growth carries the number of regimes in force, where the reading of §4 carries only whether one is. An end leaves a space under a regime when another permit is open on it, so whether one is in force is no function of what it was, the event and the place, and the growth's table of what followed cannot update it (syntaxis's §7.7). The number in force is such a function. Of the permits in Melbourne's data, 66 begin while another on the same named streets is in force; with whether one is in force carried, grown histories left about a twentieth of their permits unended.

*Remark.* Two weaker designs were tried and fail on Melbourne's record, and the failures say what a place is. A place whose state is read from the states its street spaces share sees other permits' regimes and ends what it never began: grown so, 555 permits became about a thousand. A place keeping the states its own events left, by a table learned from those shared states, still ends twice where its streets are often held by others (610 ends against 555 beginnings). A permit is a place's own history, so its state is its own last event.

**7.2 What growth tests.** Each grown history is measured as the record is: its events by kind and year, χ, the depth of its trace, its prequential code, how far the lives of its street spaces and its net compress. A measure on which the record lies outside its grown histories marks what the grammar does not hold. What the grammar cannot hold is said in advance: a permit on a set of streets no permit used, a building born where none was, time beyond the rates of its eras, and any memory of a place beyond its last event.

## 8. A Second City

**8.1 New York's blocks.** New York's permits name blocks, "WALL STREET between BROAD STREET and WILLIAM STREET". A block is resolved on the district's network (`street_between`): the street spaces of the street on the shortest path, counted in street spaces, between a junction it shares with the first cross street and one it shares with the second. Names are compared by a key that writes out abbreviations and drops case, punctuation and ordinal endings.

**8.2 Proposition (a closure's curvature).** A closure meets every cross-section of the street spaces it closes, these street spaces and the junctions at their ends passing through. Let $G$ be the graph whose vertices are those junctions and whose edges are those street spaces. Then

$$\kappa = 1 - \chi(G) = 1 - |V(G)| + |E(G)|.$$

In particular $\kappa = 1 - p$ when $G$ is a forest of $p$ trees, and $\kappa = 0$ for one run of blocks.

*Proof.* Each street space has two cross-sections, one with the junction at each end. With all $|V(G)| + |E(G)|$ volumes through, $e(v) = 2(|V(G)| + |E(G)|)$. Each cross-section met in the middle of its life is closed and opened at the event, so $c(v) = 4|E(G)|$ corners. Hence $\kappa = 1 - |V(G)| - |E(G)| + 2|E(G)| = 1 - \chi(G)$. A forest of $p$ trees has $|V(G)| = |E(G)| + p$. ∎

*Remark.* A permit that holds street spaces without closing them pinches their worldlines: $\kappa = 1 - n$ (Proposition 3.2). Closing the same run of blocks meets their interfaces and leaves $\chi$ as it was. The two act alike on the street and differ in what they do to the complex. Every closure of New York's record holds the proposition: each of its 60 rules has $\kappa = 1 - |V(G)| + |E(G)|$, with $|E(G)|$ half the cross-sections it meets and $|V(G)|$ the rest of its participants.

## 9. What the Grammar Claims

The grammar describes the record. It claims no cause, and no rule says why an event occurred. The kinds of events are made by the record's sources: censuses, registrations, approvals and permits each produce their own rules, and a source that starts later brings its rules with it. A context that tells nothing beyond the match says that, in this record, what an event does is fixed by what it consumes. A state that tells much (a regime's end follows its start) is a law of the record's construction before it is a law of the city.

## References

- Dawid, A. P. 1984. Statistical theory: the prequential approach. *Journal of the Royal Statistical Society A* 147, 278–292.
- Grünwald, P. D. 2007. *The Minimum Description Length Principle*. MIT Press.
- Knuth, D. E. 1968. Semantics of context-free languages. *Mathematical Systems Theory* 2, 127–145.
- Larsson, N. J., and Moffat, A. 2000. Off-line dictionary-based compression. *Proceedings of the IEEE* 88, 1722–1732.
- MacKay, D. J. C., and Peto, L. C. B. 1995. A hierarchical Dirichlet language model. *Natural Language Engineering* 1, 289–308.
- Huang, W. 2026. An inquiry into the formation of evidence-based partial orders in the governance of human–AI evolutionary production. Knowledge Commons, DOI 10.17613/73180-mhn03.
