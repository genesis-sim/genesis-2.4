function x1cellCREATE
    setfield diarange -40 autodia 2

    return {x1pixCREATE {argv}}
end

create xcell cell

    addfield cell ox -indirect . tx -description "alias for tx"
    addfield cell oy -indirect . ty -description "alias for ty"
    addfield cell oz -indirect . tz -description "alias for tz"

    addfield cell autofat -indirect . autodia -desc "alias for autodia"
    addfield cell fatmin -indirect . diamin -desc "alias for diamin"
    addfield cell fatmax -indirect . diamax -desc "alias for diamax"
    addfield cell fatrange -indirect . diarange -desc "alias for diarange"
    addfield cell colfield -indirect . field -desc "alias for field"

    setfield cell diarange -40 field x autodia 2

    addfield cell rootpath -description "rootpath no longer functions"
    addfield cell rooticon -description "rooticon no longer functions"
    addfield cell pixname -description "pixname no longer functions"

    addfield cell colfix -description "colfix no longer functions"
    addfield cell fatfix -description "fatfix no longer functions"
    addfield cell fatfield -description "fatfield no longer functions"

    setfield cell fatfix FALSE colfix FALSE fatfield calc_dia pixname cell

    setfield cell fieldpath .  // default to field relative to compt

    setfield cell script \
	"X1click.d1 B1DOWN <w>; X1click.d2 B2DOWN <w>; X1click.d3 B3DOWN <w>"

    x1setuphighlight cell 1
    addaction cell CREATE x1cellCREATE

addobject x1cell cell
