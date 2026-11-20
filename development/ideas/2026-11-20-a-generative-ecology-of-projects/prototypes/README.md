# Prototypes of the Generative Ecology of Projects: A Record

*Seed (2026-11-20 and 21). Four small prototypes written to test the assumptions on which the design in this folder most depends. This record states for each what was asked, what was built, what happened, including the runs that failed and the results that were not expected, and what the result does and does not show. The numbers are in `results/`; every program here runs again from this folder.*

## 1. Purpose and Scope

The design of [the general framework](../case-3-the-general-framework.md) rests on suppositions about the conduct of language models that no argument can settle. Four were chosen for a first test because the design fails without them and because each can be tested at small cost:

1. that a genome written in words is inherited when its own organ of reproduction copies it (Prototype 1);
2. that a unit of work has a compact genome from which its body can be developed again, and that checks hold such redevelopment against drift (Prototype 2);
3. that one invariant ground carries the life of a need on sites of different kinds (Prototype 3);
4. that variation part by part follows a changing environment more cheaply than variation of the whole (Prototype 4, a numerical check of Proposition 2 of the general framework, with no model).

The prototypes are deliberately small. Each uses one toy unit or one toy genome, a few lines of descent and a handful of trials. They show that a mechanism operates and where it first breaks; none measures a rate that could be carried to a real project.

| File | Content |
|---|---|
| `ask.py` | The one way to a model: the infrastructure's own provider (`api/python/science/models.py`), with a count of calls |
| `ground.py` | A ground in miniature: store, sealed journal, energy and escrow, needs, warranties, challenges, facts, settlement; conservation checked after every act |
| `unit/refcode.py`, `unit/checks.py` | The toy unit (120 lines) and its 42 checks, which no developer is shown |
| `p1_heredity.py` | Prototype 1 |
| `p2_regeneration.py`, `p2b_chain.py` | Prototype 2 and its longer chains |
| `p3_two_sites.py`, `p3b_beyond_three.py` | Prototype 3 and a note on what its supporting fact leaves open |
| `p4_modular_variation.py` | Prototype 4 |
| `results/` | `p1.json`, `p1-lines.json`, `p2.json`, `p2-chain.json`, `p2-sample-body.py`, `p3.json`, `p3-beyond-three.json`, `p4.json` |

**Models and cost.** The lineages and developers were run on Claude Haiku 5.5 (`cheap` below) and, in a few comparisons, on Claude Sonnet 5.5 (`careful`), through structured output at low effort. The runs made 322 calls in all, of which about 20 went to the careful model. Counted from characters, they read about 0.9 million characters and wrote about 0.8 million, which is about 230 thousand tokens in and 200 thousand out, thinking not counted. At list prices this is of the order of one United States dollar. No prompt contained anything of the project or of a person: the genomes, the unit and the laws were invented for the purpose.

**Safety of execution.** Bodies written by a model were run only in a child process, from a temporary directory, with a time limit, after a refusal of any body that reaches for the file system, the network or the interpreter.

## 2. Prototype 1: Heredity of a Genome Written in Words

### 2.1 Question and Design

The framework's first empirical question is whether a model, asked to write a varied copy of the text that instructs it, preserves what made that text effective, and whether the boldness of variation is itself inherited, as Corollary 2 of the paper supposes.

A founder genome of 229 words states ten organs, several of them as numerical policies (pursue a need when its bounty is at least 40; stake 20 percent; divide above 300). Two founders differ only in the organ of reproduction. The *conservative* organ reads: copy every organ word for word, then change exactly one number in one organ by at most a tenth, and copy this organ unchanged. The *bold* organ reads: rewrite every organ in fresh words, change two or three policies as you judge will serve the offspring, and you may also rewrite this organ. A model acting as the organ of reproduction writes the offspring's genome, and the offspring's genome writes the next. Each founder began three lines of six generations on the cheap model and one line of three generations on the careful model. There is no selection at all: the prototype isolates transmission.

Conduct is measured on a fixed battery of twenty-four decisions that the policies settle (which of eight needs to pursue, three stakes and claim kinds, three decisions to carry, three to challenge, three to divide, one endowment), several of them placed close to the founders' thresholds so that a changed number shows. The conduct of every genome was measured by the cheap model, so that the reader of conduct is held fixed.

### 2.2 Results

A genome measured three times gave the same twenty-four decisions each time, for both founders, so the measure has no noise of its own at this size.

| Line | Change of text per generation | Conduct like the parent | Conduct like the founder after six generations | Organs present | Sterile lines |
|---|---|---|---|---|---|
| Conservative, cheap model (3 lines) | 0.001 | 0.96 to 1.00 | 0.92, 1.00, 0.96 | 10 of 10 | 0 |
| Conservative, careful model (1 line, 3 generations) | 0.001 | 0.96 to 1.00 | 0.96 | 10 of 10 | 0 |
| Bold, cheap model (3 lines) | 0.15 to 0.49 | 0.71 to 0.93 | 0.75, 0.67, 0.71 | 10 of 10 | 0 |
| Bold, careful model (1 line, 3 generations) | 0.42 to 0.70 | 0.79 to 0.92 | 0.92 | 10 of 10 | 0 |

: Change of text is one minus the similarity ratio of parent and offspring; conduct is the share of the twenty-four decisions that agree.

Four things follow from the lines themselves (`results/p1.json`).

**Heredity held.** Under the conservative organ an offspring differed from its parent in one number, and after six generations its conduct agreed with the founder's in 22 to 24 of 24 decisions. No line lost an organ, and no line became sterile.

**The boldness of variation was inherited.** The conservative organ was copied unchanged in every generation of every line. The bold organ was reworded in each generation and kept its sense: after six generations the three bold lines read "restate every organ in fresh words with the same sense; adjust two or three policies where the offspring will likely fare better; this organ may also be rewritten", or close variants. The two kinds of line differed in their realised rate of variation by two orders of magnitude throughout.

**Text changed far more than conduct.** In the bold lines the text stood at a distance of 0.55 to 0.74 from the founder's after six generations, while conduct still agreed in 16 to 18 of 24 decisions. Much rewording was neutral for conduct.

**Variation fell on one place.** This was not expected. In the conservative lines on the cheap model, 15 of the 18 variations changed the same number, the bounty threshold of the foraging organ, which went up and down again (40, 42, 40, 38, 35, 36 in one line); two changed the premium, and the careful model's line changed the endowment. The instruction said "one number in one organ" and the model chose, nearly always, the first prominent number in the text. Where variation falls is therefore decided by the copier's habits unless the organ of reproduction says where.

### 2.3 Failed Run

The first run stopped after its forty-two reproductions, when the first measurement of conduct was refused: the schema of the answer asked for arrays of exactly eight and exactly three items, and structured output accepts no minimum number of items other than 0 or 1 (`400 invalid_request_error: For 'array' type, 'minItems' values other than 0 or 1 are not supported`). The lines of that run were lost, since the program wrote nothing before it measured. The program now checks the shape of an answer itself and keeps the lines (`results/p1-lines.json`) before measuring conduct. The lesson for the design is general: what a ground asks of an answer must be checked by the ground, since the provider's schema covers only part of it.

### 2.4 Limits

The conduct measured is the reading of stated policies, which is the easiest kind of conduct to inherit. Skill at open tasks, where the framework most needs heredity, was not measured. Six generations with no selection show transmission; they do not show that selection can maintain a genome against the drift seen in the bold lines. The founders were written to be copied (organs under headings, policies as numbers).

## 3. Prototype 2: The Genome of a Unit and Its Regeneration

### 3.1 Question and Design

The general framework gives every unit of a project a genome: a bounded text from which a model can develop the unit's body again. The prototype asks whether such a text exists for a small unit, which of its parts carry the unit's particular rules, and what happens when a body is reseeded generation after generation.

The unit is `refcode`, 120 lines that parse, order and step through reference codes such as `GB-0042/3/7a`. Its rules are conventions that cannot be guessed from its name: outer spaces are dropped and the country is put in capitals, the repository 0000 is refused, a level has no leading zero, a code without a letter sorts before the same number with a letter, the sibling after 7a is 7b and none follows z. Forty-two checks hold these rules. A developer is never shown the checks or the body.

*Sequencing.* A model reads the body and writes the genome in four bounded parts: identity (at most 110 words), laws (180), exemplars (10 lines of `call -> result`) and development (60). *Regeneration.* A developer that has only a genome writes the body; the checks are run on what it wrote. Four genomes were compared, cut from one sequencing by the careful model, with four developments each by the cheap developer and two by the careful developer for the smallest and the largest. *Reseeding in a chain.* Body, genome, body, genome, for four and then ten generations: without an anchor (one development a generation) and anchored by the checks (three developments a generation, the body that passes most is kept and sequenced).

### 3.2 Results

| Genome given to the developer | Words | Checks passed of 42, cheap developer | Careful developer |
|---|---|---|---|
| Identity alone | 63 | 37, 38, 35, 37 | 35, 36 |
| Identity and laws | 214 | 42, 42, 42, 42 | |
| Identity and exemplars | 101 | 42, 42, 42, 42 | |
| All four parts | 284 | 42, 42, 42, and one body that did not load | 42, 42 |

: Regeneration of the unit from four genomes (`results/p2.json`).

**A genome of about a hundred words regenerated the unit.** With the laws or with the exemplars beside the identity, every development that loaded passed all forty-two checks.

**What the identity alone lost was exactly what is particular to the unit.** The checks that failed were the same in nearly every development: outer spaces dropped, the country capitalised, the repository 0000 refused, equality of codes written differently, no sibling after z, the leading zero. Where the genome was silent the developer wrote what a careful parser usually does, which is to refuse input that is not already clean. The careful developer lost as much as the cheap one, since the content was absent from the genome and present in neither model's habits.

**Ten worked cases carried as much as a paragraph of laws.** The exemplars are 37 words. This was not expected, and it bears on what a genome should hold: cases chosen for the rules that are easiest to get wrong.

**Development has accidents.** One body of the twenty developed for this comparison did not load (a module used before it was imported), and the checks reported it.

| Chain | Checks kept in generations 1 to 10 | Lines of the body |
|---|---|---|
| Without anchor, first | 42, 42, 42, 42, 42, 42, 42, 42, **20**, 42 | 142 to 199 |
| Without anchor, second | 42 in every generation | 133 to 198 |
| Without anchor, third | 42, 42, 42, 42, **41, 41, 41, 41, 41, 41** | 155 to 196 |
| Anchored by the checks | 42 in every generation; the three developments gave 41, 42, 41 in generation 5 and 42, 41, 41 in generation 6 | 173 to 199 |

: Reseeding in chains of ten generations (`results/p2-chain.json`). The first run of four generations (in `results/p2.json`) lost one rule in generation 3 without an anchor and none when anchored.

**A rule lost without an anchor did not return.** In the third chain a development left out the refusal of a leading zero in generation 5; the genome sequenced from that body no longer stated the rule, and the five generations after it lacked it. The same rule was the one lost in the first run. It is the rule with the least text behind it in the genome.

**An accident of development was not inherited.** In the first chain the body of generation 9 passed 20 checks, and the body of generation 10 passed all 42: the genome sequenced from the damaged body still described the intended behaviour. A fault of the body is transmitted only if sequencing reads it as intended.

**The anchor did its work twice in ten generations.** In generations 5 and 6 of the anchored chain two of the three developments lacked a check; the one that passed was kept, and from generation 7 all three passed again. Selection on bodies repaired the genome through sequencing.

**Bodies grew.** In every chain the body grew from 120 lines to between 170 and 200 within a few generations and stayed there, with all checks passing. The checks do not see length. The genomes written by the cheap model also exceeded their stated limits at times (330 to 458 words against 390), so a bound that is only requested is not kept.

### 3.3 Limits

The unit is small, regular and well described by its own docstring, so the result is the easy case. The survey gives the reasons for caution: descriptions written by a model for agents did not help in the one study found and raised cost (Gloaguen et al. 2026), specifications decay (Agarwal et al. 2026), and independently developed versions fail together where a specification is ambiguous (Knight and Leveson 1986; Ron, Baudry and Monperrus 2026). Regeneration is judged here by the unit's own checks; a body can pass them and differ where they do not look, as the growth in length shows. Round-trip correctness (Allamanis et al. 2024) and PROOF (Zhang et al. 2026) precede this test; what is added is its use on a genome of declared parts, and the chain.

## 4. Prototype 3: One Ground and Two Sites

### 4.1 Question and Design

The general framework claims that one invariant ground serves projects of every kind, and that a kind of project is a *site*: its claim kinds, its procedures and its reserved acts. `ground.py` is that ground at the second rung (no heredity): a store by digest, a sealed journal, accounts and escrows with conservation checked after every act, and four acts on work (a need, a warranty, a challenge, a fact) with settlement. It contains no word about software or theory. `p3_two_sites.py` declares two sites and runs the life of one need on each, with a producer and a challenger run by a model and a person whose acts the script fixes.

*The software site.* The person posts a need with a bounty of 60: a function that sorts reference codes. The claim kind `agrees` names a reference held by the harness; a counterexample is an input on which the artefact and the reference differ. The producer sees the unit's genome from Prototype 2 and never its body.

*The theory site.* The person asks which of eight laws of magmas imply which, and first writes an answer, sealed in the journal. The claim kind `implies` is refuted by a magma of at most four elements that satisfies the premise and violates the conclusion. For each claim left standing the person asks the fact `no_small_countermodel`, an exhaustive search of every magma of up to three elements.

### 4.2 Results

**The same ground ran both sites.** In all eighteen trials energy was conserved after every act and the journal verified as sealed at the end.

**On the software site every artefact stood** (four trials). The producers staked 6 to 8 of a permitted 20. The challengers made sensible attempts, among them digits and spaces from other scripts (an Arabic-Indic digit, a non-breaking space, a ligature in place of two letters), codes written twice in different case, and the boundaries of every rule; none refuted. In one trial the challenger made no attempt and kept its energy.

**On the theory site the first run showed a fault of the design, in the view given to a lineage.** The producer listed eight to eleven claims and staked 9 or 10 on each; its thirty units of energy were spent after three, and the ground refused the rest. Only three claims stood, the three that the person had already written. The producer had not been told what it held. In the second run the prompt stated the balance and the price of a claim, and the producers warranted 7 or 8 claims within their energy in four of five trials, all of them true, of which 4 or 5 went beyond what the person had written first. A lineage's view must include its balance and the prices of the ground.

**Producers were careful and challengers abstained.** In the second run no false claim was made and no challenge was attempted. A third run added two laws and a producer whose genome made it bold. In five trials it made one false claim (that a constant operation is idempotent), and a challenger refuted it with a magma of two elements and collected the stake. Refutation by a model-run lineage on the theory site was therefore seen once.

**The supporting fact was weaker than it looked.** With the two added laws, sixteen implications pass the search of magmas of up to three elements. `p3b_beyond_three.py` shows that six of them are false: the integers modulo 7 with $x * y = 5x + 3y$ satisfy both added laws and violate three others. No producer happened to claim any of the six. Had one done so, the claim would have stood, with a fact in its support, and no challenger could have refuted it within the four elements that the claim kind allowed. Standing under a bounded search is a statement about the bound, and a record must say so in the standing itself.

### 4.3 Failed Runs

Two runs failed before any result. In the first, the theory trials stopped when the ground refused the person's bounty: the person held 60, had paid half a unit to seal the answer written first, and could not lock 60. The refusal was the ground's conservation working as designed, and the script now gives the person the energy for what the person does. In the second, the program crashed while printing a challenger's attempt that contained an Arabic-Indic digit, which the console's encoding could not show; the results had been written, and the program now prints in UTF-8.

### 4.4 Limits

The turn order was fixed by the script, there were two lineages, and each trial was one need. Nothing here tests carrying, premiums, upkeep, persistence or division. The producers' accuracy on these easy tasks left the challengers little to do, so the prototype says little about whether a bounty buys refutation. The survey's warnings apply in full: agents of one model share their errors (Kim et al. 2025; Begin et al. 2026), agents report completion more often than checks confirm it (Li et al. 2026), and a stake presupposes a calibration that tool-using agents were found to lack (Xuan et al. 2026).

## 5. Prototype 4: Variation Part by Part

### 5.1 Question and Design

Proposition 2 of the general framework states that when a type is a tuple of parts and each part varies at a rate of its own, the price of following a change is the logarithm of the alternatives of the part that changed, and the best rate for each part is the rate at which that part of the environment changes. The prototype checks the bound and the comparison numerically. Types are triples of parts with four alternatives each (64 types). The environment names a best alternative for each part; the parts change at rates 0.02, 0.002 and 0.0002 a turn. A type is multiplied by 1.6 for each part that matches and by 0.7 for each that does not. The run has 120,000 turns, in which the parts changed 2,425, 226 and 25 times.

### 5.2 Results

| Variation | Growth per turn | Bound of Proposition 2 or of Proposition 3 of the paper | Growth lost against the matched type (1.410) |
|---|---|---|---|
| Part by part, each part at its own rate $S_j/(T-1)$ | 1.3007 | 1.2710 | 0.109 |
| Part by part, one rate for all parts, the best such rate (0.0059) | 1.2879 | 1.2534 | 0.122 |
| The whole type at one rate, the best such rate (0.0159) | 1.2574 | 1.2099 | 0.153 |
| No variation | −0.4111 | | 1.821 |

: Growth under three schemes of variation (`results/p4.json`). Every growth is at least its bound.

Variation part by part lost 0.109 of growth against 0.153 for variation of the whole type, a reduction of the cost of tracking by 29 percent in this model, and giving each part its own rate accounts for about a third of that reduction.

### 5.3 Limits

The environment here changes one part at a time, which is the case that the proposition favours. The survey states the condition under which the biological result holds: modular structure speeds adaptation when goals vary modularly and share structure (Kashtan and Alon 2005; Parter, Kashtan and Alon 2008), and it does not arise from pressure for performance alone (Clune, Mouret and Lipson 2013). Prototype 1 adds a practical condition: a model asked to vary "one number" varied the same one each time, so part-wise rates must be stated in the organ of reproduction to be had at all.

## 6. Summary of What the Prototypes Show

| Supposition of the design | What was seen | What remains untested |
|---|---|---|
| A genome in words is inherited | Yes over six generations, with conduct preserved and no organ lost; the boldness of variation was inherited with it | Skill at open tasks; selection against drift; longer descent |
| Variation is spread over the genome | No: without instruction it fell on one number | Whether stated part-wise rates are followed |
| A unit has a compact genome that regenerates it | Yes for a unit of 120 lines, from about 100 words; identity alone loses the unit's particular rules | Larger and less regular units; genomes of several units together |
| Checks hold reseeding against drift | Yes over ten generations; without them a lost rule stayed lost | Rules that no check holds; growth of the body |
| One ground serves several kinds of site | Yes for a need's life on a software site and a theory site | Carrying, premiums, upkeep, division; a population |
| Lineages size their stakes to their means | Only when shown their balance | Calibration of stakes to confidence |
| A supporting fact gives standing | Only up to its bound; six of sixteen supported implications were false | A site with a proof checker |
| Variation part by part is cheaper | Yes in the model, by 29 percent | Environments whose changes are not modular |

The unexpected results are the ones most worth carrying into the design: the hotspot of variation, the sufficiency of a few exemplars, the growth of bodies under reseeding, the abstention of challengers, the producer that staked without knowing its balance, and the supported implications that were false.
