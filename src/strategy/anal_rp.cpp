#include <bits/stdc++.h>
using namespace std;
static inline int F(int s) { return s & 7; }
static inline int Rk(int s) { return s >> 3; }
static inline int dist(int a, int b) { return max(abs(F(a) - F(b)), abs(Rk(a) - Rk(b))); }
static const long long NX = 64LL * 64 * 64 * 64;
static bool ratt(int x, int t, uint64_t occ) { if (x == t) return false; int df = F(t) - F(x), dr = Rk(t) - Rk(x); if (df && dr) return false;
    int sf = (df > 0) - (df < 0), sr = (dr > 0) - (dr < 0), f = F(x) + sf, r = Rk(x) + sr;
    while (true) { int s = r * 8 + f; if (s == t) return true; if (occ >> s & 1) return false; f += sf; r += sr; } }
int main() {
    vector<int8_t> R0(NX), D0(NX), R1(NX), D1(NX); FILE* fp = fopen("/tmp/tb4/KRvKP.xy", "rb");
    if (fread(R0.data(), 1, NX, fp) != (size_t)NX || fread(D0.data(), 1, NX, fp) != (size_t)NX || fread(R1.data(), 1, NX, fp) != (size_t)NX || fread(D1.data(), 1, NX, fp) != (size_t)NX) return 1;
    long long A[2][3] = {}, Bc[2][3] = {};
    for (long long i = 0; i < NX; i++) { int wk = i >> 18, bk = (i >> 12) & 63, r = (i >> 6) & 63, p = i & 63;
        if (wk == bk || wk == r || wk == p || bk == r || bk == p || r == p || dist(wk, bk) <= 1 || Rk(p) < 1 || Rk(p) > 6) continue;
        uint64_t occ = (1ULL << wk) | (1ULL << bk) | (1ULL << r) | (1ULL << p);
        for (int s = 0; s < 2; s++) {
            if (s == 0 && ratt(r, bk, occ)) continue;
            if (s == 1 && Rk(wk) == Rk(p) - 1 && abs(F(wk) - F(p)) == 1) continue;
            int res = s == 0 ? R0[i] : R1[i];
            int cls = ((s == 0 && res == 1) || (s == 1 && res == 2)) ? 0 : ((s == 0 && res == 2) || (s == 1 && res == 1)) ? 2 : 1;
            if (F(wk) == F(p) && Rk(wk) < Rk(p)) A[s][cls]++;
            if (Rk(p) == 1 && dist(bk, p) == 1) Bc[s][cls]++;
        } }
    const char* nm[2] = {"white", "black"};
    for (int s = 0; s < 2; s++) { long long t = A[s][0] + A[s][1] + A[s][2]; printf("Claim A (white king in front), %s to move: %lld positions | white wins %.2f%% | draws %.2f%% | black wins %.2f%%\n", nm[s], t, 100.0 * A[s][0] / t, 100.0 * A[s][1] / t, 100.0 * A[s][2] / t); }
    for (int s = 0; s < 2; s++) { long long t = Bc[s][0] + Bc[s][1] + Bc[s][2]; printf("Claim B (pawn on 7th, king next to it), %s to move: %lld positions | white wins %.2f%% | draws %.2f%% | black wins %.2f%%\n", nm[s], t, 100.0 * Bc[s][0] / t, 100.0 * Bc[s][1] / t, 100.0 * Bc[s][2] / t); }
}
