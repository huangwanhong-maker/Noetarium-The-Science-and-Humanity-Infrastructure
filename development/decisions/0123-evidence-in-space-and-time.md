# 0123. Evidence in Space and Time: an Archive of Evidence in the Urban Studio, Weighed and Acted On

- **Status:** Accepted, provisionally (2026-11-16). The plug-in's name and colour are provisional.
- **Date:** 2026-11-16
- **Decided by:**
  - **The request.** "Please at the urban science (I think the studio one?), add plug-in that integrate our archieve system just finished. So that it can convinent for various evidences archieve in spacetime, and analysis them, decision making on top of them, etc." And: "If evidence and archieve foundation(s), or decision making foundation(s) still can be continue revision/implementation, please also continue."
  - **The design.** Claude, here, overnight, on archeion (ADR 0113, ADR 0115), tekmerion (ADR 0112) and tyche's decisions (ADR 0071, ADR 0114), in the Urban Studio (ADR 0102).

## 1. Context

The archive system (archeion: holdings by their digests, chains of custody, provenance) and the evidence system (tekmerion: the force of reports through their sources' channels) were built for law and for social data. A city's evidence differs in two ways.

- **It is located.** A complaint, an inspection or a sighting has a place, known within a radius, and a time, known within a span. Whether it is about a claim of a place and a time is itself uncertain.
- **It is repeated.** One household complains five times. A register copies an inspector's note. Counted as independent reports, such repetition overstates the evidence without bound.

Acting on it differs too. Many places each need a decision, and looking first (an inspection, a survey) costs money from one budget.

## 2. Decision

**The foundations first.**

- **tekmerion** (its decision 0007):
  - reports that share an origin, counted once, with what counting them apart would have given (`evidence/dependence.hpp`);
  - a module `located`: the share of where and when an item may be that falls in a claim's place and time, the dilution of the item's force by it, and groups of reports of one event or of events of their own.
- **tyche** (its decision 0026): the tests worth running before separate decisions within a budget (`decision/budget.hpp`, a knapsack of their EVSIs); Wilson's and Clopper and Pearson's intervals for a proportion.
- **calx** (its decision 0018): the 0/1 knapsack by branch and bound, with a certificate.

**The API composes them** in `science/spacetime_evidence.hpp`, with geodesy's distances. An archive (`noetarium.evidence/1`) holds sources, holdings with their custody, items, claims, a rule and a decision. Its operations:

- `keep`: each holding's custody and fixity, the items resting on one that is not sound, through archeion's provenance;
- `weigh`: each claim weighed by the items about its place and time, grouped by origin, its standing by the rule;
- `decide`: each claim's act, and the looks to buy within the budget;
- `timeline`: statements of time as a simple temporal network;
- `rates`: a kind of report's channel counted, with its intervals.

`science.spacetime_evidence` asks the engine, reads written times and tables, and observes holdings' digests from their files.

**A claim says what its items report.** Either an event of their own for each group (lots inspected, sightings: their force adds up), or one event (witnesses of one fire: they can only settle it). The first composition took every claim as one event. The example showed the fault: seventeen clear lots could not lower an area's probability below 0.43.

**The studio host lets a plug-in bring a server side and views** (`routes.py`, `views/*.js`), added to the host and the shared page without changing either for any other work. A tool is told the project it runs in.

**The plug-in, *Evidence in space and time*,** in the Urban Studio.

- *Evidence*: the place as the page. Claims are discs toned by the weight of their evidence, and items are glyphs by kind and by what they say. A claim or an item opens where it stands. Beneath is a strip of time. A mode, *What to do*, declares the decision and rings the places worth looking at first, and a claim is placed by a click.
- *Holdings*: each file with its digest then and now and its chain of custody, handover by handover. A file dropped is taken in; a handover is written. The sources' channels are listed.
- **Three tools** of the board's contract: the claims weighed and decided, as tables on the map; a channel counted; how the conclusions fared.
- **The place** is drawn from the data library's world tiles (`world_city.within`), for any archive.

**The visualization layer's `evidence`** gains `located.js`: the place, the claims and items, and the strip.

**The data library's `urban_evidence.py`** writes a real example. Two of New York City's public records (the health department's rodent inspections and 311's rodent complaints, 2024) become an archive of 1,566 items and 399 claims, with the resident's channel counted from the quarter before, the outcome of the quarter after kept apart, and the place's ways and footprints.

## 3. Alternatives Considered

- **A plug-in of the Archive Studio.** Its archive is of documents and linked data; a city's evidence wants the place as the page and the Urban Studio's map, tables and Workbench.
- **Evidence weighed in Python in the plug-in.** Rejected: what weighs evidence is tekmerion's, and the infrastructure's rule is that a general method goes into its foundation first.
- **Spatial smoothing of claims (kriging, a random field).** A model of the place, not of the evidence; it would hide which report moved which claim. Kept for later, beside this.
- **Counting every report as independent.** That is what the plug-in measures against and says.

## 4. Consequences

- **Checks:**
  - foundations: calx 976, tyche 1597, tekmerion 225 (88 new in all);
  - the API's spacetime evidence checks, with values by hand;
  - the visualization layer's `located` (21 of its 700);
  - the Urban Studio's suite, with the plug-in's.
  
  The page was driven in headless Chrome (17 steps, not in the suite), the 311 file altered on disk and found.
- **What the example shows** (real records; the inspector's channel, the prior's use, the acts' worths and the budget declared):
  - a complaint from a lot was 6.8 times as likely where inspectors found rats (12.2% of 213 lots) as where they passed it (1.8% of 1,217); the count is on lots inspectors chose, many because of complaints;
  - of 399 claims, 80 are established, 3 refuted, 189 open; 127 have nothing bearing on them;
  - one household's repeated complaints, counted as independent, would have overstated a claim's evidence 16 times;
  - 37 surveys within the budget are expected to improve what is done by 7.5 treatments' worth;
  - **held against the quarter after:** the archive's predictions of what later inspections found score better than the prior alone (Brier 0.234 against 0.249). They are well calibrated in the middle and overconfident at the ends, as lots of one area taken as independent should be.
- **Not built:**
  - a model of the place beneath the claims (neighbouring areas sharing their state);
  - chains of copying among sources;
  - a timeline view;
  - evidence read from documents by a model (the Law Studio's recognition) into items;
  - access rules applied to holdings.
- **Provisional:** the plug-in's name and its colour.
