import sys, itertools, chess
from math import comb
def nb(s,ds):
    f,r=chess.square_file(s),chess.square_rank(s); out=[]
    for df,dr in ds:
        nf,nr=f+df,r+dr
        if 0<=nf<8 and 0<=nr<8: out.append(chess.square(nf,nr))
    return set(out)
DIAG=[(1,1),(1,-1),(-1,1),(-1,-1)]; ORTH=[(1,0),(-1,0),(0,1),(0,-1)]
KN=[(1,2),(2,1),(2,-1),(1,-2),(-1,-2),(-2,-1),(-2,1),(-1,2)]
PZ=set(s for s in chess.SQUARES if 1<=chess.square_rank(s)<=6)
EXTRAS=sys.argv[1].split(",")
rules=set(int(x) for x in sys.argv[2:])
def place_stm(ex, free, forb):
    pieces=list(ex)
    if not pieces: yield (); return
    if len(ex)==2 and ex[0]==ex[1]:
        al=sorted(s for s in free if s not in forb[ex[0]])
        for i,a in enumerate(al):
            for b in al[i+1:]: yield (a,b)
        return
    def rec(i, used):
        if i==len(pieces): yield tuple(used); return
        t=pieces[i]
        for s in free:
            if s in used or s in forb[t]: continue
            if t=='P' and s not in PZ: continue
            yield from rec(i+1, used+[s])
    yield from rec(0,[])
def count_other(ex, rem):
    if ex=="": return 1
    if len(ex)==2 and ex[0]==ex[1]: return comb(len(rem),2)
    if ex=="P": return len(rem&PZ)
    if len(ex)==1: return len(rem)
    if ex[0]=='P' and len(ex)==2:
        return len(rem&PZ)*(len(rem)-1)
    n=1; r=len(rem)
    for t in ex:
        if t=='P': raise
        n*=r; r-=1
    return n
total=0
for stm in (True,False):
    for wk in chess.SQUARES:
        for bk in chess.SQUARES:
            if wk==bk: continue
            if 6 in rules and chess.square_distance(wk,bk)<=1: continue
            tgt = bk if stm else wk
            forb={'Q':set(),'R':set(),'B':set(),'N':set(),'P':set()}
            if 7 in rules: forb['P']=set(chess.SquareSet(chess.BB_PAWN_ATTACKS[not stm][tgt]))
            if 8 in rules: forb['N']=nb(tgt,KN)
            if 9 in rules:
                forb['Q']=nb(tgt,DIAG)|nb(tgt,ORTH); forb['R']=nb(tgt,ORTH); forb['B']=nb(tgt,DIAG)
            free=[s for s in chess.SQUARES if s not in (wk,bk)]
            for es in EXTRAS:
                for pl in place_stm(es, free, forb):
                    rem=set(free)-set(pl)
                    for eo in EXTRAS:
                        total+=count_other(eo, rem)
print(total)
