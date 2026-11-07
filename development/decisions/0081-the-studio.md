# 0081. The Studio: Rendering Projects as Canvases of Frames, with Live Scripts in a Schedule

- **Status:** Accepted, provisionally (2026-11-08). An initial implementation, built for the user's feedback.
- **Date:** 2026-11-08
- **Decided by:**
  - **The request.** The user:
    - "Formally make it a 3D GIS system editor for making publishable figures, and data analysis, visualization", the Urban Science Workbench, keeping its present interface and adding "an editor/studio module";
    - project-based, with layers, a scheduler and shared content for scripts, "a controller, realtime compiler/interpreter (for c++ and python)";
    - a script "created while running", compiled if it is C++, put into the scheduler when played, and paused, stopped or deleted;
    - and the project "like a canvas": frames of different widths and heights for the 3D world, a 2D submap, statistical figures and whatever a script draws, its scripts reading "the spatial-temporal range data of current view".
  - **The name.** The user named it the Studio.
  - **The design.** Claude, after the design note [2026-11-07-a-3d-gis-editor-for-figures](../ideas/2026-11-07-a-3d-gis-editor-for-figures.md), which holds the full design, what the user's messages changed in it (its §0.1, §0.2), and what is not yet built.

## 1. Context

The workbench draws one view, Explore, by its plug-ins: a GIS to look at and ask questions of. A figure for a paper needs more. It must be the same figure each time it is drawn, and fixed at a size. Beside the map it holds other things: an inset of where the view lies, a chart, a scale bar. It may need code of one's own that runs while one looks.

Letting scripts draw on Explore's Earth would have tangled two purposes. A script's drawing would depend on whatever the plug-ins happen to show. It would act with the page's authority. And it could not be drawn again at another size. Scripts that share content with no rules would draw differently depending on the order they ran in.

## 2. Decision

### 2.1 A Page of Its Own

The Studio is `applications/urban-science-workbench/studio.html`, beside Explore. Explore is unchanged except for two keys beside its view keys:
- *capture* keeps the view for a project: its camera, moment, theme, layers and an image;
- *studio* opens the Studio.

The Studio has its own globe, camera, clock and loop, so no script of a project touches Explore's Earth. Only one page renders at a time, and both read through the same caches.

### 2.2 A Project Is a Canvas of Frames

A project is a folder in the data library's cache (`data/cache/urban-workbench/projects/<name>/`), holding `project.json` beside `scripts/`. Its manifest is `noetarium.project/1` (`visualization/scene/project.js`, checked in Node). It is **one canvas**: the figure the project makes, a size in pixels and a background, holding **frames**. Each frame has a place, a size, a ground (none, vellum, paper) and an order.

| Frame | What it holds |
|---|---|
| `world` | The 3D globe (one, for now), and the *layers of the world* drawn in it: data from the library, what scripts draw, captures from Explore |
| `map` | The visualization layer's 2D GIS, following the world. The footprint of the world's view is drawn on it, and the world's marks as dots. It names no places, and areas in dispute are hatched in no country's colour (ADR 0069) |
| `chart` | The workbench's SVG charts (line, bars, scatter), drawn by a script, or following a numeric resource tick by tick with no code |
| `drawing` | A script's own items (lines, circles, rectangles, paths, text) in the frame's pixels |
| `text` | Titles and captions, in the serif or in Plex |

Frames are moved and resized by hand (*arrange*). A frame a script names that is not on the canvas is made in a free corner (`placeFrame`). The export composes every frame again at a multiple of the canvas's size: the world rendered at that resolution, and the charts, maps and drawings as vector with their fonts embedded.

This collapses the design note's *scenes* and *plates* into one structure, at the user's word. Sizes in millimetres at a DPI become the canvas's later sizes, not a second structure.

### 2.3 Scripts Are Systems in a Schedule

A script is a **system**. It declares its phase (each tick, or each frame), what it reads, writes and reads as previous, its parameters and the resources it brings. The schedule (`visualization/scene/schedule.js`) orders the systems from those declarations, after an ECS engine's scheduler and synchronous dataflow:
- a writer runs before every reader of what it writes;
- a resource has one writer in a phase, unless it declares a merge that is a commutative monoid (sum, min, max, union; a number broadcast over an array). Order then cannot matter;
- a loop closes only through *previous*, the value at the end of the last tick, as Lustre's `pre`;
- among systems nothing orders, the project's order, then the id, decides, so that the schedule is the same every time;
- what breaks a rule is refused with its reason, its systems left out, and the others run.

**What a script is given:**
- the moment and the tick;
- its parameters;
- the values of what it reads and of what it reads as previous;
- the world frame's range in space (camera, footprint, box of ground seen, metres of ground to a canvas pixel);
- the frames and their sizes.

**What a script gives:**
- writes, and contributions to merged resources;
- marks on the world (points, columns, lines on WGS 84, in degrees and metres);
- `chart(frame, …)` and `drawing(frame, …)` into frames;
- printed lines.

The protocol is one in three languages: `src/studio/runtime.js` for JavaScript, `api/python/science/studio.py` and `api/cpp/studio_plugin.hpp`.

Scripts **add** to the world by declared marks. They do not reach the renderer. Declared marks keep a figure determined by its project, drawable again at any size, and they keep a script within its project. Raw drawing on the graphics card, if wanted, will be a deliberate extension for trusted projects.

### 2.4 Kernels, Live

- **JavaScript** runs in the page's worker, never in the page.
- **Python** runs in a kernel kept running by the server (`studio_kernels.py`).
- **C++** is compiled alone, as a shared library, with g++ (natively or in WSL), and loaded by a host kept running (`kernels/cpp_host.cpp`), which swaps it when it is compiled again.

A script made or saved while the project plays is loaded (compiled, for C++), its declaration read, and the schedule made again. It joins the next tick, while the others run. A running script goes on running while its new version loads or compiles. A version that does not load or compile is refused with the line of its own source, and **the version before goes on running**. State is handed over through `save()` and `load()`. An error in a tick is told with its line, and that script stops while the others run. A kernel that has crashed is started again, and its scripts are loaded anew.

### 2.5 The Controller

- **The transport:** play, hold, step, reset and rate.
- **Ticks:** fixed ticks, a tick owed when one ends following at once, so that their rate does not depend on the frames'. Ticks a slow kernel cannot keep up with are dropped and counted as lag, never queued, so that the kernels set the pace, not the page.
- **Each system's life:** loading, compiling, running, paused, stopped, failed or refused.
- **Each system's keys:** pause, resume, stop, start, reload and delete.
- **Each tick's marks** are drawn together at its end, so that a frame never shows half a tick.

### 2.6 Data, Captures and Authority

- **Data.** The data library's records (events, tracks, cells) become layers of the world, and each is also a resource that scripts may read.
- **Captures.** A capture taken in Explore gives the Studio its view and moment. Its plug-in layers stand as its image only, true only from its view, until plug-ins can draw into a project's renderer (the design note, §4.6).
- **Authority.** A Python or C++ script acts with the authority of whoever started the server. Every write and every run passes the server's guard of ADR 0079: the workbench's own page, at this computer's own address, or a token.

## 3. Consequences

- **Explore keeps working.** Its suite and a run in headless Chrome found its layers, legends and plug-ins as before.
- **Checks.**
  - The workbench's suite gains 28 checks:
    - the kernels in Python and C++: loading, ticking, state handed over, errors with their lines, a failed version leaving the one before, charts for frames, a project as seeded;
    - the JavaScript runtime, including a run reloaded midway that ends as if never interrupted.
  - The visualization layer's suite gains 17: the schedule and the canvas's manifest.
  - They found a merge that joined a number and an array as text instead of adding them.
- **New public interfaces.** `visualization/scene/index.js` exports the project and the schedule. The API gains the Studio's contracts in Python and C++.
- **Gallery.** Pictures are in the [gallery's §19](../../docs/gallery.md).
- **Not yet built,** in the design note's §0.2:
  - the offline, lockstep loop for exports and films;
  - render queues;
  - several world frames, orthographic worlds;
  - legends and credits written from the data, and the preflight;
  - sizes in millimetres at a DPI;
  - the trust of a project opened from elsewhere;
  - plug-ins drawing again in a project's renderer;
  - transforms and the attribute table;
  - the project's line of time.
- **Two names.** The plug-in *The studio* of ADR 0079 and the Studio share a name. The design note proposes that the plug-in retire into the Studio, its library and scripts becoming a project's sources and scripts. That is left to the user.

## 4. Alternatives Considered

- **Editing around Explore's map.** This was the design note's first version. Scripts would have depended on the plug-ins shown, and could not be drawn again or exported as one figure.
- **A free store of shared content.** The drawing would depend on the order scripts ran in, and loops need never settle.
- **An order of scripts set by hand.** Hand-numbered orders break silently as a project grows. Here the order follows from the declarations, and `after` breaks ties.
- **Waiting on every kernel each frame.** A slow process would stall the page. Ticks are dropped and counted instead, and only a future offline loop will wait.
- **Taking a script out of the schedule when its new version fails.** Hot reloading is for trying; a typo should not stop a running figure.
