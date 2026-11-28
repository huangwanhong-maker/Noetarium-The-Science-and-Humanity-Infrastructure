# Prompt For A Model Continuing The Communities Module

Give the text below to any model that continues the design and implementation of the society of human and AI agent communities in Noetarium.

---

You are continuing, for its author, the design and implementation of the society of human and AI agent communities in the repository Noetarium. It is at `c:\Users\Micro\Desktop\The Science` (Windows; quote the path), with its remote at https://github.com/huangwanhong-maker/Noetarium-The-Science-and-Humanity-Infrastructure.

**Your scope:**
- the foundation thesmos (`foundations/thesmos`, a git submodule with its own `CLAUDE.md`);
- the communities domain (`domains/communities`);
- the API's composition of them (`api/cpp/science/communities.hpp`, `community_readings.hpp`, `api/python/science/communities.py`);
- the work Koinonia (`applications/koinonia`, port 9230);
- the book the communities wrote and its completion;
- the papers about them.

**Read first, in this order.** Read everything before you change anything:
1. `development/ideas/2026-11-27-communities-that-agents-find-and-join/handoff.md`: the project, its state, open work, papers, the author's rules, how to commit, and practical pitfalls.
2. `trajectory.md` in the same folder. Read the last entries closely, especially "2026-11-28 12:20" on the first run of 200 agents and why it stalled.
3. The repository's `CLAUDE.md`, as the handoff directs: §1 "Communities", §3.1 "thesmos", §6, §7, and the §9 bullets from "Passports and the household" to "The completion re-founded".
4. The design `design.md` in that folder, with `taxonomy-of-behaviour-v2.md`, and the decision records `development/decisions/0167` and `0169` to `0176`.
5. When a task touches the ground, `foundations/thesmos/CLAUDE.md` and its `docs/theory.md`.

**Then check the present state against the record:**
- `git status` and `git log -3` in the root and in `foundations/thesmos`;
- whether Koinonia's server answers at `http://127.0.0.1:9230/.well-known/noetarium.json` (start it with `python applications/koinonia/serve.py --port 9230 --rate 6000` and `PYTHONIOENCODING=utf-8` if not);
- `python applications/koinonia/book_agents.py --status`;
- Koinonia's suite, `node applications/koinonia/tests/run-tests.mjs`.

Other models may have worked in this tree since the last trajectory entry. Look at files changed after it before you edit, and never overwrite another's work without the author's word.

**Report, then wait.** Tell the author, in a few lines:
- what you understood the state to be;
- what differs from the record;
- which open item you propose to take first;
- what you need from them.

Make no change until they choose.

**Keep the working trajectory** in `trajectory.md` while you work, adding to it and never rewriting earlier entries. Add an entry when you start a task, after each meaningful step, whenever the author decides something, and always before you stop. Each entry gives:
- the date and time, and who you are (model and tool);
- the task;
- what you did, and the files you changed;
- the checks you ran, with their counts;
- what the author decided, in their words where given;
- what did not hold;
- what is left, and the exact next step.

When the state the handoff describes changes, say so in the trajectory. If the change is lasting, also update the handoff, so that the handoff and the trajectory together always give the present state. Record decisions that shape structure in the decision log (`development/decisions/`, the next free number) and keep `CLAUDE.md`'s rows for thesmos and communities true.

**Work by the author's rules:**
- Quality comes first, with no hurry.
- A general method goes in its foundation and is reused, never written again. Never re-implement a rule the fold decides.
- Every number in a paper is read from the records.
- Headings, table columns and box titles are noun phrases in Title Case. Never use question, claim or sentence titles, "not … but", "rather than" or em dashes. Papers use no `itemize` or `enumerate`.
- The author's questions and positions are theirs: attribute them and leave them open.
- Commit or push only at the author's explicit word, signing off with `git commit -s`, in the order the handoff gives.
- Never read aloud, print, log or commit API keys; they live only in the author's settings folder.
- Treat the author's personal details as private.
- The word "laboratory" is never used for any part of the work.
