#include <bits/stdc++.h>
using namespace std;
static inline int F(int s) { return s & 7; }
static inline int Rk(int s) { return s >> 3; }
static inline int dist(int a, int b) { return max(abs(F(a) - F(b)), abs(Rk(a) - Rk(b))); }
static const long long NX = 64LL * 64 * 64 * 64;
static bool batt(int x, int t, uint64_t occ) {
    if (x == t) return false; int df = F(t) - F(x), dr = Rk(t) - Rk(x); if (abs(df) != abs(dr)) return false;
    int sf = (df > 0) - (df < 0), sr = (dr > 0) - (dr < 0), f = F(x) + sf, r = Rk(x) + sr;
    while (true) { int s = r * 8 + f; if (s == t) return true; if (occ >> s & 1) return false; f += sf; r += sr; }
}
int main() {
    vector<int8_t> R0(NX), D0(NX), R1(NX), D1(NX);
    FILE* fp = fopen("/tmp/tb4/KBvKP.xy", "rb");
    if (fread(R0.data(), 1, NX, fp) != (size_t)NX || fread(D0.data(), 1, NX, fp) != (size_t)NX || fread(R1.data(), 1, NX, fp) != (size_t)NX || fread(D1.data(), 1, NX, fp) != (size_t)NX) return 1; fclose(fp);
    map<tuple<int,int,int,int>, array<long long,2>> T;
    for (long long i = 0; i < NX; i++) {
        int wk = i >> 18, bk = (i >> 12) & 63, b = (i >> 6) & 63, p = i & 63;
        if (wk == bk || wk == b || wk == p || bk == b || bk == p || b == p || dist(wk, bk) <= 1 || Rk(p) < 1 || Rk(p) > 6) continue;
        uint64_t occ = (1ULL << wk) | (1ULL << bk) | (1ULL << b) | (1ULL << p);
        for (int s = 0; s < 2; s++) {
            if (s == 0 && batt(b, bk, occ)) continue;
            if (s == 1 && Rk(wk) == Rk(p) - 1 && abs(F(wk) - F(p)) == 1) continue;
            int r = s == 0 ? R0[i] : R1[i]; bool blackWins = (s == 0 && r == 2) || (s == 1 && r == 1);
            int ctl = 9;
            if (batt(b, p, occ)) ctl = 0;
            for (int rr = Rk(p) - 1; rr >= 0 && ctl > 0; rr--) { int t = rr * 8 + F(p); if (t == wk || t == bk) continue; if (batt(b, t, occ) || t == b) ctl = 0; }
            if (ctl > 0) for (int t2 = 0; t2 < 64 && ctl > 1; t2++) { if ((occ >> t2 & 1) || !batt(b, t2, occ)) continue; uint64_t o2 = (occ & ~(1ULL << b)) | (1ULL << t2);
                for (int rr = Rk(p) - 1; rr >= 0; rr--) { int t = rr * 8 + F(p); if (t == wk || t == bk) continue; if (batt(t2, t, o2) || t == t2) { ctl = 1; break; } } }
            if (ctl > 1) ctl = 2;
            int kin = dist(wk, F(p)) <= Rk(p) ? 1 : 0;
            int steps = Rk(p) == 6 ? 5 : Rk(p);
            int kinsq = max(abs(F(wk) - F(p)), Rk(wk)) <= steps - (s == 0 ? 0 : 1) ? 1 : 0;
            T[{s, ctl, kinsq, Rk(p)}][blackWins]++;
        }
    }
    printf("stm ctl(0=now,1=one move,2=more) wk_in_square pawn_rank | draws black_wins\n");
    for (auto& kv : T) { auto [s, c, k, r] = kv.first; printf("%s %d %d %d | %lld %lld\n", s ? "b" : "w", c, k, r + 1, kv.second[0], kv.second[1]); }
}
