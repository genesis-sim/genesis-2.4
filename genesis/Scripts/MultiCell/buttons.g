//genesis
/* 
	functions defined in this script
=============================================================================
	FUNCTION NAME		ARGUMENTS
=============================================================================
	overlaytoggle		widget
        celltoggle              widget path
        connbutton              path field widget        
=============================================================================
*/

// ==============================================
// toggles the overlaying of graph outputs 
// ==============================================
function overlaytoggle(widget)
    overlay / {getfield {widget} state}
end

// ==============================================
// toggles the display of cell control panels 
// ==============================================
function celltoggle(widget, path)
    if ({getfield {widget} state} == 1)
	xshow {path}
    else
	xhide {path}
    end
end

// ==============================================
// sets the field of the specified connection
// ==============================================
function connbutton(path, field, widget)
    str path, field, widget

    setconn {path} {field} {getfield {widget} value}
    echo {path} {field} = {getfield {widget} value}
end

