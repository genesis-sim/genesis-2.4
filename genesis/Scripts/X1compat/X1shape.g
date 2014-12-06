function x1shapeSET(action, field, value)

    if (field == "transfmode")
	int value

	if (value)
	    setfield pixflags pc
	else
	    setfield pixflags ~pc
	end
	return 0
    else
	if (field == "iconname")
	    str iconname = {getpath {el .} -head} @ {value}
	    if ({isa xshape {iconname}})
		setfield &&xpts &{iconname}/xpts \
		         &&ypts &{iconname}/ypts \
		         &&zpts &{iconname}/zpts \
		         npts {getfield {iconname} npts}
	    else
		echo Bad iconname {value}
	    end
	    return 0
	end
    end

    return {x1highlightSET {action} {field} {value}}
end


//
// this is a version of x1pixCREATE which avoids handling -coords
// options to x1shapes.  The -coords option should be handled by
// the xshape CREATE action anyways.
//

function x1shapeCREATE(action, createargs)

    int	arg

    for (arg = 5; arg < {argc}; arg = arg+1)
	str argname = {argv {arg}}

	if ({substring x{argname} 0 1} == "x-" && ("x"@{argname}) != "x-coords")
	    str argval = {argv {arg+1}}

	    argname = {strsub x{argname} x- ""}
	    setfield {argname} {argval}
	    arg = arg + 1
	end

    end

    return 1
end


function printcareful(args)

    str arg

    foreach arg ({argv})
	echo " "{arg} -nonewline
    end
    echo ""

end


create xshape shape

    addfield shape iconname -description "Name of icon to display"
    addfield shape transfmode -description "non-zero means use pixel coords"

    setfield shape transfmode 0
    setfield shape script \
	"X1click.d1 B1DOWN <w> ; X1click.d2 B2DOWN <w>; X1click.d3 B3DOWN <w>"

    x1setuphighlight shape 0
    addaction shape SET x1shapeSET
    addaction shape CREATE x1shapeCREATE
    addclass shape output

addobject x1shape shape
