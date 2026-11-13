"""A life cycle: a product system given as a table of exchanges, scaled to a functional unit, its inventory and impacts by the matrix method,
each process's contribution, the largest sensitivities and the supply chains that carry the impact."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _materials as m  # noqa: E402


def create(api):
    class LifeCycle(api.BoardAnalysis):
        id = "life_cycle"
        title = "A life cycle"
        description = ("A product system as a table of exchanges (a process, a flow, whether the flow is a product or an elementary flow with the "
                       "environment, and the amount per unit of the process: outputs positive, inputs negative), and a method as a table of "
                       "characterisation factors. Scaled to deliver the functional unit (Heijungs and Suh 2002), it gives the inventory, the impacts "
                       "and each process's share, the coefficients the result is most sensitive to (Heijungs 2010), a Monte Carlo spread and the "
                       "supply chains that carry the impact. Each product must be made by one process; a process with several products is a "
                       "work's to partition or expand first.")
        params = [api.param("table", "Exchanges", "exchanges", kind="text", role="table", help="one row an exchange of a process"),
                  m.col(api, "process", "Process", "process"),
                  m.col(api, "flow", "Flow", "flow"),
                  m.col(api, "kind", "Kind (product or elementary)", "kind"),
                  m.col(api, "amount", "Amount per unit of the process", "amount"),
                  api.param("factors", "Characterisation factors", "factors", kind="text", role="table", help="one row a category and an elementary flow"),
                  api.param("demand", "Functional unit", "electricity (kWh)=1000", kind="text", help="product=amount, several separated by commas"),
                  api.param("gsd", "Geometric standard deviation of every coefficient", 1.1, kind="number", minimum=1, help="1 for no Monte Carlo")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            proc, flow, kind, amount = m.texts(t, params["process"]), m.texts(t, params["flow"]), m.texts(t, params["kind"]), m.nums(t, params["amount"])
            processes = sorted(set(proc) - {""})
            products = sorted({f for f, k in zip(flow, kind) if k.lower().startswith("prod")})
            interventions = sorted({f for f, k in zip(flow, kind) if f and not k.lower().startswith("prod")})
            A = [[0.0] * len(processes) for _ in products]
            B = [[0.0] * len(processes) for _ in interventions]
            for p, f, k, v in zip(proc, flow, kind, amount):
                if not p or not f or v is None:
                    continue
                j = processes.index(p)
                if k.lower().startswith("prod"):
                    A[products.index(f)][j] += v
                else:
                    B[interventions.index(f)][j] += v
            ft = board.table(params["factors"])
            cats, fflow, fval = m.texts(ft, "category"), m.texts(ft, "flow"), m.nums(ft, "factor")
            units = dict(zip(cats, m.texts(ft, "unit", optional=True)))
            categories = sorted(set(cats) - {""})
            C = [[0.0] * len(interventions) for _ in categories]
            for c, f, v in zip(cats, fflow, fval):
                if c and f in interventions and v is not None:
                    C[categories.index(c)][interventions.index(f)] += v
            demand = [0.0] * len(products)
            for part in str(params["demand"]).split(","):
                if "=" in part:
                    name, value = part.rsplit("=", 1)
                    if name.strip() not in products:
                        raise LookupError(f"the functional unit names a product the system does not make: {name.strip()}")
                    demand[products.index(name.strip())] += float(value)
            # Order the processes so that product i is made by process i, as the matrix method's paths need.
            order = []
            for i in range(len(products)):
                makers = [j for j in range(len(processes)) if A[i][j] > 0]
                if len(makers) != 1:
                    raise ValueError(f"the product {products[i]} is made by {len(makers)} processes; each product needs exactly one")
                order.append(makers[0])
            if sorted(order) != list(range(len(processes))):
                raise ValueError("each process must make exactly one product (partition or expand a process with several first)")
            processes = [processes[j] for j in order]
            A = [[row[j] for j in order] for row in A]
            B = [[row[j] for j in order] for row in B]
            req = {"products": products, "processes": processes, "interventions": interventions, "A": A, "B": B,
                   "method": {"categories": categories, "units": [units.get(c, "") for c in categories], "C": C}, "demand": demand, "paths": {"category": 0, "cutoff": 1e-3}}
            if float(params["gsd"]) > 1:
                req["monte_carlo"] = {"gsd_A": float(params["gsd"]), "gsd_B": float(params["gsd"]), "draws": 1000, "seed": 1}
            a = m.materials.ask("lca", **req)
            impacts = {"category": categories, "unit": [units.get(c, "") for c in categories], "impact": a["impacts"]}
            if "spread" in a:
                impacts["lower_95"], impacts["upper_95"] = a["spread"]["lower"], a["spread"]["upper"]
            contrib = {"category": [], "process": [], "impact": []}
            for k, c in enumerate(categories):
                for j, p in enumerate(processes):
                    contrib["category"].append(c), contrib["process"].append(p), contrib["impact"].append(a["contribution"][k][j])
            paths = {"path": [" ← ".join(reversed(w["path"])) for w in a["paths"]["walks"]], "impact": [w["value"] for w in a["paths"]["walks"]]}
            top = a["elasticities"][0][:8] if a["elasticities"] else []
            return [api.table("impacts", impacts, title="The impacts of the functional unit", roles={"name": "category", "amount": "impact"}),
                    api.table("contributions", contrib, title="Each process's contribution to each impact", roles={"category": "process", "amount": "impact"},
                              show={"kind": "chart", "chart": "bar", "x": "category", "y": "impact", "split": "process", "how": "sum"}),
                    api.table("supply_chains", paths, title=f"The supply chains that carry {categories[0]}", roles={"name": "path", "amount": "impact"}),
                    api.table("sensitivities", {"coefficient": [e["coefficient"] for e in top], "elasticity": [e["elasticity"] for e in top]},
                              title=f"The coefficients {categories[0]} is most sensitive to (a 1% change moves it by the elasticity, in %)",
                              roles={"name": "coefficient", "amount": "elasticity"}),
                    api.note(f"The functional unit ({params['demand']}) causes " + "; ".join(f"{v:,.4g} {units.get(c, '')} of {c}" for c, v in zip(categories, a["impacts"]))
                             + "." + (f" With every coefficient uncertain by a factor {params['gsd']} (lognormal), the 95% interval of {categories[0]} is "
                                      f"{a['spread']['lower'][0]:,.4g} to {a['spread']['upper'][0]:,.4g}." if "spread" in a else "")
                             + (f" It is most sensitive to {top[0]['coefficient']} (elasticity {top[0]['elasticity']:+.2f})." if top else "")),
                    api.note("The system is the table's; the characterisation factors are the method's, declared in their table. Allocation and "
                             "system expansion, where a process has several products, are choices this tool does not make.")]
    return LifeCycle()
