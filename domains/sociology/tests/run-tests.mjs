// Checks of the sociology domain: its C++ (time, a day as a composition of activities and its reallocation; relations, the statistics of social ties), built with CMake and run on calx's verification.
// Run from the repository root:  node domains/sociology/tests/run-tests.mjs

import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

import { suite, cppChecks } from '../../../development/tools/checks.mjs';

const { assert, skip, done, lines } = suite();
const domain = resolve(dirname(fileURLToPath(import.meta.url)), '..');

cppChecks({ assert, skip, lines }, { project: domain, name: 'sociology' });

done();
