"""Serve Multilateral Phronimos on this computer (ADR 0132).

Multilateral Phronimos is a project-based work for heterogeneous multilateral practice: an undertaking among parties of every kind
(states, organisations, cities, communities, persons, artificial systems) drawn as a round with no head, its relations as threads in
layers and what the record does not observe marked as such; its trajectory as strands beneath; a partnership's commitments, contributions
and each party's benefit by its own criteria; its decisions, their authority, evidence and routes of challenge, and the undertaking's
own diagnostics; the a priori power of the rules its bodies decide by; ideal points from votes; bargaining and the division of what is
shared. Its server is the API's host for project-based works (api/python/science/studio_host.py), configured here with its page, its
plug-ins under plugins/ (each templates and rules), its own routes (dossier.py) and its projects under
data/cache/multilateral-phronimos/projects/.

Usage::

    python applications/multilateral-phronimos/serve.py [--host 127.0.0.1] [--port 9210] [--open] [--store DIR] [--people --keeper NAME]
"""

from __future__ import annotations

import sys
from pathlib import Path

APP = Path(__file__).resolve().parent
ROOT = APP.parents[1]
sys.path.insert(0, str(APP))
sys.path.insert(0, str(ROOT / "api" / "python"))

import dossier  # noqa: E402
import engine  # noqa: E402
from science import multilateral as _multilateral  # noqa: E402
from science import studio_host  # noqa: E402

studio_host.configure(APP, name="multilateral-phronimos", title="Multilateral Phronimos", port=9210, engines=engine.status, routes=dossier.routes)

# The engine is kept running, a request a line (ADR 0120): a record is read again whenever it changes.
_multilateral.ENGINE.keep(2)

if __name__ == "__main__":
    sys.exit(studio_host.main(sys.argv[1:]))
