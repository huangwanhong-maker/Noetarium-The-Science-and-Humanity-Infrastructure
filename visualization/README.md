# visualization: Representation Layer

*Status: draft.* Edge, shared by every application. For the design, see [architecture §5](../docs/architecture.md) and [ADR 0005](../development/decisions/0005-visualization-as-representation-layer.md).

## 1. Purpose

This layer presents results to people, using:

- scientific figures;
- 3D scenes of the planets and the universe, built from real data;
- interactive views;
- techniques that do not exist yet.

Only `applications/`, `notebooks/` and `research/` may use this layer. Nothing in `foundations/`, `domains/`, `data/` or `observation/` depends on it. It may itself use the domains its `module.json` declares (today geodesy and geography), through their public interfaces, since units, time scales and frames travel with the data into the scene.

## 2. Structure

| Directory | Role |
|---|---|
| [`scene/`](scene/) | A device-neutral, serialisable description of what is shown. It carries units, time scales and reference frames. |
| [`renderers/`](renderers/) | One renderer per technique, beside the arithmetic they share |
| [`styles/`](styles/) | Colour maps and their validation; later typography and marks |

Code outside the layer reaches them through its public interface, `js/index.js`, which exports them as the namespaces `scene`, `styles` and `renderers`, and the three.js marks as `fieldMarks` and `canvasOf` (ADR 0026's rule of public interfaces, checked by `development/tools/check-dependencies.mjs`).

### 2.1 The Scene Model

A drawing of a field is a *visual expression* $\mathsf V=\mathsf M\circ\rho\circ r$ (Cosmopoiesis's formalism, Definition 3.10; [ADR 0053](../development/decisions/0053-fields-and-their-visual-expression.md)):

- a restriction $r$ of the field to a sub-base it is drawn on;
- a reduction $\rho$ of its fibre to what is shown;
- a mark $\mathsf M$, whose geometry the sub-base decides and whose channels the reduced fibre decides.

The field is given by its descriptor, as an engine serving physis's fields gives it (base, fibre with its unit, representation, provenance, parts and source). An expression is plain data, checked by the formalism's rules before anything is drawn: a vector's colour needs a reduction, a component is never drawn without its frame, a magnitude has no sign to diverge about, a label takes categorical hues.

| File | Content |
|---|---|
| `scene/grid.js` | The samplings of physis's fields (sphere, plane, volume, points): the places of their points and their orders, the same formulas as physis's `sampling.hpp` |
| `scene/samples.js` | A field's answers decoded: values (float32 in base64, or JSON), each point's quality, its reason or note; integral curves with |v| along them |
| `scene/scale.js` | Scales: linear, logarithmic with a floor, diverging about a centre, diverging and logarithmic in the size of the departure; nice ticks and decades |
| `scene/expression.js` | `express(field, restriction, options)` with the formalism's defaults; `validate(expression)` with its rules; `offers(field)`, the expressions a field's base and fibre allow |
| `scene/legend.js` | A legend as data: what is shown, its unit with the prefix of its values, its frame, its scale's ticks, the map's gradient, the provenance and parts, where it could not be read and why |
| `scene/volume.js` | The arithmetic of a field in a volume: the cells shown (all, the shell, three slices), the linear fit and its gradient, the value at the centre, the data of a level surface |

### 2.2 Styles

Every colour map is made in OKLab (Ottosson 2020) and validated in normal vision and under protanopia, deuteranopia and tritanopia (Machado, Oliveira and Fernandes 2009):

- **sequential maps** are monotone in lightness, so that their order reads without hue;
- **diverging maps** have two monotone arms, lightest in the middle, and ends apart;
- **categorical hues** are ordered so that neighbours stay apart.

The simulation is held to colorspacious's (`tests/fixtures/cvd.json`, made by `tests/make_cvd_fixtures.py`).

| File | Content |
|---|---|
| `styles/colour.js` | sRGB and OKLab, ΔE, lightness; ramps through stops, tables for textures, gradients for legends |
| `styles/palettes.js` | The warm family's maps: `plum`, `gold`, `orchid`, `sea` (sequential); `lavender-rust`, `orchid-gold`, `sea-brick` (diverging); `warm` (cyclic); `categorical`, the Complex Atlas's order, and `categorical-apart`, the same hues ordered so that neighbours stay apart in every vision (at least 0.158 in normal vision and 0.071 under each deficiency) |
| `styles/cvd.js` | The three deficiencies simulated; a map's lightness profile in every vision; the least separation of a list of colours |

### 2.3 Renderers

| File | Content |
|---|---|
| `renderers/raster.js` | The arithmetic of a renderer, without a device: a value's colour under an expression and a scale (with opacity rising from a clear low end or middle); a drape on a sphere or a plane as an image; curves as segments coloured at their ends |
| `renderers/three/index.js` | The marks in three.js, which is passed in: lines coloured along their length, cells as shaded boxes, a level surface, a drape on a sphere in the field's frame; a drape's image on a canvas |

## 3. Provisional JavaScript

Until the scene model and the renderers exist, the parts of the two atlases' drawing that any application could use live here, in `js/`, behind the public interface `js/index.js` ([ADR 0026](../development/decisions/0026-domains-as-libraries.md)):

| File | Content |
|---|---|
| `contour.js` | Isolines of a gridded field by marching squares, and its highs and lows |
| `flow.js` | Particles carried by a vector field on the sphere, to make a flow visible ([note](theory/particles-on-the-sphere.md)) |
| `units.js` | The units in which quantities are shown, and numbers written for reading |
| `tiles.js` | The tiles of an equirectangular map service that cover a region at a resolution |
| `isosurface.js` | A surface at a level of a volume (a brain, a region), as a triangle mesh, by surface nets |
| `complex.js` | Cell complexes drawn: a CW complex realised in space by stress majorisation, its 2-cells unfolded into a net, its cells as a Hasse diagram; surface words and facets read into Complex JSON; coloured graphs (the Feynman graphs of tensor models and group field theories) drawn in the plane and turned into their 2-complexes; a tetrahedron set in space from its six edge lengths (the quantum tetrahedra of spin foams); a colour for each index of a coloured graph, from the colour maps of `styles/`, which it re-exports under their old names |
| `stars.js` | The colour of a star from its colour index (B−V or Gaia's BP−RP) through its temperature (Ballesteros 2012; Mucciarelli and Bellazzini 2020) and a blackbody's sRGB (Helland 2012); first written for the Celestial Atlas |
| `globe/` | Virtual globes, exported as the namespace `globe` (ADR 0049): the geographic tile pyramid (`tiling.js`, the data library's); the figures of the Earth and the Moon with geodetic coordinates, ray hits and Cesium's horizon test and horizon culling point (`figure.js`); terrain tiles decoded and built into meshes with skirts at their edges and under the lines between their quarters, of the visible surface or the sea floor, their triangles ordered by quarter (`terrain.js`); the level of detail (`lod.js`: screen-space error with a falloff toward the horizon, frustum and horizon culling, a walk through tiles not yet loaded so that a close view asks for all its levels at once, parents drawn in the quarters their children cannot yet cover, imagery from ancestors by subregion, eviction); map services cut to the pyramid (`imagery.js`: EOX's Sentinel-2 cloudless 2016, NASA GIBS's Blue Marble, VIIRS, Black Marble, the sea's temperature and its anomaly, rain, coastlines and names, NASA Moon Trek); the camera of a virtual globe and its gestures (`controls.js`); a pool of workers that fetch and build terrain (`workers.js`, `worker.js`); rotations and paths sampled by an engine, interpolated by slerp with the Earth's spin factored out and by cubic Hermite polynomials (`motion.js`); a three.js renderer of a globe (`renderer-three.js`: camera-relative tiles in double precision, a logarithmic depth buffer, the Sun's light, lunar-Lambert shading for the Moon, the sea's glint, night lights, a field and a reference laid over, the provenance of the terrain) and of the sky (`sky-three.js`: the stars of a catalogue, the Sun's disc and the Milky Way in galactic coordinates); a body's map as the pyramid's two root tiles, finer from a WMTS (`imagery.js`, `mapLayer`); the Sun's own lighting with limb darkening; flights of the camera between bodies (`transfer.js`: the distance in its logarithm, raised to see both; [ADR 0050](../development/decisions/0050-instruments-and-their-acquisition.md)) |
| `gis/` | A city-scale GIS in three dimensions, exported as the namespace `gis` ([ADR 0060](../development/decisions/0060-streets-as-open-systems.md)): the local frame of a city, east, north and up from an origin, exactly on WGS 84 through the globe's figure (`frame.js`); rings, their orientation, containment and centroids, polylines, their nearest points, parts and resamplings, and mitred ribbons and their edges (`geometry.js`); colours of categories, magnitudes and signed values from the validated maps (`style.js`); a three.js scene of extruded buildings with their holes, ribbons of streets, flat spaces, columns at points, and for what moves: slabs (water standing in a street's channel, its level varying along it), swarms of walkers placed every frame, arrows lying on the ground and arcs over the city, each restyled without being rebuilt; picking and hover, map controls, flights, and the Sun's light and shadows from an azimuth and altitude (`renderer-three.js`, three.js and its MapControls passed in). Since [ADR 0078](../development/decisions/0078-the-whole-earth-in-the-urban-workbench.md): the Earth beneath a city, one view from the globe to a street, with columns on cells and rasters on its surface, one of whose values may be drawn as hatching fixed to the screen ([ADR 0086](../development/decisions/0086-access-to-services-and-unmet-need.md)) (`earth.js`, `prisms.js`); what something did, where: estimates at places on the diverging map, paler within two standard errors, and changes that stand from a date ([ADR 0087](../development/decisions/0087-what-a-change-did-and-around-it.md); `effects.js`); any place on Earth from a world's tiles, built off the page's thread and standing on the real ground, with geometry made elsewhere and waves along ways in the scene (`world.js`, `world-worker.js`); polygons with holes triangulated (`geometry.js`). Since [ADR 0079](../development/decisions/0079-data-in-space-and-time.md): data in space and time, its five forms, its files and its line of time (`spacetime.js`, pure arithmetic, importable in a worker), and marks that live in time on the graphics card (`spacetime-marks.js`) |
| `map/` | Maps of the sphere in the plane, a 2D GIS, exported as the namespace `map` ([ADR 0069](../development/decisions/0069-names-of-places-and-a-2d-gis.md)): the geography domain's projections in the plane of a drawing, with outlines, graticules, polygons, polylines and great-circle arcs cut at the map's edge (`projection.js`); zoom about a point, fitting and centring (`view.js`); names placed by priority without overlap, after the data library's naming policy (`labels.js`); an interactive map with its base, Natural Earth's areas in dispute hatched in no claimant's colour and unnamed, an overlay for an application's layers, zoom, pan, picking and a readout (`widget.js`); a map composed as a figure for publication, SVG with its fonts or PNG (`figure.js`); and a map of a town on its own plane, metres east and north of a city model's origin, with fitting, zoom, pan and picking at the scale of streets (`plane.js`, [ADR 0092](../development/decisions/0092-the-markets-domain-and-agora.md); Market Studio's map stands on it). First written for the Economy Atlas, whose map stands on it |
| `toolbox/` | The toolbox of quantitative economics's page ([ADR 0080](../development/decisions/0080-quantitative-economics-in-the-urban-workbench.md)): its catalogue of tools, the form each declares, the blocks of its answers and their charts, coloured by the page's stylesheet (`--tb-*`); imported by path (`js/toolbox/catalogue.js` and its neighbours), being a page's and not a scene's |
| `board/` | A board for thought, exported as the namespace `board` ([ADR 0090](../development/decisions/0090-a-board-for-thought.md)): what is thought kept apart from how it lies (`model.js`, the schema `noetarium.board/1`: notes of kinds, text, images, strokes of a pen, regions, shapes, links that state a relation, plans and chronicles, figures, tables, cards; layers above the ground, each hidden, held or made faint with what lies on it); every change an operation with its inverse, and a history (`operations.js`); the paths of links, snapping, the view, and a stroke kept in fewer points with its outline by its pressure (`geometry.js`); arrangements as functions from content to placement (`arrange.js`); plans and chronicles written as lines, with float and the critical chain (`schedule.js`, proleptic Gregorian); quantities defined on notes and combined by interval arithmetic (`quantities.js`); tables in the data library's schema, read from delimited text and gathered by name (`tables.js`); templates as data (`templates.js`); cards, live views that declare the parameters they need: a chart on the toolbox's charts, a map on the 2D GIS's projections and the styles' colour maps (areas in dispute hatched and unnamed, ADR 0069), and a module run on the board through the API's `science.board` (`cards.js`); and, in the page, its drawing and the hand's work, the pen among it (`kinds.js`, `view.js`), a rail and a panel built from what is declared (`tools.js`), the board as a picture (`export.js`) and a style sheet whose every colour is a variable (`board.css`). A work embeds it with `createBoard(host, options)` and serves its cards data and the running of modules; a subject comes to it as a plug-in. Tabula is the first work on it |
| `workspace/` | A project's workspace, exported as the namespace `workspace` ([ADR 0093](../development/decisions/0093-market-studio.md)), of no subject: the schema `noetarium.workspace/1` (sources with their columns bound to roles, slices of space and time, quantities, steps and their staleness, views, boards), instants and periods, the rows of a table within a slice and at an instant, a project's plane (`model.js`); and the views: what a chart gathers and draws of a table (bars, stacked bars, lines, points, a histogram, shares), how a column is scaled for a layer, and the view a table suggests (`views.js`). Pure, checked in Node. First used by Market Studio |
| `plan/` | A building drawn by hand, exported as the namespace `plan` ([ADR 0095](../development/decisions/0095-the-design-studio.md), [ADR 0096](../development/decisions/0096-what-stands-in-the-rooms.md)), of no work: the document `noetarium.building/1` (a site, storeys of walls, openings and labels, stairs, the pieces that stand in the rooms, and what is declared); the operations that change it, each a pure function with the document before as its inverse, and a history; a storey kept a plane graph whatever is drawn or moved; what a pointer snaps to; a catalogue of 44 kinds of furniture, fixtures and what stands in a garden, each with the clear floor it needs, its symbol and where it snaps to (`catalogue.js`); a storey's plan as SVG with its pieces, the lot and its surroundings, and what lies under a point; what stops a person walking on a storey (`walk.js`); and, on three.js given as an argument, the pieces' models built to size from eased solids with textures drawn from seeded sequences (`models.js`, `materials.js`) and the building in the round and from within, at eye height (`view.js`). What follows from a drawing is the architecture domain's, asked through the API. Since [ADR 0097](../development/decisions/0097-the-shell-of-a-drawn-building.md): the outside as the document keeps it (`exterior.js`: a roof's form, pitch, eaves, cover and gables, a cladding and its tone, the lot's boundary), claddings and covers drawn from seeded sequences, the roof with its soffit and fascia and what stands in a garden in the round, and a building's elevations as drawings, four on a sheet (`elevation.js`). Since [ADR 0098](../development/decisions/0098-the-street-a-building-stands-on.md): a street (`street.js`): the way in hand joined from a map's ways, its strips, its section drawn square across with the Sun's ray, the whole unrolled along its length, and its strips as ribbons in the round | draft: 124 checks |

Their checks, and those of the scene model, the styles and the renderers' arithmetic, run with `node visualization/tests/run-tests.mjs`.

## 4. Planned Renderers

| Renderer | Technique |
|---|---|
| `static` | Matplotlib, SVG, PDF |
| `web` | three.js (`renderers/three/`, written); WebGPU, CesiumJS |
| `native` | Vulkan |
| `immersive` | WebXR |
| `sonification` | Sound |

## 5. Style

The visual style is warm and humane, not that of a generic dashboard. The colour maps are defined in `styles/` and validated for colour-vision deficiencies by the checks; each application keeps its own main colour and its provenance hues in its own style sheet.

## 6. Prior Art

Study these before building: OpenSpace, NASA Eyes, CesiumJS, Stellarium and Celestia.
