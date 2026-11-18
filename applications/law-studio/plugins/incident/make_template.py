"""Writes the Law Studio's example matter *The fire at the boathouse* (ADR 0127): its matter.json and its documents. Every person,
place and word is invented.

    python applications/law-studio/plugins/incident/make_template.py applications/law-studio/plugins/incident/templates/the-boathouse
"""

import json
import sys
from pathlib import Path

base = Path(sys.argv[1])
docs = base / "documents"
docs.mkdir(parents=True, exist_ok=True)

documents = {
    "Fire investigator's report.txt": """WRENFIELD FIRE AND RESCUE — FIRE INVESTIGATION REPORT (summary)
Premises: the boathouse of Wrenfield Rowing Club, on the towpath by the river Wren.
Date of the fire: Saturday 14 March 2026.

Origin. The fire began at the foot of the east door, on the river side. Burn patterns and the depth of charring place the point of
origin within about six metres of that door.

Cause. Samples taken at the door contained paraffin. In my opinion a flame was applied to paraffin poured at the door. The
paraffin heater inside the boathouse was found switched off at the wall, and I found no sign of an electrical or heater fault.

Time. From the extent of the fire when the first crew arrived (22:06) and the materials of the building, I estimate that the
fire began between 21:35 and 21:55.

R. Osei, fire investigator
""",
    "Fire service log of calls.txt": """WRENFIELD FIRE AND RESCUE — LOG OF CALLS, 14 MARCH 2026 (extract)
21:58:12  Caller on the Mill Lane footbridge reports the boathouse on the towpath alight, flames at the east end.
22:06:40  First appliance in attendance.
""",
    "Telephone company record.txt": """NORTHLINK TELECOM — RECORD OF NETWORK EVENTS (produced under order)
Subscriber: Corrie Vane.
14/03/2026 21:31:07  data session  mast WRN-04 (Wrenfield Lane), sector 2.
Note: sector 2 of mast WRN-04 serves an area of up to about 900 metres from the point (-300, 500) on the matter's map.
""",
    "The Bell's camera, note of the still.txt": """NOTE OF A STILL FROM THE BELL INN'S DOOR CAMERA
The camera's clock reads 21:47:02. The landlord says the clock runs four minutes fast, and has done since the clocks changed; the
still was therefore taken at about 21:43.
It shows a woman in a dark jacket wheeling a bicycle away from the door. Her face is turned from the camera. She resembles
Corrie Vane in height and build. No one has identified her.
""",
    "Statement of Ada Vane.txt": """STATEMENT OF ADA VANE (sister of the accused)
On the evening of 14 March I rang my sister on the landline at Mill Row at about ten to ten. The call was answered and we spoke
for about six minutes. It was Corrie who answered. She sounded out of breath and said she had just got in.
The telephone company's record shows a call to the Mill Row landline from 21:51 to 21:57.
""",
    "Statement of a jogger.txt": """STATEMENT OF L. PRICE
I was running over the Mill Lane footbridge at about ten to ten, give or take five minutes. A man in a long grey coat was
standing on the bridge looking towards the boathouse. I did not see his face. I did not see any fire then.
""",
    "The caretaker's log.txt": """WRENFIELD ROWING CLUB — CARETAKER'S LOG
14/03/2026  20:15  Locked up. Heater off at the wall. All lights off.  T. Ashby
""",
    "Statement of Tom Ashby.txt": """STATEMENT OF TOM ASHBY (caretaker)
I locked the boathouse at a quarter past eight and went home to my cottage. I was at home at half past nine, watching the news.
Later I walked up to the Bell for a pint and paid at the bar just before ten. I walk with a stick since my operation; my doctor's
letter says I can manage about one metre and a fifth a second at my briskest.
""",
    "The Bell's till roll for Mr Ashby.txt": """THE BELL INN — TILL ROLL (extract), 14/03/2026
21:51:58  1 x bitter  £4.20  card ending 0417 (T ASHBY)
""",
    "Garage receipt.txt": """STATION ROAD SERVICE STATION — CARD RECEIPT
14/03/2026  19:12:48
Paraffin 4 L can   £11.80
Card ending 2290 (C VANE)
""",
    "Statement of Mr Hale.txt": """STATEMENT OF G. HALE
From my window across the river I saw a small light, like a lighter or a torch, at the east end of the boathouse. I think it was
between a quarter to ten and ten to ten. It went out after a few seconds. I could not see who held it.
""",
    "The Bell's till roll, disclosed late.txt": """THE BELL INN — TILL ROLL (extract), 14/03/2026, disclosed by the defence on 20 May 2026
21:44:20  1 x lime and soda  £2.10  card ending 2290 (C VANE)
""",
}
for name, text in documents.items():
    (docs / name).write_text(text + "\n(An invented document, written for the Law Studio's example.)\n", encoding="utf-8")

matter = {
    "schema": "noetarium.matter/1",
    "id": "the-boathouse",
    "title": "The fire at the boathouse (invented)",
    "about": "An invented matter: the boathouse of Wrenfield Rowing Club set alight on the evening of 14 March 2026, and Corrie Vane charged with damage by fire. Twelve items of evidence say where people were and when; one, a till roll disclosed late by the defence, is held back from the search so that its theories can be tested on it. Every person, place and word is made up.",
    "scheme": "incident/fire-ordinance",
    "date": "2026-06-01",
    "epoch": "2026-03-14T21:30:00+00:00",
    "forum": {"id": "wrenfield-court", "name": "the Wrenfield Crown Court (invented)", "bound_by": ["Wrenfield"]},
    "standard": {"name": "so that the court is sure", "argument": "justified"},
    "from_evidence": False,
    "issues": [{"id": "damage", "question": "damage_by_fire", "burden": "the prosecution", "standard": "so that the court is sure"}],
    "facts": [
        {"id": "property_of_another", "text": "the boathouse belongs to Wrenfield Rowing Club", "standing": "admitted", "basis": "agreed facts"},
        {"id": "accused_set_fire", "text": "Corrie Vane set fire to the boathouse", "standing": "alleged"},
        {"id": "bought_fuel_beforehand", "text": "Corrie Vane bought paraffin that evening", "standing": "alleged"},
    ],
    "candidates": [], "exhibits": [], "arguments": [
        {"id": "pros_preparation", "term": "intention", "reading": "inferred from preparation", "canon": "text", "text": "Section 1(2) says intention may be found from what the person did beforehand: buying paraffin two hours before is such a thing."},
        {"id": "def_direct", "term": "intention", "reading": "shown directly", "canon": "purpose", "text": "Section 1(2) first forbids inferring intention from the result alone; buying paraffin, which heats many homes, shows nothing of an intention to burn a boathouse."},
    ],
    "undercuts": [], "canon_order": [],
    "place": {
        "title": "Wrenfield, by the river Wren",
        "frame": "metres east and north of the boathouse's east door",
        "scene": {"x": 0, "y": 0, "name": "the boathouse's east door"},
        "water": [{"name": "the river Wren", "points": [[-1500, -35], [-900, -50], [-400, -40], [0, -28], [500, -55], [1000, -70], [1800, -90], [1800, -170], [1000, -150], [500, -135], [0, -108], [-400, -120], [-900, -130], [-1500, -115]]}],
        "buildings": [
            {"name": "the boathouse", "points": [[-26, -10], [0, -10], [0, 16], [-26, 16]]},
            {"name": "the Bell inn", "points": [[-585, 200], [-545, 200], [-545, 236], [-585, 236]]},
            {"name": "Mill Row (Corrie Vane's house)", "points": [[1630, 925], [1670, 925], [1670, 960], [1630, 960]]},
            {"name": "Tom Ashby's cottage", "points": [[-970, 105], [-935, 105], [-935, 135], [-970, 135]]},
            {"name": "the service station", "points": [[880, -720], [930, -720], [930, -680], [880, -680]]},
            {"name": "mast WRN-04", "points": [[-306, 494], [-294, 494], [-294, 506], [-306, 506]]},
        ],
        "ways": [
            {"name": "the towpath", "kind": "path", "points": [[-1500, -15], [-900, -30], [-400, -22], [-40, -14], [0, -14], [500, -38], [1000, -52], [1800, -72]]},
            {"name": "Wrenfield Lane", "kind": "road", "points": [[-1500, 250], [-900, 240], [-560, 195], [0, 180], [700, 420], [1300, 700], [1650, 920]]},
            {"name": "the Mill Lane footbridge", "kind": "bridge", "points": [[-120, -22], [-120, -122]]},
            {"name": "Station Road", "kind": "road", "points": [[600, -160], [900, -700], [1100, -1000]]},
            {"name": "Mill Row", "kind": "road", "points": [[1450, 860], [1850, 1010]]},
            {"name": "the boathouse lane", "kind": "path", "points": [[-13, 16], [0, 180]]},
        ],
    },
    "agents": [
        {"id": "vane", "name": "Corrie Vane", "speed": 6.0, "basis": "she rides a bicycle (her statement); 6 m/s is a fast ride on the lane", "identified": True},
        {"id": "ashby", "name": "Tom Ashby", "speed": 1.2, "basis": "he walks with a stick; his doctor's letter puts him at 1.2 m/s at his briskest", "identified": True},
        {"id": "grey", "name": "the man in the grey coat", "speed": 2.0, "basis": "known only as seen; 2 m/s, a brisk walk", "identified": False},
    ],
    "kinds": [
        {"id": "ignite", "gloss": "a flame put to paraffin at the door", "roles": ["actor"]},
        {"id": "fault", "gloss": "a fault of the heater, by no one's hand", "roles": []},
    ],
    "story": [{"name": "origin", "gloss": "how the fire began", "kinds": ["ignite", "fault"]}],
    "rooms": [{"kind": "origin", "x": 0, "y": 0, "radius": 6, "from": "21:35", "until": "21:55"}],
    "located": [
        {"id": "fire-report", "says": "event", "kind": "ignite", "x": 0, "y": 0, "radius": 6, "from": "21:35", "until": "21:55", "credibility": 0.85,
         "source": "R. Osei, fire investigator", "modality": "text", "document": "Fire investigator's report.txt", "text": "a flame applied to paraffin at the east door, between 21:35 and 21:55"},
        {"id": "fire-call", "says": "event", "kind": "origin", "x": 0, "y": 0, "radius": 15, "from": "21:30", "until": "21:58:12", "credibility": 0.99,
         "source": "the fire service's log", "modality": "record", "document": "Fire service log of calls.txt", "text": "the boathouse alight by 21:58"},
        {"id": "mast", "says": "sighting", "agent": "vane", "x": -300, "y": 500, "radius": 900, "at": "21:31:07", "credibility": 0.95,
         "source": "Northlink Telecom", "modality": "record", "document": "Telephone company record.txt", "text": "Corrie Vane's telephone on mast WRN-04, sector 2"},
        {"id": "inn-camera", "says": "sighting", "agent": "vane", "x": -560, "y": 210, "radius": 3, "at": "21:43:02", "credibility": 0.6,
         "source": "the Bell's door camera", "modality": "image", "document": "The Bell's camera, note of the still.txt", "text": "a woman like Corrie Vane wheeling a bicycle from the inn's door"},
        {"id": "landline", "says": "sighting", "agent": "vane", "x": 1650, "y": 940, "radius": 15, "at": "21:51", "credibility": 0.7,
         "source": "Ada Vane, her sister", "modality": "testimony", "document": "Statement of Ada Vane.txt", "text": "Corrie answered the landline at Mill Row at 21:51"},
        {"id": "jogger", "says": "sighting", "agent": "grey", "x": -120, "y": -70, "radius": 20, "from": "21:45", "until": "21:55", "credibility": 0.6,
         "source": "L. Price, a jogger", "modality": "testimony", "document": "Statement of a jogger.txt", "text": "a man in a grey coat on the footbridge at about ten to ten"},
        {"id": "hale", "says": "event", "kind": "ignite", "x": 0, "y": 0, "radius": 10, "from": "21:44", "until": "21:48", "credibility": 0.55,
         "source": "G. Hale, across the river", "modality": "testimony", "document": "Statement of Mr Hale.txt", "text": "a small light at the east end, between a quarter and ten to ten"},
        {"id": "caretaker-log", "says": "absence", "kind": "fault", "x": 0, "y": 0, "radius": 15, "from": "20:15", "until": "22:00", "credibility": 0.6,
         "source": "Tom Ashby's log", "modality": "document", "document": "The caretaker's log.txt", "text": "the heater off at the wall from 20:15"},
        {"id": "ashby-home", "says": "sighting", "agent": "ashby", "x": -950, "y": 120, "radius": 10, "at": "21:30", "credibility": 0.5,
         "source": "Tom Ashby", "modality": "testimony", "document": "Statement of Tom Ashby.txt", "text": "at home in his cottage at half past nine"},
        {"id": "ashby-till", "says": "sighting", "agent": "ashby", "x": -560, "y": 210, "radius": 5, "at": "21:51:58", "credibility": 0.95,
         "source": "the Bell's till roll", "modality": "record", "document": "The Bell's till roll for Mr Ashby.txt", "text": "Tom Ashby paid at the Bell's bar"},
        {"id": "receipt", "says": "sighting", "agent": "vane", "x": 905, "y": -700, "radius": 10, "at": "19:12:48", "credibility": 0.95,
         "source": "Station Road service station", "modality": "record", "document": "Garage receipt.txt", "text": "Corrie Vane's card buys four litres of paraffin"},
        {"id": "late-till", "says": "sighting", "agent": "vane", "x": -560, "y": 210, "radius": 3, "at": "21:44:20", "credibility": 0.97, "held_back": True,
         "source": "the Bell's till roll, disclosed late", "modality": "record", "document": "The Bell's till roll, disclosed late.txt", "text": "Corrie Vane's card pays at the Bell's bar at 21:44:20"},
    ],
    "bindings": {
        "makes": [{"kind": "ignite", "role": "actor", "agent": "vane", "fact": "accused_set_fire"}],
        "grounds": [{"item": "receipt", "fact": "bought_fuel_beforehand"}],
    },
    "theories": [
        {"by": "the prosecution", "text": "event ignite actor=vane at 0 0 when 21:45:10\nbelieve jogger when 21:50:00\n"},
        {"by": "the defence", "text": "event ignite actor=grey at 0 0 when 21:46:00\nbelieve jogger when 21:48:00\n"},
    ],
    "presence": {
        "agents": ["vane", "ashby"],
        "places": [{"name": "at the east door", "radius": 6, "source": "analyst", "by": "the investigator's point of origin", "stated": True},
                   {"name": "in the grounds", "radius": 40, "source": "analyst", "by": "the boathouse's fence"}],
        "times": [{"name": "the investigator's window", "from": "21:35", "until": "21:55", "source": "analyst", "by": "the fire investigator's report", "stated": True},
                  {"name": "while Mr Hale saw the light", "from": "21:44", "until": "21:48", "source": "analyst", "by": "Mr Hale's statement"}],
    },
    "search": {"generations": 12, "mu": 36, "lambda": 36, "threshold": 0.05, "invention": 1, "hole": 2, "key": 20261118},
}
(base / "matter.json").write_text(json.dumps(matter, ensure_ascii=False, indent=1), encoding="utf-8")
print("written", len(documents), "documents")
