// Checks of the industrial-ecology domain: its C++ (stocks, flows, lca, eeio, accounts, buildings), built with CMake and run on calx's verification.
// Run from the repository root:  node domains/industrial-ecology/tests/run-tests.mjs

import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

import { suite, cppChecks } from '../../../development/tools/checks.mjs';

const { assert, skip, done, lines } = suite();
const domain = resolve(dirname(fileURLToPath(import.meta.url)), '..');

cppChecks({ assert, skip, lines }, { project: domain, name: 'industrial-ecology' });

done();
