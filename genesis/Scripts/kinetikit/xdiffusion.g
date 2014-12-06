//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 2003-2004 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/

int PLOT_XDIVS = 6000

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
// Requires that the old volume is correctly set in /editcontrol/vol
// All compartments are rescaled to the segment volume.
// Generates a set of compartments named 'chain[i]' on /kinetics/
// The contents of /kinetics are now the first link on the chain,
// followed by /kinetics/chain/kinetics[0], /kinetics/chain/kinetics[1]
// and so on.
function make_1d_diff_chain(xa, segment_len, n_segment, D)
	float xa
	float segment_len
	int n_segment
	float D

	float seg_vol = xa * segment_len
	echo seg_vol = xa * segment_len
	echo {seg_vol} = {xa} * {segment_len}

	rescale_vol {seg_vol}
	setfield /editcontrol/vol value {seg_vol}
	float k = D * xa / segment_len
	float kf = k / seg_vol
	float kb = k / seg_vol
	int i

	create group /chain
	createmap /kinetics /chain	{n_segment - 1} 1 \
		-delta 4 0

	if (n_segment > 0)
	// Nasty little hack needed because GENESIS removes indices
	// from path names if index is zero
		diff_connect /chain/kinetics /chain/kinetics[1] {kf} {kb}
	end
	for (i = 2; i < n_segment - 1; i = i + 1)
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



function diffusion_load_instructions
	xtextload /diffusion/instructions \
	"        Instructions for the diffusion tool:" \
	"This tool is used to build a diffusive model in kkit." \
	"It uses the currently loaded model as a template for the" \
	"reaction scheme, and duplicates the reaction scheme across"\
	"several compartments in the specified geometry. Currently only" \
	"simple one-dimensional geometries are supported."
end

function name_diff_geometry
	return {getfield /diffusion/{getfield /diffusion mode{DIFFGEOM}} onlabel}
	/*
	if (DIFFGEOM == NO_DIFF_GEOM)
		return "No diffusion"
	end
	if (DIFFGEOM == CYL_DIFF_GEOM)
		return "Axial diffusion along cylinder"
	end
	if (DIFFGEOM == UDOMAIN_DIFF_GEOM)
		return "Microdomain on cell"
	end
	if (DIFFGEOM == SPINE_CELL_DIFF_GEOM)
		return "Spine on cell"
	end
	if (DIFFGEOM == SPINE_CYL_DIFF_GEOM)
		return "Spine on cylinder"
	end
	if (DIFFGEOM == MANYSPINE_CYL_DIFF_GEOM)
		return "Series of spines on cylinder"
	end
	*/
end

function make_diff_model(mode)
	int mode
	if (mode == DIFFGEOM)
		xhide /diffusion
		return
	end
	if (mode == 0)
		delete /kinetics/bulk
		delete /kinetics/chain
		DIFFGEOM = 0
		xhide /diffusion
		return
	end
	if (DIFFGEOM != 0)
		do_inform "Warning: Diffusion already activated, geometry = "{name_diff_geometry}
		return
	end
	float bulkvol = {getfield /diffusion/bulkvol value} * 1e-18
	float comptvol = {getfield /diffusion/comptvol value} * 1e-18
	float D = {getfield /diffusion/maindiffconst value} * 1e-12
	float Dfast = {getfield /diffusion/fastdiffconst value} * 1e-12
	float Dslow = {getfield /diffusion/slowdiffconst value} * 1e-12
	float xa
	float len

	if (mode == UDOMAIN_DIFF_GEOM)
		xa = {pow {comptvol} 0.6666666667}
		len = {pow {comptvol} 0.3333333333}
		gen_make_1d_diff_pair {bulkvol} {comptvol} {xa} {len} {D}
	end
	if (mode == SPINE_CELL_DIFF_GEOM)
		xa = {getfield /diffusion/spineshaftdia value}
		len = {getfield /diffusion/spineshaftlen value}
		xa = PI * xa * xa / 4
		gen_make_1d_diff_pair {bulkvol} {comptvol} {xa} {len} {D}
	end
	if (mode == CYL_DIFF_GEOM)
		float dia = {getfield /diffusion/cyl_dia value} * 1e-6
		len = {getfield /diffusion/cyl_len value} * 1e-6
		int nseg = {getfield /diffusion/cyl_num value}
		if (nseg < 2)
			do_inform "Error: Cylinder must have >= 2 segments"
			setfield /diffusion/cylinder state 0
			return
		end

		xa = PI * dia * dia / 4
		len = len / nseg
		make_1d_diff_chain {xa} {len} {nseg} {D}
	end

	DIFFGEOM = mode
	xhide /diffusion
end

function set_diff_mode(mode)
	int mode

	setfield /diffusion/nodiff state 0
	setfield /diffusion/cylinder state 0
	setfield /diffusion/microdomain state 0
	setfield /diffusion/spine state 0
	setfield /diffusion/spine2 state 0
	setfield /diffusion/spine3 state 0

	setfield /diffusion/{getfield /diffusion mode{mode}} state 1
	make_diff_model {mode}
end

function make_diff_plots
	str plot
	str src
	str srcname
	str plotname
	str newplot
	str chain
	int index

	foreach plot ({el /graphs/conc1/#[TYPE=xplot]})
		src = {getmsg {plot} -incoming -source 0}
		// /kinetics/ is 10 chars in length
		srcname = {substring {src} 10}
		plotname = {getpath {plot} -tail}

		if ({exists /kinetics/bulk/{srcname}})
			create xplot /moregraphs/conc3/{plotname}
			newplot = {el ^}
			addmsg /kinetics/bulk/{srcname} {newplot} PLOT Co *bulk_{plotname} *{getfield {plot} fg}
			setfield {newplot} script "delete_plot.w <s> <d>; edit_plot.D <w>"
			setfield {newplot} fg {getfield {plot} fg}
			useclock {newplot} {PLOTCLOCK}
		end
		foreach chain ({el /kinetics/chain/kinetics[]})
			index = {getfield {chain} index}
			create xplot /moregraphs/conc3/c{index}_{plotname}
			newplot = {el ^}
			addmsg /kinetics/chain/kinetics[{index}]/{srcname} {newplot} PLOT Co *c{index}_{plotname} *black
			setfield {newplot} script "delete_plot.w <s> <d>; edit_plot.D <w>"
			setfield {newplot} fg {((1 + index) * 11) % 64}
			useclock {newplot} {PLOTCLOCK}
		end
	end

	do_inform "Plots created on 'moregraphs'."
end

function make_xdiffusion
	create xform /diffusion [{EX},0,{EW},{WINHT - WMH - BORDER}]
	addfield /diffusion mode0
	setfield /diffusion mode0 "nodiff"
	addfield /diffusion mode1
	setfield /diffusion mode1 "cylinder"
	addfield /diffusion mode2
	setfield /diffusion mode2 "microdomain"
	addfield /diffusion mode3
	setfield /diffusion mode3 "spine"
	addfield /diffusion mode4
	setfield /diffusion mode4 "spine2"
	addfield /diffusion mode5
	setfield /diffusion mode5 "spine3"
	ce /diffusion
		create xtext instructions [0,0:last,100%,15%] -bg white
		create xlabel geometry \
			-label "Select diffusion geometry" -bg cyan
		create xtoggle nodiff -label "No diffusion" -onbg white \
			-script "set_diff_mode 0"
		create xtoggle cylinder [0,0:nodiff,50%,] \
			-label "Uniform cylinder" -onbg white \
			-script "set_diff_mode 1"
		create xtoggle microdomain [0:last,0:nodiff,50%,] \
			-label "Microdomain on cell" -onbg white \
			-script "set_diff_mode 2"
		create xtoggle spine [0,0:cylinder,50%,] \
			-label "Dendritic spine on sphere" -onbg white \
			-script "set_diff_mode 3"
		create xtoggle spine2 [0:last,0:cylinder,50%,] \
			-label "Dendritic spine on cylinder" \
			-onbg white -offfg gray \
			-script "set_diff_mode 4"
		create xtoggle spine3 [0,0:spine2,100%,] \
			-label "Series of dendritic spines on cylinder" \
				-onbg white -offfg gray \
			-script "set_diff_mode 5"
		create xlabel parameters \
			-label "Geometry parameters" -bg cyan
		create xdialog comptvol -label "Compartment/spine volume (fl)" \
			-value 1
		create xdialog bulkvol -label "Bulk volume (fl)" \
			-value 1000
		create xdialog cyl_len -label "Cylinder total length (um)" \
			-value 100
		create xdialog cyl_dia -label "Cylinder diameter (um)" \
			-value 1
		create xdialog cyl_num -label "Cylinder # of compartments" \
			-value 5
		create xdialog spineshaftlen -label "Spine shaft length (um)" \
			-value 1
		create xdialog spineshaftdia -label "Spine shaft diameter (um)"\
			-value 0.1
		create xlabel diffconsts \
			-label "Diffusion constants (um^2/sec)" -bg cyan
		create xdialog maindiffconst -label "Bulk diffusion, D:" \
			-value 1
		create xdialog fastdiffconst -label "Rapid diffusion (e.g, 2nd msgr):RD" \
			-value 10
		create xdialog fastdiffpath -label "Path for rapid diffusion" \
			-value ""
		create xdialog slowdiffconst -label "Slow diffusion (e.g, in membrane):SD" \
			-value 0.01
		create xdialog slowdiffpath -label "Path for slow diffusion" \
			-value ""
		create xlabel spacer -label " " -bg cyan
		create xbutton remove_diffusion -label "Remove diffusion" \
			-script remove_diffusion
		create xbutton make_diff_plots -label "Add plots for diffusive compartments" \
			-script make_diff_plots
		create xbutton HIDE -script "xhide /diffusion" -ygeom 2:last
	ce /
	diffusion_load_instructions
	set_diff_mode {DIFFGEOM}
end

