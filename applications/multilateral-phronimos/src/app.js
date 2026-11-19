// Multilateral Phronimos's page (ADR 0132): a project is an undertaking among many parties and its partnership, worked on as the studios'
// projects are (the visualization layer's studio, visualization/js/workspace/studio.js), with four views of its own, each drawn as its
// subject:
//
//   - the round: the participants on a ring with no head, the relations as threads in layers, what the record does not observe marked
//     as such; the trajectory as strands beneath; a party, a tie or a change opened where it stands;
//   - the partnership: whether it benefits every party, party by party; its commitments with their screens and states; each party's
//     benefit by its own criteria; what was given; the signs of failure;
//   - governance: decisions as records with their authority, evidence, those affected and venues; the arrangement's own diagnostics;
//     the rules its bodies decide by, with their a priori power;
//   - negotiation: the talks' zone of agreement draft by draft (ADR 0134); where members stand, from their votes; a bargain between two
//     parties; issues divided;
//   - institutions: the lattice of memberships, each issue's complex, the design declared, and reciprocity (ADR 0134).
//
// The affairs of ADR 0134 stand where their subjects are: questions decided under declared rules, mandates and review cycles on
// governance; a pair's events beneath the round.
//
// Several people may work on one record (the server started with --people).

import { startStudio } from '/visualization/js/workspace/studio.js';
import { roundView } from './round.js';
import { partnershipView } from './partnership.js';
import { governanceView } from './governance.js';
import { negotiationView } from './negotiation.js';
import { institutionsView } from './institutions.js';

startStudio({
  name: 'multilateral-phronimos',
  title: 'Multilateral Phronimos',
  engineWord: 'the multilateral engine',
  scriptHelp: 'it may use numpy, the data library, and the foundations through the API (science.multilateral, science.engines).',
  workbench: null,
  views: [roundView(), partnershipView(), governanceView(), negotiationView(), institutionsView()],
  first: 'round',
});
