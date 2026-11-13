# social-provision

*Status: draft (2026-11-13; [ADR 0105](../../development/decisions/0105-social-provision.md)). Bridge, height 2, over economics, markets, demography and sociology; C++ from the start.* How what sustains life reaches people: through households, care, public services and commons, outside markets as well as through them. Founded at the user's decision, after a research map that puts people's lives in the future, rather than the design of cities, at the centre of inquiry. Its theory note is [time-care-and-provision.md](theory/time-care-and-provision.md).

## 1. Purpose

- **Provision is broader than exchange.** Many things that sustain life circulate through households, care relationships, public services, commons and voluntary associations. The markets domain holds exchange; this domain holds the rest, and how the two meet.
- **Care is a condition of life, not a sector.** Raising children, supporting older and dependent people, maintaining households: an abundance of production does not guarantee it. The domain measures the need for care, who gives it and how the need grows as populations age.
- **Households pool money, time and care.** Their incomes are compared by declared equivalence scales. Their unpaid work is valued at declared wages, and the time left to their members is weighed against a declared line.
- **Every presumption is declared:** the hours of care a person needs, the wage of unpaid work, the line of time poverty, the scenario of morbidity. What is counted, estimated and presumed is said apart.

## 2. Scope

| Module | Scope | Status |
|---|---|---|
| `households` | Equivalence scales as declared schemes (OECD-modified, original, square root, any power); equivalised income; the value of unpaid work by generalist and specialist replacement cost and by opportunity cost (UNECE 2017); full income; time poverty against a declared line, with its rate and gap | draft (18 checks) |
| `care` | People needing care from a population by age and a prevalence, with hours a week; scenarios of compression and expansion; supply, informal (from time use) and formal; the gap and the share met; the care diamond; support ratios; the change of need split exactly among size, age structure and prevalence by the Shapley value (nomos) | draft (16 checks) |
| `services` | Public services as provision: who is reached, at what cost, with what left unmet (composing urban science's access through the API) | planned |
| `commons` | Common-pool resources and their governance: games of provision and appropriation (nomos), Ostrom's design principles as declared rules | planned |

## 3. Dependencies

| Source | What social provision uses from it |
|---|---|
| economics | `labour` (flows of a labour market), and later production and transfers for satellite accounts |
| markets | The boundary with exchange; buyers and offers where provision is bought |
| demography | Populations by age, their projection; `health`, years lived in health and in dependency |
| sociology | `time`, the day as a composition, for the time given to care and household work |
| nomos | `games`, the Shapley value for exact decompositions |
| calx | `linalg`, verification |

## 4. Used By

- The API's `science/society.hpp` composes it with sociology, demography's `health` and economics' `labour` for any work's engine.
- `science.society` asks that engine from Python.
- The Urban Studio's plug-in *Time, care and provision* is its first work. Its template is *Time and care in Europe*, on Eurostat's time-use surveys, healthy life years, EU-SILC, population projections and labour market transitions.

## 5. Open Questions

- **Care as a dynamic system.** Dependency entered and left by age, and care given and received over a life course, is a composition of systems for the infrastructure's dynamics foundation (kinesis). This domain states the parts, and kinesis would run them together.
- **Non-market provision as accounts:** a satellite account of household production.
- **Which hours of care a level of need takes:** a declared scheme until surveys of care needs are read.
- **The care diamond's shares from data:** time use gives the household's hours; formal care's must come from labour statistics.

## 6. References

- Becker, G. S. (1965), A theory of the allocation of time, *Economic Journal* 75, 493–517.
- OECD (2013), *OECD Framework for Statistics on the Distribution of Household Income, Consumption and Wealth*, OECD Publishing.
- Razavi, S. (2007), *The Political and Social Economy of Care in a Development Context*, UNRISD Programme Paper 3.
- UNECE (2017), *Guide on Valuing Unpaid Household Service Work*, United Nations.
- The theory note's §7 lists the rest.
