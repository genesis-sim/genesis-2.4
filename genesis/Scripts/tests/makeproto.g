// genesis
//
// readcell's *makeproto moves the cell's compartments under its soma and puts
// the soma in the cell's place. Both child lists must stay appendable after
// that, which needs their tail pointers updated. Run from the top of the
// source tree.
//
// Prints "MAKEPROTO: PASS" or "MAKEPROTO: FAIL (...)".
int bad = 0
create neutral /library
create compartment /library/compartment
readcell genesis/Scripts/tests/makeproto.p /library/soma
create neutral /library/after
create compartment /library/soma/newchild
str names = ""
str e
foreach e ({el /library/#})
    names = {names} @ " " @ {getpath {e} -tail}
end
if ({strcmp {names} " compartment soma after"} != 0)
    echo "children of /library: " {names}
    bad = bad + 1
end
names = ""
foreach e ({el /library/soma/#})
    names = {names} @ " " @ {getpath {e} -tail}
end
if ({strcmp {names} " dend1 newchild"} != 0)
    echo "children of /library/soma: " {names}
    bad = bad + 1
end
if ({bad} == 0)
    echo "MAKEPROTO: PASS"
else
    echo "MAKEPROTO: FAIL (" {bad} " checks)"
end
quit
