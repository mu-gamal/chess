#include <bits/stdc++.h>
using namespace std;
static inline int F(int s) { return s & 7; }
static inline int Rk(int s) { return s >> 3; }
static inline int dist(int a, int b) { return max(abs(F(a) - F(b)), abs(Rk(a) - Rk(b))); }
static const long long NX = 64LL * 64 * 64 * 64;
int main() {
    vector<int8_t> R0(NX), D0(NX), R1(NX), D1(NX); FILE* fp = fopen("/tmp/tb4/KPvKP.xy", "rb");
    if (fread(R0.data(), 1, NX, fp) != (size_t)NX || fread(D0.data(), 1, NX, fp) != (size_t)NX || fread(R1.data(), 1, NX, fp) != (size_t)NX || fread(D1.data(), 1, NX, fp) != (size_t)NX) return 1;
    long long c[2][3] = {}, tie[2][3] = {};
    for (long long i = 0; i < NX; i++) { int wk = i >> 18, bk = (i >> 12) & 63, wp = (i >> 6) & 63, bp = i & 63;
        if (wk == bk || wk == wp || wk == bp || bk == wp || bk == bp || wp == bp || dist(wk, bk) <= 1 || Rk(wp) < 1 || Rk(wp) > 6 || Rk(bp) < 1 || Rk(bp) > 6) continue;
        if (abs(F(wp) - F(bp)) < 2) continue;
        int wm = 7 - Rk(wp); if (Rk(wp) == 1) wm--; int bm = Rk(bp); if (Rk(bp) == 6) bm--;
        for (int s = 0; s < 2; s++) {
            bool wChk = Rk(bk) == Rk(wp) + 1 && abs(F(bk) - F(wp)) == 1, bChk = Rk(wk) == Rk(bp) - 1 && abs(F(wk) - F(bp)) == 1;
            if (s == 0 && wChk) continue; if (s == 1 && bChk) continue;
            int r = s == 0 ? R0[i] : R1[i];
            int res = ((s == 0 && r == 1) || (s == 1 && r == 2)) ? 0 : ((s == 0 && r == 2) || (s == 1 && r == 1)) ? 2 : 1;
            int first = s == 0 ? (wm <= bm ? 0 : 1) : (bm <= wm ? 1 : 0);
            if (wm == bm) tie[s][res]++;
            int outcome = res == 1 ? 1 : (res == 0 ? (first == 0 ? 0 : 2) : (first == 1 ? 0 : 2));
            c[s][outcome]++;
        } }
    const char* nm[2] = {"white", "black"};
    for (int s = 0; s < 2; s++) { long long t = c[s][0] + c[s][1] + c[s][2]; printf("%s to move, race positions %lld: first to promote wins %.2f%% | draw %.2f%% | the other side wins %.2f%%\n", nm[s], t, 100.0 * c[s][0] / t, 100.0 * c[s][1] / t, 100.0 * c[s][2] / t); }
}
