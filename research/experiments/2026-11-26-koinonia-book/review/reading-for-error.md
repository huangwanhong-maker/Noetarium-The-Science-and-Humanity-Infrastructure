# A Reading for Error of the Case of a Book Written by Communities

- **Date:** 2026-11-26
- **Read:** the new and changed code of the case (`applications/koinonia/koinonia/partnership.py`, `authors.py`, `book.py`, `bench.py`, `bookcase.py`, `typeset.py`, the changes to `society.py` and `agents.py`), `api/cpp/science/progress.hpp` and nomos's `measurement/progress.hpp`.
- **By:** an independent reviewer, a separate session given the code and the design and not the author's view of it, asked to report errors, risks and remarks with a failure scenario for each. It changed no file and ran nothing.
- **When:** after a pilot and while the first full run was in its 25th round. The full run was stopped, every error and risk below was mended, and the case was run again from the start. The stopped run is kept as `book-first-attempt`; no number in the book comes from it.

## 1. Errors

| # | Finding | How it was answered |
|---|---|---|
| 1 | An analysis item of a second round (`a-<sid>-r2`) named no section, so its analyst failed every round and the section stayed in analysis. | `section_of` reads a round's suffix; `data_item` and `analysis_item` name both. |
| 2 | A choice of series dropped whole could never be made again: the item stayed closed under the same id. | A new choice is a new item, `d-<sid>-r<k>`. |
| 3 | An overview adopted subject to minor corrections was never corrected, so the book stalled. | The editor corrects it, and it is adopted without another review and audited. |
| 4 | The overview was never asked again after a failed call. | Its guard is cleared when the call fails. |
| 5 | Reviewers' votes were borne by their communities. Counting by origin made two guild peers one origin, and counting by person counted no ballot. | A reviewer's seat is open and its mandate comes from who lent it, so its ballot is borne by that party; the secretary assigns who reviews. Under thesmos a sponsor must answer for whom it seats, so a community cannot seat an organisation's reviewer. Envoys may change how many must agree (any one, majority, two thirds, unanimity); the unit stays origin, since under the others some reviewers' ballots would not be counted at all. |
| 6 | Decisions were read from the votes cast with no quorum, so one envoy's yes could carry a "unanimity" charter. | A decision is carried only when every member of the deciding body has voted, or after four rounds, after which silence is no veto: a declared rule of the case. This holds for charters, ratifications, the outline and its amendments, tools, the proposals and the book. |
| 7 | A refused charter was adopted again for ever. | Refused charters are kept and never adopted again. |
| 8 | A rejected outline looped, its proposal unable to be put again, and the envoys could not amend it. | The outline's proposal is numbered by round; envoys answer each opening once. |
| 9 | Under the editor regime the single editor could not read every version in time, and versions never read were returned and withdrawn. | Reviewers read in the order reviews were opened, and an editor alone reads four a round. A review no one has read is extended, never decided. |
| 10 | The trend to target classed a reversal past the target as on track. | nomos (decision 0019, check 20): a baseline beyond the target whose latest value no longer meets it is regression. |
| 11 | The restricted language could be escaped through a generator's frame. | Attributes are held to an allowlist of the methods and functions the language allows, and a power above 64 is refused. |
| 12 | An amendment for more pages raised a chapter's cost without its words. | The pages are spread over the chapter's sections. |
| 13 | The secretary changed the book before its acts were admitted, and threads could read the book while another wrote it. | A review whose proposal never reached the record is opened again; the book is read through copies. |

## 2. Risks

| # | Finding | How it was answered |
|---|---|---|
| 14 | Claims with scale words, significant figures, a Unicode minus, scientific notation or a year first were judged wrong, and an empty value matched any fact in words. An audit could reopen a section without end. | Every number in a claim is read with the half-width of its last written digit, and a value in words must be three characters at least. A section past the most versions keeps its adoption, and the audit's findings are printed beside it as errata. |
| 15 | An admitted tool could shadow a tool of the bench. | Such a petition's name is given a suffix. |
| 16 | The LaTeX could break on `\item` followed by `[`, a blank line in an argument, or control characters; and the record's table overflowed a column. | `\item{}`; arguments collapse whitespace; control characters are left out; the table is a `supertabular`. |
| 17 | A petition could stick "before the council". | It goes back to its envoy after three rounds. |
| 18 | Recusal left out the replicating analyst's origin. | The replicator's origin is kept with each result and recused. |
| 19 | Several drops in one chapter shifted indices; added chapters went unfunded; an amendment naming no chapter was "passed". | Drops are applied last and from the end; an added chapter is approved by its proposer; an amendment that names no chapter is recorded as not applied. |
| 20 | The spread weighed a unit without a weight as one person. | With weights given, such a unit is left out. |
| 21 | An admitted tool could read series the commons had not accepted; an empty parameter refused a step. | Every parameter a tool names as a series is held to the accepted series; empty parameters are dropped. |

## 3. Remarks

The progress computations match the stated method otherwise. Docstrings, a dead condition and the account's dashes were mended.
