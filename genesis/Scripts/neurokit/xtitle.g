//genesis

str quit_form = "Do you really want to quit ?"
str help_form = "Using the Neuron simulation kit"
str file_form = "file_form"
str select_form = "Select Level of analysis"
str chosencell = "Selected cell name"
str iocell = "Cell for IO"
str author = "Your Name"

function check_iocell

str io = {getfield /{file_form}/{iocell} value}

	if (({findchar {io} /}) != 0)
		echo Invalid cell for I/O. Cell must be of form  \
		    '/cellname'.
		setfield /{file_form}/{iocell} value /cell
		return
	end
	if (({findchar {substring {io} 1} /}) != -1)
		echo Invalid cell for I/O. Cell must be of form  \
		    '/cellname'.
		setfield /{file_form}/{iocell} value /cell
	end
end

function do_title_bar

	create x1form /title_bar [0,0,{user_screenwidth},50] -nolabel
	ce ^
	disable .
    create x1button quit [1%,2,13%,] -script popup_quit
    create x1button help [15%,2,14%,] -script popup_help
    create x1button "file" [30%,2,14%,] -script popup_file
    create x1button "run cell" [45%,2,14%,] -script  \
        "set_level run_neuron"
    create x1button "edit cell" [60%,2,14%,] -script "set_level neuron"
    create x1button "edit compt" [75%,2,12%,] -script "set_level compt"
    create x1button "edit channel" [88%,2,11%,] -script  \
        "set_level channel"

	create x1form /{quit_form} [0,40,300,100] -title {quit_form}
	ce ^
	disable .
	create x1button yes -script quit
	create x1button no -script popdown_quit

	create x1form /{help_form} [160,40,600,850] -notitle
	ce ^
	disable .
	create x1label /{help_form}/header [1%,2,98%,25] -title  \
	    {help_form}
	create x1label /{help_form}/instr [1%,28,98%,25] -title  \
	       "Use the scroll bar at the left to move through the text"

	create x1text /{help_form}/text [1%,55,98%,87%] -filename {user_help}
	create x1button "CANCEL" -script popdown_help

	create x1form /{select_form} [480,40,300,100]
	ce ^
	disable .
	create x1button "Single Neuron level" -script "set_level neuron"
	create x1button "Compartment level" -script "set_level compt"
	create x1button "Channel level" -script "set_level channel"
	create x1button "CANCEL" -script popdown_select

	create x1form /{file_form} [320,40,300,325] -title "File Menu"
	ce ^
	disable .
	create x1label "Cell Selection"
	create x1dialog {chosencell} -value /mit -script  \
	    "choose_cell "<widget>
	create x1label "Cell Parameter IO"
	create x1dialog {iocell} -value /camit -script check_iocell
	create x1dialog source_file_name -value camit.p
	create x1button "Load from file" -script do_cell_load
	create x1dialog "Save to file" -script "do_cell_save "<widget>  \
	    -value newcamit.p
	create x1dialog {author} -value " "
	create x1button "New Cell" -script "do_new_cell "<widget>
	// above "create x1button" had the following option, but
	// x1buttons don't support it and set code will complain
	// about the unknown field.  ---dhb
	//	-value "/newcell"
	create x1label " "
	create x1button "CANCEL" -script popdown_file
	create neutral /dummy

    setfield /file_form/"Selected cell name" value {user_cell}
	check_iocell
    setfield /file_form/"Cell for IO" value {user_cell}
    setfield /file_form/"source_file_name" value {user_pfile}
    setfield /file_form/"Save to file" value new{user_pfile}
    setfield /file_form/{author} value {user_name}
end

function popup_quit
	xshowontop /{quit_form}
end

function popup_help
	xshowontop /{help_form}
end

function popup_file
	xshowontop /{file_form}
end

function popup_select
	xshowontop /{select_form}
end


function popdown_quit
	xhide /{quit_form}
end

function popdown_help
	xhide /{help_form}
end

function popdown_file
	xhide /{file_form}
end

function popdown_select
	xhide /{select_form}
end

function choose_cell(widget)
	str widget

	int n
	str num, cellw

    pushe /{file_form}
	xhide .
	str newcell = {getfield {chosencell} value}

	if (!({exists {newcell}}))
		echo {newcell} does not exist
		return
	end
	if ({strcmp {cellpath} {newcell}} == 0)
		return
	end

	if ({exists {cellpath}})
   		disable {cellpath}
		for (n = 1; n <= 2; n = n + 1)
			num = n
			if (({exists {cellpath}xout{num}}))
				disable {cellpath}graph{num}
				disable {cellpath}xout{num}
				xhide {cellpath}graph{num}
				xhide {cellpath}xout{num}
			end
		end
	end

    cellpath = newcell
	for (n = 1; n <= 2; n = n + 1)
		num = n
		if (!({exists {cellpath}xout{num}}))
			do_xout {num}
			do_xgraph {num}
		end
		if ({strcmp {analysis_level} "run_neuron"} == 0)
			if (num <= user_numxouts)
				xshowontop {newcell}graph{num}
				xshowontop {newcell}xout{num}
			end
		end
	end
   	enable {cellpath}
	for (n = 1; n <= (user_numxouts); n = n + 1)
		num = n
		enable {cellpath}graph{num}
		enable {cellpath}xout{num}
	end
    reset

	for (n = 1; n <= 2; n = n + 1)
	  num = n
	  cellw = (cellpath) @ "xout" @ (num) @ "/draw/cell"
	  setfield {cellw} autofat TRUE
	  setfield {cellw} fatfix FALSE
	  if ((user_symcomps))
   		setfield {cellw}  \
   		    path {cellpath}"/##[TYPE=symcompartment]"
   		setfield /compt_select/draw/cell  \
   		    path {cellpath}"/##[TYPE=symcompartment]"
	  else
   		setfield {cellw} path {cellpath}"/##[TYPE=compartment]"
   		setfield /compt_select/draw/cell  \
   		    path {cellpath}"/##[TYPE=compartment]"
	  end
	  setfield {cellw} fatfix TRUE
	  setfield {cellw} autofat FALSE
	end
	setfield /file_IO/field_path  \
	    value {cellpath}/{user_filefieldpath}
	pope
end

function do_cell_load
	str wid = "/" @ (file_form) @ "/source_file_name"
	str name

	pushe /{file_form}
	readcell {getfield {wid} value} {getfield {iocell} value}
	reset
	pushe /{file_form}
    setfield {chosencell} value {getfield {iocell} value}
    choose_cell {chosencell}
	/* \
	     find type of first compartment, assume they are all same type  \
	    */
	foreach name ({el {cellpath}/#[]})
		if ( \
		    {strcmp {getfield {name} object->name} compartment} == 0 \
		    )
			user_symcomps = 0
		else
			user_symcomps = 1
		end
		break		/* exit after first iteration of the loop */

	end
    pope
	xhide /{file_form}
	/* set correct integration method */
	intmethod "/cell_run_control/method"
end

function do_cell_save(widget)
	str widget

	pushe /{file_form}
	writecell {getfield {widget} value} {getfield {iocell} value}  \
	    -author {getfield {author} value}
    pope
	xhide /{file_form}
end

function do_new_cell(widget)
	str widget

	echo not yet implemented
end

function set_level(level)
	str level

	if (!({exists {cellpath}}))
		echo {cellpath} is not defined yet : a cell must be  \
		    loaded in.
		return
	end

	if (({strcmp {analysis_level} {level}}) == 0)
		return
	end

	if (({strcmp {level} "run_neuron"}) == 0)
		xshowontop /cell_run_control
		xshowontop {cellpath}xout1
		xshowontop {cellpath}graph1
		if ((user_numxouts) > 1)
			xshowontop {cellpath}xout2
			xshowontop {cellpath}graph2
		end
		/*
		if ({user_symcomps})
			set {cellpath}/##[TYPE=symcompartment] inject 0.0
		else
			set {cellpath}/##[TYPE=compartment] inject 0.0
		end
		*/
		do_run_clickmode /cell_run_control/Record

		xhide /compt_display
		xhide /compt_select
		xhide /compt_lib_form
	else
		if (({strcmp {analysis_level} "new"}) == 0)
		xshowontop /compt_display
		setfield /compt_select/draw/cell  \
		    path {cellpath}/##[TYPE=compartment]
		xshowontop /compt_select
		xshowontop /compt_lib_form
		end
	end
	if (({strcmp {level} "neuron"}) == 0)
		xshowontop /cell_params
	end
	if (({strcmp {level} "compt"}) == 0)
		xshowontop /compt_params
	end
	if (({strcmp {level} "channel"}) == 0)
		xshowontop /channel_params
	end

	if (({strcmp {analysis_level} "run_neuron"}) == 0)
		xshowontop /compt_display
		if ((user_symcomps))
			setfield /compt_select/draw/cell  \
			    path {cellpath}/##[TYPE=symcompartment]
		else
			setfield /compt_select/draw/cell  \
			    path {cellpath}/##[TYPE=compartment]
		end
		xshowontop /compt_select
		xshowontop /compt_lib_form

		xhide /cell_run_control
		xhide /file_IO
		xhide {cellpath}xout1
		xhide {cellpath}graph1
		xhide {cellpath}xout2
		xhide {cellpath}graph2
	end
	if (({strcmp {analysis_level} "neuron"}) == 0)
		xhide /cell_params
	end
	if (({strcmp {analysis_level} "compt"}) == 0)
		xhide /compt_params
	end
	if (({strcmp {analysis_level} "channel"}) == 0)
		xhide /channel_params
	end

	analysis_level = level
end
