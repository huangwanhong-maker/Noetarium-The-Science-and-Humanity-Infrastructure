"""The Studio's projects on the server (the design note of 2026-11-07, §4): a project is a folder holding project.json and its scripts.
A new project is made from visualization/scene/project.js's shape, kept the same here so that the server need not run the page's code,
and seeded with the three examples of scripts (in JavaScript, Python and C++) that share one resource, so that a first project runs."""

from __future__ import annotations

import json
import re
import time
from pathlib import Path

APP = Path(__file__).resolve().parent
EXAMPLES = APP / "src" / "studio" / "examples"


def slug(title: str) -> str:
    s = re.sub(r"[^a-z0-9]+", "-", str(title).lower()).strip("-")[:48]
    return s or "project"


def fresh_name(root: Path, title: str) -> str:
    base, k = slug(title), 1
    name = base
    while (root / name).exists():
        k += 1
        name = f"{base[:44]}-{k}"
    return name


def create(root: Path, title: str, project: dict | None = None) -> tuple[str, dict]:
    name = fresh_name(root, title)
    d = root / name
    (d / "scripts").mkdir(parents=True)
    now = int(time.time() * 1000)
    # The clock starts at half past one in the afternoon in Melbourne (UTC+11 in summer) of this UTC date, so that the first view is in daylight.
    start = (now // 86400000) * 86400000 + int(2.5 * 3600000)
    if project is None:
        project = {
            "schema": "noetarium.project/1",
            "meta": {"title": title, "created": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()), "saved": None},
            "scene": {"camera": {"lon": 144.9631, "lat": -37.8136, "height": 0, "range": 900000, "tilt": 0, "heading": 0},
                      "clock": {"start_ms": start, "rate": 1, "tick_hz": 10}, "theme": "night", "base": "globe"},
            "canvas": {"width": 1600, "height": 1000, "background": "theme"},
            # The figure: the world filling the canvas, a title, a locator map following it, two charts and the north and scale, each a frame.
            "frames": [
                {"id": "world", "kind": "world", "title": "The world", "x": 0, "y": 0, "w": 1600, "h": 1000, "back": "none"},
                {"id": "title", "kind": "text", "title": "Title", "x": 44, "y": 30, "w": 980, "h": 92, "back": "none",
                 "text": {"text": "A crowd that wanders, where it gathers, and how far it spreads", "size": 34, "font": "serif", "align": "left"}},
                {"id": "locator", "kind": "map", "title": "Where", "x": 1196, "y": 36, "w": 368, "h": 224, "back": "vellum",
                 "map": {"follows": "world", "projection": "equal-earth"}},
                {"id": "densest", "kind": "chart", "title": "The densest cell", "x": 44, "y": 742, "w": 520, "h": 222, "back": "vellum", "chart": {}},
                {"id": "distances", "kind": "chart", "title": "Distances", "x": 592, "y": 742, "w": 440, "h": 222, "back": "vellum", "chart": {}},
                {"id": "compass", "kind": "drawing", "title": "North and scale", "x": 1430, "y": 794, "w": 134, "h": 170, "back": "vellum"},
            ],
            "layers": [], "resources": [], "scripts": [],
        }
        # The examples: a crowd that wanders (JavaScript) writes "crowd"; where it gathers (Python) and its spread (C++) read it and draw
        # charts in frames; the north and scale (JavaScript) draws from the view alone.
        for ident, file, language in (("crowd", "crowd.js", "js"), ("gathering", "gathering.py", "python"), ("spread", "spread.cpp", "cpp"),
                                      ("compass", "compass.js", "js")):
            source = EXAMPLES / file
            if source.exists():
                (d / "scripts" / file).write_text(source.read_text(encoding="utf-8"), encoding="utf-8")
                project["scripts"].append({"id": ident, "file": file, "language": language, "enabled": True, "params": {}})
                if ident != "compass":  # it draws in a frame only, nothing on the world
                    project["layers"].append({"id": f"script:{ident}", "title": ident, "kind": "script", "script": ident, "visible": True, "opacity": 1})
    (d / "project.json").write_text(json.dumps(project, ensure_ascii=False, indent=1), encoding="utf-8")
    return name, project
