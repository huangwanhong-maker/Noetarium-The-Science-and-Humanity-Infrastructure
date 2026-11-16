# Evidence in Space and Time

*Status: draft (2026-11-16; [ADR 0123](../../../../development/decisions/0123-evidence-in-space-and-time.md)).* The Urban Studio's fifth plug-in: an archive of evidence about places and times, kept, weighed and acted on.

## 1. Concepts

- **What is kept is checked whenever it is used.** A file is taken in with the SHA-256 of its bytes and a chain of custody. Each time the archive is appraised, its digest is observed again and its custody followed (archeion). An item resting on a file that changed, or whose custody broke, does not count, and the page says why.
- **An item is about a claim only as far as where and when it may be.** An item is a report of something at a place, within a radius, and a time, within a span. Its share of a claim's disc and interval is the probability that it is about the claim, and its force is diluted by it (tekmerion's `located`).
- **Reports that repeat one registration count once.** Items with one origin (a household that complains five times, papers that print one wire) are one report; the page says what counting them apart would have given (tekmerion's `dependence`).
- **A claim says what its items report.** An event of their own each (lots inspected, sightings), whose force adds up; or one event (witnesses of one fire), which they can only settle.
- **What to do follows from what is declared.** Acts with their worths if the claim holds and if not; a way of looking first, with what it finds and costs; a budget. Then each claim's act, and the places worth looking at first, chosen together (tyche's information bought under a budget, on calx's knapsack).

## 2. Views

| View | Shows |
|---|---|
| *Evidence* | The place as the page: its streets and footprints; claims as discs toned by the weight of their evidence (lavender against, rust for, an outline where nothing bears on one); items as glyphs, a hue a kind, filled when they say the claim holds, a ring when they deny it, a cross when they do not count. A claim opens where it stands: its standing, its weight in decibans, what bears on it and how much, what to do. An item opens with who says it, what it rests on and the claims it bears on, and may be accepted or rejected. Beneath, the items along time. *What to do* declares the decision and rings the places worth looking at first. *+ a claim* places one by a click, with the archive's own terms. |
| *Holdings* | Each file with its digest then and now, its chain of custody handover by handover, and its authenticity; a file dropped is taken in, a holding handed over; the sources with their channels. |

## 3. Tools

| Tool | What it answers |
|---|---|
| *Claims weighed and decided* | The archive appraised: the claims (standing, posterior, decibans, act, whether to look first) and the items as tables placed on the map, for a board or the Workbench |
| *A kind of report's channel, counted* | Against a reference: the hit and false-alarm rates with Wilson's and the exact intervals, and the ratio of a report and of a silence |
| *How the conclusions fared* | What the archive predicts of later looks against what they found, scored (Brier, log loss) beside the prior's and the count that takes repeated reports as independent |

## 4. Templates

*Rats in the East Village and the Lower East Side, 2024* (`data/products/urban_evidence.py`, case `nyc-rodents`):

- **The records.** The health department's initial inspections and 311's complaints of rats seen, April to June 2024, at their places and times. 399 claims that rats are active within 40 m of a point.
- **Counted.** The resident's channel, from the quarter before, lot by lot.
- **Kept apart.** The quarter after's findings, for *How the conclusions fared*.
- **Declared.** The inspector's channel, the acts' worths and the budget are the example's assumptions, and its text says so. None of it is advice to anyone.

## 5. Archive

An archive is `evidence/archive.json` in the project (`noetarium.evidence/1`, as `api/cpp/science/spacetime_evidence.hpp` documents it), with the files under `evidence/holdings/`. The server side (`routes.py`) keeps it, takes files in, writes handovers, lays out examples, takes a table's rows in as items, claims or sources, and serves the place from the data library's world tiles.

## 6. Dependencies

archeion, tekmerion, tyche and calx, with geodesy's geodesics, through the API's `science/spacetime_evidence.hpp`, built by the work's recipe into `data/cache/engines/urban-studio/evidence/`; the visualization layer's `evidence/located.js`; the data library's `world_city.within` for the place.
