// The Policy Studio's page (ADR 0122): a project is a question of policy, worked on as the studios' projects are (the visualization
// layer's studio, visualization/js/workspace/studio.js), with five views of its own, each drawn as its subject:
//
//   - the theory: how the intervention is meant to bring about the outcome, drawn from one to the other, each causal claim as far
//     as it is shown, the strongest path heavier and its weakest link named; whatever is chosen opens where it stands;
//   - the evidence: for each claim its studies as a forest, pooled, and the ladder of its grade under the declared scheme, step by
//     step, with what a reader assessed beside what the rules read off the synthesis;
//   - here: the crossing from where it worked to this place, premise by premise, and the effect to expect here;
//   - the options: each a ledger of its effect, its worth and its costs, appraised with the uncertainty of the effect;
//   - the decision: adopt, reject or study first, each with what it is worth, the study worth running, and what the decision
//     turns on.
//
// Several people may work on one question (the server started with --people).

import { startStudio } from '/visualization/js/workspace/studio.js';
import { theoryView } from './theory.js';
import { evidenceView } from './evidence.js';
import { hereView } from './here.js';
import { optionsView } from './options.js';
import { decisionView } from './decision.js';

startStudio({
  name: 'policy-studio',
  title: 'Policy Studio',
  engineWord: 'the policy engine',
  scriptHelp: 'it may use numpy, the data library, and the foundations through the API (science.policy, science.appraisal, science.engines).',
  workbench: null,
  views: [theoryView(), evidenceView(), hereView(), optionsView(), decisionView()],
  first: 'theory',
});
