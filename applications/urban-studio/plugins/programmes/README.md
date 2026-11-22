# Programmes and public finance

*Status: draft ([ADR 0144](../../../../development/decisions/0144-knowledge-programmes-and-scorecards.md), [ADR 0145](../../../../development/decisions/0145-the-finances-of-local-governments.md), [ADR 0146](../../../../development/decisions/0146-land-value-capture-and-readjustment.md)).*

What a city can do and afford, and what a programme of learning did for it.

## 1. What It Does

- **Scorecards scored as their scales allow.** A scorecard's statements are scored on anchored levels, each score with the evidence it rests on, and each part is aggregated by the scheme's declared rule: the weakest link, the median, a threshold, a profile of counts. A mean of levels is never taken unless the scheme declares numbers for them, and is then marked. The scores without evidence are named, the weaknesses come out as needs with their evidence, and two assessors' agreement is Krippendorff's ordinal α (kybernesis's `scorecards`).
- **A learning programme from needs to results.** A cohort's needs (the scorecards' weaknesses among them) and the activities that reached them; each action plan's state at a date and the pace of completion, open actions counted as open; the highest result each city reached on a declared ladder at a sufficient grade of evidence; each contribution claim no better shown than its weakest step and held back by any rival not addressed; formats compared by substantiated contribution for their cost (the policy domain's `learning`).
- **A city's finances.** Accounts through a declared chart, ratios judged against a declared scheme of thresholds, the years ahead with its loans and the coverage of its debt service, and the largest new loan it could service at a declared floor of coverage, bounded by a ceiling on its stock of debt (the economics domain's `fiscal`).
- **Local government debt in Europe.** A country's local government sector from Eurostat, its debt as a share of GDP, the balance that would hold it, and a fan of the years ahead from the record's own variation.
- **A project's financing plan.** The project's cost met by what the instruments take back, a reserve and new loans, on the city's own years ahead; whether its coverage of debt service holds, and how much more of a project it can afford because of what it captures (ADR 0152).
- **Land value capture and land readjustment.** A public project's uplift on the parcels it touches, as declared values with and without it, and what each declared instrument would take back (a betterment levy, development rights by a formula or at auction, tax increment with only the project's share counted as uplift, a developer obligation): the share of the cost it recovers and the share of the uplift it takes, kept apart and beside a statutory rate; what tax increment diverts from each jurisdiction; the parcels charged more than their uplift. A readjustment by its area identity, contribution ratio, whether the owners' land keeps its value, the reserve land's revenue and the project's balance, and the replotting by the least total distance with each owner's settlement (the economics domain's `capture`).

## 2. Tools

| Tool | What it answers |
|---|---|
| `scorecard.py` | A city's scorecard part by part, its profile, its weaknesses with their evidence, two assessors' agreement |
| `programme.py` | A programme at a date: needs reached and unmet, action plans, contribution claims, formats compared |
| `finance.py` | A city's ratios judged, its years ahead, the largest new loan at a declared floor, rate and tenor |
| `local_debt.py` | A European country's local government debt from Eurostat, the stabilising balance, a fan ahead |
| `capture.py` | What a project could take back from the land it raises, instrument by instrument; a land readjustment with its replotting |
| `project.py` | Whether the city can afford the project: its cost met by the instruments' receipts, a reserve and new loans on the city's own years ahead; the coverage with and without the receipts against the floor; the largest scale affordable each way (ADR 0152) |

## 3. Templates

- **An invented cohort** (`cohort`): three invented cities (Ardel, Brisa, Corvo), a resilience scorecard of eight statements on six levels (Ardel scored by two assessors), a programme of four activities that took their weaknesses as needs, its action plans, results, claims and asks, Ardel's accounts for three years with a loan, and a tram line through its eastern districts with eight parcels, four instruments and a readjustment of five owners; then France's local government sector from Eurostat. Every number of the cohort is invented to show the tools and says so (`examples/cohort.json`); Eurostat's data are real, reused with the source acknowledged.

## 4. Checks

In the Urban Studio's suite: the plug-in's parts; the scorecard part by part with its three weaknesses; the programme's six actions and two substantiated claims; ten years ahead of the city's finances; France's local debt with a fan; four instruments of capture each recovering part of the cost, and five owners replotted with settlements summing to zero. The API's `api/tests/check_programmes.py` holds the compositions to values worked by hand on the same cohort.
