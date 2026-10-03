#!/bin/sh
# Time construction_scaling.g for growing model size and fit the exponent of
# wall time against compartments (1 = linear).
#
#     sh genesis/Scripts/tests/run_construction_test.sh [path to nxgenesis] [N ...]
#
# Run from the top of the source tree.
BIN=${1:-./genesis/src/nxgenesis}
shift 2>/dev/null
NLIST=${*:-"1000 2000 4000 8000"}
T=genesis/Scripts/tests/construction_scaling.g
for n in $NLIST; do
    t0=$(date +%s%N)
    out=$("$BIN" -nosimrc -notty -batch "$T" "$n" </dev/null 2>&1)
    t1=$(date +%s%N)
    c=$(echo "$out" | sed -n 's/.*CONSTRUCTION_DONE cells *[0-9]* *compartments *\([0-9]*\).*/\1/p')
    echo "$n $c $(awk "BEGIN{printf \"%.3f\", ($t1-$t0)/1e9}")"
done | awk '{ printf "cells %6d  compartments %7s  %8.3f s\n", $1, $2, $3
              if ($2 > 0 && $3 > 0) { x = log($2); y = log($3); n++; sx += x; sy += y; sxx += x*x; sxy += x*y } }
         END { if (n > 1) printf "wall time ~ compartments^%.2f\n", (n*sxy - sx*sy) / (n*sxx - sx*sx) }'
