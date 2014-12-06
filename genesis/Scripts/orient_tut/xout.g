// genesis

create xform /xout [312,0,800,447]
ce /xout

// Creating a view to look at the input pattern

float ret_xscale = 0.7
float ret_yscale = 0.8
float ret_xoffset = REC_SEPX/2
float ret_yoffset = REC_SEPX/2

create xlabel /xout/"Input Pattern" [2%,2%,45%,30] 

create xdraw /xout/draw2 [2%,10%,45%,300] 
setfield     /xout/draw2 \
	     xmin {ret_xmin / ret_xscale} \
	     xmax {ret_xmax / ret_xscale} \
	     ymin {ret_ymin / ret_yscale} \
	     ymax {ret_ymax / ret_yscale}


create xview /xout/draw2/inputs 
setfield     /xout/draw2/inputs \
	     tx {ret_xoffset}   \
	     ty {ret_yoffset}   \
	     sizescale {REC_SEPX * 0.8}
useclock /xout/draw2/inputs 1

// change this to path and field method?

addmsg /retina/recplane/rec[]/input /xout/draw2/inputs COORDS x y z
addmsg /retina/recplane/rec[]/input /xout/draw2/inputs VAL1 state 

// Creating a pair of views in the same draw to look at the V1 cells.

float V1_xscale  = 0.7
float V1_yscale  = 0.8
float V1_xoffset = 3 * V1_SEPX
float V1_yoffset = V1_SEPX/2

create xlabel /xout/"Activity Pattern" [52%,2%,45%,30]

create xdraw /xout/draw3 [52%,10%,45%,300] 
setfield     /xout/draw3  \
    xmin     -0.000559    \
    xmax      0.000457    \
    ymin     -0.00044435  \
    ymax      0.00044465  \
    zmin     -0.0807691   \
    zmax      0.08077     \
    vx       -0.211083    \
    vy       -0.779934    \
    vz        0.589192    \
    transform ortho3d     

// create labels for the xview widgets corresponding to V1 cells

create xshape /xout/draw3/horiz_label -text horiz  -tx -0.00028 -ty -0.0003
create xshape /xout/draw3/vert_label  -text vert   -tx  0.0002  -ty -0.0003

// Creating the horiz cells to the left of the vert cells.

create xview /xout/draw3/horiz 
setfield     /xout/draw3/horiz          \
             tx {-V1_xoffset}           \  
             ty { V1_yoffset}           \
	     value_min[0] -0.090        \
	     value_max[0]  0.060        \
             sizescale {V1_SEPX * 30.0} \
	     morph_val 0                \
	     path /V1/horiz/soma[]      \
	     field Vm                   \
	     script "plotviewgraph <w>"

// Creating the vert cells to the right of the horiz cells 

create xview /xout/draw3/vert
setfield     /xout/draw3/vert           \
             tx {V1_xoffset}            \
             ty {V1_yoffset}            \
	     value_min[0] -0.090        \
	     value_max[0]  0.060        \
             sizescale {V1_SEPX * 30.0} \
	     morph_val 0                \
	     path /V1/vert/soma[]       \
	     field Vm                   \
	     script "plotviewgraph <w>"

// Creating a set of toggles to choose the V1 cell compartment to be
// displayed in the Activity view.  These will all have to be altered
// to use addmsg, or else the above scheme will have to be altered to 
// use path.  

useclock /xout/draw3/vert 1
useclock /xout/draw3/horiz 1

ce /xout
create xtoggle /xout/soma    -script "set_soma    <w>" \
	[10,5:draw2,18%,25] 
setfield /xout/soma state 1
create xtoggle /xout/hhNa    -script "set_hhNa    <w>" \
	[10:soma,5:draw2,18%,25] 
create xtoggle /xout/hhK     -script "set_hhK     <w>" \
	[10:hhNa,5:draw2,18%,25] 
create xtoggle /xout/exc_syn -script "set_exc_syn <w>" \
	[10:hhK,5:draw2,18%,25] 
create xtoggle /xout/axon    -script "set_axon    <w>" \
	[10:exc_syn,5:draw2,18%,25] 


function set_path_toggles(widget)
  str widget
  ce /xout
  setfield soma     state 0 
  setfield hhNa     state 0 
  setfield hhK      state 0 
  setfield exc_syn  state 0 
  setfield axon     state 0 
  setfield {widget} state 1
end


function set_soma
        setfield /xout/draw3/horiz path /V1/horiz/soma[]
	setfield /xout/draw3/horiz field Vm
        setfield /xout/draw3/vert  path /V1/vert/soma[]
	setfield /xout/draw3/vert field Vm
	setfield /xout/field value Vm
        set_path_toggles soma
end
 
function set_hhNa
        setfield /xout/draw3/horiz path /V1/horiz/soma[]/HH_Na_channel
	setfield /xout/draw3/horiz field Gk
        setfield /xout/draw3/vert  path /V1/vert/soma[]/HH_Na_channel
	setfield /xout/draw3/vert  field Gk
	setfield /xout/field value Gk
        set_path_toggles hhNa
end
 
function set_hhK
        setfield /xout/draw3/horiz path /V1/horiz/soma[]/HH_K_channel
	setfield /xout/draw3/horiz field Gk
        setfield /xout/draw3/vert  path /V1/vert/soma[]/HH_K_channel
	setfield /xout/draw3/vert  field Gk
	setfield /xout/field value Gk
        set_path_toggles hhK
end
 
function set_exc_syn
        setfield /xout/draw3/horiz path /V1/horiz/soma[]/exc_syn
	setfield /xout/draw3/horiz field Gk
        setfield /xout/draw3/vert  path /V1/vert/soma[]/exc_syn
	setfield /xout/draw3/vert  field Gk
	setfield /xout/field value Gk
        set_path_toggles exc_syn
end

// NOTE: the "axon" object of genesis 1.4 is no more, so here
// we use the spikegen object to display the spikes, which is
// what the axon would have had anyway.

function set_axon
        setfield /xout/draw3/horiz path /V1/horiz/soma[]/spike
	setfield /xout/draw3/horiz field state
        setfield /xout/draw3/vert  path /V1/vert/soma[]/spike
	setfield /xout/draw3/vert  field state
	setfield /xout/field value state
        set_path_toggles axon
end

// Creating a dialog to set the field being displayed by the widget: 

create xdialog /xout/field -value Vm -script "set_field <w>" 

function set_field(widget)
  str widget
  setfield /xout/draw3/horiz field {getfield {widget} value}
  setfield /xout/draw3/vert  field {getfield {widget} value}
end

// Creating an autoscale toggle for the views 

//create xtoggle /xout/autoscale -script "autoscale_toggle <w>" 
//setfield /xout/autoscale offlabel "Autoscaling OFF" onlabel "Autoscaling ON"

create xtoggle /xout/autoscale -script "echo Not available yet!" 
setfield /xout/autoscale offlabel "Autoscaling OFF" onlabel "Autoscaling OFF"
//change onlabel when autoscaling is implemented

function autoscale_toggle(widget)
  str widget

  if ({getfield {widget} state} == 1)
    setfield /xout/draw3/horiz autoscale 1
    setfield /xout/draw3/vert  autoscale 1
  else
    setfield /xout/draw3/horiz autoscale 0
    setfield /xout/draw3/vert  autoscale 0
  end
end


xshow /xout

