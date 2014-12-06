//genesis graphs.g

/* 
	functions defined in this script
=============================================================================
	FUNCTION NAME		ARGUMENTS
=============================================================================
        makeallgraph
 	makegraph		(path)
=============================================================================
*/


// =====================================================================
//           create a graph which displays voltage of all cells
// =====================================================================
function makeallgraph
    str path1 = "/L1"
    str path2 = "/L2"
    str path3 = "/R1"
    str path4 = "/R2"
    str graphpath = "/multi_cell_graph"

    //==================================================
    //              create graph form
    //==================================================
    create xform {graphpath} [489,421,340,220] -nolabel
    pushe {graphpath}

    //==================================================
    //  make graph for intracellular potentials at soma
    //==================================================
    create xgraph graph [3%,5%,94%,90%] -title  \
        "      scaled intracellular potentials Vm"
    addmsg {path1}/soma graph PLOTSCALE Vm *{path1} *blue .25 50
    addmsg {path2}/soma graph PLOTSCALE Vm *{path2} *black .25 20
    addmsg {path3}/soma graph PLOTSCALE Vm *{path3} *red .25 -10
    addmsg {path4}/soma graph PLOTSCALE Vm *{path4} *purple .25 -40

    setfield graph xmin 0 xmax 200 ymin -70 ymax 70
    useclock graph 1
    makegraphscale {graphpath}/graph

    //=================================================
    //                display graph
    //=================================================
    xshow {graphpath}
    pope
end


// ==========================================================================
//                 create form and graphs for display of voltages
// ==========================================================================
function makegraph(path, flag)

    // path gives the location of the cell
    // graphicspath gives the location of the control form for the cell
      str path
      str graphicspath = (path) @ "/graphics"
      str togglepath = "/output/graphics" @ (path)

    // add a toggle for this cell in the main cell control panel
    maketoggle {path} {togglepath} {graphicspath}

    // =================================================
    //               create graphics form
    // =================================================
    create xform {graphicspath} [489,152,340,265] -title {graphicspath}

    // =================================================
    //      make graph for cell intracellular potential
    // =================================================
    pushe {graphicspath}
    create xgraph graph [2%,20,96%,80%] -title  \
        "   Vm and injection currents"
    addmsg {path}/soma graph PLOT Vm *soma *blue
    addmsg {path}/soma/injectpulse/injcurr graph PLOTSCALE output  \
        *soma_inj *red 100000 0
    addmsg {path}/soma/dend/injectpulse/injcurr graph PLOTSCALE output  \
        *dend_inj *green 100000 0
    setfield graph xmin 0 xmax 200 ymin -90 ymax 50
    useclock graph 1
    makegraphscale {graphicspath}/graph
    create xtoggle {graphicspath}/done  \
        [2%,2:graph.bottom,96%,12%] -script  \
        "donetoggle <widget> "{togglepath}" "{graphicspath}
    setfield {graphicspath}/done label1 "VISIBLE"
    setfield {graphicspath}/done label0 "HIDDEN"
    pope
end
