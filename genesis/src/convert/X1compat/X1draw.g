//
// Xodus 1 draws and pixes had a different pix selection mechanism from
// Xodus 2.  Here we create this functionality to make porting to Xodus 2
// easier.
//

// Here are some functions used as actions for button clicks on pixes.

function X1click(action, widget)

    // call . {action} -parent	// call the builtin click action

    pushe {widget}
    setfield .. pixchoose {getpath {el .} -tail}
    setfield .. value {getfield value}
    call .. {action}	// call the click action in the draw
    pope

    return 1

end

function x1draw_updatepixselection
    str child
    str sel_value

    sel_value = {getfield . value}
    foreach child ({el ./##[CLASS=gadget]})
	if ({exists {child} value})
	    if (sel_value == {getfield {child} value})
		setfield {child} pixflags ~is_sel pixflags is_sel
	    else
		setfield {child} pixflags is_sel pixflags ~is_sel
	    end
	end
    end
end


function x1drawSET(action, field, value)
    /*
    if (field == "refresh_flag")
	str	char

	char = {substring {value} 0 0}
	if (char == "1" || char == "T")
	    setfield ./##[CLASS=gadget] pixflags
	else
	end
    end
    */

    if (field == "value")
	setfield value {value}
	x1draw_updatepixselection
	return 1
    end

    return 0
end


// These functions set up emulation of the X1 highlighting

function x1highlightSET(action, field, value)

    if (field == "hldispmode")
	if (value == "invert")
	    setfield pixflags ~hlt1 pixflags ~hlt2
	else // This always does the old star option
	    setfield pixflags hlt1 pixflags hlt2
	end
    end

    if (field == "hlhistmode")
	if (value == "none")
	    setfield pixflags clickable_not // not clickable
	else
	    setfield pixflags ~clickable_not // clickable
	end
    end

    if (field == "value")
	setfield value {value}
	call .. UPDATEPIXSELECTION
	return 1
    end

    return 0
end

function x1setuphighlight(pix, doSetAction)

	int doSetAction

	addfield {pix} hldispmode
	setfield {pix} hldispmode invert

	addfield {pix} hlhistmode
	setfield {pix} hlhistmode lastone

	if (doSetAction)
	    addaction {pix} SET x1highlightSET
	end

end


// Here's a generic CREATE action to make sure all CREATE options get handled

function x1pixCREATE(action, createargs)

    int	arg

    for (arg = 5; arg < {argc}; arg = arg+1)
	str argname = {argv {arg}}

	if ({substring x{argname} 0 1} == "x-")
	    str argval = {argv {arg+1}}

	    argname = {strsub x{argname} x- ""}
	    setfield {argname} {argval}
	    arg = arg + 1
	end

    end

    return 1
end


create xdraw xdrawcompat
addfield xdrawcompat value -description "Pix value set on button click"
addfield xdrawcompat pixchoose -description "Path of last pix clicked"
addfield xdrawcompat refresh_flag \
	    -description "FALSE inhibits redraws during heavy pix updates"

    // these are field name aliases for widget width and height.  This
    // isn't a complete solution since the old width and height do not
    // include widget and edge references while wgeom and hgeom do.  What
    // we do here will work so long as the caller doesn't need to preserve
    // widget and edge refs.

    addfield xdrawcompat width -indirect . wgeom
    addfield xdrawcompat height -indirect . hgeom

setfield xdrawcompat refresh_flag 1
setfield xdrawcompat wx 1000 wy 1000 cx 0 cy 0 cz 0 vx 4 vy -20 vz 3
setfield xdrawcompat value / pixchoose pix

addaction xdrawcompat UPDATEPIXSELECTION x1draw_updatepixselection
addaction xdrawcompat SET x1drawSET

pushe xdrawcompat > /dev/null
    include X1shape
    include X1cell
    include X1view
pope > /dev/null

addobject x1draw xdrawcompat
