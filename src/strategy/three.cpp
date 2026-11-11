#include <bits/stdc++.h>
#include <thread>
using namespace std;

static inline int F(int s) { return s & 7; }
static inline int Rk(int s) { return s >> 3; }
static inline int dist(int a, int b) { return max(abs(F(a) - F(b)), abs(Rk(a) - Rk(b))); }
static inline int idx(int wk, int bk, int x) { return (wk * 64 + bk) * 64 + x; }
static const int N = 64 * 64 * 64;
static const int KD[8][2] = {{1,0},{-1,0},{0,1},{0,-1},{1,1},{1,-1},{-1,1},{-1,-1}};
static const int ND[8][2] = {{1,2},{2,1},{-1,2},{-2,1},{1,-2},{2,-1},{-1,-2},{-2,-1}};
enum { Q, R, B, Nn, P };

static bool slide(int x, int t, int blk, bool orth, bool diag) {
    int df = F(t) - F(x), dr = Rk(t) - Rk(x);
    if (!df && !dr) return false;
    bool o = !df || !dr, d = abs(df) == abs(dr);
    if (!((o && orth) || (d && diag))) return false;
    int sf = (df > 0) - (df < 0), sr = (dr > 0) - (dr < 0), f = F(x) + sf, r = Rk(x) + sr;
    while (true) { int s = r * 8 + f; if (s == t) return true; if (s == blk) return false; f += sf; r += sr; }
}
static bool attacks(int type, int x, int t, int wk) {
    if (x == t) return false;
    switch (type) {
        case Q: return slide(x, t, wk, true, true);
        case R: return slide(x, t, wk, true, false);
        case B: return slide(x, t, wk, false, true);
        case Nn: { int a = abs(F(x) - F(t)), b = abs(Rk(x) - Rk(t)); return (a == 1 && b == 2) || (a == 2 && b == 1); }
        default: return Rk(t) == Rk(x) + 1 && abs(F(t) - F(x)) == 1;
    }
}
static bool onBoard(int f, int r) { return f >= 0 && f < 8 && r >= 0 && r < 8; }
static bool pieceOK(int type, int x) { return type != P || (Rk(x) >= 1 && Rk(x) <= 6); }
static bool validB(int type, int wk, int bk, int x) { return wk != bk && wk != x && bk != x && dist(wk, bk) > 1 && pieceOK(type, x); }
static bool validW(int type, int wk, int bk, int x) { return validB(type, wk, bk, x) && !attacks(type, x, bk, wk); }

static void blackMoves(int type, int wk, int bk, int x, vector<int>& out) {
    out.clear();
    for (auto& d : KD) { int f = F(bk) + d[0], r = Rk(bk) + d[1]; if (!onBoard(f, r)) continue; int s = r * 8 + f;
        if (dist(s, wk) <= 1) continue;
        if (s == x) { out.push_back(s); continue; }
        if (attacks(type, x, s, wk)) continue;
        out.push_back(s); }
}
struct Mv { int nwk, nx, promo; };
static void whiteMoves(int type, int wk, int bk, int x, vector<Mv>& out) {
    out.clear();
    for (auto& d : KD) { int f = F(wk) + d[0], r = Rk(wk) + d[1]; if (!onBoard(f, r)) continue; int s = r * 8 + f; if (s == x || dist(s, bk) <= 1) continue; out.push_back({s, x, -1}); }
    if (type == P) {
        int s = x + 8; if (s == wk || s == bk) return;
        if (Rk(s) == 7) { for (int p : {Q, R, B, Nn}) out.push_back({wk, s, p}); return; }
        out.push_back({wk, s, -1});
        if (Rk(x) == 1) { int s2 = x + 16; if (s2 != wk && s2 != bk) out.push_back({wk, s2, -1}); }
        return;
    }
    if (type == Nn) { for (auto& d : ND) { int f = F(x) + d[0], r = Rk(x) + d[1]; if (!onBoard(f, r)) continue; int s = r * 8 + f; if (s == wk || s == bk) continue; out.push_back({wk, s, -1}); } return; }
    for (auto& d : KD) {
        bool diag = d[0] && d[1];
        if ((type == R && diag) || (type == B && !diag)) continue;
        int f = F(x) + d[0], r = Rk(x) + d[1];
        while (onBoard(f, r)) { int s = r * 8 + f; if (s == wk || s == bk) break; out.push_back({wk, s, -1}); f += d[0]; r += d[1]; }
    }
}

static vector<int> DW[5], DB[5];
static void retro(int type) {
    auto& W = DW[type]; auto& Bt = DB[type]; W.assign(N, -1); Bt.assign(N, -1);
    vector<int> bm; vector<Mv> wm;
    for (int i = 0; i < N; i++) { int wk = i / 4096, bk = (i / 64) % 64, x = i % 64;
        if (!validB(type, wk, bk, x)) continue; blackMoves(type, wk, bk, x, bm);
        if (bm.empty() && attacks(type, x, bk, wk)) Bt[i] = 0; }
    for (int n = 0; ; n += 2) {
        bool ch = false;
        for (int i = 0; i < N; i++) { int wk = i / 4096, bk = (i / 64) % 64, x = i % 64;
            if (W[i] >= 0 || !validW(type, wk, bk, x)) continue; whiteMoves(type, wk, bk, x, wm);
            for (auto& m : wm) { int v = m.promo < 0 ? Bt[idx(m.nwk, bk, m.nx)] : DB[m.promo][idx(m.nwk, bk, m.nx)];
                if (v == n) { W[i] = n + 1; ch = true; break; } } }
        for (int i = 0; i < N; i++) { int wk = i / 4096, bk = (i / 64) % 64, x = i % 64;
            if (Bt[i] >= 0 || !validB(type, wk, bk, x)) continue; blackMoves(type, wk, bk, x, bm); if (bm.empty()) continue;
            bool all = true; int mx = -1;
            for (int s : bm) { if (s == x) { all = false; break; } int d = W[idx(wk, s, x)]; if (d < 0) { all = false; break; } mx = max(mx, d); }
            if (all && mx == n + 1) { Bt[i] = n + 2; ch = true; } }
        if (!ch) break;
    }
}
static string fen(int type, int i) {
    char b[64]; memset(b, 0, 64); b[i / 4096] = 'K'; b[(i / 64) % 64] = 'k'; b[i % 64] = "QRBNP"[type]; string f;
    for (int r = 7; r >= 0; r--) { int e = 0; for (int x = 0; x < 8; x++) { char q = b[r * 8 + x]; if (!q) e++; else { if (e) f += char('0' + e); e = 0; f += q; } } if (e) f += char('0' + e); if (r) f += '/'; }
    return f + " w - - 0 1";
}
static void report(int type, const char* name) {
    long long ww = 0, wd = 0, bl = 0, bd = 0; int mx = 0, mi = -1;
    for (int i = 0; i < N; i++) { int wk = i / 4096, bk = (i / 64) % 64, x = i % 64;
        if (validW(type, wk, bk, x)) { if (DW[type][i] >= 0) { ww++; if (DW[type][i] > mx) { mx = DW[type][i]; mi = i; } } else wd++; }
        if (validB(type, wk, bk, x)) { if (DB[type][i] >= 0) bl++; else bd++; } }
    printf("%s | total %lld | wins %lld (wtm %lld + btm %lld) | draws %lld (wtm %lld + btm %lld) | max mate %d moves%s%s\n",
        name, ww + wd + bl + bd, ww + bl, ww, bl, wd + bd, wd, bd, (mx + 1) / 2, mi >= 0 ? " | " : "", mi >= 0 ? fen(type, mi).c_str() : "");
}

static int areaQ(int wk, int bk, int q) {
    static int seen[64]; static int stamp = 0; stamp++;
    int st[64], sp = 0, c = 0; st[sp++] = bk; seen[bk] = stamp;
    while (sp) { int s = st[--sp]; c++;
        for (auto& d : KD) { int f = F(s) + d[0], r = Rk(s) + d[1]; if (!onBoard(f, r)) continue; int t = r * 8 + f;
            if (seen[t] == stamp || t == q || attacks(Q, q, t, -1)) continue; seen[t] = stamp; st[sp++] = t; } }
    return c;
}
static int outcome(int wk, int bk, int q) {
    vector<int> bm; blackMoves(Q, wk, bk, q, bm);
    if (bm.empty()) return attacks(Q, q, bk, wk) ? 1 : 2;
    return 0;
}
static bool qSafe(int wk, int bk, int q) { return dist(q, bk) > 1 || dist(q, wk) == 1; }
static int manh(int a, int b) { return abs(F(a) - F(b)) + abs(Rk(a) - Rk(b)); }
static int oppT(int bk) {
    int f = F(bk), r = Rk(bk);
    if (f == 0) f = 2; else if (f == 7) f = 5;
    if (r == 0) r = 2; else if (r == 7) r = 5;
    return r * 8 + f;
}
static Mv ruleQ(int wk, int bk, int q) {
    vector<Mv> wm; whiteMoves(Q, wk, bk, q, wm);
    int cur = areaQ(wk, bk, q);
    for (auto& m : wm) if (outcome(m.nwk, bk, m.nx) == 1) return m;
    int best = -1, bv = INT_MAX;
    for (int k = 0; k < (int)wm.size(); k++) { auto& m = wm[k]; if (m.nwk != wk) continue;
        if (!qSafe(wk, bk, m.nx) || outcome(wk, bk, m.nx) == 2) continue;
        int a = areaQ(wk, bk, m.nx); if (a >= cur) continue;
        int v = a * 16 + dist(m.nx, wk); if (v < bv) { bv = v; best = k; } }
    if (best >= 0) return wm[best];
    bv = INT_MAX;
    for (int k = 0; k < (int)wm.size(); k++) { auto& m = wm[k]; if (m.nwk == wk) continue;
        if (!qSafe(m.nwk, bk, q) || outcome(m.nwk, bk, q) == 2) continue;
        if (areaQ(m.nwk, bk, q) > cur) continue;
        int T = oppT(bk), a0 = dist(wk, T) * 16 + manh(wk, T), a1 = dist(m.nwk, T) * 16 + manh(m.nwk, T);
        if (a1 >= a0) continue;
        int v = a1 * 16 + dist(m.nwk, q); if (v < bv) { bv = v; best = k; } }
    if (best >= 0) return wm[best];
    bv = INT_MAX;
    for (int k = 0; k < (int)wm.size(); k++) { auto& m = wm[k];
        if (!qSafe(m.nwk, bk, m.nx) || outcome(m.nwk, bk, m.nx) == 2) continue;
        int a = areaQ(m.nwk, bk, m.nx); if (a > cur) continue;
        int v = a * 256 + dist(m.nwk, bk) * 16 + (m.nwk == wk ? 0 : 1); if (v < bv) { bv = v; best = k; } }
    if (best >= 0) return wm[best];
    bv = INT_MAX;
    for (int k = 0; k < (int)wm.size(); k++) { auto& m = wm[k];
        if (!qSafe(m.nwk, bk, m.nx) || outcome(m.nwk, bk, m.nx) == 2) continue;
        int v = areaQ(m.nwk, bk, m.nx) * 256 + dist(m.nwk, bk) * 16 + dist(m.nx, m.nwk); if (v < bv) { bv = v; best = k; } }
    if (best >= 0) return wm[best];
    return wm.empty() ? Mv{-1, -1, -1} : wm[0];
}

static int edgeD(int x) { return min(x, 7 - x); }
static int pushEdge(int s) { int rd = edgeD(Rk(s)), fd = edgeD(F(s)); return 90 - (7 * fd * fd / 2 + 7 * rd * rd / 2); }
static int pushClose(int a, int b) { return 140 - 20 * dist(a, b); }
static const int BIG = 1000000;
static int searchB(int wk, int bk, int q, int ply, int depth, int alpha, int beta);
static int searchW(int wk, int bk, int q, int ply, int depth, int alpha, int beta) {
    if (ply >= depth) return pushEdge(bk) + pushClose(wk, bk);
    vector<Mv> wm; whiteMoves(Q, wk, bk, q, wm); int best = -BIG;
    for (auto& m : wm) { int v = searchB(m.nwk, bk, m.nx, ply + 1, depth, alpha, beta); if (v > best) best = v; if (best > alpha) alpha = best; if (alpha >= beta) break; }
    return best;
}
static int searchB(int wk, int bk, int q, int ply, int depth, int alpha, int beta) {
    vector<int> bm; blackMoves(Q, wk, bk, q, bm);
    if (bm.empty()) return attacks(Q, q, bk, wk) ? BIG - ply : -BIG;
    for (int s : bm) if (s == q) return -BIG;
    if (ply >= depth) return pushEdge(bk) + pushClose(wk, bk);
    int best = BIG;
    for (int s : bm) { int v = searchW(wk, s, q, ply + 1, depth, alpha, beta); if (v < best) best = v; if (best < beta) beta = best; if (alpha >= beta) break; }
    return best;
}
static int SDEPTH = 4;
static Mv ruleSF(int wk, int bk, int q) {
    vector<Mv> wm; whiteMoves(Q, wk, bk, q, wm); int bv = -BIG - 1, bi = -1;
    for (int k = 0; k < (int)wm.size(); k++) { int v = searchB(wm[k].nwk, bk, wm[k].nx, 1, SDEPTH, bv, BIG); if (v > bv) { bv = v; bi = k; } }
    return bi < 0 ? Mv{-1, -1, -1} : wm[bi];
}
static vector<Mv> SM;
static Mv (*RULE)(int, int, int) = ruleQ;
static Mv cachedRule(int wk, int bk, int q) { return SM.empty() ? ruleQ(wk, bk, q) : SM[idx(wk, bk, q)]; }
static void precomputeSF() {
    if (getenv("SDEPTH")) SDEPTH = atoi(getenv("SDEPTH"));
    SM.assign(N, Mv{-1, -1, -1}); int T = 4; vector<thread> th;
    for (int t = 0; t < T; t++) th.emplace_back([t, T] { for (int i = t; i < N; i += T) { int wk = i / 4096, bk = (i / 64) % 64, q = i % 64; if (validW(Q, wk, bk, q)) SM[i] = ruleSF(wk, bk, q); } });
    for (auto& x : th) x.join();
}
static vector<int> GS, GL;
static int goodQ(int i) {
    if (GS[i] == 2) return 1; if (GS[i] == 3 || GS[i] == 1) return 0;
    GS[i] = 1; int wk = i / 4096, bk = (i / 64) % 64, q = i % 64;
    Mv m = cachedRule(wk, bk, q); int ok = 1, L = 1;
    if (m.nwk < 0) ok = 0;
    else { int o = outcome(m.nwk, bk, m.nx);
        if (o == 2) ok = 0;
        else if (o == 0) { vector<int> bm; blackMoves(Q, m.nwk, bk, m.nx, bm);
            for (int s : bm) { if (s == m.nx) { ok = 0; break; } int j = idx(m.nwk, s, m.nx); if (!goodQ(j)) { ok = 0; break; } L = max(L, GL[j] + 1); } } }
    GS[i] = ok ? 2 : 3; GL[i] = L; return ok;
}

static void traceQ(int i) {
    auto nm = [](int q) { string x; x += char('a' + F(q)); x += char('1' + Rk(q)); return x; };
    set<int> vis;
    for (int p = 0; p < 40; p++) { int wk = i / 4096, bk = (i / 64) % 64, q = i % 64;
        if (vis.count(i)) { printf(" [repeat]\n"); return; } vis.insert(i);
        Mv m = cachedRule(wk, bk, q); printf("%s%s ", m.nwk != wk ? "K" : "Q", nm(m.nwk != wk ? m.nwk : m.nx).c_str());
        int o = outcome(m.nwk, bk, m.nx); if (o == 1) { printf("#\n"); return; } if (o == 2) { printf("stalemate\n"); return; }
        vector<int> bm; blackMoves(Q, m.nwk, bk, m.nx, bm); int pick = -1;
        for (int s : bm) { if (s == m.nx) { printf("kx%s\n", nm(s).c_str()); return; } if (GS[idx(m.nwk, s, m.nx)] == 3) { pick = s; break; } }
        if (pick < 0) pick = bm[0]; printf("k%s  ", nm(pick).c_str()); i = idx(m.nwk, pick, m.nx); }
    printf("\n");
}
static void testKQK() {
    GS.assign(N, 0); GL.assign(N, 0);
    long long tot = 0, ok = 0, over = 0; int mx = 0; vector<int> bad;
    for (int i = 0; i < N; i++) { int wk = i / 4096, bk = (i / 64) % 64, q = i % 64;
        if (!validW(Q, wk, bk, q) || DW[Q][i] < 0) continue; tot++;
        if (goodQ(i)) { if (GL[i] > 50) over++; else ok++; mx = max(mx, GL[i]); } else if (bad.size() < 6) bad.push_back(i); }
    long long totB = 0, okB = 0;
    for (int i = 0; i < N; i++) { int wk = i / 4096, bk = (i / 64) % 64, q = i % 64;
        if (!validB(Q, wk, bk, q) || DB[Q][i] < 0) continue; totB++;
        vector<int> bm; blackMoves(Q, wk, bk, q, bm); bool g = true; int L = 0;
        for (int s : bm) { int j = idx(wk, s, q); if (!goodQ(j)) { g = false; break; } L = max(L, GL[j]); }
        if (bm.empty() || (g && L <= 50)) okB++; }
    printf("KQK rule test (every black defense): wtm %lld/%lld, btm %lld/%lld, over 50 moves %lld, longest %d moves\n", ok, tot, okB, totB, over, mx);
    for (int i : bad) { printf("  fail: %s\n    ", fen(Q, i).c_str()); traceQ(i); }
}
#ifndef NO_MAIN
int main() {
    auto t0 = chrono::steady_clock::now();
    for (int t : {Q, R, B, Nn}) retro(t);
    retro(P);
    report(R, "KRK"); report(Q, "KQK"); report(B, "KBK"); report(Nn, "KNK"); report(P, "KPK");
    if (getenv("KQKSF")) { auto t1 = chrono::steady_clock::now(); precomputeSF(); printf("SF rule depth %d plies, precompute %.1fs\n", SDEPTH, chrono::duration<double>(chrono::steady_clock::now() - t1).count()); testKQK(); }
    else if (getenv("KQKTEST")) testKQK();
    if (getenv("PROBE")) { int t, wk, bk, x; sscanf(getenv("PROBE"), "%d %d %d %d", &t, &wk, &bk, &x); printf("probe %s wtm=%d\n", fen(t, idx(wk, bk, x)).c_str(), DW[t][idx(wk, bk, x)]); }
    printf("%.1fs\n", chrono::duration<double>(chrono::steady_clock::now() - t0).count());
}

#endif
