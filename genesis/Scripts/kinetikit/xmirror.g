//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 2001 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/


/******************************************************************
      Stuff for creating mirrors, which are just pools
	  with a msg to the master pool, and a different shape flag.
	  A dummy object is also created so as to simplify the 
	  code.
******************************************************************/

function make_mirror(src, window, pool)
	str src
	str window
	str pool

	echo in make_mirror {src},  {window}, {pool}
	if ({strcmp {window} "/edit/draw/tree"} == 0)
		if ({isa kpool {pool}})
			create kpool {pool}_mirror -autoindex
			str newpool = {el ^}
			addmsg {newpool} {pool} MIRROR n
			setfield {newpool} \
				xtree_shape_req mirror \
				y {{getfield {pool} y} + 1} \
				x {getfield {pool} x} \
				n {getfield {pool} n} \
				nInit {getfield {pool} nInit} \
				vol {getfield {pool} vol}
			setfield {newpool}/notes str {getfield {pool}/notes str}

			call /edit/draw/tree RESET
			return
		end
	end
	do_inform "Warning: mirrors must be made on existing pools in the edit window" 0
end


/******************************************************************
      Stuff for initializing mirrors
*****************************************************************/

function mirror_create_func(mirror)
	str mirror

	pool_create_func {mirror}
	setfield {mirror} vol {getfield {mirror}/.. vol}
	addmsg {mirror} {mirror}/.. MIRROR n
	position {mirror} I R1 I
end

function nonxmirrorproto
	create kpool /mirror

	addobject mirror /mirror \
		-author "Upi Bhalla NCBS May 2001" \
	-description "mirror class which is basically a pool"
	setdefault mirror xtree_shape_req "mirror"
	setdefault mirror manageclass "kpool"
	setdefault mirror createfunc "mirror_create_func"
end

function xmirrorproto
	ce /control/lib/tree
	create xshape shape -autoindex \
		-drawmode DrawLines \
		-fg blue \
		-coords [-.3,.3,0][.4,-.6,0][.4,.6,0][-.4,.6,0][.3,-.3,0][-.4,.6,0][-.4,-.6,0][.4,-.6,0] \
		-text "mirror" \
		-textfont 7x13bold \
		-value "mirror" \
		-pixflags v \
		-pixflags c \
		-script "edit_pool.D"

	copy {el ^} /edit/draw/tree -autoindex
	setfield ^ \
		script "edit_pool.D"
	ce /
end

function init_xmirror
    // set up the prototype
	nonxmirrorproto
	if (DO_X)
    	xmirrorproto
	end
	// Set up the dumping fields
	simobjdump mirror notes xtree_fg_req x y z
end

/******************************************************************/
