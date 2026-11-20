# Part D. Projects as Evolving Designs: Software Engineering, Organisation Science and the Commons

## 1. Scope and Method

This part surveys what software engineering, organisation science and the study of commons report about projects as evolving systems. It is written to assess six design ideas:

1. A project is an evolving system coupled to an environment that it also changes; its worth is decided outside it.
2. A project has a compact generative core (architecture, interfaces, conventions) and a bulky body; the core is conserved, the body varies.
3. An immortal body accumulates damage; periodic rebuilding from the core rejuvenates it.
4. Materials of different kinds check one another; agreement between independently produced forms is evidence.
5. Evaluation can be distributed (stake, refute, adoption as the measure of worth); scrutiny should grow with reliance.
6. Most of what the framework needs already exists in open-source practice.

**Date of search:** 20-21 November 2026. **Sources used:** Crossref and Semantic Scholar record pages (bibliographic metadata), arXiv abstract pages, and web search result pages that quote publisher, repository or library records.

**Marks.**

- [V]: a record was opened (Crossref, Semantic Scholar or arXiv) and authors, title, year and venue match what is written here.
- [P]: the work was confirmed only through search results quoting a secondary or institutional record, or a field is unconfirmed (stated in the entry). Descriptions of [P] works rest on those snippets.
- [U]: not opened. None is used.

**Limits of the method.** The record pages for older IEEE and ACM papers carry no abstract. For those [V] entries the sentence on content states only what the title and the cited secondary descriptions support, and says so ("abstract not on the record"). Empirical figures are given only where a page or search result displayed them, with their context. Pages ranges for books were not confirmed. No project or personal names were used in queries.

## 2. Annotated Works

### 2.1 Laws of software evolution and their tests

**1. [V]** Lehman, M. M. (1980). Programs, life cycles, and laws of software evolution. *Proceedings of the IEEE* 68(9), 1060-1076. DOI 10.1109/PROC.1980.11805.
The Crossref record confirms the bibliographic data; the abstract is not on the record. The paper is the standard source for the classification of programs and for the "laws" of continuing change and increasing complexity. *Bearing:* supports idea 1 (an E-type program is coupled to an environment it changes, and must keep changing) and idea 3 (complexity grows unless work is done against it). The laws are empirical generalisations from few systems, not theorems.

**2. [V]** Belady, L. A., and Lehman, M. M. (1976). A model of large program development. *IBM Systems Journal* 15(3), 225-252. DOI 10.1147/sj.153.0225.
The record confirms the data; no abstract. It is the earlier model from which Lehman's laws were drawn (OS/360 release data, as commonly cited; the content was not read here). *Bearing:* supports idea 3 as the original statement of entropy-like growth in maintained programs; weak as evidence, since it rests on one proprietary system.

**3. [P]** Israeli, A., and Feitelson, D. G. (2010). The Linux kernel as a case study in software evolution. *Journal of Systems and Software* 83(3), 485-501. Record: https://cris.huji.ac.il/en/publications/the-linux-kernel-as-a-case-study-in-software-evolution/
Search results report that the study uses 810 kernel versions released over 14 years to test Lehman's laws, and that average function complexity falls over time, mainly because many small functions are added. (A DOI guessed from memory did not resolve to this paper; none is given.) *Bearing:* a limit on idea 3: in a long-lived open-source body, complexity did not rise monotonically by this measure.

**4. [V]** Herraiz, I., Rodriguez, D., Robles, G., and Gonzalez-Barahona, J. M. (2013). The evolution of the laws of software evolution: a discussion based on a systematic literature review. *ACM Computing Surveys* 46(2), article 28. DOI 10.1145/2543581.2543595.
The record's abstract (via Semantic Scholar) says the validity of the laws is still disputed, that most controversial studies came after the last revision of the laws, and that the authors assess the laws' current validity and how the community views them. *Bearing:* warns against citing the laws as established; they support idea 1 qualitatively and idea 3 only conditionally.

### 2.2 Modularity, design rules and generative cores

**5. [P]** Simon, H. A. (1962). The architecture of complexity. *Proceedings of the American Philosophical Society* 106(6), 467-482. JSTOR 985254.
Search results confirm the citation and report the argument: complex systems that evolve are hierarchical and nearly decomposable, interactions within subsystems being stronger than between them; the Hora and Tempus parable shows that stable sub-assemblies make evolution faster. A critical reading (Agre) argues that Simon's notion of hierarchy is static. *Bearing:* supports idea 2 (a conserved structure with varying parts) and idea 3 (rebuild from stable sub-assemblies, not from nothing). Limit: near-decomposability is an assumption to be checked in each project.

**6. [V]** Parnas, D. L. (1972). On the criteria to be used in decomposing systems into modules. *Communications of the ACM* 15(12), 1053-1058. DOI 10.1145/361598.361623.
The record's abstract states that modularisation can make a system more flexible, easier to understand and quicker to develop, that the outcome depends on the criteria used, and that the unconventional decomposition compared has advantages. (The "information hiding" criterion is the paper's well-known content; the abstract as read does not use that phrase.) *Bearing:* supports idea 2: the compact core is the set of decisions that module interfaces hide the rest from.

**7. [P]** Baldwin, C. Y., and Clark, K. B. (2000). *Design Rules, Volume 1: The Power of Modularity*. MIT Press. Catalogue: https://ideas.repec.org/b/mtp/titles/0262024667.html
Search results (including the authors' companion papers) report that modular designs create options; a module's net option value depends on technical potential, the cost of independent design experiments and the module's visibility; design rules are the visible information that lets hidden modules vary. *Bearing:* the closest published theory for idea 2 and for the idea that variation in the body is valuable when the core is fixed. Limit: the option-value valuation is theoretical, and a later retrospective by the authors records that gaps limited the adoption of its tools.

**8. [V]** Sullivan, K. J., Griswold, W. G., Cai, Y., and Hallen, B. (2001). The structure and value of modularity in software design. *Proc. ESEC/FSE-9*, 99-108. DOI 10.1145/503209.503224.
Semantic Scholar confirms title, authors and year; search results report that the authors apply design-structure matrices and real-options valuation to Parnas's KWIC designs and find the results consistent with Parnas's conclusions. *Bearing:* supports idea 2 as a test of Baldwin and Clark's theory on software, on a small textbook case only.

**9. [P]** Alexander, C., Ishikawa, S., and Silverstein, M. (1977). *A Pattern Language: Towns, Buildings, Construction*. Oxford University Press. Catalogue: https://search.worldcat.org/oclc/3132495. Related: Alexander, C. (1964), *Notes on the Synthesis of Form*, Harvard University Press; (1979), *The Timeless Way of Building*, Oxford University Press.
Search results confirm the publication data and describe the book as 253 patterns forming a language, which sources characterise as a generative system. The 1964 book defines design as inventing things that display new physical order in response to function. *Bearing:* a model of a generative core as a small vocabulary plus rules of combination (idea 2). Limit: the claim that patterns generate good wholes is a design philosophy, not an empirically tested result; the entries rest on catalogue and encyclopaedic descriptions.

**10. [V]** MacCormack, A., Rusnak, J., and Baldwin, C. Y. (2006). Exploring the structure of complex software designs: an empirical study of open source and proprietary code. *Management Science* 52(7), 1015-1030. DOI 10.1287/mnsc.1060.0552.
The record confirms the data; the abstract is elided there. Search results report that dependency matrices are used to compare designs and that the results are exploratory but suggest that modes of organisation go with differently structured designs; a related paper tests the mirroring hypothesis on matched pairs. *Bearing:* supports the coupling of organisation and architecture (ideas 1, 2) with a caution that the evidence is exploratory.

**11. [V]** Colfer, L. J., and Baldwin, C. Y. (2016). The mirroring hypothesis: theory, evidence, and exceptions. *Industrial and Corporate Change* 25(5), 709-738. DOI 10.1093/icc/dtw027.
The record confirms authors, title and year (the venue field was blank in the Semantic Scholar record; the DOI prefix is that of the journal; volume and pages are from memory and unconfirmed). Abstract not on the record; the title states that the paper reviews evidence and exceptions. *Bearing:* a limit on any claim that structure follows organisation: exceptions exist, so a framework should treat mirroring as a tendency.

**12. [P]** Conway, M. E. (1968). How do committees invent? *Datamation* 14(4), 28-31. Author's page: http://www.melconway.com
Search results confirm the citation and the thesis that systems designed by organisations copy their communication structures, and note that the paper was rejected by a business journal for not having proved its thesis. *Bearing:* supports idea 1 (the project's design reflects the organisation that made it); for a population of agents it suggests that agent roles shape architecture. It is an argument, not a test.

### 2.3 Decay, debt and renewal

**13. [V]** Parnas, D. L. (1994). Software aging. *Proc. 16th International Conference on Software Engineering*, 279-287. DOI 10.1109/ICSE.1994.296790.
The record confirms author, title, year and venue; abstract not on the record (pages from memory, unconfirmed). The paper is the standard source for the claim that software ages through failure to keep up with change and through changes that degrade structure. *Bearing:* supports idea 3. Whether the paper recommends rebuilding was not read here; do not attribute that to it.

**14. [V]** Cunningham, W. (1992). The WyCash portfolio management system. *OOPSLA '92 Experience Report*. DOI 10.1145/157709.157715.
The record confirms the data; no abstract. It is the origin of the "debt" metaphor (the metaphor's content is from the paper's widespread citation, not read here). *Bearing:* supports idea 3 and the notion that debt is paid down by rewriting toward understanding.

**15. [V]** Kruchten, P., Nord, R., and Ozkaya, I. (2012). Technical debt: from metaphor to theory and practice. *IEEE Software* 29(6), 18-21. DOI 10.1109/MS.2012.167.
The record confirms title, authors and year; no abstract. The title states the paper's aim of putting the metaphor on a theoretical footing. *Bearing:* supports idea 3; shows that debt is a contested concept, not a measured quantity.

**16. [V]** Li, Z., Avgeriou, P., and Liang, P. (2015). A systematic mapping study on technical debt and its management. *Journal of Systems and Software* 101, 193-220. DOI 10.1016/j.jss.2014.12.027.
Record confirmed; no abstract. A systematic mapping of the technical-debt literature. *Bearing:* a source of the range of debt types (code, architecture, documentation, test); supports idea 3 as the debt accumulates in more than code.

**17. [P]** de Silva, L., and Balasubramaniam, D. (2012). Controlling software architecture erosion: a survey. *Journal of Systems and Software* 85(1), 132-151. Record: https://research-portal.st-andrews.ac.uk/en/publications/controlling-software-architecture-erosion-a-survey/
Search results report that the survey covers techniques to prevent erosion or to detect and restore eroded architectures, argues that no single strategy suffices and proposes a holistic framework; later papers say industry adoption of the academic methods was limited. *Bearing:* supports idea 3 (erosion is real) and the coexistence of prevention and restoration in idea 3; warns that restoration tools are little used.

**18. [P]** Huang, Y., Kintala, C., Kolettis, N., and Fulton, N. D. (1995). Software rejuvenation: analysis, module and applications. *Proc. FTCS-25*, 381-390. Record via search results: https://scholars.duke.edu/publication/802829
The reported abstract defines rejuvenation as gracefully terminating an application and restarting it in a clean internal state, with an analytic model and a reusable module, applied to a billing data subsystem. *Bearing:* the closest engineering precedent for idea 3's periodic rebuilding, but it concerns runtime state accumulation, not source-level erosion. Do not conflate the two.

**19. [P]** Aghajani, E., Nagy, C., Vega-Marquez, O. L., Linares-Vasquez, M., Moreno, L., Bavota, G., and Lanza, M. (2019). Software documentation issues unveiled. *Proc. ICSE 2019*, 1199-1210. DOI 10.1109/ICSE.2019.00122 (from search results).
The study examined 878 artefacts (emails, forum posts, issues and pull requests) and produced a taxonomy of documentation issues in four groups: information content, presentation, process and tools. *Bearing:* supports idea 3 (documentation rots, and rot has identifiable kinds) and idea 2 (what matters is whether documentation carries content that is correct and complete).

**20. [P]** Zhang, H., Wang, S., Chen, T.-H., Zou, Y., and Hassan, A. E. (2021). An empirical study of obsolete answers on Stack Overflow. *IEEE Transactions on Software Engineering* 47(4), 850-862. arXiv:1903.12282; DOI 10.1109/TSE.2019.2906315.
Search results report that, in 52,177 answer threads, 58.4% of obsolete answers were probably already obsolete when posted and only 20.5% of obsolete answers were ever updated. *Bearing:* a measured instance of a knowledge base that decays without a renewal process (idea 3); also shows that a crowd does not repair obsolescence by itself.

**21. [P]** Avelino, G., Passos, L., Hora, A., and Valente, M. T. (2016). A novel approach for estimating truck factors. *Proc. ICPC 2016*. arXiv:1604.06766; DOI 10.1109/ICPC.2016.7503718.
Search results report an automated estimator run on 133 popular GitHub projects, with 65% found to have a truck factor of at most 2, and positive or partly positive developer responses to the estimates in 53% of valid survey answers. *Bearing:* supports idea 3 (knowledge concentrates in few contributors); for a population of agents the corresponding risk is concentration of understanding in one agent or one model.

### 2.4 Mutual checking of forms

**22. [V]** Meyer, B. (1992). Applying "design by contract". *Computer* 25(10), 40-51. DOI 10.1109/2.161279.
The record confirms author, title, year and venue (volume and pages from memory, unconfirmed); no abstract. The paper presents specifications as executable assertions in the code. *Bearing:* supports idea 4 (a specification form checked against an implementation form); limit: the two forms are produced by the same author, so agreement is weak evidence of independence.

**23. [V]** Claessen, K., and Hughes, J. (2000). QuickCheck: a lightweight tool for random testing of Haskell programs. *Proc. ICFP '00*, 268-279. DOI 10.1145/351240.351266.
The record confirms the data; no abstract. The tool checks stated properties against randomly generated inputs. *Bearing:* supports idea 4: a property is a compact specification that tests the body continually.

**24. [V]** DeMillo, R. A., Lipton, R. J., and Sayward, F. G. (1978). Hints on test data selection: help for the practicing programmer. *Computer* 11(4), 34-41. DOI 10.1109/C-M.1978.218136.
The record confirms authors, title, year and venue (volume and pages from memory); no abstract. The paper introduced mutation as a test-adequacy idea (from its citation by Jia and Harman, not read here). *Bearing:* supports idea 4: a check is credible if it can catch deliberately injected faults.

**25. [V]** Jia, Y., and Harman, M. (2011). An analysis and survey of the development of mutation testing. *IEEE Transactions on Software Engineering* 37(5), 649-678. DOI 10.1109/TSE.2010.62.
The record confirms the data; the abstract is absent but the record lists 263 references. *Bearing:* the survey base for mutation analysis as a measure of how well an evaluator can discriminate (idea 4, and idea 5 for the quality of refuters).

**26. [V]** Inozemtseva, L., and Holmes, R. (2014). Coverage is not strongly correlated with test suite effectiveness. *Proc. ICSE 2014*, 435-445. DOI 10.1145/2568225.2568271.
The record confirms the data; the title states the finding. (Pages from memory.) *Bearing:* a warning for idea 4: a visible agreement signal (coverage) does not measure the ability to detect faults. A framework must not treat a pass as evidence unless the check could have failed.

**27. [V]** Knight, J. C., and Leveson, N. G. (1986). An experimental evaluation of the assumption of independence in multiversion programming. *IEEE Transactions on Software Engineering* SE-12(1), 96-109. DOI 10.1109/TSE.1986.6312924.
The record confirms the data. Search results report 27 versions written from one specification at two universities and run on one million random tests; the number of inputs on which more than one version failed was substantially higher than independence would predict. *Bearing:* the key warning for idea 4: independently produced forms share failures where the specification is hard. Agreement is evidence only to the degree that errors are independent.

**28. [P]** Eckhardt, D. E., and Lee, L. D. (1985). A theoretical basis for the analysis of multiversion software subject to coincident errors. *IEEE Transactions on Software Engineering* SE-11(12), 1511-1517. DOI 10.1109/TSE.1985.231895 (from a library catalogue record).
Search results report a probabilistic model with an "intensity of coincident errors" and a condition under which multiple versions beat one. *Bearing:* the theory behind entry 27: variation in difficulty across inputs makes coincident failure unavoidable even with independent development.

**29. [V]** Ron, J., Baudry, B., and Monperrus, M. (2026). N-version programming with coding agents. arXiv:2606.20158 (cs.SE), submitted 18 June 2026.
The abstract page states that 48 agent-generated implementations of one specification were tested on 1,000,000 random inputs; failures were highly correlated, with many co-occurring failures at hard or ambiguous parts of the specification; majority voting over three-version units reduced the mean failure count from 387.44 (single versions) to 130.99 (triples); 11,844 units showed no observed failure. A preprint, not peer reviewed as read here. *Bearing:* the most direct evidence for idea 4 with agents: diversity helps but is far from independent, and ambiguity in the specification is the shared failure point.

**30. [P]** McKeeman, W. M. (1998). Differential testing for software. *Digital Technical Journal* 10(1), 100-107.
Confirmed only as a citation in secondary sources (no record opened). Differential testing compares the outputs of several implementations of one specification on the same input. *Bearing:* supports idea 4; shares the limit of entries 27 and 29.

**31. [P]** Wimsatt, W. C. (1981). Robustness, reliability, and overdetermination. In Brewer, M. B., and Collins, B. E. (eds.), *Scientific Inquiry and the Social Sciences*, Jossey-Bass, 124-163 (page ranges vary between citations). Reprint: https://rd.springer.com/chapter/10.1007/978-94-007-2759-5_2
Search results report that the chapter's theme is triangulation by multiple means of determination, with reliability models in which parallel redundancy raises and serial dependency lowers reliability, provided probabilities lie between zero and one. *Bearing:* the philosophical base of idea 4; its condition (independent means) is the one entries 27 to 29 find hard to meet.

### 2.5 Open source as commons and ecology

**32. [P]** Raymond, E. S. (1999). *The Cathedral and the Bazaar*. O'Reilly; essay text: https://catb.org/~esr/writings/cathedral-bazaar/cathedral-bazaar/index.html
Search results report the origin of "given enough eyeballs, all bugs are shallow" (named Linus's law); a Microsoft security commentary notes that the original lesson does not refer to visual inspection only; Heartbleed is cited as a counter-example; and a summary source states that large-scale tests of the claim had not been performed (as of that source). *Bearing:* the folk basis of idea 5 and idea 6. Limit: the claim is an observation from one project.

**33. [P]** Benkler, Y. (2002). Coase's penguin, or, Linux and the nature of the firm. *Yale Law Journal* 112(3), 369-446. https://yalelawjournal.org/article/coases-penguin-or-linux-and-the-nature-of-the-firm
Search results report the argument that free software is one case of commons-based peer production, a third mode beside firms and markets, with gains from allocating many contributors to many tasks. *Bearing:* supports idea 5 (distributed evaluation and allocation) and idea 6. It is a theoretical argument with examples.

**34. [P]** Ostrom, E. (1990). *Governing the Commons: The Evolution of Institutions for Collective Action*. Cambridge University Press. Design principles as tabulated in https://pmc.ncbi.nlm.nih.gov/articles/PMC11320366/table/T1
Search results list eight principles: clear boundaries, congruence of rules with local conditions, collective-choice arrangements, monitoring, graduated sanctions, conflict resolution, minimal recognition of rights to organise, and nested enterprises. A meta-analysis of 112 studies (Cox, Arnold and Villamayor-Tomas, 2010) is reported to find the principles empirically supported in a probabilistic sense, with ten principles in its version. *Bearing:* supports idea 5, especially graduated sanctions and monitoring, and "scrutiny grows with reliance" as nested enterprise. Limit: the principles come from physical commons with scarcity and rivalry; code and text are non-rival.

**35. [V]** Mockus, A., Fielding, R. T., and Herbsleb, J. D. (2002). Two case studies of open source software development: Apache and Mozilla. *ACM Transactions on Software Engineering and Methodology* 11(3), 309-346. DOI 10.1145/567793.567795.
The record confirms the data (no abstract there). Search results report that the study measures participation, core-team size, code ownership, productivity, defect density and problem resolution intervals from mailing lists, change history and problem reports, and concludes that hybrids of open-source and commercial techniques may yield high-performance processes. *Bearing:* supports idea 6 (open-source practice already contains the institutions) and notes a small core carrying much of the work, as in entry 21.

**36. [P]** Robles, G., Merelo-Guervos, J. J., and Gonzalez-Barahona, J. M. (2005). Self-organized development in libre software projects: a model based on the stigmergy concept. *Proc. ProSim 2005*. Confirmed only as a reference in another paper's list; the exact title wording is uncertain.
Stigmergic coordination through the shared artefact is the model. Related work on stigmergy in open source (Crowston and colleagues; Bolici and Howison) was located but not opened. *Bearing:* supports idea 6: coordination through the artefact (issues, branches, tests) needs no central plan, which suits a population of agents.

**37. [P]** Mens, T., Claes, M., Grosjean, P., and Serebrenik, A. (2014). Studying evolving software ecosystems based on ecological models. In Mens, T., Serebrenik, A., and Cleve, A. (eds.), *Evolving Software Systems*, Springer, 297-326. DOI 10.1007/978-3-642-45398-4_10 (from search results).
Search results describe the chapter as comparing natural and software ecosystems, applying ecological models, with predator-prey dynamics adapted to software evolution, as part of an interdisciplinary project on ecological studies of open-source ecosystems. *Bearing:* the closest published source for the ecological framing of the whole programme; it argues from analogy, so the analogy's limits (entry 41, 42) apply.

**38. [V]** Decan, A., Mens, T., and Grosjean, P. (2019). An empirical comparison of dependency network evolution in seven software packaging ecosystems. *Empirical Software Engineering* 24, 381-416. DOI 10.1007/s10664-017-9589-y.
The Semantic Scholar record summary (abstract paraphrased; volume and pages from memory) reports libraries.io data for Cargo, CPAN, CRAN, npm, NuGet, Packagist and RubyGems; metrics of growth, changeability, reusability and fragility; networks that grow over time; a small share of packages carrying most reverse dependencies; and many fragile packages tied to rising transitive dependencies. *Bearing:* supports idea 5's "adoption is dependence" (dependence is concentrated), and warns that high dependence is also fragility (idea 5, scrutiny should follow reliance).

**39. [P]** Zimmermann, M., Staicu, C.-A., Tenny, C., and Pradel, M. (2019). Small world with high risks: a study of security threats in the npm ecosystem. *Proc. 28th USENIX Security Symposium*, 995-1010. arXiv:1902.09217.
Search results report that up to 40% of npm packages depend on code with at least one publicly known vulnerability, and that a very small number of compromised maintainer accounts could inject malware into the majority of packages. *Bearing:* strong support for "scrutiny should grow with reliance" (idea 5) and a warning that adoption concentrates risk.

**40. [P]** Kula, R. G., German, D. M., Ouni, A., Ishio, T., and Inoue, K. (2018). Do developers update their library dependencies? An empirical study on the impact of security advisories on library migration. *Empirical Software Engineering* 23(1), 384-417. DOI 10.1007/s10664-017-9521-5 (confirmed via dblp and library records quoted in search results; not opened as a primary record).
Search results report more than 4,600 GitHub projects and 2,700 dependencies; about 81.5% of systems kept outdated dependencies; 69% of surveyed developers said they did not know their dependencies were vulnerable. *Bearing:* a limit on idea 5: adopters seldom re-examine what they adopted, so adoption is a lagging and weakly informed signal of current worth.

### 2.6 Incentives, scrutiny and signals

**41. [V]** Maillart, T., Zhao, M., Grossklags, J., and Chuang, J. (2017). Given enough eyeballs, all bugs are shallow? Revisiting Eric Raymond with bug bounty programs. *Journal of Cybersecurity* 3(2), 81-90. DOI 10.1093/cybsec/tyx008.
The Crossref record confirms the data; the abstract is absent there. Search results (with an arXiv version, 1608.03445) report that, per programme, each researcher can expect to find only a bounded number of bugs, so discovery is driven by adding researchers. *Bearing:* supports idea 5 (paying refuters yields findings) and shows diminishing returns per person; it concerns security bugs on bounty platforms.

**42. [P]** Finifter, M., Akhawe, D., and Wagner, D. (2013). An empirical study of vulnerability rewards programs. *Proc. 22nd USENIX Security Symposium*, 273-288. https://www.usenix.org/conference/usenixsecurity13/technical-sessions/presentation/finifter
Search results report a study of the Chrome and Firefox programmes over about three years; both appear economically efficient compared with hiring full-time researchers; most recipients received a single reward. *Bearing:* supports idea 5 on cost; the finding is for two large well-resourced programmes.

**43. [V]** Meneely, A., and Williams, L. (2009). Secure open source collaboration: an empirical study of Linus' law. *Proc. 16th ACM CCS*, 453-462. DOI 10.1145/1653662.1653717.
The record confirms the data; abstract absent. Search results report a case study of a Linux distribution's kernel correlating vulnerabilities with developer-activity metrics, testing both "many eyes" and "too many cooks". (Specific numerical results were not retrieved.) *Bearing:* limit on idea 5: more contributors can raise risk as well as scrutiny.

**44. [V]** Sadowski, C., Soederberg, E., Church, L., Sipko, M., and Bacchelli, A. (2018). Modern code review: a case study at Google. *Proc. ICSE-SEIP 2018*. DOI 10.1145/3183519.3183525.
The record summary reports 12 interviews, a survey of 44 respondents and an analysis of review logs covering 9 million reviewed changes, on motivations, practice and satisfaction. *Bearing:* supports idea 6 (review is mature practice at scale); it studies one company with a lightweight in-house process.

**45. [V]** Bacchelli, A., and Bird, C. (2013). Expectations, outcomes, and challenges of modern code review. *Proc. ICSE 2013*, 712-721. DOI 10.1109/ICSE.2013.6606617.
The record confirms the data; abstract absent. The title states the study's three questions (pages from memory). *Bearing:* idea 6; review serves knowledge transfer as well as defect finding, so refutation is one of several purposes.

**46. [P]** He, H., Yang, H., Burckhardt, P., Kapravelos, A., Vasilescu, B., and Kaestner, C. (2024). 4.5 million (suspected) fake stars in GitHub: a growing spiral of popularity contests, scams, and malware. arXiv:2412.13459.
Search results report a detector (StarScout); in the first pass about 4.53 million suspected fake stars across 22,915 repositories; after cleaning, 3.1 million from 278,000 accounts over 15,835 repositories; a rise by two orders of magnitude in 2024; most fake stars promote malware; after two months, fake stars tend to have a negative effect on real star gains. Version 2 is retitled with 6.0 million. *Bearing:* a direct warning for idea 5: any adoption measure that confers reward will be gamed; weight evidence that is costly to fake.

**47. [P]** Raman, N., Cao, M., Tsvetkov, Y., Kaestner, C., and Vasilescu, B. (2020). Stress and burnout in open source: toward finding, understanding, and mitigating unhealthy interactions. *ICSE 2020, New Ideas and Emerging Results*. https://2020.icse-conferences.org/details/icse-2020-New-Ideas-and-Emerging-Results/13/
Search results report a classifier for toxic issue discussions, differing toxicity across communities, and an agenda for burnout research. *Bearing:* idea 6 and idea 5: load on human maintainers is a cost of scrutiny; people in the roles of the framework (reviewers, adjudicators) can burn out. The source is a short position paper.

### 2.7 Bodies of work beyond code

**48. [P]** Halfaker, A., Geiger, R. S., Morgan, J. T., and Riedl, J. (2013). The rise and decline of an open collaboration system: how Wikipedia's reaction to popularity is causing its decline. *American Behavioral Scientist* 57(5), 664-688. DOI 10.1177/0002764212469365.
Search results report that Wikipedia's quality-control measures, including automated tools and a complex rule system, are implicated in lower retention of desirable newcomers and that norm-setting has calcified. *Bearing:* a warning for ideas 3 and 5: defences against damage can suppress the variation and the new contributors that renewal depends on.

**49. [P]** Stvilia, B., Twidale, M. B., Smith, L. C., and Gasser, L. (2008). Information quality work organization in Wikipedia. *Journal of the American Society for Information Science and Technology* 59(6), 983-1001. DOI 10.1002/asi.20813.
Search results describe a study of how quality work is organised in a distributed way, with attention to correction as well as detection. *Bearing:* supports idea 6 and idea 5 (quality is produced by distributed work visible to all).

**50. [P]** Elliott, J. H., Turner, T., Clavisi, O., Thomas, J., Higgins, J. P. T., Mavergames, C., and Gruen, R. L. (2014). Living systematic reviews: an emerging opportunity to narrow the evidence-practice gap. *PLoS Medicine* 11(2), e1001603. DOI 10.1371/journal.pmed.1001603.
Search results report the argument that incremental improvement of conventional updating is unlikely to improve currency, and that dynamic, online-only, frequently updated reviews can combine currency with rigour. *Bearing:* supports idea 3's two modes outside code: continuous updating beside periodic restructuring; also shows the same pattern in a field with formal governance.

### 2.8 Evolutionary accounts of technology

**51. [P]** Arthur, W. B. (2009). *The Nature of Technology: What It Is and How It Evolves*. Free Press. https://sites.santafe.edu/~wbarthur/thenatureoftechnology.htm
Search results report the thesis of combinatorial evolution (new technologies as combinations of existing ones, with variation and selection acting once a technology exists). A review in *Issues in Science and Technology* judges that the book flirts with a Darwinian correspondence. *Bearing:* supports idea 2 (the core as the stock of combinable parts); the analogy to biology is limited, as the reviewer notes.

**52. [P]** Ziman, J. (ed.) (2000). *Technological Innovation as an Evolutionary Process*. Cambridge University Press. Review: https://www.cambridge.org/core/journals/business-history-review/article/technological-innovation-as-an-evolutionary-process-edited-by-john-ziman-cambridge-uk-cambridge-university-press-2000-xvii-379-pp-index-bibliography-notes-figures-photos-cloth-6495-isbn-0521623618/F2FD401492456EE13571F8C71F3FA43E
A review reports five sections (evolutionary thinking, innovation and culture, invention as a process, institutionalised innovation, technological change in wide perspective) from meetings of nineteen scholars. *Bearing:* the multi-author source for evolutionary models of technology; useful for the analogy's limits (here, institutionalised innovation shows that selection is organised, not natural). Chapter-level content was not read.

**53. [P]** Basalla, G. (1988). *The Evolution of Technology*. Cambridge University Press. https://www.cambridge.org/core/books/evolution-of-technology/72D7695EE9A2BAFB75EFE648D4222A03
Search results report the themes of diversity, a rejection of the necessity myth and an organic analogy covering novelty and selection, drawn selectively from organic evolution; the year is given as 1988 or 1989 by different sources. *Bearing:* idea 1 (worth decided by selection in society) with the caveat that the analogy is selective.

## 3. Synthesis by Design Idea

### 3.1 A project is an evolving system coupled to an environment it changes; worth is decided outside it

*Supported.* Lehman's classification makes the point that a program embedded in its use must keep changing, and that the change alters the use (entries 1, 2). Conway and the mirroring studies tie the design to the organisation around it (10, 11, 12). Basalla and Ziman place selection in society, not in the artefact (52, 53). The ecosystem studies show projects coupled through dependence (37, 38).

*Warnings.* The laws are disputed and rest on few, partly proprietary systems (4); one open-source test found complexity falling on a measure (3). Mirroring has exceptions (11). Biological analogies are selective (51, 53).

*Do not claim* that the laws of software evolution are established, that selection by society is a fitness function that can be read off a metric, or that an ecological model has been shown to predict software behaviour.

### 3.2 A compact generative core and a bulky body; documentation from which the body could be rebuilt

*Supported.* Simon, Parnas and Baldwin and Clark all hold that stable, visible decisions allow hidden parts to vary (5, 6, 7, 8). Alexander and Arthur supply the picture of a small vocabulary generating many wholes (9, 51).

*Warnings.* No work found tests the stronger claim that documentation can regenerate a body. The documentation literature reports rot and incompleteness (19, 20), and the best evidence on independent re-implementation (27, 29) says that ambiguous specifications produce shared errors, which is the result for a core that is incompletely stated. Near-decomposability is an assumption (5), and mirroring is only a tendency (11).

*Do not claim* that the core can be identified in advance, that it is conserved in practice (design rules change), or that documentation of any kind reproduces the body. The claim is testable, and the N-version evidence suggests where it will fail: at ambiguity.

### 3.3 Accumulated damage and periodic rebuilding; two modes

*Supported.* Aging, debt, erosion, stale answers and thin knowledge concentration are all documented (13, 14, 15, 16, 17, 20, 21). Rejuvenation by clean restart is established for runtime state (18). Living reviews pair continuous update with restructuring (50).

*Warnings.* The software-aging literature is largely silent on whether rewrites outperform incremental change; no empirical study of that comparison was found in this search, and the absence should be recorded as a gap, not as support. Restoration tools for erosion are little used (17). Defences that over-protect suppress newcomers and variation (48). Rejuvenation as engineered in 1995 is a restart, not a rewrite.

*Do not claim* that rebuilding is cheaper or safer than patching, that debt is measurable on one scale, or that rejuvenation of runtime state justifies rebuilding of source.

### 3.4 Materials check one another; agreement of independent forms is evidence

*Supported.* Contracts, properties, tests and differential testing all use a second form to check a first (22, 23, 24, 30). Triangulation is a standard epistemic strategy (31). With coding agents, majority voting reduced failures substantially (29).

*Warnings.* This is where the literature is strongest against naive claims. Independently built versions fail together (27, 28, 29); the shared failures concentrate where the specification is hard or ambiguous. Coverage does not measure the power to detect faults (26). Mutation analysis is the accepted measure of whether a check can fail (24, 25). Forms written by one author or one model family are not independent (22).

*Do not claim* that agreement between forms is evidence in proportion to their number, that agreement among agents of one model family is independent confirmation, or that a passing check is informative when it has not been shown capable of failing.

### 3.5 Distributed evaluation, staking and refutation, adoption as worth, scrutiny that grows with reliance

*Supported.* Bounties find bugs at low cost relative to employees (41, 42), and Ostrom's monitoring, graduated sanctions and nesting give an institutional template (34). Peer production is argued to allocate effort well (33). Code review is mature practice (44, 45). Dependence is concentrated, so reliance can be measured (38). The case for scrutiny growing with reliance is made by the npm risk results (39).

*Warnings.* "Many eyes" is an unproven slogan (32); the best tests find bounded returns per person (41) and sometimes more risk with more contributors (43). Adoption signals are gamed (46) and lag behind current worth, since adopters rarely re-check (40). Burnout limits the human refuter (47). Quality defences can choke renewal (48). Ostrom's principles come from rival, scarce commons (34).

*Do not claim* that adoption equals worth, that bounties produce correct refutation rather than findings that are easy to pay for, or that the number of reviewers measures scrutiny. Weight claims by the independence and cost of the evidence.

### 3.6 Most of the framework exists in open-source practice

*Supported.* Review, continuous integration (as practice, studied in the review literature rather than here), bounties, trackers and stigmergic coordination exist at scale (35, 36, 42, 44, 49).

*Warnings.* What is missing in the evidence found is the combination: no source found tests a whole system of stake, refutation and adoption, or a population of agents doing it. The Wikipedia and security-bounty cases show that practices designed for humans have side effects (48, 46, 47). Studies are of single companies or platforms (44, 42).

*Do not claim* that the practices transfer to agents without change, or that the existing practices have been validated as a system.

## 4. References with Marks

[V] Lehman 1980, *Proc. IEEE* 68(9), 1060-1076, 10.1109/PROC.1980.11805
[V] Belady and Lehman 1976, *IBM Syst. J.* 15(3), 225-252, 10.1147/sj.153.0225
[P] Israeli and Feitelson 2010, *J. Syst. Softw.* 83(3), 485-501
[V] Herraiz et al. 2013, *ACM Comput. Surv.*, 10.1145/2543581.2543595
[P] Simon 1962, *Proc. Am. Phil. Soc.* 106(6), 467-482
[V] Parnas 1972, *CACM* 15(12), 1053-1058, 10.1145/361598.361623
[P] Baldwin and Clark 2000, *Design Rules, Vol. 1*, MIT Press
[V] Sullivan et al. 2001, *ESEC/FSE-9*, 10.1145/503209.503224
[P] Alexander et al. 1977, *A Pattern Language*, OUP; Alexander 1964, 1979
[V] MacCormack, Rusnak and Baldwin 2006, *Manage. Sci.* 52(7), 1015-1030, 10.1287/mnsc.1060.0552
[V] Colfer and Baldwin 2016, *Ind. Corp. Change*, 10.1093/icc/dtw027
[P] Conway 1968, *Datamation* 14(4), 28-31
[V] Parnas 1994, *ICSE-16*, 10.1109/ICSE.1994.296790
[V] Cunningham 1992, *OOPSLA '92*, 10.1145/157709.157715
[V] Kruchten, Nord and Ozkaya 2012, *IEEE Softw.*, 10.1109/MS.2012.167
[V] Li, Avgeriou and Liang 2015, *J. Syst. Softw.*, 10.1016/j.jss.2014.12.027
[P] de Silva and Balasubramaniam 2012, *J. Syst. Softw.* 85(1), 132-151
[P] Huang et al. 1995, *FTCS-25*, 381-390
[P] Aghajani et al. 2019, *ICSE 2019*, 10.1109/ICSE.2019.00122
[P] Zhang et al. 2021, *IEEE TSE* 47(4), arXiv:1903.12282
[P] Avelino et al. 2016, *ICPC 2016*, arXiv:1604.06766
[V] Meyer 1992, *Computer*, 10.1109/2.161279
[V] Claessen and Hughes 2000, *ICFP '00*, 10.1145/351240.351266
[V] DeMillo, Lipton and Sayward 1978, *Computer*, 10.1109/C-M.1978.218136
[V] Jia and Harman 2011, *IEEE TSE* 37(5), 649-678, 10.1109/TSE.2010.62
[V] Inozemtseva and Holmes 2014, *ICSE 2014*, 10.1145/2568225.2568271
[V] Knight and Leveson 1986, *IEEE TSE* SE-12(1), 96-109, 10.1109/TSE.1986.6312924
[P] Eckhardt and Lee 1985, *IEEE TSE* SE-11(12), 1511-1517
[V] Ron, Baudry and Monperrus 2026, arXiv:2606.20158
[P] McKeeman 1998, *Digit. Tech. J.* 10(1), 100-107
[P] Wimsatt 1981, in Brewer and Collins (eds.)
[P] Raymond 1999, *The Cathedral and the Bazaar*
[P] Benkler 2002, *Yale Law J.* 112(3), 369-446
[P] Ostrom 1990, *Governing the Commons*, CUP
[V] Mockus, Fielding and Herbsleb 2002, *ACM TOSEM* 11(3), 309-346, 10.1145/567793.567795
[P] Robles et al. 2005, *ProSim 2005*
[P] Mens et al. 2014, in *Evolving Software Systems*, Springer
[V] Decan, Mens and Grosjean 2019, *Empir. Softw. Eng.*, 10.1007/s10664-017-9589-y
[P] Zimmermann et al. 2019, *USENIX Security '19*, arXiv:1902.09217
[P] Kula et al. 2018, *Empir. Softw. Eng.* 23(1), 384-417, 10.1007/s10664-017-9521-5
[V] Maillart et al. 2017, *J. Cybersecur.* 3(2), 81-90, 10.1093/cybsec/tyx008
[P] Finifter, Akhawe and Wagner 2013, *USENIX Security '13*
[V] Meneely and Williams 2009, *ACM CCS '09*, 10.1145/1653662.1653717
[V] Sadowski et al. 2018, *ICSE-SEIP 2018*, 10.1145/3183519.3183525
[V] Bacchelli and Bird 2013, *ICSE 2013*, 10.1109/ICSE.2013.6606617
[P] He et al. 2024, arXiv:2412.13459
[P] Raman et al. 2020, *ICSE-NIER 2020*
[P] Halfaker et al. 2013, *Am. Behav. Sci.* 57(5), 664-688, 10.1177/0002764212469365
[P] Stvilia et al. 2008, *JASIST* 59(6), 983-1001, 10.1002/asi.20813
[P] Elliott et al. 2014, *PLoS Med.* 11(2), e1001603, 10.1371/journal.pmed.1001603
[P] Arthur 2009, *The Nature of Technology*, Free Press
[P] Ziman (ed.) 2000, *Technological Innovation as an Evolutionary Process*, CUP
[P] Basalla 1988, *The Evolution of Technology*, CUP

**Counts:** [V] 24 (entries 1, 2, 4, 6, 8, 10, 11, 13, 14, 15, 16, 22, 23, 24, 25, 26, 27, 29, 35, 38, 41, 43, 44, 45); [P] 29 (the remaining entries); [U] 0. Total 53 works.

**Gaps recorded.** No empirical study of rewrites against incremental change was located. No tested system of stake, refutation and adoption was located. Findings on documentation sufficiency for regenerating a body were not located. Several older IEEE and ACM records carry no abstract, so their content summaries are limited.
