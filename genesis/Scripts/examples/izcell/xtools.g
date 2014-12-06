//genesis
// xtools.g - a subset of the version in genesis/startup
int XTOOLSDEBUG  // define as non-zero prior to including xtools to
                 // get debug output
/*
** user functions available in xtools
**
** function		arguments
** ======================================================
** overlay		path value
** overlaytoggle	widget // also in buttons.g
** makeconfirm		path title yes_script no_script x y
** setgraphscale	graph
** showgraphscale	graph // orig had y offset of -155
** makegraphscale	graph
** fieldbutton		path field widget
** quitbutton		widget
** cancelquit		widget
** stepbutton		widget
** formtoggle		widget form0 form1
** donebutton           form   // clicks all buttons in form except "DONE"
**
*/

function overlay(path, value)
str path
    setfield {path}/##[ISA=xgraph] overlay {value}
    echo "overlay " -nonewline
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

    create xbutton {graph}_scalebutton  \
        [{getfield {graph} xgeom},{getfield {graph} ygeom},50,25] \
           -title scale -script "showgraphscale "{form}
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
// advances the simulation by the amount of time given in the dialog
// ==============================================
function stepbutton(widget)
    str widget
    echo step  {getfield {widget} value} msec.
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
