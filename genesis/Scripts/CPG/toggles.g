//genesis toggles.g

/* 
	functions defined in this script
=============================================================================
	FUNCTION NAME		ARGUMENTS
=============================================================================
	overlaytoggle		(widget)
        windowtoggle            (widget,path)
        donetoggle              (widget,path,type)
        maketoggle              (path,type)
        synapsetoggle           (widget,path,num)
=============================================================================
*/



// ==========================================================================
//                 toggles the overlaying of graph outputs 
// ==========================================================================
function overlaytoggle(widget)
    overlay / {getfield {widget} state}
end

// ==========================================================================
// toggles the display of graphics, settings, or connections  control panels 
// ==========================================================================
function windowtoggle(widget, path)
    if ({getfield {widget} state} == 1)
	xshow {path}
        setfield {path}/done state 1
    else
	xhide {path}
        setfield {path}/done state 0
    end
end

//===========================================================================
//                      done toggles for windows
//===========================================================================
function donetoggle(widget, togglepath, windowpath)
     if ({getfield {widget} state} == 0)
        xhide {windowpath}
        setfield {togglepath} state 0
     else
        xshow {windowpath}
        setfield {togglepath} state 1
     end
end


// ========================================================================
//   add a toggle for a cell in the main cell control panel
// ========================================================================
function maketoggle(path, togglepath, typepath)
    str temp1, temp2
    str path, togglepath, typepath

    create xtoggle {togglepath} -script  \
        "windowtoggle <widget> "{typepath}
    temp1 = {substring {path} 1 2}
    temp2 = (temp1) @ " VISIBLE"
    setfield {togglepath} onlabel {temp2}
    temp2 = (temp1) @ " HIDDEN"
    setfield {togglepath} offlabel {temp2}
    setfield {togglepath} state 0
end


//=========================================================================
//             toggle for synapse type -- excitatory or inhibitory
//=========================================================================
function synapsetoggle(widget, cellpath, inhsyn, doodle)
    str cellpath, inhsyn, doodle
    str temp = "/viewform/draw" @ (doodle)
    float inhweight = {getweight {cellpath}/spike {inhsyn}}  // in tools.g
    float excweight = {getweight {cellpath}/spike {inhsyn + 1}} // excsyn

    if (({excweight} != 0) || ({inhweight} != 0))
         if ({getfield {widget} state} == 0)
             setweight {cellpath}/spike {inhsyn +1} {inhweight}  // in tools.g 
             setweight {cellpath}/spike {inhsyn} 0.0
             setfield {temp} fg green
             xupdate /viewform/draw
         else
             setweight {cellpath}/spike {inhsyn} {excweight}
             setweight {cellpath}/spike {inhsyn +1} 0.0
             setfield {temp} fg orange
             xupdate /viewform/draw
         end
    end
end
