#include <bits/stdc++.h>
using namespace std;

static inline int F(int s) { return s & 7; }
static inline int Rk(int s) { return s >> 3; }
static inline int dist(int a, int b) { return max(abs(F(a) - F(b)), abs(Rk(a) - Rk(b))); }
static inline int idx(int wk, int bk, int rk) { return (wk * 64 + bk) * 64 + rk; }

static const int KD[8][2] = {{1,0},{-1,0},{0,1},{0,-1},{1,1},{1,-1},{-1,1},{-1,-1}};
static const int RD[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};

static bool rookAttacks(int rk, int target, int block) {
    if (rk < 0 || rk == target) return false;
    if (F(rk) != F(target) && Rk(rk) != Rk(target)) return false;
    int df = (F(target) > F(rk)) - (F(target) < F(rk)), dr = (Rk(target) > Rk(rk)) - (Rk(target) < Rk(rk));
    int f = F(rk) + df, r = Rk(rk) + dr;
    while (true) { int s = r * 8 + f; if (s == target) return true; if (s == block) return false; f += df; r += dr; }
}
static bool blackInCheck(int wk, int bk, int rk) { return rookAttacks(rk, bk, wk); }

struct Mv { int piece, to; };
static void whiteMoves(int wk, int bk, int rk, vector<Mv>& out) {
    out.clear();
    for (auto& d : KD) { int f = F(wk) + d[0], r = Rk(wk) + d[1]; if (f < 0 || f > 7 || r < 0 || r > 7) continue; int s = r * 8 + f; if (s == rk || dist(s, bk) <= 1) continue; out.push_back({0, s}); }
    if (rk >= 0) for (auto& d : RD) { int f = F(rk) + d[0], r = Rk(rk) + d[1]; while (f >= 0 && f < 8 && r >= 0 && r < 8) { int s = r * 8 + f; if (s == wk || s == bk) break; out.push_back({1, s}); f += d[0]; r += d[1]; } }
}
static void blackMoves(int wk, int bk, int rk, vector<int>& out) {
    out.clear();
    for (auto& d : KD) { int f = F(bk) + d[0], r = Rk(bk) + d[1]; if (f < 0 || f > 7 || r < 0 || r > 7) continue; int s = r * 8 + f;
        if (dist(s, wk) <= 1) continue;
        if (s == rk) { out.push_back(s); continue; }
        if (rookAttacks(rk, s, wk)) continue;
        out.push_back(s); }
}
static bool validW(int wk, int bk, int rk) { return wk != bk && wk != rk && bk != rk && dist(wk, bk) > 1 && !blackInCheck(wk, bk, rk); }
static bool validB(int wk, int bk, int rk) { return wk != bk && wk != rk && bk != rk && dist(wk, bk) > 1; }

static vector<int> DW, DB;
static void retro() {
    int N = 64 * 64 * 64; DW.assign(N, -1); DB.assign(N, -1);
    vector<int> bm; vector<Mv> wm;
    for (int wk = 0; wk < 64; wk++) for (int bk = 0; bk < 64; bk++) for (int rk = 0; rk < 64; rk++) {
        if (!validB(wk, bk, rk)) continue; blackMoves(wk, bk, rk, bm);
        if (bm.empty() && blackInCheck(wk, bk, rk)) DB[idx(wk, bk, rk)] = 0; }
    for (int n = 0; ; n += 2) {
        bool ch = false;
        for (int wk = 0; wk < 64; wk++) for (int bk = 0; bk < 64; bk++) for (int rk = 0; rk < 64; rk++) {
            int i = idx(wk, bk, rk); if (DW[i] >= 0 || !validW(wk, bk, rk)) continue; whiteMoves(wk, bk, rk, wm);
            for (auto& m : wm) { int nw = m.piece ? wk : m.to, nr = m.piece ? m.to : rk; if (DB[idx(nw, bk, nr)] == n) { DW[i] = n + 1; ch = true; break; } } }
        for (int wk = 0; wk < 64; wk++) for (int bk = 0; bk < 64; bk++) for (int rk = 0; rk < 64; rk++) {
            int i = idx(wk, bk, rk); if (DB[i] >= 0 || !validB(wk, bk, rk)) continue; blackMoves(wk, bk, rk, bm); if (bm.empty()) continue;
            bool all = true; int mx = -1;
            for (int s : bm) { if (s == rk) { all = false; break; } int d = DW[idx(wk, s, rk)]; if (d < 0) { all = false; break; } mx = max(mx, d); }
            if (all && mx == n + 1) { DB[i] = n + 2; ch = true; } }
        if (!ch) break;
    }
}

static vector<int> AREAC;
static int boxArea(int wk, int bk, int rk) {
    if (rk >= 0) { int &c = AREAC[idx(wk, bk, rk)]; if (c) return c; }
    int res;
    bool att[64] = {false};
    for (int s = 0; s < 64; s++) if (dist(s, wk) <= 1 || rookAttacks(rk, s, wk) || s == rk) att[s] = true;
    bool seen[64] = {false}; int st[64], sp = 0, cnt = 0; st[sp++] = bk; seen[bk] = true;
    while (sp) { int s = st[--sp]; cnt++; for (auto& d : KD) { int f = F(s) + d[0], r = Rk(s) + d[1]; if (f < 0 || f > 7 || r < 0 || r > 7) continue; int t = r * 8 + f; if (seen[t] || att[t]) continue; seen[t] = true; st[sp++] = t; } }
    res = cnt;
    if (rk >= 0) AREAC[idx(wk, bk, rk)] = res;
    return res;
}

static const int INF = 1000000000;
static int DEPTH = 2;
static int target(int bk, int rk) {
    int rf = F(rk), rr = Rk(rk), bf = F(bk), br = Rk(bk);
    bool cr = br != rr, cf = bf != rf;
    int hh = cr ? (br < rr ? rr : 7 - rr) : 99, ww = cf ? (bf < rf ? rf : 7 - rf) : 99;
    if (cr && (hh <= ww || !cf)) { int d = rr > br ? 1 : -1; return min(7, max(0, br + 2 * d)) * 8 + bf; }
    if (cf) { int d = rf > bf ? 1 : -1; return br * 8 + min(7, max(0, bf + 2 * d)); }
    return bk;
}
static int W_AREA = 1000, W_TGT = 20, W_KD = 1, W_GUARD = 5;
static int W_IN = 0;
static bool kingOutside(int wk, int bk, int rk) {
    auto sg = [](int x) { return (x > 0) - (x < 0); };
    bool byRank = Rk(bk) != Rk(rk) && sg(Rk(wk) - Rk(rk)) != sg(Rk(bk) - Rk(rk));
    bool byFile = F(bk) != F(rk) && sg(F(wk) - F(rk)) != sg(F(bk) - F(rk));
    return byRank || byFile;
}
static int evalLeaf(int wk, int bk, int rk) {
    if (W_IN && !kingOutside(wk, bk, rk)) {
        int kd = abs(F(wk) - F(bk)) + abs(Rk(wk) - Rk(bk));
        return -1000 * 64 - 10 * kd + 5 * dist(rk, bk);
    }
    int area = boxArea(wk, bk, rk);
    int kd = abs(F(wk) - F(bk)) + abs(Rk(wk) - Rk(bk));
    int guard = (rk >= 0 && dist(wk, rk) == 1) ? 1 : 0;
    return -W_AREA * area - W_TGT * dist(wk, target(bk, rk)) - W_KD * kd + W_GUARD * guard;
}
static int searchB(int wk, int bk, int rk, int movesLeft);
static int searchW(int wk, int bk, int rk, int movesLeft) {
    if (rk < 0) return -INF;
    if (movesLeft == 0) return evalLeaf(wk, bk, rk);
    vector<Mv> wm; whiteMoves(wk, bk, rk, wm);
    int best = -INF;
    for (auto& m : wm) { int nw = m.piece ? wk : m.to, nr = m.piece ? m.to : rk; best = max(best, searchB(nw, bk, nr, movesLeft)); }
    return best;
}
static int searchB(int wk, int bk, int rk, int movesLeft) {
    vector<int> bm; blackMoves(wk, bk, rk, bm);
    if (bm.empty()) return blackInCheck(wk, bk, rk) ? INF - (DEPTH - movesLeft) : -INF;
    int worst = INF;
    for (int s : bm) { if (s == rk) return -INF; worst = min(worst, searchW(wk, s, rk, movesLeft - 1)); }
    return worst;
}
static vector<int> RMC;
static Mv ruleMove(int wk, int bk, int rk) {
    int &c = RMC[idx(wk, bk, rk)]; if (c) return {c >> 8, (c & 255) - 1};
    vector<Mv> wm; whiteMoves(wk, bk, rk, wm);
    int best = -INF - 1; Mv bm = wm[0];
    for (auto& m : wm) { int nw = m.piece ? wk : m.to, nr = m.piece ? m.to : rk; int v = searchB(nw, bk, nr, DEPTH); if (v > best) { best = v; bm = m; } }
    c = (bm.piece << 8) | (bm.to + 1);
    return bm;
}
static int playFromB(int wk, int bk, int rk, int limit);
static vector<int> PM;
static int playFromW(int wk, int bk, int rk, int limit) {
    vector<int> path; int res = -3, ply = 0;
    unordered_set<int> seen;
    while (true) {
        int i = idx(wk, bk, rk);
        if (PM[i] != INT_MIN) { int v = PM[i]; res = v >= 0 ? v : v; break; }
        if (seen.count(i)) { res = -2; break; }
        seen.insert(i); path.push_back(i);
        Mv m = ruleMove(wk, bk, rk);
        if (m.piece) rk = m.to; else wk = m.to;
        vector<int> bm; blackMoves(wk, bk, rk, bm);
        if (bm.empty()) { res = blackInCheck(wk, bk, rk) ? 1 : -1; path.pop_back(); PM[i] = res; break; }
        int pick = -1, pv = -1; bool bad = false;
        for (int s : bm) { if (s == rk) { bad = true; break; } int d = DW[idx(wk, s, rk)]; if (d < 0) { bad = true; break; } if (d > pv) { pv = d; pick = s; } }
        if (bad) { res = -1; path.pop_back(); PM[i] = -1; break; }
        bk = pick;
    }
    int n = (int)path.size();
    for (int k = n - 1; k >= 0; k--) { int d = n - k; PM[path[k]] = res >= 0 ? res + 2 * d : res; }
    int v = path.empty() ? res : PM[path[0]];
    if (v >= 0 && v > limit) return -2;
    return v;
}
int main(int argc, char** argv) {
    if (argc > 1) DEPTH = atoi(argv[1]);
    if (argc > 6) W_IN = atoi(argv[6]);
    if (argc > 5) { W_AREA = atoi(argv[2]); W_TGT = atoi(argv[3]); W_KD = atoi(argv[4]); W_GUARD = atoi(argv[5]); }
    int LIMIT = 106;
    auto t0 = chrono::steady_clock::now();
    retro();
    AREAC.assign(64 * 64 * 64, 0); RMC.assign(64 * 64 * 64, 0); PM.assign(64 * 64 * 64, INT_MIN);
    if (getenv("TRACE")) { int wk, bk, rk; sscanf(getenv("TRACE"), "%d %d %d", &wk, &bk, &rk);
        auto nm = [](int q) { string x; x += char('a' + F(q)); x += char('1' + Rk(q)); return x; };
        for (int p = 0; p < 30; p++) { Mv m = ruleMove(wk, bk, rk); printf("%s%s ", m.piece ? "R" : "K", nm(m.to).c_str()); if (m.piece) rk = m.to; else wk = m.to;
            vector<int> bm; blackMoves(wk, bk, rk, bm); if (bm.empty()) { printf("#\n"); break; } int pick = -1, pv = -1; for (int s : bm) { int d = DW[idx(wk, s, rk)]; if (d > pv) { pv = d; pick = s; } } bk = pick; printf("k%s(%d)  ", nm(bk).c_str(), boxArea(wk, bk, rk)); }
        printf("\n"); return 0; }
    long long winW = 0, winB = 0, drawW = 0, drawB = 0; int maxW = 0, maxB = 0;
    for (int i = 0; i < 64 * 64 * 64; i++) { int wk = i / 4096, bk = (i / 64) % 64, rk = i % 64;
        if (validW(wk, bk, rk)) { if (DW[i] >= 0) { winW++; maxW = max(maxW, DW[i]); } else drawW++; }
        if (validB(wk, bk, rk)) { if (DB[i] >= 0) { winB++; maxB = max(maxB, DB[i]); } else drawB++; } }
    printf("retro: white-to-move win %lld draw %lld (max DTM %d plies) | black-to-move lost %lld draw %lld (max %d plies)  %.1fs\n", winW, drawW, maxW, winB, drawB, maxB, chrono::duration<double>(chrono::steady_clock::now() - t0).count());
    if (getenv("MAXPOS")) { int c = 0;
        for (int i = 0; i < 64 * 64 * 64; i++) { if (DW[i] != maxW || !validW(i / 4096, (i / 64) % 64, i % 64)) continue; c++;
            if (c <= 12) { char b[64]; memset(b, 0, 64); b[i / 4096] = 'K'; b[(i / 64) % 64] = 'k'; b[i % 64] = 'R'; string f;
                for (int r = 7; r >= 0; r--) { int e = 0; for (int x = 0; x < 8; x++) { char q = b[r * 8 + x]; if (!q) e++; else { if (e) f += char('0' + e); e = 0; f += q; } } if (e) f += char('0' + e); if (r) f += '/'; }
                printf("%s w - - 0 1\n", f.c_str()); } }
        printf("count %d, mate in %d moves\n", c, (maxW + 1) / 2); return 0; }
    long long ok = 0, fail = 0, slow = 0, tot = 0; int maxPly = 0; vector<string> ex;
    for (int wk = 0; wk < 64; wk++) for (int bk = 0; bk < 64; bk++) for (int rk = 0; rk < 64; rk++) for (int stm = 0; stm < 2; stm++) {
        int i = idx(wk, bk, rk), r;
        if (stm == 0) { if (!validW(wk, bk, rk) || DW[i] < 0) continue; r = playFromW(wk, bk, rk, LIMIT); }
        else { if (!validB(wk, bk, rk) || DB[i] < 0) continue;
            vector<int> bm; blackMoves(wk, bk, rk, bm); if (bm.empty()) { r = 0; }
            else { int pick = -1, pv = -1; for (int s : bm) { int d = DW[idx(wk, s, rk)]; if (d > pv) { pv = d; pick = s; } } r = playFromW(wk, pick, rk, LIMIT - 1); if (r >= 0) r++; } }
        tot++;
        if (r >= 0) { ok++; maxPly = max(maxPly, r); }
        else { if (r == -2) slow++; else fail++; if (ex.size() < 8) { char b[80]; snprintf(b, 80, "wk=%c%d bk=%c%d rk=%c%d %s %s", 'a' + F(wk), Rk(wk) + 1, 'a' + F(bk), Rk(bk) + 1, 'a' + F(rk), Rk(rk) + 1, stm ? "b" : "w", r == -2 ? "no-mate-in-limit" : "lost-rook/stalemate"); ex.push_back(b); } }
    }
    printf("depth %d moves: won %lld / %lld (%.4f%%), no mate in 53 moves %lld, rook lost or stalemate %lld, longest win %d plies  %.1fs\n", DEPTH, ok, tot, 100.0 * ok / tot, slow, fail, maxPly, chrono::duration<double>(chrono::steady_clock::now() - t0).count());
    for (auto& e : ex) printf("  %s\n", e.c_str());
}
