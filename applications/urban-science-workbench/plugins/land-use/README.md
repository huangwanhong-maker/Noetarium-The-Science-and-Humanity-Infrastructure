## Land Use and Transport

*A change to the roads or the buildings, and the years after it.* Status: **draft**. Activity: research. [ADR 0094](../../../../development/decisions/0094-land-use-and-transport-interacting.md).

## 1. Layers

- **A change, and the years after it** (anywhere).
  - **A place.** Pick a place, and three things come with it:
    - its people, on GHS-POP's 1 km cells within 6 km, become households at 2.5 people each;
    - the floor of its workplaces becomes jobs, at 40 m² a job, given to the nearest cell. A workplace is a building that Overture or OpenStreetMap says is an office, a shop, a school, a hospital, a factory and the like. Where a place maps no uses, the jobs are taken where people live, and the card says so;
    - its classified roads, tertiary and above, at their classes' speeds and capacities (declared). This is the strategic network of land-use models: a cell joins it by walking.
  - **The changes, placed on the map:**
    - a road widened, which doubles its capacity;
    - a road closed;
    - more dwellings allowed in a cell, which doubles its capacity;
    - jobs added in a cell, a quarter of its households and at least 500.
  - **The years.** Ten or twenty, run twice by the engine: as things stand, and with the changes from year 1.
  - **The map.**
    - **The cells** take the colour of what the changes do to their households in the year shown, on the validated diverging map: sea glass for fewer, brick rose for more. The scale is the largest change over all the years, so the years compare.
    - **The roads** take the colour of their load in the peak hour, from pale gold to umber at 1.2 times capacity. A road closed is drawn in wine, a widened road broader.
    - **The years** play on the map, or are moved by hand.
  - **The card.** The households living there and the mean journey to work, with the changes and as things stand. The cells that gain and lose most, with their rents (100 in year 0). A chart of both against the years.
  - **The Studio.** When the view is captured, the chart goes to the Studio as a figure.

## 2. Engine

`engine/main.cpp` answers the page with the API's composition (`api/cpp/science/luti.hpp`, `science::luti::run`), which a study can also run without the page:
- `run`: the zones' households, rents, stock and accessibility and the roads' loads in the peak hour, year by year, as things stand and with the scenario;
- `status`.

The model is urban science's `luti`, on `housing`'s market, `access`'s network and nomos's spatial interaction and routing games. The network is contracted to chains between junctions and the cells' points, and Evans's equilibrium is solved each year to a gap of 10⁻³ or 1,500 iterations, whichever comes first; the card gives the gap reached. A dense, congested place converges slowly: Brooklyn's 725,000 households took about twelve minutes for ten years. A year that repeats the one before, with no event to come, is copied: so the run as things stand is solved once, and the changed run from its change on.

## 3. Checks

`tests/check_land_use.py`, run by the workbench's suite, on a place made by hand (a grid of streets, two primary roads, sixteen cells):
- the network contracted, and every year's equilibrium at the gap asked;
- nothing changing as things stand;
- households located and outside summing to those who consider the place;
- more dwellings allowed where no rent invites building changing nothing;
- a widened road's load falling, and the cells it serves gaining households after their rents rose;
- a closed road lengthening the mean journey;
- jobs added in a corner drawing households to the cells about it.

## 4. Limits

- **Declared, not fitted.** The base year is revealed from where people live, with every rent at the rent at which building pays. The parameters are declared rather than fitted to the place: the decay of the journey to work, 0.07 a minute; the value of access, 0.02 a minute; a tenth of the workers on the roads in the peak hour; room for half again the dwellings. The New York study (`research/experiments/2026-11-12-land-use-and-transport`) fits the decay to a record.
- **One mode, one type of dwelling, households that do not age.** One-way streets are not distinguished. Firms do not choose where to locate.
- **What is mapped.** Jobs are as good as the map's uses of buildings.
