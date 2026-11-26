# The Grammar of Event Spacetime

- **Status:** draft (2026-11-26)
- **Activity:** research
- **Code:** `events/include/urban/events/grammar.hpp`, on syntaxis's `foams` (its decision 0004) and morphe's `repair` (its decision 0022)
- **Decision:** [ADR 0168](../../../development/decisions/0168-the-grammar-of-event-spacetime.md)

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

## 6. What the Grammar Claims

The grammar describes the record. It claims no cause, and no rule says why an event occurred. The kinds of events are made by the record's sources: censuses, registrations, approvals and permits each produce their own rules, and a source that starts later brings its rules with it. A context that tells nothing beyond the match says that, in this record, what an event does is fixed by what it consumes. A state that tells much (a regime's end follows its start) is a law of the record's construction before it is a law of the city.

## References

- Dawid, A. P. 1984. Statistical theory: the prequential approach. *Journal of the Royal Statistical Society A* 147, 278–292.
- Grünwald, P. D. 2007. *The Minimum Description Length Principle*. MIT Press.
- Knuth, D. E. 1968. Semantics of context-free languages. *Mathematical Systems Theory* 2, 127–145.
- Larsson, N. J., and Moffat, A. 2000. Off-line dictionary-based compression. *Proceedings of the IEEE* 88, 1722–1732.
- MacKay, D. J. C., and Peto, L. C. B. 1995. A hierarchical Dirichlet language model. *Natural Language Engineering* 1, 289–308.
- Huang, W. 2026. An inquiry into the formation of evidence-based partial orders in the governance of human–AI evolutionary production. Knowledge Commons, DOI 10.17613/73180-mhn03.
