# climate-risk

*Status: planned ([ADR 0129](../../development/decisions/0129-risk-management-urban-risk-and-disaster-risk.md)). Bridge, height 5, over risk management and climate.* The risks a changing climate brings: hazards whose frequency and severity move with the climate, exposure that grows and moves, and the losses and their financing over decades rather than a year. At the user's word, it is risk management's second specialisation beside urban risk management.

## 1. Scope

| Module | Scope |
|---|---|
| `hazards` | Exceedance curves that move with a climate's state: return periods under scenarios of warming, shifts and scalings of a curve, their uncertainty | 
| `exposure` | Exposure over decades: people and assets in places whose hazards change | 
| `transition` | The risks of the transition: assets stranded by policy and prices | 
| `adaptation` | Adaptation pathways: measures, their tipping points and the decision of when to switch | 

## 2. Dependencies

| Source | What climate risk uses from it |
|---|---|
| risk-management | Criticality over scenarios, the layers of a loss and their financing |
| climate | Climate classes, the energy balance and orbital forcing; later its scenarios |
| tyche | Exceedance curves, layers |
| calx | Verification |

## 3. Open Questions

- Whether disaster risk's curves under a changing climate belong here or there: provisionally here, with disaster risk reading a curve for a given year.
