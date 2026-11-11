import math
from functools import lru_cache

import chess

TYPES = ["K", "Q", "R", "B", "N"]
BASE = {"K": 1, "Q": 1, "R": 2, "B": 2, "N": 2}
F16 = math.factorial(16)
PAWN_SQUARES = [sq for sq in chess.SQUARES if 1 <= chess.square_rank(sq) <= 6]
CASTLE_OPTIONS = ["", "K", "Q", "KQ"]


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


def fixed_officers(castle):
    f = {}
    if castle:
        f["K"] = 1
        f["R"] = len(castle)
    return f


def free_counts(comp, fixed):
    free = {t: comp[t] - fixed.get(t, 0) for t in TYPES}
    if any(v < 0 for v in free.values()):
        return None
    return free


def promotions(comp):
    return sum(max(0, comp[t] - BASE[t]) for t in TYPES)


@lru_cache(maxsize=None)
def side_table(castle, fixed_pawn):
    fixed = fixed_officers(castle)
    table = {}
    for p, comp in COMPS:
        if p < fixed_pawn:
            continue
        free = free_counts(comp, fixed)
        if free is None:
            continue
        m = sum(free.values())
        w = F16 // math.prod(math.factorial(v) for v in free.values())
        key = (p, m, p + sum(comp.values()), promotions(comp))
        table[key] = table.get(key, 0) + w
    return table


def falling(n, k):
    return math.perm(n, k) if 0 <= k <= n else 0


def case_squares(stm, cw, cb, ep):
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
    ep_square = None
    if ep is not None:
        if stm == chess.WHITE:
            fixed[chess.square(ep, 4)] = chess.Piece(chess.PAWN, chess.BLACK)
            empty = {chess.square(ep, 5), chess.square(ep, 6)}
            ep_square = chess.square(ep, 5)
            fbp = 1
        else:
            fixed[chess.square(ep, 3)] = chess.Piece(chess.PAWN, chess.WHITE)
            empty = {chess.square(ep, 2), chess.square(ep, 1)}
            ep_square = chess.square(ep, 2)
            fwp = 1
    return fixed, empty, fwp, fbp, ep_square


def allowed(rules, w_side, b_side):
    wp, _, wt, wpr = w_side
    bp, _, bt, bpr = b_side
    if 1 in rules and wt + bt == 32 and (wpr or bpr):
        return False
    if 3 in rules and wpr + bpr > 3 * (32 - wt - bt):
        return False
    if 4 in rules and 2 * (wpr + bpr) > 2 * (32 - wt - bt) + 16 - wp - bp:
        return False
    return True


def file_pair_ways(fixed, empty):
    ways = 1
    for f in range(8):
        n = 0
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
                    else:
                        if sq in empty:
                            ok = False
                        elif pc is not None and not (pc.piece_type == chess.PAWN and pc.color == want):
                            ok = False
                if ok:
                    n += 1
        ways *= n
    return ways


def case_total(stm, cw, cb, ep, rules=()):
    fixed, empty, fwp, fbp, _ = case_squares(stm, cw, cb, ep)
    rule2_ways = file_pair_ways(fixed, empty) if 2 in rules else None
    pawn_free = len([s for s in PAWN_SQUARES if s not in fixed and s not in empty])
    total_free = 64 - len(fixed) - len(empty)
    wt = side_table(cw, fwp)
    bt = side_table(cb, fbp)
    total = 0
    for wkey, ww in wt.items():
        pw, mw = wkey[0], wkey[1]
        pwf = pw - fwp
        for bkey, bw in bt.items():
            pb, mb = bkey[0], bkey[1]
            if not allowed(rules, wkey, bkey):
                continue
            pbf = pb - fbp
            if rule2_ways is not None and wkey[2] + bkey[2] == 32:
                pawn_ways = rule2_ways if pw == 8 and pb == 8 else 0
            else:
                pawn_ways = math.comb(pawn_free, pwf) * math.comb(pawn_free - pwf, pbf) if 0 <= pwf and 0 <= pbf and pwf + pbf <= pawn_free else 0
            if not pawn_ways:
                continue
            off = falling(total_free - pwf - pbf, mw + mb)
            if off:
                total += pawn_ways * off * ww * bw
    return total


def all_cases():
    for stm in (chess.WHITE, chess.BLACK):
        for cw in CASTLE_OPTIONS:
            for cb in CASTLE_OPTIONS:
                for ep in [None] + list(range(8)):
                    yield (stm, cw, cb, ep)


class Superset:
    def __init__(self, rules=()):
        self.rules = tuple(rules)
        total = sum(case_total(*case, rules=self.rules) for case in all_cases())
        assert total % (F16 * F16) == 0
        self.size = total // (F16 * F16)
