"""Split the IDA 7 dump into one .c and one .asm file per function and write index.json."""
import json, os, re, sys
d = sys.argv[1]
os.makedirs(d + '/c', exist_ok=True); os.makedirs(d + '/asm', exist_ok=True)
idx = {}
c = open(d + '/dkev.c', encoding='latin-1').read()
parts = re.split(r'^//----- \(([0-9A-F]{8})\) -+\n', c, flags=re.M)
for i in range(1, len(parts), 2):
    a, body = parts[i], parts[i + 1]
    m = re.search(r'^[^/#\n][^\n]*?([A-Za-z_][\w@?$.]*)\(', body, re.M)
    n = m.group(1) if m else 'f'
    idx[a] = {'name': n, 'c_lines': body.count('\n')}
    open(f'{d}/c/{a}_{n}.c', 'w').write(f'//----- ({a}) {n}\n' + body)
cur, buf = None, []
def flush():
    if cur: open(f'{d}/asm/{cur[0]}_{cur[1]}.asm', 'w').write(''.join(buf))
for line in open(d + '/dkev.lst', encoding='latin-1'):
    m = re.match(r'\.text:([0-9A-F]{8}) (\S+)\s+proc\s+near', line)
    if m:
        flush(); cur, buf = (m.group(1), m.group(2)), []
        idx.setdefault(m.group(1), {'name': m.group(2), 'c_lines': 0})
    if cur: buf.append(line)
    if cur and re.match(r'\.text:[0-9A-F]{8} \S+\s+endp', line): flush(); cur = None
flush()
json.dump(idx, open(d + '/index.json', 'w'), indent=0)
print(len(idx), 'functions')
