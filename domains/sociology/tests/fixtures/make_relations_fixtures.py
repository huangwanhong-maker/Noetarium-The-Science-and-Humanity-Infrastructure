"""Reference values for sociology's `relations` checks, computed by networkx on Zachary's karate club.

Run (networkx 3.x on the path):  python domains/sociology/tests/fixtures/make_relations_fixtures.py
Writes karate.txt beside this script: the edges, the club of each member, and networkx's values.

Source of the network: Zachary, W. W. (1977), An information flow model for conflict and fission in small groups,
Journal of Anthropological Research 33, 452-473; as networkx ships it (karate_club_graph, unweighted).
"""
from pathlib import Path

import networkx as nx

G = nx.karate_club_graph()
for u, v in G.edges():
    G[u][v].pop("weight", None)
club = [0 if G.nodes[i]["club"] == "Mr. Hi" else 1 for i in G.nodes()]
nx.set_node_attributes(G, {i: club[i] for i in G.nodes()}, "c")

lines = [
    "# Zachary's karate club (networkx karate_club_graph, unweighted); values by networkx " + nx.__version__,
    "# Retrieved 2026-11-13. Lines: nodes n; edge a b; club i c (0 Mr. Hi, 1 Officer); then name value [values ...]",
    f"nodes {G.number_of_nodes()}",
]
lines += [f"edge {u} {v}" for u, v in sorted(G.edges())]
lines += [f"club {i} {c}" for i, c in enumerate(club)]
lines.append(f"density {nx.density(G):.17g}")
lines.append(f"transitivity {nx.transitivity(G):.17g}")
lines.append(f"average_clustering {nx.average_clustering(G):.17g}")
lines.append(f"degree_assortativity {nx.degree_assortativity_coefficient(G):.17g}")
lines.append(f"club_assortativity {nx.attribute_assortativity_coefficient(G, 'c'):.17g}")
cons = nx.constraint(G)
lines.append("constraint " + " ".join(f"{cons[i]:.17g}" for i in G.nodes()))
eff = nx.effective_size(G)
lines.append("effective_size " + " ".join(f"{eff[i]:.17g}" for i in G.nodes()))
core = nx.core_number(G)
lines.append("core " + " ".join(str(core[i]) for i in G.nodes()))
tri = nx.triangles(G)
lines.append("triangles " + " ".join(str(tri[i]) for i in G.nodes()))
# Two steps: the others reached within two ties, for each member.
two = [len(nx.single_source_shortest_path_length(G, i, cutoff=2)) - 1 for i in G.nodes()]
lines.append("reach2 " + " ".join(str(t) for t in two))

(Path(__file__).parent / "karate.txt").write_text("\n".join(lines) + "\n", encoding="utf-8")
print("ok", G.number_of_nodes(), G.number_of_edges())
