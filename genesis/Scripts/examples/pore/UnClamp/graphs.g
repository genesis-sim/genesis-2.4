//genesis
//*************************************************************************
//Adam Franklin Strassberg
//March 15, 1992

//Graphing Functions and Graph Objects
//*************************************************************************


//*************************************************************************
//Graphing Functions - allow the user to change the scaling of the graph

//Sets the scale window to the scale values of the given graph
function set_scale(gpath)
    str gpath
    float xmin, xmax, ymin, ymax, overlay
    xmin = {getfield {gpath} xmin}
    xmax = {getfield {gpath} xmax}
    ymin = {getfield {gpath} ymin}
    ymax = {getfield {gpath} ymax}
    overlay = {getfield {gpath} overlay}

    ce /forms/gscale
    if (!{exists /forms/gscale gpath})
        addfield /forms/gscale gpath
    end
    setfield  /forms/gscale gpath {gpath}
    setfield xmin value {xmin}
    setfield xmax value {xmax}
    setfield ymin value {ymin}
    setfield ymax value {ymax}
    setfield overlay value {overlay}
    xshow .
end

//Sets the given field of a given graph to the value in the scale window
function gset(field, dialog)
    str path
    float val
    path = {getfield  /forms/gscale gpath}
    val = {getfield {dialog} value}
    echo Setting {path} {field} {val}
    setfield {path} {field} {val}
end
//*************************************************************************


//*************************************************************************
//Create Graph Objects

//Initialize Graphics Clock
setclock 1 0.1

//Object to hold all four graphs
create xform /graphs [50,10,1000,550] -notitle
create xlabel /graphs/label [10,0,98%,25] -label "Patch of Membrane"
str gpath

//Membrane Potential Graph
gpath = "/graphs/Vm"
create xgraph {gpath} [10,10:label.bottom,49%,45%] -title \
    "membrane potential"
create xbutton {gpath}"scale" [-50:Vm.right,10:label.bottom,50,25] \
	-title "scale" -script "set_scale "{gpath}
setfield {gpath} XUnits "t (msec)" YUnits "voltage (mV)"
setfield {gpath} xmax 100 ymin -20 ymax 120
useclock {gpath} 1

//Injection Current Graph
gpath = "/graphs/inj"
create xgraph {gpath}  [10,10:Vm.bottom,49%,45%] \
	-title  "injection current"
create xbutton {gpath}"scale" [-50:inj.right,10:Vm.bottom,50,25] -title  \
    "scale" -script "set_scale "{gpath}
setfield {gpath} XUnits "t (msec)" YUnits "current (pA)"
setfield {gpath} xmax 100 ymin -0.5 ymax 0.5
useclock {gpath} 1

    //Channel Conductance Graph
gpath = "/graphs/Gk"
create xgraph {gpath} [10:Vm.right,10:label.bottom,49%,45%]  \
    -title "channel conductance"
create xbutton {gpath}"scale" [-50:Gk.right,10:label.bottom,50,25] \
	-title "scale" -script "set_scale "{gpath}
setfield {gpath} XUnits "t (msec)" YUnits "conductance (pS)"
setfield {gpath} xmax 100 ymin 0.0 ymax 500.0
useclock {gpath} 1

//Channel Currents Graph
gpath = "/graphs/Ik"
create xgraph {gpath} [10:inj.right,10:Gk.bottom,49%,45%] -title  \
    "channel current"
create xbutton {gpath}"scale" [-50:Ik.right,10:Gk.bottom,50,25] -title  \
    "scale" -script "set_scale "{gpath}
setfield {gpath} XUnits "t (msec)" YUnits "current (pA)"
setfield {gpath} xmax 100 ymin -10000.0 ymax 10000.0
useclock {gpath} 1

xshow /graphs
//*************************************************************************


//*************************************************************************
//Create Scale Object

create xform /forms/gscale [300,100,235,235] -title "Graph Scale"
ce /forms/gscale
create xdialog xmin [10,45,200,25] -value "???" -script  \
    "gset xmin "<widget>
create xdialog xmax [10,80,200,25] -value "???" -script  \
    "gset xmax "<widget>
create xdialog ymin [10,115,200,25] -value "???" -script  \
    "gset ymin "<widget>
create xdialog ymax [10,150,200,25] -value "???" -script  \
    "gset ymax "<widget>
create xdialog overlay [10,185,200,25] -value "???" -script  \
    "gset overlay "<widget>
create xbutton DONE [10,10,50,25] -script "xhide /forms/gscale"
 if (!{exists /forms/gscale gpath})
     addfield /forms/gscale gpath
 end
 setfield  /forms/gscale gpath "???"
 //*************************************************************************
