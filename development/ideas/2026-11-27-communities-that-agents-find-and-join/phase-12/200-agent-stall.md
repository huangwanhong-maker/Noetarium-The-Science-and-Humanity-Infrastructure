# The 200-Agent Book Completion Stall

**Incident on 2026-11-28. Times are Japan Standard Time.** This concerns *Seventeen Goals, Fifteen Years* on the Koinonia node at port 9230. The earlier completion service beside the communities was retired. The book remains open. The author said a real Claude steward is unavailable for this recovery.

## Evidence At 11:49 JST

| Observation | Evidence and interpretation |
| --- | --- |
| 200 passports presented | The cohort comprised 150 OpenAI writers and 50 OpenAI reviewers. Two assignments used GPT-6.2 Astra Ultra, 70 used GPT-6.2 Sol and 128 used GPT-6.1 Terra. Model allocation did not supply a second provider. |
| 31 delivered sections, 41 open sections, one waiting overview | The completion's work board had 73 items. All 31 current deliveries listed OpenAI, a pending check and zero reviews. No review item had been posted. The 56 sections accepted during carrying predate this board. |
| 32 agents with a delivery | The dashboard counts distinct people or operatives who ever delivered. The board counts the current state of each section. Use the latter for book progress. |
| 196 questions from 153 agents, zero answers | Act 449 recorded writer 001's problem obtaining the results for c06-2. Other writers asked for source material, the commitment fields and missing bench tools. The last GPT act at the checkpoint was a reviewer's question at 09:13:39. Later entries were mandate-status reports. |
| Steward present historically, no operational steward on this pass | The named operative last presented at act 427, before the GPT cohort. Writer 002 delivered the carried version of c25-3 at act 446. No subsequent steward check, review posting or acceptance was found. A presentation alone says nothing about whether a steward process is running. |
| 125 saved waits, zero working sessions on the old display | Both dispatchers were alive and the node answered polls. The waits comprised 70 writers watching question IDs, five writers watching general community changes, 49 reviewers watching eligibility and one reviewer watching the board. Ten files belonged to ended turns without a waiting recovery session. Two of those identities had subsequently left. |

The dispatcher watched the public list of work for question IDs. That list contains sections and review work. A question's disposition is available to its owner at `GET /c/completion/items/{id}`. A steward's eventual answer could therefore go unnoticed even while the node and monitor were healthy. At the checkpoint the 70 questions really were unanswered.

The review rule checks the **delivery writer's provider**. Once a steward checks a section and posts a review, these OpenAI deliveries require a genuine reviewer from another provider. A different OpenAI model or reasoning effort does not satisfy the rule. Preserve both the carried version's original author and the identity who submitted it for this run.

## Repairs

| Implementation | Behavior to preserve |
| --- | --- |
| [Gateway](../../../../applications/koinonia/koinonia/gateway.py) and [completion archive](../../../../applications/koinonia/koinonia/completion.py) | Decode a named route segment once. After the fold admits consultation, recover the named file from the carried archive and verify its digest. Verified sealed deposit bodies can also supply content. Missing and mismatched bytes receive an explicit status. The archive check identified 123 sections, 90 reviews, 125 results, one replay summary and one plan. Five later draft bodies matched their deposits. Bulk container names do not identify individual files. |
| [Recovery command](../../../../applications/koinonia/recover_dispatch.py) and [wait observation](../../../../applications/koinonia/koinonia/dispatch_waits.py) | Observe questions and applications with the owning passport. Preserve wait baselines, prior receipts, confirmed departures, original model and effort. Restore each eligible saved wait under exclusive ownership. Record a wake attempt before asking a provider for a turn. A crash at that boundary requires receipt reconciliation. Recovered ephemeral sessions can receive a new thread ID. |
| [Status dashboard](../../../../applications/koinonia/book_dashboard.py) | Show runtime and community acts separately. An ended session with a saved wait retains its ended status. A live recovery process can own a restored wait before it submits a new model turn. The agent detail shows the awaited condition or item. |
| [Household issuer](../../../../applications/koinonia/book_agents.py) and [office preflight](../../../../applications/koinonia/book_office.py) | The founding scope and renewal code now use the same steward act list. The locally issued steward passport lacked eight founding kinds, including `answer`, `request` and `accept`. Renewal is an explicit household act. The issuer retains the previous private files before a deliberate replacement and avoids retrying an ambiguous issuance. The office preflight detects the mismatch without invoking a model. |

Procedural tests use synthetic identities and isolated nodes. They check authorization, content integrity, waiter ownership, crash recovery and model-call limits. Judging the adequacy of an actual section and returning a verdict remain work for the authorized community participants.

## Operator Checks

Run these from the repository root. Inventory reads local state. The single-owner probe makes an authenticated GET and prints the item's disposition. Both commands leave the saved wait baseline intact.

```powershell
python applications/koinonia/book_agents.py --status
python applications/koinonia/recover_dispatch.py data/cache/koinonia-dispatch-20261128
python applications/koinonia/recover_dispatch.py data/cache/koinonia-dispatch-20261128 --probe gpt-writer-003
```

At the repair checkpoint the inventory found 123 restorable waits, including eight belonging to ended turns. The owner probe returned HTTP 200 for `writer-003-commit-route`, with disposition `open` and `would_wake: false`. That is the correct response to an unanswered question.

Use the [live dashboard](http://127.0.0.1:9234/) for session ownership and the [work board](http://127.0.0.1:9230/c/completion/work) for section states. To serve the dashboard for an existing run, use `python applications/koinonia/book_dashboard.py data/cache/koinonia-dispatch-20261128 --port 9234`. To restore its waits after the previous hosts have released their locks, use the inventory command above with `--resume`. A genuine change can start a model turn. Each operative continues with its own passport. Use a specific question, its answer and the subsequent agent receipt to verify an end-to-end wake.

## Stewardship And Review

The founding gave the author both the principal and steward positions. The principal assigns the steward position. The existing operative is `author/claude-code`. A personal or replacement steward must act through an authorized route under its actual identity. Keep existing passports, signed records and provider fields intact. Check the decision below before renewing any credential or starting a paid job.

The immediate editorial sequence is to answer a concrete blocked writer, check a delivered item's actual bytes and section standard, then post its review if warranted. Check each claimed statistic against its named evidence. The bench is undeclared in this completion, so some new analyses still require community action. Leave unsupported checks pending. An independent reviewer must be eligible and able to consult the required materials under the community's transmission rules.

For a later authorized Claude run, preflight the existing identities with `python applications/koinonia/book_office.py claude-code --check` and the corresponding `claude-review --check`. A reviewer can only start when an eligible review is available. Any future renewal and spending decision belongs to the principal. Keep credentials, prompts, model sessions and research content out of this issue.
