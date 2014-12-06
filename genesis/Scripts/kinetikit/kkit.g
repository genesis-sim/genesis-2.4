//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-1997 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/
/* This is the kinetic simulation builder */

float VERSION = 0 // reset the version field to detect pre-kkit8 models

if (!{exists /kinetics})
	// Older versions of genesis don't have resetfastmsg
	create pool /rtest
	disable /rtest
	if ({exists /rtest oldmsgmode})
		addalias reset resetfastmsg
	end
	delete /rtest
	include PARMS.g
	if ({strcmp {argv 1} "nox"} == 0)
		DO_X = 0
	end
	// Used for the transition from pre-version 8 models
	if ({strcmp {argv 1} "fixcoords"} == 0)
		FIXCOORDS = 1
	end
	include ksim.g
	/* load in the user preferences or the automatically saved ones */
	if (DO_X)
		include xaboutkkit.g
		make_xabout
		xshow /about
		xflushevents
		include xinterface.g
		include xaxis.g
		include xgraphs.g
		// Move this later as it depends on stuff set up by the modules.
		// include xsave.g
	else
		include batch_interface.g
	end
		
	/* These files must be organized as functions only, which handle
	** the DO_X option correctly */
// Dummy declaration for the module_checksum file.
function isachild
end
// dummy declaration for proto editing which referes to uplinks
function edit_uplink
end

include module_checksum.g
include xgroup.g
include xpool.g
include xreac.g
include xenz.g
include xstim.g
include xtab.g
include xchan.g
include tabulate_model.g
include model_eqns.g
include xtransport.g
include xproto.g
/*
include xlinkinfo.g
include xuplink.g
include xdownlink.g
include xmirror.g
*/
include xfinddt.g
include xgeometry.g
// include make_bulk_exchange.g
if (DO_X)
	include xsave.g
	include model_to_db.g
	make_xdump_db
end
	
	function make_sim
		str name
	
		ce /
	
		setclock {SIMCLOCK} {SIMDT}
		setclock {DUMMYCLOCK} {SIMDT} // for BC
		setclock {CONTROLCLOCK} {CONTROLDT}
		setclock {PLOTCLOCK} {PLOTDT}
		// setclock {TABLECLOCK} {TABLEDT} Not used any more

		/* calling the assorted builder functions */
		kbegin
		if (DO_X)
			xbegin
			/* Graph specific creation funcs */
			make_xgraphs
		else
			make_batch_interface
		end
		
		// initializing the interface components
		init_xgroup
		init_xgeometry // This needs to precede pools. In MOOSE, the
						// volume stuff is now stored in geom 
						// equivalents so pools need geoms created first
		init_xpool
		init_xreac
		init_xenz
		init_xstim
		init_xtab
		init_xchan
		init_xtransport
		init_xproto
		/*
		init_xlinkinfo
		init_xuplink
		init_xdownlink
		init_xmirror
		*/
	
		if (DO_X)
			/* parameter search stuff */
			/*
			if (DO_PSEARCH)
				pbegin
			end
			*/

			xend
		end
		// swap out the current simulation context for dumps so
		// that loading in a model will not overwrite it.
		swapdump
		reset
	end


make_sim

maxerrors 100
end

if (DO_X && ((VERSION == 0) || (FIXCOORDS == 1)))
		// fix_pre_kkit8_coords
		FIXCOORDS = 1
		setfield /edit/draw/tree treemode custom
		do_inform "Finished loading pre-kkit 8 model"
		setfield /inform/ok script "convert_prekkit8"
end

echo version = {VERSION}, fixcoords= {FIXCOORDS}
