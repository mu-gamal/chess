#define NO_MAIN
#include "three.cpp"
static const long long NX = 64LL * 64 * 64 * 64;
static inline long long ix(int a, int b, int c, int d) { return ((long long)(a * 64 + b) * 64 + c) * 64 + d; }
static inline int fl(int q) { return q ^ 56; }
static bool batt(int x, int t, uint64_t occ) {
    if (x < 0 || x == t) return false; int df = F(t) - F(x), dr = Rk(t) - Rk(x); if (abs(df) != abs(dr)) return false;
    int sf = (df > 0) - (df < 0), sr = (dr > 0) - (dr < 0), f = F(x) + sf, r = Rk(x) + sr;
    while (true) { int s = r * 8 + f; if (s == t) return true; if (occ >> s & 1) return false; f += sf; r += sr; }
}
static bool pawnAtt(int p, int t) { return p >= 0 && Rk(t) == Rk(p) - 1 && abs(F(t) - F(p)) == 1; }
struct S { int wk, bk, b, p; };
static uint64_t occ(const S& s) { uint64_t o = (1ULL << s.wk) | (1ULL << s.bk); if (s.b >= 0) o |= 1ULL << s.b; if (s.p >= 0) o |= 1ULL << s.p; return o; }
static bool holds(const S& s) {
    if (s.p < 0) return true; if (s.b < 0) return false;
    if (dist(s.b, s.bk) == 1 && dist(s.b, s.wk) > 1) return false;
    if (pawnAtt(s.p, s.b)) return false;
    uint64_t o = occ(s);
    for (int rr = Rk(s.p) - 1; rr >= 0; rr--) { int t = rr * 8 + F(s.p); if (t == s.wk) return true; if (t == s.bk) continue; if (t == s.b || batt(s.b, t, o)) return true; }
    return false;
}
static void wmoves(const S& s, vector<S>& out) { out.clear(); uint64_t o = occ(s);
    for (auto& d : KD) { int f = F(s.wk) + d[0], r = Rk(s.wk) + d[1]; if (f < 0 || f > 7 || r < 0 || r > 7) continue; int t = r * 8 + f; if (t == s.b || dist(t, s.bk) <= 1 || pawnAtt(s.p, t)) continue; S q = s; q.wk = t; if (t == s.p) q.p = -1; out.push_back(q); }
    if (s.b >= 0) for (int dd : {9, 7, -7, -9}) { int f = F(s.b), r = Rk(s.b); int df = (dd == 9 || dd == -7) ? 1 : -1, dr = dd > 0 ? 1 : -1;
        while (true) { f += df; r += dr; if (f < 0 || f > 7 || r < 0 || r > 7) break; int t = r * 8 + f; if (t == s.wk || t == s.bk) break; S q = s; q.b = t; if (t == s.p) { q.p = -1; out.push_back(q); break; } out.push_back(q); if (o >> t & 1) break; } } }
static int pawnMovesTo(int p, int t) { int d = Rk(p) - Rk(t); if (Rk(p) == 6 && d >= 2) d--; return d; }
static int bishopReach(const S& s, int t, int maxm) {
    uint64_t o = occ(s); vector<int> dd(64, 99); deque<int> q; dd[s.b] = 0; q.push_back(s.b);
    while (!q.empty()) { int x = q.front(); q.pop_front(); if (dd[x] >= maxm) continue;
        for (int y = 0; y < 64; y++) if (dd[y] == 99 && !(o >> y & 1) && batt(x, y, o & ~(1ULL << s.b))) { dd[y] = dd[x] + 1; q.push_back(y); } }
    int best = 99; for (int y = 0; y < 64; y++) if (dd[y] < 99 && (y == t || batt(y, t, (o & ~(1ULL << s.b)) | (1ULL << y)))) best = min(best, dd[y]);
    return best;
}
static bool raceDraw(const S& s) {
    if (s.p < 0) return true; if (s.b < 0) return false;
    for (int rr = Rk(s.p) - 1; rr >= 0; rr--) { int t = rr * 8 + F(s.p); int pm = pawnMovesTo(s.p, t);
        if (dist(s.wk, t) <= pm) return true;
        if (t != s.bk && bishopReach(s, t, 3) <= pm) return true; }
    return false;
}
static bool wtmDraw(const S& s, vector<S>& mv) { if (raceDraw(s)) return true; wmoves(s, mv); for (auto& q : mv) if (holds(q)) return true; return false; }
static bool btmDraw(const S& s) {
    vector<S> bm, tmp; uint64_t o = occ(s);
    for (auto& d : KD) { int f = F(s.bk) + d[0], r = Rk(s.bk) + d[1]; if (f < 0 || f > 7 || r < 0 || r > 7) continue; int t = r * 8 + f;
        if (t == s.p || dist(t, s.wk) <= 1) continue; uint64_t o2 = (o & ~(1ULL << s.bk)); if (t != s.b && batt(s.b, t, o2)) continue; S q = s; q.bk = t;
        if (t == s.b) { q.b = -1; if (DB[P][idx(fl(q.bk), fl(q.wk), fl(q.p))] >= 0) return false; continue; }
        bm.push_back(q); }
    int t1 = s.p - 8;
    if (!(o >> t1 & 1)) { S q = s; q.p = t1;
        if (Rk(t1) == 0) { bool saved = batt(s.b, t1, o) || (dist(s.wk, t1) == 1 && dist(s.bk, t1) > 1); if (!saved) return false; }
        else { bm.push_back(q); if (Rk(s.p) == 6 && !(o >> (t1 - 8) & 1)) { S q2 = s; q2.p = t1 - 8; bm.push_back(q2); } } }
    for (auto& q : bm) if (!wtmDraw(q, tmp)) return false;
    return true;
}
int main() {
    for (int t : {Q, R, B, Nn}) retro(t);
    retro(P);
    vector<int8_t> R0(NX), D0(NX), R1(NX), D1(NX);
    FILE* fp = fopen("/tmp/tb4/KBvKP.xy", "rb");
    if (fread(R0.data(), 1, NX, fp) != (size_t)NX || fread(D0.data(), 1, NX, fp) != (size_t)NX || fread(R1.data(), 1, NX, fp) != (size_t)NX || fread(D1.data(), 1, NX, fp) != (size_t)NX) return 1; fclose(fp);
    long long tot[2] = {0, 0}, ok[2] = {0, 0}, fd[2] = {0, 0}, fb[2] = {0, 0}; vector<S> mv;
    for (long long i = 0; i < NX; i++) {
        S s{(int)(i >> 18), (int)((i >> 12) & 63), (int)((i >> 6) & 63), (int)(i & 63)};
        if (s.wk == s.bk || s.wk == s.b || s.wk == s.p || s.bk == s.b || s.bk == s.p || s.b == s.p || dist(s.wk, s.bk) <= 1 || Rk(s.p) < 1 || Rk(s.p) > 6) continue;
        uint64_t o = occ(s);
        if (!batt(s.b, s.bk, o)) {
            bool bw = R0[i] == 2; bool predDraw = wtmDraw(s, mv);
            tot[0]++; if (predDraw == !bw) ok[0]++; else if (bw) fd[0]++; else fb[0]++;
        }
        if (!pawnAtt(s.p, s.wk)) {
            bool bw = R1[i] == 1; bool predDraw = btmDraw(s);
            tot[1]++; if (predDraw == !bw) ok[1]++; else if (bw) fd[1]++; else fb[1]++;
        }
    }
    for (int k = 0; k < 2; k++) printf("%s to move: %lld positions, rule right %.2f%%, rule says draw but black wins %lld, rule says black wins but draw %lld\n", k ? "black" : "white", tot[k], 100.0 * ok[k] / tot[k], fd[k], fb[k]);
}
