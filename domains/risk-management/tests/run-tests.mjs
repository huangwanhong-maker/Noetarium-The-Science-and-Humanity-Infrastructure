// Checks of the risk-management domain: its C++ (criticality, the importance of a system's components under a hazard; financing, a
// loss in layers and what each costs), built with CMake and run on calx's verification.
// Run from the repository root:  node domains/risk-management/tests/run-tests.mjs

import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

import { suite, cppChecks } from '../../../development/tools/checks.mjs';

const { assert, skip, done, lines } = suite();
const domain = resolve(dirname(fileURLToPath(import.meta.url)), '..');

cppChecks({ assert, skip, lines }, { project: domain, name: 'risk-management' });

done();
