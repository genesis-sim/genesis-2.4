function x1graphCREATE

    str form

    form = {el ..}

    while (form != "/" && !{isa xform {form}})
	form = {el {form}/..}
    end

    if (form == "/")
	echo x1graphCREATE: unable to find form for element -nonewline
	pwe
    else
	setfield form {form}
    end

    return 1
end

create xgraph graph

    // these will get remade when the x1graph is created

    delete graph/x_axis
    delete graph/y_axis
    delete graph/title

    // these are field name aliases for widget width and height.  This
    // isn't a complete solution since the old width and height do not
    // include widget and edge references while wgeom and hgeom do.  What
    // we do here will work so long as the caller doesn't need to preserve
    // widget and edge refs.

    addfield graph width -indirect . wgeom
    addfield graph height -indirect . hgeom

    addfield graph form
    setfieldprot graph -readonly form 
    addaction graph CREATE x1graphCREATE

addobject x1graph graph
