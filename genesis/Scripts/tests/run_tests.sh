#!/bin/sh
# Regression tests of the GENESIS core that need no GPU: the element tree and
# its layout, readcell, and the Hines solver with several cells under one
# hsolve. Each script prints one "<NAME>: PASS" or "<NAME>: FAIL (...)" line.
#
#     sh genesis/Scripts/tests/run_tests.sh [path to nxgenesis]
#
# Run from the top of the source tree. Exit 0 when every test passes.
BIN=${1:-./genesis/src/nxgenesis}
T=genesis/Scripts/tests
fail=0
one() {   # <label> <script> [args...]: run, report, never let a crash pass
    label=$1; shift
    out=$("$BIN" -nosimrc -notty -batch "$@" < /dev/null 2>&1); rc=$?
    r=$(echo "$out" | grep -E "^[A-Z_]+: (PASS|FAIL)" | head -1)
    [ "$rc" -gt 128 ] && r="CRASH (signal $((rc - 128)))"
    printf '%-44s %s\n' "$label" "${r:-NO RESULT}"
    case "$r" in *PASS*) ;; *) fail=1 ;; esac
}
one "element layout"           $T/element_layout.g
one "readcell, coordinates"    $T/readcell_coords.g
one "readcell, *makeproto"     $T/makeproto.g
one "re-creating an element"   $T/duplicate_element.g
one "long pathnames"           $T/long_paths.g
one "one compartment, Euler"   $T/single_compartment_euler.g
for shape in "1 0" "3 0" "4 1"; do
    nc=${shape% *}; br=${shape#* }
    for m in 10 11; do
        for n in 1 2 3 8; do
            one "hsolve: $n cells x $nc comp, branched $br, method $m" \
                $T/hsolve_many_cells.g "$n" "$nc" "$m" 2000 "$br"
        done
    done
done
exit $fail
