# Transit: Service at a Stop and Who Lives within Reach of It

*Status: draft (ADR 0151). The module `urban::transit`; its checks are `transit/tests/check_transit.cpp`.*

## 1. The Concept

SDG 11.2.1 asks what share of a city's people have convenient access to public transport. UN-Habitat's metadata of 2025-04-23 defines it as the share living within walking distance along the street network of:

- 500 m of a stop of a low-capacity system (bus, BRT);
- or 1 km of a stop of a high-capacity system (rail, metro, ferry).

Frequency, safety and access for people with disabilities are named as further criteria but kept out of the core computation. The metadata's secondary indicator recommends an average wait at the peak of at most 20 minutes.

**Open terms.** The definition leaves open terms, each with more than one reading:

- which class a tram belongs to, since it is in neither list;
- whether a stop with rare service counts;
- the reference point (a home);
- distances along the network or in a straight line.

The module computes the share under each declared reading and never calls one of them the indicator's value. This is the treatment of open terms elsewhere in the infrastructure (tekmerion's `construal`).

## 2. Service at a Stop

A stop's departures in its service day $t_1 < \dots < t_n$, in seconds (past midnight as written), are read over a window $[a, b)$. A passenger arriving at an instant $u$ uniform on $[a, b)$ waits $W(u) = \min\{t_k : t_k \ge u\} - u$.

**Proposition 1.** Let $t_{j}$ be the first departure at or after $a$ and $t_{m}$ the first at or after $b$, both existing. Write $t_{j-1}^* = a$ and $t_k^* = t_k$ for $j \le k \le m$. Then
$$E[W] = \frac{1}{b-a}\sum_{k=j}^{m} \frac{(t_k - t_{k-1}^*)^2 - \max(0, t_k - b)^2}{2}.$$

*Proof.*

1. On $[t^*_{k-1}, t_k) \cap [a, b)$, $W(u) = t_k - u$.
2. For $k < m$ the piece lies inside the window and contributes $\int (t_k - u)\,du = (t_k - t^*_{k-1})^2/2$.
3. For $k = m$ the piece is $[t^*_{m-1}, b)$, contributing $[(t_m - t^*_{m-1})^2 - (t_m - b)^2]/2$. ∎

**Corollaries.**

- For a regular headway $h$, $E[W] = h/2$.
- With no departure inside the window but one at $t > b$, $E[W] = t - (a+b)/2$.

**Where the day ends.** Where the day's last departure precedes $b$, arrivals after it are not served, and the mean wait is infinite and said to be. The longest wait is the longest gap from the window's start.

The check holds Proposition 1 against the mean over 200,000 arrivals on 50 random timetables, to $10^{-4}$.

## 3. Reach

**Classes.** A reading assigns each mode a class: low, high or none. A stop counts for a class if a mode of that class serves it. Where frequency is asked, the departures of all its modes of that class, merged, must also leave a mean wait at the peak no longer than the reading allows.

**A place is within reach** if its distance to the nearest stop of low capacity is at most the low distance (500 m), or its distance to the nearest stop of high capacity is at most the high distance (1 km).

**Along the network.** The walking ways form urban science's access network, at 1 m/s so that times are metres.

- Each way is densified to points no more than 20 m apart. A place and a stop join the network at the nearest joinable point (access's `join`), which on a long straight segment would otherwise be far.
- One search from all stops of a class at once gives every place's distance (access's `nearest_supply`, on morphe's Dijkstra).

**In a straight line.** The least Euclidean distance.

**Proposition 2.** For one place, one reading and one class, the straight-line distance is at most the network distance. So the share in a straight line is at least the share along the network.

*Proof.*

1. A path of the network from a place's joining point to a stop's has length at least the Euclidean distance between the two points.
2. The walks from the place to its joining point, and from the stop's joining point to the stop, add at least the Euclidean distances spanned.
3. The triangle inequality bounds the whole by the place-to-stop distance. ∎

**Monotone in the readings.** Reading a tram as of high capacity moves its stops from the 500 m class to the 1 km class. Since every other stop keeps its class, no place loses reach, so the share does not fall. The suite's check of Melbourne relies on both propositions.

## 4. What Is Not Here

- disaggregation by sex, age and disability, which the metadata asks for and which needs people by group at the places;
- the Degree of Urbanisation's delineation of the analysis area;
- informal transport without fixed stops or schedules, which the metadata says escapes the frequency criterion;
- safety at stops and step-free access;
- affordability as the share of income spent on transport by the poorest quintile;
- peak waits weighted by passengers.

## References

- UN-Habitat. SDG indicator metadata, 11.2.1, last updated 2025-04-23. https://unstats.un.org/sdgs/metadata/
- GTFS Schedule reference. https://gtfs.org/documentation/schedule/reference/
