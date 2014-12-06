//genesis
// xtools.g - revised 8/10/91

int XTOOLSDEBUG  // define as non-zero prior to including xtools to
		 // get debug output

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
** showgraphscale	graph // orig had y offset of -155
** makegraphscale	graph
** fieldbutton		path field widget
** quitbutton		widget
** cancelquit		widget
** setstepsize		widget
** stepbutton		widget
** formtoggle		widget form0 form1
** \
     donebutton           form   // clicks all buttons in form except "DONE"
** makegif		path x y w h gif_file title
** textwindow		path x y textfile [XREV]
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
    echo -n "overlay "
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
    create x1form {path}/confirm [{x},{y},200,80] -nolabel
    create x1label {path}/confirm/label [0,0,200,30] -title {title}
    create x1button {path}/confirm/yes [10,40,80,30] -script  \
        {yes_script}
    create x1button {path}/confirm/no [100,40,80,30] -script {no_script}
    disable {path}/confirm
end

function makeconfirm(path, title, yes_script, no_script, x, y)
    if ({argc} < 6)
	echo usage: makeconfirm path title yes_script no_script x y
	return
    end
    str path, title, yes_script, no_script
    float x, y
    create x1form {path}/confirm [{x},{y},200,80] -nolabel
    create x1label {path}/confirm/label [0,0,200,30] -title {title}
    create x1button {path}/confirm/yes [10,40,80,30] -script  \
        {yes_script}
    create x1button {path}/confirm/no [100,40,80,30] -script {no_script}
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

    create x1form {path} [{x},{y},{w},{(h + 2)*(nitems + 2)}] -nolabel
    create x1label {path}/label [0,0,{w},{h}] -title {title}
    create x1button {path}/DONE [0,{h + 2},{w},{h}] -script  \
        "xhide "{path}
    for (i = firstitem; i < {argc}; i = i + 2)
	create x1button {path}/item[{(i - firstitem)/2 + 1}]  \
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
	create x1button {path}/item[{(i - firstitem)/2 + nitems}]  \
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
if (XTOOLSDEBUG)
    echo showgraphscale begin
end
    str form
    str x, y
    // find the parent form
if (XTOOLSDEBUG)
    echo " finding parent form"
end
    str parent = {el {form}/..}
    while (!{isa xform {parent}})
	parent = {el {parent}/..}
    end
if (XTOOLSDEBUG)
    echo " getting parent geometry"
end
    x = {getfield {parent} xgeom}
    y = {getfield {parent} ygeom}
if (XTOOLSDEBUG)
    echo " setting graphscale geometry"
end
    setfield {form} xgeom {x} ygeom {y}
if (XTOOLSDEBUG)
    echo " showing graphscale"
end
    xshow {form}
if (XTOOLSDEBUG)
    echo showgraphscale end
end
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

    create x1button {graph}_scalebutton  \
        [{getfield {graph} xgeom},{getfield {graph} ygeom},50,25] -title scale -script  \
        "showgraphscale "{form}
    create x1form {form} [{x},{y},180,170] -nolabel

    disable {form}
    pushe {form}
    create x1button DONE [10,10,50,15] -script "setgraphscale "{graph}
    create x1button CANCEL [70,10,50,15] -script "xhide "{form}
    create x1dialog xmin [10,35,160,25] -value {getfield {graph} xmin}
    create x1dialog xmax [10,65,160,25] -value {getfield {graph} xmax}
    create x1dialog ymin [10,95,160,25] -value {getfield {graph} ymin}
    create x1dialog ymax [10,125,160,25] -value {getfield {graph} ymax}
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
    echo step -t {getfield {widget} value}
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

        if (({getpath {button} -tail}) != "DONE")        // looks at tail of each

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
   create x1form {formpath}  \
       [{x},{y},{image_width + 10},{image_height + 25}] -nolabel
   create x1label {formpath}"/titlebar" [5,0,{image_width},20] -title  \
       {title}
   create x1button {formpath}"/DONE" [15,3,80,15] -script  \
       "xhide "{formpath}
   create x1image {formpath}"/image" [5,20,{image_width},{image_height}]
   setfield ^ wx {image_width} wy {image_height} filename {gif_file}
end

str XREV
//XREV = "R3"	
// global variable for setting width of text display from X11 revision #
// "R4" will be assumed if it is not explicitly set to "R3"

/*
The function "textwindow" creates a form 580 pixels wide and 336 pixels high
at x,y to display the specified text file with scrolling and paging.  Using
the default font for X11R4, this provides room for 24 lines with 78
characters per line and a one character right and left margin.  (The left
margin is provided automatically.)  X11R3 uses a smaller font for both the
xtext widget and the xlabel widget, however.  In addition, it does not
usually provide a scroll bar with the xtext widget.  Thus, in order to
accomodate the same number of characters, a form width of 480 pixels can be
used.  "textwindow" uses an optional argument "XREV" to determine the proper
width and also allows the size of the text window to be resized along with
the parent form.  If this argument is not specified, the larger width
appropriate to X11R4 is used.  When displaying X11R4 windows on a DECstation
with its large fonts, an even wider width of 810 should be used.  This can
either be changed below, or the window may be rescaled after it appears.
*/

//=====================================================================
// textwindow(formpath,x,y,textfile,[XREV])
// creates a form 580 pixels wide and 336 pixels high at x,y to display
// the specified text file with scrolling and paging
//=====================================================================
function textwindow(formpath, x, y, textfile)
   str formpath, textfile, XREV
   int x, y, width, height
   height = 336
   XREV = "R4"
   // the default width will be the large one for X11 R4 fonts
   width = 580
   if ({argc} > 4)   // look for a 5th argument

	XREV = $5
   end
   // if not explicitly R3, play it safe and use larger R4 width
   if ((XREV) == "R3")
	// for X11R3 fonts
	width = 490
   end
   create x1form {formpath} [{x},{y},{width},{height}] -nolabel
   pushe {formpath}
   create x1label textbar [40,0,{100*(width - 40)/width}%,20] -title  \
       "b/f - back/forward by page | j/k - by line | a/z - begin/end"
   create x1button DONE [0,0,40,20] -script "xhide "{formpath}
   create x1text text [0%,20,100%,94%] -file {textfile}
   pope
end
