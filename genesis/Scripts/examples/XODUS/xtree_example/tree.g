//genesis
//
// This example demonstrates the use of the xtree widget.
// In this case it is set up so that drag-drop operations
// cause elements to be moved around in the element
// hierarchy.
// Double click as usual causes truncation of the tree.

int side = 10

// Here is the xodus startup stuff
xcolorscale rainbow2
create xform /form [0,0,1100,500]
xshow /form
create xdraw /form/draw [0,0,100%,100%]
	setfield /form/draw xmin -2 xmax 20 ymin -1 ymax 5
create xtree /form/draw/tree

// Set up icons for tree
include icons
// set up some things for the tree to display
include manytabs
include cell



// Set tree parameters
setfield /form/draw/tree \
	path /##[] \
	treemode tree \
	sizescale 0.4 \
	script "echo on item <v>"

// This is the crucial step where I tell the xtree to move 
// elements around on drag-drop operations. The reset is
// necessary to tell the xtree to redraw
call /form/draw/tree ADDMSGARROW all all all all green 0 0 \
	"move.p <S> <D>; reset.p" "" ""
call /form/draw/tree SHOWMSGARROW

reset
