import sys
P=[2305843009213693951, 2305843009213693921, 2305843009213693907, 2305843009213693723]
# usage: crt3.py file...  ; reads lines "W pidx A B C" ; prints 2*value for each mode
res={}
for fn in sys.argv[1:]:
    for l in open(fn):
        t=l.split()
        if t and t[0]=='W': res[int(t[1])]=[int(x) for x in t[2:5]]
ps=sorted(res)
for m,name in enumerate("ABC"):
    x=0; M=1
    for i in ps[:3]:
        p=P[i]; a=res[i][m]; t=((a-x)*pow(M,-1,p))%p; x+=M*t; M*=p
    chk = "" if len(ps)<4 else (" check_ok" if x%P[ps[3]]==res[ps[3]][m] else " CHECK_FAILED")
    print(name, 2*x, "%.4e"%(2*x), "primes=%d"%len(ps), chk)
