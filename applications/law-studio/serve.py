"""Serve the Law Studio on this computer (ADR 0121).

The Law Studio is a project-based work for legal practice: a matter's evidence kept and recognised, the law it is read under stated
clause by clause, the open terms read with the courts' holdings weighed for the forum, each question answered clear, arguable or
untenable with what it rests on, and the plan of proof. Its server is the API's host for project-based works
(api/python/science/studio_host.py), configured here with its page, its plug-ins under plugins/ (a body of law each: its schemes and
templates), its own routes (matters.py) and its projects under data/cache/law-studio/projects/.

Usage::

    python applications/law-studio/serve.py [--host 127.0.0.1] [--port 9170] [--open] [--store DIR] [--people --keeper NAME]
"""

from __future__ import annotations

import sys
from pathlib import Path

APP = Path(__file__).resolve().parent
ROOT = APP.parents[1]
sys.path.insert(0, str(APP))
sys.path.insert(0, str(ROOT / "api" / "python"))

import engine  # noqa: E402
import matters  # noqa: E402
from science import law as _law  # noqa: E402
from science import studio_host  # noqa: E402

studio_host.configure(APP, name="law-studio", title="Law Studio", port=9170, engines=engine.status, routes=matters.routes)

# The engine is kept running, a request a line (ADR 0120): a matter is evaluated again whenever it changes.
_law.ENGINE.keep(2)

if __name__ == "__main__":
    sys.exit(studio_host.main(sys.argv[1:]))
