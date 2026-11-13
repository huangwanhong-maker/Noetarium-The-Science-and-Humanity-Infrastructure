# sociology

*Status: draft (2026-11-13; [ADR 0104](../../development/decisions/0104-sociology-founded-time-and-relations.md)). Field, height 0, on the foundations alone; C++ from the start.* Groups, institutions, networks and the organisation of everyday life. Founded with code at the user's decision, after a research map that puts people's lives in the future, and with them time, care and relational life, at the centre of inquiry. Its first two modules are the day as a composition of activities and the ties among people. Its theory note is [time-and-relations.md](theory/time-and-relations.md).

## 1. Purpose

- **A day is a whole.** It holds 1440 minutes, so more of one activity is less of the others. Days are compared by their ratios in Aitchison's geometry of the simplex, not minute by minute, and an activity's association with an outcome is read only as time moved from another activity.
- **Time is unequally carried.** Who gives the household's and the family's care, who has free time and who has none, are questions about how groups' days differ. The domain measures the difference and names the parts that make it.
- **Lives are lived among others.** Friendship, kinship, help and neighbouring are ties. The domain reads how closed a circle is, whether groups keep their ties among themselves, who bridges between circles and who has few to turn to.
- **Every presumption is declared:** a classification of activities into parts, the diary's resolution at which a zero is replaced, the null against which a network is tested.

## 2. Scope

| Module | Scope | Status |
|---|---|---|
| `time` | A declared classification of activity codes into the parts of a day, with what it leaves; a day closed to 1440 minutes, zeros replaced at the diary's slot; a group's day (its geometric centre, its mean, its spread); two days contrasted in minutes, ratios and Aitchison's distance; who carries a part, by group against its share of people; the balance of two kinds of time; places compared by their days, with principal coordinates; an outcome on the day's log-ratios and the isotemporal substitution table | draft (30 checks) |
| `relations` | The whole network (density, closure, reciprocity, triads, components, isolates); belonging by group (the share of ties within, Coleman's inbreeding homophily, the E–I index against the complete network's, assortativity, a QAP test); each person's ties (contacts, clustering, Burt's constraint and effective size, k-core, reach within two ties, the diversity of contacts' groups); closure against networks of the same degrees | draft (25 checks; held to networkx on Zachary's karate club) |
| `institutions` | Rules, roles and organisations as declared structures | planned |
| `stratification` | Class, status and mobility: tables of origins and destinations, log-linear models | planned |
| `culture` | Meanings and practices; with urban science's planned culture | planned |

## 3. Dependencies

| Source | What sociology uses from it |
|---|---|
| tyche | `compositions` (the simplex, balances, the centre and variation, zeros, compositions in linear models); `networks` (censuses, clustering, components and cores, assortativity, homophily, brokerage, QAP, conditional uniform graphs); `multivariate` (principal coordinates); `classes` (Hill numbers) |
| calx | `linalg`, `random`, verification |
| morphe, physis, nomos | declared; not yet used |

## 4. Used By

- **social-provision**, the bridge over it, takes the day as a composition for the time given to care and household work.
- **The API's `science/society.hpp`** composes it, with social provision, demography's `health` and economics' `labour`, for any work's engine: `day`, `reallocation` and `relations`.
- **The Urban Studio's plug-in *Time, care and provision*** is its first work. Its tools *How a day is divided*, *Time moved between activities* and *Ties among people* run on any table of days or ties. The template *Time and care in Europe* sets out the days of women and men in eighteen countries from Eurostat's harmonised time-use surveys.

## 5. Open Questions

- **Days as sequences.** A diary is an order of activities, not only their totals: sequence analysis (optimal matching) and the rhythm of a week belong to `time` when diaries are read.
- **Essential zeros.** A person who does no paid work holds an essential zero, not a rounded one. The domain replaces every zero at the diary's resolution and says so; a model for essential zeros (a mixture, or subcompositions by group) is open.
- **Networks in time.** Ties form and dissolve. Their dynamics (stochastic actor-oriented models) are a composition of systems for the infrastructure's dynamics foundation, kinesis.
- **Open relational data.** No open table of ties is in the data library yet; the tools run on a person's own.

## 6. References

- Aitchison, J. (1986), *The Statistical Analysis of Compositional Data*, Chapman and Hall.
- Burt, R. S. (1992), *Structural Holes*, Harvard University Press.
- Dumuid, D. et al. (2019), The compositional isotemporal substitution model, *Statistical Methods in Medical Research* 28, 846–857.
- Gershuny, J. (2000), *Changing Times: Work and Leisure in Postindustrial Society*, Oxford University Press.
- McPherson, M., Smith-Lovin, L. and Cook, J. M. (2001), Birds of a feather: homophily in social networks, *Annual Review of Sociology* 27, 415–444.
- Wasserman, S. and Faust, K. (1994), *Social Network Analysis*, Cambridge University Press.
- The theory note's §6 lists the rest.
