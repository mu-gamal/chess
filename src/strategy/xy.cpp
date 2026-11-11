#define NO_MAIN
#include "three.cpp"
#include <thread>
extern "C" {
#include "fathom/tbprobe.h"
}

static const long long NX = 64LL * 64 * 64 * 64;
static inline long long ix(int wk, int bk, int a, int c) { return ((long long)(wk * 64 + bk) * 64 + a) * 64 + c; }
static int TA, TC;
static vector<int8_t> RS[2], DS[2];

static inline uint64_t bt(int s) { return 1ULL << s; }
static bool att(int type, int x, int t, uint64_t occ) {
    if (x < 0 || x == t) return false;
    if (type == Nn) { int a = abs(F(x) - F(t)), b = abs(Rk(x) - Rk(t)); return (a == 1 && b == 2) || (a == 2 && b == 1); }
    int df = F(t) - F(x), dr = Rk(t) - Rk(x);
    bool o = !df || !dr, d = abs(df) == abs(dr);
    if (!((o && type != B) || (d && type != R))) return false;
    int sf = (df > 0) - (df < 0), sr = (dr > 0) - (dr < 0), f = F(x) + sf, r = Rk(x) + sr;
    while (true) { int s = r * 8 + f; if (s == t) return true; if (occ >> s & 1) return false; f += sf; r += sr; }
}
struct Pos { int k[2], p[2]; };
static int typ(int side) { return side == 0 ? TA : TC; }
static bool inCheck(const Pos& P, int side) {
    int o = 1 - side; uint64_t occ = bt(P.k[0]) | bt(P.k[1]) | (P.p[0] >= 0 ? bt(P.p[0]) : 0) | (P.p[1] >= 0 ? bt(P.p[1]) : 0);
    return att(typ(o), P.p[o], P.k[side], occ);
}
static bool validP(const Pos& P, int stm) {
    int s[4] = {P.k[0], P.k[1], P.p[0], P.p[1]};
    for (int i = 0; i < 4; i++) for (int j = i + 1; j < 4; j++) if (s[i] == s[j]) return false;
    if (dist(P.k[0], P.k[1]) <= 1) return false;
    return !inCheck(P, 1 - stm);
}
static void gen(const Pos& P, int side, vector<pair<Pos, bool>>& out) {
    out.clear(); int o = 1 - side;
    auto push = [&](Pos q, bool cap) { if (dist(q.k[0], q.k[1]) <= 1) return; if (inCheck(q, side)) return; out.push_back({q, cap}); };
    for (auto& d : KD) { int f = F(P.k[side]) + d[0], r = Rk(P.k[side]) + d[1]; if (!onBoard(f, r)) continue; int s = r * 8 + f;
        if (s == P.p[side] || s == P.k[o]) continue; Pos q = P; q.k[side] = s; bool cap = s == P.p[o]; if (cap) q.p[o] = -1; push(q, cap); }
    int x = P.p[side], t = typ(side);
    uint64_t occ = bt(P.k[0]) | bt(P.k[1]) | bt(P.p[0]) | bt(P.p[1]);
    auto tryTo = [&](int s) { if (s == P.k[0] || s == P.k[1]) return false; Pos q = P; q.p[side] = s; bool cap = s == P.p[o]; if (cap) q.p[o] = -1; push(q, cap); return !cap; };
    if (t == Nn) { for (auto& d : ND) { int f = F(x) + d[0], r = Rk(x) + d[1]; if (!onBoard(f, r)) continue; int s = r * 8 + f; if (s == P.p[side]) continue; tryTo(s); } return; }
    for (auto& d : KD) { bool dg = d[0] && d[1]; if ((t == R && dg) || (t == B && !dg)) continue;
        int f = F(x) + d[0], r = Rk(x) + d[1];
        while (onBoard(f, r)) { int s = r * 8 + f; if (s == P.k[0] || s == P.k[1]) break; if (!tryTo(s)) break; f += d[0]; r += d[1]; } }
}
static Pos dec(long long i) { Pos P; P.k[0] = i >> 18; P.k[1] = (i >> 12) & 63; P.p[0] = (i >> 6) & 63; P.p[1] = i & 63; return P; }
static long long enc(const Pos& P) { return ix(P.k[0], P.k[1], P.p[0], P.p[1]); }
static string fenX(long long i, int stm) {
    Pos P = dec(i); char bd[64]; memset(bd, 0, 64); bd[P.k[0]] = 'K'; bd[P.k[1]] = 'k'; bd[P.p[0]] = "QRBN"[TA]; bd[P.p[1]] = "qrbn"[TC]; string f;
    for (int r = 7; r >= 0; r--) { int e = 0; for (int x = 0; x < 8; x++) { char q = bd[r * 8 + x]; if (!q) e++; else { if (e) f += char('0' + e); e = 0; f += q; } } if (e) f += char('0' + e); if (r) f += '/'; }
    return f + (stm ? " b" : " w") + " - - 0 1";
}
int main(int argc, char** argv) {
    auto t0 = chrono::steady_clock::now();
    string nm = argc > 1 ? argv[1] : "BB"; auto ty = [](char c) { return (int)string("QRBN").find(c); };
    TA = ty(nm[0]); TC = ty(nm[1]);
    for (int t : {Q, R, B, Nn}) retro(t);
    int maxDep = 0; for (int t : {TA, TC}) for (int v : DB[t]) maxDep = max(maxDep, v);
    auto fl = [](int q) { return q ^ 56; };
    for (int s = 0; s < 2; s++) { RS[s].assign(NX, 0); DS[s].assign(NX, 0); }
    vector<pair<Pos, bool>> mv;
    for (int s = 0; s < 2; s++) for (long long i = 0; i < NX; i++) { Pos P = dec(i); if (!validP(P, s)) continue; gen(P, s, mv); if (mv.empty() && inCheck(P, s)) { RS[s][i] = 2; DS[s][i] = 0; } }
    for (int n = 0; n < 120; n++) {
        const int T = 4; vector<vector<pair<long long, int8_t>>> up(T * 2); vector<thread> th;
        for (int t = 0; t < T; t++) th.emplace_back([&, t] { vector<pair<Pos, bool>> mv;
            for (int s = 0; s < 2; s++) for (long long i = t; i < NX; i += T) { if (RS[s][i]) continue; Pos P = dec(i); if (!validP(P, s)) continue; gen(P, s, mv); if (mv.empty()) continue;
                bool allW = true; int mx = -1; bool win = false;
                for (auto& m : mv) { int r, d;
                    if (m.second) { allW = false; const Pos& q = m.first; int v = s == 0 ? DB[TA][idx(q.k[0], q.k[1], q.p[0])] : DB[TC][idx(fl(q.k[1]), fl(q.k[0]), fl(q.p[1]))];
                        if (v >= 0 && v == n) { win = true; break; } continue; }
                    long long j = enc(m.first); r = RS[1 - s][j]; d = DS[1 - s][j];
                    if (r == 2 && d == n) { win = true; break; }
                    if (r != 1) allW = false; else mx = max(mx, d); }
                if (win) up[t * 2 + s].push_back({i, 1});
                else if (allW && mx == n) up[t * 2 + s].push_back({i, 2}); } });
        for (auto& x : th) x.join();
        bool ch = false;
        for (int t = 0; t < T; t++) for (int s = 0; s < 2; s++) for (auto& u : up[t * 2 + s]) { RS[s][u.first] = u.second; DS[s][u.first] = n + 1; ch = true; }
        if (!ch && n > maxDep) break;
    }
    long long ww = 0, dr = 0, bw = 0; int mxw = 0, mxb = 0; long long iw = -1, ib = -1; int sw = 0, sb = 0;
    for (int s = 0; s < 2; s++) for (long long i = 0; i < NX; i++) { Pos P = dec(i); if (!validP(P, s)) continue;
        int r = RS[s][i], d = DS[s][i];
        bool whiteWins = (s == 0 && r == 1) || (s == 1 && r == 2), blackWins = (s == 1 && r == 1) || (s == 0 && r == 2);
        if (whiteWins) { ww++; if (s == 0 && d > mxw) { mxw = d; iw = i; sw = s; } }
        else if (blackWins) { bw++; if (s == 1 && d > mxb) { mxb = d; ib = i; sb = s; } }
        else dr++; }
    if (getenv("TBDIR")) { tb_init(getenv("TBDIR")); long long mm = 0, nn = 0;
        for (int s = 0; s < 2; s++) for (long long i = 0; i < NX; i++) { Pos P = dec(i); if (!validP(P, s)) continue;
            uint64_t Wb = bt(P.k[0]) | bt(P.p[0]), Bb = bt(P.k[1]) | bt(P.p[1]), K = bt(P.k[0]) | bt(P.k[1]);
            auto tb = [&](int t) { uint64_t r = 0; if (TA == t) r |= bt(P.p[0]); if (TC == t) r |= bt(P.p[1]); return r; };
            unsigned r = tb_probe_wdl(Wb, Bb, K, tb(Q), tb(R), tb(B), tb(Nn), 0, 0, 0, 0, s == 0); nn++;
            int theirs = (r == TB_WIN || r == TB_CURSED_WIN) ? 1 : (r == TB_LOSS || r == TB_BLESSED_LOSS) ? 2 : 0;
            if (theirs != RS[s][i]) { mm++; if (mm <= 8) printf("  %s ours=%d syzygy=%u\n", fenX(i, s).c_str(), RS[s][i], r); } }
        printf("syzygy check: %lld positions, mismatches %lld\n", nn, mm); }
    printf("K%cvK%c | total %lld | white wins %lld | draws %lld | black wins %lld | longest white mate %d moves %s | longest black mate %d moves %s | %.1fs\n",
        "QRBN"[TA], "QRBN"[TC], ww + dr + bw, ww, dr, bw, (mxw + 1) / 2, iw >= 0 ? fenX(iw, sw).c_str() : "-", (mxb + 1) / 2, ib >= 0 ? fenX(ib, sb).c_str() : "-",
        chrono::duration<double>(chrono::steady_clock::now() - t0).count());
}
