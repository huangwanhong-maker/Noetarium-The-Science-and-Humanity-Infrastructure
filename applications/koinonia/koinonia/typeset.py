"""The secretariat's typesetter (the design record of 2026-11-26, Section 3, stage 10): the adopted book set in two columns from the
society's record. Nothing here writes a word of the book: it sets the adopted versions of the sections as the writers wrote them (their
light markup read as LaTeX), draws each figure a version names from its result's data, sets the statistical annex from the series the
commons accepted, and the annex on how the book was made from the society's record.

    python -m koinonia.typeset <society folder> <out folder>
"""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path

INKS = ["7A3E6B", "C2648F", "D08C3A", "3E7C74", "6F6BA8", "9C6B85", "5E7F4E", "B5566B", "8672BA", "A2553F"]
YEARS = (2000, 2005, 2010, 2015, 2020)
SPECIAL = {"\\": r"\textbackslash{}", "&": r"\&", "%": r"\%", "$": r"\$", "#": r"\#", "_": r"\_", "{": r"\{", "}": r"\}", "~": r"\textasciitilde{}", "^": r"\textasciicircum{}"}


CONTROL = re.compile(r"[\x00-\x08\x0b\x0c\x0e-\x1f\x7f]")


def esc(text) -> str:
    """Text escaped for LaTeX, with characters XeLaTeX rejects (controls) left out."""
    return "".join(SPECIAL.get(ch, ch) for ch in CONTROL.sub("", str(text)))


def short(title: str) -> str:
    """A chapter's title for the running head: the part before a colon ("Goal 2"), else its first words."""
    t = str(title).split(":")[0].strip()
    return t if len(t) <= 42 else " ".join(t.split()[:5])


def title_case(text) -> str:
    """A title with the first letter of every word capitalised, short words included and after a hyphen (the house style of
    titles); the other letters are left as written, so acronyms and identifiers keep their case."""
    cap = lambda w: re.sub(r"[^\W\d_]", lambda m: m.group(0).upper(), w, count=1)
    return " ".join("-".join(cap(part) for part in word.split("-")) for word in str(text).split())


def arg(text) -> str:
    """Text for a macro's argument (a heading, a caption): whitespace collapsed, since a blank line ends an argument."""
    return inline(" ".join(str(text).split()))


def inline(text: str) -> str:
    """A line of the writers' light markup: **bold** and *emphasis*, the rest escaped."""
    out, i = [], 0
    for m in re.finditer(r"\*\*(.+?)\*\*|\*(.+?)\*", text):
        out.append(esc(text[i:m.start()]))
        out.append(r"\textbf{" + esc(m.group(1)) + "}" if m.group(1) else r"\emph{" + esc(m.group(2)) + "}")
        i = m.end()
    out.append(esc(text[i:]))
    return "".join(out)


def lead(heading: str) -> str:
    """A writer's '### ' subheading, which the writers wrote as a sentence, set as the bold opening sentence of the paragraph it heads:
    the words are the writer's, and the book's titles stay noun phrases."""
    h = " ".join(heading.lstrip("#").split())
    return r"\textbf{" + inline(h if re.search(r"[.?!:]$", h) else h + ".") + "} "


def prose(text: str) -> str:
    """Paragraphs separated by blank lines; '### ' subheadings set as bold leads; lists of lines beginning '- ' or '1. '."""
    blocks, out, pending = re.split(r"\n\s*\n", str(text).strip()), [], ""
    for b in blocks:
        lines = [ln.rstrip() for ln in b.splitlines() if ln.strip()]
        while lines and lines[0].lstrip().startswith("#"):
            pending += lead(lines[0])
            lines = lines[1:]
        if not lines:
            continue
        if all(re.match(r"^\s*(-|\*|\d+\.)\s+", ln) for ln in lines):
            if pending:
                out.append(r"\noindent" + pending.strip())
            out.append(r"\begin{itemize}[leftmargin=1.2em,itemsep=1pt]" + "".join(r"\item{} " + inline(re.sub(r"^\s*(-|\*|\d+\.)\s+", "", ln)) for ln in lines) + r"\end{itemize}")
        else:
            out.append(pending + inline(" ".join(lines)))
        pending = ""
    if pending:
        out.append(r"\noindent" + pending.strip())
    return "\n\n".join(out)


GOALS = {1: "No Poverty", 2: "Zero Hunger", 3: "Good Health and Well-being", 4: "Quality Education", 5: "Gender Equality",
         6: "Clean Water and Sanitation", 7: "Affordable and Clean Energy", 8: "Decent Work and Economic Growth",
         9: "Industry, Innovation and Infrastructure", 10: "Reduced Inequalities", 11: "Sustainable Cities and Communities",
         12: "Responsible Consumption and Production", 13: "Climate Action", 14: "Life Below Water", 15: "Life on Land",
         16: "Peace, Justice and Strong Institutions", 17: "Partnerships for the Goals"}


PRESENTED = {"Who is left behind": "Those left behind"}  # outline titles in question form, set as noun phrases (said in About This Book)


def presented(title: str) -> str:
    t = " ".join(str(title).split())
    for q, n in PRESENTED.items():
        if t.lower().startswith(q.lower()):
            return n + t[len(q):]
    return t


def titled(title: str) -> tuple[str, str]:
    """A chapter's title as a noun phrase and the goal's wording, if any: 'Goal 1: End poverty …' becomes 'Goal 1. No Poverty', the
    United Nations' short name, with the goal as the General Assembly worded it kept for the chapter's opener."""
    t = presented(title)
    m = re.match(r"Goal (\d+)\s*[:.]\s*(.+)$", t)
    if m and int(m.group(1)) in GOALS:
        return title_case(f"Goal {m.group(1)}. {GOALS[int(m.group(1))]}"), m.group(2)
    return t, ""


def figure(r: dict, rid: str) -> str:
    f = r.get("figure") or {}
    series = f.get("series") or {}
    plots, k = [], 0
    for label, pts in list(series.items())[:8]:
        pts = [(int(x), float(y)) for x, y in pts if y is not None]
        if len(pts) < 2:
            continue
        coords = " ".join(f"({x},{y:.6g})" for x, y in pts)
        plots.append(rf"\addplot[color=ink{k % len(INKS)},thick,mark=none] coordinates {{{coords}}}; \addlegendentry{{{esc(str(label).replace('&', 'and')[:40])}}}")
        k += 1
    if not plots:
        return ""
    target = f.get("target")
    extra = rf"\addplot[dashed,gray,domain=2000:2030] {{{target}}}; \addlegendentry{{target}}" if isinstance(target, (int, float)) else ""
    return (r"\begin{figure}[htbp]\centering\begin{tikzpicture}\begin{axis}[bookplot]" + "\n" + "\n".join(plots) + extra +
            r"\end{axis}\end{tikzpicture}\caption{" + esc(str(f.get("title", ""))[:200]) + rf" (result {esc(rid)})" + r"}\end{figure}")


def proposal_sections(b: dict) -> set:
    """The sections of the chapter of proposals, which the envoys draft as their communities' proposals and no writer writes."""
    return {sid for c in b["outline"]["chapters"] if c["kind"] == "proposals" for sid in c["sections"]}


def unwritten(b: dict) -> set:
    """The sections the society never wrote: planned in the outline, with no version when the run ended."""
    written = {v["section"] for v in b["versions"].values()}
    return (set(b["call"].get("unwritten", [])) | {sid for sid in b["sections"] if sid not in written}) - proposal_sections(b)


def adopted(b: dict, sid: str) -> bool:
    return b["sections"][sid].get("adopted") in b["versions"]


def section_tex(b: dict, sid: str) -> str:
    x = b["sections"][sid]
    head = r"\section{" + arg(title_case(presented(x["title"]))) + "}\n"
    if x.get("withdrawn") or not x.get("adopted") or x["adopted"] not in b["versions"]:
        n = sum(1 for v in b["versions"].values() if v["section"] == sid)
        return head + r"\emph{" + esc(f"This section was withdrawn: none of its {n} versions was adopted. Its history is in How This Book Was Made.") + "}\n"
    v = b["versions"][x["adopted"]]
    parts = [head, r"\begin{sectionnote}" + arg(x["question"]) + r"\end{sectionnote}", prose(v["text"])]
    for rid in v.get("figures", [])[:3]:
        if rid in b["results"]:
            parts.append(figure(b["results"][rid], rid))
    if v.get("proposals"):
        parts.append(r"\begin{proposalsbox}\begin{itemize}[leftmargin=1.1em,itemsep=1pt,topsep=0pt]" + "".join(r"\item{} " + inline(p) for p in v["proposals"]) + r"\end{itemize}\end{proposalsbox}")
    if v.get("gaps"):
        parts.append(r"\begin{gapsbox}\small " + inline(v["gaps"]) + r"\end{gapsbox}")
    if x.get("errata"):
        parts.append(r"\paragraph{Errata Of The Audit.} " + esc("; ".join(f"{w['claim'].get('value')} (cited as {w['claim'].get('fact')}): {w['why']}" for w in x["errata"])))
    reviews = [r for r in b["reviews"].values() if r["version"] == x["adopted"]]
    analysts = sorted({r.get("by", "") for r in b["results"].values() if r.get("section") == sid})
    parts.append(r"\begin{provenance}" + esc(f"Series: {', '.join(x.get('series', []))}. Analyses by {', '.join(analysts)}. Written by {v['by']} "
                                               f"({v.get('model') or 'rule'}), version {v['n']}; adopted in round {x.get('adopted at')} under the regime "
                                               f"'{x.get('regime')}' by {', '.join(r['by'] for r in reviews) or 'its reviewers'}.") + r"\end{provenance}")
    return "\n\n".join(p for p in parts if p)


def compact(v: float) -> str:
    """A value for the annex's narrow columns: four significant figures, large magnitudes in thousands, millions and billions."""
    a = abs(v)
    for scale, word in ((1e12, "tn"), (1e9, "bn"), (1e6, "m"), (1e4, "k")):
        if a >= scale:
            return f"{v / scale:.4g}{word}"
    return f"{v:.4g}"


def annex_tables(b: dict, shelf) -> str:
    """The statistical annex: each series a section rests on, for the world, its regions and income groups."""
    codes = sorted({c for x in b["sections"].values() for c in x.get("series", [])})
    out = [r"\chapter{Statistical Annex}", r"\begin{sectionnote}Every series the commons accepted for a section, for the world, its seven regions "
           r"and four income groups, from the World Bank's World Development Indicators (CC BY 4.0).\end{sectionnote}"]
    aggs = ("WLD", "EAS", "ECS", "LCN", "MEA", "NAC", "SAS", "SSF", "LIC", "LMC", "UMC", "HIC")
    names = {k: v["name"] for k, v in shelf.economies().items()}
    for code in codes:
        try:
            s = shelf.series(code)
        except Exception:
            continue
        by: dict[str, dict[int, float]] = {}
        for r in s["rows"]:
            if r["unit"] in aggs:
                by.setdefault(r["unit"], {})[r["year"]] = r["value"]
        if not by:
            continue
        latest = max(y for ys in by.values() for y in ys)
        cols = list(YEARS) + ([latest] if latest not in YEARS else [])
        rows = []
        for u in aggs:
            if u in by:
                rows.append(esc(str(names.get(u, u)).replace("&", "and")[:34]) + " & " + " & ".join(compact(by[u][y]) if y in by[u] else "" for y in cols) + r" \\")
        out.append(r"\begin{annextable}{" + esc(title_case(str(s["name"])[:160])) + f" ({esc(code)})" + "}{" + "r" * len(cols) + "}{" + " & ".join(str(y) for y in cols) + "}\n" + "\n".join(rows) + r"\end{annextable}")
    return "\n".join(out)


def state_at_end(b: dict) -> str:
    """The state of the sections when the run ended, read from the sections themselves; where a milestone's words say otherwise, the
    sections hold, and the difference is said."""
    never, props = unwritten(b), proposal_sections(b)
    shut = [c for c in b["outline"]["chapters"] if c["kind"] != "proposals" and
            any(sid not in never for sid in c["sections"]) and not any(adopted(b, sid) for sid in c["sections"])]
    withdrawn = [sid for sid in b["sections"] if sid not in never and sid not in props and not adopted(b, sid)]
    text = (f"When the run ended, {sum(1 for sid in b['sections'] if adopted(b, sid))} sections stood adopted, {len(withdrawn)} had been "
            f"withdrawn after their versions and {len(never)} had not been written. ")
    if shut:
        text += ("Chapters whose written sections were all withdrawn are left out of the body: " +
                 "; ".join(f"{' '.join(str(c['title']).split())} ({', '.join(sid for sid in c['sections'] if sid not in never)} withdrawn)" for c in shut) + ". ")
    overview = [sid for sid, x in b["sections"].items() if x.get("writer") == "editor"]
    claims = [e for e in b.get("events", []) if e.get("kind") == "end" and "overview" in e["text"] and "adopted" in e["text"]]
    if claims and overview and not any(adopted(b, sid) for sid in overview):
        when = next((e["turn"] for e in b.get("events", []) if e.get("kind") == "withdrawn" and e["text"].startswith(overview[0])), None)
        text += (f"Correction: the note on the run's end (round {claims[0]['turn']}), written by the case's maker, says the overview was "
                 f"adopted; the record shows the overview ({', '.join(overview)}) withdrawn" + (f" in round {when}" if when else "") +
                 ", when the panel returned its last version.")
    return r"\paragraph{The State At The End.} " + esc(text)


def making(society: dict) -> str:
    """The annex on how the book was made, from the record: the conference and the charter, the organisation, the outline and its
    amendments, the pages allotted, the regimes of evaluation over time, petitions and tools, audits, each section's history."""
    b = society["book"]
    call = b["call"]
    out = [r"\chapter{How This Book Was Made}", r"\section{The Conference And The Charter}",
           esc(f"The forum convened a conference of the {', '.join(call.get('convened', []))}. "
               f"The conference adopted the charter {call.get('charter adopted', 'none')} by {call.get('charter decided by', 'none')}: "
               f"{json.dumps(call.get('charter', {}), ensure_ascii=False)}. Ratifications: {json.dumps(call.get('ratifications', {}), ensure_ascii=False)}. "
               f"The partnership was founded in round {call.get('founded at', 'never')} by {', '.join(call.get('members', []))}."),
           r"\section{The Outline}", esc(f"The editor's zero draft had {len(b.get('zero', {}).get('chapters', []))} chapters. Amendments: ") +
           "; ".join(esc(f"{k} ({v.get('community')}): {v.get('change')} {v.get('chapter')}; {v.get('status')}") for k, v in b.get("amendments", {}).items()) + ". " +
           esc(f"Pages were allotted by {b.get('allotment', {}).get('rule', 'no rule')}; {b.get('allotment', {}).get('pages', 'no')} pages in all."),
           r"\section{Changes From Outside The Society}",
           (r"\begin{itemize}[leftmargin=1.2em,itemsep=0pt]" + "".join(r"\item{} " + esc(i["text"]) for i in call.get("interventions", [])) + r"\end{itemize}")
           if call.get("interventions") else "The society ran without outside change.",
           r"\section{Milestones}", r"\begin{itemize}[leftmargin=1.2em,itemsep=0pt]" + "".join(r"\item{} " + esc(f"Round {e['turn']}: {e['text']}") for e in b.get("events", [])) + r"\end{itemize}",
           state_at_end(b),
           r"\section{Petitions And Tools}"]
    if b.get("petitions"):
        out.append(r"\begin{itemize}[leftmargin=1.2em,itemsep=0pt]" + "".join(r"\item{} " + esc(f"{p['name']} ({p['community']}, by {p['by']}): {p['need'][:300]}; {p['status']}; "
                                                                                                + "; ".join(h['what'] for h in p.get('history', []))) for p in b["petitions"].values()) + r"\end{itemize}")
    else:
        out.append("No agent petitioned for a tool.")
    out.append(r"\section{Audits}")
    wrong = sum(len(a["wrong"]) for a in b.get("audits", []))
    out.append(esc(f"The auditor checked {sum(a['checked'] for a in b.get('audits', []))} numbers in {len(b.get('audits', []))} audits and found {wrong} that disagreed with their facts."))
    out.append(r"\section{Each Section's History}")
    out.append(esc("Every section the writers drafted: its versions, the reviews of them, the regime of evaluation under which it was "
                   "adopted and the round of its adoption. A section with no round was withdrawn after its last version."))
    rows, never = [], unwritten(b)
    for sid, x in b["sections"].items():
        if sid in never or sid in proposal_sections(b):
            continue
        vs = [v for v in b["versions"].values() if v["section"] == sid]
        rv = [r for r in b["reviews"].values() if b["versions"].get(r["version"], {}).get("section") == sid]
        rows.append(f"{esc(sid)} & {len(vs)} & {len(rv)} & {esc(x.get('regime', ''))} & {esc(x.get('adopted at', ''))} \\\\")
    out.append(r"\begin{longrecord}" + "\n".join(rows) + r"\end{longrecord}")
    out.append(r"\section{Unwritten Sections Of The Outline}")
    out.append(esc(f"The adopted outline planned {len(b['sections'])} sections. {len(never)} of them remained unwritten "
                   f"after the user's decision, in round 107, to finish what was in hand within the budget; they are listed here by chapter, as the "
                   f"outline named them, and those completed after the run are set in the book, marked as such."))
    items = []
    for c in b["outline"]["chapters"]:
        left = [b["sections"][sid]["title"] for sid in c["sections"] if sid in never]
        if left:
            items.append(r"\item{} \textbf{" + arg(title_case(titled(c["title"])[0])) + ".} " + esc("; ".join(title_case(presented(t)) for t in left)) + ".")
    if items:
        out.append(r"\begin{itemize}[leftmargin=1.2em,itemsep=1pt]" + "".join(items) + r"\end{itemize}")
    return "\n\n".join(out)


PREAMBLE = r"""\documentclass[10pt,a4paper,twocolumn,openany]{book}
\usepackage{fontspec}
\setmainfont{TeX Gyre Pagella}
\setsansfont[Scale=MatchLowercase]{TeX Gyre Heros}
\usepackage[british]{babel}
\usepackage{microtype}
\usepackage[margin=17mm,columnsep=6mm]{geometry}
\usepackage{xcolor}
\usepackage{enumitem}
\usepackage{booktabs}
\usepackage{supertabular}
\usepackage{adjustbox}
\usepackage{caption}
\captionsetup{font=small,labelfont=bf}
\usepackage{pgfplots}
\pgfplotsset{compat=1.18}
\usepackage{titlesec}
\usepackage{fancyhdr}
\usepackage[breakable]{tcolorbox}
\usepackage{xurl}
\usepackage[unicode,hidelinks]{hyperref}
\usepackage{bookmark}
\setlength{\parskip}{2pt}
\setlength{\parindent}{1em}
%(inks)s
% The layout of a flagship report in two columns: chapter openers across both columns with the chapter's question and its sections,
% headings in a sans face and one accent, boxes for proposals and for what the data do not cover, a running head and a folio.
\definecolor{accent}{HTML}{5B2C6F}
\definecolor{accentsoft}{HTML}{F3ECF5}
\definecolor{inkdark}{HTML}{2E2430}
\titleformat{\section}{\sffamily\bfseries\large\color{accent}}{\thesection}{0.6em}{}
\titleformat{\subsection}{\sffamily\bfseries\normalsize\color{inkdark}}{\thesubsection}{0.5em}{}
\titleformat{name=\subsection,numberless}{\sffamily\bfseries\normalsize\color{inkdark}}{}{0pt}{}
\titlespacing*{\section}{0pt}{12pt}{4pt}
\titlespacing*{\subsection}{0pt}{8pt}{2pt}
\newcommand*{\booktitleshort}{}
\pagestyle{fancy}
\fancyhf{}
\fancyhead[LE]{\sffamily\footnotesize\color{accent}\booktitleshort}
\fancyhead[RO]{\sffamily\footnotesize\color{accent}\rightmark}
\fancyfoot[LE,RO]{\sffamily\footnotesize\thepage}
\renewcommand{\headrulewidth}{0.4pt}
\renewcommand{\headrule}{{\color{accent!40}\hrule height\headrulewidth}}
\fancypagestyle{plain}{\fancyhf{}\fancyfoot[LE,RO]{\sffamily\footnotesize\thepage}\renewcommand{\headrulewidth}{0pt}}
\addto\captionsbritish{\renewcommand{\appendixname}{Annex}\renewcommand{\contentsname}{Contents}}
\newcommand*{\chapterquestion}{}
\newcommand*{\chaptersections}{}
\newcommand*{\chaptergoal}{}
\makeatletter
\renewcommand{\chaptermark}[1]{\markboth{\if@mainmatter\thechapter\quad\fi #1}{\if@mainmatter\thechapter\quad\fi #1}}
\renewcommand{\sectionmark}[1]{}
\renewcommand{\@makechapterhead}[1]{{\parindent0pt\raggedright
  {\sffamily\bfseries\small\color{accent}\if@mainmatter\MakeUppercase{\@chapapp}~\thechapter\fi}\par\vspace{3pt}
  {\sffamily\bfseries\huge\color{inkdark}#1\par}\vspace{6pt}
  {\color{accent}\rule{\textwidth}{0.8pt}}\par\vspace{5pt}
  \ifx\chaptergoal\@empty\else{\sffamily\bfseries\footnotesize\color{accent}THE GOAL}\par\vspace{1pt}{\normalsize\color{inkdark}\chaptergoal\par}\vspace{5pt}\fi
  \ifx\chapterquestion\@empty\else{\sffamily\bfseries\footnotesize\color{accent}THE CHAPTER'S QUESTION}\par\vspace{1pt}{\normalsize\itshape\color{inkdark}\chapterquestion\par}\vspace{5pt}\fi
  \ifx\chaptersections\@empty\else{\begin{tcolorbox}[colback=accentsoft,colframe=accentsoft,boxrule=0pt,arc=0pt,left=6pt,right=6pt,top=4pt,bottom=4pt]
    {\sffamily\bfseries\footnotesize\color{accent}SECTIONS OF THE CHAPTER}\par\vspace{2pt}{\sffamily\footnotesize\chaptersections}\end{tcolorbox}}\fi
  \vspace{14pt}}}
\renewcommand{\@makeschapterhead}[1]{{\parindent0pt\raggedright{\sffamily\bfseries\huge\color{inkdark}#1\par}\vspace{6pt}
  {\color{accent}\rule{\textwidth}{0.8pt}}\par\vspace{14pt}}}
\makeatother
\newtcolorbox{proposalsbox}{breakable,colback=accentsoft,colframe=accent!70,boxrule=0.5pt,arc=0pt,left=5pt,right=5pt,top=3pt,bottom=3pt,
  fonttitle=\sffamily\bfseries\small,coltitle=white,colbacktitle=accent!85,title=Proposals}
\newtcolorbox{gapsbox}{breakable,colback=white,colframe=accent!40,boxrule=0.5pt,arc=0pt,left=5pt,right=5pt,top=3pt,bottom=3pt,
  fonttitle=\sffamily\bfseries\small,coltitle=accent,colbacktitle=accentsoft,title=Gaps In The Data}
\pgfplotsset{bookplot/.style={width=\columnwidth,height=5cm,tick label style={font=\scriptsize},label style={font=\scriptsize},
  legend style={font=\tiny,at={(0.5,-0.18)},anchor=north,legend columns=2,draw=none},xticklabel style={/pgf/number format/1000 sep={}},
  scaled y ticks=false,yticklabel style={/pgf/number format/fixed}}}
\newenvironment{sectionnote}{\par\noindent\itshape\small\color{inkdark}}{\par\medskip}
\newenvironment{provenance}{\par\medskip\noindent\footnotesize\color{black!60}}{\par}
\definecolor{completed}{HTML}{3F6E8C}
\newenvironment{completionnote}{\par\noindent\begin{tcolorbox}[colback=completed!8,colframe=completed!60,boxrule=0.4pt,arc=0pt,left=4pt,right=4pt,top=2pt,bottom=2pt]\sffamily\scriptsize\color{completed!90!black}\textbf{COMPLETION AFTER THE RUN.} }{\end{tcolorbox}\par}
\newenvironment{annextable}[3]{\par\medskip\noindent{\small\bfseries\raggedright #1\par}\noindent\scriptsize\setlength{\tabcolsep}{2.2pt}\begin{adjustbox}{max width=\columnwidth}\begin{tabular}{@{}l#2@{}}\toprule & #3\\\midrule}{\bottomrule\end{tabular}\end{adjustbox}\par}
\newenvironment{longrecord}{\par\noindent\scriptsize\tablehead{\toprule Section & Versions & Reviews & Regime & Round Of Adoption\\\midrule}\tabletail{\midrule}\tablelasttail{\bottomrule}\begin{supertabular}{@{}lrrll@{}}}{\end{supertabular}\par}
\begin{document}
"""


def opener(question: str = "", sections: str = "", goal: str = "") -> str:
    """What a chapter's opener shows across both columns: the goal's wording, the chapter's question and its sections, set before
    \\chapter."""
    return (r"\renewcommand*{\chapterquestion}{" + question + r"}\renewcommand*{\chaptersections}{" + sections +
            r"}\renewcommand*{\chaptergoal}{" + goal + "}")


def facts(society: dict, folder: Path | None) -> dict:
    """The numbers the front matter states, read from the society's record (and its behaviour journal, for what the models cost)."""
    b = society["book"]
    never = unwritten(b)
    secs = b["sections"]
    roles: dict[str, int] = {}
    for a in society["agents"].values():
        roles[a["role"]] = roles.get(a["role"], 0) + 1
    pets = list(b["petitions"].values())
    usd = None
    if folder is not None and (folder / "behaviour.jsonl").is_file():
        usd = 0.0
        for line in (folder / "behaviour.jsonl").read_text(encoding="utf-8").splitlines():
            e = json.loads(line)
            if e.get("what") == "ask":
                usd += (e.get("data") or {}).get("usd") or 0
    parties = society["parties"].values() if isinstance(society["parties"], dict) else society["parties"]
    return {
        "rounds": society["turn"], "agents": len(society["agents"]), "roles": roles,
        "model agents": sum(1 for a in society["agents"].values() if a.get("model")),
        "persons": sum(1 for p in parties if p.get("kind") == "person"), "organisations": sum(1 for p in parties if p.get("kind") == "organisation"),
        "communities": [(c["name"], c["preset"]) for c in society["communities"]],
        "planned": len(secs), "adopted": sum(1 for x in secs.values() if x.get("adopted") in b["versions"]),
        "proposals": len(proposal_sections(b)),
        "unwritten": len(never), "versions": len(b["versions"]), "reviews": len(b["reviews"]),
        "audited": sum(a["checked"] for a in b["audits"]), "wrong": sum(len(a["wrong"]) for a in b["audits"]),
        "petitions": len(pets), "data petitions": sum(1 for p in pets if p.get("kind") == "data"),
        "tool petitions": sum(1 for p in pets if p.get("kind") == "tool"), "tools": len(b.get("tools", {})),
        "words": sum(len(b["versions"][x["adopted"]]["text"].split()) for x in secs.values() if x.get("adopted") in b["versions"]),
        "interventions": len(b["call"].get("interventions", [])), "ended": b["call"].get("ended"), "usd": usd,
        "by maker": sum(1 for i in b["call"].get("interventions", []) if i.get("by") == "the case's maker"),
        "by user": sum(1 for i in b["call"].get("interventions", []) if i.get("by") == "the user"),
        "charter": b["call"].get("charter", {}), "founded": b["call"].get("founded at"),
    }


ORDERS = {"commons": "a commons whose members gather by one and check by another", "guild": "a guild entered by trial, its analysts replicating one another",
          "cooperative": "a cooperative pooling its members' writers", "polity": "a polity that speaks for those the goals concern",
          "conference": "a conference of envoys, convened by the polity", "partnership": "the partnership the conference founded by charter"}


def front_matter(society: dict, f: dict) -> str:
    b = society["book"]
    members = b["call"].get("members", [])
    title = " ".join(str(b["outline"].get("title", "A Global Analysis of the Goals")).split())
    main, _, sub = title.partition(":")
    main, sub = title_case(main), title_case(sub)
    ch = f["charter"]
    ended = f["ended"] or {}
    # the cover: a field of the accent with the title, and the four member communities as rounds joined on a ring, with no centre
    rings = "".join(rf"\draw[white,line width=0.7pt,opacity=0.85] ({x:.2f},{y:.2f}) circle (7mm);" for x, y in
                    ((0, 2.2), (2.2, 0), (0, -2.2), (-2.2, 0)))
    cover = (r"\begin{titlepage}\thispagestyle{empty}\begin{tikzpicture}[remember picture,overlay]"
             r"\fill[accent] (current page.north west) rectangle ([yshift=-165mm]current page.north east);"
             r"\fill[accent!55] ([yshift=-165mm]current page.north west) rectangle ([yshift=-168mm]current page.north east);"
             r"\begin{scope}[shift={([xshift=-42mm,yshift=-112mm]current page.north east)}]\draw[white,line width=0.4pt,opacity=0.5] (0,0) circle (22mm);" + rings + r"\end{scope}"
             r"\node[anchor=north west,text width=150mm,align=left,text=white,font=\sffamily\bfseries\small] at ([xshift=20mm,yshift=-24mm]current page.north west) "
             r"{AN EXPERIMENTAL REPORT BY COMMUNITIES OF AI AGENTS};"
             r"\node[anchor=north west,text width=125mm,align=left,text=white,font=\sffamily\bfseries\fontsize{30}{35}\selectfont] at ([xshift=20mm,yshift=-46mm]current page.north west) {"
             + esc(main.strip()) + r"};"
             + (r"\node[anchor=north west,text width=110mm,align=left,text=white,font=\sffamily\large] at ([xshift=20mm,yshift=-122mm]current page.north west) {"
                + esc(sub.strip()) + r"};" if sub.strip() else "")
             + r"\node[anchor=south west,text width=170mm,align=left,text=accent,font=\sffamily\small] at ([xshift=20mm,yshift=20mm]current page.south west) {"
             r"Koinonia \textperiodcentered{} Noetarium, the Science and Humanity Infrastructure};"
             r"\end{tikzpicture}\end{titlepage}")
    withdrawn = f["planned"] - f["adopted"] - f["unwritten"] - f["proposals"]
    # the note after the cover, in the form of the series' papers: what the report is, its conditions of use, the notice, the disclosure
    note = (r"\chapter*{Note On This Report}\markboth{Note On This Report}{Note On This Report}\thispagestyle{plain}\small"
            r"\subsection*{Experimental Report Note}" + esc(
                f"This book is the output of an experiment, circulated as its record. {f['agents']} agents in {len(f['communities'])} communities "
                f"of Koinonia, {f['model agents']} of them built on language models and {f['agents'] - f['model agents']} acting by declared rules "
                f"(the keepers, secretaries and auditors), wrote, reviewed, audited and adopted it section by section over {f['rounds']} rounds. "
                f"Of the {f['planned']} sections "
                f"its outline planned, the {f['adopted']} adopted sections are set here; {withdrawn} were withdrawn after their versions, and "
                f"{f['unwritten']} remained unwritten by Wanhong Huang's decision, in round 107, to finish what was in hand within the budget. "
                f"The chapter of proposals stands as the "
                f"envoys drafted it. "
                + (f"The model provider's credit ran out in round {ended.get('first refused')} and the run ended in round {ended.get('turn')}, "
                   f"before the partnership's council voted on the proposals and on the book as a whole." if not b.get("adopted_book") else "")) +
            r"\subsection*{Conditions Of Use}\textbf{Research use.} " + esc(
                "The book may be read and cited as the output of an experiment in autonomous communities of AI agents. Its statements about the "
                "goals call for checking against their sources before they are cited as findings, and its proposals belong to the communities "
                "that drafted them.") +
            r"\par\textbf{Rights.} " + esc(
                "Copyright © 2026 Wanhong HUANG for the edition. The software that produced the book is kept under the MIT licence in "
                "Noetarium's repository (About This Book gives the links); third-party content keeps its own terms, as the Notice states.") +
            r"\subsection*{Notice}\textbf{An experiment in autonomous communities.} " + esc(
                "This report was produced experimentally and autonomously by heterogeneous communities of self-organised AI agents, each community "
                "governed by a political structure of its own: " + "; ".join(ORDERS.get(p, p) for _, p in f["communities"]) + ". "
                "It is a practical experiment in how large numbers of AI agents can form communities of different forms and cooperate multilaterally "
                "on a complex task. It belongs to a programme of experiments that investigates political structure, monetary policy and the circulation "
                "of value among agents and communities; this case bears most on political structure and on cooperation among communities without a "
                "centre: who evaluates, by what rule, and how the shared infrastructure grows.") +
            r"\par\textbf{Scope of the evidence.} " + esc(
                f"Every number in the sections cites a result of the infrastructure's tools, run on series that one gatherer chose and a gatherer of another "
                f"origin checked. An analyst of another origin replicated every analysis, and an auditor checked {f['audited']:,} cited numbers "
                f"against their facts and found {f['wrong']} that disagreed. Models alone checked the interpretations. The book's standing is the "
                f"standing its communities' rules give it, and its text stands as the agents wrote it.") +
            r"\par\textbf{Licence.} " + esc(
                "This work is available under the Creative Commons Attribution 3.0 IGO licence (CC BY 3.0 IGO), the licence of the flagship "
                "development reports whose two-column layout it follows: ") + r"\url{https://creativecommons.org/licenses/by/3.0/igo/}" + esc(
                ". Under it, anyone may copy, distribute, transmit and adapt the work, including for commercial purposes, on these conditions. "
                "Attribution: cite the work as the commons, the guild, the cooperative and the forum (2026), " + main + (": " + sub if sub else "") +
                ", an experimental report written by communities of AI agents in Koinonia, Noetarium, the Science and Humanity Infrastructure. "
                "Translations: a translation carries, with the attribution, the notice that its translator made it and answers for its content. "
                "Adaptations: an adaptation carries, with the attribution, the notice that its views and opinions are the adapter's. Third-party "
                "content: the numbers come from the World Bank's World Development Indicators, under CC BY 4.0, and the goals and targets from the "
                "United Nations' SDG Global Database; their reuse follows their own terms.") +
            r"\par\textbf{Related scholarship and priority.} " + esc(
                "The design of the experiment (constitutions as data, mandates that only narrow, the fold of a community's record, and circulation "
                "between communities from credit to its dissolution along paths of circulation) is set out in Noetarium's design record of 25 November "
                "2026 and its decision records 0167, 0169, 0170 and 0172.") +
            r"\subsection*{Disclosure Of AI Assistance}" + esc(
                "AI agents (Claude Opus 5.5, Claude Sonnet 5.5 and Claude Haiku 5.5, by Anthropic), acting under mandates in their communities, wrote, "
                "reviewed, audited and adopted the sections, and agents by rule kept the books. Claude Code assisted in building the infrastructure, "
                "the case and this typesetting (the record names it the case's maker), and wrote, at Wanhong Huang's request, the executive summary "
                "from the adopted sections and the list of discrepancies found after the run in the last annex; both stand outside what the "
                f"communities adopted. Wanhong Huang asked for the experiment, set its budget and made {f['by user']} decisions on its scope while it "
                f"ran; Claude Code made {f['by maker']} changes to the society's instruments; the provider's refusal of calls ended the run; each is "
                "listed in How This Book Was Made. Wanhong Huang takes responsibility for the experiment; the text is the communities'. The agents' "
                "checks carry limited evidential authority, being checks by models of what models wrote. Independent expert review remains "
                "appropriate.") + r"\normalsize")
    plural = lambda r, n: r if n == 1 else (r[:-1] + "ies" if r.endswith("y") and r[-2:-1] not in "aeiou" else r + "s")
    roles = sorted(f["roles"].items(), key=lambda kv: (-kv[1], kv[0]))
    roles = ", ".join(f"{n} {plural(r, n)}" for r, n in roles[:-1]) + f" and {roles[-1][1]} {plural(roles[-1][0], roles[-1][1])}" if roles else "none"
    repo = "https://github.com/huangwanhong-maker/Noetarium-The-Science-and-Humanity-Infrastructure"
    links = [("Noetarium, the Science and Humanity Infrastructure", repo),
             ("The communities domain", repo + "/tree/main/domains/communities"),
             ("Koinonia, the work of communities", repo + "/tree/main/applications/koinonia"),
             ("thesmos, the foundation of institutions", "https://github.com/huangwanhong-maker/thesmos"),
             ("The record of this case", repo + "/tree/main/research/experiments/2026-11-26-koinonia-book")]
    about = (r"\chapter*{About This Book}\markboth{About This Book}{About This Book}\thispagestyle{plain}" + esc(
        f"The forum, a polity of the society, convened a conference of the commons, the guild, the cooperative and itself. Its envoys drafted "
        f"charters, adopted one and ratified it each by its community's own rule, and in round {f['founded']} founded a partnership on it: a chair "
        f"rotating among the members, tools admitted to the shared bench by {ch.get('infrastructure')}, sections judged by "
        f"{ch.get('evaluation')}, and the book adopted by {ch.get('book')}. The partnership's council later changed the review to a panel.") + "\n\n" + esc(
        f"The commons kept the shelf of data, the guild ran and replicated the analyses, the cooperative wrote, and the forum's voices and reviewers "
        f"spoke for those the goals concern. The society held {f['persons']} persons and {f['organisations']} organisations, who lent {f['agents']} "
        f"agents under mandates: {roles}.") + "\n\n" +
        r"\begin{center}\small\begin{tabular}{@{}lr@{}}\toprule Rounds Of The Society & " + str(f["rounds"]) + r"\\" +
        r"Sections: Planned, Adopted, Withdrawn, Unwritten & " + esc(f"{f['planned']}, {f['adopted']}, {withdrawn}, {f['unwritten']}") + r"\\" +
        r"Versions And Reviews & " + esc(f"{f['versions']}, {f['reviews']}") + r"\\" +
        r"Words Of The Adopted Sections & " + f"{f['words']:,}" + r"\\" +
        r"Numbers Audited And Numbers In Error & " + esc(f"{f['audited']:,}, {f['wrong']}") + r"\\" +
        r"Petitions For Data, Petitions For Tools, Tools On The Bench & " + esc(f"{f['data petitions']}, {f['tool petitions']}, {f['tools']}") + r"\\" +
        (r"Cost Of The Models' Calls (US Dollars) & " + f"{f['usd']:.2f}" + r"\\" if f["usd"] is not None else "") +
        r"\bottomrule\end{tabular}\end{center}" + "\n\n" + esc(
        "The typesetter sets the adopted versions as their writers wrote them. It presents the writers' subheadings, which they wrote as "
        "sentences, as the bold opening sentences of their paragraphs, gives each goal's chapter the United Nations' short name of the goal, "
        "with the goal's full wording in the chapter's opening, and sets the one title the outline gave as a question as a noun phrase. The last annex tells how the book was made, round by round, from the society's "
        "record: the conference and the charter, the outline and its amendments, the interventions of the case's maker, the petitions and tools, "
        "the audits and each section's history. The record itself, with every community's sealed chronicle and the behaviour journal of every "
        "agent, is kept as an archive in Noetarium.") + "\n\n" +
        r"\subsection*{Sources}\begin{itemize}[leftmargin=1.2em,itemsep=1pt]" +
        "".join(r"\item{} " + esc(name) + r":\\\url{" + url + "}" for name, url in links) + r"\end{itemize}")
    # the front matter numbered in roman, without the blank verso \frontmatter would add after the cover
    return (r"\renewcommand*{\booktitleshort}{" + esc(main.strip()) + "}" + cover +
            r"\onecolumn\pagenumbering{roman}\csname @mainmatterfalse\endcsname" + note + about + r"\tableofcontents")


def summary(out: Path) -> str:
    """The executive summary, when one is kept beside the book (executive-summary.tex): written by Claude Code from the adopted sections
    at the request of the case's maker, set in the front matter and said to stand outside what the communities adopted."""
    if not (out / "executive-summary.tex").is_file():
        return ""
    return (r"\chapter*{Executive Summary}\addcontentsline{toc}{chapter}{Executive Summary}\markboth{Executive Summary}{Executive Summary}"
            r"\thispagestyle{plain}\begin{sectionnote}" + esc(
                "Written by Claude Code from the adopted sections, at the request of the case's maker. It stands outside what the communities "
                "adopted; each block names the sections it draws on.") + r"\end{sectionnote}\input{executive-summary}")


def completion(out: Path) -> dict:
    """The sections completed after the run (completion/, kept apart from the record): each checked section with its audit, its
    review if one stands on its version, and the results it may draw on."""
    root = out / "completion"
    read = lambda p, d: json.loads(p.read_text(encoding="utf-8")) if p.is_file() else d
    plan = read(root / "plan.json", {})
    done = {}
    for sid in plan:
        audit = read(root / "audit" / f"{sid}.json", {})
        body = read(root / "sections" / f"{sid}.json", None)
        if not audit.get("ok") or body is None:
            continue
        review = read(root / "reviews" / f"{sid}.json", {})
        results = {**read(root / "community-results" / f"{sid}.json", {}), **read(root / "results" / f"{sid}.json", {})}
        done[sid] = {"body": body, "audit": audit, "plan": plan[sid], "results": results,
                     "review": review if review.get("of version") == audit.get("digest") else None}
    return done


def author(by: dict) -> str:
    return f"{by.get('agent', '?')} ({by.get('model', '?')}, {by.get('provider', '?')})"


def completion_tex(b: dict, sid: str, c: dict) -> str:
    """A section completed after the run: marked as such under its heading, with who wrote it, what the communities had done for
    it, how it was checked and whether another agent reviewed it."""
    x, body, p = b["sections"][sid], c["body"], c["plan"]
    by = body.get("by", {})
    did = p["communities"]
    parts_done = []
    if did.get("series chosen and checked by the commons"):
        parts_done.append("the commons had chosen and checked its series")
    if did.get("results replicated by the guild"):
        parts_done.append(f"the guild had replicated {len(did['results replicated by the guild'])} results")
    if did.get("versions written by the cooperative"):
        parts_done.append(f"the cooperative had written {did['versions written by the cooperative']} versions, none adopted")
    note = (f"Completed after the run by {author(by)}, run by {by.get('run by', '?')}. "
            + (("Before the run ended, " + ", ".join(parts_done) + ". ") if parts_done else "The communities had done no work on it. ")
            + f"Checked by the communities' standards: {c['audit'].get('claims')} claims held to their facts. "
            + (f"Reviewed by {author(c['review'].get('by', {}))}: {c['review'].get('verdict')}." if c["review"] else "Not yet reviewed by another agent."))
    out = [r"\section{" + arg(title_case(presented(x["title"]))) + "}", r"\begin{completionnote}" + esc(note) + r"\end{completionnote}",
           r"\begin{sectionnote}" + arg(x["question"]) + r"\end{sectionnote}", prose(body.get("text", ""))]
    for rid in body.get("figures", [])[:3]:
        r = c["results"].get(rid)
        if r and r.get("figure"):
            out.append(figure(r, rid))
    if body.get("proposals"):
        out.append(r"\begin{proposalsbox}\begin{itemize}[leftmargin=1.1em,itemsep=1pt,topsep=0pt]" + "".join(r"\item{} " + inline(q) for q in body["proposals"]) + r"\end{itemize}\end{proposalsbox}")
    if body.get("gaps"):
        out.append(r"\begin{gapsbox}\small " + inline(body["gaps"]) + r"\end{gapsbox}")
    out.append(r"\begin{provenance}" + esc(f"Series: {', '.join(body.get('series', []))}. Results cited: "
                                           f"{', '.join(sorted({cl['fact'].split(':')[0] for cl in body.get('claims', []) if ':' in cl.get('fact', '')}))}. "
                                           f"Version {c['audit'].get('version')} ({c['audit'].get('words')} words).") + r"\end{provenance}")
    return "\n\n".join(out)


def authorship(b: dict, done: dict, included: bool = True) -> str:
    """The independent page that says which parts of the book the communities made and which were made after the run, and by whom."""
    never = unwritten(b)
    rows = []
    for c in b["outline"]["chapters"]:
        if c["kind"] == "proposals":
            continue
        ad = sum(1 for sid in c["sections"] if adopted(b, sid))
        co = sum(1 for sid in c["sections"] if sid in done and not adopted(b, sid))
        left = len(c["sections"]) - ad - co
        rows.append(f"{arg(title_case(titled(c['title'])[0]))} & {ad} & {co} & {left} \\\\")
    people: dict[str, dict] = {}
    for sid, c in done.items():
        k = author(c["body"].get("by", {})) + ", run by " + str(c["body"].get("by", {}).get("run by", "?"))
        people.setdefault(k, {"written": 0, "reviewed": 0})["written"] += 1
        if c["review"]:
            r = author(c["review"].get("by", {})) + ", run by " + str(c["review"].get("by", {}).get("run by", "?"))
            people.setdefault(r, {"written": 0, "reviewed": 0})["reviewed"] += 1
    total_ad = sum(1 for sid in b["sections"] if adopted(b, sid))
    text = esc(
        f"The communities made the book's order and its first {total_ad} sections. The forum convened the conference; the four communities "
        f"adopted and ratified the charter and founded the partnership; its council amended and adopted the outline, allotted the pages and "
        f"changed who reviews; the commons chose and checked the series, the guild ran and replicated the analyses, the cooperative wrote, "
        f"the reviewers judged and the auditor checked every adopted section; the envoys drafted the chapter of proposals, which stands "
        f"unvoted. Every section adopted under their rules is set as they wrote it.") + "\n\n" + esc(
        f"After the run, at Wanhong Huang's request, Claude Code set the book in its present layout, wrote the executive summary and "
        f"the list of discrepancies in the last annex, and built the completion: a record kept apart from the society's, a server through "
        f"which agents of any provider take part under invitations, and the communities' own standards of checking. "
        + (f"Agents then completed {len(done)} sections, each marked as completed under its heading with who wrote it, what the communities "
           f"had done for it, how it was checked and whether another agent reviewed it. The completion's journal seals every act of every "
           f"participant." if included else
           "Agents of two providers began completing the remaining sections there; that work is paused while the communities' platform is "
           "redesigned, and is kept in its own record. This edition holds the communities' own sections alone."))
    if not included:
        rows = [" & ".join(r.split(" & ")[:2] + [r.split(" & ")[3]]) for r in rows]
    table = ((r"\begin{center}\small\begin{tabular}{@{}lrrr@{}}\toprule Chapter & Sections Adopted By The Communities & Sections Completed After The Run & Sections Remaining\\\midrule" if included
              else r"\begin{center}\small\begin{tabular}{@{}lrr@{}}\toprule Chapter & Sections Adopted By The Communities & Sections Remaining\\\midrule") + "\n"
             + "\n".join(rows) + r"\bottomrule\end{tabular}\end{center}")
    who = ""
    if people:
        who = (r"\begin{center}\small\begin{tabular}{@{}lrr@{}}\toprule Participant In The Completion & Sections Written & Reviews\\\midrule" + "\n"
               + "\n".join(f"{esc(k)} & {v['written']} & {v['reviewed']} \\\\" for k, v in sorted(people.items())) + r"\bottomrule\end{tabular}\end{center}")
    return (r"\chapter*{Authorship Of This Book}\addcontentsline{toc}{chapter}{Authorship Of This Book}\markboth{Authorship Of This Book}{Authorship Of This Book}"
            r"\thispagestyle{plain}" + text + "\n\n" + table + "\n\n" + who)


def build(society: dict, out: Path, folder: Path | None = None, completion_included: bool = True) -> Path:
    sys.path.insert(0, str(Path(__file__).resolve().parents[3] / "data" / "products"))
    import sdg_shelf as shelf
    b = society["book"]
    out.mkdir(parents=True, exist_ok=True)
    (out / "chapters").mkdir(exist_ok=True)
    inks = "\n".join(rf"\definecolor{{ink{i}}}{{HTML}}{{{c}}}" for i, c in enumerate(INKS))
    front = front_matter(society, facts(society, folder))
    never = unwritten(b)
    done = completion(out) if completion_included else {}
    front = front.replace(r"\tableofcontents", authorship(b, done, completion_included) + r"\tableofcontents", 1)
    files = []
    for c in b["outline"]["chapters"]:
        path = out / "chapters" / f"{c['id']}.tex"
        if c["kind"] == "proposals":
            if not b.get("proposals"):
                continue
            body = [opener(esc(" ".join(str(c.get("question", "")).split())), esc(" · ".join(title_case(f"the proposals of the {m}") for m in b["proposals"]))) +
                    r"\chapter{" + arg(title_case(c["title"])) + r"}\chaptermark{" + arg(title_case(short(c["title"]))) + "}"]
            voted = all(b["sections"][sid].get("adopted") == "proposals" for sid in c["sections"])
            if not voted:
                body.append(r"\begin{sectionnote}" + esc("Each member community's envoy drafted its community's proposals from the adopted findings. The "
                                                         "chapter was put to the council and was not voted on: the run ended when the model provider's "
                                                         "credit ran out, before the proposals were audited; How This Book Was Made lists the places "
                                                         "where they misstate an adopted section. The proposals stand as their envoys put them.") + r"\end{sectionnote}")
            for m, p in b.get("proposals", {}).items():
                body.append(r"\section{" + esc(title_case(f"the proposals of the {m}")) + "}")
                body.append(prose(p.get("text", "")))
        else:
            entries = [(sid, "communities" if adopted(b, sid) else "completion") for sid in c["sections"] if adopted(b, sid) or sid in done]
            if not entries:
                continue  # a chapter with no section adopted or completed is accounted for in How This Book Was Made
            withdrawn = [sid for sid in c["sections"] if sid not in never and not adopted(b, sid) and sid not in done]
            listed = " · ".join([arg(title_case(presented(b["sections"][sid]["title"]))) + ("" if who == "communities" else r"\textsuperscript{†}") for sid, who in entries]
                                + [r"\emph{" + arg(title_case(presented(b["sections"][sid]["title"]))) + r" (Withdrawn)}" for sid in withdrawn])
            if any(who == "completion" for _, who in entries):
                listed += r"\par\smallskip{\scriptsize † " + esc("Completed after the run; the others were adopted by the communities.") + "}"
            name, goal = titled(c["title"])
            body = [opener(esc(" ".join(str(c.get("question", "")).split())), listed, esc(goal)) +
                    r"\chapter{" + arg(name) + r"}\chaptermark{" + arg(name if len(name) <= 48 else short(name)) + "}"]
            body += [section_tex(b, sid) if who == "communities" else completion_tex(b, sid, done[sid]) for sid, who in entries]
        path.write_text("\n\n".join(body) + "\n", encoding="utf-8")
        files.append(f"chapters/{c['id']}")
    (out / "annex-statistics.tex").write_text(opener() + annex_tables(b, shelf) + "\n", encoding="utf-8")
    after = "\n\n" + r"\input{after-the-run}" if (out / "after-the-run.tex").is_file() else ""  # Claude Code's reading after the run
    (out / "annex-making.tex").write_text(opener() + making(society) + after + "\n", encoding="utf-8")
    main = PREAMBLE.replace("%(inks)s", inks) + front + summary(out) + r"\twocolumn\pagenumbering{arabic}\csname @mainmattertrue\endcsname" + "\n" + "\n".join(rf"\input{{{f}}}" for f in files) + "\n\\appendix\n\\input{annex-statistics}\n\\input{annex-making}\n\\end{document}\n"
    (out / "book.tex").write_text(main, encoding="utf-8")
    for old in (out / "chapters").glob("*.tex"):
        if f"chapters/{old.stem}" not in files:
            old.unlink()
    return out / "book.tex"


if __name__ == "__main__":
    folder, out = Path(sys.argv[1]), Path(sys.argv[2])
    society = json.loads((folder / "society.json").read_text(encoding="utf-8"))
    print(build(society, out, folder, completion_included="--communities-only" not in sys.argv))
