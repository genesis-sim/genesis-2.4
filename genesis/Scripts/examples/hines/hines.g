//genesis

/* This is an example of the use of hsolve elements for doing the
** hines numbering scheme for solution of a compartmental model of
** a neuron */

float RA = 0.5
float RM = 10
float CM = 0.01
float EREST_ACT = -0.065

float dt = 50e-6
float graph_dt = 1e-4
int   chanmode = 0


function make_hsolve
    if ({exists /test/solve})
        delete /test/solve
    end
  create hsolve /test/solve
  setfield /test/solve path "/test/##[][TYPE=compartment]"
    setfield /test/solve chanmode {chanmode} comptmode 1
  setmethod /test/solve 11
  call /test/solve SETUP
//  showfield /test/solve *
  reset
end // make_hsolve


function change_stepsize
    dt = {getfield /cform/stepsize value}
    setclock 0 {dt}
    echo dt = {dt}
    // re-makes solver, if needed
    make_hsolve
end

function graphicstoggle
    if ({getfield /cform/graphics state} == 1)
        setclock 1 100.0
    else
        setclock 1 {graph_dt}
    end
end

function modetoggle
    if ({getfield /cform/mode state} == 1)
        chanmode = 2
    else
        chanmode = 0
    end
    make_hsolve
end

function setup
  str name

  create neutral /library
  create compartment /library/compartment

  readcell hinescell.p /test

  create xform /form [0,0,400,400] -nolabel
  create xdraw /form/draw [0,0,100%,100%] -wx 2e-3 -wy 2e-3 -transform  \
    ortho3d -bg white
  setfield /form/draw xmin -8e-4 xmax 8e-4 ymin -8e-4 ymax 8e-4

  create xcell /form/draw/cell -path "/test/##[][TYPE=compartment]"  \
    -colfield Vm -colmin -0.07 -colmax 0.03 -diarange -40
  useclock /form/draw/cell 1
  xcolorscale hot
  xshow /form

  create xform /gform [400,0,400,400] -nolabel
  create xgraph /gform/graph [0,0,100%,100%]  -bg white
  setfield ^ xmax 0.1 ymax 0.05 ymin -0.09 overlay 1
  useclock /gform/graph 1
  xshow /gform

  create xform /cform [0,420,800,80]
  create xbutton /cform/RESET [0,0,33%,30] -script reset_sim
  create xbutton /cform/RUN [0:RESET,0,33%,30] -script run_sim
  create xbutton /cform/QUIT [0:RUN,0,33%,30] -script quit
  create xdialog /cform/stepsize -title "dt" [0,0:RESET,33%,30] \
    -script "change_stepsize"  -value {dt}

  create xtoggle /cform/graphics [0:stepsize,0:RESET,33%,30]  -script graphicstoggle
    setfield /cform/graphics offlabel "Graphics enabled" \
        onlabel "Graphics disabled" state 0
  graphicstoggle

  create xtoggle /cform/mode [0:graphics,0:RESET,33%,30]  -script modetoggle
    setfield /cform/mode offlabel "Chanmode 0" onlabel "Chanmode 2" state 0
  modetoggle

  xshow /cform

  addmsg /test/soma /gform/graph PLOT Vm *soma *red
  addmsg /test/second_dend11 /gform/graph PLOT Vm *sec_dend *green
  addmsg /test/glom1 /gform/graph PLOT Vm *glom *blue
  setfield /test/soma inject 3.0e-9

  setclock 0 {dt}
  disable /library
  make_hsolve

end //setup

function reset_sim
    setfield /gform/graph overlay 0
    setfield /gform/graph/soma fg red
    setfield /gform/graph/sec_dend fg green
    setfield /gform/graph/glom fg blue
    make_hsolve
end // reset_sim

function run_sim
    reset_sim
    echo running simulation with hsolver
    reset
    silent -1
    step 0.1 -time
    silent 0
    echo running simulation without hsolver
    delete /test/solve
    setfield /gform/graph overlay 1
    reset
    setfield /gform/graph/soma fg magenta
    setfield /gform/graph/sec_dend fg turquoise
    setfield /gform/graph/glom fg cyan
    silent -1
    step 0.1 -time
    silent 0
    end //run_sim

setup
