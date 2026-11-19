// The Law Studio's page (ADR 0121): a project is a matter, worked on as the studios' projects are (the visualization layer's studio,
// visualization/js/workspace/studio.js), with five views of its own, each drawn as its subject:
//
//   - the matter: its questions at the top, the rules that give them beneath, and on the ground the facts and open terms it all
//     rests on, each as it stands; whatever is chosen opens beside itself;
//   - the evidence: the documents as a reading room, the passages that bear on the facts marked in the text, put forward by hand or
//     proposed by a language model and each found in its document, accepted or rejected by a person; exhibits with their custody;
//   - the law: the scheme the matter is read under, clause by clause in the words of its official text, each clause with the rules
//     that state it;
//   - the readings: each open term's readings, with the courts' holdings weighed for the forum, the parties' arguments and what a
//     model proposes;
//   - the plan of proof: the ways to establish each question and to resist it, and what is needed in every way;
//   - space and time (ADR 0127): the evidence drawn where and when it speaks, as a cube whose floor is the place and whose height is
//     time, each person's possible whereabouts as prisms, a theory's paths, and whether each could have been at the scene;
//   - theories of the case (ADR 0127): accounts of what happened found by an evolutionary search, weighed, read under the law and
//     tested on what the matter holds back;
//   - theories of the law (ADR 0133): doctrines (readings of the open terms, ratios of precedents whose reasons were unstated) found
//     by an evolutionary search, judged by how they fit the decided cases and by the declared purposes, and tested on the decisions
//     given after the matter.
//
// Several people may work on one matter (the server started with --people).

import { startStudio } from '/visualization/js/workspace/studio.js';
import { matterView } from './matter.js';
import { evidenceView } from './evidence.js';
import { lawView } from './law.js';
import { readingsView } from './readings.js';
import { planView } from './plan.js';
import { spacetimeView } from './spacetime.js';
import { theoriesView } from './theories.js';
import { doctrineView } from './doctrine.js';

startStudio({
  name: 'law-studio',
  title: 'Law Studio',
  engineWord: 'the law engine',
  scriptHelp: 'it may use numpy, the data library, and the foundations through the API (science.law with its theories and doctrines, science.recognise, science.interpret, science.engines).',
  workbench: null,
  views: [matterView(), evidenceView(), lawView(), readingsView(), planView(), spacetimeView(), theoriesView(), doctrineView()],
  first: 'matter',
});
