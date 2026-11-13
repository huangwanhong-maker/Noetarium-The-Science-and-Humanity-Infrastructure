"""Reference values for the water module: EPANET's own solution of its example network Net1 at the first instant.

EPANET 2.2 (US EPA, public domain), run through WNTR 1.5 (Klise et al. 2018, BSD licence; used here only to compute reference
numbers) on Net1.inp (EPANET's example network 1, public domain, copied beside this script from WNTR's library). WNTR reports in SI:
heads and pressures in m, demands and flows in m³/s. Written to water_net1_t0.txt as "node <id> <head> <pressure> <demand>" and
"link <id> <flow>".

Run:  PYTHONPATH=<a folder holding wntr 1.5> .venv/Scripts/python domains/civil-engineering/tests/fixtures/make_water_fixtures.py
(generated 2026-11-13).
"""
import tempfile
from pathlib import Path

import wntr

HERE = Path(__file__).resolve().parent
wn = wntr.network.WaterNetworkModel(str(HERE / "Net1.inp"))
wn.options.time.duration = 0
wn.options.hydraulic.accuracy = 1e-6  # EPANET's stopping rule, tightened from Net1's 0.001 so that the reference is converged
with tempfile.TemporaryDirectory() as tmp:
    results = wntr.sim.EpanetSimulator(wn).run_sim(file_prefix=str(Path(tmp) / "net1"))
head, pressure, demand = (results.node[k].iloc[0] for k in ("head", "pressure", "demand"))
flow = results.link["flowrate"].iloc[0]
out = HERE / "water_net1_t0.txt"
with out.open("w", encoding="utf-8", newline="\n") as f:
    f.write(f"# EPANET 2.2 through WNTR {wntr.__version__} on Net1.inp at t = 0: accuracy 1e-6; heads, pressures (m), demands, flows (m3/s); 2026-11-13\n")
    for node in head.index:
        f.write(f"node {node} {head[node]:.17g} {pressure[node]:.17g} {demand[node]:.17g}\n")
    for link in flow.index:
        f.write(f"link {link} {flow[link]:.17g}\n")
print("wrote", out)
