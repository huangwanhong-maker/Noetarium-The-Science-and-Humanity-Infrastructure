"""Check session ownership independently of saved wait files and sealed book acts."""
import json
import os
from pathlib import Path
import sys
from tempfile import TemporaryDirectory

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from book_dashboard import build, host_alive, session


def check(name, ok):
    print(json.dumps({'kind': 'assert', 'name': name, 'ok': bool(ok)}))
    assert ok, name


def write(path, value):
    path.write_text(json.dumps(value), encoding='utf-8')


with TemporaryDirectory() as folder:
    run = Path(folder)
    waits = run / 'wait-state'
    waits.mkdir()
    people = ['writer-a', 'writer-b', 'writer-c']
    write(run / 'roster.json', [{'agent': n, 'role':'writer', 'model':'fixture', 'effort':'low'} for n in people])
    write(run / 'cohort-status.json', {'pid':os.getpid(),'agents':{n:{'state':'completed'} for n in people}})
    write(run / 'recovery-status.json', {'pid':os.getpid(),'agents':{
        'writer-b':{'state':'waiting','restored_wait':True}, 'writer-c':{'state':'working','thread_id':'fixture'}}})
    for name in people[:2]:
        write(waits / (name + '.json'), {'pending':True,'arguments':{'until':'item_change','item':'question-a'}})
    node = run / 'node.json'
    write(node, {'record':[{'kind':'deliver','author':'author/writer-a','fields':{'item':'section-1'}}]})
    result = build(run, node)
    a,b,c = result['agents']
    check('an ended turn with a stale wait stays ended', a['runtime']=='Turn ended' and a['unownedWait'])
    check('restored wait belongs to a live new host before a model turn starts', b['runtime']=='Waiting' and not b['unownedWait'])
    check('delivery is independent from session ownership', a['delivered'] and c['runtime']=='Running' and not c['delivered'])
    check('the summary separates live waits, stale waits, running agents and contributors',
          all(result['counts'][k]==1 for k in ['waiting','unownedWaits','running','delivered']))
    check('offline owner is never presented as waiting', session({'state':'waiting'},{'pending':True},False)==('Host offline',True))
    check('a running host can be verified without signalling the process', host_alive(os.getpid()) and not host_alive(None))
