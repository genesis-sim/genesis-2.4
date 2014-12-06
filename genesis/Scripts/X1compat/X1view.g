function x1viewSET(action, field, value)

    if (field == "pixcolor")
	setfield shape[1] fg {value}
	return 0 // allow setfield to set shape[0] fg via indirect field
    end

    if (field == "viewicon")
	str iconname = {getpath {el .} -head} @ {value}
	if ({isa xshape {iconname}})
	    setfield shape[0] &&xpts &{iconname}/xpts \
		     &&ypts &{iconname}/ypts \
		     &&zpts &{iconname}/zpts \
		     npts {getfield {iconname} npts}
	    setfield shape[1] &&xpts &{iconname}/xpts \
		     &&ypts &{iconname}/ypts \
		     &&zpts &{iconname}/zpts \
		     npts {getfield {iconname} npts}
	else
	    echo x1viewSET: Bad iconname {value}
	end
	return 0 // allow setfield to set viewicon field
    end

    return {x1highlightSET {action} {field} {value}}
end


create xview view

    setfield view script \
	"X1click.d1 B1DOWN <w>; X1click.d2 B2DOWN <w>; X1click.d3 B3DOWN <w>"

    addfield view ox -indirect . tx -desc "alias for tx"
    addfield view oy -indirect . ty -desc "alias for ty"
    addfield view oz -indirect . tz -desc "alias for tz"

    addfield view vmin -indirect . value_min[0] -desc "alias for value_min[0]"
    addfield view vmax -indirect . value_max[0] -desc "alias for value_max[0]"

    addfield view viewicon
    addfield view pixcolor -indirect shape[0] fg \
					-desc "alias for shape[0-1] fg fields"

    str fld
    foreach fld (nx ny autospace)
	addfield view {fld} -description "nonfunctional Xodus 1 field"
	setfield view {fld} 1
    end

    x1setuphighlight view 0
    addaction view SET x1viewSET
    addaction view CREATE x1pixCREATE

    // these will get remade when the x1view is created

    delete view/shape[0]
    delete view/shape[1]

addobject x1view view
