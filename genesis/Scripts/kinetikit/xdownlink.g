//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-2000 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

// Almost all the functions are identical to those for uplinks.

/******************************************************************
      Stuff for initializing links
******************************************************************/

function downlinkproto
	create uplink /downlink
	addmsgdef /downlink INPUT

    addobject downlink /downlink \
        -author "Upi Bhalla NCBS Feb 2001" \
        -description "link for connecting modules"

    setdefault downlink editfunc "edit_uplink"
    setdefault downlink xtree_fg_req "pink"
    setdefault downlink xtree_textfg_req "red"
    setdefault downlink manageclass "group"
    setdefault downlink createfunc "uplink_create_func"
end

function xdownlinkproto
	ce /control/lib/tree
	create xshape shape -autoindex \
		-fg pink \
		-coords [0,0,0][-0.5,1,0][-0.5,0.5,0][-1,0.5,0][-1,-0.5,0][-0.5,-0.5,0][-0.5,-1,0][0,0,0][0.5,-1,0][0.5,-0.5,0][1,-0.5,0][1,0.5,0][0.5,0.5,0][0.5,1,0][0,0,0] \
		-drawmode FillPoly \
		-linewidth 5 \
		-value "downlink" \
		-pixflags v \
		-pixflags c \
		-script "edit_link.D"

	copy {el ^}  /edit/draw/tree -autoindex
	setfield ^ \
		script "edit_link.D"

    ce /
end


function init_xdownlink
    // set up the prototype
	downlinkproto
	if (DO_X)
	    xdownlinkproto
	
		// Set up call for dragging pools to downlink
		call /edit/draw/tree ADDMSGARROW "/kinetics/##[]" \
			"/kinetics/##[TYPE=downlink]" all all none -1 0 \
			"thing_to_uplink.p <S> <D>" \
			"" \
			""
	end
	simobjdump downlink nmsgr nmodule nlink notes xtree_fg_req \
		xtree_textfg_req x y z
end

/******************************************************************/
