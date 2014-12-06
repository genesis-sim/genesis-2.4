//genesis

str lastinjectsite1 = "none"
str lastinjectsite2 = "none"

// This function is accessed from the 'when' command in the xsout file
function set_field(widget, view)
str widget
str view
	setfield {view} field {getfield {widget} value}
end

// This function is accessed from the 'when' command in the xsout file
function autoscale_toggle(widget, view)
str widget
	if (({getfield {widget} state}) == 1)
		setfield {view} autoscale TRUE
	else
		setfield {view} autoscale FALSE
	end
end

function click_site(num)
	str num

	setfield /cell_run_control/click_site{num}  \
	    value {getfield {cellpath}xout{num}/draw value}
end

function do_inject(num)
	str num
	int inum = num
	str widget = (cellpath) @ "xout" @ (num) @ "/draw"

	str name = {getfield {widget} value}
	str iname = {getfield {name} name} @ {getfield {name} index}

	click_site {num}

	setfield {name}  \
	    inject {{getfield /cell_run_control/"inject (nanoAmps)" value}/1e9}
	if (inum == 1)
		lastinjectsite1 = name
	else
		lastinjectsite2 = name
	end
	echo Setting injection on {name} to  \
	    {getfield /cell_run_control/"inject (nanoAmps)" value} nA.
	if (({exists {widget}/Inj{iname}}))
		return
	end

	copy /xproto/draw/Inj {widget}/Inj{iname}
	setfield {widget}/Inj{iname} tx {getfield {name} x}  \
	    ty {getfield {name} y} tz {getfield {name} z}
end

function clear_inject(num)
	str num
	int inum = num
	str widget = (cellpath) @ "xout" @ (num) @ "/draw"

	str name = {getfield {widget} value}
	str iname = {getfield {name} name} @ {getfield {name} index}
	click_site {num}

	setfield {name} inject {0.0}
	if (inum == 1)
		lastinjectsite1 = "none"
	else
		lastinjectsite2 = "none"
	end
	echo Setting injection on {name} to 0 nA.
	if (({exists {widget}/Inj{iname}}))
		delete {widget}/Inj{iname}
	end
end


function do_vclamp(num)
	str num
	int inum = num
	str widget = (cellpath) @ "xout" @ (num) @ "/draw"
	float temp

	str name = {getfield {widget} value}
	str iname = {getfield {name} name} @ {getfield {name} index}
	if (inum == 1)
		lastinjectsite1 = name
	else
		lastinjectsite2 = name
	end
	click_site {num}

	if (({exists {widget}/Vclamp{iname}}))
		temp = {getfield {name}/Vclamp x}
		temp = temp*1e3
		setfield /cell_run_control/"Clamp voltage (mV)"  \
		    value {temp}
		return
	end

	copy /xproto/draw/Vclamp {widget}/Vclamp{iname}
	setfield {widget}/Vclamp{iname} tx {getfield {name} x}  \
	    ty {getfield {name} y} tz {getfield {name} z}

	/*
	copy /library/Vclamp {widget}/Vclamp{iname}
	push {widget}/Vclamp{iname}/Vclamp
	*/
	copy /library/Vclamp {name}
	pushe {name}/Vclamp

	temp = {getfield /cell_run_control/"Clamp voltage (mV)" value}
	echo Setting clamp Voltage on {name} to {temp} mV.
	setfield . x {temp/1.0e3}
	addmsg . lowpass INJECT x
	addmsg {name} PID SNS Vm
	addmsg PID {name} INJECT output
	resched
	pope
end


function clear_vclamp(num)
	str num
	int inum
	str widget = (cellpath) @ "xout" @ (num) @ "/draw"

str name = {getfield {widget} value}
str iname = {getfield {name} name} @ {getfield {name} index}
	click_site {num}

	setfield {name} inject {0.0}
	if (inum == 1)
		lastinjectsite1 = "none"
	else
		lastinjectsite2 = "none"
	end
	echo Removing Voltage clamp from {name}
	if (({exists {widget}/Vclamp{iname}}))
		delete {widget}/Vclamp{iname}
		delete {name}/Vclamp
	end
end


int col1 = 15
int col2 = 15
function add_plot(num)
	str num
	str draww = (cellpath) @ "xout" @ (num) @ "/draw"
	str graphw = (cellpath) @ "graph" @ (num) @ "/graph"
	str newplot = {getfield {draww} value}
    str  \
        fieldpath = (newplot) @ "/" @ ({getfield {draww}/cell fieldpath})
    str field = {getfield {draww}/cell colfield}
    str name
	int col
    click_site {num}

	if ({strcmp {num} "1"} == 0)
		col = col1
	else
		col = col2
	end
	name = ({getfield {newplot} name}) @ "[" @ ({getfield {newplot} index}) @ "]"
    if (({exists {draww}/Tr{name}}))
        echo already recording from {newplot}, {name}
        return
    end
	if (!({exists {fieldpath}}))
		echo cannot record from {fieldpath} : it does not exist
		return
	end
    //DHB addmsg {fieldpath} {graphw} PLOT {field} *{name} *hot{col}
    addmsg {fieldpath} {graphw} PLOT {field} *{name} *{col}
	setfield {graphw}  \
	    YUnits {getfield {draww}/cell fieldpath}:{field}
	call {graphw}/{name} RESET
	copy /xproto/draw/Trode {draww}/Tr{name}
	setfield {draww}/Tr{name} pixcolor /*"hot"*/{col}  \
	    tx {getfield {newplot} x} ty {getfield {newplot} y}  \
	    tz {{getfield {newplot} z} + 0.00001}
    col = col + 7
	if (col > 63)
        col = col - 60
    end
	if ({strcmp {num} "1"} == 0)
		col1 = col
	else
		col2 = col
	end
    echo adding plot for element '{fieldpath}' of {field}
end

function drop_plot(num)
	str num
	str draww = (cellpath) @ "xout" @ (num) @ "/draw"
	str graphw = (cellpath) @ "graph" @ (num) @ "/graph"
    int nmsgs, i
    str oldplot = {getfield {draww} value}
    str fieldpath = {el {oldplot}/{getfield {draww}/cell fieldpath}}
    str src, name
    str field = {getfield {draww}/cell colfield}
    click_site {num}

    nmsgs = {getmsg {graphw} -incoming -count}

    for (i = 0; i < nmsgs; i = i + 1)
        src = {getmsg {graphw} -incoming -source {i}}
        if ({strcmp {src} {fieldpath}} == 0)
            deletemsg {graphw}  {i} -incoming
            i = i - 1
            nmsgs = nmsgs - 1
			/*
            name = {get({oldplot},name)}+ {get({oldplot},index)}+{field}
			*/
            name = ({getfield {oldplot} name}) @ "[" @ ({getfield {oldplot} index}) @ "]"
            delete {draww}/Tr{name}
    		echo dropping plot for '{field}' on element  \
    		    '{fieldpath}'
        end
    end

end

function color_plot(num)
	str num
	str draww = (cellpath) @ "xout" @ (num) @ "/draw"
	str graphw = (cellpath) @ "graph" @ (num) @ "/graph"
    int nmsgs, i, k, l
	int col
    str oldplot, src, name, char
    str field = {getfield {draww}/cell colfield}
    str fieldpath = ({getfield {draww}/cell fieldpath})
	str otype

	if ({strcmp {num} "1"} == 0)
		col = col1
	else
		col = col2
	end
    nmsgs = {getmsg {graphw} -incoming -count}
    col = col + 7
	if (col > 63)
        col = col - 60
    end
	if ({strcmp {num} "1"} == 0)
		col1 = col
	else
		col2 = col
	end

    for (i = 0; i < nmsgs; i = i + 1)
        src = {getmsg {graphw} -incoming -source {0}}
//        deletemsg {graphw}  {0} -incoming
		/*
		k = strlen({src})	
		for (l=k; l>0; l=l-1) 
			char=substring({src},{l},{l})
			if (strcmp({char},"/")==0)
				break
			end
		end
		if (strcmp({fieldpath},".") == 0) //plot is not a subelement
			oldplot = {src}
		else
			l = l - 1
			oldplot = substring({src},0,{l})
		end
        name = {get({oldplot},name)}+ {get({oldplot},index)}+{field}
        name = {get({oldplot},name)}+"["+{get({oldplot},index)}+"]"
		*/
		/*
		*/
		oldplot = (src)
		if ({strcmp {fieldpath} "."} != 0)		//plot is a subelement

			for (k = 0; k < 5; k = k + 1)
				oldplot = ({el {oldplot}/..})
				otype = {getfield {oldplot} object->name}
				if ( \
				    (({strcmp {otype} compartment}) == 0) || (({strcmp {otype} symcompartment}) == 0) \
				    )
					break
				end
			end
		end
        name = ({getfield {oldplot} name}) @ "[" @ ({getfield {oldplot} index}) @ "]"
    	//DHB addmsg {src} {graphw} PLOT {field} *{name} *hot{col}
//    	addmsg {src} {graphw} PLOT {field} *{name} *{col}
		setfield {graphw}/{name} fg {col}
		setfield {draww}/Tr{name} pixcolor /*"hot"*/{col}
    end
end

function do_spike(num)
	str num
	str compt = {getfield {cellpath}xout{num}/draw value}
	str channame = {getfield /cell_run_control/"Syn Type" value}
	str chanpath = compt @ "/" @ channame
	float weight
	click_site {num}

	if (!{exists {chanpath}})
		echo channel '{chanpath}' does not exist
		return
	end

	call /stimulus/spike_on_command SEND_SPIKE {chanpath}

	echo delivering spike to '{chanpath}'
end

function do_unspike(num)
	str num  // make this a no-op - DEB 8/195
//	str compt = {getfield {cellpath}xout{num}/draw value}
//	str channame = {getfield /cell_run_control/"Syn Type" value}
//	str chanpath = (compt) @ "/" @ (channame)
//	click_site {num}

//	if (!({exists {chanpath}}))
//		echo channel '{chanpath}' does not exist
//		return
//	end
//    setfield {chanpath} X 0.0
//    setfield {chanpath} Y 0.0
end

function do_act(num)
	str num
	str draww = (cellpath) @ "xout" @ (num) @ "/draw"
	str compt = {getfield {draww} value}
	str channame = {getfield /cell_run_control/"Syn Type" value}
	str chanpath = (compt) @ "/" @ (channame)
	str iname = {getfield {compt} name} @ {getfield {compt} index}
	float act
	click_site {num}

	if (!({exists {chanpath}}))
		echo channel '{chanpath}' does not exist
		return
	end

    act = {getfield /cell_run_control/"Amount of synaptic activation" value}

	pushe {draww}
	if (!({exists act{channame}{iname}}))
    	if ({exists /xproto/draw/act{channame}})
    		copy /xproto/draw/act{channame} act{channame}{iname}
		else
    		copy /xproto/draw/actACh act{channame}{iname}
		end
    	setfield {draww}/act{channame}{iname} tx {getfield {compt} x}  \
    	    ty {getfield {compt} y} tz {getfield {compt} z}
		create neutral act{channame}{iname}/dummy
		addmsg act{channame}{iname}/dummy {chanpath} ACTIVATION  \
		    z
	end
   	echo setting activation on {chanpath} to {act}
	setfield act{channame}{iname}/dummy z {act}
	pope
end

function do_unact(num)
	str num
	str draww = (cellpath) @ "xout" @ (num) @ "/draw"
	str compt = {getfield {draww} value}
	str channame = {getfield /cell_run_control/"Syn Type" value}
	str chanpath = (compt) @ "/" @ (channame)
	str iname = {getfield {compt} name} @ {getfield {compt} index}
	click_site {num}

	if (!({exists {chanpath}}))
		echo channel '{chanpath}' does not exist
		return
	end

	pushe {draww}
	if ({exists act{channame}{iname}})
		delete act{channame}{iname}
	else
		echo no activation has been set on {channame}
	end
	pope
end

function do_rand(num)
	str num
	str draww = (cellpath) @ "xout" @ (num) @ "/draw"
	str compt = {getfield {draww} value}
	str channame = {getfield /cell_run_control/"Syn Type" value}
	str chanpath = (compt) @ "/" @ (channame)
	str iname = {getfield {compt} name} @ {getfield {compt} index}
	float rate, weight
	click_site {num}

	if (!({exists {chanpath}}))
		echo channel '{chanpath}' does not exist
		return
	end

	pushe {draww}

    if (({exists rand{channame}{iname}}))
		echo rand input already being sent to {chanpath}
		pope
		return
	end
    addmsg /stimulus/rand {chanpath} RAND_ACTIVATION rate weight
    // This setfield shouldn't be necessary as /stimulus/rand fields
    // should always be updated from the interface elements.  Uncomment
    // if there's a consistency problem.  ---dhb
    //
    //setfield /stimulus/rand \
    //  weight {getfield /cell_run_control/"Spike weight" value} \
    //	rate {getfield /cell_run_control/"Spike rate (Hz)" value}

   	if ({exists /xproto/draw/rand{channame}})
    	copy /xproto/draw/rand{channame} rand{channame}{iname}
	else
    	copy /xproto/draw/randACh rand{channame}{iname}
	end
    setfield rand{channame}{iname} tx {getfield {compt} x}  \
        ty {getfield {compt} y} tz {getfield {compt} z}
    echo delivering rand spikes to '{chanpath}'
	pope
end

function do_unrand(num)
	str num
	str draww = (cellpath) @ "xout" @ (num) @ "/draw"
	str compt = {getfield {draww} value}
	str channame = {getfield /cell_run_control/"Syn Type" value}
	str chanpath = (compt) @ "/" @ (channame)
	str iname = {getfield {compt} name} @ {getfield {compt} index}
	click_site {num}

	if (!({exists {chanpath}}))
		echo channel '{chanpath}' does not exist
		return
	end

	pushe {draww}
    if (({exists rand{channame}{iname}}))
            echo deleting random spike input to {chanpath}
            deletemsg {chanpath} 0 -find /stimulus/rand RAND_ACTIVATION
            delete rand{channame}{iname}
	end
	pope
end

function setup_mouse(draw, x, y, but1, func1, but2, func2, but3, func3)
	str draw, but1, but2, but3, func1, func2, func3
	int x, y

	if (({exists {draw}}) == 0)
		le /
		le {cellpath}xout
		echo draw widget '{draw}' does not exist.
		return
	end

	/*
	x = get({draw},x) + x
	y = get({draw},y) + y
	*/

	if (!({exists {draw}/../MOUSE}))
		pushe {draw}/..
		create x1label MOUSE [{x},{y},330,32] -title  \
		    "MOUSE                                     "
	else
		pushe {draw}/..
	end

	if (!({exists {but1}}))
		create x1button {but1} [{x + 70},{y + 2},75,]
	end

	if (!({exists {but2}}))
		//DHB create xbutton {but2} [{x + 136},{y + 2},50,25]
		create x1button {but2} [10:{but1},{y + 2},75,]
	end
	if (!({exists {but3}}))
		create x1button {but3} [10:{but2},{y + 2},75,]
	end

	dropwhen {draw} all
	when {draw} click1 do {func1}
	when {draw} click2 do {func2}
	when {draw} click3 do {func3}

	pope
end

function select_mouse(draw, but1, func1, but2, func2, but3, func3)
	str draw, but1, but2, but3
	str func1, func2, func3

	if (!({exists {draw}}))
		echo draw widget '{draw}' does not exist.
		return
	end

	pushe {draw}/..

	if (({exists {but1}}) && ({exists {but2}}) && ({exists {but3}}))
		xraise MOUSE
		xraise {but1}
		xraise {but2}
		xraise {but3}

		dropwhen {draw} all
		when {draw} click1 do {func1}
		when {draw} click2 do {func2}
		when {draw} click3 do {func3}
	end
	pope
end

function scalecell(cell)
    str cell

    int x, y, w
    str temp

    //DHB temp = {getfield {cell}/.. form}
    //DHB x = {getfield {temp} x}
    //DHB y = {getfield {temp} y}
    //DHB w = {getfield {temp} width}
    x = {getfield {cell}/../.. xgeom}
    y = {getfield {cell}/../.. ygeom}
    w = {getfield {cell}/../.. wgeom}

    create x1form {cell}_scale [{x},{y},200,415] -title Scale
    create x1button {cell}/scale [0,0,50,] -script  \
        "disp_cellscale "{cell}

    pushe {cell}_scale
    create x1dialog colfield -value {getfield {cell} colfield}
    //DHB create xdialog fatfield -value {getfield {cell} fatfield}
    create x1dialog colmin -value {getfield {cell} colmin}
    create x1dialog colmax -value {getfield {cell} colmax}
    create x1dialog fatmin -value {getfield {cell} fatmin}
    create x1dialog fatmax -value {getfield {cell} fatmax}

    create x1toggle autocol [1%,3:fatmax.bottom,48%,] -state 0
    setfield autocol label0 "Autocol OFF" label1 "Autocol ON"

    create x1toggle colfix [51%,3:fatmax.bottom,48%,] -state 0
    setfield colfix label0 "Colfix OFF" label1 "Colfix ON"

    create x1toggle autofat [1%,3:autocol.bottom,48%,] -state 0
    setfield autofat label0 "Autofat OFF" label1 "Autofat ON"

    create x1toggle fatfix [51%,3:autocol.bottom,48%,] -state 1
    setfield fatfix label0 "Fatfix OFF" label1 "Fatfix ON" state 1

    create x1dialog fatrange -value {getfield {cell} fatrange}
    create x1dialog fieldpath -value {getfield {cell} fieldpath}
    create x1button APPLY -script  \
        "applycellscale "{cell}" "{cell}_scale" 0"
    create x1button APPLY_AND_VANISH -script  \
        "applycellscale "{cell}" "{cell}_scale" 1"

    pope
end


function disp_cellscale(cell)
    str cell
    pushe {cell}_scale

    setfield colmin value {getfield {cell} colmin}
    setfield colmax value {getfield {cell} colmax}
    setfield fatmin value {getfield {cell} fatmin}
    setfield fatmax value {getfield {cell} fatmax}
    pope
    xshowontop {cell}_scale
end

function set_cell_flag(cell, flagname, flag)
    str cell, flagname
    int flag
    str state = "FALSE"

    if (flag == 1)
        state = "TRUE"
    end
    if ({strcmp {state} {getfield {cell} {flagname}}} != 0)
        setfield {cell} {flagname} {state}
    end
end

function applycellscale(cell, cellscale, hide)
    str cell, cellscale
    int hide

    str colfield, fatfield, fieldpath
    float colmin, colmax, fatmin, fatmax, fatrange
    int autofat, autocol, fatfix, colfix

	int l = {strlen {cellpath}} + 4
	str num = {substring {cell} {l} {l}}
	str graphw = (cellpath) @ "graph" @ (num) @ "/graph"
    int nmsgs, i
    str field = {getfield {cell} colfield}
    str path, oldplot, src, name, char
	str col

    pushe {cellscale}
    colfield = {getfield colfield value}
    //DHB fatfield = {getfield fatfield value}
    fieldpath = {getfield fieldpath value}
    autocol = {getfield autocol state}
    autofat = {getfield autofat state}
    //DHB colfix = {getfield colfix state}
    //DHB fatfix = {getfield fatfix state}
    colmin = {getfield colmin value}
    colmax = {getfield colmax value}
    fatmin = {getfield fatmin value}
    fatmax = {getfield fatmax value}
    fatrange = {getfield fatrange value}
        //DHB ({strcmp {colfield} {field}} != 0) || ({strcmp {fatfield} {getfield {cell} fatfield}} != 0) || ({strcmp {fieldpath} {getfield {cell} fieldpath}} != 0) \

    if ( \
        ({strcmp {colfield} {field}} != 0) || ({strcmp {fieldpath} {getfield {cell} fieldpath}} != 0) \
        )
    	nmsgs = {getmsg {graphw} -incoming -count}
    	for (i = 0; i < nmsgs; i = i + 1)
        	src = {getmsg {graphw} -incoming -source {0}}
        	deletemsg {graphw}  {0} -incoming
    	end
		foreach name ({el {cell}/../Tr#[]})
        	oldplot = ({substring {getfield {name} name} 2}) @ "[" @ ({getfield {name} index}) @ "]"
			col = {getfield {name} fg}
    		path = (cellpath) @ "/" @ (oldplot) @ "/" @ (fieldpath)
			if (({exists {path}}))
    			addmsg {path} {graphw} PLOT {colfield}  \
    			    *{oldplot} *{col}
			end
		end
        setfield {cell} colfield {colfield}
        //DHB setfield {cell} fatfield {fatfield}
        setfield {cell} fieldpath {fieldpath}
		setfield {graphw} YUnits {fieldpath}:{colfield}
    end

    set_cell_flag {cell} autofat {autofat}
    set_cell_flag {cell} autocol {autocol}
    //DHB set_cell_flag {cell} colfix {colfix}
    //DHB set_cell_flag {cell} fatfix {fatfix}

    if (autocol == 0)
        if (colmin != ({getfield {cell} colmin}))
            setfield {cell} colmin {colmin}
        end
        if (colmax != ({getfield {cell} colmax}))
            setfield {cell} colmax {colmax}
        end
    end
    if (autofat == 0)
        if (fatmin != ({getfield {cell} fatmin}))
            setfield {cell} fatmin {fatmin}
        end
        if (fatmax != ({getfield {cell} fatmax}))
            setfield {cell} fatmax {fatmax}
        end
    end
    if (fatrange != ({getfield {cell} fatrange}))
        setfield {cell} fatrange {fatrange}
    end


    if (hide)
        xhide {cellscale}
    end

    xupdate ..
    pope
end

function do_xout(num)
	str num

	str draww = (cellpath) @ "xout" @ (num) @ "/draw"
	int xoutwidth = (user_screenwidth - 380)/2
	if ({strcmp {num} "1"} == 0)
		if ((user_numxouts) == 1)
			create x1form {cellpath}xout1 [380,50,600,470]
		else
			create x1form {cellpath}xout1  \
			    [380,50,{xoutwidth},470]
		end
	else
		create x1form {cellpath}xout2  \
		    [{380 + xoutwidth},50,{xoutwidth},470]
	end
	pushe {cellpath}xout{num}
	disable .
	create x1label "CELL GEOMETRY" [1%,2,70%,30]
	create x1draw draw [1%,2,98%,2:parent.bottom] -transform ortho3d \
	    -wx {1.4e-3} -wy  {1.4e-3} \
	    -cz 0.2e-3 -vx 10 -vy -15 -vz 7 /* -transform ortho3d */
	//DHB setfield ^ wx {user_wx} wy {user_wy} cx {user_cx} cy {user_cy}  \
	    //DHB cz {user_cz}
	setfield ^ xmin {user_cx - user_wx/2} xmax {user_cx + user_wx/2} \
	    ymin {user_cy - user_wy/2} ymax {user_cy + user_wy/2}
	//setfield ^ transform o
	if ((user_symcomps))
		create x1cell draw/cell -path  \
		    {cellpath}/##[TYPE=symcompartment] /* -fatfield dia */  \
		    -colfield Vm -autocol FALSE -colmin -0.1 -colmax  \
		    0.05
	else
		create x1cell draw/cell -path  \
		    {cellpath}/##[TYPE=compartment] /* -fatfield dia */  \
		    -colfield Vm -autocol FALSE -colmin -0.1 -colmax  \
		    0.05
	end
	if ({strcmp {num} "1"} == 0)

	setfield {draww}/cell fatrange {user_fatrange1}  \
		    fieldpath {user_path1} colfield {user_field1}  \
		    colmax {user_colmax1} colmin {user_colmin1}  \
		    labelmode none rooticon none
    	// set_cell_flag {cellpath}xout1/draw/cell colfix {user_colfix1}
    	// set_cell_flag {cellpath}xout1/draw/cell fatfix {user_fatfix1}
	else
		setfield {draww}/cell fatrange {user_fatrange2}  \
		    fieldpath {user_path2} colfield {user_field2}  \
		    colmax {user_colmax2} colmin {user_colmin2}  \
		    labelmode none rooticon none
    	// set_cell_flag {cellpath}xout2/draw/cell colfix {user_colfix2}
    	// set_cell_flag {cellpath}xout2/draw/cell fatfix {user_fatfix2}
	end
   	scalecell {draww}"/cell"

   	setup_mouse {draww} 50 0 inject "do_inject "{num} unject  \
   	    "clear_inject "{num} dummy do_dummy
	setup_mouse {draww} 50 0 Vclamp "do_vclamp "{num} unclamp  \
	    "clear_vclamp "{num} dummy do_dummy
   	setup_mouse {draww} 50 0 add_plot "add_plot "{num} drop_plot  \
   	    "drop_plot "{num} dummy do_dummy
	setup_mouse {draww} 50 0 RandSyn "do_rand "{num} UnRand  \
	    "do_unrand "{num} dummy do_dummy
	setup_mouse {draww} 50 0 SynAct "do_act "{num} UnAct  \
	    "do_unact "{num} dummy do_dummy
	setup_mouse {draww} 50 0 SynSpike "do_spike "{num} UnSpike  \
	    "do_unspike "{num} dummy do_dummy
	useclock draw/cell 9
	//	draw_control draw 0 0
	pope
end
