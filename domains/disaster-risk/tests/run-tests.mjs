// Checks of the disaster-risk domain: its C++ (disruption, a hazard's depth on the roads; evacuation, people on foot brought to
// safety), built with CMake and run on calx's verification.
// Run from the repository root:  node domains/disaster-risk/tests/run-tests.mjs

import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

import { suite, cppChecks } from '../../../development/tools/checks.mjs';

const { assert, skip, done, lines } = suite();
const domain = resolve(dirname(fileURLToPath(import.meta.url)), '..');

cppChecks({ assert, skip, lines }, { project: domain, name: 'disaster-risk' });

done();
