//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-1997 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/
//ksim : this file has functions for building up the
// basic infrastructure of the kkit simulations.

function kbegin
	initdump -ignoreorphans 1
	// The table object needs to have its dump fields specified
	// explicitly
	simobjdump table input output alloced step_mode stepsize x y z

	if (!{exists /kinetics})
		create neutral /kinetics
		create neutral /models
		create neutral /modelplots
		create neutral /modelstim
		disable /models
		disable /modelplots
		disable /modelstim
	end
	if (!{exists /kclip})
		create neutral /kclip
		disable /kclip
	end
	floatformat %0.5g
end

function do_simctl_update // dummy function, defined later.
end

// Converts volume in m^3 to dia in um
function calc_dia(vol)
    float vol
    return {{pow {vol * 0.75e18 / PI} {1.0/3.0}} * 2}
end

// Converts dia in um to volume in m^3
function calc_vol(dia)
    float dia
    return {dia * dia * dia * PI / 6e18} 
end

// This function resets the simulation parameters
function kparms
		setclock {SIMCLOCK} {SIMDT}
		setclock {DUMMYCLOCK} {SIMDT}
		setclock {CONTROLCLOCK} {CONTROLDT}
		setclock {PLOTCLOCK} {PLOTDT}
		if (DO_X)
			setfield /control/runtime value {MAXTIME}
			setfield /graphs/#[TYPE=xgraph] xmax {MAXTIME}
			setfield /editcontrol/vol value {DEFAULT_VOL}
			setfield /editcontrol/dia value {calc_dia {DEFAULT_VOL}}
			setfield /editcontrol/newvol value {DEFAULT_VOL}
			setfield /editcontrol/newdia value {calc_dia {DEFAULT_VOL}}
			do_simctl_update
		end
end


