"""Serve the Urban Studio on this computer (ADR 0102).

The Urban Studio is a project-based work for the analysis of cities and the life in them. Its server is the API's host for project-based
works (api/python/science/studio_host.py, which Market Studio's design made general), configured here: its page, its plug-ins under
plugins/ (each bringing tools, templates, an engine and declared schemes), and its projects under data/cache/urban-studio/projects/.

Usage::

    python applications/urban-studio/serve.py [--host 127.0.0.1] [--port 9090] [--open] [--store DIR] [--python EXE] [--token T]
"""

from __future__ import annotations

import sys
from pathlib import Path

APP = Path(__file__).resolve().parent
ROOT = APP.parents[1]
sys.path.insert(0, str(APP))
sys.path.insert(0, str(ROOT / "api" / "python"))

import engine  # noqa: E402
from science import studio_host  # noqa: E402

studio_host.configure(APP, name="urban-studio", title="Urban Studio", port=9090, engines=engine.status)

if __name__ == "__main__":
    sys.exit(studio_host.main(sys.argv[1:]))
