// genesis
//
// Creating an element that already exists replaces it, with a warning. With
// the duplicate check done through the element hash table, an element made at
// start-up (/output) must be in that table, or creating it again appends a
// second /output.
//
// Prints "DUPLICATE_ELEMENT: PASS" or "DUPLICATE_ELEMENT: FAIL (...)".
create neutral /output
create neutral /output
int n = 0
str e
foreach e ({el /#})
    if ({strcmp {getpath {e} -tail} "output"} == 0)
        n = n + 1
    end
end
if ({n} == 1)
    echo "DUPLICATE_ELEMENT: PASS"
else
    echo "DUPLICATE_ELEMENT: FAIL (" {n} " elements named /output)"
end
quit
