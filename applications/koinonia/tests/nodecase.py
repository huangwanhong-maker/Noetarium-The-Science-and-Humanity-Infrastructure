"""A node built for the checks of the gateway and of the archive (Phase 10 of the design of 2026-11-27): a notary, two households of
two persons with operatives of four providers, and two hosts: the completion (on its preset: writers by application while sections are
open, review by another provider, sections read only through the providers its norms name) and an atelier (on the commons preset), which
recognises no notary. Every party is invented. The clock is the node's own, set by the checks.
"""

from __future__ import annotations

import sys
from pathlib import Path

APP = Path(__file__).resolve().parents[1]
ROOT = APP.parents[1]
sys.path[:0] = [str(APP), str(ROOT / "api" / "python")]

from koinonia import passports  # noqa: E402
from koinonia.behaviour import episode  # noqa: E402
from koinonia.gateway import Gateway  # noqa: E402
from koinonia.node import Node, parse_iso  # noqa: E402

START = "2026-12-03T00:00:00Z"
TERM = {"since": "2026-12-01T00:00:00Z", "until": "2026-12-28T00:00:00Z"}
TOOL = '''def tool(data, params):
    xs = data["shelf"]
    return {"facts": {"n": len(xs), "mean": sum(xs) / len(xs)}, "summary": "the mean of the shelf's values"}
'''


class Clock:
    def __init__(self, at: str = START):
        self.t = parse_iso(at)

    def __call__(self) -> float:
        return self.t

    def advance(self, seconds: float) -> None:
        self.t += seconds


class Case:
    """The node, its gateway, and the credentials the checks hold."""

    def __init__(self, folder: Path | None):
        self.clock = Clock()
        self.node = Node(folder, name="test-node", url="http://127.0.0.1:9231/", clock=self.clock, operator="keeper")
        self.gateway = Gateway(self.node)
        n = self.node
        n.found("notary", "commons", "nina", [["nina", "steward"]], [{"id": "nina", "kind": "person"}])
        n.notary = "notary"
        self.home_a = passports.found_household(n, "author", [{"id": "author/codex", "provider": "openai"}, {"id": "author/gem", "provider": "google"},
                                                              {"id": "author/far", "provider": "openai"}])
        self.home_b = passports.found_household(n, "bea", [{"id": "bea/claude", "provider": "anthropic"}, {"id": "bea/late", "provider": "anthropic"}])
        n.found("completion", "completion", "author", [["author", "principal"], ["keeper", "steward"]],
                [{"id": "author", "kind": "person"}, {"id": "keeper", "kind": "person"}])
        n.found("atelier", "commons", "ria", [["ria", "steward"]], [{"id": "ria", "kind": "person"}])
        self.local("completion", "keeper", "recognise", {"of": "notary", "level": "evidence"})
        for sid in ("s-1", "s-2"):
            self.local("completion", "keeper", "request", {"id": sid, "work": "section", "spec": "plan", "matter": "the book"})
        n.hosted("completion").content["shelf"] = [3, 4, 5, 8]
        n.hosted("completion").content["plan"] = {"sections": ["s-1", "s-2"]}
        self.principal_a = passports.principal_credential(n, self.home_a, "author")
        self.principal_b = passports.principal_credential(n, self.home_b, "bea")

    def local(self, community: str, author: str, kind: str, fields: dict) -> dict:
        """An act taken at this computer by a person of the community (the steward), constituted by its episode."""
        a = self.node.act(author, kind, fields)
        a["conduct"] = episode(author, "act", route="page", address="local", act=kind)
        return self.node.step(community, [a])["outcomes"][0]

    def call(self, method: str, path: str, body=None, token: str = "", address: str = "127.0.0.1", query: dict | None = None, gateway=None):
        headers = {"Authorization": f"Bearer {token}"} if token else {}
        return (gateway or self.gateway).handle(method, path, query or {}, headers, body, address)

    def issue(self, principal_token: str, home: str, operative: str, model: str, communities=("completion",), term=None, **extra):
        body = {"operative": operative, "model": model, "matters": ["the book"], "communities": list(communities), "allowance": {"bench runs": 50},
                "slice": {"bench runs": 5}, "term": term or TERM, **extra}
        return self.call("POST", f"/c/{home}/passports", body, principal_token)
