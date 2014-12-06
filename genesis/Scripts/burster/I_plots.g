//genesis - I_plots.g -- functions to make plots of activation parameters

function make_Igraphs
    create xform /output/Iform [385,0,765,550] -nolabel
    pushe /output/Iform
    create xbutton hide [0,5,20%,25] -title  \
        "Hide Plots" -script "xhide /output/Iform"
    create xlabel label [0:hide.right,5,60%,25] -title "Channel Currents"
    create xbutton delete [0:label.right,5,20%,25] -title  \
        "Delete Plots" -script delete_plots
    create xgraph Na [0,5:label.bottom,50%,160] -title "Na current"
    setfield ^ XUnits Sec xmax {user_xmax1} ymax 5e-7 YUnits Amperes
    create xgraph Ca [50%,5:label.bottom,50%,160] -title "Ca current"
    setfield ^ XUnits Sec xmax {user_xmax1} ymax 5e-7 YUnits Amperes
    create xgraph K [0,5:Na.bottom,50%,160] -title "K current"
    setfield ^ XUnits Sec xmax {user_xmax1} ymin -5e-7 ymax 0  \
        YUnits Amperes
    create xgraph B [50%,5:Na.bottom,50%,160] -title "B current"
    setfield ^ XUnits Sec xmax {user_xmax1} ymax 2e-8 YUnits Amperes
    create xgraph A [0,5:K.bottom,50%,160] -title "A current"
    setfield ^ XUnits Sec xmax {user_xmax1} ymin -5e-8 ymax 0  \
        YUnits Amperes
    create xgraph C [50%,5:K.bottom,50%,160] -title "C current"
    setfield ^ XUnits Sec xmax {user_xmax1} ymin -2e-8 ymax 0  \
        YUnits Amperes
    pope
end

function send_Imsgs(cellpath)
    str cellpath
    str compt = (cellpath) @ "/soma"
    pushe /output/Iform
    addmsg {compt}/Na Na PLOT Ik *Ik *blue
    addmsg {compt}/K K PLOT Ik *Ik *blue
    addmsg {compt}/A A PLOT Ik *Ik *blue
    if ({exists {compt}/Ca})
        addmsg {compt}/Ca Ca PLOT Ik *Ik *blue
    end    // this could be done for all with a foreach loop over an EL

    addmsg {compt}/B B PLOT Ik *Ik *blue
    addmsg {compt}/K_C C PLOT Ik *Ik *blue
    pope
end

function do_paradigm
    if ({exists /output/Iform})
        setfield /output/Iform/#[ISA=xgraph]  \
            xmin {getfield /molluscgraph1/graph xmin}  \
            xmax {getfield /molluscgraph1/graph xmax}
	xshowontop /output/Iform
    else
	if (({cellpath}) != "/new")	// if cell has been loaded

	    make_Igraphs
	    send_Imsgs {cellpath}
	    xshowontop /output/Iform
            reset
	end
    end
end

function delete_plots
    // it core dumps if plots deleted while running
    stop
    sleep 1
    delete /output/Iform
end
