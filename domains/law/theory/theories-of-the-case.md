# Theories of the Case, Searched For, Read Under the Law and Tested

*Status: draft (2026-11-18; [ADR 0127](../../../development/decisions/0127-theories-of-the-case-in-space-and-time.md)). The law domain's module `theories`, on tekmerion's `scenarios` and `construal`, kinesis's `prisms`, heuresis's `evolution`, apodeixis and archeion.* This note is for **learning** and for **use**: it says what a theory of the case is here, what the search for theories does and does not establish, and what is exact.

## 1. The Problem

A court must decide what happened, and it was not there. It has items of evidence, each speaking of a place and a time within an error, each more or less credible; it has the parties' accounts; and it has a law that makes a consequence follow from facts. Three difficulties are classical.

1. **The accounts are few, the possibilities many.** A prosecution offers one account and a defence another. An account the evidence allows equally well, and that no one put forward, may never be weighed. Wagenaar, van Koppen and Crombag (1993) and Bex (2011) ask that alternative scenarios be sought, not only compared.
2. **Where and when constrain who.** A person seen at an inn at 21:43 and at home at 21:51 could have been at a door between only if they could travel that far in that time. Such constraints are exact, and are often checked by hand, if at all.
3. **A story fitted to everything predicts nothing.** An account built from all the evidence explains it by construction. One that predicts evidence it was not built from is better grounded.

The module answers each: an evolutionary search over accounts in space and time, with an archive kept by what each account says (1); every account's paths checked against the speeds of the people in it, exactly (2); and evidence held back from the search, on which each account kept is tested once, the test sealed before it is read (3).

## 2. Definitions

**2.1 Problem.** A problem is $(A, K, S, R, I)$: agents $A$, each with a bound on speed $v_a$; kinds of event $K$, each with roles; a story scheme $S$, slots each filled by an event of one of its kinds (the first slot central); rooms $R$, where and when an event of a kind may be placed; and items $I$. Each item $i$ has a place $x_i \in \mathbb{R}^2$ within a radius $r_i$, a span $[s_i, u_i]$, a credibility $c_i \in (0, 1)$, and says one of: a *sighting* (agent $a_i$ was within $r_i$ of $x_i$ at some instant of the span), an *event* (something of kind or slot $k_i$ happened there then, by $a_i$ in a role when it says who), an *absence* (nothing of kind $k_i$ happened there then, or $a_i$ was not there), an *interval* (the event of one kind came between $\ell_i$ and $m_i$ after that of another). Some items are *held back*.

**2.2 Account.** An account $\sigma$ is a finite set of events $e = (k, \rho, x, t)$ (a kind, an assignment $\rho$ of agents to roles, a place, an instant) and a stance on each item: believed (with an instant $\tau_i \in [s_i, u_i]$ for a sighting) or doubted.

**2.3 Fixes and trajectories.** The fixes of agent $a$ under $\sigma$ are its believed sightings $(x_i, \tau_i, r_i)$ and the events in which it has a role $(x, t, 0)$. Its record admits a history when there are points $q_k \in D(x_k, r_k)$ with $|q_{k+1} - q_k| \le v_a (t_{k+1} - t_k)$ in order of time (kinesis's prisms).

**2.4 Coherence.** $\sigma$ is coherent when every agent's record admits a history; every believed event-item is explained by an event of its kind (or slot) within its disc and span and by the agent it names; no believed absence is contradicted; and every believed interval holds between the first events of its two kinds.

**2.5 Score.** $\mathrm{imp}(\sigma) = \sum_{i \text{ not held back}} -\log c_i^{[\text{believed}]}(1-c_i)^{[\text{doubted}]}$, the negative log-probability that the evidence is as reliable as $\sigma$ takes it to be, items taken as independent; and
$$\mathrm{score}(\sigma) = \mathrm{imp}(\sigma) + \lambda \cdot \#\{\text{events no believed item speaks for}\} + \mu \cdot \#\{\text{slots unfilled}\},$$
with declared costs $\lambda, \mu$ in nats (1 and 2 by default). Over a set $\Sigma$ of coherent accounts, the relative plausibility of $\sigma$ is $\exp(-\mathrm{score}(\sigma)) / \sum_{\sigma' \in \Sigma} \exp(-\mathrm{score}(\sigma'))$ (Allen and Pardo 2019).

*Remark.* Believing an item of credibility $c < 1/2$ costs more than doubting it: a witness more likely wrong than right gives no support, and an account that leans on one pays for it.

**2.6 Bindings and the legal consequence.** Bindings say what an account makes hold: an event of kind $k$ with agent $a$ in role $r$ makes fact $f$ hold; a believed item makes fact $f$ hold. The facts the bindings address are then *proved* where the account makes them hold and *not proved* where it does not; every other fact enters as the matter declares it. Each question is then asked of the resulting record as any question of the matter is (`norms`, apodeixis): clear, arguable or untenable over the admissible readings of the open terms.

*Register.* After the Generative Relational framework (the note on facts, §1): items are manifestational; an account and what it makes hold are relational (they emerge from the evidence and the rules it is read under, persist while the account is held, and are what the law acts on); what happened is ontological and left open. No arrow of the schema (§6) lets an item name an event of an account: two accounts may read one item differently, and no item identifies what it is evidence of.

## 3. What Is Exact

**Proposition 1 (the prism between exact fixes).** If an agent's fixes are exact ($r_k = 0$) and its record admits a history, the places a history can occupy at $t \in [t_k, t_{k+1}]$ are exactly $D(x_k, v(t - t_k)) \cap D(x_{k+1}, v(t_{k+1} - t))$.

*Proof.* Necessity: a history travelling at most $v$ is within $v|t - t_j|$ of $x_j$ at every $t$. Sufficiency: for $p$ in the lens, the straight paths $x_k \to p \to x_{k+1}$ at constant speed take at most $v$; joined to the rest of the record, which admits a history, they form one. □

With radii the two discs are an outer bound, and a history is sought (cyclic projections onto the discs and the bounds on each step, whose limit lies in the intersection when it is not empty: Bregman 1965) and checked exactly. So consistency is *consistent* with a witness, *inconsistent* with the pair of fixes that cannot be joined, or *undecided*; it is never guessed. (Three discs may meet pairwise and not together; kinesis's checks hold an example.)

**Proposition 2 (reaching a region).** Between consecutive fixes, $\delta(t) = \min_p \max_i (|p - c_i| - \rho_i(t))$ over the slice's discs and a region's disc is convex in $t$; the region is out of reach during an interval iff $\delta > 0$ throughout it, and the instants at which it is within reach form one interval on each piece.

*Proof.* On a piece the radii $\rho_i(t)$ are affine, so $(p, t) \mapsto |p - c_i| - \rho_i(t)$ is jointly convex; a maximum of convex functions is convex; minimising a jointly convex function over $p$ leaves a convex function of $t$. A convex function's sublevel set is an interval. □

The minimiser over $p$ at each $t$ is one of finitely many candidates (one, two or three discs active: a centre, a point on the segment between two centres, an additively weighted Apollonius point of three), so $\delta(t)$ is exact at each $t$; its least value on a piece is found by Brent's method to a stated tolerance in time, and an impossibility is claimed only when $\delta$ exceeds what that tolerance can hide ($v$ times it).

**Proposition 3 (the test does not leak).** Nothing held back affects the search: held-back items are not assessed, their stances are never varied, a model is not shown them, and the archive's judge reads only the assessment on the rest. *By construction*, held by the checks: every kept theory's reading of the held-back item is "held back", and a model's prompt never contains it.

## 4. What Is Searched For, and What That Establishes

The search is heuresis's evolution over accounts: generators (anchored on the items, doubting some, from the scheme, at random), mutations (retime, move, recast, flip a stance, restamp a sighting, drop, add, repair, give the central event to another hand or kind), crossovers (by agent, by time, uniform), a model's proposals in four roles, and the parties' declared accounts. The archive keeps, in each niche, the best coherent account; a niche is what an account says of the central slot (which kind, whose hand) and each question's answer under it. So the best account in which the accused did not do it is sought as actively as the best account in which they did.

**What it establishes.** That the accounts kept are coherent, with their scores and their legal consequences, exactly. **What it does not.** That no better account exists, or that an alternative not found is excluded. The standing of a question over the theories found is said of what was found: *clear* when it follows under every account kept, *untenable* when under none, *arguable* otherwise; an account under which it is arguable (turning on how an open term is read) counts among the alternatives.

**The burden.** Whoever bears it must exclude the alternatives. Here an alternative is *reasonable* when it is a coherent account found under which the question is not clear, of a relative plausibility of at least a declared threshold. The threshold is the matter's; "so that the court is sure" is not a number in any law, and none is claimed for it.

## 5. The Test on What Was Held Back

Each account kept is tested once on the held-back items. The test is registered in a journal sealed by archeion's hash chain *before* the items are read ("registered a test … not yet read"), and its outcome written after. The account's events and its other stances are held; the instants of its believed sightings are free within their spans (tried as they stand, at their middles, and 400 times at random, keyed); the items are believed. It is *admitted* when some such instants make it coherent; else it *must doubt* what it cannot admit (each item in turn, then all), paying for it; else it *no longer holds*. A test that finds no admitting instants has not shown that none exist, and says how many it tried.

## 6. The Schema

[schema/theories.cat](../schema/theories.cat) states a theory's record as categories, and kategoria answers each statement: the layers of a theory (a holding, an item read from it, an account it bears on, what happened) lie in the three registers in order, what happened the terminal object; the schema of a record (events of kinds in accounts, parts played in roles, items read from holdings of a modality, stances taken in accounts, bindings) is thin; a role of a part is a role of its event's kind; a holding is manifestational whatever its modality. Two accounts of one fire are a set-valued functor on it; an account in which someone is the actor of a fault, and one kept as though it were a trace, are refused. The domain writes accounts as such functors (`theories/schema.hpp`), and kategoria's verdict shares none of the domain's code.

## 7. The Example

*The fire at the boathouse* (the Law Studio's plug-in `incident`; every person, place and word invented). An ordinance makes damage by fire follow from setting fire to another's property with intention, and leaves open whether intention may be inferred from preparation. Twelve items: an investigator's report and window, a log of calls, a telephone's mast, an inn's camera, a sister's account of a landline, a jogger on a footbridge, a witness who saw a light, a caretaker's log and his own account, a till roll placing him at the inn, a receipt for paraffin, and a till roll placing the accused at the inn at 21:44:20, disclosed late and held back. What the search found is the ADR's §4 and the gallery's.

## 8. Caveats

- Space is Euclidean: no street network, no river that must be crossed at a bridge. A path the theory needs may be impossible on the ground and possible here.
- Credibilities are declared, not estimated; items are taken as independent (an item that shares an origin with another should be folded into one first, tekmerion's `dependence`).
- An absence of an agent is checked at its fixes, not along the path between them.
- Relative plausibility ranks what was found. A niche the search never reached has no account, and its absence is not evidence.
- The search is keyed: the same matter, options and key give the same theories; another key may find others.

## 9. References

- Hägerstrand, T. (1970), What about people in regional science?, *Papers of the Regional Science Association* 24. Miller, H. J. (2005), A measurement theory for time geography, *Geographical Analysis* 37.
- Bregman, L. M. (1965), The method of successive projection for finding a common point of convex sets, *Soviet Mathematics Doklady* 6.
- Pennington, N. and Hastie, R. (1993), Reasoning in explanation-based decision making, *Cognition* 49. Bex, F. J. (2011), *Arguments, Stories and Criminal Evidence*.
- Wagenaar, W. A., van Koppen, P. J. and Crombag, H. F. M. (1993), *Anchored Narratives: The Psychology of Criminal Evidence*.
- Allen, R. J. and Pardo, M. S. (2019), Relative plausibility and its critics, *International Journal of Evidence and Proof* 23.
- Mouret, J.-B. and Clune, J. (2015), Illuminating search spaces by mapping elites, arXiv:1504.04909.
- Huang, W., *Rethinking Relation in a Generative Relational Framework* (read here from the author's summary).
