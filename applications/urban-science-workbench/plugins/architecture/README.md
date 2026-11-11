## Architecture

*What could be built, and how it would stand in its street.* Status: **draft**. Activity: research. [ADR 0089](../../../../development/decisions/0089-what-could-be-built.md).

## 1. Layers

- **What could be built** (anywhere).
  - **A block.** Pick a place: the block is the face of the streets about it, and its buildings are its lots. A lot is a footprint as built, since parcels are not in the open data.
  - **The rules, declared.** None is any city's planning scheme:
    - *a street wall and towers*: a street wall to 24 m, towers set back 10 m above it, to 100 m; a plot ratio of 12; three fifths of the street line walled;
    - *mid-rise*: a street wall to 16 m, upper floors set back 5 m, to 32 m; a plot ratio of 4.5; four fifths walled;
    - *towers apart*: set back 6 m from the street and the neighbours, a low base, towers to 150 m; a plot ratio of 8.
  - **What a design keeps.** Half the floor standing now, or a tenth: in a dense block the rules may allow less than stands, and the card then says which limit binds.
  - **The search.** heuresis's NSGA-III over each lot's podium, upper part and their storeys, and one passage through the block or none, for 30, 60 or 150 generations of 128.
  - **The card.**
    - **The measures.** Six against the block as built: floor; winter sun on the streets; winter sun on the neighbours' faces; the heat a winter day needs per m² of floor; the streets' integration; and the floor within a five-minute walk. The plot ratio, height and street wall are given beside them.
    - **The front.** Floor against winter sun on the streets, each design a dot by its passage; a dot picked is drawn.
    - **The keys.** The designs best in each measure, the balanced one (whose worst measure is least), and the block as built.
  - **The map.**
    - **The design.** It stands in place of the block's buildings, which are hollowed from the city's drawing and its shadows.
    - **The sun.** The hours of sun on the open ground about the block, 10:00–14:00 solar time on the winter solstice, stand in cells from plum to gold.
    - **The clock.** A key sets it to the winter day at noon.
  - **The Studio.** When the view is captured, the layer gives its front as figures (floor against each other measure), and the Studio lays them as chart frames.

## 2. Engine

`engine/main.cpp` answers the page with the API's composition (`api/cpp/science/design.hpp`, `science::design::Designer`), which a study can also run without the page:
- `design`: the block, its lots and passages, and the designs as built and on the front, each measured, with every massing on the front checked against the rules;
- `sunmap`: the hours of sun on the open ground for a design of the last front, or as built;
- `status`.

The measures are the architecture domain's (`rules`, `form`, `frontage`, `environment`'s `sun.hpp` and `balance.hpp`) and urban science's (configuration, access); the search is heuresis's.

## 3. Checks

`tests/check_architecture.py`, run by the workbench's suite, on a grid of streets whose block holds four buildings known by hand:
- the block found;
- each lot's street edges;
- the floor as built;
- every massing within the rules;
- every design within its constraints;
- the search the same when asked again;
- the hours of sun bounded and summing to the street sun.

## 4. Limits

- **Lots are footprints.** On real parcels the envelopes would be larger.
- **The model.**
  - **Shade.** Shading is of prisms on flat ground at the receivers sampled, every 4 m.
  - **Heat.** The heat is a balance of the envelope, with no storage, ventilation or internal gains. The air is declared at 8 °C by day and 4 °C by night everywhere.
  - **Configuration.** It is of the ways within 1.1 km. Access counts floor, not homes or jobs.
- **The search has not converged.** The front's extremes move by up to a tenth between seeds (the study's §5).
