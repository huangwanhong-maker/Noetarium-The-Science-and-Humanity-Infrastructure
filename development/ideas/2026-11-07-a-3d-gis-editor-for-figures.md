---
title: The Studio, a Rendering Project Editor in the Urban Science Workbench, for Publishable Figures, Analysis and Live Code
status: promoted (ADR 0081, 2026-11-08); this note keeps the full design and what is not yet built
date: 2026-11-07
revised: 2026-11-07, after the user's second message (projects with their own renderer replace the first version's editing around the live map; a scheduler and shared content are designed), and after the third (a controller; C++ and Python compiled or interpreted live, their output streamed into the project). 2026-11-08: the first implementation (§0.2), and the project made a canvas of frames at the user's word.
related: [the Urban Science Workbench, ADR 0053 (fields and their visual expression), ADR 0078 (the whole Earth), ADR 0079 (data in space and time), ADR 0080 (quantitative economics), visualization/scene, visualization/js/gis, visualization/js/map, Cosmopoiesis's runtime (calx's real-time scheduling)]
---

# The Studio: Rendering Projects in the Urban Science Workbench

*This note designs the Studio before anything is built. It is written for the user to decide on. What is decided becomes an ADR (§13), and §12 lists the decisions left open.*

## 0. The Position in Brief

**The request**, in the user's words:

- make the workbench "a 3D GIS system editor for making publishable figures, and data analysis, visualization";
- keep the present interface as it is, and add "an editor/studio module";
- take QGIS and Photoshop as models for editing by layers, and Unity for programmable plug-ins;
- let scripts be written in the editor and render as they are written, working with other layers and scripts;
- let what the workbench already draws (its line of time, its 2D and 3D maps) be sources, as QGIS takes the current view as a layer's input and freezes it into a figure;
- design what a script takes, what it gives and what it shares;
- make it project-based: a new rendering project, holding many layers and scripts;
- give the render loop a scheduler that decides the order;
- give scripts shared content;
- keep scripts away from the main 3D Earth, which plug-ins draw. The Studio is "an independent render project";
- give it "a controller", and "realtime compiler/interpreter (for c++ and python)" whose output appears as it runs, or is drawn in the project.

The user added: "Don't believe me, I just discuss." §0.1 says where this design departs from what was suggested, and why.

Ten claims, each argued below:

1. **The Studio is a workspace of projects, beside Explore, not inside it.** Explore stays exactly as it is, drawn by its plug-ins. The Studio opens projects, and each project draws in a renderer of its own, with its own camera, clock and loop. No script of a project ever touches the main Earth (§2).
2. **A project holds scenes and plates.** It holds:
   - sources;
   - shared content;
   - layers;
   - scripts;
   - **scenes**: render setups, each with its camera, clock, lighting and base;
   - **plates**: pages for print, which frame the scenes, 2D maps and charts.

   It is a folder with a manifest, like a QGIS project or a Unity project (§4).
3. **Everything the workbench holds can enter a project as a source.** The main view enters as a frozen capture, with the camera, moment, layers and the versions of their data. Other sources are:
   - the 2D map's features;
   - records of the line of time;
   - plug-ins' layers, run in the project's own renderer where they allow it;
   - toolbox answers, tables, files and scripts (§4.3, §4.5).
4. **A layer is a recipe, never pixels.** A layer is a source, then transforms, then a mark, after ADR 0053's V = M ∘ ρ ∘ r. Editing is non-destructive (§4.4).
5. **A project runs as a world on a schedule.** Layers are entities with components, shared content is a set of resources, and scripts are systems. Each system declares what it reads and writes. A scheduler orders the systems by those declarations, every frame and every tick, as an ECS engine (Bevy's, Unity's DOTS) does (§5).
6. **The render loop has fixed phases and render queues:** input, clock, ticks, updates, the commit of results, the scene's sync, the render passes by queue, and capture. In offline mode every frame waits for its work to settle, so that an exported figure or film is determined by the project (§6).
7. **Shared content is resources with declared access.** One writer for each resource in each phase. Several writers only where their merge is a commutative monoid, so that order cannot matter. A feedback loop must pass through a value of the previous tick, as in synchronous dataflow (Lustre's `pre`). Scripts coordinate through shared content without the order of running deciding the drawing (§7).
8. **A script is a system or a transform with declared edges.** It reads layers, resources and a read-only context of its scene, and writes resources, data and declared marks, which the host draws at longitudes and latitudes (§8).
   - **JavaScript** runs in a worker, so the page's authority is never its own.
   - **Python and C++** run in **kernels**: processes kept running for the project, which interpret or compile a script as it changes, keep its state, and stream its output into the project as it runs (§8.7).
9. **A controller runs the project.** It holds:
   - the transport: play, pause, step and record;
   - the schedule, and the kernels' life;
   - how each kernel keeps time: free-running while live, in lockstep for export;
   - the one stream by which their output arrives;
   - the controls bound to resources, steering a scene live (§6.4).
10. **Publication is designed in.** Sizes are in millimetres at a DPI. Legends, scale bars and credits are written from the data. A preflight check runs before every export, and a project's package can draw its figures again (§9).

### 0.2 The First Implementation, and the Project as a Canvas

The user asked for an initial implementation, to give feedback on: a script made while the project runs, compiled if it is C++, put into the scheduler when played, and run by it in the render loop, with pause, stop and delete. Explore was to keep working, untouched. While it was being built, the user saw that its examples only drew on the world. The user asked for a project like a canvas, whose layers differ in width and height. The view's range in space and time should be readable by scripts. Beside the 3D world there should be a small 2D map, statistics figures, and anything a script draws. **This revises §4.2 and §9.** A project is now one canvas of frames, and plates in millimetres become the canvas's later sizes, not a second structure.

**Built** (`applications/urban-science-workbench/studio.html`, `src/studio/`, `studio_kernels.py`, `studio_project.py`, `kernels/`; `visualization/scene/project.js` and `schedule.js`; `api/python/science/studio.py`, `api/cpp/studio_plugin.hpp`):

- **A page of its own,** `/studio.html`. Explore gained two keys beside its view keys, *capture* and *studio*, and nothing else.
- **A project** is a folder of the data library's cache: `project.json` beside `scripts/`. A new project is seeded with four scripts in the three languages that share a resource and draw into frames.
- **The canvas.** It has a size in pixels and a background, and holds frames, each with a place, a size, a ground (none, vellum, paper) and an order:
  - `world`: one globe, its camera and clock; the layers of the world are drawn in it;
  - `map`: the 2D GIS following the world, the footprint of its view drawn on it, the world's marks as dots. No names are drawn, and areas in dispute are hatched in no country's colour;
  - `chart`: the workbench's own SVG charts, drawn by a script or following a numeric resource tick by tick;
  - `drawing`: a script's own items, in the frame's pixels;
  - `text`.

  Frames are moved and resized by hand (*arrange*). A frame a script names that is not yet on the canvas is made in a free corner. The export composes every frame again at a multiple of the canvas's size: the world at that resolution, and the charts, maps and drawings as vector with their fonts.
- **What a script is given:**
  - the moment and the tick;
  - its parameters;
  - the values of what it reads, and of what it reads as previous;
  - the world frame's range: its camera; the footprint along its edges, when all of it is ground; the box of ground seen; the metres of ground to a canvas pixel;
  - the frames and their sizes.
- **What a script gives:**
  - writes, and contributions to merged resources;
  - marks on the world (points, columns, lines on WGS 84);
  - `chart(frame, …)` and `drawing(frame, …)` into frames;
  - printed lines.

  It is the same in JavaScript (the page's worker), Python and C++.
- **The schedule** (§5) as designed:
  - a writer runs before its readers;
  - a merge is a commutative monoid, a number broadcast over an array;
  - a loop closes only through previous;
  - what breaks a rule is refused with its reason, and the rest run.
- **The controller:**
  - play, hold, step, reset and rate;
  - fixed ticks, chained so that their rate does not hang on the frames';
  - lag counted, never queued;
  - systems loading, compiling, running, paused, stopped, failed or refused;
  - pause, resume, stop, start, reload and delete.
- **Kernels:**
  - a Python process kept running;
  - a C++ host that loads each script's shared library, compiled alone with g++ (in WSL here), and swaps it.

  A new version that does not load leaves the one before running, and the error comes back with its line. State is handed over through `save()` and `load()`. The server's guard (its own page, its own address, or a token) holds for every write and every run.
- **Data layers** from the data library's records (events, tracks, cells) are drawn on the world, and each is also a resource that scripts may read. **Captures** from Explore carry the camera, moment, layers and an image. The Studio takes their view, and lays their image over the world.

**Not yet built:**
- the offline, lockstep loop for exports and films (an export now composes the present frame);
- render queues;
- several world frames, or orthographic and 2D-only worlds;
- legends and credits written from the data;
- the preflight;
- sizes in millimetres at a DPI;
- the trust of a project opened from elsewhere (§8.5);
- plug-ins drawing again in a project's renderer (§4.6), so that a capture's plug-in layers are its image only, true only from its view;
- transforms (§8.1), tables and the attribute table;
- a line of time for the project's clock.

### 0.1 Where the Design Departs from What Was Suggested

| Suggested | Taken | Departed from, and why |
|---|---|---|
| **An independent render project; scripts kept off the main Earth** | Wholly. A project has its own renderer, camera, clock and loop. Explore stays the plug-ins' (§2). | Nothing, but two costs are paid knowingly. A second graphics context uses memory, and the same tiles may be loaded twice. The Studio is therefore a page of its own, so that only one of the two renders at a time, and both read through the same caches (§2.3). |
| **Project-based** | Wholly: a project is a folder, with a manifest of sources, resources, layers, scripts, scenes and plates. | One addition: a project holds several **scenes**, as a Unity project holds scenes and a QGIS project holds layouts. A figure of three panels then needs one project, not three. |
| **A scheduler for the render loop** | Fixed phases, render queues, and systems ordered by their declared access (§5, §6) | **An order chosen by hand alone.** Hand-numbered orders (Unity's script execution order) break silently as a project grows. Here the order follows from what each system reads and writes, and a hint only breaks ties. |
| **Shared content for scripts** | Shared resources, typed, named and saved with the project (§7) | **A store any script writes at any time.** The drawing would depend on the order scripts happened to run in, and a loop of writes need never settle. Access is declared, a writer comes before its readers, and a feedback loop passes through the previous tick. |
| **Unity's programmability** | Systems with a lifecycle, an inspector built from declarations, presets, play mode, code reloaded while running | **Arbitrary engine access for a person's scripts.** A project opened from elsewhere would run code with the page's authority. A script that draws by calls cannot be frozen, drawn again at a size, or written as vector. Scripts declare marks (§8.4). |
| **C++ and Python compiled or interpreted live, their output in the project** | Kernels kept running, which reload a script on each change, keep its state, and stream records, resources and marks into the project (§8.7). A controller over them (§6.4). | **Waiting on them each frame while live.** A process apart that the frame waits on stalls the page whenever it is slow. While live, a kernel runs on its own clock and the controller takes its latest output. Only the offline loop waits for it, tick by tick, so that an export is determined. **Server code for a project opened from elsewhere.** A kernel acts with the authority of whoever started the server, so it runs only for a project marked trusted. |
| **Photoshop's layers** | The non-destructive stack, adjustment layers, masks, history | Editing pixels |
| **3D for publishable figures** | Scene frames | **Perspective as the only figure.** It hides what stands behind and has no true scale. Plates offer 2D map frames in stated projections, orthographic cameras, and a preflight that says when a scale bar is false (§9.2). |

## 1. Purpose

A scientist using the workbench today can find, compute and see. What they cannot do is keep what they have made:

- fix a view so that it can be reproduced;
- compose several views into a figure with its legend and scale;
- export it at a journal's size and resolution;
- write code that draws, simulates or analyses, beside data of their own, and keep all of it as one piece of work.

The Studio is where that is done. It serves all three activities (CLAUDE.md §1):
- **use:** figures and analyses that can be relied on;
- **research:** live scripts over layers;
- **learning:** a project is a readable record of how a figure was made.

## 2. The Studio and the Workbench

### 2.1 Two Workspaces

| | Explore (as it is) | The Studio |
|---|---|---|
| What it is for | finding and seeing the world, from the Earth to a doorstep | making: figures, analyses, simulations |
| What draws | the plug-ins, through their layers | a project, through its layers and scripts |
| Its camera, clock, loop | the workbench's, one for the session | each scene's own |
| Its state | live, and not saved | saved in the project |
| Code | plug-ins in the application's directory | a project's scripts, in a worker |

### 2.2 How They Meet

- **From Explore to the Studio:** a key in Explore, *capture to a project*, sends a frozen capture of the view (§4.5) to the open project, or to a new one. Explore is not left.
- **From the Studio to Explore:** a scene's camera can be shown in Explore (*see it in the workbench*), so that the live world around a figure can be explored again.
- **Nothing else crosses.** A project's scripts never run in Explore, and the plug-ins draw in a project only through the adapter of §4.6.

### 2.3 Why a Page of Its Own

The Studio opens as a page of the same application (`/studio/`), on the same server, with the same libraries:

- **Its own loop and context.** A script that stalls or a scene that fills the memory cannot slow or break Explore.
- **One graphics context at a time.** A page in the background is throttled by the browser, so the two seldom render at once. A laptop's 4 GB of graphics memory holds one of them comfortably.
- **One cache.** Tiles, records and products go through the server and the browser's cache, which both pages share, so that the second loads quickly.
- **Its own address.** A project can be opened by a link (`/studio/#project=…`), as a scene of the workbench is.

*Alternative.* A full-screen mode within the same page would switch faster, but it would share the page's memory and loop with Explore, which is what the user asked to keep apart.

## 3. Four Precedents, and What Is Taken from Each

| Precedent | Taken | Not taken |
|---|---|---|
| **QGIS** | The project. The layer tree. A layer as source, symbology and labels. Several print layouts in one project, their map frames following or locked. A stated coordinate reference system for every frame. The attribute table. Processing as a graph. | Modal dialogs. |
| **Photoshop** | The non-destructive stack. Adjustment layers. Masks. Opacity and blending. The history of steps that can be undone. | Editing pixels. |
| **Unity** | The project with its scenes. Entities with components. The inspector from declared fields. Scripts with a lifecycle (start, fixed update, update, late update, events). Render queues. Play mode. The profiler. Code reloaded while running. | Full engine access for scripts; a hand-numbered execution order. |
| **Bevy's ECS, and synchronous dataflow** (Lustre) | Systems declaring what they read and write, ordered and checked by the scheduler. Cycles only through a delay. | Parallelism by default: deterministic order first, parallel later (§5.4). |

## 4. The Project

### 4.1 What a Project Holds

A project is a folder:

```
<project>/
├── project.json     the manifest: meta, sources, resources, layers, scripts, scenes, plates
├── scripts/         its scripts: .js (in a worker), .py and .cpp (on the server, ADR 0079)
├── data/            files brought in, by their content's hash
├── captures/        frozen captures from Explore: their recipes, and previews
└── exports/         figures and films made, with the package that draws them again (§9.3)
```

`project.json` follows a schema (`noetarium.project/1`), validated before anything is drawn, as ADR 0053's expressions are. Its parts:

| Part | What |
|---|---|
| `meta` | title, authors, licence, the software's commit, created and saved |
| `sources` | where data come from (§4.3), versions pinned where frozen |
| `resources` | the shared content (§7): named, typed values with their writers |
| `layers` | entities with components (§4.4), in a tree of groups |
| `scripts` | systems and transforms (§8), each with its declared access |
| `scenes` | render setups (§4.2), each naming the layers it draws |
| `plates` | pages for print (§9.1), each framing scenes, 2D maps and charts |

### 4.2 Scenes

A scene is what one renderer draws:

- **its camera or cameras:** perspective or orthographic, named viewpoints, and keyframes (§9.4);
- **its clock:** a moment, a span, a rate, and a fixed tick for simulations (§6.2);
- **its lighting:** the Sun at the clock's moment, or fixed;
- **its theme:** day, night or print;
- **its base:** the globe with its imagery and terrain, a city from the world's tiles, a frozen capture from Explore, a 2D plane in a stated projection, or nothing;
- **the layers it draws**, by reference, so that one layer can stand in several scenes.

A project may hold several scenes: the city at 1:5,000, the region, and the world, each with its own camera, framed side by side on a plate.

### 4.3 Sources

A source says where data come from, and is never a copy of them, except where freezing needs one.

| Kind | What | Version pinned by |
|---|---|---|
| `capture` | a frozen view from Explore (§4.5) | the capture itself |
| `map2d` | features of the 2D map: countries, coasts, places, a city's footprints and ways | the product's file and hash |
| `record` | a record of the line of time: events, tracks, cells, grids, flows | its content's hash |
| `library` | a record or table of the data library | the product's hash or build stamp |
| `plugin-layer` | a plug-in's layer with its parameters (§4.6) | the plug-in's commit and the products it reads |
| `answer` | a toolbox answer: its blocks, values by row, measures for each place | the request's hash and the engine's commit |
| `file` | a file dropped or chosen | its hash, kept in `data/` |
| `script` | what a transform outputs (§8) | its source's hash and its inputs' versions |

### 4.4 Layers as Entities

A layer is an entity, and what it is depends on its components:

| Component | What |
|---|---|
| **source** | the data it draws: a source of §4.3, or another layer's output |
| **transforms** | records to records, in order: a filter, a window of time, an aggregation onto cells, a toolbox tool, a script |
| **mark** | how it is drawn, after ADR 0053: columns, flats, drapes, ribbons, lines, points, blooms, threads, arrows, arcs, tubes, labels, surfaces |
| **style** | the channels: colour (a validated map, scale and domain), height, size, opacity, labels |
| **placement** | anchored on WGS 84, on the ground or at a height, or in the scene's local frame |
| **queue** | its render queue and its place in it (§6.3) |
| **mask** | a region or a window of time it is restricted to |
| **legend** | its entry: title, unit and ramp, written from its style |
| **behaviour** | systems attached to it (§8) |

The kinds the tree shows are combinations of these:

| Kind | What it is |
|---|---|
| **data layer** | a source, transforms, a mark and a style |
| **group** | layers drawn together, with an opacity and visibility of their own |
| **adjustment layer** | a screen-space pass on what lies beneath it in the tree: grading, fog by distance, dimming, desaturation |
| **annotation layer** | text, arrows, callouts and leaders anchored at places |
| **plug-in layer** | a plug-in's layer run in the scene (§4.6) |
| **script layer** | a layer whose marks a system declares (§8.4) |

### 4.5 Capturing the Main View

This is the user's QGIS idea. A **capture** freezes the main view of Explore into a source that a project's scene can take as its base or as a layer. It copies, by value:

- the camera: position, orientation, field of view;
- the moment and its window;
- the Sun and the theme;
- the city's form and solidity;
- the layers shown, with their parameters and styles.

It also pins the version of every source drawn:
- **products, records, answers:** by hash;
- **the Earth's imagery:** fixed mosaics (Sentinel-2 cloudless 2016, Black Marble);
- **Overture's buildings:** the dated release;
- **services on demand** (Space2Stats, Data360): the keys of the cache.

A capture can be of the whole view, or of a rectangle or polygon drawn on it, which becomes its extent. Capturing at several moments of the line of time gives a row of captures, panels across time. A capture is not a screenshot: the Studio draws it again from its recipe at any size, and a preview image is kept only for speed.

### 4.6 Plug-in Layers in a Project

Plug-ins draw Explore, and stay its drawers. A project uses what they draw in one of three ways, best first:

1. **Run in the project's renderer.** A plug-in layer is code that draws through what it is given (`ctx.earth`, `ctx.gis`, `ctx.clock`, `ctx.area`). A plug-in that declares itself portable (`portable: true`) receives the project's own renderer, clock and place, and draws into the project's scene at any resolution. Its parameters are set in the inspector if it declares them.
2. **Its outputs as data.** A layer that declares `outputs()` gives the records it draws. The project draws them with its own marks and style, and its scripts can read them.
3. **As an image of the capture.** This is the last resort: a plug-in that is neither of the above is drawn as the capture's image at the capture's resolution. The preflight says so (§9.2).

Plug-ins adopt the first two one at a time; none is obliged to.

## 5. The World and Its Schedule

### 5.1 Entities, Resources, Systems

A running project is a **world**:

| Part | What |
|---|---|
| **entities** | the layers (§4.4), and things the systems create, such as the agents of a simulation |
| **components** | what an entity carries: its source, mark, style, placement, state |
| **resources** | the shared content (§7): named, typed values that belong to no entity |
| **systems** | the scripts with a lifecycle (§8), each declaring what it reads and writes |
| **transforms** | pure functions, records to records, run when their inputs change and cached (§8.1) |

### 5.2 The Schedule

A system declares its access:

```js
reads:  { cafes: api.layer('events'), radius: api.resource('number') },
writes: { crowd: api.resource('cells') },
phase:  'tick',          // or 'update', 'late', 'event'
after:  ['arrivals'],    // optional: a hint, only to break a tie
```

From these declarations the scheduler builds a graph for each phase. A system A precedes a system B when A writes what B reads, when B reads A's output, or when B names A in `after`. It then orders the systems by a topological order of that graph, the same every time:
- ties are broken by the layers' order in the tree, then by name;
- **conflicts are refused when they are made, never at the run:**
  - two writers of one resource in one phase, unless their merge is declared (§7.3);
  - a cycle that passes through no `previous()` read.

The refusal names what conflicts.

The schedule is shown in the Studio, each phase's systems in order with their time in the last frame, after Unity's profiler and Cosmopoiesis's SCHED page.

### 5.3 Two Kinds of Time

- **The clock's time** is the scene's moment: the line of time, the Sun, records' marks.
- **The tick** is a fixed step of the clock, Δt, in which simulations advance (Unity's fixed update). A frame runs as many ticks as the clock has advanced. The same ticks run whatever the frame rate, so a simulation is the same on any machine.

### 5.4 Determinism First

Systems run one at a time, in the schedule's order. Two systems whose access is disjoint could run in parallel in workers. Since the order of their effects would not change, a later stage may do so, without changing what is drawn.

## 6. The Render Loop

### 6.1 The Phases of a Frame

| Phase | What happens |
|---|---|
| 1. **input** | the edits and interactions of the frame (the camera moved, a pick, a slider) are applied as commands, at its start and only then |
| 2. **clock** | the scene's clock advances if it is playing; the ticks owed are counted |
| 3. **ticks** | the tick systems run once for each tick owed, in schedule order |
| 4. **updates** | the update systems run, then the late updates (what follows the camera, labels), in schedule order |
| 5. **commit** | results that have arrived from workers and the server (transforms, Python and C++) are committed now and only now, so that a frame never sees half its data new |
| 6. **sync** | the scene's marks are patched from what changed. A large rebuild is spread over frames within a budget, the old drawing kept until the new one is whole, so that nothing flickers. |
| 7. **render** | the passes, in the queues' order (§6.3) |
| 8. **capture** | on export only: the frame drawn at its size, in tiles if larger than the graphics card allows (§9.1) |

### 6.2 When It Runs

- **On demand.** By default a scene is drawn again only when something has changed (an edit, a result, the camera). An idle project costs nothing.
- **Playing.** With the clock playing, or a system that asks for every frame, it runs continuously.
- **Offline**, for export:
  - the clock steps exactly, frame n at t₀ + n/fps;
  - each frame waits until it has **settled**: no transform pending, every tile in view loaded, every system run;
  - seeds are fixed.

  An exported figure or film is then determined by the project, its sources' versions and the commit (§9.3).

### 6.3 Render Queues

What is drawn is drawn in queues, after Unity's, with the layer tree deciding order within a queue:

| Queue | What |
|---|---|
| **background** | sky, space, the stars |
| **base** | the globe's terrain and imagery, a city's ground and buildings, a capture's base |
| **opaque** | layers' solid marks (columns, solids, tubes), drawn with depth |
| **transparent** | layers with opacity, drawn bottom-up in the tree's order, and within a layer from far to near |
| **overlay** | lines, labels and annotations drawn above. Each layer says whether it is hidden behind what stands in front, or seen through it, as the threads of ADR 0079 are. |
| **post** | adjustment layers, as screen-space passes in the tree's order: grading, fog, dimming within a mask |
| **page** | on a plate only: legends, scale bars, north arrows and text, drawn as vector, never into the scene |

A layer may name its queue and a place in it. By default its mark decides the queue.

### 6.4 The Controller

The user asked for a controller. It is the project's director: everything that starts, stops and paces a project's running is its job.

| It controls | What |
|---|---|
| **transport** | play, pause, step a tick or a frame, the rate, a span to loop, and record (the offline loop, §6.2) |
| **the schedule** | runs the phases of §6.1 in the scheduler's order; enables, disables or runs a system alone |
| **the kernels** | the JavaScript worker, and the Python and C++ kernels (§8.7): starting, compiling, reloading, restarting after a crash, and how each keeps time (free-running or lockstep) |
| **the stream** | the one channel by which the kernels' outputs reach the page, and the commit of what has come (phase 5) |
| **bindings** | controls bound to resources: sliders and keys on a control surface of the dock; later a gamepad (the browser's Gamepad API) or a MIDI controller's knobs (Web MIDI), to steer a scene live |
| **automation** | the movements of bound controls recorded as keyframes, so that a film of a steered scene can be drawn again exactly |

It is shown as the Studio's transport, at the head of the page, and as a panel of the dock. The panel shows each kernel's state (compiling, running, *behind by n ticks*, crashed), its compiler's or interpreter's messages placed at their lines in the editor, the schedule, and the bindings.

*Remark on the word.* "Controller" can mean the project's director, as here. It can also mean the controls that steer a scene live, as in TouchDesigner or a game. The design gives both, the second as the bindings. §12 asks which the user meant first.

## 7. Shared Content

The user asked for scripts to share content. Shared content is a set of **resources**: named, typed values that belong to the project, not to any one script.

### 7.1 What a Resource Can Be

| Type | Examples |
|---|---|
| **variable** | a year, a radius, a threshold, a colour, a place, a moment |
| **record** | events, tracks, cells, grids, flows: the population of a simulation, a selection |
| **table** | columns of numbers and text |
| **colour map and domain** | shared by two panels, so that they read on one scale |
| **geometry** | a region drawn, a route, a mask |
| **image** | a texture, a raster made by a script |

Each is saved with the project if marked `saved`, and its value at a capture or export is recorded with it.

### 7.2 Who May Write

- **One writer in a phase.** A resource has one writer in each phase: a system, the inspector, or a control on a plate (a slider). The writer comes before every reader in the schedule, so that readers see the value of this tick or frame.
- **A previous value.** A system may read `previous(resource)`, the value at the end of the last tick, whatever writes it now. This is the only way a loop may close (§7.4).

### 7.3 Several Writers, Order Not Mattering

Several systems may write one resource only if it declares a **merge** that is a commutative monoid. Each writer contributes, and the merge combines the contributions in any order:
- a sum (counts, flows);
- a minimum or maximum;
- a union (of sets, of selections);
- a histogram added.

Since the merge is associative and commutative with an identity, the result does not depend on which writer ran first. The scheduler therefore treats such writers as unordered among themselves, all before the readers. A merge that is not commutative (a last write, a concatenation in order) is refused, with this reason.

### 7.4 Feedback

A simulation's state feeds itself. Agents move by where they were, and a crowd grows from the last crowd. Such a loop is allowed when it passes through a `previous()` read, a delay of one tick: in the formal terms of synchronous dataflow (Lustre's `pre`), every cycle of the dependency graph carries a delay. A loop without a delay has no order that could compute it, and is refused when made.

### 7.5 Why This and Not a Free Store

A store any script writes at any moment is easier to write for. But:
- its readers see whatever the last writer left, so the drawing depends on the order the scripts happened to run in;
- a loop of writes need never settle;
- nothing can say which script a figure's value came from.

The design keeps what such a store offers, scripts that coordinate through shared values, while keeping the drawing a function of the project.

## 8. Scripts

### 8.1 Transforms and Systems

| Kind | Runs | State | For |
|---|---|---|---|
| **transform** | when its inputs or parameters change; cached by their hash | none | analysis: records to records; the ADR 0079 contract with named inputs |
| **system** | in a phase of each frame or tick | its own, and the resources it writes | simulation, animation, interaction, drawing |

Both have the same form, and the inspector builds their parameters' form from what they declare.

### 8.2 What a Script Is Given

| Input | What |
|---|---|
| **layers** | records from layers or transforms, typed by form, through declared ports |
| **resources** | the shared content it declares it reads, or reads as `previous()` |
| **parameters** | numbers, choices, colours, places and moments, stored in the project |
| **the context**, read-only | the scene's clock (moment, window, tick, Δt), its camera and extent, its place and local frame, the Sun, the theme, the selection. A script is told of a change only in what it declares it watches. |
| **services**, recorded | queries of what the scene holds at a place: `heightAt(lon, lat)`, `buildingsIn(box)`, `nameAt(lon, lat)` (by the naming policy), `localFrame(lon, lat)`. Each query and its answer's version is recorded, so that a capture pins what the script saw. |

"What happens on the Earth now", in a project, is therefore what its scene holds and what its sources say. Today's earthquakes or weather come as sources, the moment as the clock, and the ground as a service, each declared and so each pinned when a figure is made.

### 8.3 What a Script Gives

| Output | Used by |
|---|---|
| **records** | other layers and scripts, and its layer's mark |
| **resources** written | other scripts, the inspector, controls on a plate |
| **marks**, declared (§8.4) | the renderer, live and on export |
| **annotations** | the annotation layer, and the plate |
| **charts and tables** | the inspector, and chart frames on a plate |
| **notes** | the inspector, and a caption if asked |

### 8.4 Drawing at a Place

A system draws at longitudes and latitudes by **declaring marks**; the host draws them:

```js
export default (api) => api.system({
  title: 'A crowd that gathers at the cafés',
  reads:  { cafes: api.layer('events'), radius: api.resource('number') },
  writes: { crowd: api.resource('cells', { merge: 'sum' }) },
  phase: 'tick',
  tick(ctx, { cafes, radius, crowd }) {
    // … advance the crowd by one tick, from previous(crowd)
  },
  draw(ctx, { crowd }) {
    return [api.marks.columns('The crowd', {
      at: crowd.rings, time: ctx.clock.moment,
      channels: { colour: { values: crowd.values.walkers, map: 'plum', scale: 'sqrt', unit: 'walkers' }, height: { values: crowd.values.walkers, top: 120 } },
    })];
  },
});
```

`draw` returns the marks; the host patches the scene when they change (phase 6). A declared mark can be:
- drawn again at any resolution;
- captured and exported, as vector where it is flat;
- checked by the preflight;
- drawn on the graphics card with the moment as a uniform.

A drawing call made every frame can do none of these. When a need for a mark the host lacks recurs, it becomes a new mark of the host (CLAUDE.md §2.2).

### 8.5 Where Scripts Run

| Language | Where | Kinds |
|---|---|---|
| **JavaScript** | the project's worker, one for the project | transforms and systems. A system's tick and draw cross to the page as messages, its marks as transferable buffers, within the frame's budget. |
| **Python, C++** | a kernel each, kept running for the project on the server (§8.7) | transforms and systems, live; outputs streamed into the project |

What a script may touch: its declared inputs, its resources and the context, never the page, the renderer or the network.
- **JavaScript.** A project opened from elsewhere can draw and compute in its worker, but cannot act with the page's authority (ADR 0079 §2.6, ADR 0029).
- **Python and C++.** A kernel is a process on the machine, with the authority of whoever started the server. It therefore runs only for a project the user has marked trusted (§8.7).

### 8.6 Live

With *live* on, a transform runs again on each change of its code, parameters or inputs, after a short pause, and a run that is overtaken is cancelled. A system is reloaded with its state kept where its declarations still match, and restarted where they do not, saying so. The code editor is CodeMirror 6 (MIT), from jsDelivr at a pinned version, with highlighting, completion of the API, and errors marked at their lines.

### 8.7 Kernels: C++ and Python Live

The user asked for C++ and Python to be compiled or interpreted live, their output shown, or drawn in the project, as they run. A process started for each run, as ADR 0079's host does, cannot do that: it starts from nothing, keeps no state, and C++ compiles anew each time. Each language therefore has a **kernel**: a process kept running for the project, which keeps its scripts' state between runs and reloads a script as it changes.

| | Python kernel | C++ kernel |
|---|---|---|
| **Its process** | a CPython kept running on the server (`python -I`), with numpy and the infrastructure's `science` package | a host program kept running in WSL (or natively), built once, loading each script as a shared library (`dlopen`) |
| **When code changes** | the module reloaded in place (`importlib`), its state handed over where its declarations still match | the script compiled alone as a shared library against a precompiled header of the API and the foundations it includes, then swapped in; its state handed over through `save` and `load` if it declares them |
| **Time to see a change** | a fraction of a second | a few seconds, the target, to be measured at S2. The precompiled header is what makes it possible: a whole engine takes over a minute, and one script against the full include path took 23 s after ADR 0079's fix. |
| **What it can be** | transforms, and systems at any tick its speed allows | transforms, and systems at native speed |
| **What it outputs** | as JavaScript's: records, resources, declared marks, charts, notes, and text on its console. A Matplotlib figure written as SVG becomes a vector chart. | the same, through the C++ API |
| **When it fails** | the exception at its line, in the editor; the kernel carries on | a compile error at its line. A crash restarts the kernel, from the last checkpoint of its state if it keeps one. |

**Keeping time.** A kernel is a process apart, and the page's frame cannot wait on it without stalling. Each kernel runs in one of two ways, chosen for each:

- **Free-running.** It runs on its own clock, each output stamped with the tick it belongs to. At phase 5 the controller commits the latest output whose tick has come, and shows how far behind it is. Live work runs this way, so what a frame shows depends on timing.
- **Lockstep.** The controller sends tick n and waits for its output before drawing frame n. It is slower, but determined. The offline loop (§6.2) always runs the kernels in lockstep, so that a film or figure does not depend on how fast a process happened to be.

**The stream.**
- **One stream per project.** The kernels' outputs reach the page on a single response held open, of binary frames: a header (kernel, script, tick, kind, length), then the payload as typed arrays, not JSON.
- **One connection.** The stream holds one of the six connections a browser gives the server. ADR 0080 found what happens when they run out.
- **Control the other way.** Starting, reloading and stopping go to the server as short requests.

**Who may run.** A kernel runs with the authority of whoever started the server, as ADR 0079's scripts do:
- only at this computer's own address, behind ADR 0079's guard;
- only for a project the user has marked **trusted**, by a key in the project's inspector.

A project opened from elsewhere starts untrusted: its JavaScript runs in the worker, and its Python and C++ wait.

*Alternatives considered:*
1. **Python in the page, by Pyodide** (CPython compiled to WebAssembly, MPL 2.0), in the worker. It is sandboxed and needs no server, and numpy and SciPy are there. But the infrastructure's C++ is out of reach until bound for WebAssembly, and it takes a few seconds to start. It is a good second way for untrusted projects (§12).
2. **C++ interpreted by Cling or clang-repl** (LLVM). A change would be seen at once, but it is a large dependency that is not on the development machine.
3. **C++ compiled to WebAssembly by Emscripten**, run in the worker. It is sandboxed and fast, but not installed, and the foundations are untried under it.

The design starts with the kernels and leaves these open.

## 9. Plates and Publication

### 9.1 Plates

A plate is a page of a project: QGIS's print layout, in the workbench's hand. A project may have several, the figures of a paper.

| Element | What |
|---|---|
| **page** | size in millimetres (a journal's widths offered: 89 and 183 mm, or 3.5 and 7 in), DPI (300, 600), margins, a grid |
| **scene frame** | a scene drawn through one of its cameras, at the frame's resolution, following the scene or locked to a moment |
| **map frame** | a 2D map in a stated projection (UTM by geodesy, Equal Earth, a city's local frame), as vector |
| **chart frame** | a toolbox answer's or a script's chart, as vector |
| **legend** | written from the layers' styles and units |
| **scale bar**, **north arrow** | from the frame's projection or orthographic camera |
| **inset** | a locator map, the main frame's extent outlined |
| **text**, **letters**, **credits** | the title, the caption, panel letters, and credits written from the sources' licences |
| **controls** | a slider or key bound to a resource, for figures read on a screen |

Export:

| Format | How |
|---|---|
| **PNG, TIFF** | each scene frame drawn offscreen at its pixel size, in tiles by offsetting the projection where larger than the graphics card allows, supersampled, composited with the vector elements |
| **SVG** | the page as vector, scene frames embedded as images, fonts embedded, after the 2D map's existing figure |
| **PDF** | from the SVG page (§12) |
| **film** | WebM by the page's MediaRecorder, or PNG frames, from the offline loop (§6.2) |

### 9.2 Preflight

A preflight check runs before every export. It warns, and refuses only what would mislead:

- type below 6 pt at print size;
- a colour map not validated for colour-vision deficiencies, or a categorical map on ordered values;
- a legend without units;
- a scale bar on a perspective frame, true only at one depth (it offers the orthographic camera, or removes the bar);
- a map frame with no projection stated;
- a place named against the naming policy (ADR 0069);
- credits missing for a source drawn;
- a plug-in layer drawn only as an image of a capture (§4.6);
- a capture whose sources have since changed (keep, or refresh).

### 9.3 The Package, and Drawing Again

An export writes, beside the figure, the package that draws it again:
- the project at that moment, with its resources' values;
- a manifest of its sources: version, hash, licence, retrieval;
- the software's commit.

Drawn again headless from the same commit, it gives the same figure. Only the renderer's own arithmetic (rounding on the graphics card, anti-aliasing) may differ by a pixel between machines. The check holds it to a tolerance, not to identity, and says so.

## 10. The Interface

### 10.1 Explore Gains One Key

*Capture to a project*, beside the view's keys. Nothing else in Explore changes.

### 10.2 The Studio

```
┌ The Studio · Walkers and the weather ──── scene: City, 1:5,000 ▾ ── ▶ ‖ ⏭ · live ● ── export ┐
│ ┌ Project ───────────────┐                                       ┌ Inspector ────────────┐ │
│ │ Scenes                 │                                       │ The crowd   system    │ │
│ │   ▸ City, 1:5,000      │                                       │ phase   tick, 10 Hz   │ │
│ │   ▸ The region         │      the scene's own renderer         │ reads   cafes (layer) │ │
│ │ Layers                 │      (its camera, its clock)          │         radius        │ │
│ │   ◉ The crowd    script│                                       │ writes  crowd (sum)   │ │
│ │   ◉ Cafés        data  │                                       │ radius  ──●── 150 m   │ │
│ │   ◐ Dim the base  adj  │                                       │ mark    columns, plum │ │
│ │   ◉ Capture a    base  │                                       │ queue   opaque        │ │
│ │ Resources              │                                       └───────────────────────┘ │
│ │   radius = 150 m       │                                                                 │
│ │   crowd  cells, sum    │                                                                 │
│ │ Scripts · Plates       │                                                                 │
│ └────────────────────────┘                                                                 │
│ ┌ crowd.js ──────────────────────── console · schedule · table ─────────────────────────────┐ │
│ │ ticks   arrivals → the crowd → dispersal        updates  labels        2.1 ms of 16.7     │ │
│ └────────────────────────────────────────────────────────────────────────────────────────────┘ │
│ ─────────────────────────── the scene's clock and its line of time ────────────────────────── │
└──────────────────────────────────────────────────────────────────────────────────────────────┘
```

- **The project tree** (left) holds scenes, layers, resources, scripts and plates.
- **The viewport** (centre) is the chosen scene's renderer. Choosing a plate turns it into the page.
- **The inspector** (right) shows what is chosen, one thing at a time.
- **The dock** (bottom) holds the code, the console, the controller's panel (§6.4: the kernels, the schedule with each system's time, the bindings), and the attribute table.
- **The transport** at the head of the page is the controller's: play, pause, step, rate and record. It is Unity's play mode, here *live*.

### 10.3 Rules Kept

All the workbench's rules hold:
- square corners, hairlines and vellum sheets;
- serif for titles, IBM Plex for labels and figures;
- pink and purple, the data's colours kept apart from the interface's;
- no pop-ups, one thing at a time;
- places named only by the data library's policy, the areas in dispute unnamed and uncoloured.

## 11. Where the Code Goes, and in What Stages

### 11.1 Where

| Part | Where | Why there |
|---|---|---|
| The project's schema and validation; scenes; layers as entities | `visualization/scene/project.js` | device-neutral: the scene model made persistent (ADR 0053) |
| The world, the scheduler, resources and their merges | `visualization/scene/world.js`, `schedule.js` | pure and testable in Node; any application could run a project |
| Plates: layout, legends, scale bars, preflight | `visualization/scene/plate.js`, `preflight.js` | device-neutral rules |
| Rendering a plate to SVG; PDF | `visualization/renderers/svg/` | a renderer, beside `raster.js` and `three/` |
| A scene's renderer instance; offscreen capture in tiles; queues and passes | `visualization/js/gis` (`scene-host.js`, `capture.js`) | the 3D map's libraries, instanced again |
| The Studio page: tree, viewport, inspector, dock, plate canvas, history | `applications/urban-science-workbench/studio/` | the application's interface |
| The project worker, with the scripts' API | `applications/urban-science-workbench/studio/worker.js` | the trust boundary |
| The controller: transport, kernels' life, the stream's commit, bindings, automation | `applications/urban-science-workbench/studio/controller.js` | the application's runtime |
| The kernels: the Python kernel, the C++ host with its precompiled header and loading of shared libraries | `applications/urban-science-workbench/studio/kernels/` | processes the server keeps, as it keeps the plug-ins' engines |
| The scripts' API in Python and C++ (systems, transforms, marks) | `api/python/science/studio.py`, `api/cpp/studio_plugin.hpp` | beside ADR 0079's contract, the same names in each language |
| Projects on the server; Python and C++ transforms | routes `/projects/…` in `serve.py`, with the studio's guard; ADR 0079's host | the same trust as today |
| Capture to a project; the plug-ins' adapter (`portable`, `outputs`) | the workbench's shell and plug-in loader | Explore's side |

### 11.2 Stages

Each stage ends with something a person can use, checked, with pictures in the gallery.

| Stage | Delivers | Checked by |
|---|---|---|
| **S0. Projects and capture** | The project's folder and schema. The Studio page with one scene, its own renderer and base. *Capture to a project* in Explore. Layers from records and answers. A PNG at 300 DPI with its legend and credits. | The project's round trip. A capture drawn again within tolerance after Explore has moved. The export's size. |
| **S1. The world and its schedule** | Entities, resources, systems. The scheduler with refusals and its view. The loop's phases, on demand and playing. Render queues. The controller's transport and bindings. The project worker. History and undo. | The order follows the declarations. Each refusal (two writers, a cycle without delay). Merges commute. The same ticks at any frame rate. |
| **S2. Scripts live, in three languages** | Transforms and systems in JavaScript, live. The Python kernel, reloading in place. The C++ kernel, with its precompiled header and its scripts swapped as shared libraries. The stream. The controller's panel, its kernels free-running or in lockstep. CodeMirror. Console. Services. | Exactly what depends on a change runs. An overtaken run is cancelled. A reload keeps state where it can. A C++ change seen within its target time, measured. Lockstep gives the same ticks as a run in one process. A crashed kernel restarts and says so. |
| **S3. Plates** | Scene, map and chart frames; legend, scale bar, north arrow, inset, letters, credits, controls. PNG, TIFF, SVG, PDF. Preflight. The package. | Dimensions in mm at a DPI. Each preflight rule. A figure of a paper drawn end to end, and again from its package. |
| **S4. Time and film** | Small multiples, keyframed cameras, the offline loop, film. | Frames at their moments. A film drawn twice the same within tolerance. |
| **S5. Depth** | Plug-ins made portable one at a time (Street Oceanography and the territory first). Presets. Selections and the attribute table. An expression language. Parallel systems. | Each as it comes. |

S0 shows the idea whole: capture what one sees in Explore, open it as a project of its own, and get a figure that can be drawn again.

## 12. Decisions Left to the User

1. **Names.** The design uses *the Studio* for the workspace, *project*, *scene*, *plate* (a page for print), *capture*, *resource*, *system* and *transform*. The existing plug-in *The studio* (ADR 0079) would retire into it, its library and scripts becoming a project's sources and scripts. Keep these names, or choose others.
2. **A page of its own, or a mode.** Recommended: a page of the same application (`/studio/`), for the isolation the user asked for. The alternative is a full-screen mode in the same page.
3. **Where projects live.** Recommended: beside the research they serve, in `research/<project>/studio/<name>/`, committed (the manifest and scripts are small; data are never committed). The alternative is the cache, private and uncommitted.
4. **Shared content's rules** (§7). Recommended: one writer a phase, commutative merges for several, feedback through `previous()`. The alternative, a free store, is simpler to write for, but lets the order of running decide the drawing.
5. **How far a script may reach** (§8.5). Recommended: a worker, with declared access only. The alternative, the page's full authority, would let a project opened from elsewhere act as the user.
6. **Kernels for Python and C++** (§8.7). Recommended:
   - kernels kept running on the server, for trusted projects;
   - free-running while live, and in lockstep for export;
   - C++ by g++ with a precompiled header and shared libraries swapped.

   Alternatives: Python in the page by Pyodide (sandboxed, without the foundations' C++), and C++ by Cling or Emscripten (not installed).
7. **PDF.** Our own small writer (an image with vector paths), or a library whose licence is compatible.
8. **An expression language** for filters and channels: a small typed language with a grammar, which morphe's formal tooling and the user's expertise both suit, or JavaScript expressions. Recommended: the grammar, at S5.
9. **The 2D map frame's projections**, at S3: UTM, Equal Earth, a city's local frame, and Web Mercator with a warning about area. Add or remove.
10. **What comes first after S0**: the world and its schedule (S1, S2), or plates and print (S3).
11. **What "controller" means first** (§6.4). The project's director over transport, schedule, kernels and stream, as designed? Or the controls that steer a scene live (sliders, keys, a gamepad, MIDI knobs), which the design gives as its bindings? Both are in the design, and the answer decides which comes first.
12. **Trust.** Recommended: a project starts untrusted, and its Python and C++ run only after the user marks it trusted in its inspector. The alternative is to trust every project in the user's own folders.

## 13. What Becomes of This Note

Once the user has decided §12, an ADR (the next free number, 0081 at the time of writing) records what is decided, and this note's status becomes *promoted*, with a link to it. The work then begins with S0.
