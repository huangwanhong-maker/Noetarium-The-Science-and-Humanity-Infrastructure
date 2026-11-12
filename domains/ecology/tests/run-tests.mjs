// Checks of the ecology domain: its C++ (taxonomy and distinctness; biodiversity on tyche's classes; stage-classified
// populations; the forest's allometry, composition, future and crowns; connectivity of habitat by graphs and circuits),
// built with CMake and run on calx's verification.
// Run from the repository root:  node domains/ecology/tests/run-tests.mjs

import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

import { suite, cppChecks } from '../../../development/tools/checks.mjs';

const { assert, skip, done, lines } = suite();
const domain = resolve(dirname(fileURLToPath(import.meta.url)), '..');

cppChecks({ assert, skip, lines }, { project: domain, name: 'ecology' });

done();
