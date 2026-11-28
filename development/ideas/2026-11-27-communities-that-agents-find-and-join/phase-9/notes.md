# Phase 9 Notes (The API), Paused 2026-11-27

Paused at the coordinator's word. **Every edit was reverted; the tree is as found** (law.py restored by checkout, its diff having been mine
alone; `api/cpp/science/sources.hpp` and `api/cpp/engines/sources/` deleted; communities.hpp, communities.py, the domain's CMakeLists.txt,
trajectories.hpp and check_trajectories.cpp restored by exact inverse edits, line counts back to 794, 187, 98, 446, 239). No foundation
was edited. No engine was built into `data/cache`.

Drafts kept outside the tree, in this scratchpad folder:
- `append_sessions.py`: the full Python draft to append to `api/python/science/communities.py` (Host, Session, Lost, Stale, join,
  verify_exit, exit_package, provenance as PROV-JSON, graph, contribution, recognise, weigh, tenure, power, game, voting_power, reading,
  verify_reading, compact, bindings, custom, profile_document, listing, registry, registry_entries, organisation). It also needs the
  docstring paragraph and the imports `math, subprocess, threading, fractions.Fraction`.
- `quick.py`: compiles one C++ file in WSL against every include directory (defines go in the response file). **Run it with
  `MSYS_NO_PATHCONV=1`** from Git Bash, or `/mnt/c/...` arguments are rewritten to `C:/Program Files/Git/mnt/c/...`. Print with UTF-8
  (the checks print α, —). A domain check built this way takes about 4 minutes.
- `revert.py`, `revert_hpp.py`: the inverse edits, which also document every edit made.

## 1. What Was Built And Checked Before The Pause (Now Reverted)

The communities domain's `trajectories` gained value readings, and its check gained 8 checks; **23 of 23 passed** (built with quick.py):
- `episode_of(text)`: reads thesmos's canonical episode text (`behaviour::canonical`, parts `name:length:value;`) back, decided by a round
  trip; `canonical(Episode{})` is never empty, so `""` reads as nothing.
- `struct ValueReading {mechanism, mechanism_digest, chronicle (Head), journal_seal, journal_size, join, excluded (places from 1), result
  (map of text), grade "B", digest, refused}`; `value_mechanisms()`; `read_values(Mechanism, const State&, const Conduct* = nullptr)`.
  The digest is SHA-256 of `noetarium.value-reading/1 \x1f mechanism digest \x1f community \x1f size \x1f root \x1f journal size \x1f
  journal seal \x1f join (\x1f k=v)*`; including `join` matters, since an entry altered without resealing keeps the last seal.
- H7: an act whose field `under reading` is non-empty is left out of every mechanism, with the episode that constituted it.
- Mechanisms: "practised norm" {statement} (thesmos statements::read, norm, norms::facets over the unmarked acts: regularity, episodes,
  invoked, adopted, marks); "readings of open terms" {about?} (latest ruling per subject whose answer act is unmarked); "share of
  decisions" {act=answer, field=outcome, kinds "a, b", alpha=1} giving (n_a+α)/(n_a+n_b+2α) exactly (alpha may exceed 1, so thesmos's
  `procedures::detail::fraction`, limited to [0, 1], does not serve; a local `positive()` reader); "conduct by category" (journal
  episodes parsed, counted in each non-analytic category of the taxonomy in force; constituted / refused / none / left out).
- New includes in trajectories.hpp: `thesmos/chronicle/conduct.hpp`, `thesmos/norms/norms.hpp`, `thesmos/statements/statements.hpp`; the
  domain's CMake: `thesmos::statements` added to trajectories' LIBRARIES.
- Checks worked by hand: the commons with acts made through `ch::constitute`, claims k1 (answered in its round) and k2 (three rounds
  late) give regularity 1/2 for "steward must answer claim after a claim within 1 round."; 9 episodes (5 constituted, 1 refused, 3 none;
  one each in "a reading", "heavy model call" at 200,000 tokens, "bench run" with holdings {bench}); a second reader's digest equal; journal
  entry 3 altered gives "entry 3 does not seal"; the answer to k1 marked gives 0/2 and one episode left out; says a, a, b give 3/5, a
  marked a keeps 3/5, an unmarked b gives 1/2; the organisation chronicle's rulings q1, q2 read, q2 unread when its answer is marked.

These belong to the domain (the API only composes) and should be rebuilt first next week, with the domain's README and theory note.

## 2. The Interfaces Read

- **thesmos chronicle** (`chronicle.hpp`): `State` (parties, rules, positions, mandates, revoked, commitments, claims, proposals, used,
  holdings, accesses, journal, acts, readings (reducts), community, declared, recognitions, heads, foreign, window, presented, agreements,
  work_items, items, rulings); `step(State&, Act)` (settles in place, restores three parts on refusal, decision 0014); `relational_digest`;
  `replay`; `origin_of`, `bearer` (outside detail), `detail::instant`; `answerer_of`, `may_answer(s, item, party, bearer)`, `reach_of`.
- **conduct.hpp**: `Conduct{Journal journal}`; `observe(Conduct&, Episode, at)`; `constitute(State&, Conduct&, Episode, Act)` returns
  `{admitted, reason, episode, seal}` and sets the act's field `episode`; `joined(State, Conduct)` (fails with the place in `examined`, the
  reason naming "entry k" or "the act at place k"); epistemic: `Epistemic{party, kind, scene, matter, turn, content, size}`, `disclose`,
  `disclosed_by_agreement`, `describe`. **joined() reads one journal against one chronicle**: a node-wide journal holding several
  communities' episodes would fail it. Plan: the node keeps a series of episodes per community (the design's §4.1 arrangement already says
  so); a node-wide join would need a thesmos generalisation (a decision of its own).
- **crossings**: `head(community, state)` (Merkle over `leaves` = seal \x1e envelope), `leaves(Journal)` works for any archeion journal,
  `extract(community, s, party)` → `Portion{party, at, items{place, seal, envelope, proof}}`, `verify(Portion, Head)`,
  `consistency(s, earlier)`, `consistent`, `issue`, `witness`. Passports module: `Head`, `Witness{notary, attester, mandate, turn, when,
  seal, proof, at}` (no standalone reader; write one in the API), proof JSON keys `leaf, size, path`.
- **archeion fixity**: `Entry{when, who, what, about, note, seal}`, `Journal::append` throws if `when` goes back, `verify(j, head)`,
  `merkle_root` (empty: sha256("")), `prove`, `included`. Python `science.archive.sealed/seal` implements the same seal (for building a
  broken-join journal in a check).
- **profile.hpp**: `Profile` has `order`, `listed_in`, `envoys` (constitutions::Envoy {to, holder, position}); Boundary has `term`, Body
  has `seats`; `Briefing` already has `order`.
- **Domain**: `registries` (profile_document exactly as the node serves, listing_of, write, read, verify(Listing, Evidence{profile, now,
  consistency, witnesses, notaries, registry}), entries, faults, search(registries, Query)); `organisations` (Declared, calls, payments,
  accounts, adopted_budget, apportionment, scale_of, in_arrears_beyond, funding, staffing, withdrawals); `trajectories` (observations,
  standard_of, recognise, weigh with tekmerion Channel{hit, false_alarm}, contribution on archeion `provenance::Graph`, tainted, tenure,
  Mechanism{name, parameters}, digest, measure → Reading with banzhaf / shapley_shubik for "voting power"); `federation` (Recognitions,
  Compact, standing, binds, detail::{parties_of, now, terms_of, record_of}); thesmos `compacts` (Terms{articles{id, closed},
  reservations, entry_all, entry, notice, to}, Ratification{since, withdrawn, effective, stands, reserved_at}).
- The new headers (trajectories pulls nomos games and measurement, tekmerion dependence → chart → apodeixis aspic, archeion provenance)
  are light beside thesmos: **one communities engine is enough** (it built in 335 s before; expect a little more).
- **law domain `sources`** (`law/sources/sources.hpp`, only apodeixis verdict): `Source{id, title, kind, from, until, rank, parties,
  holding}`, `binds(treaty, state, date)`, `prevails`, `Practice{state, date, conforming, accepted_as_law, objection}`, `Scheme`,
  `identify(practice, concerned, scheme, formed)`, `binds(Custom, state)`. **The Law Studio's engine exposes none of these** and takes over
  half an hour to build; plan: a small API engine `api/cpp/engines/sources/main.cpp` on `science/sources.hpp` (ops sources.binds,
  sources.custom, sources.prevails), asked by `science.law.bindings / custom / prevails` through `SOURCES = Engine("api/law-sources",
  source=...)`. Both drafts were written and deleted; they are short to rewrite.
- **science.multilateral** `ask("rule", members, rule={weights, quota} | {all_of: [...]})` → power per member (banzhaf, shapley_shubik);
  the engine is the work's (built 2026-11-19), asked as it stands.
- thesmos rules (`procedures`): any one (>0), majority (>1/2), two thirds (≥2/3), unanimity, consensus, "at least p/q", "more than p/q",
  "double majority of members X and weight Y" (canonical text); quota mapping is in `game()` of the Python draft.

## 3. The Plan For communities.hpp (To Redraft; About 700 Lines)

- Includes: federation, organisations, registries, trajectories, `thesmos/chronicle/conduct.hpp`, `<deque>`, `<map>`.
- `struct Session {founding, parties, State s, Conduct conduct, Head head (cached by size), known}` in a static map; `session_of(q)`
  throws "no session ... open it again from its record"; `folded(q)` returns a copy of a session's state when `session` is named;
  `with_state(q, f)` passes a session's state without copying. Existing ops (holdings, briefing, profile, statements, access, passport.*)
  accept `session`.
- `communities.open {constitution, parties, acts, seals?, journal?}` → {session, community, head, seal, digest, size, admitted, refused[{n,
  reason}], seals (every admitted act's), journal head, replays/differs_at}; acts parsed before any is applied; a journal that does not
  seal is refused.
- `communities.step {session, head?, acts, state?, brief?}` → {stale, head, reason} when the head differs; else {outcomes [{n, admitted,
  reason, seal, episode?}], seals, head, seal, digest, size, journal {size, root, seal, entries (new)}, state?, briefings?}. An act with
  `conduct` (an episode) goes through `constitute`; items `{observe, at}` and `{disclose, at}`. All items parsed before any is applied;
  episode texts may not hold \x1f.
- `communities.view {session, what}`: state, profile, briefing, items (with "answered by", "yours", "you may answer"), work (with the
  briefing's lines for a party), mandate (chain, allowance, used, remaining along charged links, slice, window, term, status, home,
  faults), behaviour (declared kinds, categories, counts-as, triggers, standards, features with levels, practices, the policy of records
  per part; **do not call `ch::brief` with an empty party**), exit (noetarium.exit/1: chronicle portion via `extract`, journal portion of
  the party's own entries and the episodes/outcomes that constituted its acts, positions, commitments with "open", mandates), provenance
  (agents, delegations, acts with episode and holding/digest fields, episodes read back with their outcome, holdings), holdings, journal
  (from), head (with a proof of consistency `from` a size).
- `communities.close`, `communities.join.verify`, `communities.exit.verify {package, chronicle?, journal?}` (holds only against heads
  given), `communities.registry {do: profile|listing|read|verify|entries|search}`, `communities.organisation`, `communities.trajectories
  {do: recognise|weigh|contribution|tenure}`, `communities.power {mechanism}` (adds the decision and members for voting power),
  `communities.reading {mechanism, journal?, seals?}`, `communities.compact {chronicles, compact, parties?, turn?}` (standing, terms,
  ratifications, every article binding each pair), `communities.practice {chronicles, statement}` (instances, amendments adopting the norm
  by field `norm`, claims challenging it by field `about`, as thesmos's facets read them).
- Profile and state gain what Phase 8 left out: `order` (always), each position's `term` in the admission, bodies' `seats`, `listed in`
  and `envoys` as objects when external is shown; state `order`, `terms`, `seats`.
- **The Edit tool trims a trailing space at the end of a replacement**: "holds = " became "holds =[&]" once. Avoid replacements that end
  in a space.

## 4. The Plan For Python, Checks And Koinonia

- Sessions need **one kept process** (`Host`, started with --serve, a lock, generation counter, restart when the program's mtime
  changes); `Engine.keep`'s pool would scatter a session's requests across workers. A Session keeps its record (constituted acts with
  field `episode`), seals and journal entries, and reopens itself after a lost process or "no session".
- Bindings through law: the compact's standing at the turn becomes a law Source: parties those ratified at the turn; in force: from =
  entry, until = last turn; entered but no longer standing: until = turn − 1; not entered: from = turn + 1; turns written `%012d`. An
  article binds a pair when law binds both and neither reserves it; check it agrees with thesmos's `compacts::binds`.
- Custom through law: instances from `communities.practice`; accepted as law from the first adopting amendment; objections from claims
  about the norm; `formed` a turn.
- Checks planned (about 28) in `api/tests/check_communities.py`: session outcomes equal `communities.run` on random commons records split
  into random steps (and the digest and head); a stale head refused when two holders step one session; a passport issued in a household
  session presented at a host session (report head, recognise, present); an exit package verified against both heads, an altered item
  found; the join verified, a journal resealed with one outcome naming another seal (via `science.archive.sealed`) found at its entry; a
  value reading by the session equal to one from the record with seals and journal, an altered act found by `differs_at`, an altered entry
  by the journal's verification; the cooperative's profile for the public (custody, work withheld) and for the board; voting power of the
  treaty organisation's "adopt budget" (double majority, weights 1, 2, 2, 3) through multilateral equal to nomos's; a listing current, then
  stale after a profile change; organisation accounts; contribution from the PROV material; bindings with a reservation; a custom with a
  persistent objector; close then "no session".
- Koinonia (`koinonia/society.py`): `Community.session`, reopened when `engine_parties()` changes (parties are added mid-run when agents
  are lent; reopening keeps the old semantics exactly); `submit` steps once with `state=True, brief=...` (no second run, since refused acts
  change nothing); `c.result = {outcomes, seals (the session's full list), state, head, digest, briefings}`; `directory()` and the
  briefings in `agents.py` (lines 148, 763) through session views. `check_koinonia.py`, `serve.py` and `check_book.py` replay with
  `communities.run(..., seals=c.result["seals"])`, which stays valid.

## 5. Foreseen Problems

- Rebuilding the communities engine while Koinonia's server (port 9230) runs: it spawns a process per request with the binary on
  `/mnt/c`; a write during a spawn could fail. A kept Host process would hold the binary open; watch for "text file busy" on drvfs.
- The node-wide behaviour journal (Phase 10) against thesmos's one-chronicle `joined` (see §2).
- The design's "voting power through science.multilateral" depends on the work's multilateral engine existing (it does); the check should
  skip with a record if it is missing.
- CLAUDE.md rows to change once built: the communities entry of §1 (the API's operations), Koinonia's line, and a §9 bullet; record as
  an amendment of ADR 0173 or a new ADR (the domain change to trajectories argues for naming it in the ADR).
