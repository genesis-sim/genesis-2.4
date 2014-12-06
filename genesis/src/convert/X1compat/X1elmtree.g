function x1elmtreeSET(action, field, value)

    if (field == "linkmode")
	if (value == "stems")
	    setfield arrowmode none
	end
	if (value == "msgs")
	    setfield arrowmode msgs
	end
	if (value == "both")
	    setfield arrowmode msgs
	end

	return 0
    end

    return {x1highlightSET {action} {field} {value}}
end


create xtree tree

    addfield tree linkmode
    setfield tree linkmode stems arrowmode none treemode tree namemode none

    x1setuphighlight tree 0
    addaction tree SET x1elmtreeSET
    addaction tree CREATE x1pixCREATE

    // this gets remade when the x1elmtree is created

    delete tree/shape[0]

addobject x1elmtree tree
