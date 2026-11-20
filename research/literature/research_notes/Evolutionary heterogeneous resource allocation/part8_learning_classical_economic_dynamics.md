# Learning Classical Dynamical-System Models of Economies from Observed Data

Scope: how laws of motion for aggregate, sectoral and distributional economic dynamics are learned from records, with their uncertainty and their validity out of sample. Agent-level models are left to another part. Written for a dynamical-systems foundation (kinesis) that already declares flows, maps, SDEs and systems with events; fits templates by least squares and Levenberg–Marquardt; discovers sparse laws (SINDy, weak SINDy); learns balance laws of densities on graphs; estimates transfer operators; and detects critical transitions.

Markers:
- **[V]**: verified by reading the source itself during this research. "[V abstract]" means only the abstract was read; "[V text]" means the full text was read.
- **[S]**: from a search snippet or a secondary summary only.
- **[M]**: from memory, not checked during this research. "[M; DOI checked]" means the reference exists as cited (title, journal, year, authors, volume and pages checked against Crossref's metadata), but the content claim was not checked.

Full texts read: Barnett and Serletis (1998 working paper of the 2000 JEDC article) and LeBaron (1994 working paper, its introduction and macroeconomics section). arXiv abstracts were read through the arXiv API. About 50 tool calls; researched 2026-11-20.

---

## 1. Which methods learn an economic law of motion (not just a forecast) from data, and what evidence shows they recover something real out of sample?

### Takeaway
The econometric tradition learns laws of motion in three ways:
- **Linear reduced forms with identified shocks:** VAR/SVAR, local projections (LP), dynamic factor models.
- **Linear-Gaussian state-space likelihoods:** Kalman filtering, used for nowcasting and for estimating linearised DSGE models.
- **Explicit regime processes:** Markov switching, time-varying parameters.

The evidence that these recover something real out of sample is modest:
- Structural (DSGE) and statistical models forecast about equally badly in absolute terms since the Great Moderation.
- Nowcasting with factor models is the clearest operational success.
- Nonlinear machine-learning forecasts help mainly in episodes of stress.
- The strongest out-of-sample claim for an explicitly *dynamical* law learned from data is the two-variable fitness–GDP velocity field of Tacchella et al. (2018), and it is a nonparametric analogue method, not an equation.

Newer flexible learners (GP-VARs, echo-state networks, neural ODEs/UDEs) are benchmarked mostly as forecasters. A 2026 cross-country stress test found that physics-style structural priors (PINN, UDE) did *worse* than ARIMA and plain neural ODEs on sparse annual macro data.

### Cited Findings

**1.1 VARs, structural VARs and local projections**
- **Sims (1980) and the VAR.** Sims proposed unrestricted vector autoregressions as an alternative to the "incredible" identifying restrictions of large macro-econometric models. This founded the reduced-form-plus-identified-shocks tradition [M; DOI checked] — [Sims 1980, *Econometrica* 48(1):1–48, doi:10.2307/1912017](https://doi.org/10.2307/1912017).
- **The Handbook surveys.** The standard modern surveys of identification and propagation are the Handbook of Macroeconomics vol. 2 chapters: Stock and Watson on dynamic factor models, FAVARs and SVARs (pp. 415–525), and Ramey on shocks and their propagation (pp. 71–162) [M; DOI checked] — [Stock & Watson 2016, doi:10.1016/bs.hesmac.2016.04.002](https://doi.org/10.1016/bs.hesmac.2016.04.002); [Ramey 2016, doi:10.1016/bs.hesmac.2016.03.003](https://doi.org/10.1016/bs.hesmac.2016.03.003).
- **Local projections.** Jordà (2005) estimates impulse responses by a separate regression at each horizon, with no law of motion imposed [M; DOI checked] — [Jordà 2005, *AER* 95(1), doi:10.1257/0002828053828518](https://doi.org/10.1257/0002828053828518).
- **In population, LPs and VARs agree.** With unrestricted lag lengths, local projections and VARs estimate the same impulse responses. They differ only in finite-sample bias and variance [M; DOI checked] — [Plagborg-Møller & Wolf 2021, *Econometrica* 89(2), doi:10.3982/ECTA17813](https://doi.org/10.3982/ECTA17813).
- **In samples, a bias–variance trade-off.** A simulation over thousands of data-generating processes calibrated to US data shows a clear trade-off. LPs have lower bias but much higher variance at intermediate and long horizons. Bias-corrected LP is preferred only if bias is all that matters. Otherwise Bayesian VARs are preferred at short and long horizons, and least-squares VARs at intermediate and long horizons [V abstract]. The DGPs come from an estimated dynamic factor model, so none is exactly a finite-order VAR [S] — [Li, Plagborg-Møller & Wolf, arXiv:2104.00655 (v4, Jan 2024)](https://arxiv.org/abs/2104.00655). Published in the *Journal of Econometrics* (2024) according to a search snippet [S]; Crossref returned only the NBER and SSRN versions (doi:10.3386/w30207).

**1.2 State space, the Kalman filter, dynamic factor models and nowcasting**
- **The Kalman filter** is the recursive linear-Gaussian filter. It gives the exact likelihood of a linear state-space model, and so the estimation engine for every linear latent-state model in this tradition [M; DOI checked] — [Kalman 1960, *J. Basic Eng.* 82:35–45, doi:10.1115/1.3662552](https://doi.org/10.1115/1.3662552).
- **Diffusion indexes.** A few principal-component factors from many series improve macro forecasts [M; DOI checked] — [Stock & Watson 2002, *JBES* 20:147–162, doi:10.1198/073500102317351921](https://doi.org/10.1198/073500102317351921).
- **Nowcasting** with a dynamic factor model in state-space form, updated as each data release arrives (the model's precision improves as information accumulates within the quarter) [M; DOI checked] — [Giannone, Reichlin & Small 2008, *JME*, doi:10.1016/j.jmoneco.2008.05.010](https://doi.org/10.1016/j.jmoneco.2008.05.010). The Handbook survey is [Bańbura, Giannone, Modugno & Reichlin 2013, *Handbook of Economic Forecasting* 2A:195–237, doi:10.1016/B978-0-444-53683-9.00004-9](https://doi.org/10.1016/b978-0-444-53683-9.00004-9) [M; DOI checked].
- **Reservoir computing as a nowcaster.** A Multi-Frequency Echo State Network matched or beat MIDAS and dynamic factor models in multistep forecasts of US GDP growth. It takes many series at far lower computational cost [V abstract] — [Ballarin et al. 2024, *IJF* 40(3):1206–1237, doi:10.1016/j.ijforecast.2023.10.009; arXiv:2211.00363](https://arxiv.org/abs/2211.00363).

**1.3 Regime switching and drifting parameters**
- **Markov switching.** Hamilton's model lets the mean growth of US real GNP switch between two states following a hidden Markov chain. The inferred recession probabilities line up with NBER dates [M; DOI checked] — [Hamilton 1989, *Econometrica* 57(2):357–384, doi:10.2307/1912559](https://doi.org/10.2307/1912559). The Handbook survey is [Hamilton 2016, "Macroeconomic Regimes and Regime Shifts", doi:10.1016/bs.hesmac.2016.03.004](https://doi.org/10.1016/bs.hesmac.2016.03.004) [M; DOI checked].
- **Nonlinearity found by fitting.** Fitting explicitly nonlinear specifications to macro data has been "generally more successful at finding strong evidence for nonlinearities" than diagnostic testing. LeBaron cites Hamilton (1989), Beaudry–Koop, Neftci, Potter and Teräsvirta–Anderson; this work supports asymmetry between expansions and contractions and between positive and negative shocks [V text] — [LeBaron 1994, "Chaos and Nonlinear Forecastability in Economics and Finance", working paper](https://econwpa.ub.uni-muenchen.de/econ-wp/fin/papers/9411/9411001.pdf).
- **Time-varying parameters.** Time-varying-parameter VARs with stochastic volatility let coefficients drift as random walks [M; DOI checked] — [Primiceri 2005, *REStud* 72:821–852, doi:10.1111/j.1467-937X.2005.00353.x](https://doi.org/10.1111/j.1467-937X.2005.00353.x); [Cogley & Sargent 2005, *RED* 8:262–302, doi:10.1016/j.red.2004.10.009](https://doi.org/10.1016/j.red.2004.10.009).
- **Switching variances, not rules.** For US monetary policy, Sims and Zha found that the best-fitting regime-switching model switches the shock variances rather than the policy rule [M; DOI checked] — [Sims & Zha 2006, *AER* 96(1):54–81, doi:10.1257/000282806776157678](https://doi.org/10.1257/000282806776157678).

**1.4 Estimating DSGE models by likelihood and Bayesian methods**
- **The Bayesian toolkit.** An and Schorfheide set out the standard Bayesian estimation and evaluation of linearised DSGE models: Kalman-filter likelihood, random-walk Metropolis–Hastings, marginal data densities, comparison with DSGE-VARs [M; DOI checked] — [An & Schorfheide 2007, *Econometric Reviews* 26:113–172, doi:10.1080/07474930701220071](https://doi.org/10.1080/07474930701220071).
- **The Handbook chapter on solution and estimation**: perturbation and projection solutions, likelihood by Kalman and particle filters, Bayesian computation [M; DOI checked] — [Fernández-Villaverde, Rubio-Ramírez & Schorfheide 2016, Handbook of Macroeconomics vol. 2, doi:10.1016/bs.hesmac.2016.03.006](https://doi.org/10.1016/bs.hesmac.2016.03.006).
- **Nonlinear DSGE likelihood** by the particle filter [M; DOI checked] — [Fernández-Villaverde & Rubio-Ramírez 2007, *REStud* 74:1059–1087, doi:10.1111/j.1467-937x.2007.00437.x](https://doi.org/10.1111/j.1467-937x.2007.00437.x).
- **Smets and Wouters.** A medium-scale New Keynesian model estimated by Bayesian methods, reported as competitive with Bayesian VARs in forecasting [M; DOI checked] — [Smets & Wouters 2007, *AER* 97(3):586–606, doi:10.1257/aer.97.3.586](https://doi.org/10.1257/aer.97.3.586).
- **The absolute record is poor.** Evaluated in absolute terms, the benchmark medium-scale DSGE forecasts inflation and GDP growth very poorly. Statistical and judgmental forecasts do equally poorly. The authors argue that forecasting ability during the Great Moderation is not a good metric for judging models, and that a "wrong" model could show the same pattern [S] — [Edge & Gürkaynak 2010, *Brookings Papers on Economic Activity* Fall 2010:209–259](https://www.brookings.edu/articles/how-useful-are-estimated-dsge-model-forecasts-for-central-bankers-with-comments-and-discussion).

**1.5 Data-driven laws with explicit out-of-sample tests**
- **The fitness–GDP velocity field.** Tacchella, Mazzilli and Pietronero model growth as a two-dimensional dynamical system in GDP per capita and "fitness", a complexity measure computed from product-level exports, and forecast by analogues. They report beating the IMF's five-year GDP forecasts by more than 25% on the available data, with errors uncorrelated with the IMF's [S] — [Tacchella, Mazzilli & Pietronero 2018, *Nature Physics* 14:861–865, doi:10.1038/s41567-018-0204-y](https://doi.org/10.1038/s41567-018-0204-y) (DOI checked). The World Bank/IFC working-paper version describes the "selective predictability scheme" as a two-dimensional kernel regression in fitness and GDP per capita. Critics noted that the IMF benchmark is opaque, and that the gain may partly reflect removing IMF biases [S] — [World Bank Open Knowledge](https://openknowledge.worldbank.org/server/api/core/bitstreams/b4a274b3-7d57-5477-aa88-fc76f4c4cf1f/content); [Cosmos coverage](https://cosmosmagazine.com/physics/physicists-barge-in-on-economists-predictions-ensue).
- **A follow-up on technological fitness** in the same forecasting frame exists [S] — [Angelini, Gabrielli, Tacchella, Zaccaria 2024, *Chaos, Solitons & Fractals* 184:115006, doi:10.1016/j.chaos.2024.115006](https://doi.org/10.1016/j.chaos.2024.115006).
- **Where machine learning helps.** Of four features of ML (nonlinearity, regularisation, cross-validation, loss function), "nonlinearity is the true game changer for macroeconomic prediction". Its gains are associated with high macroeconomic uncertainty, financial stress and housing-bubble bursts. The standard factor model remains the best regularisation, and L2 loss is preferred [V abstract] — [Goulet Coulombe, Leroux, Stevanovic & Surprenant 2022, *JAE* 37(5):920–964, doi:10.1002/jae.2910](https://doi.org/10.1002/jae.2910); [arXiv:2008.12477](https://arxiv.org/abs/2008.12477).
- **Random forests for inflation.** Random forests improve US inflation forecasts in a data-rich setting [M; DOI checked] — [Medeiros, Vasconcelos, Veiga & Zilberman 2021, *JBES* 39:98–119, doi:10.1080/07350015.2019.1637745](https://doi.org/10.1080/07350015.2019.1637745).
- **STAR and neural networks.** Smooth-transition autoregressions and neural networks were compared with linear AR models on G7 macro series; nonlinear models help for some series and horizons, mainly in combination [M; DOI checked] — [Teräsvirta, van Dijk & Medeiros 2005, *IJF* 21:755–774, doi:10.1016/j.ijforecast.2005.04.010](https://doi.org/10.1016/j.ijforecast.2005.04.010).
- **GP-VARs.** A Gaussian-process prior on the conditional-mean function of a VAR, with stochastic volatility, estimated by scalable MCMC. Applied to the asymmetric transmission of uncertainty shocks, and evaluated in a US forecasting exercise [S] — [Hauzenberger, Huber, Marcellino & Petz, *JBES* 43(1):27–43 (2025; online 2024), doi:10.1080/07350015.2024.2322089](https://doi.org/10.1080/07350015.2024.2322089) (DOI checked); [arXiv:2112.01995](https://arxiv.org/abs/2112.01995).
- **Neural ODEs, PINNs and UDEs on macro data.** A 2026 diagnostic study compared ARIMA, LSTM, neural ODE, PINN and UDE across 23 countries on sparse annual macro data, with several temporal splits and five seeds. "None of the evaluated models achieve consistently strong forecasting performance". Less-constrained models (ARIMA, neural ODE) consistently beat the structurally constrained PINN and UDE. Structural priors "can act as misregularizers" when they do not match the data-generating process. The failure modes named are prior misalignment, regime shifts, structural breaks and optimisation instability [V abstract] — [Anand et al., "SciML in the Wild", arXiv:2607.09684](https://arxiv.org/abs/2607.09684).
- **Neural ODEs elsewhere in economics.** A latent neural ODE fitted to poverty indicators in Odisha, 2007–2020, claims only that it "reproduces the observed data with high accuracy". No out-of-sample test is in the abstract [V abstract] — [Samota, Chakraverty & Sethi, arXiv:2604.00632](https://arxiv.org/abs/2604.00632). Agent-based-model-informed neural networks (restricted graph networks keeping mass conservation and locality) learn coupled GDP dynamics of the ten largest economies and are used for counterfactuals. Out-of-sample superiority is shown only for the synthetic SIR case [V abstract] — [Antulov-Fantulin, arXiv:2512.05764](https://arxiv.org/abs/2512.05764).
- **Universal differential equations** are defined as mechanistic differential equations with embedded universal approximators (the SciML formalism) [V abstract] — [Rackauckas et al., arXiv:2001.04385](https://arxiv.org/abs/2001.04385). Neural ODEs were introduced in [Chen et al., arXiv:1806.07366](https://arxiv.org/abs/1806.07366) [V abstract].

**1.6 What the tradition assumes about the dynamics**
- **VARs and LPs** assume linear, time-invariant conditional means and stationarity (or cointegration). Structural VARs further assume that the structural shocks can be recovered from current and past observables (invertibility). LPs relax the law of motion but not linearity in shocks [M].
- **Linearised DSGE models** assume rational expectations, a unique stable solution (determinacy), and dynamics near a steady state. The Kalman filter assumes Gaussian shocks [M] — see [FV–RR–S 2016](https://doi.org/10.1016/bs.hesmac.2016.03.006).
- **Regime models** make discrete change explicit, but assume a fixed small number of recurring regimes [M].

### Inferences
- **Few methods yield a law with parameters or terms.** Only a minority give a law whose terms can be read and transplanted. Linear-Gaussian state-space models with structural restrictions (DSGE) do, as do regime-switching models with interpretable regimes and low-dimensional analogue fields (Tacchella et al.). Most "learned dynamics" papers in economics validate forecasts, not laws.
- **A law is credible when it beats the right benchmarks out of sample.** The benchmarks are a random walk, an AR or BVAR, a factor model and forecast combinations. It should also reproduce model-free objects (LP impulse responses) that it was not fitted to. No surveyed method routinely does both.
- **Structure can hurt.** The 2026 SciML result and the poor absolute DSGE record together say this: imposed structure helps only if it is right, and the record often cannot tell. A learned law should therefore always carry a less-constrained comparator.

### Gaps
- I found no study that evaluated the *recovered structure* of a learned macro law out of sample (as opposed to point forecasts), for example by checking a learned SINDy or neural ODE law's implied impulse responses against LPs on later data.
- The published version of Li–Plagborg-Møller–Wolf, and the details of Edge–Gürkaynak and Tacchella et al., come from snippets only.

---

## 2. Has equation discovery (SINDy, symbolic regression, Koopman/DMD) produced credible economic laws, and where has it failed?

### Takeaway
No. I found no peer-reviewed case in which SINDy, symbolic regression or Koopman/DMD discovered an economic law from real aggregate data that was then confirmed out of sample. The successes are recoveries of *known* models from *synthetic* data:
- the Goodwin cycle by SINDy;
- the Solow–Swan equation by OccamNet;
- EDMD on a simulated Samuelson–Hicks–Goodwin–Puu model.

Where real data are used, results range from descriptive to negative:
- DMD extracts cycles from stock valuations and makes trading signals.
- The one real-data SINDy/UDE attempt on US Goodwin variables (1948–2025) found no interaction term at any regularisation strength.

The failure modes are those the chaos debate already exposed: short samples, measurement noise, nonstationarity and structural breaks.

### Cited Findings

**2.1 SINDy**
- **The method.** SINDy is sparse regression of time derivatives on a library of candidate terms [M; DOI checked] — [Brunton, Proctor & Kutz 2016, *PNAS* 113:3932, doi:10.1073/pnas.1517384113](https://doi.org/10.1073/pnas.1517384113).
- **Variants for noise and scarce data.** Weak (Galerkin) SINDy — [Messenger & Bortz 2021, *Multiscale Model. Simul.* 19:1474–1497, doi:10.1137/20M1343166](https://doi.org/10.1137/20M1343166) [M; DOI checked]. Ensemble-SINDy with bootstrapped inclusion probabilities, for the low-data, high-noise limit — [Fasel, Kutz, Brunton & Brunton 2022, *Proc. R. Soc. A* 478, doi:10.1098/rspa.2021.0904](https://doi.org/10.1098/rspa.2021.0904) [M; DOI checked].
- **Conformal prediction with Ensemble-SINDy** gives target coverage for forecasts, feature-importance-based model selection, and more robust coefficient intervals under non-Gaussian noise. It was demonstrated on predator–prey and chaotic systems, not economic data [V abstract] — [Fasel 2025, arXiv:2507.11739](https://arxiv.org/abs/2507.11739).
- **The one direct macro test.** SINDy and a SINDy-plus-UDE pipeline were applied to Goodwin's growth cycle (predator–prey between the employment rate and the labour share):
  - On synthetic nonstationary data, SINDy recovered the predator–prey structure.
  - On real US quarterly data, 1948–2025 (employment, capacity utilisation, labour share), neither method recovered the key interaction term at any regularisation strength. Plain SINDy returned dense polynomials, and the hybrid found near-zero neural corrections.

  [S; SSRN preprint, not peer reviewed; SSRN refused automated fetching] — [Khundadze, "Learning Macroeconomic Dynamics from Data: Applications of Universal Differential Equations and SINDy", SSRN 6491442](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=6491442).
- **No other macro applications.** A search for other peer-reviewed SINDy applications to macroeconomic data found none [S] (same search).

**2.2 Symbolic regression**
- **OccamNet on social-science data.** A neuro-symbolic system extends OccamNet to find compact functions and differential equations in noisy longitudinal social-science data, and is shown to "discover interpretable models from real-world data in economics and sociology" [V abstract] — [Balla, Huang, Dugan, Dangovski & Soljačić, *Humanities and Social Sciences Communications* 12 (2025), doi:10.1057/s41599-025-04405-x; arXiv:2210.00563](https://arxiv.org/abs/2210.00563). The Solow–Swan equation is recovered from a *synthetic* dataset. The real-data part maps the accuracy–complexity trade-off of growth models across 18 OECD countries, as a test of how general Solow–Swan is [S] — [arXiv PDF](https://arxiv.org/pdf/2210.00563).
- **Genetic programming on experimental data.** Duffy and Engle-Warnick used genetic programming to infer the strategies subjects played in a repeated ultimatum game. This is behavioural, not aggregate dynamics [S] — [Springer chapter](https://link.springer.com/chapter/10.1007/978-3-7908-1784-3_4).
- **No accepted law.** No source found claims that symbolic regression has produced an accepted new economic law [S] — [search summary; AI-Descartes context, *Nat. Commun.* 2023](https://www.nature.com/articles/s41467-023-37236-y).

**2.3 Koopman operators and DMD**
- **Background.** DMD was introduced by Schmid; EDMD approximates the Koopman operator on a dictionary of observables; the modern review is Brunton et al. [M; DOIs checked] — [Schmid 2010, *JFM* 656:5–28, doi:10.1017/S0022112010001217](https://doi.org/10.1017/S0022112010001217); [Williams, Kevrekidis & Rowley 2015, *J. Nonlinear Sci.* 25:1307–1346, doi:10.1007/s00332-015-9258-5](https://doi.org/10.1007/s00332-015-9258-5); [Brunton, Budišić, Kaiser & Kutz 2022, *SIAM Review* 64:229–340, doi:10.1137/21M1401243](https://doi.org/10.1137/21M1401243).
- **DMD for trading strategies** [M; DOI checked] — [Mann & Kutz 2016, *Quantitative Finance* 16:1643–1655, doi:10.1080/14697688.2016.1170194](https://doi.org/10.1080/14697688.2016.1170194); [arXiv:1508.04487](https://arxiv.org/pdf/1508.04487).
- **Cycles in stock valuations.** Koopman-mode analysis of stock valuations finds cyclic behaviour that persisted through the Great Recession. It separates robust from non-robust features by requiring modes to persist across subgroups of the data [S] — [Hua et al. 2016, *Physica A* 448:172–180](https://www.sciencedirect.com/science/article/abs/pii/S0378437115010870).
- **EDMD for business cycles uses no real data.** Data are generated from a nonlinear model of the Samuelson–Hicks–Goodwin–Puu family [S] — [Leventides, Melas & Poulios 2022, *Data Science in Finance and Economics* 2(2):117–146, doi:10.3934/DSFE.2022006](https://doi.org/10.3934/DSFE.2022006) (DOI checked). A follow-up uses EDMD with Takens embedding to build a linear autoregressive approximation of a chaotic macro model [S] — [DSFE 2(4)](https://aimspress.com/aimspress-data/dsfe/2022/4/PDF/DSFE-02-04-021.pdf).
- **DMD reconstruction error as a regime detector.** It detects transient dynamics and regime changes, demonstrated on Kuramoto–Sivashinsky and atmospheric reanalysis (the North Atlantic Oscillation shift around 1970) [V abstract] — [Gottwald & Gugole 2019, *J. Stat. Phys.*, doi:10.1007/s10955-019-02392-3; arXiv:1904.09082](https://arxiv.org/abs/1904.09082). Caveat: if the transition happens on the same timescale as the equilibrium dynamics, the reconstruction error may not show it [S] (search summary of the same paper).

**2.4 Delay embedding and empirical dynamic modelling (EDM)**
- **Theory.** Delay-coordinate reconstruction rests on Takens's theorem, Embedology, and its generalisation to multivariate embeddings [M; DOIs checked] — [Sauer, Yorke & Casdagli 1991, *J. Stat. Phys.* 65:579–616, doi:10.1007/BF01053745](https://doi.org/10.1007/BF01053745); [Deyle & Sugihara 2011, *PLoS ONE* 6:e18295, doi:10.1371/journal.pone.0018295](https://doi.org/10.1371/journal.pone.0018295).
- **Convergent cross mapping** for causality in weakly coupled deterministic systems [M; DOI checked] — [Sugihara et al. 2012, *Science* 338:496, doi:10.1126/science.1227079](https://doi.org/10.1126/science.1227079).
- **Equation-free forecasting of a fishery** (salmon) by EDM [M; DOI checked] — [Ye et al. 2015, *PNAS* 112, doi:10.1073/pnas.1417063112](https://doi.org/10.1073/pnas.1417063112).
- **In finance.** Transfer entropy and CCM with Fourier-transform surrogates (separating linear from nonlinear causality) find significant *nonlinear* causality among German and US stock indices that correlation misses [V abstract] — [Ma, Prosperino, Haluszczynski & Räth, arXiv:2312.16185 (*Chaos* 2024)](https://arxiv.org/abs/2312.16185). Pairwise CCM on index returns finds nonlinear coupling rising in crises (GFC, COVID-19) [S] — [*J. Risk Financial Manag.* 2025](https://mdpi-res.com/d_attachment/jrfm/jrfm-19-00558/article_deploy/jrfm-19-00558.pdf).
- **CCM under noise.** Noise lowers cross-map skill, while the convergence rate is more robust [S] — [Mønster et al., arXiv:1603.01155](https://pdf.arxiv.org/pdf/1603.01155).

### Inferences
- **Three reasons it fails on macro aggregates.** (a) A quarterly record of about 300 points over 75 years spans several policy regimes and measurement redefinitions. (b) The candidate laws (Goodwin, Solow) are low-dimensional caricatures of high-dimensional, shock-driven systems, so the "true" sparse law may not exist at the observed scale. (c) Derivative estimation amplifies measurement noise. The Khundadze result fits all three.
- **Real data works only for descriptive objects.** Equation discovery has worked on real economic data only for cycles, modes and couplings in high-frequency financial data, where samples are long. This mirrors the chaos literature's finding that financial data are "more promising" than macro data (§3).
- **What the foundation should claim.** A sparse law from economic data should be reported as one candidate among an ensemble, with inclusion probabilities, a linear comparator and an out-of-sample test. It should never be reported as "the" law.

### Gaps
- The published version of Balla et al. was not read. Whether its OECD analysis tests out of sample is not established.
- The full text of Khundadze (SSRN) could not be fetched.
- No credible Koopman/DMD forecasting comparison against VARs or random walks on macro data was found.

---

## 3. What is the verdict of the literature on low-dimensional chaos and nonlinearity in economic series?

### Takeaway
The verdict has been stable since the mid-1990s:
- **Nonlinearity: strong evidence.** Business-cycle asymmetry, regime switching and volatility clustering are well documented.
- **Low-dimensional deterministic chaos: no credible evidence.** Claims for chaos (Divisia monetary aggregates; a few exchange rates) did not survive scrutiny or replication.
- **Nonlinear forecasting adds little out of sample**, except in stress episodes.

Barnett and Serletis add a deeper point: no existing test can locate chaos "within the structure of the economic system" as opposed to chaotic external shocks. They advise against taking a strong position either way.

### Cited Findings
- **The BDS test** uses the correlation integral and has power against general dependence [M; DOI checked] — [Brock, Dechert, Scheinkman & LeBaron 1996, *Econometric Reviews* 15:197–235, doi:10.1080/07474939608800353](https://doi.org/10.1080/07474939608800353) (Crossref misspells the first author as "Broock").
- **BDS is not a test of chaos.** It tests whiteness (C(N,m,ε) = C(N,1,ε)^m) against general dependence, linear or nonlinear. It "does not provide a direct test for nonlinearity or for chaos", since the sampling distribution under those nulls is unknown. It can give indirect evidence of nonlinear dependence, which is "necessary but not sufficient for chaos" [V text] — [Barnett & Serletis 1998 WP (*JEDC* 24:703–724, 2000, doi:10.1016/S0165-1889(99)00023-8)](https://econwpa.ub.uni-muenchen.de/econ-wp/em/papers/9805/9805003.pdf) (DOI checked).
- **Nonlinearity yes, chaos no consensus.** "There is a broad consensus of support for the proposition that the (macroeconomic) data generating processes are characterized by a pattern of nonlinear dependence, but there is no consensus at all on whether there is chaos." The evidence they cite [V text] — same source:
  - Brock and Sayers (1988), Frank and Stengos (1988), and Frank, Gencay and Stengos (1988) find no chaos in US, Canadian and international macro series.
  - Barnett and Chen's (1988) claim of chaos in US Divisia monetary aggregates drew strong criticism from Ramsey, Sayers and Rothman (1990).
- **The macro null result:** [Brock & Sayers 1988, *JME* 22:71–90, doi:10.1016/0304-3932(88)90170-5](https://doi.org/10.1016/0304-3932(88)90170-5) [content as reported in Barnett–Serletis, V; DOI checked].
- **Why macro results disappoint.** Macro results have "not yet led to particularly encouraging results (mainly due to the small samples and high noise levels for most macroeconomic series)". Financial data, being longer and better measured, show "clear evidence of nonlinear dependence and some evidence of chaos". Two examples: Scheinkman and LeBaron's (1989) weekly CRSP returns; Serletis and Gogas (1997), chaos-consistent results in two of seven East European black-market exchange rates [V text] — same source.
- **High-frequency data.** Studies over short periods (FTSE 100 one- to sixty-minute returns; FTSE, S&P 500, DAX and Nikkei at 15 seconds to 5 minutes) reject independence in favour of nonlinear structure but find "no evidence of low-dimensional chaotic processes" [V text] — same source.
- **A blind competition of tests.** Barnett et al. ran a single-blind controlled competition: five generating processes (chaotic Feigenbaum recursion, GARCH, nonlinear MA, ARCH, ARMA) at sample sizes 380 and 2000; five tests (Hinich bispectrum, BDS, White, Kaplan, NEGM Lyapunov). It showed differing power and, importantly, different null hypotheses (four nulls among five tests). Differences between tests are therefore not just power [V text] — same source; original results in [Barnett et al., robustness to measurement error, inference method and sample size (*JEDC*)](https://pure.psu.edu/en/publications/robustness-of-nonlinearity-and-chaos-tests-to-measurement-error-i/) [S].
- **The conclusion of Barnett and Serletis** [V text] — same source:
  - Dynamic noise "makes it difficult and perhaps impossible to distinguish between (noisy) high-dimensional chaos and pure randomness". Estimates of dimension and Lyapunov exponents are sensitive to dynamic noise.
  - "There have been no published tests of chaos 'within the structure of the economic system', and there is very little chance that any such tests will be available in this field for a very long time." Existing tests cannot tell whether detected chaos comes from the economy or from chaotic external shocks (e.g. the weather).
  - "While there have been many published tests for chaotic nonlinear dynamics, little agreement exists among economists about the correct conclusions."
- **LeBaron's summary.** The search for chaos found "little or no evidence for chaos in any economic time series, but they have turned up a surprising amount of unexplained nonlinear structure". "The direct evidence for deterministic chaos in many economic series remains weak." Two nonlinear-forecasting studies of macro series (Granger et al. 1993; Jaditz and Sayers 1993) find that nonlinear forecasting "does not add much in terms of out of sample forecast performance", which "may suggest some problems in terms of stability/stationarity". Simulated-null (bootstrap) comparisons "helped keep researchers honest" [V text] — [LeBaron 1994](https://econwpa.ub.uni-muenchen.de/econ-wp/fin/papers/9411/9411001.pdf).
- **A later assessment** of chaotic time-series analysis in economics [M; DOI checked] — [Faggini 2014, *Chaos* 24(4), doi:10.1063/1.4903797](https://doi.org/10.1063/1.4903797).
- **The modern echo.** The gains of nonlinear ML forecasts are concentrated in high uncertainty and financial stress [V abstract] — [Goulet Coulombe et al. 2022](https://arxiv.org/abs/2008.12477).

### Inferences
- **Three lessons for learned laws.**
  - Detecting nonlinearity is easy and says little about which law holds.
  - Distinguishing deterministic structure from dynamic noise is generally impossible at macro sample sizes.
  - Surrogate and simulated-null testing is the field's main safeguard.
- **"No chaos" is not "linear".** The dominant nonlinearity is *stochastic* and *state-dependent*: regimes and conditional heteroskedasticity. Learned economic laws should therefore be stochastic (SDEs, regime-switching maps), not deterministic flows with noise added afterwards.

### Gaps
- I read the 1998 working paper, not the published 2000 JEDC article. The published conclusions are assumed to match.
- I found no post-2015 systematic re-test of low-dimensional chaos on modern long macro panels.

---

## 4. How are stochastic economic dynamics (drift, diffusion, jumps, distributions) estimated from data, and with what identification guarantees?

### Takeaway
The approaches by object:
- **Diffusions of single series.** Three tools:
  - closed-form likelihood expansions for discretely sampled diffusions (Aït-Sahalia), which converge to the true MLE;
  - nonparametric drift and diffusion estimators;
  - Kramers–Moyal conditional moments with finite-sampling corrections.
- **Jumps.** Tested and modelled with high-frequency asymptotics and self-exciting processes.
- **Distributions** (income, wealth, firm and city sizes). Estimated as random-growth Fokker–Planck (Kolmogorov forward) models through their stationary Pareto tails and, more demandingly, their *transition speeds*. Gabaix–Lasry–Lions–Moll showed the canonical model's transitions are about an order of magnitude too slow. The newest approach is a functional state-space model of the density jointly with aggregates (Chang–Chen–Schorfheide).

Identification is delicate in three ways:
- Diffusion coefficients are identified from high-frequency data, but drifts need a long span.
- Celebrated nonlinear short-rate drifts were artefacts of finite samples (Chapman–Pearson).
- Kramers–Moyal estimates at finite sampling interval are biased and need corrections that can themselves fake structure.

### Cited Findings

**4.1 Drift and diffusion of SDEs**
- **Closed-form likelihood expansions.** A Hermite expansion gives an explicit sequence of closed-form functions converging to the true transition density of a discretely sampled diffusion. The resulting estimator converges to the true MLE and is consistent, asymptotically normal and efficient [S] — [Aït-Sahalia 2002, *Econometrica* 70(1):223–262, doi:10.1111/1468-0262.00274](https://doi.org/10.1111/1468-0262.00274) (DOI checked). Extended to multivariate diffusions [S] — [Aït-Sahalia 2008, *Ann. Statist.* 36(2)](https://projecteuclid.org/euclid.aos/1205420523).
- **Nonparametric short-rate models** reported a nonlinear drift (strong mean reversion only at extreme rates) [M; DOIs checked] — [Aït-Sahalia 1996, *RFS* 9:385–426, doi:10.1093/rfs/9.2.385](https://doi.org/10.1093/rfs/9.2.385); [Stanton 1997, *J. Finance* 52:1973–2002, doi:10.1111/j.1540-6261.1997.tb02748.x](https://doi.org/10.1111/j.1540-6261.1997.tb02748.x).
- **The artefact.** Applying those same estimators to simulated paths of a square-root diffusion, whose drift is linear, produced nonlinearities "of the type and magnitude" previously reported. Nonlinearity of the short-rate drift is therefore not a robust stylised fact [S] — [Chapman & Pearson 2000, *J. Finance* 55:355–388, doi:10.1111/0022-1082.00208](https://doi.org/10.1111/0022-1082.00208) (DOI checked).
- **Kramers–Moyal estimation** of drift and diffusion from conditional moments is reviewed in [Friedrich, Peinke, Sahimi & Tabar 2011, *Physics Reports* 506:87–162, doi:10.1016/j.physrep.2011.05.003](https://doi.org/10.1016/j.physrep.2011.05.003) [M; DOI checked]. An economic application: the foreign-exchange market, with a Fokker–Planck equation in *scale* for increments [M; DOI checked] — [Friedrich, Peinke & Renner 2000, *PRL* 84:5224, doi:10.1103/PhysRevLett.84.5224](https://doi.org/10.1103/PhysRevLett.84.5224).
- **Finite sampling distorts Kramers–Moyal estimates** [S] — [Ragwitz & Kantz 2001, *PRL* 87:254501, doi:10.1103/PhysRevLett.87.254501](https://doi.org/10.1103/PhysRevLett.87.254501) (DOI checked).
  - The first correction was first order in the sampling interval and assumed Gaussian finite-time conditionals [S].
  - Friedrich et al. commented that different "correction" terms "may fake large corrections" [S] — [comment, arXiv:physics/0203005](https://arxiv.org/pdf/physics/0203005).
  - Arbitrary-order corrections followed (Anteneodo and Riera 2009), and finite-time coefficients via the adjoint Fokker–Planck equation [S] — [Lade 2009, arXiv:0905.4324](https://ar5iv.arxiv.org/html/0905.4324); [arXiv:2104.12813](https://arxiv.org/pdf/2104.12813).
- **Identification of drift and diffusion.** Under high-frequency sampling on a fixed span, the diffusion coefficient is identified through quadratic variation. The drift is not consistently estimable without a long span [M].

**4.2 Jumps**
- **Testing for jumps** in a discretely observed process by high-frequency asymptotics [M; DOI checked] — [Aït-Sahalia & Jacod 2009, *Ann. Statist.* 37:184–222, doi:10.1214/07-AOS568](https://doi.org/10.1214/07-AOS568).
- **Contagion** modelled by mutually exciting (Hawkes) jumps across markets [M; DOI checked] — [Aït-Sahalia, Cacho-Diaz & Laeven 2015, *JFE* 117:585–606, doi:10.1016/j.jfineco.2015.03.002](https://doi.org/10.1016/j.jfineco.2015.03.002).

**4.3 Distributions as Fokker–Planck (Kolmogorov forward) systems**
- **Gibrat's law and Zipf's law.** Random growth (Gibrat) with a lower reflecting barrier yields Zipf's law for city sizes [M; DOI checked] — [Gabaix 1999, *QJE* 114:739–767, doi:10.1162/003355399556133](https://doi.org/10.1162/003355399556133). The survey of power laws in economics and finance is [Gabaix 2009, *Annu. Rev. Econ.* 1:255–294, doi:10.1146/annurev.economics.050708.142940](https://doi.org/10.1146/annurev.economics.050708.142940) [M; DOI checked].
- **The speed of transitions.** Standard random-growth theories generate transitions of top inequality that are too slow compared with the data; the NBER version says "an order of magnitude too slow". Two parsimonious deviations fit the data: "scale dependence" (e.g. changing skill prices) and "type dependence" (a group of high-growth types). Convergence speeds are derived by operator and PDE methods [S] — [Gabaix, Lasry, Lions & Moll 2016, *Econometrica* 84(6):2071–2111, doi:10.3982/ECTA13569](https://doi.org/10.3982/ECTA13569) (DOI checked); [NBER w21363](https://www.nber.org/papers/w21363).
- **Continuous-time heterogeneous-agent models** couple a Hamilton–Jacobi–Bellman equation with a Kolmogorov forward equation for the distribution [M; DOI checked] — [Achdou, Han, Lasry, Lions & Moll, *REStud* 89:45–86 (2022; online 2021), doi:10.1093/restud/rdab002](https://doi.org/10.1093/restud/rdab002).
- **Wealth distribution and mobility** are estimated jointly by simulated moments [M; DOI checked] — [Benhabib, Bisin & Luo 2019, *AER* 109:1623–1647, doi:10.1257/aer.20151684](https://doi.org/10.1257/aer.20151684).
- **Densities as time series.** A time series of cross-sectional densities is treated as a functional time series, with nonstationary (unit-root) components in the densities [M; DOI checked] — [Chang, Kim & Park 2016, *J. Econometrics* 192:152–167, doi:10.1016/j.jeconom.2015.06.025](https://doi.org/10.1016/j.jeconom.2015.06.025).
- **A functional state-space model** [S] — [Chang, Chen & Schorfheide 2024, *JPE* 132(12):4021–4067, doi:10.1086/731411](https://doi.org/10.1086/731411) (DOI checked):
  - Its transition is a functional VAR for aggregates and a cross-sectional density jointly.
  - Its measurement equation accounts for the sampling error of log densities estimated from repeated cross-sections.
  - Sieve approximations make it finite-dimensional.
  - Applied to technology shocks, GDP, employment and the earnings distribution, it finds modest spillovers between aggregate and distributional dynamics.
- **Distribution dynamics of cross-country incomes** by stochastic kernels (the "twin peaks") [M; DOI checked] — [Quah 1996, *Eur. Econ. Rev.* 40:1353–1375, doi:10.1016/0014-2921(95)00051-8](https://doi.org/10.1016/0014-2921(95)00051-8).

**4.4 Networks and flows among sectors and regions**
- **A fitness model of the World Trade Web.** GDP as fitness reproduces its topology [M; DOI checked] — [Garlaschelli & Loffredo 2004, *PRL* 93:188701, doi:10.1103/PhysRevLett.93.188701](https://doi.org/10.1103/PhysRevLett.93.188701).
- **World input–output networks over time** are mostly described, not modelled. Three studies [S] — [search results](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC4519177/):
  - Cerina et al. track WIOD 1995–2011 communities (a growing German-led European community, China's rise).
  - del Río-Chanona et al. track PageRank and strength over time — [PMC5270329](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC5270329/).
  - Moosavi and Isacchini model the evolving network as a sequence of Markov chains with the Kemeny constant as a flow indicator — [PMC5655482](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC5655482/).
- **Limits of the data.** WIOD's annual tables (1995–2011 in that release) limit dynamic estimation, and one dynamic-model paper uses a single year's network because input–output networks "evolve very slowly" [S] — same search.
- **Harris–Wilson spatial dynamics** (retail or urban structure as Lotka–Volterra-type flows among zones): the integrated methodology — [Wilson 2008, *J. R. Soc. Interface* 5:865–871, doi:10.1098/rsif.2007.1288](https://doi.org/10.1098/rsif.2007.1288) [M; DOI checked]. Made a stochastic model with Bayesian inference from data — [Ellam, Girolami, Pavliotis & Wilson 2018, *Proc. R. Soc. A* 474:20170700, doi:10.1098/rspa.2017.0700](https://doi.org/10.1098/rspa.2017.0700) [M; DOI checked]. Calibrated by neural networks — [Gaskin, Pavliotis & Girolami 2023, *PNAS* 120, doi:10.1073/pnas.2216415120](https://doi.org/10.1073/pnas.2216415120). Networks inferred from time series — [Gaskin, Pavliotis & Girolami 2024, *PNAS Nexus* 3, doi:10.1093/pnasnexus/pgae063](https://doi.org/10.1093/pnasnexus/pgae063) [M; DOIs checked].

### Inferences
- **Guarantees hold only within the model class.** The guarantees that exist (consistency and efficiency of Aït-Sahalia's approximate MLE; convergence of finite-time-corrected Kramers–Moyal estimates) assume the model class is right and the process stationary and Markov. Chapman–Pearson shows that the decisive check is *simulation from the fitted null*: does the "discovered" nonlinearity reappear when the truth is linear?
- **Transition speed tests a distributional law.** For distributional laws, the speed of transition, not the stationary shape, is the informative test. Many laws share a Pareto tail; only some reproduce the speed of observed change (GLLM).
- **Fitted balance laws on economic graphs are scarce.** There is no standard, validated method for learning conservation laws of value among regions or sectors from a few snapshots. The closest are the Bayesian or neural calibration of Harris–Wilson SDEs and the functional VAR of densities. This is a gap the infrastructure's `balance` module could fill, with honest reporting.

### Gaps
- Not verified by reading: the full GLLM text, Chapman–Pearson's estimator details, and the published Chang–Chen–Schorfheide text.
- No study was found that fits a Fokker–Planck model to *time series* of firm-size distributions and validates it out of sample.
- No validated dynamic model of input–output coefficient change (beyond RAS-type updating) was found in this search.

---

## 5. How should a learned economic model report what the record cannot identify, structural breaks, and the Lucas critique?

### Takeaway
Identification failures are widespread in estimated macro models: observational equivalence, partial and weak identification (Canova–Sala). Rank conditions exist for linearised models (Komunjer–Ng; Iskrev), and profile likelihood separates structural from practical non-identifiability (Raue et al.).

Structural instability is pervasive in macro relations (Stock–Watson 1996). Since the mid-1980s, inflation and output have been hard to forecast better than naive benchmarks (Stock–Watson 2007; Edge–Gürkaynak).

The empirical evidence on the Lucas critique is mixed:
- Little direct evidence (Ericsson–Irons).
- Reduced forms can look stable because the policy shifts in the record were small (Rudebusch).
- The critique matters for the early-1980s shift in US monetary policy (Lubik–Surico).

Forecast competitions add three lessons: combinations and hybrids beat pure machine learning (M4); out-of-sample comparisons need tests that handle nesting and instability (Diebold–Mariano, Clark–West, Giacomini–White); and conformal intervals need adaptations for dependence and drift.

### Cited Findings

**5.1 Identifiability**
- **Komunjer and Ng: rank conditions.** Dynamic identification of DSGE parameters from first and second moments. Full rank of the Jacobian of the solution parameters with respect to the deep parameters is necessary but not sufficient. Rank and order conditions are given for stochastically singular and non-singular models, accommodating measurement error and a priori restrictions [S] — [Komunjer & Ng 2011, *Econometrica* 79(6):1995–2032, doi:10.3982/ECTA8916](https://doi.org/10.3982/ECTA8916) (DOI checked).
- **Iskrev: local identification** through the rank of the Jacobian of the model's moments [M; DOI checked] — [Iskrev 2010, *JME* 57:189–202, doi:10.1016/j.jmoneco.2009.12.007](https://doi.org/10.1016/j.jmoneco.2009.12.007).
- **Canova and Sala: identification problems are widespread.** "Observational equivalence, partial and weak identification problems are widespread". They "lead to biased estimates, unreliable t-statistics and may induce investigators to select false models". Diagnostics are proposed [S] — [Canova & Sala 2009, *JME* 56:431–449, doi:10.1016/j.jmoneco.2009.03.014](https://doi.org/10.1016/j.jmoneco.2009.03.014) (DOI checked).
- **Structural versus practical non-identifiability** of partially observed dynamical models, by the profile likelihood [M; DOI checked] — [Raue et al. 2009, *Bioinformatics* 25:1923–1929, doi:10.1093/bioinformatics/btp358](https://doi.org/10.1093/bioinformatics/btp358).

**5.2 The Lucas critique**
- **The critique.** Reduced-form parameters change when policy changes, because agents' decision rules depend on the policy regime [M; DOI checked] — [Lucas 1976, *Carnegie-Rochester Conf. Ser.* 1:19–46, doi:10.1016/S0167-2231(76)80003-6](https://doi.org/10.1016/S0167-2231(76)80003-6).
- **Ericsson and Irons.** Reviewing all articles in the Social Science Citation Index that cite Lucas (1976), they conclude that "virtually no evidence exists that empirically substantiates the Lucas critique", using tests of super exogeneity [S] — [Ericsson & Irons 1995, Fed IFDP 506](https://www.federalreserve.gov/econres/ifdp/the-lucas-critique-in-practice-theory-without-measurement.htm).
- **Rudebusch.** Estimated policy rules show US policy changed, yet VARs often do not reject parameter stability. This is reconciled by the modest size of historical policy shifts and the limited sensitivity of plausible forward-looking reduced forms to them [S] — [Rudebusch 2005, *JMCB* 37:245–272, doi:10.1353/mcb.2005.0024](https://doi.org/10.1353/mcb.2005.0024) (DOI checked).
- **Estrella and Fuhrer.** Some forward-looking models may be *less* stable than their backward-looking counterparts [S] — [Boston Fed WP 99-4](https://www.bostonfed.org/-/media/Documents/Workingpapers/PDF/wp99_4.pdf).
- **Lubik and Surico.** Empirical findings confirm the critique's relevance for the early-1980s shift in US monetary policy [S] — [Lubik & Surico 2010, *JAE* 25:177–194, doi:10.1002/jae.1129](https://doi.org/10.1002/jae.1129) (DOI checked); [Richmond Fed WP 06-05](https://www.richmondfed.org/-/media/RichmondFedOrg/publications/research/working_papers/2006/pdf/wp06-5.pdf).

**5.3 Structural breaks and instability**
- **Instability is pervasive.** Widespread instability was found in univariate and bivariate forecasting relations among US monthly macro series [M; DOI checked] — [Stock & Watson 1996, *JBES* 14:11–30, doi:10.1080/07350015.1996.10524626](https://doi.org/10.1080/07350015.1996.10524626).
- **Inflation.** US inflation became harder to forecast relative to naive models after the mid-1980s; the unobserved-components model with stochastic volatility fits this [M; DOI checked] — [Stock & Watson 2007, *JMCB* 39:3–33, doi:10.1111/j.1538-4616.2007.00014.x](https://doi.org/10.1111/j.1538-4616.2007.00014.x).
- **Testing and dating breaks.** Multiple structural changes are tested and dated in [Bai & Perron 1998, *Econometrica* 66:47–78, doi:10.2307/2998540](https://doi.org/10.2307/2998540). An introduction is [Hansen 2001, *JEP* 15(4):117–128, doi:10.1257/jep.15.4.117](https://doi.org/10.1257/jep.15.4.117). The survey of forecasting under instability is [Rossi 2013, *Handbook of Economic Forecasting* 2B:1203–1324, doi:10.1016/B978-0-444-62731-5.00021-X](https://doi.org/10.1016/b978-0-444-62731-5.00021-x) [M; DOIs checked].

**5.4 Out-of-sample comparisons and the record of forecasting**
- **The random walk.** It matched structural exchange-rate models at 1–12-month horizons, even with realised fundamentals [M; DOI checked] — [Meese & Rogoff 1983, *JIE* 14:3–24, doi:10.1016/0022-1996(83)90017-X](https://doi.org/10.1016/0022-1996(83)90017-X).
- **Tests of predictive accuracy** [M; DOIs checked]: equal accuracy — [Diebold & Mariano 1995, *JBES* 13:253–263, doi:10.1080/07350015.1995.10524599](https://doi.org/10.1080/07350015.1995.10524599); nested models — [Clark & West 2007, *J. Econometrics* 138:291–311, doi:10.1016/j.jeconom.2006.05.023](https://doi.org/10.1016/j.jeconom.2006.05.023); conditional predictive ability, robust to instability and estimation windows — [Giacomini & White 2006, *Econometrica* 74:1545–1578, doi:10.1111/j.1468-0262.2006.00718.x](https://doi.org/10.1111/j.1468-0262.2006.00718.x).
- **M4** (100,000 series, 61 methods) — [Makridakis, Spiliotis & Assimakopoulos 2020, *IJF* 36:54–74, doi:10.1016/j.ijforecast.2019.04.014](https://doi.org/10.1016/j.ijforecast.2019.04.014) (DOI checked):
  - Of the eight entries using pure ML or ML with time-series methods, the pure-ML ones were none more accurate than the combination benchmark, and only one beat Naïve2.
  - 12 of the 17 most accurate methods were combinations of mostly statistical methods.
  - The winning hybrid of statistics and ML was nearly 10% more accurate than the combination benchmark [S].
  - Critics questioned M4's realism [S] — [SAS commentary](https://blogs.sas.com/content/forecasting/2020/01/08/m4-forecasting-competition-results-and-commentary).
- **M5.** On hierarchical retail sales with explanatory variables, the leading entries were dominated by gradient-boosted trees [M; DOI checked] — [Makridakis, Spiliotis & Assimakopoulos 2022, *IJF* 38:1346–1364, doi:10.1016/j.ijforecast.2021.11.013](https://doi.org/10.1016/j.ijforecast.2021.11.013).

**5.5 Uncertainty quantification**
- **Bootstrap intervals for impulse responses** (bootstrap-after-bootstrap bias correction) [M; DOI checked] — [Kilian 1998, *REStat* 80:218–230, doi:10.1162/003465398557465](https://doi.org/10.1162/003465398557465).
- **Conformal inference for dependent data.** Block permutations keep exact validity under exchangeability and approximate validity for time series under weak conditions on the conformity score [V abstract] — [Chernozhukov, Wüthrich & Zhu 2018, COLT, arXiv:1802.06300](https://arxiv.org/abs/1802.06300).
- **Adaptive conformal inference.** Provable long-run coverage frequency "irrespective of the true data generating process" under arbitrary distribution shift [V abstract] — [Gibbs & Candès 2021, arXiv:2106.00170](https://arxiv.org/abs/2106.00170).
- **Conformal Growth-at-Risk.** Conformal calibration of extremal quantiles for US GDP improves calibration and robustness, especially in the tails [V abstract] — [Bogani, Fontana, Neri & Vantini 2024, arXiv:2411.00520](https://arxiv.org/abs/2411.00520).

### Inferences
- **Two kinds of non-identification must be reported.**
  - *Structural*: the law's parameters (or terms) are not determined by the observables, whatever the sample. Rank conditions and observational equivalence show it.
  - *Practical*: a flat likelihood or posterior at this sample size, shown by profile likelihood, prior-to-posterior overlap or ensemble disagreement.

  For equation discovery, a third kind applies: *model-class non-identification*. Several sparse laws fit equally well. Ensemble inclusion probabilities and information criteria across libraries show it.
- **The Lucas critique becomes a support condition.** A law learned under a policy regime is a reduced form valid for the range of policy variation in the record. An evaluator should flag any proposed intervention outside that range: "out of support of the record". Low power of stability tests (Rudebusch) means "no break detected" must not be read as "invariant".
- **Breaks belong in the law.** They should be reported as part of it (dated breaks, regime probabilities, drifting coefficients), not as residual noise. Forecast evaluation should be per period (Giacomini–White), to show *when* a learned law helps.

### Gaps
- I found no paper that systematically applies the identification diagnostics of the DSGE literature (rank, profile likelihood) to *machine-learned* economic laws (SINDy, neural ODE).
- The detailed results of Ericsson–Irons, Rudebusch and Lubik–Surico come from snippets.

---

## What this means for the infrastructure

### Takeaway
kinesis already has the core: templates with LM fitting, SINDy and weak SINDy, balance laws on graphs, transfer operators and transition detectors. What the econometric literature adds is less new estimators than three disciplines:
- **latent states with measurement error**, through state-space likelihoods and regime processes;
- **mandatory benchmarks and model-free checks**: random walk, AR/BVAR, factor models, combinations, and local-projection impulse responses;
- **explicit reporting of what the record does not identify**: rank and profile diagnostics, ensemble inclusion probabilities, finite-sample nulls, breaks, and the support of past policy variation.

A learned economic law can serve as an evaluator of allocations whose order matters. But order effects come precisely from the nonlinear, state-dependent terms that the record identifies worst. So the evaluator must say when a ranking of orders is driven by unidentified structure.

### Recommendations (inferences from the findings above)

**A. Methods to add to the foundation (where they belong)**

1. **A measurement layer for declared systems.** A declaration gains an observation equation, with measurement noise and missing or mixed-frequency observations. Its likelihood comes from tyche's filters:
   - Kalman for linear-Gaussian templates;
   - extended or unscented filters and particle filters for nonlinear ones (as in Fernández-Villaverde & Rubio-Ramírez 2007).

   This turns template fitting into latent-state estimation (output gaps, nowcasts in the Giannone–Reichlin–Small manner) without new estimators in kinesis. tyche already holds Gaussian conditioning, hidden Markov models and VARs; the composition belongs in kinesis's `fit`.
2. **Regime-switching laws.** A flow or map whose parameters follow a hidden Markov chain (Hamilton 1989), and drifting parameters (TVP with stochastic volatility, Primiceri 2005). These are "systems with events" whose events are unobserved. The detector of transitions and the regime model should share one representation, so that a detected break becomes a regime in the law.
3. **Baselines a learned law must beat, as declared objects:**
   - a random walk, AR and BVAR (Li–Plagborg-Møller–Wolf favour BVARs for precision);
   - a dynamic factor model;
   - an analogue or EDM forecaster (kinesis's "forecaster with no law"; Tacchella et al.'s analogue field is the strongest economic precedent);
   - an equal-weight combination (M4's lesson).

   Comparisons are run by Diebold–Mariano, Clark–West (nested) and Giacomini–White (conditional, period by period), with losses reported over time.
4. **Local projections as a falsification test.** Every learned law (template, SINDy, neural ODE) implies impulse responses. These are compared with LP or VAR responses estimated directly from the record, which in population estimate the same responses (Plagborg-Møller & Wolf 2021). A law whose responses fall outside the LP bands has learned forecasts, not dynamics.
5. **Stochastic laws estimated properly:**
   - Aït-Sahalia's closed-form likelihood expansion for discretely sampled diffusions;
   - Kramers–Moyal estimation with finite-sampling-interval corrections (with the warning that corrections can fake structure);
   - jump tests and self-exciting jumps.

   Every nonparametric drift or diffusion estimate gets a **Chapman–Pearson check**: refit the same estimator on paths simulated from the best *simple* (linear-drift) model, and report whether the apparent nonlinearity survives. This mirrors the transitions module's tests against a fitted null.
6. **Dynamics of distributions:**
   - **A functional state-space model**: a density represented by log-spline or sieve coefficients, measured with sampling error that depends on cross-section size, jointly with aggregates (Chang–Chen–Schorfheide 2024).
   - **Fokker–Planck laws** on a grid of income or wealth cells. This is the `balance` module applied to a state space of incomes rather than places.
   - **Transition speed as a reported diagnostic**: the spectral gap, compared with the speed of observed change (Gabaix–Lasry–Lions–Moll 2016).
7. **Balance laws for flows among regions and sectors**: Harris–Wilson-type spatial SDEs with Bayesian or neural calibration (Ellam et al. 2018; Gaskin et al. 2023). The urban-science domain's LUTI work already composes Wilson's models through nomos; the dynamic, stochastic, *learned* version is kinesis's. Input–output coefficient drift remains a research gap.
8. **Koopman/DMD extended**: Hankel DMD and EDMD beside the transfer operator, with bootstrap bands on eigenvalues. The DMD reconstruction error serves as a regime indicator (Gottwald–Gugole), wired into `transitions` with its stated blind spot (transitions on the same timescale as the dynamics).
9. **Conformal wrappers for forecasts**: block-permutation conformal for dependent data (Chernozhukov–Wüthrich–Zhu) and adaptive conformal under drift (Gibbs–Candès). These give interval validity without trusting the law's own likelihood.
10. **Equation discovery made honest for economics:**
    - Ensemble-SINDy with inclusion probabilities and conformal coefficient intervals (Fasel 2025).
    - Libraries that include regime indicators and stochastic terms.
    - A required synthetic-recovery test on data simulated *at the record's length and noise*. Khundadze's synthetic success and real failure show why it is needed.

**B. Learned economic laws as evaluators of allocations whose order matters**

- **Use the law as a stochastic transition map.** An allocation sequence (a₁, a₂, …) enters a learned law as a sequence of inputs or interventions; its value is a functional of the simulated path. Under a *linear time-invariant* law with additive effects, order matters only through timing and discounting, by superposition. Genuine order effects (complementarity, hysteresis, regime-triggering) come only from nonlinear or state-dependent terms, which §2–§3 show are the terms the economic record identifies worst. Hence:
  - **Evaluate with an ensemble of laws.** Use posterior draws, an E-SINDy bootstrap ensemble, the regime-switching posterior, *and the linear benchmark*. Report whether the *ranking* of orders is stable across the ensemble. This can be kybernesis's dual verdict: a standing (ranking robust, arguable, or undecided) beside a measure (the expected difference with its interval).
  - **Attribute every order effect** to the terms that produce it (which library terms, which regime transitions). Report the identification status of those terms (inclusion probability, profile-likelihood flatness). An ordering preference that rests on terms with low inclusion probability is "undecided by the record".
  - **Flag out-of-support allocations (Lucas).** If an allocation moves inputs or policy variables outside the range observed in the record, or into a regime never observed, the evaluator returns its value with the flag "out of support of the record" and widens or refuses the interval. This parallels tyche's withholding of estimates outside their area of applicability (ADR 0128). Absence of a detected break is not evidence of invariance (Rudebusch 2005).
  - **Cost and fidelity.** Learned laws are cheap surrogates relative to structural or agent-based evaluators, so they suit the inner loop of an evolutionary search. Candidate orders the surrogate ranks highly should be re-evaluated by a more structural evaluator, with the disagreement reported (multi-fidelity, as in part 5 on pluggable evaluators).

**C. What each method should report about what the data cannot identify (a proposed minimum contract)**

| Method | Must report |
|---|---|
| Template fit (LS/LM, state-space likelihood) | Jacobian/Fisher rank and its smallest singular values (Iskrev; Komunjer–Ng); profile-likelihood intervals marking practically unidentified parameters (Raue et al.); for Bayesian fits, prior-versus-posterior overlap |
| VAR/SVAR/LP baselines | The identifying assumption (recursive, sign, external instrument) as declared data; bias–variance choice (LP vs BVAR); bootstrap or Bayesian bands |
| Regime-switching or TVP law | Regime probabilities over time; number of regimes chosen and alternatives; dated breaks (Bai–Perron) with intervals |
| SINDy / weak SINDy / symbolic regression | Inclusion probability of each term across an ensemble; rival sparse laws within a tolerance of the best fit; recovery on synthetic data at the record's length and noise; comparison with a linear law |
| Neural ODE / UDE | Forecast skill against less-constrained comparators (the 2026 SciML diagnostic); sensitivity to seeds and splits; no structural claim without an LP check |
| Diffusion / Kramers–Moyal | Sampling interval and the correction used; span-limited drift identification; the Chapman–Pearson simulated-null result |
| Distribution laws (Fokker–Planck, functional VAR) | Cross-section sampling error; transition speed versus observed; stationary versus transient fit |
| Koopman/DMD/EDM | Embedding dimension and lag choices; surrogate tests (Fourier or shuffled) separating linear from nonlinear structure; eigenvalue bootstrap bands |
| Any forecast | Rolling-origin out-of-sample losses against random walk, AR/BVAR, factor model and combination; Diebold–Mariano / Clark–West / Giacomini–White; conformal coverage achieved |
| Any law used for policy or allocation | Range of policy and input variation in the record (the support); "out of support" flags; "no break detected" stated with the test's power |

### Gaps
- No surveyed source tests learned economic laws as *evaluators of sequences of allocations*. The order-effect argument above is inference, not a cited result.
- No standard exists for reporting model-class non-identification of equation discovery on economic data. The contract above is a proposal.
- The economics-specific record of neural ODEs, UDEs and reservoir computing is thin and recent (2024–2026, mostly preprints). It should be revisited before any of these is adopted as a default.
