"""Participatory budgets in Pabulib's format (.pb): a file of three sections, META, PROJECTS and VOTES, each a header line and rows
separated by semicolons (Faliszewski, Flis, Peters, Pierczyński, Skowron, Stolicki, Szufa and Talmon 2023, *Participatory Budgeting:
Data, Tools and Analysis*, IJCAI; the format at https://pabulib.org/format).

    from data.formats import pabulib
    pb = pabulib.read(path)          # {"meta": {...}, "projects": [...], "votes": [...]}
    E = pabulib.election(pb)         # {"budget", "costs", "approvals", "ids", ...}: projects by index, voters' approvals as lists

Approval ballots are read as such; ordinal and cumulative ballots are read as the projects each voter named (their approval set),
said so in ``election``'s ``reading``. A project's coordinates, when it has several, are averaged by Pabulib's own convention.
"""

from __future__ import annotations

from pathlib import Path


def read(path) -> dict:
    meta: dict = {}
    projects: list = []
    votes: list = []
    section, header = None, None
    with open(Path(path), encoding="utf-8-sig") as f:
        for raw in f:
            line = raw.rstrip("\r\n")
            if not line.strip():
                continue
            word = line.strip()
            if word in ("META", "PROJECTS", "VOTES"):
                section, header = word, None
                continue
            cells = line.split(";")
            if header is None:
                header = [c.strip() for c in cells]
                continue
            row = dict(zip(header, cells))
            if section == "META":
                meta[row.get("key", "")] = row.get("value", "")
            elif section == "PROJECTS":
                projects.append(row)
            elif section == "VOTES":
                votes.append(row)
    if not projects:
        raise ValueError(f"{path}: no projects")
    return {"meta": meta, "projects": projects, "votes": votes}


def _num(x, fallback=None):
    try:
        return float(str(x).replace(",", "."))
    except (TypeError, ValueError):
        return fallback


def election(pb: dict) -> dict:
    """Projects by index (their ids, costs, names, categories, coordinates, whether the city selected them) and each voter's approval
    set as indices, with the voters' other columns kept (age, sex, neighbourhood where given)."""
    projects = pb["projects"]
    ids = [p["project_id"].strip() for p in projects]
    index = {pid: k for k, pid in enumerate(ids)}
    costs = [_num(p.get("cost"), 0.0) for p in projects]
    approvals, voters, unknown = [], [], 0
    for v in pb["votes"]:
        chosen = [index[x.strip()] for x in (v.get("vote") or "").split(",") if x.strip() in index]
        unknown += sum(1 for x in (v.get("vote") or "").split(",") if x.strip() and x.strip() not in index)
        approvals.append(sorted(set(chosen)))
        voters.append({k: v[k] for k in v if k not in ("vote", "points")})
    meta = pb["meta"]
    kind = meta.get("vote_type", "approval")
    return {
        "budget": _num(meta.get("budget"), 0.0), "currency": meta.get("currency", ""), "costs": costs, "approvals": approvals, "ids": ids,
        "names": [p.get("name", "") for p in projects], "categories": [p.get("category", "") for p in projects],
        "selected": [str(p.get("selected", "")).strip() == "1" for p in projects], "votes": [_num(p.get("votes"), 0.0) for p in projects],
        "lat": [_num(p.get("latitude")) for p in projects], "lon": [_num(p.get("longitude")) for p in projects],
        "voters": voters, "unknown_votes": unknown, "meta": meta,
        "reading": "approval ballots" if kind == "approval" else f"{kind} ballots read as the projects each voter named",
    }
