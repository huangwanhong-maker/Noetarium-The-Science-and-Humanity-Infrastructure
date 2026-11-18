// Checks of the needs domain: its C++ (groups, the language of statements, the evidence for a statement, the search), built with
// CMake and run on calx's verification.
// Run from the repository root:  node domains/needs/tests/run-tests.mjs

import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

import { suite, cppChecks } from '../../../development/tools/checks.mjs';

const { assert, skip, done, lines } = suite();
const domain = resolve(dirname(fileURLToPath(import.meta.url)), '..');

cppChecks({ assert, skip, lines }, { project: domain, name: 'needs' });

done();
