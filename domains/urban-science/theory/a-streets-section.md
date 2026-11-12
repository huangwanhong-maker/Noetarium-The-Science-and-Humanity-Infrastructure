# A Street's Section

*Status: draft (2026-11-12). The formalism of the urban-science domain's `section` ([ADR 0098](../../../development/decisions/0098-the-street-a-building-stands-on.md)): what a street is when it is measured between the faces that stand on it, and what follows. Every statement here is checked by the domain's checks (`section/tests/check_section.cpp`) on a street worked by hand; the limit of a building's height is the architecture domain's ([A Drawn Building](../../architecture/theory/a-drawn-building.md), §11).*

## 1. What a Street Is Here

**Definition 1.1.** A *street* is a polyline $c : [0, L] \to \mathbb{R}^2$ by arc length, its *centreline* (metres; $x$ east, $y$ north), with a finite set of *buildings* about it: prisms on flat ground, each a footprint ring and a height. Its *left* at $s$ is the unit vector $\ell(s)$ a quarter turn counter-clockwise from its direction.

Nothing is said of ownership or of a reserve: the width of a street is measured between what stands on it. A way that passes under a building has no face there.

**Definition 1.2 (stations and faces).** *Stations* stand at $s_i = L\,(i + \tfrac12)/n$, $n = \lfloor L / \text{step} \rfloor$. At a station the *face* on a side is the first point at which the ray $c(s) \pm t\,\ell(s)$ enters a building's footprint, $t \le$ a reach: its distance $d$, and the building's height $H$. A side with no face is *open*.

**Definition 1.3.** At a station with both faces, the *width* is $W = d_l + d_r$ and the *enclosure* is $\dfrac{(H_l + H_r)/2}{W}$: the mean height over the width.

## 2. The Sky

**Proposition 2.1.** From a point on the ground, an endless wall whose top stands at an angle $\beta$ above the horizon, seen square on, fills the share $(1 - \cos\beta)/2$ of the view of the sky that a horizontal surface has. *Proof.* The view factor from a horizontal element to a part of the hemisphere is the area of that part's projection on the ground plane, over $\pi$ (Nusselt). The directions to the wall's top lie on a great half-circle through the two ends of the street, tilted $\beta$ up from the horizon; the wall fills the lune between that half-circle and the horizon on its side. The lune's projection is a half-disc less a half-ellipse of semi-axes $1$ and $\cos\beta$: area $\tfrac{\pi}{2}(1 - \cos\beta)$. ∎

**Definition 2.2 (sky).** The *sky* at a station is $\psi = 1 - \sum_{\text{sides with a face}} \dfrac{1 - \cos\beta}{2}$, $\tan\beta = H/d$: the sky view factor at the centreline of a canyon of that section running on without end (Oke 1981; Johnson and Watson 1984). With both faces, $\psi = (\cos\beta_l + \cos\beta_r)/2$.

*Caveat.* It is the section's sky, not the place's: a gap between two buildings a little along the street opens more sky than the station's section says, and a tower beyond the face closes some. It compares sections.

## 3. The Sides

**Definition 3.1.** A side's *line* is the median of its faces' distances. The side is *built* over the share of stations that have a face, and *aligned* over the share whose face lies within 2 m of the line: the continuity of its street wall.

## 4. The Strips

**Definition 4.1.** A *division* of the street is a sequence of *strips* from the left, each with a kind and a width $w_j > 0$, centred on the centreline: strip $j$ lies between the offsets $-T/2 + \sum_{i<j} w_i$ and that plus $w_j$, $T = \sum w_j$. Each kind belongs to a group: people on foot (footpath, frontage, shared), cycles, green (verge, median), transit (bus, tram), vehicles (parking, lane). A group's *share* is its strips' width over $T$.

**Definition 4.2.** The division *fits* when $T \le \min_i W_i$ over the stations that have both faces.

**Definition 4.3 (what is asked).** A *scheme* states limits (a least, a most, a clause): on a strip's width by its kind, on the enclosure, on the share for people on foot, on the hours of sun on a footpath. A limit not stated is not found. Nothing is any authority's until a scheme states it.

## 5. Sun on the Footpaths

**Definition 5.1.** On each side at each station a *footpath point* stands on the ground: at the middle of that side's outermost footpath where the division has one, else one metre out from the face, else three metres from the centreline. Its hours of sun over the instants of a day are those at which the Sun is up and the ray toward it clears every building (the architecture domain's thresholds). A side's sun is the mean over its stations, and its *sunny* share the share of stations with at least the hours asked.

**Definition 5.2 (how high each may rise).** Each building that is a face of some station is given its *limit*: the greatest height at which every footpath point that has the hours asked keeps them, the others as they stand (A Drawn Building, Proposition 11.2 and Definition 11.3).

*Remark.* A building shades the footpath at its own foot when the Sun stands behind it, whatever its height above a metre or two; such a point has not the hours and asks nothing of it. The limit speaks for the footpath across the street, and for the one beside a building when the Sun comes along the street.

## 6. What Is Left Out

The ground's relief; trees, awnings and arcades; the sky's diffuse light and what the faces reflect; a centreline that bends sharply between stations; who owns the ground between the faces; the traffic, the noise and the air.

## References

- G. T. Johnson and I. D. Watson, "The determination of view-factors in urban canyons", *Journal of Climate and Applied Meteorology* 23 (1984) 329–335.
- R. L. Knowles, *Sun Rhythm Form*, MIT Press, 1981.
- T. R. Oke, "Canyon geometry and the nocturnal urban heat island: comparison of scale model and field observations", *Journal of Climatology* 1 (1981) 237–254.
