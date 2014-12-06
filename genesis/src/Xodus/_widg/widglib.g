//genesis
/* $Id: widglib.g,v 1.1 2005/06/17 21:23:51 svitak Exp $ */
/* $Log: widglib.g,v $
/* Revision 1.1  2005/06/17 21:23:51  svitak
/* This file was relocated from a directory with the same name minus the
/* leading underscore. This was done to allow comiling on case-insensitive
/* file systems. Makefile was changed to reflect the relocations.
/*
/* Revision 1.1.1.1  2005/06/14 04:38:32  svitak
/* Import from snapshot of CalTech CVS tree of June 8, 2005
/*
/* Revision 1.6  1998/07/15 06:27:19  dhb
/* Upi update
/*
 * Revision 1.5  1997/06/30 18:00:25  venkat
 * Added readonly field "nested" to xform.
 *
 * Revision 1.4  1995/02/04 01:03:29  venkat
 * Added SHOW action for object xform
 *
# Revision 1.3  1994/12/21  21:52:19  dhb
# Added output class tag to all widgets.
#
# Revision 1.2  1994/12/06  00:38:08  dhb
# Nov 8 1994 changes from Upi Bhalla
# */

//==========================================================================
//  name        data            function    class
//==========================================================================

object  xform    xform_type XForm widget output \
    -author     "Maneesh Sahani Caltech July/92" \
    -actions    PROCESS RESET CREATE SET COPY SHOW DELETE XUPDATE \
    -description    "Containing window for other graphical interface objects" \
    -fields	fg "" bg "" xgeom "" ygeom "" wgeom "" hgeom "" \
		title "" auto_resize "" \
	-readonly nested "Flag to indicate nested/toplevel form" \
	-hidden widget "Pointer to widget for form" \
	-hidden shell "Pointer to widget for shell" \
	-hidden outerframe "Pointer to widget for outer frame" \
	-hidden middleframe "Pointer to widget for middle frame" \
	-hidden innerframe "Pointer to widget for inner frame" \
	-hidden label_w "Pointer to widget for optional label"
//*    -messages   LABEL 0  1 label \        /* type 0: label */
//*    -fields     "label   :   value being displayed" \      */


object  xlabel    xlabel_type XLabel widget output \
    -author     "Maneesh Sahani Caltech July/92" \
    -actions    PROCESS RESET CREATE SET COPY DELETE XUPDATE \
    -description    "Static display for a text string" \
    -fields	fg "" bg "" xgeom "" ygeom "" wgeom "" hgeom "" \
		label "" font "" \
	-hidden widget "Pointer to widget for label"
//*    -messages   LABEL 0  1 label \        /* type 0: label */
//*    -fields     "label   :   value being displayed" \      */


object  xbutton    xbutton_type XButton widget output \
    -author     "Maneesh Sahani / Upi Bhalla Caltech Feb/93" \
    -actions    PROCESS RESET CREATE SET COPY DELETE XUPDATE \
		B1DOWN B2DOWN B3DOWN XOCOMMAND ANYBDOWN \
    -description    "executes script actions from button presses" \
    -fields	fg "" bg "" xgeom "" ygeom "" wgeom "" hgeom "" \
		onlabel "" offlabel "" onfg "" offfg "" onbg "" offbg "" \
		onfont "" offfont "" script "" \
	-hidden widget "Pointer to widget for button"
//*    -messages   LABEL 0  1 button \        /* type 0: button */
//*    -fields     "labelon   :   value being displayed" \      */


object  xtoggle    xtoggle_type XToggle widget output \
    -author     "Maneesh Sahani / Upi Bhalla Caltech Feb/93" \
    -actions    PROCESS RESET CREATE SET COPY DELETE XUPDATE \
		B1DOWN B2DOWN B3DOWN XOCOMMAND ANYBDOWN \
		B1UP B2UP B3UP ANYBUP \
    -description    "maintains toggle state and executes script actions from toggle presses" \
    -fields	fg "" bg "" xgeom "" ygeom "" wgeom "" hgeom "" \
		onlabel "" offlabel "" onfg "" offfg "" onbg "" offbg "" \
		onfont "" offfont "" script "" state "" \
	-hidden widget "Pointer to widget for toggle"

//*    -messages   LABEL 0  1 button \        /* type 0: button */
//*    -fields     "labelon   :   value being displayed" \      */


object  xdialog    xdialog_type XDialog widget output \
    -author     "Maneesh Sahani / Upi Bhalla Caltech May/93" \
    -actions    PROCESS RESET CREATE SET COPY DELETE XUPDATE \
		B1DOWN B2DOWN B3DOWN XOCOMMAND KEYPRESS ANYBDOWN \
    -description    "executes script actions from dialog events" \
	-messages	INPUT 0 1 value \	/* type 0: dialog */
    -fields	fg "" bg "" xgeom "" ygeom "" wgeom "" hgeom "" \
		value "" script "" label "" font "" fvalue "" \
	-hidden widget "Pointer to widget for dialog"

object xtext	xtext_type XText widget output \
	-author "Maneesh Sahani Caltech / Jason Leigh Univ of Illinois @ Chicago July/93" \
	-actions PROCESS RESET CREATE SET COPY DELETE XOCOMMAND \
		KEYPRESS XUPDATE SAVE2 B1DOWN B2DOWN B3DOWN ANYBDOWN DUMP \
	-description "opens a text editing widget" \
	-fields	fg "" bg "" xgeom "" ygeom "" wgeom "" hgeom "" \
		initialtext "" editable "" filename "" font "" \
	-hidden hiddentext "internal field with text pointer" \
	-hidden widget "Pointer to widget for text"

// function for loading text from a script file to an xtext
addfunc xtextload do_xtextload


object ximage	ximage_type Ximage widget output \
	-author "Jason Leigh Univ of Illinois @ Chicago July/93, Upi Bhalla" \
	-actions PROCESS RESET CREATE SET COPY DELETE \
		XOCOMMAND B1DOWN B2DOWN B3DOWN ANYBDOWN XUPDATE SAVE2 \
	-description "Displays a gif file" \
	-fields	fg "" bg "" xgeom "" ygeom "" wgeom "" hgeom "" \
		filename "" srcx "" srcy "" dstx "" dsty "" script "" \
	-hidden widget "Pointer to widget for image"
//	-messages None that I know of.

object xfastplot	xfastplot_type XFastPlot widget output \
	-author "Upi Bhalla NCBS May 1998" \
	-actions PROCESS RESET CREATE SET COPY DELETE \
		XOCOMMAND B1DOWN B2DOWN B3DOWN ANYBDOWN KEYPRESS XUPDATE SAVE2 \
	-description "Efficient but limited plotting widget" \
	-fields	fg "" bg "" xgeom "" ygeom "" wgeom "" hgeom "" \
		xmin "" xmax "" ymin "" ymax "" npts "" overlay_flag "" \
		line_flag "" roll_flag "" oscope_flag "" trig_sign "" \
		ac_trig_flag "" trig_thresh "" update_dt "" request_dt "" \
		yval "" xval "" xaxis_flag "" yaxis_flag "" color "" \
		nnewpts "" lborder "" rborder "" tborder "" bborder "" \
		title "" xtextmax "" xtextmid "" xtextmin "" \
		ytextmax "" ytextmid "" ytextmin "" script "" \
		shortptr_flag "" ptindex "" \
	-hidden widget "Pointer to widget for image" \
	-hidden pts "Internal data array" \
	-hidden xptr "" \
	-hidden yptr "" \
	-hidden alloced_npts "" \
	-hidden plotmode "" \
	-hidden wid "" \
	-hidden ht "" \
	-hidden xoffset "" \
	-hidden yoffset "" \
	-hidden xscale "" \
	-hidden yscale "" \
	-hidden nptr "" \
	-hidden ringpts "" \
	-hidden trig_flag "" \
	-hidden ac_trig_buf "" \
	-hidden colorptr "" \
	-hidden reset_flag "" \
	-hidden pm "" \
	-hidden double_flag "" \
	-hidden color_slot "" \
	-hidden nnewpts_slot "" \
	-hidden index_slot "" \
	-hidden sxval "" \
	-hidden syval "" \
	-messages	NPLOT 0 3 yvalues npts ptindex \
				WAVEPLOT 1 2 yvalues npts \
				XPLOT 2 1 xvalue \
				COLOR 3 1 color_int \
				UPDATE_DT 4 1 dt

