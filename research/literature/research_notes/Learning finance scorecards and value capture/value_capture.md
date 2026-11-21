# Land Value Capture and Land Readjustment: Instruments, Uplift and Incidence

Notes for the design of software that models and compares value-capture instruments for a declared project. Compiled 2026-11-22.

**Verification markers.** [V] = checked against the primary source I read (full text extracted from the PDF, or the statute or page fetched; where a page was read through the fetch tool's summary, the quoted figures were checked against its quotations). [S] = stated by a secondary source or by a search-result abstract I could not open in full. [M] = from memory or my own inference, unverified.

**Naming.** Following the infrastructure's place-name policy (`data/products/naming.json`, ADR 0069), the jurisdiction the brief calls "Taiwan" is written here as Chinese Taipei; the sources call it Taiwan. **Japanese material** appears only as literature (definitions and worked formulas from a Japanese project). No Japanese city is used as a case in the recommendations of the last section.

**Primary documents read in full (text extracted):**
- OECD/Lincoln Institute, *Global Compendium of Land Value Capture Policies* (2022), 253 pp. — [PDF](https://www.oecd.org/content/dam/oecd/en/publications/reports/2022/06/global-compendium-of-land-value-capture-policies_64f3c257/4f9559ee-en.pdf)
- Smolka, *Implementing Value Capture in Latin America* (Lincoln PFR, 2013) — [PDF](https://www.lincolninst.edu/sites/default/files/pubfiles/implementing-value-capture-in-latin-america-full_1.pdf)
- Suzuki, Murakami, Hong and Tamayose, *Financing Transit-Oriented Development with Land Values* (World Bank, 2015) — [PDF](https://documents1.worldbank.org/curated/en/468551468165570019/pdf/936860PUB00ISB0TransportDevLV0final.pdf)
- Germán and Bernstein, *Land Value Capture: Tools to Finance Our Urban Future* (Lincoln policy brief, Sept 2018) — [PDF](https://www.lincolninst.edu/sites/default/files/pubfiles/land-value-capture-policy-brief.pdf)
- De Souza, "What is Land Readjustment?", ch. 1 of JICA-RI's *Land Readjustment* book — [PDF](https://www.jica.go.jp/jica-ri/publication/booksandreports/l75nbg00000ltoak-att/LAND_READJUSTMENT_Chapter1_web.pdf)
- Ballaney/UN-Habitat, *Supply of Land for Development: Land Readjustment Experience in Gujarat, India* (2013) — [PDF](https://new.unhabitat.org/sites/default/files/download-manager-files/Supply%20of%20Land%20for%20Development-%20Land%20Readjustment%20Experience%20in%20Gujarat%2C%20India%20-%20Urban%20Legal%20Case%20Studies%20Volume%202.pdf)
- UN-Habitat, *Land Readjustment in the Republic of Korea* (2019) — [PDF](https://unhabitat.org/sites/default/files/documents/2019-05/lrk_layout_ha_150519_0.pdf)
- Dye and Merriman, "Tax Increment Financing", *Land Lines* (Jan 2006) — [PDF](https://www.lincolninst.edu/app/uploads/2024/04/tax-increment-financing-lla060102.pdf)
- Debrezion, Pels and Rietveld, Tinbergen Institute working paper TI 2004-023 (the pre-publication version of the 2007 JREFE meta-analysis) — [PDF](https://papers.tinbergen.nl/04023.pdf)
- Colombia, MinVivienda, *Guía para la implementación de la participación en plusvalía* — [PDF](https://www.minvivienda.gov.co/sites/default/files/documentos/guia-para-la-participacion-en-plusvalia.pdf)

**Statutes and pages fetched:** BauGB §§57–58; Spain's RDL 7/2015 art. 18; FHWA's TIF/but-for page; São Paulo's CEPAC auction release.

---

## Key Question 1: How the OECD–Lincoln Global Compendium classifies instruments, and what data it reports on their use

### Takeaway
The Compendium's "OECD-Lincoln taxonomy" has five instruments: infrastructure levy, developer obligations, charges for development rights, land readjustment and strategic land management. It deliberately excludes property and land taxes, tax increment finance, joint ventures and PPPs. Its data come from an expert questionnaire in 60 countries, not from fiscal accounts. It reports counts of countries using each instrument, legal bases, appeals, who administers, valuation methods and obstacles. It reports almost no comparable revenue figures. Only 23% of countries base the total of an infrastructure levy on actual land value increments; the rest base it on cost.

### Cited Findings
**Scope and taxonomy**
- **Coverage.** 60 countries, data collected 2020–2021 by an online questionnaire of over 350 queries, completed by academic experts. Experts were asked to describe "typical scenarios of LVC use in the country within the last 10 years". [V] — [OECD Compendium, Box 1.2](https://www.oecd.org/content/dam/oecd/en/publications/reports/2022/06/global-compendium-of-land-value-capture-policies_64f3c257/4f9559ee-en.pdf)
- **Regions and income.** 27 countries (45%) in Europe, 12 (20%) Asia-Pacific, 11 (18%) Americas, 10 (17%) Middle East and Africa. 35 (58%) high-income, 11 (18%) lower-middle-income, 2 (3%) low-income. [V] — [OECD Compendium, Table 1.1 text](https://www.oecd.org/content/dam/oecd/en/publications/reports/2022/06/global-compendium-of-land-value-capture-policies_64f3c257/4f9559ee-en.pdf)
- **Exclusions.** The taxonomy "does not include land and property taxes", since these are "usually levied in a uniform manner without distinguishing property owners that are affected by public interventions". Likewise "joint ventures, public-private partnerships, or tax increment finance are not included … due to difficulties in delineating their role in capturing value increments." [V] — [OECD Compendium, ch. 1](https://www.oecd.org/content/dam/oecd/en/publications/reports/2022/06/global-compendium-of-land-value-capture-policies_64f3c257/4f9559ee-en.pdf)
- **The five definitions**, quoted [V] — [OECD Compendium, ch. 1](https://www.oecd.org/content/dam/oecd/en/publications/reports/2022/06/global-compendium-of-land-value-capture-policies_64f3c257/4f9559ee-en.pdf):
  - **Infrastructure levy:** "a tax or fee levied on landowners possessing land that has gained in value due to infrastructure investment initiated by the government". The government identifies the catchment. The amount "should be based on the amount of land value benefit obtained", paid once or over time. Other names: betterment contributions, betterment levies, special assessments.
  - **Developer obligations:** "a cash or in-kind payment designed to defray the costs of new or additional public infrastructure and services private development requires", triggered by the developer's application. They are negotiated or set by formula, and include impact fees, negotiated exactions, development charges and inclusionary zoning.
  - **Charges for development rights:** "cash or in-kind contributions payable in exchange for development rights or additional development potential above a set baseline". They require predefined baseline and maximum densities. Related terms: sale of development rights, sale of air rights, transfer of building rights.
  - **Land readjustment:** "pooling fragmented land parcels for joint development, with owners transferring a portion of their land for public use to capture value increments and cover development costs"; also called land pooling.
  - **Strategic land management:** governments "buying, developing, selling and leasing land to advance public needs and recoup value increments". "If governments acquire land at predevelopment prices, they can fully capture increases in land value". Leasing captures increments "through higher rents".
- **Equivalences across names.** Colombia's betterment levies are "similar in scope to 'special assessments' in the United States". Israel's and Poland's "infrastructure levies" are in fact developer obligations. [V] — [OECD Compendium](https://www.oecd.org/content/dam/oecd/en/publications/reports/2022/06/global-compendium-of-land-value-capture-policies_64f3c257/4f9559ee-en.pdf)

**Use of instruments**
- **Counts (at least occasional use, rare use excluded):**

  | Instrument | Countries |
  |---|---|
  | Developer obligations | 43 |
  | Strategic land management | 39 |
  | Land readjustment | 30 |
  | Infrastructure levy | 22 |
  | Charges for development rights | 20 |

  All countries except Uganda use some form of LVC at least occasionally. [V] — [OECD Compendium, Fig. 1.11](https://www.oecd.org/content/dam/oecd/en/publications/reports/2022/06/global-compendium-of-land-value-capture-policies_64f3c257/4f9559ee-en.pdf)
- **By income.** Low- and lower-middle-income countries use on average 2 instruments, against 2.5 for high- and upper-middle-income countries. Chile, Egypt and India use all five regularly. Lower-income countries rely more on strategic land management and land readjustment; higher-income countries use developer obligations "much more frequently". [V] — same source
- **By region.** Land readjustment "in the Americas is particularly rare"; charges for development rights are common in Asia-Pacific. [V] — same source

**Legal basis, appeals and administration**
- **Legal basis.** Over 80% of countries using infrastructure levies and land readjustment have a legal basis for them. About one third of countries have no legal basis for charges for development rights or strategic land management. Fig. 1.3 shows "Yes" at 83% / 73% / 67% for the first three instruments; the labels for the last two were not cleanly recoverable from the PDF text. [V] — same source
- **Appeals** are most frequent against the infrastructure levy and land readjustment. Fig. 1.4 shows "always or frequently" at 17%, 11% and 14% for three instruments and 52% / 33% "sometimes"; the exact mapping of the labels to instruments could not be recovered from the extracted text. [V for the qualitative ranking; label mapping unverified]
- **Administration.** Implementation is mostly local. Land readjustment and strategic land management are more often shared with national government or public corporations. Local officials have some discretion over planning permits in 49 of 60 countries. [V] — same source

**Valuation and obstacles**
- **Valuation.** Market-based land valuation is used in 69% of all countries, 74% of high-income, 77% of upper-middle-income and 46% of lower-middle and low-income countries. Lower-income countries resort more to non-market approaches because cadastres are poor. [V] — same source, Fig. 1.8
- **Obstacles.** The most common is resistance by property owners (58% high-income; 63% middle- and low-income, averaged over instruments). Next is lack of administrative capacity (35% vs 68%). "Charges are too high" is cited by 31% vs 19%, and an inadequate legal framework by 18% vs 45%. "By far the most common" obstacle outside the list was political will. [V; percentages read from Fig. 1.9 in the order printed, 58/31/35/6/19 high-income and 63/18/68/48/45 middle and low] — same source

**How charges are set**
- **Infrastructure levy.** It is applied mostly to transport, then utilities and public space. "Only 23% of countries estimate the amount of total fees based on actual land value increments"; the rest base it on cost. Fees are assigned to plots mostly "according to a score … calculated using known characteristics" (area, zoning, density, taxable value, distance). France and the UK use land area alone. [V] — same source, Fig. 1.13
- **Developer obligations.** Over 90% of countries using them apply them to new development; over 60% apply them to higher-density applications. 79% define the impact area as within the development's boundary. They are assigned more often by established rule than by negotiation. [V] — same source, Fig. 1.14
- **Land readjustment.** Over 80% of countries using it use it for urban expansion. Plots are "most commonly" reallocated proportionally to the value of the original plots. Indonesia, Italy and Turkey use area alone; Chile and China use value and area. Israel and Hong Kong use "vertical" readjustment, allocating units in buildings. Consent varies: all owners, a share (typically two thirds; Colombia and Korea a simple majority of 51%), or none. Portugal expropriates if full consent fails. [V] — same source, Fig. 1.16

**Country figures relevant to instrument design** [V] — [OECD Compendium, country profiles](https://www.oecd.org/content/dam/oecd/en/publications/reports/2022/06/global-compendium-of-land-value-capture-policies_64f3c257/4f9559ee-en.pdf)
- **Colombia.** The infrastructure levy (contribución de valorización, legal basis since 1921) "usually amounts to the estimated total cost of public works". It is sometimes proportional to the increment. Up to 30% of administrative costs may be added, though usually less than 10% is charged. The levy is allocated by a formula on distance, location, size, quality and value. Participación en plusvalías recovers "between 30% and 50% of the land value increase resulting from changes in land-use regulations or building indexes". It has faced problems because increases are hard to estimate, and the largest cities have strengthened developer obligations instead.
- **Germany.** Urban development contracts (städtebauliche Verträge) can require up to 30% affordable units, as in Frankfurt. Erschließungsbeiträge "can amount to 90% of public costs". For road renewal, 25%–75% of costs are passed to benefiting owners depending on road type. In Umlegung, owners "typically" give up to 25% in greenfield, more in brownfield; owners are compelled to participate.
- **India.** The betterment levy "can vary from 30% to 50% of the increase in value of land, depending on the state Acts", and among the states with provisions it "has only been implemented in Gujarat", where a uniform rate per m² recovers the scheme's cost. Land readjustment reserves "23% to 50% of the readjusted plots" for public improvements. Excess floor area is charged at "usually between 30% to 70%" of market value. "Public entities conduct analyses to define the contribution ratio that maintains the proportionality of landowners' share before and after pooling."
- **Korea.** Land is redistributed "after deducting a share for public improvements and another one for future sales or leases". "100% of the cost of public improvements related to land readjustment projects is recovered through the sale or lease of those plots". Privately led projects need consent of owners representing 1/2 of owners and 2/3 of land area.
- **Spain.** Reparcelación urbanística rests on Land Act 7/2015 arts. 13.2.c and 14.c, with details in 17 regional laws, and is used "frequent[ly]". "The burden imposed on landowners cannot be larger than the benefits of the project." Owners receive a plot of proportional value as close as possible to the original. The infrastructure levy (contribución especial) is used rarely.
- **United Kingdom.** "Developer obligations effectively capture about 30% of development value on greenfield sites, notably for large builders … developers … can shift the costs to landowners."
- **Hong Kong.** All land is leased. Leases run 75–99 years historically and 50 years for new leases. The land premium is usually set by auction, plus annual rent of 3% of rateable value. Lease revenue was 19%–27% of the yearly budget in 2016–2019. Lease modification charges for changes of use or density are "calculated according to the estimated land value gains". In 2018-19 they were 27.2% of lease revenue and 5.4% of the budget. Lease renewal charges have "not been successful".

### Inferences
- The taxonomy suits a module's top-level type: five instruments, plus TIF and recurrent property or land taxes as two further types the Compendium excludes but the brief asks for. Its two axes are who triggers the charge (government action for the levy and readjustment; developer initiative for obligations and rights charges) and what base it is levied on (cost, value increment, area or a score). These are natural fields of a declared instrument. [M]
- The Compendium's data are categorical, expert-reported frequencies, not revenues or capture rates. A module cannot calibrate capture rates from it; it can only supply legal parameters such as statutory shares and consent thresholds. [M]
- **A conflict to keep visible.** The general section says Korea requires a simple majority (51%), while the Korea profile says 1/2 of owners and 2/3 of area for private projects. Probably different project types. [M]

### Gaps
- No exact country-by-instrument counts by region were extractable from Fig. 1.11's bar segments, nor the last two legal-basis percentages.
- The Compendium gives no comparable revenue series. Revenue appears only in scattered country profiles, such as Hong Kong's.
- The policy-highlights PDF was unreachable; the full report was read instead.

---

## Key Question 2: How land readjustment works — contribution ratio, reserve land, replotting, value preservation, settlement money and feasibility

### Takeaway
Land readjustment is a pair of accounting identities.
- **Area.** Public land before + private land before = public land after + private land returned + reserve land.
- **Value.** Each owner's returned plot, plus or minus settlement money, must be worth at least the original plot, and in proportion to it. The contribution ratio d = (P + R)/A must be matched by a unit-value uplift y with (1 − d)·y ≥ 1. The project is feasible when reserve-land sales, subsidies and contributions cover costs.

Germany caps the area deduction in law (30% greenfield, 10% elsewhere) and settles the excess advantage in money. Gujarat levies half the increment, computed in practice from low administered values. Korea's Act set 50% returned, 30% infrastructure and 20% cost-recovery land.

### Cited Findings

**Definitions and formulas (from the Japanese literature, reported as literature)** [V] — [De Souza, JICA-RI ch. 1](https://www.jica.go.jp/jica-ri/publication/booksandreports/l75nbg00000ltoak-att/LAND_READJUSTMENT_Chapter1_web.pdf)
- **Contribution ratio (減歩率, genbu ritsu):** "The percentage difference in private property area before and after replotting". It serves "(i) to adjust the demand for land required for proper urbanization (public infrastructure), and (ii) to create supply to – partially or fully – finance project costs."
- **Reserve land (保留地, horyūchi):** "plots of land to be sold to finance the land readjustment projects". Their quantity is set by an economic criterion (financing new public facilities) and an equity criterion (equitably balancing the value increase). Small owners may contribute money instead of land.
- **Statutory value cap (LRL art. 96.2):** "The equivalent value of the land contribution shall not exceed the percentage stipulated on the increase of the total value of private land after the project". Revenue from reserve land in public projects may be used only for the project.
- **Settlement money:** "The equity is money to be collected and paid to clear imbalance for the replot value, if any." Land evaluation is used "to judge the contribution for reserve land, calculate compensation for loss in damage, calculate the replotting area, and calculate equity collection and payment". Japanese practice uses a street-value method: a unit value per street, adjusted by street, accessibility and plot coefficients.
- **The area identity and contribution ratio.** With M = public area before, N = public area after, A = private area before, E = private area returned, R = reserve land:
  - M + A = N + E + R
  - P = N − M (net increase in public area)
  - E = A − P − R
  - d = (P + R)/A = P/A + R/A
- **The proportional ratio (value preservation):**
  - Pr = V′/V = (E·e)/(A·a) = (1 − d)·y, where a and e are the average unit values before and after (private land excluding reserve land) and y = e/a.
  - "Equal balance" requires Pr = 1. For example, d = 33.3% requires y = 1.5.
  - *Caveat:* the source's prose defines y as "previous land price divided by posterior land price", which contradicts its own arithmetic; y = e/a is meant.
- **Worked example (Misato Chuo, 114.8 ha):**
  - Public area 14.0% → 32.5%.
  - A = 987,667 m², P = 213,201 m², R = 160,137 m², E = 614,329 m².
  - d = 21.6% + 16.2% = 37.8%.
  - a = ¥151,000/m², e = ¥295,000/m², so y = 1.954 and Pr = (1 − 0.378)·1.954 = 1.215.
  - Feasibility: expenditure ¥61,561m. Revenue was 57% reserve-land sales (¥35,092m) and 41% national, prefectural and municipal subsidies (¥25,349m), plus other revenue of ¥1,120m. The station's construction cost was excluded.
  - I recomputed the identities from the table and they hold. [V]
- **Hold-outs.** The chapter frames LR against the hold-out problem: owners demanding more than their true reservation value in voluntary assembly (citing Miceli and Sirmans 2007). [V for the framing]

**Germany: Umlegung under the Baugesetzbuch** [V] — [§58 BauGB](https://www.gesetze-im-internet.de/bbaug/__58.html); [§57 BauGB](https://www.gesetze-im-internet.de/bbaug/__57.html)
- **Area-based distribution (§58).** The area contribution (Flächenbeitrag) offsets the advantage the readjustment brings. It may be "nur bis zu 30 vom Hundert der eingeworfenen Fläche" in areas first developed and "nur bis zu 10 vom Hundert" elsewhere. An advantage beyond what the capped area covers must be compensated in money. The authority may take money instead of area in whole or part. Money values are taken at the date of the Umlegungsbeschluss.
- **Value-based distribution (§57).** The mass is distributed "in dem Verhältnis, in dem die zu berücksichtigenden Eigentümer an der Umlegung beteiligt sind". "Jedem Eigentümer soll ein Grundstück mindestens mit dem Verkehrswert zugeteilt werden" of the former plot, valued at the date of the Umlegungsbeschluss. "Unterschiede zwischen den so ermittelten Verkehrswerten sind in Geld auszugleichen."
- **Practice.** Typical greenfield contribution is up to 25%, more in brownfield. Participation is compulsory and expropriation rare. [V] — [OECD Compendium, Germany](https://www.oecd.org/content/dam/oecd/en/publications/reports/2022/06/global-compendium-of-land-value-capture-policies_64f3c257/4f9559ee-en.pdf)

**Spain: reparcelación**
- **Cession of edificabilidad.** National law (RDL 7/2015, art. 18.1.b) requires the developer of a transformation to cede to the administration land free of urbanisation charges corresponding to a percentage of the weighted average edificabilidad that "no podrá ser inferior al 5 por ciento ni superior al 15 por ciento". Planning law may raise it, "de forma proporcionada y motivada", "hasta … un máximo del 20 por ciento". For actuaciones de dotación, it is determined "sólo al incremento de la edificabilidad media ponderada". [V] — [BOE, RDL 7/2015](https://www.boe.es/buscar/act.php?id=BOE-A-2015-11723)
- **Equidistribution.** The approving act of an equidistribution instrument effects the "subrogation of original plots by resulting plots" (art. 23.1, as summarised by the fetch). [V via fetch summary]
- **History.** The duty to cede 10% dates from the 1975 reform; some regional laws raised it to 15% from the 1990s. Constitutional Court ruling 54/2002 partly annulled a regional law requiring cession in consolidated urban land. [S] — [search of INAP journal articles](https://revistasonline.inap.es/index.php/REALA/article/download/8894/8943/9121)
- **Value preservation and consent.** The burden "cannot be larger than the benefits". Owner consent is required, at a level varying by project type. [V] — [OECD Compendium, Spain](https://www.oecd.org/content/dam/oecd/en/publications/reports/2022/06/global-compendium-of-land-value-capture-policies_64f3c257/4f9559ee-en.pdf)

**India: Gujarat's town planning schemes (TPS)**
- **The contribution.** Scheme costs "shall be met in whole or part by a contribution to be levied by the local authority in proportion to the increment in the land value. The contribution is stipulated to be half of the increment in land value." [V] — [Ballaney/UN-Habitat 2013, p. 39](https://new.unhabitat.org/sites/default/files/download-manager-files/Supply%20of%20Land%20for%20Development-%20Land%20Readjustment%20Experience%20in%20Gujarat%2C%20India%20-%20Urban%20Legal%20Case%20Studies%20Volume%202.pdf)
- **Appropriation limits (1999 amendment, s. 40(3)(jj)).** Up to 15% of the scheme area for roads, 5% for parks and open space, 5% for social infrastructure, and 15% "for sale by appropriate authority" (the proceeds to be spent on infrastructure in the scheme). Separately, up to 10% for housing for "socially and economically backward classes". "The total maximum suggested percentage of appropriations … is 50 per cent." In practice "the usual prescribed 35 to 40 per cent" is taken. [V] — same source, pp. 47, 126, 129
- **The "F form" (valuation and distribution statement).** For each owner it tabulates the original plot and value, the final plot, size and value, "the increase in the land value or the increment", "the compensation to be paid for the land appropriated", and "the net demand … which is the increment minus the compensation". (Elsewhere: the halved increment minus compensation.) It can levy "a larger betterment on a landowner who contributes less land". [V] — same source, pp. 141–142
- **Surat's draft schemes** show the columns "Increment (section 65)", "Contribution (section 66) 50%" and "Contribution (+) compensation (−) (section 67)". [S] — [search result on Surat TP74 form](https://www.suratmunicipal.gov.in/Content/Documents/Departments/Draftschemes/TP74%20PAL/TP74_PAL_FORM.pdf)
- **How the increment is computed in practice:**
  - "the increment in land value is worked out which is the addition of the development costs to the original land values times two. This increment is then halved and is the landowner's contribution… From this amount the compensation… is deducted and the 'net demand' or the development charge is levied." [V] — same source, p. 154
  - My reading: final value = original value + 2 × per-plot development cost, so the halved increment equals the development cost. The "50% of increment" rule is thus back-solved into cost recovery, not value capture. [M]
- **Low recovery:**
  - Development charges "usually range from Rs. 150 to 300 per m² of land, while the actual development costs for good quality infrastructure range from Rs. 700 to 1000 per m²".
  - Valuation uses jantri (the ready reckoner), well below market. Owners accept 35–40% land loss "because the market value of the land after the town planning scheme is done are at least 10 times higher".
  - "But the negative impact of this is low quality infrastructure and the authority not being able to tap into the land value gains."
  - [V] — same source, pp. 153–154, 160
- **Land bank.** Up to about 10% of the scheme area can be appropriated for the authority's land bank; such plots sell above market because of clear titles, size and shape. [V] — same source, p. 140
- **Duration:**
  - Of 204 Gujarat schemes, 29 took 4y2m–10y, 47 took 10–15y, 49 took 15–20y and 19 took over 20y; 60 had no data. The text says "86 per cent … taken more than 10 years".
  - In Ahmedabad (63 schemes): 2, 14, 11 and 7, with 29 lacking data. The text says 97%.
  - **Inconsistency:** the tables give 115/144 = 79.9% for Gujarat and 32/34 = 94.1% for Ahmedabad among schemes with data, not 86% and 97%. [V for counts; arithmetic mine]
  - Same source, pp. 65–66.
- **Retrofitting built-up land (TPS 97 Naroda North, Ahmedabad).** About 70% of the area was already developed, so only an extra 18.08% could be appropriated (21.68% total under roads, amenities and revenue plots; final plots 78.32%). A higher betterment charge was levied instead. Declared June 2004, prepared in 13 months, sanctioned 30 August 2006. [V] — same source, pp. 128–129
- **Mittal's work** on Gujarat (self-financing TPS) was not found in my searches. [Gap]

**Korea** [V] — [UN-Habitat 2019](https://unhabitat.org/sites/default/files/documents/2019-05/lrk_layout_ha_150519_0.pdf)
- **Statutory split and practice:**
  - "Prior to the Land Readjustment Act of 1966 landowners contributed 25 to 30% of their land."
  - Under the Act, "50% of the readjusted land is retained after replotting by the original land owners, 30% is for infrastructure and the remaining 20% for development cost recovery." Minor deficits are met from the municipal budget.
  - "In practice the rate of decrease of private land went up to 68% in high value districts of Seoul."
- **Seoul by decade (1960s, 1970s, 1980s):** 20, 14 and 5 districts. Average public land 28.4%, 30.0% and 47.5%; average reduction of housing lot size 31.6%, 43.7% and 55.0%.
- **Worked decrease rate (Myeonmok, 1962–69):**
  - Public land decrease 203,758/1,061,887.3 = 19.19%; land for sale 62,974.5/1,061,887.3 = 5.93%; total 25.12%.
  - Verified as 1 − 795,154.1/1,061,887.3.
  - *Caveat:* the table lists public land of 270,345.8 m² after and 68,786.8 m² of existing public ("33 clause") land before. The difference is 201,559 m², not 203,758 m², an unexplained gap in the source.
- **Funding.** Before Gangnam, national and city assistance was 30–50% of programme costs. In Gangnam's Yeongdong District 1, sale of land "set out for recompense" covered more than 90% of costs, and 99.9% in District 2.
- **Speculation and appraisal:**
  - "The buoyant real estate market and the prospect of speculative gains may have contributed significantly to land owners agreeing to higher levels of decrease."
  - "Plots in Land Readjustment projects were speculated upon and commanded high prices"; an Act on Special Taxes for Controlling Real Estate Speculation followed.
  - Cooperative-led projects show higher decrease ratios than public ones, because public bodies absorb logistical costs.
  - Appraisal moved from government to certified appraisers in 1973. The Publicly Noticed Value system (1970s) and the Public Announcement System of Land Price (1989) mass-appraise from reference lots.

**Chinese Taipei (urban land consolidation)**
- Cost-equivalent land is land "used to pay for public facilities" within a readjustment area. It is central to financial self-liquidation; consolidation is governed by arts. 56–60-1 of the Equalization of Land Rights Act. [S] — [IRSPSD 11(2)](https://www.jstage.jst.go.jp/article/irspsd/11/2/11_14/_article); [Judicial Yuan Interpretation 739](https://www2.judicial.gov.tw/FYDownload/EN/p03_01.asp?expno=739)
- From memory: art. 60 caps the combined contribution of public-facility land and cost-equivalent land at 45% of the district's total area unless a majority of owners agree. Zone expropriation returns at least 40% of land to owners. [M — unverified; the statute text was not retrieved]

**Colombia**
- Land readjustment is used to incorporate rural land and for renewal, usually through a real estate trust (fiducia) set up by owners, developers or public entities. [V] — [OECD Compendium, Colombia](https://www.oecd.org/content/dam/oecd/en/publications/reports/2022/06/global-compendium-of-land-value-capture-policies_64f3c257/4f9559ee-en.pdf)
- Consent by simple majority. [V] — same source, general section

**Hong & Needham (2007) and Sorensen**
- These were not retrieved. From memory: Hong and Needham frame LR as a self-financing, consensual instrument whose success depends on the uplift exceeding the contribution, the legal power to bind minorities, and trusted valuation. Sorensen documents its role in planned suburban growth and its tendency to produce small plots and fragmented outcomes where uplift is weak. [M]

### Inferences
- **A general value-based LR model** for a module (my synthesis of §57 BauGB, LRL art. 96 and the Gujarat F form) [M]:
  - Let parcel i have entry value V_i (before, at the valuation date) and allocated value V′_i (after, at the same date's market conditions, with the project).
  - Entitlement: V*_i = V_i · (ΣV′ / ΣV), proportional shares of the distributable value.
  - Settlement s_i = V′_i − V*_i (positive: the owner pays; negative: the owner is paid).
  - Value preservation requires V*_i ≥ V_i, equivalently ΣV′ ≥ ΣV, which aggregates to (1 − d)·y ≥ 1 on private land.
  - The public's capture is the value of P + R. Reserve land is the part convertible to cash: R·e_R.
- **Feasibility:** costs C ≤ R·e_R·(1 − selling costs) + subsidies + cash contributions + betterment levied. With project duration T (10–20 years in Gujarat), revenues and costs must be discounted. [M]
- **An area-based model (§58)** is a special case: d_i ≤ d̄ (30%/10%), with the excess advantage max(0, ΔV_i − value of the area taken) collected in money. [M]
- **Gujarat's practice** shows that the statutory share (50%) and the effective capture can diverge by an order of magnitude when the valuation basis is administered rather than market. A module must keep "statutory rate", "valuation basis" and "effective capture" as separate quantities. [M, from the V findings]

### Gaps
- No primary text was retrieved for Chinese Taipei's art. 60, Hong and Needham (2007), Sorensen, or Mittal on Gujarat.
- No primary data on hold-out frequency or displacement in LR projects was found.
- The Gujarat Act's own text (indiacode) could not be downloaded.

---

## Key Question 3: How tax increment financing revenue is computed, and its documented risks

### Takeaway
TIF freezes a district's base assessed value B. It earmarks the tax on the increment I (all overlapping jurisdictions' combined rate) to the district authority for a term, usually 15–30 years, often to service bonds. It creates no new tax; it reallocates. The "but-for" test is the legal claim that the increment would not exist without the district. It is close to pro forma.

The best-known evidence (Illinois) finds that municipalities adopting TIF do not grow faster overall. Commercial TIF districts cannibalise commercial growth elsewhere in the same city, and overlapping jurisdictions (school districts) bear most of the diverted revenue.

### Cited Findings

**Mechanics** [V] — [Dye and Merriman 2006](https://www.lincolninst.edu/app/uploads/2024/04/tax-increment-financing-lla060102.pdf)
- **Value accounting.** B = base-year assessed value in the district, N = base-year value outside it, I = increment in the district, G = growth outside.
  - "Before-TIF value = before TIF local government tax base = B + N"
  - "after-TIF value = B + N + I + G"
  - "after-TIF tax base available to local governments = B + N + G"
  - "TIF district authority's tax base = I"
- **Revenue.** The district "gets revenues from the increment times the combined tax rate for all local governments together". The Illinois-like example rates are municipal 0.15%, school 0.60%, others 0.25%, combined 1.00%. "For each 15 cents of its own would-be tax revenues the municipality puts on the line, the school district and other local governments contribute another 85 cents."
- **Designation and history.** Designation usually requires a finding of "blighted" or "underdeveloped" and that development would not occur "but for" the subsidy. "It is only a bit of an overstatement to characterize the 'blight' and 'but for' findings as merely pro forma exercises, since specialized consultants can produce the needed evidence in almost all cases." TIF is authorised in 48 states, dates to California in 1952, and expenditures are often debt-financed.
- **Base and term (FHWA).** The base revenue keeps flowing to the general fund; revenue above the benchmark, "or a designated portion of it", goes to a special account. Revenues increase "NOT because of higher tax rates". After termination all revenue returns. FHWA's page states district life inconsistently as 15–20, 15–30 and 25–30 years. [V] — [FHWA, "Essential Nexus, Rough Proportionality, and But-For Tests"](https://www.fhwa.dot.gov/ipd/value_capture/resources/value_capture_resources/essential_nexus/tax_increment.aspx)
- **TIF is not value capture as such.** "TIFs can similarly direct a portion of funds captured by the property tax toward specific public purposes, but they are not in themselves an additional means of land value capture." [V] — [Germán and Bernstein 2018](https://www.lincolninst.edu/sites/default/files/pubfiles/land-value-capture-policy-brief.pdf)
- **Windfalls.** FHWA calls TIF "revenue segregation". It estimates landowners may keep 60–80% of publicly created land value, and that TIFs, "unlike special assessments, exactions, or impact fees", impose no additional burden on benefiting properties. [V] — [FHWA](https://www.fhwa.dot.gov/ipd/value_capture/resources/value_capture_resources/essential_nexus/tax_increment.aspx)

**Risks and evidence**
- **Self-financing.** A 1998 PPIC study of 38 California districts found "only four TIF districts … to generate enough new revenue to be self-financing". [V, as quoted by FHWA]
- **Courts.** Illinois and Missouri appellate cases (Burr Ridge 2003, Malec v. Belleville 2011, Castel Properties v. Marion 1994, JG St. Louis West v. Des Peres 2001) turned on unsupported blight findings and undocumented but-for findings, with deference to boards in Missouri. [V via fetch summary] — same source
- **Illinois evidence** [V] — [Dye and Merriman 2006](https://www.lincolninst.edu/app/uploads/2024/04/tax-increment-financing-lla060102.pdf):
  - **First study** (Dye and Merriman 2000, 235 Chicago-area municipalities, two-stage selection correction with the inverse Mills ratio): "property values in TIF-adopting municipalities grew at the same rate as or even less rapidly than in nonadopting municipalities"; the offset "seemed to come from smaller growth in non-TIF area".
  - **Second study** (2003): 246 six-county municipalities, 1,242 Illinois municipalities and 247 TIF districts.
    - With controls, six-county: "no positive impact of TIF adoption on the growth in citywide property values. Any growth in the TIF district is offset by declines elsewhere."
    - All-county sample: "a significantly negative impact of TIF adoption on growth in overall available (non-TIF) property values".
    - "Commercial and industrial TIF districts both show a significantly negative impact on growth in commercial assessed values outside the district."
  - **Raw means, six-county** (100 adopters / 146 non-adopters): pre-adoption 1980–84 gross EAV growth 4.66 vs 4.41% per year; post-adoption 1995–98 5.20 vs 6.46; adopters' net-of-TIF growth (G/N) 5.06.
  - **Raw means, 102-county** (205 / 1,037): 3.31 vs 1.86 pre; 6.27 vs 7.60 post; adopters' available EAV growth 5.19.
  - **The 247 districts:** average base about $11 million; district EAV growth averaging 24% a year against 3% in the host municipalities; most growth in the first years, declining about 1 point a year. The TIF–city growth relation is U-shaped, turning positive at host growth of about 6%.
- **Reverse causality.** "Anticipated growth in property values could lead to TIF adoption if municipalities attempt to capture revenues from overlapping governments." [V] — same source
- **Wisconsin.** Kashian, Skidmore and Merriman examine whether TIF-district growth comes at the expense of the rest of the city. [S — not read] — [search result](https://www.lincolninst.edu/app/uploads/2024/04/1319_KSM20Final.pdf)

### Inferences
- **Revenue identity for a module** [M]:
  - R_t = s · τ_t^combined · max(0, AV_t^district − B), where s is the designated share (≤ 1) and τ the sum of all overlapping rates, unless some jurisdictions are exempted ("pass-through").
  - PV = Σ_{t=1..T} R_t/(1+r)^t.
  - Bond capacity ≈ PV / debt-service coverage. [M]
- **Diversion identity.** Let β ∈ [0, 1] be the share of I that is truly "but-for". Then the diversion from overlapping jurisdiction j in year t is D_{j,t} = τ_j · (1 − β) · I_t, and the true net new revenue is τ · β · I_t. Displacement within the city (lower G) is a second loss, −τ · ΔG. A module should require β and ΔG as declared assumptions or estimates, never default β = 1. [M]
- Capture of "uplift" by TIF is bounded by the effective property-tax rate on value over the term. With a 1% combined rate on capital value and r = 5% over 23 years, PV ≈ 13.5% of a permanent increment; so TIF captures at most a small share of the value uplift. This is consistent with FHWA's 60–80% retained. [M, arithmetic mine]

### Gaps
- Merriman's later Lincoln report on improving TIF, the GAO and other state audits, and studies of TIF's school-finance effects were not read.
- Comparative evidence on capture rates of TIF versus special assessments was not found.

---

## Key Question 4 (objective a): The mechanics of levies, developer obligations, development rights and leasing

### Takeaway
- **Betterment levies and special assessments** apportion a total (usually the project's cost, sometimes a share of value gain) across a benefit zone by a formula.
- **Impact fees, development charges and planning obligations** are per-unit charges at permission.
- **Charges for development rights** price floor area above a basic FAR, by formula (Brazil's "virtual land" method) or by auction (São Paulo's CEPACs, traded securities with zone conversion factors).
- **Leasing** captures through an auctioned premium, annual rent and modification premiums (Hong Kong), and in rail-plus-property through granting rights at "before-rail" prices and selling at "after-rail" prices.

### Cited Findings

**Betterment levies and special assessments**
- **Definition.** "Owners of select properties pay the municipality a fee, which defrays the cost of a public improvement or service from which the owner specifically benefits." [V] — [Germán and Bernstein 2018](https://www.lincolninst.edu/sites/default/files/pubfiles/land-value-capture-policy-brief.pdf)
- **Colombia's valorización:** total ≈ cost of works plus up to 30% administration (usually under 10%), apportioned by distance, location, size, quality and value. [V] — [OECD Compendium](https://www.oecd.org/content/dam/oecd/en/publications/reports/2022/06/global-compendium-of-land-value-capture-policies_64f3c257/4f9559ee-en.pdf)
- **Revenue weight** [V] — [Smolka 2013, ch. 3](https://www.lincolninst.edu/sites/default/files/pubfiles/implementing-value-capture-in-latin-america-full_1.pdf):
  - Betterment levies typically account "for much less than 1 percent of own local revenues": Mexico ≤ 0.42% of municipal revenues; Brazil in 2011 0.25% of fiscal revenues and 6.8% of property-related tributes; Rosario 0.30%.
  - Outliers: Medellín in 1968, 45% of all local public expenditures; Cali in the early 1980s, 30%; Bogotá in 1993, 24% of local revenues; Bogotá recently, about US$1 billion of works.
  - Cuenca (Ecuador): almost US$200 per capita over 10 years against Bogotá's US$150. 90% of households paid within four years; 95% of projects collected 60% from contributions; 3% non-compliant.
  - "More than 50 percent of Medellín's main road grid was paid for with betterment levies."
  - In Brazil, 2,890 municipalities (52.5%) collected some betterment revenue; in only 667 of 5,505 did it exceed 10% of property tributes.
- **Buenos Aires** (Law 23.514 of 1987): for a 40 km subway line, a 5% addition to all property taxes plus a 2.4% surcharge within 400 m of stations. The fund received about US$750 million in 2012. [V] — same source
- **Germany:** road renewal 25–75% to benefiting owners; Erschließungsbeiträge up to 90% of public costs. [V] — [OECD Compendium](https://www.oecd.org/content/dam/oecd/en/publications/reports/2022/06/global-compendium-of-land-value-capture-policies_64f3c257/4f9559ee-en.pdf)
- **US apportionment** methods (front-foot, area, zone of benefit, assessed value) and the legal "special benefit" requirement are standard. [M — not verified in a source read here]

**Colombia's participación en plusvalía (a charge on regulatory uplift)**
- **Rate.** Councils set a rate that "podrá oscilar entre el treinta (30%) y el cincuenta (50%) del mayor valor por metro cuadrado". Bogotá phased it in from 30% (2004) to 50% (2006 onward). [V] — [MinVivienda guía](https://www.minvivienda.gov.co/sites/default/files/documentos/guia-para-la-participacion-en-plusvalia.pdf); [Smolka 2013](https://www.lincolninst.edu/sites/default/files/pubfiles/implementing-value-capture-in-latin-america-full_1.pdf)
- **Triggering acts (art. 74, Ley 388):** conversion of rural to urban land, changes of zoning or use, and higher density. **Exigibility:** only at a building-licence application, change of use or transfer, with discounts of 5% and 10% in specified cases. [V] — [MinVivienda guía](https://www.minvivienda.gov.co/sites/default/files/documentos/guia-para-la-participacion-en-plusvalia.pdf)
- **Relation to valorización.** Art. 87 contemplates plusvalía from public works only where valorización was not used to finance them, an explicit rule against double charging. [S] — [search of academic article on plusvalía vs valorización](https://repository.urosario.edu.co/bitstreams/2a89d543-0b7c-4fcd-b38c-4cf85be54569/download)

**Developer obligations: impact fees, development charges, s.106/CIL**
- **San Francisco** impact-fee revenue: US$96.1m (FY2013-14), US$90.2m (FY2014-15), US$236.9m (FY2015-16). [V] — [Germán and Bernstein 2018](https://www.lincolninst.edu/sites/default/files/pubfiles/land-value-capture-policy-brief.pdf)
- **Florida** collected over US$2 billion in FY2005–06, more than 5% of revenues in 48 counties and 28% of local revenues in large counties such as Orange (Burge 2010). [V as cited] — [Smolka 2013](https://www.lincolninst.edu/sites/default/files/pubfiles/implementing-value-capture-in-latin-america-full_1.pdf)
- **England.** Developer contributions secured through s.106 and CIL were about £7bn in 2018-19, roughly two thirds as affordable housing, up 16% from £6bn in 2016/17. [S] — [MHCLG 2018-19 study page](https://www.gov.uk/government/publications/section-106-planning-obligations-and-the-community-infrastructure-levy-in-england-2018-to-2019-report-of-study)
- **Legal limits (US).** The essential nexus and rough proportionality tests (Nollan 1987, Dolan 1994, extended to monetary exactions by Koontz 2013) limit exactions to the impact of the development. [M — FHWA's page did not define them]

**Charges for development rights: OODC and CEPACs (Brazil)**
- **Legal basis.** Federal Law 10,257/2001 (Statute of the City) codified the onerous grant of the right to build (Outorga Onerosa do Direito de Construir, OODC) above a basic FAR, and CEPACs. [V] — [Germán and Bernstein 2018](https://www.lincolninst.edu/sites/default/files/pubfiles/land-value-capture-policy-brief.pdf)
- **The "virtual land" method** [V] — [Smolka 2013, pp. 37–39](https://www.lincolninst.edu/sites/default/files/pubfiles/implementing-value-capture-in-latin-america-full_1.pdf):
  - With basic FAR 1 and maximum 3, a developer of 750 m² either buys a 750 m² plot or buys 250 m² and pays for 500 m² of rights. "For this additional area the developer would be paying the equivalent of two more plots of land with the original or pre-existing FAR of 1".
  - The base unit land value comes from the city's property-tax valuation maps. The method is used in Blumenau, Curitiba, Porto Alegre, Salvador and São Luís.
  - Other cities: Florianópolis and Natal charge a percentage of construction cost; Campo Grande "a fixed 70 percent of the estimated land value increment"; Salvador "50 percent over the original value".
  - In Curitiba, rights "were generally sold at less than the full appreciation value".
- **CEPAC conversion and stock accounting** [V] — [Smolka 2013, ch. 5](https://www.lincolninst.edu/sites/default/files/pubfiles/implementing-value-capture-in-latin-america-full_1.pdf):
  - "Usually one CEPAC is needed for each square meter of building rights, but … adjustments are made within a range from 2 to 0.5 m² for more or less desirable locations." The rights are auctioned in tranches over time as "a market control strategy".
  - **Faria Lima** (2004–2010): seven auctions at US$550–2,000 per CEPAC; 682,669 offered, 638,074 bought; US$723 million.
  - **Água Espraiada** (2004–2012) by CVM distribution:

    | Date | CEPACs | US$ | Average US$ per CEPAC |
    |---|---|---|---|
    | 14/7/2004 | 299,368 | 51.4m | 172 |
    | 10/1/2007 | 317,781 | 65.3m | 206 |
    | 23/12/2008 | 186,740 | 103.6m | 555 |
    | 5/9/2008 | 1,099,880 | 386.5m | 351 |
    | 9/2/2012 | 1,360,338 | 865.7m | 636 |
    | Public total | 3,263,907 | 1,447.5m | 443 |
    | Private offers | 127,092 | 25.7m | 202 |
    | Grand total | 3,390,999 | 1,473.2m | — |

  - **Stock identity:** 2,333,897 used for licences; 1,057,102 still in circulation; 3,750,000 authorised; balance 359,001. Both identities check: 3,390,999 − 2,333,897 = 1,057,102 and 3,750,000 − 3,390,999 = 359,001. [V; arithmetic mine]
  - In its original formulation the public would retain "usually around 50 percent" of the land value increment, and proceeds are reinvested inside the operation's perimeter.
  - **Curitiba (Linha Verde):** 4,830,000 CEPACs authorised at a minimum of US$100. The first auction (June 2012) offered 141,588; three bidders took 70%. All sold at the minimum, raising US$14.2 million against US$30 million expected.
- **Faria Lima, 5 December 2019:** "vendeu em leilão 93 mil títulos" for "R$ 1,637 bilhão", "com ágio de cerca de 170% em relação ao valor de referência" (reference R$607.38 million). Proceeds "só podem ser utilizados no perímetro da Operação Urbana". [V] — [Prefeitura de São Paulo](https://prefeitura.sp.gov.br/w/noticia/leilao-de-cepacs-da-operacao-urbana-faria-lima-captou-r-1-64-bi-para-a-cidade-1). Implied ≈ R$17,600 per CEPAC achieved, against ≈ R$6,530 reference. [M, arithmetic]
- **Cumulative total.** São Paulo "has generated nearly US$2 billion from CEPACs" (as of 2018). [V] — [Germán and Bernstein 2018](https://www.lincolninst.edu/sites/default/files/pubfiles/land-value-capture-policy-brief.pdf)
- **Air rights.** New York's Hudson Yards Infrastructure Corporation issued bonds for the No. 7 extension, repaid by a District Improvement Bonus (paid density), transferable development rights and property taxes. [V] — same source

**Land leasing and development-based capture**
- **Hong Kong's lease system** (premium by auction, 3% annual rent, 19–27% of budget, modification charges) as in KQ1. [V] — [OECD Compendium](https://www.oecd.org/content/dam/oecd/en/publications/reports/2022/06/global-compendium-of-land-value-capture-policies_64f3c257/4f9559ee-en.pdf)
- **Rail plus property (R+P)** [V] — [Suzuki et al. 2015, Overview](https://documents1.worldbank.org/curated/en/468551468165570019/pdf/936860PUB00ISB0TransportDevLV0final.pdf):
  - The government "gives exclusive property development rights of government-owned land at a 'before-rail' market price". MTR partners with developers and sells "at an 'after-rail' market price".
  - 2000–2012: property development 38% of MTR's corporate income, related businesses 28%, transit operations 34%. Farebox recovery was 185.5% in 2012.
  - 1980–2005: the government received a net US$18bn equivalent (US$22.2bn of land premiums, market capitalisation, dividends and IPO proceeds, less US$4.2bn equity injected).
- **Two families of LVC.** The World Bank divides LVC into "development-based" (selling or leasing land and rights) and "tax- or fee-based". Development-based "has a much better chance of working well administratively in places with an inadequate property tax system". [V] — same source

### Inferences
- **Each instrument is a map from a declared object to a payment** [M]:
  - Levy: (cost or ΔV, zone, plot scores) → L_i.
  - Developer obligation: (units, schedule) → charge.
  - Development rights: (ΔFAR, land value or auction) → C.
  - TIF: (B, AV_t, τ, s, T) → R_t.
  - LR: (parcels, plan) → (returned plots, settlements, reserve land).
  - Lease: (premium, rent, term) → cash flows.
- A common interface (the payment schedule over time, who pays, the base, legal caps) makes comparison possible. [M]

### Gaps
- Not read: Canadian (Ontario) development charge methodology; the US special-assessment apportionment rules in a primary text; Mumbai's FSI pricing formula; the Hudson Yards revenue record; Delhi's and Hyderabad's chapters in Suzuki et al.

---

## Key Question 5 (objective b): How value uplift is estimated

### Takeaway
Uplift is estimated by hedonic regression of prices on distance or station zones, by before-and-after comparison, and by quasi-experimental designs (difference-in-differences, with selection correction). Meta-analyses find modest average residential premia near rail (a few percent; about 2.6% per 250 m nearer) and larger commercial premia, with very wide dispersion. Specification matters: omitting highway access biases rail effects upward, and non-parametric "zone" comparisons overstate them.

### Cited Findings

**Debrezion, Pels and Rietveld (working paper)** [V] — [TI 2004-023](https://papers.tinbergen.nl/04023.pdf)
- **Effect per distance.** "The mean impact of a station on property value (rent) for every 250 metres close to the station is 2.61%", ranging from −12.84% to +38.70% (n = 73).
- **Within ¼ mile (Table 5).**

  | Group | Mean | n |
  |---|---|---|
  | Overall | 8.9% | 70 |
  | Residential | 6.2% | 49 |
  | Commercial | 15.2% | 21 |
  | Commuter rail | 19.4% | 15 |
  | Heavy rail/metro | 5.3% | 28 |
  | Light rail | 7.6% | 23 |
  | BRT | 2.5% | 4 |

- **Meta-regression (within ¼ mile, unweighted).** Commuter rail +0.1845 (t = 2.633) relative to light rail; commercial +0.1064 (t = 1.798); R² = 0.13, 73 observations.
- **Specification effects.** "Models with highway accessibility on average report 4% lower railway station proximity effects", insignificant in the ¼-mile specification. Semi-log specifications may give "on average, in a 12% increase". Non-parametric studies "attribute the differences in property value solely to station proximity, so that result may be overstated". No temporal or spatial effect was found.
- **Conclusion.** "When highway accessibility is not explicitly addressed, railway impacts on property values tend to be overestimated."
- **Published version.** The 2007 *JREFE* version (35(2): 161–180) is summarised as a station-zone premium of about 4.2% for the average residence and 16.4% for the average commercial property. [S] — [RePEc](https://econpapers.repec.org/RePEc:kap:jrefec:v:35:y:2007:i:2:p:161-180)
- *Caveat:* the working paper and the published abstract differ in wording on short-distance effects. I read only the working paper.

**Other meta-analyses and case evidence**
- **Mohammad, Graham, Melo and Anderson (2013),** *TR-A* 50: 158–170: 23 studies, 102 observations. Purchase price and rent changes are statistically similar, and there is little effect of property characteristics. Moderators: land use, rail type, system maturity, distance, location. [S, abstract only] — [IDEAS](https://ideas.repec.org/a/eee/transa/v50y2013icp158-170.html). Claims that commuter rail exceeds light rail by about 24–25 points and that Europe and East Asia exceed North America by about 9 points come from an AI-written wiki. [unverified — do not use]
- **Rio de Janeiro (Barra da Tijuca):** tunnels, an expressway and Lúcio Costa's plan were "associated with an increase in land value of 1,900 percent from 1972 to 1975", against about 435% in other high-value areas of the city. A crude before-after comparison with a comparison group. [V] — [Smolka 2013](https://www.lincolninst.edu/sites/default/files/pubfiles/implementing-value-capture-in-latin-america-full_1.pdf)
- **Selection.** Dye and Merriman correct for selection into treatment (TIF adoption) by a first-stage adoption model and the inverse Mills ratio. They warn that growth inside a targeted district says nothing about causation. [V] — [Dye and Merriman 2006](https://www.lincolninst.edu/app/uploads/2024/04/tax-increment-financing-lla060102.pdf)

**Valuing regulatory uplift and practical estimation**
- **Theory versus practice.** In theory the value of rights to FAR 3 versus a baseline of 1 is "the difference between the residual values of their respective highest and best uses". In practice "no two buildings in the area are the same and changes in some plots affect the highest and best use for nearby plots"; higher density is not always more profitable. [V] — [Smolka 2013](https://www.lincolninst.edu/sites/default/files/pubfiles/implementing-value-capture-in-latin-america-full_1.pdf)
- **Consensual increments.** In development-based LVC "the increment in land value is calculated using a method agreed by stakeholder consensus". With taxes or fees "the accuracy of the estimated incremental value is often challenged". [V] — [Suzuki et al. 2015](https://documents1.worldbank.org/curated/en/468551468165570019/pdf/936860PUB00ISB0TransportDevLV0final.pdf)
- **Valuation infrastructure.** Its lack is the main barrier to moving from area-based to value-based land readjustment. [S] — [search summary of Yomralioglu](https://web.itu.edu.tr/tahsin/PAPERBOX/B06.pdf)

### Inferences
- **Uplift must be counterfactual** [M]:
  - ΔV = V(with) − V(without) at the same date.
  - A before-after comparison adds the market trend: ΔV_BA = ΔV + trend.
  - Difference-in-differences subtracts a comparison area's change, but announcement effects (anticipation) shift the "before". Displacement from untreated areas (as in TIF) contaminates the control group.
  - A module should store the estimator, the comparison set, the dates (announcement, construction, opening) and the uncertainty with each uplift estimate.
- **Meta-analytic magnitudes for screening only** [M]:
  - Residential: about 2–3% per 250 m nearer, or about 6% within ¼ mile.
  - Commercial: about 15%.
  - Commuter rail higher.
  - The dispersion (−13% to +39%) means a project-specific estimate is needed.

### Gaps
- Mohammad et al.'s coefficients and later meta-analyses (e.g., the 2022 *TR-A* meta-analysis applying a transit-planning lens) were not read.
- No source on the share of uplift that is anticipation versus realised.
- No primary studies on the uplift from BRT or roads used for levies.

---

## Key Question 6: Empirical capture rates and incidence for each instrument

### Takeaway
Capture rates are rarely measured against a counterfactual uplift; most figures are statutory shares or cost-recovery ratios.
- **Statutory shares:** 30–50% for Colombia's plusvalía and India's betterment levies; 50% of the increment in Gujarat, but administered values push effective capture far lower; Spain's 5–15% of edificabilidad; Germany's area cap of 30%.
- **Estimated or reported:** about 30% of development value captured by developer obligations on English greenfield sites; FHWA's view that owners keep 60–80% of publicly created value; Korea's 100% recovery of public-improvement cost by reserve land.
- **Incidence:** impact fees appear to be borne by landowners (land prices fall about $1 per $1 of fee) while house prices rise. TIF's incidence falls on overlapping jurisdictions and on the untreated parts of the city.
- **Failures documented:** speculation (Korea); under-recovery and delay (Gujarat; over 10 years for most schemes); unsold or under-priced rights (Curitiba); TIF cannibalisation (Illinois). I found no primary quantitative evidence on displacement caused by these instruments.

### Cited Findings

**Capture and recovery rates by instrument**
- **Statutory shares:**
  - Colombia's plusvalía 30–50% of the uplift per m². [V] — [MinVivienda guía](https://www.minvivienda.gov.co/sites/default/files/documentos/guia-para-la-participacion-en-plusvalia.pdf)
  - India's betterment levy 30–50% depending on the state Act. [V] — [OECD Compendium](https://www.oecd.org/content/dam/oecd/en/publications/reports/2022/06/global-compendium-of-land-value-capture-policies_64f3c257/4f9559ee-en.pdf)
  - Gujarat's contribution half the increment. [V] — [Ballaney/UN-Habitat 2013](https://new.unhabitat.org/sites/default/files/download-manager-files/Supply%20of%20Land%20for%20Development-%20Land%20Readjustment%20Experience%20in%20Gujarat%2C%20India%20-%20Urban%20Legal%20Case%20Studies%20Volume%202.pdf)
  - Spain 5–15% (up to 20%) of weighted edificabilidad. [V] — [BOE](https://www.boe.es/buscar/act.php?id=BOE-A-2015-11723)
  - Brazil: Campo Grande 70% of the estimated increment; São Paulo's urban operations originally "around 50 percent". [V] — [Smolka 2013](https://www.lincolninst.edu/sites/default/files/pubfiles/implementing-value-capture-in-latin-america-full_1.pdf)
- **Reported effective capture:**
  - English developer obligations: about 30% of development value on greenfield sites. [V] — [OECD Compendium, UK](https://www.oecd.org/content/dam/oecd/en/publications/reports/2022/06/global-compendium-of-land-value-capture-policies_64f3c257/4f9559ee-en.pdf)
  - Landowners keep 60–80% of publicly created value. [V, FHWA's estimate, method unstated] — [FHWA](https://www.fhwa.dot.gov/ipd/value_capture/resources/value_capture_resources/essential_nexus/tax_increment.aspx)
- **Cost recovery (not value capture):**
  - Korea's LR: 100% of public-improvement cost from reserve land. [V] — [OECD Compendium](https://www.oecd.org/content/dam/oecd/en/publications/reports/2022/06/global-compendium-of-land-value-capture-policies_64f3c257/4f9559ee-en.pdf)
  - Gangnam districts: more than 90% and 99.9% of costs from land sales; earlier Seoul projects relied on 30–50% public assistance. [V] — [UN-Habitat 2019](https://unhabitat.org/sites/default/files/documents/2019-05/lrk_layout_ha_150519_0.pdf)
  - Gujarat: charges of Rs 150–300/m² against Rs 700–1,000/m² true cost (about 15–43% of cost), while land values rose at least tenfold. [V; ratio mine] — [Ballaney/UN-Habitat 2013](https://new.unhabitat.org/sites/default/files/download-manager-files/Supply%20of%20Land%20for%20Development-%20Land%20Readjustment%20Experience%20in%20Gujarat%2C%20India%20-%20Urban%20Legal%20Case%20Studies%20Volume%202.pdf)
  - Hong Kong's MTR: property development financed the network, with a net US$18bn to government in 1980–2005. [V] — [Suzuki et al. 2015](https://documents1.worldbank.org/curated/en/468551468165570019/pdf/936860PUB00ISB0TransportDevLV0final.pdf)
- **Market-priced rights:** São Paulo's 2019 Faria Lima auction cleared at about 2.7× the reference value. Curitiba's cleared at the legal minimum and raised under half the expectation. [V] — [Prefeitura SP](https://prefeitura.sp.gov.br/w/noticia/leilao-de-cepacs-da-operacao-urbana-faria-lima-captou-r-1-64-bi-para-a-cidade-1); [Smolka 2013](https://www.lincolninst.edu/sites/default/files/pubfiles/implementing-value-capture-in-latin-america-full_1.pdf)

**Incidence**
- **Impact fees (Dade County, FL).** Each additional $1 of fees raised the prices of new and existing houses by about $1.60 and lowered the price of undeveloped residential land by about $1.00 (Ihlanfeldt and Shaughnessy 2004, *RSUE* 34(6): 639–661). [S, abstract only] — [RePEc](https://econpapers.repec.org/RePEc:eee:regeco:v:34:y:2004:i:6:p:639-661). Later work reports mixed capitalisation findings. [S] — [Burge working paper](https://www.lincolninst.edu/app/uploads/2024/04/2134_1456_Burge_WP12GB1.pdf)
- **UK developer obligations.** Developers "can shift the costs to landowners". [V] — [OECD Compendium, UK](https://www.oecd.org/content/dam/oecd/en/publications/reports/2022/06/global-compendium-of-land-value-capture-policies_64f3c257/4f9559ee-en.pdf)
- **TIF.** The municipality risks 15 cents per dollar of increment while overlapping jurisdictions risk 85 cents. Growth in the district is "offset by declines elsewhere", and commercial TIF cannibalises commercial growth outside. [V] — [Dye and Merriman 2006](https://www.lincolninst.edu/app/uploads/2024/04/tax-increment-financing-lla060102.pdf)
- **Land readjustment.** Owners bear the contribution in land and keep the residual uplift. In Gujarat they "are happy paying low development charges and letting go of the 35 to 40 per cent", because values rise tenfold, so most of the uplift stays private. [V] — [Ballaney/UN-Habitat 2013](https://new.unhabitat.org/sites/default/files/download-manager-files/Supply%20of%20Land%20for%20Development-%20Land%20Readjustment%20Experience%20in%20Gujarat%2C%20India%20-%20Urban%20Legal%20Case%20Studies%20Volume%202.pdf)

**Failures**
- **Speculation:** Korean LR plots "were speculated upon and commanded high prices", and owners may have accepted higher deductions for speculative gains. [V] — [UN-Habitat 2019](https://unhabitat.org/sites/default/files/documents/2019-05/lrk_layout_ha_150519_0.pdf)
- **Delay:** most finalised Gujarat schemes took over 10 years (see the inconsistency noted in KQ2). [V] — [Ballaney/UN-Habitat 2013](https://new.unhabitat.org/sites/default/files/download-manager-files/Supply%20of%20Land%20for%20Development-%20Land%20Readjustment%20Experience%20in%20Gujarat%2C%20India%20-%20Urban%20Legal%20Case%20Studies%20Volume%202.pdf)
- **Hold-outs and consent:** consent thresholds range from unanimity to none, and Portugal expropriates if consent fails. Owner resistance is the most common obstacle. [V] — [OECD Compendium](https://www.oecd.org/content/dam/oecd/en/publications/reports/2022/06/global-compendium-of-land-value-capture-policies_64f3c257/4f9559ee-en.pdf)
- **Unsold rights:** about 360,000 Água Espraiada CEPACs remained unoffered in 2013. São Paulo froze a further 500,000 Faria Lima CEPACs because the area was "already too congested". [V] — [Smolka 2013](https://www.lincolninst.edu/sites/default/files/pubfiles/implementing-value-capture-in-latin-america-full_1.pdf)
- **Lease renewals:** Hong Kong's leaseholders refused substantial renewal charges. [V] — [OECD Compendium](https://www.oecd.org/content/dam/oecd/en/publications/reports/2022/06/global-compendium-of-land-value-capture-policies_64f3c257/4f9559ee-en.pdf)
- **Displacement:** the World Bank lists affordable housing "to alleviate shortages and offset potential gentrification" as a use of LVC proceeds, a recognition of the risk, not evidence of its size. [V] — [Suzuki et al. 2015](https://documents1.worldbank.org/curated/en/468551468165570019/pdf/936860PUB00ISB0TransportDevLV0final.pdf)

### Inferences
- Three different ratios are reported under the name "capture rate", and a module must keep them apart [M]:
  - κ = captured / counterfactual uplift (true capture);
  - ρ = revenue / project cost (cost recovery);
  - σ = statutory share × (valuation basis / market value) (effective statutory capture).
- **Gujarat** shows σ much lower than the statutory 50%. **Korea** shows ρ = 1 with κ unknown. **CEPAC auctions** price the rights at market (high κ on the regulatory uplift of the rights sold, but none on existing floor space).
- **Incidence differs from the statutory payer.** Fees levied on developers fall largely on landowners (land prices) and, in Dade County, partly on buyers. TIF falls on overlapping jurisdictions. A module should hold an incidence split as a declared or estimated parameter. [M]

### Gaps
- No systematic cross-instrument study of κ against counterfactual uplift was found. Not read: Medda (2012), Mathur, Crook–Henneberry–Whitehead.
- No primary quantitative evidence on displacement or gentrification caused by LVC instruments specifically.
- The England £7bn figure was not checked against the MHCLG report itself.

---

## What this means for a computational infrastructure

### Takeaway
A module that compares instruments for a declared project should hold:
- one declared project (its catchment, schedule, cost and the plan's change of rights);
- one uplift estimate with its counterfactual and uncertainty;
- one cash-flow object per instrument.

Each cash-flow object carries who pays, the base, legal caps, timing and incidence. Comparisons should be made on present values, cost recovery, capture of counterfactual uplift, and distribution. The figures above give parameter ranges and checkable identities. Japanese cases are used only for the formulas they illustrate, never as recommended cases.

### Quantities a module should hold (with units and sources of their ranges)
- **Uplift field.** V_i(with, t) and V_i(without, t) per parcel in currency at a stated date, decomposed into infrastructure uplift and regulatory uplift (ΔFAR, change of use, rural to urban).
  - Store the estimator (hedonic, DiD, before-after), the comparison set, the dates (announcement, construction, opening) and an interval.
  - Prior ranges for screening: residential ≈ +2.6% per 250 m nearer a station, or ≈ +6% within ¼ mile; commercial ≈ +15%; commuter rail higher. Dispersion −13% to +39%.
  - Sources: [Debrezion et al.](https://papers.tinbergen.nl/04023.pdf) [V]
- **Infrastructure levy.** Total L = min(cost·(1 + a), λ·ΣΔV_i) with a ≤ 30% (Colombia) and λ the declared share. Apportionment L_i = L·s_i/Σs over a catchment with a declared score s_i (area, frontage, distance, value). Only 23% of countries base the total on value. [V for parameters; formula M]
- **Developer obligations.** A rate per unit (m², dwelling) or negotiated, levied at permission, with an incidence split to land and house prices (Dade County: −1.0 and +1.6 per unit of fee [S]).
- **Charges for development rights:**
  - Formula: C = (ΔA / FAR_basic) · p_land · k, the virtual-land method, with k a policy factor (0.5–0.7 in Brazilian cities). [V Smolka; formula M]
  - Auction: CEPACs with zone conversion factors f_z ∈ [0.5, 2] m² per certificate. The stock identities are authorised = sold + unissued and sold = used + in circulation.
- **TIF:**
  - B, AV_t, the designated share s, the overlapping rates τ_j, the term T (15–30 years), the discount rate r and the but-for share β.
  - R_t = s·Στ_j·max(0, AV_t − B).
  - Diversion D_{j,t} = τ_j(1 − β)I_t.
  - Displacement −τ·ΔG elsewhere in the city.
- **Land readjustment:**
  - Area identity M + A = N + E + R, with P = N − M and d = (P + R)/A.
  - Unit values a, e, so y = e/a and Pr = (1 − d)·y.
  - Per-parcel entry values V_i and allocated values V′_i, entitlement V*_i = V_i·ΣV′/ΣV and settlement s_i = V′_i − V*_i.
  - Statutory caps: the 30%/10% area deduction (Germany); 50% total appropriation with a 15/5/5/15 split plus 10% for weaker sections (Gujarat); the 50/30/20 split (Korea's 1966 Act); 5–15% (≤ 20%) cession of edificabilidad (Spain); the 45% cap (Chinese Taipei, [M]).
  - Consent threshold; duration T (Gujarat: mostly over 10 years); reserve-land sale value net of selling costs; subsidies.
- **Leasing:** an auctioned premium (PV of rents over the term), annual rent (3% of rateable value in Hong Kong), a modification premium equal to the value gain from changed rights, and renewal.
- **Valuation basis** per instrument (market, mass appraisal, administered schedule such as jantri), with its ratio to market.

### Accounting identities and invariants to check
1. **LR area conservation:** M + A = N + E + R, holding exactly in m². (Misato Chuo, Myeonmok: recompute and flag mismatches like the 2,199 m² in the Korean table.)
2. **LR value preservation:** V*_i ≥ V_i for all i (or ΣV′ ≥ ΣV). Σ s_i = 0 when the distributable value is fully allocated. Statutory form: the contribution's value is at most the increase (LRL art. 96.2, BauGB §57–58, Spain's "burden ≤ benefit").
3. **LR feasibility:** Σ_t (reserve-land sales + subsidies + levies − costs)_t / (1+r)^t ≥ 0.
4. **TIF value accounting:** the before base B + N; after B + N + I + G; the local base B + N + G; the district base I. Revenue uses the combined rate, so overlapping jurisdictions' shares must sum to the district's revenue.
5. **CEPAC stock:** authorised = issued + unissued; issued = consumed in licences + in circulation (Água Espraiada checks).
6. **No double charging of one uplift:** Σ over instruments of capture on parcel i ≤ ΔV_i. Example: Colombia's art. 87 bars plusvalía from works financed by valorización. Stacking a betterment levy, a rights charge and TIF on one increment must be checked.
7. **Present values in one currency and price level,** with the valuation date of each statute (the Umlegungsbeschluss date in Germany).

### Comparisons across instruments a module should report
- PV of public revenue and its timing (upfront for rights auctions, premiums and LR land sales; spread for levies and TIF).
- Cost recovery ρ = PV revenue / PV cost.
- True capture κ = PV captured / PV counterfactual uplift. Effective statutory capture σ.
- Incidence by party: original owners, developers, buyers or tenants, overlapping jurisdictions, and owners outside the catchment (TIF displacement).
- Risk: dependence on market cycles (OECD notes revenue instability in boom–bust cycles), auction clearing (Curitiba), the β and ΔG assumptions for TIF, consent and hold-out risk for LR, and litigation frequency (highest for levies and LR).
- Administrative requirements: cadastre quality, market-based valuation, legal basis. The Compendium's obstacle frequencies are usable as qualitative priors.

### Pitfalls a module must guard against
- **Before-after mistaken for uplift.** It conflates market trend and anticipation with the project's effect. Require a declared counterfactual.
- **Treating β = 1 in TIF.** Dye and Merriman find no net citywide gain and commercial cannibalisation.
- **Statutory rate mistaken for capture.** Gujarat's 50% of an increment computed from administered values and costs recovered perhaps 15–43% of cost, while land rose tenfold.
- **Cost-based levies presented as value capture.** Only 23% of countries base levy totals on value increments; most base them on cost (multiple responses were allowed, so the complement is not exactly "cost").
- **Ignoring incidence.** Fees on developers are capitalised into land and house prices.
- **Uniform meta-analytic premia** applied to a specific project despite enormous dispersion and specification bias (omitted road access, non-parametric zone comparisons).
- **Discounting and duration.** LR schemes taking 10–20 years change feasibility. Report the internal timeline.
- **Unsold or under-priced rights.** Model the auction's demand curve or a minimum price, not a fixed sale of all certificates.
- **Name collisions across jurisdictions.** "Infrastructure levy" in Israel and Poland means developer obligations. Declare instruments by mechanism, as the OECD-Lincoln taxonomy does, not by local name.
- **Place names and cases.** Follow the naming policy (Chinese Taipei for the jurisdiction). Do not use Japanese cities as recommended cases; the Japanese LR formulas are general and may be used.
