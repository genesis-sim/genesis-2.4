//genesis
// Neuron.g

// set the prompt
setprompt "Neuron !"
// simulation time step in msec
setclock 0 0.01 //  max size for reasonable accuracy - for spike plots, too
setclock 1 0.05 // output interval for Vm plots
setclock 2 0.05 // output interval for others
floatformat %.3g // do reasonable rounding for display

echo SIMULATION CONFIGURATION
echo =========================
echo Hodgkin-Huxley
echo multi-compartmental neuron (soma + dendrite)
echo with synaptic connections

// start with no passive compartments between dendrites
int NCableSects = 0

//==================================================
//                  STARTUP SCRIPTS
//==================================================
include constants.g 
include tools.g 
include channel.g 
include compartment.g 
include cell.g 
include inputs.g 
include make_cable.g 
include forms.g 
include setcolors.g // function colorize sets widget colors

//=========================================================================
//  Some functions used in the main script

//  SendPlotMsgs  - sends messages to the graphics forms

//  ResetDefaults - similar to the main script, but graph forms pre-exist
//=========================================================================

function SendPlotMsgs
  pushe /output/dend1graphs
  addmsg /input/injectpulse input_grf PLOT output *Inject *black
  addmsg /input/Apulse/spiketrain input_grf PLOTSCALE state *SourceA  \
      *red 1 2
  addmsg /input/Bpulse/spiketrain input_grf PLOTSCALE state *SourceB  \
      *blue 1 4
  addmsg /cell/dend1/Ex_channel dend1Gk_grf PLOT Gk *Gex *black
  addmsg /cell/dend1/Inh_channel dend1Gk_grf PLOT Gk *Ginh *red
  addmsg /cell/dend1 dend1Vm_grf PLOT Vm *dend1 *black
  pope

  pushe /output/dend2graphs
  addmsg /cell/dend2/Ex_channel dend2Gk_grf PLOT Gk *Gex *black
  addmsg /cell/dend2/Inh_channel dend2Gk_grf PLOT Gk *Ginh *red
  addmsg /cell/dend2 dend2Vm_grf PLOT Vm *dend2 *black
  pope

  pushe /output/somagraphs
  addmsg /cell/soma/Ex_channel activation_grf PLOT X *m *black
  addmsg /cell/soma/Ex_channel activation_grf PLOT Y *h *red
  addmsg /cell/soma/Inh_channel activation_grf PLOT X *n *blue

  addmsg /cell/soma/Ex_channel somaGk_grf PLOT Gk *Na *black
  addmsg /cell/soma/Inh_channel somaGk_grf PLOT Gk *K *red
  addmsg /cell/soma somaVm_grf PLOT Vm *soma *black
  pope
end// SendPlotMsgs


function ResetDefaults
    stop
    delete /cell
    delete /input
    delete /output/control
    delete /output/popups
    // re-initialize toggle state
    xhide /output/dend2graphs
    NCableSects = 0
    makeneuron /cell {soma_l} {soma_d} {dend_l} {dend_d}    // in cell.g
    makeinputs    // inputs.g
    loadcontrol
    SendPlotMsgs
//    check
    colorize
    reset_kludge    // in cell.g
end

//=========================================================================

//                         MAIN SCRIPT - Neuron   

//=========================================================================

//==================================================
// check and initialize the simulation
//==================================================
makeneuron /cell {soma_l} {soma_d} {dend_l} {dend_d}// in cell.g

// inputs.g
makeinputs
// in forms.g (This is done after the elements are created)
loadgraphs
loadcontrol
SendPlotMsgs
// check
colorize
reset_kludge // in cell.g

echo ==================
echo Simulation loaded.
echo ==================
