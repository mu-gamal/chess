import math
import sys
import time
from collections import defaultdict
from functools import lru_cache

import chess

TYPES = ["K", "Q", "R", "B", "N"]
BASE = {"K": 1, "Q": 1, "R": 2, "B": 2, "N": 2}
F16 = math.factorial(16)
FAC = [math.factorial(i) for i in range(70)]
PAWN_SQUARES = [sq for sq in chess.SQUARES if 1 <= chess.square_rank(sq) <= 6]
CASTLE_OPTIONS = ["", "K", "Q", "KQ"]
INF = 10 ** 9


def light(sq):
    return (chess.square_file(sq) + chess.square_rank(sq)) % 2 == 1


def compositions():
    out = []
    for p in range(9):
        spare = 8 - p
        for q in range(0, 2 + spare):
            for r in range(0, 3 + spare):
                for b in range(0, 3 + spare):
                    for n in range(0, 3 + spare):
                        extra = max(0, q - 1) + max(0, r - 2) + max(0, b - 2) + max(0, n - 2)
                        if extra <= spare:
                            out.append((p, {"K": 1, "Q": q, "R": r, "B": b, "N": n}))
    return out


COMPS = compositions()


@lru_cache(maxsize=None)
def side_table(fixr, fp, rule5, split_n):
    table = defaultdict(lambda: defaultdict(lambda: [0] * 9))
    for p, comp in COMPS:
        if p < fp or comp["R"] < fixr:
            continue
        free_r = comp["R"] - fixr
        nk = comp["N"] if split_n else 0
        mo = comp["Q"] + free_r + comp["N"] - nk
        other = FAC[comp["Q"]] * FAC[free_r] * FAC[comp["N"]]
        po = max(0, comp["Q"] - 1) + max(0, comp["R"] - 2) + max(0, comp["N"] - 2)
        for bl in range(comp["B"] + 1):
            bd = comp["B"] - bl
            if rule5:
                pr = po + max(0, bl - 1) + max(0, bd - 1)
                if pr > 8 - p:
                    continue
            else:
                pr = po + max(0, comp["B"] - 2)
            w = F16 // (other * FAC[bl] * FAC[bd])
            table[p][(bl, bd, nk, mo)][pr] += w
    return {p: dict(d) for p, d in table.items()}


def case_geometry(stm, cw, cb, ep):
    fixed = {}
    if cw:
        fixed[chess.E1] = chess.Piece(chess.KING, chess.WHITE)
    if "K" in cw:
        fixed[chess.H1] = chess.Piece(chess.ROOK, chess.WHITE)
    if "Q" in cw:
        fixed[chess.A1] = chess.Piece(chess.ROOK, chess.WHITE)
    if cb:
        fixed[chess.E8] = chess.Piece(chess.KING, chess.BLACK)
    if "K" in cb:
        fixed[chess.H8] = chess.Piece(chess.ROOK, chess.BLACK)
    if "Q" in cb:
        fixed[chess.A8] = chess.Piece(chess.ROOK, chess.BLACK)
    empty = set()
    fwp = fbp = 0
    if ep is not None:
        if stm == chess.WHITE:
            fixed[chess.square(ep, 4)] = chess.Piece(chess.PAWN, chess.BLACK)
            empty = {chess.square(ep, 5), chess.square(ep, 6)}
            fbp = 1
        else:
            fixed[chess.square(ep, 3)] = chess.Piece(chess.PAWN, chess.WHITE)
            empty = {chess.square(ep, 2), chess.square(ep, 1)}
            fwp = 1
    return fixed, empty, fwp, fbp


def file_pair_poly(fixed, empty, forbidden=frozenset(), attacker=None, nsq=frozenset()):
    poly = {(0, 0): 1}
    for f in range(8):
        fp = defaultdict(int)
        for w in range(1, 7):
            for b in range(w + 1, 7):
                ok = True
                for r in range(1, 7):
                    sq = chess.square(f, r)
                    pc = fixed.get(sq)
                    want = chess.WHITE if r == w else chess.BLACK if r == b else None
                    if want is None:
                        if pc is not None and pc.piece_type == chess.PAWN:
                            ok = False
                    elif sq in empty or (pc is not None and not (pc.piece_type == chess.PAWN and pc.color == want)):
                        ok = False
                    elif want == attacker and sq in forbidden and pc is None:
                        ok = False
                if ok:
                    sqs = [chess.square(f, r) for r in (w, b) if chess.square(f, r) not in fixed]
                    k = sum(1 for q in sqs if light(q))
                    j = sum(1 for q in sqs if q in nsq)
                    fp[(k, j)] += 1
        new = defaultdict(int)
        for (a1, b1), v in poly.items():
            for (a2, b2), c in fp.items():
                new[(a1 + a2, b1 + b2)] += v * c
        poly = new
    return dict(poly)


def pawn_ways(PL, PD, pwf, pbf, kl):
    t = 0
    for wl in range(max(0, pwf - PD), min(pwf, PL) + 1):
        bl = kl - wl
        bd = pbf - bl
        if bl < 0 or bd < 0:
            continue
        t += math.comb(PL, wl) * math.comb(PD, pwf - wl) * math.comb(PL - wl, bl) * math.comb(PD - (pwf - wl), bd)
    return t


@lru_cache(maxsize=None)
def pawn_dist(ck_light, P1, f, P2, g, a, o):
    out = defaultdict(int)
    for x1 in range(0, min(a, P1) + 1):
        for x2 in range(0, min(a - x1, P2) + 1):
            x3 = a - x1 - x2
            if x3 > g:
                continue
            wa = math.comb(P1, x1) * math.comb(P2, x2) * math.comb(g, x3)
            r1 = P1 - x1 + f
            r2 = P2 - x2
            r3 = g - x3
            for y1 in range(0, min(o, r1) + 1):
                for y2 in range(0, min(o - y1, r2) + 1):
                    y3 = o - y1 - y2
                    if y3 > r3:
                        continue
                    wb = math.comb(r1, y1) * math.comb(r2, y2) * math.comb(r3, y3)
                    kc = x1 + y1
                    ko = x2 + x3 + y2 + y3
                    kl = kc if ck_light else ko
                    out[(kl, x3 + y3)] += wa * wb
    return dict(out)


def falling(n, k):
    return math.perm(n, k) if 0 <= k <= n else 0


def limit(rules, T, pw, pb):
    L = INF
    if 1 in rules and T == 32:
        L = 0
    if 3 in rules:
        L = min(L, 3 * (32 - T))
    if 4 in rules:
        L = min(L, (2 * (32 - T) + 16 - pw - pb) // 2)
    return L


@lru_cache(maxsize=None)
def combine(fixwr, fixbr, fwp, fbp, pw, pb, rules, stm):
    rule5 = 5 in rules
    split = 8 in rules
    wt = side_table(fixwr, fwp, rule5, split and stm == chess.WHITE).get(pw, {})
    bt = side_table(fixbr, fbp, rule5, split and stm == chess.BLACK).get(pb, {})
    fixed_off = 2 + fixwr + fixbr
    bitems = []
    for (bl, bd, bk, bm), arr in bt.items():
        c, s = [], 0
        for v in arr:
            s += v
            c.append(s)
        c += [s] * 9
        bitems.append((bl, bd, bk, bm, bl + bd + bk + bm, c))
    Ltab = {}
    for n in range(0, 64):
        L = limit(rules, pw + pb + n + fixed_off, pw, pb)
        Ltab[n] = min(L, 16)
    out = defaultdict(int)
    out32 = defaultdict(int)
    for (wl, wd, wk, wm), warr in wt.items():
        wn = wl + wd + wk + wm
        nz = [(i, a) for i, a in enumerate(warr) if a]
        for bl, bd, bk, bm, bn, cb in bitems:
            n = wn + bn
            L = Ltab[n]
            if L < 0:
                continue
            s = 0
            for i, a in nz:
                j = L - i
                if j < 0:
                    break
                s += a * cb[j]
            if s:
                key = (wl + bl, wd + bd, wk + bk, wm + bm)
                if pw + pb + n + fixed_off == 32:
                    out32[key] += s
                else:
                    out[key] += s
    return dict(out), dict(out32)


def all_cases():
    for stm in (chess.WHITE, chess.BLACK):
        for cw in CASTLE_OPTIONS:
            for cb in CASTLE_OPTIONS:
                for ep in [None] + list(range(8)):
                    yield (stm, cw, cb, ep)


@lru_cache(maxsize=None)
def knight_factor(FL, FD, Nf, co_light, nl, nd, nk):
    AL = FL - Nf if co_light else FL
    AD = FD if co_light else FD - Nf
    t = 0
    for a in range(0, nk + 1):
        b = nk - a
        x = falling(AL, a) * falling(AD, b)
        if x:
            t += math.comb(nk, a) * x * falling(FL - a, nl) * falling(FD - b, nd)
    return t


D_CACHE = {}
PHI_CACHE = {}


def get_D(ckey, C, R):
    k = (ckey, R)
    d = D_CACHE.get(k)
    if d is None:
        d0 = defaultdict(int)
        dk = defaultdict(int)
        for (nl, nd, nk, mo), v in C.items():
            x = v * falling(R - nl - nd - nk, mo)
            if x:
                if nk:
                    dk[(nl, nd, nk)] += x
                else:
                    d0[(nl, nd)] += x
        d = (tuple(d0.items()), tuple(dk.items()))
        D_CACHE[k] = d
    return d


def phi(ckey, C, R, FL, FD, Nf, co_light):
    d0, dk = get_D(ckey, C, R)
    k0 = (ckey, R, FL, FD)
    a = PHI_CACHE.get(k0)
    if a is None:
        a = 0
        for (nl, nd), v in d0:
            a += v * falling(FL, nl) * falling(FD, nd)
        PHI_CACHE[k0] = a
    if not dk:
        return a
    k1 = (ckey, R, FL, FD, Nf, co_light)
    b = PHI_CACHE.get(k1)
    if b is None:
        b = 0
        for (nl, nd, nk), v in dk:
            b += v * knight_factor(FL, FD, Nf, co_light, nl, nd, nk)
        PHI_CACHE[k1] = b
    return a + b


def officer_sum(ckey, C, TL, TD, pwf, pbf, G, nout, g, co_light):
    R = TL + TD - pwf - pbf
    t = 0
    for (kl, jn), gv in G.items():
        if not gv:
            continue
        FL = TL - kl
        FD = TD - (pwf + pbf - kl)
        if FL < 0 or FD < 0:
            continue
        t += gv * phi(ckey, C, R, FL, FD, nout + g - jn, co_light)
    return t


def king_adjacent(a, b):
    return max(abs(chess.square_file(a) - chess.square_file(b)), abs(chess.square_rank(a) - chess.square_rank(b))) <= 1


def king_pairs(fixed, empty, rules):
    free = [s for s in chess.SQUARES if s not in fixed and s not in empty]
    wk = [k for k, p in fixed.items() if p.piece_type == chess.KING and p.color == chess.WHITE]
    bk = [k for k, p in fixed.items() if p.piece_type == chess.KING and p.color == chess.BLACK]
    wopts = wk if wk else free
    bopts = bk if bk else free
    for a in wopts:
        for b in bopts:
            if a == b:
                continue
            if 6 in rules and king_adjacent(a, b):
                continue
            yield tuple(x for x in (a, b) if x not in fixed)


@lru_cache(maxsize=None)
def officer_val(sig, pw, pb, TL, TD, grp, stm, rules):
    fixwr, fixbr, fwp, fbp = sig
    ck_light, P1, f, P2, g, nout = grp
    C, C32 = combine(fixwr, fixbr, fwp, fbp, pw, pb, rules, stm)
    pwf, pbf = pw - fwp, pb - fbp
    if pwf + pbf > P1 + f + P2 + g:
        return 0
    a, o = (pwf, pbf) if stm == chess.WHITE else (pbf, pwf)
    G = pawn_dist(ck_light, P1, f, P2, g, a, o)
    base = (sig, pw, pb, stm, rules)
    v = officer_sum(base + ("m",), C, TL, TD, pwf, pbf, G, nout, g, not ck_light)
    if C32 and 2 not in rules:
        v += officer_sum(base + ("32",), C32, TL, TD, pwf, pbf, G, nout, g, not ck_light)
    return v


@lru_cache(maxsize=None)
def officer_val32(sig, TL, TD, poly, nout, g, co_light, stm, rules):
    fixwr, fixbr, fwp, fbp = sig
    _, C32 = combine(fixwr, fixbr, fwp, fbp, 8, 8, rules, stm)
    if not C32:
        return 0
    return officer_sum((sig, 8, 8, stm, rules, "32"), C32, TL, TD, 8 - fwp, 8 - fbp, dict(poly), nout, g, co_light)


POLY_CACHE = {}


def case_total(stm, cw, cb, ep, rules):
    fixed, empty, fwp, fbp = case_geometry(stm, cw, cb, ep)
    sig = (len(cw), len(cb), fwp, fbp)
    groups = defaultdict(int)
    groups32 = defaultdict(int)
    kings_fixed = {p.color: k for k, p in fixed.items() if p.piece_type == chess.KING}
    for ks in king_pairs(fixed, empty, rules):
        occ = set(fixed) | set(empty) | set(ks)
        TL = sum(1 for s in chess.SQUARES if s not in occ and light(s))
        TD = 64 - len(occ) - TL
        kings = dict(kings_fixed)
        free_kings = list(ks)
        if chess.WHITE not in kings:
            kings[chess.WHITE] = free_kings.pop(0)
        if chess.BLACK not in kings:
            kings[chess.BLACK] = free_kings.pop(0)
        target = kings[not stm]
        ck_light = light(target)
        forb = set()
        if 7 in rules:
            forb = set(chess.SquareSet(chess.BB_PAWN_ATTACKS[not stm][target]))
            if any(fixed.get(s) == chess.Piece(chess.PAWN, stm) for s in forb):
                continue
            forb = {s for s in forb if s in PAWN_SQUARES and s not in occ}
        nsq = set()
        if 8 in rules:
            nsq = {s for s in chess.SquareSet(chess.BB_KNIGHT_ATTACKS[target]) if s not in occ}
        nz = {s for s in nsq if s in PAWN_SQUARES}
        nout = len(nsq) - len(nz)
        pfree = [s for s in PAWN_SQUARES if s not in occ]
        same = [s for s in pfree if light(s) == ck_light]
        opp = [s for s in pfree if light(s) != ck_light]
        grp = (ck_light, len(same) - len(forb), len(forb), len(opp) - len(nz), len(nz), nout)
        groups[(TL, TD, grp)] += 1
        if 2 in rules:
            kz = tuple(sorted(s for s in ks if s in PAWN_SQUARES))
            groups32[(TL, TD, kz, tuple(sorted(forb)), tuple(sorted(nz)), nout, not ck_light)] += 1
    total = 0
    for (TL, TD, grp), cnt in groups.items():
        sub = 0
        for pw in range(fwp, 9):
            for pb in range(fbp, 9):
                sub += officer_val(sig, pw, pb, TL, TD, grp, stm, rules)
        total += cnt * sub
    if 2 in rules:
        geo = (tuple(sorted(fixed.items())), tuple(sorted(empty)))
        for (TL, TD, kz, forb, nz, nout, co_light), cnt in groups32.items():
            key = (geo, kz, forb, nz, stm)
            if key not in POLY_CACHE:
                f2 = dict(fixed)
                for s in kz:
                    f2[s] = chess.Piece(chess.KING, chess.WHITE)
                POLY_CACHE[key] = tuple(sorted(file_pair_poly(f2, empty, frozenset(forb), stm, frozenset(nz)).items()))
            total += cnt * officer_val32(sig, TL, TD, POLY_CACHE[key], nout, len(nz), co_light, stm, rules)
    return total


def superset_size(rules=()):
    rules = tuple(sorted(rules))
    total = sum(case_total(*c, rules) for c in all_cases())
    assert total % (F16 * F16) == 0
    return total // (F16 * F16)


def run_chunk(rules, start, end, out):
    cases = list(all_cases())[start:end]
    part = sum(case_total(*c, rules) for c in cases)
    with open(out, "a") as fh:
        fh.write(f"{start} {end} {part}\n")


def sum_chunks(path):
    seen = {}
    for line in open(path):
        a, b, v = line.split()
        seen[(int(a), int(b))] = int(v)
    covered = sorted(seen)
    pos = 0
    for a, b in covered:
        assert a == pos, (a, pos)
        pos = b
    assert pos == len(list(all_cases())), pos
    total = sum(seen.values())
    assert total % (F16 * F16) == 0
    return total // (F16 * F16)


if __name__ == "__main__":
    args = sys.argv[1:]
    if args and args[0] == "chunk":
        start, end, out = int(args[1]), int(args[2]), args[3]
        rules = tuple(sorted(int(x) for x in args[4:]))
        t0 = time.time()
        run_chunk(rules, start, end, out)
        print("chunk", start, end, "%.1fs" % (time.time() - t0))
    elif args and args[0] == "sum":
        n = sum_chunks(args[1])
        print(n, "%.4e" % n)
    else:
        rules = tuple(int(x) for x in args)
        t0 = time.time()
        n = superset_size(rules)
        print(rules, n, "%.4e" % n, "%.1fs" % (time.time() - t0))
