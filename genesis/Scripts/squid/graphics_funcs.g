//genesis


// GENERAL PURPOSE GRAPHICS FUNCTIONS:

// The following functions affect all graphs in the path subtree:
//    cleargraph(path), graphclock(path,clock), overlaygraph(path) 

// The follow functions affect all forms in the path subtree:
//    hidegraphics(path), showgraphics(path)

// The follow function affects all forms:
//    hideall


function cleargraph(path)
str path
str name
int oldoverlay
    foreach name ({el {path}/##[TYPE=xgraph]})
	pushe {name}
	oldoverlay = {getfield . overlay}
	setfield overlay 0 
	call . RESET
	setfield overlay {oldoverlay} 
	pope
    end
end


function graphclock(path, clock)
str path
int clock
str name
    foreach name ({el {path}/##[TYPE=xgraph]})
	useclock {name} {clock}
    end
end

function overlaygraph(path)
str path
str name
    foreach name ({el {path}/##[TYPE=xgraph]})
	setfield {name} overlay $1
    end
end

function hidegraphics(path)
str path
str name
    foreach name ({el {path}/##[TYPE=xform]})
	xhide {name}
    end
end

function showgraphics(path)
str path
str name
    foreach name ({el {path}/##[TYPE=xform]})
	xshow {name}
    end
end

function hideall
str name
    foreach name ({el /##[TYPE=xform]})
	xhide {name}
    end
end
