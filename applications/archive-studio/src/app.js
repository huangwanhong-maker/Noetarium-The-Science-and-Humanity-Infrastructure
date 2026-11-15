// The Archive Studio's page (ADR 0119, ADR 0120): a project of an archive, worked on as the studios' projects are (the visualization
// layer's studio, visualization/js/workspace/studio.js), with five views of its own, each drawn as its subject:
//
//   - the archive: its holdings as a wall, each drawn by its kind, opening in place into a reading room where it is seen (an image
//     in deep zoom from the server's IIIF service), heard or read, described in Dublin Core, Records in Contexts or CIDOC CRM,
//     annotated or coded, its fixity verified and its chain of custody followed;
//   - the arrangement: the finding aid, arranged in place, checked by archeion, written and read as EAD3, packed as BagIt; and the
//     project's sealed journal;
//   - the graph: the project's linked data drawn as the page, a node opening where it stands; a desk for SPARQL, SHACL's shapes,
//     entailment and the text of each file, JSON-LD among them;
//   - the datasets: tables and their codebooks (DDI), files held to them, versions compared, variables harmonised;
//   - the catalogue: studies of Europe's social-science data archives, taken into the project as linked data.
//
// Several people may work on one archive (the server started with --people): each page says who is there and what the others did.
// The analyses of cases, coded texts, scales and models are the studio's tools, on the project's tables.

import { startStudio } from '/visualization/js/workspace/studio.js';
import { archiveView } from './archive.js';
import { arrangementView } from './arrangement.js';
import { graphView } from './graph.js';
import { datasetsView } from './datasets.js';
import { catalogueView } from './catalogue.js';

startStudio({
  name: 'archive-studio',
  title: 'Archive Studio',
  engineWord: 'the engines of linked data, archives and inquiry',
  scriptHelp: 'it may use numpy, the data library, and the foundations through the API (science.linked, science.archive, science.inquiry, science.engines).',
  workbench: null,
  views: [archiveView(), arrangementView(), graphView(), datasetsView(), catalogueView()],
  first: 'archive',
});
