"""Flows that balance: measured flows between processes reconciled so that every process keeps its balance, unknown flows found from the
balances, and a test of whether the measurements can balance at all."""

import math
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _materials as m  # noqa: E402


def create(api):
    class BalancedFlows(api.BoardAnalysis):
        id = "balanced_flows"
        title = "Flows that balance"
        description = ("A material flow system as a table of flows (from a process or from outside, to a process or to outside, or a process's stock "
                       "change), each measured with a standard deviation, known exactly, or unknown. The measured flows are adjusted by weighted least "
                       "squares until every process balances, the unknown ones are found where the balances fix them, and a χ² test says whether "
                       "the measurements can balance within their stated errors; the largest normalised adjustment points to a gross error "
                       "(STAN's method: Cencic and Rechberger 2008).")
        params = [api.param("table", "Flows", "flows", kind="text", role="table", help="one row a flow"),
                  m.col(api, "name", "Name", "flow"),
                  m.col(api, "from_", "From (empty: outside)", "from"),
                  m.col(api, "to", "To (empty: outside; 'stock' for a stock change)", "to"),
                  m.col(api, "value", "Value (empty: unknown)", "value"),
                  m.col(api, "sd", "Standard deviation (0: exact)", "sd")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            names, src, dst = m.texts(t, params["name"]), m.texts(t, params["from_"]), m.texts(t, params["to"])
            val, sd = m.nums(t, params["value"]), m.nums(t, params["sd"], optional=True)
            processes = sorted({p for p in src + dst if p and p.lower() != "stock"})
            flows = []
            for i in range(t.rows):
                stock = dst[i].lower() == "stock"
                f = {"name": names[i] or f"flow {i + 1}", "from": src[i] or None, "to": None if stock or not dst[i] else dst[i], "stock": stock}
                if val[i] is not None:
                    f["value"], f["sd"] = val[i], sd[i] if sd[i] is not None else 0.0
                flows.append(f)
            a = m.materials.ask("reconcile", processes=processes, flows=flows)
            rows = {"flow": [f["name"] for f in flows], "from": [f["from"] or "(outside)" for f in flows],
                    "to": ["(stock)" if f["stock"] else f["to"] or "(outside)" for f in flows], "kind": a["kind"],
                    "measured": [f.get("value") for f in flows], "reconciled": a["value"], "sd": a["sd"], "adjustment": a["adjustment"],
                    "normalised": a["normalised"], "determined": ["yes" if o else "no" for o in a["observable"]]}
            worst = max((i for i in range(len(flows)) if a["normalised"][i] is not None), key=lambda i: abs(a["normalised"][i]), default=None)
            undetermined = [flows[i]["name"] for i in range(len(flows)) if not a["observable"][i]]
            verdict = ("the measurements and the balances agree within the stated errors" if a["p"] >= 0.05 else
                       "the measurements and the balances disagree beyond the stated errors")
            return [api.table("reconciled", rows, title="The flows, reconciled", roles={"name": "flow", "amount": "reconciled"},
                              show={"kind": "chart", "chart": "bar", "x": "flow", "y": "adjustment", "how": "sum"}),
                    api.table("flow_links", {"source": [l["source"] for l in a["links"]], "target": [l["target"] for l in a["links"]],
                                             "value": [l["value"] for l in a["links"]]}, title="The reconciled system, as links for a diagram of flows"),
                    api.note(f"{len(processes)} processes, {len(flows)} flows; {a['redundancy']} balance{'s' if a['redundancy'] != 1 else ''} left to test after the "
                             f"unknowns. χ² = {a['chi2']:.2f} on {a['redundancy']} degrees of freedom, p = {a['p']:.3g}: {verdict}."
                             + (f" The largest normalised adjustment is at {flows[worst]['name']} ({a['normalised'][worst]:+.2f})"
                                + (", beyond 1.96: a gross error there is the first to look for." if abs(a['normalised'][worst]) > 1.96 else ".") if worst is not None else "")
                             + (f" The balances do not determine {', '.join(undetermined)}; more measurements are needed there." if undetermined else "")),
                    api.note("What reconciliation assumes: independent, normal errors of the stated sizes, and processes that conserve mass. A flow "
                             "adjusted by much more than its error is not corrected by it, only flagged.")]
    return BalancedFlows()
