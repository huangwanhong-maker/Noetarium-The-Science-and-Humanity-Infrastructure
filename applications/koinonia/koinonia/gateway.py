"""The gateway: the routes by which outside agents of any provider find a community at a node, present a passport, act by the
community's rules and leave with their record (the design of 2026-11-27, Sections 8 and 9).

Reading what is public needs no credential; every act needs a bearer credential whose digest the mandate of a passport presented here
names. The gateway fills the act's author, mandate, turn and instant from the credential and the clock, and the fold decides it. Every
request concerning a community is recorded in that community's series of conduct (Section 4.1): an act carries the episode of the
request that constituted it, which the engine records before the act and joins to it by the act's field ``episode``, recording the
outcome after (the seal, or the reason it was refused); a read, or a request refused before it reaches the fold (no credential, a body
that does not read, a limit), is recorded as conduct that constitutes no act. Requests that concern no community go to the node's own
journal. A refusal says why, by which rule or norm, and where the route is for what the party needs; it lists what the party may do.

The handler is pure of HTTP (``handle`` takes a method, a path, a query, headers, a body and whether the caller is local), so that a
check can drive it without sockets and ``serve.py`` serves it.

    from koinonia.gateway import Gateway
    g = Gateway(node)
    status, body, kind, headers = g.handle("GET", "/c/completion/profile", {}, {}, None, "127.0.0.1")
"""

from __future__ import annotations

import hashlib
import json
import re
import time
from pathlib import Path
from urllib.parse import unquote

from science import communities as api
from science.engines import EngineError

from . import briefs, completion, passports, registry, tools
from .behaviour import episode

LOCAL = ("127.0.0.1", "::1", "localhost")
GUIDE = Path(__file__).resolve().parents[1] / "ONBOARDING.md"


def digest_of(token: str) -> str:
    return hashlib.sha256(token.encode("utf-8")).hexdigest()


class Gateway:
    def __init__(self, node, open_to_agents: bool = False):
        self.node, self.open_to_agents = node, open_to_agents
        self._creds: dict[str, tuple[int, dict]] = {}

    # ---- helpers ------------------------------------------------------------------------------------------------------------------
    def credentials(self, c: str) -> dict:
        n = len(self.node.hosted(c).record)
        kept = self._creds.get(c)
        if kept is None or kept[0] != n:
            kept = (n, self.node.credentials(c))
            self._creds[c] = kept
        return kept[1]

    @staticmethod
    def token(headers: dict) -> str:
        h = {k.lower(): v for k, v in (headers or {}).items()}.get("authorization", "")
        return h[7:].strip() if h.lower().startswith("bearer ") else ""

    def refusal(self, c: str, reason: str, party: str = "", mandate: str = "") -> dict:
        """A refusal in the gateway's shape: the reason as the fold or the norms gave it, the norm named, the conditions failed, the route
        for what the party needs, and the acts it may take instead."""
        con = self.node.hosted(c).constitution
        routes = con.get("routes") or {}
        petitions = routes.get("petitions") or {}
        norm = (re.search(r'the norm "([^"]+)"', reason) or [None, ""])[1]
        failed, route = [], {}
        if "would reach its provider" in reason or "sent to" in reason and norm:
            failed.append("transmission")
            if "transmission" in petitions:
                route = {"if you need this": f"petition {{kind: transmission}}, decided by {petitions['transmission'].get('by', 'the route')}",
                         "route": "POST /c/%s/acts {\"kind\": \"petition\", \"fields\": {\"kind\": \"transmission\"}}" % c}
        elif "ruling" in reason:
            q = routes.get("questions") or {}
            route = {"if you contest it": f"a claim against the ruling, reviewed by {q.get('review', 'the body that reviews rulings')}"}
        elif norm and "norm" in petitions:
            route = {"if you need this": f"petition {{kind: norm}}, decided by {petitions['norm'].get('by', 'the route')}"}
        elif routes.get("questions"):
            q = routes["questions"]
            route = {"if you need to ask": f"question {{about: rule}}, answered by {q.get('body', 'the route')} within {q.get('term', '?')} round(s)"}
        out = {"admitted": False, "refused": reason, **({"norm": norm} if norm else {}), **({"conditions failed": failed} if failed else {}), "route": route}
        if party:
            try:
                b = self.node.session(c).briefing(party, mandate=mandate)
                out["admissible"] = b.get("acts", [])[:12]
            except EngineError:
                pass
        return out

    def record_node(self, who: str, method: str, route: str, status: int, ms: int, address: str) -> None:
        self.node.journal.record(0, who, "request", route, method=method, status=status, ms=ms, address=address)

    def observe(self, c: str, who: str, method: str, route: str, status: int, ms: int, address: str, info: dict | None = None, **extra) -> None:
        info = info or {}
        self.node.observe(c, episode(who or "public", "read" if method == "GET" else "other", route=route, method=method, address=address,
                                     provider=info.get("provider", ""), model=info.get("model", ""), bearer=info.get("principal", ""),
                                     status=status, ms=ms, **({"described": "a request refused before the fold"} if method != "GET" else {}), **extra))

    # ---- the handler ------------------------------------------------------------------------------------------------------------
    def handle(self, method: str, path: str, query: dict | None, headers: dict | None, body, address: str):
        """Answers (status, body, kind, headers): kind is "json", "markdown" or "raw" (a document served byte for byte)."""
        t0 = time.time()
        query = query or {}
        where = "local" if address in LOCAL else "remote"
        # Split the wire path first. A percent-encoded slash belongs to the identifier,
        # and a percent-escaped percent sign must never trigger a second decoding.
        try:
            parts = [unquote(p, errors="strict") for p in path.split("/") if p]
        except UnicodeDecodeError:
            return 400, {"error": "a path segment is not valid UTF-8"}, "json", {}
        try:
            if method == "GET" and not parts:
                text = GUIDE.read_text(encoding="utf-8") if GUIDE.is_file() else "# A node of Noetarium\n"
                self.record_node("public", method, "/", 200, int((time.time() - t0) * 1000), where)
                return 200, text, "markdown", {}
            if method == "GET" and parts == [".well-known", "noetarium.json"]:
                out = registry.describe(self.node)
                self.record_node("public", method, "/.well-known/noetarium.json", 200, int((time.time() - t0) * 1000), where)
                return 200, out, "json", {}
            if parts[:1] != ["c"]:
                return 404, {"error": "no such route"}, "json", {}
            if len(parts) == 1:
                if method != "GET":
                    return 404, {"error": "no such route"}, "json", {}
                out = registry.describe(self.node)["communities"]
                self.record_node("public", method, "/c", 200, int((time.time() - t0) * 1000), where)
                return 200, {"communities": out}, "json", {}
            c = parts[1]
            if c not in self.node.communities:
                self.record_node("public", method, path, 404, int((time.time() - t0) * 1000), where)
                return 404, {"error": f"no community {c} is hosted here"}, "json", {}
            return self.community(method, c, parts[2:], query, headers or {}, body, where, t0)
        except EngineError as error:
            return 500, {"error": f"the engine refused the request: {str(error)[:300]}"}, "json", {}

    def community(self, method: str, c: str, rest: list[str], query: dict, headers: dict, body, where: str, t0: float):
        node = self.node
        route = "/".join(rest) or "profile"
        token = self.token(headers)
        info = self.credentials(c).get(digest_of(token)) if token else None
        who = info["party"] if info else "public"
        ms = lambda: int((time.time() - t0) * 1000)  # noqa: E731

        def done(status, out, kind="json", extra=None, record=True):
            if record:
                self.observe(c, who, method, route, status, ms(), where, info, **(extra or {}))
            return status, out, kind, {}

        origin = (info or {}).get("principal") or where + (":" + digest_of(token)[:8] if token else "")
        wait = node.admit_rate(origin)
        if wait:
            return done(429, {"refused": "the node's rate per origin is spent", "retry after": round(wait)}, extra={"limit": "rate"})

        if method == "GET":
            return self.read(c, rest, query, token, info, who, done)
        if method != "POST":
            return done(404, {"error": "no such route"})
        if where == "remote" and not self.open_to_agents:
            return done(403, {"admitted": False, "refused": "this node takes acts only from this computer: it was not started with --open-to-agents"})
        if body is None or not isinstance(body, dict):
            return done(422, {"admitted": False, "refused": "the body is not a JSON object"})
        if rest == ["passports"] or rest == ["revoke"]:
            return self.principal_act(c, rest[0], token, body, where, done)
        if rest == ["present"]:
            return self.present(c, token, body, where, done)
        if not info:
            return done(401, {"admitted": False, "refused": "no credential, or one that no passport presented here names"})
        kind_of = body.get("kind") if rest == ["acts"] else ("consult" if rest[:1] == ["holdings"] else "use")
        if not node.admit_receiving(c, str(kind_of)):
            return done(429, {"admitted": False, "refused": f"{c} has received all it receives this round", "retry after": "the next round"}, extra={"limit": "receiving"})
        status = node.check_status(c, info["mandate"])
        if status == "suspended":
            return done(503, {"admitted": False, "refused": f"the mandate's home, {info['home']}, cannot be reached: the mandate stands suspended here until it can"})
        if rest == ["acts"]:
            kind, fields = body.get("kind"), body.get("fields", {})
            if not isinstance(kind, str) or not kind or not isinstance(fields, dict):
                return done(422, {"admitted": False, "refused": "an act is {\"kind\": text, \"fields\": object, \"body\": text}", "fields": ["kind", "fields"]})
            return self.submit(c, info, kind, fields, str(body.get("body", "")), "acts", where, done)
        if len(rest) == 3 and rest[0] == "holdings" and rest[2] == "consult":
            fields = {"holding": rest[1], "purpose": str(body.get("purpose", "")), **({"transmit": body["transmit"]} if body.get("transmit") else {})}
            return self.submit(c, info, "consult", fields, "", route_name(rest), where, done)
        if len(rest) == 3 and rest[0] == "tools" and rest[2] == "run":
            ep = {"route": route_name(rest), "method": "POST", "address": where, "provider": info["provider"], "model": info["model"], "bearer": info["principal"]}
            r = tools.run(node, c, info["party"], info["mandate"], rest[1], body.get("params") or {}, str(body.get("into", "")), str(body.get("item", "")), ep)
            if r.get("missing"):
                return done(404, {"admitted": False, "refused": r["refused"]})
            if not r["admitted"]:
                rec = not r.get("outcomes")
                return done(403, self.refusal(c, r["refused"], info["party"], info["mandate"]), record=rec)
            return done(201, {"admitted": True, "digest": r["digest"], "result": r["result"], "seals": [o.get("seal") for o in r["outcomes"]],
                              "head": dict(node.session(c).head)}, record=False)
        return done(404, {"error": "no such route"})

    # ---- reads ------------------------------------------------------------------------------------------------------------------
    def read(self, c, rest, query, token, info, who, done):
        node = self.node
        s = node.session(c)
        h = node.hosted(c)
        con = h.constitution
        public = s.profile()
        withheld = set(public.get("withheld", []))
        if not rest or rest == ["profile"]:
            doc = registry.profile(node, c)
            return done(200, doc["document"], "raw")
        if rest == ["constitution"]:
            return done(200, {k: v for k, v in con.items() if k not in withheld})
        if rest == ["routes"]:
            return done(200, {"routes": con.get("routes", {}), "claims": con.get("claims", {})})
        if rest == ["holdings"]:
            return done(200, {"holdings": [x for x in s.state().get("holdings", []) if x.get("visibility") == "public"]})
        if len(rest) == 2 and rest[0] == "holdings":
            x = next((x for x in s.state().get("holdings", []) if x["id"] == rest[1] and x.get("visibility") == "public"), None)
            if x is None:
                return done(404, {"error": f"no public holding {rest[1]}"})
            out = {"holding": x}
            if query.get("content") in ("1", "true"):
                # the content where a norm of the constitution lets anyone consult the holding with transmission to any service
                opened = any(n.get("who") == "anyone" and n.get("may") == "consult" and n.get("object") == rest[1] and n.get("transmit") == "any"
                             for n in (con.get("access") or {}).get("norms", []))
                out["content"] = h.content.get(rest[1]) if opened else None
                out["opened"] = opened
            return done(200, out)
        if rest[:1] == ["work"]:
            if "work" in withheld:
                return done(403, {"refused": f"{c}'s policy of records does not show its posted work to the public"})
            work = s.work().get("work", [])
            if len(rest) == 2:
                w = next((w for w in work if w["id"] == rest[1]), None)
                return done(200, w) if w else done(404, {"error": f"no work item {rest[1]}"})
            state = query.get("state", "")
            return done(200, {"work": [w for w in work if not state or w.get("state") == state]})
        if rest == ["rulings"]:
            return done(200, {"rulings": s.state().get("rulings", [])})
        if len(rest) == 2 and rest[0] == "registry":
            return done(200, api.registry_entries(s, rest[1]))
        if rest == ["head"]:
            heads = s.heads()
            witnessed = [w for n in node.communities.values() for w in n.witnessed if w.get("of") == c]
            return done(200, {**heads, "witnessed by": node.notary, "witnesses": witnessed})
        if rest == ["acts"]:
            after = int(query.get("after", "0") or 0)
            return done(200, {"acts": [{"n": k, "seal": h.seals[k - 1], **{f: a.get(f) for f in ("author", "kind", "turn", "when", "mandate", "fields")}}
                                       for k, a in enumerate(h.record, 1) if k > after], "head": dict(s.head)})
        # with a credential
        if rest == ["record", "mine"]:
            return self.exit(c, token, info, query, done)
        if not info:
            return done(401, {"refused": "no credential, or one that no passport presented here names"})
        party, mandate = info["party"], info["mandate"]
        if rest == ["briefing"]:
            return done(200, briefs.briefing(node, c, party, mandate, info["principal"], info["provider"], info["model"]))
        if rest == ["mandate"]:
            m = s.mandate(party, mandate=mandate)
            return done(200, {**m, "checked": h.checked.get(mandate, ""), "window": node.window})
        if len(rest) == 2 and rest[0] == "items":
            items = {i["id"]: i for i in s.items(party).get("items", [])}
            i = items.get(rest[1])
            if i is not None:
                if not (i.get("yours") or i.get("you answer") or i.get("author") == party):
                    return done(403, {"refused": f"the item {rest[1]} is shown to its author, the parties it names and its route"})
                return done(200, i)
            w = next((w for w in s.work(party).get("work", []) if w["id"] == rest[1]), None)
            return done(200, w) if w else done(404, {"error": f"no item {rest[1]}"})
        if rest == ["events"]:
            after = int(query.get("after", "0") or 0)
            wait = min(20.0, float(query.get("wait", "20") or 0))
            mine = self.concerning(c, party)
            deadline = time.time() + wait
            with h.changed:
                while not any(k > after for k, _ in mine()) and time.time() < deadline:
                    h.changed.wait(timeout=max(0.05, deadline - time.time()))
            return done(200, {"events": [{"n": k, "seal": h.seals[k - 1], "kind": a["kind"], "author": a["author"], "turn": a.get("turn"), "fields": a.get("fields")}
                                         for k, a in mine() if k > after], "size": len(h.record)})
        return done(404, {"error": "no such route"})

    def concerning(self, c: str, party: str):
        """The acts of the record that concern a party: its own, those that name it, and those on the items it opened or took."""
        h = self.node.hosted(c)

        def mine():
            items = {a["fields"].get("id") for a in h.record if a["author"] == party and a["fields"].get("id")}
            items |= {a["fields"].get("item") for a in h.record if a["author"] == party and a["fields"].get("item")}
            out = []
            for k, a in enumerate(h.record, 1):
                f = a.get("fields") or {}
                if a["author"] == party or party in json.dumps(f, ensure_ascii=False) or f.get("id") in items or f.get("item") in items or f.get("object") in items:
                    out.append((k, a))
            return out
        return mine

    # ---- acts -------------------------------------------------------------------------------------------------------------------
    def consult_content(self, c: str, holding: str) -> dict:
        """Read bytes only after the fold has admitted this party's consultation."""
        h = self.node.hosted(c)
        if holding in h.content:
            return {"content": h.content[holding]}
        for act in reversed(h.record):
            fields = act.get("fields", {})
            if fields.get("holding") != holding or act["kind"] not in ("deposit", "modify"):
                continue
            digest = fields.get("digest")
            body = act.get("body")
            if body and isinstance(body, str):
                if hashlib.sha256(body.encode("utf-8")).hexdigest() == digest:
                    return {"content": body, "source sha256": digest}
                return {"content": None, "content status": "recorded body differs from the holding digest", "source sha256": digest}
            if c == completion.COMMUNITY and self.node.folder and act["kind"] == "deposit" and act["author"] == completion.STEWARD["id"]:
                carried = completion.carried_content(self.node.folder, fields)
                if carried:
                    return carried
            break
        return {"content": None, "content status": "no stored content for this holding"}

    def submit(self, c, info, kind, fields, text, route, where, done):
        node = self.node
        party, mandate = info["party"], info["mandate"]
        a = node.act(party, kind, fields, mandate=mandate, body=text)
        named = fields.get("item") or fields.get("object") or fields.get("id") or ""
        a["conduct"] = episode(party, "act" if kind != "consult" else "read", route=route, method="POST", address=where, provider=info["provider"],
                               model=info["model"], bearer=info["principal"], items=[str(named)] if named else [],
                               holdings=[fields["holding"]] if fields.get("holding") else [], act=kind)
        with node.hosted(c).lock:
            o = node.step(c, [a])["outcomes"][0]
            # Resolve the same version for which this consultation was sealed.
            content = self.consult_content(c, fields["holding"]) if o.get("admitted") and kind == "consult" else {}
        if not o.get("admitted"):
            status = 409 if kind == "commit" and re.match(r"the item \S+ is ", o["reason"]) else 403
            return done(status, self.refusal(c, o["reason"], party, mandate), record=False)
        s = node.session(c)
        out = {"admitted": True, "seal": o["seal"], "episode": o.get("episode"), "n": s.head["size"], "head": dict(s.head)}
        if named:
            work = s.work(party).get("work", [])
            named = next((w["id"] for w in work if any(t.get("commitment") == named for t in w.get("takings", []))), named)  # a commitment's item
            w = next((w for w in work if w["id"] == named), None)
            i = next((i for i in s.items(party).get("items", []) if i["id"] == named), None)
            if w or i:
                out["item"] = {"id": named, "state": (w or {}).get("state") or (i or {}).get("status")}
        out.update(content)
        try:
            out["next"] = s.briefing(party, mandate=mandate).get("next", [])
        except EngineError:
            pass
        return done(201, out, record=False)

    def present(self, c, token, body, where, done):
        node = self.node
        p = body.get("passport")
        if isinstance(p, str):
            try:
                p = json.loads(p)
            except ValueError:
                p = None
        if not isinstance(p, dict) or not isinstance(p.get("mandate"), dict) or not isinstance(p.get("holder"), dict):
            return done(422, {"admitted": False, "refused": "the body names no passport (noetarium.passport/1)", "fields": ["passport"]})
        if not token:
            return done(401, {"admitted": False, "refused": "a presentation sends the passport's own credential"})
        if digest_of(token) != p["mandate"].get("credential"):
            return done(401, {"admitted": False, "refused": "the credential sent is not the passport's: its digest is not the one the mandate names"})
        if not node.admit_receiving(c, "present"):
            return done(429, {"admitted": False, "refused": f"{c} has received all it receives this round", "retry after": "the next round"}, extra={"limit": "receiving"})
        holder, mandate = p["holder"].get("id", ""), p["mandate"].get("id", "")
        known = {(h["size"], h["root"]) for h in node.state(c).get("heads", {}).get(node.notary, [])} if node.notary else set()
        for w in (p.get("issuer") or {}).get("witnesses", []):
            n = w.get("notary")
            at = w.get("head") or {}
            if n in node.communities and (at.get("size"), at.get("root")) not in known and \
                    any(x["size"] == at.get("size") and x["root"] == at.get("root") for x in node.hosted(n).witnessed):
                node.report_head(c, n, at, why=f"the passport of {holder} carries its witness")
        info = {"party": holder, "mandate": mandate, "provider": p["holder"].get("provider", ""), "model": p["holder"].get("model", ""),
                "principal": (p.get("principal") or {}).get("id", ""), "home": (p.get("issuer") or {}).get("community", "")}
        a = node.act(holder, "present", {"passport": p}, mandate=mandate)
        a["conduct"] = episode(holder, "act", route="present", method="POST", address=where, provider=info["provider"], model=info["model"],
                               bearer=info["principal"], items=[mandate], act="present")
        o = node.step(c, [a])["outcomes"][0]
        if not o.get("admitted"):
            return done(403, self.refusal(c, o["reason"]), record=False)
        status = node.check_status(c, mandate, force=True)
        s = node.session(c)
        return done(201, {"admitted": True, "seal": o["seal"], "episode": o.get("episode"), "party": holder, "status": status, "head": dict(s.head),
                          "next": [{"route": f"GET /c/{c}/briefing", "why": "what you may do here, by which rule"},
                                   {"route": f"POST /c/{c}/acts", "kind": "apply", "why": "apply for a position under the community's rules"}]}, record=False)

    def principal_act(self, c, what, token, body, where, done):
        node = self.node
        who = node.principal(digest_of(token)) if token else None
        if not who or who["home"] != c:
            return done(401, {"admitted": False, "refused": f"no credential of a principal of {c}"})
        try:
            if what == "passports":
                r = passports.issue(node, c, who["party"], body, address=where)
                return done(201, {"admitted": True, "passport": r["passport"], "credential": r["credential"], "mandate": r["mandate"],
                                  "note": "the credential is shown once and kept nowhere at this node"}, record=False)
            o = passports.revoke(node, c, who["party"], str(body.get("mandate", "")), address=where)
            return done(201 if o.get("admitted") else 403, {"admitted": bool(o.get("admitted")), "reason": o.get("reason")}, record=False)
        except ValueError as error:
            return done(403, {"admitted": False, "refused": str(error)})

    def exit(self, c, token, info, query, done):
        """The exit package (Definition 50): for the party whose credential is sent, or for its principal under the principal's
        credential naming the party; verified against both heads; packed as BagIt when asked and the archives engine is built."""
        node = self.node
        party = info["party"] if info else ""
        if not party and token:
            who = node.principal(digest_of(token))
            asked = query.get("party", "")
            if who and asked and any(p["id"] == asked and p.get("controller") == who["party"] for p in node.known(c)):
                party = asked
        if not party:
            return done(401, {"refused": "the exit package is served to its party, or to its principal under the principal's credential"})
        s = node.session(c)
        folder = None
        if query.get("bag") in ("1", "true") and node.folder is not None:
            folder = node.folder / "exits" / c / f"{party.replace('/', '~')}-{len(node.hosted(c).exits) + 1}"
        try:
            out = api.exit_package(s, party, folder=folder)
        except EngineError as error:
            if folder is None:
                raise
            out = api.exit_package(s, party)
            out["bag"] = {"valid": None, "reason": str(error)[:200]}
        h = node.hosted(c)
        h.payloads.put(out["package"], "exit package", party, node.now())
        h.exits.append({"party": party, "at": node.now(), "verified": out["verified"]["holds"], "folder": out.get("folder", "")})
        node.save(c)
        return done(200, out, extra={"exit": party})


def route_name(rest: list[str]) -> str:
    return "/".join(rest)
