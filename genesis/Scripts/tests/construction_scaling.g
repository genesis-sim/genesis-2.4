// genesis
//
// How model construction time grows with model size.
//
// Builds N_CELLS copies of one 17-compartment cell (a soma and four
// branches of four compartments, Hodgkin-Huxley channels on the soma) under
// /net with copy, puts them all under one hsolve and runs SETUP, then quits.
// It does no simulation, so the run's wall time is construction time.
//
//   nxgenesis -nosimrc -notty -batch construction_scaling.g [N_CELLS]
//
// run_construction_test.sh times it for growing N and fits the exponent:
// building a model n times larger should take about n times longer.

int N_CELLS = 1000
if ({argc} > 0)
    N_CELLS = {argv 1}
end
float PI = 3.141592653589793
float EREST_ACT = -0.070
float soma_area = 20e-6 * PI * 20e-6
float dend_area = 50e-6 * PI * 2e-6

create neutral /library
create tabchannel /library/Na_chan
setfield /library/Na_chan Ek 0.045 Xpower 3 Ypower 1
setupalpha /library/Na_chan X -4500.0 -100000.0 -1.0 0.045 -0.01 4000.0 0.0 0.0 0.070 0.018
setupalpha /library/Na_chan Y 70.0 0.0 0.0 0.070 0.02 1000.0 0.0 1.0 0.040 -0.01
create tabchannel /library/K_chan
setfield /library/K_chan Ek -0.082 Xpower 4
setupalpha /library/K_chan X -600.0 -10000.0 -1.0 0.060 -0.01 125.0 0.0 0.0 0.070 0.08
disable /library

create neutral /library/cell
create compartment /library/cell/soma
setfield /library/cell/soma Rm {0.333333 / soma_area} Cm {0.01 * soma_area} Ra 1e6 Em -0.0594 initVm -0.070
copy /library/Na_chan /library/cell/soma/Na_chan
copy /library/K_chan /library/cell/soma/K_chan
setfield /library/cell/soma/Na_chan Gbar {1200.0 * soma_area}
setfield /library/cell/soma/K_chan Gbar {360.0 * soma_area}
addmsg /library/cell/soma/Na_chan /library/cell/soma CHANNEL Gk Ek
addmsg /library/cell/soma /library/cell/soma/Na_chan VOLTAGE Vm
addmsg /library/cell/soma/K_chan /library/cell/soma CHANNEL Gk Ek
addmsg /library/cell/soma /library/cell/soma/K_chan VOLTAGE Vm
int b, c
str comp, prev
for (b = 0; b < 4; b = b + 1)
    prev = "/library/cell/soma"
    for (c = 0; c < 4; c = c + 1)
        comp = "/library/cell/b" @ {b} @ "c" @ {c}
        create compartment {comp}
        setfield {comp} Rm {0.333333 / dend_area} Cm {0.01 * dend_area} Ra 1e7 Em -0.0594 initVm -0.070
        addmsg {prev} {comp} AXIAL Vm
        addmsg {comp} {prev} RAXIAL Ra Vm
        prev = {comp}
    end
end

create neutral /net
int n
for (n = 0; n < {N_CELLS}; n = n + 1)
    copy /library/cell /net/cell{n}
end
create hsolve /net/solver
setfield /net/solver path "/net/##[][TYPE=compartment]" chanmode 1
call /net/solver SETUP
echo "CONSTRUCTION_DONE cells " {N_CELLS} " compartments " {getfield /net/solver ncompts}
quit
