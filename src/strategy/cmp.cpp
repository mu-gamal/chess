#define NO_MAIN4
#include "four.cpp"
extern "C" {
#include "fathom/tbprobe.h"
}
int main(int argc, char** argv) {
    string tb = argv[1]; string nm = argv[2];
    if (!tb_init(tb.c_str()) || TB_LARGEST < 4) { printf("tb init failed %u\n", TB_LARGEST); return 1; }
    for (int t : {Q, R, B, Nn}) retro(t);
    retro(P);
    auto ty = [](char c) { return (int)string("QRBNP").find(c); };
    int ta = ty(nm[0]), tb2 = ty(nm[1]);
    if (ta == P && tb2 == P) { for (int pr : {Q, R, B, Nn}) { string fn = string(getenv("TBDIR")) + "/K" + "QRBN"[pr] + "PvK.b4"; FILE* fp = fopen(fn.c_str(), "rb"); vector<int8_t> v(N4); fread(v.data(), 1, N4, fp); fclose(fp); STB[{pr, P}] = v; } }
    else if (tb2 == P) { for (int pr : {Q, R, B, Nn}) { TA = min(ta, pr); TB = max(ta, pr); retro4(); STB[{TA, TB}] = B4; } }
    TA = ta; TB = tb2; retro4();
    long long mism = 0, n = 0;
    auto bb = [&](int type, int a, int b) { uint64_t r = 0; if (TA == type) r |= 1ULL << a; if (TB == type) r |= 1ULL << b; return r; };
    for (long long i = 0; i < N4; i++) { int wk = i >> 18, bk = (i >> 12) & 63, a = (i >> 6) & 63, b = i & 63;
        if (!valid4(wk, bk, a, b)) continue;
        uint64_t W = (1ULL << wk) | (1ULL << a) | (1ULL << b), Bl = 1ULL << bk, K = (1ULL << wk) | (1ULL << bk);
        for (int stm = 0; stm < 2; stm++) {
            if (stm == 0 && blackChecked(wk, bk, a, b)) continue;
            unsigned r = tb_probe_wdl(W, Bl, K, bb(Q, a, b), bb(R, a, b), bb(B, a, b), bb(Nn, a, b), bb(P, a, b), 0, 0, 0, stm == 0);
            n++;
            bool ours = stm == 0 ? W4[i] >= 0 : B4[i] >= 0;
            bool theirs = stm == 0 ? (r == TB_WIN || r == TB_CURSED_WIN) : (r == TB_LOSS || r == TB_BLESSED_LOSS);
            if (r == TB_RESULT_FAILED) { printf("probe failed\n"); return 1; }
            if (ours != theirs) { mism++; if (mism <= 12) { string f = fen4(i); if (stm) f.replace(f.find(" w "), 3, " b "); printf("%s ours=%s syzygy=%u\n", f.c_str(), ours ? "win" : "draw", r); } }
        } }
    printf("checked %lld, mismatches %lld\n", n, mism);
}
