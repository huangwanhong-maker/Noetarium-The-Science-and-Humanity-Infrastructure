# 0132. The Diplomacy Domain and Multilateral Phronimos

- **Status:** Accepted, provisionally; its planned modules built by [ADR 0134](0134-the-affairs-of-an-undertaking.md)
- **Date:** 2026-11-19
- **Decided by:** the user. They asked for a work of multilateral knowledge management, evolution and governance, named it, agreed to its design (the domain `diplomacy`, the first case, the colour and port, the order of work, the domain as a bridge), and set the companion repository's licence. Claude wrote the design ([the idea](../ideas/2026-11-19-multilateral-phronimos.md)) and its survey.

## 1. Context

The user asked for a work for heterogeneous multilateral practice: partnerships among governments, international organisations, cities, firms, civil society, communities and artificial systems. It is to hold:

- governance;
- visualization;
- the parties' benefits and values;
- relational structure;
- project progress;
- archive objects;
- evidence;
- event trajectories.

Its concepts are those of their monograph *Future Talent Strategy: The Possibility of the Human–AI Multilateral Phronimos* (Huang 2026, DOI 10.17613/s9xqh-xm397). The survey ([Multilateral practice, diplomacy and partnerships](../../research/literature/reports/Multilateral%20practice,%20diplomacy%20and%20partnerships.md)) found that the infrastructure holds most of what is general. It lacks:

- a domain for the objects of multilateral practice;
- some methods in nomos and tyche: power indices beyond Shapley–Shubik, bargaining solutions, ideal points from votes;
- readers in the data library: IATI, UN votes, treaty records;
- the work itself.

## 2. Decision

1. **A domain `diplomacy`.** It is the field of multilateral relations among parties of every kind, not only states. It is a bridge at height 1 over law and policy: law holds treaties, sources and proceedings, and policy holds theories of change and transfer. It stands on the foundations kybernesis, tekmerion, archeion, nomos, tyche, apodeixis, morphe and calx, and is C++ from the start. Its modules, in their order:
   - **`relations`:** an undertaking as a typed dependency structure R = (V, {E_k}, D, Γ) (the monograph's §10.4.2):
     - participants of declared kinds;
     - relations of declared types, each tie present or recorded absent, a pair outside the observed design *not observed*, never absent;
     - decisions with their authorities, evidence, those affected and routes of challenge;
     - constraints;
     - each participant's relational position at a time;
     - the undertaking's own diagnostics of failure, the monograph's Table 10.2 computed: evidential gaps, authorisation, participation, AI dependency, succession, concentration.
   - **`partnerships`:**
     - commitments screened by declared criteria (SMART);
     - contributions, and whether commitments are honoured;
     - a ledger of values, by party, kind and scale, never aggregated without a declared rule;
     - dominance between value vectors respecting scale types;
     - mutual benefit read party by party as a dual verdict;
     - the modes of failure detectable from the record.
   - **Planned:**
     - `rules` (decision rules as simple games and their a priori power);
     - `institutions` (design, memberships as a formal context, regime complexes);
     - `events` (events as reports, each origin counted once);
     - `negotiation`;
     - `mandates`;
     - `reviews`.
2. **The work Multilateral Phronimos** (`applications/multilateral-phronimos`, port 9210):
   - on the shared studio host and page, its subjects as plug-ins;
   - drawn as a round with no head, relations as threads in layers and the trajectory as strands beneath;
   - orchid violet with laurel, provisionally.
3. **Foundations first where a general method is missing:**
   - nomos: Banzhaf and Penrose power, the Nash and Kalai–Smorodinsky solutions, the nucleolus;
   - tyche: ideal points;
   - each in the foundation's own repository, under its own decision.
4. **The companion repository** *Phronesis: The Practical Wisdom Infrastructure* holds documents only. They refer to this repository, say how to use the work, and cite the monograph, and are licensed CC BY-NC 4.0 (the user's word).

## 3. Alternatives Considered

- **A plug-in of the Urban Studio.** Rejected by the criterion recorded in ADR 0131 (the idea, §2.6): the object is an undertaking across many parties and places, with its own people.
- **A field at height 0.** Possible. A bridge over law and policy reuses treaties, sources and proceedings, and theories of change and transfer, rather than writing them again.
- **Partnerships as a domain of their own.** Rejected for now: partnerships are multilateral relations with commitments and contributions, and share the domain's relations, decisions and diagnostics.
- **One composite index of a partnership's quality.** Rejected. The monograph's Proposition 12.1 says alternatives that cross on two dimensions have no unique ranking without a justified rule, and the survey finds the indicators mostly ordinal.

## 4. Consequences

- **The order of the domains:** `diplomacy` at height 1, beside geodesy, evolution, cell biology, urban science, medical imaging, markets and industrial ecology.
- **Examples** use open data and cases invented and said to be. No Japanese city is used as a case (the user's word, 2026-11-19).
- **Built with this record:**
  - morphe's blocks, cut vertices and bridges (its decision 0020, 8 checks), for the succession diagnostic;
  - the domain's `relations` and `partnerships` (81 checks), with the theory note `domains/diplomacy/theory/undertakings-and-partnerships.md`, passing alone with their down-set;
  - nomos's voting power, two-party bargaining with Adjusted Winner, and the core and nucleolus (its decision 0014, 57 checks);
  - tyche's ideal points from votes, by EM accelerated by SQUAREM with a parametric bootstrap (its decision 0030, 15 checks);
  - the API's `science/multilateral.hpp` and `science.multilateral` (20 checks);
  - the data library's readers of IATI and of the UN General Assembly's votes, and the Migration MPTF as its first open case (20 checks). The votes are read only from a file a person downloads, as the Library's terms ask;
  - the visualization layer's `multilateral` (26 checks);
  - the work, with two plug-ins: the Migration MPTF, and the Charter's rules with an invented basin partnership (31 checks; driven in headless Chrome, 21 steps);
  - the companion repository's documents, written in a local clone and pushed only with the user's agreement: what Phronesis is, how to use the work, the correspondence between the monograph's concepts and the software (never a summary of its argument, which its licence, CC BY-NC-ND 3.0 IGO, does not allow), its contributor terms and its first decision.
- **Not yet built:** the planned modules; transfer between undertakings; a model's proposals; the record sealed in archeion's journal.
