// genesis
//
// Elements whose pathnames are long (244 characters here, built with relative
// names) can be created, listed and probed for missing children, through the
// hash lookups in Attach() and in path resolution. GENESIS stops at about 300
// characters elsewhere, in older code, so this stays below that.
//
// Prints "LONG_PATHS: PASS" or "LONG_PATHS: FAIL (...)".
int i
for (i = 0; i < 4; i = i + 1)
    create neutral abcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghij
    ce abcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghij
end
create neutral first
create neutral second
int n = 0
str e
foreach e ({el ./#})
    n = n + 1
end
if ({n} == 2 && !{exists nothere/leaf})
    echo "LONG_PATHS: PASS"
else
    echo "LONG_PATHS: FAIL (" {n} " children)"
end
quit
