// Checks of the policy domain: its C++ (theories of change, the evidence for a claim, transfer to a place, options appraised, adoption),
// built with CMake and run on calx's verification.
// Run from the repository root:  node domains/policy/tests/run-tests.mjs

import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

import { suite, cppChecks } from '../../../development/tools/checks.mjs';

const { assert, skip, done, lines } = suite();
const domain = resolve(dirname(fileURLToPath(import.meta.url)), '..');

cppChecks({ assert, skip, lines }, { project: domain, name: 'policy' });

done();
