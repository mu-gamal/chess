import random
import sys

import chess

import superset as S


def promotions5(board, color):
    q = len(board.pieces(chess.QUEEN, color))
    r = len(board.pieces(chess.ROOK, color))
    n = len(board.pieces(chess.KNIGHT, color))
    bs = board.pieces(chess.BISHOP, color)
    bl = sum(1 for s in bs if (chess.square_file(s) + chess.square_rank(s)) % 2 == 1)
    bd = len(bs) - bl
    return max(0, q - 1) + max(0, r - 2) + max(0, n - 2) + max(0, bl - 1) + max(0, bd - 1)


def side_key(board, color, rule5=False):
    comp = {
        "K": len(board.pieces(chess.KING, color)),
        "Q": len(board.pieces(chess.QUEEN, color)),
        "R": len(board.pieces(chess.ROOK, color)),
        "B": len(board.pieces(chess.BISHOP, color)),
        "N": len(board.pieces(chess.KNIGHT, color)),
    }
    p = len(board.pieces(chess.PAWN, color))
    pr = promotions5(board, color) if rule5 else S.promotions(comp)
    return (p, None, p + sum(comp.values()), pr)


def rule2_ok(board):
    if chess.popcount(board.occupied) != 32:
        return True
    for f in range(8):
        w = [chess.square_rank(s) for s in board.pieces(chess.PAWN, chess.WHITE) if chess.square_file(s) == f]
        b = [chess.square_rank(s) for s in board.pieces(chess.PAWN, chess.BLACK) if chess.square_file(s) == f]
        if len(w) != 1 or len(b) != 1 or w[0] >= b[0]:
            return False
    return True


def check(board, rules):
    if 7 in rules:
        k = board.king(not board.turn)
        if board.pieces(chess.PAWN, board.turn) & chess.BB_PAWN_ATTACKS[not board.turn][k]:
            return False
    if 8 in rules:
        k = board.king(not board.turn)
        if board.pieces(chess.KNIGHT, board.turn) & chess.BB_KNIGHT_ATTACKS[k]:
            return False
    if 9 in rules:
        k = board.king(not board.turn)
        near = chess.BB_KING_ATTACKS[k]
        f, r = chess.square_file(k), chess.square_rank(k)
        orth = 0
        for df, dr in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            if 0 <= f + df < 8 and 0 <= r + dr < 8:
                orth |= chess.BB_SQUARES[chess.square(f + df, r + dr)]
        diag = near & ~orth
        if board.pieces(chess.QUEEN, board.turn) & near:
            return False
        if board.pieces(chess.ROOK, board.turn) & orth:
            return False
        if board.pieces(chess.BISHOP, board.turn) & diag:
            return False
    if 6 in rules:
        wk, bk = board.king(chess.WHITE), board.king(chess.BLACK)
        if chess.square_distance(wk, bk) <= 1:
            return False
    if 2 in rules and not rule2_ok(board):
        return False
    w = side_key(board, chess.WHITE, 5 in rules)
    b = side_key(board, chess.BLACK, 5 in rules)
    if w[0] + w[3] > 8 or b[0] + b[3] > 8:
        return False
    return S.allowed(rules, w, b)


def main():
    games = int(sys.argv[1]) if len(sys.argv) > 1 else 2000
    rules = [int(x) for x in sys.argv[2:]] or [1, 2, 3, 4, 5, 6, 7, 8, 9]
    rng = random.Random(7)
    positions = failures = 0
    for _ in range(games):
        board = chess.Board()
        while not board.is_game_over() and rng.random() > 0.005:
            board.push(rng.choice(list(board.legal_moves)))
            positions += 1
            if not check(board, rules):
                failures += 1
                print("FAIL", board.fen())
    print("games", games, "positions", positions, "failures", failures)


if __name__ == "__main__":
    main()
