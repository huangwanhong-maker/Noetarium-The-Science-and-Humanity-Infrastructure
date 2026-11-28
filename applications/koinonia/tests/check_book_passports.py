"""Exercise the issuer with temporary credential files and a recording household."""

import json
from decimal import Decimal
from pathlib import Path
from subprocess import CompletedProcess
import sys
import tempfile
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import book_agents
import book_office
from koinonia.book_roles import STEWARD_KINDS


def check(name, holds):
    print(json.dumps({"kind": "assert", "name": "koinonia book passports: " + name, "ok": bool(holds)}))
    assert holds, name


def household(url, method, body, token):
    return 201, {"admitted": True, "passport": {"mandate": {"kinds": list(body.get("kinds", ["apply"]))}},
                 "credential": "fixture-only"}


with tempfile.TemporaryDirectory() as temp, patch.object(book_agents, 'principal', return_value='fixture') as principal, \
        patch.object(book_agents, 'call', side_effect=household) as calls:
    directory = Path(temp)
    agents = [(name, *book_agents.AGENTS[name]) for name in ('claude-code', 'claude-review')]
    book_agents.issue('example/', book_agents.HOME, agents, False, directory)
    bodies = [c.args[2] for c in calls.call_args_list]
    check("the exact designated steward receives the founding act scope", bodies[0]['kinds'] == STEWARD_KINDS)
    check("a reviewer keeps the ordinary household mandate", 'kinds' not in bodies[1])
    calls.reset_mock()
    principal.reset_mock()
    book_agents.issue('example/', book_agents.HOME, agents, False, directory)
    check("a valid existing passport is left in place", not calls.called and not principal.called)
    steward = directory / 'claude-code' / 'passport.json'
    steward.write_text(json.dumps({'mandate': {'kinds': ['consult', 'attest']}}), encoding='utf-8')
    try:
        book_agents.issue('example/', book_agents.HOME, agents, False, directory)
    except SystemExit as err:
        check("a legacy contributor mandate is diagnosed without an issuance", 'claude-code --again' in str(err) and not calls.called and not principal.called)
    else:
        raise AssertionError('legacy steward silently skipped')
    book_agents.issue('example/', book_agents.HOME, agents[:1], False, directory, again=True)
    check("a deliberate repair issues only to the same designated steward", calls.call_count == 1 and
          calls.call_args.args[2]['operative'] == 'author/claude-code' and json.loads(steward.read_bytes())['mandate']['kinds'] == STEWARD_KINDS)
    archive = list((steward.parent / 'previous').iterdir())
    check("the previous local credential pair is retained during renewal", len(archive) == 1 and
          json.loads((archive[0] / 'passport.json').read_bytes())['mandate']['kinds'] == ['consult', 'attest'] and
          (archive[0] / 'prompt.txt').is_file())
    calls.reset_mock()
    calls.side_effect = TimeoutError()
    try:
        book_agents.issue('example/', book_agents.HOME, agents[:1], False, directory, again=True)
    except SystemExit as err:
        check("ambiguous steward issuance is never replayed automatically", calls.call_count == 1 and 'unknown outcome' in str(err))
    else:
        raise AssertionError('ambiguous issuance silently retried')


def cli(command, **kwargs):
    if command[1:3] == ['auth', 'status']:
        response = {'loggedIn': True, 'authMethod': 'claude.ai', 'apiProvider': 'firstParty'}
    else:
        response = {'type': 'result', 'total_cost_usd': 0.2, 'result': 'Fixture question answered.', 'modelUsage': {'test-sonnet': {}}}
        config = json.loads(Path(command[command.index('--mcp-config') + 1]).read_bytes())
        check("the session exposes only the identity-bound book MCP and disables built-in tools",
              list(config['mcpServers']) == ['book'] and 'claude-code' in config['mcpServers']['book']['args'] and
              '--strict-mcp-config' in command and command[command.index('--tools') + 1] == '' and
              command[command.index('--permission-mode') + 1] == 'dontAsk')
        check("the approved cap reaches the CLI", command[command.index('--max-budget-usd') + 1] == '0.50')
    return CompletedProcess(command, 0, stdout=json.dumps(response).encode(), stderr=b'')


with tempfile.TemporaryDirectory() as temp:
    directory = Path(temp)
    (directory / 'koinonia-budget.json').write_text(json.dumps({'stop_at_usd':1, 'spent_usd':0.1}), encoding='utf-8')
    private = directory / 'identities'
    run_dir = directory / 'runs'
    with patch.object(book_office, 'ROOT', run_dir), patch.object(book_agents, 'settings', return_value=private), \
            patch.object(book_office, 'executable', return_value='fixture'), patch.object(book_office.subprocess, 'run', side_effect=cli) as process, \
            patch.object(book_office, 'public', side_effect=lambda path: {'head':{'size':2}} if path == 'head' else
                         {'acts':[{'n':3,'kind':'answer','seal':'fixture','author':'author/claude-code'},
                                  {'n':4,'kind':'say','seal':'another','author':'author/unrelated'}]}):
        book_office.run('claude-code', 'sonnet', Decimal('0.50'), 'fixture', 'fixture instructions')
        record = json.loads(next(run_dir.glob('run-*.json')).read_bytes())
        check("the run records reported usage, actual model and only the operative's admitted acts", record['total_cost_usd'] == 0.2 and
              record['models'] == ['test-sonnet'] and [a['n'] for a in record['acts']] == [3])
        (run_dir / 'run-interrupted.json').write_text(json.dumps({'max_usd':'0.40','state':'running'}), encoding='utf-8')
        check("an interrupted session reserves its full cap", book_office.remaining() == Decimal('0.30'))
        process.reset_mock()
        try:
            book_office.run('claude-code', 'sonnet', Decimal('0.31'), 'fixture', 'fixture instructions')
        except ValueError as exc:
            check("a new model call cannot exceed the remaining cap", 'exceeds' in str(exc) and
                  all(c.args[0][1] == 'auth' for c in process.call_args_list))
        else:
            raise AssertionError('an over-budget call ran')

reviews = [{'id':'s','kind':'section','state':'delivered','deliveries':[{'provider':'openai'}]},
           {'id':'r','kind':'review','state':'open','of':'s'}]
check("a reviewer cannot run without eligible work", not book_office.review_work(reviews[:1], 'author/claude-review'))
check("an open review of the other provider is eligible", book_office.review_work(reviews, 'author/claude-review') == ['r'])
reviews[0]['deliveries'][0]['provider'] = 'anthropic'
check("an open review of the same provider stays closed to the office", not book_office.review_work(reviews, 'author/claude-review'))
