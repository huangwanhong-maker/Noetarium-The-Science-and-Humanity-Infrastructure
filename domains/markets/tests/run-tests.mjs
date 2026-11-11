// Checks of the markets domain: its C++ (a market and what is computed from it, the size of a market, markets in space), built with
// CMake and run on calx's verification.
// Run from the repository root:  node domains/markets/tests/run-tests.mjs

import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

import { suite, cppChecks } from '../../../development/tools/checks.mjs';

const { assert, skip, done, lines } = suite();
const domain = resolve(dirname(fileURLToPath(import.meta.url)), '..');

cppChecks({ assert, skip, lines }, { project: domain, name: 'markets' });

done();
