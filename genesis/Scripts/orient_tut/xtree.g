//genesis

create xform /tree [300,0,800,500]

// Creating the draw for the element tree.

create xdraw /tree/draw [0,0,100%,93%]
setfield /tree/draw xmin -2 xmax 20 ymin -1 ymax 5

// Creating an xtree to help us move around the simulation.

create xtree /tree/draw/tree 

create xtoggle /tree/show_element [0,93%,100%,7%]
 
// icons.g defines the icons for the different element types.
// It is included here since it requires /tree/draw/tree.

include icons.g 

// Set tree parameters.

setfield /tree/draw/tree \
        path /##[]       \
        treemode tree    \
        sizescale 0.4    \
        script "change_to_element.d1 <v>; zoom_tree.d2 <w> <v>; truncate.d3 <v>"

echo xtree created
reset

function setup_tree
    call /tree/draw/tree TRUNCATE /output
    call /tree/draw/tree TRUNCATE /tree
    call /tree/draw/tree TRUNCATE /xout
// call /tree/draw/tree TRUNCATE /conn
    call /tree/draw/tree TRUNCATE /library
    call /tree/draw/tree TRUNCATE /control
    call /tree/draw/tree TRUNCATE /xgraphs
end

setup_tree

function reset_tree
    call /tree/draw/tree RESET
end

// This function selects the element as the "working element"
// and does a showfield on the element if the toggle is down.

function change_to_element(item)
    str item
    int show_state

    echo changing to element {item}
    ce {item}
    show_state = {getfield /tree/show_element state}
    if (show_state)
        showfield -all
    end
end

// This function makes the selected element the base of the tree.

function zoom_tree(widget, item)
    str widget, item

    if ({strcmp {getfield {widget} path} "/##[]"} != 0)
        echo zooming tree out to /
        setfield {widget} path /##[]
	reset_tree
	setup_tree
    else
        echo zooming in to {item}
        setfield {widget} path {item}/../##  // this isn't quite right
    end

    reset_tree
end



// This function crunches the selected element and its children into a tree object.
// This function is no longer strictly necessary.

function truncate(item)
    str item
    call /tree/draw/tree TRUNCATE {item}
    reset_tree
end

disable /tree
