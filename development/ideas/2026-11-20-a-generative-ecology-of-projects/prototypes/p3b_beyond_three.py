"""Prototype 3, a note. What the fact `no_small_countermodel` leaves open.

The theory site supports a claim "P implies Q" by searching every magma of up to three elements. With the two
laws added in the third run, sixteen implications pass that search. This script shows that six of them are
false: the magma on the integers modulo 7 with x*y = 5x + 3y satisfies H and I and violates A, D and E. A
claim of the kind `implies` could be refuted on this site only by a magma of at most four elements, so none of
the six could have been refuted there, and each would have stood with a fact in its support.

    python p3b_beyond_three.py        writes results/p3-beyond-three.json
"""
import json
from pathlib import Path

N = 7
M = [[(5 * x + 3 * y) % N for y in range(N)] for x in range(N)]
E = range(N)
LAWS = {
    "A: x*y = y*x": all(M[x][y] == M[y][x] for x in E for y in E),
    "D: x*(x*y) = y": all(M[x][M[x][y]] == y for x in E for y in E),
    "E: (x*y)*y = x": all(M[M[x][y]][y] == x for x in E for y in E),
    "H: x*(y*x) = y": all(M[x][M[y][x]] == y for x in E for y in E),
    "I: (x*y)*x = y": all(M[M[x][y]][x] == y for x in E for y in E),
}
record = {"magma": "the integers modulo 7 with x*y = 5x + 3y", "satisfies": LAWS,
          "refuted_in_general": ["H implies A", "H implies D", "H implies E", "I implies A", "I implies D", "I implies E"],
          "note": "each of these holds in every magma of up to three elements"}
out = Path(__file__).resolve().parent / "results" / "p3-beyond-three.json"
out.write_text(json.dumps(record, indent=1), encoding="utf-8")
print(json.dumps(LAWS, indent=1))
