"""The work a community's mission declares, kept by the work beside the chronicle (ADR 0169).

A chronicle reads envelopes, never bodies (thesmos's second invariant): an act of delivery says which commitment it performs and
what it charges, and its body (the findings) is kept here, under the commitment's id, with its digest in the envelope's
``artefact``. What counts as a well-formed delivery, and when two checks agree, are the declared standards of the stewards whose
operatives accept; they are here so that every agent and every page reads the same standard.

The first mission is the commons' (the design record of 2026-11-25, Section 6.2): to check entries a language model drafted and
nobody checked. Its material is the urban-science domain's catalogue of indexes (ADR 0128), whose 1,536 entries say how each index
is carried up a hierarchy of units; a check says of each entry whether what it declares holds, is at fault, or is in doubt. Rules
of aggregation declared here are applied by rule agents; model agents read the definitions. What the society finds is a proposal
for the catalogue's keeper, of grade C, and never changes the catalogue.
"""

from __future__ import annotations

import csv
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
CATALOGUE = ROOT / "domains" / "urban-science" / "indicators" / "catalogue.tsv"
VERDICTS = ("holds", "fault", "doubt")
FIELDS = ("id", "name", "kind", "unit", "aggregation", "scales", "definition", "source")

# Kinds and units of an intensive quantity, which a sum up a hierarchy does not carry.
INTENSIVE_KINDS = {"rate", "ratio", "coefficient", "density", "price", "ordinal", "composite", "latent", "distribution",
                   "spatial-statistic", "categorical", "binary", "coded"}
INTENSIVE_UNITS = {"1", "[0,1]", "0–100", "[−1,1]", "score", "0–10", "%", "%/yr", "1/yr", "°C"}
ORDINAL_MARKS = ("Likert", "levels", "ordinal")

# The declared rules of aggregation (Stevens 1946 on what each scale permits; Openshaw 1984 and the catalogue's README on carrying a
# quantity up a hierarchy). Each is (code, statement, test).
RULES = [
    ("R1", "an intensive quantity (a rate, ratio, share, index or price) is not summed up a hierarchy",
     lambda e: e["aggregation"] == "sum" and (e["kind"] in INTENSIVE_KINDS or e["unit"] in INTENSIVE_UNITS or "per " in e["unit"])),
    ("R2", "an ordinal level is not averaged or summed; it is carried by its median or not at all",
     lambda e: (e["kind"] == "ordinal" or any(m in e["unit"] for m in ORDINAL_MARKS)) and (e["aggregation"] == "sum" or e["aggregation"].startswith("wmean"))),
    ("R3", "a median is recomputed from the distribution, not averaged or summed",
     lambda e: e["name"].lower().startswith("median") and (e["aggregation"] == "sum" or e["aggregation"].startswith("wmean"))),
    ("R4", "an entry names its source", lambda e: not e["source"].strip()),
    ("R5", "an entry states its definition", lambda e: not e["definition"].strip()),
]


def read_catalogue(path: Path = CATALOGUE) -> list[dict]:
    with path.open(encoding="utf-8", newline="") as f:
        return [{k: (row.get(k) or "").strip() for k in FIELDS} for row in csv.DictReader(f, delimiter="\t")]


def key(text: str) -> str:
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def sample(entries: list[dict], size: int, seed: str = "koinonia") -> list[dict]:
    """A declared sample: entries ordered by the hash of seed and id within each rule of aggregation, taken in turn from each, so
    that every kind of aggregation is reached and the same seed always gives the same sample."""
    strata: dict[str, list[dict]] = {}
    for e in sorted(entries, key=lambda e: key(seed + e["id"])):
        strata.setdefault(e["aggregation"].split(":")[0], []).append(e)
    out, order = [], sorted(strata)
    while len(out) < size and any(strata.values()):
        for s in order:
            if strata[s] and len(out) < size:
                out.append(strata[s].pop(0))
    return out


def batches(entries: list[dict], per: int = 4) -> list[dict]:
    return [{"id": f"b{n + 1:02d}", "entries": [e["id"] for e in entries[i:i + per]]} for n, i in enumerate(range(0, len(entries), per))]


def rule_check(entry: dict) -> dict:
    fired = [(code, text) for code, text, test in RULES if test(entry)]
    if fired:
        return {"entry": entry["id"], "verdict": "fault", "why": "; ".join(f"{c}: {t}" for c, t in fired)}
    return {"entry": entry["id"], "verdict": "holds", "why": "no declared rule of aggregation is broken (" + ", ".join(c for c, _, _ in RULES) + ")"}


def shown(entry: dict) -> dict:
    """What a checker is shown of an entry."""
    return {k: entry[k] for k in FIELDS}


def well_formed(findings, batch: dict) -> str:
    """Why a delivery's findings fail the stewards' standard, or nothing: one verdict for each entry of the batch, each a declared
    verdict with a reason."""
    if not isinstance(findings, list):
        return "the findings are not a list"
    seen = {}
    for f in findings:
        if not isinstance(f, dict) or f.get("verdict") not in VERDICTS or not str(f.get("why", "")).strip():
            return "a finding lacks a declared verdict or its reason"
        seen[f.get("entry")] = f
    missing = [e for e in batch["entries"] if e not in seen]
    if missing:
        return "no finding for " + ", ".join(missing)
    extra = [e for e in seen if e not in batch["entries"]]
    return ("findings for entries outside the batch: " + ", ".join(map(str, extra))) if extra else ""


def disagreements(a: list[dict], b: list[dict]) -> list[str]:
    """The entries on which two checks differ; a doubt agrees with nothing."""
    va, vb = {f["entry"]: f["verdict"] for f in a}, {f["entry"]: f["verdict"] for f in b}
    return [e for e in va if e in vb and (va[e] != vb[e] or va[e] == "doubt")]


def digest(findings) -> str:
    return key(json.dumps(findings, sort_keys=True, ensure_ascii=False))[:16]


class Work:
    """The mission's material and what the society has found: batches, the bodies of deliveries by commitment, disputes, findings
    entry by entry, and the guild's trials."""

    def __init__(self, data: dict | None = None):
        d = data or {}
        self.entries: dict[str, dict] = d.get("entries", {})
        self.batches: list[dict] = d.get("batches", [])
        self.bodies: dict[str, dict] = d.get("bodies", {})        # commitment id -> {by, origin, provider, kind, findings | adjudication}
        self.disputes: list[dict] = d.get("disputes", [])          # {id, entry, batch, verdicts, status, adjudication}
        self.findings: dict[str, dict] = d.get("findings", {})     # entry -> {finding, how, verdicts, adjudication}
        self.trials: list[dict] = d.get("trials", [])              # {id, entry, known}
        self.regime: str = d.get("regime", "")                     # the arrangement of circulation declared before any compact
        self.services: dict[str, dict] = d.get("services", {})     # commitment id -> {provider, target, turn, kind, state}
        self.withheld: dict[str, int] = d.get("withheld", {})      # "provider>target" -> times an agent was withheld at the limit
        self.withheld_at: dict[str, int] = d.get("withheld_at", {})  # community -> the last round another withheld service from it
        self.applicants: list[str] = d.get("applicants", [])       # communities that apply to join the league

    def to_json(self) -> dict:
        return {"entries": self.entries, "batches": self.batches, "bodies": self.bodies, "disputes": self.disputes,
                "findings": self.findings, "trials": self.trials, "regime": self.regime, "services": self.services, "withheld": self.withheld,
                "withheld_at": self.withheld_at, "applicants": self.applicants}

    @classmethod
    def lists(cls, owners: dict[str, int], per: int = 4, seed: str = "koinonia") -> "Work":
        """Work lists owned by several communities, cut in turn from one declared list (as ``catalogue``), each batch with its owner."""
        whole = cls.catalogue(size=sum(owners.values()), per=per, seed=seed)
        order = [e for b in whole.batches for e in b["entries"]]
        out, i = [], 0
        for name, n in owners.items():
            for k, j in enumerate(range(i, i + n, per)):
                out.append({"id": f"{name[:4]}-{k + 1:02d}", "owner": name, "entries": order[j:min(j + per, i + n)]})
            i += n
        whole.batches = out
        return whole

    @classmethod
    def catalogue(cls, size: int = 48, per: int = 4, seed: str = "koinonia", at_risk: float = 0.25) -> "Work":
        """The stewards' declared work list: a quarter of it entries the declared rules find at fault (checked first by risk, as an
        audit samples where faults are likely), the rest a sample stratified by rule of aggregation, all in an order fixed by the
        seed."""
        entries = read_catalogue()
        flagged = [e for e in sorted(entries, key=lambda e: key(seed + e["id"])) if rule_check(e)["verdict"] == "fault"][:round(size * at_risk)]
        taken = {e["id"] for e in flagged}
        chosen = flagged + sample([e for e in entries if e["id"] not in taken], size - len(flagged), seed)
        chosen.sort(key=lambda e: key(seed + "/order/" + e["id"]))
        return cls({"entries": {e["id"]: e for e in chosen}, "batches": batches(chosen, per)})

    def batch(self, ident: str) -> dict | None:
        return next((b for b in self.batches if b["id"] == ident), None)

    def dispute(self, ident: str) -> dict | None:
        return next((d for d in self.disputes if d["id"] == ident), None)

    def settle_batch(self, batch_id: str, deliveries: list[dict]) -> list[dict]:
        """Records the findings of a batch once two checks of different origins are in: agreed entries become findings, the others
        disputes. Returns the new disputes."""
        a, b = deliveries[0]["findings"], deliveries[1]["findings"]
        apart = set(disagreements(a, b))
        new = []
        for f in a:
            e = f["entry"]
            verdicts = [{"by": d["by"], "origin": d["origin"], "provider": d["provider"], "kind of check": d.get("kind of check", d["provider"]),
                         **next(x for x in d["findings"] if x["entry"] == e)} for d in deliveries]
            if e in apart:
                if not self.dispute(f"d-{e}"):
                    d = {"id": f"d-{e}", "entry": e, "batch": batch_id, "verdicts": verdicts, "status": "open", "adjudication": None}
                    self.disputes.append(d)
                    new.append(d)
                self.findings[e] = {"finding": "open", "how": "disputed", "verdicts": verdicts}
            else:
                self.findings[e] = {"finding": f["verdict"], "how": "agreed", "verdicts": verdicts}
        return new

    def adjudicated(self, dispute_id: str, adjudication: dict, head: dict) -> None:
        """Records the guild's finding on a disagreement (a person's replaces a model's, and is carried back again)."""
        d = self.dispute(dispute_id)
        if d.get("adjudication") and d["adjudication"].get("grade") == "a person" and adjudication.get("grade") != "a person":
            return
        d["status"], d["adjudication"] = "adjudicated", {**adjudication, "guild head": head}
        self.findings[d["entry"]] = {"finding": adjudication["verdict"], "how": "adjudicated", "verdicts": d["verdicts"], "adjudication": d["adjudication"]}

    def counts(self) -> dict:
        tally = {"entries": len(self.entries), "batches": len(self.batches), "holds": 0, "fault": 0, "open": 0, "agreed": 0, "adjudicated": 0}
        for f in self.findings.values():
            tally[f["finding"] if f["finding"] in ("holds", "fault") else "open"] += 1
            tally[f["how"]] = tally.get(f["how"], 0) + 1
        tally["unchecked"] = len(self.entries) - len(self.findings)
        tally["disputes open"] = sum(d["status"] in ("open", "referred") for d in self.disputes)
        return tally
