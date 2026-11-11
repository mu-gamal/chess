#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
static inline int F(int s){return s&7;} static inline int Rk(int s){return s>>3;}
static bool R[16]; static int NT=2;
static int bd[64]; // 0 empty, 1 wK,2 bK, 'P','N','Q','R','B' white upper; lower black
static ll cnt[3];
static int wk,bk;
static void evalPos(const string& wc,const string& bc){
    // rules 6-9 on black king by white pieces
    int wp=0,bp=0,wn=0,bn=0; int wf[8]={0},bf[8]={0};
    for(int s=0;s<64;s++){ int c=bd[s]; if(!c||c==1||c==2) continue;
        int df=abs(F(s)-F(bk)), dr=abs(Rk(s)-Rk(bk));
        if(c=='P'){ wp++; wf[F(s)]++; if(R[7]&&df==1&&Rk(s)==Rk(bk)-1) return; }
        else if(c=='p'){ bp++; bf[F(s)]++; }
        else if(isupper(c)){ wn++;
            if(c=='N'&&R[8]&&((df==1&&dr==2)||(df==2&&dr==1))) return;
            if(R[9]){ if(c=='Q'&&max(df,dr)==1) return; if(c=='R'&&df+dr==1) return; if(c=='B'&&df==1&&dr==1) return; } }
        else bn++; }
    int mw=NT-wp-wn, mb=NT-bp-bn; if(mw<0||mb<0) return;
    int ew=0,eb=0; for(int f=0;f<8;f++){ ew+=max(wf[f]-1,0); eb+=max(bf[f]-1,0); }
    cnt[0]++;
    if(!((mb==0&&ew>0)||(mw==0&&eb>0))) cnt[1]++;
    if(ew<=mb&&eb<=mw) cnt[2]++;
}
static void place(const string& pcs,int idx,int last,bool white,const function<void()>& next){
    if(idx==(int)pcs.size()){ next(); return; }
    char t=pcs[idx]; int start=(idx>0&&pcs[idx-1]==t)?last+1:0;
    for(int s=start;s<64;s++){ if(bd[s]) continue; if(t=='P'&&(Rk(s)<1||Rk(s)>6)) continue;
        bd[s]=white?t:tolower(t); place(pcs,idx+1,s,white,next); bd[s]=0; }
}
int main(int argc,char**argv){
    vector<string> comps; string spec=argv[1]; size_t pos=0; while(pos<=spec.size()){ size_t e=spec.find(',',pos); if(e==string::npos) e=spec.size(); comps.push_back(spec.substr(pos,e-pos)); pos=e+1; }
    NT=atoi(argv[2]); for(int i=3;i<argc;i++) R[atoi(argv[i])]=true;
    for(wk=0;wk<64;wk++) for(bk=0;bk<64;bk++){ if(wk==bk) continue; if(R[6]&&max(abs(F(wk)-F(bk)),abs(Rk(wk)-Rk(bk)))<=1) continue;
        bd[wk]=1; bd[bk]=2;
        for(auto& wc:comps) for(auto& bc:comps) place(wc,0,-1,true,[&](){ place(bc,0,-1,false,[&](){ evalPos(wc,bc); }); });
        bd[wk]=0; bd[bk]=0; }
    printf("%lld %lld %lld\n",cnt[0],cnt[1],cnt[2]);
}
