# Knowledge and Learning Programmes: From Needs to Results

*Status: draft (ADR 0144). The theory note of the policy domain's `learning`.*

## 1. The Object

A knowledge and learning programme brings a cohort of participants (cities, agencies, teams) through activities of declared formats to actions of their own. Practice names its stages: the identification of needs, often from a diagnostic such as a scorecard; collaborative learning (workshops, peer exchange); a study visit or technical deep dive; advisory support; follow-up; and the capture of results (the World Bank's *Art of Knowledge Exchange*, 2nd ed., 2015; the survey *Learning finance scorecards and value capture*, Part I).

The domain keeps a programme as four records and computes on each. It never sums them into one score, and it never judges whether a programme was worth doing.

**Definition 1 (programme).** A programme is a tuple P = (U, N, A, X, R, C, K; L, G, g*). Its components are:

- participants U;
- needs N, each of a participant, with a priority;
- activities A, each with a format, a date, a cost, the participants it includes (all when none are named) and the needs it addresses;
- actions X, each of a participant, with a planned date, a due date and dated updates (started, completed, dropped, revised);
- results R, each of a participant, at a level of a declared ladder L (lowest first) and with a grade of evidence on a declared scale G (lowest first);
- contribution claims C;
- asks K;
- a sufficient grade g* ∈ G.

## 2. Coverage of Needs

An activity a reaches a need n of participant u when n is among a's needs and a includes u. A need is unmet when no activity reaches it. A participant's coverage is the share of its needs reached.

*Remark.* A need may come from a scorecard. The statements a participant's card scores at or below a declared level are its weaknesses, and each enters as a need with the scorecard's evidence (kybernesis's `scorecards`, decision 0006). Its priority is the number of levels below the cut plus one.

## 3. Action Plans at a Date

**Definition 2 (state).** The state of an action at a date τ is read from its updates dated at or before τ, in order:

- **dropped**, if it was dropped;
- **completed** on time or late, if it was completed, according as the date of completion lies at or before its due date or after it;
- otherwise **overdue** if τ is past the due date, **in progress** if it was started, **not yet due** if it has a due date, and **not started** otherwise.

A participant's share completed counts its actions not dropped.

**The pace of completion.** For each action with a planned date at or before τ, let T be the time from its planned date to its completion. T is observed for completed actions. It is censored at τ for open actions, and at the date it was dropped for a dropped one. The survival function S(t) = P(T > t) is estimated by Kaplan and Meier's product-limit estimator (peira's `survival`).

*Rationale.* Reading the pace only from completed actions would judge a cohort by those who finished first. Censoring keeps the open actions in the risk set for as long as they were open. Treating dropped actions as censored assumes that dropping is uninformative about the time an action would have taken. That is a declared reading: a competing-risks reading (Aalen–Johansen, also peira's) is the alternative where dropping is common.

## 4. Results on a Ladder

Results are read per participant. The highest level evidenced is the greatest level of the ladder among the participant's results whose grade is at least g*; results below g* are set aside and counted. The ladder is ordinal. The domain reports how many participants reached each level and none at all, and no mean of levels.

*Remark.* Three published codebooks of capacity outcomes overlap and differ: the CDRF of 2009, the WBI primer of 2011 and the *Art of Knowledge Exchange* (the survey's Part I). A ladder is therefore declared by the programme and never fixed by the domain, and a mapping between ladders is a declaration of its own.

## 5. Contribution Claims

**Definition 3 (claim).** A claim c consists of:

- a result r claimed;
- a chain of steps s₁, …, s_m from an activity to r, each step a pair (from, to) with the grade of its evidence (or none);
- rival explanations, each addressed by evidence at a grade or not addressed.

The chain is a path in a theory of change whose links are shown at their steps' grades (the change module, Definition of standing). By the change module's Proposition 1, a chain is no better shown than its weakest step: its standing is min_k grade(s_k), or *not shown* if a step has no evidence.

**Definition 4 (substantiated).** A claim is substantiated when its weakest step is at least g* and every rival is addressed at g* or above.

*Rationale.* This follows Mayne's conditions for a credible contribution claim (2001; the survey's Part I): a reasoned theory of change, activities carried out, a chain confirmed by evidence, and other influencing factors accounted for. The last is the rivals. A claim that cannot say what else could have produced its result is not substantiated, however strong its steps.

## 6. Formats Compared

Each substantiated claim carries one unit of credit, shared equally among the distinct formats of the activities on its chain. A format's credit is the sum of its shares, and its cost the sum of its activities' costs.

**Definition 5 (frontier).** A format is on the frontier when no other format has at least its credit at no more than its cost, and is strictly better in one of the two.

*Caveat.* Equal shares is a declared rule. A format's credit counts substantiated claims, not outcomes in the world, so the comparison claims no more than the claims do. The survey found no head-to-head evidence comparing study visits, peer exchange, workshops and twinning: the frontier is a programme's own record, not a general ranking.

## 7. Asks

An ask is a participant's request, answered or open at τ. The median time to an answer is taken among those answered by τ.

## 8. References

- Mayne, J. (2001), Addressing attribution through contribution analysis: using performance measures sensibly, *Canadian Journal of Program Evaluation* 16(1), 1–24.
- Otoo, S., Agapitova, N. and Behrens, J. (2009), *The Capacity Development Results Framework*, World Bank Institute.
- World Bank (2015), *The Art of Knowledge Exchange: A Results-Focused Planning Guide for Development Practitioners*, 2nd ed.
- Kaplan, E. L. and Meier, P. (1958), Nonparametric estimation from incomplete observations, *Journal of the American Statistical Association* 53, 457–481.
- The survey *Learning finance scorecards and value capture* (`research/literature/reports/`), Part I.
