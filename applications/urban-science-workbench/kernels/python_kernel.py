"""The Studio's Python kernel (the design note of 2026-11-07, §8.7): a process kept running for the Studio, holding the systems of its
projects' Python scripts between ticks. The protocol is science.studio's: one JSON request a line, one answer a line.

    python -I applications/urban-science-workbench/kernels/python_kernel.py
"""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[3] / "api" / "python"))

from science.studio import serve  # noqa: E402

if __name__ == "__main__":
    sys.exit(serve())
