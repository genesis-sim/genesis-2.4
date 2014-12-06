// genesis
// helpforms.g
// Forms for Neuron Tutorial help display, using  the ximage widget for
// display of GIF files and the xtext widget for scrolling of text files

function loadhelp
// create the forms with help screens
   str screens = "/output/popups/helpscreens"
   create neutral {screens}
   textwindow {screens}/help 10 370 help.hlp
   textwindow {screens}/running 10 370 running.hlp
   makegif {screens}/inputs 0 0 640 350 neuron1.gif "Neuron Inputs"
   textwindow {screens}/params 10 370 params.hlp
   textwindow {screens}/totry 10 370 totry.hlp

   // create popup help menu
    str form = "/output/popups/helpmenu"
    create xform {form} [770,10,255,260] -title "Help Menu"
    pushe {form}
    create xbutton helphelp [0,40,240,30] -title "Using Help" -script  \
        "xshowontop "{screens}"/help"
    create xbutton running [0,75,240,30] -title  \
        "Running the Simulation" -script  \
        "xshowontop "{screens}"/running"
    create xbutton inputs [0,110,240,30] -title "Neuron Inputs" -script \
         "xshowontop "{screens}"/inputs"
    create xbutton parameters [0,145,240,30] -title "Model Parameters"  \
        -script "xshowontop "{screens}"/params"
    create xbutton totry [0,180,240,30] -title "Things to Try" -script  \
        "xshowontop "{screens}"/totry"
    create xbutton quit [0,215,240,30] -title "DONE" -script  \
        "xhide "{form}
    pope
end
