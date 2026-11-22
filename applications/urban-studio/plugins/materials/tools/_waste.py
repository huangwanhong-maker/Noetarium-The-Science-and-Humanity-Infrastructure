"""What the waste tools share: a city's waste survey, ladder of control, facilities, other sources and sorted samples read from the board's
tables, as the industrial-ecology domain's `waste` takes them (ADR 0148, ADR 0153)."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _materials as m  # noqa: E402
from science import waste as ws  # noqa: E402


def read(board, params) -> dict:
    """The survey, ladder, facilities, other sources and sorted samples the parameters name, with the strata's names."""
    h = board.table(params["households"])
    sample = [ws.household(int(st), int(ar), p, k, d) for st, ar, p, k, d in zip(m.nums(h, "stratum"), m.nums(h, "area"), m.nums(h, "persons"), m.nums(h, "kg"), m.nums(h, "days"))]
    st = board.table(params["strata"])
    strata = {int(s): H for s, H in zip(m.nums(st, "stratum"), m.nums(st, "households"))}
    names = dict(zip([int(x) for x in m.nums(st, "stratum")], m.texts(st, "name", optional=True)))
    lt = board.table(params["ladder"])
    levels, criteria = [], []
    for lv, name, crit in sorted(zip(m.nums(lt, "level"), m.texts(lt, "name"), m.texts(lt, "criterion", optional=True)), key=lambda r: r[0]):
        if not levels or levels[-1] != name:
            levels.append(name), criteria.append([])
        if crit:
            criteria[-1].append(crit)
    frm = str(params.get("controlled_from") or "").strip()
    if frm not in levels:
        raise LookupError(f"the ladder has no level '{frm}': {', '.join(levels)}")
    ladder = {"levels": levels, "criteria": criteria, "controlled_from": levels.index(frm)}
    ft = board.table(params["facilities"])
    facilities = [ws.facility(i, k.strip().lower(), rc or 0.0, rv or 0.0, to, [c.strip() for c in met.split(";") if c.strip()], name=n)
                  for i, n, k, rc, rv, to, met in zip(m.texts(ft, "id"), m.texts(ft, "name", optional=True), m.texts(ft, "kind"), m.nums(ft, "received"),
                                                      m.nums(ft, "recovered", optional=True), m.texts(ft, "residue_to", optional=True), m.texts(ft, "met", optional=True))]
    other = {}
    for part in str(params.get("other") or "").split(","):
        if "=" in part:
            k, v = part.split("=", 1)
            other[k.strip()] = float(v)
    sorted_samples = []
    sname = str(params.get("sorted") or "").strip()
    if sname and sname in board.tables:
        so = board.table(sname)
        fractions = [c for c in so.columns if c not in ("stratum", "area")]
        cols = {f: m.nums(so, f) for f in fractions}
        for i, (s_, a_) in enumerate(zip(m.nums(so, "stratum"), m.nums(so, "area"))):
            sorted_samples.append(ws.sorted_sample(int(s_), int(a_), {f: cols[f][i] or 0.0 for f in fractions}))
    return {"survey": ws.survey(strata, sample), "ladder": ladder, "facilities": facilities, "other": other,
            "not_delivered": float(params.get("not_delivered") or 0), "sorted": sorted_samples, "names": names}


def account(board, params) -> tuple[dict, dict]:
    """The account the tables give, and what was read."""
    R = read(board, params)
    return ws.account(R["survey"], R["ladder"], R["facilities"], other=R["other"], not_delivered=R["not_delivered"], sorted=R["sorted"]), R
