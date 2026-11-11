#include <bits/stdc++.h>
using namespace std;
typedef unsigned __int128 u128; typedef uint64_t u64; typedef long long ll;
#ifndef PIDX
#define PIDX 0
#endif
static constexpr u64 PRIMES[4] = {2305843009213693951ULL, 2305843009213693921ULL, 2305843009213693907ULL, 2305843009213693723ULL};
static constexpr u64 P = PRIMES[PIDX];
static constexpr u64 ninvc(u64 p) { u64 x = 1; for (int i = 0; i < 7; i++) x *= 2 - p * x; return ~x + 1; }
static constexpr u64 NINV = ninvc(P);
static u64 R2, ONE;
static inline u64 redc(u128 x) { u64 m = (u64)x * NINV; u64 t = (u64)((x + (u128)m * P) >> 64); return t >= P ? t - P : t; }
static inline u64 red128(u128 x) { u64 hi = (u64)(x >> 64) % P; return redc(((u128)hi << 64) | (u64)x); }
static inline u64 mm(u64 a, u64 b) { return redc((u128)a * b); }
static inline u64 am(u64 a, u64 b) { u64 s = a + b; return s >= P ? s - P : s; }
static inline u64 sm(u64 a, u64 b) { return a >= b ? a - b : a + P - b; }
static inline u64 toM(u64 x) { return mm(x % P, R2); }
static inline u64 fromM(u64 a) { return redc(a); }
static u64 powM(u64 a, u64 e) { u64 r = ONE; while (e) { if (e & 1) r = mm(r, a); a = mm(a, a); e >>= 1; } return r; }

static u64 FAC[80], IFAC[80], FALL[80][80], BIN[80][80];
static void init_math() {
    u64 r = (u64)((((u128)1) << 64) % P); R2 = (u64)((u128)r * r % P); ONE = toM(1);
    FAC[0] = ONE; for (int i = 1; i < 80; i++) FAC[i] = mm(FAC[i - 1], toM(i));
    for (int i = 0; i < 80; i++) IFAC[i] = powM(FAC[i], P - 2);
    for (int n = 0; n < 80; n++) { FALL[n][0] = ONE; for (int k = 1; k < 80; k++) FALL[n][k] = k <= n ? mm(FALL[n][k - 1], toM(n - k + 1)) : 0; }
    for (int n = 0; n < 80; n++) for (int k = 0; k < 80; k++) BIN[n][k] = k <= n ? mm(FAC[n], mm(IFAC[k], IFAC[n - k])) : 0;
}
static inline u64 fall(int n, int k) { if (n < 0 || k < 0 || k > n) return 0; return FALL[n][k]; }

static bool Rl[16];
static int NT = 15, MCAP = 7, MAXP = 8;
static bool EXC = true;
struct Comp { int p, q, r, b, n; };
static vector<Comp> COMPS;
static void build_comps() {
    for (int p = 0; p < 9; p++) { int spare = 8 - p;
        for (int q = 0; q < 2 + spare; q++) for (int r = 0; r < 3 + spare; r++) for (int b = 0; b < 3 + spare; b++) for (int n = 0; n < 3 + spare; n++) {
            int extra = max(0, q - 1) + max(0, r - 2) + max(0, b - 2) + max(0, n - 2);
            if (extra <= spare) COMPS.push_back({p, q, r, b, n});
        } }
}
static inline int sqf(int s) { return s & 7; }
static inline int sqr(int s) { return s >> 3; }
static inline int SQ(int f, int r) { return r * 8 + f; }
static inline bool light(int s) { return ((sqf(s) + sqr(s)) & 1) == 1; }
static inline int dist(int a, int b) { return max(abs(sqf(a) - sqf(b)), abs(sqr(a) - sqr(b))); }
static inline bool diagadj(int a, int b) { return abs(sqf(a) - sqf(b)) == 1 && abs(sqr(a) - sqr(b)) == 1; }
static inline bool orthadj(int a, int b) { return abs(sqf(a) - sqf(b)) + abs(sqr(a) - sqr(b)) == 1; }
static inline bool knightrel(int a, int b) { int df = abs(sqf(a) - sqf(b)), dr = abs(sqr(a) - sqr(b)); return (df == 1 && dr == 2) || (df == 2 && dr == 1); }

// ---------------- geometry ----------------
static int NK = 22, ND, NO, NN, GS[5];
static u64 PKt[22][24], PNt[9][10], POt[5][6], PDt[5][6];
static int NS, NS2, csIdx[9][8], csP[64], csE[64], tr[64][10];
static unordered_map<int, vector<u64>> MVC[5];
static inline void decode(int L, int idx, int& ik, int& in, int& io, int& id) {
    id = 0; io = 0; in = 0; int t = idx;
    if (L >= 4) { id = t % ND; t /= ND; }
    if (L >= 3) { io = t % NO; t /= NO; }
    if (L >= 2) { in = t % NN; t /= NN; }
    ik = t;
}
static const vector<u64>& monoVec(int L, int ek, int en, int eo, int ed) {
    int key = ((ek * 16 + en) * 16 + eo) * 16 + ed;
    auto it = MVC[L].find(key);
    if (it != MVC[L].end()) return it->second;
    vector<u64> v(GS[L]);
    for (int i = 0; i < GS[L]; i++) { int ik, in, io, id; decode(L, i, ik, in, io, id); v[i] = mm(mm(PKt[ik][ek], PNt[in][en]), mm(POt[io][eo], PDt[id][ed])); }
    return MVC[L].emplace(key, move(v)).first->second;
}
static void init_geo() {
    ND = Rl[9] ? 5 : 1; NO = Rl[9] ? 5 : 1; NN = Rl[8] ? 9 : 1;
    GS[1] = NK; GS[2] = NK * NN; GS[3] = GS[2] * NO; GS[4] = GS[3] * ND;
    for (int i = 0; i < 22; i++) { PKt[i][0] = ONE; for (int e = 1; e < 24; e++) PKt[i][e] = mm(PKt[i][e - 1], toM(i)); }
    for (int i = 0; i < 9; i++) { PNt[i][0] = ONE; for (int e = 1; e < 10; e++) PNt[i][e] = mm(PNt[i][e - 1], toM(i)); }
    for (int i = 0; i < 5; i++) { POt[i][0] = ONE; PDt[i][0] = ONE; for (int e = 1; e < 6; e++) { POt[i][e] = mm(POt[i][e - 1], toM(i)); PDt[i][e] = POt[i][e]; } }
    NS = 0; memset(csIdx, -1, sizeof(csIdx));
    for (int p = 0; p <= MAXP; p++) { int me = EXC ? max(p - 1, 0) : 0; for (int e = 0; e <= me; e++) { csIdx[p][e] = NS; csP[NS] = p; csE[NS] = e; NS++; } }
    NS2 = NS * NS;
    for (int s = 0; s < NS; s++) for (int a = 0; a < 10; a++) { int p = csP[s] + a; if (p > MAXP) { tr[s][a] = -1; continue; } int e = EXC ? csE[s] + max(a - 1, 0) : 0; tr[s][a] = csIdx[p][e]; }
}
struct SqI { int kind; bool wok; int ek, ed, eo, en; };
typedef map<array<int, 6>, ll> TM;
typedef map<array<int, 4>, ll> SM4;
static void enumFile(const SqI sq[8], int k2r, TM& tm, SM4& s32) {
    int bek = 0, ben = 0, beo = 0, bed = 0, b0 = 0, br0 = -1;
    int zl[8], m = 0;
    for (int r = 0; r < 8; r++) {
        if (sq[r].kind == 2 || sq[r].kind == 3 || r == k2r) { bek += sq[r].ek; ben += sq[r].en; beo += sq[r].eo; bed += sq[r].ed; if (sq[r].kind == 3) { b0++; br0 = r; } }
        else if (sq[r].kind == 1) zl[m++] = r;
    }
    int tot = 1; for (int i = 0; i < m; i++) tot *= 3;
    for (int code = 0; code < tot; code++) {
        int c = code, a = 0, b = b0, ek = bek, en = ben, eo = beo, ed = bed, wr = -1, br = br0; bool ok = true;
        for (int i = 0; i < m; i++) { int d = c % 3; c /= 3; if (!d) continue; int r = zl[i];
            if (d == 1) { if (!sq[r].wok) { ok = false; break; } a++; wr = r; } else { b++; br = r; }
            ek += sq[r].ek; en += sq[r].en; eo += sq[r].eo; ed += sq[r].ed; }
        if (!ok) continue;
        tm[{a, b, ek, en, eo, ed}]++;
        if (a == 1 && b == 1 && wr < br) s32[{ek, en, eo, ed}]++;
    }
}
struct FP { int level = 1; vector<u64> buf; int off[8][10]; bool any = false; };
static int levelOf(const TM& t) { int L = 1; for (auto& kv : t) { if (kv.first[5] > 0) L = max(L, 4); else if (kv.first[4] > 0) L = max(L, 3); else if (kv.first[3] > 0) L = max(L, 2); } return L; }
static void evalFP(const TM& t, int L, FP& fp) {
    fp.level = L; fp.any = !t.empty(); int g = GS[L];
    for (int a = 0; a < 8; a++) for (int b = 0; b < 10; b++) fp.off[a][b] = -1;
    int cnt = 0;
    for (auto& kv : t) { int a = kv.first[0], b = kv.first[1]; if (fp.off[a][b] < 0) fp.off[a][b] = cnt++; }
    fp.buf.assign((size_t)cnt * g, 0);
    for (auto& kv : t) {
        u64* v = fp.buf.data() + (size_t)fp.off[kv.first[0]][kv.first[1]] * g;
        const vector<u64>& mv = monoVec(L, kv.first[2], kv.first[3], kv.first[4], kv.first[5]);
        u64 c = toM((u64)kv.second);
        for (int i = 0; i < g; i++) v[i] = am(v[i], mm(c, mv[i]));
    }
}
static void evalS(const SM4& t, vector<u64>& out) {
    int g = GS[4]; out.assign(g, 0);
    for (auto& kv : t) { const vector<u64>& mv = monoVec(4, kv.first[0], kv.first[1], kv.first[2], kv.first[3]); u64 c = toM((u64)kv.second); for (int i = 0; i < g; i++) out[i] = am(out[i], mm(c, mv[i])); }
}
static vector<u64> A0, A1; static vector<u128> D0, D1; static vector<char> nz0, nz1, dz0, dz1;
static void pmul(const vector<u64>& src, const vector<char>& nz, const FP& F, vector<u128>& dst, vector<char>& dnz, int g) {
    int G4 = GS[4];
    for (int sw = 0; sw < NS; sw++) for (int sb = 0; sb < NS; sb++) {
        int s = sw * NS + sb; if (!nz[s]) continue;
        const u64* sp = src.data() + (size_t)s * G4;
        for (int a = 0; a < 8; a++) { int tw = tr[sw][a]; if (tw < 0) break;
            for (int b = 0; b < 10; b++) { int tb = tr[sb][b]; if (tb < 0) break; int o = F.off[a][b]; if (o < 0) continue;
                const u64* fp = F.buf.data() + (size_t)o * g; int d = tw * NS + tb; u128* dp = dst.data() + (size_t)d * G4; dnz[d] = 1;
                for (int i = 0; i < g; i++) dp[i] += (u128)sp[i] * fp[i];
            } }
    }
}
static void fin(vector<u128>& dst, vector<char>& dnz, vector<u64>& out, vector<char>& nz, int g) {
    int G4 = GS[4];
    for (int s = 0; s < NS2; s++) { nz[s] = dnz[s]; if (!dnz[s]) continue; u128* dp = dst.data() + (size_t)s * G4; u64* op = out.data() + (size_t)s * G4; for (int i = 0; i < g; i++) { op[i] = red128(dp[i]); dp[i] = 0; } dnz[s] = 0; }
}
static void lift(vector<u64>& arr, const vector<char>& nz, int gold, int m) {
    int G4 = GS[4];
    for (int s = 0; s < NS2; s++) { if (!nz[s]) continue; u64* p = arr.data() + (size_t)s * G4; for (int i = gold - 1; i >= 0; i--) { u64 v = p[i]; for (int t = m - 1; t >= 0; t--) p[(size_t)i * m + t] = v; } }
}
static const int NSG = 18;
static bool sigOn[NSG];
static vector<u64> Acc[NSG][2], Acc32[NSG][2];
static void accAdd(int sig, int ck, const vector<u64>& A, const vector<char>& nz, const vector<u64>& vec) {
    if (!sigOn[sig]) return; int G4 = GS[4]; vector<u64>& acc = Acc[sig][ck];
    for (int s = 0; s < NS2; s++) { if (!nz[s]) continue; const u64* ap = A.data() + (size_t)s * G4; u64* cp = acc.data() + (size_t)s * G4; for (int i = 0; i < G4; i++) cp[i] = am(cp[i], mm(ap[i], vec[i])); }
}
static void acc32Add(int sig, int ck, const vector<u64>& S, const vector<u64>& vec) {
    if (!sigOn[sig] || !Rl[2]) return; int G4 = GS[4]; vector<u64>& acc = Acc32[sig][ck];
    for (int i = 0; i < G4; i++) acc[i] = am(acc[i], mm(S[i], vec[i]));
}
static vector<u64> vmul(const vector<u64>& a, const vector<u64>& b) { vector<u64> r(a.size()); for (size_t i = 0; i < a.size(); i++) r[i] = mm(a[i], b[i]); return r; }
static vector<u64> vadd(const vector<u64>& a, const vector<u64>& b) { vector<u64> r(a.size()); for (size_t i = 0; i < a.size(); i++) r[i] = am(a[i], b[i]); return r; }
static vector<u64> vneg(const vector<u64>& a) { vector<u64> r(a.size()); for (size_t i = 0; i < a.size(); i++) r[i] = sm(0, a[i]); return r; }
static vector<u64> sqMono(int s, int T) {
    return monoVec(4, light(s) == light(T) ? 1 : 0, (Rl[8] && knightrel(s, T)) ? 1 : 0, (Rl[9] && orthadj(s, T)) ? 1 : 0, (Rl[9] && diagadj(s, T)) ? 1 : 0);
}
static void geometry(bool withEp) {
    int G4 = GS[4];
    for (int s = 0; s < NSG; s++) if (sigOn[s]) for (int c = 0; c < 2; c++) { Acc[s][c].assign((size_t)NS2 * G4, 0); Acc32[s][c].assign(G4, 0); }
    A0.assign((size_t)NS2 * G4, 0); A1.assign((size_t)NS2 * G4, 0); D0.assign((size_t)NS2 * G4, 0); D1.assign((size_t)NS2 * G4, 0);
    nz0.assign(NS2, 0); nz1.assign(NS2, 0); dz0.assign(NS2, 0); dz1.assign(NS2, 0);
    const int E1 = SQ(4, 0), E8 = SQ(4, 7), A1s = SQ(0, 0), H1s = SQ(7, 0), A8s = SQ(0, 7), H8s = SQ(7, 7);
    vector<u64> oneV(G4, ONE);
    for (int T = 0; T < 64; T++) {
        int ft = sqf(T), rt = sqr(T), ck = light(T) ? 1 : 0;
        for (int cfg = 0; cfg < 17; cfg++) {
            int e = -1, noA = 0;
            if (cfg > 0) { if (!withEp) break; e = (cfg - 1) / 2; noA = (cfg - 1) % 2; if (noA && !Rl[10]) continue; }
            int Psq = -1, Ea = -1, Eb = -1;
            if (e >= 0) { Psq = SQ(e, 4); Ea = SQ(e, 5); Eb = SQ(e, 6); if (T == Psq || T == Ea || T == Eb) continue; }
            int epf = e >= 0 ? 1 : 0;
            FP F0[8], F1[8]; bool hasK2[8]; vector<u64> s0v[8], s1v[8]; int lev[8];
            for (int f = 0; f < 8; f++) {
                SqI sq[8];
                for (int r = 0; r < 8; r++) { int s = SQ(f, r); SqI& q = sq[r];
                    q.ek = light(s) == light(T) ? 1 : 0; q.ed = (Rl[9] && diagadj(s, T)) ? 1 : 0; q.eo = (Rl[9] && orthadj(s, T)) ? 1 : 0; q.en = (Rl[8] && knightrel(s, T)) ? 1 : 0; q.wok = true;
                    if (s == T) q.kind = 2; else if (s == Psq) q.kind = 3; else if (s == Ea || s == Eb) q.kind = 2;
                    else if (r >= 1 && r <= 6) { q.kind = 1; if (Rl[7] && r == rt - 1 && abs(f - ft) == 1) q.wok = false; if (noA && r == 4 && abs(f - e) == 1) q.wok = false; }
                    else q.kind = 0;
                }
                TM t0, t1; SM4 z0, z1;
                enumFile(sq, -1, t0, z0);
                for (int r = 0; r < 8; r++) { if (sq[r].kind > 1) continue; int s = SQ(f, r); if (Rl[6] && dist(s, T) <= 1) continue; enumFile(sq, r, t1, z1); }
                int L = max(levelOf(t0), levelOf(t1)); lev[f] = L;
                evalFP(t0, L, F0[f]); evalFP(t1, L, F1[f]); hasK2[f] = !t1.empty();
                if (Rl[2]) { evalS(z0, s0v[f]); evalS(z1, s1v[f]); }
            }
            int ord[8]; for (int i = 0; i < 8; i++) ord[i] = i;
            stable_sort(ord, ord + 8, [&](int a, int b) { return lev[a] < lev[b]; });
            int curL = 1, g = GS[1];
            fill(nz0.begin(), nz0.end(), 0); fill(nz1.begin(), nz1.end(), 0);
            nz0[0] = 1; for (int i = 0; i < g; i++) A0[i] = ONE;
            for (int oi = 0; oi < 8; oi++) { int f = ord[oi];
                while (curL < lev[f]) { int m = curL == 1 ? NN : curL == 2 ? NO : ND; lift(A0, nz0, g, m); lift(A1, nz1, g, m); g *= m; curL++; }
                pmul(A1, nz1, F0[f], D1, dz1, g); if (hasK2[f]) pmul(A0, nz0, F1[f], D1, dz1, g);
                pmul(A0, nz0, F0[f], D0, dz0, g);
                fin(D1, dz1, A1, nz1, g); fin(D0, dz0, A0, nz0, g);
            }
            while (curL < 4) { int m = curL == 1 ? NN : curL == 2 ? NO : ND; lift(A0, nz0, g, m); lift(A1, nz1, g, m); g *= m; curL++; }
            vector<u64> S0(G4, ONE), S1(G4, 0);
            if (Rl[2]) for (int f = 0; f < 8; f++) for (int i = 0; i < G4; i++) { S1[i] = am(mm(S1[i], s0v[f][i]), mm(S0[i], s1v[f][i])); S0[i] = mm(S0[i], s0v[f][i]); }
            int nd = 0, no = 0, nn = 0;
            for (int s = 0; s < 64; s++) { if (diagadj(s, T)) nd++; if (orthadj(s, T)) no++; if (knightrel(s, T)) nn++; }
            vector<u64> base = monoVec(4, 0, Rl[8] ? 8 - nn : 0, Rl[9] ? 4 - no : 0, Rl[9] ? 4 - nd : 0);
            if (noA) base = vneg(base);
            // white castling pattern sums
            vector<u64> wsum[3]; bool wany[3] = {false, false, false};
            for (int pat = 1; pat <= 3; pat++) { // 1=K(h1) 2=Q(a1) 3=KQ
                vector<int> rk; if (pat & 1) rk.push_back(H1s); if (pat & 2) rk.push_back(A1s);
                bool ok = T != E1 && !(Rl[6] && dist(T, E1) <= 1);
                for (int r : rk) { if (T == r) ok = false; if (Rl[9] && orthadj(T, r)) ok = false; }
                if (!ok) continue;
                vector<u64> v = sqMono(E1, T); for (int r : rk) v = vmul(v, sqMono(r, T));
                int fw = (int)rk.size(); if (!wany[fw]) { wsum[fw] = v; wany[fw] = true; } else wsum[fw] = vadd(wsum[fw], v);
            }
            accAdd(0 * 2 + epf, ck, A1, nz1, base); acc32Add(epf, ck, S1, base);
            for (int fw = 1; fw <= 2; fw++) if (wany[fw]) { vector<u64> v = vmul(base, wsum[fw]); accAdd((fw * 3 + 0) * 2 + epf, ck, A0, nz0, v); acc32Add((fw * 3) * 2 + epf, ck, S0, v); }
            if (T == E8) {
                vector<u64> bsum[3], bksum[3]; bool bany[3] = {false, false, false};
                for (int pat = 1; pat <= 3; pat++) {
                    vector<int> rk; if (pat & 1) rk.push_back(H8s); if (pat & 2) rk.push_back(A8s);
                    vector<u64> v = oneV; for (int r : rk) v = vmul(v, sqMono(r, T));
                    vector<u64> ks(G4, 0); for (int r : rk) if (!(Rl[6] && dist(r, T) <= 1)) ks = vadd(ks, sqMono(r, T));
                    vector<u64> vk = vmul(v, ks);
                    int fb = (int)rk.size(); if (!bany[fb]) { bsum[fb] = v; bksum[fb] = vk; bany[fb] = true; } else { bsum[fb] = vadd(bsum[fb], v); bksum[fb] = vadd(bksum[fb], vk); }
                }
                for (int fb = 1; fb <= 2; fb++) {
                    vector<u64> v1 = vmul(base, bsum[fb]), v0 = vneg(vmul(base, bksum[fb]));
                    int sg = (0 * 3 + fb) * 2 + epf;
                    accAdd(sg, ck, A1, nz1, v1); accAdd(sg, ck, A0, nz0, v0); acc32Add(sg, ck, S1, v1); acc32Add(sg, ck, S0, v0);
                    for (int fw = 1; fw <= 2; fw++) if (wany[fw]) { vector<u64> v = vmul(v1, wsum[fw]); int s2 = (fw * 3 + fb) * 2 + epf; accAdd(s2, ck, A0, nz0, v); acc32Add(s2, ck, S0, v); }
                }
            }
        }
        if (getenv("V3LOG")) fprintf(stderr, "T %d done\n", T);
    }
}
static vector<vector<u64>> invVander(int m) {
    vector<vector<u64>> V(m, vector<u64>(2 * m, 0));
    for (int i = 0; i < m; i++) { u64 x = toM(i), p = ONE; for (int j = 0; j < m; j++) { V[i][j] = p; p = mm(p, x); } V[i][m + i] = ONE; }
    for (int c = 0; c < m; c++) { int pv = c; while (V[pv][c] == 0) pv++; swap(V[pv], V[c]); u64 inv = powM(V[c][c], P - 2); for (int j = 0; j < 2 * m; j++) V[c][j] = mm(V[c][j], inv);
        for (int r = 0; r < m; r++) if (r != c && V[r][c]) { u64 f = V[r][c]; for (int j = 0; j < 2 * m; j++) V[r][j] = sm(V[r][j], mm(f, V[c][j])); } }
    vector<vector<u64>> I(m, vector<u64>(m)); for (int i = 0; i < m; i++) for (int j = 0; j < m; j++) I[i][j] = V[i][m + j];
    return I;
}
static void interp(u64* a) {
    static vector<vector<u64>> IK, IN, IO, ID; static bool init = false;
    if (!init) { IK = invVander(NK); IN = invVander(NN); IO = invVander(NO); ID = invVander(ND); init = true; }
    int G4 = GS[4]; int dims[4] = {ND, NO, NN, NK}; int stride = 1; vector<vector<u64>>* mats[4] = {&ID, &IO, &IN, &IK};
    u64 tmp[32];
    for (int d = 0; d < 4; d++) { int m = dims[d]; if (m > 1) { auto& Mx = *mats[d];
            for (int base = 0; base < G4; base++) { if ((base / stride) % m) continue;
                for (int j = 0; j < m; j++) { u128 acc = 0; for (int i = 0; i < m; i++) acc += (u128)Mx[j][i] * a[base + i * stride]; tmp[j] = red128(acc); }
                for (int j = 0; j < m; j++) a[base + j * stride] = tmp[j]; } }
        stride *= m; }
}
// ---------------- officers ----------------
struct SE { int bl, bd, q, r, n, mo; u64 pr[9]; };
static array<vector<SE>, 9> sideTab(int fixr, int fp, bool r5, bool sn, bool sq) {
    array<map<array<int, 6>, array<u64, 9>>, 9> t;
    for (auto& c : COMPS) { if (c.p < fp || c.r < fixr) continue; int fr = c.r - fixr; int n = sn ? c.n : 0, q = sq ? c.q : 0, r = sq ? fr : 0; int mo = c.q + fr + c.n - n - q - r;
        int po = max(0, c.q - 1) + max(0, c.r - 2) + max(0, c.n - 2);
        for (int bl = 0; bl <= c.b; bl++) { int bd = c.b - bl, pr;
            if (r5) { pr = po + max(0, bl - 1) + max(0, bd - 1); if (pr > 8 - c.p) continue; } else pr = po + max(0, c.b - 2);
            u64 w = mm(mm(mm(IFAC[c.q], IFAC[fr]), mm(IFAC[c.n], IFAC[bl])), IFAC[bd]);
            auto it = t[c.p].find({bl, bd, q, r, n, mo}); if (it == t[c.p].end()) { array<u64, 9> z; z.fill(0); it = t[c.p].emplace(array<int, 6>{bl, bd, q, r, n, mo}, z).first; }
            if (pr > 8) { fprintf(stderr, "pr>8\n"); exit(1); }
            it->second[pr] = am(it->second[pr], w); } }
    array<vector<SE>, 9> out;
    for (int p = 0; p < 9; p++) for (auto& kv : t[p]) { SE e; e.bl = kv.first[0]; e.bd = kv.first[1]; e.q = kv.first[2]; e.r = kv.first[3]; e.n = kv.first[4]; e.mo = kv.first[5]; for (int i = 0; i < 9; i++) e.pr[i] = kv.second[i]; out[p].push_back(e); }
    return out;
}
static int limitf(int T, int pw, int pb) {
    int L = 1000000000;
    if (Rl[1] && T == 32) L = 0;
    if (Rl[3]) L = min(L, 3 * (32 - T));
    if (Rl[4]) { int x = 2 * (32 - T) + 16 - pw - pb; int fl = x >= 0 ? x / 2 : -((-x + 1) / 2); L = min(L, fl); }
    return L;
}
struct FMap { vector<u64> k, v; int sh = 60; size_t cnt = 0;
    void init(size_t cap) { size_t c = 16; int b = 4; while (c < cap * 2) { c <<= 1; b++; } k.assign(c, 0); v.assign(c, 0); sh = 64 - b; cnt = 0; }
    void grow() { vector<u64> ok, ov; ok.swap(k); ov.swap(v); size_t c = ok.size() * 2; k.assign(c, 0); v.assign(c, 0); sh--; cnt = 0; for (size_t i = 0; i < ok.size(); i++) if (ok[i]) addraw(ok[i], ov[i]); }
    inline void addraw(u64 key, u64 val) { size_t mask = k.size() - 1; size_t h = (key * 0x9E3779B97F4A7C15ULL) >> sh; while (k[h] && k[h] != key) h = (h + 1) & mask; if (!k[h]) { k[h] = key; v[h] = val; cnt++; } else v[h] = am(v[h], val); }
    inline void add(u64 key, u64 val) { if ((cnt + 1) * 2 > k.size()) grow(); addraw(key + 1, val); }
};
static const int JQ = 9, JB = 5, JR = 5, JN = 9, FKN = 33;
static u64 Bd[JQ * JB * JR * JN][FKN];
static u64 T1[JQ][JB][JR][9][FKN], T2[5][JB][5][9][FKN], PHI[FKN][5][5][9];
static u64 resM[3];
static void officers(int sig, u64 sres[3]) {
    int fw = sig / 6, fb = (sig / 2) % 3, epf = sig % 2; int G4 = GS[4];
    auto st = sideTab(fw, 0, Rl[5], Rl[8], Rl[9]);
    auto ot = sideTab(fb, epf, Rl[5], false, false);
    sres[0] = sres[1] = sres[2] = 0;
    int nNp = Rl[8] ? 9 : 1, nOp = Rl[9] ? 5 : 1, nDp = Rl[9] ? 5 : 1;
    for (int pw = 0; pw <= MAXP; pw++) for (int pb = epf; pb <= MAXP; pb++) {
        if (st[pw].empty() || ot[pb].empty()) continue;
        int Rtot = 62 - fw - fb - 2 * epf - pw - pb; if (Rtot < 0) continue;
        int nms = NT - fw - pw, nmo = NT - fb - pb;
        struct OI { int bl, bd, mo, nt, tg; u64 c[18]; }; vector<OI> oi;
        for (auto& e : ot[pb]) { OI x; x.bl = e.bl; x.bd = e.bd; x.mo = e.mo; x.nt = e.bl + e.bd + e.mo; if (nmo - x.nt < 0) continue; x.tg = min(nmo - x.nt, MCAP); u64 s = 0; for (int i = 0; i < 9; i++) { s = am(s, e.pr[i]); x.c[i] = s; } for (int i = 9; i < 18; i++) x.c[i] = s; oi.push_back(x); }
        int Ltab[80]; for (int n = 0; n < 80; n++) Ltab[n] = min(limitf(pw + pb + n + 2 + fw + fb, pw, pb), 16);
        FMap comb; comb.init((size_t)st[pw].size() * oi.size() / 2 + 16);
        for (auto& e : st[pw]) { int ns = e.bl + e.bd + e.q + e.r + e.n + e.mo; if (nms - ns < 0) continue; int ts = min(nms - ns, MCAP);
            int nzi[9], nzc = 0; for (int i = 0; i < 9; i++) if (e.pr[i]) nzi[nzc++] = i; if (!nzc) continue;
            for (auto& o : oi) { int L = Ltab[ns + o.nt]; if (L < 0) continue; u128 acc = 0; bool any = false;
                for (int t = 0; t < nzc; t++) { int j = L - nzi[t]; if (j < 0) break; acc += (u128)e.pr[nzi[t]] * o.c[j]; any = true; }
                if (!any) continue; u64 s = red128(acc); if (!s) continue;
                u64 key = ((((((((((u64)(e.bl + o.bl) << 5 | (u64)(e.bd + o.bd)) << 4 | e.bl) << 4 | e.bd) << 4 | e.q) << 4 | e.r) << 4 | e.n) << 5 | (u64)(e.mo + o.mo)) << 3 | ts) << 3) | o.tg;
                comb.add(key, s); } }
        if (!comb.cnt) continue;
        int csw[8], csb[8], EW = EXC ? max(pw, 1) : 1, EB = EXC ? max(pb, 1) : 1;
        for (int e = 0; e < EW; e++) csw[e] = csIdx[pw][e]; for (int e = 0; e < EB; e++) csb[e] = csIdx[pb][e];
        for (int ck = 0; ck < 2; ck++) {
            vector<vector<u64>> cum(EW * EB, vector<u64>(G4));
            for (int ew = 0; ew < EW; ew++) for (int eb = 0; eb < EB; eb++) { const u64* a = Acc[sig][ck].data() + (size_t)(csw[ew] * NS + csb[eb]) * G4; u64* c = cum[ew * EB + eb].data();
                for (int i = 0; i < G4; i++) { u64 v = a[i]; if (ew) v = am(v, cum[(ew - 1) * EB + eb][i]); if (eb) v = am(v, cum[ew * EB + eb - 1][i]); if (ew && eb) v = sm(v, cum[(ew - 1) * EB + eb - 1][i]); c[i] = v; } }
            FMap s4; s4.init(1 << 16);
            for (size_t ci = 0; ci < comb.k.size(); ci++) { if (!comb.k[ci]) continue; u64 key = comb.k[ci] - 1, val = comb.v[ci];
                int to = key & 7, ts = (key >> 3) & 7, mo = (key >> 6) & 31, n = (key >> 11) & 15, r = (key >> 15) & 15, q = (key >> 19) & 15, sbd = (key >> 23) & 15, sbl = (key >> 27) & 15, nd = (key >> 31) & 31, nl = (key >> 36) & 31;
                int bK = ck ? nl : nd, bO = ck ? nd : nl, sbK = ck ? sbl : sbd, Mt = mo + q + r + n; int tag = ts * 8 + to;
                int mq = Rl[9] ? min(q, 8) : 0, mr = Rl[9] ? min(r, 4) : 0, mn = Rl[8] ? min(n, 8) : 0;
                for (int jq = 0; jq <= mq; jq++) { u64 vq = mm(val, FALL[q][jq]); for (int jr = 0; jr <= mr; jr++) { u64 vr = mm(vq, FALL[r][jr]); for (int jn = 0; jn <= mn; jn++) { int u = jq + jr + jn; u64 x = mm(mm(vr, FALL[n][jn]), fall(Rtot - bK - bO - u, Mt - u)); if (!x) continue;
                            u64 k4 = ((((((u64)bK << 5 | bO) << 4 | sbK) << 4 | jq) << 3 | jr) << 4 | jn) << 6 | tag; s4.add(k4, x); } } } }
            vector<pair<u64, u64>> e2; { FMap E2; E2.init(s4.cnt * 2 + 16);
                for (size_t i = 0; i < s4.k.size(); i++) { if (!s4.k[i]) continue; u64 key = s4.k[i] - 1, val = s4.v[i]; int tag = key & 63, jn = (key >> 6) & 15, jr = (key >> 10) & 7, jq = (key >> 13) & 15, sbK = (key >> 17) & 15, bO = (key >> 21) & 31, bK = (key >> 26) & 31;
                    int mb = Rl[9] ? min(sbK, 4) : 0; for (int jb = 0; jb <= mb; jb++) { u64 x = mm(val, FALL[sbK][jb]); u64 k2 = ((((((u64)tag << 4 | jq) << 3 | jb) << 3 | jr) << 4 | jn) << 5 | (u64)(bK - jb)) << 5 | bO; E2.add(k2, x); } }
                for (size_t i = 0; i < E2.k.size(); i++) if (E2.k[i]) e2.push_back({E2.k[i] - 1, E2.v[i]}); }
            sort(e2.begin(), e2.end());
            size_t pos = 0;
            while (pos < e2.size()) { int tag = (int)(e2[pos].first >> 24); size_t end = pos; while (end < e2.size() && (int)(e2[end].first >> 24) == tag) end++;
                int ts = tag / 8, to = tag % 8; vector<int> used; int mq = 0, mb = 0, mr = 0, mn = 0;
                for (size_t i = pos; i < end; i++) { u64 key = e2[i].first; int bO = key & 31, bk2 = (key >> 5) & 31, jn = (key >> 10) & 15, jr = (key >> 14) & 7, jb = (key >> 17) & 7, jq = (key >> 20) & 15;
                    int Tj = Rtot - (jq + jb + jr + jn); if (Tj < 0) continue; int ji = ((jq * JB + jb) * JR + jr) * JN + jn;
                    if (i == pos || ((e2[i - 1].first >> 10) != (key >> 10))) { bool seen = false; for (int uu : used) if (uu == ji) { seen = true; break; } if (!seen) { used.push_back(ji); memset(Bd[ji], 0, sizeof(Bd[ji])); } }
                    mq = max(mq, jq); mb = max(mb, jb); mr = max(mr, jr); mn = max(mn, jn);
                    int hi = min(FKN - 1, Tj); u64 val = e2[i].second;
                    for (int FK = bk2; FK <= hi; FK++) Bd[ji][FK] = am(Bd[ji][FK], mm(val, mm(FALL[FK][bk2], fall(Tj - FK, bO)))); }
                pos = end;
                vector<char> has(JQ * JB * JR * JN, 0); for (int uu : used) has[uu] = 1;
                for (int jq = 0; jq <= mq; jq++) for (int jb = 0; jb <= mb; jb++) for (int jr = 0; jr <= mr; jr++) for (int N = 0; N < nNp; N++) { u128 acc[FKN]; for (int k = 0; k < FKN; k++) acc[k] = 0; bool any = false;
                                for (int jn = 0; jn <= min(N, mn); jn++) { int ji = ((jq * JB + jb) * JR + jr) * JN + jn; if (!has[ji]) continue; any = true; u64 c = BIN[N][jn]; if (jn & 1) c = sm(0, c); for (int k = 0; k < FKN; k++) acc[k] += (u128)c * Bd[ji][k]; }
                                for (int k = 0; k < FKN; k++) T1[jq][jb][jr][N][k] = any ? red128(acc[k]) : 0; }
                int mqd = min(4, mq);
                for (int qd = 0; qd <= mqd; qd++) for (int jb = 0; jb <= mb; jb++) for (int O = 0; O < nOp; O++) for (int N = 0; N < nNp; N++) { u128 acc[FKN]; for (int k = 0; k < FKN; k++) acc[k] = 0;
                                for (int qo = 0; qo <= O && qd + qo <= mq; qo++) for (int jr = 0; jr <= O - qo && jr <= mr; jr++) { u64 c = mm(BIN[O][qo], BIN[O - qo][jr]); if ((qo + jr) & 1) c = sm(0, c); const u64* t = T1[qd + qo][jb][jr][N]; for (int k = 0; k < FKN; k++) acc[k] += (u128)c * t[k]; }
                                for (int k = 0; k < FKN; k++) T2[qd][jb][O][N][k] = red128(acc[k]); }
                for (int TK = 0; TK < FKN; TK++) for (int D = 0; D < nDp; D++) for (int O = 0; O < nOp; O++) for (int N = 0; N < nNp; N++) { u128 acc = 0;
                                for (int qd = 0; qd <= D && qd <= mqd; qd++) for (int jb = 0; jb <= D - qd && jb <= mb; jb++) { int FK = TK - qd - jb; if (FK < 0) continue; u64 c = mm(BIN[D][qd], BIN[D - qd][jb]); if ((qd + jb) & 1) c = sm(0, c); acc += (u128)c * T2[qd][jb][O][N][FK]; }
                                PHI[TK][D][O][N] = red128(acc); }
                bool use32 = Rl[2] && ts == 0 && to == 0;
                if (use32 && !(pw == 8 && pb == 8)) continue;
                int thw[3], thb[3]; // mw = ts, mb = to
                thw[0] = EW - 1; thb[0] = EB - 1;
                thw[1] = to == 0 ? 0 : EW - 1; thb[1] = ts == 0 ? 0 : EB - 1;
                thw[2] = min(to, EW - 1); thb[2] = min(ts, EB - 1);
                const u64* vecs[3]; for (int m = 0; m < 3; m++) vecs[m] = use32 ? Acc32[sig][ck].data() : cum[thw[m] * EB + thb[m]].data();
                u128 ac[3] = {0, 0, 0}; u64 part[3] = {0, 0, 0}; int cnt = 0;
                for (int UK = 0; UK < NK; UK++) { int TK = 32 - UK; for (int UN = 0; UN < NN; UN++) { int N = Rl[8] ? 8 - UN : 0; for (int UO = 0; UO < NO; UO++) { int O = Rl[9] ? 4 - UO : 0; for (int UD = 0; UD < ND; UD++) { int D = Rl[9] ? 4 - UD : 0;
                                int pt = ((UK * NN + UN) * NO + UO) * ND + UD; u64 ph = PHI[TK][D][O][N]; if (!ph) continue;
                                for (int m = 0; m < 3; m++) ac[m] += (u128)ph * vecs[m][pt];
                                if (++cnt == 32) { for (int m = 0; m < 3; m++) { part[m] = am(part[m], red128(ac[m])); ac[m] = 0; } cnt = 0; } } } } }
                for (int m = 0; m < 3; m++) sres[m] = am(sres[m], am(part[m], red128(ac[m])));
            }
        }
    }
}
int main(int argc, char** argv) {
    init_math(); build_comps();
    bool basic = false, epOnly = false; vector<int> sigList;
    for (int i = 1; i < argc; i++) { string a = argv[i];
        if (a == "-x") basic = true; else if (a == "-e") epOnly = true; else if (a == "-noexc") EXC = false;
        else if (a == "-nt") NT = atoi(argv[++i]); else if (a == "-mcap") MCAP = atoi(argv[++i]);
        else if (a == "-sig") sigList.push_back(atoi(argv[++i]));
        else if (a == "-c") { COMPS.clear(); string spec = argv[++i]; size_t pos = 0; while (pos < spec.size()) { size_t e = spec.find(',', pos); if (e == string::npos) e = spec.size(); int p, q, r, b, n; sscanf(spec.substr(pos, e - pos).c_str(), "%d:%d:%d:%d:%d", &p, &q, &r, &b, &n); COMPS.push_back({p, q, r, b, n}); pos = e + 1; } }
        else Rl[atoi(argv[i])] = true; }
    MAXP = 0; for (auto& c : COMPS) MAXP = max(MAXP, c.p);
    for (int s = 0; s < NSG; s++) { int fw = s / 6, fb = (s / 2) % 3, ep = s % 2; sigOn[s] = basic ? (s == 0) : epOnly ? (fw == 0 && fb == 0) : true; }
    if (!sigList.empty()) { for (int s = 0; s < NSG; s++) sigOn[s] = false; for (int s : sigList) sigOn[s] = true; }
    bool withEp = false; for (int s = 0; s < NSG; s++) if (sigOn[s] && (s % 2)) withEp = true;
    init_geo();
    double t0 = clock();
    geometry(withEp);
    fprintf(stderr, "geometry %.1fs NS=%d G4=%d\n", (clock() - t0) / CLOCKS_PER_SEC, NS, GS[4]);
    for (int s = 0; s < NSG; s++) if (sigOn[s]) for (int c = 0; c < 2; c++) { for (int st = 0; st < NS2; st++) interp(Acc[s][c].data() + (size_t)st * GS[4]); interp(Acc32[s][c].data()); }
    fprintf(stderr, "interp %.1fs\n", (clock() - t0) / CLOCKS_PER_SEC);
    u64 tot[3] = {0, 0, 0};
    for (int s = 0; s < NSG; s++) { if (!sigOn[s]) continue; u64 sr[3]; officers(s, sr); printf("S %d %d %llu %llu %llu\n", PIDX, s, (unsigned long long)fromM(sr[0]), (unsigned long long)fromM(sr[1]), (unsigned long long)fromM(sr[2])); fflush(stdout);
        for (int m = 0; m < 3; m++) tot[m] = am(tot[m], sr[m]); fprintf(stderr, "sig %d done %.1fs\n", s, (clock() - t0) / CLOCKS_PER_SEC); }
    printf("W %d %llu %llu %llu\n", PIDX, (unsigned long long)fromM(tot[0]), (unsigned long long)fromM(tot[1]), (unsigned long long)fromM(tot[2]));
    return 0;
}
