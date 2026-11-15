// The Archive Studio's page (ADR 0119): a project of an archive, worked on as the studios' projects are (the visualization layer's
// studio, visualization/js/workspace/studio.js), with three views of its own, each drawn as its subject:
//
//   - the archive: its holdings as a wall, each drawn by its kind, opening in place into a reading room where it is seen, heard or
//     read, described and annotated, and its fixity verified;
//   - the graph: the project's linked data drawn as the page, a node opening where it stands with its statements and what can be
//     done with them; a desk for SPARQL, SHACL's shapes, entailment and the text of each file;
//   - the catalogue: studies of Europe's social-science data archives, taken into the project as linked data.
//
// What is computed on linked data is symploke's, asked of the work's engine by its server (archive.py); what is drawn is the
// visualization layer's (visualization/js/linked).

import { startStudio } from '/visualization/js/workspace/studio.js';
import { archiveView } from './archive.js';
import { graphView } from './graph.js';
import { catalogueView } from './catalogue.js';

startStudio({
  name: 'archive-studio',
  title: 'Archive Studio',
  engineWord: 'the engine of linked data',
  scriptHelp: 'it may use numpy, the data library, and the foundations through the API (science.linked for linked data, science.engines).',
  workbench: null,
  views: [archiveView(), graphView(), catalogueView()],
  first: 'archive',
});
