"""Read-only loopback monitor of the Koinonia book cohort."""

import argparse
from collections import Counter, deque
import ctypes
from datetime import datetime, timedelta, timezone
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
import json
import os
from pathlib import Path
import re
from threading import Lock
import time
from urllib.parse import urlsplit


APP = Path(__file__).resolve().parent
CACHE = APP.parents[1] / 'data' / 'cache' / 'koinonia'
JST = timezone(timedelta(hours=9))
GENERIC = {'preparing':'Preparing', 'starting-turn':'Starting', 'dispatched':'Connecting', 'working':'Running',
           'waiting':'Waiting', 'completed':'Turn ended', 'interrupted':'Interrupted', 'failed':'Failed',
           'launch-failed':'Launch failed', 'native-dispatched':'Dispatched', 'parked':'Waiting'}
ACTIONS = {'present':'Passport presented','apply':'Application recorded','commit':'Item claimed','deliver':'Delivered',
           'review':'Reviewed','leave':'Left','consult':'Consulted holding','question':'Question filed','say':'Message posted',
           'deposit':'Deposited holding','report':'Report filed','vote':'Voted','petition':'Petition filed','use':'Used tool'}
LOCK = Lock()
CACHED = None
UPDATED = 0


def load(path, fallback=None):
    try:
        return json.loads(path.read_bytes())
    except FileNotFoundError:
        if fallback is None:
            raise
        return fallback


def safe_text(value):
    text = str(value or '')[:5000]
    text = re.sub(r'(?i)(?:authorization\s*:\s*)?bearer\s+\S+', '[credential withheld]', text)
    return re.sub(r'[A-Za-z0-9_+/=-]{32,}', '[value withheld]', text)


def stage(row):
    if row['left']:
        return 'Left'
    if row['reviewed']:
        return 'Reviewed'
    if row['delivered']:
        return 'Delivered'
    if row['claimed']:
        return 'Item claimed'
    if row['application']:
        return 'Application ' + row['application']
    if row['presented']:
        return 'Passport presented'
    if row['registry']:
        return 'Registry reached'
    return 'Connecting'


def read_io(run_dir, row):
    log = run_dir / 'io-log' / (row['id'] + '.jsonl')
    if not log.exists():
        return
    for line in log.read_bytes().splitlines():
        try:
            entry = json.loads(line)
        except ValueError:
            continue
        path = entry.get('path') or ''
        status = entry.get('status')
        if path.endswith('/registry/registry') and status == 200 and entry.get('directContact', True):
            row['registry'] = True
        if entry.get('kind') == 'apply' and entry.get('state') in ('admitted','declined','open','accepted','pending','approved'):
            row['application'] = entry['state']
        if path:
            row['lastRequest'] = {'path': safe_text(path), 'status': status, 'at': entry.get('at')}


def host_alive(pid):
    if not isinstance(pid, int) or pid <= 0:
        return False
    if os.name == 'nt':
        from ctypes import wintypes
        api = ctypes.WinDLL('kernel32', use_last_error=True)
        api.OpenProcess.argtypes = [wintypes.DWORD, wintypes.BOOL, wintypes.DWORD]
        api.OpenProcess.restype = wintypes.HANDLE
        api.GetExitCodeProcess.argtypes = [wintypes.HANDLE, ctypes.POINTER(wintypes.DWORD)]
        api.CloseHandle.argtypes = [wintypes.HANDLE]
        handle = api.OpenProcess(0x1000, False, pid)
        if not handle:
            return False
        try:
            code = wintypes.DWORD()
            return bool(api.GetExitCodeProcess(handle, ctypes.byref(code))) and code.value == 259
        finally:
            api.CloseHandle(handle)
    try:
        os.kill(pid, 0)
    except ProcessLookupError:
        return False
    except PermissionError:
        return True
    return True


def session(run, wait, live):
    state = run.get('state', '')
    label = GENERIC.get(state, state.replace('-', ' ').capitalize() or 'Unreported')
    if state in {'working', 'dispatched', 'starting-turn', 'reconnecting', 'waiting', 'parked'} and not live:
        label = 'Host offline'
    pending = bool(wait.get('pending'))
    owned = live and state in {'waiting', 'parked'}
    return label, pending and not owned


def build(run_dir, node_file):
    roster = load(run_dir / 'roster.json')
    initial = load(run_dir / 'cohort-status.json', {})
    recovery = load(run_dir / 'recovery-status.json', {})
    native = load(run_dir / 'native-status.json', {})
    live = {host.get('pid'):host_alive(host.get('pid')) for host in (initial, recovery)}
    rows = {}
    for person in roster:
        name = person['agent']
        newer = recovery.get('agents', {}).get(name, {})
        reconnect = bool(newer.get('restored_wait') or newer.get('thread_id') or newer.get('turn_id') or newer.get('dispatched_at'))
        host = recovery if reconnect else initial
        run = host.get('agents', {}).get(name, {})
        direct = native.get(name, {})
        if direct and not reconnect:
            run = direct
        wait = load(run_dir / 'wait-state' / (name + '.json'), {})
        runtime, unowned = session(run, wait, live.get(host.get('pid'), False))
        row = {'id':name,'role':person['role'],'model':person['model'],'effort':person['effort'],'runtime':runtime,
               'reconnected':reconnect,'registry':bool(direct.get('registry')),'presented':False,'claimed':False,'delivered':False,
               'reviewed':False,'left':False,'application':'','item':'','report':safe_text(run.get('report')),
               'waitCondition':wait.get('arguments',{}) if wait.get('pending') else {}, 'unownedWait':unowned,
               'lastRequest':None,'acts':deque(maxlen=12),'actCount':0}
        read_io(run_dir, row)
        rows[name] = row
    node = load(node_file)
    for index, act in enumerate(node['record'], start=1):
        author = act.get('author','')
        if not author.startswith('author/'):
            continue
        row = rows.get(author[7:])
        if row is None:
            continue
        kind = act.get('kind','')
        fields = act.get('fields',{})
        if kind == 'assess' and fields.get('criterion') == 'review':
            kind = 'review'
        item = fields.get('item') or (fields.get('object') if kind == 'review' else '') or ''
        if kind in ('deliver','review') and not item:
            item = (fields.get('id') or '').split('~')[0]
        if isinstance(item,str) and re.fullmatch(r'[\w~. -]{1,90}',item):
            row['item'] = item
        if kind == 'present':
            row['presented'] = True
        elif kind == 'apply' and not row['application']:
            row['application'] = 'recorded'
        elif kind == 'commit':
            row['claimed'] = True
        elif kind == 'deliver':
            row['delivered'] = True
        elif kind == 'review':
            row['reviewed'] = True
        elif kind == 'leave':
            row['left'] = True
        row['actCount'] += 1
        row['acts'].append({'n':index,'at':act.get('when'), 'kind':ACTIONS.get(kind,kind.capitalize()),'item':item if isinstance(item,str) and len(item) < 90 else ''})
    for name,row in rows.items():
        row['stage'] = stage(row)
        row['acts'] = list(reversed(row['acts']))
    counts = {'total':len(rows),'running':sum(r['runtime']=='Running' for r in rows.values()),'waiting':sum(r['runtime']=='Waiting' for r in rows.values()),
              'presented':sum(r['presented'] for r in rows.values()),'delivered':sum(r['delivered'] for r in rows.values()),
              'registry':sum(r['registry'] for r in rows.values()), 'unownedWaits':sum(r['unownedWait'] for r in rows.values()),
              'lastActions': dict(Counter(r['acts'][0]['kind'] if r['acts'] else 'No recorded act' for r in rows.values()))}
    return {'asOf':datetime.now(JST).isoformat(timespec='seconds'),'counts':counts,'agents':list(rows.values())}


def snapshot(run_dir, node_file):
    global CACHED, UPDATED
    with LOCK:
        if CACHED is None or time.monotonic() - UPDATED >= 5:
            value = build(run_dir, node_file)
            CACHED = json.dumps(value, ensure_ascii=False).encode()
            UPDATED = time.monotonic()
        return CACHED


class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        path = urlsplit(self.path).path
        if path in ('/','/index.html'):
            self.serve((APP/'book-dashboard.html').read_bytes(), 'text/html; charset=utf-8')
        elif path == '/api/status':
            try:
                self.serve(snapshot(self.server.run_dir, self.server.node_file), 'application/json; charset=utf-8')
            except (OSError, ValueError, KeyError):
                self.send_error(503, 'Snapshot temporarily unavailable')
        else:
            self.send_error(404)

    def serve(self, body, mime):
        self.send_response(200)
        self.send_header('Content-Type', mime)
        self.send_header('Content-Length',str(len(body)))
        self.send_header('Cache-Control','no-store')
        self.send_header('X-Content-Type-Options','nosniff')
        self.end_headers()
        self.wfile.write(body)

    def log_message(self, *args):
        pass


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('run_dir', type=Path, help='existing dispatcher run directory')
    parser.add_argument('--node-store', type=Path, default=CACHE / 'node')
    parser.add_argument('--port', type=int, default=9234)
    args = parser.parse_args()
    node_file = args.node_store / 'communities' / 'completion' / 'hosted.json'
    build(args.run_dir, node_file)
    with ThreadingHTTPServer(('127.0.0.1', args.port), Handler) as server:
        server.run_dir, server.node_file = args.run_dir, node_file
        server.serve_forever()
