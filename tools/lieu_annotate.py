#!/usr/bin/env python3
"""Annotate a LIEUnn.DLL disassembly for porting to src/levels/.

usage: lieu_annotate.py LIEUnn.DLL > lieunn_ann.dis

Every level DLL exports @FonctionEditor@16 (ecx = op, edx = state, stack:
arg, arg2, callback table). On each call the entry copies the engine's
callback table into DLL globals (mov eax,[ebp+8]; mov eax,[eax+K]; mov
[G],eax) and edx into the state pointer global. The global addresses differ
per DLL, so this script recovers them from the entry and then annotates:

  call dword ptr [G]       ; CB->name            (slot K/4, lieu_cb_t order)
  [reg + off] after reg = state pointer           ; ST(name or 0xOFF)
  immediates that point at C strings              ; "text"
  calls through the e9 thunk block                ; target sub_XXXXXXXX
  indirect jumps / calls through tables           ; table entries listed
  negative immediates                             ; decimal value

The header lists the entry, state pointer, slot map, function starts and
the pointer tables found in .data (the trigger table is the one called
with an index from op 3).
"""
import os
import re
import struct
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)

# lieu_cb_t slot order (src/lieu_cb.h, byte offset = index * 4).
SLOTS = ['animation', 'dialogue', 'evenement', 'cinematique', 'kill_perso',
         'pos_perso', 'etat_objet', 'col_objet', 'joindre', 'change_camera',
         'change_piece', 'bruitage', 'ajoute_zone', 'retire_zone', 'combat',
         'direction_perso', 'direction_zone', 'direction_spec', 'distance',
         'anim_objet', 'bloque', 'fouille', 'stop_joindre', 'vire_col',
         'simult', 'print_msg', 'joindre_obj', 'joindre_perm', 'modify_var',
         'random', 'select_pile', 'on_joindre', 'link_objet', 'place_animal',
         'divers', 'load_anim', 'free_anim', 'play_anim', 'pause_anim',
         'divers_empile']


def load_names():
    """ST offset -> name from lieu_names.h and lieu_vars.h."""
    names = {}
    for f in ('lieu_names.h', 'lieu_vars.h'):
        p = os.path.join(ROOT, 'src', 'levels', f)
        if not os.path.exists(p):
            continue
        for n, v in re.findall(r'#define\s+((?:ST|V)_[A-Z0-9_]+)\s+(0x[0-9A-Fa-f]+)\b', open(p).read()):
            names.setdefault(int(v, 16), n)
    return names


# Shared C ports of functions most DLLs carry unchanged, by their address
# in LIEU01 / LIEU02 (see src/levels/lieu_common.c).
COMMON = {
    (1, 0x1000123A): 'lieu_event4', (2, 0x1000123A): 'lieu_event4',
    (1, 0x100022B4): 'lieu_event5', (2, 0x100022B4): 'lieu_event5',
    (1, 0x10005BEC): 'lieu_event6', (2, 0x10005BEC): 'lieu_event6',
    (1, 0x10001200): 'reset_floats', (2, 0x10001200): 'reset_floats',
    (1, 0x1001932C): 'lieu_action_register', (2, 0x1001A1AC): 'lieu_action_register',
    (1, 0x100193CC): 'pre90_scene', (2, 0x1001A24C): 'pre90_scene',
    (1, 0x100194DC): 'lieu_pre_scene', (2, 0x1001A35C): 'lieu_pre_scene',
    (1, 0x10019B00): 'sprintf', (2, 0x1001A980): 'sprintf',
    (1, 0x10019B70): 'abs', (2, 0x1001A9F0): 'abs',
}


def load_personas():
    p = os.path.join(ROOT, 'src', 'levels', 'lieu_names.h')
    if not os.path.exists(p):
        return {}
    return {int(v): n for n, v in re.findall(r'\b(P_\w+)\s*=\s*(\d+)', open(p).read())}


PERSONA = load_personas()


class PE:
    def __init__(self, path):
        self.d = open(path, 'rb').read()
        pe = struct.unpack_from('<I', self.d, 0x3c)[0]
        nsec = struct.unpack_from('<H', self.d, pe + 6)[0]
        opt = struct.unpack_from('<H', self.d, pe + 20)[0]
        self.base = struct.unpack_from('<I', self.d, pe + 24 + 28)[0]
        exp_rva = struct.unpack_from('<I', self.d, pe + 24 + 96)[0]
        self.secs = []
        o = pe + 24 + opt
        for i in range(nsec):
            name, vs, va, rs, rp = struct.unpack_from('<8sIIII', self.d, o + i * 40)
            self.secs.append((name.rstrip(b'\0').decode(), self.base + va, max(vs, rs), rp, rs))
        # first export (@FonctionEditor@16): AddressOfFunctions[0]
        self.export = None
        if exp_rva:
            b = self.read(self.base + exp_rva + 28, 4)
            if b:
                fa = struct.unpack('<I', b)[0]
                self.export = self.base + struct.unpack('<I', self.read(self.base + fa, 4))[0]

    def sec(self, va):
        for s in self.secs:
            if s[1] <= va < s[1] + s[2]:
                return s
        return None

    def read(self, va, n):
        s = self.sec(va)
        if not s:
            return None
        off = va - s[1]
        if off >= s[4]:
            return b'\0' * n
        return self.d[s[3] + off:s[3] + off + n]

    def cstr(self, va):
        s = self.sec(va)
        if not s or s[0] not in ('.data', '.rdata'):
            return None
        b = self.read(va, 200) or b''
        t = b.split(b'\0')[0]
        if len(t) == 0 and s[0] == '.data' and va % 4 == 0:
            return ''          # empty string ("" arguments, e.g. pos_perso)
        if len(t) < 2 or not all(32 <= c < 127 for c in t):
            return None
        return t.decode()


def disasm(path, tables_text=()):
    od = os.environ.get('OBJDUMP')
    if not od:
        try:
            od = subprocess.check_output(['brew', '--prefix', 'llvm'], text=True).strip() + '/bin/llvm-objdump'
        except Exception:
            od = 'llvm-objdump'
    def run(*extra):
        out = subprocess.check_output([od, '-d', '--no-show-raw-insn', '--x86-asm-syntax=intel',
                                       *extra, path], text=True)
        res = []
        for ln in out.splitlines():
            m = re.match(r'^([0-9a-f]{8}):\s+(.*)$', ln)
            if m:
                t = re.sub(r'\s*<[^>]*>', '', m.group(2)).replace('\t', ' ').strip()
                res.append((int(m.group(1), 16), t))
        return res

    ins = run()
    # Linear sweep decodes a switch table inside .text as code and can
    # swallow the start of the code after it. Re-disassemble from branch
    # targets that land shortly after a table and are not instruction starts.
    for T in sorted(tables_text):
        starts = {a for a, _ in ins}
        targets = sorted({int(m.group(1), 16) for _, t in ins
                          for m in [re.match(r'j[a-z]+ 0x([0-9a-f]+)$', t)] if m
                          and T < int(m.group(1), 16) < T + 512
                          and int(m.group(1), 16) not in starts})
        for v in targets:
            if v in {a for a, _ in ins}:
                continue
            new = run('--start-address=0x%x' % v, '--stop-address=0x%x' % (v + 128))
            have = {a for a, _ in ins}
            keep = []
            for a, t in new:
                if a in have and keep:
                    break
                keep.append((a, t))
            if not keep:
                continue
            end = keep[-1][0] + 1
            prev = max((a for a in have if a < v), default=None)
            ins = [x for x in ins if not (v <= x[0] < end or x[0] == prev)] + keep
            ins.sort()
    return ins


def analyze(path, _tables=None):
    pe = PE(path)
    ins = disasm(path)
    ttab = set()
    for _, t in ins:
        for m in re.finditer(r'(?:\[4\*e[a-z]{2} \+ |byte ptr \[e[a-z]{2} \+ )0x([0-9a-f]{8})\]', t):
            v = int(m.group(1), 16)
            if pe.sec(v) and pe.sec(v)[0] == '.text':
                ttab.add(v)
    if ttab:
        ins = disasm(path, ttab)
    idx = {a: i for i, (a, _) in enumerate(ins)}
    stnames = load_names()

    # e9 thunk block at the start of .text
    thunk = {}
    for a, t in ins[:400]:
        m = re.match(r'jmp 0x([0-9a-f]+)$', t)
        if m and a < pe.base + 0x2000:
            thunk[a] = int(m.group(1), 16)
    ex = pe.export or pe.base + 0x1046
    entry = thunk.get(ex, ex)

    # callback slot globals and state pointer from the entry's copy loop
    slot, state_g = {}, None
    i = idx[entry]
    win = ins[i:i + 600]
    for k in range(len(win) - 1):
        _, t0 = win[k]; _, t1 = win[k + 1]
        m1 = re.match(r'mov (e[a-z]{2}), dword ptr \[(e(?:a|b|c|d)x|esi|edi)(?: \+ 0x([0-9a-f]+))?\]$', t0)
        m2 = re.match(r'mov dword ptr \[0x([0-9a-f]+)\], (e[a-z]{2})$', t1)
        if m1 and m2 and m1.group(1) == m2.group(2):
            slot[int(m2.group(1), 16)] = int(m1.group(3) or '0', 16) // 4
        # unoptimised: mov eax,[ebp+8] / mov eax,[eax+K] / mov [G],eax is
        # matched by the pair above; state = edx stored first
        m3 = re.match(r'mov dword ptr \[0x([0-9a-f]+)\], edx$', t0)
        if m3 and state_g is None:
            state_g = int(m3.group(1), 16)
        if state_g is None and t0 == 'mov eax, dword ptr [ebp - 0x8]':
            m4 = re.match(r'mov dword ptr \[0x([0-9a-f]+)\], eax$', t1)
            if m4:
                state_g = int(m4.group(1), 16)
    if len(slot) > 45 or len(slot) < 30:
        print('warning: %d callback globals found' % len(slot), file=sys.stderr)

    # function starts: thunk targets + direct call targets
    funcs = set(thunk.values())
    for a, t in ins:
        m = re.match(r'call 0x([0-9a-f]+)$', t)
        if m:
            funcs.add(thunk.get(int(m.group(1), 16), int(m.group(1), 16)))
    funcs.add(entry)
    # prologues after padding / ret: functions only reached through tables
    for k in range(1, len(ins) - 1):
        if ins[k][1] == 'push ebp' and ins[k + 1][1] == 'mov ebp, esp' and \
                (ins[k - 1][1] in ('int3', 'ret', 'nop') or ins[k - 1][1].startswith(('ret ', 'jmp '))):
            funcs.add(ins[k][0])

    # pointer tables referenced as [4*reg + T]
    tables = {}
    for a, t in ins:
        for m in re.finditer(r'\[4\*e[a-z]{2} \+ 0x([0-9a-f]+)\]', t):
            T = int(m.group(1), 16)
            s = pe.sec(T)
            if not s or s[0] == '.text' and not t.startswith('jmp'):
                continue
            ents = []
            for j in range(64):
                b = pe.read(T + 4 * j, 4)
                if not b:
                    break
                v = struct.unpack('<I', b)[0]
                if not pe.sec(v) or pe.sec(v)[0] != '.text':
                    break
                ents.append(thunk.get(v, v))
            if ents:
                tables[T] = (t.split()[0], ents)
                if t.startswith('call'):
                    funcs.update(ents)

    # trigger table: op 3 calls [4*arg + TRIG]; INIT fills it with
    # mov dword ptr [TRIG + 4*k], imm
    trig_base, trig = None, {}
    for a, t in ins[idx[entry]:idx[entry] + 600]:
        m = re.match(r'mov eax, dword ptr \[4\*eax \+ 0x([0-9a-f]+)\]$', t)
        if m:
            trig_base = int(m.group(1), 16)
    stores = {}
    for a, t in ins:
        m = re.match(r'mov dword ptr \[0x([0-9a-f]+)\], 0x([0-9a-f]+)$', t)
        if m:
            g, v = int(m.group(1), 16), int(m.group(2), 16)
            if pe.sec(v) and pe.sec(v)[0] == '.text':
                stores[g] = thunk.get(v, v)
    if trig_base:
        for k in range(128):
            if trig_base + 4 * k in stores:
                trig[stores[trig_base + 4 * k]] = trig.get(stores[trig_base + 4 * k], []) + [k]
        funcs.update(trig)

    # jump-table bytes inside .text are data, not code
    skip = set()
    for T, (kind, ents) in tables.items():
        if pe.sec(T)[0] == '.text':
            for j in range(len(ents) * 4):
                skip.add(T + j)
    # switch case-index byte tables (mov cl, byte ptr [eax + T]) follow the
    # jump table: data up to the next function start
    starts = sorted(f for f in funcs if f in idx)
    for a, t in ins:
        m = re.search(r'byte ptr \[e[a-z]{2} \+ 0x([0-9a-f]{8})\]', t)
        if m:
            T = int(m.group(1), 16)
            sc = pe.sec(T)
            if sc and sc[0] == '.text':
                nxt = next((f for f in starts if f > T), T + 256)
                for j in range(T, min(nxt, T + 256)):
                    skip.add(j)

    def fname(v):
        if v == entry:
            return '@FonctionEditor@16'
        if v in trig:
            return 'trig%s_%08X' % ('_'.join('%02d' % k for k in trig[v]), v)
        return 'sub_%08X' % v

    m = re.search(r'LIEU(\d+)\.DLL$', path, re.I)
    level = int(m.group(1)) if m else -1

    def stn(off):
        if off == 0x960 + 4 * level:
            return 'ST_START_POINT(%d)' % level
        if off in stnames:
            return stnames[off]
        if 0x190 < off < 0x190 + 100 * 4 and off % 4 == 0:
            i = (off - 0x190) // 4
            return 'ST_VIE(%s)' % PERSONA.get(i, i)
        return '0x%X' % off

    return dict(pe=pe, ins=ins, idx=idx, thunk=thunk, entry=entry, slot=slot,
                state_g=state_g, funcs=funcs, tables=tables, trig_base=trig_base,
                trig=trig, skip=skip, fname=fname, stn=stn)


def signature(info, f):
    """Instructions of function f with addresses abstracted: callback
    globals become their slot name, the state global S, code targets the
    callee's own signature key is not needed (just 'code'), data 'data'."""
    pe, ins, idx, funcs = info['pe'], info['ins'], info['idx'], info['sorted']
    i = idx[f]
    out = []
    for a, t in ins[i:]:
        if a != f and a in info['funcset']:
            break
        if a in info['skip']:
            continue
        if t == 'int3':
            break
        def rep(m):
            v = int(m.group(1), 16)
            if v in info['slot']:
                return 'CB%d' % info['slot'][v]
            if v == info['state_g']:
                return 'S'
            sc = pe.sec(v)
            if sc and sc[0] == '.text':
                return 'code'
            if sc:
                st = pe.cstr(v)
                return '"%s"' % st if st is not None else 'data'
            return m.group(0)
        out.append(re.sub(r'0x([0-9a-f]{8})', rep, t))
        if len(out) > 20000:
            break
    return tuple(out)


def finish(info):
    ins, idx = info['ins'], info['idx']
    info['funcs'] = {f for f in info['funcs'] if f in idx}
    # a thunk / jump target that starts by using the caller's frame is a
    # fragment inside another function, not a function of its own
    frag = {f for f in info['funcs']
            if '[ebp ' in ins[idx[f]][1] and not ins[idx[f]][1].startswith('push')}
    info['funcs'] -= frag
    info['sorted'] = sorted(info['funcs'])
    info['funcset'] = set(info['funcs'])
    return info


def port_names():
    """(level, address) -> C function noted in the ports' comments."""
    names = {}
    d = os.path.join(ROOT, 'src', 'levels')
    for f in sorted(os.listdir(d)):
        m = re.match(r'lieu(\d\d)(?:_trig)?\.c$', f)
        if not m:
            continue
        lv = int(m.group(1))
        for ln in open(os.path.join(d, f)):
            mm = re.search(r'^(?:static\s+)?[a-z0-9_ *]+?\b(\w+)\s*\([^;]*\)\s*\{.*?sub_(1000[0-9A-Fa-f]{4})', ln)
            if mm:
                names[(lv, int(mm.group(2), 16))] = mm.group(1)
    return names


def main():
    path = sys.argv[1]
    info = finish(analyze(path))
    pe, ins, idx, thunk, entry, slot = (info[k] for k in ('pe', 'ins', 'idx', 'thunk', 'entry', 'slot'))
    state_g, funcs, tables, trig_base, trig, skip, fname, stn = (
        info[k] for k in ('state_g', 'funcs', 'tables', 'trig_base', 'trig', 'skip', 'fname', 'stn'))
    stnames = load_names()

    # functions identical to already-ported levels (same code modulo layout)
    same = {}
    refs = []
    for lv in (1, 2):
        rp = os.path.join(os.path.dirname(os.path.abspath(path)), 'LIEU%02d.DLL' % lv)
        if os.path.exists(rp) and os.path.abspath(rp) != os.path.abspath(path):
            refs.append((lv, finish(analyze(rp))))
    pn = port_names()
    sigs = {}
    for lv, ri in refs:
        for f in ri['sorted']:
            sg = signature(ri, f)
            if len(sg) >= 8:
                sigs.setdefault(sg, []).append((lv, f, ri['fname'](f)))
    for f in info['sorted']:
        sg = signature(info, f)
        if sg in sigs:
            same[f] = sigs[sg]
    common = {}
    for f, hits in same.items():
        for lv, ra, _ in hits:
            if (lv, ra) in COMMON:
                common[f] = COMMON[(lv, ra)]
    base_fname = fname

    def fname(v):
        return common[v] if v in common else base_fname(v)

    # header
    print('; %s  image base 0x%08X' % (os.path.basename(path), pe.base))
    print('; entry @FonctionEditor@16 = 0x%08X, state pointer global = %s' %
          (entry, '0x%08X' % state_g if state_g else '?'))
    print('; callback globals (engine table slot -> lieu_cb_t member):')
    for g, k in sorted(slot.items(), key=lambda x: x[1]):
        print(';   [0x%08X] slot %2d  CB->%s' % (g, k, SLOTS[k] if k < len(SLOTS) else 'slot%d' % k))
    print('; pointer tables:')
    for T, (kind, ents) in sorted(tables.items()):
        print(';   0x%08X (%s, %d entries): %s' % (T, kind, len(ents), ' '.join('%X' % e for e in ents)))
    # functions to port: not identical to a ported level, below the shared
    # library code (which starts at action_register in every DLL seen)
    lib = min([f for f, n in common.items() if n == 'lieu_action_register'] or [1 << 32])
    order = info['sorted']
    callers = {}
    for a, t in ins:
        m = re.match(r'call 0x([0-9a-f]+)$', t)
        if m:
            v = int(m.group(1), 16)
            callers.setdefault(thunk.get(v, v), 0)
            callers[thunk.get(v, v)] += 1
    print('; functions to port (level-specific; size in bytes, calls from code), plus the entry\'s INIT tail:')
    for i, f in enumerate(order):
        if f in same or f == entry:
            continue
        end = order[i + 1] if i + 1 < len(order) else f
        j = idx[f]
        while j < len(ins) and ins[j][0] < end and ins[j][1] != 'int3':
            j += 1
        end = ins[j][0] if j < len(ins) else end      # stop at int3 padding
        if end - f < 8:
            continue           # jump target inside a shared function
        body = [t for a, t in ins[idx[f]:] if a < end]
        script = any(('dword ptr [0x%08x]' % g) in t for t in body for g in list(slot) + [state_g or 0])
        print(';   %-24s %6d bytes  %d calls%s' % (fname(f), end - f, callers.get(f, 0),
                                                 '' if script else '  (no CB/ST: C runtime, skip)'))
    if same:
        print('; functions identical to an already-ported level (reuse the C port):')
        rest = 0
        for f in sorted(same):
            if f not in common and f not in trig:
                rest += 1
                continue
            first = {}
            for lv, ra, n in same[f]:
                first.setdefault(lv, (ra, n))
            hits = ', '.join('L%02d %s%s' % (lv, n, (' = ' + pn[(lv, ra)] + '()') if (lv, ra) in pn else '')
                             for lv, (ra, n) in sorted(first.items())[:1])
            print(';   %s == %s' % (fname(f), hits))
        if rest:
            print(';   + %d more (C runtime: sprintf, abs, ...)' % rest)
    if trig_base:
        print('; trigger table 0x%08X (op 3: call [4*arg + table], filled by INIT):' % trig_base)
        for v, ks in sorted(trig.items(), key=lambda x: x[1][0]):
            print(';   %s -> 0x%08X' % (','.join(str(k) for k in ks), v))
    print('; %d functions' % len(funcs))
    print()

    regs = {}   # register -> 'S' when it holds the state pointer
    for a, t in ins:
        if a in skip or t == 'int3':
            continue
        if a in funcs:
            print('\n%s:%s' % (fname(a), ('    ; == ' + ', '.join(sorted({'L%02d %s' % (lv, n) for lv, _, n in same[a]})[:2])) if a in same else ''))
            regs = {}
        if a in thunk:
            print('%08x: %s    ; thunk -> %s' % (a, t, fname(thunk[a])))
            continue
        c = []
        m = re.match(r'call dword ptr \[0x([0-9a-f]+)\]$', t)
        if m and int(m.group(1), 16) in slot:
            k = slot[int(m.group(1), 16)]
            c.append('CB->' + (SLOTS[k] if k < len(SLOTS) else 'slot%d' % k))
        m = re.match(r'call 0x([0-9a-f]+)$', t)
        if m:
            v = int(m.group(1), 16)
            c.append(fname(thunk.get(v, v)))
        # state-pointer tracking
        for r, off in re.findall(r'\[(e[a-z]{2}) \+ 0x([0-9a-f]+)\]', t):
            if regs.get(r) == 'S':
                c.append('ST(%s)' % stn(int(off, 16)))
        for r in re.findall(r'\[(e[a-z]{2})\]', t):
            if regs.get(r) == 'S':
                c.append('ST(0x0)')
        if state_g:
            t2 = t.replace('dword ptr [0x%08x]' % state_g, 'S')
        else:
            t2 = t
        ma = re.match(r'add (e[a-z]{2}), 0x([0-9a-f]+)$', t)
        if ma and regs.get(ma.group(1)) == 'S':
            c.append('&ST(%s)' % stn(int(ma.group(2), 16)))
        md = re.match(r'(?:mov|lea|add|sub|xor|and|or|imul|movsx|movzx|pop|inc|dec) (e[a-z]{2})\b(.*)$', t)
        if md:
            r = md.group(1)
            if t.startswith('mov') and state_g and md.group(2).strip() == ', dword ptr [0x%08x]' % state_g:
                regs[r] = 'S'
            else:
                regs.pop(r, None)
        if t.startswith('call'):
            for r in ('eax', 'ecx', 'edx'):
                regs.pop(r, None)
        if t.startswith(('jmp', 'ret')):
            pass
        # strings, negative immediates, tables
        for h in re.findall(r'0x([0-9a-f]{8})\b', re.sub(r'\[[^\]]*\]', '', t)):
            v = int(h, 16)
            s = pe.cstr(v)
            if s is not None:
                c.append('"%s"' % s)
            elif v >= 0xFFFF0000:
                c.append(str(v - (1 << 32)))
        for m in re.finditer(r'\[4\*e[a-z]{2} \+ 0x([0-9a-f]+)\]', t):
            T = int(m.group(1), 16)
            if T in tables:
                c.append('table %X' % T)
        print('%08x: %s%s' % (a, t2, ('    ; ' + ' '.join(c)) if c else ''))


if __name__ == '__main__':
    main()
