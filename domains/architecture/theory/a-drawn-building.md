# A Drawn Building

*Status: draft (2026-11-12). The formalism of the architecture domain's `plan`, `programme`, `interior` and `shell`, of `rules`' siting and of `environment`'s panes and solar limits ([ADR 0095](../../../development/decisions/0095-the-design-studio.md), [ADR 0096](../../../development/decisions/0096-what-stands-in-the-rooms.md), [ADR 0097](../../../development/decisions/0097-the-shell-of-a-drawn-building.md), [ADR 0098](../../../development/decisions/0098-the-street-a-building-stands-on.md)): what a building is when it is drawn by hand, and what follows from the drawing. Every statement here is checked by the domain's tests on plans worked by hand.*

## 1. What Is Drawn

**Definition 1.1 (storey).** A *storey* is a finite plane graph $G = (V, W)$ with straight edges: vertices $V \subset \mathbb{R}^2$ (metres; $x$ east, $y$ north) and walls $W$, each a segment between two vertices, such that two walls meet only at a common end. Each wall $w$ carries a thickness $t_w \ge 0$ and a kind, *solid* or *open*. On the walls stand *openings*, doors and windows, each an interval of its wall with a height, a sill, a glazed share and an openable share. A finite set of *labels*, points of the plane each with a use, names the rooms. A storey has a height, floor to floor, and a ceiling.

**Definition 1.2 (building).** A *building* is a sequence of storeys and a set of *stairs*, each rising from a point of one storey to a point of the next.

Nothing else is drawn. In particular the rooms are not: they follow.

*Remark.* Graphs that are not plane (walls that cross, a wall through a vertex that does not end there, a wall twice) are refused, each fault named (`plan::read`). The page that draws keeps a storey plane by cutting walls where they meet (`visualization/js/plan`).

## 2. Rooms

**Definition 2.1.** The *faces* of $G$ are the orbits of the face permutation $\varphi = \sigma^{-1} \alpha$ on its darts, $\sigma$ the rotation at each vertex and $\alpha$ the reversal of a dart (physis, `plane_faces`). A *room* is a face of positive signed area, that is, a bounded face of its component traversed counter-clockwise.

**Proposition 2.2 (Euler).** For a connected storey with $F$ faces, the unbounded one among them, $|V| - |W| + F = 2$; with $c$ components, the rooms number $|W| - |V| + c$.

**Definition 2.3 (holes).** A component of $G$ that lies inside a room of another component is a *hole* of the smallest such room: its outer walk bounds the room from within. The room's area is its face's less its holes'.

**Proposition 2.4.** The rooms' areas sum to the area enclosed by the outer walks of the components that lie in no room. *Proof.* Each such region is tiled by the bounded faces of its component and, recursively, of the components within them. ∎

**Definition 2.5 (sides).** Each wall has the room on its left and the room on its right, or the outside. It *divides nothing* when the two are the same. A use is *indoor* or not (a court, a terrace); a wall is an *outer wall* when exactly one of its sides is an indoor room.

*Remark.* Because walls are stored and rooms are read, a wall shared by two rooms is one wall, and adjacency is a property of the drawing. Room planners that store rooms must keep two copies of each shared wall equal.

## 3. Areas inside the Walls

**Definition 3.1.** The *net area* of a room with ring $r$ is the area of $\{p \in r : d(p, e_k) \ge t_k / 2 \text{ for every edge } e_k\}$: what is left inside the walls' faces.

**Proposition 3.2.** While the mitred offset of $r$ by $(t_k / 2)_k$ is a simple ring none of whose edges vanishes, it is that set (physis, `inset`). For uniform $d = t/2$ its area is
$$A - d\,P + d^2 \sum_i \tan(\varphi_i / 2),$$
$P$ the perimeter and $\varphi_i$ the exterior angle at vertex $i$ (negative at a reflex vertex). For an L of six right angles the sum is $5 - 1 = 4$.

Where a room has holes, or its walk repeats a vertex (a wall standing in it), the first-order area $A - \sum_k L_k t_k / 2$ is given, each face of a wall that divides nothing counted, and is said to be approximate.

**Definition 3.3 (width).** The *width* of a room is twice the largest $d$ for which the uniform inset of its net ring by $d$ is not empty. For a convex room it is the diameter of the largest circle the room holds, and for a rectangle its lesser side. For a room with reflex corners the mitred inset is a subset of the true one, so the width is a bound from below on that diameter: for an L, its wider arm.

## 4. Light, Air and Access

**Definition 4.1.** An opening on an outer wall *lights* the indoor room on its inner side by its area times its glazed share, and *airs* it by its area times its openable share. A room's light and air are those sums as shares of its net area.

**Definition 4.2 (access graph).** Its vertices are the outside and every room of every storey. Two spaces are joined when a door stands on a wall between them, when an open wall lies between them, or when a stair has its foot in one and its head in the other.

**Definition 4.3.** The *depth* of a room is its distance from the outside in the access graph. A room $x$ is *reached through* $y$ when every path from the outside to $x$ passes $y$ (a dominator). The *relative asymmetry* of a space among the $k$ spaces reached is
$$RA = \frac{2\,(MD - 1)}{k - 2},$$
$MD$ its mean depth to the other $k - 1$ (Hillier and Hanson 1984, *The Social Logic of Space*, 108–109).

**Proposition 4.4.** $0 \le RA \le 1$, with $RA = 0$ for the hub of a star and $RA = 1$ for an end of a chain. *Proof.* $MD \ge 1$, with equality when every other space is adjacent; and the total depth from a vertex of a connected graph on $k$ vertices is at most $1 + 2 + \dots + (k - 1)$, attained from the end of a path, so $MD \le k/2$. ∎

## 5. Rules

**Definition 5.1.** A *limit* is a pair of bounds, at least and at most, either of which may be absent, with the clause that states it. A limit with neither is *not stated*. A *finding* is one rule met or not by one subject: what was asked, what was found, the unit, and a note.

A scheme states limits; a rule not stated is not evaluated. The domain holds the forms, and no jurisdiction's numbers.

**The forms of habitation** (`habitability.hpp`): a ceiling's height; glass and openable area as shares of a habitable room's floor; the clear width of a door and of an entrance; the width of a passage; a stair's riser $R$, its going $G$ and $2R + G$, each within a range.

**The forms of siting** (`siting.hpp`), for a building given storey by storey as its outer walls:

- a height and a number of storeys;
- a street setback, the least distance of a wall's face from an edge that faces a street;
- a side setback growing with a wall's height $h$: $s(h) = s_0 + \sum_i m_i \operatorname{clamp}(h - a_i, 0, b_i - a_i)$;
- walls on a boundary: a wall within a tolerance of an edge stands on it and is exempt from the setback; an edge of length $L$ may carry $b + q\,(L - b)$ of wall, the whole edge when $L \le b$;
- coverage, open space, and a garden share by the lot's size.

*Caveat.* A wall's distance from an edge is taken at the wall's ends, which is the least distance unless a corner of the lot points into the wall. Coverage adds half the outer walls' thickness along their length to the floor and leaves out the corners' squares ($4 (t/2)^2$ for a rectangle). Both are stated on their findings.

## 6. The Brief

**Definition 6.1.** A *brief* is a labelled graph $B$: a vertex for each space asked for, labelled by a use and a least area; an edge for each relation between uses, labelled *opens to*, *adjoins*, *apart* or *not through*.

**Definition 6.2.** A building *answers* $B$ when there is an injective map $\varphi$ from the vertices of $B$ to rooms of the same use and at least the area asked, such that for an edge $(a, b)$: *opens to* means $\varphi(a)$ and $\varphi(b)$ are joined in the access graph; *adjoins*, that they share a wall; *apart*, that they are not joined; *not through*, that $\varphi(a)$ is not reached through $\varphi(b)$.

Relations are stated between uses and read universally over the rooms of the first use ("no bedroom is reached only through a bedroom"). With that reading the map exists exactly when each statement holds: the needs give the injection by counting, and each relation is a condition on every room of a use. So the brief is checked statement by statement, each with its reason (`programme::check`).

## 7. Quantities, Cost and Heat

Quantities are sums over the drawing: floors to the walls' centrelines; outer walls by length and storey height, gross and net of their openings, by the side their outward normal faces; inner walls by length and ceiling height; the roof as what each storey covers that the next does not, $\sum_k \max(0, A_k - A_{k+1})$, exact when the storeys nest.

**Proposition 7.1.** For quantities $q_i \ge 0$ and rates in $[\ell_i, u_i]$ with $0 \le \ell_i \le u_i$, the cost lies in $[\sum q_i \ell_i, \sum q_i u_i]$, and both ends are attained. ∎

The fabric's transmission coefficient is $H = \sum U A$ over outer walls, windows, outer doors, the roof in plan and the ground slab (W/K). Thermal bridges and the air exchanged are left out.

## 8. What Stands in the Rooms

**Definition 8.1 (piece).** A *piece* is a rectangle on a storey's floor: a middle $c$, a bearing $\beta$ (the direction its front faces), a width $w$ across its front and a depth $d$; with the *clear floor* it needs, a depth $f \ge 0$ before it and $s \ge 0$ at each side, or $a > 0$ all round; and whether it *obstructs*. In its frame, $u = (\sin\beta, -\cos\beta)$ to its right and $v = (\cos\beta, \sin\beta)$ to its front, its footprint is $\{c + x u + y v : |x| \le w/2,\ |y| \le d/2\}$, counter-clockwise, and its floors are the rectangles adjoining it.

**Definition 8.2.** In the room its middle lies in, a piece is

- *within* when its footprint lies in the room's net ring (Definition 3.1) and outside its holes, crossing neither;
- *apart* when its footprint shares no area with another obstructing piece's. For convex rings the shared area is that of one ring clipped by each edge of the other (Sutherland and Hodgman 1974);
- *usable* when its floors lie in the room and share no area with an obstructing piece's footprint. Two pieces' floors may share area;
- *reached*, for a person who is a disc of diameter $p$, when a point of $E \ominus B_{p/2}$ joined within that set to a door of the room lies in its floors grown by $p/2$. Here $E$ is the room's free floor, its net ring less the obstructing footprints and the stairs, and $\ominus$ is erosion by the disc.

**Proposition 8.3 (monotone in the person).** If a piece is reached for a diameter $p$, it is reached for every $p' < p$. *Proof.* Let $q \in E \ominus B_{p/2}$ be such a point and $y$ the point of the floors nearest to it, so that $|q - y| \le p/2$. Since $B(q, p/2) \subseteq E$, every point $q'$ of the segment from $q$ toward $y$ with $|q - q'| \le (p - p')/2$ has $B(q', p'/2) \subseteq B(q, p/2) \subseteq E$. So that stretch of the segment lies in $E \ominus B_{p'/2}$, which contains $E \ominus B_{p/2}$ and with it the path from the door; and its end is within $p'/2$ of $y$. ∎

*Caveat.* The domain computes $E$ on a grid of 5 cm, the cells' middles half a cell off the room's corner, the erosion by eikon's exact distance transform between middles, and joins cells by their edges. A gap within one cell of $p$ may be found either way. The floors and the diameter are a catalogue's or a person's to state; none is a standard's.

**Doors.** A door's leaf sweeps the square on its span, of its width, on the side it swings to: the wall's left, or the indoor side of an outer wall, unless the opening says otherwise. A piece that shares area with that square stands in the leaf's way.

## 9. The Shell

**Definition 9.1 (footprint).** Direct each outer wall of a storey so that the indoor room is on its left. The directed walls decompose into closed walks (at a vertex where more than two meet, the leftmost turn is taken, so that two parts of a building touching at a corner give two walks). Each walk is a ring of the storey's *footprint*: counter-clockwise when it bounds a part of the building, clockwise when it bounds a court within it. The ring is first on the walls' centrelines; its edges moved to their right by half their walls' thickness, and consecutive lines met, give the ring at the walls' *outer faces* (a mitred offset, kept when it is a simple ring of the same sense whose edges keep their directions). Consecutive walls in one line and of one thickness are one edge.

**Definition 9.2 (roof of equal slope).** Let $P$ be a simple ring, the *eaves*, and $h : P \to \mathbb{R}_{\ge 0}$ the time at which the mitred wavefront, moving inward from every edge of $P$ at unit speed, reaches a point: the straight skeleton of $P$ read as a height (physis, `roof`; Aichholzer, Aurenhammer, Alberts and Gärtner 1995). The *roof of pitch* $\theta$ over $P$ from a level $z_0$ is the surface $z = z_0 + \tan\theta \, h(p)$. On the *face* of an edge $e$, the set its part of the wavefront sweeps, $h$ is the distance to the line of $e$. An edge marked a *gable* does not move: it has no face, its neighbours' faces run on to it, and the wall beneath it is carried up to the roof.

**Proposition 9.3 (area).** The area of a roof of pitch $\theta$ over $P$, gables or none, is $\operatorname{area}(P) / \cos\theta$. *Proof.* The faces tile $P$, and on each the surface is a plane whose gradient has length $\tan\theta$: its area element is $\sqrt{1 + \tan^2\theta}\, dA = dA / \cos\theta$. ∎ The same holds for a shed, one plane of that pitch over $P$.

**Definition 9.4 (a roof on walls).** Let $F$ be a footprint ring at the outer faces, $o \ge 0$ an overhang, and $P$ the ring $F$ moved out by $o$ (kept when it is simple). The roof *stands on the walls* from a plate at height $z_p$ when $z = z_p + \tan\theta\,(h(p) - o)$.

**Proposition 9.5.** Where $P$ is that offset and the offsets of $F$ by every $s \le o$ are simple rings, every point of $F$ has $h \ge o$, with $h = o$ on an edge of $F$ whose eave is not a gable, away from its ends' neighbours' faces: the roof passes through the top of the walls' outer faces, lies nowhere below the plate within them, and its edge hangs $o \tan\theta$ below the plate. *Proof.* $F$ is the mitred inset of $P$ by $o$ without an event, that is, the wavefront of $P$ at time $o$; so $h = o$ on $F$ where every eave moves. A gable's line does not move, which only delays the wavefront: $h \ge o$ on $F$ still. ∎

**Definition 9.6 (the walls' rise).** Along an edge of $F$, the roof's height above the plate is a polyline with a corner wherever the edge passes from one face to the next. Where it is positive the wall is carried up: beneath a gable (a triangle over a rectangle's end), beneath a shed (a trapezium or a triangle), and about a flat roof (a parapet).

*Caveat.* physis accepts a gable only on an eave that is an edge of the convex hull of $P$ between two convex corners, where no part of $P$ lies beyond its line; elsewhere the eave is made to rise and the roof says so. A roof stands on the top storey alone: what a lower storey covers that the one above does not is a flat roof at that storey's height. A court is not roofed over, nor does the roof about it fall toward it.

## 10. Sun, Sky and Daylight at the Glass

**Definition 10.1 (pane).** A glazed opening of an outer wall is a *pane*: a rectangle in the wall's outer face with a horizontal outward normal $n$. What stands about it is a set of prisms on flat ground, the neighbours and the building's own footprints, each to its storey's top. An eave above the pane's wall is an *overhang*: a ledge of depth $o$ whose underside is at height $z_o$.

**Proposition 10.2 (the overhang).** Let the Sun stand at altitude $\alpha$ in a horizontal direction $u$ with $n \cdot u > 0$. The ray from a point of the pane at height $z < z_o$ passes under the ledge's edge, and so is cut off by the ledge, exactly when $\dfrac{z_o - z}{\tan\alpha}\,(n \cdot u) < o$. *Proof.* The ray gains height $\tan\alpha$ per unit of horizontal run; it reaches $z_o$ after a run $(z_o - z)/\tan\alpha$, of which the part square to the face is the run times $n \cdot u$. ∎

**Definition 10.3.** A pane's *hours of sun* over instants with weights $w_k$ are $\sum_k w_k\, s_k$, $s_k$ the share of its sample points (nine: three across, three up) that the Sun is in front of and whose rays clear every prism and the overhang. Its *direct energy* on a clear day is $\sum_k I(\alpha_k) \cos\alpha_k\, (n \cdot u_k)\, s_k\, w_k$ per square metre of glass, $I(\alpha) = 1353 \times 0.7^{(1/\sin\alpha)^{0.678}}$ W/m² (Meinel and Meinel 1976). Diffuse and reflected light are left out: the energy is a bound from below.

**Definition 10.4 (angle of visible sky).** In the vertical plane through the pane's middle along $n$, the sky is hidden below $\varepsilon_{\mathrm{low}} = \max \arctan\dfrac{H - z}{t}$ over the prisms the ray along $n$ enters at a distance $t$, and above $\varepsilon_{\mathrm{high}} = \arctan\dfrac{z_o - z}{o}$ under an overhang ($90°$ without one). The *angle of visible sky* is $\theta = \max(0, \varepsilon_{\mathrm{high}} - \varepsilon_{\mathrm{low}})$.

**Definition 10.5 (average daylight factor).** For a room with windows of glass areas $A_w$ and angles $\theta_w$ in degrees, surfaces of total area $A$ (floor, ceiling and walls, the windows among them) and mean reflectance $R$, and glass of diffuse transmittance $T$,

$$\overline{DF} = \frac{T \sum_w A_w \theta_w}{A\,(1 - R^2)} \ \text{per cent}$$

(Crisp and Littlefair 1984; BRE Digest 309). It estimates the mean illuminance on the working plane as a share of that on unobstructed ground under an overcast sky, for a room lit from the side that is not deep.

*Caveat.* The formula is an estimate fitted to model rooms, not a theorem. $T$ and $R$ are declared values (0.68 and 0.5 unless stated). BS 8206-2, which read 2% as a room that looks daylit and 5% as one needing no lamps by day, is withdrawn for BS EN 17037:2018, which asks for illuminances reached over a share of the year; a limit is a scheme's to state. The roof above the plate is not among the prisms.

## 11. How High a Building May Rise

**Definition 11.1.** Let receivers $r$ (points where the sun is wanted), instants $k$ with weights $w_k$, and prisms of which some are variables with heights $x_v$. For a receiver on the ground the ray toward the Sun enters a prism's footprint after a horizontal run $t$; the prism shades the receiver at that instant exactly when its height exceeds the *threshold* $\vartheta = t \tan\alpha_k$ (architecture, `sun.hpp`). The receiver's hours are $S_r(x) = \sum_k w_k\,[\,x_v \le \vartheta_{rkv} \text{ for every } v\,]$ over the instants at which nothing fixed shades it.

**Proposition 11.2 (one building's limit).** Fix every height but $x_v$. Then $S_r$ is a step function of $x_v$ that never rises, and for a need $N \le S_r(x)$ the greatest height at which $S_r \ge N$ is

$$\bar{x}_v(r) = \vartheta_{(j)}, \qquad j = \min\Big\{ i : c_r + \sum_{l \le i} w_{(l)} \ge N \Big\},$$

where $c_r$ are the hours $r$ has whatever $x_v$ is, the instants at which $v$ alone can shade $r$ are ordered by threshold, $\vartheta_{(1)} \ge \vartheta_{(2)} \ge \dots$, and $\bar{x}_v(r) = \infty$ when $c_r \ge N$. *Proof.* An instant at which another prism shades $r$ is dark whatever $x_v$; one at which $v$ is not on the ray is counted in $c_r$; each of the others is lit exactly while $x_v \le \vartheta$. At height $x$ the lit ones are those with $\vartheta \ge x$, an initial segment of the order; the least segment whose weights reach $N - c_r$ ends at $\vartheta_{(j)}$. ∎

**Definition 11.3 (limit).** A receiver is *protected* when it has $N$ hours as things stand. The *limit* of $v$ is $\min_r \bar{x}_v(r)$ over the protected receivers: the greatest height at which each keeps its $N$ hours, the others as they stand. It is never below $v$'s present height.

*Remark.* This is Knowles's solar envelope (1981) for one building and a set of points. Limits are not jointly attainable: two buildings each raised to its limit may together take an hour that neither takes alone, since the instants each may take can differ. A design of several heights is checked whole.

## 12. What Is Left Out

Curved walls; ceilings that slope; stairs that turn, and the void a stair needs in the floor above; a roof with dormers, a mansard or a roof over a court; the heat of a day hour by hour; light traced from the sky and reflected; the rules that need the neighbours; pieces that are not rectangles; a piece under another; what a fixture is connected to.
