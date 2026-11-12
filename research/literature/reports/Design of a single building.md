# No Tool Computes a Whole House, and Most of the Mathematics to Do So Is Known

*A survey made on 2026-11-11 for the design of a single building: its plan, its outside, its interior and its equipment. Three parts: the software in use, the literature, and the open data and real rules for a first case (a lot in Melbourne, Victoria). §10 states how each statement was checked and what was not.*

The software in use divides in two. **Consumer tools draw a house and compute nothing about it**: Arcadium, Planner 5D, Homestyler, Coohom, RoomSketcher and Maket state no daylight, heat or rule computation. **Professional tools compute, but on massing at the scale of a site** (Autodesk Forma: sun, daylight potential, wind, noise, carbon), or for one building type (Finch, Architechtures: multi-unit housing; qbiq, Hypar: offices). No tool surveyed covers plan, exterior, interior, equipment and performance together; none publishes which provisions of a code it checks; none states the uncertainty of a result; every building-design product is closed. Agents that act on a design are arriving (Snaptrude, Finch, Homestyler), so an assistant is not by itself new.

The literature is older and deeper than the tools suggest:

1. **Plans are combinatorial objects with exact counts.** A graph has a rectangular dual exactly when it has no separating triangle (in the four-corner form), and plans of $n$ rooms are counted by known sequences. Architects enumerated all small rectangular plans in 1976, to eight rooms, and then dimensioned them. What is not on record is enumeration inside a non-rectangular envelope or over several storeys.
2. **Learned and language-model generators give no guarantee** that a constraint holds; guarantees appear only where a solver finishes the work.
3. **Shape grammars in full generality cannot be parsed** (membership is unsolvable), so a parser must restrict the formalism. One façade grammar has been both learned and parsed (Martinović and Van Gool 2013). Explicit symmetry groups appear in the analysis of patterns and of single designs, never as a group acting on a learned grammar.
4. **Furniture has been laid out under published clearances, with suggestions offered beside the person's own arrangement** (Merrell et al. 2011): the best precedent for an assistant in a studio.
5. **Services are routed on a plan taken as given**; none found is computed with the plan.
6. **Fast building physics has open validation material**: the BESTEST reports, and EnergyPlus and Radiance, which run from Python.

For the infrastructure this suggests: a studio in which a person designs by hand; live measures held to BESTEST, EnergyPlus and Radiance; rules as declared schemes with a preset per jurisdiction carrying its version; enumeration and grammars as assistants, held to the known counts; fixtures as our own parametric objects, since no open library of them with connections exists; and costs and carbon as rates the person declares, since no open source of them was found.

## 1. The Tools Draw Designs, and Few Compute Anything about Them

### 1.1 The Commercial Products

Marks: [V] the vendor's page or documentation was opened; [S] seen only as a search snippet of a vendor page; NV not verified.

| Product | Produces | What its generative part does | Checks it states | Scale | Export |
|---|---|---|---|---|---|
| Arcadium 3D | 3D model, plans, elevations, AI images [V] | image generation over a 3D view; an external chatbot writes a JSON that it loads [V] | none; dimensions and areas [V] | room to house | GLB, STL, OBJ, PDF [V] |
| Planner 5D | plan, 3D, renders, catalogue [V] | recognises an uploaded plan; images restyled by third-party models [V] | none; a price estimator [V] | room to dwelling | DWG, DXF [S] |
| Homestyler | plan, 3D, renders; construction, plumbing and electricity drawings [V] | plan image to 3D; auto-floorplan; restyle; a chat agent [V] | none stated [V] | room to dwelling | DWG, PDF of drawings [S] |
| Coohom | plan, 3D interior, renders, cabinets, bill of materials [V] | layouts suggested for a room; furniture auto-layout [V, S] | collision between cabinet parts only [S] | room to dwelling | CAD; no whole-model export [V] |
| RoomSketcher | 2D and 3D floor plans, walkthrough [V] | a scan or PDF becomes a project; AI render [V] | none; total area [V] | dwelling | JPG, PNG, PDF only [S] |
| Maket.ai | plans, furnished layouts, renders [V] | layouts from a brief, edited in natural language [V] | none; defers to a licensed professional [V] | single house | PDF, DXF [V] |
| Finch | plans, unit mixes, areas, BIM geometry [V] | unit mix and circulation; plans from a firm's library; an agent [V] | compliance against a firm's standards (Enterprise); provisions NV [V] | multi-unit building | Revit, Rhino, CSV [V] |
| TestFit | site plans, massing, parking, yield [V] | generative site and parking layout [V] | zoning data (add-on), cut and fill, pro forma [V] | lot and site | DXF, SKP, glTF, CSV [V] |
| Autodesk Forma (Site Design) | massing, area metrics, analyses [V, blog] | fast predictions of wind and noise; massing options [V, blog] | sun hours, shadow, daylight potential, wind, noise, views, solar, microclimate, total carbon [V, blog; S] | site to district | IFC, OBJ; Revit, Rhino [V, blog] |
| Hypar | 3D space plans, layout options [V] | furniture and equipment suggestions; layout suggestions [V, S] | spaces against programme targets [V] | a floor (workplace) | PDF, Excel, Revit, Rhino, DXF/DWG [V] |
| Snaptrude | programme, massing, BIM model, drawings, renders [V] | an agent turns a prompt or site into programme, adjacencies, massing, renders [V] | daylight and illuminance (Pro); areas; solar exposure [V] | building on a site | .rvt, Rhino [V]; IFC, DWG [S] |
| Architechtures | residential building, BIM, areas, takeoff, budget [V] | geometry generated to fit entered parameters [V] | compliance with entered urban parameters; takeoff priced by the user's unit costs [V] | multi-unit building | XLSX, DXF, IFC [V] |
| qbiq | office test fits, renders, models, takeoff [V] | layouts for an existing floor plate, validated by in-house architects [V] | efficiency, privacy and daylight-exposure metrics [V] | office floor | CAD, Revit, PDF [V] |
| Higharc | home plans, estimating data, construction documents [V] | a learned model furnishes rooms in an existing BIM home; services routing "Not yet modeled" [V] | rule-based standards during design [V] | single-family house, for builders | NV |
| Esri CityEngine | procedural 3D models from CGA rules [V] | none learned; rules written by hand or by the Facade Wizard [V] | none found | building to city | DWG, FBX, glTF, IFC 2x3, OBJ, USD and others [V] |
| Streetmix | street cross-sections [V] | none [V] | a capacity estimate per lane [V] | one section | image [V] |
| Giraffe | site layouts, massing, schedules, cash flow [V] | parcel search; parametric subdivision, apartment and parking layouts [V] | feasibility; zoning as layers [V]; shadows [S] | parcel to precinct | GeoJSON, DXF, IFC, OBJ, GLB [S] |
| Interior AI, RoomGPT | a restyled image of a room [V] | image generation over a photo [V] | none | one image | image; no geometry [V] |

*Remark.* CityEngine generates forward only: a rule "takes geometry as input and alters or replaces it", and its Facade Wizard "creates CGA code one way only". No parsing of existing geometry into rules, and no learning of rules, was found in its documentation.

*Remark.* cove.tool, formerly web software for energy, carbon and cost, became in January 2025 a consultancy ("cove"); its educational licences ended on 31 December 2025; whether commercial subscriptions continue was not verified.

*Caveat.* Autodesk's product pages refused the reader (403); Forma's facts are from Autodesk's own blog and from search snippets. Hypar's are from its documentation.

### 1.2 What None of Them Appears to Do

Limited to the pages read:

- **Recover or learn rules from existing buildings.** No vendor describes parsing a building into a derivation.
- **Join a single house's layout with a physical calculation.** Analyses appear at site or massing scale, as an add-on, or as a metric.
- **Publish which provisions are checked,** or report compliance rule by rule.
- **Cover plan, exterior, interior, equipment and performance in one tool.**
- **Put geometry behind an AI render,** or state that an image stays faithful to the model.
- **State the uncertainty or provenance of a result.**
- **Export open BIM from the consumer tier.** IFC appears only in professional tools.
- **Open their source.** The exceptions are Streetmix (AGPL-3.0-or-later), Hypar's Elements library (MIT) and the original RoomGPT repository (MIT).

### 1.3 Open-Source Tools and Reference Programs

| Tool | Licence | Use to us |
|---|---|---|
| EnergyPlus | its own BSD-3-style licence with a naming clause | a reference for heat; runs from Python (`pyenergyplus`, shipped with the install) |
| Radiance | The Radiance Software License, Version 2.0 (BSD-style) | a reference for daylight; `pip install pyradiance` brings the binaries |
| OpenStudio | its own modified BSD licence | — |
| IfcOpenShell | LGPL-3.0-or-later | reading and writing IFC, as a tool |
| FreeCAD (BIM workbench) | LGPL-2.1-or-later | — |
| Topologic (topologicpy) | LGPL-3.0-or-later (PyPI) | cell complexes of buildings; comparison |
| Bonsai (formerly BlenderBIM) | GPL-3.0-or-later | cannot enter an MIT core |
| Sweet Home 3D | GPL-2.0-or-later | cannot enter an MIT core |
| Ladybug, Honeybee | AGPL-3.0 | cannot enter an MIT core |

Sources: github.com/NREL/EnergyPlus (LICENSE.txt), energyplus.readthedocs.io/en/latest/api.html, radiance-online.org/download-install/license, github.com/LBNL-ETA/pyradiance, github.com/IfcOpenShell/IfcOpenShell, github.com/FreeCAD/FreeCAD, pypi.org/project/topologicpy, github.com/ladybug-tools, sweethome3d.com.

## 2. Plans Are Combinatorial Objects with Exact Counts

Marks in §2 to §7: citations with a DOI were matched against their Crossref record unless marked [NV]. [A] the abstract or text was read; [R] the statement is from recollection of the paper, with only the citation checked: re-read before quoting.

### 2.1 A Graph Has a Rectangular Dual Exactly When It Has No Separating Triangle

**Theorem (four-corner form).** Let $G$ be a plane graph whose interior faces are all triangles and whose outer face is a 4-cycle. $G$ has a rectangular dual (rooms are rectangles, the dual is the graph of walls shared, no four rectangles meet at a point) if and only if $G$ has no separating triangle [A, Fusy 2009].

**General biconnected form** (Koźmiński and Kinnen's Theorem 3, as quoted by Kumar and Shekhawat [A]): a nonseparable plane graph with triangular interior faces is rectangularly dualizable if and only if it has no separating triangle and at most four corner-implying paths.

*Caveat.* For graphs with cut vertices the published condition is contested: Kumar and Shekhawat give a counterexample (arXiv:2102.05304, a preprint) and a note of 2024 reports "a minor error", corrected with proof [A].

*Caveat.* The theorem needs the complete adjacency graph. A programme's required adjacencies are usually a proper subgraph, which must first be completed to a triangulation (Roth et al. 1982; Kumar and Shekhawat 2021, [doi:10.1016/j.tcs.2021.04.014](https://doi.org/10.1016/j.tcs.2021.04.014)).

- Koźmiński, Kinnen (1985). Rectangular duals of planar graphs. *Networks* 15(2), 145–157. [doi:10.1002/net.3230150202](https://doi.org/10.1002/net.3230150202)
- Bhasker, Sahni (1987). A linear time algorithm to check for the existence of a rectangular dual of a planar triangulated graph. *Networks* 17(3), 307–317. [doi:10.1002/net.3230170306](https://doi.org/10.1002/net.3230170306)
- Bhasker, Sahni (1988). A linear algorithm to find a rectangular dual of a planar triangulated graph. *Algorithmica* 3, 247–278. [doi:10.1007/BF01762117](https://doi.org/10.1007/BF01762117) [A]
- He (1993). On finding the rectangular duals of planar triangular graphs. *SIAM J. Comput.* 22(6), 1218–1226. [doi:10.1137/0222072](https://doi.org/10.1137/0222072) [R]
- Kant, He (1997). Regular edge labeling of 4-connected plane graphs and its applications in graph drawing problems. *Theor. Comput. Sci.* 172, 175–193. [doi:10.1016/S0304-3975(95)00257-X](https://doi.org/10.1016/S0304-3975(95)00257-X). Each regular edge labeling gives one rectangular dual [R].
- Fusy (2009). Transversal structures on triangulations: a combinatorial study and straight-line drawings. *Discrete Math.* 309(7), 1870–1894. [doi:10.1016/j.disc.2007.12.093](https://doi.org/10.1016/j.disc.2007.12.093) [A]. That the structures of one triangulation form a distributive lattice is [R].
- Kumar, Shekhawat (2024). On the characterization of rectangular duals. *Notes on Number Theory and Discrete Mathematics* 30(1), 141–149. [doi:10.7546/nntdm.2024.30.1.141-149](https://doi.org/10.7546/nntdm.2024.30.1.141-149) [A]
- Eppstein, Mumford, Speckmann, Verbeek (2012). Area-universal and constrained rectangular layouts. *SIAM J. Comput.* 41(3), 537–564. [doi:10.1137/110834032](https://doi.org/10.1137/110834032). **A layout realises every assignment of room areas if and only if it is one-sided** [R]: the theorem behind "enumerate, then dimension".
- Felsner (2013). Rectangle and square representations of planar graphs. In *Thirty Essays on Geometric Graph Theory*, 213–248. [doi:10.1007/978-1-4614-0110-0_12](https://doi.org/10.1007/978-1-4614-0110-0_12). A survey.
- Rooms that are not rectangles, for graphs with no rectangular dual: Yeap, Sarrafzadeh (1993), *SIAM J. Comput.* 22(3), 500–526, [doi:10.1137/0222035](https://doi.org/10.1137/0222035); Liao, Lu, Yen (2003), *J. Algorithms* 48(2), 441–451, [doi:10.1016/S0196-6774(03)00057-9](https://doi.org/10.1016/S0196-6774(03)00057-9); Alam et al. (2013), *Discrete Comput. Geom.* 50(3), 784–810, [doi:10.1007/s00454-013-9521-1](https://doi.org/10.1007/s00454-013-9521-1): every planar triangulation has a contact representation by rectilinear polygons of at most 8 sides with prescribed areas [R].

### 2.2 Plans of $n$ Rooms Are Counted by Three Known Sequences

Counts of unlabelled plans, not reduced by the symmetries of the rectangle; terms read from the OEIS entries.

| $n$ rooms | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 |
|---|---|---|---|---|---|---|---|---|---|---|
| Slicing floorplans (large Schröder, A006318 shifted by one) | 1 | 2 | 6 | 22 | 90 | 394 | 1806 | 8558 | 41586 | 206098 |
| Mosaic floorplans (Baxter, A001181) | 1 | 2 | 6 | 22 | 92 | 422 | 2074 | 10754 | 58202 | 326240 |
| Generic rectangulations (A342141) | 1 | 2 | 6 | 24 | 116 | 642 | 3938 | 26194 | 186042 | 1395008 |

*Remark.* Mosaic and slicing floorplans are counted up to the sliding of walls, under which room adjacencies can change. Generic rectangulations preserve wall adjacency. **For plans that realise an adjacency graph, the third row is the relevant one.**

- Yao, Chen, Cheng, Graham (2003). Floorplan representations: complexity and connections. *ACM TODAES* 8(1), 55–80. [doi:10.1145/606603.606607](https://doi.org/10.1145/606603.606607) [A]
- Ackerman, Barequet, Pinter (2006). A bijection between permutations and floorplans, and its applications. *Discrete Appl. Math.* 154(12), 1674–1684. [doi:10.1016/j.dam.2006.03.018](https://doi.org/10.1016/j.dam.2006.03.018). Baxter permutations and mosaic floorplans [A]; separable permutations and slicing floorplans [R].
- Chung, Graham, Hoggatt, Kleiman (1978). The number of Baxter permutations. *J. Combin. Theory A* 24(3), 382–394. [doi:10.1016/0097-3165(78)90068-7](https://doi.org/10.1016/0097-3165(78)90068-7)
- Reading (2012). Generic rectangulations. *Eur. J. Combin.* 33(4), 610–623. [doi:10.1016/j.ejc.2011.11.004](https://doi.org/10.1016/j.ejc.2011.11.004). A bijection with 2-clumped permutations [A].
- Merino, Mütze (2023). Combinatorial generation via permutation languages. III. Rectangulations. *Discrete Comput. Geom.* 70(1), 51–122. [doi:10.1007/s00454-022-00393-w](https://doi.org/10.1007/s00454-022-00393-w). Exhaustive generation of rectangulation classes.
- Asinowski, Cardinal, Felsner, Fusy (2025). Combinatorics of rectangulations: old and new bijections. *Combinatorial Theory* 5(1). [doi:10.5070/C65165025](https://doi.org/10.5070/C65165025)
- Inoue, Takahashi, Fujimaki (2009). Counting rectangular drawings or floorplans in polynomial time. *IEICE Trans. Fundamentals* E92-A(4), 1115–1120. [doi:10.1587/transfun.E92.A.1115](https://doi.org/10.1587/transfun.E92.A.1115)

### 2.3 Architects Enumerated Small Plans in 1976 and Then Dimensioned Them

EPB = *Environment and Planning B*.

- **Mitchell, Steadman, Liggett (1976).** Synthesis and optimization of small rectangular floor plans. EPB 3(1), 37–70. [doi:10.1068/b030037](https://doi.org/10.1068/b030037). Generates exhaustively all topologically distinct arrangements of rectangular rooms in a rectangular outline that meet given adjacency requirements, to an "effective upper limit" of **eight** rooms; dimensions by linear equations, linear programming, nonlinear programming, or enumeration of modular dimensions [A].
- Gero (1977). Note on the above. EPB 4(1), 81–88. [doi:10.1068/b040081](https://doi.org/10.1068/b040081). Argues the procedure cannot reach the global optimum; dynamic programming can [A, snippet].
- Earl (1977). A note on the generation of rectangular dissections. EPB 4(2), 241–246. [doi:10.1068/b040241](https://doi.org/10.1068/b040241)
- Bloch, Krishnamurti (1978). The counting of rectangular dissections. EPB 5(2), 207–214. [doi:10.1068/b050207](https://doi.org/10.1068/b050207). A census "for all weights less than eleven" [A, snippet].
- Flemming (1978). Wall representations of rectangular dissections and their use in automated space allocation. EPB 5(2), 215–232. [doi:10.1068/b050215](https://doi.org/10.1068/b050215). Loosely packed arrangements: EPB 13(2), 189–205 (1986), [doi:10.1068/b130189](https://doi.org/10.1068/b130189); EPB 16(3), 327–359 (1989), [doi:10.1068/b160327](https://doi.org/10.1068/b160327).
- Grason (1971). An approach to computerized space planning using graph theory. *Proc. Design Automation Workshop*, 170–178. [doi:10.1145/800158.805070](https://doi.org/10.1145/800158.805070) [R]
- Galle (1981). An algorithm for exhaustive generation of building floor plans. *Commun. ACM* 24(12), 813–825. [doi:10.1145/358800.358804](https://doi.org/10.1145/358800.358804)
- Roth, Hashimshony, Wachman (1982). Turning a graph into a rectangular floor plan. *Building and Environment* 17(3), 163–173. [doi:10.1016/0360-1323(82)90037-3](https://doi.org/10.1016/0360-1323(82)90037-3). Non-convex envelopes: 20(4), 211–219 (1985), [doi:10.1016/0360-1323(85)90036-8](https://doi.org/10.1016/0360-1323(85)90036-8): a construction, not a census [A, partial].
- Steadman (1983). *Architectural Morphology: An Introduction to the Geometry of Building Plans.* London: Pion. ISBN 0850860865 (no DOI). Tabulates rectangular dissections to $n = 7$ (a bookseller's description).
- Rinsma (1987). Nonexistence of a certain rectangular floorplan with specified areas and adjacency. EPB 14(2), 163–166. [doi:10.1068/b140163](https://doi.org/10.1068/b140163). **Dimensioning can fail.** Also EPB 15(1), 111–118 (1988), [doi:10.1068/b150111](https://doi.org/10.1068/b150111); Rinsma, Giffin, Robinson (1990), EPB 17(1), 57–71, [doi:10.1068/b170057](https://doi.org/10.1068/b170057).
- Schwarz, Berry, Shaviv (1994). Representing and solving the automated building design problem. *Computer-Aided Design* 26(9), 689–698. [doi:10.1016/0010-4485(94)90019-1](https://doi.org/10.1016/0010-4485(94)90019-1). States the problem NP-complete [A, partial].
- **Medjdoub, Yannou (2000).** Separating topology and geometry in space planning. *Computer-Aided Design* 32(1), 39–61. [doi:10.1016/S0010-4485(99)00084-6](https://doi.org/10.1016/S0010-4485(99)00084-6). Constraint programming; a complete set of "topological solutions" is enumerated before dimensions [A]. Limits on rooms and floors not confirmed.
- Shekhawat (2018). Enumerating generic rectangular floor plans. *Automation in Construction* (AiC) 92, 151–165. [doi:10.1016/j.autcon.2018.03.037](https://doi.org/10.1016/j.autcon.2018.03.037)
- Shekhawat, Upasani, Bisht, Jain (2021). A tool for computer-generated dimensioned floorplans based on given adjacencies (GPLAN). AiC 127, 103718. [doi:10.1016/j.autcon.2021.103718](https://doi.org/10.1016/j.autcon.2021.103718). Its limit on rooms not verified.
- Shekhawat, Jain, Bisht, Kondaveeti, Goswami (2021). Graph-based approach for enumerating floorplans based on users specifications. *AI EDAM* 35(4), 438–459. [doi:10.1017/S0890060421000275](https://doi.org/10.1017/S0890060421000275) [A]. One floor.
- Bisht et al. (2022). Transforming an adjacency graph into dimensioned floorplan layouts. *Comput. Graph. Forum* 41(6), 5–22. [doi:10.1111/cgf.14451](https://doi.org/10.1111/cgf.14451)
- Shekhawat et al. (2023). Automated generation of floorplans with non-rectangular rooms. *Graphical Models* 127, 101175. [doi:10.1016/j.gmod.2023.101175](https://doi.org/10.1016/j.gmod.2023.101175). The outer boundary stays rectangular [A, snippet]. Raveena, Shekhawat (2023), A theory of L-shaped floor-plans, *Theor. Comput. Sci.* 942, 57–92, [doi:10.1016/j.tcs.2022.11.025](https://doi.org/10.1016/j.tcs.2022.11.025).
- Surveys: Liggett (2000), AiC 9(2), 197–215, [doi:10.1016/S0926-5805(99)00005-9](https://doi.org/10.1016/S0926-5805(99)00005-9); Weber, Mueller, Reinhart (2022), AiC 140, 104385, [doi:10.1016/j.autcon.2022.104385](https://doi.org/10.1016/j.autcon.2022.104385).

**What is open, as far as these sources show.** Exhaustive enumeration is on record only for rectangular rooms in a rectangle, to about 8–10 rooms, and for rectangulation classes in combinatorics. Non-rectangular rooms and L- or T-shaped outlines are treated by construction and existence theory, not by census. No enumeration over several storeys was found.

## 3. Learned Generators Give No Guarantee; Solvers Do

### 3.1 Optimisation

- Michalek, Choudhary, Papalambros (2002). Architectural layout design optimization. *Eng. Optim.* 34(5), 461–484. [doi:10.1080/03052150214016](https://doi.org/10.1080/03052150214016) [R]
- Merrell, Schkufza, Koltun (2010). Computer-generated residential building layouts. *ACM Trans. Graph.* (TOG) 29(6), art. 181. [doi:10.1145/1882261.1866203](https://doi.org/10.1145/1882261.1866203). A Bayesian network proposes the programme; stochastic optimisation makes the plans, two storeys among them [A, secondary].
- Bao, Yan, Mitra, Wonka (2013). Generating and exploring good building layouts. TOG 32(4). [doi:10.1145/2461912.2461977](https://doi.org/10.1145/2461912.2461977)
- **Wu, Fan, Liu, Wonka (2018).** MIQP-based layout design for building interiors. *Comput. Graph. Forum* 37(2), 511–521. [doi:10.1111/cgf.13380](https://doi.org/10.1111/cgf.13380). Rooms as unions of rectangles; size, position, adjacency and outline as constraints [A]; what the solver returns satisfies them [R].
- Several levels, stochastic: Rodrigues, Gaspar, Gomes (2013), AiC 35, 482–498, [doi:10.1016/j.autcon.2013.06.005](https://doi.org/10.1016/j.autcon.2013.06.005); Dino (2016), AiC 69, 131–150, [doi:10.1016/j.autcon.2016.05.020](https://doi.org/10.1016/j.autcon.2016.05.020).
- Laignel et al. (2021). Floor plan generation through a mixed constraint programming-genetic optimization approach. AiC 123, 103491. [doi:10.1016/j.autcon.2020.103491](https://doi.org/10.1016/j.autcon.2020.103491)

### 3.2 Learning

None of the abstracts read claims that constraints are guaranteed; adherence is measured afterwards, by realism, diversity, graph-edit distance to the input graph, IoU and user studies.

- Wu et al. (2019). Data-driven interior plan generation for residential buildings (RPLAN). TOG 38(6). [doi:10.1145/3355089.3356556](https://doi.org/10.1145/3355089.3356556) [R]
- Hu et al. (2020). Graph2Plan. TOG 39(4). [doi:10.1145/3386569.3392391](https://doi.org/10.1145/3386569.3392391)
- Nauata et al. (2020). House-GAN. ECCV. [doi:10.1007/978-3-030-58452-8_10](https://doi.org/10.1007/978-3-030-58452-8_10) [A]. House-GAN++ (2021), CVPR. [doi:10.1109/CVPR46437.2021.01342](https://doi.org/10.1109/CVPR46437.2021.01342)
- Shabani, Hosseini, Furukawa (2023). HouseDiffusion. CVPR. [doi:10.1109/CVPR52729.2023.00529](https://doi.org/10.1109/CVPR52729.2023.00529) [A]
- Para et al. (2021). Generative layout modeling using constraint graphs. ICCV. [doi:10.1109/ICCV48922.2021.00662](https://doi.org/10.1109/ICCV48922.2021.00662). A learned constraint graph, then an optimiser [R].
- Chang et al. (2021). Building-GAN. ICCV. [doi:10.1109/ICCV48922.2021.01174](https://doi.org/10.1109/ICCV48922.2021.01174). Several storeys, learned.
- van Engelenburg et al. (2024). MSD: a benchmark dataset for floor plan generation of building complexes. ECCV. [doi:10.1007/978-3-031-73636-0_4](https://doi.org/10.1007/978-3-031-73636-0_4)
- Leng et al. (2023). Tell2Design. ACL. [doi:10.18653/v1/2023.acl-long.820](https://doi.org/10.18653/v1/2023.acl-long.820)
- Yin et al. (2025). FloorPlan-LLaMa. ACL. [doi:10.18653/v1/2025.acl-long.331](https://doi.org/10.18653/v1/2025.acl-long.331). Argues that FID, graph-edit distance and PSNR miss domain knowledge [A].
- Lara et al. (2026). Generative floor plan design with LLMs via reinforcement learning with verifiable rewards. arXiv:2605.14117. An improvement, not a guarantee [A].
- **Xiang et al. (2026). Co-Layout.** arXiv:2511.12474. The language model only extracts constraints; an integer programme on a grid solves rooms and furniture together [A]: the nearest to enforced constraints with a language model.
- Preprints: ChatHouseDiffusion (arXiv:2410.11908); DStruct2Design (arXiv:2407.15723); Shiohara, Yamasaki (arXiv:2604.04859).

## 4. Grammars of Buildings Are Written by Hand, and in General Cannot Be Parsed

### 4.1 Shape Grammars

- Stiny, Gips (1972). Shape grammars and the generative specification of painting and sculpture. *Information Processing 71*, 1460–1465. [NV: no DOI]
- Stiny (1980). Introduction to shape and shape grammars. EPB 7(3), 343–351. [doi:10.1068/b070343](https://doi.org/10.1068/b070343). Kindergarten grammars. EPB 7(4), 409–462. [doi:10.1068/b070409](https://doi.org/10.1068/b070409): the symmetry of the shapes in a rule fixes in how many distinct ways it applies [R].
- Stiny, Mitchell (1978). The Palladian grammar. EPB 5(1), 5–18. [doi:10.1068/b050005](https://doi.org/10.1068/b050005). Counting Palladian plans. EPB 5(2), 189–198. [doi:10.1068/b050189](https://doi.org/10.1068/b050189) [R]
- Koning, Eizenberg (1981). The language of the prairie: Frank Lloyd Wright's prairie houses. EPB 8(3), 295–323. [doi:10.1068/b080295](https://doi.org/10.1068/b080295)
- Flemming (1987). More than the sum of parts: the grammar of Queen Anne houses. EPB 14(3), 323–350. [doi:10.1068/b140323](https://doi.org/10.1068/b140323)
- Duarte (2005). Towards the mass customization of housing: the grammar of Siza's houses at Malagueira. EPB 32(3), 347–380. [doi:10.1068/b31124](https://doi.org/10.1068/b31124)
- Knight (1999). Shape grammars: six types. EPB 26(1), 15–31. [doi:10.1068/b260015](https://doi.org/10.1068/b260015)

Every one of these was written by hand from a corpus; none was learned.

### 4.2 Procedural Buildings

- Wonka, Wimmer, Sillion, Ribarsky (2003). Instant architecture. TOG 22(3), 669–677. [doi:10.1145/882262.882324](https://doi.org/10.1145/882262.882324). Split grammars [R].
- Müller, Wonka, Haegler, Ulmer, Van Gool (2006). Procedural modeling of buildings. TOG 25(3), 614–623. [doi:10.1145/1141911.1141931](https://doi.org/10.1145/1141911.1141931). CGA shape [R].
- Schwarz, Müller (2015). Advanced procedural modeling of architecture. TOG 34(4), art. 107. [doi:10.1145/2766956](https://doi.org/10.1145/2766956)

### 4.3 Inverse Procedural Modelling

- Müller, Zeng, Wonka, Van Gool (2007). Image-based procedural modeling of facades. TOG 26(3), art. 85. [doi:10.1145/1276377.1276484](https://doi.org/10.1145/1276377.1276484). Rules for one façade, not the grammar of a style [R].
- Teboul et al. (2011). Shape grammar parsing via reinforcement learning. CVPR. [doi:10.1109/CVPR.2011.5995319](https://doi.org/10.1109/CVPR.2011.5995319). The grammar is given by hand; the parse is found [R].
- Riemenschneider et al. (2012). Irregular lattices for complex shape grammar facade parsing. CVPR. [doi:10.1109/CVPR.2012.6247857](https://doi.org/10.1109/CVPR.2012.6247857)
- **Martinović, Van Gool (2013).** Bayesian grammar learning for inverse procedural modeling. CVPR, 201–208. [doi:10.1109/CVPR.2013.33](https://doi.org/10.1109/CVPR.2013.33). Learns 2D attributed stochastic context-free grammars from labelled façades by Bayesian model merging, and parses with them about as well as with a hand-written grammar [A]. **The clearest case of a façade grammar both learned and parsed.**
- Wu, Yan, Dong, Zhang, Wonka (2014). Inverse procedural modeling of facade layouts. TOG 33(4). [doi:10.1145/2601097.2601162](https://doi.org/10.1145/2601097.2601162). A split grammar of least description length for a given layout [A].
- Bokeloh, Wand, Seidel (2010). A connection between partial symmetry and inverse procedural modeling. TOG 29(4). [doi:10.1145/1778765.1778841](https://doi.org/10.1145/1778765.1778841) [R]
- Zhang et al. (2013). Layered analysis of irregular facades via symmetry maximization. TOG 32(4). [doi:10.1145/2461912.2461923](https://doi.org/10.1145/2461912.2461923) [R]
- Survey: Musialski et al. (2013). A survey of urban reconstruction. *Comput. Graph. Forum* 32(6), 146–177. [doi:10.1111/cgf.12077](https://doi.org/10.1111/cgf.12077)

### 4.4 Symmetry Groups Appear in Analysis, Not Acting on a Grammar

- Liu, Collins, Tsin (2004). A computational model for periodic pattern perception based on frieze and wallpaper groups. *IEEE TPAMI* 26(3), 354–371. [doi:10.1109/TPAMI.2004.1262332](https://doi.org/10.1109/TPAMI.2004.1262332)
- Pauly et al. (2008). Discovering structural regularity in 3D geometry. TOG 27(3). [doi:10.1145/1360612.1360642](https://doi.org/10.1145/1360612.1360642) [R]
- Economou (1999). The symmetry lessons from Froebel building gifts. EPB 26(1), 75–90. [doi:10.1068/b260075](https://doi.org/10.1068/b260075)
- **Park (2000).** Subsymmetry analysis of architectural designs: some examples. EPB 27(1), 121–136. [doi:10.1068/b2462](https://doi.org/10.1068/b2462). Designs read through the subgroup lattice of their symmetry group [R]. Park (2018). Subsymmetries for the analysis and design of housing facades. *Nexus Network Journal* 20(1), 251–266. [doi:10.1007/s00004-017-0358-4](https://doi.org/10.1007/s00004-017-0358-4): the seven frieze groups and their subsymmetries as a device for composing façades [A, snippet].

No work was found that learns a building grammar, parses with it, and carries an explicit group.

### 4.5 Membership Is Unsolvable for Shape Grammars in General

- Yue, Krishnamurti (2013). Tractable shape grammars. EPB 40(4), 576–594. [doi:10.1068/b38227](https://doi.org/10.1068/b38227). A shape grammar can simulate any Turing machine, so "in general, its membership problem is unsolvable"; parametric subshape recognition is in NP [A].
- Krishnamurti (1980). The arithmetic of shapes. EPB 7(4), 463–484. [doi:10.1068/b070463](https://doi.org/10.1068/b070463). Krishnamurti (1981). The construction of shapes. EPB 8(1), 5–40. [doi:10.1068/b080005](https://doi.org/10.1068/b080005) [R]
- Wortmann, Stouffs (2018). Algorithmic complexity of shape grammar implementation. *AI EDAM* 32(2), 138–146. [doi:10.1017/S0890060417000440](https://doi.org/10.1017/S0890060417000440)
- Grasl, Economou (2013). From topologies to shapes: parametric shape grammars implemented by graphs. EPB 40(5), 905–922. [doi:10.1068/b38156](https://doi.org/10.1068/b38156)

Any parser must therefore restrict the formalism, as split grammars and lattice grammars do.

## 5. Interiors Have Published Clearances; Services Are Routed on a Given Plan

### 5.1 Furniture

- **Merrell, Schkufza, Li, Agrawala, Koltun (2011).** Interactive furniture layout using interior design guidelines. TOG 30(4), art. 87. [doi:10.1145/2010324.1964982](https://doi.org/10.1145/2010324.1964982). Read in full [A]. Clearance, circulation (a person as a disk of radius 18 in), conversation distance of 4–8 ft, balance, alignment, emphasis, as soft terms of a density sampled by Metropolis–Hastings; values from Panero and Repetto, *Anatomy for Interior Designers* (1975), and DeChiara, Panero and Zelnik, *Time-Saver Standards for Interior Design and Space Planning* (2001). **Suggestions are offered beside the person's own arrangement**, with a study of 18 participants.
- Yu et al. (2011). Make it home. TOG 30(4), art. 86. [doi:10.1145/2010324.1964981](https://doi.org/10.1145/2010324.1964981)
- Fisher et al. (2012). Example-based synthesis of 3D object arrangements. TOG 31(6). [doi:10.1145/2366145.2366154](https://doi.org/10.1145/2366145.2366154) [R]
- Learned, without stated ergonomics: ATISS (NeurIPS 2021, arXiv:2110.03675); DiffuScene (CVPR 2024, [doi:10.1109/CVPR52733.2024.01938](https://doi.org/10.1109/CVPR52733.2024.01938)); LayoutGPT (NeurIPS 2023), which reports furniture out of bounds; Holodeck (CVPR 2024, [doi:10.1109/CVPR52733.2024.01536](https://doi.org/10.1109/CVPR52733.2024.01536)), where a language model writes relational constraints and a solver treats them softly.

Of those read, only Merrell et al. and Yu et al. encode ergonomics explicitly, and only Merrell et al. cite design handbooks.

### 5.2 Services

- Deng, Li, Tian (2025). A review: the application of path optimization algorithms in building mechanical, electrical, and plumbing pipe design. *Buildings* 15(12), 2093. [doi:10.3390/buildings15122093](https://doi.org/10.3390/buildings15122093) [snippet]
- Chen et al. (2022). Rule-based generation of HVAC duct routing. AiC 139, 104264. [doi:10.1016/j.autcon.2022.104264](https://doi.org/10.1016/j.autcon.2022.104264)
- Li et al. (2025). Automated layout generation of HVAC piping from building floor plans. AiC 177, 106359. [doi:10.1016/j.autcon.2025.106359](https://doi.org/10.1016/j.autcon.2025.106359)
- Pestana, Paice, West (2024). Optimizing MEP design in early AEC projects through generative design. AiC 165, 105566. [doi:10.1016/j.autcon.2024.105566](https://doi.org/10.1016/j.autcon.2024.105566)
- Jorens et al. (2018). Air distribution network design as optimisation. *Energy and Buildings* 175, 48–56. [doi:10.1016/j.enbuild.2018.07.018](https://doi.org/10.1016/j.enbuild.2018.07.018)
- Calixto et al. (2026). Modelling and optimizing HVAC systems for early-stage building design. CP 2026, LIPIcs 379. [doi:10.4230/LIPIcs.CP.2026.10](https://doi.org/10.4230/LIPIcs.CP.2026.10) [A]

All that could be read take the plan as given.

### 5.3 Fast Building Physics and Its Validation

- **ISO 52016-1:2017**, the hourly calculation of energy needs and internal temperatures, replacing ISO 13790:2008 (whose simple hourly method has five resistances and one capacitance). Comparisons: Zakula et al. (2019), *Applied Energy* 254, 113553, [doi:10.1016/j.apenergy.2019.113553](https://doi.org/10.1016/j.apenergy.2019.113553); Michalak (2014), *Energy* 75, 568–578, [doi:10.1016/j.energy.2014.08.019](https://doi.org/10.1016/j.energy.2014.08.019).
- **ANSI/ASHRAE Standard 140** (BESTEST). The standard's text is sold; its supporting files are provided without charge [S]. Openly available: Judkoff, Neymark (1995), *IEA building energy simulation test (BESTEST) and diagnostic method*, NREL, [doi:10.2172/90674](https://doi.org/10.2172/90674); Neymark et al. (2020), *Update of ASHRAE Standard 140 Section 5.2*, Argonne, [doi:10.2172/1643690](https://doi.org/10.2172/1643690).
- **Daylight.** EN 17037:2018 (300 lx over half the space for half the daylight hours, as daylight-factor targets by latitude) [NV against the standard]; ANSI/IES LM-83-23 (spatial daylight autonomy). The split-flux method: Hopkinson, Longmore, Petherbridge (1954), *Trans. Illum. Eng. Soc.* 19(7), 201–219, [doi:10.1177/147715355401900701](https://doi.org/10.1177/147715355401900701); Tregenza (1989), [doi:10.1177/096032718902100305](https://doi.org/10.1177/096032718902100305). Radiosity: Goral et al. (1984), [doi:10.1145/964965.808601](https://doi.org/10.1145/964965.808601). Climate-based metrics: Reinhart, Mardaljevic, Rogers (2006), *Leukos* 3(1), 7–31, [doi:10.1582/LEUKOS.2006.03.01.001](https://doi.org/10.1582/LEUKOS.2006.03.01.001).

### 5.4 Space Syntax inside Buildings

- Hillier, Hanson (1984). *The Social Logic of Space.* [doi:10.1017/CBO9780511597237](https://doi.org/10.1017/CBO9780511597237)
- Benedikt (1979). To take hold of space: isovists and isovist fields. EPB 6(1), 47–65. [doi:10.1068/b060047](https://doi.org/10.1068/b060047)
- Turner, Doxa, O'Sullivan, Penn (2001). From isovists to visibility graphs. EPB 28(1), 103–121. [doi:10.1068/b2684](https://doi.org/10.1068/b2684)
- Peponis et al. (1997). Convex partitions and their local properties. EPB 24(5), 761–781. [doi:10.1068/b240761](https://doi.org/10.1068/b240761)

## 6. Design by Hand with Assistance Has Precedents, and Rule Checking a Long Literature

### 6.1 Tools in Which the Person and the Machine Both Act

- Horvitz (1999). Principles of mixed-initiative user interfaces. CHI. [doi:10.1145/302979.303030](https://doi.org/10.1145/302979.303030)
- Michalek, Papalambros (2002). Interactive design optimization of architectural layouts. *Eng. Optim.* 34(5), 485–501. [doi:10.1080/03052150214021](https://doi.org/10.1080/03052150214021). The designer edits while the optimiser runs [R].
- Harada, Witkin, Baraff (1995). Interactive physically-based manipulation of discrete/continuous models. SIGGRAPH. [doi:10.1145/218380.218443](https://doi.org/10.1145/218380.218443). A layout dragged through continuous and discrete changes [R].
- Merrell et al. (2011), §5.1.
- Marks et al. (1997). Design galleries. SIGGRAPH. [doi:10.1145/258734.258887](https://doi.org/10.1145/258734.258887)
- **Smith, Whitehead, Mateas (2011). Tanagra.** *IEEE TCIAIG* 3(3), 201–215. [doi:10.1109/TCIAIG.2011.2159716](https://doi.org/10.1109/TCIAIG.2011.2159716). Game levels: the designer fixes parts and a solver fills the rest, never contradicting the designer [R].
- Mao et al. (2024). HI-TAM. *Frontiers in Computer Science* 6. [doi:10.3389/fcomp.2024.1460381](https://doi.org/10.3389/fcomp.2024.1460381). Eight professionals made office plans with a generative assistant they trained [A, snippet].
- Negendahl (2015). Building performance simulation in the early design stage. AiC 54, 39–53. [doi:10.1016/j.autcon.2015.03.002](https://doi.org/10.1016/j.autcon.2015.03.002)

### 6.2 Rules Checked Automatically

- Fenves (1966). Tabular decision logic for structural design. *J. Struct. Div.* 92(6), 473–490. [doi:10.1061/JSDEAG.0001567](https://doi.org/10.1061/JSDEAG.0001567)
- **Eastman, Lee, Jeong, Lee (2009).** Automatic rule-based checking of building designs. AiC 18(8), 1011–1033. [doi:10.1016/j.autcon.2009.07.002](https://doi.org/10.1016/j.autcon.2009.07.002). The survey; its four stages (rule interpretation, model preparation, rule execution, reporting) are [R].
- **Solihin, Eastman (2015).** Classification of rules for automated BIM rule checking development. AiC 53, 69–82. [doi:10.1016/j.autcon.2015.03.003](https://doi.org/10.1016/j.autcon.2015.03.003). Rules classed by how much derived data they need [R].
- Beach, Hippolyte, Rezgui (2020). Towards the adoption of automated regulatory compliance checking in the built environment. AiC 118, 103285. [doi:10.1016/j.autcon.2020.103285](https://doi.org/10.1016/j.autcon.2020.103285)
- Amor, Dimyadi (2021). The promise of automated compliance checking. *Developments in the Built Environment* 5, 100039. [doi:10.1016/j.dibe.2020.100039](https://doi.org/10.1016/j.dibe.2020.100039)
- Rules read from text: Zhang, El-Gohary (2016), [doi:10.1061/(ASCE)CP.1943-5487.0000346](https://doi.org/10.1061/(ASCE)CP.1943-5487.0000346); Fuchs et al. (2024), *Adv. Eng. Inform.* 62, 102735, [doi:10.1016/j.aei.2024.102735](https://doi.org/10.1016/j.aei.2024.102735).

No work was found that joins live rule checking, enumeration and suggestion in one tool; that question was not searched in depth.

## 7. Open Plans Are Apartments, Open Fixtures Do Not Exist, and Open Costs Were Not Found

Licence names as quoted on the source pages.

### 7.1 Plans and Interiors

| Dataset | Contents | Licence | Usable |
|---|---|---|---|
| Swiss Dwellings v3.0.0 (Archilyse) | 45,176 apartments, about 3,100 buildings; vector, in metres, with simulations | "Creative Commons Attribution 4.0 International" (zenodo.org/records/7788422) | yes |
| Modified Swiss Dwellings (MSD) | 4,167 training plans; graphs and arrays | "CC BY 4.0" (data.4tu.nl) | yes |
| ProcTHOR-10K | about 10,000 procedurally generated houses | Apache-2.0 | yes |
| ResPlan | 17,000 plans, vector with room graphs | "CC BY 4.0 (data) and MIT (code)", from undisclosed listings | with caution |
| HouseExpo | 35,126 plans | MIT label, "built on SUNCG" | with caution |
| CubiCasa5K | 5,000 plans, raster and SVG | CC BY-NC-SA 4.0 | locally only |
| FloorPlanCAD | 15,663 CAD drawings | CC BY-NC 4.0; drawings' copyright not the authors' | locally only |
| RPLAN | over 80,000 plans | by form; "non-commercial research and academic purposes"; no redistribution | no |
| 3D-FRONT | furnished rooms | signed agreement; "for scientific research purpose only"; bars "providing external services" | no |
| Structured3D, ZInD, LIFULL HOME'S | — | agreement or application; academic only | no |

No open collection of plans of houses was found, in Australia or elsewhere.

### 7.2 Furniture, Fixtures and Materials

| Source | Contents | Licence |
|---|---|---|
| Poly Haven | models, textures, HDRIs | CC0 |
| ambientCG | PBR materials | CC0 1.0 |
| Kenney Furniture Kit | 140 low-poly pieces | CC0 |
| Quaternius | low-poly packs | CC0 |
| FreeCAD-library | parts, among them architectural, HVAC, pipes, electrical | CC BY 3.0 |

No open library of fixtures or appliances with dimensions and service connections was found.

### 7.3 Costs, Carbon and Climate

- **Costs.** Rawlinsons and Cordell are proprietary. The ABS's Building Approvals and Building Activity are CC BY 4.0, but an open series per square metre or by element was not confirmed.
- **Embodied carbon.** The EPiC database is CC BY-NC-ND 4.0; the ICE database is educational-only since 30 September 2026. Neither can be used.
- **Weather files.** NatHERS climate files may not be passed to third parties. climate.onebuilding.org states no licence; CSIRO's conditions were not read.
- **Prices.** The Victorian Valuer-General's sales statistics (medians by suburb) are CC BY 4.0; no open per-property sales were found.

## 8. Real Rules Are Formulas, and Many Are Relations to the Neighbours

The first case read: one dwelling on a lot in Victoria. Read from the Victoria Planning Provisions at amendment VC322 (gazetted 2 November 2026), through the planning schemes' API. **Re-read the clauses before coding a preset.**

### 8.1 Which Rules Apply Depends on the Lot

- In the General Residential and Neighbourhood Residential Zones, "A permit is required to construct or extend one dwelling on a lot less than 300 square metres" (cl. 32.08-5, 32.09-5). Clause 54 applies only then.
- Lots of 300 m² or more fall to Part 5 of the Building Regulations 2018, whose values differ on overshadowing and open space.
- Clause 54 was rewritten as deemed-to-comply by VC282 (operative 8 September 2025) and amended by VC315 (24 September 2026) and VC306 (October 2026). A schedule to a zone or an overlay can modify a standard.

### 8.2 The Standards of Clause 54

| Standard | Clause | Value |
|---|---|---|
| Street setback | 54.02-1 | the lesser of the abutting building's setback and 6 m; with no abutting building, 6 m on a Transport Zone 2 street, 4 m otherwise |
| Building height | 54.02-2; 32.08-11; 32.09-11 | the zone's maximum: GRZ 11 m and 3 storeys, NRZ 9 m and 2 storeys; +1 m on a slope over 2.5° |
| Side and rear setbacks | 54.02-3 | 1 m, plus 0.3 m per metre of height over 3.6 m up to 6.9 m, plus 1 m per metre over 6.9 m |
| Walls on boundaries | 54.02-4 | length at most the greater of 10 m + 25% of the remaining boundary, or the abutting walls; average height 3.2 m, none over 3.6 m |
| Site coverage | 54.02-5 | NRZ 60%; GRZ 65%; RGZ, MUZ 70% |
| Garden area | 32.08-4; 32.09-4 | lots of 400–500 m²: 25%; to 650: 30%; above: 35% |
| Tree canopy | 54.02-6 | 1 to 3 trees by site area; deep soil 12 m² |
| Private open space | 54.03-2 | 20% of the lot and at least 25 m², of which 25 m² secluded and 3 m wide; or a balcony or roof by table |
| Solar access to open space | 54.03-3 | the southern boundary of secluded open space at least $(2 + 0.9h)$ m from a wall of height $h$ to its north |
| Daylight to new windows | 54.03-4 | each habitable room window faces a space of at least 3 m² and 1 m, clear to the sky |
| Daylight to existing windows | 54.04-1 | a light court of 3 m² and 1 m; walls over 3 m set back at least half their height within a 55° arc from the window's centre |
| Existing north-facing windows | 54.04-2 | within 3 m of the boundary: 1 m, plus 0.6 m per metre over 3.6 m up to 6.9 m, plus 1 m per metre over 6.9 m; north is N20°W to N30°E |
| Overshadowing | 54.04-3 | the neighbour's secluded open space unshadowed over more than 50%, or 25 m² with 3 m minimum (the lesser), for 5 hours between 9 am and 3 pm on 22 September |
| Overlooking | 54.04-4 | within 9 m horizontally, 45° from the window's plane, from 1.7 m above the floor; bedrooms excluded |
| Permeability | 54.05-1 | at least 20% of the site |
| Rooftop solar area | 54.05-3 | 15, 26 or 34 m² by bedrooms |

### 8.3 The Building Code for a House

National Construction Code 2022, class 1a, from the ABCB Housing Provisions.

| Requirement | Clause | Value |
|---|---|---|
| Ceiling heights | 10.3.1(1) | habitable rooms 2.4 m; kitchen, corridor, bathroom, laundry 2.1 m; stairway 2.0 m above the nosing line |
| Natural light | 10.5.1 | windows at least 10% of the room's floor area; roof lights 3% |
| Ventilation | 10.6.2(1) | openable area at least 5% of the floor area |
| Stairs | 11.2.2 | riser 115–190 mm; going 240–355 mm; $2R + G$ between 550 and 700 mm; 2 to 18 risers a flight |
| Livable housing | H8D2; LHDS 2022 | entrance and internal doors 820 mm clear; corridors 1,000 mm; an access path 1,000 mm wide at 1:14 at most; 1,200 × 900 mm clear in front of the pan; one hobless shower |
| Energy | H6D2; Specification 42 | 7 stars with heating and cooling load limits; whole-of-home rating of at least 60 |

No minimum room sizes were found in the pages of the code read.

### 8.4 The Site's Data in Victoria

| Item | Access | Licence |
|---|---|---|
| Vicmap Property (parcels) | download; WFS at opendata.maps.vic.gov.au (`parcel_view`) | CC BY 4.0 |
| Vicmap Planning (zones, overlays) | the same WFS (`plan_zone`, `plan_overlay`) | CC BY 4.0 |
| City of Melbourne: property boundaries, 2023 building footprints with elevations, 3D models | data.melbourne.vic.gov.au | CC BY 4.0 |
| Elevation: GA's 5 m LiDAR DEM; Vicmap Elevation; ELVIS | services; download | CC BY 4.0 where stated |
| The planning scheme's text | `api.app.planning.vic.gov.au/planning/v2/ordinance/scheme/vpp/<clause>` (undocumented) | CC BY 4.0 by the department's general terms; not confirmed for scheme text |
| NCC 2022 | free online | CC BY 4.0 by the ABCB's FAQ; CC BY-ND on the volume's own notice: unresolved |

## 9. What Appears Not to Have Been Done

1. **Enumeration of plans in a non-rectangular envelope over several storeys.** Not found (§2.3).
2. **A façade grammar constrained by the interior's need for windows.** Not found as a grammar. Near it: Merrell et al. 2010 derive the exterior from the plan; ShellMaker (Xu and Aliaga 2026, [doi:10.1007/978-3-032-37083-9_1](https://doi.org/10.1007/978-3-032-37083-9_1)) completes an exterior about fixed openings.
3. **A building grammar that is learned, parsed, and carries an explicit symmetry group.** Not found. Learned and parsed without a group: Martinović and Van Gool 2013. Groups without a grammar: Liu et al. 2004; Park 2000, 2018.
4. **Equipment computed with the plan.** Not found; routing takes the plan as given (§5.2).
5. **Live rule checking, enumeration and suggestion in one tool where a person designs by hand.** Not found, on a shallow search (§6).
6. **A single house computed inside and out,** with its rules reported one by one and its results' uncertainty stated. No tool surveyed does it (§1.2).

What has been done, and must not be claimed as new: the enumeration of small rectangular plans and their dimensioning (1976); complete topological solutions by constraint programming on one floor (2000); furniture laid out under published clearances with suggestions (2011); a façade grammar learned and parsed (2013); rules checked automatically (from 1966).

## 10. How This Survey Was Checked, and Its Limits

- **Method.** Three research agents, by web search and reading, on 2026-11-11; this report consolidates what they returned. Pages were read through a summarising reader, so re-check any quoted wording before printing it.
- **Tools.** Facts are from vendors' own pages unless marked [S] or NV. Autodesk's product pages, RoomSketcher's and Giraffe's help pages refused the reader.
- **Literature.** Every citation with a DOI was matched against Crossref unless marked [NV]. Statements marked [R] were not re-read. ScienceDirect, iso.org, ASHRAE's and MDPI's pages refused the reader, so several abstracts could not be read. Coverage of 2025–2026 is a sample. One paper's author copy (Yao et al. 2003) was downloaded once from a server whose certificate failed its check; only two sentences were used, which agree with the published abstract.
- **Data and rules.** Licences are quoted from the source pages. The clause values were read from the planning schemes' API on the day; later amendments will change them. The Building Regulations' Part 5 was read from a practice note of July 2024, not from the regulations.
- **Not surveyed.** Structure; acoustics; ventilation and air flow; fire and egress; construction sequencing; other jurisdictions' rules; worldwide sources of parcels, weather and costs.
