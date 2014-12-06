//genesis - phsplot.g - Adds phase plots to the SQUID tutorial
// edited by Ed Vigmond -- Phase is strictly speaking the incorrect name for 
// the plot since phase plot is v and its derivative. Technically it is
// a state plot and to this end, it is labelled as such but it is too large a 
// programming effort to change all the variables with phase in them. 
// Phase and state are thus equivalent terms in programming terms.

//==============================================================
// functions to determine what will be plotted and set the scales
//==============================================================
function testvar(plotvar)
    int testvar
    str plotvar
    if (({plotvar}=="n")||({plotvar}=="m")||({plotvar}=="h")||({plotvar}=="V"))
        return 1
    else
        echo "Unrecognized variable for state plot: "{plotvar}
        return 0
    end
end

function new_plot
    pushe /output/phase_plot
    str xvar = {substring {getfield xplot value} 0 0}
    str yvar = {substring {getfield yplot value} 0 0}
    if ({xvar} == "v")    //  also allow lower case for "V"
	xvar = "V"
    end
    if ({yvar} == "v")
        yvar = "V"
    end
    if (({testvar {xvar}})&({testvar {yvar}}) == 0) //test for valid variables
        pope
        // with error message if variables are invalid
        return
    end
    deletemsg phase_grf  1 -incoming
    deletemsg phase_grf  0 -incoming
    if ({yvar} == "V")    // First message plots y-axis

      setfield phase_grf ymin -20 ymax 120 YUnits "Vm (mV)"
      addmsg /axon /output/phase_plot/phase_grf PLOT Vm *state *black
    else
      setfield phase_grf ymin 0 ymax 1 YUnits {yvar}
      if ({yvar} == "n")
        addmsg /axon/K/n /output/phase_plot/phase_grf PLOT m *state  *black
      else
        addmsg /axon/Na/{yvar} /output/phase_plot/phase_grf PLOT m *state *black
      end
    end
    if ({xvar} == "V")    // Second message plots x-axis

      setfield phase_grf xmin {-20 + EREST_ACT} xmax {120 + EREST_ACT}  \
          XUnits "Vm (mV)"
      addmsg /axon /output/phase_plot/phase_grf X Vm *state
    else
      setfield phase_grf xmin 0 xmax 1 XUnits {xvar}
      if ({xvar} == "n")
        addmsg /axon/K/n /output/phase_plot/phase_grf X m *state
      else
        addmsg /axon/Na/{xvar} /output/phase_plot/phase_grf X m *state
      end
    end
    pope
    squid_reset
end

//==============================================================
// function to create the form and initial graph for phase plots
//==============================================================
function add_squid_phaseplot
    create xform /output/phase_plot [555,10,480,550] -nolabel
    pushe /output/phase_plot
    create xgraph phase_grf [0,0,480,300] -title "State Plot"
    setfield phase_grf xmin {-20 + EREST_ACT} xmax {120 + EREST_ACT}  \
        ymin 0 ymax 1 // phase_plot 1
    setfield phase_grf XUnits "Vm (mV)" YUnits "n"
    // change this?
    useclock phase_grf 1

    create xlabel allowed_vars [3,305,474,25] -title  \
        "Allowed variables for plotting are n, m, h, V"
    create xdialog xplot [3,330,235,25] -title "x-axis:" -value "V"  \
        -script new_plot
    create xdialog yplot [242,330,235,25] -title "y-axis:" -value "n"  \
        -script new_plot
    // also put in a graph for activation parameters vs time
    create xgraph act_grf [0,360,480,190] -title  \
        "H-H activation parameters vs time"
    setfield act_grf xmax 50 ymax 1 XUnits "t (msec)" YUnits ""
    useclock act_grf 1
    // should later add scale button and rescale x-axis for voltage clamp
    pope

    addmsg /axon/K/n /output/phase_plot/phase_grf PLOT m *state *black
    addmsg /axon /output/phase_plot/phase_grf X Vm *state
    addmsg /axon/Na/m /output/phase_plot/act_grf PLOT m *m *black
    addmsg /axon/Na/h /output/phase_plot/act_grf PLOT m *h *red
    addmsg /axon/K/n /output/phase_plot/act_grf PLOT m *n *blue
    squid_reset
end
