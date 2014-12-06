//genesis

function add_squid_graphs(path)
str path, gpath

    create xform {path}/graphs [0,0,1000,500] -notitle
    create xlabel {path}/graphs/label [10,0,98%,25] -label "Squid Axon"

    // plot membrane potential

    gpath = (path) @ "/graphs/Vm"
    create xgraph {gpath} [10,10:label.bottom,49%,45%] -title \
         "membrane potential"
    create xbutton {gpath}"scale" [-50:Vm.right,10:label.bottom,50,25] \
	-title "scale" -script "set_scale "{gpath}
    setfield {gpath} XUnits "t (msec)" YUnits "voltage (mV)"
    setfield {gpath} xmax 20 ymin {-20 + EREST_ACT}  \
        ymax {120 + EREST_ACT}

    // plot injection currents
    gpath = (path) @ "/graphs/inj"
    create xgraph {gpath}  \
        [10,10:Vm.bottom,49%,45%] -title  \
        "injection current"
    create xbutton {gpath}"scale" [-50:inj.right,10:Vm.bottom,50,25] -title \
         "scale" -script "set_scale "{gpath}
    setfield {gpath} XUnits "t (msec)" YUnits "current (uA)"
    setfield {gpath} xmax 20 ymin -0.5 ymax 0.5

    // plot channel conductances
    gpath = (path) @ "/graphs/Gk"
    create xgraph {gpath}  \
        [10:Vm.right,10:label.bottom,49%,45%] -title  "channel conductance"
    create xbutton {gpath}"scale" [-50:Gk.right,10:label.bottom,50,25] \
	 -title "scale"  -script "set_scale "{gpath}
    setfield {gpath} XUnits "t (msec)" YUnits "conductance (mS)"
    setfield {gpath} xmax 20 ymin 0.0 ymax 0.5

    // plot channel currents
    gpath = (path) @ "/graphs/Ik"
    create xgraph {gpath}  \
        [10:inj.right,10:Gk.bottom,49%,45%] -title  "channel current"
    create xbutton {gpath}"scale" [-50:Ik.right,10:Gk.bottom,50,25] \
	-title "scale" -script "set_scale "{gpath}
    setfield {gpath} XUnits "t (msec)" YUnits "current (uA)"
    setfield {gpath} xmax 20 ymin -10 ymax 10


    setclock 1 0.1
    graphclock {path} 1
    // make an exception for the injection graph - use simulation clock
    useclock {path}"/graphs/inj" 0
    showgraphics {path}
end

function set_scale(gpath)
    str gpath
    float xmin, xmax, ymin, ymax, overlay
    xmin = {getfield {gpath} xmin}
    xmax = {getfield {gpath} xmax}
    ymin = {getfield {gpath} ymin}
    ymax = {getfield {gpath} ymax}
    overlay = {getfield {gpath} overlay}

    pushe /forms/gscale
    setfield  /forms/gscale gpath {gpath}
    setfield xmin value {xmin}
    setfield xmax value {xmax}
    setfield ymin value {ymin}
    setfield ymax value {ymax}
    setfield overlay value {overlay}
    xshow .
    pope
end
