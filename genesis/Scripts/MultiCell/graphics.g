//genesis

/* 
	functions defined in this script
=============================================================================
	FUNCTION NAME		ARGUMENTS
=============================================================================
 	loadgraphics
 	conductance_form	(graphicspath,cellpath,channelname,x,y)
 	connection_form		(graphicspath,path,num,x,y)
 	injection_form		(graphicspath,cellpath,name,x,y)
 	makegraphics		(path)
=============================================================================
*/

include x1tools.g 
include buttons.g 

function loadgraphics
    // activate XODUS
    
    // give it color
    xcolorscale hot
    // =================================================
    //                MAIN CONTROL PANEL
    // =================================================
    create x1form /output/main_control [10,0,500,120] -title  \
        "MultiCell - A simple neuronal network simulation by M.Wilson 6/88"
    pushe /output/main_control
    create x1button reset [10,35,80,30] -script reset -title RESET
    create x1toggle overlay [92,35,80,30] -title "overlay" -script  \
        "overlaytoggle <widget>"
    create x1button stop [174,35,80,30] -title STOP -script stop
    create x1button quit [330,35,80,30] -title QUIT -script  \
        "quitbutton <widget>"
    makeconfirm quit "Quit MultiCell?" "quit" "cancelquit <widget>" 500  \
        10
    create x1dialog step [10,67,200,30] -script "stepbutton <widget>"  \
        -title "STEP (msec)" -value 500
    create x1dialog stepsize [210,67,200,30] -title "dt (msec)" -script  \
        "setstepsize <widget>" -value {getclock 0}
    pope
    xshow /output/main_control
    // =================================================
    //                CELL CONTROL PANEL
    // =================================================
    create x1form /output/cells [515,0,100,70] -nolabel
    xshow /output/cells
end

function conductance_form(graphicspath, cellpath, channelname, x, y)
str graphicspath
str cellpath
str channelname
float x, y
str target = (cellpath) @ "/" @ (channelname)

    create x1label {graphicspath}/{channelname} [{x},{y},160,30] -title  \
        {channelname}
    pushe {graphicspath}/{channelname}
    create x1dialog E [{x},{y + 35},160,30] -script  \
        "fieldbutton "{target}" Ek <widget>" -title "Eequil (mV)" -value \
         {getfield {target} Ek}
    create x1dialog gmax [{x},{y + 70},160,30] -script  \
        "fieldbutton "{target}" gmax <widget>" -title "Gm (mS/syn)"  \
        -value {getfield {target} gmax}
    create x1dialog tau1 [{x},{y + 105},160,30] -script  \
        "fieldbutton "{target}" tau1 <widget>" -title "Tau1 (msec)"  \
        -value {getfield {target} tau1}
    create x1dialog tau2 [{x},{y + 140},160,30] -script  \
        "fieldbutton "{target}" tau2 <widget>" -title "Tau2 (msec)"  \
        -value {getfield {target} tau2}
    pope
end

function connection_form(graphicspath, path, num, x, y)
str graphicspath, axonname, num
float x, y
str graphics = graphicspath @ "/conn" @ num
str connection = path @ ":" @ num

    create x1label {graphics} [{x},{y},160,30] -title  \
        "to "{getconn {connection} target}
    pushe {graphics}
    create x1dialog weight [{x},{y + 35},160,30] -script  \
        "connbutton "{connection}" weight <widget>" -title  \
        "weight (syn)" -value {getconn {connection} weight}
    create x1dialog delay [{x},{y + 70},160,30] -script  \
        "connbutton "{connection}" delay <widget>" -title  \
        "delay (msec)" -value {getconn {connection} delay}
    pope
end

function injection_form(graphicspath, cellpath, name, x, y)
    str graphicspath, cellpath, name
    float value

    create x1dialog {graphicspath}/{name}inject [{x},{y},160,30] -title  \
        "I "{name}" (uA)" -script  \
        "fieldbutton "{cellpath}" inject <widget>" -value  \
        {getfield {cellpath} inject}
end

function makegraphics(path)
    if ({argc} < 1)
	echo usage: makegraphics path [x y]
	return
    end
    if ({argc} > 1)
	// optionally read in the form position
	int fx = $2
	int fy = $3
    else
	int fx = 10
	int fy = 120
    end
    // path gives the location of the cell
    // graphics path gives the location of the control form for the cell
    str path
    str graphicspath = (path) @ "/graphics"

    // add a toggle for this cell in the main cell control panel
    create x1toggle /output/cells/{getpath {path} -tail} -script  \
        "celltoggle <widget> "{graphicspath}

    // =================================================
    //                    main form
    // =================================================
    create x1form {graphicspath} [{fx},{fy},565,775] -title  \
        {graphicspath}
    pushe {graphicspath}

    // =================================================
    //      make graph1 for intracellular potentials
    // =================================================
    create x1graph graph1 [5,50,400,350] -title  \
        "intracellular potential Vm (mV)"
    addmsg {path}/soma/dend graph1 PLOT Vm *dend *red
    addmsg {path}/soma graph1 PLOT Vm *soma *black
    setfield graph1 yoffset 150
    setfield graph1 xmin 0 xmax 500 ymin -90 ymax 200
    useclock graph1 1
    makegraphscale {graphicspath}/graph1

    // =================================================
    //       make graph2 for synaptic conductances
    // =================================================
    create x1graph graph2 [5,410,400,350] -title "GNa and GK (mS)"
    addmsg {path}/soma/dend/Na_channel graph2 PLOT Gk *Na *black
    addmsg {path}/soma/dend/K_channel graph2 PLOT Gk *K *red
    setfield graph2 yoffset 0
    setfield graph2 xmin 0 xmax 500 ymin -0.1e-5 ymax 5e-5
    useclock graph2 1
    makegraphscale {graphicspath}/graph2

    // =================================================
    //                 current injection 
    // =================================================
    create x1label {graphicspath}/injectlabel [410,50,160,30] -title  \
        "current injection"
    injection_form {graphicspath} {path}/soma/dend dend 410 85
    injection_form {graphicspath} {path}/soma soma 410 120

    // =================================================
    //               conductance parameters 
    // =================================================
    conductance_form {graphicspath} {path}/soma/dend Na_channel 410 400
    conductance_form {graphicspath} {path}/soma/dend K_channel 410 600

    // =================================================
    //               connection parameters 
    // =================================================
    connection_form {graphicspath} {path}/axon 0 410 180

    /* display the widgets and prepare to start */
    xshow {graphicspath}
    pope
end
