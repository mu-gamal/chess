import sys
P=[4611686018427387847, 4611686018427387817, 4611686018427387787, 4611686018427387761]
groups={}; total=None
for line in sys.stdin:
    t=line.split()
    if not t: continue
    if t[0]=="G": groups[int(t[1])]=[int(x) for x in t[2:]]
    elif t[0]=="T": total=[int(x) for x in t[1:]]
    else: total=[int(x) for x in t]
if total is None or (len(sys.argv)>1 and sys.argv[1]=="groups"):
    need=int(sys.argv[2]) if len(sys.argv)>2 else None
    if need is not None:
        missing=[g for g in range(need) if g not in groups]
        if missing: print("missing groups", missing); sys.exit(1)
    total=[sum(v[i] for v in groups.values())%P[i] for i in range(4)]
M=1; x=0
for p,a in zip(P[:3],total[:3]):
    t=((a-x)*pow(M,-1,p))%p
    x+=M*t; M*=p
ok = x % P[3] == total[3]
print(x, "%.4e"%x, "check_ok" if ok else "CHECK_FAILED")
