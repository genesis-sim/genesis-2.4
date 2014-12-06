// genesis
// helpforms.g
// Forms for help display, using the xtext widget for scrolling of text files
// and the ximage widget for display of GIF format bitmapped image files

str XREV
//XREV = "R3"	
// global variable for setting width of text display from X11 revision #
// "R4" will be assumed if it is not explicitly set to "R3"

/* NOTE: 
The version of the function "textwindow" provided in the March 1991 GENESIS
distribution xtools.g creates a form 490 pixels wide and 336 pixels high at
x,y to display the specified text file with scrolling and paging.  The
actual text window is 484 x 316 pixels.  Using the default font for X11R3,
this provides room for 24 lines with 78 characters per line and a one
character right and left margin.  (The left margin is provided
automatically.)  X11R4 uses a larger font for both the xtext widget and the
xlabel widget, however.  In addition, it provides a scroll bar with the
xtext widget.  Thus, in order to accomodate the same number of charaters, a
form width of 580 pixels should be used.  The following replacement for
"textwindow" uses an optional argument "XREV" to determine the proper width
and also allows the size of the text window to be resized along with the
parent form.  If this argument is not specified, the larger width
appropriate to X11R4 is used.  When displaying X11R4 windows on a DECstation
with its large fonts, an even wider width of 810 should be used.  This can
either be changed below, or the window may be rescaled after it appears.
*/

//=====================================================================
// textwindow(formpath,x,y,textfile,[XREV])
// creates a form 490 pixels wide and 336 pixels high at x,y to display
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
   create xform {formpath} [{x},{y},{width},{height}] -nolabel
   pushe {formpath}
   create xlabel textbar [40,0,{100*(width - 40)/width}%,20] -title  \
       "Use the scroll bar at the left to move through the text"
   create xbutton DONE [0,0,40,20] -script "xhide "{formpath}
   create xtext text [0%,20,100%,94%] -filename {textfile}
   pope
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
       [{x},{y},{image_width + 10},{image_height + 25}] -nolabel
   create xlabel {formpath}"/titlebar" [5,0,{image_width},20] -title  \
       {title}
   create xbutton {formpath}"/DONE" [15,0,80,20] -script  \
       "xhide "{formpath}
   create ximage {formpath}"/image" [5,20,{image_width},{image_height}]
   setfield ^ wgeom {image_width} hgeom {image_height} filename {gif_file}
end
//-------------------------------------------------------------------------

function loadhelp
// create the forms with help screens
   str screens = "/output/helpscreens"
   create neutral {screens}
   textwindow {screens}/help 0 440 help.hlp {XREV}
   textwindow {screens}/intro 0 440 intro.hlp {XREV}
   textwindow {screens}/running 0 440 running.hlp {XREV}
   textwindow {screens}/integration 0 440 num_int.hlp {XREV}
   textwindow {screens}/totry 0 440 totry.hlp {XREV}

   // create popup help menu
    str form = "/output/helpmenu"
    create xform {form} [450,50,240,190] -title "Help Menu"
    pushe {form}
    create xbutton helphelp  -title "Using Help" -script  \
        "xshowontop "{screens}"/help"
    create xbutton intro  -title "Introduction" -script  \
        "xshowontop "{screens}"/intro"
    create xbutton running -title "Running the Simulation"  -script  \
        "xshowontop "{screens}"/running"
    create xbutton int_method  -title  "Integration Methods" -script  \
        "xshowontop "{screens}"/integration"
    create xbutton totry  -title "Things to Try" -script  \
        "xshowontop "{screens}"/totry"
    create xbutton quit  -title "Quit Help" -script "xhide "{form}
    pope
end
