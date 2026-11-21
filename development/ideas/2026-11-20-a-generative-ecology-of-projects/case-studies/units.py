"""The three units of Study A: where they live, what the repository checks of them, and their documented contract.

Everything here was fixed before any development was made (PLAN.md, Section 2): the exported names, the domain
of each function, the tolerance within which a development must agree with the accepted implementation, and the
hidden inputs of two tiers. Tolerances are tight where the unit's documents give the formula and at the documented
accuracy where they give only an accuracy.
"""
from __future__ import annotations

import math
import random
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[4]

MS_LO, MS_HI = -2208988800000, 4102444800000          # 1900-01-01 to 2100-01-01, Unix milliseconds
JD_LO, JD_HI = 2415020.5, 2488069.5                   # the same span as Julian dates


def _angle(tol, lo, hi):
    return {"kind": "number", "tol": tol, "period": 360.0, "range": [lo, hi]}


def _num(tol):
    return {"kind": "number", "tol": tol}


SUN = {
    "name": "sun",
    "module": "domains/astronomy/js/sun.js",
    "note": "domains/astronomy/theory/subsolar-point-and-twilight.md",
    "tests": "domains/astronomy/tests/run-tests.mjs",
    "block": ("// ---- the Sun seen from the Earth", "// ---- galaxies"),
    "fixtures": ["domains/astronomy/tests/fixtures/subsolar.json"],
    "checks": 10,
    "purpose": "The Sun seen from the Earth: its apparent position, the place where it stands at the zenith, "
               "its altitude and azimuth at any place, and the classes of daylight and twilight.",
    "imports": {"../../geodesy/js/index.js": ["DEG", "toVector", "wrapLongitude"]},
    "dependencies": "From '../../geodesy/js/index.js' the unit may import: DEG, the number of radians in a degree; "
                    "toVector(lon, lat), the unit vector of a place given in degrees, in Earth-centred Earth-fixed "
                    "coordinates with x toward (0 E, 0 N), y toward (90 E, 0 N) and z toward the north pole; "
                    "wrapLongitude(lon), a longitude in degrees brought into the range from -180 to 180.",
    "interface": [
        "J2000  (a number)",
        "SUNRISE_ALTITUDE  (a number)",
        "TWILIGHT  (an object { civil, nautical, astronomical })",
        "julianDate(ms) -> number",
        "gmst(jd) -> number",
        "sunPosition(jd) -> { ra, dec, longitude, distanceAu }",
        "subsolarPoint(ms) -> { lon, lat }",
        "sunAltAz(lon, lat, sub) -> { altitude, azimuth }      (sub is a value of subsolarPoint)",
        "daylight(altitude) -> string",
        "sunVector(sub) -> [x, y, z]",
        "localMeanSolarTime(ms, lon) -> number",
    ],
    "constants": {"J2000": {"kind": "exact"}, "SUNRISE_ALTITUDE": {"kind": "exact"}, "TWILIGHT": {"kind": "exact"}},
    "functions": {
        "julianDate": {"spec": _num(1e-8), "domain": "ms: Unix milliseconds from 1900-01-01 to 2100-01-01"},
        "gmst": {"spec": _angle(1e-6, 0, 360), "domain": "jd: a Julian date from 2415020.5 to 2488069.5"},
        "sunPosition": {"spec": {"kind": "object", "fields": {"ra": _angle(1e-6, 0, 360), "dec": _num(1e-6),
                                                              "longitude": _angle(1e-6, 0, 360), "distanceAu": _num(5e-4)}},
                        "domain": "jd: a Julian date from 2415020.5 to 2488069.5"},
        "subsolarPoint": {"spec": {"kind": "object", "fields": {"lon": _angle(2e-6, -180, 180), "lat": _num(1e-6)}},
                          "domain": "ms: Unix milliseconds from 1900-01-01 to 2100-01-01"},
        "sunAltAz": {"spec": {"kind": "object", "fields": {"altitude": _num(1e-6), "azimuth": _angle(1e-6, 0, 360)}},
                     "domain": "lon from -180 to 180 and lat from -89.9 to 89.9, in degrees; sub: { lon from -180 to 180, lat from -90 to 90 }; "
                               "places within 1 degree of the subsolar point or of its antipode are outside the domain"},
        "daylight": {"spec": {"kind": "string"}, "domain": "altitude: degrees from -90 to 90"},
        "sunVector": {"spec": {"kind": "array", "each": _num(1e-9), "length": 3},
                      "domain": "sub: { lon from -180 to 180, lat from -90 to 90 }, in degrees"},
        "localMeanSolarTime": {"spec": {"kind": "number", "tol": 1e-9, "period": 24.0, "range": [0, 24]},
                               "domain": "ms: Unix milliseconds from 1900-01-01 to 2100-01-01; lon: degrees from -180 to 180"},
    },
}

SPHERE = {
    "name": "sphere",
    "module": "domains/geodesy/js/sphere.js",
    "note": None,
    "tests": "domains/geodesy/tests/run-tests.mjs",
    "block": ("// ---- the sphere", "// ---- the ellipsoid"),
    "fixtures": [],
    "checks": 6,
    "purpose": "The Earth as a sphere of mean radius: places as unit vectors, local directions, great-circle "
               "angles and distances, motion along great circles, and the area of zones.",
    "imports": {},
    "dependencies": "The unit imports nothing.",
    "interface": [
        "DEG  (a number)",
        "EARTH_RADIUS_M  (a number)",
        "wrapLongitude(lon) -> number",
        "toVector(lon, lat) -> [x, y, z]",
        "fromVector(v) -> [lon, lat]",
        "eastNorth(lon, lat) -> [[ex, ey, ez], [nx, ny, nz]]",
        "centralAngle(lon1, lat1, lon2, lat2) -> number",
        "distanceM(lon1, lat1, lon2, lat2) -> number",
        "move(lon, lat, eastM, northM) -> [lon, lat]",
        "zoneFraction(lat1, lat2) -> number",
    ],
    "constants": {"DEG": _num(1e-15), "EARTH_RADIUS_M": _num(0.1)},
    "functions": {
        "wrapLongitude": {"spec": _angle(1e-9, -180, 180), "domain": "lon: degrees from -1080 to 1080"},
        "toVector": {"spec": {"kind": "array", "each": _num(1e-12), "length": 3},
                     "domain": "lon from -360 to 360 and lat from -90 to 90, in degrees"},
        "fromVector": {"spec": {"kind": "array", "items": [_angle(1e-9, -180, 180), _num(1e-9)]},
                       "domain": "v: three numbers with a length from 0.1 to 10, not within one part in a million of the polar axis"},
        "eastNorth": {"spec": {"kind": "array", "each": {"kind": "array", "each": _num(1e-12), "length": 3}, "length": 2},
                      "domain": "lon from -360 to 360 and lat from -89.9 to 89.9, in degrees"},
        "centralAngle": {"spec": _num(1e-9), "domain": "two places, lon from -360 to 360 and lat from -90 to 90, in degrees, "
                                                       "not within 0.001 radian of being antipodal"},
        "distanceM": {"spec": _num(0.5), "domain": "two places, lon from -360 to 360 and lat from -90 to 90, in degrees, "
                                                    "not within 0.001 radian of being antipodal"},
        # The planned tolerance of `move` (1e-7 degree in each coordinate) was tighter than the 0.1 m allowed on the radius
        # implies; the corrected contract compares the arrival as a place, within 0.5 m, as `distanceM` is compared.
        "move": {"spec": {"kind": "array", "items": [_angle(1e-7, -180, 180), _num(1e-7)]},
                 "spec_corrected": {"kind": "place", "tol_m": 0.5, "range": [-180, 180]}, "skip": "pole",
                 "domain": "lon strictly between -180 and 180 and lat from -89.9 to 89.9, in degrees; eastM and northM in metres, "
                           "each from -2e7 to 2e7; arrivals within 0.01 degree of a pole are outside the domain"},
        "zoneFraction": {"spec": _num(1e-12), "domain": "lat1 and lat2: degrees from -90 to 90"},
    },
}

_QUANTITY = {name: _num(1e-9) for name in ("MAT", "MAP", "Pthreshold", "Psummer", "Pwinter", "Thot", "Tcold", "Tmon10", "Pdry", "Psdry", "Pswet", "Pwdry", "Pwwet")}

CLIMATE = {
    "name": "climate",
    "module": "domains/climate/js/climate.js",
    "note": "domains/climate/theory/koppen-geiger-classification.md",
    "tests": "domains/climate/tests/run-tests.mjs",
    "block": ("// Climates constructed to meet one rule each", "// The C++"),
    "fixtures": [],
    "checks": 7,
    "purpose": "The climate of a place from its normals, which are twelve monthly mean temperatures T in degrees Celsius and twelve "
               "monthly precipitation totals P in millimetres, January first: its class in the Koppen-Geiger classification, "
               "the quantities that the classification uses, and its dry months.",
    "imports": {},
    "dependencies": "The unit imports nothing.",
    "interface": [
        "KOPPEN_CODES  (an array of the class codes, strings)",
        "koppenValue(code) -> number",
        "koppenQuantities(T, P) -> { MAT, MAP, Pthreshold, Psummer, Pwinter, Thot, Tcold, Tmon10, Pdry, Psdry, Pswet, Pwdry, Pwwet, summer }",
        "koppen(T, P) -> string, or null",
        "meanTemperature(tmax, tmin) -> array of numbers",
        "dryMonths(T, P) -> array of booleans",
        "koppenGrid(tmax, tmin, ppt) -> Uint8Array      (tmax, tmin, ppt: twelve arrays, one for each month, over the cells of a grid)",
    ],
    "constants": {"KOPPEN_CODES": {"kind": "exact"}},
    "functions": {
        "koppenValue": {"spec": {"kind": "exact"}, "domain": "code: a string"},
        "koppenQuantities": {"spec": {"kind": "object", "fields": {**_QUANTITY, "summer": {"kind": "string"}}},
                             "domain": "T: twelve numbers from -90 to 60; P: twelve numbers from 0 to 5000"},
        "koppen": {"spec": {"kind": "exact"}, "domain": "T: twelve numbers from -90 to 60; P: twelve numbers from 0 to 5000"},
        "meanTemperature": {"spec": {"kind": "array", "each": _num(1e-12), "length": 12},
                            "domain": "tmax and tmin: twelve numbers each, from -100 to 70"},
        "dryMonths": {"spec": {"kind": "exact"}, "domain": "T: twelve numbers from -90 to 60; P: twelve numbers from 0 to 5000"},
        "koppenGrid": {"spec": {"kind": "array", "each": _num(0)},
                       "domain": "tmax, tmin and ppt: twelve arrays each, all of one length from 1 to 50; temperatures from -100 to 70, "
                                 "precipitation from 0 to 5000; no value missing"},
    },
}

UNITS = {"sun": SUN, "sphere": SPHERE, "climate": CLIMATE}

# What each of the repository's checks calls, read from the tests themselves (a check's name begins with the key). The
# analysis uses it to tell a faulty function that a failing check calls from one that is rejected only because its
# development fails elsewhere.
CHECKS_CALL = {
    "sun": {
        "sun: subsolar longitude": ["subsolarPoint"], "sun: subsolar latitude": ["subsolarPoint"], "sun: subsolar point": ["subsolarPoint"],
        "sun: GMST": ["gmst"], "sun: Julian date": ["julianDate"],
        "sun: altitude at the subsolar point": ["sunAltAz", "subsolarPoint"], "sun: altitude on the terminator": ["sunAltAz", "subsolarPoint"],
        "sun: azimuth seen from the south": ["sunAltAz", "subsolarPoint"], "sun: daylight classes": ["daylight"],
        "sun: local mean solar time": ["localMeanSolarTime"],
    },
    "sphere": {
        "sphere: vector round trip": ["fromVector", "toVector", "wrapLongitude"], "sphere: east, north and up": ["eastNorth", "toVector"],
        "sphere: one degree of latitude": ["distanceM"], "sphere: haversine": ["centralAngle", "toVector"],
        "sphere: a quarter turn east": ["move"], "sphere: through the north pole": ["move", "wrapLongitude"],
    },
    "climate": {
        "köppen: fifteen constructed climates": ["koppen"], "köppen: a missing month": ["koppen"],
        "köppen: summer is the warmer half-year": ["koppenQuantities"],
        "köppen: classes are invariant under the half-year shift": ["koppen"], "köppen: classes are invariant under the wreath product": ["koppen"],
        "köppen: every one of the 30 classes": ["koppen"], "climate: the Mediterranean summer": ["dryMonths"],
    },
}


def called_by(unit_name: str, check_name: str) -> list[str]:
    """The functions that a check of the repository calls."""
    for key, functions in CHECKS_CALL[unit_name].items():
        if check_name.startswith(key):
            return functions
    raise KeyError(f"no declaration of what the check calls: {check_name}")


# ---- the unit's texts ---------------------------------------------------------------------------------------

def source(unit) -> str:
    return (ROOT / unit["module"]).read_text(encoding="utf-8")


def neutral(text: str) -> str:
    """The same text without the name of the project or of a person (PLAN.md, Section 5)."""
    text = re.sub(r"Noetarium|The Science Infrastructures?", "the project", text)
    text = re.sub(r"\bHuang\b|\bWanhong\b|Serendip\w*", "the author", text)
    return text


def comments_and_signatures(unit) -> str:
    """The unit's texts as they stand: every comment, every exported signature, no body and no value."""
    out, depth, skipping = [], 0, False
    for line in source(unit).splitlines():
        stripped = line.strip()
        if skipping:
            depth += line.count("{") - line.count("}")
            if depth <= 0:
                skipping = False
            continue
        if stripped.startswith("//") or stripped == "":
            out.append(line)
            continue
        if stripped.startswith("import "):
            out.append(line)
            continue
        m = re.match(r"export function (\w+)\(([^)]*)\)\s*\{", stripped)
        if m:
            out.append(f"export function {m.group(1)}({m.group(2)}) {{ ... }}")
            depth = line.count("{") - line.count("}")
            skipping = depth > 0
            continue
        m = re.match(r"export const (\w+) = \(([^)]*)\) =>", stripped)
        if m:
            out.append(f"export const {m.group(1)} = ({m.group(2)}) => ...;")
            continue
        m = re.match(r"export const (\w+) =", stripped)
        if m:
            out.append(f"export const {m.group(1)} = ...;")
            depth = line.count("{") - line.count("}")
            skipping = depth > 0 and not stripped.endswith(";")
            continue
        # anything else at the top level is private code and is left out
        depth = line.count("{") - line.count("}")
        skipping = depth > 0
    return neutral("\n".join(out).strip() + "\n")


def note(unit) -> str:
    if not unit["note"]:
        return ""
    text = (ROOT / unit["note"]).read_text(encoding="utf-8")
    text = re.sub(r"\A---\n.*?\n---\n", "", text, flags=re.S)        # front matter names files of the repository
    return neutral(text.strip() + "\n")


def identity(unit) -> str:
    lines = [f"Unit `{Path(unit['module']).name}`. {unit['purpose']}", "", "It exports exactly these names:"]
    lines += [f"  {entry}" for entry in unit["interface"]]
    lines += ["", unit["dependencies"]]
    return "\n".join(lines) + "\n"


def repository_texts(unit) -> str:
    parts = [identity(unit), "The unit's own comments and signatures, as the repository keeps them (bodies and values removed):", "",
             "```js", comments_and_signatures(unit).rstrip(), "```"]
    if unit["note"]:
        parts += ["", "The unit's theory note:", "", note(unit).rstrip()]
    return "\n".join(parts) + "\n"


def words(text: str) -> int:
    return len(text.split())


# ---- the hidden inputs --------------------------------------------------------------------------------------

def _angular_distance(lon1, lat1, lon2, lat2) -> float:
    p1, p2 = math.radians(lat1), math.radians(lat2)
    a = math.sin((p2 - p1) / 2) ** 2 + math.cos(p1) * math.cos(p2) * math.sin(math.radians(lon2 - lon1) / 2) ** 2
    return 2 * math.asin(min(1.0, math.sqrt(a)))


def in_domain(unit_name: str, fn: str, args) -> str | None:
    """None when the arguments lie in the documented domain of the function; otherwise the reason."""
    def number(x):
        return isinstance(x, (int, float)) and not isinstance(x, bool) and math.isfinite(x)

    def sub_ok(s):
        return isinstance(s, dict) and number(s.get("lon")) and number(s.get("lat")) and -180 <= s["lon"] <= 180 and -90 <= s["lat"] <= 90

    try:
        if unit_name == "sun":
            if fn in ("julianDate", "subsolarPoint"):
                (ms,) = args
                return None if number(ms) and MS_LO <= ms <= MS_HI else "ms outside 1900 to 2100"
            if fn in ("gmst", "sunPosition"):
                (jd,) = args
                return None if number(jd) and JD_LO <= jd <= JD_HI else "jd outside 1900 to 2100"
            if fn == "sunAltAz":
                lon, lat, sub = args
                if not (number(lon) and number(lat) and -180 <= lon <= 180 and -89.9 <= lat <= 89.9 and sub_ok(sub)):
                    return "arguments outside their ranges"
                d = _angular_distance(lon, lat, sub["lon"], sub["lat"])
                if d < math.radians(1) or d > math.pi - math.radians(1):
                    return "within 1 degree of the subsolar point or of its antipode"
                return None
            if fn == "daylight":
                (alt,) = args
                return None if number(alt) and -90 <= alt <= 90 else "altitude outside -90 to 90"
            if fn == "sunVector":
                (sub,) = args
                return None if sub_ok(sub) else "sub outside its ranges"
            if fn == "localMeanSolarTime":
                ms, lon = args
                return None if number(ms) and number(lon) and MS_LO <= ms <= MS_HI and -180 <= lon <= 180 else "arguments outside their ranges"
        if unit_name == "sphere":
            if fn == "wrapLongitude":
                (lon,) = args
                return None if number(lon) and abs(lon) <= 1080 else "lon outside -1080 to 1080"
            if fn == "toVector":
                lon, lat = args
                return None if number(lon) and number(lat) and abs(lon) <= 360 and abs(lat) <= 90 else "arguments outside their ranges"
            if fn == "fromVector":
                (v,) = args
                if not (isinstance(v, list) and len(v) == 3 and all(number(x) for x in v)):
                    return "v is not three numbers"
                r = math.hypot(*v)
                if not 0.1 <= r <= 10:
                    return "length outside 0.1 to 10"
                return None if math.hypot(v[0], v[1]) > 1e-6 * r else "on the polar axis"
            if fn == "eastNorth":
                lon, lat = args
                return None if number(lon) and number(lat) and abs(lon) <= 360 and abs(lat) <= 89.9 else "arguments outside their ranges"
            if fn in ("centralAngle", "distanceM"):
                lon1, lat1, lon2, lat2 = args
                if not all(number(x) for x in args) or abs(lon1) > 360 or abs(lon2) > 360 or abs(lat1) > 90 or abs(lat2) > 90:
                    return "arguments outside their ranges"
                return None if _angular_distance(lon1, lat1, lon2, lat2) <= math.pi - 0.001 else "within 0.001 radian of antipodal"
            if fn == "move":
                lon, lat, east, north = args
                ok = all(number(x) for x in args) and abs(lon) < 180 and abs(lat) <= 89.9 and abs(east) <= 2e7 and abs(north) <= 2e7
                return None if ok else "arguments outside their ranges"
            if fn == "zoneFraction":
                lat1, lat2 = args
                return None if number(lat1) and number(lat2) and abs(lat1) <= 90 and abs(lat2) <= 90 else "arguments outside their ranges"
        if unit_name == "climate":
            def twelve(x, lo, hi):
                return isinstance(x, list) and len(x) == 12 and all(number(v) and lo <= v <= hi for v in x)

            if fn in ("koppen", "koppenQuantities", "dryMonths"):
                T, P = args
                return None if twelve(T, -90, 60) and twelve(P, 0, 5000) else "T or P is not twelve numbers in range"
            if fn == "koppenValue":
                (code,) = args
                return None if isinstance(code, str) else "the code is not a string"
            if fn == "meanTemperature":
                tmax, tmin = args
                return None if twelve(tmax, -100, 70) and twelve(tmin, -100, 70) else "tmax or tmin is not twelve numbers in range"
            if fn == "koppenGrid":
                tmax, tmin, ppt = args
                grids = [tmax, tmin, ppt]
                if not all(isinstance(g, list) and len(g) == 12 and all(isinstance(m, list) for m in g) for g in grids):
                    return "not three lists of twelve arrays"
                n = len(tmax[0])
                if not 1 <= n <= 50 or any(len(m) != n for g in grids for m in g):
                    return "the arrays are not of one length from 1 to 50"
                ok = all(number(v) and -100 <= v <= 70 for g in (tmax, tmin) for m in g for v in m) and all(number(v) and 0 <= v <= 5000 for m in ppt for v in m)
                return None if ok else "a value is missing or out of range"
    except (TypeError, ValueError):
        return "arguments of the wrong shape"
    return "no such function"


def hidden_inputs(unit_name: str, seed: int = 20261121, n: int = 300) -> list[dict]:
    """The hidden inputs of a unit: `n` random ones inside each function's domain (tier main), and the boundaries (tier edge)."""
    rng = random.Random(seed)
    cases: list[dict] = []

    def add(fn, args, tier):
        if in_domain(unit_name, fn, args) is None:
            cases.append({"fn": fn, "args": args, "tier": tier})

    def many(fn, draw):
        made = 0
        while made < n:
            args = draw()
            if in_domain(unit_name, fn, args) is None:
                cases.append({"fn": fn, "args": args, "tier": "main"})
                made += 1

    u = rng.uniform
    if unit_name == "sun":
        many("julianDate", lambda: [u(MS_LO, MS_HI) if rng.random() < 0.5 else float(rng.randrange(MS_LO, MS_HI))])
        many("gmst", lambda: [u(JD_LO, JD_HI)])
        many("sunPosition", lambda: [u(JD_LO, JD_HI)])
        many("subsolarPoint", lambda: [float(rng.randrange(MS_LO, MS_HI))])
        many("sunAltAz", lambda: [u(-180, 180), u(-89.9, 89.9), {"lon": u(-180, 180), "lat": u(-23.44, 23.44)}])
        many("daylight", lambda: [u(-90, 90)])
        many("sunVector", lambda: [{"lon": u(-180, 180), "lat": u(-90, 90)}])
        many("localMeanSolarTime", lambda: [float(rng.randrange(MS_LO, MS_HI)), u(-180, 180)])
        for ms in (0.0, -1.0, 86400000.0, 946684800000.0, -631152000000.0, float(MS_LO), float(MS_HI)):
            add("julianDate", [ms], "edge")
            add("subsolarPoint", [ms], "edge")
        for jd in (2451545.0, 2446895.5, JD_LO, JD_HI, 2451623.8, 2451716.3):
            add("gmst", [jd], "edge")
            add("sunPosition", [jd], "edge")
        for alt in (-0.833, -6.0, -12.0, -18.0, 0.0, 90.0, -90.0, -0.8330001, -6.0000001, -12.0000001, -18.0000001, -0.8329999, -5.9999999):
            add("daylight", [alt], "edge")
        for lon, lat, sub in ((180.0, 10.0, {"lon": -170.0, "lat": 5.0}), (-180.0, 10.0, {"lon": 170.0, "lat": -5.0}),
                              (20.0, 0.0, {"lon": 20.0, "lat": 15.0}), (20.0, 40.0, {"lon": 20.0, "lat": 15.0}),
                              (20.0, -40.0, {"lon": 20.0, "lat": 15.0}), (0.0, 0.0, {"lon": 90.0, "lat": 0.0}),
                              (0.0, 0.0, {"lon": -90.0, "lat": 0.0}), (100.0, 50.0, {"lon": -80.0, "lat": 0.0})):
            add("sunAltAz", [lon, lat, sub], "edge")
        for sub in ({"lon": 0.0, "lat": 90.0}, {"lon": 0.0, "lat": -90.0}, {"lon": 180.0, "lat": 0.0}, {"lon": -180.0, "lat": 0.0},
                    {"lon": 90.0, "lat": 0.0}, {"lon": 0.0, "lat": 0.0}):
            add("sunVector", [sub], "edge")
        for ms, lon in ((0.0, 0.0), (0.0, 180.0), (0.0, -180.0), (-1.0, 0.0), (-3600000.0, -30.0), (43200000.0, 180.0),
                        (43200000.0, -180.0), (float(MS_LO), 15.0), (86399999.0, 0.0)):
            add("localMeanSolarTime", [ms, lon], "edge")
    elif unit_name == "sphere":
        def place():
            return u(-180, 180), math.degrees(math.asin(u(-1, 1)))

        def vector():
            while True:
                v = [u(-1, 1), u(-1, 1), u(-1, 1)]
                r = math.hypot(*v)
                if 0.05 < r <= 1:
                    scale = u(0.1, 10) / r
                    return [x * scale for x in v]

        def avoiding_seam(lon):
            return abs(((lon + 180) % 360)) > 1e-6 and abs(((lon + 180) % 360) - 360) > 1e-6

        def wrap_draw():
            while True:
                lon = u(-1080, 1080)
                if avoiding_seam(lon):
                    return [lon]

        many("wrapLongitude", wrap_draw)
        many("toVector", lambda: list(place()))
        many("fromVector", lambda: [vector()])
        many("eastNorth", lambda: [u(-360, 360), u(-89.9, 89.9)])
        many("centralAngle", lambda: [*place(), *place()])
        many("distanceM", lambda: [*place(), *place()])
        many("move", lambda: [u(-179.999, 179.999), u(-89.9, 89.9), u(-2e7, 2e7), u(-2e7, 2e7)])
        many("zoneFraction", lambda: [u(-90, 90), u(-90, 90)])
        for lon in (180.0, -180.0, 540.0, -540.0, 0.0, 360.0, -360.0, 179.999999, -179.999999, 900.0):
            add("wrapLongitude", [lon], "edge")
        for lon, lat in ((0.0, 90.0), (0.0, -90.0), (180.0, 0.0), (-180.0, 0.0), (90.0, 0.0), (0.0, 0.0), (360.0, 45.0)):
            add("toVector", [lon, lat], "edge")
        for v in ([-1.0, 0.0, 0.0], [1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, -1.0, 0.0], [3.0, 4.0, 0.0], [0.0, 0.5, 0.5]):
            add("fromVector", [v], "edge")
        for lon, lat in ((0.0, 0.0), (90.0, 0.0), (180.0, 45.0), (-180.0, -45.0), (35.0, 89.9), (35.0, -89.9)):
            add("eastNorth", [lon, lat], "edge")
        for base in ((10.0, 20.0), (-120.0, -45.0), (0.0, 89.0), (179.9999, 0.0)):
            for step in (1e-3, 1e-5, 1e-7):
                add("centralAngle", [base[0], base[1], base[0] + step, base[1] + step / 2], "edge")
                add("distanceM", [base[0], base[1], base[0] + step, base[1] + step / 2], "edge")
        add("centralAngle", [10.0, 20.0, 10.0, 20.0], "edge")
        add("distanceM", [10.0, 20.0, 10.0, 20.0], "edge")
        add("centralAngle", [-170.0, 10.0, 170.0, 10.0], "edge")
        add("distanceM", [-170.0, 10.0, 170.0, 10.0], "edge")
        quarter = (math.pi / 2) * 6371008.8
        for args in ([0.0, 0.0, quarter, 0.0], [30.0, 89.0, 0.0, math.radians(2) * 6371008.8], [10.0, 20.0, 0.0, 0.0],
                     [179.5, 0.0, 111195.0, 0.0], [-179.5, 0.0, -111195.0, 0.0], [10.0, 20.0, 1.0, 1.0],
                     [0.0, 0.0, 0.0, quarter * 2], [45.0, 45.0, -1.0e7, 1.0e7]):
            add("move", args, "edge")
        for lat1, lat2 in ((0.0, 0.0), (-90.0, 90.0), (90.0, -90.0), (0.0, 90.0), (30.0, 30.0), (-30.0, 30.0)):
            add("zoneFraction", [lat1, lat2], "edge")
    elif unit_name == "climate":
        def season(a, b):
            return [a + b * math.cos(2 * math.pi * (m - 6.5) / 12) for m in range(12)]

        def climate():
            if rng.random() < 0.5:                       # a seasonal climate with a wetter and a drier half-year
                T = season(u(-40, 35), u(-35, 35))
                wet = [300 * rng.random() ** 2, 300 * rng.random() ** 2]
                P = [wet[0 if 3 <= m <= 8 else 1] * rng.random() for m in range(12)]
            else:                                        # the same with months disturbed one by one
                T = [t + u(-2, 2) for t in season(u(-25, 30), u(-25, 25))]
                P = [300 * rng.random() ** 2 for _ in range(12)]
            return [T, P]

        def grid():
            cells = rng.randrange(1, 7)
            T = [[0.0] * cells for _ in range(12)]
            spread = [[u(2, 14) for _ in range(cells)] for _ in range(12)]
            ppt = [[0.0] * cells for _ in range(12)]
            for k in range(cells):
                t, p = climate()
                for m in range(12):
                    T[m][k], ppt[m][k] = t[m], p[m]
            tmax = [[T[m][k] + spread[m][k] / 2 for k in range(cells)] for m in range(12)]
            tmin = [[T[m][k] - spread[m][k] / 2 for k in range(cells)] for m in range(12)]
            return [tmax, tmin, ppt]

        many("koppen", climate)
        many("koppenQuantities", climate)
        many("dryMonths", climate)
        many("meanTemperature", lambda: [[u(-60, 55) for _ in range(12)], [u(-70, 45) for _ in range(12)]])
        saved, n = n, 60
        many("koppenGrid", grid)
        n = saved
        for code in ["Af", "Am", "Aw", "BWh", "BWk", "BSh", "BSk", "Csa", "Csb", "Csc", "Cwa", "Cwb", "Cwc", "Cfa", "Cfb", "Cfc",
                     "Dsa", "Dsb", "Dsc", "Dsd", "Dwa", "Dwb", "Dwc", "Dwd", "Dfa", "Dfb", "Dfc", "Dfd", "ET", "EF"]:
            add("koppenValue", [code], "main")
        for code in ("Xx", "af", "", "B", "ET "):
            add("koppenValue", [code], "edge")

        def flat(v):
            return [float(v)] * 12

        def halves(april_to_september, october_to_march):     # precipitation month by month, January first
            return [float(april_to_september[m - 3]) if 3 <= m <= 8 else float(october_to_march[(m + 3) % 12]) for m in range(12)]

        warm = [5.0, 6.0, 9.0, 13.0, 17.0, 20.0, 22.0, 21.0, 17.0, 12.0, 8.0, 5.0]
        boundaries = [
            (flat(18), flat(250)),                                                   # the coldest month at exactly 18
            (flat(10), flat(100)),                                                   # the warmest month at exactly 10
            (warm, flat(80)),                                                        # the warmest month at exactly 22
            ([0.0, 1.0, 4.0, 9.0, 14.0, 18.0, 20.0, 19.0, 15.0, 9.0, 4.0, 1.0], flat(60)),            # the coldest month at exactly 0
            ([-38.0, -35.0, -25.0, -10.0, 0.0, 8.0, 11.0, 9.0, 1.0, -12.0, -28.0, -36.0], flat(30)),   # the coldest month at exactly -38
            ([-39.0, -35.0, -25.0, -10.0, 0.0, 8.0, 11.0, 9.0, 1.0, -12.0, -28.0, -36.0], flat(30)),
            (flat(25), [60.0] + [200.0] * 11),                                       # the driest month at exactly 60
            (flat(25), [59.0] + [200.0] * 11),
            ([1.0, 2.0, 5.0, 10.0, 10.0, 12.0, 12.0, 12.0, 10.0, 6.0, 3.0, 1.0], flat(70)),            # months at exactly 10
            ([15.0, 5.0, 15.0, 15.0, 5.0, 15.0, 5.0, 15.0, 5.0, 5.0, 15.0, 5.0], halves([10] * 6, [100] * 6)),   # half-years equal in temperature
            (flat(3), [10.0, 10.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 10.0, 10.0]),      # precipitation at exactly ten thresholds
            (flat(3), [5.0, 5.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 5.0, 5.0]),          # and at exactly five
            (flat(0), flat(20)),                                                     # the warmest month at exactly 0
            (flat(18), flat(10)),                                                    # an arid climate with a mean of exactly 18
            (warm, halves([2, 2, 2, 2, 2, 390], [3, 3, 3, 3, 3, 385])),              # dry summer and dry winter, the half-years equal in rain
        ]
        for T, P in boundaries:
            add("koppen", [T, P], "edge")
            add("koppenQuantities", [T, P], "edge")
        add("dryMonths", [[float(t) for t in range(12)], [2.0 * t for t in range(12)]], "edge")
        add("dryMonths", [flat(10), flat(20)], "edge")
        add("dryMonths", [flat(-5), flat(0)], "edge")
        add("meanTemperature", [flat(10), flat(0)], "edge")
    for index, case in enumerate(cases):
        case["id"] = index
    return cases
