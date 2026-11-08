// Checks of the demography domain: its C++ (life tables, fertility and reproduction, projection and the stable population),
// built with CMake and run on calx's verification.
// Run from the repository root:  node domains/demography/tests/run-tests.mjs

import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

import { suite, cppChecks } from '../../../development/tools/checks.mjs';

const { assert, skip, done, lines } = suite();
const domain = resolve(dirname(fileURLToPath(import.meta.url)), '..');

cppChecks({ assert, skip, lines }, { project: domain, name: 'demography' });

done();
