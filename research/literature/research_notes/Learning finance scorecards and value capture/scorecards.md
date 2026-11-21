# Scorecards for Cities and Governments, and the Measurement Theory of Aggregating Ordinal Scores

Verification markers: **[V]** verified against the primary source, read in full text for these notes; **[S]** stated by a secondary source, a search snippet or an abstract; **[M]** from memory or inference, not verified. **[V, computed]** marks a result I computed from a table transcribed under [V].

Primary texts read in full: the PEFA 2016 Framework (full 2016 text with links, retrieved from the Wayback Machine because pefa.org returns HTTP 403 to automated clients), the PEFA 2016 brochure, the UNDRR (then UNISDR) Disaster Resilience Scorecard for Cities, preliminary and detailed levels (May 2017), the Rockefeller/Arup City Resilience Index booklet, Burns, Deininger, Selod and Dalrymple (2010) on implementing the LGAF, and the OECD/JRC *Handbook on Constructing Composite Indicators* (2008).

## 1. How PEFA 2016 Aggregates Dimension Scores into an Indicator Score (M1, M2, "+")

### Takeaway
PEFA 2016 scores each of its 94 dimensions on an ordinal A–D scale. A dimension earns a level only when every requirement of that level is met. Each multidimensional indicator is aggregated within itself by a declared method:
- **M1, weakest link:** the minimum, with "+" added if any other dimension scores higher.
- **M2, averaging:** a fixed lookup table over the multiset of dimension scores.

Nothing is aggregated across indicators. I found that the M2 table is exactly a step function of the arithmetic mean of the codes D=1, C=2, B=3, A=4, with rounding that is not uniform. It therefore assumes the levels are equally spaced, while M1 needs only their order.

### Cited Findings
**Structure**
- PEFA 2016 has 31 performance indicators in 94 dimensions, grouped into seven pillars [V] ([PEFA 2016 brochure](https://www.pefa.org/sites/pefa/files/resources/downloads/PEFA_2016_Framework_Final_WEB_0.pdf)):
  1. budget reliability;
  2. transparency of public finances;
  3. management of assets and liabilities;
  4. policy-based fiscal strategy and budgeting;
  5. predictability and control in budget execution;
  6. accounting and reporting;
  7. external scrutiny and audit.
- Scale: "Each dimension is scored separately on a four-point ordinal scale: A, B, C, or D, according to precise criteria established for each dimension." [V] ([PEFA Framework 2016, Part 2 §2.1](https://web.archive.org/web/20210423214147/https://www.pefa.org/sites/pefa/files/resources/downloads/PEFA%202016_latest%20version_with%20links%20%282%29.pdf))

**Level semantics: all requirements must be met**
- "In order to justify a particular score for a dimension, every aspect specified in the scoring requirements must be fulfilled. If the requirements are only partly met, the criteria are not satisfied and a lower score should be given that coincides with achievement of all requirements for the lower performance rating." [V] (same, §2.1)
- "A score of C reflects the basic level of performance for each indicator and dimension, consistent with good international practices. A score of D means that the feature being measured is present at less than the basic level of performance or is absent altogether, or that there is insufficient information to score the dimension." [V] (same)
- A is "warranted if evidence clearly demonstrates that an internationally-recognized level of good performance is achieved." [V] ([brochure](https://www.pefa.org/sites/pefa/files/resources/downloads/PEFA_2016_Framework_Final_WEB_0.pdf))

**Special values**
- **D\*:** "A score of D due to insufficient information is distinguished from D scores for low-level performance by the use of an asterisk--that is, D\*. The aggregation of multidimensional indicators containing D\* scores is no different from aggregation with other D scores. Aggregate indicator scores will not include an asterisk, and thus the insufficiency of information is only noted at the dimension level." [V] (§2.1.1)
- **NA:** "In cases where one or more dimensions of a multidimensional indicator are not applicable, the assessor proceeds as if the 'not applicable' dimensions did not exist." A D on one dimension can force NA on others; the example is PI-26.1, no internal audit function, making PI-26.2–26.4 NA. [V] (§2.1.2)
- **NU:** "it may be decided for certain reasons that a particular indicator will not be used … 'NU' is entered instead of a score." [V] (§2.1.2)
- "The use of NA and NU must be justified in the PEFA report. Assessments that score less than two-thirds (21) of the PEFA indicators should be referred to as 'partial PEFA assessments'." [V] (§2.1.2)

**Standard quantifiers.** PEFA fixes these words throughout [V] (§2.1.4):

| Term | Share by value |
|---|---|
| All | 90 percent or more |
| Most | 75 percent or more |
| Majority | 50 percent or more |
| Some | 25 percent or more |
| A Few | less than 25 percent and more than 10 percent |

**M1, weakest link** [V] (§2.2):
- Use: "used for multidimensional indicators where poor performance on one dimension is likely to undermine the impact of good performance on other dimensions of the same indicator."
- Steps:
  1. "Each dimension is initially assessed separately and given a score on the four-point calibration scale."
  2. "The aggregate score for the indicator is the lowest score given for any dimension."
  3. "Where any of the other dimensions score higher, a '+' is added to the indicator score. Note: It is NOT acceptable to choose the score for one of the higher-scoring dimensions and add a '-' for any lower scoring dimensions."

**M2, averaging** [V] (§2.2):
- Use: "The aggregate indicator score awarded using this method is based on an approximate average of the scores for the individual dimensions of an indicator, as specified in a conversion table (table 1). Use of this method is prescribed for selected multidimensional indicators where a low score on one dimension of the indicator does not necessarily undermine the impact of a high score on another dimension of the same indicator."
- Steps: find the section for two, three or four dimensions; "Identify the row in the table that matches the scores for each dimension of the indicator; the ordering of the dimension scores does not matter"; "Enter the corresponding overall score for the indicator."
- Limits: "The conversion table applies to indicators using M2 (AV) scoring methodology only. Using it for indicators designated for M1 (WL) will result in an incorrect score. The conversion table is intended for use on individual indicators only and is not suitable for aggregating scores across the full set, or subsets, of indicators. No standard methodology has been developed for aggregation across indicators because each indicator measures a different subject and has no standard, quantitative relationship with other indicators." [V]

**Table 1, "Conversion table for indicator scores using the averaging method M2 (AV)", transcribed exactly** [V] (PEFA Framework 2016, p. 10). Read each row as an unordered multiset of dimension scores mapped to the overall score.

- **Two dimensions:** DD→D; DC→D+; DB→C; DA→C+; CC→C; CB→C+; CA→B; BB→B; BA→B+; AA→A.
- **Three dimensions:**
  - DDD→D; DDC→D+; DDB→D+; DDA→C;
  - DCC→D+; DCB→C; DCA→C+; DBB→C+; DBA→B; DAA→B;
  - CCC→C; CCB→C+; CCA→B; CBB→B; CBA→B; CAA→B+;
  - BBB→B; BBA→B+; BAA→A; AAA→A.
- **Four dimensions:**
  - DDDD→D; DDDC→D; DDDB→D+; DDDA→D+; DDCC→D+; DDCB→D+; DDCA→C; DDBB→C; DDBA→C+; DDAA→C+;
  - DCCC→D+; DCCB→C; DCCA→C+; DCBB→C+; DCBA→C+; DCAA→B; DBBB→C+; DBBA→B; DBAA→B; DAAA→B+;
  - CCCC→C; CCCB→C+; CCCA→C+; CCBB→C+; CCBA→B; CCAA→B; CBBB→B; CBBA→B; CBAA→B+; CAAA→B+;
  - BBBB→B; BBBA→B+; BBAA→B+; BAAA→A; AAAA→A.
- The table's notes: "Dimension scores can be counted in any order. It is only the quantities of each score that are important for aggregation." and "Table 1 MUST NOT be applied to indicators using the M1 (WL) scoring method." [V]
- Row counts: 10, 20 and 35 rows, which are exactly the multisets of size 2, 3 and 4 over {A, B, C, D} [V, computed].

**The "+" rule:**
- Under M1, "+" is a flag that some dimension lies above the minimum [V].
- Under M2, "+" appears as the half-steps D+, C+ and B+ of the table. There is no "A+" and no "−" [V].

**Method of each indicator** [V] (each indicator's specification in Part 2, extracted from the text):

| Method | Indicators |
|---|---|
| Single dimension | PI-1, 4, 5, 9 |
| M1 (WL) | PI-2, 18, 20, 22, 23, 26, 28, 29, 30 |
| M2 (AV) | PI-3, 6, 7, 8, 10, 11, 12, 13, 14, 15, 16, 17, 19, 21, 24, 25, 27, 31 |

That is 4 + 9 + 18 = 31 indicators.

**Numbers used for analysis.** A PEFA note converts letter scores to numbers for analysis: A 4, B+ 3.5, B 3, C+ 2.5, C 2, D+ 1.5, D 1 [S] ([Transfer of PEFA scores into numerical values](https://www.PEFA.org/sites/pefa/files/Transfer%20of%20PEFA%20Scores%20into%20numerical%20values.pdf), seen as a search snippet, not read).

**What I found by computing on Table 1** [V, computed]:
- **M2 is a function of the number of dimensions and the sum of the codes D=1, C=2, B=3, A=4, and of nothing else.** Every pair of multisets with the same n and the same mean receives the same score.
- **The step maps** from mean to score are:
  - n=2: the exact mean in half-steps (1.5→D+, 2.5→C+, 3.5→B+).
  - n=3: 1.33→D+, 1.67→D+, 2.33→C+, **2.67→B**, 3.33→B+, **3.67→A**.
  - n=4: **1.25→D**, 1.75→D+, **2.25→C+**, 2.75→B, 3.25→B+, 3.75→A.
- **The rounding is not uniform.** Below C, means round down to the half-step (1.67→D+, 1.25→D, 1.75→D+), except DDC at 1.33→D+. From C upward they round up (2.67→B, 3.67→A, 2.25→C+).
- **Two properties hold.** The table never lowers the result when one dimension improves (it is monotone), and it always lies between the minimum and maximum dimension score.

**Quality assurance and evidence in the report** [V] (Part 3):
- Box 1.1 records the assessment's oversight team, assessment manager and team, and the reviewers of the concept note and draft report, "in particular the PEFA Secretariat".
- For each dimension the report "indicates the factual evidence, including quantitative data, which has been used to substantiate the assessment", notes any "issues in relation to the timeliness or reliability of data", and states explicitly where information was insufficient.
- Performance change since a previous assessment is reported with "evidence for each dimension and indicator score compared with the previous score" and with comparability issues disclosed.

### Inferences
- **M1 is ordinally meaningful [M, by Roberts's definition, §3].** min(x₁,…,xₙ) commutes with every strictly increasing recoding f, since min f(xᵢ) = f(min xᵢ), and the "+" flag ("some xᵢ > min") is invariant as well. M1 therefore needs only the order of A > B > C > D.
- **M2 needs equal spacing [M, from the computation above].** It is a step function of the arithmetic mean of equally spaced codes. Statements such as "indicator X scores higher than indicator Y under M2" are not invariant under strictly increasing recodings.
  - Example: {D, A} → C+ beats {C, C} → C, since the mean is 2.5 against 2.0.
  - Recode D = −10 with C = 2, B = 3, A = 4. The order is preserved, yet {D, A} now averages −3 and falls below {C, C}.
  - PEFA limits the damage by confining M2 to dimensions it judges independent within one indicator, and by forbidding aggregation across indicators.
- **D\* is a value plus a flag.** It aggregates exactly like D, and the flag is dropped at indicator level. Software should keep the flag in the trace even though the rule discards it.
- **NA works as deletion before lookup.** An indicator with one NA dimension of four uses the three-dimension rows. A deletion rule should be declared per rule.
- **Each PEFA dimension is a cumulative (Guttman-type) scale.** Requirement sets are nested by level and all must hold, so "level = the highest level whose requirements are all met". That is itself a conjunctive, non-compensatory rule inside the dimension.

### Gaps
- I did not read the PEFA Field Guide or the PEFA Handbook Vol. II; both are on pefa.org, which refused automated access. Their worked examples of M1 and M2 are therefore unverified.
- I did not read the subnational PEFA guidance (it adds indicator HLG-1 [M]).
- The numeric "transfer" note was seen only as a snippet.

## 2. How the UNDRR Disaster Resilience Scorecard for Cities Defines and Summarises Its Scores, with the City Resilience Index, the LGAF and Local Indices

### Takeaway
The UNDRR Scorecard (2017 version) has two levels:
- **Preliminary:** 47 questions on the Ten Essentials, each scored 0–3 with anchored descriptors.
- **Detailed:** 117 criteria, each scored 0–5 with descriptors specific to the criterion, often cumulative counts ("at least 8 of the 10 listed groups").

Results are summarised by averaging or consensus at the level of each Essential and by a "final percentage score" that excludes criteria judged not relevant. Cities may apply their own weights across the Essentials. The Scorecard itself warns that scores are subjective and "not comparable across different cities".

I found no published critique aimed specifically at the averaging; this is a gap. The CRI scores 156 questions 1–5, qualitatively and quantitatively, and declines to produce a single overall score. The LGAF copies PEFA's A–D pre-coded statements, which panels of experts decide collectively.

### Cited Findings
**UNDRR Disaster Resilience Scorecard for Cities (May 2017)**
- **Purpose:** "This Scorecard provides a set of assessments that will allow local governments to monitor and review progress and challenges in the implementation of the Sendai Framework for Disaster Risk Reduction: 2015-2030, and assess their disaster resilience. It is structured around UNISDR's Ten Essentials for Making Cities Resilient." Developed with the support of USAID, the European Commission, IBM and AECOM [V] ([Preliminary level assessment](https://preparecenter.org/wp-content/sites/default/files/unisdr_disaster_resilience_scorecard_for_cities_preliminary.pdf)).
- **The Ten Essentials,** as named in Figure 2 [V] (same). The chapter headers spell some differently, for instance "Organize for Resilience":
  1. Organise for disaster resilience;
  2. Identify, understand and use current and future risk scenarios;
  3. Strengthen financial capability for resilience;
  4. Pursue resilient urban development and design;
  5. Safeguard natural buffers to enhance the protective functions offered by natural capital;
  6. Strengthen institutional capacity for resilience;
  7. Understand and strengthen societal capacity for resilience;
  8. Increase infrastructure resilience;
  9. Ensure effective disaster response;
  10. Expedite recovery and build back better.

  Grouping: "Essentials 1-3 cover governance and financial capacity; Essentials 4-8 cover the many dimensions of planning and disaster preparation; Essentials 9-10 cover the disaster response itself and post-event recovery." [V]
- **Two levels** [V] (same):
  - "Level 1: Preliminary level, responding to key Sendai Framework targets and indicators, and with some critical sub-questions. This approach is suggested for use in a 1 to 2 day city multi-stakeholder workshop. In total there are 47 questions / indicators, each with a 0 – 3 score."
  - "Level 2: Detailed assessment. This approach is a multi-stakeholder exercise that may take 1 – 4 months and can be a basis for a detailed city resilience action plan. The detailed assessment includes 117 indicator criteria, each with a score of 0 – 5."
- **Preliminary questions per Essential,** extracted from the reference codes P1.1 … P10.2 [V, with a caveat]: E1 3, E2 5, E3 4, E4 4, E5 3, E6 6, E7 4, E8 8, E9 7, E10 2. That makes 46, against the document's stated 47; one reference was probably lost in the PDF layout.
- **Example preliminary anchors** [V] (same), question P1.1, "Does the city master plan (or relevant strategy/plan) include and implement disaster risk reduction approaches in line with the Sendai Framework?":
  - "3 – Fully integrated DDR plan, full Sendai Framework compliance and coverage across all of the Ten Essentials."
  - "2 – Stand-alone DDR plan complying with Sendai Framework and addressing all of the Ten Essentials."
  - "1 – Plans offering partial compliance with Sendai Framework and covering some of the Ten Essentials."
  - "0 – No plans / compliance."

  P1.3, integration of resilience with other city functions: "3 – Explicit or semi-explicit decision point for resilience in decision-making process(es), applied to all policy and budget proposals in all relevant functional areas … 1 – Applied ad hoc or occasionally. 0 – Not applied."
- **Example detailed anchors** [V] ([Detailed level assessment](https://preparecenter.org/wp-content/sites/default/files/unisdr_disaster_resilience_scorecard_for_cities_detailed.pdf)). Every one of the 117 criteria carries its own six-level descriptor.
  - Criterion 1.1.1, "To what extent are risk factors considered within the City Vision / Strategic Plan?": "5 – The plan includes a range of actions / priorities … that directly respond to current and anticipated future risks. 4 – … respond to current identified risks. 3 – The plan context is framed around clear presentation of the city risk factors. 2 – A robust risk assessment methodology is integral to the city plan. 1 – There is evidence within the plan that risks (hazards x likelihood) is broadly understood … 0 – Risks are not considered in the plan."
  - Consultation in plan making, a cumulative count: "5 – Yes – All relevant groups have been invited and attended … 4 – At least 8 of the 10 listed groups (right) have been engaged / consulted. 3 – At least 6 of the 10 … 2 – At least 4 … 1 – At least 2 of the listed groups were invited."
  - Its level 0 reads, as printed, "0 – Stakeholder engagement has been undertaken." A "not" is evidently missing [V]. The source text contains errors, and an encoding must be checked against the Excel tool.
  - Other criteria use frequency bands. 6.2.1.1: "5 – Average 1 or more exposures per person per week … 0 – Average 1 exposure per person per year or worse" [V].
- **Summarising, averaging, weights and percentages** [V] ([Preliminary](https://preparecenter.org/wp-content/sites/default/files/unisdr_disaster_resilience_scorecard_for_cities_preliminary.pdf), the same text in the [Detailed](https://preparecenter.org/wp-content/sites/default/files/unisdr_disaster_resilience_scorecard_for_cities_detailed.pdf)):
  - As a high-level workshop survey, "Sometimes an average or consensus score is applied at the level of each of the 'Ten Essentials', rather than for each individual criteria / assessment".
  - "For the detailed assessment it is possible to opt out of completing some assessment criteria if they are not relevant to your city … Your final percentage score excludes any assessment criteria that you have deemed not to be relevant."
  - "local governments are free to apply their own weighting to scoring, across the 'Ten Essentials' and decide on their own 'evidence' to support the assessment."
  - "it is very unlikely that any city will score maximum points, and most will not score more than 50%."
  - "The scores are not normative and therefore not comparable across different cities. The Scorecard was not designed to facilitate competition between cities, but to identify and promote sharing of knowledge."
- **Subjectivity, justification and consultation** [V] (same):
  - "For the preliminary assessment, questions all need to be scored – the scoring is intentionally simple and crude. Treat the questions as prompts … the conversation is often as important as the score."
  - "individual scores may unavoidably be subjective – use your judgment … Recording your justification for each evaluation score will enable validation, as well as future revisions and tracking of progress".
  - "Consulting your citizen groups as you complete the Scorecard will improve the validity of your results".
  - An MS Excel tool "facilitates scoring at the two levels" and "allows simple recording of comments or suggested actions".
- **Evidence lists.** Each Essential lists the "Data you will need to answer this section", for Essential 1 "organization charts … memoranda of understanding … meeting minutes and actions". UNISDR "has provided some suggestions regarding the types of evidence that would generally satisfy the scoring requirements" [V] (same).
- **External evaluation.** Schofield and Twigg (2019) evaluated lessons from applications of the Scorecard and found it gave a practical framing of disaster risk reduction issues, while identifying a need to adapt it [S] (cited in [Bulletin of Earthquake Engineering 2020, s10518-020-00993-y](https://link.springer.com/article/10.1007/s10518-020-00993-y)). That paper customises the Scorecard for liquefaction [S].
- **Later versions.** UNDRR now runs the Scorecard online under MCR2030 and publishes addenda. The SME addendum is said to use yes/no questions [S] ([MCR2030 Scorecard page](https://mcr2030.undrr.org/quick/13215); search snippets).

**Arup / Rockefeller Foundation City Resilience Index (CRI)**
- **Structure:** "four dimensions, 12 goals and 52 indicators" [V] ([CRI booklet](https://mercociudades.org/wp-content/uploads/2020/10/CRI-total-Booklet.pdf)).
- **No single score:** "The Index will measure relative performance over time rather than comparison between cities. It will not deliver an overall single score for comparing performance between cities, neither will it provide a world ranking of the most resilient cities." [V] (same)
- **156 qualitative prompt questions,** an average of three per sub-indicator. Each "assesses the adequacy of the mechanism and processes in place to achieve the outcome articulated by the indicators", "Scored on a linear scale between 1 and 5, based upon consideration of a 'best case' and 'worst case' scenario relevant to a particular area of city performance". The booklet's example is insurance against high-risk hazards, with worst-case and best-case texts [V] (same).
- **156 quantitative prompt questions,** which identify "quantitative metrics that can be used by cities as proxies for past and current performance … Scored on relevant city data in a specific unit … A score from 1 to 5 is then automated, based on a standardised performance scale"; for example, the "percentage of buildings with insurance cover for high risk hazards" [V] (same).
- **Presentation:** answers "are aggregated and presented according to the 12 goals" [S] ([Arup CRI page](https://www.arup.com/projects/city-resilience-index), as summarised by search). How they are aggregated is not stated in the booklet I read.

**World Bank Land Governance Assessment Framework (LGAF)**
- **Original structure** [V] ([Burns, Deininger, Selod, Dalrymple 2010, FIG Congress](https://www.fig.net/resources/proceedings/fig_proceedings/fig2010/papers/ts03a/ts03a_burns_deininger_et_al_4640.pdf)):
  - "The methodology adopted for the LGAF is based on the methodology adopted in developing the Public Expenditure and Financial Accountability (PEFA …) assessment framework."
  - "five thematic areas … with a coherent set of 21 indicators"; "Within each indicator a set of dimensions totaling 80, between 2 and 6 per indicator, was developed … For each dimension, a set of pre-coded statements were developed, as per the PEFA methodology. These statements describe four situations graded from A through to D."
- **Generic anchors** (Table 1) [V] (same):
  - "A – Dimension description is the best option towards a good land governance scenario."
  - "B – … generally the second best set of options that make progress towards good land governance."
  - "C – … generally struggles to meet the criteria for good land governance however some attempts are being made."
  - "D – There are no attempts in this area that indicate good land governance operates."
- **Process** [V] (same). Expert investigators first gather data, and "A key output from the expert investigation is a typology of land tenures". Then "The core approach to implement the LGAF is using Panels of Experts. Invited experts meet to form a panel in order to investigate a specific set of dimensions and to provide a collective and motivated ranking for each one of them"; there were eight panels in 2010.
  - "Sampling may be undertaken … to confirm panel opinions with evidence or address a lack of data to verify rankings … at the Country Coordinator's discretion".
  - "Depending on the country context, a few dimensions may not be eligible for scoring."
- **Revised version:** 116 dimensions on the A–D scale, chosen by expert panels "among a list of pre-coded statements", with panels "typically … comprised of between three and eight members" and nine panels [S] ([World Bank LGAF page](https://www.worldbank.org/en/programs/land-governance-assessment-framework)). The 2013 Implementation Manual speaks of "consensus among experts in a participatory and deliberative process" [S]. A Georgia report used 21 indicators, 80 dimensions, 16 additional parameters and nine panels [S].

**Other local governance or capacity indices**
- None were read for these notes; see Gaps. The PEFA framework is also applied to subnational governments [M].

### Inferences
- **The UNDRR averages and percentages assume equal spacing [M].** The "average … at the level of each of the Ten Essentials" and the "final percentage score" both treat 0–5 (or 0–3) as equally spaced and compensatory across criteria. They are therefore not meaningful in Roberts's sense when the anchors are only ordered, which is how they are written (for example "framed around clear presentation" against "robust risk assessment methodology").
- **The percentage's denominator varies with relevance [M].** Excluding "not relevant" criteria means two cities' percentages rest on different criterion sets, compounding the Scorecard's own warning against comparing cities.
- **Some anchors are counts and some are bands [M].** Count anchors ("at least 8 of 10 groups") and frequency bands are ordinal codings of an underlying count or rate. Software could keep the count itself and derive the level by the declared thresholds, so that no information is lost.
- **The CRI is two scores per question [M].** A judged 1–5 score anchored at worst and best cases, and a 1–5 score derived automatically from a metric by a standardised scale (a declared threshold function). Declining a single overall score is the CRI's own non-compensatory choice at the top.
- **The LGAF reaches its scores by deliberated consensus [M].** It does not record independent ratings that could later be measured for agreement.

### Gaps
- I found no source critiquing the Scorecard's Essential-level averaging or percentage scores specifically. The full Schofield and Twigg (2019) evaluation was not reached.
- The Scorecard's Excel tool, the online platform, and the later versions (the 2020 addenda, MCR2030) were not read, so it is unverified whether the online tool computes averages or percentages by default.
- The CRI's aggregation from 156 questions to 52 indicators and 12 goals is not stated in the booklet.
- The LGAF 2013 Implementation Manual and the Deininger, Selod and Burns (2012) book were not read; the download returned HTML. The panel procedures (voting against consensus, dissent recorded) are unverified.
- No local governance or capacity index (for instance UN-Habitat's Urban Governance Index or municipal self-assessment tools) was read; one is needed to complete objective (a).

## 3. Which Aggregations of Ordinal Scores Are Meaningful in Roberts's Sense, and Which Non-Compensatory Rules Are Used in Practice

### Takeaway
A statement about scores is meaningful when its truth survives every admissible transformation of the scales. For ordinal scales the admissible transformations are all strictly increasing maps.

- **Meaningful on one common ordinal scale:** comparisons of medians, order statistics, minima and maxima, counts at or above each level, threshold, lexicographic and Pareto rules, and lookups defined on the levels themselves.
- **Not meaningful:** comparisons of arithmetic means and sums, and so of averages, percentages of the maximum, and PEFA's M2.
- **When each dimension has its own independent ordinal scale,** essentially only dictatorial (projection) rules survive.

The OECD/JRC Handbook states that weights in linear and geometric aggregation are trade-offs, not importance. It recommends non-compensatory multi-criteria rules (Condorcet–Kemeny–Young outranking) when weights are meant as importance.

### Cited Findings
- **The scale types.** The OECD/JRC Handbook (2008) reproduces the measurement-theoretic hierarchy [V] ([OECD/JRC Handbook, pp. 52–53](https://www.oecd.org/sdd/42495745.pdf)):
  - "A fundamental point in measurement theory is that of the uniqueness of scale, i.e. which admissable transformations of scale allow for the truth or falsity of the statement involving numerical scales to remain unchanged (problem of meaningfulness). In the case of an ordinal scale, it is unique up to a strictly monotone increasing transformation".
  - An interval scale is "unique up to a positive affine transformation".
  - For ordinal numbers, interpreting arithmetic "as a quantitative statement about the property … is not possible for ordinal-scale numbers but can be done for differences between interval-scale numbers"; "the fewer the admissible transformations of a scale, the more meaningful the statements involving that scale."
- **Stevens (1946)** introduced the nominal, ordinal, interval and ratio scales with their "permissible statistics", the median and percentiles for ordinal data [M] ([Stevens, Science 103:677–680](https://doi.org/10.1126/science.103.2684.677), not read).
- **Roberts (1979)** defines a statement as meaningful if its truth value is unchanged when every scale in it is replaced by an admissible transformation [M] (*Measurement Theory with Applications to Decisionmaking, Utility, and the Social Sciences*, Addison-Wesley, not read). The Handbook cites Roberts (1979) for the claim that weights with ordinal criterion scores mean importance coefficients [V] ([Handbook p. 105](https://www.oecd.org/sdd/42495745.pdf)).
- **Marcus-Roberts and Roberts (1987), "Meaningless Statistics"** [S] ([J. Educational Statistics 12(4):383–394](https://ideas.repec.org/a/sae/jedbes/v12y1987i4p383-394.html); secondary summaries):
  - Computing a mean on an ordinal scale is not itself forbidden, but conclusions drawn from it may not be meaningful.
  - The mean of Group I can exceed that of Group II on the original scale and fall below it after an admissible transformation.
  - Comparing medians is meaningful, since the order is preserved under every admissible transformation.
- **Ebert and Welsch (2004), "Meaningful environmental indices: a social choice approach"** [S] ([JEEM 47(2):270–283](https://ideas.repec.org/a/eee/jeeman/v47y2004i2p270-283.html), abstract):
  - A meaningful index is one whose underlying ordering is independent of admissible transformations of the variables.
  - Arithmetic-mean indices are mostly not meaningful.
  - For ratio-scale variables the meaningful index is a geometric mean.
  - The result for ordinal, non-comparable variables (only dictatorial rules) is from memory [M].
- **Aggregation theory beyond Ebert and Welsch** [M] (not read):
  - Aczél and Roberts (1989), "On the possible merging functions" (*Mathematical Social Sciences* 17), characterise the merging functions that are meaningful by scale type.
  - Orlov (1981), Ovchinnikov (1996, "Means on ordered sets", *Math. Soc. Sci.* 32) and Marichal (2002, "On order invariant synthesizing functions", *J. Math. Psych.* 46) show the following. The continuous, idempotent functions whose comparisons are invariant when one strictly increasing map is applied to all arguments are the lattice polynomials. The symmetric ones are exactly the order statistics, which include the median, min and max. Weighted medians and the Sugeno integral are their weighted analogues.
- **Compensability** [V] ([OECD/JRC Handbook, pp. 32–33](https://www.oecd.org/sdd/42495745.pdf)):
  - "In both linear and geometric aggregations, weights express trade-offs between indicators. A deficit in one dimension can thus be offset (compensated) by a surplus in another. This implies an inconsistency between how weights are conceived (usually measuring the importance of the associated variable) and the actual meaning when geometric or linear aggregations are used."
  - "To ensure that weights remain a measure of importance, other aggregation methods should be used, in particular methods that do not allow compensability. Moreover, if different goals are equally legitimate and important, a non-compensatory logic might be necessary."
  - "A non-compensatory multi-criteria approach (MCA) could assure non-compensability by finding a compromise between two or more legitimate goals. In its basic form this approach does not reward outliers, as it retains only ordinal information".
  - It "could be computationally costly when the number of countries is high" (Munda and Nardo 2007).
- **The Handbook's definitions** [V] ([OECD/JRC Handbook, pp. 104–105](https://www.oecd.org/sdd/42495745.pdf)):
  - "Compensability refers to the existence of trade-offs, i.e. the possibility of offsetting a disadvantage on some criteria by a sufficiently large advantage on another criterion … a preference relation is non-compensatory if no trade-off occurs".
  - "the use of weights with ordinal criterion scores originates in non-compensatory aggregation procedures and gives the weights the meaning of importance coefficients (Keeney & Raiffa, 1976; Podinovskii, 1994; Roberts, 1979)".
  - Following Vansnick (1990), the compensatory and non-compensatory approaches derive from Borda (1784) and Condorcet (1785).
  - Arrow's theorem means "no perfect aggregation rule may exist".
- **The non-compensatory rule (NCMC)** [V] ([Handbook §6.13, pp. 112–113](https://www.oecd.org/sdd/42495745.pdf)):
  - "weights in additive aggregations necessarily take the meaning of substitution rates (trade-offs) and do not indicate the importance of the associated indicator"; "For the weights to be interpreted as 'importance coefficients' … non-compensatory aggregation procedures must be used".
  - NCMC (Munda 1995; Roy 1996; Vincke 1992) computes an outranking matrix of pairwise comparisons, eⱼₖ = Σ_q [w_q(Pr_jk) + ½ w_q(In_jk)], summing the weights of indicators on which j beats k plus half of those tied. It then ranks by the Condorcet–Kemeny–Young–Levenglick procedure.
  - The worked example on the Technology Achievement Index notes that a compensatory equal-weight rule "would imply that it is acceptable to renounce … 2% of patents … in exchange for a 2% increase in electricity consumption."
- **Munda and Nardo, and Greco et al.** Munda and Nardo (2009), "Noncompensatory/nonlinear composite indicators for ranking countries: a defensible setting" (*Applied Economics* 41) is the journal version of the method [M]. Greco, Ishizaka, Tasiou and Torrisi (2019) review weighting, aggregation and robustness and warn that composite indices "could sweep methodological issues in their underlying framework under the carpet" [S] ([Social Indicators Research 141:61–94](https://link.springer.com/article/10.1007/s11205-017-1832-9), abstract).
- **Non-compensatory rules in practice** [V] for each instance (sources above):
  - **PEFA M1:** the minimum, with a "+" flag.
  - **PEFA dimensions:** all requirements must be met for a level, a conjunctive and cumulative rule.
  - **PEFA across indicators:** a refusal to aggregate.
  - **UNDRR detailed criteria:** cumulative threshold anchors.
  - **LGAF:** pre-coded statements, a holistic choice of one anchored description.
  - **CRI:** a refusal to give a single overall score.
  - **OECD/JRC:** NCMC by outranking.

### Inferences
- **Two conditions decide meaningfulness [M].**
  - Whether the dimensions share one ordinal scale whose levels are comparable across dimensions. PEFA's A–D is calibrated to a common meaning (C = the "basic level … consistent with good international practices"), so a case can be made for comparable ordinal scales within a scheme.
  - Whether the statement compares objects or reports a value. Reporting "the median dimension score is B" is meaningful; reporting "the mean is 2.6" is not.
- **Rules admissible on comparable ordinal levels [M]:**
  - min (weakest link) and max;
  - the k-th order statistic, including the median, with a declared tie rule for even n (lower or upper median);
  - the profile vector of counts at each level, and cumulative counts ("number of dimensions at B or above"), invariant under any relabelling that keeps the order;
  - threshold rules ("at least k of n at level ≥ B", "no dimension below C");
  - lexicographic rules (first by the minimum, then by the count at the minimum, and so on, as a leximin on levels);
  - Pareto dominance between profiles, reported as a partial order with incomparability kept;
  - weighted median and Sugeno integral, whose weights are importance and not trade-offs;
  - lookup tables declared directly on multisets of levels.
- **A meaningful lookup table need not be meaningful in its comparisons [M].** A table is defined on symbols, but conclusions such as "this indicator improved more than that one" may still rest on an implicit numeric spacing. PEFA's M2 is a function of the code sum, so it inherits the interval assumption. A tool can detect this: test whether a declared table is a function of the sum under some equally spaced coding, and if it is, label the rule interval-dependent.
- **Rules that need interval scales [M]:** sums, means, percentages of the maximum score (UNDRR's), weighted linear indices, and PEFA M2. With mere ratio scales (positive quantities with a true zero, such as the CRI's quantitative metrics before banding), the geometric mean is meaningful for comparisons (Ebert and Welsch).

### Gaps
- Roberts (1979), Stevens (1946), Aczél and Roberts (1989), Marichal (2002), Ovchinnikov (1996) and the full texts of Munda and Nardo and of Greco et al. were not read; their theorems are stated from memory [M] and should be checked before they are cited in a theory note.
- I did not verify whether PEFA's documentation states the equal-spacing assumption behind M2 explicitly. "Approximate average" is as far as the 2016 text goes [V].

## 4. How Scorecards Record the Evidence Behind Each Score, and How Reliability Among Assessors Is Measured

### Takeaway
All four schemes ask for evidence per score: PEFA most strictly, then LGAF and UNDRR.
- **PEFA:** factual evidence, data-quality issues and insufficiency are stated per dimension; D\* marks insufficient information; draft reports are peer-reviewed with the reviewers named.
- **LGAF:** "collective and motivated" panel rankings, optionally checked by sampling.
- **UNDRR:** recorded justification "will enable validation".

None of the schemes I read measures agreement among assessors statistically; disagreement is resolved by consensus, review and moderation. Krippendorff's α with the ordinal difference function suits independent ordinal ratings with missing values.

### Cited Findings
- **PEFA** [V] ([PEFA Framework 2016](https://web.archive.org/web/20210423214147/https://www.pefa.org/sites/pefa/files/resources/downloads/PEFA%202016_latest%20version_with%20links%20%282%29.pdf)):
  - The report must give for each dimension "the rationale for its scoring", indicate "the factual evidence, including quantitative data, which has been used to substantiate the assessment … specific wherever possible, for example, in terms of quantities, dates, and time spans", note "issues in relation to the timeliness or reliability of data and evidence", and state where "insufficient information has been obtained either for a whole indicator or one of its dimension".
  - D\* marks a D from insufficient information at the dimension level (§2.1.1).
  - NA and NU must be justified (§2.1.2).
  - Sampling "should explain the reason for the use of sampling and justify the sampling approach they adopt" (§2.1.5).
  - Box 1.1 records the oversight team, the assessment team, and the invited and actual reviewers of the concept note and draft report, including the PEFA Secretariat, with dates.
- **PEFA performance change** [V] (same): "Presentation of evidence for each dimension and indicator score compared with the previous score", "Highlighting comparability issues … so that the robustness of the evidence of change is fully disclosed", and "Explanation of changes in performance that may not be captured by a change of the score, but is nevertheless evidenced".
- **UNDRR** [V] ([Preliminary](https://preparecenter.org/wp-content/sites/default/files/unisdr_disaster_resilience_scorecard_for_cities_preliminary.pdf)):
  - "Recording your justification for each evaluation score will enable validation, as well as future revisions and tracking of progress".
  - Each Essential lists "Data you will need".
  - Cities "decide on their own 'evidence'" from UNISDR's suggested types.
  - Disagreement is resolved in workshops ("an average or consensus score is applied at the level of each of the 'Ten Essentials'"; "Enable dialogue and concensus between key city stakeholders").
- **LGAF** [V] ([Burns et al. 2010](https://www.fig.net/resources/proceedings/fig_proceedings/fig2010/papers/ts03a/ts03a_burns_deininger_et_al_4640.pdf)):
  - Expert investigators assemble the legal and statistical evidence as "briefing material for the Panels of Expert".
  - Panels give "a collective and motivated ranking".
  - Sampling may "confirm panel opinions with evidence".
  - Panels have three to eight members, with consensus "in a participatory and deliberative process" [S] ([World Bank LGAF page](https://www.worldbank.org/en/programs/land-governance-assessment-framework)).
- **CRI.** Questions are answered "from a team composed of experts from various fields" [S] (search summary of [Arup CRI](https://www.arup.com/projects/city-resilience-index)). The qualitative 1–5 score is anchored by written best and worst cases, and the quantitative score is automated from a metric [V] ([CRI booklet](https://mercociudades.org/wp-content/uploads/2020/10/CRI-total-Booklet.pdf)).
- **Krippendorff's α** [S] ([Wikipedia, "Krippendorff's alpha"](https://en.wikipedia.org/wiki/Krippendorff%27s_alpha), a secondary source):
  - α = 1 − D_o/D_e, the observed disagreement over that expected by chance.
  - It applies to any number of coders and to incomplete data, where unpairable values are dropped.
  - Its ordinal difference function is δ²(v, v′) = (Σ_{g=v}^{v′} n_g − (n_v + n_{v′})/2)², which uses only rank frequencies and so is invariant under strictly increasing recodings.
  - Common practice is to "rely on data with reliabilities α ≥ 0.800", to "consider data with 0.800 > α ≥ 0.667 only to draw tentative conclusions", and to "discard data whose agreement measures α < 0.667". The minimum should depend on the cost of wrong conclusions.
- **Other agreement measures.** Weighted κ (Cohen 1968) and Gwet's AC2 are alternatives for ordinal ratings [M].

### Inferences
- **The schemes end in one consensus score [M].** None of the schemes keeps the independent ratings that agreement statistics need. Moderated scores cannot be audited for assessor reliability afterwards.
- **A design that measures reliability [M]** has assessors score independently first and keeps those ratings sealed. It then computes ordinal α per criterion, and over the scheme, before moderation. The moderated score is kept as a separate act with its reasons, as the LGAF's "motivated ranking" and the UNDRR's justification already do.
- **Calibration [M].** Assessors score shared anchor cases before scoring; a calibration set gives an α of its own. The PEFA Secretariat's review of draft reports (Box 1.1) acts as moderation by a third party. The PEFA CHECK endorsement criteria are a formalised version [M; not read].

### Gaps
- The PEFA CHECK quality-endorsement criteria, PEFA training and certification of assessors, and any published inter-rater study of PEFA, UNDRR, CRI or LGAF scores were not found or read.
- I found no source on how the Scorecard workshops reach a consensus (voting, moderation rules, recorded dissent).

## What This Means for a Computational Infrastructure

The points below are design inferences [M] from the verified material above. They fit the gap ADR 0131 names ("ordinal scorecards in kybernesis"), kybernesis's kernel of "scales and meaningfulness", nomos's agreement of coders, and archeion's sealed journal.

1. **A scheme is data, declared in four layers.**
   - **Statements** (criteria or dimensions). Each has an identifier, its text, its parent, and its scale.
   - **Levels.** An ordered set of symbols (A > B > C > D; 5 > … > 0; 3 > … > 0), each with an anchoring descriptor. Where the source is cumulative, as in PEFA ("every aspect … must be fulfilled") and the UNDRR count anchors, each level holds a list of requirements, and the level is derived as the highest level whose requirements all hold. A count or rate behind a banded level (UNDRR's "at least 8 of 10", the CRI's automated 1–5) is kept with its declared thresholds.
   - **Special values,** each with a declared rule:
     - NA: delete before aggregation.
     - NU: excluded, with a reason.
     - D\*: aggregate as D, with a flag kept in the trace.
     - UNDRR "not relevant": excluded from denominators.
     - Each requires a justification.
   - **Aggregation nodes,** one per parent, each naming its rule. PEFA's tree is 94 dimensions under 31 indicators (4 single, 9 M1, 18 M2), with no node above the indicators, by the Framework's own statement. A scheme that declares no aggregation at a level ("not suitable for aggregating scores across … indicators"; the CRI's "no overall single score") must make the software refuse it.
2. **Rules carry their scale requirement, and the software checks it.**
   - **Admissible on one comparable ordinal scale:** min with the "+" flag (PEFA M1); max; the k-th order statistic and median, with a declared tie rule; profiles of counts per level and cumulative counts; threshold and conjunctive rules; lexicographic rules and leximin; Pareto dominance, with incomparability reported; weighted median and Sugeno integral, whose weights are importance; lookup tables over multisets of levels.
   - **Interval-dependent:** sums, means, percentages of the maximum (UNDRR), weighted linear indices, PEFA M2.
   - The scheme may still declare an interval-dependent rule, since PEFA and UNDRR do, but it is labelled so, and comparisons that depend on it are marked.
   - **A mechanical meaningfulness verdict:** recompute the rule under strictly increasing recodings of the levels. A counterexample, such as PEFA's {D, A} against {C, C} under D = −10, makes the verdict "fails, here".
   - **A lookup-table test:** check whether a declared table is a function of the code sum. PEFA's Table 1 is, with the step maps given in §1.
   - **The M2 table is encoded verbatim,** never computed from the mean, since its rounding is not uniform. Guard tests: 10, 20 and 35 rows; symmetry; monotonicity; values between min and max.
3. **Comparisons across units are governed by the scheme.** The UNDRR Scorecard declares its scores "not comparable across different cities", and cities may reweight the Essentials and drop criteria. A tool should refuse rankings of cities on such a scheme, or demand a common declared weighting and criterion set and state that it departs from the scheme. Change over time within one unit is the supported reading in UNDRR, the CRI and PEFA.
4. **Justification traces are kept per score.** Each score records:
   - who scored it and when;
   - the evidence items, each by digest, with the passage or figure and its date;
   - each requirement of the chosen level and of the next higher level, marked met, unmet or unknown;
   - why the next level was not reached (PEFA's partial-fulfilment rule);
   - data-quality notes and the D\* flag;
   - NA, NU and not-relevant decisions with reasons;
   - for re-assessments, the previous score, its evidence and the comparability issues (PEFA's "performance change").
5. **Disagreement is recorded, not overwritten.**
   - Independent ratings are kept apart, and sealed before moderation.
   - The moderated or consensus score is a separate act with its reasons (the LGAF's "motivated ranking").
   - Reviewers' comments are recorded (PEFA Box 1.1).
   - Krippendorff's α with the ordinal difference function is reported per criterion and per scheme, with the conventional 0.667 and 0.800 marks shown as declared thresholds, not built in.
   - Calibration rounds on shared anchor cases are their own record.
6. **Model proposals stay apart.** If a language model drafts a score or a justification, it is a proposal of grade C, kept separate from the record's score, as the infrastructure already does in tekmerion and the Law and Policy Studios.
7. **Defaults.**
   - A non-compensatory display of profiles (counts per level, the weakest link, the distribution per Essential or pillar), not a single number.
   - Any compensatory index computed only on the scheme's explicit declaration, labelled with its interval assumption, and shown beside a verdict on how robust its rankings are to recodings.
