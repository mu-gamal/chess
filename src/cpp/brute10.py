import sys, chess
from math import comb, perm
def nb(s,ds):
    f,r=chess.square_file(s),chess.square_rank(s); out=set()
    for df,dr in ds:
        nf,nr=f+df,r+dr
        if 0<=nf<8 and 0<=nr<8: out.add(chess.square(nf,nr))
    return out
DIAG=[(1,1),(1,-1),(-1,1),(-1,-1)]; ORTH=[(1,0),(-1,0),(0,1),(0,-1)]
KN=[(1,2),(2,1),(2,-1),(1,-2),(-1,-2),(-2,-1),(-2,1),(-1,2)]
PZ=set(s for s in chess.SQUARES if 1<=chess.square_rank(s)<=6)
EXTRAS=sys.argv[1].split(",")
rules=set(int(x) for x in sys.argv[2:])
total=0
for stm in (True,False):
  for ep in [None]+list(range(8)):
    if ep is None: fixed={}; empty=set(); A=set()
    else:
        if stm: fixed={chess.square(ep,4):'p'}; empty={chess.square(ep,5),chess.square(ep,6)}; A=nb(chess.square(ep,4),[(1,0),(-1,0)])
        else: fixed={chess.square(ep,3):'P'}; empty={chess.square(ep,2),chess.square(ep,1)}; A=nb(chess.square(ep,3),[(1,0),(-1,0)])
    base=[s for s in chess.SQUARES if s not in fixed and s not in empty]
    for wk in base:
      for bk in base:
        if wk==bk: continue
        if 6 in rules and chess.square_distance(wk,bk)<=1: continue
        tgt = bk if stm else wk
        fP=set(chess.SquareSet(chess.BB_PAWN_ATTACKS[not stm][tgt])) if 7 in rules else set()
        fN=nb(tgt,KN) if 8 in rules else set()
        free=[s for s in base if s not in (wk,bk)]
        for es in EXTRAS:
          for eo in EXTRAS:
            if ep is not None and 'P' not in eo: continue
            if es=="": stm_pl=[()]
            elif es=="P": stm_pl=[(s,) for s in free if s in PZ and s not in fP]
            elif es=="N": stm_pl=[(s,) for s in free if s not in fN]
            else: raise
            for pl in stm_pl:
                if 10 in rules and ep is not None and not (set(pl)&A and es=="P"): continue
                rem=set(free)-set(pl)
                left=eo.replace('P','',1) if ep is not None else eo
                if left=="": total+=1
                elif left=="P": total+=len(rem&PZ)
                elif left=="N": total+=len(rem)
                else: raise
print(total)
