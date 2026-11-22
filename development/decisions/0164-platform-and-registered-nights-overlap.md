# 0164. Platform and Registered Nights Overlap

- **Status:** Accepted, provisionally
- **Date:** 2026-11-23
- **Amends:** [ADR 0156](0156-the-visitor-stock.md), [ADR 0157](0157-tourism-accounts-and-levies.md), [ADR 0158](0158-recovery-and-inoperability.md), [ADR 0159](0159-a-regions-own-table.md), [ADR 0160](0160-two-regions.md), [ADR 0161](0161-jobs-supported-and-lost.md), [ADR 0162](0162-the-design-of-levies.md)
- **Decided by:** an error found after ADR 0156–0163 were pushed. Eurostat's statistics on short-stay accommodation booked through platforms (tour_ce_omn12) carry the caution that their nights cannot be added to the nights spent at tourist accommodation establishments (tour_occ_nin2m), since a rental booked through a platform may also be a registered establishment. The visitor stock and every economic tool built on it added them.

## 1. Context

The visitor stock's reading "with platform rentals" summed registered nights and platform nights, and the tourism accounts, the levy, the levy's design, the recovery, the regional tables and the jobs all took that sum as a place's paid nights. Where a member state requires holiday flats to be registered, a night booked through a platform may also be counted by the establishment survey. The size of that overlap is not published and is not known to Eurostat; it differs by country and by year.

Two facts bound it. The platforms' data cover short-stay accommodation, NACE 55.20 (holiday and other short-stay accommodation), and not hotels or camping. So a platform night that is also registered is a registered night in NACE I552, and the overlap in a month is at most min(platform nights, registered I552 nights). It is at least nothing.

## 2. Decision

1. **The domain's `stock`** (urban science's `visitors`, 3 checks more, 17 in all) takes, beside the registered and platform nights, the registered short-stay nights, and a reading declares the share of the largest overlap removed, `overlap` in [0, 1]: each month's platform nights lose overlap · min(platform, registered short-stay), spread over their origins in proportion. The default is 1, the overlap as large as it can be, so that no night is counted twice and the stock is a lower bound; 0 gives the upper bound, the sum Eurostat advises against.
2. **The registered short-stay nights** come from the year's nights by class (tour_occ_nin2: I551, I552, I553), each month's registered nights times the year's I552 share, since Eurostat publishes no monthly nights by class (tour_occ_nin2m has no I552 alone). A place whose classes are not published (the Metropolitan City of Venice, a NUTS 3 code) takes its region's share (Veneto). Both are declared stand-ins.
3. **The API** reads `overlap` in each reading and `registered_short` among the nights (1 check more). **`science.visitors`** gives `holiday_share`, `registered_short`, `paid_nights` (the year's lower and upper bounds) and `nights` (paid nights by origin under a declared overlap, asked of the engine), and its readings are five: registered only; with platform rentals at the lower bound; with platform rentals added whole (the upper bound); with unregistered stays, on the lower bound; with same-day visitors, on the lower bound.
4. **Every economic tool** of the visitors plug-in (what spending becomes, the region's table, two regions, a levy, the design of levies, when visitors stay away) takes paid nights at the lower bound by default, and each has a parameter `overlap` to read any share up to the upper bound. The design of levies gives platform rentals their nights less that share of the largest overlap.
5. **The records are amended, not rewritten.** ADR 0156–0162 keep their figures as they were computed, each status pointing here; this record gives the corrected figures. The Urban Studio's suite holds the five readings in their order (registered ≤ lower ≤ upper, lower ≤ unregistered ≤ same-day) and the stock's note on the overlap; the gallery's pictures and captions of §54 are retaken at the lower bound.

## 3. Alternatives Considered

- **The upper bound as default, the lower as a reading.** It is the sum Eurostat says cannot be made; a tool that reports value added, jobs and revenue should not count a night twice by default.
- **A declared share of platform nights taken as registered.** No source gives it for any city here; the bound by NACE I552 follows from what the platforms cover, and a declared share remains possible through `overlap`.
- **Platform nights left out of economic tools.** Where registration is incomplete they are paid nights that the establishment survey misses; leaving them out would make a lower bound lower than the facts allow.
- **Monthly I552 nights from another table.** Eurostat's monthly table has no class below I551–I553; the year's share is the stand-in, and the seasonality of holiday lodging is taken to be that of all registered nights.

## 4. Consequences

- **What changes** (2024, the template's declarations unchanged; the earlier figures are the upper bounds):
  - the largest overlap is the platform nights' 41% in Vienna, 23% in Prague, 16% in Brussels and 33% in Grande Lisboa; in Berlin and in the Metropolitan City of Venice every platform night could be a registered short-stay night, and the lower bound is the registered nights alone;
  - the Metropolitan City of Venice: 127 visitors per thousand residents on an average day and 328 in August (Gini 0.444), against 146 and 369 added whole; the Brussels-Capital Region still the flattest (0.055); Grande Lisboa's platform rentals 28% of paid nights at the lower bound, 37% at the upper;
  - Prague's paid nights: 22.6 million at the lower bound, 24.0 million at the upper. Its visitors spend 2,798 million euro (2,966 at the upper bound), which become 1,003 million of value added directly and 1,725 million along the chains (1,063 and 1,829), 24.6% leaking abroad either way; 1,413 million of the value added arises in Prague (1,498);
  - through Prague's own table: 0.59 of value added per euro and 22.1% leaking to the rest of the country, as before, being ratios;
  - jobs supported: about 43,700 in all and 36,700 in Prague (46,300 and 38,900);
  - a levy of 2 euro a night raises 44.6 million (47.2), 77% borne by visitors as before; by class, the flat levy raises 44.8 million and a rate of 3% raises 54.3 million (47.4 and 56.6), 8.0% of a camping night and 2.2% of a hotel night as before; holding nights at 90% takes 19.83 euro a night and creates a rent of 404 million (19.54 and 422);
  - half the visitors' spending lost for 90 days costs Czechia 513 million euro of output and 213 million of value added (544 and 225), 291 million of it what each sector would lose alone (309), about 5,400 job-years (5,700); through Prague's own table 399 million (423); through two regions 406 million in Prague and 108 million elsewhere (430 and 114); with 30% of lodging capacity closed at first, 700 million (731).
- **Checked:** with `overlap` at 0 the tools give the earlier figures exactly (2,966, 1,063, 1,829, 1,498 million euro and 46,348 jobs), so the correction changes nothing else.
- **Unaffected:** the short-term rentals (ADR 0163) compare an estimate from one platform's listings with the platforms' own nights rented out, and add nothing to registered nights.
- **Not built:** a country's registration rules as data, which would say where the overlap is likely large; platform nights by class of listing; monthly nights by class from national sources.
