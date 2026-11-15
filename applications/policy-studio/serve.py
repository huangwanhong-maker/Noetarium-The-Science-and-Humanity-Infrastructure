"""Serve the Policy Studio on this computer (ADR 0122).

The Policy Studio is a project-based work for evidence-based policy making: a question worked from its theory of change (how the
intervention is meant to bring about the outcome, link by link) through the evidence for each link (studies pooled and graded by a
declared scheme) and the argument that what worked there will work here, to the options appraised and the decision: adopt, reject,
or study first. Its server is the API's host for project-based works (api/python/science/studio_host.py), configured here with its
page, its plug-ins under plugins/ (each a subject's templates and grading schemes), its own routes (questions.py) and its projects
under data/cache/policy-studio/projects/.

Usage::

    python applications/policy-studio/serve.py [--host 127.0.0.1] [--port 9190] [--open] [--store DIR] [--people --keeper NAME]
"""

from __future__ import annotations

import sys
from pathlib import Path

APP = Path(__file__).resolve().parent
ROOT = APP.parents[1]
sys.path.insert(0, str(APP))
sys.path.insert(0, str(ROOT / "api" / "python"))

import engine  # noqa: E402
import questions  # noqa: E402
from science import policy as _policy  # noqa: E402
from science import studio_host  # noqa: E402

studio_host.configure(APP, name="policy-studio", title="Policy Studio", port=9190, engines=engine.status, routes=questions.routes)

# The engine is kept running, a request a line (ADR 0120): a question is appraised again whenever it changes.
_policy.ENGINE.keep(2)

if __name__ == "__main__":
    sys.exit(studio_host.main(sys.argv[1:]))
