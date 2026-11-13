// Checks of the musicology domain: its C++ (pitch: intervals, cents, the commas and beats; tuning: Pythagorean chains, sanfen
// sunyi, just intonation, meantone, equal and circulating temperaments, al-Urmawī's seventeen steps, and their measures),
// built with CMake and run on calx's verification.
// Run from the repository root:  node domains/musicology/tests/run-tests.mjs

import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

import { suite, cppChecks } from '../../../development/tools/checks.mjs';

const { assert, skip, done, lines } = suite();
const domain = resolve(dirname(fileURLToPath(import.meta.url)), '..');

cppChecks({ assert, skip, lines }, { project: domain, name: 'musicology' });

done();
