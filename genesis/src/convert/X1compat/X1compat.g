//genesis

int X1COMPAT

if (X1COMPAT)
    return // compatibility already loaded
end

X1COMPAT = 1

xcolorscale hot //rainbow

// this is a small set of functions to provide backward compatability
// with some Xodus 1 commands not easily mapped to Xodus 2 commands.
// The XADDPTS variable can be set to zero to disable inclusion of
// xaddpts in the compatibility library.  Eventually xaddpts is to
// be included in Xodus 2.

// Mon Aug 15 17:33:49 PDT 1994
// Added a number of exended object to provide BC functionality for pix/draw
// selections.

int XADDPTS = 1

//
// "when" commands
//

function when(widg, action, do, script)

	if ({argc} != 4)
	    echo "usage: when <widget> <action> do <script>"
	    return
	end

	str command = {getarg {arglist {script}} -arg 1}

	if (action == "click1")
	    script = {strsub {script} {command} {command}.d1}
	else if (action == "click2")
	    script = {strsub {script} {command} {command}.d2}
	else if (action == "click3")
	    script = {strsub {script} {command} {command}.d3}
	else if ({strncmp {action} drag 4} == 0)
	    script = {strsub {script} {command} {command}.y}
	end // drag
	end // click3
	end // click2
	end // click1

	str old = {getfield {widg} script}

	if (old == "nil" || old == "")
	    setfield {widg} script {script}
	else
	    setfield {widg} script {old}";"{script}
	end
end

function dropwhen(widg, script)

	if (script == "all")
	    setfield {widg} script ""
	else
	    str scrstr = {getfield {widg} script}

	    scrstr = {strsub {scrstr} {script} ""}
	    scrstr = {strsub {scrstr} ";;" ";"}
	    setfield {widg} script {scrstr}
	end
end

function listwhen(widg)

	showfield {widg} script

end



//
// xshowall
//

function xshowall

	str elm

	foreach elm ({el /##[CLASS=widget]})
	    xshow {elm}
	end

end


//
// Create Xodus 1 compatability objects
//

create xform /x1compat
pushe /x1compat > /dev/null

    include X1widget label
    include X1widget button
    include X1widget toggle
    include X1widget dialog
    include X1text
    include X1widget image

    include X1graph
    include X1draw  // makes all the pixes too

pope > /dev/null

    // these are field name aliases for widget width and height.  This
    // isn't a complete solution since the old width and height do not
    // include widget and edge references while wgeom and hgeom do.  What
    // we do here will work so long as the caller doesn't need to preserve
    // widget and edge refs.

    addfield /x1compat width -indirect . wgeom
    addfield /x1compat height -indirect . hgeom

	// scheduler complains if the widgets aren't schedulable
	addclass /x1compat output

addobject x1form /x1compat


//
// xaddpts
//
//	This command will eventually be added to Xodus 2.  The XADDPTS
//	script variable will control its inclusion here.

if (!XADDPTS)
    return
end

function xaddpts(plotpath)

	float	x, y
	int	npts
	int	nargs = {argc}
	int	arg
	int	i

	if (nargs < 3 || nargs%2 != 1)
	    echo usage: $0 plotpath x y [additional-x-y-pairs]
	    return
	end

	if (!{isa xplot {plotpath}})
	    echo $0: element {plotpath} is not an xplot or does not exist
	    return
	end

	npts = {getfield {plotpath} npts}
	for (arg = 2; arg < nargs; arg = arg + 2)
	    if ({argv {arg}} == "-color")
		setfield {plotpath} fg {argv {arg+1}}
	    else
		x = {argv {arg}}
		y = {argv {arg+1}}
		setfield {plotpath} xpts->table[{npts}] {x} \
				    ypts->table[{npts}] {y}
		npts = npts + 1
	    end
	end
	setfield {plotpath} npts {npts}
end

// add some aliases for old Xodus commands which are used in callbacks

addalias xshow_on_top xshowontop
