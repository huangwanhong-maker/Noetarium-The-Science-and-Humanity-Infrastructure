# Shelter: People in Slum or Informal Households and in Inadequate Housing

*Status: draft (ADR 0155). The module `urban::shelter`; its checks are `shelter/tests/check_shelter.cpp`.*

## 1. The Concept

SDG 11.1.1 is the proportion of urban population living in slums, informal settlements or inadequate housing. UN-Habitat's metadata (last updated 2021-12-20) gives it two components, each a share of people:

- **Slum or informal households.** After the expert group of 2002, a household is a slum household when its members lack one or more of five things:
  - an improved water source;
  - improved sanitation;
  - sufficient living area: not more than three people to a habitable room;
  - a durable dwelling: permanent material for walls, roof and floor, and a site that is not hazardous;
  - security of tenure: a document that proves it, and protection from forced eviction, de facto or perceived.

  Informal settlements are taken into the same component, since their criteria are already among the five.
- **Inadequate housing.** A household whose net monthly expenditure on housing exceeds 30% of its monthly income.

The data are household surveys and censuses (DHS, MICS, LSMS and others) with their weights, strata and clusters.

**Open terms.** Each component has readings, and the module computes the shares under every declared one:

- which water sources and toilets are improved: the expert group's lists, or the JMP's ladder, which counts delivered water as improved and a toilet shared with other households as limited;
- how many people to a room is too many: the expert group considered a range of two to four, and the MDGs fixed three;
- which parts of a dwelling must be durable, and whether a hazardous site counts;
- what makes tenure secure (a document, protection from eviction, both or either), and whether tenure is counted at all: many surveys did not ask it, and four components were reported;
- the share of income beyond which housing is unaffordable.

None of the shares is the indicator's value until a reading is chosen (tekmerion's `construal`; transit's and open space's readings of SDG 11.2.1 and 11.7.1).

## 2. Status under a Reading

For a household $h$ and a reading $\rho$, each component $c$ has a status $s_c(h) \in \{\text{no}, \text{yes}, \text{unknown}, \text{uncounted}\}$, "yes" meaning deprived. A missing answer makes a status unknown unless the other answers already decide it: walls of mud make a dwelling not durable whatever its roof.

**Definition (the union rule).** The household is a slum household, $S(h) = \text{yes}$, when some counted component is deprived; $S(h) = \text{unknown}$ when none is but some is unknown; otherwise no.

**Proposition 1 (monotone in the reading).** Call $\rho'$ wider than $\rho$ when, for every household and component, $s_c(h) = \text{yes}$ under $\rho$ implies yes under $\rho'$, and $s_c(h) \in \{\text{yes}, \text{unknown}\}$ under $\rho$ implies the same under $\rho'$. Then the sets $\{S = \text{yes}\}$ and $\{S \in \{\text{yes}, \text{unknown}\}\}$ grow, and both bounds of §3 do not fall.

*Proof.* The union of components is monotone in each. ∎

Fewer improved codes, a shared toilet counted against, fewer people to a room, more parts that must be durable, a hazardous site counted, and tenure counted (or a stricter rule of tenure) are each wider. The JMP's ladder is wider for toilets and narrower for water, so it is ordered with no other reading.

## 3. Shares, Bounds and Intervals

Let $w_h$ be the survey weight and $m_h$ the household's members. The share of people in slum households is the ratio
$$\hat R = \frac{\sum_h w_h m_h \mathbf 1[S(h) = \text{yes}]}{\sum_h w_h m_h}.$$

**Missing answers.** Counting every unknown status as not deprived and as deprived gives the bounds $\hat R_- \le \hat R_+$, which hold whatever the missing answers are (partial identification, after Manski). The reading says which value to report: either bound, or the households whose status is unknown left out of both sums (complete cases, which assumes them missing at random).

**Intervals.** The value is peira's ratio estimator with the ultimate-cluster variance on the survey's strata and clusters, and a Student interval on clusters less strata degrees of freedom. A district's share is the same ratio with the survey's other households given zero in both sums, so that its variance is the domain's, not that of a subsample treated as a survey of its own.

**What else is reported.** Each component's share of people deprived; the people deprived of one component alone, which says which component alone puts people in the count; the people by number of deprivations, among households whose every counted status is known; inadequate housing and the union of both components.

## 4. What Is Not Here

- the delineation of the urban population (the Degree of Urbanisation); the survey is taken as urban;
- the other criteria of adequate housing (accessibility, cultural adequacy), and price-to-income and rent-to-income ratios;
- homelessness, which the indicator does not capture;
- informal settlements by land and plan, apart from tenure;
- area-level indicators from imagery (built-up morphology), and small-area estimation for places the survey does not represent (peira's Fay–Herriot model is there to be used);
- imputation of missing answers, beyond bounds and complete cases.

## References

- UN-Habitat. SDG indicator metadata, 11.1.1, last updated 2021-12-20. https://unstats.un.org/sdgs/metadata/
- UN-Habitat (2003). *Slums of the World: The Face of Urban Poverty in the New Millennium.* Nairobi.
- WHO and UNICEF Joint Monitoring Programme (2017). *Progress on Drinking Water, Sanitation and Hygiene: 2017 Update and SDG Baselines.* Geneva.
- Manski, C. F. (2003). *Partial Identification of Probability Distributions.* Springer.
