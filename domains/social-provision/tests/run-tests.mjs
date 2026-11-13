// Checks of the social-provision domain: its C++ (households, their scales, unpaid work and time poverty; care, its need, supply, gap and the change of need), built with CMake and run on calx's verification.
// Run from the repository root:  node domains/social-provision/tests/run-tests.mjs

import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

import { suite, cppChecks } from '../../../development/tools/checks.mjs';

const { assert, skip, done, lines } = suite();
const domain = resolve(dirname(fileURLToPath(import.meta.url)), '..');

cppChecks({ assert, skip, lines }, { project: domain, name: 'social-provision' });

done();
