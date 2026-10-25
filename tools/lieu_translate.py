#!/usr/bin/env python3
"""Draft C for a range of an annotated LIEUnn.DLL listing.

usage: lieu_translate.py LISTING LO HI    (hex addresses, HI exclusive)

LISTING is the output of tools/lieu_annotate.py. Prints structured C for
the instructions in [LO, HI): callback calls with their arguments in
__fastcall order, ST() accesses, if/else rebuilt from the jcc chains, and
/* WARN */ lines where it could not decide. The output is a draft: check
every function against the listing (docs/LEVEL_PORTING.md).

Written while porting LIEU03.
"""
import sys, re

import os
LISTING = sys.argv[1] if len(sys.argv) > 3 else None

_NAMES = open(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'src', 'levels', 'lieu_names.h')).read()
P = {int(v): n for n, v in re.findall(r'\b(P_\w+)\s*=\s*(\d+)', _NAMES)}
DV = {int(v): n for n, v in re.findall(r'\b(DV_\w+)\s*=\s*(\d+)', _NAMES)}
DV_PERSONA_OPS = {1,3,6,8,9,12,18,26,29,34,40}

# name -> (nargs, ret, formats). formats: g generic, d decimal, x hex, s string, P persona, N -persona, V DV op, A persona-by-divers-op
CB = {
 'animation':(1,0,'s'), 'dialogue':(2,0,'sP'), 'evenement':(2,0,'gg'), 'cinematique':(1,0,'s'),
 'pos_perso':(8,0,'Pddxdsxd'), 'etat_objet':(9,0,'sdddxxxdd'), 'joindre':(5,0,'Nddxx'),
 'change_camera':(2,0,'dC'), 'change_piece':(2,0,'dd'), 'bruitage':(3,0,'sdd'),
 'ajoute_zone':(6,0,'dddddd'), 'retire_zone':(1,0,'d'), 'combat':(2,0,'Pd'),
 'direction_perso':(1,1,'P'), 'direction_zone':(1,1,'d'), 'direction_spec':(1,1,'x'),
 'distance':(2,1,'dd'), 'anim_objet':(5,0,'sdggg'), 'bloque':(1,0,'B'), 'fouille':(1,0,'P'),
 'stop_joindre':(1,0,'N'), 'vire_col':(2,0,'gg'), 'print_msg':(3,0,'sgg'), 'joindre_perm':(2,0,'Ng'),
 'modify_var':(2,0,'ag'), 'random':(1,1,'d'), 'select_pile':(2,0,'gg'), 'on_joindre':(1,1,'P'),
 'link_objet':(4,0,'gsgg'), 'divers':(3,1,'VAg'), 'free_anim':(1,0,'g'), 'play_anim':(2,0,'gg'),
 'divers_empile':(3,0,'VAg'), 'place_animal':(6,0,'ddddxx'), 'load_anim':(2,0,'sg'),
 'col_objet':(2,0,'gg'), 'simult':(2,0,'gg'), 'joindre_obj':(2,0,'gg'), 'pause_anim':(2,0,'gg'),
}
HELPERS = {'lieu_action_register':(4,0,'sPss'), 'lieu_pre_scene':(5,0,'PPddd')}

def s32(n):
    n &= 0xffffffff
    return n - (1 << 32) if n & 0x80000000 else n

def fmt_imm(n, f='g', opn=None):
    n = s32(n)
    if f == 'P':
        return P.get(n, str(n) if n < 10 else hex(n).upper().replace('0X','0x'))
    if f == 'N':
        if n < 0 and -n in P: return '-' + P[-n]
        if n > 0 and n in P: return P[n]
        return str(n) if (-10 < n < 10 or f=='d') else ('-0x%X' % -n if n < 0 else '0x%X' % n)
    if f == 'd': return str(n)
    if f == 's': return 'NULL' if n == 0 else str(n)
    if f == 'C': return {1:'CAM_AUTO',2:'CAM_LOCKED',99:'CAM_ANY'}.get(n, str(n))
    if f == 'B': return {0:'BLOQUE_NONE',3:'BLOQUE_ALL'}.get(n, str(n))
    if f == 'V': return DV.get(n, str(n))
    if n < 0: return str(n)
    if n < 10: return str(n)
    return '0x%X' % n

def strip_paren(t):
    if t.startswith('(') and t.endswith(')'):
        d = 0
        for k, ch in enumerate(t):
            if ch == '(': d += 1
            elif ch == ')':
                d -= 1
                if d == 0 and k != len(t) - 1: return t
        return t[1:-1]
    return t

class V:  # value
    def __init__(s, kind, p, prec=0, item=None):
        s.kind, s.p, s.prec, s.item = kind, p, prec, item   # kind: imm str e call S
    def __repr__(s): return 'V(%s,%r)' % (s.kind, s.p)

class Item:
    def __init__(s, kind, addr, text='', **kw):
        s.kind, s.addr, s.text = kind, addr, text   # stmt cond jmp ret dead
        s.__dict__.update(kw)

def parse(lo, hi):
    ins = []
    for l in open(LISTING).read().split('\n'):
        m = re.match(r'^([0-9a-f]{8}): (\S+)\s*(.*)$', l)
        if not m: continue
        a = int(m.group(1), 16)
        if a < lo or a >= hi: continue
        mn = m.group(2); rest = m.group(3)
        op, _, cm = rest.partition(';')
        ops = [o.strip() for o in op.split(',')] if op.strip() else []
        # re-split carefully for 'dword ptr [x + y]' (no commas inside brackets)
        ins.append(dict(a=a, mn=mn, ops=ops, cm=cm.strip(), raw=l))
    return ins

class Tr:
    def __init__(s, ins, locals_=None):
        s.ins = ins; s.items = []; s.regs = {}; s.stack = []; s.cmp = None
        s.targets = set(int(i['ops'][0],16) for i in ins if i['mn'] in ('jmp','je','jne','jl','jge','jle','jg','jb','jae','jbe','ja') and i['ops'] and i['ops'][0].startswith('0x'))
        s.tmp = 0; s.warn = []; s.pending_cdecl = None; s.locals = {}

    # --- value helpers
    def reg(s, r): return s.regs.get(r)
    def text(s, v, f='g'):
        if v is None: return '/*?*/0'
        if v.kind == 'imm': return fmt_imm(v.p, f)
        if v.kind == 'str': return '"%s"' % v.p
        if v.kind == 'call':
            it = v.item
            if it.kind == 'dead' or getattr(it, 'used', False):
                s.warn.append('call %x used twice' % it.addr)
            # hazard: live items after the call item?
            idx = s.items.index(it)
            after = [x for x in s.items[idx+1:] if x.kind not in ('dead',)]
            if after and not it.text.startswith('TEMP'):
                s.tmp += 1
                nm = 't%d' % s.tmp
                it.text = 'i32 %s = %s;' % (nm, it.expr)
                it.is_tmp = True
                v.p = nm
                v.kind = 'e'
                return nm
            it.kind = 'dead'; it.used = True
            return it.expr
        if v.kind == 'e' and f == 'P' and False: return v.p
        return v.p
    def textf(s, v, f):
        # format by arg kind; persona/non-imm expression passthrough
        if v is None: return '/*?*/0'
        if v.kind == 'imm': return fmt_imm(v.p, f)
        t = s.text(v, f)
        if f == 'N' and v.kind == 'e' and t.startswith('-') : return t
        return t

    def operand_val(s, o, cm):
        o = o.replace('dword ptr ', '').strip()
        if re.fullmatch(r'-?0x[0-9a-f]+', o):
            neg = o.startswith('-')
            n = int(o.replace('-', ''), 16)
            if neg: n = -n
            sm = re.search(r'"([^"]*)"', cm)
            if sm and not neg and n > 0x10000000: return V('str', sm.group(1))
            if not neg and n > 0x10000000 and n < 0x10100000: return V('str', '')
            return V('imm', s32(n))
        if o in ('eax','ebx','ecx','edx','esi','edi'): return s.regs.get(o)
        if o == 'S': return V('S', 'S')
        m = re.search(r'(&?ST\(.*\))', cm)
        if o.startswith('[') and m:
            return V('e', m.group(1))
        if o.startswith('[ebp'):
            mm = re.match(r'\[ebp ([-+]) 0x([0-9a-f]+)\]', o)
            return V('e', 'loc_%s%s' % ('m' if mm.group(1) == '-' else 'p', mm.group(2)))
        return V('e', '/*%s*/' % o)

    def mem_lhs(s, o, cm):
        m = re.search(r'ST\(.*\)', cm)
        if m: return m.group(0)
        o = o.replace('dword ptr ', '').strip()
        mm = re.match(r'\[ebp ([-+]) 0x([0-9a-f]+)\]', o)
        if mm: return 'loc_%s%s' % ('m' if mm.group(1) == '-' else 'p', mm.group(2))
        return '/*mem %s*/' % o

    def emit(s, a, text, **kw):
        it = Item('stmt', a, text, **kw); s.items.append(it); return it

    def stack_pop(s):
        return s.stack.pop() if s.stack else None

    def do_call_cb(s, a, name, args_fmt, ret):
        nargs, ret, fmts = args_fmt
        args = []
        args.append(s.regs.get('ecx'));
        if nargs >= 2: args.append(s.regs.get('edx'))
        extra = nargs - 2
        stk = []
        for k in range(max(0, extra)):
            stk.append(s.stack_pop())
        args += stk
        # last pushed = arg3 ... stk[0] is last pushed => arg index 2
        txts = []
        if name in ('divers',) and args[0] is not None and args[0].kind == 'imm' and args[0].p == 9: fmts = 'VAd'
        for i, v in enumerate(args):
            f = fmts[i]
            if f == 'A':
                opv = args[0]
                if opv is not None and opv.kind == 'imm' and opv.p in DV_PERSONA_OPS:
                    f = 'P'
                else: f = 'g'
            if f == 'a':
                txts.append(s.text(v)); continue
            txts.append(s.textf(v, f))
        expr = ('%s(%s)' if name in ('place_animal','load_anim') else 'CB->%s(%s)') % (name, ', '.join(txts))
        return expr, ret

    def run(s):
        ins = s.ins
        i = 0
        for k, x in enumerate(ins):
            a, mn, ops, cm = x['a'], x['mn'], x['ops'], x['cm']
            if a in s.targets:
                for r in ('eax','ecx','edx'): s.regs.pop(r, None)
                if s.stack: s.warn.append('stack nonempty at label %x: %r' % (a, s.stack))
            if mn in ('push',) and ops in (['ebp'],['ebx'],['esi'],['edi']) and a < s.ins[0]['a'] + 8: continue
            if mn == 'push' and ops and ops[0] in ('ebx','esi','edi') and k < 5: continue
            if mn == 'mov' and ops == ['ebp','esp']: continue
            if mn in ('pop','leave'): continue
            if mn == 'ret':
                s.items.append(Item('ret', a)); continue
            if mn == 'mov':
                d, sr = ops
                if d.startswith('dword ptr ['):
                    lhs = s.mem_lhs(d, cm)
                    v = s.operand_val(sr, cm)
                    f = 'g'
                    if re.match(r'ST\((ST_HERO|V_ENGAGED_PERSONA|V_SAVED_HERO)\)$', lhs) and v is not None and v.kind == 'imm' and v.p != 4: f = 'P'
                    if lhs == 'ST(ST_LEVEL)': f = 'd'
                    t = s.textf(v, f)
                    if lhs.startswith('ST(ST_VIE(') and v is not None and v.kind == 'imm' and v.p == 2: t = 'VIE_DEAD'
                    if lhs == 'ST(V_ENGAGED_PERSONA)' and v is not None and v.kind == 'imm' and v.p == 0: t = '0'
                    # persona-ish stores
                    s.emit(a, '%s = %s;' % (lhs, t))
                else:
                    if sr == 'S': s.regs[d] = V('S','S'); continue
                    v = s.operand_val(sr, cm)
                    if v is not None and v.kind == 'call':
                        pass
                    s.regs[d] = v
                continue
            if mn == 'xor' and ops[0] == ops[1]:
                s.regs[ops[0]] = V('imm', 0); continue
            if mn == 'add' and ops[0] == 'esp':
                n = int(ops[1], 16) // 4
                if s.pending_cdecl:
                    it, name, expr_fn = s.pending_cdecl
                    args = [s.stack_pop() for _ in range(n)]
                    it.expr = expr_fn(args)
                    it.text = it.expr + ';'
                    s.pending_cdecl = None
                continue
            if mn == 'add':
                d, sr = ops
                m = re.search(r'&ST\(.*\)', cm)
                if m: s.regs[d] = V('e', m.group(0)); continue
                v = s.regs.get(d); w = s.operand_val(sr, cm)
                s.regs[d] = V('e', '(%s + %s)' % (s.text(v), s.text(w, 'd'))); continue
            if mn == 'sub':
                d, sr = ops
                v = s.regs.get(d); w = s.operand_val(sr, cm)
                s.regs[d] = V('e', '(%s - %s)' % (s.text(v), s.text(w, 'd'))); continue
            if mn == 'neg':
                v = s.regs.get(ops[0])
                if v.kind == 'imm': s.regs[ops[0]] = V('imm', -v.p)
                else: s.regs[ops[0]] = V('e', '-' + s.text(v))
                continue
            if mn == 'cdq':
                s.cdq = True; continue
            if mn == 'idiv':
                n = s.regs.get('eax'); d = s.regs.get(ops[0])
                nt, dt = s.text(n), s.text(d, 'd')
                s.regs['eax'] = V('e', '(%s / %s)' % (nt, dt))
                s.regs['edx'] = V('e', '(%s %% %s)' % (nt, dt))
                continue
            if mn == 'cmp':
                l, r = ops
                if l.startswith('dword ptr ['):
                    lv = s.operand_val(l, cm)
                else: lv = s.operand_val(l, cm)
                rv = s.operand_val(r, cm)
                lt = s.text(lv)
                f = 'g'
                if lv is not None and lv.kind == 'e':
                    if re.match(r'ST\((ST_HERO|V_ENGAGED_PERSONA|V_SAVED_HERO)\)', lt): f = 'P'
                    if lt.startswith('ST(ST_VIE(') and rv.kind == 'imm' and rv.p == 2:
                        s.cmp = (lt, 'VIE_DEAD', '2'); continue
                if f == 'P' and rv.kind == 'imm' and rv.p == 4 and 'ST_HERO' in lt: f = 'g'
                if lt == 'ST(ST_LEVEL)': f = 'd'
                if lt == 'ST(V_ENGAGED_PERSONA)' and rv.kind == 'imm' and rv.p == 0: f = 'g'
                s.cmp = (lt, s.textf(rv, f), s.textf(rv, 'd' if f in ('P',) else f)); continue
            if mn == 'test':
                v = s.regs.get(ops[0])
                s.cmp = (s.text(v), '0', '0'); continue
            if mn in ('je','jne','jl','jge','jle','jg'):
                tgt = int(ops[0], 16)
                s.items.append(Item('cond', a, cond=(s.cmp, mn), target=tgt)); continue
            if mn == 'jmp':
                tgt = int(ops[0], 16)
                s.items.append(Item('jmp', a, target=tgt)); continue
            if mn == 'push':
                v = s.operand_val(ops[0], cm)
                s.stack.append(v); continue
            if mn == 'call':
                o = ops[0]
                mm = re.search(r'CB->(\w+)', cm)
                if mm and o.startswith('dword ptr ['):
                    name = mm.group(1)
                    sig = CB[name]
                    expr, ret = s.do_call_cb(a, name, sig, sig[2])
                    it = s.emit(a, expr + ';', expr=expr)
                    it.iscall = True
                    s.regs.pop('eax', None)
                    if ret: s.regs['eax'] = V('call', None, item=it)
                    continue
                nm = cm.strip()
                if nm in HELPERS:
                    sig = HELPERS[nm]
                    expr, ret = s.do_call_cb(a, nm, sig, sig[2])
                    expr = expr.replace('CB->', '')
                    it = s.emit(a, expr + ';', expr=expr); it.iscall = True
                    s.regs.pop('eax', None)
                    continue
                if nm == 'sprintf' or nm == 'sub_1000EC70':
                    it = s.emit(a, '/*cdecl*/', expr='')
                    it.iscall = True
                    if nm == 'sprintf':
                        fn = lambda args: 'snprintf(%s)' % ', '.join(['g_txt', 'sizeof(g_txt)'] + [s.text(v) for v in args[1:]])
                    else:
                        fn = lambda args: 'abs(%s)' % strip_paren(s.text(args[0], 'd'))
                    s.pending_cdecl = (it, nm, fn)
                    s.regs['eax'] = V('call', None, item=it)
                    continue
                m2 = re.match(r'(trig\d+)_', nm)
                if m2:
                    s.emit(a, '%s();' % m2.group(1)); s.items[-1].iscall = True; continue
                s.emit(a, '/*?? call %s %s*/' % (o, nm)); continue
            s.emit(a, '/*?? %s %s ; %s*/' % (mn, ','.join(ops), cm))
        return s.items

# ---------------- structuring
NEG = {'je':'jne','jne':'je','jl':'jge','jge':'jl','jle':'jg','jg':'jle'}
OPS = {'je':'==','jne':'!=','jl':'<','jge':'>=','jle':'<=','jg':'>'}
class Node:
    def __init__(s, kind, **kw): s.kind = kind; s.__dict__.update(kw)

def cond_text(cmp, jc, negate):
    # jump taken when (a OP b); body executes when NOT taken (negate=True) -> condition = NOT jc
    jc2 = NEG[jc] if negate else jc
    a, b, bp = cmp
    op = OPS[jc2]
    if op not in ('==', '!='): b = bp
    if b == '0' and op in ('==', '!=') and a.startswith('CB->'):
        return a if op == '!=' else '!' + a
    return '%s %s %s' % (a, op, b)

def bnot_atom(c, jc, neg): return ('atom', c, jc, neg)
def atom_text(x):
    _, cmp, jc, neg = x
    return cond_text(cmp, jc, neg)
def mk_or(l):
    out = []
    for e in l:
        if e[0] == 'or': out += list(e[1])
        elif e == ('F',): continue
        else: out.append(e)
    if any(e == ('T',) for e in out): return ('T',)
    if not out: return ('F',)
    return out[0] if len(out) == 1 else ('or', tuple(out))
def mk_and(l):
    out = []
    for e in l:
        if e[0] == 'and': out += list(e[1])
        elif e == ('T',): continue
        else: out.append(e)
    if any(e == ('F',) for e in out): return ('F',)
    if not out: return ('T',)
    return out[0] if len(out) == 1 else ('and', tuple(out))
def ite(cT, cN, X, Y):
    if X == Y: return X
    if X == ('T',): return mk_or([cT, Y])
    if X == ('F',): return mk_and([cN, Y])
    if Y == ('T',): return mk_or([cN, X])
    if Y == ('F',): return mk_and([cT, X])
    if Y[0] == 'or' and Y[1][-1] == X: return mk_or([mk_and([cN] + list(Y[1][:-1])), X])
    xs = list(X[1]) if X[0] == 'and' else [X]
    ys = list(Y[1]) if Y[0] == 'and' else [Y]
    if len(ys) > len(xs) and ys[-len(xs):] == xs:
        return mk_and([mk_or([cT, mk_and(ys[:-len(xs)])])] + xs)
    return ('ite', cT, X, Y)
def render_b(e, ctx=None):
    k = e[0]
    if k == 'atom': return atom_text(e)
    if k == 'T': return '1'
    if k == 'F': return '0'
    if k == 'or':
        return ' || '.join(('(' + render_b(x) + ')') if x[0] in ('and', 'ite') else render_b(x) for x in e[1])
    if k == 'and':
        return ' && '.join(('(' + render_b(x) + ')') if x[0] in ('or', 'ite') else render_b(x) for x in e[1])
    if k == 'ite':
        return '%s ? %s : %s /*TERNARY*/' % (atom_text(e[1]), render_b(e[2]), render_b(e[3]))

class Structurer:
    def __init__(s, items):
        s.items = [it for it in items if it.kind != 'dead']
        s.ret_idx = len(s.items) - 1
        s.used_labels = set()
        s.used_labels_pre = set()
    def idx(s, addr):
        for k, it in enumerate(s.items):
            if it.addr >= addr: return k
        return len(s.items) - 1
    def block(s, i, r):
        out = []
        while i < r:
            it = s.items[i]
            if it.addr in s.used_labels_pre and it.kind != 'ret':
                out.append(Node('label', addr=it.addr))
            if it.kind == 'stmt':
                out.append(Node('stmt', text=it.text, addr=it.addr)); i += 1; continue
            if it.kind == 'ret':
                i += 1; continue
            if it.kind == 'jmp':
                t = s.idx(it.target)
                if t == s.ret_idx: out.append(Node('stmt', text='return;', addr=it.addr))
                elif t == i + 1: pass
                else:
                    s.used_labels.add(it.target)
                    out.append(Node('stmt', text='goto L%X;' % (it.target & 0xffff), addr=it.addr))
                i += 1; continue
            if it.kind == 'cond':
                j = i
                while j < r and s.items[j].kind == 'cond': j += 1
                n = j - i
                best = None
                for k in range(1, n + 1):
                    B = i + k
                    last = s.items[i + k - 1]
                    S = s.idx(last.target)
                    if S <= B: continue
                    ok = True
                    for u in range(i, i + k):
                        t = s.idx(s.items[u].target)
                        if t not in (B, S) and not (i < t < B): ok = False
                    if ok: best = (k, B, S)
                if best is None:
                    s.used_labels.add(it.target)
                    c = it.cond
                    out.append(Node('stmt', text='if (%s) goto L%X;' % (cond_text(c[0], c[1], False), it.target & 0xffff), addr=it.addr))
                    i += 1; continue
                k, B, S = best
                memo = {}
                def F(u):
                    if u in memo: return memo[u]
                    if u == B: return ('T',)
                    cu = s.items[u]
                    cT = bnot_atom(cu.cond[0], cu.cond[1], False)
                    cN = bnot_atom(cu.cond[0], cu.cond[1], True)
                    t = s.idx(cu.target)
                    X = ('T',) if t == B else (('F',) if t == S else F(t))
                    Y = F(u + 1)
                    memo[u] = ite(cT, cN, X, Y)
                    return memo[u]
                ctext = render_b(F(i))
                bodyend = S
                E = None
                lastit = s.items[S - 1] if S - 1 >= B else None
                if lastit is not None and lastit.kind == 'jmp':
                    e = s.idx(lastit.target)
                    if e > S and e != s.ret_idx:
                        E = e; bodyend = S - 1
                body = s.block(B, bodyend)
                els = s.block(S, E) if E is not None else []
                out.append(Node('if', cond=ctext, body=body, els=els, addr=it.addr))
                i = E if E is not None else S
                continue
            i += 1
        return out

def render(nodes, ind=1, out=None, comment_ifs=True):
    pad = '    ' * ind
    for n in nodes:
        if n.kind == 'stmt': out.append(pad + n.text)
        elif n.kind == 'label': out.append('L%X:' % (n.addr & 0xffff))
        elif n.kind == 'if':
            out.append(pad + '/* 0x%X */' % (n.addr & 0xffff))
            render_if(n, ind, out)
    return out

def render_if(n, ind, out, first=True):
    pad = '    ' * ind
    out.append(pad + ('if (%s) {' % n.cond))
    render(n.body, ind + 1, out)
    els = n.els
    while els:
        if len(els) == 1 and els[0].kind == 'if':
            e = els[0]
            out.append(pad + '} else if (%s) {' % e.cond)
            render(e.body, ind + 1, out)
            els = e.els
        else:
            out.append(pad + '} else {')
            render(els, ind + 1, out)
            els = None
    out.append(pad + '}')

def main():
    if len(sys.argv) != 4:
        sys.exit(__doc__)
    lo = int(sys.argv[2], 16); hi = int(sys.argv[3], 16)
    ins = parse(lo, hi)
    tr = Tr(ins)
    items = tr.run()
    # drop trailing label items before ret handled by structurer
    st = Structurer(items)
    # prepass for labels: find which targets are used via goto => do two passes
    st.used_labels_pre = set()
    nodes = st.block(0, len(st.items))
    if st.used_labels:
        st.used_labels_pre = set(st.used_labels)
        st.used_labels = set()
        nodes = st.block(0, len(st.items))
    out = render(nodes, 1, [])
    print('\n'.join(out))
    for w in tr.warn: print('/* WARN %s */' % w)
    if tr.stack: print('/* WARN leftover stack %r */' % tr.stack)

main()
