import chess, chess.syzygy, sys, time
tb = chess.syzygy.open_tablebase("tb")

def box_area(rk, bk):
    rf, rr = chess.square_file(rk), chess.square_rank(rk)
    bf, br = chess.square_file(bk), chess.square_rank(bk)
    w = (rf if bf < rf else 7 - rf) if bf != rf else 8
    h = (rr if br < rr else 7 - rr) if br != rr else 8
    return w * h

def rook_safe(b):
    rk = next(iter(b.pieces(chess.ROOK, chess.WHITE)))
    wk, bk = b.king(chess.WHITE), b.king(chess.BLACK)
    if chess.square_distance(bk, rk) == 1 and chess.square_distance(wk, rk) > 1:
        return False
    return True

def axis_info(rk, bk):
    rf, rr = chess.square_file(rk), chess.square_rank(rk)
    bf, br = chess.square_file(bk), chess.square_rank(bk)
    cut_rank, cut_file = br != rr, bf != rf
    hh = (rr if br < rr else 7 - rr) if cut_rank else 99
    ww = (rf if bf < rf else 7 - rf) if cut_file else 99
    if cut_rank and (hh <= ww or not cut_file):
        d = 1 if rr > br else -1
        return "rank", chess.square(bf, max(0, min(7, br + 2 * d)))
    if cut_file:
        d = 1 if rf > bf else -1
        return "file", chess.square(max(0, min(7, bf + 2 * d)), br)
    return None, bk

def white_move(b):
    moves = list(b.legal_moves)
    for m in moves:
        b.push(m); mate = b.is_checkmate(); b.pop()
        if mate: return m, "mate"
    safe = []
    for m in moves:
        b.push(m)
        ok = not b.is_stalemate() and rook_safe(b)
        b.pop()
        if ok: safe.append(m)
    wk, bk = b.king(chess.WHITE), b.king(chess.BLACK)
    rk = next(iter(b.pieces(chess.ROOK, chess.WHITE)))
    axis, tgt = axis_info(rk, bk)
    def keeps(m):
        if axis == "rank": return chess.square_rank(m.to_square) == chess.square_rank(rk)
        if axis == "file": return chess.square_file(m.to_square) == chess.square_file(rk)
        return True
    rmoves = [m for m in safe if m.from_square == rk]
    if not rook_safe(b):
        cand = sorted([m for m in rmoves if keeps(m)] or rmoves, key=lambda m: (-chess.square_distance(m.to_square, bk), m.to_square))
        if cand: return cand[0], "flee"
    area = box_area(rk, bk)
    best = None
    for m in rmoves:
        t = m.to_square
        if chess.square_distance(t, bk) < 3 and chess.square_distance(t, wk) > 1: continue
        a = box_area(t, bk)
        if a < area and (best is None or a < best[0]): best = (a, m)
    if best: return best[1], "shrink"
    wf, wr = chess.square_file(wk), chess.square_rank(wk)
    bf, br = chess.square_file(bk), chess.square_rank(bk)
    opp_file = wf == bf and abs(wr - br) == 2
    opp_rank = wr == br and abs(wf - bf) == 2
    if opp_file or opp_rank:
        for m in rmoves:
            t = m.to_square
            if (opp_file and chess.square_rank(t) == br) or (opp_rank and chess.square_file(t) == bf):
                b.push(m); chk = b.is_check(); b.pop()
                if chk: return m, "check"
    best = None
    cur = chess.square_distance(wk, tgt)
    for m in safe:
        if m.from_square != wk: continue
        dd = chess.square_distance(m.to_square, tgt)
        guard = chess.square_distance(m.to_square, rk) == 1
        key = (dd, 0 if guard else 1, chess.square_distance(m.to_square, bk), m.to_square)
        if dd < cur and (best is None or key < best[0]): best = (key, m)
    if best: return best[1], "approach"
    waits = sorted([m for m in rmoves if keeps(m) and box_area(m.to_square, bk) <= area], key=lambda m: (-chess.square_distance(m.to_square, bk), m.to_square))
    if waits: return waits[0], "wait"
    return (safe[0] if safe else moves[0]), "fallback"

def black_move(b):
    best = None
    for m in b.legal_moves:
        if b.is_capture(m): return m
        b.push(m)
        d = tb.probe_dtz(b)
        b.pop()
        key = (d, m.uci())
        if best is None or key > best[0]: best = (key, m)
    return best[1]

LIMIT = 106
memo = {}
def play(b):
    path = []
    seen = set()
    res = None
    while True:
        k = b.board_fen() + (" w" if b.turn else " b")
        if k in memo: res = memo[k]; break
        if b.is_checkmate(): res = 0 if b.turn == chess.BLACK else -1; break
        if b.is_stalemate() or not b.pieces(chess.ROOK, chess.WHITE): res = -1; break
        if k in seen or len(path) > LIMIT: res = -1; break
        seen.add(k); path.append(k)
        m = white_move(b)[0] if b.turn else black_move(b)
        b.push(m)
    n = len(path)
    for i, k in enumerate(path):
        memo[k] = (res + n - i) if res >= 0 else -1
    return memo[path[0]] if path else res

t0 = time.time()
stats = {}; fails = []; maxd = [0]
for wk in chess.SQUARES:
    for bk in chess.SQUARES:
        if chess.square_distance(wk, bk) <= 1: continue
        for rk in chess.SQUARES:
            if rk in (wk, bk): continue
            for turn in (chess.WHITE, chess.BLACK):
                b = chess.Board(None)
                b.set_piece_at(wk, chess.Piece(chess.KING, True)); b.set_piece_at(bk, chess.Piece(chess.KING, False)); b.set_piece_at(rk, chess.Piece(chess.ROOK, True))
                b.turn = turn
                if not b.is_valid(): continue
                w = tb.probe_wdl(b)
                truth = "win" if (w > 0) == (turn == chess.WHITE) and w != 0 else ("draw" if w == 0 else "loss")
                d = play(b.copy())
                r = "win" if 0 <= d <= LIMIT else ("slow" if d > LIMIT else "fail")
                key = (truth, r); stats[key] = stats.get(key, 0) + 1
                if truth == "win" and r != "win" and len(fails) < 12: fails.append((b.fen(), r, d))
                if r == "win": maxd[0] = max(maxd[0], d)
print(stats, "max plies", maxd[0], "%.0fs" % (time.time() - t0))
for f in fails: print(f)
