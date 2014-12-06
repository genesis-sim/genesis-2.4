// genesis

// Commands to specify the inputs to the simulation.

/*
 * A more modern GENESIS implentation would replace the script_out object with
 * an extended object based upon a neutral object.  An "addaction" command
 * would be used to define a new PROCESS action.  The function which defines
 * the action for the each_step element would be "do_autosweep".  The main
 * function which steps the simulation "autosweep" would no longer set the
 * command field of a script_out object.
 */

float bar_x,bar_y,bar_w,bar_h,bar_dx,bar_dy
float baserate = 10  // spikes/sec
float dt = 1.0e-4
float firing_rate

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
    setfield /retina/recplane/rec[]/input rate {baserate}
    bar {bar_x} {bar_y} {bar_w} {bar_h} {firing_rate}
    bar_x = bar_x + bar_dx;
    bar_y = bar_y + bar_dy;
end


// a dummy function

function do_nothing
end


/* 
 * Autosweep calculates the parameters for the bar and sweep rate, then
 * causes the simulator to step the appropriate number of times, then
 * cleans up. 
 */

function autosweep(direction, widget)
    str direction // vertical or horizontal
    str widget
    float vel = {getfield {widget} value}
    int	nsteps
	float sweepdt
	float avel = {abs {vel}}
	if (avel == 0)
		avel = 1
	end

    if ({strcmp {direction} "vertical"} == 0)
      nsteps = {REC_NX / avel}
	  if (vel > 0)
      	bar_x = {-0.5 * REC_NX * REC_SEPX}
	  else 
      	bar_x = {0.5 * (REC_NX -2) * REC_SEPX}
	  end
      bar_w = {1.5 * REC_SEPX}
      bar_y = 0
      bar_h = 2
      bar_dx = {REC_SEPX * vel / avel}
      bar_dy = 0
   	  sweepdt = nsteps * dt / REC_NX
    else // horizontal 
      nsteps = {REC_NY / avel}
	  if (vel > 0)
      	bar_y  = {-0.5 * REC_NY * REC_SEPY}
	  else
      	bar_y  = {0.5 * (REC_NY -2) * REC_SEPY}
	  end
      bar_x  = 0
      bar_w  = 2
      bar_h  = {1.5 * REC_SEPY}  
      bar_dx = 0
      bar_dy = {REC_SEPY * vel/avel}
   	  sweepdt = nsteps * dt / REC_NY
    end

    setfield /output/each_step command do_autosweep
	setclock 2 {sweepdt}
	firing_rate = {getfield /control/bar_firing_rate value}
	baserate = {getfield /control/basal_firing_rate value}
    reset
    echo sweeping {direction} bar across {REC_NX} cells in {nsteps} timesteps
    step {nsteps}
    echo finished {nsteps} steps
    setfield /output/each_step command do_nothing
    call /output/each_step RECALC 
    /*
     * Setting the command does not change the actual command stored,
     * so we force the change using the RECALC. To be fixed !! 
     */
end


// The script_out elements execute the script defined in their 'command'
// field every timestep. 

create   script_out /output/each_step 
setfield /output/each_step command do_nothing

create script_out /output/refresh_out
setfield /output/refresh_out command do_refresh
useclock /output/refresh_out 1
useclock /output/each_step 2
