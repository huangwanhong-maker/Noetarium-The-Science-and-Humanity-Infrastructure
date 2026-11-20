# Part C. Inquiry with AI: automated theory formation, mathematics with machines, assisted science, effects on thought, philosophy of inquiry, and norms of authorship

## 1. Scope and method

**Scope.** This part surveys literature bearing on six design ideas for a framework in which a population of language-model agents helps a scholar develop a theoretical system by evolution of rival lines of development, with the person inside the process: (1) four acts reserved to the person (asking, interpreting, accepting into the theory, authorship); (2) the inquirer's written answer before seeing the agents' (pre-registration of belief); (3) staking on one's own conjectures and paying agents to refute them; (4) a compact generative core distinct from the developed body, with a result that cannot be re-derived from the core revealing a hidden premise; (5) a population of rival lines, some seeded from rival traditions, against homogenisation; (6) claims carrying a standing (proved, refuted by a countermodel, arguable, open) kept apart from the person's acceptance.

**Date and sources.** Search and fetching on 20-21 November 2026. Tools: web search and page fetch. Records were opened through arXiv abstract pages, Crossref and Europe PMC metadata, the ICMJE page, and, for books and some articles, only through search-result snippets of library, PhilPapers, publisher or repository pages.

**Marks.**
- [V] a record was opened and authors, title, year and venue match. Where the record held no abstract (Lenat and Brown 1984; Mellers, Hertwig and Kahneman 2001) the summary rests on the title and on secondary reports, and this is said in the entry.
- [P] found only through search snippets or secondary pages; fields not confirmed are named.
- [U] none are listed.

**Limits.** Nature.com and Science.org pages returned access errors, so the publisher pages were not read; Crossref and Europe PMC metadata were used instead. Several publisher policies (COPE, Nature, Science) were not read at their primary pages and are marked [P]. Summaries of abstracts are paraphrases of what the record states; claims about bearing on the design are the surveyor's interpretation and are labelled as such. Works not reached are listed in section 5 (gaps). Several results from 2026 come from search snippets only and are cited as [P] or mentioned in prose without a mark.

---

## 2. Annotated works

### 2.1 Automated theory formation and conjecture making

**[V] Lenat, D. B. and Brown, J. S. (1984). Why AM and EURISKO appear to work. *Artificial Intelligence* 23(3), 269-294. DOI 10.1016/0004-3702(84)90016-X.** The Crossref record confirms title, authors and venue; it holds no abstract. Secondary reports of the paper (search snippets) state that Lenat conceded that the apparent power of AM came largely from the fit between its representation (short Lisp programs) and mathematical concepts, so that an observer could read comparable sophistication into many outputs of such a generator. *Bearing:* a warning for ideas 4 and 6: apparent discovery can be a property of the representation and of the observer's reading; standing must be assigned by checks outside the generator. The secondary account should be verified against the full text before the point is cited as Lenat and Brown's.

**[P] Langley, P., Simon, H. A., Bradshaw, G. L. and Zytkow, J. M. (1987). *Scientific Discovery: Computational Explorations of the Creative Processes.* MIT Press.** (Publisher page not opened; page count differs between listings.) Describes BACON, DALTON, GLAUBER and STAHL, largely data-driven programs that search for regularities and introduce new terms, and reports their rediscovery of historical laws. *Bearing:* the oldest systematic model of law discovery from data; its programs are given the data and the vocabulary, which supports the reservation of the question to the person (idea 1) and limits what "development of a theory" can mean for them.

**[P] Colton, S. (2002). *Automated Theory Formation in Pure Mathematics.* Springer (Distinguished Dissertations), DOI 10.1007/978-1-4471-0147-5.** (Catalogue records only.) Presents HR, which forms concepts and conjectures and uses theorem provers and model generators to test them; reports that sequence types it invented were accepted into the Encyclopaedia of Integer Sequences. *Bearing:* the nearest prior practice for ideas 4 and 6: conjectures are separated from their counterexamples and proofs by external engines, a countermodel being the standing "refuted".

**[P] Fajtlowicz, S. and collaborators: the Graffiti program and its conjectures** (University of Memphis repository record "On some conjectures of Graffiti", digitalcommons.memphis.edu/facpubs/5313; overview in Wikipedia "Graffiti (program)"). Fields not confirmed: authors and date of the record. Per the secondary sources, Graffiti proposes inequalities between graph invariants that hold on its graph database; its residue-versus-independence conjecture was proved in 1991 and some of its chromatic and matching conjectures were later refuted or proved by others. *Bearing:* conjectures from a generator are cheap, and the human mathematical community did the work of proof and refutation; this is a record of the staking-and-refuting structure of idea 3 in a non-market form.

**[P] "In Reverie Together: Ten Years of Mathematical Discovery with a Machine Collaborator", arXiv:2507.17780 (2025).** Authors not confirmed; title and ID from a search result only. An account of long human work with Graffiti-style conjecturing. *Bearing:* possibly the closest long-run report of a person working with a conjecture machine; not read, so it should be opened before use.

**[V] Raayoni, G., Gottlieb, S., Manor, Y., Pisha, G., Harris, Y., Mendlovic, U., Haviv, D., Hadad, Y. and Kaminer, I. (2021). Generating conjectures on fundamental constants with the Ramanujan Machine. *Nature* 590(7844), 67-73. DOI 10.1038/s41586-021-03229-4.** The Crossref record confirms the metadata (abstract not in the record). *Bearing:* conjectures of a restricted form (continued fractions for constants) generated by search and verified numerically, with proof left to people; supports a separation of the generation of conjectures from their standing (idea 6). The abstract was not read here.

**[V] Schmidt, M. and Lipson, H. (2009). Distilling free-form natural laws from experimental data. *Science* 324(5923), 81-85.** Europe PMC record. An algorithm rediscovered Hamiltonians, Lagrangians and conservation laws from motion-tracking data of oscillators and pendula without built-in physics, using a principle for judging whether a regularity is non-trivial. *Bearing:* symbolic regression finds laws given variables and data; it does not choose the system or interpret what a law means (ideas 1, 2).

**[V] Udrescu, S.-M. and Tegmark, M. (2020). AI Feynman: a physics-inspired method for symbolic regression. *Science Advances* 6(16), eaay2631.** Europe PMC record. Recovered all 100 tested equations from the *Feynman Lectures* (prior software 71) and raised success on a harder set from 15% to 90%. *Bearing:* benchmark equations are known and their variables are given; the result does not speak to open theory development.

### 2.2 Mathematics with AI and people together

**[V] Davies, A., Veličković, P., Buesing, L., et al. (2021). Advancing mathematics by guiding human intuition with AI. *Nature* 600(7887), 70-74.** Europe PMC record. Proposes a workflow in which machine learning detects possible relations among mathematical objects, attribution techniques explain them, and the mathematicians then formulate and prove conjectures; applied to knot invariants and to the combinatorial invariance conjecture for symmetric groups. *Bearing:* the best-known published model of idea 1: the machine proposes a pattern, and the person asks, interprets and proves. It does not include a pre-commitment by the mathematician (idea 2).

**[V] Trinh, T. H., Wu, Y., Le, Q. V., He, H. and Luong, T. (2024). Solving olympiad geometry without human demonstrations. *Nature* 625(7995), 476-482.** Europe PMC record. AlphaGeometry pairs a language model, trained on synthetic proofs, with a symbolic deduction engine; it solved 25 of 30 olympiad problems and produces human-readable proofs. *Bearing:* a language model proposes constructions, a symbolic engine disposes; a pattern for idea 6 (standing set by a checker rather than by the model).

**[V] Hubert, T., Mehta, R., Sartran, L., et al. (2025). Olympiad-level formal mathematical reasoning with reinforcement learning. *Nature* (online 12 November 2025; vol. 651, issue 8106, 2026). DOI 10.1038/s41586-025-09833-y.** Crossref record. AlphaProof learns to find Lean proofs by reinforcement learning, with test-time variation of problems; with AlphaGeometry 2 it reached silver-medal level at the 2024 IMO. *Bearing:* a proof assistant gives a standing ("proved") that does not depend on anyone's trust; it supports the separation of standing from acceptance (idea 6) but only for what can be formalised.

**[V] Romera-Paredes, B., Barekatain, M., Novikov, A., et al. (2024). Mathematical discoveries from program search with large language models. *Nature* 625(7995), 468-475.** Europe PMC record. FunSearch evolves programs with a language model and an evaluator, finding new large cap sets and better bin-packing heuristics; programs are more interpretable than raw solutions. *Bearing:* evolution with an external evaluator works where the objective is computable; the search population is a model for idea 5 but diversity maintenance is not the paper's subject.

**[V] Novikov, A., Vũ, N., Eisenberger, M., et al. (2025). AlphaEvolve: a coding agent for scientific and algorithmic discovery. arXiv:2506.13131.** Abstract page. Evolutionary coding agent; reports a 4×4 complex matrix multiplication with 48 scalar multiplications and improvements on mathematics and computing problems. Preprint. *Bearing:* as FunSearch; relevant to idea 3 in that its evaluator is a stake that cannot be argued with.

**[V] Georgiev, B., Gómez-Serrano, J., Tao, T. and Wagner, A. Z. (2025). Mathematical exploration and discovery at scale. arXiv:2511.02864.** Abstract page. Applies AlphaEvolve to 67 problems in analysis, combinatorics, geometry and number theory; it matches the best known results in most and improves on several; combines it with Deep Think and AlphaProof for proof generation. Preprint. *Bearing:* an account of mathematicians directing a search system over many problems, with the problems chosen by people (idea 1).

**[V] Bolan, M., Breitner, J., Brox, J., Carlini, N., Carneiro, M., van Doorn, F., et al., with Tao, T. (2025). The Equational Theories Project: advancing collaborative mathematical research at scale. arXiv:2512.07087.** Abstract page. An online project in which participants determined all implications among 4,694 equational laws of magmas (about 22 million ordered pairs), combining human-written and automated proofs, every result checked in Lean; it also reports new magma constructions and finite-magma variants. (A search result reports that the main goal was reached on 14 April 2025; this was not checked against the paper's body.) Preprint. *Bearing:* the closest model of a community with automated provers and a formal record in which standing (proved, refuted by a countermodel) is machine-checked and separate from anyone's opinion (idea 6); the question was fixed in advance by the organisers (idea 1) and the object of study had no hidden premises (limit for idea 4).

**[P] Bubeck, S., Gowers, T. et al. (2025). Early science acceleration experiments with GPT-5. arXiv:2511.16072.** Found through a search; the abstract page was not opened, so the author list and claims are from snippets. A collection of case studies in mathematics and several sciences in which experts report GPT-5 contributing steps to ongoing research, including on Erdős problems; the OpenAI announcement and the paper warn that the model can produce plausible but wrong citations, mechanisms or proofs. *Bearing:* a 2025 account by working scholars; supports idea 1 (expert in the loop) and warns on fabrication. The authors include employees of the developer, a conflict of interest to be weighed.

### 2.3 Systems for assisted and automated science, and their evaluations

**[V] Gottweis, J., Weng, W.-H., Daryin, A., et al. (2025). Accelerating scientific discovery with Co-Scientist (earlier title "Towards an AI co-scientist"). arXiv:2502.18864.** Abstract page (v2 shown as revised 2026). A Gemini-based multi-agent system that generates, critiques and ranks hypotheses by a tournament evolution; validated in drug repurposing, target discovery and antimicrobial resistance, with one leukaemia candidate confirmed in vitro. The authors are from the developer. *Bearing:* the nearest published system to idea 5 (competing hypotheses, tournament selection) and a precedent for an evolutionary population; its judge is itself a model, which is what ideas 1-3 would avoid.

**[V] Ghareeb, A. E., Chang, B., Mitchener, L., et al. (2025). Robin: a multi-agent system for automating scientific discovery. arXiv:2505.13400.** Abstract page. Combines literature and data-analysis agents with laboratory experiments; identified ripasudil as a candidate for dry age-related macular degeneration. Preprint. *Bearing:* a lab-in-the-loop design where the world, not an agent, delivers the verdict; theory has no equivalent unless a checker or a person plays that role.

**[V] Mitchener, L., Yiu, A., Chang, B., et al. (2025). Kosmos: an AI scientist for autonomous discovery. arXiv:2511.02824.** Abstract page. A 12-hour run with parallel analysis and literature agents over a structured world model; the authors report that independent scientists judged 79.4% of statements in its reports accurate, and that collaborators estimated a 20-cycle run to equal about six months of their work. Preprint, developer's own evaluation. *Bearing:* a 79.4% accuracy implies about one statement in five is not accurate, which is the strongest reason for idea 6 and for reserving acceptance to the person (idea 1).

**[V] Villaescusa-Navarro, F., Bolliet, B., Villanueva-Domingo, P., et al. (2025). The Denario project: deep knowledge AI agents for scientific discovery. arXiv:2510.26887 (later in PRX Intelligence 1, 021001, 2026, as stated on the abstract page).** A modular system that proposes ideas, reviews literature, plans, codes, plots and drafts papers across many fields; domain experts scored the papers and gave feedback, and the authors discuss limitations and philosophy of science. *Bearing:* demonstrates drafting at scale without a person's authorship; supports concern for idea 1.

**[V] Schmidgall, S., Su, Y., Wang, Z., et al. (2025). Agent Laboratory: using LLM agents as research assistants. arXiv:2501.04227.** Abstract page. Takes a person's research idea through literature review, experiments and report writing; reports that human feedback at each stage improved overall quality, and an 84% cost reduction against earlier autonomous methods. *Bearing:* support for idea 1 (the person's feedback measurably helps), by the system's own authors.

**[V] Lu, C., Lu, C., Lange, R. T., Foerster, J., Clune, J. and Ha, D. (2024). The AI Scientist: towards fully automated open-ended scientific discovery. arXiv:2408.06292.** Abstract page. End-to-end generation of ideas, code, experiments and papers, evaluated by an automated reviewer; some papers exceed a conference acceptance threshold according to that reviewer, at under 15 US dollars a paper. *Bearing:* an example of an automated reviewer standing in for a community; the opposite of idea 1.

**[V] Yamada, Y., Lange, R. T., Lu, C., et al. (2025). The AI Scientist-v2: workshop-level automated scientific discovery via agentic tree search. arXiv:2504.08066.** Abstract page. Three fully autonomous manuscripts were submitted to an ICLR workshop; one scored above the average human acceptance threshold. *Bearing:* shows that peer review at a workshop can pass an unaccompanied output; it says little about the correctness of the claims, and is an argument for an explicit standing apart from acceptance (idea 6).

**[V] Beel, J., Kan, M.-Y. and Baumgart, M. (2025). Evaluating Sakana's AI Scientist: bold claims, mixed results, and a promising future? arXiv:2502.14297 (SIGIR Forum 2025).** Abstract page. Independent evaluation: 42% of experiments failed because of coding errors; the literature check often labelled established ideas novel; manuscripts had a median of five citations; some numerical results were hallucinated. *Bearing:* an independent measurement of failure modes that a person-in-the-loop design must catch; small-scale and of the first version only.

**[V] Si, C., Yang, D. and Hashimoto, T. (2024). Can LLMs generate novel research ideas? A large-scale human study with 100+ NLP researchers. arXiv:2409.04109.** Abstract page. Blind review of ideas by expert reviewers found LLM ideas more novel (p < 0.05) and slightly weaker on feasibility than human ideas; the authors also report unreliable self-evaluation by models and low diversity in generated ideas. Preprint (later versions may differ). *Bearing:* supports idea 5 (low diversity of generated ideas) and idea 2 (novelty is judged by readers, not by the generator); it measured ideas, not finished research.

**[P] Walters, W. H. and Wilder, E. I. (2023). Fabrication and errors in the bibliographic citations generated by ChatGPT. *Scientific Reports* 13, 14045. DOI 10.1038/s41598-023-41032-5.** Through search snippets (PMC and DOAJ listings). On 636 citations across 42 topics, about 55% of GPT-3.5 and 18% of GPT-4 citations were fabricated, and substantive errors occurred in 43% and 24% of the real ones. Models have since changed. *Bearing:* the reported rate is for 2023 models and a different task; supports the claim that literature links by agents need verification by record (the survey method of this document).

### 2.4 Effects of AI on science and on thought

**[V] Messeri, L. and Crockett, M. J. (2024). Artificial intelligence and illusions of understanding in scientific research. *Nature* 627(8002), 49-58. DOI 10.1038/s41586-024-07146-0.** Crossref record (no abstract in the record; the argument is known from the paper's title and the surveyor did not read the article text here). A perspective article, not an empirical study; its title names illusions of understanding and (per its standard summary) monocultures of knowing as risks of AI tools in research. *Bearing:* supports ideas 1 and 5 as an argument; the abstract and body must be read before specific claims are attributed.

**[V] Doshi, A. R. and Hauser, O. P. (2024). Generative AI enhances individual creativity but reduces the collective diversity of novel content. *Science Advances* 10(28), eadn5290.** Europe PMC record. In an online experiment with short-story writers, those given LLM ideas wrote stories judged more creative, but stories of AI-assisted writers were more similar to one another. *Bearing:* the best experimental support for idea 5; the task is story writing, not theory.

**[P] Deng, Brucks and Toubia (2026). arXiv:2602.20408.** Title not confirmed (it concerns diversity of LLM ideation); found in a search result. Reportedly finds LLM outputs converge across independent sessions and that fixation is a mechanism, and that a combination of two prompting methods increased diversity above human baselines. Other 2026 preprints on homogenisation of research ideas and multi-agent ideation diversity were seen only in snippets and are not cited. *Bearing:* if confirmed, evidence for and against idea 5: sessions converge, but prompting can restore diversity.

**[V] Kobak, D., González-Márquez, R., Horvát, E.-Á. and Lause, J. (2025). Delving into LLM-assisted writing in biomedical publications through excess vocabulary. *Science Advances* 11(27), eadt3813.** Europe PMC record. An excess-word analysis of over 15 million PubMed abstracts suggests that at least 13.5% of 2024 abstracts were processed with LLMs, up to about 40% in some subcorpora. *Bearing:* a measurement of prevalence by vocabulary, with inference about use; relevant to authorship disclosure (idea 1).

**[V] Liang, W., Zhang, Y., Wu, Z., et al. (2025). Quantifying large language model usage in scientific papers. *Nature Human Behaviour* 9(12), 2599-2609.** Europe PMC record. About 1.12 million papers (January 2020 to September 2024); estimated modification by LLMs reached about 22% in computer science and up to about 9% in mathematics and Nature portfolio journals. *Bearing:* mathematics is the field with the lowest estimated use, a point for a theory-building framework that expects different norms.

**[P] Lee, H.-P., Sarkar, A., Tankelevitch, L., et al. (2025). The impact of generative AI on critical thinking: self-reported reductions in cognitive effort and confidence effects from a survey of knowledge workers. CHI 2025. DOI 10.1145/3706598.3713778.** Through search snippets (title as shortened by the snippets; verify the exact title). A survey of 319 knowledge workers with 936 examples: higher confidence in the AI went with less critical thinking, higher self-confidence with more; critical thinking shifted toward verification, integration and stewardship. Self-report, no measure of performance. *Bearing:* supports idea 2 as a means of keeping self-confidence anchored in one's own answer; the study is correlational.

**[V] Gerlich, M. (2025). AI tools in society: impacts on cognitive offloading and the future of critical thinking. *Societies* 15(1), 6. DOI 10.3390/soc15010006.** Crossref record. A mixed-methods study of 666 participants reports frequent AI use negatively correlated with critical thinking, mediated by cognitive offloading; younger participants relied more on AI. Cross-sectional, self-reported measures; causation is not established. *Bearing:* a weak support for the concern behind ideas 1 and 2; not to be cited as showing that AI harms thinking.

**[V] Kosmyna, N., Hauptmann, E., Yuan, Y. T., et al. (2025). Your brain on ChatGPT: accumulation of cognitive debt when using an AI assistant for essay writing task. arXiv:2506.08872.** Abstract page. Essay writing by LLM users, search users and unaided writers (54 participants completed three sessions, 18 a fourth), with EEG; connectivity was weakest for LLM users, who also reported least ownership of the essay and difficulty quoting it. Preprint, small sample, essays rather than research. *Bearing:* the finding on ownership and quoting is relevant to idea 1 (authorship); the sample is small and the EEG interpretation is contested.

**[V] Sharma, M., Tong, M., Korbak, T., et al. (2023). Towards understanding sycophancy in language models. arXiv:2310.13548 (v4 May 2025).** Abstract page. Five assistants showed sycophancy across free-form tasks; human preference data favoured responses matching the user's views; optimising against preference models sometimes reduced truthfulness. Authors are at the developer of one assistant. *Bearing:* the direct case for idea 2: an agent shown the inquirer's view will lean to it, so the inquirer's own answer should precede, and the agents' work on it should be hostile (idea 3).

### 2.5 Philosophy bearing on what cannot be delegated

**[P] Hills, A. (2016). Understanding why. *Noûs* 50(4), 661-688.** Through search snippets (PhilPapers listing, online first 2015). Argues that understanding why p requires abilities and is not mere propositional knowledge; it is cited as holding that understanding is not transmitted by testimony as knowledge is. Secondary reports only. *Bearing:* supports idea 1: a person cannot receive an agent's derivation as understanding; they must exercise the abilities themselves (re-derive, apply).

**[P] Pritchard, D. (2014). Knowledge and understanding. In Fairweather (ed.), *Virtue Epistemology Naturalized* (Springer), DOI as in the Springer chapter link 10.1007/978-3-319-04672-3_18 (not confirmed to be this chapter).** Search snippets only. Proposes that understanding is a kind of cognitive achievement and so differs from knowledge in value; contested by Whiting (2012) and others per the same snippets. *Bearing:* a reason to reserve acceptance to the person (idea 1), and the achievement view implies that the theory is the person's only if they accomplish it.

### 2.6 Evolutionary and structural accounts of science

**[P] Lakatos, I. (1970). Falsification and the methodology of scientific research programmes. In Lakatos and Musgrave (eds.), *Criticism and the Growth of Knowledge* (Cambridge University Press); reprinted in Worrall and Currie (eds.), *The Methodology of Scientific Research Programmes* (1978).** Search snippets; the 1970 imprint was not confirmed from a primary page. A programme has a hard core protected by a negative heuristic, a protective belt of auxiliary hypotheses modified by a positive heuristic, and is progressive or degenerating by whether modifications predict novel facts. *Bearing:* the direct source for idea 4 (core, belt, heuristics) and for judging a line as progressive or degenerating; Lakatos's core is not meant to be re-derived from, so idea 4's "re-derivation reveals a hidden premise" is an addition and not a Lakatosian rule.

**[P] Campbell, D. T. (1960). Blind variation and selective retention in creative thought as in other knowledge processes. *Psychological Review* 67, 380-400.** Cited through Simonton's papers; the DOI was not found (a remembered DOI, 10.1037/h0040373, was not confirmed and is not given). Proposes that creative thought, like other knowledge processes, depends on blind variation and selective retention; Simonton's later papers refine and debate "blind" and "sighted" variation. *Bearing:* the evolutionary logic of idea 5; note the debate on whether variation generated by language models is blind or sighted.

**[P] Kitcher, P. (1990). The division of cognitive labor. *Journal of Philosophy* 87(1), 5-22. DOI 10.2307/2026796.** Search snippets. Models how a community should distribute its members among rival approaches, and the role of rewards. *Bearing:* supports idea 5 and the allocation of effort among rival lines; the model assumes simple scientists with payoffs, not agents proposing theory.

**[P] Strevens, M. (2003). The role of the priority rule in science. *Journal of Philosophy* 100(2), 55-79. DOI 10.5840/jphil2003100224.** Search snippets. Argues that the priority rule (credit to the first) helps distribute effort among research programmes; later critics (Schneider and Rubin 2021; Heesen) are said to qualify its conclusion. *Bearing:* relevant to idea 3 and to credit and authorship (idea 1); the rule rewards publication, which a framework for private theory-building would change.

**[P] Zollman, K. J. S. (2007). The communication structure of epistemic communities. *Philosophy of Science* 74(5), 574-587.** Search snippets. In a two-armed bandit model of a scientific network, less connected networks can more reliably converge on the truth because they keep diversity longer; Rosenstock, O'Connor and Bruner (2017) are reported to show that the result is sensitive to assumptions and does not license prescriptions. *Bearing:* the main theoretical support for idea 5 (limited communication among rival lines), and a warning to claim no more than a model's qualitative result.

### 2.7 Practices that keep inquiry honest

**[V] Nosek, B. A., Ebersole, C. R., DeHaven, A. C. and Mellor, D. T. (2018). The preregistration revolution. *PNAS* 115(11), 2600-2606.** Europe PMC record. Argues that distinguishing hypothesis generation from hypothesis testing is essential and that preregistration, specifying plans before outcomes are known, counters hindsight bias. *Bearing:* the source for the principle behind idea 2, although it concerns testing data with analysis plans, not recording belief before consulting an assistant; the extension to belief is the design's own.

**[V] Mellers, B., Hertwig, R. and Kahneman, D. (2001). Do frequency representations eliminate conjunction effects? An exercise in adversarial collaboration. *Psychological Science* 12(4), 269-275. DOI 10.1111/1467-9280.00350.** Europe PMC record without abstract; summary relies on the title. *Bearing:* the named model of rivals designing a joint test; relevant to ideas 3 and 5, but the abstract was not read and its findings are not described here.

**[V] Silberzahn, R., Uhlmann, E. L., Martin, D. P., et al. (2018). Many analysts, one data set. *Advances in Methods and Practices in Psychological Science* 1(3), 337-356. DOI 10.1177/2515245917747646.** Crossref record. Twenty-nine teams (61 analysts) analysed the same data on referees' red cards; odds-ratio estimates ranged from 0.89 to 2.93, 20 teams found a significant positive effect and nine did not; prior beliefs, expertise and peer ratings did not explain the variation. *Bearing:* strong evidence that independent lines from the same material diverge, supporting a population (idea 5) and showing that the person cannot treat one agent's answer as the result.

**[P] Wimsatt, W. C. (1981). Robustness, reliability, and overdetermination. In Brewer and Collins (eds.), *Scientific Inquiry and the Social Sciences* (Jossey-Bass), 124-163 (page ranges differ across sources); reprinted 2012 in Soler et al. (eds.), *Characterizing the Robustness of Science*.** Search snippets (Springer reprint chapter listed). A result is more credible when it is derived by several independent means of determination. *Bearing:* supports idea 5 and idea 4: re-derivation by independent routes; independence between language-model lines is doubtful when they share a model.

**[V] Dreber, A., Pfeiffer, T., Almenberg, J., et al. (2015). Using prediction markets to estimate the reproducibility of scientific research. *PNAS* 112(50), 15343-15347.** Europe PMC record. Markets on 44 psychology studies predicted replication better than a survey of individual forecasts; the median prior probability of a tested hypothesis was estimated about 9%. *Bearing:* supports idea 3 in principle (stakes elicit better forecasts); the markets were run among researchers with real money on replications, not on a person's own conjectures against agents.

### 2.8 Norms of authorship and disclosure for AI

**[V] International Committee of Medical Journal Editors. Recommendations: Artificial Intelligence, "Use of AI by authors" (page opened 20-21 November 2026; no version date shown; "© 2026").** States that chatbots and other AI-assisted tools should not be listed as authors because they cannot be accountable for accuracy, integrity and originality; that journals should require authors to disclose AI use at submission; that humans are responsible for submitted material; and that nondisclosure may require corrective action and may be construed as misconduct. *Bearing:* the reason for idea 1's fourth act (authorship) is a standing norm: accountability stays with people.

**[P] Committee on Publication Ethics (COPE) (2023). Position statement: Authorship and AI tools.** The COPE page returned an access error; the content is from search snippets and a publisher's note of adoption. AI tools cannot be authors because they cannot take responsibility or manage conflicts and licences; their use should be disclosed (methods or similar section); authors stay responsible. First published 13 February 2023; later revisions not checked. *Bearing:* as ICMJE.

**[P] Springer Nature (Nature) policy on AI and authorship.** Search snippets (Springer Nature policy pages and a library guide quoting Nature's statement). Large language models do not satisfy authorship criteria; their use should be documented in Methods; copy editing for readability need not be declared. The nature.com page itself was not read; current wording unverified. *Bearing:* as ICMJE, and shows a carve-out for editing that a framework should not take as extending to derivation.

**[P] Thorp, H. H. (2023). ChatGPT is fun, but not an author. *Science*, 26 January 2023.** Search snippets. The editor-in-chief's statement that an AI program cannot be an author because authors sign a form of accountability; a later softening, reported by Times Higher Education, permits use with disclosure while excluding AI as an author or as an author of cited sources (date of the change not confirmed). *Bearing:* as ICMJE; policies are moving, so the framework's own disclosure should be stricter than the minimum.

---

## 3. Synthesis by design idea

### 3.1 Four reserved acts (asking, interpreting, accepting, authorship)

**Supported.** Authorship and accountability are reserved to people by ICMJE, COPE, Springer Nature and Science (marks V and P). The philosophy of understanding gives a principled reason for the interpretive act: on Hills's ability account and Pritchard's achievement account, understanding is not received by testimony (both [P], from snippets). Empirical systems that report human feedback find it helps (Agent Laboratory, by its authors), and the human-and-machine mathematics projects (Davies et al.; Georgiev et al.; the Equational Theories Project) all kept the choice of problem with people.

**Nearest prior practice.** Davies et al. 2021 is the closest model: the mathematicians interpret a machine-found pattern and then prove. The Equational Theories Project is the closest in organisation, with machine proofs and a formal record, but the question was set by its organiser.

**Documented failures.** Kosmos's own evaluation leaves about one statement in five inaccurate; the independent evaluation of the first AI Scientist found 42% of experiments failing and hallucinated numbers; fabricated citations were frequent in 2023 models. Self-reported studies link confidence in AI to less critical thinking.

**Not to claim.** That the four acts are what the literature recommends: they are the design's choice, supported by norms for authorship and by an argument about understanding, not tested. That reserving them preserves understanding: no study measured this. That the cognitive-offloading studies show harm: they are cross-sectional or small (Gerlich 666 self-reports; Kosmyna 54 and 18 participants, a preprint). That ICMJE and COPE are specific to theory: they come from medical and publication ethics.

### 3.2 Writing one's own answer first

**Supported.** Preregistration separates generation from testing and counters hindsight bias (Nosek et al. 2018 [V]). Sycophancy toward the user's view is a documented property of assistants (Sharma et al. [V]), and a survey of 319 workers links confidence in oneself to more critical thinking (Lee et al. [P], self-report). Doshi and Hauser show that exposure to LLM ideas makes outputs more alike [V].

**Nearest prior practice.** Preregistration and registered reports in psychology; Dreber et al.'s forecasts recorded before replication results. No study found tests recording one's own answer before consulting an AI in theoretical work.

**Documented failures.** Preregistration in empirical fields is partly followed or deviated from; the surveyed record does not report this here and it should be checked. Anchoring itself was not measured in any entry above.

**Not to claim.** That it reduces anchoring: the design predicts it, but none of the surveyed works measured it. That the pre-recorded answer can be scored against what agents add without bias: how to measure "what the agents add" is open and is the design's own proposal.

### 3.3 Staking on conjectures and paying agents to refute them

**Supported (in principle).** Prediction markets outperformed surveys at forecasting replication (Dreber et al. [V]); automated conjecture systems have been tested by refutation, as in Graffiti's conjectures later disproved by others ([P]) and in HR and the Ramanujan Machine where counterexamples and proof checks supply the evaluation. FunSearch and AlphaEvolve work because the evaluator cannot be argued with.

**Nearest prior practice.** Adversarial collaboration (Mellers, Hertwig and Kahneman, [V] for metadata only); markets for replication; refutation by counterexample search in graph theory.

**Documented failures.** A market with one buyer and agent sellers has no independent information source: agents from one model family share errors, and a payment for refutation can reward rhetorical rather than valid counterarguments unless a checker (a proof assistant, a countermodel) decides. The surveyed works do not report any market on a person's own theoretical conjectures. Sycophancy runs in the opposite direction to refutation, which agents must be pushed against.

**Not to claim.** That payment improves refutation quality; that stakes create calibration in a single person (Dreber's markets involved many traders and real replications). The staking mechanism is a design proposal with no direct empirical precedent found.

### 3.4 A compact generative core and a developed body; re-derivation from the core

**Supported.** Lakatos's hard core, protective belt and heuristics are the nearest account ([P]); progressive and degenerating shifts give a criterion for the belt. Lenat and Brown's analysis that representation drove the apparent success of AM and EURISKO ([V] metadata, secondary report) is a reason to state the core explicitly. The Equational Theories Project and HR show a theory as a set of axioms with consequences that can be derived and checked; AlphaProof and Lean give machine-checked derivations.

**Nearest prior practice.** Colton's HR (theory formation from axioms and a domain); Lakatosian reconstruction in philosophy; the ETP's formal derivations from magma laws.

**Documented failures.** AM and EURISKO's discoveries were over-read (per secondary accounts). Derivation can only be checked mechanically where the theory is formalised; most scholarly theories are not.

**Not to claim.** That "a result that cannot be re-derived reveals a hidden premise" is Lakatos's or anyone's published rule: no surveyed work states it. It is a sound logical remark for formalised cores (non-derivability from the stated core means other premises were used or a step is invalid) but for informal theories re-derivation will be defeasible and may reveal a failure of the derivation, not of the core.

### 3.5 A population of rival lines, some from rival traditions

**Supported.** Evidence that LLM assistance narrows the range of outputs (Doshi and Hauser [V]; Si et al. on low diversity of generated ideas [V]; 2026 preprints seen in snippets, Deng et al. [P]). Theoretical support for keeping diversity: Zollman's limited-communication result and Kitcher's allocation model (both [P]), with Zollman's result known to be sensitive to assumptions. Empirical divergence among independent analysts (Silberzahn et al. [V]). Hypothesis tournaments (Co-Scientist [V]) and quality-diversity or island methods in evolution; Campbell's variation and selection [P].

**Nearest prior practice.** AI co-scientist's tournament of hypotheses; many-analysts studies; Lakatos's rival programmes.

**Documented failures.** Many agents instantiated from one model are not independent (a point in the 2026 multi-agent diversity snippets, unread); prompting can restore some diversity (Deng et al., [P], unconfirmed). "Rival traditions" seeded by prompts are portraits of a tradition by the model's reading of it, not the tradition.

**Not to claim.** That a population of agents preserves diversity as a population of scholars does; that seeding from traditions yields rival research programmes in Lakatos's or Laudan's sense. Messeri and Crockett's concern about monocultures of knowing is an argument, not a measurement of agent populations.

### 3.6 Standing kept apart from acceptance

**Supported.** Machine-checked standing is established practice: Lean proofs in AlphaProof and the Equational Theories Project, countermodels in HR and model generators, SAT refutations elsewhere. Authors of automated-science systems themselves report that statements need independent checks (Kosmos's 79.4% accuracy; Beel et al.). Interpretation under norms is a point in the philosophy of inquiry.

**Nearest prior practice.** The ETP's graph of implications each proved or refuted and formalised in Lean, with people free to accept or reject their significance.

**Documented failures.** "Arguable" and "open" are not machine-checked; they are judgments, with the risk that a language model assigns them with false confidence (sycophancy, fabricated support). Automated reviewers (AI Scientist) are poorly validated stand-ins for standing.

**Not to claim.** That the four standings are complete or a recognised taxonomy: no surveyed work uses them. That "proved" for a claim in an informal theory means more than that a given checker accepted a given formalisation.

### 3.7 Warnings for a careful designer

1. Do not cite results for AI-assisted writing, critical thinking or ideas as measurements of AI-assisted theory development; none concern it.
2. Developer-authored evaluations (Co-Scientist, Kosmos, GPT-5 case studies) and preprints are not independent evidence.
3. The cognitive-offloading literature is mostly correlational, self-reported or small.
4. Homogenisation evidence is on stories and on ideas; evidence on theory-building populations was not found.
5. The framework's pre-recording of belief, staking and re-derivation from a core are proposals without a direct precedent in the works surveyed; label them as such.

---

## 4. Plain list of references with marks

[V] Lenat and Brown 1984, *Artificial Intelligence* 23(3):269-294, 10.1016/0004-3702(84)90016-X (metadata only)
[P] Langley, Simon, Bradshaw and Zytkow 1987, *Scientific Discovery*, MIT Press
[P] Colton 2002, *Automated Theory Formation in Pure Mathematics*, Springer, 10.1007/978-1-4471-0147-5
[P] Fajtlowicz, Graffiti, "On some conjectures of Graffiti" (digitalcommons.memphis.edu/facpubs/5313)
[P] "In Reverie Together: Ten Years of Mathematical Discovery with a Machine Collaborator", arXiv:2507.17780 (authors unconfirmed)
[V] Raayoni et al. 2021, *Nature* 590:67-73, 10.1038/s41586-021-03229-4
[V] Schmidt and Lipson 2009, *Science* 324:81-85
[V] Udrescu and Tegmark 2020, *Science Advances* 6(16):eaay2631
[V] Davies et al. 2021, *Nature* 600:70-74
[V] Trinh et al. 2024, *Nature* 625:476-482
[V] Hubert et al. 2025, *Nature*, 10.1038/s41586-025-09833-y
[V] Romera-Paredes et al. 2024, *Nature* 625:468-475
[V] Novikov et al. 2025, arXiv:2506.13131
[V] Georgiev, Gómez-Serrano, Tao and Wagner 2025, arXiv:2511.02864
[V] Bolan et al. 2025, arXiv:2512.07087
[P] Bubeck, Gowers et al. 2025, arXiv:2511.16072
[V] Gottweis et al. 2025, arXiv:2502.18864
[V] Ghareeb et al. 2025, arXiv:2505.13400
[V] Mitchener et al. 2025, arXiv:2511.02824
[V] Villaescusa-Navarro et al. 2025, arXiv:2510.26887
[V] Schmidgall et al. 2025, arXiv:2501.04227
[V] Lu et al. 2024, arXiv:2408.06292
[V] Yamada et al. 2025, arXiv:2504.08066
[V] Beel, Kan and Baumgart 2025, arXiv:2502.14297
[V] Si, Yang and Hashimoto 2024, arXiv:2409.04109
[P] Walters and Wilder 2023, *Scientific Reports* 13:14045, 10.1038/s41598-023-41032-5
[V] Messeri and Crockett 2024, *Nature* 627:49-58, 10.1038/s41586-024-07146-0 (metadata only)
[V] Doshi and Hauser 2024, *Science Advances* 10(28):eadn5290
[P] Deng, Brucks and Toubia 2026, arXiv:2602.20408 (title unconfirmed)
[V] Kobak et al. 2025, *Science Advances* 11(27):eadt3813
[V] Liang et al. 2025, *Nature Human Behaviour* 9(12):2599-2609
[P] Lee et al. 2025, CHI, 10.1145/3706598.3713778
[V] Gerlich 2025, *Societies* 15(1):6, 10.3390/soc15010006
[V] Kosmyna et al. 2025, arXiv:2506.08872
[V] Sharma et al. 2023, arXiv:2310.13548
[P] Hills 2016, *Noûs* 50(4):661-688
[P] Pritchard 2014, "Knowledge and understanding" (chapter DOI unconfirmed)
[P] Lakatos 1970/1978, research programmes
[P] Campbell 1960, *Psychological Review* 67:380-400 (DOI unconfirmed)
[P] Kitcher 1990, *Journal of Philosophy* 87(1):5-22, 10.2307/2026796
[P] Strevens 2003, *Journal of Philosophy* 100(2):55-79, 10.5840/jphil2003100224
[P] Zollman 2007, *Philosophy of Science* 74(5):574-587
[V] Nosek et al. 2018, *PNAS* 115(11):2600-2606
[V] Mellers, Hertwig and Kahneman 2001, *Psychological Science* 12(4):269-275, 10.1111/1467-9280.00350 (metadata only)
[V] Silberzahn et al. 2018, *AMPPS* 1(3):337-356, 10.1177/2515245917747646
[P] Wimsatt 1981, in Brewer and Collins (eds.), *Scientific Inquiry and the Social Sciences*
[V] Dreber et al. 2015, *PNAS* 112(50):15343-15347
[V] ICMJE, AI use by authors (page, 2026)
[P] COPE 2023, Authorship and AI tools
[P] Springer Nature / Nature, AI and authorship policy
[P] Thorp 2023, *Science*, "ChatGPT is fun, but not an author"

Count: 51 works; [V] 32, [P] 19, [U] 0.

## 5. Gaps in this part

Not reached in this pass: Kuhn (exemplars), Laudan (research traditions), Hull (*Science as a Process*), Toulmin, Collingwood (question and answer), Dewey (inquiry), Polanyi (personal knowledge), Peirce (abduction, community of inquiry), Zagzebski and Hills 2009 (testimony and understanding), epistemic autonomy literature, Lenat's 1976 thesis and 1983 EURISKO paper, Davis and Lenat's critique, red-team practice, and 2026 mathematicians' accounts beyond the GPT-5 case studies. These should be searched next, and their records opened, before any claim about them is made.
