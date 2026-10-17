// genesis
//
// One hsolve over many identical cells must compute each cell exactly as an
// hsolve over that cell alone.
//
// Builds N_CELLS identical Hodgkin-Huxley cells, each a soma plus
// (NCOMP - 1) passive dendritic compartments in a line, every soma driven by
// the same injected current, all under ONE hsolve. Next to them it builds the
// same cell once more under its OWN hsolve, as the reference. After the run it
// compares every cell's soma and far-end Vm with the reference.
//
//   nxgenesis -nosimrc -notty -batch hsolve_many_cells.g [N_CELLS] [NCOMP] [METHOD] [STEPS] [BRANCH]
//
//   N_CELLS  cells under the shared hsolve (default 8)
//   NCOMP    compartments per cell (default 1: a single soma)
//   METHOD   10 backward Euler, 11 Crank-Nicolson (default 11)
//   STEPS    time steps of 10 us (default 2000: 20 ms, several spikes)
//   BRANCH   1: compartments 1 and 2 both hang off the soma, so each cell
//            branches at its root; 0 (default): an unbranched line
//
// Prints "HSOLVE_MANY_CELLS: PASS" when every cell agrees with the reference
// within 1e-9 V, otherwise "FAIL" and the cells that do not.
//
// With NCOMP = 1 the reference hsolve holds a single compartment, so the same
// run also checks the solver's one-compartment shortcut.
//
// Run from the directory that holds genesis/src/startup/schedule.g (the
// top of the source tree), or edit the include below.

int   N_CELLS = 8
int   NCOMP   = 1
int   METHOD  = 11
int   STEPS   = 2000
int   BRANCH  = 0
if ({argc} > 0)
    N_CELLS = {argv 1}
end
if ({argc} > 1)
    NCOMP = {argv 2}
end
if ({argc} > 2)
    METHOD = {argv 3}
end
if ({argc} > 3)
    STEPS = {argv 4}
end
if ({argc} > 4)
    BRANCH = {argv 5}
end

float DT        = 10e-6
float INJECT    = 0.5e-9
float PI        = 3.141592653589793
float EREST_ACT = -0.070
float CM_DENS   = 0.01
float RM_DENS   = 0.333333
float RA_DENS   = 0.3
float ENA       = {EREST_ACT + 0.115}
float EK        = {EREST_ACT - 0.012}
float ELEAK     = {EREST_ACT + 0.010613}
float SOMA_L    = 20e-6
float SOMA_D    = 20e-6
float DEND_L    = 50e-6
float DEND_D    = 2e-6

include genesis/src/startup/schedule.g

echo "=== hsolve_many_cells: N_CELLS=" {N_CELLS} " NCOMP=" {NCOMP} " METHOD=" {METHOD} " STEPS=" {STEPS} " BRANCH=" {BRANCH} " ==="

create neutral /library
pushe /library
create tabchannel Na_chan
setfield Na_chan Ek {ENA} Ik 0 Gk 0 Xpower 3 Ypower 1 Zpower 0
setupalpha Na_chan X \
    {100000.0 * (0.025 + EREST_ACT)} -100000.0 -1.0 {-1.0 * (0.025 + EREST_ACT)} -0.01 \
    4000.0 0.0 0.0 {0.0 - EREST_ACT} 0.018
setupalpha Na_chan Y \
    70.0 0.0 0.0 {0.0 - EREST_ACT} 0.02 \
    1000.0 0.0 1.0 {-0.03 - EREST_ACT} -0.01
create tabchannel K_chan
setfield K_chan Ek {EK} Ik 0 Gk 0 Xpower 4 Ypower 0 Zpower 0
setupalpha K_chan X \
    {10000.0 * (0.010 + EREST_ACT)} -10000.0 -1.0 {-1.0 * (0.010 + EREST_ACT)} -0.01 \
    125.0 0.0 0.0 {0.0 - EREST_ACT} 0.08
pope
disable /library

float soma_area  = SOMA_L * PI * SOMA_D
float soma_xarea = PI * (SOMA_D/2) * (SOMA_D/2)
float dend_area  = DEND_L * PI * DEND_D
float dend_xarea = PI * (DEND_D/2) * (DEND_D/2)

// make_cell <path>: soma with HH channels plus NCOMP-1 passive compartments
function make_cell(path)
    str path
    int c
    str comp, prev
    create neutral {path}
    create compartment {path}/c0
    setfield {path}/c0 Em {ELEAK} initVm {EREST_ACT} inject {INJECT} \
        Rm {RM_DENS / soma_area} Cm {CM_DENS * soma_area} Ra {RA_DENS * SOMA_L / soma_xarea}
    copy /library/Na_chan {path}/c0/Na_chan
    setfield {path}/c0/Na_chan Gbar {1200.0 * soma_area}
    addmsg {path}/c0/Na_chan {path}/c0 CHANNEL Gk Ek
    addmsg {path}/c0 {path}/c0/Na_chan VOLTAGE Vm
    copy /library/K_chan {path}/c0/K_chan
    setfield {path}/c0/K_chan Gbar {360.0 * soma_area}
    addmsg {path}/c0/K_chan {path}/c0 CHANNEL Gk Ek
    addmsg {path}/c0 {path}/c0/K_chan VOLTAGE Vm
    for (c = 1; c < {NCOMP}; c = c + 1)
        comp = {path} @ "/c" @ {c}
        prev = {path} @ "/c" @ {c - 1}
        if ({BRANCH} && {c} <= 2)
            prev = {path} @ "/c0"
        end
        create compartment {comp}
        setfield {comp} Em {ELEAK} initVm {EREST_ACT} \
            Rm {RM_DENS / dend_area} Cm {CM_DENS * dend_area} Ra {RA_DENS * DEND_L / dend_xarea}
        addmsg {prev} {comp} AXIAL Vm
        addmsg {comp} {prev} RAXIAL Ra Vm
    end
end

int n
create neutral /net
for (n = 0; n < {N_CELLS}; n = n + 1)
    make_cell /net/cell{n}
end
create neutral /ref
make_cell /ref/cell

setclock 0 {DT}
useclock /net/##[] 0
useclock /ref/##[] 0

create hsolve /net/solver
setfield /net/solver path "/net/##[][TYPE=compartment]" chanmode 1 calcmode 1
call /net/solver SETUP
setmethod /net/solver {METHOD}
create hsolve /ref/solver
setfield /ref/solver path "/ref/##[][TYPE=compartment]" chanmode 1 calcmode 1
call /ref/solver SETUP
setmethod /ref/solver {METHOD}
useclock /net/solver 0
useclock /ref/solver 0

reset
step {STEPS}

// chanmode 1 keeps the elements' Vm fields current, so no HGET is needed
str far = "/c" @ {NCOMP - 1}
float ref_soma = {getfield /ref/cell/c0 Vm}
float ref_far  = {getfield /ref/cell{far} Vm}
echo "reference: soma Vm " {ref_soma} "  far Vm " {ref_far}

int bad = 0
float d1, d2
for (n = 0; n < {N_CELLS}; n = n + 1)
    d1 = {abs {{getfield /net/cell{n}/c0 Vm} - {ref_soma}}}
    d2 = {abs {{getfield /net/cell{n}{far} Vm} - {ref_far}}}
    // written so that a NaN difference fails: every comparison with NaN is false
    if (!({d1} <= 1e-9 && {d2} <= 1e-9))
        echo "cell" {n} ": soma Vm " {getfield /net/cell{n}/c0 Vm} "  far Vm " {getfield /net/cell{n}{far} Vm} "  differs"
        bad = {bad} + 1
    end
end
if ({bad} == 0)
    echo "HSOLVE_MANY_CELLS: PASS"
else
    echo "HSOLVE_MANY_CELLS: FAIL (" {bad} " of " {N_CELLS} " cells differ from the reference)"
end
quit
