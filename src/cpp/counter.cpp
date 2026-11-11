#include "head.inc"

struct PRArr { Z v[9]; };
typedef std::map<std::array<int, 6>, PRArr> SideMap;

static std::mutex side_mu;
static std::map<std::array<int, 5>, std::vector<SideMap>> SIDE_CACHE;

static const std::vector<SideMap>& side_table(int fixr, int fp, bool rule5, bool split_n, bool split_qrb) {
    std::lock_guard<std::mutex> lk(side_mu);
    std::array<int, 5> key = {fixr, fp, rule5, split_n, split_qrb};
    auto it = SIDE_CACHE.find(key);
    if (it != SIDE_CACHE.end()) return it->second;
    std::vector<SideMap> t(9);
    for (auto& c : COMPS) {
        if (c.p < fp || c.r < fixr) continue;
        int free_r = c.r - fixr;
        int n = split_n ? c.n : 0;
        int q = split_qrb ? c.q : 0;
        int r = split_qrb ? free_r : 0;
        int mo = c.q + free_r + c.n - n - q - r;
        int po = std::max(0, c.q - 1) + std::max(0, c.r - 2) + std::max(0, c.n - 2);
        for (int bl = 0; bl <= c.b; bl++) {
            int bd = c.b - bl, pr;
            if (rule5) { pr = po + std::max(0, bl - 1) + std::max(0, bd - 1); if (pr > 8 - c.p) continue; }
            else pr = po + std::max(0, c.b - 2);
            Z w = IFACZ[c.q] * IFACZ[free_r] * IFACZ[c.n] * IFACZ[bl] * IFACZ[bd];
            t[c.p][{bl, bd, q, r, n, mo}].v[pr] += w;
        }
    }
    auto res = SIDE_CACHE.emplace(key, t);
    return res.first->second;
}

static const int INF = 1000000000;
static int limitf(const Rules& R, int T, int pw, int pb) {
    int L = INF;
    if (R.has(1) && T == 32) L = 0;
    if (R.has(3)) L = std::min(L, 3 * (32 - T));
    if (R.has(4)) { int x = 2 * (32 - T) + 16 - pw - pb; int fl = x >= 0 ? x / 2 : -((-x + 1) / 2); L = std::min(L, fl); }
    return L;
}

static inline u64 pk8(int a, int b, int c, int d, int e, int f, int g, int h) {
    return ((u64)a << 35) | ((u64)b << 30) | ((u64)c << 25) | ((u64)d << 20) | ((u64)e << 15) | ((u64)f << 10) | ((u64)g << 5) | (u64)h;
}
static inline int uf(u64 k, int i) { return (int)((k >> (35 - 5 * i)) & 31); }

static const int JQ = 9, JB = 5, JR = 5, JN = 9;
static inline int jidx(int q, int b, int r, int n) { return ((q * JB + b) * JR + r) * JN + n; }

struct OffTable { int Rtot; std::unordered_map<int, std::vector<Z>> B[2]; bool empty; };

struct Ctx {
    std::map<std::array<int, 8>, std::pair<OffTable, OffTable>> off_cache;
    std::map<std::vector<int>, std::vector<std::pair<std::array<int, 4>, Z>>> pawn_cache2;
    std::unordered_map<std::string, Z> phi_cache;
};

struct K7 { int a[7]; bool operator==(const K7& o) const { return memcmp(a, o.a, sizeof(a)) == 0; } };
struct K7H { size_t operator()(const K7& k) const { u64 h = 1469598103934665603ULL; for (int i = 0; i < 7; i++) { h ^= (u64)k.a[i] + 0x9e3779b97f4a7c15ULL; h *= 1099511628211ULL; } return (size_t)h; } };
typedef std::unordered_map<K7, Z, K7H> M7;

static void build_B(const std::unordered_map<u64, Z>& comb, const Rules& R, int Rtot, OffTable& T) {
    T.Rtot = Rtot; T.empty = comb.empty();
    bool r8 = R.has(8), r9 = R.has(9);
    for (int ck = 0; ck < 2; ck++) {
        M7 s0; s0.reserve(comb.size());
        for (auto& kv : comb) {
            u64 k = kv.first;
            int nl = uf(k, 0), nd = uf(k, 1), sbl = uf(k, 2), sbd = uf(k, 3), q = uf(k, 4), r = uf(k, 5), n = uf(k, 6), mo = uf(k, 7);
            int bK = ck ? nl : nd, bO = ck ? nd : nl, sbK = ck ? sbl : sbd;
            K7 key{{bK, bO, sbK, mo + q + r + n, q, r, n}};
            s0[key] += kv.second;
        }
        M7 s1; s1.reserve(s0.size() * 2);
        for (auto& kv : s0) { int q = kv.first.a[4]; int mq = r9 ? std::min(q, 8) : 0;
            for (int jq = 0; jq <= mq; jq++) { K7 key = kv.first; key.a[4] = jq; s1[key] += kv.second * falling(q, jq); } }
        M7().swap(s0);
        M7 s2; s2.reserve(s1.size() * 2);
        for (auto& kv : s1) { int r = kv.first.a[5]; int mr = r9 ? std::min(r, 4) : 0;
            for (int jr = 0; jr <= mr; jr++) { K7 key = kv.first; key.a[5] = jr; s2[key] += kv.second * falling(r, jr); } }
        M7().swap(s1);
        M7 s3; s3.reserve(s2.size() * 2);
        for (auto& kv : s2) { int n = kv.first.a[6]; int mn = r8 ? std::min(n, 8) : 0;
            for (int jn = 0; jn <= mn; jn++) { K7 key = kv.first; key.a[6] = jn; s3[key] += kv.second * falling(n, jn); } }
        M7().swap(s2);
        M7 s4; s4.reserve(s3.size());
        for (auto& kv : s3) {
            int bK = kv.first.a[0], bO = kv.first.a[1], M = kv.first.a[3];
            int u = kv.first.a[4] + kv.first.a[5] + kv.first.a[6];
            Z x = kv.second * falling(Rtot - bK - bO - u, M - u);
            if (x.zero()) continue;
            K7 key = kv.first; key.a[3] = 0;
            s4[key] += x;
        }
        M7().swap(s3);
        std::unordered_map<u64, Z> E2;
        for (auto& kv : s4) {
            int bK = kv.first.a[0], bO = kv.first.a[1], sbK = kv.first.a[2], jq = kv.first.a[4], jr = kv.first.a[5], jn = kv.first.a[6];
            int mb = r9 ? std::min(sbK, 4) : 0;
            for (int jb = 0; jb <= mb; jb++) {
                Z x = kv.second * falling(sbK, jb);
                if (x.zero()) continue;
                u64 key = ((u64)jidx(jq, jb, jr, jn) << 16) | ((u64)(bK - jb) << 8) | (u64)bO;
                E2[key] += x;
            }
        }
        auto& B = T.B[ck];
        for (auto& kv : E2) {
            int j = (int)(kv.first >> 16), bk2 = (int)((kv.first >> 8) & 255), bO = (int)(kv.first & 255);
            int jn = j % JN, jr = (j / JN) % JR, jb = (j / JN / JR) % JB, jq = j / JN / JR / JB;
            int Tj = Rtot - (jq + jb + jr + jn);
            if (Tj < 0) continue;
            auto& v = B[j];
            if (v.empty()) v.assign(Tj + 1, Z());
            for (int FK = 0; FK <= Tj; FK++) {
                Z x = falling(FK, bk2) * falling(Tj - FK, bO);
                if (!x.zero()) v[FK] += kv.second * x;
            }
        }
    }
}

static const std::pair<OffTable, OffTable>& offtables(Ctx& cx, const Rules& R, int fixwr, int fixbr, int fwp, int fbp, int pw, int pb, int stm, int Rtot) {
    std::array<int, 8> k = {fixwr, fixbr, fwp, fbp, pw, pb, stm, Rtot};
    auto it = cx.off_cache.find(k);
    if (it != cx.off_cache.end()) return it->second;
    bool rule5 = R.has(5);
    bool sn = R.has(8), sq9 = R.has(9);
    const auto& WT = side_table(fixwr, fwp, rule5, sn && stm == WHITE, sq9 && stm == WHITE);
    const auto& BT = side_table(fixbr, fbp, rule5, sn && stm == BLACK, sq9 && stm == BLACK);
    const SideMap& st = stm == WHITE ? WT[pw] : BT[pb];
    const SideMap& ot = stm == WHITE ? BT[pb] : WT[pw];
    int fixed_off = 2 + fixwr + fixbr;
    struct OI { int bl, bd, mo, tot; Z c[18]; };
    std::vector<OI> oitems;
    for (auto& kv : ot) {
        OI x; x.bl = kv.first[0]; x.bd = kv.first[1]; x.mo = kv.first[5]; x.tot = x.bl + x.bd + x.mo;
        Z s; for (int i = 0; i < 9; i++) { s += kv.second.v[i]; x.c[i] = s; } for (int i = 9; i < 18; i++) x.c[i] = s;
        oitems.push_back(x);
    }
    int Ltab[80];
    for (int n = 0; n < 80; n++) Ltab[n] = std::min(limitf(R, pw + pb + n + fixed_off, pw, pb), 16);
    std::unordered_map<u64, Z> out, out32; out.reserve(1 << 20);
    for (auto& kv : st) {
        int bl = kv.first[0], bd = kv.first[1], q = kv.first[2], r = kv.first[3], n = kv.first[4], mo = kv.first[5];
        int sn_ = bl + bd + q + r + n + mo;
        std::vector<std::pair<int, Z>> nz;
        for (int i = 0; i < 9; i++) if (!kv.second.v[i].zero()) nz.push_back({i, kv.second.v[i]});
        if (nz.empty()) continue;
        int sbl = sq9 ? bl : 0, sbd = sq9 ? bd : 0;
        for (auto& o : oitems) {
            int ntot = sn_ + o.tot;
            int L = Ltab[ntot];
            if (L < 0) continue;
            Z s; bool any = false;
            for (auto& ia : nz) { int j = L - ia.first; if (j < 0) break; s += ia.second * o.c[j]; any = true; }
            if (!any || s.zero()) continue;
            u64 key = pk8(bl + o.bl, bd + o.bd, sbl, sbd, q, r, n, mo + o.mo);
            if (pw + pb + ntot + fixed_off == 32) out32[key] += s; else out[key] += s;
        }
    }
    std::pair<OffTable, OffTable> P;
    build_B(out, R, Rtot, P.first);
    build_B(out32, R, Rtot, P.second);
    auto res = cx.off_cache.emplace(k, std::move(P));
    return res.first->second;
}

struct Cat { int size; bool att_ok; bool K; bool D; bool O; bool N; };

static const std::vector<std::pair<std::array<int, 4>, Z>>& pawn_dist(Ctx& cx, const std::vector<std::array<int, 6>>& catv, int a, int o) {
    std::vector<int> key;
    for (auto& c : catv) for (int x : c) key.push_back(x);
    key.push_back(a); key.push_back(o);
    auto it = cx.pawn_cache2.find(key);
    if (it != cx.pawn_cache2.end()) return it->second;
    std::map<std::array<int, 6>, Z> st; st[{0, 0, 0, 0, 0, 0}] = Z(1);
    for (auto& cv : catv) {
        Cat c{cv[0], (bool)cv[1], (bool)cv[2], (bool)cv[3], (bool)cv[4], (bool)cv[5]};
        std::map<std::array<int, 6>, Z> nx;
        for (auto& kv : st) {
            int ua = kv.first[0], uo = kv.first[1];
            int maxx = c.att_ok ? std::min(a - ua, c.size) : 0;
            for (int x = 0; x <= maxx; x++) for (int y = 0; y <= std::min(o - uo, c.size - x); y++) {
                Z w = kv.second * binz(c.size, x) * binz(c.size - x, y);
                int t = x + y;
                std::array<int, 6> nk = {ua + x, uo + y, kv.first[2] + (c.K ? t : 0), kv.first[3] + (c.D ? t : 0), kv.first[4] + (c.O ? t : 0), kv.first[5] + (c.N ? t : 0)};
                nx[nk] += w;
            }
        }
        st.swap(nx);
    }
    std::map<std::array<int, 4>, Z> agg;
    for (auto& kv : st) if (kv.first[0] == a && kv.first[1] == o) agg[{kv.first[2], kv.first[3], kv.first[4], kv.first[5]}] += kv.second;
    std::vector<std::pair<std::array<int, 4>, Z>> out(agg.begin(), agg.end());
    auto res = cx.pawn_cache2.emplace(key, out);
    return res.first->second;
}

static Z phi(Ctx& cx, const OffTable& T, u64 tid, const Rules& R, int ck, int TK, int D, int O, int N) {
    char buf[128]; snprintf(buf, sizeof(buf), "%llu|%d|%d|%d|%d|%d", (unsigned long long)tid, ck, TK, D, O, N);
    std::string k(buf);
    auto it = cx.phi_cache.find(k);
    if (it != cx.phi_cache.end()) return it->second;
    const auto& B = T.B[ck];
    bool r8 = R.has(8), r9 = R.has(9);
    Z total;
    int mD = r9 ? D : 0, mO = r9 ? O : 0, mN = r8 ? N : 0;
    for (int qd = 0; qd <= mD; qd++) for (int jb = 0; jb <= mD - qd; jb++)
    for (int qo = 0; qo <= mO; qo++) for (int jr = 0; jr <= mO - qo; jr++)
    for (int jn = 0; jn <= mN; jn++) {
        int jq = qd + qo;
        if (jq >= JQ || jb >= JB || jr >= JR || jn >= JN) continue;
        auto bit = B.find(jidx(jq, jb, jr, jn));
        if (bit == B.end()) continue;
        int FK = TK - qd - jb;
        if (FK < 0 || FK >= (int)bit->second.size()) continue;
        Z c = binz(D, qd) * binz(D - qd, jb) * binz(O, qo) * binz(O - qo, jr) * binz(N, jn) * bit->second[FK];
        if ((jq + jb + jr + jn) & 1) total = total - c; else total += c;
    }
    cx.phi_cache.emplace(k, total);
    return total;
}

struct Piece { int type, color; };
enum { NONE = 0, PAWN = 1, KING = 6, ROOK = 4 };
struct Case { int stm; std::string cw, cb; int ep; };

static std::vector<Case> all_cases() {
    std::vector<Case> v;
    const char* opts[4] = {"", "K", "Q", "KQ"};
    for (int s = 0; s < 2; s++) { int stm = s == 0 ? WHITE : BLACK;
        for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) for (int ep = -1; ep < 8; ep++) v.push_back({stm, opts[i], opts[j], ep}); }
    return v;
}

static void geometry(const Case& c, std::map<int, Piece>& fixed, std::set<int>& empty, int& fwp, int& fbp) {
    fixed.clear(); empty.clear(); fwp = fbp = 0;
    auto has = [](const std::string& s, char ch) { return s.find(ch) != std::string::npos; };
    if (!c.cw.empty()) fixed[sq(4, 0)] = {KING, WHITE};
    if (has(c.cw, 'K')) fixed[sq(7, 0)] = {ROOK, WHITE};
    if (has(c.cw, 'Q')) fixed[sq(0, 0)] = {ROOK, WHITE};
    if (!c.cb.empty()) fixed[sq(4, 7)] = {KING, BLACK};
    if (has(c.cb, 'K')) fixed[sq(7, 7)] = {ROOK, BLACK};
    if (has(c.cb, 'Q')) fixed[sq(0, 7)] = {ROOK, BLACK};
    if (c.ep >= 0) {
        if (c.stm == WHITE) { fixed[sq(c.ep, 4)] = {PAWN, BLACK}; empty.insert(sq(c.ep, 5)); empty.insert(sq(c.ep, 6)); fbp = 1; }
        else { fixed[sq(c.ep, 3)] = {PAWN, WHITE}; empty.insert(sq(c.ep, 2)); empty.insert(sq(c.ep, 1)); fwp = 1; }
    }
}

static std::vector<int> pawn_attacks(int color, int s) {
    std::vector<int> v; int f = sqf(s), r = sqr(s); int dr = color == WHITE ? 1 : -1;
    for (int df : {-1, 1}) { int nf = f + df, nr = r + dr; if (nf >= 0 && nf < 8 && nr >= 0 && nr < 8) v.push_back(sq(nf, nr)); }
    return v;
}
static std::vector<int> knight_attacks(int s) {
    std::vector<int> v; int f = sqf(s), r = sqr(s);
    int d[8][2] = {{1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}};
    for (auto& x : d) { int nf = f + x[0], nr = r + x[1]; if (nf >= 0 && nf < 8 && nr >= 0 && nr < 8) v.push_back(sq(nf, nr)); }
    return v;
}
static std::vector<int> diag_nb(int s) { std::vector<int> v; int f = sqf(s), r = sqr(s); for (int df : {-1, 1}) for (int dr : {-1, 1}) { int nf = f + df, nr = r + dr; if (nf >= 0 && nf < 8 && nr >= 0 && nr < 8) v.push_back(sq(nf, nr)); } return v; }
static std::vector<int> orth_nb(int s) { std::vector<int> v; int f = sqf(s), r = sqr(s); int d[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}; for (auto& x : d) { int nf = f + x[0], nr = r + x[1]; if (nf >= 0 && nf < 8 && nr >= 0 && nr < 8) v.push_back(sq(nf, nr)); } return v; }
static inline int dist(int a, int b) { return std::max(abs(sqf(a) - sqf(b)), abs(sqr(a) - sqr(b))); }

static std::map<std::array<int, 4>, Z> file_pair_poly(const std::map<int, Piece>& fixed, const std::set<int>& empty, const std::set<int>& forb, int attacker, bool ckl, const std::set<int>& dset, const std::set<int>& oset, const std::set<int>& nset) {
    std::map<std::array<int, 4>, u64> poly; poly[{0, 0, 0, 0}] = 1;
    for (int f = 0; f < 8; f++) {
        std::map<std::array<int, 4>, u64> fp;
        for (int w = 1; w < 7; w++) for (int b = w + 1; b < 7; b++) {
            bool ok = true;
            for (int r = 1; r < 7 && ok; r++) {
                int s = sq(f, r);
                auto it = fixed.find(s);
                const Piece* pc = it == fixed.end() ? nullptr : &it->second;
                int want = r == w ? WHITE : (r == b ? BLACK : -1);
                if (want == -1) { if (pc && pc->type == PAWN) ok = false; }
                else if (empty.count(s) || (pc && !(pc->type == PAWN && pc->color == want))) ok = false;
                else if (want == attacker && forb.count(s) && !pc) ok = false;
            }
            if (ok) {
                std::array<int, 4> inc = {0, 0, 0, 0};
                for (int r : {w, b}) { int s = sq(f, r); if (fixed.count(s)) continue; if (light(s) == ckl) inc[0]++; if (dset.count(s)) inc[1]++; if (oset.count(s)) inc[2]++; if (nset.count(s)) inc[3]++; }
                fp[inc] += 1;
            }
        }
        std::map<std::array<int, 4>, u64> nw;
        for (auto& a : poly) for (auto& c : fp) nw[{a.first[0] + c.first[0], a.first[1] + c.first[1], a.first[2] + c.first[2], a.first[3] + c.first[3]}] += a.second * c.second;
        poly = nw;
    }
    std::map<std::array<int, 4>, Z> out;
    for (auto& kv : poly) out[kv.first] = Z(kv.second);
    return out;
}

struct Grp { int TK, ck; std::vector<std::array<int, 6>> cats; int dout, oout, nout;
    bool operator<(const Grp& o) const { return std::tie(TK, ck, cats, dout, oout, nout) < std::tie(o.TK, o.ck, o.cats, o.dout, o.oout, o.nout); } };

static Z case_total(Ctx& cx, const Case& c, const Rules& R, const std::set<int>& extraA) {
    std::map<int, Piece> fixed; std::set<int> empty; int fwp, fbp;
    geometry(c, fixed, empty, fwp, fbp);
    int fixwr = 0, fixbr = 0;
    for (char ch : c.cw) if (ch == 'K' || ch == 'Q') fixwr++;
    for (char ch : c.cb) if (ch == 'K' || ch == 'Q') fixbr++;
    int stm = c.stm;
    int wkf = -1, bkf = -1;
    for (auto& kv : fixed) if (kv.second.type == KING) { if (kv.second.color == WHITE) wkf = kv.first; else bkf = kv.first; }
    std::vector<int> freesq;
    for (int s = 0; s < 64; s++) if (!fixed.count(s) && !empty.count(s)) freesq.push_back(s);
    std::vector<int> wopts = wkf >= 0 ? std::vector<int>{wkf} : freesq;
    std::vector<int> bopts = bkf >= 0 ? std::vector<int>{bkf} : freesq;
    bool r7 = R.has(7), r8 = R.has(8), r9 = R.has(9);
    std::map<Grp, u64> groups;
    struct G32 { int TK, ck; std::vector<int> kz, forb, ds, os, ns; int dout, oout, nout;
        bool operator<(const G32& o) const { return std::tie(TK, ck, kz, forb, ds, os, ns, dout, oout, nout) < std::tie(o.TK, o.ck, o.kz, o.forb, o.ds, o.os, o.ns, o.dout, o.oout, o.nout); } };
    std::map<G32, u64> groups32;
    int Rbase = -1;
    for (int wk : wopts) for (int bk : bopts) {
        if (wk == bk) continue;
        if (R.has(6) && dist(wk, bk) <= 1) continue;
        bool occ[64] = {false};
        for (auto& kv : fixed) occ[kv.first] = true;
        for (int s : empty) occ[s] = true;
        occ[wk] = occ[bk] = true;
        int target = stm == WHITE ? bk : wk;
        bool ckl = light(target);
        int TK = 0, TT = 0;
        for (int s = 0; s < 64; s++) if (!occ[s]) { TT++; if (light(s) == ckl) TK++; }
        Rbase = TT;
        std::set<int> forb, dset, oset, nset;
        bool bad = false;
        if (r7) for (int s : pawn_attacks(stm == WHITE ? BLACK : WHITE, target)) {
            auto it = fixed.find(s);
            if (it != fixed.end() && it->second.type == PAWN && it->second.color == stm) bad = true;
            if (pawnzone(s) && !occ[s]) forb.insert(s);
        }
        if (r9) {
            for (int s : orth_nb(target)) { auto it = fixed.find(s); if (it != fixed.end() && it->second.type == ROOK && it->second.color == stm) bad = true; }
            for (int s : diag_nb(target)) if (!occ[s]) dset.insert(s);
            for (int s : orth_nb(target)) if (!occ[s]) oset.insert(s);
        }
        if (bad) continue;
        if (r8) for (int s : knight_attacks(target)) if (!occ[s]) nset.insert(s);
        for (int s : extraA) if (pawnzone(s) && !occ[s]) forb.insert(s);
        int dout = 0, oout = 0, nout = 0;
        std::map<std::array<int, 5>, int> cm;
        for (int s = 0; s < 64; s++) {
            if (occ[s]) continue;
            bool pz = pawnzone(s);
            if (!pz) { if (dset.count(s)) dout++; if (oset.count(s)) oout++; if (nset.count(s)) nout++; continue; }
            cm[{(int)!forb.count(s), (int)(light(s) == ckl), (int)dset.count(s), (int)oset.count(s), (int)nset.count(s)}]++;
        }
        std::vector<std::array<int, 6>> cats;
        for (auto& kv : cm) cats.push_back({kv.second, kv.first[0], kv.first[1], kv.first[2], kv.first[3], kv.first[4]});
        groups[{TK, (int)ckl, cats, dout, oout, nout}]++;
        if (R.has(2)) {
            std::vector<int> kz;
            for (int s : {wk, bk}) if (!fixed.count(s) && pawnzone(s)) kz.push_back(s);
            std::sort(kz.begin(), kz.end());
            std::vector<int> fv(forb.begin(), forb.end()), dv, ov, nv;
            for (int s : dset) if (pawnzone(s)) dv.push_back(s);
            for (int s : oset) if (pawnzone(s)) ov.push_back(s);
            for (int s : nset) if (pawnzone(s)) nv.push_back(s);
            groups32[{TK, (int)ckl, kz, fv, dv, ov, nv, dout, oout, nout}]++;
        }
    }
    if (Rbase < 0) return Z();
    int sigk = ((fixwr * 3 + fixbr) * 2 + fwp) * 2 + fbp;
    Z total;
    for (auto& gv : groups) {
        const Grp& G = gv.first;
        int pzfree = 0, Dtot = G.dout, Otot = G.oout, Ntot = G.nout;
        for (auto& c : G.cats) { pzfree += c[0]; if (c[3]) Dtot += c[0]; if (c[4]) Otot += c[0]; if (c[5]) Ntot += c[0]; }
        Z sub;
        for (int pw = fwp; pw <= 8; pw++) for (int pb = fbp; pb <= 8; pb++) {
            int pwf = pw - fwp, pbf = pb - fbp;
            if (pwf + pbf > pzfree) continue;
            int Rtot = Rbase - pwf - pbf;
            const auto& OT = offtables(cx, R, fixwr, fixbr, fwp, fbp, pw, pb, stm, Rtot);
            int a = stm == WHITE ? pwf : pbf, o = stm == WHITE ? pbf : pwf;
            const auto& PD = pawn_dist(cx, G.cats, a, o);
            u64 tid = ((((u64)sigk * 9 + pw) * 9 + pb) * 2 + (stm == WHITE)) * 4;
            for (auto& e : PD) {
                int uK = e.first[0], uD = e.first[1], uO = e.first[2], uN = e.first[3];
                int TKp = G.TK - uK;
                if (TKp < 0 || TKp > Rtot) continue;
                Z v;
                if (!OT.first.empty) v += phi(cx, OT.first, tid, R, G.ck, TKp, Dtot - uD, Otot - uO, Ntot - uN);
                if (!R.has(2) && !OT.second.empty) v += phi(cx, OT.second, tid + 1, R, G.ck, TKp, Dtot - uD, Otot - uO, Ntot - uN);
                sub += v * e.second;
            }
        }
        total += sub * Z(gv.second);
    }
    if (R.has(2)) {
        int pwf = 8 - fwp, pbf = 8 - fbp;
        int Rtot = Rbase - pwf - pbf;
        const auto& OT = offtables(cx, R, fixwr, fixbr, fwp, fbp, 8, 8, stm, Rtot);
        if (!OT.second.empty) {
            u64 tid = ((((u64)sigk * 9 + 8) * 9 + 8) * 2 + (stm == WHITE)) * 4 + 1;
            for (auto& gv : groups32) {
                const G32& G = gv.first;
                std::map<int, Piece> f2 = fixed;
                for (int s : G.kz) f2[s] = {KING, WHITE};
                std::set<int> fb(G.forb.begin(), G.forb.end()), ds(G.ds.begin(), G.ds.end()), os(G.os.begin(), G.os.end()), ns(G.ns.begin(), G.ns.end());
                auto poly = file_pair_poly(f2, empty, fb, stm, G.ck, ds, os, ns);
                int Dtot = (int)G.ds.size() + G.dout, Otot = (int)G.os.size() + G.oout, Ntot = (int)G.ns.size() + G.nout;
                Z v;
                for (auto& e : poly) {
                    int TKp = G.TK - e.first[0];
                    v += phi(cx, OT.second, tid, R, G.ck, TKp, Dtot - e.first[1], Otot - e.first[2], Ntot - e.first[3]) * e.second;
                }
                total += v * Z(gv.second);
            }
        }
    }
    return total;
}

int main(int argc, char** argv) {
    init_math();
    build_comps();
    Rules R;
    int threads = 2;
    bool basic_only = false, no_castle = false; int only_w = -1;
    for (int i = 1; i < argc; i++) {
        if (!strncmp(argv[i], "-j", 2)) { threads = atoi(argv[i] + 2); continue; }
        if (!strcmp(argv[i], "-x")) { basic_only = true; continue; }
        if (!strcmp(argv[i], "-e")) { no_castle = true; continue; }
        if (!strcmp(argv[i], "-w")) { only_w = atoi(argv[++i]); continue; }
        if (!strcmp(argv[i], "-c")) {
            COMPS.clear();
            std::string spec = argv[++i];
            size_t pos = 0;
            while (pos < spec.size()) {
                size_t e = spec.find(',', pos); if (e == std::string::npos) e = spec.size();
                int p, q, r, b, n; sscanf(spec.substr(pos, e - pos).c_str(), "%d:%d:%d:%d:%d", &p, &q, &r, &b, &n);
                COMPS.push_back({p, q, r, b, n}); pos = e + 1;
            }
            continue;
        }
        R.r[atoi(argv[i])] = true;
    }
    auto cases0 = all_cases();
    std::vector<Case> cases;
    for (auto& c : cases0) if ((!basic_only || (c.cw.empty() && c.cb.empty() && c.ep < 0)) && (!no_castle || (c.cw.empty() && c.cb.empty()))) cases.push_back(c);
    std::map<std::array<int, 4>, std::vector<int>> bysig;
    for (size_t i = 0; i < cases.size(); i++) {
        int fw = 0, fb = 0; for (char ch : cases[i].cw) if (ch != ' ') fw++; for (char ch : cases[i].cb) if (ch != ' ') fb++;
        bysig[{cases[i].stm, fw, fb, cases[i].ep >= 0}].push_back((int)i);
    }
    std::vector<std::vector<int>> work;
    for (auto& kv : bysig) work.push_back(kv.second);
    std::vector<size_t> gid;
    for (size_t i = 0; i < work.size(); i++) gid.push_back(i);
    if (only_w >= 0) { auto w = work[only_w]; work.clear(); work.push_back(w); gid.clear(); gid.push_back(only_w); }
    if (getenv("CC_GROUPS")) {
        std::vector<std::vector<int>> w2; std::vector<size_t> g2;
        std::string spec = getenv("CC_GROUPS"); size_t pos = 0;
        while (pos < spec.size()) { size_t e = spec.find(',', pos); if (e == std::string::npos) e = spec.size(); int g = atoi(spec.substr(pos, e - pos).c_str()); w2.push_back(work[g]); g2.push_back(g); pos = e + 1; }
        work = w2; gid = g2;
    }
    std::vector<Z> part(cases.size());
    std::vector<std::thread> th;
    std::mutex mu; size_t next = 0;
    for (int t = 0; t < threads; t++) th.emplace_back([&]() {
        while (true) {
            size_t w;
            { std::lock_guard<std::mutex> lk(mu); if (next >= work.size()) break; w = next++; }
            Ctx cx;
            Z gs;
            for (int i : work[w]) {
                std::set<int> none;
                Z v = case_total(cx, cases[i], R, none);
                const Case& cc = cases[i];
                if (R.has(10) && cc.ep >= 0) {
                    int rr = cc.stm == WHITE ? 4 : 3;
                    std::set<int> A;
                    for (int df : {-1, 1}) { int nf = cc.ep + df; if (nf >= 0 && nf < 8) A.insert(sq(nf, rr)); }
                    v = v - case_total(cx, cc, R, A);
                }
                part[i] = v; gs += v;
            }
            { std::lock_guard<std::mutex> lk(mu); printf("G %zu", gid[w]); for (int k = 0; k < NP; k++) printf(" %llu", (unsigned long long)gs.a[k]); printf("\n"); fflush(stdout); }
        }
    });
    for (auto& t : th) t.join();
    Z tot;
    for (auto& z : part) tot += z;
    printf("T");
    for (int i = 0; i < NP; i++) printf(" %llu", (unsigned long long)tot.a[i]);
    printf("\n");
    return 0;
}
