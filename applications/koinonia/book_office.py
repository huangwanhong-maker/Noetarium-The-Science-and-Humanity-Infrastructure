"""Run one existing Anthropic book operative through its own passport and the community API.

    python applications/koinonia/book_office.py claude-code --check
    python applications/koinonia/book_office.py claude-code --max-usd 0.75
    python applications/koinonia/book_office.py claude-review --max-usd 0.50

The default is a local preflight. --max-usd starts one Claude Code session, with its built-in tools disabled.
Each invocation records its cap, outcome, Claude-reported usage and the community's own act numbers under
data/cache/koinonia-book-offices. A reviewer starts only when its eligible work is available.
"""

import argparse
from contextlib import contextmanager
from datetime import datetime, timezone
from decimal import Decimal
import http.client
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
import time
import uuid

import book_agents
from koinonia.dispatch import client as book_transport
from koinonia.book_roles import STEWARD_KINDS


ROOT = book_transport.DEFAULT_RUN
IDENTITY = re.compile(r'claude-(?:code|review|[0-9]{3})')


def now():
    return datetime.now(timezone.utc).isoformat(timespec='seconds')


def public(path):
    client = http.client.HTTPConnection('127.0.0.1', 9230, timeout=60)
    try:
        client.request('GET', '/c/completion/' + path)
        response = client.getresponse()
        if response.status != 200:
            raise ValueError(f'The local community returned HTTP {response.status}.')
        return json.loads(response.read())
    finally:
        client.close()


def review_work(work, operative):
    items = {item['id']: item for item in work}
    relevant = []
    for item in work:
        if item['kind'] != 'review':
            continue
        if item['state'] == 'taken' and any(t.get('taker') == operative for t in item.get('takings', [])):
            relevant.append(item['id'])
        elif item['state'] == 'open':
            deliveries = items.get(item.get('of'), {}).get('deliveries', [])
            provider = deliveries[-1].get('provider') if deliveries else None
            if provider and provider != 'anthropic':
                relevant.append(item['id'])
    return relevant


def prepare(identity):
    if not IDENTITY.fullmatch(identity):
        raise ValueError('Select an existing Anthropic steward or reviewer identity.')
    transport = book_transport.Community(identity, ROOT)
    if identity == 'claude-code':
        missing = sorted(set(STEWARD_KINDS) - set(transport.passport['mandate']['kinds']))
        if missing:
            raise ValueError('The issued steward passport lacks ' + ', '.join(missing) + '. Use the household issuer to repair it.')
    path = book_agents.settings() / identity / 'prompt.txt'
    original = path.read_text(encoding='utf-8-sig')
    prompt = original.replace(transport.bearer, '[supplied by the local community transport]')
    if 'http://127.0.0.1:9230/c/platform-league/registry/registry' not in prompt:
        raise ValueError('The issued prompt names a different discovery node.')
    return transport.passport['mandate']['id'], prompt


def executable():
    program = shutil.which('claude.exe' if os.name == 'nt' else 'claude')
    if program:
        return program
    shim = shutil.which('claude.cmd') if os.name == 'nt' else None
    if shim:
        binary = Path(shim).parent / 'node_modules' / '@anthropic-ai' / 'claude-code' / 'bin' / 'claude.exe'
        if binary.is_file():
            return str(binary)
    raise ValueError('The native Claude Code executable could not be located.')


@contextmanager
def exclusive(name):
    ROOT.mkdir(parents=True, exist_ok=True)
    with (ROOT / (name + '.lock')).open('a+b') as f:
        f.seek(0)
        try:
            if os.name == 'nt':
                import msvcrt
                msvcrt.locking(f.fileno(), msvcrt.LK_NBLCK, 1)
            else:
                import fcntl
                fcntl.flock(f, fcntl.LOCK_EX | fcntl.LOCK_NB)
        except OSError as err:
            raise ValueError(f'Another book office process holds the {name} lock.') from err
        yield


def remaining():
    budget = json.loads((book_agents.settings().parent / 'koinonia-budget.json').read_text(encoding='utf-8-sig'), parse_float=Decimal)
    left = Decimal(budget['stop_at_usd']) - Decimal(budget['spent_usd'])
    for path in ROOT.glob('run-*.json'):
        previous = json.loads(path.read_bytes(), parse_float=Decimal)
        amount = previous.get('total_cost_usd')
        left -= Decimal(str(previous['max_usd'] if amount is None else amount))
    return left


def record(path, state):
    temporary = path.with_suffix('.tmp')
    temporary.write_text(json.dumps(state, ensure_ascii=False, indent=2), encoding='utf-8')
    temporary.replace(path)


def run(identity, model, cap, mandate, prompt):
    binary = executable()
    # Require the subscribed first-party account, with no API-key or other provider fallback.
    env = {k: v for k, v in os.environ.items() if not k.upper().startswith(('ANTHROPIC_', 'CLAUDE_CODE_USE_'))}
    auth = subprocess.run([binary, 'auth', 'status', '--json'], capture_output=True, env=env, timeout=35)
    account = json.loads(auth.stdout or b'{}')
    if auth.returncode or not account.get('loggedIn') or account.get('authMethod') != 'claude.ai' or account.get('apiProvider') != 'firstParty':
        raise ValueError('The first-party Claude subscription is unavailable.')
    with exclusive(identity), tempfile.TemporaryDirectory(prefix='koinonia-office-') as temporary:
        before = public('head')['head']['size']
        if identity != 'claude-code' and not review_work(public('work')['work'], 'author/' + identity):
            print('The community has no eligible review for this operative. Model usage: 0.')
            return
        cwd = Path(temporary)
        config = cwd / 'community.json'
        config.write_text(json.dumps({'mcpServers': {'book': {'command': sys.executable, 'args': ['-u', '-B', book_transport.__file__, identity,
                                                                                                  '--directory', str(ROOT)]}}}), encoding='utf-8')
        instructions = ('Use only the two book community tools. They supply your own passport, authorization, and the target '
                'provider for consultation. Pass a URL path; presenting sends an empty body. GET @last with a JSON pointer '
                'reads another part of the last reply. Use the community for evidence and questions. Check actual bytes and '
                'the section standard before attesting. A digest identifies an artefact; unsupported checks and verdicts '
                'must remain pending. The other operative uses its own passport. Report exact IDs of your admitted acts.')
        command = [binary, '-p', '--no-session-persistence', '--output-format', 'json', '--model', model,
                   '--effort', 'medium', '--max-budget-usd', str(cap), '--permission-mode', 'dontAsk', '--tools', '',
                   '--strict-mcp-config', '--mcp-config', str(config), '--allowedTools', 'mcp__book__community_read',
                   'mcp__book__community_act', '--setting-sources', '', '--no-chrome', '--append-system-prompt', instructions]
        status = ROOT / ('run-' + uuid.uuid4().hex + '.json')
        state = {'identity': identity, 'mandate': mandate, 'requested_model': model, 'max_usd': str(cap), 'started': now(), 'state': 'running'}
        with exclusive('budget'):
            left = remaining()
            if cap > left:
                raise ValueError(f'The requested cap ${cap} exceeds the remaining project allowance ${left}.')
            record(status, state)
        print('Run record: ' + str(status), flush=True)
        proc = subprocess.run(command, input=prompt.encode(), capture_output=True, cwd=cwd, env=env)
        try:
            outcome = json.loads(proc.stdout or b'{}')
        except ValueError:
            outcome = {}
        state.update(ended=now(), state='finished' if proc.returncode == 0 and outcome.get('type') == 'result' and not outcome.get('is_error') else 'failed',
                     exit_code=proc.returncode, subtype=outcome.get('subtype'), models=list(outcome.get('modelUsage') or {}),
                     total_cost_usd=outcome.get('total_cost_usd'), report=str(outcome.get('result') or '')[:4000])
        record(status, state)
        acts = public(f'acts?after={before}')['acts']
        state['acts'] = [{'n': a['n'], 'kind': a['kind'], 'seal': a['seal']} for a in acts if a.get('author') == 'author/' + identity]
        record(status, state)
        print(json.dumps({k: state[k] for k in ('identity','state','models','total_cost_usd','acts')}))
        if state['state'] != 'finished':
            raise SystemExit(1)


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument('identity')
    parser.add_argument('--model', default='sonnet')
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument('--check', action='store_true')
    mode.add_argument('--max-usd', type=Decimal)
    parser.add_argument('--wait-minutes', type=int, default=0,
                        help='For a reviewer, watch the public work board for this many minutes before a single capped run.')
    a = parser.parse_args()
    if a.wait_minutes < 0 or a.wait_minutes and (a.identity == 'claude-code' or a.max_usd is None):
        parser.error('--wait-minutes requires a reviewer and a spending cap.')
    if a.max_usd is not None and (not a.max_usd.is_finite() or a.max_usd <= 0):
        parser.error('Choose a finite, positive dollar cap.')
    mandate, prompt = prepare(a.identity)
    eligible = review_work(public('work')['work'], 'author/' + a.identity) if a.identity != 'claude-code' else []
    deadline = time.monotonic() + a.wait_minutes * 60
    while a.wait_minutes and not eligible and deadline > time.monotonic():
        time.sleep(min(30, max(0, deadline - time.monotonic())))
        eligible = review_work(public('work')['work'], 'author/' + a.identity)
    if a.max_usd is None:
        print(json.dumps({'identity': a.identity, 'mandate': mandate, 'eligible_review_items': eligible, 'remaining_usd': str(remaining())}))
    elif a.identity != 'claude-code' and not eligible:
        print('The community has no eligible review for this operative. Model usage: 0.')
    else:
        run(a.identity, a.model, a.max_usd, mandate, prompt)


if __name__ == '__main__':
    try:
        main()
    except (ValueError, OSError, KeyError, subprocess.TimeoutExpired) as err:
        raise SystemExit(str(err))
