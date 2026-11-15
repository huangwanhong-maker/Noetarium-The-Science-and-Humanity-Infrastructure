# Law Studio

Status: **draft** (2026-11-15). A work of the infrastructure ([ADR 0121](../../development/decisions/0121-the-law-studio.md)): a project-based studio for legal practice. A project is a matter: its evidence kept and recognised, the law it is read under stated clause by clause, its open terms read with the courts, each question answered with what it rests on, and what remains to be proved. Port 9170. Its name, colour and port are provisional.

It serves **use** first (preparing a matter) and **learning** (seeing how an argument depends on its clauses, its evidence and its readings).

*Caveat.* The studio computes what follows from a scheme, a record of facts and declared readings. A scheme is its author's reading of the law, dated and cited so that it can be checked against the clauses. Nothing here is legal advice.

## 1. Concepts

**A matter is read under a scheme.** A scheme states a body of law clause by clause (the law domain's `schemes`): instruments with their provisions, rules and exceptions each cited to the provisions they render, the open terms with their readings, and the courts' holdings on those terms. The words of a clause come from its official text, fetched by the data library; the scheme only names the clause.

**A question is a consequence of the law.** The studio answers it over the admissible readings of the open terms (apodeixis): *clear* when it follows under every one, *arguable* under some, *untenable* under none. It says what the answer rests on, clause by clause.

**Whose reading binds is declared.** A matter names its forum and the legal orders that bind it. A holding of a court of such an order, on an instrument the court interprets with authority, binds and fixes its reading; another persuades; one given after the date of the question is left out (the law domain's `authority`).

**Evidence is kept, then recognised, then weighed.** Each document is kept as it is with the SHA-256 of its bytes. A passage that says a fact holds, or denies it, is marked in the document's text. An exhibit has a chain of custody, which archeion follows; one whose custody breaks is undercut by that finding and stays in view. What the passages and exhibits give a fact is its standing in a chart of evidence (tekmerion), held to the matter's standard of proof.

**A model proposes and never decides.** A language model may read a document against the matter's facts. It must copy each passage it relies on, and each is looked for in the document: one that is not there is refused, never repaired. What passes is a proposal, which counts for nothing until a person reads it and accepts it. A model may also propose readings of the open terms; a proposal fixes nothing until counsel makes an argument of it.

**The plan of proof says what remains.** For each question: the ways to establish it and to resist it, each a short list of facts to make out, facts to show do not hold and readings to argue for, with none to spare; and what is needed in every way (the law domain's `issues`).

## 2. The Page

Five views of its own, beside the studios' map, table, chart and board.

| View | Shows |
|---|---|
| Matter | The matter as its argument: each question with its answer, the rules that give it, their exceptions, the facts as they stand and the open terms. A small matter is a drawing; a large one is an outline, unfolded where something is still in play. Whatever is chosen opens where it stands: a fact with its standing and evidence, a rule with the words of its clause, a term with who read it how |
| Evidence | The documents as a reading room: the text with its passages marked, the facts with the passages for and against each, exhibits with their custody. Words selected are marked as evidence by hand; "Have a model read it" asks a model |
| Law | The scheme clause by clause, each clause with the rules that state it, marked where they apply in the matter; the scheme's faults, if any; the courts' holdings |
| Readings | Each open term with its readings side by side: the holdings weighed for the forum, the parties' arguments by their canons, a model's proposals; the forum, the date of the question and the ranking of the canons, which the matter states |
| Plan of proof | The ways to establish each question and to resist it; the facts it turns on, where their standing is set |

Several people may work on one matter (`--people`): roles, keys, a sealed journal of every change, and what the others did told to each page (the API's studio host, ADR 0120).

## 3. Plug-ins: Bodies of Law

A plug-in is a directory under `plugins/` with a `plugin.json`, schemes under `schemes/` (`noetarium.scheme/1`) and templates under `templates/<name>/` (a `matter.json`, `noetarium.matter/1`, and its `documents/`).

| Plug-in | Brings |
|---|---|
| `park` | An invented bylaw of an invented borough, after Hart's vehicle in the park: one open term, two courts, a matter of three documents, one with a broken chain of custody. It carries its own words and needs nothing fetched |
| `privacy` | **Transfers of personal data out of the Union.** Chapter V of the GDPR with Articles 3 and 4, the standard contractual clauses (Decision 2021/914) and the EU–US adequacy decision (2023/1795), in the Official Journal's words; five open terms; the Court of Justice's readings (*Breyer*, *EDPS v SRB*, *Schrems II*), each passage found in its judgment, and the Board's Guidelines 2/2018 as persuasive; the treaty frame (ICCPR Article 17, ECHR Article 8, Convention 108) as texts. A matter of four invented documents: an Irish company's nightly transfer of coded health records to a processor in the United States |

The privacy scheme says in its own text what it does not encode (the lawfulness of the processing itself, binding corporate rules in detail, Article 48, the residual derogation, onward transfers), and how things stood on its date: Decision 2023/1795 upheld by the General Court with an appeal pending, the protocol amending Convention 108 not in force.

## 4. Running It

```
python applications/law-studio/engine.py build        # the law engine, with g++ (natively or in WSL); some minutes
python data/products/legal_texts.py                    # the official texts the privacy scheme cites, into the cache
python applications/law-studio/serve.py --open         # add --people --keeper "your name" to work with others
```

The park's example needs only the engine. A language model needs a key, kept for the person at this computer in `models.json` in their settings folder (`%APPDATA%\noetarium`, or `~/.config/noetarium`) as `{"keys": {"anthropic": "..."}}`, never in the repository and never given to a page. A model is asked only from the work's own page at this computer's own address.

## 5. Structure

| Part | Holds |
|---|---|
| `serve.py`, `matters.py` | The server on the API's studio host, and its own routes: a project's matter, documents, evaluation, what a model proposes, templates; the schemes of the plug-ins |
| `engine.py`, `engine/main.cpp` | The law engine: a program around the API's composition `science/law.hpp`, kept running by the server |
| `src/` | The page: `app.js` and one module a view (`matter.js`, `evidence.js`, `law.js`, `readings.js`, `plan.js`), on the visualization layer's studio page and `evidence` drawings |
| `plugins/` | The bodies of law |
| `tests/` | `run-tests.mjs` (46 checks), `check_models.py` (what a model proposes, with a stand-in), `drive.mjs` (the page driven in headless Chrome; not in the suite) |

A project is a folder under `data/cache/law-studio/projects/<id>/`: `matter.json`, `documents/` (each with its digest, and the text read from it), `proposals.json`, the host's `project.json` and `journal.jsonl`.

## 6. Dependencies

The law domain (`schemes`, `authority`, `issues`, `norms`, `exhibits`, `proof`, `facts`), apodeixis, tekmerion and archeion, through the API (`science/law.hpp`, `science.law`, `science.recognise`, `science.models`, `science.studio_host`); the data library's `legal_texts`; the visualization layer's `workspace/studio.js` and `evidence`.

## 7. Open Questions

1. **Colour and name.** A claret (#8E3B5E) and a periwinkle (#6F6BA8) on warm paper, until the user chooses.
2. **Scans.** A document with no text layer is kept and cannot be read; transcription is not built.
3. **Collections.** Near-duplicates, threading, review with certified recall, legal hold and privilege logs (the survey's first section) are not built.
4. **More law.** The lawfulness of processing, the treaty frame as rules for states, other jurisdictions' transfer regimes.
5. **Formats.** LegalRuleML, Akoma Ntoso and the Data Privacy Vocabulary, to import and export.
6. **A panel of interpreters.** tekmerion's interpreters (a stated law over readings, precedents under a prior, a regression, a POMDP over a sequence of cases) are reached from scripts through `science.interpret`; the Readings view shows holdings, arguments and a model's proposals, and does not yet weigh a panel.
7. **Tools.** The studio takes tools as the other studios do (modules of the board's contract, from a plug-in's `tools/`); none is written yet.

## 8. References

- The two surveys: [Models and tools for legal practice](../../research/literature/reports/Models%20and%20tools%20for%20legal%20practice.md), [Sources of international privacy law](../../research/literature/reports/Sources%20of%20international%20privacy%20law.md).
- The law domain's notes: [facts and their registers](../../domains/law/theory/facts-and-their-registers.md), [schemes, authority and the plan of proof](../../domains/law/theory/schemes-authority-and-the-plan-of-proof.md).
- Regulation (EU) 2016/679; Commission Implementing Decisions (EU) 2021/914 and 2023/1795; judgments C-582/14 *Breyer*, C-311/18 *Schrems II*, C-413/23 P *EDPS v SRB*, T-354/22 *Bindl* (under appeal).
