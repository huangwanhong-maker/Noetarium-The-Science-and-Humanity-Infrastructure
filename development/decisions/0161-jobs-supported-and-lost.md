# 0161. Jobs Supported and Jobs Lost

- **Status:** Accepted, provisionally
- **Date:** 2026-11-22
- **Decided by:** the user's request of ADR 0144, which left the priorities to Claude, and their word to continue with economics first. Value added and output are what the accounts and the recovery paths gave; the people employed behind them, which a council and a public ask about first, were missing.

## 1. Context

ADR 0157 to ADR 0160 read visitors' spending and shocks through a country's, a region's or two regions' input–output tables. The data library's regional employment (ADR 0159) holds each region's and its country's employment in ten industry groups, and the tables hold each product's output, so persons employed per unit of output follow.

## 2. Decision

1. **The economics domain's `tourism`** takes each product's persons employed a year per unit of output and gives the jobs supported directly (by the products visitors buy) and in all (along the chains): the coefficients times the domestic demand and the output induced (1 check).
2. **The API** reads `jobs` with a table: `tourism.accounts` reports the jobs supported (by region on a two-region table), `regional.table` and `regional.two` carry the country's coefficients to the regions, and `recovery.path` reports the job-years lost, the coefficients times the output lost (1 check).
3. **`science.visitors.with_jobs`** sets each product's coefficient as its group's national employment over the group's national output, from the data library's regional employment, which now records each product's group.
4. **The visitors tools** report the jobs supported and the job-years lost wherever the employment is held.

## 3. Alternatives Considered

- **Employment by product.** Eurostat's employment is published by group; a group's products share a coefficient, which overstates jobs in capital-intensive products and understates them in labour-intensive ones within a group, and is said to be.
- **Full-time equivalents.** Persons employed are what the regional series counts; hours would need the national accounts' employment in hours, by group.
- **Regional productivity.** A region's jobs per unit of output are taken as the country's, as its output is (ADR 0159).

## 4. Consequences

- **What Prague shows** (Czechia's employment and table of 2022): Prague's visitors' spending supports about 30,500 jobs in the products they buy and 46,300 in all, 38,900 of them in Prague on the two-region table; the 90-day halving of their spending costs about 5,700 job-years.
- **Not built:** employment by product; hours; jobs by occupation or wage.
- **Provisional:** the coefficients' grouping.
