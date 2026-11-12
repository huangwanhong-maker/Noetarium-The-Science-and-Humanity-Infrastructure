"""Where a species lives, though not always seen: occupancy from repeated visits, with detection modelled."""

import math
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _forest as f  # noqa: E402


def create(api):
    class Occupancy(api.BoardAnalysis):
        id = "occupancy"
        title = "Occupancy, though not always seen"
        description = ("Sites visited several times, a species detected or not at each visit (a bird at a point count, a frog at a pond, a bat at "
                       "a detector): the share of sites it occupies (ψ) and the chance of detecting it where it is (p), with covariates of the "
                       "sites, by MacKenzie et al.'s model (peira's imperfect detection). The naive share of sites where it was seen "
                       "underestimates ψ whenever p < 1; each site's posterior chance of being occupied is given for the map.")
        params = [api.param("table", "Visits", "visits", kind="text", role="table", help="one row a site"),
                  api.param("visits", "Visit columns", "", kind="text", role="columns", of="table", help="one column a visit: 1 detected, 0 not, empty not visited"),
                  api.param("covariates", "Site covariates", "", kind="text", role="columns", of="table", help="columns of the sites that may explain ψ"),
                  api.param("site", "Site", "", kind="text", role="column", of="table"),
                  api.param("lat", "Latitude", "", kind="text", role="column", of="table"),
                  api.param("lon", "Longitude", "", kind="text", role="column", of="table")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            cols = [c.strip() for c in str(params["visits"]).split(",") if c.strip()]
            if len(cols) < 2:
                raise ValueError("occupancy needs at least two visit columns: detection is told from repeated visits")
            data = [t.column(c) for c in cols]
            y = [[(None if not math.isfinite(data[j][i]) else (1 if data[j][i] > 0 else 0)) for j in range(len(cols))] for i in range(t.rows)]
            site = {}
            for c in [c.strip() for c in str(params["covariates"]).split(",") if c.strip()]:
                v = t.column(c)
                finite = [x for x in v if math.isfinite(x)]
                mu = sum(finite) / max(1, len(finite))
                sd = (sum((x - mu) ** 2 for x in finite) / max(1, len(finite) - 1)) ** 0.5 or 1
                site[c] = [(x - mu) / sd if math.isfinite(x) else 0 for x in v]   # standardised, so that coefficients compare
            a = f.ecology.ask("occupancy", y=y, site=site)
            coef = {"part": ["ψ"] * len(a["psi_names"]) + ["p"] * len(a["p_names"]), "term": a["psi_names"] + a["p_names"],
                    "logit_coefficient": [round(x, 4) for x in a["psi"] + a["p"]], "se": [round(x, 4) for x in a["psi_se"] + a["p_se"]]}
            sites = {"site": f._text(t, params["site"]) if params.get("site") and t.has(params["site"]) else [str(i + 1) for i in range(t.rows)],
                     "psi": [round(x, 4) for x in a["site_psi"]], "occupied": [round(x, 4) for x in a["occupied"]],
                     "seen": ["seen" if any(v == 1 for v in row) else "" for row in y]}
            roles = {"name": "site", "amount": "occupied"}
            if params.get("lat") and params.get("lon") and t.has(params["lat"]) and t.has(params["lon"]):
                sites["lat"], sites["lon"] = list(t.column(params["lat"])), list(t.column(params["lon"]))
                roles.update(lat="lat", lon="lon")
            p0 = 1 / (1 + math.exp(-a["p"][0]))
            return [api.table("occupancy_model", coef, title="ψ and p: their coefficients on the logit scale (covariates standardised)"),
                    api.table("occupancy_sites", sites, title="Each site: its ψ, and the chance it is occupied given what was seen", roles=roles,
                              show={"kind": "layer", "layer": "points", "colour": "occupied", "map": "sea"} if "lat" in sites else None),
                    api.note(f"Seen at {a['sites_detected']:.0f} of {a['sites_surveyed']:.0f} sites ({f.share(a['naive'])}); with detection modelled the species "
                             f"occupies about {f.share(a['finite_sample'])} of them (mean ψ {f.share(a['mean_psi'])}), detected at a visit where present with "
                             f"p ≈ {f.share(p0)}. Presumed: sites closed to change across the visits, detections independent given occupancy."
                             + ("" if a["converged"] else " The fit did not converge: read it with care."))]
    return Occupancy()
