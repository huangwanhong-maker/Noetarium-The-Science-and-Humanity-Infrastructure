// The Urban Studio's page (ADR 0102): a project about a city and the life in it, worked on as project software is, on the
// visualization layer's studio (visualization/js/workspace/studio.js), which it shares with Market Studio. What the Urban Studio
// knows of cities, forests and populations is in its plug-ins' tools, declared by the server; a placed table opens in the Urban
// Science Workbench, where the city stands in three dimensions.

import { startStudio } from '/visualization/js/workspace/studio.js';

const WORKBENCH = new URLSearchParams(location.search).get('workbench') || `${location.protocol}//${location.hostname}:8970/`;
document.getElementById('to-workbench').href = WORKBENCH;

startStudio({
  name: 'urban-studio',
  title: 'Urban Studio',
  engineWord: 'the engines of its plug-ins',
  scriptHelp: 'it may use numpy, the data library, and the domains through the API (science.engines; the ecology domain through science.ecology).',
  workbench: WORKBENCH,
});
