#!/bin/sh
# Run hsolve_many_cells.g over the cases that matter and print one line each.
#
#     sh genesis/Scripts/tests/run_hsolve_tests.sh [path to nxgenesis]
#
# Run from the top of the source tree. Exit 0 when every case passes.
BIN=${1:-./genesis/src/nxgenesis}
T=genesis/Scripts/tests/hsolve_many_cells.g
fail=0
for shape in "1 0" "3 0" "4 1"; do
    nc=${shape% *}; br=${shape#* }
    for m in 10 11; do
        for n in 1 8; do
            out=$("$BIN" -nosimrc -notty -batch "$T" "$n" "$nc" "$m" 2000 "$br" </dev/null 2>&1)
            rc=$?
            r=$(echo "$out" | grep "^HSOLVE_MANY_CELLS:" | head -1)
            [ "$rc" -gt 128 ] && r="CRASH (signal $((rc - 128)))"
            printf 'cells %-2s compartments %s branched %s method %s  %s\n' "$n" "$nc" "$br" "$m" "${r:-NO RESULT}"
            case "$r" in *PASS*) ;; *) fail=1 ;; esac
        done
    done
done
exit $fail
