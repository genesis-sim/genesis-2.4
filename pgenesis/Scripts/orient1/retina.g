// genesis

// assigning the retinal parameters:
float	REC_SEPX = 40e-6   // separation of cells in x direction in meters
float	REC_SEPY = 40e-6   // separation of cells in y direction in meters
float	REC_NX = retina_nx        // number of cells in x direction
float	REC_NY = retina_ny        // number of cells in y direction
float   ret_xmin = {-REC_NX * REC_SEPX / 2}
float   ret_ymin = {-REC_NY * REC_SEPY / 2}
float   ret_xmax = {REC_NX * REC_SEPX / 2}
float   ret_ymax = {REC_NY * REC_SEPY / 2}

// Setting the axonal propagation velocity
float CABLE_VEL = 1  // scale factor = 1/(cable velocity) sec/meter

float bar_x,bar_y,bar_w,bar_h,bar_dx,bar_dy
float bar_vel = 0.01
float bar_firing_rate = 500
float baserate = 10  // spikes/sec
int sweeping = 0

float dt = 1.0e-4
float refresh_factor = 10.0


function create_retina end
function connect_retina end
function setup_retinal_input end
function bar end
function do_autosweep end
function autosweep end
function display_retina end
function get_current_time end
function nstep end
function setdt end
function set_bar_vel end
function set_bar_firing_rate end
function setrefresh end
function do_refresh end

function create_retina
	// creating the retina
	create neutral /retina

	// creating a receptor cell in the library 
	// which is just a randomspike input module; rate is in spikes/sec 
	if (!{exists /library})
	  create neutral /library 
	  disable /library 
	end

	create neutral     /library/rec
	create randomspike /library/rec/input  
	setfield ^ rate 1.0 abs_refract 0.001 

	// creating a plane (map) of receptor cells on the retina
	// based on the /library/rec prototype 
	// Usage :
	// createmap prototype destination 
	// number_in_x_direction number_in_y_direction
	// -delta separation_in_x_direction separation_in_y_direction
	// -origin offset_in_x_direction offset_in_y_direction

	createmap /library/rec /retina/recplane \
	        {REC_NX}   {REC_NY} \
		-delta  {REC_SEPX} {REC_SEPY}   \
		-origin	{-REC_NX * REC_SEPX / 2} {-REC_NY * REC_SEPY / 2}
end

function connect_retina
	/*
	 * Usage :
	 * rvolumeconnect source-path destination-path
	 *		 [-relative]
	 *		 [-sourcemask {box,ellipsoid} x1 y1 z1 x2 y2 z2]
	 *		 [-sourcehole {box,ellipsoid} x1 y1 z1 x2 y2 z2]
	 *		 [-destmask   {box,ellipsoid} x1 y1 z1 x2 y2 z2]
	 *		 [-desthole   {box,ellipsoid} x1 y1 z1 x2 y2 z2]
	 *		 [-probability p]
	 */

	echo Making connections from the retina to the V1 horiz cells
	rvolumeconnect /retina/recplane/rec[]/input \
		/V1/horiz/soma[]/exc_syn@1          \
		-relative                           \
	        -sourcemask box -1 -1 0  1 1 0      \
		-destmask box {-2.4 * V1_SEPX} {-0.6 * V1_SEPY} {-5.0 * V1_SEPZ} \
	                      { 2.4 * V1_SEPX} { 0.6 * V1_SEPY} { 5.0 * V1_SEPZ}

	echo Making connections from the retina to the V1 vert cells
	rvolumeconnect /retina/recplane/rec[]/input \
		      /V1/vert/soma[]/exc_syn@2	   \
		      -relative			   \
		      -sourcemask box -1 -1 0  1 1 0 \
		      -destmask   box {-0.6 * V1_SEPX} {-2.4 * V1_SEPY} {-5.0 * V1_SEPZ} \
				      { 0.6 * V1_SEPX} { 2.4 * V1_SEPY} { 5.0 * V1_SEPZ}

	echo Setting weights and delays for ret->V1 connections.
	// assigning delays using the rvolumedelay function

	/* 
	 * Usage :
	 * rvolumedelay path 
	 * [-fixed delay]
	 * [-radial propagation_velocity] 
	 * [-uniform range]   (not used here)
	 * [-gaussian sd max] (not used here)
	 * [-exp mid max]     (not used here)
	 * [-absoluterandom]  (not used here)
	 */
	rvolumedelay /retina/recplane/rec[]/input -radial {CABLE_VEL}

	// syndelay    /V1/horiz/soma[]/exc_syn 0.0001 -add
	// syndelay    /V1/vert/soma[]/exc_syn  0.0001 -add 

	// assigning weights using the rvolumeweight function

	/* 
	 * Usage :
	 *  rvolumeweight sourcepath 
	 *          [-fixed weight]
	 *          [-decay decay_rate max_weight min_weight]
	 *          [-uniform range] 
	 *          [-gaussian sd max] 
	 *          [-exponential mid max]
	 *          [-absoluterandom]
	 */
	rvolumeweight /retina/recplane/rec[]/input -fixed 0.22
end

function setup_retinal_input
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

    setfield /retina/recplane/rec[x>{x1}][y>{y1}][x<{x2}][y<{y2}]/input \
	     rate {rate}
end


// Sweeps a bar across the visual field; bar_x and bar_y are
// incremented every time this function is called.

function do_autosweep
    if (sweeping)
	setfield /retina/recplane/rec[]/input rate {baserate}
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

function display_retina
	xcolorscale rainbow2
	create xform /xout [220,0,800,400]
	ce /xout
	create xbutton sweep_horiz -wgeom 20% \
	    -script "autosweep horizontal"
	create xbutton sweep_vert -wgeom 20% -xgeom 20% -ygeom 0 \
	    -script "autosweep vertical"
	create xbutton step  -wgeom 40% -script "step@all"
	create xbutton reset -wgeom 40% -script "reset@all"
	create xbutton quit -wgeom 40% -script "xshow /quitform"
	create xdialog nstep -wgeom 40% -script "nstep <w>" -value 100
	create xdialog current_time -wgeom 40% -value 0.000
	create xdialog dt_sec -wgeom 40% -script "setdt <w>" -value 1.0e-4
	create xdialog refresh_every_n_steps -wgeom 40% \
	    -script "setrefresh <w>" -value 10
	create xlabel "Input pattern control" -wgeom 40%
	create xdialog bar_vel -wgeom 40% -value 0.01 -script "set_bar_vel <w>"
	create xdialog bar_firing_rate -wgeom 40% -value 500.0 -script "set_bar_firing_rate <w>"

	create xform /quitform [0,40,300,100] -title "Do you really want to quit ?"
	disable /quitform
	create xbutton /quitform/yes -script quit@all
	create xbutton /quitform/no -script "xhide /quitform"

	// Font specifications.  These aren't needed, since defaults are
	// used if nothing is specified, but they were used to make the 
	// figure for the Book of Genesis.

	setfield /##[OBJECT=xlabel]  font 9x15bold 
	setfield /##[OBJECT=xbutton] onfont 9x15bold offfont 9x15bold
	setfield /##[OBJECT=xdialog] font 9x15bold	

	// Creating a view to look at the input pattern
	float ret_xscale = 0.7
	float ret_yscale = 0.8
	float ret_xoffset = REC_SEPX/2
	float ret_yoffset = REC_SEPY/2

	create xlabel /xout/"Input Pattern" [50%,2%,45%,30] 

	create xdraw /xout/draw [50%,10%,45%,300] 
	setfield     /xout/draw \
		     xmin {ret_xmin / ret_xscale} \
		     xmax {ret_xmax / ret_xscale} \
		     ymin {ret_ymin / ret_yscale} \
		     ymax {ret_ymax / ret_yscale}

	create xview /xout/draw/inputs 
	setfield     /xout/draw/inputs \
		     tx {ret_xoffset}   \
		     ty {ret_yoffset}   \
		     sizescale {REC_SEPX * 0.8}

	addmsg /retina/recplane/rec[]/input /xout/draw/inputs COORDS x y z
	addmsg /retina/recplane/rec[]/input /xout/draw/inputs VAL1 state 

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

// $Log: retina.g,v $
// Revision 1.1.1.1  2005/06/14 04:38:39  svitak
// Import from snapshot of CalTech CVS tree of June 8, 2005
//
// Revision 1.1  1999/12/18 03:02:05  mhucka
// Added PGENESIS from GENESIS 2.1 CDROM distribution.
//
// Revision 1.5  1996/09/20 18:36:56  ghood
// cleaned up for PSC release
//
// Revision 1.4  1996/08/16 17:19:48  ghood
// *** empty log message ***
//
// Revision 1.3  1996/06/21 16:43:59  ngoddard
// removed debugging mods
//
// Revision 1.2  1996/06/21  16:09:33  ghood
// General debugging
//
// Revision 1.1  1996/05/03 19:11:37  ngoddard
// Initial revision
//
