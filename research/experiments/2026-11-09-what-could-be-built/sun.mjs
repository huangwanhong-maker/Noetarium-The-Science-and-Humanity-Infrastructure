// The Sun's altitude and azimuth at a place on a date, at hours of local solar time, from the astronomy domain's subsolar point
// (domains/astronomy/js/sun.js), as the Urban Science Workbench's plug-in Architecture takes it. Low-precision theory: 0.01° in
// right ascension and declination (the module's own statement).
//
//   node sun.mjs '{"lat": -37.81, "lon": 144.96, "date": "2026-06-21", "hours": [10, 11, 12, 13, 14]}'
//
// prints [[altitude, azimuth], ...] in radians (azimuth from north through east).

import { subsolarPoint, sunAltAz } from '../../../domains/astronomy/js/index.js';

const r = JSON.parse(process.argv[2]);
const [y, m, d] = r.date.split('-').map(Number);
const out = r.hours.map((hour) => {
  const s = sunAltAz(r.lon, r.lat, subsolarPoint(Date.UTC(y, m - 1, d) + (hour - r.lon / 15) * 3600e3));
  return [(s.altitude * Math.PI) / 180, (s.azimuth * Math.PI) / 180];
});
process.stdout.write(JSON.stringify(out));
