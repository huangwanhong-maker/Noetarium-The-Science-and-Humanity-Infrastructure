// Checks of the diplomacy domain: its C++ (an undertaking's typed relations and diagnostics, a partnership's record), built with CMake
// and run on calx's verification.
// Run from the repository root:  node domains/diplomacy/tests/run-tests.mjs

import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

import { suite, cppChecks } from '../../../development/tools/checks.mjs';

const { assert, skip, done, lines } = suite();
const domain = resolve(dirname(fileURLToPath(import.meta.url)), '..');

cppChecks({ assert, skip, lines }, { project: domain, name: 'diplomacy' });

done();
