// genesis

float dt = 1.0e-4
float refresh_factor = 10.0

create xform   /control [0,0,300,444]
create xbutton /control/sweep_vert [50%,0,50%,30] \
    -script "autosweep vertical /control/bar_vel"
create xbutton /control/sweep_horiz [0,0,50%,30] \
    -script "autosweep horizontal /control/bar_vel"
create xbutton /control/step  -script "step"
create xbutton /control/stop  -script "stop"
create xbutton /control/reset -script "reset"
create xbutton /control/quit  -script "xshow /quitform"
create xdialog /control/nstep -script "nstep <w>" -value 100
create xdialog /control/current_time -value 0.000
create xdialog /control/dt_sec -script "setdt <w>" -value 1.0e-4
create xdialog /control/refresh_every_n_steps \
    -script "setrefresh <w>" -value 10
create xlabel /control/"Input pattern control"
create xdialog /control/bar_vel -label "Bar Velocity" \
    -value 0.01
    // the autosweep script is in ret_input.g
create xdialog /control/bar_firing_rate -value 200.0

create xform /quitform [0,40,300,100] -title "Do you really want to quit ?"
disable /quitform
create xbutton /quitform/yes -script quit
create xbutton /quitform/no -script "xhide /quitform"

function get_current_time
    setfield /control/current_time value {getstat -time}
end

function nstep(widget)
    str widget
    echo doing {getfield {widget} value} steps
    step {getfield {widget} value}
end

function setdt(widget)
    str widget
    echo setting timesteps to {getfield {widget} value} 
    dt = {getfield {widget} value}
    setclock 0 {dt}  // sec
    setclock 1 {dt * refresh_factor}  // sec : for the refresh views
end

function setrefresh(widget)
    str widget
    echo setting refresh_factor to {getfield {widget} value} 
    refresh_factor = {getfield {widget} value}
    setclock 1 {dt * refresh_factor}  // sec : for the refresh views
end

create xdialog /control/basal_firing_rate -value 10
// create xtoggle /control/xconn_toggle 
/*
setfield ^ script "showtoggle <w> /xconn"  \
	   state 1                             \
	   onlabel  "xconn hidden"             \
	   offlabel "xconn visible"
setfield ^ script "echo Not available yet!"  \
	   state 1                             \
	   onlabel  "xconn hidden"             \
	   offlabel "xconn visible"
*/

create xtoggle /control/xout_toggle 
setfield ^ script "showtoggle  <w> /xout" \
	   state 0                             \
	   onlabel  "xout hidden"              \
	   offlabel "xout visible"

create xtoggle /control/xtree_toggle 
/*
setfield ^ script "showtoggle  <w> /tree" \
	   state 1                             \
	   onlabel  "xtree hidden"             \
	   offlabel "xtree visible"
*/
setfield ^ script "echo Not available yet!" \
	   state 1                             \
	   onlabel  "xtree hidden"             \
	   offlabel "xtree visible"


function showtoggle(widget,form)
    str widget
    str form
    echo {widget} {form}
    if ({getfield {widget} state} == 1)
      xhide {form}
    else
      xshow {form}
    end
end 

xshow /control

function do_refresh // for calling refreshes 
    get_current_time
end
