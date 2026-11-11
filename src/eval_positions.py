import json
import re
import sys
from pathlib import Path

import chess
import chess.engine
import chess.syzygy

ROOT = Path(__file__).resolve().parent.parent
DATA = ROOT / "log" / "data.js"
OUT = ROOT / "log" / "evals.js"
TB = ROOT / "tablebases"
ENGINES = {
    "darwin": ROOT / "engine/macos-universal/stockfish/stockfish-macos-universal",
    "linux": ROOT / "engine/linux-arm64-universal/stockfish/stockfish-linux-arm64-universal",
}
DEPTH = int(sys.argv[1]) if len(sys.argv) > 1 else 25
WDL = {2: "win", 1: "win", 0: "draw", -1: "loss", -2: "loss"}


def tb_result(tb, board):
    if chess.popcount(board.occupied) > 5:
        return None
    try:
        wdl = tb.probe_wdl(board)
    except (KeyError, chess.syzygy.MissingTableError):
        return None
    if board.turn == chess.BLACK:
        wdl = -wdl
    return WDL[wdl]


def main():
    fens = re.findall(r'fen:\s*"([^"]+)"', DATA.read_text(encoding="utf-8"))
    engine_path = ENGINES["darwin" if sys.platform == "darwin" else "linux"]
    engine = chess.engine.SimpleEngine.popen_uci(str(engine_path))
    tb = chess.syzygy.open_tablebase(str(TB))
    evals = {}
    for fen in fens:
        board = chess.Board(fen)
        info = engine.analyse(board, chess.engine.Limit(depth=DEPTH))
        score = info["score"].white()
        entry = {"depth": info.get("depth", DEPTH)}
        if score.is_mate():
            entry["mate"] = score.mate()
        else:
            entry["cp"] = score.score()
        result = tb_result(tb, board)
        if result:
            entry["tb"] = result
        evals[fen] = entry
        print(fen, entry)
    tb.close()
    engine.quit()
    OUT.write_text("window.EVALS = " + json.dumps(evals, indent=1) + ";\n", encoding="utf-8")


if __name__ == "__main__":
    main()
