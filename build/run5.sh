#!/bin/bash
cd "$(dirname "$0")"
export TBDIR="$HOME/mnt/Chess Crack/tablebases"
OUT=count5_results.txt; touch $OUT
start=$(date +%s)
for e in $(ls "$TBDIR"/*.rtbw | xargs -n1 basename | sed 's/.rtbw//' | awk '{if(length($0)-1==5) print}'); do
  grep -q "^$e " $OUT && continue
  now=$(date +%s); el=$((now-start))
  if [[ "$e" == *P* ]]; then need=40; else need=16; fi
  [ $((el+need)) -gt 165 ] && break
  if [[ "$e" == *P* ]]; then hi=32; else hi=10; fi
  ./count5 $e 0 $hi >> $OUT
done
echo "done $(wc -l < $OUT)/110"
