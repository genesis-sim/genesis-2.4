// genesis
// tut_forms.g
// Forms for SQUID Tutorial help display, using  the
// xtext widget for scrolling of text files

// This also creates additional widgets which are not in squid_forms.g

/*
The function "textwindow" creates a form 590 pixels wide and 336 pixels high
at x,y to display the specified text file with scrolling and paging.  Using
the default font (7x13bold), this provides room for 24 lines with 78
characters per line and a one character right and left margin.  (The left
margin is provided automatically.)
*/

//=====================================================================
// textwindow(formpath,x,y,textfile,[XREV])
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


// toggle the display of state plot
function phasetoggle
  if ({getfield /forms/popups/phasetoggle state} == 1)
      xshowontop /output/phase_plot
  else
      xhide /output/phase_plot
  end
end

//===================================================================
//  Toggle blocking (disabling) of a channel by setting value of gbar
//===================================================================
function blocking_toggle(compartment, channel, widget)
    str compartment, channel, widget
    float length = ({getfield  {compartment} length})
    float diameter = ({getfield  {compartment} diameter})
    // cm^2
    float area = 1e-8*length*PI*diameter
    if ({getfield {widget} state} == 1)
        setfield {compartment}/{channel} gbar 0
        echo {channel}" channel blocked"
    else    // reset gbar to the proper value

	if ((channel) == "Na")
	    setfield {compartment}/{channel} gbar {GNa_squid*area}
	else	// assume it is the K channel

            setfield {compartment}/{channel} gbar {GK_squid*area}
	end
	echo {channel}" channel unblocked"
    end
    squid_reset
end

function add_squid_popups
   str screens = "/forms/helpscreens"
   create neutral {screens}
   textwindow {screens}/running 55 225 squid.text
   create xform /forms/popups [0,500,215,160] -nolabel
   pushe /forms/popups
   create xbutton running [10,5,180,30] -title "HELP" -script  \
       "xshowontop "{screens}"/running"
   create xtoggle phasetoggle [10,40,180,30] -title "" -script  \
       phasetoggle
   setfield phasetoggle label0 "State Plot Hidden"
   setfield phasetoggle label1 "State Plot Visible" state 0

   create xtoggle Na_block [10,75,180,30] -title "" -script  \
       "blocking_toggle /axon Na <widget>"
   setfield Na_block label0 "Na channel unblocked"  \
       label1 "Na channel blocked"
   setfield Na_block state 0

   create xtoggle K_block [10,110,180,30] -title "" -script  \
       "blocking_toggle /axon K <widget>"
   setfield K_block label0 "K channel unblocked"  \
       label1 "K channel blocked" state 0

   pope
   xshow /forms/popups
end
