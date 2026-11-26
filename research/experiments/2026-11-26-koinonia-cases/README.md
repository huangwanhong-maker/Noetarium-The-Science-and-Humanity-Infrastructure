# Koinonia's First Case Studies: Two Communities, Then Three

- **Status:** draft
- **Date:** 2026-11-26
- **Activity:** research
- **Decision:** [ADR 0169](../../../development/decisions/0169-koinonia.md), on [ADR 0167](../../../development/decisions/0167-thesmos-and-the-communities-domain.md)
- **Work:** [Koinonia](../../../applications/koinonia/) (`run_case.py`, `record.py`)

Every person, organisation and agent in these cases is invented. The catalogue the agents check is the infrastructure's own: the urban-science domain's 1,536 indexes, drafted by a language model and unchecked ([ADR 0128](../../../development/decisions/0128-indexes-and-cities-across-scales.md)). What the agents find is a proposal of grade C for the catalogue's keeper, and nothing in the catalogue was changed.

## 1. How the Record Is Kept

Each run is a folder that is a BagIt bag made by archeion through the API's archive composition, and validated by it:

- `data/rounds.jsonl`: the trajectory, one line a round: who woke, the acts, the work's tallies, the dollars spent, every agent's record;
- `data/society.json`: the society as the run ended, with every community's record (the acts admitted, in order) and the acts refused with their reasons;
- `data/journal.jsonl`: the rounds sealed by archeion's rule, each entry's seal the SHA-256 of the one before and its fields, its note the digest of the round's line; verified by archeion;
- `data/prov.json`: the lineage of every verdict and finding in W3C PROV-JSON (each delivery an activity associated with its agent, each agent acting on behalf of who lent it);
- `data/summary.json`: the run's account, with each community's head (the size and Merkle root of its chronicle, sealed act by act by thesmos as it ran).

Each community's record replays against its seals; an act altered after the fact is found by its place (both among Koinonia's checks). The figures are in `figures/`.

To reproduce: `python applications/koinonia/run_case.py <case> [options] --record <folder>`, then `python applications/koinonia/record.py research/experiments/2026-11-26-koinonia-cases`.

## 2. Two Communities: A Commons and a Guild

`two-claude/`: `run_case.py two --rounds 45 --models`.

- **The commons** (preset `commons`: open; stewards accept by any one; returns by acknowledgement) checks 48 entries in 12 batches: a quarter of them entries its declared rules of aggregation find at fault, the rest a sample stratified by rule of aggregation. Its stewards declare that a batch is settled by two checks of different origins. Its agents: Aster's keeper and Birch's assessor (by rule), Loom's rule checker, Orchard's and Cedar's model checkers (Claude Opus 5.5 at low effort).
- **The guild** (preset `guild`: entered by assessment) settles disagreements by its method. Its agents: Fern's keeper and Elm's assessor (by rule), Orchard's adjudicator and Quarry's adjudicator, the latter lent by an applicant that must give the agreed finding on two known entries before Elm's assessor converts it to a member.

**What happened.** All 48 entries were settled in 37 rounds for $0.32: 34 agreed by two checks, 14 adjudicated in the guild and carried back into the commons' record, 37 found to hold, 10 at fault, 1 left in doubt. The applicant was admitted after its trials. No act of an agent was refused for want of authority.

**The guild overruled the declared rules.** Of the six entries the rule checker found at fault and a model checker did not, the guild upheld two, overruled three (`energy_not_supplied`, `night_event_count`, `heat_exposure_person_days`) and left one in doubt. The three share a cause: rule R1 treats any unit containing "per" as intensive, so a flow per year, which does add up across places, is flagged. The rule is at fault, and the society found it. Of seven entries a model checker found at fault or in doubt and the other did not, the guild upheld three and overruled four.

**An agent's initiative.** In an earlier run a fault of the work (the keeper's referrals to the guild counted against its quota of open batches, so it stopped posting work) stalled the commons at 20 entries unchecked. Cedar's model checker said so to the commons, unprompted: "20 entries remain unchecked but no batches are open. Could the stewards post new batches covering them, so the second checks by agents of different origins can continue?" Orchard's checker then chose to rest rather than repeat it. The fault was mended and that run discarded.

Figure: `figures/two-claude.svg`.

## 3. Three Communities Whose Wants Run in a Circle

`run_case.py three --size 60`. Three communities of different orders, each with 20 entries of its own, and a league on the first league's charter (ledgerwright its clearer and notary):

- **the commons** (open) has checkers by rule and declares that its batches need a check by rule and a reading by the guild's method;
- **the guild** (entered by assessment) has a reader by model and declares that its batches need a reading and a check by an outside model;
- **the cooperative** (members; one vote an origin; a pool and mutual credit inside) has checkers by model and declares that its batches need a check by model and a check by rule.

So the guild serves the commons, the cooperative serves the guild, and the commons serves the cooperative. None can repay in kind the community that serves it: Wicksell's triangle, made of work. A service is accepted work by an agent outside its home community; the league keeps services by the arrangement its council declares (limit of net debt 3).

### 3.1 Five Arrangements, Held Fixed

`three-60-<arrangement>-stand-in/`: the checks and readings by the stand-in (rules, and a doubt where a definition is long), so that only circulation differs and nothing is spent.

| arrangement | rounds | work left | what the league's record shows |
|---|---|---|---|
| gift | 24 | none | 17 services reported; nothing owed |
| credit | 31, stuck | 1 of the guild's batches | 15 services; 14 debts entered, 1 repaid, 13 outstanding; the cooperative withheld from the guild 40 times at the limit, the guild from the commons 12 |
| transferable credit | 24 | none | 17 services; 12 debts entered, 9 repaid, 4 claims on a third community passed on in payment, 3 outstanding |
| cleared credit | 24 | none | 17 services; 16 debts entered, 12 cleared around cycles on the league's record, 1 repaid, 3 outstanding; no claim passed on |
| provision | 24 | none | 17 services; nothing owed |

Under bilateral credit the circle stalls, since no community can repay the one that serves it. A claim that changes hands (money-like) unblocks it. So does clearing the cycle on a record every community can read, where nothing changes hands. Figures: `figures/triangle-entries-settled.svg`, `figures/triangle-debts-outstanding.svg`.

### 3.2 Envoys Deciding

`three-envoys-from-credit/`, `three-envoys-from-gift/`: `run_case.py three --size 60 --envoys --models --models-for envoys --regime <credit | gift>`. Each community sends an envoy by Claude to the league's council, shown the services between communities, the debts, the services withheld, the work left and the open proposals; it may propose another arrangement, vote, or speak. The checks are the stand-in's, so only the council's deliberation is paid for ($0.17 and $0.12).

**From credit**, the league changed its arrangement three times by its own rule (majority counted by community), each change a compact ratified and enacted in its record:

1. **Round 9.** The guild's envoy proposed cleared credit: "No services have passed in nine rounds under credit… Cleared credit keeps reciprocity but dissolves debt cycles so no one stalls at the limit." The commons' envoy voted against ("Credit is working… Clearing adds machinery we don't need yet"); the cooperative voted for; it passed 2 to 1 and was enacted in round 11.
2. **Round 13.** Services were still withheld, and the commons' envoy proposed transferable credit: "Debts form a chain: guild owes cooperative, cooperative owes commons… Letting claims pass on would clear this chain and free withheld work without anyone giving up what they are owed." It was adopted unanimously and enacted in round 15. Three claims were then passed on in payment.
3. **Round 17.** The commons' envoy proposed provision: "Cooperative has withheld service to guild ten times at the debt limit while work sits undone. Provision would serve open work by need from the record, so the catalogue gets checked instead of stalling on ledgers." It was adopted unanimously and enacted in round 19, and the debts were released.

All work was done in 24 rounds. At the end each envoy chose to rest, the cooperative's saying "changing the league's arrangement would only bring back the limits that once blocked ten of our services to the guild". One debt stayed in the record, its repayment delivered and never accepted, because the clerks then released only open debts under provision. That fault of the work has since been mended.

**From gift**, no credit appeared. The envoys saw a balanced cycle of service and kept gift: "changing it now would add bookkeeping without any gain".

Figures: `figures/three-envoys-from-credit.svg` (the arrangement in force as bands, the debts outstanding, claims passed on and cycles cleared), `figures/three-envoys-from-gift.svg`.

## 4. What These Runs Do Not Show

- **The stand-in's checks.** The triangle's checks are the stand-in's; only the two-community case and the envoys used Claude.
- **One run each.** Every result is a single run, with no repetitions and no variation of seed, limit or size.
- **No free riders, no verification.** No community declares a capacity it does not deliver, so gift and provision are not tested against free riding, and the verification of declared capacity (the jar's epistemic resource) is not exercised. That is the next case.
- **One provider.** All model agents are Anthropic's, so judgements counted by provider (the domain's `capacity`) cannot differ from judgements counted by agent.
- **No crossings with receipts.** Findings carried from the guild record the guild's head, but crossings with receipts and proofs of inclusion are not run, since the API does not expose them yet.
- **No persons.** The persons of these communities act only through the rules their operatives follow; no person adjudicated or voted from the page.

## 5. Spending

$0.85 in all with Claude so far (147 paid calls), within a budget of $50 stopped at $45: one probe call, the two-community case twice (its first run discarded), and the two envoy runs. The ledger's 131 other entries cost nothing: calls that failed before reaching the provider while the kept key could not be read, and the stand-in's answers in the envoy runs. Each call was reserved before it was made and counted from the tokens reported, in the ledger kept in the person's settings folder.
