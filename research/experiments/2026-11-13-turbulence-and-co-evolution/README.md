# Turbulence, and Systems of Different Kinds Evolving Together

*Status: draft (2026-11-13). Research and learning. [ADR 0103](../../../development/decisions/0103-kinesis-and-the-dynamics-lab.md); the theory in kinesis's note (`foundations/kinesis/docs/theory.md`, §6, §9 and §10). At the user's request: "try a fluid dynamics and investigate turbalence in the lab", and "several integrated heteogenous dynamics examples … system 1 has fluid dynamics, system 2 has chemistry dynamics, system 3 has some dynamics produced by a spiece (like a fish) they combined. They co-evoluated"; and their question whether "a structured field theory based method" with an interaction action "S_{int} will be easier".*

## 1. The Questions

1. **Turbulence.** Can a turbulent cascade be declared in the language of dynamical systems, and can what is exact of it be told from what is only measured?
2. **Coupling by an action.** When is a coupling between two systems a term of an action, and what does writing it so give?
3. **Co-evolution.** When a fluid, its chemistry and a species each have a law of their own and are joined at their inputs, what does the whole do that no part does?
4. **Behaviours.** When two animals' behaviours are coupled unequally, which sets the pace?

Every system is one of the Dynamics Lab's examples 13 to 16 (`applications/dynamics-lab/examples/`), and every number below is asked of the Lab's engine by `study.py` and kept in `results/`.

## 2. Method

- **Turbulence.** The Sabra shell model (L'vov, Podivilov, Pomyalov, Procaccia and Vandembroucq 1998): fourteen shells of wavenumbers 1 to 8192, one complex amplitude each, a force on the first and a viscosity ν. It is a model of the cascade, not a discretisation of the equations of a fluid: it has no space. Its declaration is written by kinesis's catalogue, 28 equations with what is observed. Averages are over a time of 600 after a transient of 50, with errors from ten blocks.
- **Coupling by an action.** Two pendulums given by their actions, joined by the term −k/2 (a.q − b.q)² of an action of the whole; and the same two joined by a wire one way.
- **A sea with life in it.** Convection (Lorenz 1963), nutrient and plankton, and fish, joined by four wires: overturning stirs nutrient up, plankton takes it and warms the layer it lives in, fish graze plankton. It is a conceptual model with numbers chosen to show a mechanism. It is not a model of any sea.
- **Behaviours.** Two callers, each a phase that advances at its own pace and is pulled by the other's; and a third animal, excitable, that hears how loud the two are together and whose answer hurries both.

## 3. What It Shows

### 3.1 The Cascade Carries What Is Put In, Whatever Removes It

| | ν = 3 × 10⁻⁵ | ν = 10⁻⁴ |
|---|---|---|
| power put in | 4.96 ± 0.11 | 4.19 ± 0.12 |
| power dissipated | 4.95 ± 0.13 | 4.19 ± 0.12 |
| flux of energy through shell 3 | 4.96 | 4.18 |
| flux through shell 6 | 4.95 | 4.14 |

- **Exact, and held.** The nonlinear term gives the energy and the second quadratic quantity no rate of change, to rounding, at any state (a check of kinesis's catalogue: 10⁻²⁰ of the terms' size). In the steady state the power put in equals the power dissipated, and the flux through a shell is the power put in less what the shells up to it dissipate: the fluxes through shells 3 and 6 agree with the input to within a hundredth. This is the model's form of Kolmogorov's four-fifths law.
- **Measured.** The energy of the shells falls as k^−0.78 ± 0.02 over eight octaves (ν = 3 × 10⁻⁵), steeper than the 2/3 of Kolmogorov's argument of 1941. The flatness of a shell's amplitude grows down the cascade, from 1.9 at shell 4 (a complex Gaussian amplitude has 2) to 3.2 at shell 9: the small scales are quiet for long and then large. That is intermittency, and it is why the exponent is not 2/3. The error of the exponent is of the line through nine means; over so short a range the exponent also carries a ripple of period three in the shells, which the forcing of one shell leaves.
- **With more than three times the viscosity** the cascade ends a shell or two sooner and the power dissipated is 16% less: it is set mostly by the large scales that are forced, not by the viscosity that removes it.
- **It is chaotic at many scales at once.** The three largest Lyapunov exponents are 7.8 ± 0.2, 4.8 ± 0.2 and 2.8 ± 0.1, against a turnover of the largest scale of about one unit of time: the fastest instability belongs to the small scales.

### 3.2 A Coupling That Is a Force Is a Term of an Action; One That Is Not Cannot Be

- **Joined by an action,** two pendulums are one system whose energy is kept (to 6 × 10⁻¹⁵ in kinesis's check). The one that was moved passes its swing to the other: the second swings widest, 0.400, at t = 17.2, against half a beat of 16.9 for small swings; and over two beats each holds half of the energy, 0.404 and 0.395, with 0.023 in the spring, which sum to the 0.8224 the whole began with.
- **Joined by a wire one way,** the first keeps its own energy to itself (to 2 × 10⁻¹¹) and the second is driven at its own frequency: its energy ranges over 9, eleven times what the whole began with. Nothing is kept, and no action of the two alone gives this coupling.
- **The answer to the question asked.** An interaction action is the right form exactly for couplings that are reciprocal: each acts on the other through one term, and what the whole action is invariant under is kept (energy; momentum when the term depends on a difference alone). It is easier there, since one line states the coupling and the conservation laws follow. The couplings between living things are mostly not of this kind: what eats is not eaten back, and what is heard need not hear. Those are wires. They are also terms of an action, the response-field action of Martin, Siggia and Rose, in which every state has a second field beside it; that form treats reciprocal and one-way couplings, and noise, alike, and it is recorded in kinesis's notes and not built.

### 3.3 Life Changes the Fluid It Lives In, from the Top of the Food Chain Down

| | plankton | heating of the fluid | overturning ⟨x²⟩ |
|---|---|---|---|
| with fish | 0.4167 | 25.33 | 55.8 |
| without fish | 1.832 | 36.66 | 85.5 |

- **Control from above.** With fish the plankton stands at death/gain = 0.1/0.24 = 0.4167 exactly: the level at which the fish neither grow nor decline. Nothing in the chemistry or the fluid sets it.
- **Without fish** there is 4.4 times the plankton, the heating it adds carries the fluid from 25.3 to 36.7, and the overturning is half as strong again. In this model the fish, by grazing, calm the convection: an effect that runs from a species through chemistry to a fluid.
- **The threshold.** The steady state with life in it loses its stability at a warming of 6.590, where the heating passes 24.74, the threshold of the fluid alone (6.576 if the fluid were alone at that heating: the coupling moves it by a fifth of a percent). A sweep of the warming shows chaos only from about 7.2: just above the threshold the instability grows at 0.004 a unit of time, more slowly than the sweep waits. A diagram delays what an eigenvalue dates.
- **Fished.** As the fish die faster (0.06 to 0.14) the plankton rises in proportion (0.250 to 0.583), the heating from 24.0 to 26.7 and the overturning from 52.5 to 59.3. At the two lowest rates the heating is below 24.74 and the steady state is stable, yet a path started away from it is still chaotic over the time averaged: below the threshold the fluid's chaos and its steady state coexist.
- **By generations.** With the species as a map (one generation in each step of the whole) the three are of two kinds and are advanced side by side; the fish persist and the plankton is held near the same level.

### 3.4 The One That Listens Less Leads

- **Locked,** two callers of paces 1 and 1.3 call at one pace, (b_listens · 1 + a_listens · 1.3) / (a_listens + b_listens): a mean of their own paces weighted by how much the *other* listens. Measured against the closed form: 1.25000 (a listens 1, b 0.2), 1.05000 (reversed), 1.21429 (0.25 and 0.1), and 1.15000, the plain mean, when they listen equally. Listening equally is the case in which the pull is that of a potential, −k cos(b.th − a.th), and could be an action; the unequal case could not, and it is the unequal case that has a leader.
- **Not locked** when together they listen less than their paces differ (0.3): each keeps a pace nearer its own (1.038 and 1.262 at 0.1 and 0.1).
- **A third that answers.** It answers more when the two are in step (0.516) than when they drift (0.433), and when they drift it answers in bursts (a second moment of 3.4 against 2.8). Its answer hurries both callers alike, and the pace they then keep is exactly theirs and its: 1.25 + 0.05 × 0.5159 = 1.27580, as measured.

## 4. What It Does Not Show

- A shell model is not a fluid. It has no space, no vortices and no boundaries; of turbulence it keeps the cascade and its intermittency. Turbulence in a discretised fluid is not in the Lab, which discretises no partial differential equation.
- The exponent 0.78 is over eight octaves at one viscosity and one length of run; published values for longer cascades are lower. It is given with the error of its fit, which is not the error of the exponent.
- The sea is a toy. Its couplings are of the right kind (stirring, uptake, grazing, heating by absorbed light) and its numbers are chosen, not measured. That the fish calm the convection is a property of this model, and says only that such a path of influence can be declared and followed.
- Nothing here is a field: every part is a few numbers. The coupling of fields (a concentration carried by a flow, animals moving through both) needs fields that run, which kinesis reads and does not yet run.

## 5. Files

| File | Holds |
|---|---|
| `study.py` | The script: starts the Lab's server, asks the studies and the sweeps, writes the results |
| `results/turbulence.json` | The means, errors and second moments at two viscosities, the power laws, the three largest exponents |
| `results/joined.json` | The beat, the shares of the energy, the ranges of the energies under a wire one way |
| `results/sea.json` | The threshold, the means with and without fish, and as the fish are fished |
| `results/behaviours.json` | The paces of the duet against the closed form, and the trio in step and drifting |

Pictures of the four are in the gallery's §34.
