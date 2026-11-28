# Phase 9 Progress (The API), Second Attempt

Started 2026-11-27 from `notes.md` and `append_sessions.py`.

## Plan

1. The domain's `trajectories` gains value readings (`read_values`, `episode_of`): share of decisions, practised norm, conduct by category; H7 marks.
2. `api/cpp/science/communities.hpp`: sessions (open, step, view, close), views, profile fields of Phase 8, `communities.registry`,
   `communities.join.verify`, `communities.exit.verify`, `communities.power`, `communities.reading`; the behaviour journal a series per
   community (one per session).
3. `api/python/science/communities.py`: Host, Session, provenance, exit_package, reading, verify_reading, power, registry helpers.
4. Checks in `api/tests/check_communities.py`; Koinonia's `Society.submit` on sessions.
5. Deferred: bindings and custom (science.law), voting power through science.multilateral, organisations through the API.

## Done

- Domain: `trajectories` gains `episode_of`, `ValueReading`, `value_mechanisms`, `read_values` (share of decisions, practised norm, conduct
  by category; H7); CMake adds `thesmos::statements`; 4 checks in `check_trajectories.cpp` (section 5). Not built: readings of open terms.
- API C++ written (sessions, views, registry, join, exit.verify, power, reading); profile and state gain order, terms, seats, listed in,
  envoys; `work_json` factored out of `state_json`.
- Python: `sessions_block.py` (scratchpad) appended to `communities.py`; docstring and imports.
- Checks appended to `api/tests/check_communities.py` (about 20).
- Koinonia: `Society.session`, `submit` on `Session.step(state=True, brief=...)`, `directory` and agents' briefings through session views.
- First engine build (everything in one engine): 795.5 s (335 s before Phase 9). API checks 62 of 62 passed (33 s); Koinonia 78 of 78
  (78 s); the communities domain 134 of 134.
- Split: power and value readings moved to their own engine (`api/cpp/science/community_readings.hpp`,
  `api/cpp/engines/community-readings/main.cpp`, Python `READINGS`, `_read`), the session engine including only the domain's new light
  header `communities/trajectories/episodes.hpp` (episode_of moved there). Session engine 607.5 s, readings engine 586.2 s.
- After the split: API checks 62 of 62 (34 s, 20 new); Koinonia 78 of 78 (46 s).
- The whole run (2026-11-28): 21,219 of 21,219 checks in 85 suites, 17 groups skipped for missing data; thesmos 345, communities 134,
  the API 862 (1 skipped), Koinonia 78. Phase 9 done; nothing committed.
