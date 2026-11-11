# 0091. An Agent on the Board

- **Status:** Accepted, provisionally (2026-11-11).
- **Date:** 2026-11-11
- **Decided by:**
  - **The request.** The user: "Implement an AI Agent plug-in that allows utilizing LLM agents to operate the board from natural language. It can automatically create or draw something, even utilize existing plug-ins that it is allowed to use (the AI Agent plug-in should have a setting panel if loaded, then one can choose which board plug-ins it can use, just like how an LLM company does at its website frontend). Of course, it must allow users in the AI Agent plug-in's settings to set up the API keys. I think currently supporting you only is OK, but remain interfaces for other LLMs."
  - **The design.** Claude, on [ADR 0090](0090-a-board-for-thought.md)'s board and plug-ins.

## 1. Context

The board (ADR 0090) keeps what is thought apart from how it lies, and every change to it is an operation with its inverse. A subject comes to it as a plug-in of templates, cards and modules, each of which declares what it needs.

Those two properties are what an agent needs of a ground it works on. Its acts can be the board's own operations, so that they are seen, checked and undone like a person's; and what it may draw on can be read from the plug-ins' own declarations, so that no plug-in is written for it.

The user's turn of ADR 0090 stands: tools are tools, and the judgement is the person's. An agent on the board is a faster hand, and its work is a draft.

## 2. Decision

### 2.1 The Agent Is a Plug-in, and Brings a Panel

`applications/tabula/plugins/ai-agent/`. It brings no template and no card: it brings a **panel**, a sheet at the side of the board opened by a key in the bar. Two seams were added to Tabula for it, both general:

- **Panels.** A plug-in may answer `panels: [{id, label, title, mount(host, ctx)}]`. The page gives each a key in the bar and a sheet at the right; what is selected moves aside for it. `ctx` is `{api, B, packs, modules, say, request, close}`.
- **A plug-in's server side.** `plugins/<name>/server.py` with `handle(method, path, body, context)`, asked at `/api/plugin/<name>/<path>` through `ctx.request`. It is asked under the rule of a module's run (ADR 0079, 0090): by this work's own page, at this computer's own address. A request to read carries the page's header, which a page elsewhere cannot send without being asked about.

### 2.2 Its Acts Are the Board's Operations

`tools.js`, pure. The model is given fourteen tools: `read_board`, `add_items` (notes of any kind, text, regions, shapes, figures, plans and chronicles), `connect`, `update_items`, `remove_items`, `draw` (strokes of the pen through points), `add_table`, `read_table`, `arrange`, `layers`, `show`, `lay_template`, `add_card`, `run_module`.

- **Each call is one step of the board's history,** made of the board's operations and checked against the board before it is given to it. It is undone like any other step.
- **A call that cannot be carried out changes nothing and says why,** in words the model can act on: the kinds of note there are, the line of a plan that is not read, the name a figure uses that no note defines.
- **Items may be given without places.** They are laid in a free part of the board, or within the region named, which grows to hold them; a place is given when the arrangement itself says something.
- **No tool writes code.** A module card with a script of its own is refused: the agent runs only the modules that are installed.

### 2.3 What It May Use Is the Person's to Say

The settings list the plug-ins, each with what it brings (templates, cards, modules, read from their declarations). Those ticked are the agent's to draw on; running modules is a tick of its own. All ticked is kept as "all", so that a plug-in installed later is allowed too.

- A plug-in that is not allowed is not told to the model, and a call that names it is refused by the executor, which reads the settings when the call comes.
- The board's own tools are always the agent's.

### 2.4 A Conversation Is Only Added To

`agent.js`, pure: a loop held in the page. The person's words go to the model with the tools; while the model asks for tools, each call is carried out and answered; when it stops asking, its words are the answer.

The provider keeps a model's reasoning only while everything before it stands as it was sent: the system prompt, the tools, the earlier messages. An account of the present kind is refused a request that replays reasoning after such a change. So:

- **The tools are the same whatever is allowed,** and name no plug-in.
- **The system prompt is that of the conversation's beginning.**
- **A change of the settings is told in a line beside the person's next words** (what is no longer the agent's by name; what is newly so, as it would have been told at the beginning), while the executor follows it at once.
- **Messages are taken only off the end:** a turn that the provider declined, or whose first step failed, is taken back whole.
- **The same form keeps the provider's cache warm.**
- As a net beneath this, the request asks that reasoning whose past has changed be dropped and the step go on, where it would otherwise be refused and the conversation could not continue. The server says so where it is watched.

The conversation stays able to go on after a call that fails, a stop, an answer cut short, a refusal and a provider that cannot be reached. It is about one board: on another, it begins again.

### 2.5 Who Answers, and the Key

`server.py`. The page holds the conversation and asks the server for one step at a time.

- **A provider** is a class with its models, `unavailable()` and `complete(key, request)`. Claude is the one written, through the Anthropic SDK: Opus 5.5 by default, with Sonnet 5.5, Haiku 5.5 and Fable 5.1; adaptive thinking; the effort the person chooses; a request declined for safety answered by the model the API recommends, where that is offered.
- **The form of a conversation is the Messages API's,** the richest of the forms in use. A provider of another form translates to and from it; the page and the tools do not change.
- **The key is kept in the person's own settings folder** on this computer (`%APPDATA%\noetarium\tabula`, or `~/.config/noetarium/tabula`; `--settings` names another), in a file readable by them alone where the system keeps such a thing. It is never under `data/cache/`: every work's server mounts that folder at `/data/`, and a public instance would serve it.
- **The page is never given the key back:** it is told that one is kept and how it ends. With none kept, the server's own environment is tried.

### 2.6 What Was Found

Driving it with Claude Opus 5.5:

- **Asked to size a market from an empty board,** it laid five assumptions with their ranges in a region, a figure computing buyers, market and sales from them (236 k a year, 9,900 to 3 M), ran the module of Melbourne's cafés and restaurants (1,677 places) and drew them on a map, and said that every number was its own placeholder to replace. About 50,000 tokens read, most from the cache, and 1,800 written.
- **A plan's lines are a small language, and the first plan it wrote was not in it:** an entry's name held commas. The executor now refuses a plan that is not read and names the line; the second plan was read.
- **Changing the settings in the middle of a conversation was refused by the provider,** the reasoning of earlier turns being bound to the system prompt they were made under. §2.4 is what was learnt.
- **A plug-in's settings must not live in the data cache.** The first design kept the key under `data/cache/tabula/`, which this server and every other work's serve.

### 2.7 Checks

- **Tabula's suite: 108,** of which 65 are new, none asking a model: the tools on a board without a page, the conversation with a scripted model, the server side with a provider that asks no one, and the server's refusals.
- **The page was driven for real,** 32 assertions in two drives, with a key and the network: not part of the suite.

## 3. Alternatives Considered

- **The agent as a part of the board.** Rejected: the board is of no subject and asks nothing of the network. An agent is one way among others of working on it.
- **The loop on the server.** Rejected: the board lives in the page, and its operations, its history and its cards' runs are there. The server is asked for what only it can do: hold the key and ask the provider.
- **The key in the page's storage.** Rejected: any script of the page could read it, and a board's plug-ins are scripts.
- **A tool that runs a script the model writes.** Rejected. A module runs with the authority of whoever started the server; what the agent runs is what is installed.
- **Tools made from what is allowed** (a tool present only where there is something for it, its choices listed in its schema). Built first, and withdrawn: it rewrites what a conversation was told whenever the settings change (§2.4).
- **A managed agent of the provider's.** Not taken: the tools act on a board in the person's page, and the loop is short.
- **The model's words as they are written** (streaming). Deferred: a step is shown when it is whole, and each call as it is carried out.

## 4. Consequences

- **Any plug-in is the agent's to use once it is ticked,** with nothing written for the agent: its templates, cards and modules are told to the model from their declarations.
- **A plug-in of Tabula can now bring a panel and a server side.**
- **Debts, stated:**
  - **One provider is written.** The seam for others is there and unused.
  - **The agent reads a board's words, not its pictures:** an image, a stroke and a chart are named to it, not shown.
  - **What it writes is a draft.** It is told to mark its own numbers as assumptions and to give facts their sources; nothing checks that it did.
  - **A whole turn is not undone by one key:** the board's undo takes its steps one by one.
  - **The conversation is not kept** beyond the visit, and several people on one board are not thought of.
  - **The drives with a model are not in the suite.**
- **The Anthropic SDK** (MIT) is needed by the agent alone, in the Python that runs the server; without it the panel says so and the rest of the work is as it was.
