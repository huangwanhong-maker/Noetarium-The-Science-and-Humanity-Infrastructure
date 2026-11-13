"""Reference values for environmentally extended input-output analysis, from pymrio's own test system.

pymrio (Stadler 2021, J. Open Res. Softw. 9: 8; GPL-3, used here only to compute reference numbers)
ships a small multi-regional table, six regions of eight sectors, with an extension "emissions". Its
calc_all() gives the accounts by region: D_cba_reg (consumption-based, with F_Y), D_pba_reg
(production-based, with F_Y), D_imp_reg and D_exp_reg. This script writes the table and those accounts
to eeio_pymrio_test.txt in plain sections the C++ check reads:

  regions R / sectors n / stressors K, then Z (R n rows), Y (R n rows, R columns, summed over each
  region's final demand categories), F (K rows), F_Y (K rows, R columns, summed likewise), and the four
  accounts (K rows, R columns each).

Run:  PYTHONPATH=<a folder holding pymrio 0.7> .venv/Scripts/python domains/industrial-ecology/tests/fixtures/make_eeio_fixtures.py
(pymrio 0.7.0, pandas 2, numpy 2; generated 2026-11-13).
"""
from pathlib import Path

import pymrio

io = pymrio.load_test()
io.calc_all()
regions = list(io.get_regions())
sectors = list(io.get_sectors())
ext = io.emissions


def by_region(frame):
    return frame.T.groupby(level="region", sort=False).sum().T[regions]


Y = by_region(io.Y)
FY = by_region(ext.F_Y)
rows = [f"{r}|{s}" for r in regions for s in sectors]
Z = io.Z.loc[[(r, s) for r in regions for s in sectors], [(r, s) for r in regions for s in sectors]]
F = ext.F[[(r, s) for r in regions for s in sectors]]
out = Path(__file__).with_name("eeio_pymrio_test.txt")


def write(f, name, frame):
    f.write(f"{name} {frame.shape[0]} {frame.shape[1]}\n")
    for row in frame.values:
        f.write(" ".join(f"{v:.17g}" for v in row) + "\n")


with out.open("w", encoding="utf-8", newline="\n") as f:
    f.write(f"# pymrio {pymrio.__version__} load_test(), emissions; generated 2026-11-13\n")
    f.write("regions " + " ".join(regions) + "\n")
    f.write("sectors " + " ".join(s.replace(" ", "_") for s in sectors) + "\n")
    f.write("stressors " + " ".join("/".join(map(str, k)) if isinstance(k, tuple) else str(k) for k in ext.F.index) + "\n")
    write(f, "Z", Z)
    write(f, "Y", Y)
    write(f, "F", F)
    write(f, "F_Y", FY)
    write(f, "D_cba_reg", ext.D_cba_reg[regions])
    write(f, "D_pba_reg", ext.D_pba_reg[regions])
    write(f, "D_imp_reg", ext.D_imp_reg[regions])
    write(f, "D_exp_reg", ext.D_exp_reg[regions])
print("wrote", out, len(regions), "regions,", len(sectors), "sectors,", len(ext.F.index), "stressors")
