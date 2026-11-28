"""Run a resumed agent in its own Codex thread with its issued passport and original model."""

import asyncio
import collections
import json
import os
from pathlib import Path
import shutil
import subprocess
import re
import time

from ..dispatch_waits import now
from .transport import DYNAMIC_TOOLS, DirectIO

BEARER = re.compile(r'Authorization: Bearer ([A-Za-z0-9._~-]+)')


def save(path, data):
    tmp = path.with_suffix('.tmp')
    tmp.write_text(json.dumps(data, ensure_ascii=False), encoding='utf-8')
    # On Windows a dashboard reader may briefly hold the old snapshot open.
    for attempt in range(6):
        try:
            tmp.replace(path)
            return
        except PermissionError:
            if attempt == 5:
                raise
            time.sleep(0.03 * (attempt + 1))


class Cohort:
    def __init__(self, directory):
        self.directory = Path(directory)
        roster = json.loads((self.directory / 'roster.json').read_bytes())
        self.people = {p['agent']: p for p in roster}
        self.ids = {}
        self.state = {}
        self.pending = {}
        self.number = 0
        self.jobs = set()
        self.io = DirectIO(self.directory)
        self.dirty = True
        self.guard = asyncio.Semaphore(10)
        self.status_file = self.directory / 'recovery-status.json'
        self.events = (self.directory / 'recovery-events.jsonl').open('a', encoding='utf-8', buffering=1)

    def mark(self, name, **info):
        self.state[name].update(info)
        self.events.write(json.dumps({'at': now(), 'agent': name, **info}, ensure_ascii=False) + '\n')
        self.dirty = True

    def snapshot(self):
        if self.dirty:
            save(self.status_file, {'at': now(), 'pid': os.getpid(),
                 'counts': dict(collections.Counter(v['state'] for v in self.state.values())), 'agents': self.state})
            self.dirty = False

    def send(self, msg):
        self.proc.stdin.write((json.dumps(msg, ensure_ascii=False) + '\n').encode())

    async def request(self, method, params):
        self.number += 1
        i = self.number
        fut = asyncio.get_running_loop().create_future()
        self.pending[i] = fut
        self.send({'id': i, 'method': method, 'params': params})
        await self.proc.stdin.drain()
        return await fut

    async def handle(self, msg):
        method = msg['method']
        p = msg.get('params', {})
        name = self.ids.get(p.get('threadId'))
        if 'id' in msg:
            if method != 'item/tool/call' or not name:
                self.send({'id': msg['id'], 'error': {'code': -32601, 'message': 'Only the issued community tools are available.'}})
                return
            tool = p['tool']
            if self.state[name]['state'] in ('starting-turn', 'dispatched'):
                self.mark(name, state='working', first_tool=tool)
            if tool == 'wait_for_community':
                self.mark(name, state='waiting', waiting_since=now())
            try:
                result = await self.io.call(name, tool, p.get('arguments') or {})
            except Exception as exc:
                result = {'success': False, 'contentItems': [{'type': 'inputText', 'text': 'Community transport: ' + str(exc)[:250]}]}
            self.send({'id': msg['id'], 'result': result})
            if tool == 'wait_for_community':
                self.mark(name, state='working')
            return
        if not name:
            return
        if method == 'item/completed':
            item = p['item']
            if item.get('type') == 'agentMessage' and item.get('text'):
                self.state[name]['report'] = BEARER.sub('[private]', item['text'])[-4000:]
                self.dirty = True
        elif method == 'turn/completed':
            turn = p['turn']
            self.mark(name, state=turn['status'], ended=now(), error=turn.get('error'))
        elif method == 'error' and not p.get('willRetry'):
            self.mark(name, provider_error=p.get('error', {}).get('message', '')[:400])

    async def read_messages(self):
        while line := await self.proc.stdout.readline():
            msg = json.loads(line)
            if 'method' not in msg:
                f = self.pending.pop(msg.get('id'), None)
                if f and not f.done():
                    if 'error' in msg:
                        f.set_exception(RuntimeError(str(msg['error'])[:1500]))
                    else:
                        f.set_result(msg.get('result'))
            else:
                t = asyncio.create_task(self.handle(msg))
                self.jobs.add(t)
                t.add_done_callback(self.jobs.discard)
        for f in self.pending.values():
            if not f.done():
                f.set_exception(RuntimeError('Codex app-server disconnected'))

    async def start(self, name, prior=''):
        person = self.people[name]
        settings = Path(os.environ.get('APPDATA') or Path.home() / '.config')
        original = (settings / 'noetarium' / 'koinonia-agents' / name / 'prompt.txt').read_text(encoding='utf-8-sig')
        if len(BEARER.findall(original)) != 1:
            raise ValueError('Invalid issued prompt: ' + name)
        msg = BEARER.sub('Authorization: Bearer [supplied locally by your community tools]', original)
        if prior:
            msg += '\n\nReconnection under the same passport. ' + prior
        instructions = ('You are an independent agent working with your peers in the same community on the same book. '
             'Coordinate with your peers using the community routes and rules. Carry out your own attached mandate. '
             'Use community_read and community_act as your HTTP transport. They supply your own passport and credentials. '
             'For a pending transport receipt, use await_community_request with its request_id; do not send a duplicate act. '
             'A parked wait means end this turn and the scheduler will wake you with the result. '
             'Pass URL paths; presenting takes an empty body. You can use @last with a JSON pointer to inspect a response. '
             'To wait for eligible work, use wait_for_community, which keeps you connected while other agents work. '
             'Preserve source attribution and be explicit about missing evidence. Take your record and leave when your '
             'issued instructions and community rules call for it. Use the community tools for all actions and communication.')
        async with self.guard:
            try:
                thread = await self.request('thread/start', {'model': person['model'], 'allowProviderModelFallback': False,
                         'cwd': str(self.directory / 'workdir'), 'environments': [], 'sandbox': 'read-only', 'approvalPolicy': 'never',
                         'ephemeral': True, 'dynamicTools': DYNAMIC_TOOLS, 'selectedCapabilityRoots': [],
                         'developerInstructions': instructions, 'config': {'model_reasoning_effort': person['effort']}})
                if thread['model'] != person['model']:
                    raise ValueError('Unexpected assigned model: ' + thread['model'])
                tid = thread['thread']['id']
                self.ids[tid] = name
                self.mark(name, state='starting-turn', thread_id=tid, resolved_model=thread['model'])
                turn = await self.request('turn/start', {'threadId': tid, 'model': person['model'], 'effort': person['effort'],
                           'input': [{'type': 'text', 'text': msg, 'text_elements': []}]})
                status = self.state[name]['state']
                self.mark(name, state='dispatched' if status == 'starting-turn' else status,
                          turn_id=turn['turn']['id'], dispatched_at=now())
            except Exception as exc:
                self.mark(name, state='launch-failed', error=str(exc)[:900])

    async def boot(self):
        options = {'features.apps': False, 'features.plugins': False, 'features.hooks': False,
                   'features.multi_agent': False, 'features.shell_tool': False, 'features.browser_use': False,
                   'features.computer_use': False, 'features.image_generation': False, 'web_search': 'disabled',
                   'project_doc_max_bytes': 0}
        cmd = [shutil.which('codex') or 'codex', 'app-server', '--stdio']
        for key, value in options.items():
            cmd.extend(['-c', key + '=' + json.dumps(value)])
        (self.directory / 'workdir').mkdir(exist_ok=True)
        err = (self.directory / 'host.log').open('ab', buffering=0)
        self.proc = await asyncio.create_subprocess_exec(*cmd, cwd=self.directory / 'workdir', stdin=subprocess.PIPE,
                        stdout=subprocess.PIPE, stderr=err, limit=20*1024*1024, creationflags=getattr(subprocess, 'CREATE_NO_WINDOW', 0))
        reader = asyncio.create_task(self.read_messages())
        await self.request('initialize', {'clientInfo': {'name':'koinonia-book','title':'Koinonia Book','version':'1.0'},
            'capabilities': {'experimentalApi':True,'requestAttestation':False}})
        self.send({'method':'initialized','params':{}})
        return reader
