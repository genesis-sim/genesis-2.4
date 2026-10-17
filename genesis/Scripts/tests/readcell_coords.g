// genesis
//
// readcell of a branched cell with nonzero coordinates: every compartment
// exists, keeps its coordinates, and the cell runs under hsolve (see
// element_layout.g for what breaks it). Run from the top of the source tree
// (it reads genesis/Scripts/tests/coords.p).
//
// Prints "READCELL_COORDS: PASS" or "READCELL_COORDS: FAIL (...)".
include genesis/src/startup/schedule.g
int bad = 0
create neutral /library
create compartment /library/compartment
disable /library
readcell genesis/Scripts/tests/coords.p /cell
str names = ""
str e
foreach e ({el /cell/#[TYPE=compartment]})
    names = {names} @ " " @ {getpath {e} -tail}
end
if ({strcmp {names} " soma dend1 dend2 dend3 dend4"} != 0)
    echo "compartments: " {names}
    bad = bad + 1
end
if ({abs {{getfield /cell/dend2 x} - 100e-6}} > 1e-9 || {abs {{getfield /cell/dend4 y} - 100e-6}} > 1e-9)
    echo "coordinates: dend2 x " {getfield /cell/dend2 x} " dend4 y " {getfield /cell/dend4 y}
    bad = bad + 1
end
setfield /cell/soma inject 1e-10
create hsolve /cell/solver
setfield /cell/solver path "/cell/##[][TYPE=compartment]" chanmode 0
useclock /cell/##[] 0
useclock /cell/solver 0
call /cell/solver SETUP
setmethod /cell/solver 11
setclock 0 1e-5
reset
step 2000
call /cell/solver HGET
if (!({getfield /cell/dend4 Vm} > {getfield /cell/dend4 Em}))
    echo "dend4 did not depolarise: Vm " {getfield /cell/dend4 Vm}
    bad = bad + 1
end
if ({bad} == 0)
    echo "READCELL_COORDS: PASS"
else
    echo "READCELL_COORDS: FAIL (" {bad} " checks)"
end
quit
