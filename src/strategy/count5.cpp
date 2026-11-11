#include <bits/stdc++.h>
#include <thread>
extern "C" {
#include "fathom/tbprobe.h"
}
using namespace std;
enum { Q, R, B, N, P };
static inline int F(int s) { return s & 7; }
static inline int Rk(int s) { return s >> 3; }
static inline int dist(int a, int b) { return max(abs(F(a) - F(b)), abs(Rk(a) - Rk(b))); }
struct Pc { int color, type; };
static vector<Pc> PCS;
static bool att(int type, int color, int x, int t, uint64_t occ) {
    if (x == t) return false;
    int df = F(t) - F(x), dr = Rk(t) - Rk(x);
    if (type == P) return dr == (color == 0 ? 1 : -1) && abs(df) == 1;
    if (type == N) { int a = abs(df), b = abs(dr); return (a == 1 && b == 2) || (a == 2 && b == 1); }
    bool o = !df || !dr, d = abs(df) == abs(dr);
    if (!((o && type != B) || (d && type != R))) return false;
    int sf = (df > 0) - (df < 0), sr = (dr > 0) - (dr < 0), f = F(x) + sf, r = Rk(x) + sr;
    while (true) { int s = r * 8 + f; if (s == t) return true; if (occ >> s & 1) return false; f += sf; r += sr; }
}
struct Cnt { long long w = 0, d = 0, l = 0, ep = 0, fail = 0; };
int main(int argc, char** argv) {
    string nm = argv[1]; int lo = atoi(argv[2]), hi = atoi(argv[3]);
    tb_init(getenv("TBDIR"));
    string ws = nm.substr(1, nm.find('v') - 1), bs = nm.substr(nm.find('v') + 2);
    auto ty = [](char c) { return (int)string("QRBNP").find(c); };
    for (char c : ws) PCS.push_back({0, ty(c)});
    for (char c : bs) PCS.push_back({1, ty(c)});
    bool pawns = false, bothPawns = false; int wp = 0, bp = 0;
    for (auto& p : PCS) if (p.type == P) { pawns = true; (p.color ? bp : wp)++; }
    bothPawns = wp && bp;
    vector<pair<int,int>> reps;
    if (pawns) { for (int r = 0; r < 8; r++) for (int f = 0; f < 4; f++) reps.push_back({r * 8 + f, 2}); }
    else { int tri[10][2] = {{0,0},{1,0},{2,0},{3,0},{1,1},{2,1},{3,1},{2,2},{3,2},{3,3}};
        for (auto& t : tri) { int f = t[0], r = t[1], sq = r * 8 + f; reps.push_back({sq, (f == r || r == 0 && false) ? 4 : 8}); } }
    if (!pawns) for (auto& rp : reps) { int f = F(rp.first), r = Rk(rp.first); rp.second = (f == r) ? 4 : 8; }
    int np = PCS.size(); auto t0 = chrono::steady_clock::now();
    Cnt total;
    for (int ri = lo; ri < hi && ri < (int)reps.size(); ri++) {
        int wk = reps[ri].first, mult = reps[ri].second;
        vector<Cnt> part(64); vector<thread> th;
        for (int T = 0; T < 4; T++) th.emplace_back([&, T] {
            vector<int> sq(np);
            for (int bk = T; bk < 64; bk += 4) {
                if (dist(wk, bk) <= 1) continue;
                Cnt& c = part[bk];
                function<void(int)> rec = [&](int k) {
                    if (k == np) {
                        uint64_t occ = (1ULL << wk) | (1ULL << bk), W = 1ULL << wk, Bl = 1ULL << bk, bb[5] = {0, 0, 0, 0, 0};
                        for (int i = 0; i < np; i++) { occ |= 1ULL << sq[i]; (PCS[i].color ? Bl : W) |= 1ULL << sq[i]; bb[PCS[i].type] |= 1ULL << sq[i]; }
                        for (int s = 0; s < 2; s++) {
                            int ok = s ? wk : bk; bool chk = false;
                            for (int i = 0; i < np && !chk; i++) if (PCS[i].color == s && att(PCS[i].type, s, sq[i], ok, occ)) chk = true;
                            if (chk) continue;
                            auto probe = [&](int ep) { unsigned r = tb_probe_wdl(W, Bl, (1ULL << wk) | (1ULL << bk), bb[Q], bb[R], bb[B], bb[N], bb[P], 0, 0, ep, s == 0);
                                if (r == TB_RESULT_FAILED) { c.fail++; return; }
                                bool sw = r == TB_WIN || r == TB_CURSED_WIN, sl = r == TB_LOSS || r == TB_BLESSED_LOSS;
                                if ((s == 0 && sw) || (s == 1 && sl)) c.w++; else if ((s == 1 && sw) || (s == 0 && sl)) c.l++; else c.d++; };
                            probe(0);
                            if (bothPawns) for (int i = 0; i < np; i++) { if (PCS[i].color != 1 - s || PCS[i].type != P) continue;
                                int op = sq[i], r4 = s == 1 ? 3 : 4; if (Rk(op) != r4) continue;
                                int dir = s == 1 ? 8 : -8, skip = op - dir, orig = op - 2 * dir;
                                if ((occ >> skip & 1) || (occ >> orig & 1)) continue;
                                bool beside = false; for (int j = 0; j < np; j++) if (PCS[j].color == s && PCS[j].type == P && Rk(sq[j]) == r4 && abs(F(sq[j]) - F(op)) == 1) beside = true;
                                if (!beside) continue; c.ep++; probe(skip); }
                        }
                        return;
                    }
                    int start = 0;
                    if (k > 0 && PCS[k].color == PCS[k - 1].color && PCS[k].type == PCS[k - 1].type) start = sq[k - 1] + 1;
                    for (int s = start; s < 64; s++) {
                        if (s == wk || s == bk) continue; bool used = false; for (int i = 0; i < k; i++) if (sq[i] == s) used = true; if (used) continue;
                        if (PCS[k].type == P && (Rk(s) == 0 || Rk(s) == 7)) continue;
                        sq[k] = s; rec(k + 1);
                    }
                };
                rec(0);
            } });
        for (auto& x : th) x.join();
        for (auto& c : part) { total.w += c.w * mult; total.d += c.d * mult; total.l += c.l * mult; total.ep += c.ep * mult; total.fail += c.fail; }
    }
    printf("%s reps %d-%d | white wins %lld | draws %lld | black wins %lld | en passant %lld | failed %lld | %.1fs\n", nm.c_str(), lo, hi, total.w, total.d, total.l, total.ep, total.fail,
        chrono::duration<double>(chrono::steady_clock::now() - t0).count());
}
