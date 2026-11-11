#define NO_MAIN
#include "three.cpp"
#include <thread>
extern "C" {
#include "fathom/tbprobe.h"
}
static const long long NX = 64LL * 64 * 64 * 64;
static inline long long ix(int a, int b, int c, int d) { return ((long long)(a * 64 + b) * 64 + c) * 64 + d; }
static inline int fl(int q) { return q ^ 56; }
static inline uint64_t bt(int s) { return 1ULL << s; }
static int TA, TC;
struct Tab { vector<int8_t> R[2], D[2]; };
static Tab CUR;
static map<pair<int,int>, Tab*> LOADED;
static string TBDIR4;
static const char* NM = "QRBNP";

static bool att(int type, int side, int x, int t, uint64_t occ) {
    if (x < 0 || x == t) return false;
    if (type == P) return Rk(t) == Rk(x) + (side == 0 ? 1 : -1) && abs(F(t) - F(x)) == 1;
    if (type == Nn) { int a = abs(F(x) - F(t)), b = abs(Rk(x) - Rk(t)); return (a == 1 && b == 2) || (a == 2 && b == 1); }
    int df = F(t) - F(x), dr = Rk(t) - Rk(x);
    bool o = !df || !dr, d = abs(df) == abs(dr);
    if (!((o && type != B) || (d && type != R))) return false;
    int sf = (df > 0) - (df < 0), sr = (dr > 0) - (dr < 0), f = F(x) + sf, r = Rk(x) + sr;
    while (true) { int s = r * 8 + f; if (s == t) return true; if (occ >> s & 1) return false; f += sf; r += sr; }
}
struct Pos { int k[2], p[2]; };
static int typ(int side) { return side ? TC : TA; }
static uint64_t occOf(const Pos& PS) { uint64_t o = bt(PS.k[0]) | bt(PS.k[1]); if (PS.p[0] >= 0) o |= bt(PS.p[0]); if (PS.p[1] >= 0) o |= bt(PS.p[1]); return o; }
static bool inCheck(const Pos& PS, int side, int otype) { int o = 1 - side; return PS.p[o] >= 0 && att(otype, o, PS.p[o], PS.k[side], occOf(PS)); }
static bool pawnOK(int type, int sq) { return type != P || (Rk(sq) >= 1 && Rk(sq) <= 6); }
static bool validP(const Pos& PS, int stm) {
    int s[4] = {PS.k[0], PS.k[1], PS.p[0], PS.p[1]};
    for (int i = 0; i < 4; i++) for (int j = i + 1; j < 4; j++) if (s[i] == s[j]) return false;
    if (dist(PS.k[0], PS.k[1]) <= 1) return false;
    if (!pawnOK(TA, PS.p[0]) || !pawnOK(TC, PS.p[1])) return false;
    return !inCheck(PS, 1 - stm, typ(stm));
}
static Pos dec(long long i) { Pos PS; PS.k[0] = i >> 18; PS.k[1] = (i >> 12) & 63; PS.p[0] = (i >> 6) & 63; PS.p[1] = i & 63; return PS; }
static long long enc(const Pos& PS) { return ix(PS.k[0], PS.k[1], PS.p[0], PS.p[1]); }

static pair<int,int> getTab(int A, int C, int s, const Pos& q) {
    auto it = LOADED.find({A, C});
    if (it != LOADED.end()) { long long j = enc(q); return {it->second->R[s][j], it->second->D[s][j]}; }
    it = LOADED.find({C, A});
    if (it == LOADED.end()) { fprintf(stderr, "missing table K%cvK%c\n", NM[A], NM[C]); exit(1); }
    long long j = ix(fl(q.k[1]), fl(q.k[0]), fl(q.p[1]), fl(q.p[0])); return {it->second->R[1 - s][j], it->second->D[1 - s][j]};
}
static pair<int,int> cap3(int side, int t, const Pos& q) {
    int v = side == 0 ? DB[t][idx(q.k[0], q.k[1], q.p[0])] : DB[t][idx(fl(q.k[1]), fl(q.k[0]), fl(q.p[1]))];
    return v >= 0 ? make_pair(2, v) : make_pair(0, 0);
}
enum { MNORM, MCAP, MPROMO, MEP };
struct Mv2 { Pos q; int kind, ptype, epsq; };
static void gen0(const Pos& PS, int side, vector<Mv2>& out);
static void gen(const Pos& PS, int side, vector<Mv2>& out) {
    gen0(PS, side, out);
    if (side != 0 || TC != B || PS.p[1] < 0) return;
    int bc = (F(PS.p[1]) + Rk(PS.p[1])) & 1; vector<Mv2> keep;
    for (auto& m : out) { if (m.kind == MCAP) { keep.push_back(m); continue; } int dst = m.q.k[0] != PS.k[0] ? m.q.k[0] : m.q.p[0]; if (((F(dst) + Rk(dst)) & 1) != bc) keep.push_back(m); }
    out.swap(keep);
}
static void gen0(const Pos& PS, int side, vector<Mv2>& out) {
    out.clear(); int o = 1 - side, myt = typ(side), ot = typ(o);
    auto legal = [&](const Pos& q, bool capd) { if (dist(q.k[0], q.k[1]) <= 1) return false; if (capd) return true; return !inCheck(q, side, ot); };
    for (auto& d : KD) { int f = F(PS.k[side]) + d[0], r = Rk(PS.k[side]) + d[1]; if (!onBoard(f, r)) continue; int s = r * 8 + f;
        if (s == PS.p[side] || s == PS.k[o]) continue; Pos q = PS; q.k[side] = s; bool cap = s == PS.p[o]; if (cap) q.p[o] = -1;
        if (legal(q, cap)) out.push_back({q, cap ? MCAP : MNORM, myt, -1}); }
    int x = PS.p[side]; uint64_t occ = occOf(PS);
    if (myt == P) {
        int dir = side == 0 ? 8 : -8, last = side == 0 ? 7 : 0, start = side == 0 ? 1 : 6;
        int s = x + dir;
        if (!(occ >> s & 1)) {
            Pos q = PS; q.p[side] = s;
            if (legal(q, false)) { if (Rk(s) == last) { for (int pr : {Q, R, B, Nn}) out.push_back({q, MPROMO, pr, -1}); } else out.push_back({q, MNORM, P, -1}); }
            if (Rk(x) == start && !(occ >> (s + dir) & 1)) { Pos q2 = PS; q2.p[side] = s + dir;
                if (legal(q2, false)) { int ep = -1; if (ot == P && Rk(PS.p[o]) == Rk(s + dir) && abs(F(PS.p[o]) - F(s + dir)) == 1) ep = s; out.push_back({q2, MNORM, P, ep}); } }
        }
        for (int df : {-1, 1}) { int f = F(x) + df; if (f < 0 || f > 7) continue; int t = s + df; if (t != PS.p[o]) continue;
            Pos q = PS; q.p[side] = t; q.p[o] = -1; if (!legal(q, true)) continue;
            if (Rk(t) == last) { for (int pr : {Q, R, B, Nn}) out.push_back({q, MCAP, pr, -1}); } else out.push_back({q, MCAP, P, -1}); }
        return;
    }
    auto tryTo = [&](int s) { if (s == PS.k[0] || s == PS.k[1]) return false; Pos q = PS; q.p[side] = s; bool cap = s == PS.p[o]; if (cap) q.p[o] = -1; if (legal(q, cap)) out.push_back({q, cap ? MCAP : MNORM, myt, -1}); return !cap; };
    if (myt == Nn) { for (auto& d : ND) { int f = F(x) + d[0], r = Rk(x) + d[1]; if (!onBoard(f, r)) continue; tryTo(r * 8 + f); } return; }
    for (auto& d : KD) { bool dg = d[0] && d[1]; if ((myt == R && dg) || (myt == B && !dg)) continue;
        int f = F(x) + d[0], r = Rk(x) + d[1];
        while (onBoard(f, r)) { int s = r * 8 + f; if (s == PS.k[0] || s == PS.k[1]) break; if (!tryTo(s)) break; if (occ >> s & 1) break; f += d[0]; r += d[1]; } }
}
static pair<int,int> epOption(int capSide, const Pos& q, int epsq) {
    Pos r = q; r.p[capSide] = epsq; r.p[1 - capSide] = -1;
    auto v = cap3(capSide, P, r);
    if (v.first == 2) return {1, v.second + 1};
    return {0, 0};
}
static pair<int,int> childVal(int side, const Mv2& m) {
    int o = 1 - side;
    if (m.kind == MCAP) return cap3(side, m.ptype, m.q);
    if (m.kind == MPROMO) return side == 0 ? getTab(m.ptype, TC, o, m.q) : getTab(TA, m.ptype, o, m.q);
    long long j = enc(m.q); pair<int,int> v = {CUR.R[o][j], CUR.D[o][j]};
    if (m.epsq >= 0) { auto e = epOption(o, m.q, m.epsq);
        if (e.first == 1) { if (v.first == 1) v.second = min(v.second, e.second); else v = e; }
        else if (v.first == 2) v = {0, 0}; }
    return v;
}
static Tab* loadTab(int A, int C) {
    string fn = TBDIR4 + "/K" + NM[A] + "vK" + NM[C] + ".xy";
    FILE* fp = fopen(fn.c_str(), "rb"); if (!fp) return nullptr;
    Tab* t = new Tab; for (int s = 0; s < 2; s++) { t->R[s].resize(NX); t->D[s].resize(NX); fread(t->R[s].data(), 1, NX, fp); fread(t->D[s].data(), 1, NX, fp); }
    fclose(fp); return t;
}
static string fenX(const Pos& PS, int stm, int ep) {
    char bd[64]; memset(bd, 0, 64); bd[PS.k[0]] = 'K'; bd[PS.k[1]] = 'k'; bd[PS.p[0]] = "QRBNP"[TA]; bd[PS.p[1]] = "qrbnp"[TC]; string f;
    for (int r = 7; r >= 0; r--) { int e = 0; for (int x = 0; x < 8; x++) { char q = bd[r * 8 + x]; if (!q) e++; else { if (e) f += char('0' + e); e = 0; f += q; } } if (e) f += char('0' + e); if (r) f += '/'; }
    string eps = "-"; if (ep >= 0) { eps = ""; eps += char('a' + F(ep)); eps += char('1' + Rk(ep)); }
    return f + (stm ? " b" : " w") + " - " + eps + " 0 1";
}
static int epSquareFor(const Pos& PS, int stm) {
    int o = 1 - stm; if (TA != P || TC != P) return -1;
    int op = PS.p[o], mp = PS.p[stm];
    int r4 = o == 0 ? 3 : 4; if (Rk(op) != r4 || Rk(mp) != r4 || abs(F(op) - F(mp)) != 1) return -1;
    int dir = o == 0 ? 8 : -8; int skip = op - dir, orig = op - 2 * dir;
    uint64_t occ = occOf(PS); if ((occ >> skip & 1) || (occ >> orig & 1)) return -1;
    return skip;
}
int main(int argc, char** argv) {
    auto t0 = chrono::steady_clock::now();
    string nm = argv[1]; auto ty = [](char c) { return (int)string("QRBNP").find(c); };
    TA = ty(nm[0]); TC = ty(nm[1]); TBDIR4 = getenv("TB4") ? getenv("TB4") : "/tmp/tb4";
    for (int t : {Q, R, B, Nn}) retro(t);
    retro(P);
    int maxDep = 0; for (int t = 0; t < 5; t++) for (int v : DB[t]) maxDep = max(maxDep, v);
    vector<pair<int,int>> need;
    if (TA == P) for (int pr : {Q, R, B, Nn}) need.push_back({pr, TC});
    if (TC == P) for (int pr : {Q, R, B, Nn}) need.push_back({TA, pr});
    for (auto& pq : need) { if (LOADED.count(pq) || LOADED.count({pq.second, pq.first})) continue;
        Tab* t = loadTab(pq.first, pq.second); pair<int,int> key = pq;
        if (!t) { t = loadTab(pq.second, pq.first); key = {pq.second, pq.first}; }
        if (!t) { printf("missing dependency K%cvK%c\n", NM[pq.first], NM[pq.second]); return 1; }
        LOADED[key] = t; for (int s = 0; s < 2; s++) for (int8_t v : t->D[s]) maxDep = max(maxDep, (int)v); }
    for (int s = 0; s < 2; s++) { CUR.R[s].assign(NX, 0); CUR.D[s].assign(NX, 0); }
    { vector<Mv2> mv; for (int s = 0; s < 2; s++) for (long long i = 0; i < NX; i++) { Pos PS = dec(i); if (!validP(PS, s)) continue; gen(PS, s, mv); if (mv.empty() && inCheck(PS, s, typ(1 - s))) CUR.R[s][i] = 2; } }
    for (int n = 0; n < 125; n++) {
        const int T = 4; vector<vector<pair<long long, int8_t>>> up(T * 2); vector<thread> th;
        for (int t = 0; t < T; t++) th.emplace_back([&, t] { vector<Mv2> mv;
            for (int s = 0; s < 2; s++) for (long long i = t; i < NX; i += T) { if (CUR.R[s][i]) continue; Pos PS = dec(i); if (!validP(PS, s)) continue; gen(PS, s, mv); if (mv.empty()) continue;
                bool allW = true, win = false; int mx = -1;
                for (auto& m : mv) { auto v = childVal(s, m);
                    if (v.first == 2 && v.second == n) { win = true; break; }
                    if (v.first != 1) allW = false; else mx = max(mx, v.second); }
                if (win) up[t * 2 + s].push_back({i, 1}); else if (allW && mx == n) up[t * 2 + s].push_back({i, 2}); } });
        for (auto& x : th) x.join();
        bool ch = false;
        for (int t = 0; t < T; t++) for (int s = 0; s < 2; s++) for (auto& u : up[t * 2 + s]) { CUR.R[s][u.first] = u.second; CUR.D[s][u.first] = n + 1; ch = true; }
        if (!ch && n > maxDep) break;
    }
    if (getenv("DUMP")) { string fn = TBDIR4 + "/K" + NM[TA] + "vK" + NM[TC] + ".xy"; FILE* fp = fopen(fn.c_str(), "wb"); for (int s = 0; s < 2; s++) { fwrite(CUR.R[s].data(), 1, NX, fp); fwrite(CUR.D[s].data(), 1, NX, fp); } fclose(fp); }
    long long ww = 0, dr = 0, bw = 0, epn = 0, mm = 0, nn = 0; int mxw = 0, mxb = 0; string fw = "-", fb = "-";
    bool chk = getenv("TBDIR") != nullptr; if (chk) tb_init(getenv("TBDIR"));
    for (int s = 0; s < 2; s++) for (long long i = 0; i < NX; i++) { Pos PS = dec(i); if (!validP(PS, s)) continue;
        for (int pass = 0; pass < 2; pass++) {
            int r = CUR.R[s][i], d = CUR.D[s][i], ep = -1;
            if (pass == 1) { ep = epSquareFor(PS, s); if (ep < 0) continue; epn++;
                auto e = epOption(s, PS, ep); pair<int,int> v = {r, d};
                if (e.first == 1) { if (v.first == 1) v.second = min(v.second, e.second); else v = e; } else if (v.first == 2) v = {0, 0};
                r = v.first; d = v.second; }
            bool whiteWins = (s == 0 && r == 1) || (s == 1 && r == 2), blackWins = (s == 1 && r == 1) || (s == 0 && r == 2);
            if (whiteWins) { ww++; if (s == 0 && r == 1 && d > mxw) { mxw = d; fw = fenX(PS, s, ep); } }
            else if (blackWins) { bw++; if (s == 1 && r == 1 && d > mxb) { mxb = d; fb = fenX(PS, s, ep); } }
            else dr++;
            if (chk) { auto tbb = [&](int t) { uint64_t x = 0; if (TA == t) x |= bt(PS.p[0]); if (TC == t) x |= bt(PS.p[1]); return x; };
                unsigned res = tb_probe_wdl(bt(PS.k[0]) | bt(PS.p[0]), bt(PS.k[1]) | bt(PS.p[1]), bt(PS.k[0]) | bt(PS.k[1]), tbb(Q), tbb(R), tbb(B), tbb(Nn), tbb(P), 0, 0, ep < 0 ? 0 : ep, s == 0); nn++;
                int th2 = (res == TB_WIN || res == TB_CURSED_WIN) ? 1 : (res == TB_LOSS || res == TB_BLESSED_LOSS) ? 2 : 0;
                if (th2 != r) { mm++; if (mm <= 8) printf("  %s ours=%d syzygy=%u\n", fenX(PS, s, ep).c_str(), r, res); } } } }
    if (chk) printf("syzygy check: %lld positions, mismatches %lld\n", nn, mm);
    printf("K%cvK%c | total %lld (en passant %lld) | white wins %lld | draws %lld | black wins %lld | longest white mate %d moves %s | longest black mate %d moves %s | %.1fs\n",
        NM[TA], NM[TC], ww + dr + bw, epn, ww, dr, bw, (mxw + 1) / 2, fw.c_str(), (mxb + 1) / 2, fb.c_str(), chrono::duration<double>(chrono::steady_clock::now() - t0).count());
}
