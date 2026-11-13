# The Codebook of Historical Objects

*Version 1, format `noetarium.histos/1` (ADR 0110). Status: draft.* How a record of historical objects is written, so that two keepers reading the same sources write the same record, and so that every statement says how it stands. It stands beside the [codebook of occasions, beings and ties](codebook.md), which it does not replace. The C++ that reads it is [`objects/record.hpp`](objects/include/history/objects/record.hpp); the checks hold the two together.

## 1. Principles

1. **A historical object is anything in history that can be inquired into**: an event, a period, a place, a person, a polity, a work, an artifact, a theory, a kind of music, a problem.
2. **A record holds assertions, never facts.** Every statement says who states it and with what grade; its standing follows by rule (§6).
3. **The model proposes; it is never the source.** What a language model writes from memory is of grade C whatever it says, and is in the default layer of no reader.
4. **Conditions are recorded.** What a thing was made with and done by (an instrument, a press, a practice, an institution) is an object like any other, and a premise of what it made possible.
5. **A silence is said.** Where the record does not answer, the answer is that the record is silent, never a guess.

## 2. The File

A record is a file of JSON lines, in UTF-8, each line one row with a `"row"`: `record`, `kind`, `relation-type`, `source`, `object`, `relation` or `emergence`. Empty lines and lines beginning with `#` are passed over. Rows may come in any order, except that a kind names only kinds written before it. Ids are unique within their sort.

```
{"row": "record", "id": "…", "format": "noetarium.histos/1", "title": "…", "about": "…"}
```

`"sources": n` in the record's row changes how many confirmed sources establish a statement (2 when not given).

## 3. Kinds

The core's kinds, each a kind of those to its right:

| Kind | Is a kind of | What it is |
|---|---|---|
| `Event` | Object | something that happened, at a date |
| `Period` | Object | a span of time named by a tradition; its bounds are a convention, and the row says whose |
| `Place` | Object | a place on the Earth, with `lat` and `lon` in degrees on WGS 84 |
| `Agent` | Object | who acts: `Person`, or `Collective` with `Institution` and `Polity` |
| `Work` | Object | a thing of the mind made once: a text, a composition |
| `Artifact` | Object | a material thing: a manuscript, an instrument, a set of bells |
| `Idea` | Object | `Concept`, `Theory`, `Problem` |
| `Tradition` | Object | what classifies: a style, a `Genre`, a `School` (which is also a `Collective`) |
| `Technology` | Object | a kind of tool or technique |
| `Practice` | Object | what is done in common: a way of performing, of teaching, of paying |

A discipline adds kinds beneath these: `{"row": "kind", "id": "Tuning", "name": "tuning", "under": ["Theory"], "plugin": "music"}`.

*Rule.* A **problem** is an object: it is posed, it has conditions, it is answered or stays open. A **condition** is not a kind but a role (§5): anything may be a condition of something.

## 4. Objects

```
{"row": "object", "id": "equal-zhu", "kind": "Tuning", "name": "Zhu Zaiyu's equal temperament", "names": {"zh": "新法密率"},
 "date": "1584", "tradition": "chinese", "discipline": "music", "summary": "…", "said": [ … ]}
```

- **`name`** is in English; **`names`** holds the name in its own script, by language tag.
- **`date`** is EDTF, in the historians' convention (Julian before 15 October 1582) unless `"calendar"` says `gregorian`, `julian` or `julian-march`. Years before the era are astronomical: 239 BC is `-0238`. A life or a span is an interval; an approximate date ends in `~`; a date known only within centuries is an interval. An object *appears* at the beginning of its date.
- **Places** are objects; an object is placed by a relation `at`. Places are named by the data library's policy (ADR 0069), and no border of any time is recorded.
- **What a discipline adds** is written in the row and kept as written: a tuning's `"sound": {"tuning": "meantone-1/4"}`, a dispute's `"map"`.

## 5. Relations and Emergences

A relation joins two objects by a type, which says between which kinds it holds.

| Family | Type | From → to | Read |
|---|---|---|---|
| layer | `specialises` | Object → Object, of comparable kinds | the North German organ school specialises Baroque music |
| layer | `exemplifies` | Object → Tradition, Idea, Technology or Practice | this fugue exemplifies the fugue |
| layer | `part_of` | Object → Object, of comparable kinds | an aria is part of an opera |
| layer | `within` | Object → Period | falls within |
| dependence | `enabled_by` | Object → Object | role: condition |
| dependence | `answers` | Object → Problem | role: problem |
| dependence | `draws_on`, `taught_by` | Object → Object; Person → Person | role: source |
| dependence | `against` | Object → Object | role: against |
| association | `made_by`, `member_of`, `at`, `held_by`, `sets_out` | see `record.hpp` | |

- **The three relations of "extends" are kept apart.** A kind is a kind of another (§3). An object *specialises* an object. An object *exemplifies* a classifying object, which is itself an object. A part of what exemplifies a class does not exemplify it.
- **An emergence** is the appearing of one object with its premises *together*, each in a role:

```
{"row": "emergence", "id": "zhu-1584", "product": "equal-zhu", "date": "1584", "marked_by": ["luxue-1584"],
 "premises": [{"what": "p-circle", "role": "problem", "note": "…"}, {"what": "abacus", "role": "condition"}], "said": [ … ]}
```

- **A relation of dependence is a premise written short**: the premises so written for one object are gathered into one emergence of it. Two emergences written for one object are alternatives: the object rests on neither alone.
- **What is refused:** a relation between kinds its type does not allow; a circle of `specialises`, `exemplifies`, `part_of` or `within`; a circle of dependence; a premise that begins after what it is a premise of has appeared; a problem's role taken by what is no problem.

## 6. What Is Said, and How It Stands

Every object, relation, emergence and premise may carry `"said"`, a list of assertions:

```
{"by": "wtc-title", "grade": "B", "quote": "…", "where": "title page", "documentary": true, "confirmed": "a keeper, 2026-11-13"}
```

| Grade | Meaning | Its source's kind |
|---|---|---|
| A | a structured source records it: an authority file, a catalogue | `structured`, `reference`, `primary` |
| B | a named text states it, and the passage is quoted | `reference`, `primary` |
| C | a language model proposes it, from memory | `model`, and only `model` |

- **`documentary`**: the source is the thing itself speaking (an author's own sentence, a dated title page).
- **`confirmed`**: who held the assertion against its source, and when. An assertion not confirmed is a reference to be looked up.
- **`disputes`**: the source denies the statement.

**Standing**, from the assertions:

| Standing | When |
|---|---|
| disputed | a source that is not a model denies it |
| established | a confirmed documentary assertion of grade A or B; or confirmed assertions of grade A or B by two different sources |
| attested | some assertion of grade A or B, short of that |
| proposed | nothing but a model's word, or nothing |

A reader takes a **layer**: the established alone (the default); with the attested; with the proposed. What is disputed is in no layer. A premise written in an emergence with no word of its own stands as the emergence does.

## 7. What Is Not Recorded

- **Strength.** No number is put on an influence: nothing grounds one.
- **Facts by inheritance.** What is said of Baroque music is not thereby said of a Baroque work.
- **Explanations in the record's own voice.** "Because" is a premise with a role and a source, or it is not there.

## 8. Open Questions

1. **A second coder.** A record's roles and types are one writer's; the agreement of two (nomos's α) comes before any claim that rests on them.
2. **Periods.** Whose convention a period's bounds are is in its summary today, and deserves a field.
3. **Relations with dates.** A relation may carry a date and nothing yet reads it.
4. **The two codebooks.** An emergence is an occasion and an object a being; a record of objects read as a history complex needs the map between them written.
