//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 2001 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

/******************************************************************
      Stuff for connecting protos
******************************************************************/
// note that libs cannot be dragged onto protos yet.

function find_parent_module(thing)
	str thing

	str elm = {el {thing}/..}

	while ({isa uplink {elm}} == 0)
		elm = {el {elm}/..}
	end
	elm = {el {elm}/..}
	return {elm}
end

function find_src_of_msg(dest, type)
	str dest
	str type
	int n = {getmsg {dest} -in -count}
	int i
	str t

	for (i = 0; i < n ; i = i + 1)
		t = {getmsg {dest} -in -type {i}}
		if ({strcmp {t} {type}} == 0)
			return {getmsg {dest} -in -source {i}}
		end
	end
	return "/kinetics"
end

function unproto(thing)
	str thing

	str nm = {getpath {el {thing} } -tail}
	str mod = {find_parent_module {thing}}
	str newicon

	if ({exists {mod}/{nm}})
		do_warning 0 "Warning: cannot unproto "{thing}": name conflict"
		return(0)
	end
	int m = {getmsg {thing}/.. -in -find {thing} MSGR}
	if (m >= 0)
		deletemsg {thing}/.. {m} -in
	end

	if ({isa enz {thing}})
		// find its parent pool
		str pa = {find_src_of_msg {thing} "ENZYME"}
		if (!{isa pool {pa}})
			do_warning "Could not find parent of enzyme "{thing} 0
		end
		// Check if it is connected up to the auto_substrate etc.
		deletemsg {thing} 0 -in -find {thing}/../auto_substrate SUBSTRATE
		deletemsg {thing}/../auto_substrate 0 -in -find {thing} REAC
		deletemsg {thing}/../auto_product 0 -in -find {thing} MM_PRD
		str otherenz = {find_src_of_msg {thing}/../auto_product MM_PRD}
		if ({strcmp {otherenz} "/kinetics"} == 0) // nothing else.
			delete {thing}/../auto_substrate
			delete {thing}/../auto_product
		end

		setfield {thing} xtree_textfg_req {getfield {pa} xtree_fg_req}
		move {thing} {pa}/{nm}
	else
		setfield {thing} xtree_textfg_req {getfield {mod} xtree_fg_req}
		move {thing} {mod}/{nm}
	end
	call /edit/draw/tree RESET
	return(1)
end

// Assesses the kinds of things attached to the proto, decides
// if the proto is for msgrs, enzs, or complex interactions.
function count_proto_elms(proto)
	str proto

	int npool = 0
	int n = 0

	str name

	foreach name ({el {proto}/##[]})
		if ({isa pool {name}})
			npool = npool + 1
		end
		n = n + 1
	end

	if (n == 0)
		return 0
	end

	setfield {proto} nelm {n}
	setfield {proto} npool {npool}

	if (npool != n) // Count modmsgsrs
		setfield {proto} nmodmsgr {getmsg {proto} -incoming -count}
	else
		setfield {proto} nmodmsgr 0
	end
end

// This function provides a magic substrate and product if a bare enzyme
// (without sub or prd) is dragged on. If additional bare enzymes are 
// dragged on, the same default substrate and prd are put on.
function enz_to_proto(enz, proto)
	str enz, proto

	count_proto_elms {proto}

	if ({getfield {proto} npool} > 0)
		if (({getfield {proto} npool} != 2) || !{exists {proto}/auto_substrate})
			return
		end
	end

	if (({getmsg {enz} -incoming -count} < 2) || ({getmsg {enz} -outgoing -count} < 3))
		if (!{exists {proto}/auto_substrate})
			float x = {getfield {enz} x}
			float y = {getfield {enz} y}
			create kpool {proto}/auto_substrate
			pool_create_func {proto}/auto_substrate
			addmsg {proto}/auto_substrate {proto} MSGR
			setfield {proto}/auto_substrate x {x - 2} y {y + 1} \
				xtree_shape_req mirror
			create kpool {proto}/auto_product
			pool_create_func {proto}/auto_product
			addmsg {proto}/auto_product {proto} MSGR
			setfield {proto}/auto_product x {x + 2} y {y + 1} \
				xtree_shape_req mirror
		end
		addmsg {enz} {proto}/auto_substrate REAC sA B
		addmsg {proto}/auto_substrate {enz} SUBSTRATE n
		addmsg {enz} {proto}/auto_product MM_PRD pA
	end

end

// This is called by the proto when something is dropped onto the
// parent link. Note that the link checks that no links are set up yet.
function thing_to_proto(thing, proto)
	str thing, proto

	str srcpa, newicon

	// Check that the thing can be put onto a proto
	if (({isa enz {thing}} == 0) && ({isa mirror {thing}} == 0) && ({strcmp {getfield {thing} "manageclass"} "group"} != 0))
		do_warning 0 "Warning: Element '"{thing}"' cannot be moved onto a proto"
		return 0
	end

	// Check that the src is not already present on proto
	if ({strcmp {proto} {el {thing}/.. }} == 0)
		unproto {thing}
		return 1
	end

	// Check that the src is not already present on another proto
	if ({isa proto {el {thing}/.. }})
		do_warning 0 "Warning: Element '"{thing}"' is already a child of a proto."
		return 0
	end

	// Check that the src isn't named the same as an existing elm
	if ({exists {proto}/{getpath {thing} -tail }})
		do_warning 0 "cannot move "{thing}" onto existing element of same name"
		return 0
	end

	echo move {thing} {proto}
	move {thing} {proto}
	str newthing = {proto} @ "/" @ {getpath {thing} -tail}
	echo newthing = {newthing}

	if ({isa enz {newthing}})
		enz_to_proto {newthing} {proto}
	end

	// Set the new textcolor of the thing
	setfield {newthing} xtree_textfg_req {getfield {proto} xtree_fg_req }

	// update the display
	call /edit/draw/tree RESET

	// we need to update the edit info here 
end

// Compares the compatibility of up and dn protos
function check_compatibility(up, dn)
	str up, dn

	count_proto_elms {up}
	count_proto_elms {dn}

	int nup = {getfield {up} nelm}
	int npoolup = {getfield {up} npool}
	int nmsgrup = {getfield {up} nmodmsgr}
	int ndn = {getfield {dn} nelm}
	int npooldn = {getfield {dn} npool}
	int nmsgrdn = {getfield {dn} nmodmsgr}

	// echo nup={nup}, npoolup={npoolup}, nmsgrup={nmsgrup}
	// echo ndn={ndn}, npooldn={npooldn}, nmsgrdn={nmsgrdn}
	
	if ((nup == npoolup) && (ndn == npooldn) && (nup == ndn))
		// msgr to msgr
		return 1
	end

	if ((nup == npoolup) && (nup == nmsgrdn)) 
		// msgr to modmsgr
		return 1
	end

	if ((ndn == npooldn) && (nmsgrup == ndn))
		// modmsgr to msgrs
		return 1
	end
	
	return 0
end

// Duplicates the orig proto onto the linkinfo passed in.
// Sets up mirror msgs depending on is_master.
// 1 means dup is a mirr. 0 means no mirr. -1 means dup is master.
// Returns the duplicate.
function do_dup(li, is_master)
	str li
	int is_master
	str orig = {el {li}/../proto}

	str dup
	copy {orig} {li} -autoindex
	dup = {el ^}

	str pool
	str name
	if (is_master == 1)
		foreach pool ({el {orig}/#[][ISA=kpool]})
			name = {getpath {pool} -tail}
			addmsg {dup}/{name} {pool} MIRROR n
			// setfield {dup}/{name} xtree_shape_req mirror
		end
	end
	if (is_master == -1)
		foreach pool ({el {orig}/#[][TYPE=kpool]})
			name = {getpath {pool} -tail}
			addmsg {pool} {dup}/{name} MIRROR n
			// setfield {pool} xtree_shape_req mirror
		end
	end
	if (is_master == 0)
		clonemsgs {orig}/##[] {dup}/##[] {li}/../../#[],{li}/../../#[][ISA!=uplink]/##[],{li}/../../#[][ISA=uplink]/proto/##[]
	end
	return {dup}
end

function make_elist(path)
	str path

	str ret = ""
	str name
	foreach name ({el {path}})
		ret = {ret} @ " " @ {name}
	end
	return {ret}
end

function make_modmsgr_elist(proto)
	str proto
	int n = {getmsg {proto} -incoming -count}
	int i
	str ret = ""

	for (i = 0 ; i < n; i = i + 1)
		ret = {ret} @ " " @ {getmsg {proto} -incoming -source {i}}
	end
	return {ret}
end

// Duplicates protos, handles assorted interesting cases.
// Takes the target uplinkinfo and downlinkinfo as arguments.
function dup_protos(upli, dnli)
	str upli, dnli

	str up = {el {upli}/../proto}
	str dn = {el {dnli}/../proto}

	int nup = {getfield {up} nelm}
	int npoolup = {getfield {up} npool}
	int nmsgrup = {getfield {up} nmodmsgr}
	int ndn = {getfield {dn} nelm}
	int npooldn = {getfield {dn} npool}
	int nmsgrdn = {getfield {dn} nmodmsgr}

	str newup
	str newdn
	str uplist
	str dnlist
	
	if ((nup == npoolup) && (ndn == npooldn) && (nup == ndn))
		// msgr to msgr
		newup = {do_dup {upli} 1}
		newdn = {do_dup {dnli} -1}
		uplist = {make_elist {newup}/#[]}
		dnlist = {make_elist {newdn}/#[]}
		// echo msgr to msgr
	end

	if ((nup == npoolup) && (nup == nmsgrdn))
		// msgr to modmsgr
		newup = {do_dup {upli} 1}
		newdn = {do_dup {dnli} 0}
		uplist = {make_elist {newup}/#[]}
		dnlist = {make_modmsgr_elist {newdn}}
		// echo msgr to modmsgr
	end

	if ((ndn == npooldn) && (nmsgrup == ndn))
		// modmsgr to msgrs. Note that msgrs are always masters.
		newup = {do_dup {upli} 0}
		newdn = {do_dup {dnli} 1}
		uplist = {make_elist {newdn}/#[]}
		dnlist = {make_modmsgr_elist {newup}}
		// echo modmsgr to msgr
	end
	// echo uplist = {uplist}
	// echo dnlist = {dnlist}

	// Make MIRROR msgs to slave dnlist to uplist.
	int i
	for (i = 0; i < {getarg {arglist {uplist}} -count}; i = i + 1)
		addmsg {getarg {arglist {dnlist}} -arg {i + 1}} \
			{getarg {arglist {uplist}} -arg {i + 1}} \
			MIRROR n
			// setfield {getarg {arglist {dnlist}} -arg {i + 1}} xtree_shape_req mirror
	end
end


/******************************************************************
      Stuff for initializing protos
******************************************************************/

// Ruthlessly points to its parent to handle all editing operations.
function edit_proto(proto)
	str proto

	edit_uplink {proto}/..
end

function kprotoproto
	create neutral /kproto
	create text /kproto/notes
    addfield /kproto editfunc -description "func for proto edit"
    addfield /kproto xtree_fg_req -description "color for proto icon"
    addfield /kproto xtree_textfg_req -description "text color for proto icon"
    addfield /kproto plotfield -description "field to plot"
    addfield /kproto manageclass -description "Class that can manage it"
	addfield /kproto nelm -description "Number of elements protoed"
	addfield /kproto npool -description "Number of pools protoed"
	addfield /kproto nmodmsgr -description "Number of modmsgrs in proto"
	addmsgdef /kproto MSGR

	addobject proto /kproto \
        -author "Upi Bhalla NCBS Apr 2001" \
        -description "proto plus a notes field"
    setdefault proto editfunc "edit_proto"
    setdefault proto xtree_fg_req "yellow"
    setdefault proto xtree_textfg_req "black"
    setdefault proto plotfield "x"
    setdefault proto manageclass "proto"
    setdefault proto nelm 0
    setdefault proto npool 0
    setdefault proto nmodmsgr 0
    ce /
end


function init_xproto
    // set up the prototype proto
    kprotoproto
	simobjdump proto x y z
end

/******************************************************************/
