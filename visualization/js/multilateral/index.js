// Multilateral practice drawn (ADR 0132), of no work: an undertaking as a round with no head, its relations as threads in layers and,
// read from a chosen seat, every pair marked present, absent or not observed (round.js); its trajectory as strands beneath, each change
// a knot (strands.js); a partnership's commitments with their screens, progress and states, and each party's benefit by its own
// criteria (ledger.js); a decision rule's a priori power, and ideal points on a line (power.js); and the affairs by which an
// undertaking changes (affairs.js, ADR 0134): a question under a rule, a negotiation's zone draft by draft, a pair's events, mandates
// in time, review cycles, the lattice of memberships and reciprocity. They take what the API's multilateral engine answers, and the
// records it read, and ask no server; multilateral.css is their default look.

export { PALETTE, KINDS, kindOf, STATUS, STATES, STANDINGS, VERDICT, LAYER_INKS, LAYER_DASHES, status, layerCounts, layerLooks, holdsOn, fit, figure, balanced } from './common.js';
export { layoutRound, svgRound } from './round.js';
export { knotsOf, svgStrands, KNOTS } from './strands.js';
export { svgCommitments, svgBenefits } from './ledger.js';
export { svgPower, svgLine, svgBargaining } from './power.js';
export { ACTS, hemicycle, svgOutcome, svgZone, svgEventSeries, svgMandates, svgReviews, svgLattice, svgReciprocity } from './affairs.js';
