//genesis settings.g

/*	functions defined in this script
=============================================================================
	FUNCTION NAME		ARGUMENTS
=============================================================================
 	conductance_form	(graphicspath,cellpath,channelname,x,y)
        makesettings            (path)
=============================================================================
*/

// ==========================================================================
//      create a display of conductance information for settings window
// ==========================================================================
function conductance_form(graphicspath, cellpath, channelname, x, y)
    str graphicspath
    str cellpath
    str channelname
    float x, y
    str target = (cellpath) @ "/" @ (channelname)

    create xlabel {graphicspath}/{channelname} [{x},{y},200,30] -title  \
        {channelname}
    pushe {graphicspath}/{channelname}
    create xdialog E [{x},{y + 35},200,30] -script  \
        "fieldbutton "{target}" Ek <widget>" -title "Eequil (mV)" -value \
         {getfield {target} Ek}
    create xdialog gmax [{x},{y + 70},200,30] -script  \
        "fieldbutton "{target}" gmax <widget>" -title "Gm (mS/syn)"  \
        -value {getfield {target} gmax}
    create xdialog tau1 [{x},{y + 105},200,30] -script  \
        "fieldbutton "{target}" tau1 <widget>" -title "Tau1 (msec)"  \
        -value {getfield {target} tau1}
    create xdialog tau2 [{x},{y + 140},200,30] -script  \
        "fieldbutton "{target}" tau2 <widget>" -title "Tau2 (msec)"  \
        -value {getfield {target} tau2}
    pope
end


// ==========================================================================
//           create settings form and place displays in the window
// ==========================================================================
function makesettings(path)
    // path gives the location of the cell
    // settings path gives the location of the control form for the cell

    str path, temp1, temp2
    str settingspath = (path) @ "/settings"
    str togglepath = "/output/settings" @ (path)

    // add a toggle for this cell in the main cell control panel
    maketoggle {path} {togglepath} {settingspath}

    // =================================================
    //           create settings form
    // =================================================
   create xform {settingspath} [855,5,210,425] -title {settingspath}
   create xtoggle {settingspath}/done [5,20,200,25] -script  \
       "donetoggle <widget> "{togglepath}" "{settingspath}
    setfield {settingspath}/done offlabel "HIDDEN"
    setfield {settingspath}/done onlabel "VISIBLE"
    pushe {settingspath}

    // =================================================
    //     create conductance parameters buttons
    // =================================================
    conductance_form {settingspath} {path}/soma/dend Na_channel 5 50
    conductance_form {settingspath} {path}/soma/dend K_channel 5 235

    pope
end
