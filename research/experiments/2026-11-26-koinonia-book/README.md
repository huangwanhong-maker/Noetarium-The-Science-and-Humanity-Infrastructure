# A Book Written by Communities

- **Status:** draft (2026-11-26)
- **Decision:** [ADR 0172](../../../development/decisions/0172-a-book-written-by-communities.md); design record [2026-11-26, a book written by communities](../../../development/ideas/2026-11-26-a-book-written-by-communities.md)
- **Work:** Koinonia (`applications/koinonia`), case `book`, run by `run_book.py`, kept by `record_book.py`, read by `summarise_book.py`
- **The book:** `docs/reports/2026-11-26-koinonia-book/book.pdf` (170 two-column pages, with a note on the experiment in front; sections never written and chapters with none adopted are listed in its last annex, not set), set by the secretariat's typesetter from the record

Four invented communities with no organisation above them were set to write together a global analysis of the Sustainable Development Goals with proposals, a book planned at about 560 two-column pages. Nothing outside the society wrote, chose or corrected any of it; the case's maker changed the society's instruments while it ran, and each change is recorded below and in the book's last annex. Every number here is read from the run's record by `summarise_book.py` (`summary.json`).

## 1. The Record

| Folder | What it holds |
|---|---|
| `book-run/` | The run: a BagIt bag made and validated by archeion. `data/society.json` (every community's constitution and chronicle, the agents, the book with every body delivered), `data/behaviour.jsonl` (61,847 entries, every wake, decision, model call and act, sealed one to the next; it ends at the head the society kept), `data/rounds.jsonl`, `data/journal.jsonl` (the rounds sealed), `data/prov.json` (each number's lineage as W3C PROV: series, results, versions, reviews, agents and who lent them), `data/measures.json` |
| `book-pilot/` | The pilot on six sections. It showed versions a third of the length asked, and reviewers marking rounding as fault |
| `book-first-attempt/` | The first full run, stopped at its 25th round to mend the errors a reading for error found. No number in the book comes from it |
| `review/reading-for-error.md` | The independent reading of the code: thirteen errors and eight risks, each with how it was answered |
| `figures/` | Each run's sections by stage and its spending, round by round, as SVG |
| `summary.json` | What the run did, read from its record |

The pilot's and the first attempt's journals hold entry by entry. Each was stopped during a round, so its last entries come after the head its society last saved (entry 11,337 of 11,394, and 5,815 of 5,920). The run's journal ends at the head it kept.

## 2. What the Society Did

1. **The conference.** The forum convened the four communities. Each envoy (Claude Opus 5.5) proposed a charter, and the commons' was adopted by every community's consent: a chair rotating among the members every twelve rounds, tools admitted to the shared bench by two thirds, review by peers of the guild, and the book adopted by unanimity. Every community then ratified it by its own constitutional rule, through the voices of its deciding body. The partnership was founded in round 4.
2. **The outline.** On the editor's zero draft the envoys put sixteen amendments, of which the council passed nine, the forum's new chapter *Who is left behind* among them. The outline the council adopted held 30 chapters and 166 sections: the overview, the seventeen goals, seven regions, four chapters across the goals and the proposals. The page budget of 469 pages was allotted by the Method of Equal Shares over the members' approvals. It funded 20 chapters; most regional chapters and several environmental goals were left at two pages a section.
3. **Who reviews.** In round 60 the guild's envoy proposed that a panel review each section, one reviewer from each member; the council enacted it in round 62. The forum's later proposal that its own reviewers review was never enacted.
4. **The work.** The commons' gatherers chose and checked series from the World Bank's 1,498 indicators. The guild's analysts kept 459 results replicated by an analyst of another origin, on the bench (nomos's trend to target after the United Nations Statistics Division's method, the spread among countries, convergence, breaks, links) and on three tools the society admitted. The cooperative's writers wrote 247 versions. The partnership's reviewers wrote 694 reviews: 537 recommending revision, 151 minor corrections and 6 adoption. The faults most often major were of inference (435): a result read backwards, or a cause claimed that no result shows.
5. **The audit.** The auditor checked 1,344 numbers of the adopted versions against the facts they cite, and found 5 wrong. Three sections were reopened, and two kept their adoption with the findings printed as errata.
6. **Extending the infrastructure.** The agents filed 275 petitions. 180 asked for data the World Bank's catalogue does not hold, and were recorded as gaps of the shelf. Of the 95 for tools, six were declined by their envoys and 62 never carried. The council admitted 27. One was not yet built when the run ended. Twenty-three were built by the guild and found unsound by a checker of another origin. Three were built, found sound and admitted to the bench: a compound rate over a chosen period (round 65), a comparison of national poverty over a fixed panel of countries (round 107), and the pace still required to reach a target (round 111).
7. **The ending.** 32 sections were adopted. 27 sections were withdrawn when their fifth version did not pass the panel, the editors' overview among them (the panel voted against its fifth version in round 206 and the withdrawal was attested in round 207; the note on the run's end, written by the case's maker into the book's events and interventions and carried into `summary.json`, says the overview was adopted, which is wrong, and the book's last annex says so). 102 sections were never written, at the user's decision to finish what was in hand within the budget. Each community's envoy drafted its proposals, and the chapter was put to the council in round 207. From round 216 the model provider refused every call, the account's credit balance being too low. Neither the proposals nor the book was voted on, and the run ended in round 227.

## 3. What It Cost

The run cost $84.87 in calls to models: writers $49.94 (1,359 calls), reviewers $14.40 (1,940), analysts $5.98, envoys $5.56, editors $5.54, gatherers $3.45 and voices $0.00. The ledger for the work's models, with the pilot, the first attempt and Koinonia's earlier cases, stood at about $95 when the provider stopped. The budget was $80, then $100 at the user's word.

## 4. Interventions

Ten changes came from outside the society, recorded where they happened (`book.call.interventions`) and in the book's last annex:

- **By the case's maker:**
  - round 61: writing by parts, and the writers on Opus moved to Sonnet;
  - round 68: the reviewers' form of faults, and the non-guild reviewers moved to Haiku;
  - round 72: what each call carries trimmed;
  - round 82: each tool's summary says how its result is read;
  - round 86: a rule of re-review, and revisions confined to the parts their faults point to;
  - round 98: a fault in the reviewers' token limit mended (the note in the record counts 176 of 243 calls cut off since round 93; the behaviour journal shows 1,199 of 1,940 reviewer calls cut off from round 69, after the change of round 68, to round 99, 984 of them before round 93);
  - round 194: the same fault in the overview's and the proposals' limits mended, and the editors moved to Sonnet.
- **By the user:**
  - round 93: half the words for sections not yet drafted;
  - round 107: finish what is in hand within $100.
- **By the provider:** rounds 216 to 227, every call refused for want of credit.

## 5. Findings

1. **No centre was needed to organise, and the society organised itself quickly.** Four communities of different orders reached a charter by consent, ratified it each by its own rule, and founded an organisation in four rounds. They amended its outline by vote, allotted its pages by a rule of proportional justice, and changed who reviews when the record gave them a reason.
2. **Its review was strict, and stricter than its budget.** Under the panel, a section needed about three versions; 27 sections failed after five. Reviewers of every model recommended revision about two times in three, mostly for faults of inference that were real. The society's quality standard, not its organisation, set the cost of the book.
3. **What was adopted is numerically sound.** Five wrong numbers in 1,344 audited.
4. **The politics of extending the infrastructure filtered heavily.** Of 95 petitions for tools, three reached the bench. Most petitions named data, not computation: what the agents most lacked was evidence the catalogue does not hold.
5. **The instruments, not the institutions, failed first.** Each of the seven interventions by the case's maker mended an instrument: a writer's form, a reviewer's form, a token limit, a tool's summary. None changed a rule the society had chosen. Two of them mended faults in the case's own code that cost money and stalled the review.
