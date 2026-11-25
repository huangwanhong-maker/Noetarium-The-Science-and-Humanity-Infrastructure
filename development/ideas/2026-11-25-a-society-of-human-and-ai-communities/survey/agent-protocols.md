---
status: seed
---

# Protocols by Which AI Agents Act for Principals: A Survey with Its State of Verification

*This survey was made on 2026-11-25 by a separate search session for the design record [A Society of Human and AI Communities](../design.md), under a bounded budget of searches. Its tags say how each statement was obtained. It is kept as that session wrote it, with its title and its last heading renamed; [existing-work.md](../existing-work.md) says what the design takes from it.*

Search date: 25 November 2026. Tags: **[O]** primary page or document opened and read; **[S]** found by search, abstract or excerpt read, page not opened; **[M]** from memory, unconfirmed. Quotation marks enclose words read in the source or, for [S], in the search excerpt. The MCP pages were read as full text; A2A, RFC 9943 and the FIDO post through an extraction tool, with quotations as it reported them. Points: (a) authority, (b) identity, (c) revocation and expiry, (d) records, (e) governance. A budget of 22 tool calls ended the checking.

## 1. Model Context Protocol

- **Maintainer, version, address.** Open project begun by Anthropic in November 2024 [M]. Revision 2026-07-28 succeeds 2025-11-25: https://modelcontextprotocol.io/specification/2026-07-28/changelog and https://modelcontextprotocol.io/specification/2026-07-28/basic/authorization [O]. Excerpts mostly treat it as current since 28 July 2026, one as a release candidate; a next target of 2027-03-30 is listed [S].
- **Shape of 2026-07-28 [O].** Sessions and the `initialize` handshake are removed; each request carries protocol version and client capabilities in `_meta`. Server-initiated requests, elicitation among them, give way to Multi Round-Trip Requests: a result of type `input_required` asks for more and the client retries the original request. Tasks leave the core for the official extension `io.modelcontextprotocol/tasks`; capabilities gain an `extensions` field. Roots, Sampling and Logging are deprecated.
- **(a) Authority [O].** "Authorization is OPTIONAL". "A protected MCP server acts as an OAuth 2.1 resource server"; the client acts "on behalf of a resource owner"; the authorization server's details "are beyond the scope of this specification". Required: OAuth 2.1 (draft-ietf-oauth-v2-1-13), Protected Resource Metadata (RFC 9728), Resource Indicators (RFC 8707), issuer validation (RFC 9207). Authority is a set of scope strings, raised by step-up challenge. A token serves one server, which "MUST NOT accept or transit any other tokens". Delegation chains, limits of spending and token exchange are absent from the page; optional "MCP Authorization Extensions" (https://github.com/modelcontextprotocol/ext-auth) include Enterprise Managed Authorization [S].
- **(b) Identity [O].** The client software is identified by a Client ID Metadata Document (an HTTPS URL as `client_id`), by pre-registration, or by Dynamic Client Registration (deprecated); "Clients SHOULD identify themselves on each request" with a self-declared `clientInfo`. The user is known to the authorization server alone. The agent and the party answering for it lack a field.
- **(c) Revocation and expiry [O].** "Invalid or expired tokens MUST receive an HTTP 401 response"; refresh tokens are at the authorization server's discretion; a revocation procedure is absent.
- **(d) Records [O].** Receipts, signed results and audit logs are absent from both pages; OpenTelemetry trace context may travel in `_meta`.
- **(e) Governance.** Enhancement proposals (SEPs) as pull requests; a feature lifecycle with "a minimum twelve-month deprecation window" [O]. Held since December 2025 by the Agentic AI Foundation, a fund of the Linux Foundation [M]; the foundation's site carries a post on this revision [S] (https://aaif.io/blog/mcp-2026-changes-the-rules).
- **Registry.** Preview since September 2025 [M]; not checked.

## 2. Agent2Agent (A2A)

- **Maintainer, version, address.** https://a2a-protocol.org/latest/specification/ [O; characters 0 to 100,000 and 150,000 to 250,000 of 257,757]: "Latest Released Version" 1.0.0; a release date is absent. Begun by Google in April 2025; a Linux Foundation project since June 2025 [M].
- **(a) Authority [O].** "The client obtains the necessary credentials through an out-of-band process". "Once authenticated, the A2A Server authorizes requests based on the authenticated identity and its own policies." Within a task, an agent needing further credentials may "delegate the fulfillment of this authorization to the client", and "the client may further delegate the authorization request to its client". For these, "The A2A protocol does not define the scope, representation, validity, or revocation semantics".
- **(b) Identity [O].** The Agent Card names a `provider` and `securitySchemes`. "Agent Cards MAY be digitally signed using JSON Web Signature (JWS)" over content "canonicalized using the JSON Canonicalization Scheme (JCS)"; the header carries `alg` and `kid`; the verifier is to "Retrieve the public key using the `kid` and `jku` (or from a trusted key store)". The trust anchor of a card's key is left open.
- **(c) Revocation and expiry [O].** "Expired or revoked keys MUST NOT be used for verification"; the way to learn of a revocation is unstated.
- **(d) Records [O].** "Agents SHOULD provide audit trails for sensitive operations"; format, signing and receipts are unspecified.
- **(e) Governance.** The page links a post dated 2026/08/27 on A2A joining the Agentic AI Foundation [O, link title only]; excerpts confirm a hosted project beside MCP, with varying dates [S].

## 3. Agent Payments Protocol (AP2)

- **Maintainer, version, address.** Google, 16 September 2025, v0.1 [M]. Excerpts: v0.2 on 28 April 2026, the day Google gave AP2 to the FIDO Alliance [S]. FIDO post of 26 May 2026, https://fidoalliance.org/building-the-trust-layer-for-agentic-payments-with-ap2-and-verifiable-intent/ [O]: AP2 and Mastercard's Verifiable Intent, "co-developed with Google", were contributed to FIDO. A FIDO deck for W3C TPAC 2026 reports a draft v0.1 for FIDO review, a target of December 2026 and a merging of the two specifications [S] (https://www.w3.org/2026/Talks/TPAC/fido-agentic-payments-tpac2026.pdf). https://ap2-protocol.org/specification/ returned HTTP 404 [O].
- **Mandates [O].** Mandates are "Verifiable Digital Credentials (structured, cryptographically signed objects)" capturing what a user has authorised an agent to do. Checkout Mandate: "This captures the details about and conditions on what the user wants to buy." Payment Mandate: "This captures the details about and conditions on how the user wants to pay (amount, instrument, timing)." Each has an open phase, "during which they capture the user's constraints and goals for the transaction", and a closed phase, in which "they capture the user's (or agent's) authorization for a specific transaction amount bound to a finalized checkout." The post is silent on the Intent Mandate and Cart Mandate of v0.1 [M].
- **Signing.** The post leaves signer and format unnamed [O]. Excerpts: SD-JWT verifiable credentials with key binding and selective disclosure; in autonomous mode an open mandate carries the agent's public key as a `cnf` claim [S].
- **Verifiable Intent [O].** "a cryptographic credential framework that transforms user authorization into portable, verifiable evidence"; "If AP2 defines how intent is created and shared, Verifiable Intent defines how it is proven." Its intent statements hold "constraints, scope, and delegation rules".
- **(c), (d).** Revocation, expiry and the agent's identification are unaddressed in the post [O] and absent from the excerpts [S]. Record: mandates "form a durable record of consent" and serve "dispute resolution through structured, portable evidence of user intent" [O].

## 4. Other Payment and Commerce Protocols

- **Agentic Commerce Protocol.** OpenAI and Stripe, September 2025 [M]; latest specification dated 2026-04-17; Apache 2.0 by one source, "Beta" by another; OpenAI withdrew its first Instant Checkout in March 2026 [S]. Authority is a payment token scoped to merchant, amount and time [M].
- **Universal Commerce Protocol.** Google and Shopify, January 2026 [S].
- **x402.** Coinbase, May 2025: stablecoin payment over HTTP 402, the payer a wallet address [M]; one excerpt reports a move to the Linux Foundation in July 2026, unverified [S].
- **Trusted Agent Protocol.** Visa, October 2025: a registered agent signs its HTTP requests for the merchant to verify [M]; folded into Visa Intelligent Commerce Connect, launched April 2026 [S].
- **Agent Pay.** Mastercard, April 2025: scoped, tokenised card credentials issued to agents [S].

## 5. Identity and Authorization

- **OpenID Foundation.** "Identity Management for Agentic AI: The new frontier of authorization, authentication, and security for an AI agent world", arXiv 2510.25819 [S, title]; the Foundation's whitepaper of October 2025 [M]. Position [M]: single-domain agents fit OAuth 2.1; recursive delegation, narrowing of scope across agents and propagating revocation remain open.
- **IETF [S].** OAuth working group rechartered in June 2026 with agents acting for users in scope; draft-ietf-oauth-identity-chaining approved for publication; WIMSE interim of 3 June 2026 on agent authentication, adoption under discussion. Individual drafts: draft-klrc-aiagent-auth, draft-mishra-oauth-agent-grants-02 ("OAuth Profile for Delegated AI Agent Authorization"), draft-mcguinness-oauth-ai-agent-instance-00, draft-sweeney-wimse-credential-delegation-00 ("Credential Delegation Protocol for AI Agents in Multi-System Environments").
- **Agent name service.** Not found [S]; an individual draft of 2025 proposed a DNS-like directory of agents backed by PKI certificates [M].
- **W3C [M].** Verifiable Credentials Data Model 2.0 (Recommendation, May 2025), with Bitstring Status List for revocation; DIDs 1.0 (Recommendation, July 2022).
- **Capability tokens [M].** UCAN (1.0 release candidate): signed delegations between DIDs, each link able only to narrow; revocation by any issuer in the chain; a signed receipt of an invocation's result. ZCAP-LD (W3C community draft): capability chains with caveats and invocation proofs. Biscuit (v3): public-key tokens narrowed offline by appended Datalog checks, with revocation identifiers. Macaroons (Birgisson and colleagues, 2014): HMAC-chained caveats; expiry by caveat.

## 6. Signed Records

- **SCITT.** RFC 9943, the SCITT architecture, Proposed Standard, June 2026 (formerly draft-ietf-scitt-architecture-22); IETF SCITT working group; https://datatracker.ietf.org/doc/draft-ietf-scitt-architecture/ [O]. TS abbreviates Transparency Service.
  - Signed Statement: "an identifiable and non-repudiable Statement about an Artifact signed by an Issuer." Transparent Statement: "a Signed Statement that is augmented with a Receipt created via Registration in a TS." Issuer: "an identifier representing an organization, device, user, or entity securing Statements about supply chain Artifacts." [O]
  - Receipt: "a COSE Single Signer Data Object as defined in [RFC9942]" [O], the service's signed proof that a statement is in its log [M]; RFC 9942 is COSE Receipts, June 2026 [O].
  - Identity: `iss` and `sub` in the CWT Claims header, the key by `kid`, `x5t` or `x5chain`; statements are COSE_Sign1 [O].
  - Revocation: the log is "linear and irrevocable"; a later statement from the same issuer and subject supersedes; "Revocation strategies for compromised keys are out of scope for this document." [O]
  - Audit: a service gives enough "to reproduce the Registration checks"; its registration policies are themselves registered [O].
- **Certificate Transparency.** RFC 9162, December 2021 [O, cited in RFC 9943]. A log returns a signed certificate timestamp, a promise of inclusion; signed tree heads give proofs of inclusion and consistency [M].
- **Sigstore Rekor [M].** Version 1 returned a signed entry timestamp as a promise of inclusion; version 2 returns an inclusion proof against a signed checkpoint.
- **C2PA [M].** A manifest of assertions bound to an asset by a signed claim with a trusted time-stamp. The word *receipt* lies outside the vocabulary of these three.

## 7. Research Proposals

- **South and seven co-authors, "Authenticated Delegation and Authorized AI Agents", arXiv 2501.09674, 16 January 2025; an ICML 2025 position paper [S].** A framework for authenticated, authorised and auditable delegation: OAuth 2.0 and OpenID Connect extended with agent-specific credentials, and natural-language permissions translated into auditable access-control settings [S]. (b) An agent-ID token beside the user's ID token, joined by a delegation token; (c) OAuth's expiry and revocation; (d) auditability as an aim, the log format open [M].
- **Chan and colleagues, *Infrastructure for AI Agents*, arXiv 2501.10114, January 2025 [M; not found by the search].** Systems external to agents for attribution (identity binding, certification, agent IDs), for shaping interaction (agent channels, oversight layers, commitment devices) and for response (incident reporting, rollbacks). An agenda; formats are open.
- **Chan and colleagues, *IDs for AI Systems*, arXiv 2406.12137, June 2024 [M].** An identifier for each instance of an AI system, carrying information on the system and its deployer; issuance and governance are open.

## 8. Other Agent-Communication Efforts

- **ACP (IBM).** Merged into A2A in August 2025; repository archived, with a notice to migrate [S].
- **AGNTCY.** Begun at Cisco: agent discovery, identity, messaging and observability [S]; a Linux Foundation project since July 2025 [M].
- **ANP.** Open-source community protocol; a 2025 survey lists its stage as "Landing"; 2026 status not found [S]. Identity by DIDs (`did:wba`) [M].
- **NANDA.** Not found [S]; an index of agents led from MIT [M].
- **W3C AI Agent Protocol Community Group.** Not found [S]; formed in 2025, outside the standards track [M].

## 9. Federation among Servers

- **ActivityPub [M].** W3C Recommendation, January 2018. An actor is a URL on its server, which answers for it; HTTP signatures by convention. A `Move` carries followers; posts stay.
- **AT Protocol.** IETF working group "Authenticated Transfer" (atp): charter approved 19 March 2026, announced 2 April 2026, based on draft-holmgren-at-repository, keeping PLC and `did:web` identifiers; a status session at IETF 126 (July 2026); published RFCs not found [S] (https://datatracker.ietf.org/doc/charter-ietf-atp/). Identity is a DID whose rotation keys can be held apart from the host; the account's record is a signed Merkle repository, exportable and movable between hosts [M].
- **Matrix [M].** Identity is `@user:server`; a room is a hash-linked graph of events signed by servers and replicated among them; portable accounts are proposed.

## 10. Law

- **UNCITRAL Model Law on Automated Contracting [S].** Adopted 11 July 2024; endorsed by General Assembly resolution 79/119 of 4 December 2024 (text at https://uncitral.un.org/sites/uncitral.un.org/files/mlac_en.pdf). Article 7: parties may agree their own attribution (7(1)); the default attributes the action to the user of the system (7(2), wording unconfirmed); "Attribution of an action carried out by an automated system shall not be denied on the sole ground that the outcome was unexpected." (7(3)); other law on the consequences of attribution stands (7(4)). A model law takes force where enacted; enactments not found.
- **In force, European Union [S].** AI Act: Article 50 transparency duties have applied since 2 August 2026, with a grace to 2 December 2026 for machine-readable marking by generative systems already on the market. The Digital Omnibus, Regulation (EU) 2026/1744, in force 27 July 2026 by several sources, moved high-risk duties to 2 December 2027 (Annex III) and 2 August 2028 (Annex I).
- **In force, United States [M].** UETA section 14 and E-SIGN give effect to contracts formed by electronic agents; California AB 316 (2026) bars the defence that an AI acted autonomously. State laws naming AI agents: not found [S].
- **Pending or withdrawn [M].** Product Liability Directive 2024/2853, treating software as a product: transposition due 9 December 2026. The proposed AI Liability Directive was withdrawn in 2025.

## 11. Findings for the Design

**What existing protocols specify of authority and answerability**

1. MCP and A2A carry authority as OAuth-style bearer credentials: MCP fixes scopes, one audience per token and step-up; A2A leaves the decision to the server's "own policies" [O].
2. Among the protocols opened, AP2 with Verifiable Intent alone makes authority a signed object shown to third parties, first as constraints, then closed on one transaction [O]. Its subject is purchase.
3. Chains that only narrow exist in capability tokens (UCAN, ZCAP-LD, Biscuit, Macaroons) [M]; IETF work on delegation for agents stands at individual drafts [S]; the MCP and A2A cores carry none [O].
4. Answerability stops early: MCP names client software, A2A a provider on an optionally signed card, AP2 a user's key [O, S]; the model law's default points to the person using the system [S].

**What they leave to implementers**

5. Revocation. MCP relies on token expiry; A2A defines none; AP2's was not found; SCITT's log is "linear and irrevocable", with supersession [O]. Revocation that travels down a chain is open.
6. Records. MCP specifies none; A2A recommends audit trails; AP2 claims a "durable record of consent" [O]. RFC 9943 with RFC 9942 is the finished standard for signed statements and receipts [O].
7. Limits of spending or use appear in payment protocols alone; an MCP scope is a string.
8. The unit of action. An MCP 2026-07-28 tool call is stateless and may span several round trips; tasks are an extension [O]. An adapter can attach a mandate to each request in `_meta` or as a declared extension (inference).

**Terms and formats to adopt or to keep apart**

9. In AP2 a "mandate" is a signed verifiable credential capturing what a user has authorised an agent to do in a purchase: a Checkout Mandate and a Payment Mandate, each open and then closed [O]; Intent, Cart and Payment belong to v0.1 [M]. A general mandate needs a qualifier; AP2's open and closed forms correspond to a standing mandate and one authorised act.
10. "Receipt" in SCITT is a transparency service's proof of registration [O]; in UCAN it is a signed result of an invocation [M]. The design should state one sense.
11. Formats at hand: COSE_Sign1 statements with COSE receipts; JWS over JCS; SD-JWT credentials with key binding; RFC 8707 and RFC 9728 on the MCP side.
12. Venues: MCP and A2A under the Agentic AI Foundation, AP2 under FIDO, OAuth and SCITT in the IETF [O, S]. *Client*, *issuer* and *task* carry protocol meanings there.
