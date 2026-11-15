"""Calibrate: raw values made memberships of a set, by Ragin's direct method."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import _common as c  # noqa: E402


def create(api):
    class Calibrate(api.BoardAnalysis):
        id = "calibrate"
        title = "Calibrate a set"
        description = ("A column of raw values made a membership of a set between 0 and 1, by Ragin's direct method (2008, Redesigning Social Inquiry, ch. 5): "
                       "three anchors say where a case is fully out, where it is neither in nor out, and where it is fully in, and the log-odds of membership "
                       "run linearly between them (±3 at the outer anchors). The anchors are the person's to declare and to defend; none is guessed.")
        params = [api.param("table", "Cases", "", kind="text", role="table"),
                  c.col(api, "raw", "Raw values"),
                  api.param("out", "Fully out at", 0.0, kind="number"),
                  api.param("crossover", "Neither in nor out at", 0.5, kind="number"),
                  api.param("full", "Fully in at", 1.0, kind="number"),
                  api.param("name", "Call the membership", "membership", kind="text")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            rows, dropped = c.complete(t, [params["raw"]])
            raw = c.numbers(t, params["raw"])
            m = c.inquiry.ask("calibrate", raw=[raw[i] for i in rows], out=params["out"], crossover=params["crossover"], **{"in": params["full"]})["members"]
            member = [None] * t.rows
            for i, v in zip(rows, m):
                member[i] = round(v, 4)
            columns = {name: t.raw(name) for name in t.columns}
            columns[params["name"] or "membership"] = member
            inside = sum(1 for v in m if v > 0.5)
            return [api.table(f"{t.name}_calibrated", columns, title=f"{t.title or t.name}, with {params['name']}"),
                    api.note(f"{len(m)} cases calibrated: {inside} more in the set than out, {len(m) - inside} more out (a membership of exactly 0.5 is in neither)."
                             + c.left_out(dropped))]
    return Calibrate()
