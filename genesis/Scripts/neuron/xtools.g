//genesis
// xtools.g - for use with XODUS 2 widgets

/*
** user functions available in xtools
**
** function		arguments
** ======================================================
** xrange 		path xmin xmax
** yrange 		path ymin ymax
** graphclock		path clock
** overlay		path value
** overlaytoggle	widget // also in buttons.g
** showgraphics		path
** hidegraphics		path
** makeconfirm		path title yes_script no_script x y
** makepopup            path title x y w h [name script ...]
** addpopup             path name script [name script ...]
** setgraphscale	graph
** showgraphscale	graph
** makegraphscale	graph
** fieldbutton		path field widget
** connbutton		path field widget  // like fieldbutton for setconn
** quitbutton		widget
** cancelquit		widget
** setstepsize		widget
** stepbutton		widget
** formtoggle		widget form0 form1
** donebutton           form   // clicks all buttons in form except "DONE"
** makegif		path x y w h gif_file title
** textwindow		path x y textfile
**
*/

function xrange(name, xmin, xmax)
    if ({argc} < 3)
	echo usage: xrange name xmin xmax
	return
    end
    str name
    float xmin, xmax
    if ({isa xgraph {name}})
	setfield {name} xmin {xmin} xmax {xmax}
    else
	setfield {name}/##[ISA=xgraph] xmin {xmin} xmax {xmax}
    end
end

function yrange(name, ymin, ymax)
    if ({argc} < 3)
	echo usage: xrange name xmin xmax
	return
    end
    str name
    float ymin, ymax
    if ({isa xgraph {name}})
	setfield {name} ymin {ymin} ymax {ymax}
    else
	setfield {name}/##[ISA=xgraph] ymin {ymin} ymax {ymax}
    end
end

function graphclock(path, clock)
str path
int clock
str num
    useclock {path}/##[ISA=xgraph] {clock}
end

function overlay(path, value)
str path
    setfield {path}/##[ISA=xgraph] overlay {value}
    echo  "overlay " -n
    if (value == 0)
	echo OFF
    else
	echo ON
    end
end

// ==============================================
// toggles the overlaying of graph outputs 
// ==============================================
function overlaytoggle(widget)
    overlay / {getfield {widget} state}
end

function showgraphics(path)
    if ({argc} < 1)
	echo usage: showgraphics path
	return
    end
    str path
    str name
    foreach name ({el {path}/##[ISA=xform]})
	xshow {name}
    end
end

function hidegraphics(path)
    if ({argc} < 1)
	echo usage: showgraphics path
	return
    end
    str path
    str name
    foreach name ({el {path}/##[ISA=xform]})
	xhide {name}
    end
end

function makeconfirm(path, title, yes_script, no_script, x, y)
    if ({argc} < 6)
	echo usage: makeconfirm path title yes_script no_script x y
	return
    end
    str path, title, yes_script, no_script
    float x, y
    create xform {path}/confirm [{x},{y},200,80] -nolabel
    create xlabel {path}/confirm/label [0,0,200,30] -title {title}
    create xbutton {path}/confirm/yes [10,35,80,30] -script  \
        {yes_script}
    create xbutton {path}/confirm/no [100,35,80,30] -script {no_script}
    disable {path}/confirm
end

function makepopup(path, title, x, y, w, h)// name,script,name,script...

    if ({argc} < 6)
	echo usage: makepopup path title x y w h [name script ...]
	return
    end
    str path, title
    float x, y, w, h
    int i
    int firstitem = 7
    int nitems = ({argc} - firstitem + 1)/2

    create xform {path} [{x},{y},{w},{(h + 2)*(nitems + 2)}] -nolabel
    create xlabel {path}/label [0,0,{w},{h}] -title {title}
    create xbutton {path}/DONE [0,{h + 2},{w},{h}] -script  \
        "xhide "{path}
    for (i = firstitem; i < {argc}; i = i + 2)
	create xbutton {path}/item[{(i - firstitem)/2 + 1}]  \
	    [0,{(h + 2)*((i - firstitem)/2 + 2)},{w},{h}] -title  \
	    {argv {i}} -script {argv {i + 1}}
    end
    disable {path}
    xshow {path}
end

function addpopup(path)// name,script,name,script...

    if ({argc} < 3)
	echo usage: addpopup path name script [name script ...]
	return
    end
    str path, title
    float w, h
    float formh
    int i
    int firstitem = 2
    int nadds = ({argc} - firstitem + 1)/2
    int nitems

    // locate the last item in the popup
    nitems = 1
    while ({exists {path}/item[{nitems}]} == 1)
	nitems = nitems + 1
    end
    // get the width and height from the  popup label
    w = {getfield {path}/label width}
    h = {getfield {path}/label height}
    // resize the form
    formh = {getfield {path} height}
    setfield {path} height {formh + (h + 2)*nadds}
    for (i = firstitem; i < {argc}; i = i + 2)
	create xbutton {path}/item[{(i - firstitem)/2 + nitems}]  \
	    [0,{(h + 2)*((i - firstitem)/2 + nitems + 1)},{w},{h}]  \
	    -title {argv {i}} -script {argv {i + 1}}
    end
end

function setgraphscale(graph)
    str graph
    str form = graph @ "_scaleform"
    str xmin = {getfield {form}/xmin value}
    str xmax = {getfield {form}/xmax value}
    str ymin = {getfield {form}/ymin value}
    str ymax = {getfield {form}/ymax value}
    setfield {graph} xmin {xmin} xmax {xmax} ymin {ymin} ymax {ymax}
    xhide {form}
end

function showgraphscale(form)
    str form
    str x, y
    // find the parent form
    str parent = {el {form}/..}
    while (!{isa xform {parent}})
	parent = {el {parent}/..}
    end
    x = {getfield {parent} xgeom}
    y = {getfield {parent} ygeom}
    setfield {form} xgeom {x} ygeom {y}
    xshow {form}
end

function makegraphscale(graph)
    if ({argc} < 1)
	echo usage: makegraphscale graph
	return
    end
    str graph
    str graphName = {getpath {graph} -tail}
    float x, y
    str form = graph @ "_scaleform"
    str parent = {el {graph}/..}
    while (!{isa xform {parent}})
	parent = {el {parent}/..}
    end

    x = {getfield {graph} x}
    y = {getfield {graph} y}

    create xbutton {graph}_scalebutton  \
        [{getfield {graph} xgeom},{getfield {graph} ygeom},50,25] -title scale -script  \
        "showgraphscale "{form}
    create xform {form} [{x},{y},180,170] -nolabel

    disable {form}
    pushe {form}
    create xbutton DONE [10,5,55,25] -script "setgraphscale "{graph}
    create xbutton CANCEL [70,5,55,25] -script "xhide "{form}
    create xdialog xmin [10,35,160,25] -value {getfield {graph} xmin}
    create xdialog xmax [10,65,160,25] -value {getfield {graph} xmax}
    create xdialog ymin [10,95,160,25] -value {getfield {graph} ymin}
    create xdialog ymax [10,125,160,25] -value {getfield {graph} ymax}
    pope
end

// ==============================================
// sets the field of the specified element
// ==============================================
function fieldbutton(path, field, widget)
    str path, field, widget
    setfield {path} {field} {getfield {widget} value}
    echo {path} {field} = {getfield {widget} value}
end

// ==============================================
// sets the field of the specified connection
// ==============================================
function connbutton(path, field, widget)
    str path, field, widget
    setconn {path} {field} {getfield {widget} value}
    echo {path} {field} = {getfield {widget} value}
end

function quitbutton(widget)
    str widget
    // display the confirmation widget
    xshowontop {widget}/confirm
end

function cancelquit(widget)
    str widget
    // hide the confirmation widget
    xhide {widget}/..
end

// ==============================================
// sets the global simulation clock
// ==============================================
function setstepsize(widget)
    str widget
    float dt = ({getfield {widget} value})
    setclock 0 {dt}
    echo dt = {dt}
    adjustspike
end

// ==============================================
// advances the simulation by the amount of time given in the dialog
// ==============================================
function stepbutton(widget)
    str widget
    echo step {getfield {widget} value} " -time"
    step {getfield {widget} value} -time
end

// ==============================================
//       toggles the display of two forms
// ==============================================
function formtoggle(widget, form0, form1)
    if ({getfield {widget} state} == 1)
	xshow {form1}
	xhide {form0}
    else
        xshow {form0}
	xhide {form1}
    end
end
//==================================================================
//    Clicks all buttons in form except "DONE"
//    (used with a menu containing dialog boxes to send all values)
//==================================================================
function donebutton(form)
    str form, button
    foreach button ({el {form}/#})    // returns full paths

        if (({getpath {button} -tail}) != "DONE")   // looks at tail of each

           xsendevent {button} click1
        end
    end
    xhide {form}
end

//=====================================================================
// makegif(formpath,x,y,image_width,image_height,gif_file,title)
// Creates a form to display a GIF file with label and "DONE" button
// The form will have width=image_width+10 and height=image_height+25
//=====================================================================
function makegif(formpath, x, y, image_width, image_height, gif_file,  \
    title)
   str formpath, gif_file, title
   int x, y, image_width, image_height
   create xform {formpath}  \
       [{x},{y},{image_width + 25},{image_height + 40}] -nolabel
   create xlabel {formpath}"/titlebar" [5,0,{image_width},20] -title  \
       {title}
   create xbutton {formpath}"/DONE" [15,0,80,25] -script  \
       "xhide "{formpath}
   create ximage {formpath}"/image" [5,20,{image_width},{image_height}]
   setfield ^ wgeom {image_width} hgeom {image_height} filename {gif_file}
end

/*
The function "textwindow" creates a form 590 pixels wide and 336 pixels high
at x,y to display the specified text file with scrolling and paging.  Using
the default font (7x13bold), this provides room for 24 lines with 78
characters per line and a one character right and left margin.  (The left
margin is provided automatically.)
*/

//=====================================================================
// textwindow formpath,x,y,textfile
// creates a form 590 pixels wide and 336 pixels high at x,y to display
// the specified text file with scrolling and paging
//=====================================================================
function textwindow(formpath, x, y, textfile)
   str formpath, textfile
   int x, y, width, height
   height = 336
   width = 590
   create xform {formpath} [{x},{y},{width},{height}] -nolabel
   pushe {formpath}
   create xlabel textbar [40,0,{100*(width - 40)/width}%,20] -title  \
       "Use the scroll bar at the left to move through the text"
   create xbutton DONE [0,0,40,20] -script "xhide "{formpath}
   create xtext text [0%,20,100%,94%] -filename {textfile} -bg white
   pope
end
