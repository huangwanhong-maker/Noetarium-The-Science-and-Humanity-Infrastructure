# Time, Care and Provision

*The Urban Studio's second plug-in: people's lives beyond production and consumption, on the sociology and social-provision domains.* Status: **draft** (2026-11-13; [ADR 0104](../../../../development/decisions/0104-sociology-founded-time-and-relations.md), [ADR 0105](../../../../development/decisions/0105-social-provision.md)).

## 1. Concepts

What sustains a life reaches people through households, care, public services and the ties they live among, not only through markets. The plug-in asks what a researcher of everyday life and a planner of care ask:

- how a day is divided, and how women's and men's days differ;
- who carries the unpaid work, and what it is worth;
- who has too little time;
- how many years are lived in health;
- how much care an ageing population will need, and what makes the need grow;
- what a labour market's flows hold;
- how people are tied to one another.

The mathematics is the sociology and social-provision domains', with demography's `health` and economics' `labour`, on tyche's compositions and relational statistics, calx's non-negative matrices and nomos's Shapley value. It is asked through the API's engine (`science/society.hpp`). The theory notes are [`domains/sociology/theory/time-and-relations.md`](../../../../domains/sociology/theory/time-and-relations.md) and [`domains/social-provision/theory/time-care-and-provision.md`](../../../../domains/social-provision/theory/time-care-and-provision.md). What is counted, estimated and presumed is said apart in every answer.

## 2. Tools

| Tool | Asks of | Answers |
|---|---|---|
| How a day is divided (`the_day.py`) | days by place, sex and age | each day as a composition of 1440 minutes; places and sexes set out by how their days differ (principal coordinates of Aitchison's distances); the parts that set them apart; the balance of unpaid against paid work; how far apart women's and men's days are, place by place |
| Unpaid work, and who carries it (`unpaid_work.py`) | days by place and sex | women's share of the household's and family's care; paid and unpaid work together; its value at a declared wage (replacement cost) |
| Who has too little time (`time_poverty.py`) | people's committed minutes | the rate and gap of time poverty against a declared line or a multiple of the median, by group |
| Time moved between activities (`reallocation.py`) | days with an outcome | the outcome on the day's log-ratios, and the isotemporal substitution table with an interval for each move |
| Years lived in health (`healthy_life.py`) | life tables, prevalence by age band | Sullivan's health expectancy at every age, held against published healthy life years where given; the assumption below the first band declared |
| Care ahead (`care_ahead.py`) | population now and projected, prevalence | the people needing care and their hours, now and later; support ratios; the change split exactly among size, age structure and prevalence |
| A labour market's flows (`labour_flows.py`) | flows between employment, unemployment and inactivity | each period's steady state, spells and half-life; the change of the steady unemployment rate split among the six flows |
| Ties among people (`relations.py`) | a table of ties, optionally people with groups | closure against the same degrees; belonging by group with a QAP test; each person's constraint, effective size, reach and diversity of contacts |

## 3. Template

*Time and care in Europe*, on seven Eurostat datasets (CC BY 4.0), runs from the template's plan in about forty seconds. It holds:

- the days of women and men in eighteen countries (the harmonised time-use surveys, 2010);
- unpaid work and its value;
- years lived in health in 2023, held against Eurostat's own;
- the care Europe will need by 2050;
- France's labour market through 2020's lockdown.

The relational and reallocation tools have no open data in the template: they run on a person's own tables.

## 4. Engine

`engine/main.cpp` is a program around `science::society::answer`. It is built by the Urban Studio's `engine.py` into `data/cache/engines/urban-studio/provision/engine`, and asked one request a process.
