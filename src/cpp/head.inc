#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <thread>
#include <mutex>
#include <string>
#include <set>

typedef unsigned __int128 u128;
typedef uint64_t u64;

static const int NP = 4;
static const u64 PR_[NP] = {4611686018427387847ULL, 4611686018427387817ULL, 4611686018427387787ULL, 4611686018427387761ULL};

struct Z {
    u64 a[NP];
    Z() { for (int i = 0; i < NP; i++) a[i] = 0; }
    explicit Z(u64 v) { for (int i = 0; i < NP; i++) a[i] = v % PR_[i]; }
    bool zero() const { for (int i = 0; i < NP; i++) if (a[i]) return false; return true; }
};
static inline Z operator+(const Z& x, const Z& y) { Z r; for (int i = 0; i < NP; i++) { u64 s = x.a[i] + y.a[i]; if (s >= PR_[i]) s -= PR_[i]; r.a[i] = s; } return r; }
static inline Z operator-(const Z& x, const Z& y) { Z r; for (int i = 0; i < NP; i++) { u64 s = x.a[i] + PR_[i] - y.a[i]; if (s >= PR_[i]) s -= PR_[i]; r.a[i] = s; } return r; }
static inline Z operator*(const Z& x, const Z& y) { Z r; for (int i = 0; i < NP; i++) r.a[i] = (u64)((u128)x.a[i] * y.a[i] % PR_[i]); return r; }
static inline Z& operator+=(Z& x, const Z& y) { x = x + y; return x; }

static u64 powmod(u64 b, u64 e, u64 m) { u64 r = 1; b %= m; while (e) { if (e & 1) r = (u64)((u128)r * b % m); b = (u64)((u128)b * b % m); e >>= 1; } return r; }

static Z FACZ[80], IFACZ[80];
static Z FALL[80][80];
static u64 BINOM[80][80];
static Z BINZ[80][80];

static void init_math() {
    FACZ[0] = Z(1);
    for (int i = 1; i < 80; i++) FACZ[i] = FACZ[i - 1] * Z((u64)i);
    for (int i = 0; i < 80; i++) { Z r; for (int j = 0; j < NP; j++) r.a[j] = powmod(FACZ[i].a[j], PR_[j] - 2, PR_[j]); IFACZ[i] = r; }
    for (int n = 0; n < 80; n++) { FALL[n][0] = Z(1); for (int k = 1; k < 80; k++) FALL[n][k] = (k <= n) ? FALL[n][k - 1] * Z((u64)(n - k + 1)) : Z(); }
    for (int n = 0; n < 80; n++) for (int k = 0; k < 80; k++) BINOM[n][k] = 0;
    for (int n = 0; n < 70; n++) { BINOM[n][0] = 1; for (int k = 1; k <= n; k++) BINOM[n][k] = BINOM[n - 1][k - 1] + (k <= n - 1 ? BINOM[n - 1][k] : 0); }
    for (int n = 0; n < 80; n++) for (int k = 0; k < 80; k++) BINZ[n][k] = Z(BINOM[n][k]);
}
static inline const Z& falling(int n, int k) { static Z zero; if (n < 0 || k < 0 || k > n) return zero; return FALL[n][k]; }
static inline const Z& binz(int n, int k) { static Z zero; if (n < 0 || k < 0 || k > n) return zero; return BINZ[n][k]; }

enum { WHITE = 1, BLACK = 0 };
static inline int sqf(int s) { return s & 7; }
static inline int sqr(int s) { return s >> 3; }
static inline int sq(int f, int r) { return r * 8 + f; }
static inline bool light(int s) { return ((sqf(s) + sqr(s)) & 1) == 1; }
static inline bool pawnzone(int s) { return sqr(s) >= 1 && sqr(s) <= 6; }

struct Rules { bool r[16]; Rules() { memset(r, 0, sizeof(r)); } bool has(int i) const { return r[i]; } };

struct Comp { int p, q, r, b, n; };
static std::vector<Comp> COMPS;
static void build_comps() {
    for (int p = 0; p < 9; p++) { int spare = 8 - p;
        for (int q = 0; q < 2 + spare; q++) for (int r = 0; r < 3 + spare; r++) for (int b = 0; b < 3 + spare; b++) for (int n = 0; n < 3 + spare; n++) {
            int extra = std::max(0, q - 1) + std::max(0, r - 2) + std::max(0, b - 2) + std::max(0, n - 2);
            if (extra <= spare) COMPS.push_back({p, q, r, b, n});
        } }
}

