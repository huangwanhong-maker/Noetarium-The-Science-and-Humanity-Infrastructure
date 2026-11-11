"""The land-use and transport study (ADR 0094): builds study.cpp in WSL (or with this computer's g++) against the foundations and
the domains, runs it on the data library's journeys to work in New York City, and writes results/summary.json and
results/tracts.csv. The figures are analyse.py's.

    python research/experiments/2026-11-12-land-use-and-transport/run.py

The product is made first if it is missing (python data/products/commuting.py nyc-lodes: about 420 MB of LODES's files, kept in
data/cache/raw/lodes/).
"""

from __future__ import annotations

import pathlib
import shutil
import subprocess
import sys
import time

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[2]
PRODUCT = ROOT / "data" / "cache" / "products" / "commuting" / "nyc-lodes"
RESULTS = HERE / "results"
INCLUDE = ["foundations/morphe/*/include", "foundations/calx/numerics/*/include", "foundations/calx/compute/*/include", "foundations/physis/*/include",
           "foundations/tyche/*/include", "foundations/nomos/*/include", "domains/urban-science/*/include", "domains/architecture/*/include"]
WSL = shutil.which("g++") is None and sys.platform == "win32"


def path(p: pathlib.Path) -> str:
    return subprocess.run(["wsl", "-e", "wslpath", "-a", str(p)], capture_output=True, text=True).stdout.strip() if WSL else str(p)


def build() -> str:
    dirs = []
    for pattern in INCLUDE:
        dirs += sorted(p for p in ROOT.glob(pattern) if p.is_dir())
    out = "$HOME/.cache/science-build/land-use-and-transport" if WSL else str(HERE / "results" / "build")
    args = ["g++", "-std=c++20", "-O3", "-march=native", "-DNDEBUG", "-pthread"] + [f"-I{path(d)}" for d in dirs] + [path(HERE / "study.cpp")]
    started = time.time()
    line = f"mkdir -p {out} && " + " ".join(f"'{a}'" for a in args) + f" -o {out}/study"
    r = subprocess.run(["wsl", "-e", "bash", "-c", line] if WSL else ["bash", "-c", line], capture_output=True, text=True)
    if r.returncode != 0:
        sys.exit(r.stderr[-4000:])
    print(f"built the study in {time.time() - started:.0f} s")
    return f"{out}/study"


def main() -> None:
    if not (PRODUCT / "index.json").exists():
        subprocess.run([sys.executable, str(ROOT / "data" / "products" / "commuting.py"), "nyc-lodes"], check=True)
    RESULTS.mkdir(exist_ok=True)
    exe = build()
    line = f"{exe} '{path(PRODUCT)}' '{path(RESULTS)}'"
    started = time.time()
    r = subprocess.run(["wsl", "-e", "bash", "-c", line] if WSL else ["bash", "-c", line])
    if r.returncode != 0:
        sys.exit(r.returncode)
    print(f"ran the study in {time.time() - started:.0f} s; results in {RESULTS.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
