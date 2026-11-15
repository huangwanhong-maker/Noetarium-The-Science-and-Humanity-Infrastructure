"""Process tracing: rival explanations weighed clue by clue."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import _common as c  # noqa: E402


def create(api):
    class Tracing(api.BoardAnalysis):
        id = "tracing"
        title = "Weigh rival explanations"
        description = ("Rival explanations of one case, each with a prior, weighed by clues: for each clue, how likely it is to be found under each explanation, "
                       "and whether it was found or sought and missing. The posterior after each clue, and the weight of evidence between each pair in decibans "
                       "(Good 1950; Fairfield and Charman 2017); with two explanations, each clue is also named by Van Evera's tests (hoop, smoking gun, "
                       "doubly decisive, straw in the wind). The clues are taken as independent given the explanation, and that is said.")
        params = [api.param("hypotheses", "Explanations", "", kind="text", role="table"),
                  c.col(api, "name", "Explanation", of="hypotheses"),
                  c.col(api, "prior", "Prior", of="hypotheses"),
                  api.param("clues", "Clues", "", kind="text", role="table", help="one row a clue; one column for each explanation, P(clue | explanation)"),
                  c.col(api, "clue", "Clue", of="clues"),
                  c.col(api, "found", "Found", of="clues", help="1 or yes when found; 0 or no when sought and missing")]

        def run(self, board, params, context):
            h, k = board.table(params["hypotheses"]), board.table(params["clues"])
            names, priors = c.text(h, params["name"]), c.numbers(h, params["prior"])
            c.need(k, names + [params["clue"], params["found"]])
            clues = []
            found = c.text(k, params["found"])
            for i, name in enumerate(c.text(k, params["clue"])):
                clues.append({"name": name, "likelihood": [c.numbers(k, n)[i] for n in names], "found": found[i].lower() not in ("0", "no", "false", "missing")})
            r = c.inquiry.ask("weigh", hypotheses=[{"name": n, "prior": p} for n, p in zip(names, priors)], clues=clues)
            steps = {"after": ["(prior)"] + [s["clue"] for s in r["steps"]]}
            for j, n in enumerate(names):
                steps[n] = [r["prior"][j]] + [s["posterior"][j] for s in r["steps"]]
            out = [api.table("posterior", steps, title="The explanations' probabilities, clue by clue")]
            if len(names) == 2:
                tests = c.inquiry.ask("tests", clues=[{"name": x["name"], "certainty": x["likelihood"][0], "uniqueness": 1 - x["likelihood"][1]} for x in clues])["tests"]
                out.append(api.table("tests", {"clue": [x["clue"] for x in tests], "test": [x["test"] for x in tests], "certainty": [x["certainty"] for x in tests],
                                               "uniqueness": [x["uniqueness"] for x in tests]}, title=f"Van Evera's tests for {names[0]} against {names[1]}"))
            db = r["decibans"]
            pairs = [f"{names[a]} over {names[b]}: {db[a][b]:+.1f} dB" for a in range(len(names)) for b in range(a + 1, len(names))]
            out.append(api.note(f"Leading: {r['leading']}. " + "; ".join(pairs) + f". Assumed: {r['assumption']}."))
            return out
    return Tracing()
