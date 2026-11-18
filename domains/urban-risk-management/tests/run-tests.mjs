// Checks of the urban-risk-management domain: its C++ (lifelines, which roads matter most to reaching a service; response, coverage
// by stations and where to add them), built with CMake and run on calx's verification.
// Run from the repository root:  node domains/urban-risk-management/tests/run-tests.mjs

import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

import { suite, cppChecks } from '../../../development/tools/checks.mjs';

const { assert, skip, done, lines } = suite();
const domain = resolve(dirname(fileURLToPath(import.meta.url)), '..');

cppChecks({ assert, skip, lines }, { project: domain, name: 'urban-risk-management' });

done();
