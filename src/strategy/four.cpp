#define NO_MAIN
#include "three.cpp"

static const long long N4 = 64LL * 64 * 64 * 64;
static inline long long id4(int wk, int bk, int a, int b) { return ((long long)(wk * 64 + bk) * 64 + a) * 64 + b; }
static int TA, TB;
static vector<int8_t> W4, B4;
static map<pair<int,int>, vector<int8_t>> STB;
static int lookB(int t1, int s1, int t2, int s2, int wk, int bk) { if (t1 > t2 || (t1 == t2 && s1 > s2)) { swap(t1, t2); swap(s1, s2); } return STB[{t1, t2}][id4(wk, bk, s1, s2)]; }

static bool rayAtt(int type, int x, int t, uint64_t occ) {
    if (x == t) return false;
    if (type == Nn) { int a = abs(F(x) - F(t)), b = abs(Rk(x) - Rk(t)); return (a == 1 && b == 2) || (a == 2 && b == 1); }
    if (type == P) return Rk(t) == Rk(x) + 1 && abs(F(t) - F(x)) == 1;
    int df = F(t) - F(x), dr = Rk(t) - Rk(x);
    bool o = !df || !dr, d = abs(df) == abs(dr);
    if (!((o && type != B) || (d && type != R))) return false;
    int sf = (df > 0) - (df < 0), sr = (dr > 0) - (dr < 0), f = F(x) + sf, r = Rk(x) + sr;
    while (true) { int s = r * 8 + f; if (s == t) return true; if (occ >> s & 1) return false; f += sf; r += sr; }
}
static inline uint64_t bit(int s) { return 1ULL << s; }
static bool canon(int a, int b) { return TA != TB || a < b; }
static void norm(int& a, int& b) { if (TA == TB && a > b) swap(a, b); }
static bool valid4(int wk, int bk, int a, int b) {
    if (wk == bk || wk == a || wk == b || bk == a || bk == b || a == b) return false;
    if (dist(wk, bk) <= 1) return false;
    if ((TA == P && (Rk(a) < 1 || Rk(a) > 6)) || (TB == P && (Rk(b) < 1 || Rk(b) > 6))) return false;
    return canon(a, b);
}
static bool blackChecked(int wk, int bk, int a, int b) {
    uint64_t occ = bit(wk) | bit(a) | bit(b) | bit(bk);
    return rayAtt(TA, a, bk, occ) || rayAtt(TB, b, bk, occ);
}
struct BM { int s, cap; };
static void bMoves(int wk, int bk, int a, int b, vector<BM>& out) {
    out.clear();
    for (auto& d : KD) { int f = F(bk) + d[0], r = Rk(bk) + d[1]; if (!onBoard(f, r)) continue; int s = r * 8 + f;
        if (dist(s, wk) <= 1) continue;
        uint64_t occ = bit(wk) | bit(a) | bit(b);
        if (s == a) { occ &= ~bit(a); if (rayAtt(TB, b, s, occ)) continue; out.push_back({s, 1}); continue; }
        if (s == b) { occ &= ~bit(b); if (rayAtt(TA, a, s, occ)) continue; out.push_back({s, 2}); continue; }
        if (rayAtt(TA, a, s, occ) || rayAtt(TB, b, s, occ)) continue;
        out.push_back({s, 0}); }
}
struct WM { int wk, a, b, promo; };
static void slideMoves(int type, int x, uint64_t occ, vector<int>& out) {
    out.clear();
    if (type == Nn) { for (auto& d : ND) { int f = F(x) + d[0], r = Rk(x) + d[1]; if (!onBoard(f, r)) continue; int s = r * 8 + f; if (!(occ >> s & 1)) out.push_back(s); } return; }
    for (auto& d : KD) { bool dg = d[0] && d[1]; if ((type == R && dg) || (type == B && !dg)) continue;
        int f = F(x) + d[0], r = Rk(x) + d[1];
        while (onBoard(f, r)) { int s = r * 8 + f; if (occ >> s & 1) break; out.push_back(s); f += d[0]; r += d[1]; } }
}
static void wMoves(int wk, int bk, int a, int b, vector<WM>& out) {
    out.clear(); vector<int> t;
    for (auto& d : KD) { int f = F(wk) + d[0], r = Rk(wk) + d[1]; if (!onBoard(f, r)) continue; int s = r * 8 + f; if (s == a || s == b || dist(s, bk) <= 1) continue; out.push_back({s, a, b, -1}); }
    uint64_t occ = bit(wk) | bit(bk) | bit(a) | bit(b);
    for (int side = 0; side < 2; side++) {
        int type = side ? TB : TA, x = side ? b : a, other = side ? a : b;
        auto add = [&](int s, int pr) { int na = side ? a : s, nb = side ? s : b; if (pr < 0) norm(na, nb); out.push_back({wk, na, nb, pr}); };
        if (type == P) {
            int s = x + 8; if (occ >> s & 1) continue;
            if (Rk(s) == 7) { for (int pr : {Q, R, B, Nn}) add(s, pr); continue; }
            add(s, -1);
            if (Rk(x) == 1 && !(occ >> (x + 16) & 1)) add(x + 16, -1);
            continue;
        }
        slideMoves(type, x, occ, t); for (int s : t) add(s, -1);
    }
}
static void retro4() {
    W4.assign(N4, -1); B4.assign(N4, -1);
    vector<BM> bm; vector<WM> wm;
    for (long long i = 0; i < N4; i++) { int wk = i >> 18, bk = (i >> 12) & 63, a = (i >> 6) & 63, b = i & 63;
        if (!valid4(wk, bk, a, b)) continue; bMoves(wk, bk, a, b, bm);
        if (bm.empty() && blackChecked(wk, bk, a, b)) B4[i] = 0; }
    int maxDep = 0;
    for (int t = 0; t < 5; t++) for (int v : DW[t]) maxDep = max(maxDep, v);
    for (auto& kv : STB) for (int8_t v : kv.second) maxDep = max(maxDep, (int)v);
    for (int n = 0; ; n += 2) {
        bool ch = false;
        for (long long i = 0; i < N4; i++) { if (W4[i] >= 0) continue; int wk = i >> 18, bk = (i >> 12) & 63, a = (i >> 6) & 63, b = i & 63;
            if (!valid4(wk, bk, a, b) || blackChecked(wk, bk, a, b)) continue; wMoves(wk, bk, a, b, wm);
            for (auto& m : wm) { int v; if (m.promo < 0) v = B4[id4(m.wk, bk, m.a, m.b)]; else if (Rk(m.a) == 7 && TA == P) v = lookB(m.promo, m.a, TB, m.b, m.wk, bk); else v = lookB(TA, m.a, m.promo, m.b, m.wk, bk); if (v == n) { W4[i] = n + 1; ch = true; break; } } }
        for (long long i = 0; i < N4; i++) { if (B4[i] >= 0) continue; int wk = i >> 18, bk = (i >> 12) & 63, a = (i >> 6) & 63, b = i & 63;
            if (!valid4(wk, bk, a, b)) continue; bMoves(wk, bk, a, b, bm); if (bm.empty()) continue;
            bool all = true; int mx = -1;
            for (auto& m : bm) { int d;
                if (m.cap == 1) d = DW[TB][idx(wk, m.s, b)];
                else if (m.cap == 2) d = DW[TA][idx(wk, m.s, a)];
                else { int x = a, y = b; d = W4[id4(wk, m.s, x, y)]; }
                if (d < 0) { all = false; break; } mx = max(mx, d); }
            if (all && mx == n + 1) { B4[i] = n + 2; ch = true; } }
        if (!ch && n > maxDep) break;
    }
}
static string fen4(long long i) {
    int wk = i >> 18, bk = (i >> 12) & 63, a = (i >> 6) & 63, b = i & 63;
    char bd[64]; memset(bd, 0, 64); bd[wk] = 'K'; bd[bk] = 'k'; bd[a] = "QRBNP"[TA]; bd[b] = "QRBNP"[TB]; string f;
    for (int r = 7; r >= 0; r--) { int e = 0; for (int x = 0; x < 8; x++) { char q = bd[r * 8 + x]; if (!q) e++; else { if (e) f += char('0' + e); e = 0; f += q; } } if (e) f += char('0' + e); if (r) f += '/'; }
    return f + " w - - 0 1";
}
#ifndef NO_MAIN4
int main(int argc, char** argv) {
    auto t0 = chrono::steady_clock::now();
    for (int t : {Q, R, B, Nn}) retro(t);
    retro(P);
    string nm = argc > 1 ? argv[1] : "QQ"; auto ty = [](char c) { return (int)string("QRBNP").find(c); };
    int ta = ty(nm[0]), tb = ty(nm[1]);
    if (ta == P && tb == P) { for (int pr : {Q, R, B, Nn}) { string fn = string(getenv("TBDIR")) + "/K" + "QRBN"[pr] + "PvK.b4"; FILE* fp = fopen(fn.c_str(), "rb"); if (!fp) { printf("missing %s\n", fn.c_str()); return 1; } vector<int8_t> v(N4); fread(v.data(), 1, N4, fp); fclose(fp); STB[{pr, P}] = v; } }
    else if (tb == P) { for (int pr : {Q, R, B, Nn}) { TA = min(ta, pr); TB = max(ta, pr); retro4(); STB[{TA, TB}] = B4; } }
    TA = ta; TB = tb;
    retro4();
    if (getenv("DUMP")) { FILE* fp = fopen(getenv("DUMP"), "wb"); fwrite(B4.data(), 1, N4, fp); fclose(fp); }
    long long ww = 0, wd = 0, bl = 0, bd = 0, mi = -1; int mx = 0;
    for (long long i = 0; i < N4; i++) { int wk = i >> 18, bk = (i >> 12) & 63, a = (i >> 6) & 63, b = i & 63;
        if (!valid4(wk, bk, a, b)) continue;
        if (!blackChecked(wk, bk, a, b)) { if (W4[i] >= 0) { ww++; if (W4[i] > mx) { mx = W4[i]; mi = i; } } else wd++; }
        if (B4[i] >= 0) bl++; else bd++; }
    printf("K%svK | total %lld | wins %lld (wtm %lld + btm %lld) | draws %lld (wtm %lld + btm %lld) | max mate %d moves | %s | %.1fs\n",
        nm.c_str(), ww + wd + bl + bd, ww + bl, ww, bl, wd + bd, wd, bd, (mx + 1) / 2, mi >= 0 ? fen4(mi).c_str() : "-", chrono::duration<double>(chrono::steady_clock::now() - t0).count());
}

#endif
