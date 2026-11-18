"""A search for needs by evolution: statements generated without a language model, varied, judged on half of the areas, admitted
through a reusable holdout, and confirmed on the areas never seen, in order."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _needs as n  # noqa: E402


def create(api):
    class Evolve(api.BoardAnalysis):
        id = "evolve"
        title = "Search for needs by evolution"
        description = ("A population of statements of need evolves for the generations asked, with the strategies chosen: generators (the language walked, "
                       "subgroup discovery, a regression tree, the declared rules, the simulation's unmet demand), mutations and crossovers, a strategy of "
                       "survival. A statement that clears its gap stably and productively on half of the areas, and that a quarter of the areas, asked "
                       "through a reusable holdout, does not contradict, is kept; each kept statement is then tested once on the last quarter, by an online "
                       "procedure that controls false discoveries. The view Needs holds a search of the project's own, kept and continued; this tool runs one apart.")
        params = n.case_params(api) + [
            api.param("generations", "Generations", 10, minimum=1, maximum=60, step=1),
            api.param("survival", "Survival", "plus", kind="choice", options=["plus", "generational", "aging", "deterministic-crowding", "restricted-tournament", "clearing", "nsga2", "alps", "niches"]),
            api.param("parents", "Parents", "tournament", kind="choice", options=["tournament", "uniform", "ranking", "boltzmann", "truncation", "lexicase", "epsilon-lexicase", "novelty"]),
            api.param("schedule", "Choice of operators", "fixed", kind="choice", options=["fixed", "probability-matching", "adaptive-pursuit", "ucb", "thompson"]),
            api.param("rule", "Confirmation", "e-lond", kind="choice", options=["e-lond", "lond", "lord++", "saffron", "alpha-investing"]),
        ]

        def run(self, board, params, context):
            d, case = n.data_of(params)
            options = {"strategy": {"survival": params.get("survival") or "plus", "parents": params.get("parents") or "tournament",
                                    "schedule": params.get("schedule") or "fixed"}, "rule": params.get("rule") or "e-lond"}
            if options["strategy"]["parents"] in ("lexicase", "epsilon-lexicase"):
                options["objectives"] = ["strata"]
            run = n.nd.ask("evolve", data=d, options=options, generations=int(params.get("generations") or 10))
            rep = run["report"]
            rows = [n.row(k["text"], k.get("evidence") or {}) for k in rep["archive"]]
            rows.sort(key=lambda r: -(r.get("z") or 0))
            confirmed = sum(1 for c in rep["confirmations"] if c["confirmed"])
            out = [api.table("needs_found", n.columns(rows), title=f"Needs kept after {rep['generation']} generations")] if rows else []
            out.append(api.note(f"{rep['evaluations']} statements judged in {rep['generation']} generations; {len(rep['admissions'])} admitted, "
                                f"{len(rep['archive'])} kept, {len(rep['retired'])} retired; {confirmed} of {len(rep['confirmations'])} confirmed on the areas kept aside "
                                f"by {rep['online']['rule']} at α = {rep['online']['alpha']}. The holdout was asked {rep['holdout']['queries']} times and told "
                                f"{rep['holdout']['overfits']} of {rep['holdout']['budget']}. The journal of confirmations ends in {rep['journal_head'][:16]}…"))
            return out
    return Evolve()
