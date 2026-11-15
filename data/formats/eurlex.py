"""Official texts of European Union law as the Publications Office publishes them (XHTML from CELLAR), read into provisions.

An act of the Official Journal is published with its structure marked: each recital in a ``div.eli-subdivision`` with id ``rct_N``,
each article in one with id ``art_N`` (its title in a ``div.eli-title``), each numbered paragraph in a ``div`` with id ``NNN.PPP``,
and each point of a list as a row of a two-column table, its marker ("(a)", "(i)", "1.") in the first cell. An annex's clauses (the
standard contractual clauses of Implementing Decision (EU) 2021/914) are not so marked: a heading "Clause N" opens each.

``read_act`` gives the provisions, each with an id that says where it stands, its label as a lawyer cites it, its words, and its
parent:

    {"id": "gdpr/art-46/2/c", "label": "Article 46(2)(c)", "parent": "gdpr/art-46/2",
     "text": "standard data protection clauses adopted by the Commission in accordance with the examination procedure referred to in
              Article 93(2);"}

An article's own text is its title; a paragraph's is its words without the points beneath it, and with whatever follows them; marks
of footnotes are left out. Nothing is interpreted: the words are the publication's, with runs of whitespace made one space.

``read_judgment`` gives a judgment of the Court of Justice or the General Court as its numbered paragraphs and its whole text, for
finding a passage in it.

Reuse: the Commission's documents may be reused with their source acknowledged (Commission Decision 2011/833/EU; EUR-Lex's legal
notice says the same of its data). Only the Official Journal as published is authentic; a text read here is for working with.
"""

from __future__ import annotations

import html.parser
import re


class Node:
    __slots__ = ("tag", "attrs", "children")

    def __init__(self, tag: str, attrs: dict):
        self.tag, self.attrs, self.children = tag, attrs, []

    def cls(self) -> str:
        return self.attrs.get("class", "") or ""

    def find_all(self, test) -> list["Node"]:
        out = []
        for c in self.children:
            if isinstance(c, Node):
                if test(c):
                    out.append(c)
                out += c.find_all(test)
        return out


class _Builder(html.parser.HTMLParser):
    VOID = {"br", "col", "img", "meta", "link", "hr", "input"}

    def __init__(self):
        super().__init__(convert_charrefs=True)
        self.root = Node("root", {})
        self.stack = [self.root]

    def handle_starttag(self, tag, attrs):
        node = Node(tag, dict(attrs))
        self.stack[-1].children.append(node)
        if tag not in self.VOID:
            self.stack.append(node)

    def handle_startendtag(self, tag, attrs):
        self.stack[-1].children.append(Node(tag, dict(attrs)))

    def handle_endtag(self, tag):
        for k in range(len(self.stack) - 1, 0, -1):
            if self.stack[k].tag == tag:
                del self.stack[k:]
                break

    def handle_data(self, data):
        self.stack[-1].children.append(data)


def parse(xhtml: str) -> Node:
    b = _Builder()
    b.feed(xhtml)
    return b.root


def text(node) -> str:
    """A node's words, footnote marks left out, runs of whitespace made one space."""
    parts: list[str] = []

    def walk(n):
        if isinstance(n, str):
            parts.append(n)
            return
        if "oj-note-tag" in n.cls() or n.tag in ("script", "style"):
            return
        if n.tag == "br":
            parts.append(" ")
        for c in n.children:
            walk(c)

    walk(node)
    return re.sub(r"\s+", " ", "".join(parts)).strip()


MARKER = re.compile(r"^(?:\(([0-9]{1,3}|[a-zA-Z]{1,5})\)|([0-9]{1,3}|[a-zA-Z])[.)]|[—–-])$")


def _marker(s: str) -> str | None:
    """The key of a list's marker ("(a)" → "a", "1." → "1", a dash → ""), or None when the text is no marker."""
    m = MARKER.match(s.strip())
    if not m:
        return None
    return (m.group(1) or m.group(2) or "")


def _rows(table: Node) -> list[list[Node]]:
    rows = []
    for tr in table.find_all(lambda n: n.tag == "tr"):
        # Only this table's own rows: a row of a table nested in a cell belongs to that table.
        cells = [c for c in tr.children if isinstance(c, Node) and c.tag in ("td", "th")]
        rows.append(cells)
    return rows


def _own_rows(table: Node) -> list[list[Node]]:
    out = []

    def walk(n, depth):
        for c in n.children:
            if not isinstance(c, Node):
                continue
            if c.tag == "tr":
                out.append([d for d in c.children if isinstance(d, Node) and d.tag in ("td", "th")])
            elif c.tag != "table":
                walk(c, depth)

    walk(table, 0)
    return out


def body(nodes) -> tuple[str, list[dict]]:
    """The words of a run of nodes and the points listed among them: (text, [{"marker", "text", "points"}])."""
    texts: list[str] = []
    points: list[dict] = []
    for n in nodes:
        if isinstance(n, str):
            if n.strip():
                texts.append(re.sub(r"\s+", " ", n).strip())
            continue
        if n.tag == "p":
            t = text(n)
            if t:
                texts.append(t)
        elif n.tag == "table":
            listed = False
            for cells in _own_rows(n):
                if len(cells) == 2 and _marker(text(cells[0])) is not None:
                    sub_text, sub_points = body(cells[1].children)
                    points.append({"marker": _marker(text(cells[0])), "shown": text(cells[0]), "text": sub_text, "points": sub_points})
                    listed = True
                else:
                    t = " ".join(filter(None, (text(c) for c in cells)))
                    if t:
                        texts.append(t)
            if not listed and not texts:
                pass
        elif n.tag in ("div", "tbody", "span", "blockquote"):
            t, p = body(n.children)
            if t:
                texts.append(t)
            points += p
    return "\n".join(texts), points


def _add_points(out: list[dict], parent: str, label: str, points: list[dict]) -> None:
    seen: dict[str, int] = {}
    for k, p in enumerate(points):
        key = p["marker"] or str(k + 1)
        seen[key] = seen.get(key, 0) + 1
        ident = f"{parent}/{key}" + (f"~{seen[key]}" if seen[key] > 1 else "")
        shown = p["shown"] if p["marker"] else "—"
        lab = f"{label}({p['marker']})" if p["marker"] else f"{label}, indent {k + 1}"
        out.append({"id": ident, "label": lab, "text": p["text"], "parent": parent, "marker": shown})
        _add_points(out, ident, lab, p["points"])


def read_act(xhtml: str, prefix: str, clauses: bool = True) -> list[dict]:
    """The provisions of an act of the Official Journal: its recitals, its articles with their paragraphs and points, and, where an
    annex sets out numbered clauses, those clauses."""
    root = parse(xhtml)
    out: list[dict] = []
    subdivisions = root.find_all(lambda n: n.tag == "div" and "eli-subdivision" in n.cls())
    for d in subdivisions:
        ident = d.attrs.get("id", "")
        m = re.fullmatch(r"rct_(\d+)", ident)
        if m:
            t = body(d.children)
            words = t[0]
            if t[1]:   # a recital is set as one row: its number, then its words
                words = "\n".join(filter(None, [words] + [p["text"] for p in t[1]]))
            out.append({"id": f"{prefix}/rct-{m.group(1)}", "label": f"Recital {m.group(1)}", "text": words, "parent": ""})
            continue
        m = re.fullmatch(r"art_(\d+[a-z]?)", ident)
        if not m:
            continue
        n = m.group(1)
        art, label = f"{prefix}/art-{n}", f"Article {n}"
        titles = d.find_all(lambda x: x.tag == "div" and "eli-title" in x.cls())
        title = text(titles[0]) if titles else ""
        paragraphs = [c for c in d.children if isinstance(c, Node) and c.tag == "div" and re.fullmatch(r"\d{3}\.\d{3}", c.attrs.get("id", ""))]
        rest = [c for c in d.children if isinstance(c, Node) and c not in paragraphs and "eli-title" not in c.cls() and "oj-ti-art" not in c.cls() and "oj-sti-art" not in c.cls()]
        own_text, own_points = body(rest)
        out.append({"id": art, "label": label, "title": title, "text": own_text or title, "parent": ""})
        _add_points(out, art, label, own_points)
        for p in paragraphs:
            k = str(int(p.attrs["id"].split(".")[1]))
            words, points = body(p.children)
            words = re.sub(r"^\d+\.\s+", "", words)
            pid, plabel = f"{art}/{k}", f"{label}({k})"
            out.append({"id": pid, "label": plabel, "text": words, "parent": art})
            _add_points(out, pid, plabel, points)
    if clauses:
        out += read_clauses(root, prefix)
    return out


def read_clauses(root: Node, prefix: str) -> list[dict]:
    """The numbered clauses an annex sets out, each opened by a heading "Clause N": its title, its words and its points. Where a
    clause gives a text for each of several modules, the points of the later modules repeat the earlier markers and are told apart
    by a suffix (``/a~2``)."""
    out: list[dict] = []
    # The clauses' headings and bodies are siblings: walk every container that holds a "Clause N" heading directly.
    heading = lambda n: isinstance(n, Node) and n.tag == "p" and "oj-ti-grseq-1" in n.cls()   # noqa: E731
    containers = [c for c in [root] + root.find_all(lambda n: True) if any(heading(k) and re.fullmatch(r"Clause \d+", text(k)) for k in c.children)]
    seen: set[str] = set()
    for c in containers:
        kids = [k for k in c.children if isinstance(k, Node)]
        i = 0
        while i < len(kids):
            m = re.fullmatch(r"Clause (\d+)", text(kids[i])) if heading(kids[i]) else None
            if not m:
                i += 1
                continue
            n = m.group(1)
            j = i + 1
            title = ""
            if j < len(kids) and heading(kids[j]) and not re.fullmatch(r"Clause \d+", text(kids[j])) and not text(kids[j]).upper().startswith("SECTION"):
                title = text(kids[j])
                j += 1
            run = []
            while j < len(kids) and not (heading(kids[j]) and (re.fullmatch(r"Clause \d+", text(kids[j])) or text(kids[j]).upper().startswith(("SECTION", "ANNEX", "APPENDIX")))):
                run.append(kids[j])
                j += 1
            ident, label = f"{prefix}/clause-{n}", f"Clause {n}"
            if ident not in seen:
                seen.add(ident)
                words, points = body(run)
                out.append({"id": ident, "label": label, "title": title, "text": words or title, "parent": ""})
                _add_points(out, ident, label, points)
            i = j
    return out


def read_judgment(xhtml: str) -> dict:
    """A judgment as its numbered paragraphs ([{"n", "text"}]) and its whole text, a paragraph a line."""
    root = parse(xhtml)
    paragraphs: list[dict] = []
    lines: list[str] = []
    # A paragraph's number stands in a p of class "coj-count" (judgments since about 2018) or "count" (earlier ones).
    counted = lambda n: n.tag == "p" and any(t in ("count", "coj-count") for t in n.cls().split())   # noqa: E731
    for table in root.find_all(lambda n: n.tag == "table"):
        for cells in _own_rows(table):
            if len(cells) == 2:
                count = cells[0].find_all(counted)
                if count and re.fullmatch(r"\d+", text(count[0])) and not cells[1].find_all(lambda n: n.tag == "table" and n.find_all(lambda q: counted(q) and re.fullmatch(r"\d+", text(q)))):
                    words, points = body(cells[1].children)
                    full = "\n".join(filter(None, [words] + [f"{p['shown']} {p['text']}" for p in points]))
                    paragraphs.append({"n": int(text(count[0])), "text": full})
    for p in root.find_all(lambda n: n.tag == "p"):
        t = text(p)
        if t:
            lines.append(t)
    return {"paragraphs": paragraphs, "text": "\n".join(lines)}
