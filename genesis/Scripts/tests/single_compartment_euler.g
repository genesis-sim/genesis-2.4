// genesis
//
// Backward Euler on a cell of one compartment gives the same voltage in every
// solver mode. The one-compartment shortcut of do_euler_hsolve subtracted the
// previous voltage, so under chanmode 2 a passive compartment charged by inject
// read -0.6 mV instead of -69 mV after 1 ms. Run from the top of the source
// tree.
//
//   nxgenesis -nosimrc -notty -batch single_compartment_euler.g
//
// Prints "SINGLE_COMPARTMENT_EULER: PASS" or "... FAIL (...)".
include genesis/src/startup/schedule.g
function cell(path, mode)
    str path
    int mode
    create compartment {path}
    setfield {path} Em -0.07 Rm 1e8 Cm 1e-10 Ra 1e6 inject 1e-10 initVm -0.07
    create hsolve {path}_solver
    setfield {path}_solver path {path} chanmode {mode}
    useclock {path} 0
    useclock {path}_solver 0
    call {path}_solver SETUP
    setmethod {path}_solver 10
end
cell /m0 0
cell /m2 2
setclock 0 1e-4
reset
step 10
call /m0_solver HGET
call /m2_solver HGET
float v0 = {getfield /m0 Vm}
float v2 = {getfield /m2 Vm}
if ({abs {{v0} - {v2}}} <= 1e-9 && {v0} < -0.065)
    echo "SINGLE_COMPARTMENT_EULER: PASS"
else
    echo "SINGLE_COMPARTMENT_EULER: FAIL (chanmode 0 " {v0} ", chanmode 2 " {v2} ")"
end
quit
