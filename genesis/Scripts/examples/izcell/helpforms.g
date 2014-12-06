// genesis
// helpforms.g
// Forms for help display, using  the ximage widget for
// display of GIF files and the xtext widget for scrolling of text files

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
   create xlabel textbar [40,0,{100*(width - 40)/width}%,20] -label  \
     "Click and drag the left mouse button on the scroll bar to scroll the text"
   create xbutton DONE [0,0,40,20] -script "xhide "{formpath}
   create xtext text [0%,20,100%,94%] -filename {textfile} -bg white
   pope
end

function loadhelp
// create the forms with help screens
   textwindow /output/help 265 370 izcell.hlp
   makegif /output/figure1 0 0 470 622 figure1.gif \
     "     Figure 1 - Izhikevich IEEE Trans NN Sept 2004"
   makegif /output/figure2 0 0 851 579 figure2.gif \
     "     Figure 2 - Izhikevich IEEE Trans NN Nov 2003"
end
