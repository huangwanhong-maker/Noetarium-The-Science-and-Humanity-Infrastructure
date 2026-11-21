"""The site of Study C: a fragment of the theoretical corpus declared so that procedures decide its claims.

The fragment is Section 1 of the theory note of the economics domain. A transfer network is a finite directed
multigraph of positions and legs, each leg carrying a transport U > 0. The holonomy of a closed walk is the product
of the transports of its legs, a leg walked against its direction counting as 1/U, and its loop deficit is minus the
logarithm of the holonomy. A network is priced when a potential exists whose ratios are the transports.

Everything is computed with exact rationals. A vocabulary of properties is declared, each with a procedure, and
three kinds of claim follow (PLAN.md, Section 4.2): *implies*, refuted by a network; *separates*, decided by the
witness it carries; *determined*, refuted by one network with two admissible flows.
"""
from __future__ import annotations

import itertools
import random
from fractions import Fraction

MAX_POSITIONS, MAX_LEGS = 6, 12


class Malformed(Exception):
    """A network, a cycle or a flow that is not of the declared form."""


def rational(value) -> Fraction:
    if isinstance(value, bool):
        raise Malformed("a transport is a positive rational")
    if isinstance(value, int):
        out = Fraction(value)
    elif isinstance(value, float):
        out = Fraction(str(value))
    elif isinstance(value, str):
        try:
            out = Fraction(value.strip())
        except (ValueError, ZeroDivisionError) as error:
            raise Malformed(f"not a rational: {value!r}") from error
    else:
        raise Malformed(f"not a rational: {value!r}")
    if out <= 0:
        raise Malformed("a transport is positive")
    if out.numerator > 10**6 or out.denominator > 10**6:
        raise Malformed("a rational beyond a million in numerator or denominator")
    return out


def network(obj) -> tuple[int, tuple]:
    """A network from its written form {"positions": n, "legs": [[from, to, transport], ...]}; positions are 0 to n - 1."""
    if not isinstance(obj, dict) or not isinstance(obj.get("positions"), int) or isinstance(obj.get("positions"), bool) or not isinstance(obj.get("legs"), list):
        raise Malformed('a network is written {"positions": n, "legs": [[from, to, transport], ...]}')
    n = obj["positions"]
    if not 1 <= n <= MAX_POSITIONS or len(obj["legs"]) > MAX_LEGS:
        raise Malformed(f"at most {MAX_POSITIONS} positions and {MAX_LEGS} legs")
    legs = []
    for leg in obj["legs"]:
        if not isinstance(leg, list) or len(leg) != 3:
            raise Malformed("a leg is [from, to, transport]")
        u, v, transport = leg
        if not all(isinstance(x, int) and not isinstance(x, bool) for x in (u, v)) or not (0 <= u < n and 0 <= v < n) or u == v:
            raise Malformed("a leg joins two different positions of the network")
        legs.append((u, v, rational(transport)))
    return n, tuple(legs)


def written(net) -> dict:
    n, legs = net
    return {"positions": n, "legs": [[u, v, str(U)] for u, v, U in legs]}


# ---- walks and their holonomies -----------------------------------------------------------------------------

def _steps(net, directed: bool):
    n, legs = net
    out = [[] for _ in range(n)]
    for index, (u, v, _) in enumerate(legs):
        out[u].append((v, index, 1))
        if not directed:
            out[v].append((u, index, -1))
    return out


def cycles(net, directed: bool = False, longest: int | None = None):
    """The simple closed walks of a network, each from its least position, as lists of (leg, sense): a walk visits no
    position twice and uses no leg twice. With `directed`, legs are walked only along their direction."""
    n, legs = net
    steps = _steps(net, directed)
    for start in range(n):
        stack = [(start, [], frozenset([start]), frozenset())]
        while stack:
            at, path, seen, used = stack.pop()
            for to, index, sense in steps[at]:
                if index in used:
                    continue
                if to == start and len(path) >= 1:
                    yield path + [(index, sense)]
                elif to > start and to not in seen and (longest is None or len(path) + 1 < longest):
                    stack.append((to, path + [(index, sense)], seen | {to}, used | {index}))


def holonomy(net, walk) -> Fraction:
    _, legs = net
    out = Fraction(1)
    for index, sense in walk:
        out *= legs[index][2] if sense == 1 else 1 / legs[index][2]
    return out


def potential(net):
    """A potential p > 0 with p(head) = p(tail) U on every leg, found along a spanning forest, or None when none exists."""
    n, legs = net
    steps = _steps(net, False)
    p: list[Fraction | None] = [None] * n
    for root in range(n):
        if p[root] is not None:
            continue
        p[root] = Fraction(1)
        queue = [root]
        while queue:
            at = queue.pop()
            for to, index, sense in steps[at]:
                if p[to] is None:
                    p[to] = p[at] * legs[index][2] if sense == 1 else p[at] / legs[index][2]
                    queue.append(to)
    return p if all(p[v] == p[u] * U for u, v, U in legs) else None


def _components(net) -> int:
    n, legs = net
    parent = list(range(n))

    def find(x):
        while parent[x] != x:
            parent[x] = parent[parent[x]]
            x = parent[x]
        return x

    for u, v, _ in legs:
        parent[find(u)] = find(v)
    return len({find(x) for x in range(n)})


def _strongly_connected(net) -> bool:
    n, legs = net

    def reach(forward: bool) -> int:
        seen, queue = {0}, [0]
        while queue:
            at = queue.pop()
            for u, v, _ in legs:
                a, b = (u, v) if forward else (v, u)
                if a == at and b not in seen:
                    seen.add(b)
                    queue.append(b)
        return len(seen)

    return reach(True) == n and reach(False) == n


# ---- the vocabulary -----------------------------------------------------------------------------------------

def _pairs(net) -> dict:
    out: dict = {}
    for u, v, U in net[1]:
        out.setdefault((min(u, v), max(u, v)), []).append(U if u < v else 1 / U)
    return out


PROPERTIES = {
    "priced": ("there are prices: a positive number p at every position such that the transport of every leg is p(head) / p(tail)",
               lambda net: potential(net) is not None),
    "flat": ("every closed walk has holonomy 1, that is, every loop deficit vanishes; legs may be walked against their direction",
             lambda net: all(holonomy(net, w) == 1 for w in cycles(net))),
    "directed_flat": ("every closed walk that follows the legs along their direction has holonomy 1",
                      lambda net: all(holonomy(net, w) == 1 for w in cycles(net, directed=True))),
    "triangles_flat": ("every closed walk through exactly three positions, on three legs walked in either direction, has holonomy 1",
                       lambda net: all(holonomy(net, w) == 1 for w in cycles(net, longest=3) if len(w) == 3)),
    "digons_flat": ("any two legs between the same two positions agree: walked from one position to the other they give the same transport",
                    lambda net: all(len(set(values)) == 1 for values in _pairs(net).values())),
    "acyclic": ("the network has no closed walk without repetition: its legs form a forest",
                lambda net: len(net[1]) - net[0] + _components(net) == 0),
    "connected": ("any two positions are joined by legs walked in either direction", lambda net: _components(net) == 1),
    "strongly_connected": ("any position is reached from any other along the direction of the legs", _strongly_connected),
    "simple": ("no two positions are joined by more than one leg", lambda net: all(len(values) == 1 for values in _pairs(net).values())),
    "complete": ("every two positions are joined by at least one leg",
                 lambda net: len(_pairs(net)) == net[0] * (net[0] - 1) // 2),
    "symmetric": ("for every leg there is a leg in the opposite direction between the same positions",
                  lambda net: all(any(a == v and b == u for a, b, _ in net[1]) for u, v, _ in net[1])),
    "unit": ("every transport equals 1", lambda net: all(U == 1 for _, _, U in net[1])),
}


def holds(net, name: str) -> bool:
    return PROPERTIES[name][1](net)


def vocabulary() -> str:
    return "\n".join(f"- {name}: {text}." for name, (text, _) in PROPERTIES.items())


# ---- the three kinds of claim -------------------------------------------------------------------------------

def counters(net, premises, conclusion) -> bool:
    """True when a network has every premise and lacks the conclusion."""
    return all(holds(net, p) for p in premises) and not holds(net, conclusion)


def well_formed(premises, conclusion) -> str | None:
    if not isinstance(premises, list) or not premises or not all(isinstance(p, str) for p in premises) or not isinstance(conclusion, str):
        return "a claim has a list of premises and one conclusion"
    unknown = [p for p in premises + [conclusion] if p not in PROPERTIES]
    if unknown:
        return f"outside the vocabulary: {', '.join(unknown)}"
    if conclusion in premises:
        return "the conclusion is among the premises"
    return None


def retentions(net, cycle, flows) -> list[Fraction]:
    """The retentions of the positions of a directed cycle: the position entered by leg i - 1 and left by leg i receives
    U(i-1) x(i-1) and passes on x(i); its retention is their ratio (the note's Proposition 1.5)."""
    n, legs = net
    if not isinstance(cycle, list) or len(cycle) < 2 or len(set(cycle)) != len(cycle) or not all(isinstance(i, int) and not isinstance(i, bool) and 0 <= i < len(legs) for i in cycle):
        raise Malformed("a cycle is a list of at least two different legs, by their places in the list of legs")
    for i, index in enumerate(cycle):
        if legs[cycle[i - 1]][1] != legs[index][0]:
            raise Malformed("the legs of a cycle follow one another along their direction")
    if len({legs[i][0] for i in cycle}) != len(cycle):
        raise Malformed("a cycle passes through each of its positions once")
    if not isinstance(flows, list) or len(flows) != len(cycle):
        raise Malformed("there is one flow for each leg of the cycle")
    x = [rational(value) for value in flows]
    return [legs[cycle[i - 1]][2] * x[i - 1] / x[i] for i in range(len(cycle))]


def refutes_determined(counterexample) -> tuple[bool, str]:
    """Decides a counterexample to the claim that the retentions of a cycle are determined by the network."""
    net = network(counterexample.get("network"))
    cycle = counterexample.get("cycle")
    first, second = retentions(net, cycle, counterexample.get("flows")), retentions(net, cycle, counterexample.get("other_flows"))
    walk = [(index, 1) for index in cycle]
    product_one, product_two = Fraction(1), Fraction(1)
    for r in first:
        product_one *= r
    for r in second:
        product_two *= r
    assert product_one == product_two == holonomy(net, walk), "the product of the retentions is the holonomy"
    if first != second:
        return True, f"one cycle, two flows: retentions {[str(r) for r in first]} and {[str(r) for r in second]}, both with product {product_one}"
    return False, "the two flows give the same retentions"


# ---- the table of small networks, and larger ones drawn at random -------------------------------------------

SMALL = (Fraction(1), Fraction(2), Fraction(1, 2))


def table():
    """Every network of at most three positions with at most four legs, every network of four positions with at most four
    legs, every network of four positions with exactly one leg between each two positions, and every network of three
    positions with exactly one leg in each direction between each two positions; transports among 1, 2 and 1/2."""
    for n in (1, 2, 3, 4):
        options = [(u, v, U) for u in range(n) for v in range(n) if u != v for U in SMALL]
        for k in range(0, 5):
            for legs in itertools.combinations_with_replacement(options, k):
                yield n, legs
    pairs = [(u, v) for u in range(4) for v in range(u + 1, 4)]
    choices = [[(u, v, U) for U in SMALL] + [(v, u, U) for U in SMALL] for u, v in pairs]
    for legs in itertools.product(*choices):
        yield 4, legs
    ordered = [(u, v) for u in range(3) for v in range(3) if u != v]
    for transports in itertools.product(SMALL, repeat=6):
        yield 3, tuple((u, v, U) for (u, v), U in zip(ordered, transports))


TABLE_EXTENT = ("every network of at most 3 positions and at most 4 legs, every network of 4 positions and at most 4 legs, every network of "
                "4 positions with exactly one leg between each two positions, and every network of 3 positions with exactly one leg in each "
                "direction between each two positions, with transports among 1, 2 and 1/2")
LARGER = (Fraction(1), Fraction(2), Fraction(3), Fraction(1, 2), Fraction(1, 3), Fraction(2, 3), Fraction(3, 2))


def larger(count: int = 20000, seed: int = 20261121):
    """Networks of four to six positions with four to ten legs, drawn at random; a fifth of them are made flat by drawing
    prices first, so that premises which need flatness are met often enough to be tested."""
    rng = random.Random(seed)
    for _ in range(count):
        n = rng.randrange(4, 7)
        k = rng.randrange(4, 11)
        prices = [rng.choice(LARGER) for _ in range(n)] if rng.random() < 0.2 else None
        legs = []
        for _ in range(k):
            u = rng.randrange(n)
            v = rng.choice([x for x in range(n) if x != u])
            legs.append((u, v, prices[v] / prices[u] if prices else rng.choice(LARGER)))
        yield n, tuple(legs)


LARGER_EXTENT = "20,000 networks of 4 to 6 positions and 4 to 10 legs drawn at random with a fixed seed"


class Tables:
    """The property of every network of the table and of the larger sample, computed once."""

    def __init__(self):
        names = list(PROPERTIES)
        self.names = names
        self.small = [(net, tuple(holds(net, name) for name in names)) for net in table()]
        self.large = [(net, tuple(holds(net, name) for name in names)) for net in larger()]

    def counter(self, premises, conclusion, which: str = "small"):
        index = [self.names.index(p) for p in premises]
        target = self.names.index(conclusion)
        met = 0
        for net, row in (self.small if which == "small" else self.large):
            if all(row[i] for i in index):
                met += 1
                if not row[target]:
                    return net, met
        return None, met
