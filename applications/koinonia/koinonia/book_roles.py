"""The founding steward and the terms of the proposed additional book steward."""

import hashlib
import json

STEWARD = {"id": "author/claude-code", "provider": "anthropic", "model": "claude-opus-5-5", "agent": "Claude Code"}
STEWARD_KINDS = ["present", "request", "accept", "release", "admit", "answer", "propose", "vote", "deposit", "modify", "report", "recognise",
                 "attest", "consult", "use", "say"]

GPT_STEWARD = 'author/codex-steward'
GPT_MODEL = 'gpt-6.2-sol'
BOOK_STEWARD_KINDS = ['present', 'consult', 'answer', 'attest', 'request', 'accept', 'release', 'report', 'say']


def appointment_proposal(constitution, founding_seal):
    def digest(value):
        canonical = json.dumps(value, sort_keys=True, ensure_ascii=False, separators=(',', ':')).encode('utf-8')
        return hashlib.sha256(canonical).hexdigest()

    terms = {'community': 'completion', 'principal': 'author', 'office': 'steward', 'operative': GPT_STEWARD,
             'issuer': 'author-household', 'provider': 'openai', 'model': GPT_MODEL, 'kinds': sorted(BOOK_STEWARD_KINDS),
             'matters': ['the book'], 'communities': ['completion'], 'action': 'admit an additional steward',
             'existing_offices': 'retain', 'review_rule': 'another provider than the writer',
             'founding_seal': founding_seal, 'constitution_sha256': digest(constitution)}
    return terms, digest(terms)
