//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-1997 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

function fake_addmsg(dummy, src,dest,type,field,arg2,arg3)
	str dummy, src,dest,type,field,arg2,arg3
	echo in fake_addmsg

	if ({strcmp {type} "PLOT"} == 1)
		addmsg {src} {dest} INPUT {field}
	end
end

function alloc_fakeplot(a, b, c, d, e)
	call . TABCREATE {FAKEPLOTSIZE} 0 {FAKEPLOTSIZE}
	setfield . step_mode 3
	useclock . {PLOTCLOCK}

	return 1
end

function dummy_treefn
end

function make_batch_interface

	objsubstitute xcoredraw neutral
	objsubstitute xgraph neutral

	create neutral faketree
	addaction faketree RESET dummy_treefn
	addaction faketree ADDMSGARROW dummy_treefn
	addaction faketree undump dummy_treefn
	addobject faketree faketree

	create table fakeplot
	// addaction fakeplot ADDMSG fake_addmsg
	// addmsgdef fakeplot PLOT data name color -type 4
	addaction fakeplot CREATE alloc_fakeplot

	addfield fakeplot pixflags
	addfield fakeplot script
	addfield fakeplot fg
	addfield fakeplot ysquish
	addfield fakeplot do_slope
	addfield fakeplot wy

	addobject fakeplot fakeplot
	swapdump
	simobjdump fakeplot
	simobjdump faketree
	simobjdump neutral
	swapdump

	create neutral /graphs
	create neutral /moregraphs

	objsubstitute xtree faketree
	objsubstitute xplot fakeplot
	objsubstitute xtext neutral
	msgsubstitute fakeplot PLOT INPUT .

	addalias xtextload echo

	// This stands in for the volume control stuff.
	if (!{exists /editcontrol})
		create neutral /editcontrol
		create neutral /editcontrol/vol
    		addfield /editcontrol/vol value
    		setfield /editcontrol/vol value {DEFAULT_VOL}
		create neutral /editcontrol/newdia
    		addfield /editcontrol/newdia value
    		setfield /editcontrol/newdia value {DEFAULT_VOL}
		create neutral /editcontrol/dia
    		addfield /editcontrol/dia value
    		setfield /editcontrol/dia value {calc_dia {DEFAULT_VOL}}
		create neutral /editcontrol/conc
    		addfield /editcontrol/conc units
    		addfield /editcontrol/conc unitname
    		addfield /editcontrol/conc scalefactor
    		setfield /editcontrol/conc scalefactor 1000
	end

	if (!{exists /file})
		create neutral /file
		create neutral /file/modpath
			addfield /file/modpath value
	end

end
