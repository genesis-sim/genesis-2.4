//genesis initialize.g

/* 
	functions defined in this script
=============================================================================
	FUNCTION NAME		ARGUMENTS
=============================================================================
 	loadgraphics
=============================================================================
*/

// ==========================================================================
//            initiate graphics at beginning of simulation
// ==========================================================================
function loadgraphics
    // activate XODUS
    
    // give it color
    xcolorscale hot
    // =====================================================
    //               create main control panel
    // =====================================================
    create xform /output/main_control [489,5,340,125] -title  \
        "CPG--by S. Crook"
    pushe /output/main_control
    create xbutton reset [3%,20,30%,25] -script reset -title RESET
    create xtoggle overlay [35%,20,30%,25] -title "OVERLAY" -script  \
        "overlaytoggle <widget>"
    create xbutton stop [67%,20,30%,25] -title STOP -script stop
    create xbutton quit [3%,47,30%,25] -title QUIT -script  \
        "quitbutton <widget>"
    create xbutton help [35%,47,30%,25] -title HELP -script  \
        "xshow /helpmenu"
    create xbutton defaults [67%,47,30%,25] -title DEFAULTS -script  \
        "resetdefaults"

    makeconfirm quit "Quit?" "quit" "cancelquit <widget>" 500 400
    create xdialog step [3%,74,46%,30] -script "stepbutton <widget>"  \
        -title "STEP (msec)" -value 200
    create xdialog stepsize [51%,74,46%,30] -title "dt (msec)" -script  \
        "setstepsize <widget>" -value {getclock 0}
    pope
    xshow /output/main_control

    // ======================================================
    // create graphics, settings, and connection toggle forms
    // ======================================================
    create xform /output/graphics [5,5,100,145] -title Graphs
    xshow /output/graphics

    create xform /output/settings [125,5,100,145] -title Settings
    xshow /output/settings

    create xform /output/connections [245,5,100,145] -title Connections
    xshow /output/connections

    create xform /output/inputs [365,5,100,145] -title Inputs
    xshow /output/inputs
end
