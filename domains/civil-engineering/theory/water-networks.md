# Water Networks

*Status: draft (2026-11-13). The theory note of the civil-engineering domain's `water` module ([ADR 0107](../../../development/decisions/0107-civil-engineering-founded-water-networks.md)).*

## 1. Concepts

A water distribution network delivers water from sources (reservoirs, tanks) to junctions, where it is drawn, through pipes that lose head by friction and pumps that add it. Head is the energy per unit weight of water, in metres. At a junction, the head less its elevation is the pressure, also in metres of water.

The network's service is what each junction receives, and at what pressure. That is the difference the handbook draws between a resource's presence and its access (its §1.1).

## 2. The Network

**Definition 2.1.** A network is a graph:

- its vertices are junctions (elevation z_i, demand q_i), reservoirs (head H_r) and tanks (elevation, level, area);
- its edges are links with a law of head loss h_e(Q), increasing in the flow:
  - pipes, by Hazen–Williams or Darcy–Weisbach with fittings' losses;
  - pumps, a negative loss along their curves;
  - check valves, closing to reverse flow.

EPANET's input files declare networks so (Rossman et al. 2020, Appendix C), and the module reads them, converting to SI at reading.

## 3. Hydraulics at an Instant

The heads H and flows Q satisfy H_a − H_b = h_e(Q_e) on every open link, and continuity at every junction, with sources' heads fixed. This is physis's network of conduits (its decision 0029). There the solution is shown to exist and be unique for increasing laws, and to minimise the content; it is found by Todini and Pilati's global gradient algorithm.

**Statuses.** Pumps and check valves settle by rounds:

- an open one whose flow would reverse closes;
- a closed pump opens when the head against it falls below its shutoff head;
- a closed check valve opens when its upstream head exceeds its downstream head.

The rounds end when no status changes. EPANET does the same within its iterations (Manual, §12.2.2).

**Pressure-driven demand.** Under demand-driven analysis every connected junction draws its demand, whatever its pressure. Under pressure-driven analysis (Wagner et al. 1988) a junction draws
$$q = q_{\mathrm{req}}\sqrt{\frac{p - p_{\min}}{p_{\mathrm{req}} - p_{\min}}}$$
between p_min and p_req, nothing below and q_req above. The module writes this as an emitter: a link from the junction to a fixed head z + p_min whose law is the inverse of Wagner's, increasing.

The inverse is steepened beyond its ends, so it stays increasing and the solution unique. Delivery then departs from Wagner's law by less than 10⁻⁴ q_req, which is checked.

**Junctions cut off from every source.** Under pressure-driven analysis they stay in the system, so that what they lose is counted. A junction that asks for water is tied to its fixed head by its emitter. A junction that asks for nothing is tied to a fixed head at its elevation by a link of unit conductance.

Since nothing in such an island draws, no flow passes its ties, and their strength is immaterial to the answer. A tie must still not be faint. The island's pipes carry nothing, so their slopes sit at the floor and their weights near 10⁹, and beside them a faint tie is lost to rounding and the equations become singular. Closing one pipe of EPANET's Net3 behind a dead end of junctions with no demand made exactly such an island.

## 4. Service

**Age.** In a steady state, water runs from the sources downhill in head, except through pumps. So the flow graph has no directed cycle that does not pass through a pump, and the ages are found by sweeping:
$$a_v = \frac{\sum_{e \to v} Q_e (a_{u(e)} + L_e A_e / |Q_e|)}{\sum_{e \to v} Q_e}.$$
EPANET's transport over time converges to these ages when the flows stay steady.

**Resilience.** Todini's index is the share of the power that could be spared above the required heads that is spared:
$$I_r = \frac{\sum_i q_i (h_i - h_i^*)}{\sum_r Q_r H_r + \sum_k Q_k \Delta H_k - \sum_i q_i h_i^*}.$$
It is 1 when nothing is dissipated, and falls as the network dissipates the head it is given.

**Criticality.** Each link is closed in turn under pressure-driven demand, and the delivered share is recorded. A branch whose loss cuts a junction off costs that junction's demand exactly. A link within a loop costs only what the remaining paths cannot carry at the required pressure.

## 5. Validation

- EPANET 2.2's own solution of its example network Net1 at its first instant, through WNTR: heads and pressures to a millimetre, flows to 10⁻⁶ m³/s (EPANET writes single precision).
- Darcy–Weisbach on one pipe against Swamee and Jain's friction factor.
- Wagner's law at the solved pressure.
- A pump that cannot lift, a check valve against reverse flow, an isolated junction.
- Ages down a chain, Todini's index by hand.
- Net1's day: the pattern's multipliers, and the tank moving by its net inflow.
- Criticality: a branch costs its demand exactly, and a loop's pipe costs nothing; an island of junctions that draw nothing, cut off behind a closed pipe, is solved and costs nothing.

## 6. References

- Rossman, L. A. et al. (2020), *EPANET 2.2 User Manual*, EPA/600/R-20/133.
- Rossman, L. A. and Boulos, P. F. (1996), Numerical methods for modeling water quality in distribution systems, *J. Water Resour. Plan. Manage.* 122, 137–146.
- Todini, E. and Pilati, S. (1988), A gradient algorithm for the analysis of pipe networks.
- Todini, E. (2000), *Urban Water* 2, 115–122.
- Wagner, J. M., Shamir, U. and Marks, D. H. (1988), *J. Water Resour. Plan. Manage.* 114, 276–294.
- Klise, K. A. et al. (2017), *Environ. Model. Softw.* 95, 420–431.
