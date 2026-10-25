#!/usr/bin/env python3
"""Differential test of a level port (src/levels/lieuNN*.c) against its DLL.

usage: lieu_difftest.py NN [target...] [-n BUDGET] [-j JOBS] [-v]

Emulates the original x86 code of bin/game/LIEUNN.DLL (from the listing
tools/lieu_annotate.py prints) and runs the C port on the same inputs. Both
sides call mock engine callbacks; the sequence of callback calls with their
arguments, the final state block and the entry's return value must match.

What is run, per level, through the exported entry @FonctionEditor@16:
  init     op 1, flag 0/1 (INIT: arming, trigger 0 or the death ending)
  tick     op -1 (pending event, then the level's per-frame body)
  trigNN   op 3, arg NN, for every slot of the trigger table
Functions called from these (helpers, tick body, action_register callers)
are checked through their callers: the emulator follows every call.

Inputs are random state blocks and callback return values, biased to the
constants the code compares against, then steered by branch coverage: when
a conditional jump is only seen one way, the compared state slot or callback
result is set so that the other way is taken.

How the two sides are made comparable
  * No engine: the C port is linked against tools/lieu_difftest_harness.c,
    which mocks lieu_cb_t (tracing each call). For ops -1/1/3 lieu_common.c
    is NOT linked: lieu_action_register, lieu_pre_scene and event4/5/6 are
    traced stubs, and the emulator treats calls to the DLL's copies of them
    the same way. For ops 4/5/6 (targets ev4/ev5/ev6) a second harness links
    the real lieu_common.c and the emulator runs the DLL's own handlers, each
    case in a forked child so the DLL globals start fresh. A level's own
    lNN_reset_dll_globals() (if it defines one) runs before every case.
  * Callbacks that return a value draw it from a per-case pool, indexed by
    hash(call text) + count of identical earlier calls. The result depends
    on what was called, not on when, so the order of independent calls does
    not change the draw.
  * Runs of side-effect-free queries (direction_*, distance, on_joindre and
    the pure Divers ops) are sorted before comparing: the original
    evaluates the arguments of a call such as pos_perso(..., Divers(DV_DESC_POS),
    ...) in its own order, which the C port is free to change.
  * strings built with sprintf are formatted by the emulator (%d %c %s).

Limits: integer code only (plus the x87 forms of the float accumulators); teardown is not run; DLL globals other than the state pointer are not
compared; branches whose condition is computed (not a plain compare of a
state slot or callback result) are only reached by chance.

Exit status 1 if any target has a mismatch or an emulator error.
"""
import argparse
import bisect
import collections
import concurrent.futures
import multiprocessing
import os
import random
import re
import struct
import subprocess
import sys
import tempfile
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, HERE)
import lieu_annotate as ann  # noqa: E402

BASE = 0x50000000          # where the emulated state block lives
NST = 0x1000               # bytes compared / randomised
STACK = 0x7ff00000
SENT = 0xDEAD0000          # return address of the emulated entry
CBTAB = 0x60000000         # fake engine callback table (INIT copies from it)
POOL_N = 1021

M32 = 0xFFFFFFFF


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def fnv(s):
    h = 0xcbf29ce484222325
    for c in s.encode('latin-1', 'replace'):
        h ^= c
        h = (h * 0x100000001b3) & 0xFFFFFFFFFFFFFFFF
    return h


# ---------------------------------------------------------------------------
# callback signatures, from src/lieu_cb.h

def parse_cb_header():
    t = open(os.path.join(ROOT, 'src', 'lieu_cb.h')).read()
    i = t.index('typedef struct {')
    j = t.index('} lieu_cb_t;')
    body = re.sub(r'/\*.*?\*/', '', t[i:j], flags=re.S)
    cbs = collections.OrderedDict()
    for m in re.finditer(r'([\w\s\*]+?)\(\*(\w+)\)\(([^)]*)\)\s*;', body):
        rt = 'v' if m.group(1).strip() == 'void' else 'i'
        kinds = ''
        for p in m.group(3).split(','):
            p = p.strip()
            if p == 'void' or not p:
                continue
            kinds += 's' if 'c8' in p else ('p' if '*' in p else 'i')
        cbs[m.group(2)] = (kinds, rt)
    return cbs


# stubs of lieu_common.c: name -> kinds (args after the first two are on the
# stack and popped by the callee)
SHARED = {'lieu_action_register': 'sisS', 'lieu_pre_scene': 'iiiii'}

# Divers ops that only read (DV_DESC_POS, CAN_FIGHT, ANGLE_TO, IN_ZONE,
# WAS_HIT, PERSONA_PRESENT, STRUCT_DIST, PLAYER_ZONE, COMBAT_NEAR)
PURE_DIVERS = {1, 2, 8, 9, 18, 29, 31, 32, 36}
PURE_CB = {'direction_perso', 'direction_zone', 'direction_spec', 'distance', 'on_joindre'}


def gen_cb_c(cbs):
    out = []
    ct = {'i': 'i32', 's': 'const c8 *', 'p': 'i32 *'}
    for name, (kinds, rt) in cbs.items():
        params = ', '.join('%s a%d' % (ct[k], i) for i, k in enumerate(kinds)) or 'void'
        fmt = ','.join({'i': '%d', 's': '%s', 'p': '%s'}[k] for k in kinds)
        vals = ''.join(', ' + {'i': 'a%d', 's': 'S_(a%d)',
                               'p': 'P_(a%d)'}[k] % i
                       for i, k in enumerate(kinds))
        body = '    KEY("%s(%s)"%s); OUT("%%s", kb);\n' % (name, fmt, vals)
        if name == 'modify_var':
            body += '    *a0 = a1;\n'
        rtype = 'void'
        if rt == 'i':
            rtype = 'i32'
            if name == 'random':
                body += '    { u32 n = (u32)a0; i32 v = draw(kb); return n ? (i32)((u32)v % n) : 0; }\n'
            elif name == 'divers':
                body += '    { i32 v = draw(kb); g_last_divers = v; return v; }\n'
            else:
                body += '    return draw(kb);\n'
        out.append('static %s cb_%s(%s) {\n%s}\n' % (rtype, name, params, body))
    out.append('static const lieu_cb_t g_cb = {\n')
    for name in cbs:
        out.append('    .%s = cb_%s,\n' % (name, name))
    out.append('};\n')
    return ''.join(out)


# ---------------------------------------------------------------------------
# the annotated listing

class Listing:
    def __init__(self, text, pe):
        self.pe = pe
        self.ins = {}            # addr -> (instruction text, comment)
        self.label = {}          # addr -> function label
        self.slots = {}          # callback global -> name
        self.trig = {}           # trigger index -> function address
        self.trig_base = None
        self.tables = {}         # jump table address -> [target addresses]
        self.state_g = None
        self.entry = None
        order = []
        in_trig = False
        for ln in text.split('\n'):
            if ln.startswith(';'):
                m = re.search(r'entry @FonctionEditor@16 = 0x([0-9A-Fa-f]+), state pointer global = 0x([0-9A-Fa-f]+)', ln)
                if m:
                    self.entry = int(m.group(1), 16)
                    self.state_g = int(m.group(2), 16)
                m = re.match(r';\s+\[0x([0-9A-Fa-f]+)\] slot\s+\d+\s+CB->(\w+)', ln)
                if m:
                    self.slots[int(m.group(1), 16)] = m.group(2)
                m = re.match(r';\s+0x([0-9A-Fa-f]+) \(jmp, \d+ entries\):\s*(.*)$', ln)
                if m:
                    self.tables[int(m.group(1), 16)] = [int(x, 16) for x in m.group(2).split()]
                m = re.match(r'; trigger table 0x([0-9A-Fa-f]+)', ln)
                if m:
                    self.trig_base = int(m.group(1), 16)
                    in_trig = True
                    continue
                if in_trig:
                    m = re.match(r';\s+([\d,]+) -> 0x([0-9A-Fa-f]+)', ln)
                    if m:
                        for k in m.group(1).split(','):
                            self.trig[int(k)] = int(m.group(2), 16)
                    else:
                        in_trig = False
                continue
            m = re.match(r'^([0-9a-f]{8}): (.*)$', ln)
            if m:
                a = int(m.group(1), 16)
                ins, _, cmt = m.group(2).partition(';')
                self.ins[a] = (ins.strip(), cmt.strip())
                order.append(a)
                continue
            m = re.match(r'^([A-Za-z_@][^:\s]*):', ln)
            if m:
                self.label[len(order)] = m.group(1)
        self.addrs = order
        self.next = {order[i]: order[i + 1] for i in range(len(order) - 1)}
        # function starts: label -> address of its first instruction
        self.fstart = {}
        for idx, name in self.label.items():
            if idx < len(order):
                self.fstart[order[idx]] = name
        self.fsorted = sorted(self.fstart)

    def func_of(self, addr):
        i = bisect.bisect_right(self.fsorted, addr) - 1
        return self.fsorted[i] if i >= 0 else None

    def func_end(self, start):
        i = bisect.bisect_right(self.fsorted, start)
        return self.fsorted[i] if i < len(self.fsorted) else max(self.ins) + 1

    def resolve(self, addr):
        """Follow the e9 thunk block."""
        for _ in range(4):
            ins, cmt = self.ins.get(addr, ('', ''))
            if cmt.startswith('thunk ->'):
                addr = int(ins.split()[1], 16)
            else:
                break
        return addr

    def callees(self, f, ev=False):
        out = set()
        for a in range(f, self.func_end(f)):
            if a not in self.ins:
                continue
            ins, cmt = self.ins[a]
            if ins.startswith('call 0x'):
                if cmt in ('sprintf', 'abs') or (not ev and (cmt in SHARED or cmt.startswith('lieu_'))):
                    continue
                out.add(self.resolve(int(ins.split()[1], 16)))
        return out

    def closure(self, f, ev=False):
        seen = set()
        todo = [f]
        while todo:
            x = todo.pop()
            if x in seen or x not in self.ins:
                continue
            seen.add(x)
            todo.extend(self.callees(x, ev))
        return seen


# ---------------------------------------------------------------------------
# the emulator

REG32 = ['eax', 'ecx', 'edx', 'ebx', 'esp', 'ebp', 'esi', 'edi']
RIDX = {n: i for i, n in enumerate(REG32)}
R8 = {'al': (0, 0), 'cl': (1, 0), 'dl': (2, 0), 'bl': (3, 0),
      'ah': (0, 8), 'ch': (1, 8), 'dh': (2, 8), 'bh': (3, 8)}
R16 = {'ax': 0, 'cx': 1, 'dx': 2, 'bx': 3, 'sp': 4, 'bp': 5, 'si': 6, 'di': 7}
JCC = {'je', 'jne', 'jl', 'jge', 'jle', 'jg', 'ja', 'jae', 'jb', 'jbe', 'js', 'jns'}


class EmuError(Exception):
    pass


def cond(mn, a, b):
    ua, ub = a & M32, b & M32
    return {'je': a == b, 'jne': a != b, 'jl': a < b, 'jge': a >= b, 'jle': a <= b,
            'jg': a > b, 'ja': ua > ub, 'jae': ua >= ub, 'jb': ua < ub, 'jbe': ua <= ub,
            'js': s32(a - b) < 0, 'jns': s32(a - b) >= 0}[mn]


class Ctx:
    """Static per-DLL data shared by all emulations."""

    def __init__(self, L, cbs, ev=False):
        self.ev = ev             # run the DLL's shared code instead of stubbing it
        self.L = L
        self.cbs = cbs
        self.code = {}
        self.pe = L.pe
        self.dcache = {}

    def pe_dword(self, a):
        v = self.dcache.get(a)
        if v is None:
            b = self.pe.read(a, 4)
            v = s32(struct.unpack('<I', b)[0]) if b and len(b) == 4 else 0
            self.dcache[a] = v
        return v

    def cstr(self, a):
        s = self.pe.sec(a)
        if not s or s[0] not in ('.data', '.rdata'):
            return None
        b = self.pe.read(a, 256) or b''
        return b.split(b'\0')[0].decode('latin-1')


class Operand:
    """get(e) / set(e, v) / tag(e) for one decoded operand."""
    pass


def parse_ea(s, ctx):
    """'[eax + 4*ecx - 0x10]' -> function e -> address"""
    inner = s[s.index('[') + 1:s.rindex(']')].strip()
    toks = re.findall(r'([+-]?)\s*([^+\-\s][^+\-]*?)(?=\s*(?:[+-]|$))', inner)
    regs, disp = [], 0
    for sign, t in toks:
        t = t.strip()
        sg = -1 if sign == '-' else 1
        m = re.match(r'^(\d)\*(e..)$', t)
        if m:
            regs.append((RIDX[m.group(2)], int(m.group(1)) * sg))
        elif t in RIDX:
            regs.append((RIDX[t], sg))
        else:
            disp += sg * int(t, 0)
    if len(regs) == 0:
        return lambda e: disp & M32
    if len(regs) == 1 and regs[0][1] == 1:
        r0 = regs[0][0]
        return lambda e: (e.r[r0] + disp) & M32
    return lambda e: (disp + sum(e.r[r] * m for r, m in regs)) & M32


def make_operand(s, ctx):
    s = s.strip()
    o = Operand()
    o.mem = False
    o.size = 4
    L = ctx.L
    if s == 'S':
        s = 'dword ptr [0x%08x]' % L.state_g
    sz = 4
    for pre, n in (('dword ptr ', 4), ('word ptr ', 2), ('byte ptr ', 1)):
        if s.startswith(pre):
            s = s[len(pre):]
            sz = n
    o.size = sz
    if s.startswith('['):
        ea = parse_ea(s, ctx)
        o.mem = True
        o.ea = ea
        if sz == 4:
            o.get = lambda e, ea=ea: e.rd(ea(e))
            o.set = lambda e, v, ea=ea: e.wr(ea(e), v)
            o.tag = lambda e, ea=ea: e.mtag(ea(e))
        else:
            mask = (1 << (8 * sz)) - 1
            def g(e, ea=ea, mask=mask):
                a = ea(e)
                return (e.rd(a & ~3) >> (8 * (a & 3))) & mask
            def st(e, v, ea=ea, mask=mask):
                a = ea(e)
                sh = 8 * (a & 3)
                old = e.rd(a & ~3) & M32
                e.wr(a & ~3, (old & ~(mask << sh)) | ((v & mask) << sh))
            o.get = g
            o.set = st
            o.tag = lambda e: None
        return o
    if s in RIDX:
        i = RIDX[s]
        o.get = lambda e, i=i: e.r[i]
        def sr(e, v, i=i):
            e.r[i] = s32(v)
            e.rt[i] = None
        o.set = sr
        o.tag = lambda e, i=i: e.rt[i]
        o.ea = None
        return o
    if s in R8:
        i, sh = R8[s]
        o.size = 1
        o.get = lambda e, i=i, sh=sh: (e.r[i] >> sh) & 0xFF
        def s8(e, v, i=i, sh=sh):
            e.r[i] = s32((e.r[i] & ~(0xFF << sh)) | ((v & 0xFF) << sh))
            e.rt[i] = None
        o.set = s8
        o.tag = lambda e: None
        return o
    if s in R16:
        i = R16[s]
        o.size = 2
        o.get = lambda e, i=i: e.r[i] & 0xFFFF
        def s16(e, v, i=i):
            e.r[i] = s32((e.r[i] & ~0xFFFF) | (v & 0xFFFF))
            e.rt[i] = None
        o.set = s16
        o.tag = lambda e: None
        return o
    v = s32(int(s, 0))
    o.get = lambda e, v=v: v
    o.set = None
    o.tag = lambda e: None
    o.imm = v
    return o


def tag_add(t, k, sign=1):
    """tag of (sign * value + k)"""
    if t is None:
        return None
    return (t[0], t[1], t[2] * sign, t[3] * sign + k)


class Emu:
    def __init__(self, ctx, case, op, arg, arg2):
        self.ctx = ctx
        self.L = ctx.L
        self.r = [0] * 8
        self.rt = [None] * 8
        self.mem = {}
        self.mt = {}
        self.st = list(case.st)
        self.st0 = case.st
        self.stt = {}          # tags of written state slots
        self.stw = set()
        self.stread = {}
        self.pool = case.pool
        self.cnt = collections.Counter()
        self.trace = []
        self.tpc = []
        self.pool_used = set()
        self.strmem = {}
        self.fl = ('c', 0, 0, None, None)
        self.cov = set()
        self.events = []
        self.funcs = set()
        self.steps = 0
        self.op, self.arg, self.arg2 = op, arg, arg2
        self.fpu = []          # x87 stack, top last (Python floats)
        self.fsw = 0

    # memory
    def rd(self, a):
        a &= M32
        if BASE <= a < BASE + NST:
            if a & 3:
                raise EmuError('unaligned state read %x' % a)
            off = a - BASE
            self.stread.setdefault(off, self.st[off >> 2])
            return self.st[off >> 2]
        v = self.mem.get(a)
        if v is None:
            v = self.ctx.pe_dword(a)
        return v

    def wr(self, a, v):
        a &= M32
        if BASE <= a < BASE + NST:
            if a & 3:
                raise EmuError('unaligned state write %x' % a)
            off = a - BASE
            self.st[off >> 2] = s32(v)
            self.stw.add(off)
            self.stt.pop(off, None)
            return
        self.mem[a] = s32(v)
        self.mt.pop(a, None)

    def mtag(self, a):
        a &= M32
        if BASE <= a < BASE + NST:
            off = a - BASE
            if off in self.stw:
                return self.stt.get(off)
            return ('st', off, 1, 0)
        return self.mt.get(a)

    def settag(self, a, t):
        a &= M32
        if BASE <= a < BASE + NST:
            off = a - BASE
            if t is None:
                self.stt.pop(off, None)
            else:
                self.stt[off] = t
        elif t is None:
            self.mt.pop(a, None)
        else:
            self.mt[a] = t

    # strings
    def strval(self, a):
        a &= M32
        if a == 0:
            return None
        if a in self.strmem:
            return self.strmem[a]
        s = self.ctx.cstr(a)
        return s if s is not None else '?%x' % a

    def fmtarg(self, k, v):
        if k == 'i':
            return str(s32(v))
        if k == 'p':
            if BASE <= (v & M32) < BASE + NST:
                return '&ST(%d)' % ((v & M32) - BASE)
            return '&DLL-global'       # a pointer to the DLL's own .data
        s = self.strval(v)
        return '(null)' if s is None else '"%s"' % s

    # callbacks
    def do_cb(self, name, kinds, rets, pc=0):
        r = self.r
        n = len(kinds)
        args = []
        for i in range(n):
            if i == 0:
                args.append(r[1])
            elif i == 1:
                args.append(r[2])
            else:
                args.append(self.rd(r[4] + 4 * (i - 2)))
        kk = kinds.replace('S', 's')
        txt = '%s(%s)' % (name, ','.join(self.fmtarg(k, v) for k, v in zip(kk, args)))
        r[4] = s32(r[4] + 4 * max(0, n - 2))
        self.trace.append(txt)
        self.tpc.append(pc)
        if name == 'modify_var':
            self.wr(args[0], args[1])
        r[0] = 0
        self.rt[0] = None
        if rets:
            c = self.cnt[txt]
            self.cnt[txt] += 1
            idx = (fnv(txt) + c * 2654435761) % (1 << 64) % len(self.pool)
            self.pool_used.add(idx)
            v = self.pool[idx]
            mod = 0
            if name == 'random':
                nn = args[0] & M32
                v = (v & M32) % nn if nn else 0
                mod = nn
            r[0] = s32(v)
            self.rt[0] = ('ret', idx, 1, 0)

    def sprintf(self):
        r = self.r
        sp = r[4]
        buf = self.rd(sp)
        fmt = self.strval(self.rd(sp + 4)) or ''
        args = [self.rd(sp + 8 + 4 * i) for i in range(8)]
        out, ai = '', 0
        for m in re.finditer(r'%([-+ #0]*\d*(?:\.\d+)?)([diucsxX%])|([^%]+)', fmt):
            if m.group(3):
                out += m.group(3)
                continue
            f, c = m.group(1), m.group(2)
            if c == '%':
                out += '%'
                continue
            v = args[ai]
            ai += 1
            if c == 'c':
                out += chr(v & 0xFF)
            elif c == 's':
                out += ('%' + f + 's') % (self.strval(v) or '(null)')
            elif c == 'u':
                out += ('%' + f + 'd') % (v & M32)
            elif c in 'xX':
                out += ('%' + f + c) % (v & M32)
            else:
                out += ('%' + f + 'd') % s32(v)
        out = out.split('\0')[0]
        self.strmem[buf & M32] = out
        r[0] = len(out)
        self.rt[0] = None

    # running
    def run(self, entry, maxsteps=400000):
        code = self.ctx.code
        pc = entry
        L = self.L
        while pc != SENT:
            f = code.get(pc)
            if f is None:
                f = code[pc] = decode(self.ctx, pc)
            self.steps += 1
            if self.steps > maxsteps:
                raise EmuError('step limit')
            pc = f(self)


def decode(ctx, pc):
    L = ctx.L
    if pc not in L.ins:
        # the listing leaves out the shared epilogue the switch cases jump to
        # (event4/5: pop edi/esi/ebx; leave; ret, between data tables)
        b = L.pe.read(pc, 8) or b''
        m = re.match(rb'^(?:[\x5f\x5e\x5b])*\xc9(?:\xc3|\xc2(..))', b, re.S)
        if not m:
            raise EmuError('jump to %08x outside the listing' % pc)
        n = struct.unpack('<H', m.group(1))[0] if m.group(1) else 0
        def f(e):
            e.r[4] = e.r[5]
            e.r[5] = e.rd(e.r[4])
            e.rt[5] = None
            ret = e.rd(e.r[4] + 4) & M32
            e.r[4] = s32(e.r[4] + 8 + n)
            return ret
        return f
    ins, cmt = L.ins[pc]
    nxt = L.next.get(pc, pc)
    parts = ins.split(None, 1)
    mn = parts[0]
    ops = [x.strip() for x in parts[1].split(',')] if len(parts) > 1 else []
    O = lambda i: make_operand(ops[i], ctx)
    cbs = ctx.cbs

    def bad():
        raise NotImplementedError('%08x: %s' % (pc, ins))

    if mn == 'push':
        a = O(0)
        def f(e):
            sp = s32(e.r[4] - 4)
            e.r[4] = sp
            e.wr(sp, a.get(e))
            e.settag(sp, a.tag(e))
            return nxt
        return f
    if mn == 'pop':
        a = O(0)
        def f(e):
            sp = e.r[4]
            v = e.rd(sp)
            t = e.mtag(sp)
            e.r[4] = s32(sp + 4)
            a.set(e, v)
            if ops[0] in RIDX:
                e.rt[RIDX[ops[0]]] = t
            return nxt
        return f
    if mn == 'mov':
        d, s = O(0), O(1)
        isreg = ops[0] in RIDX
        di = RIDX.get(ops[0])
        def f(e):
            v = s.get(e)
            t = s.tag(e)
            d.set(e, v)
            if isreg:
                e.rt[di] = t
            elif d.mem and d.size == 4:
                e.settag(d.ea(e), t)
            return nxt
        return f
    if mn in ('movzx', 'movsx'):
        d, s = O(0), O(1)
        sx = mn == 'movsx'
        def f(e):
            v = s.get(e)
            if sx:
                v = v - (1 << (8 * s.size)) if v & (1 << (8 * s.size - 1)) else v
            d.set(e, v)
            return nxt
        return f
    if mn == 'lea':
        d = O(0)
        ea = parse_ea(ops[1], ctx)
        # lea reg, [reg2 + k] keeps the tag of reg2
        m = re.match(r'^\[(e..)\s*([+-])\s*(0x[0-9a-f]+)\]$', ops[1])
        def f(e):
            v = ea(e)
            d.set(e, v)
            if m:
                e.rt[RIDX[ops[0]]] = tag_add(e.rt[RIDX[m.group(1)]], int(m.group(3), 16) * (1 if m.group(2) == '+' else -1))
            return nxt
        return f
    if mn in ('add', 'sub', 'and', 'or', 'xor', 'cmp', 'test', 'adc', 'sbb'):
        a, b = O(0), O(1)
        isreg = ops[0] in RIDX
        zero = mn == 'xor' and ops[0] == ops[1]
        immb = getattr(b, 'imm', None)
        def f(e):
            x, y = a.get(e), b.get(e)
            if mn == 'cmp':
                e.fl = ('c', x, y, a.tag(e), b.tag(e))
                return nxt
            if mn == 'test':
                if ops[0] == ops[1]:
                    e.fl = ('c', x, 0, a.tag(e), None)
                else:
                    e.fl = ('c', s32(x & y), 0, None, None)
                return nxt
            t = None
            if mn == 'add':
                r = x + y
                if immb is not None:
                    t = tag_add(a.tag(e), immb)
            elif mn == 'sub':
                r = x - y
                if immb is not None:
                    t = tag_add(a.tag(e), -immb)
            elif mn == 'and':
                r = x & y
            elif mn == 'or':
                r = x | y
            elif mn == 'xor':
                r = 0 if zero else x ^ y
            elif mn == 'adc' or mn == 'sbb':
                raise NotImplementedError(ins)
            r = s32(r)
            a.set(e, r)
            if isreg:
                e.rt[RIDX[ops[0]]] = t
            elif a.mem and t is not None and a.size == 4:
                e.settag(a.ea(e), t)
            if mn == 'sub':
                e.fl = ('c', x, y, None, None)
            else:
                e.fl = ('c', r, 0, None, None)
            return nxt
        return f
    if mn in ('inc', 'dec', 'neg', 'not'):
        a = O(0)
        isreg = ops[0] in RIDX
        def f(e):
            x = a.get(e)
            t = a.tag(e)
            if mn == 'inc':
                r, t = x + 1, tag_add(t, 1)
            elif mn == 'dec':
                r, t = x - 1, tag_add(t, -1)
            elif mn == 'neg':
                r, t = -x, tag_add(t, 0, -1)
            else:
                r, t = ~x, None
            r = s32(r)
            a.set(e, r)
            if isreg:
                e.rt[RIDX[ops[0]]] = t
            elif a.mem and a.size == 4:
                e.settag(a.ea(e), t)
            if mn != 'not':
                e.fl = ('c', r, 0, None, None)
            return nxt
        return f
    if mn in ('shl', 'shr', 'sar'):
        a, b = O(0), O(1)
        def f(e):
            x, n = a.get(e), b.get(e) & 31
            if mn == 'shl':
                r = x << n
            elif mn == 'shr':
                r = (x & M32) >> n
            else:
                r = x >> n
            a.set(e, s32(r))
            e.fl = ('c', s32(r), 0, None, None)
            return nxt
        return f
    if mn == 'imul':
        if len(ops) == 2:
            a, b = O(0), O(1)
            def f(e):
                a.set(e, s32(a.get(e) * b.get(e)))
                return nxt
        elif len(ops) == 3:
            a, b, c = O(0), O(1), O(2)
            def f(e):
                a.set(e, s32(b.get(e) * c.get(e)))
                return nxt
        else:
            bad()
        return f
    if mn == 'cdq':
        def f(e):
            e.r[2] = -1 if e.r[0] < 0 else 0
            e.rt[2] = None
            return nxt
        return f
    if mn in ('idiv', 'div'):
        a = O(0)
        def f(e):
            d = a.get(e)
            if mn == 'div':
                n = ((e.r[2] & M32) << 32) | (e.r[0] & M32)
                d &= M32
            else:
                n = (e.r[2] << 32) | (e.r[0] & M32)
            if d == 0:
                raise EmuError('division by zero')
            q = abs(n) // abs(d)
            if (n < 0) != (d < 0) and mn == 'idiv':
                q = -q
            e.r[0] = s32(q)
            e.r[2] = s32(n - q * d)
            e.rt[0] = e.rt[2] = None
            return nxt
        return f
    if mn == 'xchg':
        a, b = O(0), O(1)
        def f(e):
            x, y = a.get(e), b.get(e)
            a.set(e, y)
            b.set(e, x)
            return nxt
        return f
    if mn in JCC:
        tgt = int(ops[0], 16)
        def f(e):
            fl = e.fl
            t = cond(mn, fl[1], fl[2])
            e.cov.add((pc, t))
            e.events.append((pc, t, mn, fl))
            return tgt if t else nxt
        return f
    if mn == 'jmp':
        if ops[0].startswith('0x'):
            tgt = int(ops[0], 16)
            return lambda e: tgt
        a = O(0)
        return lambda e: a.get(e) & M32
    if mn == 'leave':
        def f(e):
            e.r[4] = e.r[5]
            e.r[5] = e.rd(e.r[4])
            e.rt[5] = None
            e.r[4] = s32(e.r[4] + 4)
            return nxt
        return f
    if mn == 'ret':
        n = int(ops[0], 0) if ops else 0
        def f(e):
            sp = e.r[4]
            ret = e.rd(sp) & M32
            e.r[4] = s32(sp + 4 + n)
            return ret
        return f
    if mn == 'nop':
        return lambda e: nxt
    if mn == 'call':
        direct = ops[0].startswith('0x')
        if cmt.startswith('CB->'):
            name = cmt[4:].split()[0]
            kinds, rt = cbs[name]
            rets = rt == 'i'
            return lambda e: (e.do_cb(name, kinds, rets, pc), nxt)[1]
        if cmt in SHARED and not ctx.ev:
            kinds = SHARED[cmt]
            return lambda e: (e.do_cb(cmt, kinds, False, pc), nxt)[1]
        if cmt == 'sprintf':
            return lambda e: (e.sprintf(), nxt)[1]
        if cmt == 'abs':
            def f(e):
                e.r[0] = abs(e.rd(e.r[4] + 4))
                e.rt[0] = None
                return nxt
            return f
        if direct:
            tgt = int(ops[0], 16)
            tfun = L.func_of(L.resolve(tgt))
            if not ctx.ev and (cmt.startswith('lieu_event') or cmt in ('reset_floats', 'pre90_scene')):
                raise EmuError('%08x: call to shared %s not supported' % (pc, cmt))
        else:
            a = O(0)
            tfun = None
        def f(e):
            sp = s32(e.r[4] - 4)
            e.r[4] = sp
            e.wr(sp, nxt)
            e.mt.pop(sp, None)
            t = tgt if direct else a.get(e) & M32
            e.funcs.add(tfun if direct else L.func_of(L.resolve(t)))
            return t
        return f
    if mn in ('fld', 'fcomp', 'fmul', 'fstp', 'fld1', 'fldz', 'fnstsw'):
        # the few x87 forms the shared event code uses (float accumulators)
        if mn in ('fld1', 'fldz'):
            c = 1.0 if mn == 'fld1' else 0.0
            def f(e):
                e.fpu.append(c)
                return nxt
            return f
        if mn == 'fnstsw':
            if ops[0] != 'ax':
                bad()
            def f(e):
                e.r[0] = s32((e.r[0] & ~0xFFFF) | e.fsw)
                e.rt[0] = None
                return nxt
            return f
        if not ops[0].startswith(('dword ptr', 'qword ptr')):
            bad()
        wide = ops[0].startswith('qword')
        ea = parse_ea(ops[0], ctx)
        def ldf(e):
            a = ea(e)
            if wide:
                return struct.unpack('<d', struct.pack('<II', e.rd(a) & M32, e.rd(a + 4) & M32))[0]
            return struct.unpack('<f', struct.pack('<I', e.rd(a) & M32))[0]
        if mn == 'fld':
            def f(e):
                e.fpu.append(ldf(e))
                return nxt
        elif mn == 'fmul':
            def f(e):
                e.fpu[-1] = e.fpu[-1] * ldf(e)
                return nxt
        elif mn == 'fcomp':
            def f(e):
                x, y = e.fpu.pop(), ldf(e)
                e.fsw = 0x100 if x < y else (0x4000 if x == y else (0 if x > y else 0x4500))
                return nxt
        else:       # fstp
            if wide:
                bad()
            def f(e):
                v = struct.unpack('<I', struct.pack('<f', e.fpu.pop()))[0]
                e.wr(ea(e), v)
                return nxt
        return f
    bad()


class Case:
    # `more`: further (arg, arg2) calls made after the first one on the same
    # state and DLL globals (ops 4/5/6: the float accumulators persist)
    __slots__ = ('st', 'pool', 'arg', 'arg2', 'more')

    def __init__(self, st, pool, arg=0, arg2=0, more=()):
        self.st, self.pool, self.arg, self.arg2, self.more = st, pool, arg, arg2, tuple(more)


def emulate(ctx, case, op):
    L = ctx.L
    e = Emu(ctx, case, op, case.arg, case.arg2)
    e.mem[L.state_g] = BASE
    if L.trig_base is not None:
        for k, fa in L.trig.items():
            e.mem[L.trig_base + 4 * k] = fa
    for arg, arg2 in ((case.arg, case.arg2),) + case.more:
        e.r[4] = STACK
        e.r[5] = 0
        e.r[1] = op
        e.r[2] = BASE
        if op == 1:
            arg2 = CBTAB
        # stack at entry: [esp] = return, [esp+4] = arg2, [esp+8] = arg
        e.r[4] = STACK - 12
        e.mem[STACK - 12] = SENT
        e.mem[STACK - 8] = arg2
        e.mem[STACK - 4] = arg
        e.run(L.entry)
    return e


# ---------------------------------------------------------------------------
# cases

def static_values(L, funcs):
    """Constants compared against, and per-slot compare constants."""
    vals = {0, 1, 2, 3, 4, -1}
    slots = collections.defaultdict(collections.Counter)
    for f in funcs:
        for a in range(f, L.func_end(f)):
            if a not in L.ins:
                continue
            ins, cmt = L.ins[a]
            m = re.match(r'^(cmp|sub|test) (.*), (-?0x[0-9a-f]+)$', ins)
            if m:
                v = s32(int(m.group(3), 0))
                vals.update((v, v - 1, v + 1))
                mm = re.match(r'^dword ptr \[e.. \+ (0x[0-9a-f]+)\]$', m.group(2))
                if mm and cmt.startswith('ST('):
                    o = int(mm.group(1), 16)
                    if o < NST:
                        c = slots[o]
                        c[v] += 3
                        c[v + 1] += 1
                        c[v - 1] += 1
    return sorted(vals), slots


SEQ = []     # ops 4/5/6: args whose handler touches DLL globals (set per target)


def make_case(rng, vals, slots, ac=0):
    arg, arg2 = ac if isinstance(ac, tuple) else (ac, 0)
    st = [0] * (NST // 4)
    pm = rng.choice((0.2, 0.5, 0.8, 0.95))
    for o in range(0, NST, 4):
        p = slots.get(o)
        if p is None:
            if rng.random() < 0.3:
                st[o >> 2] = rng.choice((0, 0, 1, 2, 3, -1))
        elif rng.random() < pm:
            ks = sorted(p)
            st[o >> 2] = rng.choices(ks, weights=[p[k] for k in ks])[0]
        else:
            st[o >> 2] = rng.choice(sorted(p) + [0, 1])
    zp = rng.choice((0.1, 0.3, 0.5))
    pool = [0 if rng.random() < zp else rng.choice(vals) for _ in range(POOL_N)]
    more = ()
    if SEQ and rng.random() < 0.4:
        # a run of calls on the same globals: maybe a toggle first, then one
        # event repeated (the accumulators need ~40-60 calls to hit a limit)
        more = ([(rng.choice(SEQ), 0)] if rng.random() < 0.5 else []) + \
               [(rng.choice(SEQ), 0)] * rng.choice((1, 1, 2, 40, 60, 100))
    return Case(st, pool, arg, arg2, more)


def flip_candidates(mn, fl, taken):
    """For the tagged side of the compare that ended at a jcc, yield
    (tag, new source value) that make the jump go the other way."""
    _, a, b, ta, tb = fl
    for tag, other, left in ((ta, b, True), (tb, a, False)):
        if tag is None or (ta is not None and tb is not None and not left):
            continue
        kind, key, mult, add = tag
        for cand in (other, other + 1, other - 1, 0, 1, 2, -1, other + 0x100, 0x7fff):
            x, y = (cand, other) if left else (other, cand)
            if cond(mn, x, y) != (not taken):
                continue
            # value = mult*src + add  ->  src = (value - add) * mult
            src = (cand - add) * mult
            yield kind, key, s32(src)
            break


def apply_flip(case, kind, key, src):
    if kind == 'st':
        st = list(case.st)
        st[key >> 2] = src
        return Case(st, case.pool, case.arg, case.arg2, case.more)
    pool = list(case.pool)
    pool[key] = src
    return Case(case.st, pool, case.arg, case.arg2, case.more)


def mutate(rng, c, reads, used, vals, slots):
    """A copy of case c with a few of the state slots it read, or the
    callback results it used, set to new values."""
    st, pool = list(c.st), list(c.pool)
    for _ in range(rng.choice((1, 1, 2, 3))):
        if reads and (not used or rng.random() < 0.6):
            o = rng.choice(reads)
            st[o >> 2] = rng.choice(sorted(slots[o]) + [0, 1, 2]) if o in slots and rng.random() < 0.7 else rng.choice(vals)
        elif used:
            pool[rng.choice(used)] = rng.choice(vals)
    return Case(st, pool, c.arg, c.arg2, c.more)


def explore(ctx, op, vals, slots, argchoices, budget, seed):
    rng = random.Random(seed)
    covered = set()
    corpus = []
    info = []
    queue = collections.deque()
    tried = collections.Counter()
    n0 = max(120, 3 * len(argchoices))
    for i in range(n0):         # every argument value at least a few times
        queue.append(make_case(rng, vals, slots, argchoices[i % len(argchoices)] if i < 3 * len(argchoices) else rng.choice(argchoices)))
    n = 0
    while n < budget:
        if not queue:
            # the steering ran dry: new random cases and mutants of the corpus
            for _ in range(20):
                queue.append(make_case(rng, vals, slots, rng.choice(argchoices)))
            for _ in range(60 if info else 0):
                c0, reads, used = rng.choice(info)
                queue.append(mutate(rng, c0, reads, used, vals, slots))
        c = queue.popleft()
        n += 1
        try:
            e = emulate(ctx, c, op)
        except (EmuError, NotImplementedError, KeyError):
            continue
        fresh = bool(e.cov - covered)
        if fresh:
            covered |= e.cov
            corpus.append(c)
            info.append((c, sorted(e.stread), sorted(e.pool_used)))
        kids = []
        for pc, t, mn, fl in e.events:
            if (pc, not t) in covered or tried[(pc, not t)] >= 40:
                continue
            tried[(pc, not t)] += 1
            for kind, key, src in flip_candidates(mn, fl, t):
                kids.append(apply_flip(c, kind, key, src))
        if fresh:                         # follow a path that just paid off
            queue.extendleft(reversed(kids))
        else:
            queue.extend(kids)
    return corpus


# ---------------------------------------------------------------------------
# running the C side and comparing

def is_pure(t):
    name = t.split('(')[0]
    if name == 'divers':
        m = re.match(r'divers\((-?\d+),', t)
        return bool(m) and int(m.group(1)) in PURE_DIVERS
    return name in PURE_CB


def run_c(harness, op, cases, tmo=120):
    """Run cases through the harness; returns a list of dicts (or error strings)."""
    results = [None] * len(cases)
    i = 0
    while i < len(cases):
        inp = []
        for c in cases[i:]:
            inp.append('CASE %d %d %d %d %d\n%s\n%s\n%s\n' % (op, c.arg, c.arg2, len(c.pool), len(c.more),
                                                           ' '.join(map(str, c.pool)),
                                                           ' '.join(map(str, c.st)),
                                                           ' '.join('%d %d' % m for m in c.more)))
        try:
            r = subprocess.run([harness], input=''.join(inp), capture_output=True, text=True, encoding="latin-1", timeout=tmo)
            out, rc = r.stdout, r.returncode
        except subprocess.TimeoutExpired as ex:
            out, rc = (ex.stdout or b'').decode() if isinstance(ex.stdout, bytes) else (ex.stdout or ''), -99
        cur = None
        started = 0
        for ln in out.split('\n'):
            if ln == 'B':
                cur = {'tr': [], 'w': {}, 'ret': None, 'done': False}
                started = 1
            elif cur is None:
                continue
            elif ln.startswith('T '):
                cur['tr'].append(ln[2:])
            elif ln.startswith('W '):
                _, o, v = ln.split()
                cur['w'][int(o)] = int(v)
            elif ln.startswith('R '):
                cur['ret'] = int(ln[2:])
            elif ln == 'END':
                cur['done'] = True
                results[i] = cur
                i += 1
                cur = None
        if i < len(cases) and results[i] is None:
            # the case in flight crashed or timed out
            results[i] = {'crash': 'C side crashed (exit %s) after %d calls' % (rc, len(cur['tr']) if cur else 0),
                          'tr': cur['tr'] if cur else []}
            i += 1
    return results


def first_diff(a, b):
    for j in range(max(len(a), len(b))):
        x = a[j] if j < len(a) else None
        y = b[j] if j < len(b) else None
        if x != y:
            return j, x, y
    return None


def compare(e, cres, case):
    """None if equal, else a text describing the first difference."""
    if 'crash' in cres:
        return cres['crash'] + '\n    last calls: ' + ' | '.join(cres['tr'][-3:])
    ta = [t for t in e.trace if not is_pure(t)]
    tb = [t for t in cres['tr'] if not is_pure(t)]
    pa = collections.Counter(t for t in e.trace if is_pure(t))
    pb = collections.Counter(t for t in cres['tr'] if is_pure(t))
    if ta != tb:
        j, x, y = first_diff(ta, tb)
        lines = ['calls differ at #%d (listing %d calls, C %d calls)' % (j, len(e.trace), len(cres['tr']))]
        lines += ['      %s' % c for c in ta[max(0, j - 4):j]]
        lines.append('    listing: %s' % x)
        lines += ['             %s' % c for c in ta[j + 1:j + 3]]
        lines.append('    C      : %s' % y)
        lines += ['             %s' % c for c in tb[j + 1:j + 3]]
        if G.get('full'):
            lines.append('    full listing trace:')
            lines += ['      %08x %s' % (a, c) for a, c in zip(e.tpc, e.trace)]
            lines.append('    full C trace:')
            lines += ['      %s' % c for c in cres['tr']]
        return '\n'.join(lines)
    est = {o: e.st[o >> 2] for o in range(0, NST, 4) if e.st[o >> 2] != case.st[o >> 2]}
    if est != cres['w']:
        d = {hex(k): (est.get(k, case.st[k >> 2]), cres['w'].get(k, case.st[k >> 2]))
             for k in sorted(set(est) | set(cres['w'])) if est.get(k) != cres['w'].get(k)}
        return 'final state differs (offset: (listing, C)): %s' % d
    if e.r[0] != cres['ret']:
        return 'return value differs: listing %d, C %d' % (e.r[0], cres['ret'])
    if pa != pb:
        return 'PURE'          # only side-effect-free queries differ: noted, not a mismatch
    return None


# ---------------------------------------------------------------------------
# C port <-> DLL function names

def c_functions():
    """[(name, file, first line, last line, text)] of the level's C files."""
    out = []
    for f in LEVEL_FILES:
        lines = open(f).read().split('\n')
        i = 0
        while i < len(lines):
            m = re.match(r'^(?:static\s+)?(?:const\s+)?[A-Za-z_][\w\s\*]*?\b(\w+)\s*\([^;{]*\)\s*\{\s*$', lines[i])
            if m and not lines[i].startswith(('if', 'for', 'while', 'switch')):
                j = i
                while j < len(lines) and lines[j] != '}':
                    j += 1
                out.append((m.group(1), f, i, j))
                i = j
            i += 1
    return out


def c_name_for(addr, cfuncs, texts):
    """The C function whose header comment (or, failing that, body) names
    the DLL address."""
    pat = re.compile(r'(?:sub_|0x|\b)%08X\b' % addr, re.I)
    enclosing = None
    for f, lines in texts.items():
        mine = sorted((c for c in cfuncs if c[1] == f), key=lambda c: c[2])
        for n, ln in enumerate(lines):
            if not pat.search(ln):
                continue
            nxt = next((c for c in mine if c[2] > n), None)
            if nxt and all(re.match(r'^\s*(/?\*|$|static [^{]*;$)', lines[k]) for k in range(n + 1, nxt[2])):
                return nxt[0]           # comment block right above the definition
            inside = next((c for c in mine if c[2] <= n <= c[3]), None)
            if inside and not enclosing:
                enclosing = inside[0]
    return enclosing


# ---------------------------------------------------------------------------
# ops 4/5/6: the shared event handlers

def find_event_fn(L, op):
    """Address of the function the entry calls for op 4, 5 or 6 (event4/5/6),
    found through the entry's own jump table (index op + 1)."""
    for a in range(L.entry, L.func_end(L.entry)):
        ins = L.ins.get(a)
        if ins and ins[0].startswith('jmp dword ptr [4*eax + 0x'):
            m = re.search(r'\+ (0x[0-9a-f]+)\]', ins[0])
            tb = L.tables.get(int(m.group(1), 16))
            if tb and len(tb) == 8:
                a = tb[op + 1]
                for _ in range(8):
                    if L.ins[a][0].startswith('call 0x'):
                        return L.func_of(L.resolve(int(L.ins[a][0].split()[1], 16)))
                    a = L.next[a]
    return None


def event_dispatch(L, f):
    """Parse the switch at the top of an event handler: the first direct jmp
    leads to `[dec/inc arg;] cmp arg, N; ja default; [mov cl, byte [B + arg];]
    jmp [4*reg + T]`. Returns None, or dict(off, n, default, byte, table):
    switch value = arg - off."""
    a = f
    for _ in range(40):
        ins = L.ins[a][0]
        if ins.startswith('jmp 0x'):
            break
        a = L.next[a]
    else:
        return None
    a = int(L.ins[a][0].split()[1], 16)
    d = dict(off=0, n=None, default=None, byte=None, table=None)
    for _ in range(14):
        if a not in L.ins:
            return None
        ins = L.ins[a][0]
        if ins.startswith(('dec dword ptr [ebp', 'inc dword ptr [ebp')):
            d['off'] = 1 if ins.startswith('dec') else -1
        m = re.match(r'cmp dword ptr \[ebp - 0x[0-9a-f]+\], (0x[0-9a-f]+)$', ins)
        if m and d['n'] is None:
            d['n'] = int(m.group(1), 16)
        if ins.startswith('ja 0x'):
            d['default'] = int(ins.split()[1], 16)
        m = re.search(r'byte ptr \[eax \+ (0x[0-9a-f]+)\]', ins)
        if m:
            d['byte'] = int(m.group(1), 16)
        m = re.match(r'jmp dword ptr \[4\*e.. \+ (0x[0-9a-f]+)\]', ins)
        if m:
            d['table'] = int(m.group(1), 16)
            break
        a = L.next[a]
    if d['n'] is None or d['default'] is None or d['table'] not in L.tables:
        return None
    return d


def event_args(L, f, op):
    """(handled args, unhandled args to try, args whose handler touches DLL
    globals) of an event handler."""
    d = event_dispatch(L, f)
    if d is None:
        hi = {4: 0x70, 5: 0x70, 6: 8}[op]
        return list(range(0, hi)), [-1, hi, 0x1000], []
    tab = L.tables[d['table']]

    def dest(v):
        idx = v
        if d['byte'] is not None:
            b = L.pe.read(d['byte'] + v, 1)
            idx = b[0] if b else 0
        return tab[idx] if idx < len(tab) else d['default']

    def is_default(t):
        return t == d['default'] or L.ins.get(t, ('',))[0] == 'jmp 0x%x' % d['default']
    handled, unhandled, seq = [], [], []
    for v in range(d['n'] + 1):
        t = dest(v)
        arg = v + d['off']
        if is_default(t):
            unhandled.append(arg)
            continue
        handled.append(arg)
        a = t
        for _ in range(400):
            ins, cmt = L.ins.get(a, ('', ''))
            m = re.search(r'\[(0x[0-9a-f]+)\]', ins)
            if m and not ins.startswith('call') and int(m.group(1), 16) not in L.slots \
                    and int(m.group(1), 16) != L.state_g and ' S' not in ' ' + ins:
                seq.append(arg)
                break
            if ins.startswith('jmp 0x') and (int(ins.split()[1], 16) == d['default'] or not (f <= int(ins.split()[1], 16) < L.func_end(f))):
                break
            a = L.next.get(a, a + 1)
    extra = [d['n'] + d['off'] + 1, d['n'] + d['off'] + 2, 0x1000, -1, d['off'] - 1]
    return handled, unhandled[:6] + extra, seq


LEVEL_FILES = []
G = {}      # shared with the worker processes (fork)


def run_target(t):
    """t = (label, op, argchoices, funcaddr, budget, extra, ev)"""
    global SEQ
    label, op, argchoices, faddr, budget, extra, ev = t
    L, seed = G['L'], G['seed']
    ctx, harness = (G['ctx_ev'], G['harness_ev']) if ev else (G['ctx'], G['harness'])
    SEQ = G['seq'].get(label, [])
    t0 = time.time()
    closure = L.closure(faddr, ev) if faddr else L.closure(L.entry)
    vals, slots = static_values(L, closure | {L.entry})
    corpus = explore(ctx, op, vals, slots, argchoices, budget, seed + op)
    rng = random.Random(seed + 1000 + op)
    cases = list(corpus)
    for c in corpus:                     # mutants of the interesting cases
        for _ in range(3):
            m = Case(list(c.st), list(c.pool), c.arg, c.arg2, c.more)
            for _ in range(rng.choice((1, 2, 4))):
                if rng.random() < 0.5 and slots:
                    o = rng.choice(sorted(slots))
                    m.st[o >> 2] = rng.choice(sorted(slots[o]) + [0, 1])
                else:
                    m.pool[rng.randrange(POOL_N)] = rng.choice(vals)
            cases.append(m)
    if ev:                               # every argument value, whatever its coverage
        for ac in argchoices:
            cases.append(make_case(rng, vals, slots, ac))
    for _ in range(extra):
        cases.append(make_case(rng, vals, slots, rng.choice(argchoices)))
    res = run_c(harness, op, cases)
    cov = set()
    funcs = set()
    ran = bad = err = pure = 0
    argsseen = set()
    badargs = collections.Counter()
    mism = []
    errs = []
    for c, cr in zip(cases, res):
        try:
            e = emulate(ctx, c, op)
        except (EmuError, NotImplementedError, KeyError, IndexError) as ex:
            err += 1
            if len(errs) < 2:
                errs.append(repr(ex))
            continue
        ran += 1
        argsseen.update([c.arg] + [m[0] for m in c.more])
        cov |= e.cov
        funcs |= {f for f in e.funcs if f}
        d = compare(e, cr, c)
        if d == 'PURE':
            pure += 1
        elif d:
            bad += 1
            badargs[c.arg] += 1
            if len(mism) < 2:
                reads = ', '.join('0x%X=%d' % (o, v) for o, v in sorted(e.stread.items()) if v)[:300]
                mism.append((d, reads, c))
    # branch coverage over the functions that ran (the entry only counts
    # branches outside its opcode dispatch)
    skip = G['skip']
    jcc = []
    for f in sorted(funcs | {L.entry}):
        for a in range(f, L.func_end(f)):
            if a in L.ins and L.ins[a][0].split()[0] in JCC and a not in skip:
                if f == L.entry and not ((a, True) in cov or (a, False) in cov):
                    continue
                jcc.append(a)
    both = sum(1 for a in jcc if (a, True) in cov and (a, False) in cov)
    oneway = [(a, (a, True) in cov) for a in jcc if ((a, True) in cov) != ((a, False) in cov)]
    never = [a for a in jcc if (a, True) not in cov and (a, False) not in cov]
    return dict(label=label, cases=ran, bad=bad, pure=pure, err=err, errs=errs, mism=mism, jcc=len(jcc), both=both,
                oneway=oneway, never=never, funcs=funcs, secs=time.time() - t0, argsseen=argsseen, badargs=badargs)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('level', type=int)
    ap.add_argument('target', nargs='*', help='init, tick, trigNN / NN, ev4 / ev5 / ev6 (ev4:ARG, ev5:ARG:PHASE '
                    'for one value), or a DLL function address')
    ap.add_argument('-n', '--budget', type=int, default=1500, help='minimum emulator runs per target while exploring')
    ap.add_argument('--max-budget', type=int, default=15000, help='cap of the per-target budget (8 runs per branch)')
    ap.add_argument('-x', '--extra', type=int, default=300, help='additional random cases per target')
    ap.add_argument('-j', '--jobs', type=int, default=os.cpu_count() or 2)
    ap.add_argument('--seed', type=int, default=1)
    ap.add_argument('--full', action='store_true', help='print the whole traces of a mismatch')
    ap.add_argument('-v', '--verbose', action='store_true', help='list branches seen one way only')
    ap.add_argument('--work', default=os.path.join(os.environ.get('TMPDIR', '/tmp'), 'lieu_difftest'))
    args = ap.parse_args()

    nn = '%02d' % args.level
    dll = os.path.join(ROOT, 'bin', 'game', 'LIEU%s.DLL' % nn)
    if not os.path.exists(dll):
        sys.exit('%s not found' % dll)
    cfiles = sorted(os.path.join(ROOT, 'src', 'levels', f) for f in os.listdir(os.path.join(ROOT, 'src', 'levels'))
                    if re.match(r'lieu%s(_trig)?\.c$' % nn, f))
    if not cfiles:
        sys.exit('no port: src/levels/lieu%s.c' % nn)
    LEVEL_FILES[:] = cfiles
    work = os.path.join(args.work, nn)
    os.makedirs(work, exist_ok=True)

    # listing
    lst = subprocess.run([sys.executable, os.path.join(HERE, 'lieu_annotate.py'), dll],
                         capture_output=True, text=True, check=True).stdout
    open(os.path.join(work, 'listing.dis'), 'w').write(lst)
    pe = ann.PE(dll)
    L = Listing(lst, pe)
    cbs = parse_cb_header()
    ctx = Ctx(L, cbs)

    # C harness
    open(os.path.join(work, 'cbmocks.inc'), 'w').write(gen_cb_c(cbs))
    harness = os.path.join(work, 'harness')
    cc = os.environ.get('CC', 'clang')
    reset = 'l%d_reset_dll_globals' % args.level
    hasreset = any(re.search(r'\b%s\b' % reset, open(f).read()) for f in cfiles)
    rflags = ['-DHAVE_LEVEL_RESET', '-DLEVEL_RESET=' + reset] if hasreset else []
    cmd = [cc, '-std=c99', '-O1', '-w'] + rflags + ['-I' + os.path.join(ROOT, 'src'), '-I' + os.path.join(ROOT, 'src', 'libsys'),
           '-DLEVEL_ENTRY=lieu%s_fonction_editor' % nn, '-DCB_MOCKS_INC="%s"' % os.path.join(work, 'cbmocks.inc'),
           '-o', harness, os.path.join(HERE, 'lieu_difftest_harness.c')] + cfiles
    r = subprocess.run(cmd, capture_output=True, text=True)
    if r.returncode:
        sys.exit('C build failed:\n' + r.stderr[:3000])
    # ops 4/5/6 build: the real shared code (src/levels/lieu_common.c, with
    # its own float accumulators) instead of the traced stubs
    harness_ev = os.path.join(work, 'harness_ev')
    cmd = [cc, '-std=c99', '-O1', '-w', '-DREAL_SHARED'] + rflags + ['-I' + os.path.join(ROOT, 'src'), '-I' + os.path.join(ROOT, 'src', 'libsys'),
           '-DLEVEL_ENTRY=lieu%s_fonction_editor' % nn, '-DCB_MOCKS_INC="%s"' % os.path.join(work, 'cbmocks.inc'),
           '-o', harness_ev, os.path.join(HERE, 'lieu_difftest_harness.c'),
           os.path.join(ROOT, 'src', 'levels', 'lieu_common.c')] + cfiles
    r = subprocess.run(cmd, capture_output=True, text=True)
    evmsg = ''
    if r.returncode:
        harness_ev = None
        evmsg = 'C build with lieu_common.c failed, ops 4/5/6 not run:\n' + r.stderr[:2000]
    ctx_ev = Ctx(L, cbs, ev=True)

    # targets
    cfuncs = c_functions()
    texts = {f: open(f).read().split('\n') for f in cfiles}
    targets = [('init', 1, [0, 0, 0, 1], None, 'INIT tail of the entry', False),
               ('tick', -1, [0], None, 'per-frame body', False)]
    seen = {}
    for k in sorted(L.trig):
        fa = L.trig[k]
        if fa in seen:
            continue
        seen[fa] = k
        targets.append(('trig%02d' % k, 3, [k], fa, None, False))
    # ops 4/5/6
    seqargs = {}
    evinfo = {}
    for op in (4, 5, 6):
        fa = find_event_fn(L, op)
        if fa is None:
            continue
        handled, unhandled, seq = event_args(L, fa, op)
        phases = (0, 1, 2, -1) if op == 5 else (0,)
        if op == 6:
            unhandled = [x for x in unhandled if abs(x) < 64]
        evinfo['ev%d' % op] = (fa, len(handled), len(unhandled))
        seqargs['ev%d' % op] = seq if op == 4 else []
        targets.append(('ev%d' % op, op, [(a, ph) for a in handled + unhandled for ph in phases], fa, None, True))
    want = set(args.target)
    single = []
    for w in sorted(want):
        m = re.match(r'^ev([456]):(-?\w+)(?::(-?\w+))?$', w)
        if m:
            op = int(m.group(1))
            fa = find_event_fn(L, op)
            if fa:
                targets.append((w, op, [(int(m.group(2), 0), int(m.group(3) or '0', 0))], fa, None, True))
                seqargs[w] = seqargs.get('ev%d' % op, [])
                single.append(w)
    if want:
        def hit(t):
            lab, op, ac, fa, _, ev = t
            if lab in single:
                return True
            return (lab in want or (lab.startswith('trig') and str(int(lab[4:])) in want)
                    or (fa and ('%08X' % fa in {w.upper().replace('0X', '').replace('SUB_', '') for w in want})))
        targets = [t for t in targets if hit(t)]
        if not targets:
            sys.exit('no such target: %s' % ' '.join(sorted(want)))
    # trigger slots that alias an already listed handler get a short run
    skip = set()
    for i, a in enumerate(L.addrs):
        if L.ins[a][0].startswith('jmp dword ptr [4*'):
            skip.update(L.addrs[max(0, i - 6):i])
    if harness_ev is None:
        print(evmsg)
        targets = [t for t in targets if not t[5]]
    G.update(ctx=ctx, ctx_ev=ctx_ev, L=L, harness=harness, harness_ev=harness_ev, seed=args.seed, skip=skip,
             full=args.full, seq=seqargs)
    jobs = []
    for lab, op, ac, fa, _, ev in targets:
        small = fa is not None and (L.func_end(fa) - fa) < 40 and not ev
        # big handlers need more runs: about 8 per conditional jump
        fs = L.closure(fa, ev) if fa else L.closure(L.entry) - (L.closure(L.trig[0]) if lab == 'tick' and 0 in L.trig else set())
        nj = sum(1 for f in fs for a in range(f, L.func_end(f)) if a in L.ins and L.ins[a][0].split()[0] in JCC)
        budget = max(args.budget, min(8 * nj, args.max_budget))
        if lab in single:
            budget, small = 200, False
        jobs.append((lab, op, ac, fa, 60 if small else budget, 20 if small else args.extra, ev))

    print('LIEU%s: entry 0x%08X, %d callbacks, %d trigger slots; %d targets' %
          (nn, L.entry, len(L.slots), len(L.trig), len(jobs)))
    t0 = time.time()
    ctxmp = multiprocessing.get_context('fork')
    with concurrent.futures.ProcessPoolExecutor(max_workers=args.jobs, mp_context=ctxmp) as ex:
        results = list(ex.map(run_target, jobs))
    # report
    tot = dict(cases=0, bad=0, err=0, jcc=0, both=0, pure=0)
    failed = False
    print('%-7s %-28s %-24s %6s %9s %7s %s' % ('target', 'DLL function', 'C function', 'cases', 'branches', 'cov', 'result'))
    for (lab, op, ac, fa, _, ev), r in zip(targets, results):
        if fa:
            dn = L.fstart.get(fa, 'sub_%08X' % fa)
            cn = c_name_for(fa, cfuncs, texts) or '?'
            if dn.startswith('lieu_event'):
                cn = 'common:' + dn
        else:
            subs = sorted(f for f in r['funcs'] if f != L.entry and L.fstart.get(f, '').startswith('sub_'))
            dn = ','.join('sub_%08X' % f for f in subs[:2]) or 'entry'
            cn = ','.join(c_name_for(f, cfuncs, texts) or '?' for f in subs[:2]) or ('init' if lab == 'init' else 'tick')
        pct = 100.0 * r['both'] / r['jcc'] if r['jcc'] else 100.0
        status = 'ok' if not r['bad'] and not r['err'] else ('MISMATCH x%d' % r['bad'] if r['bad'] else 'EMU-ERROR x%d' % r['err'])
        if status == 'ok' and r['pure']:
            status += ' (%d cases differ only in pure queries)' % r['pure']
        print('%-7s %-28s %-24s %6d %4d/%-4d %6.1f%% %s' % (lab, dn[:28], cn[:24], r['cases'], r['both'], r['jcc'], pct, status))
        if ev and lab in evinfo:
            fa_, nh, nu = evinfo[lab]
            hs = {a for a, _ph in ac}
            print('    switch: %d handled + %d unhandled argument values, %d of %d exercised' %
                  (nh, nu, len(r['argsseen'] & hs), len(hs)))
            if hs - r['argsseen']:
                print('    not exercised: %s' % ' '.join(str(x) for x in sorted(hs - r['argsseen'])))
        if ev and r['badargs']:
            print('    mismatching arguments: %s' % ' '.join('%d(x%d)' % kv for kv in sorted(r['badargs'].items())))
        for d, reads, c in r['mism']:
            if ev:
                print('    input: arg=%d arg2=%d, then %s' % (c.arg, c.arg2, ' '.join('%d' % m[0] for m in c.more[:6]) + (' ...' if len(c.more) > 6 else '')
                                                                    if c.more else 'no further calls'))
            print('    ' + d.replace('\n', '\n    '))
            print('    state read (non-zero): %s' % reads)
        for ee in r['errs']:
            print('    emulator error: %s' % ee)
        if args.verbose and r['oneway']:
            print('    one-way branches: %s' % ' '.join('%x%s' % (a, 'T' if t else 'F') for a, t in r['oneway'][:40]))
        if args.verbose and r['never']:
            print('    never executed: %s' % ' '.join('%x' % a for a in r['never'][:40]))
        for k in ('cases', 'bad', 'err', 'jcc', 'both', 'pure'):
            tot[k] += r[k]
        failed |= bool(r['bad'] or r['err'])
    print('LIEU%s total: %d cases, branch coverage %d/%d (%.1f%%), %d mismatching, %d emulator errors, %d pure-query-only diffs, %.0fs' %
          (nn, tot['cases'], tot['both'], tot['jcc'], 100.0 * tot['both'] / max(1, tot['jcc']), tot['bad'], tot['err'],
           tot['pure'], time.time() - t0))
    sys.exit(1 if failed else 0)


if __name__ == '__main__':
    main()
