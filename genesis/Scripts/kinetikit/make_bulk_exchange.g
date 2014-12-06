//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-2001 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

int PLOT_XDIVS = 6000

function make_xdiffusion
	echo in make_xdiffusion
end

function check_msg_type(elm, mtype, direction)
	str elm
	str mtype
	str direction

	int i
	int nmsg
	nmsg = {getmsg {elm} -{direction} -count}
	for (i = 0; i < nmsg; i = i + 1)
		if ({strcmp {getmsg {elm} -{direction} -type {i}} {mtype}} == 0)
			return 1
		end
	end
	return 0
end

// Creates a set of reactions to represent diffusive exchange of
// molecules with a bulk pool. Does so only if the molecule is not
// buffered or sumtotalled

function make_bulk_exchange(method, bulkvol, smallvol, xa, len, D)
	int method	// Method 4 is Gillespie 1, Method 3 is GB. 
	float bulkvol
	float smallvol
	float xa	// Cross sectional area of diffusion channel
	float len	// length of diffusional channel
	float D		// Diffusion constant

	float k = D * xa / len
	float kf = k / smallvol
	float kb = k / bulkvol
	int m = 0

	str name
	str bulkname
	str bname
	float orig_vol = {getfield /editcontrol/vol value}

	if (method == 3)
		m = 3
	end
	if (method == 4)
		m = 1
	end

	if ({exists /solve})
		delete /solve
		reset
	end

	if (!{exists /bulk})
		copy /kinetics /bulk


		foreach name ({el /kinetics/##[TYPE=kpool]})
			if (({getfield {name} slave_enable} == 0) && ({check_msg_type {name} "SUMTOTAL" "in"} == 0))
				bulkname = "/bulk/" @ {substring {name} 9}

				create kreac {name}/diff
				setfield {name}/diff \
					kf {kf} \
					kb {kb} \
					x {{getfield {name} x} + 1} \
					y {{getfield {name} y} + 3}

				bname = "b_" @ {getpath {name} -tail}
				create kpool {name}/{bname}
				setfield {name}/{bname} \
					slave_enable 2 \
					vol {getfield {name} vol} \
					x {{getfield {name} x} + 2} \
					y {{getfield {name} y} + 2}
	
				addmsg {name}/diff {name} REAC A B
				addmsg {name} {name}/diff SUBSTRATE n
				addmsg {name}/diff {name}/{bname} REAC B A
				addmsg {name}/{bname} {name}/diff PRODUCT n
		
				addmsg {bulkname} {name}/{bname} SUMTOTAL n nInit
			end
		end
	end


	rescale_vol {smallvol}

	// Now we hack things so that the bulk stuff is also scaled in vol
	move /kinetics /foobar
	move /bulk /kinetics
	setfield /editcontrol/vol value {orig_vol}
	rescale_vol {bulkvol}
	setfield /editcontrol/vol value {smallvol}

	move /kinetics /bulk
	move /foobar /kinetics

	reset
	if (m > 0)
		create gsolve /kinetics/solve
		setfield /kinetics/solve method {m}
		ce /kinetics
		reset
		resched
		ce /
	end
	call /edit/draw/tree RESET
end

function add_bulk_plots
	str name
	int i
	int nmsg
	str bulkname
	str plotname
	if (DO_X)
		foreach name ({el /kinetics/##[TYPE=kpool]})
			if ({check_msg_type {name} "PLOT" "out"} == 1)
				bulkname = "/bulk" @ {substring {name} 9}
				// echo addmsg {bulkname} /moregraphs/conc3 PLOT Co
					// *{getpath {bulkname} -tail}.Co *blue
				addmsg {bulkname} /moregraphs/conc3 PLOT Co \
					*{getpath {bulkname} -tail}.Co *blue
			end
		end
	else
		foreach name ({el /kinetics/##[TYPE=kpool]})
			if ({check_msg_type {name} "INPUT" "out"} == 1)
				bulkname = "/bulk" @ {substring {name} 9}
				plotname = "/moregraphs/conc3/" @ {getpath {bulkname} -tail} @ ".bulk.Co"
				create table {plotname}
				setfield  {plotname} step_mode 3 stepsize 0
				call {plotname} TABCREATE {PLOT_XDIVS} 0 1
				addmsg {bulkname} {plotname} INPUT Co
				useclock {plotname} 3
			end
		end
	end
end


function make_buff_exchange(bulkvol, smallvol, xa, len, D)
	float bulkvol
	float smallvol
	float xa	// Cross sectional area of diffusion channel
	float len	// length of diffusional channel
	float D		// Diffusion constant

	float k = D * xa / len
	float kf = k / smallvol
	float kb = k / bulkvol
	int m = 0

	str name
	str bulkname
	str bname
	float molbulkvol = 6e20 * bulkvol
	float molsmallvol = 6e20 * smallvol
	float orig_co

	foreach name ({el /kinetics/##[TYPE=kpool]})
		bname = {getpath {name} -head} @ "b_" @ {getpath {name} -tail}
		if ({exists {bname}})
			if (({getfield {name} slave_enable} == 0) && ({check_msg_type {name} "SUMTOTAL" "in"} == 0))
				echo {bname}
				create kreac {name}/diff
				setfield {name}/diff \
					kf {kf} \
					kb {kb} \
					x {{getfield {name} x} + 1} \
					y {{getfield {name} y} + 3}

				orig_co = {getfield {bname} CoInit}
				setfield {bname} \
					vol {molbulkvol} \
					x {{getfield {name} x} + 2} \
					y {{getfield {name} y} + 2}
				setfield {bname} CoInit {orig_co}
	
				orig_co = {getfield {name} CoInit}
				setfield {name} vol {molsmallvol}
				setfield {name} CoInit {orig_co}
				addmsg {name}/diff {name} REAC A B
				addmsg {name} {name}/diff SUBSTRATE n
				addmsg {name}/diff {bname} REAC B A
				addmsg {bname} {name}/diff PRODUCT n
			end
		end
	end
	// Now we hack things so that the bulk stuff is also scaled in vol
	setfield /editcontrol/vol value {smallvol}
	call /edit/draw/tree RESET
end


// Should run after the system has settled determinstically.
function make_bulk_buffer(bulkvol, xa, len, D)
	float bulkvol
	float xa	// Cross sectional area of diffusion channel
	float len	// length of diffusional channel
	float D		// Diffusion constant

	float smallvol = {getfield /editcontrol/vol value}
	float molbulkvol = 6e20 * bulkvol
	float orig_co

	float k = D * xa / len
	float kf = k / smallvol
	float kb = k / bulkvol
	int m = 0

	str name
	str bulkname
	str bname
	float orig_vol = {getfield /editcontrol/vol value}

		foreach name ({el /kinetics/##[TYPE=kpool]})
			if (({getfield {name} slave_enable} == 0) && ({check_msg_type {name} "SUMTOTAL" "in"} == 0))
				create kreac {name}/diff
				setfield {name}/diff \
					kf {kf} \
					kb {kb} \
					x {{getfield {name} x} + 1} \
					y {{getfield {name} y} + 3}

				bname = "b_" @ {getpath {name} -tail}
				create kpool {name}/{bname}
				orig_co = {getfield {name} Co}
				setfield {name}/{bname} \
					slave_enable 4 \
					vol {molbulkvol} \
					x {{getfield {name} x} + 2} \
					y {{getfield {name} y} + 2}
				setfield {name}/{bname} CoInit {orig_co}
	
				addmsg {name}/diff {name} REAC A B
				addmsg {name} {name}/diff SUBSTRATE n
				addmsg {name}/diff {name}/{bname} REAC B A
				addmsg {name}/{bname} {name}/diff PRODUCT n
			end
		end

	reset
	call /edit/draw/tree RESET
end

function reconnect_ca_stim
	deletemsg /kinetics/Ca_tab 0 -out

	copy /kinetics/Ca /kinetics/Ca_input
	position /kinetics/Ca_input R2 R2 I
	setfield /kinetics/Ca slave_enable 0
	addmsg /kinetics/Ca_tab /kinetics/Ca_input SUMTOTAL output output

	create kreac /kinetics/Ca_stoch
	setfield /kinetics/Ca_stoch kf 100 kb 100 // Nice fast rates
		// to approximate Ca buffers
	position /kinetics/Ca_stoch \
		{{getfield /kinetics/Ca x} + 1} \
		{{getfield /kinetics/Ca y} + 3} I
		
	addmsg /kinetics/Ca_stoch /kinetics/Ca_input REAC A B
	addmsg /kinetics/Ca_input /kinetics/Ca_stoch SUBSTRATE n
	addmsg /kinetics/Ca_stoch /kinetics/Ca REAC B A
	addmsg /kinetics/Ca /kinetics/Ca_stoch PRODUCT n
end

// This function does the whole process of setting up a model
// with diffusion. It assumes that there is a sumtotalled Ca pool
// controlled by Ca_tab, that should be fixed up with a stoch input
// instead, to approximate buffer stochasticity.
function make_diffuse(smallvol, bulkvol, xa, len, D)
	float smallvol
	float bulkvol
	float xa	// Cross sectional area of diffusion channel
	float len	// length of diffusional channel
	float D		// Diffusion constant

	float volscale = smallvol / {getfield /editcontrol/vol value}
	rescale_vol {smallvol}
	setfield /kinetics/Ca_tab table =*={volscale}
	reset
	setfield /control/runtime value 2800
	do_run
	make_bulk_buffer {bulkvol} {xa} {len} {D}

	reconnect_ca_stim
	reset
end

// Duplicates the entire reaction scheme, sets up 1-d diffusion
// between corresponding molecules.
// Has some extra stuff for doing calcium stimulus
// Uses diffusion to connect two compartments
function diff_connect(src, dest, kf, kb)
	str src
	str dest
	float kf
	float kb

	str name
	str destname
	int srclen = {strlen {src}}

	foreach name ({el {src}/##[TYPE=kpool]})
		if (({getfield {name} slave_enable} == 0) && ({check_msg_type {name} "SUMTOTAL" "in"} == 0))
			destname = {dest} @ {substring {name} {srclen}}

			create kreac {name}/diff
			setfield {name}/diff \
				kf {kf} \
				kb {kb} \
				x {{getfield {name} x} + 1} \
				y {{getfield {name} y} + 3}

				addmsg {name}/diff {name} REAC A B
				addmsg {name} {name}/diff SUBSTRATE n
				addmsg {name}/diff {destname} REAC B A
				addmsg {destname} {name}/diff PRODUCT n
		end
	end
end

// Makes a 1-d chain of identical compartments, diffusively
// coupled according to the cross-section area xa, the segment
// length and Diffusion constant.
// Requires that the volume is correctly set in /editcontrol/vol
function make_1d_diff_chain(xa, segment_len, n_segment, D)
	float xa
	float segment_len
	int n_segment
	float D

	float orig_vol = {getfield /editcontrol/vol value}
	float k = D * xa / segment_len
	float kf = k / orig_vol
	float kb = k / orig_vol
	int i

	create group /chain
	createmap /kinetics /chain	{n_segment} 1 \
		-delta 4 0

	if (n_segment > 0)
	// Nasty little hack needed because GENESIS removes indices
	// from path names if index is zero
		diff_connect /chain/kinetics /chain/kinetics[1] {kf} {kb}
	end
	for (i = 2; i < n_segment; i = i + 1)
		diff_connect /chain/kinetics[{i - 1}] /chain/kinetics[{i}] {kf} {kb}
	end

	diff_connect kinetics /chain/kinetics[0] {kf} {kb}
	position /chain R4 I I
	move /chain /kinetics
	reset
	if (DO_X)
		call /edit/draw/tree RESET
	end
end

// General function for converting a point model to a pair of
// diffusively connected compartments
function gen_make_1d_diff_pair(bulkvol, smallvol, xa, len, D)
	float bulkvol
	float smallvol
	float xa	// Cross sectional area of diffusion channel
	float len	// length of diffusional channel
	float D		// Diffusion constant

	float k = D * xa / len
	float kf = k / smallvol
	float kb = k / bulkvol
	float orig_vol = {getfield /editcontrol/vol value}

	copy /kinetics /bulk
	diff_connect /kinetics /bulk {kf} {kb}

	rescale_vol {smallvol}

	// Now we hack things so that the bulk stuff is also scaled in vol
	move /kinetics /foobar
	move /bulk /kinetics
	setfield /editcontrol/vol value {orig_vol}
	rescale_vol {bulkvol}
	setfield /editcontrol/vol value {smallvol}

	move /kinetics /bulk
	move /foobar /kinetics
	move /bulk /kinetics

	reset
	if (DO_X)
		call /edit/draw/tree RESET
	end
end

// Specialized to deal with the nonscaf_syn3c.g model.
// Do not use for general calculations.
function make_1d_diff_pair(bulkvol, smallvol, xa, len, D)
	float bulkvol
	float smallvol
	float xa	// Cross sectional area of diffusion channel
	float len	// length of diffusional channel
	float D		// Diffusion constant

	float k = D * xa / len
	float kf = k / smallvol
	float kb = k / bulkvol
	float orig_vol = {getfield /editcontrol/vol value}

	reconnect_ca_stim
	copy /kinetics /bulk
	delete /bulk/Ca_input
	delete /bulk/Ca_stoch
	delete /bulk/Ca_tab
	diff_connect /kinetics /bulk {kf} {kb}

	rescale_vol {smallvol}
	setfield /kinetics/Ca_tab table =*={smallvol / orig_vol}

	// Now we hack things so that the bulk stuff is also scaled in vol
	move /kinetics /foobar
	move /bulk /kinetics
	setfield /editcontrol/vol value {orig_vol}
	rescale_vol {bulkvol}
	setfield /editcontrol/vol value {smallvol}

	move /kinetics /bulk
	move /foobar /kinetics

	reset
	if (DO_X)
		call /edit/draw/tree RESET
	end
end

function clear_non_diffusing_molecules
	str name
	foreach name (PKC/PKC-DAG-AA* PKC/PKC-Ca-AA* PKC/PKC-Ca-memb* PKC/PKC-DAG-memb* PKC/PKC-basal* PKC/PKC-AA* DAG PLA2/PLA2-cytosolic PLA2/PLA2-Ca* PLA2/PIP2-PLA2* PLA2/PIP2-Ca-PLA2* PLA2/DAG-Ca-PLA2* PLA2/PLA2*-Ca PLA2/PLA2* PLCbeta/PLC PLCbeta/PLC-Ca PLCbeta/PLC-Ca-Gq PLCbeta/PLC-Gq BetaGamma G*GTP G*GDP Gq/G-GDP Gq/mGluR Gq/Rec-Glu Gq/Rec-Gq Gq/Rec-Glu-Gq Gq/Blocked-rec-Gq MAPK/Raf-GTP-Ras MAPK/Raf*-GTP-Ras Ras/GEF-Gprot-bg Ras/GTP-Ras Ras/GDP-Ras EGFR/EGFR EGFR/L.EGFR EGFR/Internal_L.EGFR PLC_g/PLC_g PLC_g/PLC_G* PLC_g/Ca.PLC_g PLC_g/Ca.PLC_g* AC/AC1-CaM AC/AC1 AC/AC2* AC/AC2-Gs AC/AC2 AC/AC1-Gs AC/AC2*-Gs Gs-alpha)
		delete {name}/diff
	end
end


// Specialized to deal with the nonscaf_syn3c.g and related models.
// This version handles second messenger calculations.
// Do not use for general calculations.
// Here D1 is the diffusion constant for proteins, and D2 for 2nd mess
// This version also deletes the diffusion step for a bunch of
// non-diffusing molecules.

function make_1d_diff_2ndmess_pair(bulkvol, smallvol, xa, len, D1, D2)
	float bulkvol
	float smallvol
	float xa	// Cross sectional area of diffusion channel
	float len	// length of diffusional channel
	float D1		// Diffusion constant for proteins
	float D2		// Diffusion constant for 2nd messengers.

	make_1d_diff_pair {bulkvol} {smallvol} {xa} {len} {D1}

	float k = D2 * xa / len
	float kf = k / smallvol
	float kb = k / bulkvol
	
	str name

	foreach name (Ca cAMP AA DAG IP3)
		setfield /kinetics/{name}/diff \
			kf {kf} \
			kb {kb}
	end
	clear_non_diffusing_molecules
	reset
end
