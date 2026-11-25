"""Communities for scripts in Python and the works' servers (ADR 0167): constitutions admitted by form, a community's record run on its
constitution by thesmos's fold, decisions counted by origin with their sensitivity to who controls whom, and the communities domain's
jar, asked of the API's own engine (``api/cpp/engines/communities/main.cpp``, built on first use).

    from science import communities
    communities.admit(communities.preset("commons"))                  # {admitted, faults, profile}
    r = communities.run(constitution, parties, acts)                  # {outcomes, seals, state, head, digest}
    communities.run(constitution, parties, acts, seals=r["seals"])    # ... and "replays": whether the record seals as before
    communities.count(parties, ballots, unit="origin", rule="majority")
    communities.jar(regime="provision with duty", need=4, free_riders=0.5, epistemic=0.1, runs=20)

The computations are composed in ``api/cpp/science/communities.hpp``, which documents the shapes of parties, acts and ballots.
"""

from __future__ import annotations

import json
from pathlib import Path

from .engines import Engine, EngineError, clean  # noqa: F401

ROOT = Path(__file__).resolve().parents[3]
SUBDOMAIN = ROOT / "domains" / "communities" / "human-ai"
ENGINE = Engine("api/communities", source=ROOT / "api" / "cpp" / "engines" / "communities" / "main.cpp",
                extra_includes=[SUBDOMAIN / "jar" / "include", SUBDOMAIN / "lending" / "include"], env="NOETARIUM_COMMUNITIES_ENGINE")
PRESETS = ROOT / "domains" / "communities" / "presets" / "data"
REGIMES = ("currency", "currency with income", "currency with targeted income", "provision, open", "provision with duty")
UNITS = ("person", "origin", "operative", "community")


def presets() -> list[str]:
    """The names of the presets kept in the communities domain."""
    return sorted(p.stem for p in PRESETS.glob("*.json"))


def preset(name: str) -> dict:
    """A preset constitution, as written."""
    path = PRESETS / f"{name}.json"
    if not path.is_file():
        raise ValueError(f"no preset {name!r}; the presets are {', '.join(presets())}")
    return json.loads(path.read_text(encoding="utf-8"))


def ask(op: str, **request) -> dict:
    if not ENGINE.status().get("current"):
        ENGINE.build(log=lambda *_: None)
    return ENGINE.ask(f"communities.{op}", **request)


def admit(constitution: dict | str) -> dict:
    return ask("admit", constitution=constitution)


def run(constitution: dict | str, parties: list[dict], acts: list[dict], seals: list[str] | None = None) -> dict:
    request = {"constitution": constitution, "parties": parties, "acts": acts}
    if seals is not None:
        request["seals"] = seals
    return ask("run", **request)


def count(parties: list[dict], ballots: list[dict], unit: str = "origin", rule: str = "majority") -> dict:
    if unit not in UNITS:
        raise ValueError(f"no unit of count {unit!r}; the units are {', '.join(UNITS)}")
    return ask("count", parties=parties, ballots=ballots, unit=unit, rule=rule)


def statements(constitution: dict | str, texts: list[str] | None = None, parties: list[dict] | None = None, acts: list[dict] | None = None) -> dict:
    """The constitution's rules as institutional statements (or the texts given, read by the grammar), each duty and prohibition
    measured over the record when parties and acts are given."""
    request = {"constitution": constitution, "parties": parties or [], "acts": acts or []}
    if texts is not None:
        request["texts"] = texts
    return ask("statements", **request)


def jar(regime: str = "provision with duty", runs: int = 20, **setting) -> dict:
    if regime not in REGIMES:
        raise ValueError(f"no arrangement {regime!r}; the arrangements are {', '.join(REGIMES)}")
    return ask("jar", regime=regime, runs=runs, **setting)
