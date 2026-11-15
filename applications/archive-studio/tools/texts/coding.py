"""Coded passages: the agreement of coders, the codes by case, the codes that go together, and saturation."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import _common as c  # noqa: E402


def create(api):
    class Coding(api.BoardAnalysis):
        id = "coding"
        title = "Coded passages"
        description = ("Passages of texts, each coded by one or more people (the table the reading rooms write: document, begin, end, quote, code, coder). "
                       "How far the coders agree on each code beyond chance (Krippendorff's α, and Cohen's κ for two), over every passage any of them coded; "
                       "the cases by the codes they carry; the codes that fall on the same passages; and saturation (Guest, Namey and Chen 2020): whether "
                       "documents read after a base of four still bring codes not seen before. A passage's quote is checked against its document when the "
                       "documents are given.")
        params = [api.param("segments", "Coded passages", "codings", kind="text", role="table"),
                  api.param("documents", "Documents (optional)", "", kind="text", role="table", help="document, case, text: to check every quote and name the cases")]

        def run(self, board, params, context):
            s = board.table(params["segments"])
            c.need(s, ["document", "begin", "end", "quote", "code", "coder"])
            docs, segs, cods, codes = [], {}, [], {}
            doc_ids, beg, end, quote, code, coder = c.text(s, "document"), c.numbers(s, "begin"), c.numbers(s, "end"), c.text(s, "quote"), c.text(s, "code"), c.text(s, "coder")
            for i in range(s.rows):
                key = (doc_ids[i], int(beg[i]), int(end[i]))
                if key not in segs:
                    segs[key] = {"id": f"s{len(segs) + 1}", "document": key[0], "begin": key[1], "end": key[2], "quote": quote[i]}
                cods.append({"segment": segs[key]["id"], "code": code[i], "coder": coder[i] or "unnamed"})
                codes.setdefault(code[i], {"id": code[i], "label": code[i]})
            if params["documents"]:
                d = board.table(params["documents"])
                c.need(d, ["document", "text"])
                cases = c.text(d, "case") if d.has("case") else c.text(d, "document")
                docs = [{"id": a, "case": b, "text": t} for a, b, t in zip(c.text(d, "document"), cases, d.text("text"))]
            else:
                docs = [{"id": x, "case": x, "text": ""} for x in sorted({k[0] for k in segs})]
                for sg in segs.values():   # without the documents, a quote is checked against nothing: its span is the quote's own
                    sg["begin"], sg["end"] = 0, len(sg["quote"].encode("utf-8"))
                    sg["document"] = sg["document"]
            r = c.inquiry.ask("coding", codes=list(codes.values()), documents=docs, segments=list(segs.values()), codings=cods)
            out = []
            if "agreement" in r:
                a = r["agreement"]
                out.append(api.table("agreement", {"code": [x["code"] for x in a], "passages": [x["units"] for x in a], "given": [x["given"] for x in a],
                                                   "alpha": [x["alpha"] for x in a], "kappa": [x["kappa"] for x in a], "note": [x["note"] for x in a]},
                                     title=f"Agreement of {len(r['coders'])} coders, code by code"))
            m = r["cases"]
            out.append(api.table("codes_by_case", {"case": m["rows"], **{col: [row[j] for row in m["values"]] for j, col in enumerate(m["columns"])}}, title="Codes by case"))
            co = r["co_occurrence"]
            out.append(api.table("codes_together", {"code": co["rows"], **{col: [row[j] for row in co["values"]] for j, col in enumerate(co["columns"])}}, title="Codes on the same passages"))
            said = [f"{len(segs)} passages of {len(docs)} documents coded {len(cods)} times with {len(codes)} codes by {len(r['coders'])} coders."]
            if r.get("faults") and params["documents"]:
                said.append(f"{len(r['faults'])} faults: " + "; ".join(f"{f['where']}: {f['what']}" for f in r["faults"][:4]) + ("…" if len(r["faults"]) > 4 else ""))
            if "saturation" in r:
                sat = r["saturation"]
                said.append(f"Saturation reached after {int(sat['reached_after'])} documents." if sat.get("reached_after") else "Saturation is not reached: the last documents still brought new codes.")
            out.append(api.note(" ".join(said)))
            return out
    return Coding()
