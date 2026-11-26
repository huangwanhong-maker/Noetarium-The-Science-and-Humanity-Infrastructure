# Koinonia's Case Studies: Two Communities, Three, and Four

- **Status:** draft
- **Date:** 2026-11-26
- **Activity:** research
- **Decision:** [ADR 0169](../../../development/decisions/0169-koinonia.md), on [ADR 0167](../../../development/decisions/0167-thesmos-and-the-communities-domain.md)
- **Work:** [Koinonia](../../../applications/koinonia/) (`run_case.py`, `record.py`)
- **Paper:** [Credit and Its Dissolution in a Recorded Society of Human and AI Communities](../../../docs/reports/2026-11-26-koinonia/), whose numbers are read from this record by `values.py`

Every person, organisation and agent in these cases is invented. The catalogue the agents check is the infrastructure's own: the urban-science domain's 1,536 indexes, drafted by a language model and unchecked ([ADR 0128](../../../development/decisions/0128-indexes-and-cities-across-scales.md)). What the agents find is a proposal of grade C for the catalogue's keeper; nothing in the catalogue was changed.

Every run here was made from one version of the work, after an independent reading for error found faults in the first implementation (among them operations on the league's books that changed net positions) and they were mended. The runs made before the reading were discarded. The reading and what changed are in the paper's Appendix C and in [its report](../../../docs/reports/2026-11-26-koinonia/review/reading-for-error.md).

## 1. How the Record Is Kept

Each run is a folder that is a BagIt bag, made by archeion through the API's archive composition and validated by it:

- `data/behaviour.jsonl`: the behaviour journal: every wake, decision, act (admitted or refused, with how it came: from an agent, a person's page or a case's setup), withholding, model call (with the digest of the instructions given, the prompt, the answer, its tokens and cost) and fault, each entry sealed to the one before by archeion's rule;
- `data/rounds.jsonl`: the trajectory, one line a round: who woke, the acts, the work's tallies, the dollars spent, every agent's tally;
- `data/society.json`: the society as the run ended, with every community's record (the acts admitted, in order), the acts refused with their reasons, and the journal's head;
- `data/journal.jsonl`: the rounds sealed by archeion's rule, each entry's note the digest of its round's line;
- `data/prov.json`: the lineage of every verdict and finding in W3C PROV-JSON;
- `data/summary.json`: the run's account, with each community's head (the size and Merkle root of its chronicle) and the behaviour journal's head and count.

`record.py` verifies each behaviour journal against the head and count its run kept, so that a journal cut short is found, and the paper's `values.py` replays every community's chronicle from its record to the head the run kept. Figures are in `figures/`.

To reproduce a run: `python applications/koinonia/run_case.py <case> [options] --record <folder>`; then `python applications/koinonia/record.py research/experiments/2026-11-26-koinonia-cases`. Runs with the stand-in spend nothing and give the same result every time; runs with Claude differ from run to run.

## 2. Two Communities: A Commons and a Guild

`two-claude/`: `run_case.py two --rounds 45 --models`.

- **The commons** (open; stewards accept by any one) checks 48 entries in 12 batches, each by two checks of different origins: Loom's rule checker and Orchard's and Cedar's model checkers (Claude Opus 5.5 at low effort).
- **The guild** (entered by trial) settles disagreements with two adjudicators by model, one lent by an applicant admitted after it gave the agreed finding on two known entries.

**What happened.** In 45 rounds, for $0.37, 37 entries were agreed by two checks and 10 settled by the guild and carried back into the commons' record: 39 hold, 8 at fault. The guild's two adjudicators offered for the same dispute, so the guild settled one dispute at a time; when the run reached its limit of 45 rounds one adjudication had been delivered and not yet accepted. No act was refused.

**The guild overruled the declared rules.** Where the rule checker found an entry at fault and the other check did not, the guild upheld it twice and overruled it three times (`intersection_crash_frequency`, `night_event_count`, `heat_exposure_person_days`). Rule R1 treats a quantity as intensive when its unit contains "per" or its kind is declared a rate, so counts over a period, which add up across places, are flagged: the rule is at fault. Where a model checker found an entry at fault or in doubt against the other, the guild overruled it five times. The checkers by model, of their own initiative, told the commons' stewards four times that disputes were piling up.

## 3. Three Communities Whose Wants Run in a Circle

`run_case.py three --size 60`. The commons (checkers by rule) needs a reading by the guild's method; the guild (a reader by model) needs a check by an outside model; the cooperative (checkers by model) needs a check by rule. So the guild serves the commons, the cooperative the guild and the commons the cooperative, and none can repay in kind the community that serves it. A league on the first league's charter keeps the services (limit of net debt 3).

`three-60-<arrangement>-stand-in/`, the checks by the stand-in so that only the arrangement differs:

| arrangement | rounds | batches left | debts entered | repaid in kind | paid by a claim | cleared | claims passed on | withheld |
|---|---|---|---|---|---|---|---|---|
| gift | 24 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
| credit | 29, stalled | 3 | 12 | 1 | 0 | 0 | 0 | 93 |
| transferable credit | 24 | 0 | 12 | 5 | 4 | 0 | 5 | 2 |
| cleared credit | 24 | 0 | 16 | 1 | 0 | 12 | 0 | 24 |
| provision | 24 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |

Under bilateral credit the circle stalls. A claim passed on unblocks it, and so does clearing on the league's record with every claim staying where it was. Gift and provision coincide here: no agent ever had two communities abroad with open work at once.

## 4. A Free Rider and a Community That Cannot Repay

`run_case.py four --size 96 --free-rider <reacts | reads standing>`. A polity of persons with no checkers is admitted to the league by two thirds of its members; its 12 batches need a check by rule and a check by model, and it can never repay in kind. The cooperative free-rides, by one of two declared rules: one that *reacts* serves abroad only when its community was refused service in one of the last three rounds; one that *reads its standing* serves abroad when the league's record shows its community would be refused. Work arrives one batch every six rounds.

`four-96-<rider>-<arrangement>-stand-in/`: the free rider's services abroad, and the polity's batches settled of 12:

| arrangement | reacts: abroad | reacts: polity settled | reads standing: abroad | reads standing: polity settled |
|---|---|---|---|---|
| gift | 0 | 0 | 0 | 0 |
| credit | 8 | 3 | 8 | 3 |
| transferable credit | 0 | 0 | 4 | 0 |
| cleared credit | 8 | 3 | 8 | 3 |
| provision with duty | 4 | 2 | 16 | 12, all work done |

Gift cannot tell will not from cannot. Credit compels both free riders and caps the polity at what it can owe. A transferable claim lets the free rider that reacts escape, its debt passed to a creditor it does not need. Provision with a duty read from the record serves the polity in full beside a free rider that reads its standing, and little beside one that reacts, since a default lapses once the defaulter has been busy for a while. A duty read from the record binds a party that reads the record.

## 5. Envoys Deciding

`three-envoys-from-<credit | gift>-<k>/`, `four-envoys-from-<credit | gift>-<k>/`: `--envoys --models --models-for envoys`. Each member community sends an envoy by Claude to the league's council, shown what the league's record says; the arrangement is theirs to decide by a majority of the member communities. The checks are the stand-in's, so only the deliberation is paid for.

- **The circle, from credit** (three runs): every run left credit once services were withheld at the limit and ended in provision with duty, no debt outstanding. The cooperative, a debtor holding a claim on the guild, proposed transferable credit every time ("Under transferable credit I can pass guild's debt of 3 to commons, which cuts my net debt"). In one run it was enacted first and three claims were passed on; in two the guild's provision with duty came first and transferable credit was refused.
- **The circle, from gift** (two runs): no credit appeared. The commons, carrying most of the work left, proposed provision, and it was adopted; in this circle it changes nothing.

- **The free rider and the polity** (three runs, the cooperative's checkers reading their standing): the envoys first admitted the polity, unanimously, partly on a reason that was false (that it would add checking capacity; it has no checkers, and nothing they were shown said it had). From credit the council moved to provision with duty, once directly and once after cleared credit, under which nothing flowed; from gift it moved to provision with duty in round 10. Every run ended with all work done, the polity's 12 batches among it, and no envoy proposed bilateral or transferable credit. The cooperative's envoy voted for the duty that binds its community and said, more than once, that the right course was to serve; its checkers followed their declared rule throughout, so an envoy's word is not its community's conduct.

## 6. What These Runs Do Not Show

- **Constructed societies.** The communities, their rules and parameters are declared; the work is real, the society a construction.
- **Declared free riders.** Each free rider follows a declared rule; one that learns to look busy is not tested.
- **One provider.** All model agents are Anthropic's, so judgements counted by provider cannot differ from those counted by agent.
- **Few runs with models.** Each condition with Claude was run one to three times.
- **No crossings with receipts.** Findings carried from the guild record the guild's head; crossings with receipts and proofs of inclusion are not run.
- **Voice.** The polity has no envoy in the league's council.
- **Persons.** Persons act from the page in the work's checks and through their agents' declared rules here; no person decided in real time.

## 7. Spending

Within a budget of $50 stopped at $45, the ledger kept in the person's settings folder stands at $3.93. The nine runs of this record that asked Claude cost $2.48: the two-community case $0.37, the five envoy runs of the circle $0.68, the three of the fourth case $1.42; every stand-in run cost nothing. The rest went on runs discarded before the reading for error and on the page's own use. Each call was reserved before it was made and counted from the tokens reported.
