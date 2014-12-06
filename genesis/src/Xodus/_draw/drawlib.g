//genesis
/* $Id: drawlib.g,v 1.1 2005/06/17 21:23:51 svitak Exp $ */
/* $Log: drawlib.g,v $
/* Revision 1.1  2005/06/17 21:23:51  svitak
/* This file was relocated from a directory with the same name minus the
/* leading underscore. This was done to allow comiling on case-insensitive
/* file systems. Makefile was changed to reflect the relocations.
/*
/* Revision 1.1.1.1  2005/06/14 04:38:33  svitak
/* Import from snapshot of CalTech CVS tree of June 8, 2005
/*
/* Revision 1.10  1997/07/18 19:52:53  dhb
/* Changes from PSC: new messages for parallel xviewing
/*
 * Revision 1.9  1997/06/03 19:39:33  venkat
 * Made the processed field hidden in the xgraph and xplot elements
 *
# Revision 1.8  1996/10/05  17:22:26  dhb
# Added USECLOCK action to xgraph
#
# Revision 1.7  1995/08/03  01:00:22  venkat
# Added xpixflags command but retained pixflags for backwards compatibility
#
# Revision 1.6  1995/05/12  23:39:01  dhb
# Merged in 1.4.1.1 changes
#
<<<<<<< 1.5
# Revision 1.5  1995/04/27  01:40:32  venkat
# Added ADDMSGIN action to the xplot object
#
=======
# Revision 1.4.1.1  1995/05/12  23:18:14  dhb
# Changes to xview to support 1.4 xfileview type functionality
# using disk_in and xview elements.
#
>>>>>>> 1.4.1.1
# Revision 1.4  1995/02/04  01:04:06  venkat
# Made labelpix field hidden for object xplot
#
# Revision 1.3  1994/12/21  21:42:38  dhb
# Added the output class tag to objects which didn't have it
#
# Revision 1.2  1994/12/06  00:39:15  dhb
# Nov 8 1994 changes from Upi Bhalla
# */

// actions
addaction ADDMSGARROW 213
addaction SHOWMSGARROW 214
addaction MOVECUSTOM 215
addaction TRUNCATE 216
addaction XSHAPE_ADDPT 217
addaction XSHAPE_MOVEPT 218
addaction ADDPTS 219

// functions
addfunc xpixflags do_pixflags char*
// for backwards compatibility
addfunc pixflags do_pixflags char*

//==========================================================================
//  name        data            function    class
//==========================================================================
object  xcoredraw    xcoredraw_type XCoreDraw widget output \
    -author     "Upi Bhalla Caltech Apr/93" \
    -actions    PROCESS RESET CREATE SET COPY DELETE \
		B1DOWN B2DOWN B3DOWN ANYBDOWN B1DOUBLE B2DOUBLE B3DOUBLE \
		B1ENTER B2ENTER B3ENTER XUPDATE XODROP XOCOMMAND \
    -fields	fg "" bg "" xgeom "" ygeom "" wgeom "" hgeom "" \
		xmin "" xmax "" ymin "" ymax "" script "" \
    -description    "Core class for draws. Displays in xy plane" \
	-hidden widget "Pointer to widget/gadget"

object  xdumbdraw    xdumbdraw_type XDumbDraw widget output \
    -author     "Upi Bhalla Caltech Apr/93" \
    -actions    PROCESS RESET CREATE SET COPY DELETE \
		B1DOWN B2DOWN B3DOWN ANYBDOWN B1DOUBLE B2DOUBLE B3DOUBLE \
		B1ENTER B2ENTER B3ENTER XUPDATE XODROP XOCOMMAND \
    -description    "Simple window for pixes" \
    -fields	fg "" bg "" xgeom "" ygeom "" wgeom "" hgeom "" \
		xmin "" xmax "" ymin "" ymax "" zmin "" zmax "" script "" \
	-hidden widget "Pointer to widget/gadget"

object  xdraw    xdraw_type XDraw widget output \
    -author     "Upi Bhalla Caltech Apr/93" \
    -actions    PROCESS RESET CREATE SET COPY DELETE \
		B1DOWN B2DOWN B3DOWN ANYBDOWN B1DOUBLE B2DOUBLE B3DOUBLE \
		B1ENTER B2ENTER B3ENTER XUPDATE XODROP XOCOMMAND \
    -description    "3-d window with full transforms, for pixes" \
    -fields	fg "" bg "" xgeom "" ygeom "" wgeom "" hgeom "" \
		xmin "" xmax "" ymin "" ymax "" zmin "" zmax "" script "" \
		cx "" cy "" cz "" wx "" wy "" vx "" vy "" vz "" \
		transform "" \
	-hidden widget "Pointer to widget/gadget" \
	-messages   CENTER 0  3 cx cy cz \  /* type 0: center in space  */
		ZOOM 1  2 wx wy \  				/* type 1: size of window */
		VIEWPOINT 2  3 vx vy vz 		/* type 2: viewpoint in spc */

object  xgraph    xgraph_type XGraph widget output \
    -author     "Upi Bhalla Mount Sinai May/93" \
    -actions    PROCESS RESET CREATE SET COPY DELETE \
		B1DOWN B2DOWN B3DOWN ANYBDOWN B1DOUBLE B2DOUBLE B3DOUBLE \
		B1ENTER B2ENTER B3ENTER XUPDATE XODROP XOCOMMAND \
		ADDMSGIN DELETEMSGIN \
		USECLOCK \
    -messages   PLOT 0  3 data name color \  /* type 0: plot */
    -messages   PLOTSCALE 1  5 data name color scale yoffset \  /* type 1: plotscale */
    -messages   X 2  2 x name \  /* type 2: x. controls x axis of plot with same name */
    -messages   WAVEPLOT 3  3 data name color \  /* type 3: waveplot */
    -description    "graph widget. Automatically creates plots" \
    -fields	fg "" bg "" xgeom "" ygeom "" wgeom "" hgeom "" \
		xmin "" xmax "" ymin "" ymax "" xoffset "" yoffset "" \
		script "" overlay "" \
	-hidden widget "Pointer to widget/gadget" \
	-hidden processed "Internal flag that signals if the element has been processed or not"

object  xpix    xpix_type XPix gadget output \
    -author     "Upi Bhalla Caltech Apr/93" \
    -actions    PROCESS RESET CREATE SET COPY DELETE \
		B1DOWN B2DOWN B3DOWN ANYBDOWN B1DOUBLE B2DOUBLE B3DOUBLE \
		B1ENTER B2ENTER B3ENTER XUPDATE XODROP XOCOMMAND \
		XODRAG XOWASDROPPED \
    -description    "core pix" \
    -fields	fg "" script "" tx "" ty "" tz "" \
	-hidden widget "Pointer to widget/gadget"

object  xsphere    xsphere_type XSphere gadget output \
    -author     "Upi Bhalla Caltech Apr/93" \
    -actions    PROCESS RESET CREATE SET COPY DELETE \
		B1DOWN B2DOWN B3DOWN ANYBDOWN B1DOUBLE B2DOUBLE B3DOUBLE \
		B1ENTER B2ENTER B3ENTER XUPDATE XODROP XOCOMMAND \
		XODRAG XOWASDROPPED \
    -description    "test pix" \
    -fields	fg "" script "" tx "" ty "" tz "" r "" \
	-hidden widget "Pointer to widget/gadget"

object  xshape    xshape_type XShape gadget output \
    -author     "Upi Bhalla Caltech Apr/93" \
    -actions    PROCESS RESET CREATE SET COPY DELETE \
		B1DOWN B2DOWN B3DOWN ANYBDOWN B1DOUBLE B2DOUBLE B3DOUBLE \
		B1ENTER B2ENTER B3ENTER XUPDATE XODROP XOCOMMAND \
		XODRAG XOWASDROPPED DUMP XSHAPE_ADDPT XSHAPE_MOVEPT \
		SHOW \
    -messages   PATH 0  1 dummy \  /* type 0: path */
    -description    "back compat, draws assorted shapes" \
		"When a msg is present, then the value of the shape is set"\
                "to the full path of the source of the msg on PROCESS." \
                "drawmodes: DrawPoints,DrawLines,DrawSegs,DrawArrows,FillRects,FillPoly" \
                "linestyles: LineSolid, LineOnOffDash, or LineDoubleDash" \
                "capstyles: CapNotLast, CapButt, CapRound, CapProjecting" \
                "joinstyles: JoinMiter, JoinRound, JoinBevel" \
                "textmodes: draw, fill, nodraw" \
    -fields	fg "" script "" tx "" ty "" tz "" \
		drawmode "" linewidth "" linestyle "" \
		capstyle "" joinstyle "" text "" textcolor "" \
		textmode "" textfont "" pixflags "" value "" npts "" \
		xpts "" ypts "" zpts "" \
	-hidden widget "Pointer to widget/gadget"

object  xplot    xplot_type XPlot gadget output \
    -author     "Upi Bhalla Caltech May/93" \
    -actions    PROCESS RESET CREATE SET COPY DELETE \
		B1DOWN B2DOWN B3DOWN ANYBDOWN B1DOUBLE B2DOUBLE B3DOUBLE \
		B1ENTER B2ENTER B3ENTER XUPDATE XODROP XOCOMMAND \
		XODRAG XOWASDROPPED DUMP SHOW ADDMSGIN ADDPTS \
	-messages   PLOT 0  3 data name color \  /* type 0: plot */
      -messages   PLOTSCALE 1  5 data name color scale yoffset \  /* type 1: plotscale */
        -messages   X 2  2 x name \  /* type 2: x. controls x axis of plot with same name */
	-messages   WAVEPLOT 3  3 data name color \  /* type 3: waveplot */
    -description    "New object for drawing graphical plots" \
        "xmin,ymin,xmax,ymax: range of plotted values. Change" \
        " these if you wish to set offsets and scaling" \
        "wx,wy: scale factors for plot." \
        "memory_mode: [expand squish limit roll oscope halve file]" \
        "auto_mode: [none scale center both] (not yet imp.)" \
        "sensitivity: [select local_value move scale edit draw]"\
        " specifies mouse actions, not yet implemented" \
        "overlay_no: specifies how old this overlay version is"\
        "tx,ty,tz: specify offset of plot in parent draw coords" \
        "do_slope: flag. when set to 1, it differentiates the input" \
    -fields	 script "" tx "" ty "" tz "" fg "color of plot" \
		linewidth "width of plot line" \
		linestyle "[LineSolid,LineOnOffDash, LineDoubleDash]" \
		xmin "" xmax "" ymin "" ymax "" \
		wx "" wy "" plot_placement "" memory_mode "" auto_mode "" \
		sensitivity "" pixflags "" npts "" xpts "" ypts "" \
	-hidden widget "Pointer to widget/gadget" \
	-hidden labelpix "Pointer to labelpix widget" \
	-hidden processed "Internal flag that signals if the element has been processed or not"
	

object  xaxis    xaxis_type XAxis gadget output \
    -author     "Upi Bhalla Caltech May/93" \
    -actions    PROCESS RESET CREATE SET COPY DELETE \
		B1DOWN B2DOWN B3DOWN ANYBDOWN B1DOUBLE B2DOUBLE B3DOUBLE \
		B1ENTER B2ENTER B3ENTER XUPDATE XODROP XOCOMMAND \
		XODRAG XOWASDROPPED \
    -description    "makes axis for plot gadgets" \
    -fields	fg "" script "" tx "" ty "" tz "" \
		linewidth "" linestyle "" textcolor "" textmode "" \
		textfont "" units "" title "" unitoffset "" \
		titleoffset "" ticklength "" tickmode "" labeloffset "" \
		rotatelabels "" axx "" axy "" axz "" axmin "" axmax "" \
		axlength "" tickx "" ticky "" tickz "" labmin "" labmax "" \
	-hidden widget "Pointer to widget/gadget"

object  xgif    xgif_type XGif gadget output \
    -author     "Upi Bhalla / Jason Leigh Aug/93" \
    -actions    PROCESS RESET CREATE SET COPY DELETE \
		B1DOWN B2DOWN B3DOWN ANYBDOWN B1DOUBLE B2DOUBLE B3DOUBLE \
		B1ENTER B2ENTER B3ENTER XUPDATE XODROP XOCOMMAND \
		XODRAG XOWASDROPPED \
    -description    "draws gif images as a pix" \
    -fields	fg "" script "" tx "" ty "" tz "" \
    		filename "" srcx "" srcy "" dstx "" dsty "" \
	-hidden widget "Pointer to widget/gadget"

object  xcell    xcell_type XCell gadget output \
    -author     "Upi Bhalla Mount Sinai June/93" \
    -actions    PROCESS RESET CREATE SET COPY DELETE \
		B1DOWN B2DOWN B3DOWN ANYBDOWN B1DOUBLE B2DOUBLE B3DOUBLE \
		B1ENTER B2ENTER B3ENTER XUPDATE XODROP XOCOMMAND \
		XODRAG XOWASDROPPED \
  -messages   COLOR 0  1 data \  /* type 0: sets color of compt */
  		DIAMETER 1  1 data \  /* type 1: sets dia of compt  */
  		POSITION 2  3 x y z \  /* type 2: sets xyz of compt  */
  		COLDIA 3  2 col dia \  /* type 3: sets col and dia  */
  		COLDIAPOS 4  5 col dia x y z \  /* type 4: col, dia,xyz  */
    -description    "makes cell gadget" \
  -fields	fg "" script "" tx "" ty "" tz "" \
		colmin "" colmax "" diamin "" diamax "" diarange "" \
		soma "" path "" field "" fieldpath "" \
		autocol "" autodia "" labelmode ""  \
	-hidden widget "Pointer to widget/gadget"

 
object  xvar    xvar_type XVar gadget output \
    -author     "Upi Bhalla Mount Sinai Dec/93" \
    -actions    PROCESS RESET CREATE SET COPY DELETE \
		B1DOWN B2DOWN B3DOWN ANYBDOWN B1DOUBLE B2DOUBLE B3DOUBLE \
		B1ENTER B2ENTER B3ENTER XUPDATE XODROP XOCOMMAND \
		XODRAG XOWASDROPPED \
  -messages   VAL1 0  1 data \  /* type 0: Value 1 to display */
  -messages   VAL2 1  1 data \  /* type 1: Value 2 to display */
  -messages   VAL3 2  1 data \  /* type 2: Value 3 to display */
  -messages   VAL4 3  1 data \  /* type 3: Value 4 to display */
  -messages   VAL5 4  1 data \  /* type 4: Value 5 to display */
    -description    "makes var gadget which is used to display" \
      "numerical values in a variety of modes. More than" \
      "one value can be displayed simultaneously using the" \
      "following independent display parameters: " \
      "color, xoffset, yoffset, zoffset, morphing, text" \
      "The widget interpolates appropriately between child" \
      "shapes to generate the display. These child shapes must" \
      "live in ./shape[0], ./shape[1], etc" \
      "Each display parameter can be attached to a specified" \
      "value msg using the color_val, morph_val etc fields" \
      "the varmode has the general option 'shape'; the" \
      "special options graph, surface, and contour;" \
      "as well as some backwards compat options" \
  -fields	fg "" script "" mode "" \
		varmode "shape graph surface contour colorview boxview fillboxview colorboxview" \
		value_min "" value_max "" \
		values "" color_val "msg # to display in color" morph_val "" \
		xoffset_val "" yoffset_val "" zoffset_val "" \
		text_val "" textcolor_val "" linethickness_val "" \
		autolimits "" sizescale "" \
		script "" tx "" ty "" tz "" \
	-hidden widget "Pointer to widget/gadget"

object  xview    xview_type XView gadget output \
    -author     "Upi Bhalla Mount Sinai Dec/93" \
    -actions    PROCESS RESET CREATE SET COPY DELETE SHOW \
		B1DOWN B2DOWN B3DOWN ANYBDOWN B1DOUBLE B2DOUBLE B3DOUBLE \
		B1ENTER B2ENTER B3ENTER XUPDATE XODROP XOCOMMAND \
		XODRAG XOWASDROPPED \
  -messages   VAL1 0  1 data \  /* type 0: Value 1 to display */
  -messages   VAL2 1  1 data \  /* type 1: Value 2 to display */
  -messages   VAL3 2  1 data \  /* type 2: Value 3 to display */
  -messages   VAL4 3  1 data \  /* type 3: Value 4 to display */
  -messages   VAL5 4  1 data \  /* type 4: Value 5 to display */
  -messages   COORDS 5  3 x y z \  /* type 5: coords of values */
  -messages   IVAL1 6  2 index data \  /* type 6: Value 1 to display at indexed point */
  -messages   IVAL2 7  2 index data \  /* type 7: Value 2 to display at indexed point */
  -messages   IVAL3 8  2 index data \  /* type 8: Value 3 to display at indexed point */
  -messages   IVAL4 9  2 index data \  /* type 9: Value 4 to display at indexed point */
  -messages   IVAL5 10 2 index data \  /* type 10: Value 5 to display at indexed point */
  -messages   ICOORDS 11  4 index x y z \  /* type 11: coords of indexed point */
    -description    "makes view gadget which is like an array of xvars." \
      "It displays numerical values in a variety of modes. Similar rules" \
      "apply as for the xvar display parameters. In addition to displaying" \
      "values from incoming msgs, the values for the xview can be specified" \
      "using paths. xview can display values of fields in messages" \
      "between other elements using the path options." \
      "This is slow and has limitations when it comes to displaying" \
      "simulations in parallel." \ 
      "Field descriptions..." \
      "path: slower but more flexible alternative to msgs" \
      " for displaying values. Must also specify field" \
      "relpath: optional relative path from 'path'. Display" \
      " fields of child elements of 'path' using this." \
      "msgpath: optional. Select a subset of msgins/outs that link to msgpath" \
      " Used to display fields in msgins/msgouts from relpath"\
      "msgtype,msgslotno: optional. Applied only if field = msgin or msgout"\
      "viewmode: one of: shape graph surface contour"\
      "colorview boxview fillboxview colorboxview"\
      "valuemode: one of: path index lookup" \
  -fields	viewmode "" value_min "" value_max "" \
		fg "" script "" \
		values "" color_val "" morph_val "" \
		xoffset_val "" yoffset_val "" zoffset_val "" \
		text_val "" textcolor_val "" linethickness_val "" \
		autolimits "" sizescale "" \
		xpts "" ypts "" zpts "" ncoords "" \
		path "" relpath "" field "" field2 "" \
 		msgpath "" msgtype "" msgslotno "" \
		tx "" ty "" tz "" \
		valuemode "" \
	-hidden widget "Pointer to widget/gadget"

object  xtree    xtree_type XTree gadget output \
    -author     "Upi Bhalla Mount Sinai Jan/94" \
    -actions    PROCESS RESET CREATE SET COPY DELETE \
		B1DOWN B2DOWN B3DOWN ANYBDOWN B1DOUBLE B2DOUBLE B3DOUBLE \
		B1ENTER B2ENTER B3ENTER XUPDATE XODROP XOCOMMAND \
		XODRAG XOWASDROPPED \
        ADDMSGARROW SHOWMSGARROW MOVECUSTOM TRUNCATE \
		DUMP UNDUMP \
    -description    "makes tree gadget which is used to display" \
      "elements and their messages, and also manipulate them." \
      "This is a very complex widget, the user is referred to the" \
      "documentation for more complete details." \
      "The layout can be one of :" \
      "  tree = Elements positioned according to hierarchy " \
      "  geometry = elements are positioned according to their xyz coords"\
      "  custom = elements positions are user-defined" \
    -fields \
	fg "" script "" \
	treemode "" arrowmode "" namemode "" sizescale "" \
	ncoords ""  \
	path "" iconfunc "" treefg "" msgpathmode "" \
	treefile "" tx "" ty "" tz "" \
	highlight1 "Color for highlight 1" \
	highlight2 "Color for highlight 2" \
	hlmode "highlightmode: one of 'none', 'one' or 'multi'" \
	orientation "direction of top of tree. One of: up down left right" \
	-hidden widget "Pointer to widget/gadget"
