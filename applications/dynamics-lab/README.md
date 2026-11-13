# Dynamics Lab

Status: **draft** (2026-11-13). Serves **research**, **learning** and **use**. [ADR 0103](../../development/decisions/0103-kinesis-and-the-dynamics-lab.md).

*The name is the user's. It is the proper name of this work; no part of the infrastructure is called a laboratory.*

## 1. Concepts

A place to declare dynamical systems, to see what they do, and to go from a record back to a law.

- **A script declares systems.** One notation serves a flow, a map, a system with events, a stochastic differential, a system given by its action, an open quantum system, a system of rules, and a system made of systems. A system gives its state, then its change, its structure, its constraints and what is told apart (the user's four primitives); or it is derived from another, or made of parts. A script that is wrong is told where: the line, the column, and what may follow there.
- **A study is a tool that was run.** Each tool is run on a system or on a record, with what it was asked written beside what it answered, so that a study can be run again and a project shows its work. When the script changes, the studies made before are marked.
- **Answers say what they rest on.** A run names its method. An exponent comes with the error of its finite time. A braid read from samples is certified only under the bound on speed one states, with the margin by which it holds, and otherwise the windings the record admits are listed. A landscape says which kind it is. A ranking of models says it is among the candidates tried.
- **From a record to a model.** A record is a table one brings, or a run of a system (a record whose law is known). It is given a law with no law proposed, by a template whose numbers are unknown, among candidates, or against the catalogue's models of its shape in every order of its components; or continued with no law at all. What is found is a declaration again, which can be added to the script and studied like any other.
- **What it is not.** It studies small systems declared as terms. It discretises no partial differential equation, and of what the language reads it does not yet run fields, reversals or unknown functions: those are read, described, and said not to be carried out.

The mathematics is the foundation kinesis's (`foundations/kinesis`), with morphe's braid groups and physis's open quantum systems beneath it; the drawings are the visualization layer's `dynamics`; the page computes nothing.

## 2. Using It

```
python applications/dynamics-lab/serve.py --open
```

The first time, the page offers to build the engine (g++ 11 or later, on this computer or in WSL; about a minute), or run `python applications/dynamics-lab/engine.py build`.

- **The script** (left). It is read as one types. Beneath it, what it declares: each system with its form (a flow, a map, stochastic, an action, quantum, rules, of parts, derived), or the error where it is wrong.
- **The bench** (right). The system selected, its equations set as mathematics, and the tools that apply to it. A tool opens a form made from its declaration; **Run** adds a study. Below, the project's records: **Bring one** takes a table (a column of times, then one for each component).
- **The sheet** (middle). The studies in order, each with its figures, its numbers and what it rests on; **Again** repeats one with its request, **Remove** takes it away. A study may offer to add what it found to the script (a derived system, a fitted model).
- **Examples.** Sixteen: Lorenz's system; maps and their diagram; events; systems given by an action; noise, landscapes and slowing down; grammars as dynamical systems; fields without a background (read, not run); a diffusion model's forward process; open quantum systems; braids of trajectories; systems of systems; from a record to a model; and four case studies: turbulence as a cascade; two pendulums joined by an action; a sea with life in it (a fluid, its chemistry and a species); behaviours together.
- **Models.** The catalogue's thirty, by field: one chosen is added to the script as it is declared there, with its source.
- **Keep** keeps the project on this computer under its name: its script, its records and its studies.
- **What one may write.** The guide on the page; `GET /api/grammar` for the grammar in BNF; kinesis's `language/README.md`.

```
system pendulum {
  state q(t) : circle
  param m = 1, l = 1, g = 9.81
  change { action S = integral(m * l^2 * q'^2 / 2 + m * g * l * cos(q)) }
  structure { time t in real ; histories stationary S }
}

system two_wells {
  state X : real
  param s = 0.45
  noise W : wiener
  change { dX = (X - X^3) * dt + s * dW }
}

system anbn {
  state w : words over { S, a, b }, start S
  change {
    grow : S -> a S b @ 0.7
    stop : S -> a b   @ 0.3
  }
  structure { derivations ; weights probability }
}
```

The tools:

| Tool | On | Answers |
|---|---|---|
| Run | any system that runs | its components against time, one against another, or three in the round; the method; the events |
| Exponents | flows, maps, actions, merged composites | Lyapunov's exponents with the error of a finite time, their sum, Kaplan and Yorke's dimension |
| Averages | the same, where the system declares what is observed | the mean of each observation along one long run, with its error and its second moment; set against a scale, the power law through the means |
| Equilibria | the same | those Newton's method reaches from seeds in a box, with their kinds and eigenvalues |
| Diagram | the same | the attractor as a parameter moves |
| Basins | the same, of two components or more | where each state of a grid ends, each end named (at rest and of what kind, or still moving), with its share |
| Landscape | flows and stochastic differentials | a potential, the exact stationary landscape in one dimension, or one from long paths; refused, with the reason, for a flow that is no gradient |
| Operator | maps of one component | the invariant density and the eigenvalues of the transfer operator on a partition |
| Slowing down | a run, or a series | the trends of variance and autocorrelation against red-noise surrogates; the fluctuation exponent |
| Braid | a system whose components are points of the plane | the braid of the run, what the record certifies under a bound on speed, the windings admitted, the entropy, the Alexander polynomial of the closure |
| A braid word | a word typed | its reduction, permutation, entropy and polynomial |
| Words | a system of rules | its words within a bound, the histories of each, the graph of its forms, a sample |
| Stationary state | an open quantum system | its stationary state and whether it is the only one, the gap, the spectrum |
| Derive | a derived or composed system | the declaration it stands for |
| Discover a law | a record | a sparse law of monomials, as a declaration |
| Fit a template | a record and a template | the template's numbers, by least squares or Levenberg–Marquardt |
| Match models | a record and candidates | the candidates ranked on a part kept aside; with the catalogue's models if asked |
| Forecast with no law | a record | a forecaster's run against the part kept aside, and how long it holds |
| Linear operator | a record | the operator that best carries each sample to the next: its eigenvalues and rates |

## 3. Structure

| Part | Holds |
|---|---|
| `index.html`, `styles.css`, `src/app.js` | The page: the script, the sheet, the bench; projects, examples, models, records |
| `src/tools.js` | The tools, each declared: what it applies to, its fields, how its answer is shown. A tool joins the Lab by being added to this list |
| `serve.py` | The server: the page, the shared code, the engine kept alive (a request a line), the examples, the catalogue, projects kept under `data/cache/dynamics-lab/projects/` as documents `noetarium.dynamics/1` |
| `engine.py`, `engine/main.cpp` | The engine: a program around the API's `science/dynamics.hpp`, built into `data/cache/engines/dynamics-lab/` |
| `examples/*.dyn` | The examples: a file's first comment line is its title, its second what it is about, and each line `#! {…}` a study it opens with |
| `recipe.json` | The one step of setting up: the engine |
| `tests/run-tests.mjs`, `tests/drive.mjs` | Its checks; the page driven in headless Chrome |

## 4. Checks

`node applications/dynamics-lab/tests/run-tests.mjs`: 80. The page's elements and files; each tool an operation the server allows and the engine answers; the examples and the studies they open with; the server on a directory of its own (a project kept, read back and put away; a name that is no name, another page and a document of no format refused); and, where the engine is built from the present sources, every example read and every study of it carried out through the server and answered as it is known to be: Lorenz's equilibria and the sum of its exponents, the logistic map's ln 2, a ball's first bounce at √(2/g), the pendulum's equation derived from its action, the wells of a landscape, the basins' ends by their kinds, a grammar's words and the histories of one, a driven atom's stationary population 9/19, three stirrers on a figure of eight braiding with the entropy 3 log((3 + √5)/2), a turn certified at sixteen samples and not at four, the follower of a driven pair coming to its drive, Lorenz's seven terms and three numbers recovered from a record and its model found first among the catalogue's; the cascade of a shell model with the power put in, the power dissipated and the flux through two shells equal, and its spectrum steeper than 2/3; two pendulums joined by an action whose energies sum to what the whole began with, and a wire one way that keeps nothing; a sea in which the plankton stands at death/gain with fish and at 4.4 times that without; two callers at the pace their listening gives, to rounding; and what is refused says why.

`node applications/dynamics-lab/tests/drive.mjs [--shots <directory>]` drives the page in headless Chrome through every example, a tool run by hand, a model added from the catalogue, a script changed and made wrong, and night (109 assertions; not in the suite: it needs Chrome and a built engine).

## 5. Dependencies

kinesis (through the API's `science/dynamics.hpp`), and beneath it calx, morphe, physis and tyche; the visualization layer's `dynamics` and styles; three.js 0.170.0 and KaTeX from jsDelivr. Python's standard library; g++ for the engine. No data.

## 6. Limits and Open Questions

- A study that takes more than two minutes is stopped; a run records at most 400,000 samples; a request holds at most 8 MB.
- Systems are small: a quantum state of dimension at most 64, an action of at most three coordinates.
- Read and not run: fields weighted by an action, the expansion of a field theory into rules, the reversal of a stochastic differential, unknown functions. The example *Fields without a background* shows what is said of them.
- No fluid is discretised: the turbulence here is a shell model's, a cascade with no space. Fields coupled to fields (a concentration carried by a flow, animals moving through both) are not here.
- Not here yet: a system known only as a black box (an oracle: a simulator or a device driven and read); learning an unknown function, for which torch on the CPU is allowed; plug-ins; drawing a system by hand; a record's braid with points that arrive and leave; several people on one project.
- The colour, an iris with a rose minor, is provisional until the user chooses.
