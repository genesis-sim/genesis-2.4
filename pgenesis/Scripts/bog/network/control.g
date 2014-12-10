// genesis

float bar_x,bar_y,bar_w,bar_h,bar_dx,bar_dy
float bar_vel = 0.01
float bar_firing_rate = 500
float baserate = 10  // spikes/sec
int sweeping = 0

function setup_control end
function bar end
function do_autosweep end
function autosweep end
function setup_control_display end
function get_current_time end
function nstep end
function setdt end
function set_bar_firing_rate end
function set_bar_vel end
function setrefresh end
function do_refresh end

function setup_control
	create neutral /sweeper
	useclock /sweeper 0
	addaction /sweeper PROCESS do_autosweep

	create neutral /refresher
	useclock /refresher 1
	addaction /refresher PROCESS do_refresh

	addtask Simulate /sweeper -action PROCESS
	if (display)
	    addtask Simulate /refresher -action PROCESS
	end
	resched
end

/* 
 * Bar generates a bar-shaped pattern of activity in the 
 * retina rec cells.
 *
 * rate is in spikes/sec
 */

function bar(x,y,w,h,rate)
    float x,y,w,h,rate
    float x1,y1,x2,y2

    x1 = x - w/2
    y1 = y - h/2
    x2 = x + w/2
    y2 = y + h/2

    /* 
     * Here we are using advanced wildcarding features to create a list
     * of elements to be used (by the setfield command)  based on their
     * x and y positions. 
     */

    setfield@{workers} /retina/recplane/rec[x>{x1}][y>{y1}][x<{x2}][y<{y2}]/input \
	     rate {rate} -empty_ok
end


// Sweeps a bar across the visual field; bar_x and bar_y are
// incremented every time this function is called.

function do_autosweep
    if (sweeping)
	setfield@{workers} /retina/recplane/rec[]/input rate {baserate}
	bar_x = bar_x + bar_dx;
	if (bar_x > (0.5 * REC_NX * REC_SEPX))
		bar_x = {-0.5 * REC_NX * REC_SEPX}
	end
	bar_y = bar_y + bar_dy;
	if (bar_y > (0.5 * REC_NY * REC_SEPY))
		bar_y = {-0.5 * REC_NY * REC_SEPY}
	end
	bar {bar_x} {bar_y} {bar_w} {bar_h} {bar_firing_rate}
    end
end


/* 
 * Autosweep calculates the parameters for the bar and sweep rate, then
 * causes the simulator to step the appropriate number of times, then
 * cleans up. 
 */

function autosweep(direction)
    str direction // vertical or horizontal
    int	nsteps = {sim_steps}

    if ({strcmp {direction} "vertical"} == 0)
      bar_x = {-0.5 * REC_NX * REC_SEPX}
      bar_y = 0
      bar_w = {2 * REC_SEPX}  
      bar_h = {REC_NY * REC_SEPY}
      bar_dx = {REC_SEPX * bar_vel}
      bar_dy = 0
    else // horizontal 
      bar_x  = 0
      bar_y  = {-0.5 * REC_NY * REC_SEPY}
      bar_w  = {REC_NX * REC_SEPX}
      bar_h  = {2 * REC_SEPY}
      bar_dx = 0
      bar_dy = {REC_SEPY * bar_vel}
    end

    echo sweeping {direction} bar for {nsteps} timesteps
    sweeping = 1
    step@all {nsteps}
    sweeping = 0
    echo finished {nsteps} steps
end

function setup_control_display
	xcolorscale rainbow2
	create xform /xout [220,0,400,400]
	ce /xout
	create xbutton sweep_horiz -script "autosweep horizontal"
	create xbutton sweep_vert -script "autosweep vertical"
	create xbutton step -script "step@all"
	create xbutton test -script "echo@all hello"
	create xbutton reset -script "reset@all"
	create xbutton quit -script "xshow /quitform"
	create xdialog nstep -script "nstep <w>" -value 100
	create xdialog current_time -value 0.000
	create xdialog dt_sec -script "setdt <w>" -value 1.0e-4
	create xdialog refresh_every_n_steps \
	    -script "setrefresh <w>" -value 10
	create xlabel "Input pattern control"
	create xdialog bar_vel -value 0.01 -script "set_bar_vel <w>"
	create xdialog bar_firing_rate -value 500.0 -script "set_bar_firing_rate <w>"

	create xform /quitform [0,40,300,100] -title "Do you really want to quit ?"
	disable /quitform
	create xbutton /quitform/yes -script "async quit@all"
	create xbutton /quitform/no -script "xhide /quitform"

	// Font specifications.  These aren't needed, since defaults are
	// used if nothing is specified, but they were used to make the 
	// figure for the Book of Genesis.

	setfield /##[OBJECT=xlabel]  font 9x15bold 
	setfield /##[OBJECT=xbutton] onfont 9x15bold offfont 9x15bold
	setfield /##[OBJECT=xdialog] font 9x15bold	

	xshow /xout
end

function get_current_time
    if (display)
	setfield /xout/current_time value {getstat -time}
    end
end

function nstep(widget)
    str widget
    echo doing {getfield {widget} value} steps
    step@all {getfield {widget} value}
end

function setdt(widget)
    str widget
    echo setting timesteps to {getfield {widget} value} 
    dt = {getfield {widget} value}
    setclock@all 0 {dt}  // sec
    setclock@all 1 {dt * refresh_factor}  // sec : for the refresh views
end

function set_bar_firing_rate(widget)
    str widget
    echo setting timesteps to {getfield {widget} value} 
    bar_firing_rate = {getfield {widget} value}
end

function set_bar_vel(widget)
    str widget
    echo setting timesteps to {getfield {widget} value} 
    bar_vel = {getfield {widget} value}
end

function setrefresh(widget)
    str widget
    echo setting refresh_factor to {getfield {widget} value} 
    refresh_factor = {getfield {widget} value}
    setclock@all 1 {dt * refresh_factor}  // sec : for the refresh views
end

function do_refresh // for calling refreshes 
    get_current_time
end
// $Log: control.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:39  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.1  1999/12/18 03:02:00  mhucka
// Added PGENESIS from GENESIS 2.1 CDROM distribution.
//
// Revision 1.2  1997/08/11 04:20:38  ngoddard
//  corrected termination for interactive
//
// Revision 1.1  1997/07/02 20:12:21  ghood
// Initial revision
//
// Revision 1.4  1996/09/20 18:55:21  ghood
// cleaned up for PSC release
//
// Revision 1.3  1996/08/13 21:27:46  ghood
// General debugging to get parallel example scripts working.
//
// Revision 1.2  1996/06/21 16:09:35  ghood
// General debugging
//
// Revision 1.1  1996/05/03 19:11:44  ngoddard
// Initial revision
//
