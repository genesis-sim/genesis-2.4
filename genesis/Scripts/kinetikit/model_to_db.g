//genesis
/**********************************************************************
** This program is part of kinetikit and is
**           copyright (C) 1995-2001 Upinder S. Bhalla.
** It is made available under the terms of the GNU General Public License. 
** See the file COPYRIGHT for the full notice.
**********************************************************************/
int MAX_OPTIONS = 17

function xdumpdb
	int state
	if (!{exists /kinetics/doqcsinfo})
		create doqcsinfo /kinetics/doqcsinfo
	end
	pushe /kinetics/doqcsinfo
		setfield filename {getfield /dumpdb/filename value}
		setfield accessname {getfield /dumpdb/accessname value}
		state = {getfield /dumpdb/atype state}
		if (state == 1)
			setfield accesstype network
		else
			setfield accesstype pathway
		end
		setfield developer {getfield /dumpdb/developer value}
		setfield transcriber {getfield /dumpdb/transcriber value}
		setfield citation {getfield /dumpdb/citation value}
		setfield species {getfield /dumpdb/Species value}
		setfield tissue {getfield /dumpdb/Tissue value}
		setfield cellcompartment {getfield /dumpdb/Cell_Compartment value}
		setfield methodology {getfield /dumpdb/Methodology value}
		setfield sources {getfield /dumpdb/Sources value}
		setfield model_implementation {getfield /dumpdb/Model_Implementation value}
		setfield model_validation {getfield /dumpdb/Model_Validation value}
		call /dumpdb/NOTES PROCESS
		call notes FROM_XTEXT /dumpdb/NOTES
	pope

	save_sim {getfield /dumpdb/filename value} __nothere

	xhide /dumpdb
end

function make_dumpobj
	create neutral /dumpobj
	addfield /dumpobj filename
	addfield /dumpobj accessname
	addfield /dumpobj accesstype
	addfield /dumpobj transcriber
	addfield /dumpobj developer
	addfield /dumpobj citation
	addfield /dumpobj species
	addfield /dumpobj tissue
	addfield /dumpobj cellcompartment
	addfield /dumpobj methodology
	addfield /dumpobj sources
	addfield /dumpobj model_implementation
	addfield /dumpobj model_validation
	addfield /dumpobj editfunc -description "func for dumpobj edit"
	create text /dumpobj/notes
	addobject doqcsinfo /dumpobj \
		-author "Upi Bhalla NCBS 2005" \
		-description "Object for dumping models in form good for doqcs"
	setdefault doqcsinfo editfunc "show_dumpdb"
end

// Assumes that the doqcs info is the only one there is.
function do_update_dumpdb
	pushe /kinetics/doqcsinfo
		setfield /dumpdb/filename value {getfield  filename}
		setfield /dumpdb/accessname value {getfield accessname}
		if ({strcmp {getfield accesstype} "network"} == 0)
			setfield /dumpdb/atype state 1
		else
			setfield /dumpdb/atype state 0
		end
		setfield /dumpdb/developer value {getfield developer}
		setfield /dumpdb/transcriber value {getfield transcriber}
		setfield /dumpdb/citation value {getfield citation}
		setfield /dumpdb/Species value {getfield species}
		setfield /dumpdb/Tissue value {getfield tissue}
		setfield /dumpdb/Cell_Compartment value {getfield cellcompartment}
		setfield /dumpdb/Methodology value {getfield methodology}
		setfield /dumpdb/Sources value {getfield sources}
		setfield /dumpdb/Model_Implementation value {getfield model_implementation}
		setfield /dumpdb/Model_Validation value {getfield model_validation}
		call notes TO_XTEXT /dumpdb/NOTES
	pope
end

function show_dumpdb
	if (!{exists /kinetics/doqcsinfo})
		create doqcsinfo /kinetics/doqcsinfo
	else
		do_update_dumpdb
	end
	xshow /dumpdb
end

function do_clear_option(w)
	str w
	str lname = {getfield {w}/../listname value}
	setfield /dumpdb/{lname} value ""
end

function do_set_option(w, i)
	str w
	int i
	str lname = {getfield {w}/../listname value}
	str opt = {getfield /option_window/{lname}_list f[{i}]}
	str prev = {getfield /dumpdb/{lname} value}
	if ({strlen {prev}} > 0)
		setfield /dumpdb/{lname} value {prev}", "{opt}
	else
		setfield /dumpdb/{lname} value {opt}
	end
end

function make_option_window
	int i
	create xform /option_window [500,100,300,630]
	pushe /option_window
	create neutral listname
	addfield listname value
	create xtext instructions -hgeom 100  \
		-initialtext "Select any option by clicking it. By default successive selections will be appended, separated by commas. Use Clear to clear"
	create xbutton Clear -script "do_clear_option <w>"
	create xbutton Hide -script "xhide /option_window"
	create xlabel Options -bg cyan
	for (i = 0; i < MAX_OPTIONS; i = i + 1)
		create xbutton but[{i}] -script "do_set_option <w> "{i}
	end
	pope
end

function show_option_window(lname)
	str lname
	pushe /option_window
	setfield listname value {lname}
	int i
	int noptions = {getfield {lname}_list nfields}
	for (i = 0; i < noptions; i = i + 1)
		setfield but[{i}] label {getfield {lname}_list f[{i}]}
		xshow but[{i}]
	end
	for (i = noptions; i < MAX_OPTIONS ; i = i + 1)
		xhide but[{i}]
	end
	xshow /option_window
	pope
end

function make_option_list(name)
	str name
	str lastwidg = {getpath {el ^} -tail}
	create neutral /option_window/{name}_list
	create xdialog {name} -wgeom 80% \
		-value "" -script "set_options <w> "{name}
	create xbutton {name}_view [0:last,0:{lastwidg},20%,30] \
		-label "Options.." -script "show_option_window "{name}
end

function make_and_set_extfield(value)
	if (!{exists . nfields})
		addfield . nfields
		setfield . nfields 0
	end
	int i = {getfield . nfields}
	addfield . f[{i}]
	setfield . f[{i}] {value}
	setfield . nfields {i + 1}
end

function fill_option_lists
	pushe /option_window/Species_list
		make_and_set_extfield "rat"
		make_and_set_extfield "human"
		make_and_set_extfield "bovine"
		make_and_set_extfield "E. Coli"
		make_and_set_extfield "General Mammalian"
		make_and_set_extfield "mouse"
		make_and_set_extfield "Xenopus"
	pope
	pushe /option_window/Tissue_list
		make_and_set_extfield "Neuronal"
		make_and_set_extfield "Lung"
		make_and_set_extfield "Liver"
		make_and_set_extfield "E.coli Expression"
		make_and_set_extfield "Sf9 Expression"
		make_and_set_extfield "Sf21 Expression"
		make_and_set_extfield "CHO Expression"
		make_and_set_extfield "NIH 3T3 Expression"
		make_and_set_extfield "Oocyte extract"
	pope
	pushe /option_window/Cell_Compartment_list
		make_and_set_extfield "Surface"
		make_and_set_extfield "Cytosol"
		make_and_set_extfield "Nucleus"
		make_and_set_extfield "Postsynaptic Density"
		make_and_set_extfield "Synaptic Spine"
		make_and_set_extfield "Synaptic Cleft"
		make_and_set_extfield "Presynaptic Bouton"
		make_and_set_extfield "Synaptic Vesicle"
		make_and_set_extfield "Endocytotic Vesicle"
		make_and_set_extfield "Golgi"
		make_and_set_extfield "ER membrane"
		make_and_set_extfield "Mitochondrial membrane"
		make_and_set_extfield "Mitochondrial lumen"
		make_and_set_extfield "Microsomes"
		make_and_set_extfield "Extracellular"
		make_and_set_extfield "Recycling Compartment"
		make_and_set_extfield "Endosome"
	pope
	pushe /option_window/Methodology_list
		make_and_set_extfield "Quantitative match to experiments"
		make_and_set_extfield "Qualitative"
		make_and_set_extfield "Hypothetical"
		make_and_set_extfield "Illustrative"
	pope
	pushe /option_window/Sources_list
		make_and_set_extfield "Peer-reviewed publication"
		make_and_set_extfield "Book"
		make_and_set_extfield "In-house"
		make_and_set_extfield "Other database"
	pope
	pushe /option_window/Model_Implementation_list
		make_and_set_extfield "Exact GENESIS implementation"
		make_and_set_extfield "Mathematically equivalent"
		make_and_set_extfield "Approximate implementation"
		make_and_set_extfield "Not implemented/tested in GENESIS "
	pope
	pushe /option_window/Model_Validation_list
		make_and_set_extfield "Replicates original data "
		make_and_set_extfield "Approximates original data "
		make_and_set_extfield "Quantitatively predicts new data"
		make_and_set_extfield "Qualitative predictions"
	pope
end

function make_xdump_db
	make_dumpobj
	create xform /dumpdb [5%,100,50%,640] \
		-label "Generate database dump file"
	addfield /dumpdb elmpath
	pushe /dumpdb
	setfield elmpath /kinetics/doqcsinfo
	create xdialog filename -label "Database dump file name" \
		-value "db.g"
	create xdialog accessname -label "Name of accession:" \
		-value "New_pathway" -script "set_field <w> <v>"
	// create xdialog atype -label "Type of accession:" -value "network"
	create xlabel atypelabel -wgeom 50% -label "Accession type:"
	create xtoggle atype [0:last,0:accessname,50%,] \
		-onlabel "Network" -offlabel "Pathway"
	create xdialog transcriber -label "Transcriber:" \
		-value "Upinder S. Bhalla, NCBS" -script "set_field <w> <v>"
	create xdialog developer -label "Model developer:" \
		-value "Upinder S. Bhalla, NCBS" -script "set_field <w> <v>"
	create xdialog citation -label "Citation:" \
		-value "citation here" -script "set_field <w> <v>"
	create xlabel visilabel -wgeom 50% -label "Public model access:"
	create xtoggle public_invisible [0:last,0:citation,50%,] \
		-onlabel "Invisible" -offlabel "Visible"
	make_option_window
	create xlabel optlabel -label "Database Accession Options" -bg cyan
	make_option_list "Species"
	make_option_list "Tissue"
	make_option_list "Cell_Compartment"
	make_option_list "Methodology"
	make_option_list "Sources"
	make_option_list "Model_Implementation"
	make_option_list "Model_Validation"
	fill_option_lists
	
	/*
	create xdialog species -label "Species" -value "rat"
	create xdialog tissue -label "Tissue" -value "brain"
	create xdialog cellcompartment -label "Cell Compartment" -value "cytosol"
	*/
	create xlabel notes -label "NOTES" -bg cyan
	create xtext NOTES -hgeom 100 -editable 1
	create xbutton Cancel -label "Hide" \
		-script "xhide /dumpdb"
	create xbutton Submit -script "xdumpdb"
	pope
end
