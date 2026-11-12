# 0102. The Urban Studio

- **Status:** Accepted, provisionally (2026-11-13).
- **Date:** 2026-11-13
- **Decided by:**
  - **The request.** The user asked whether the ecology methods should be a plug-in of the Urban Science Workbench, or whether to "create a urban studio … project and professional, custom analysis-focusing. Urban WORKBENCH as a embed /external module", or rename the Workbench a 3D GIS. They then decided:
    - "I prefer studio one be an indepent, as workbencj one initially designed to visualize major … even studio can export some structural data that workbench can recognize, and visualize them. Then two project connet together";
    - "make it can be extenable, by future add some plug-in or modules. And make it like market studio, also can call the white broad";
    - "Don't remove" the Workbench's programmable interface: "Each application have major focusings";
    - the Studio is not a small addition but "major";
    - "extenable, programmable, but delicate, beautiful, elegant, gentle UI are important … we may add more algorithms, analysis tools on this studio in the future".
  - **The design.** Claude, here.

## 1. Context

The Urban Science Workbench is a 3D GIS from the Earth to a doorstep, with fifteen plug-ins of layers and analyses, a studio of scripts and records in space and time (ADR 0079), and a canvas of figures (ADR 0081). Market Studio (ADR 0093) is a project-based platform: data bound to roles, slices of space and time, tools that are modules of the board's contract, plug-ins, boards. Its server and page were general except for their names.

The infrastructure's rule forbids one application to depend on another. The Studio cannot embed the Workbench; it can embed what lies below both and hand data across.

## 2. Decision

### 2.1 Two Works, Each With Its Focus

- **The Urban Studio** (`applications/urban-studio`, port 9090) is the work of project-based, programmable analysis of cities and the life in them:
  - a project holds its data with their columns bound to roles, its slices, quantities, steps, views and boards;
  - tools are modules of the board's contract, brought by plug-ins or written as a project's own scripts;
  - each subject arrives as a plug-in, the first being ecology and life (ADR 0101).
- **The Urban Science Workbench** keeps its name and everything it holds, its studio and canvas of figures among them. It remains the work of seeing the city in three dimensions.
- **They are joined by a document, not by code.**
  - A placed table of a Studio project is written as a record in space and time, in the form `visualization/js/gis/spacetime.js` reads.
  - The record goes under `data/cache/urban-studio/exports/`, which every work's server serves at `/data/`.
  - The Workbench shows a record named by `?record=` (a path under `/data/` only) through its line of time, and flies to it.
  - A key *In the Workbench* does both from the Studio.

### 2.2 What Moved Below Both Works

| Layer | What | Before |
|---|---|---|
| API | `science.studio_host`: the host for project-based works, configured by a work's directory, name, title, port and engines, with plug-ins that bring tools, templates, an engine and declared schemes, and an export for other works | Market Studio's `serve.py` |
| API | `science.workspace`: a project's sources read as tables | Market Studio's `sources.py` |
| API | `science.engines`: an engine in C++ built from a source into the cache, current while none of its files changes, under the sessions' WSL lock, and asked one request a process | copied in six works' `engine.py` |
| Visualization | `workspace/studio.js`: the page of a studio, `startStudio(config)`, with a panel of plug-ins, a register of the methods a project used, and a key to another work | Market Studio's `app.js` |

The Urban Studio is the first work on them. Market Studio keeps its own files for now, and can move onto these with its checks as the guard; so can the other works' engine builders.

### 2.3 The Plug-in Contract

A plug-in is a directory under `plugins/`:

- `plugin.json`: title, about, version, colour, the domains it stands on, its theory note;
- `tools/*.py`: board modules;
- `templates/*.json`: projects with a plan;
- `engine/main.cpp`: an engine on one of the API's compositions, built by the work's recipe;
- `schemes/*.json`: declared files that its tools offer as choices, each naming its source.

Adding one changes nothing else of the work. The page lists the plug-ins with what each brings. The project's register lists the methods it ran, with their parameters.

### 2.4 Its Look

Heather (#9B72A6) and moss (#6E9171), provisionally, until the user chooses. Heather marks only what is chosen or in the hand; moss marks what lives. Paper has a breath of green by day and aubergine dusk by night, with hairlines, serif type and generous air. Its mark is a leaf with its vein.

## 3. Alternatives Considered

- **The ecology methods as a Workbench plug-in.** Rejected. A plug-in is a layer and a drawer; a study needs its data bound, its slices, its plan, its register and its boards, and the Workbench is already heavy enough to need switches.
- **Renaming the Workbench "3D GIS".** Rejected. The 3D GIS is a component of the visualization layer (`visualization/js/gis`, `earth.js`, `world.js`); the Workbench is a work on it, and keeps its name and its analyses.
- **Embedding the Workbench in the Studio.** Rejected by the rule that nothing depends on an application. What both need lies below them; what passes between them is a document.
- **Copying Market Studio.** Rejected. Its server and page were general; they moved into the API and the visualization layer, and the Urban Studio stands on them.
- **Moving the Workbench's canvas of figures (ADR 0081) into the Studio.** Considered and withdrawn. The canvas is visualization, the Workbench's own focus, and the user asked that nothing be removed.

## 4. Consequences

- **Checks.** The Urban Studio's suite runs on a store of its own: the page, the recipe, the plug-ins, their schemes and templates; every tool of the ecology plug-in run on a small forest and held to counts by hand; occupancy recovered from simulated visits; the current of a corridor; the record written for the Workbench; a run refused to another page; and Melbourne's template. The page was driven in headless Chrome: the template's plan run from the page in 127 s, its seventeen views drawn, and a record opened in the Workbench (not in the suite).
- **Examples.** *Melbourne's urban forest* (nine steps) and *New York's street trees, 2015* (five steps).
- **Not built:**
  - Market Studio and the other works moved onto the shared host, page and engine builder;
  - a 3D view inside the Studio (the Workbench is its 3D view);
  - plug-ins beyond ecology;
  - the material stock of buildings.
- **Provisional:** the name, the colour and the port.
