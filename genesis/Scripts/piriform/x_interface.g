// genesis

echo "******Setting up XODUS Interface******"

xcolorscale hot


function test_mode(func,parm1,parm2)
str func, parm2
int parm1


    if((!{strcmp "net_view" {func}})||(!{strcmp "side_view" {func}})|| \
       (!{strcmp "spike_view" {func}})||(!{strcmp "field_view" {func}})|| \
       (!{strcmp "conduct_view" {func}}))

       	    if (({getfield /menu/draw/mode state}) == 1)
	       
	       if(!{strcmp "net_view" {func}})
	       	    net_view {parm1} {parm2}
	       end
	       if(!{strcmp "side_view" {func}})
	       	    side_view {parm1}
	       end
	       if(!{strcmp "spike_view" {func}})
	       	    spike_view {parm1} {parm2}
	       end
	       if(!{strcmp "field_view" {func}})
	       	    field_view {parm1}
	       end
	       if(!{strcmp "conduct_view" {func}})
	       	    conduct_view {parm1} {parm2}
	       end

	     else
	       xshow /x_interface/not_play
	     end
     end

     if(!{strcmp "set_parameters" {func}})

       	    if (({getfield /menu/draw/mode state}) == 0)     	
	       set_parameters
	    else
	       xshow /x_interface/not_sim
	    end
     end

end


create neutral /x_interface

ce /x_interface
create xform not_sim [200,250,220,63] 
create xlabel not_sim/label -title "NOT IN SIMULATE MODE" -bg red
create xbutton not_sim/cancel -title "cancel" -script "xhide /x_interface/not_sim"
create xform not_play [200,250,220,63] 
create xlabel not_play/label -title "NOT IN PLAYBACK MODE" -bg red
create xbutton not_play/cancel -title "cancel" -script "xhide /x_interface/not_play"
create xform dir_error [200,250,220,63] 
create xlabel dir_error/label -title "DIRECTORY ALREADY EXISTS" -bg red
create xbutton dir_error/cancel -title "cancel" -script "xhide /x_interface/dir_error"


// Make xform containing info about running simulation

create xform /x_interface/simulate [30,477,300,303]
ce ^
create xlabel label -title "simulate" -bg pink
create xdialog step_size -title "step size (msec)" -value "0.1"
create xdialog sim_dur -title "simulation duration (msec)" -value "500"
create xdialog directory -title "data directory name" -value "default"
create xdialog out_step -title "output rate (msec)" -value "1.0"
create xbutton reset -title "reset" -script "reset"
create xbutton run -title "start simulation" -script start_sim
create xbutton continue -title "continue simulation" -script cont_sim
create xbutton stop -title "stop" -script "stop"
create xbutton sim_stat -title "simulation status" -script sim_stat
create xbutton cancel -title "cancel" -script "xhide /x_interface/simulate"

/*
// Make xform containing info about simulation parameters

create xform /x_interface/parameters [147,50,300,380]
ce ^
create xlabel weights -title "weights"
create xdialog wt_bulb_Ia -title "bulb_Ia" -value "1.0"
create xdialog wt_ff_Ia -title "ff_Ia" -value "1.0"
create xdialog wt_fb_soma -title "fb_soma" -value "1.0"
create xdialog wt_fb_fb -title "fb_fb" -value "1.0"
create xdialog wt_pyr_ff -title "pyr_ff" -value "1.0"
create xdialog wt_pyr_fb -title "pyr_fb" -value "1.0"
create xdialog wt_pyr_RA -title "pyr_RA" -value "1.0"
create xdialog wt_pyr_CA -title "pyr_CA" -value "1.0"
create xdialog wt_pyr_local -title "pyr_LOCAL" -value "1.0"
create xlabel channels -title "synaptic channel open times (msec)"
create xdialog chan_Aff_Na -title "Aff_Na" -value "3.0"
create xdialog chan_RA_Na -title "RA_Na" -value "3.0"
create xdialog chan_CA_Na -title "CA_Na" -value "3.0"
create xdialog chan_K -title "K" -value "100.00"
create xdialog chan_Cl -title "Cl" -value "7.0"
create xlabel label -title "" 
create xbutton apply -title "apply" -script "test_mode set_parameters 0 null"
create xbutton cancel -title "cancel" -script "xhide /x_interface/parameters"
*/

// forms associated with setting synaptic parameters

create xform /x_interface/Aff_syn [122,103,260,189] 
ce ^
create xlabel label -title "bulb to pyr synapse" -bg pink
create xlabel weights -title "synaptic weight" -bg pink
create xdialog wt_bulb_Ia -title "bulb_Ia" -value "1.0"
create xlabel open_times -title "synaptic channel open times (msec)" -bg pink
create xdialog Na -title "Na" -value "3.0"
create xbutton apply -title "apply" -script "test_mode set_parameters 0 null" 
create xbutton cancel -title "cancel" -script "xhide /x_interface/Aff_syn"

create xform /x_interface/ff_syn [122,150,260,189] 
ce ^
create xlabel label -title "ff to pyr synapse" -bg pink
create xlabel weights -title "synaptic weight" -bg pink
create xdialog wt_ff_Ia -title "ff_Ia" -value "1.0"
create xlabel open_times -title "synaptic channel open times (msec)" -bg pink
create xdialog K -title "K" -value "100.0"
create xbutton apply -title "apply" -script "test_mode set_parameters 0 null" 
create xbutton cancel -title "cancel" -script "xhide /x_interface/ff_syn"

create xform /x_interface/CA_syn [122,203,260,189] 
ce ^
create xlabel label -title "rostral pyr to caudal pyr synapse" -bg pink
create xlabel weights -title "weight" -bg pink
create xdialog wt_CA_pyr -title "from rostral pyr" -value "1.0"
create xlabel open_times -title "synaptic channel open times (msec)" -bg pink
create xdialog Na -title "Na" -value "3.0"
create xbutton apply -title "apply" -script "test_mode set_parameters 0 null"
create xbutton cancel -title "cancel" -script "xhide /x_interface/CA_syn"

create xform /x_interface/RA_syn [213,282,260,189] 
ce ^
create xlabel label -title "caudal pyr to rostral pyr synapse" -bg pink
create xlabel weights -title "weight" -bg pink
create xdialog wt_pyr_RA -title "from caudal pyr" -value "1.0"
create xlabel open_times -title "synaptic channel open times (msec)" -bg pink
create xdialog Na -title "Na" -value "3.0"
create xbutton apply -title "apply" -script "test_mode set_parameters 0 null"
create xbutton cancel -title "cancel" -script "xhide /x_interface/RA_syn"

create xform /x_interface/fb_syn [222,348,260,189] 
ce ^
create xlabel label -title "fb to pyr synapse" -bg pink
create xlabel weights -title "weight" -bg pink
create xdialog wt_fb_pyr -title "fb_pyr" -value "1.0"
create xlabel open_times -title "synaptic channel open times (msec)" -bg pink
create xdialog Cl -title "Cl" -value "7.0"
create xbutton apply -title "apply" -script "test_mode set_parameters 0 null"
create xbutton cancel -title "cancel" -script "xhide /x_interface/fb_syn"

create xform /x_interface/LOCAL_syn [72,437,260,189] 
ce ^
create xlabel label -title "local pyr to pyr synapse" -bg pink
create xlabel weights -title "weight" -bg pink
create xdialog wt_pyr_local -title "from local pyr" -value "1.0"
create xlabel open_times -title "synaptic channel open times (msec)" -bg pink
create xdialog Na -title "Na" -value "3.0"
create xbutton apply -title "apply" -script "test_mode set_parameters 0 null"
create xbutton cancel -title "cancel" -script "xhide /x_interface/LOCAL_syn"

create xform /x_interface/Aff_ff_syn [0,95,260,140] 
ce ^
create xlabel label -title "bulb to ff synapse" -bg pink
create xlabel weights -title "weight" -bg pink
create xdialog wt_Aff_ff -title "Aff_to_ff" -value "1.0"
create xbutton apply -title "apply" -script "test_mode set_parameters 0 null"
create xbutton cancel -title "cancel" -script "xhide /x_interface/Aff_ff_syn"

create xform /x_interface/fb_ff_syn [0,120,260,140] 
ce ^
create xlabel label -title "pyr to ff synapse" -bg pink
create xlabel weights -title "weight" -bg pink
create xdialog wt_pyr_ff -title "pyr_to_ff" -value "1.0"
create xbutton apply -title "apply" -script "test_mode set_parameters 0 null"
create xbutton cancel -title "cancel" -script "xhide /x_interface/fb_ff_syn"

create xform /x_interface/pyr_fb_syn [345,445,260,140] 
ce ^
create xlabel label -title "pyr to fb synapse" -bg pink
create xlabel weights -title "weight" -bg pink
create xdialog wt_pyr_fb -title "pyr_to_fb" -value "1.0"
create xbutton apply -title "apply" -script "test_mode set_parameters 0 null"
create xbutton cancel -title "cancel" -script "xhide /x_interface/pyr_fb_syn"

create xform /x_interface/Aff_fb_syn [325,415,260,140] 
ce ^
create xlabel label -title "bulb to fb synapse" -bg pink
create xlabel weights -title "weight" -bg pink
create xdialog wt_Aff_fb -title "Aff_to_fb" -value "1.0"
create xbutton apply -title "apply" -script "test_mode set_parameters 0 null"
create xbutton cancel -title "cancel" -script "xhide /x_interface/Aff_fb_syn"


// Make forms associated with network viewing along z-axis

// Form associated with layer Ia

create xform /x_interface/Ia_network [320,220,220,168]
ce ^
create xlabel label -title "Ia_network" -bg pink
create xbutton Vm -title "Vm" -script "test_mode net_view 1 Ia_Vm"
create xbutton Im -title "Im" -script "test_mode net_view 1 Ia_Im"
create xbutton gNa -title "synaptic conductance (Na)" -script "test_mode net_view 1 pyr_gAff"
create xbutton gK -title "synaptic conductance (K)" -script "test_mode net_view 1 pyr_gFF"
create xbutton quit -title "cancel" -script "xhide /x_interface/Ia_network"

// Forms associated with supIb, deepIb, III

create xform /x_interface/supIb_network [320,295,220,138]
ce ^ 
create xlabel label -title "supIb_network" -bg pink
create xbutton Vm -title "Vm" -script "test_mode net_view 1 supIb_Vm"
create xbutton Im -title "Im" -script "test_mode net_view 1 supIb_Im"
create xbutton gchan -title "synaptic conductance (Na)" -script "test_mode net_view 1 pyr_gCA"
create xbutton quit -title "cancel" -script "xhide /x_interface/supIb_network"

create xform /x_interface/deepIb_network [320,369,220,138]
ce ^ 
create xlabel label -title "deepIb_network" -bg pink
create xbutton Vm -title "Vm" -script "test_mode net_view 1 deepIb_Vm"
create xbutton Im -title "Im" -script "test_mode net_view 1 deepIb_Im"
create xbutton gchan -title "synaptic conductance (Na)" -script "test_mode net_view 1 pyr_gRA"
create xbutton quit -title "cancel" -script "xhide /x_interface/deepIb_network"

create xform /x_interface/III_network [320,520,220,138]
ce ^ 
create xlabel label -title "III_network" -bg pink
create xbutton Vm -title "Vm" -script "test_mode net_view 1 III_Vm"
create xbutton Im -title "Im" -script "test_mode net_view 1 III_Im"
create xbutton gchan -title "synaptic conductance (Na)" -script "test_mode net_view 1 pyr_gLocal"
create xbutton quit -title "cancel" -script "xhide /x_interface/III_network"

// Form associated with soma 

create xform /x_interface/soma_network [320,443,220,138]
ce ^ 
create xlabel label -title "soma_network" -bg pink
create xbutton Vm -title "Vm" -script "test_mode net_view 1 soma_Vm"
create xbutton Im -title "Im" -script "test_mode net_view 1 soma_Im"
create xbutton gchan -title "synaptic conductance (Cl)" -script "test_mode net_view 1 pyr_gFB"
create xbutton quit -title "cancel" -script "xhide /x_interface/soma_network"

// Forms associated with inhibitory interneurons

create xform /x_interface/ff_network [57,106,220,114]
ce ^
create xlabel label -title "ff_network" -bg "pink"
create xbutton Vm -title "Vm" -script "test_mode net_view 1 ff_Vm"
create xbutton gchan -title "synaptic conductance (Na)" -script "test_mode net_view 1 ff_g"
create xbutton quit -title "cancel" -script "xhide /x_interface/ff_network"

create xform /x_interface/fb_network [334,442,220,114]
ce ^
create xlabel label -title "fb_network" -bg "pink"
create xbutton Vm -title "Vm" -script "test_mode net_view 1 fb_Vm"
create xbutton gchan -title "synaptic conductance (Na)" -script "test_mode net_view 1 fb_g"
create xbutton quit -title "cancel" -script "xhide /x_interface/fb_network"

// Make form associated with network viewing along y-axis

create xform /x_interface/full_cell_view [230,174,300,145]
ce ^
create xlabel label -title " full cell view " -bg pink
create xdialog parameter -title "parameter (Vm,Im,Gk)" -value "Vm"
create xdialog row -title "row" -value "5"
create xbutton display -title "display" -script "test_mode side_view 1 null"
create xbutton quit -title "cancel" -script "xhide /x_interface/full_cell_view"

// Forms associated with diffreent layer options

create xform /x_interface/Ia_options [175,125,250,139]
ce ^
create xlabel label -title " Ia_options " -bg pink
create xbutton network_view -title "network view"  \
	-script "xshow /x_interface/Ia_network"
create xbutton conductance_Na -title "total synaptic conductance (Na)" 	\
	-script "test_mode conduct_view 1 pyr_gAff"
create xbutton conductance_K -title "total synaptic conductance (K)" 	\
	-script "test_mode conduct_view 1 pyr_gFF"
create xbutton quit -title "cancel" -script "xhide /x_interface/Ia_options"

create xform /x_interface/supIb_options [175,200,250,114]
ce ^
create xlabel label -title " supIb_options " -bg pink
create xbutton network_view -title "network view" \ 
	-script "xshow /x_interface/supIb_network"
create xbutton Na_conductance -title "total synaptic conductance (Na)" 	\
	-script "test_mode conduct_view 1 pyr_gCA"
create xbutton quit -title "cancel" -script "xhide /x_interface/supIb_options"

create xform /x_interface/deepIb_options [175,275,250,114]
ce ^
create xlabel label -title " deepIb_options " -bg pink
create xbutton network_view -title "network view" \ 
	-script "xshow /x_interface/deepIb_network"
create xbutton Na_conductance -title "total synaptic conductance (Na)"     \ 	
	-script "test_mode conduct_view 1 pyr_gRA"
create xbutton quit -title "cancel" -script "xhide /x_interface/deepIb_options"

create xform /x_interface/soma_options [175,350,250,114]
ce ^
create xlabel label -title " soma_options " -bg pink
create xbutton network_view -title "network view" \ 
	-script "xshow /x_interface/soma_network"
create xbutton Cl_conductance -title "total synaptic conductance (Cl)" 	\
	-script "test_mode conduct_view 1 pyr_gFB"
create xbutton quit -title "cancel" -script "xhide /x_interface/soma_options"

create xform /x_interface/III_options [175,425,250,114]
ce ^
create xlabel label -title " III_options " -bg pink
create xbutton network_view -title "network view" \ 
	-script "xshow /x_interface/III_network"
create xbutton Na_conductance -title "total synaptic conductance (Na)" 	\
	-script "test_mode conduct_view 1 pyr_gLocal"
create xbutton quit -title "cancel" -script "xhide /x_interface/III_options"


// Make form associated with field potential viewing

create xform /x_interface/field_pot [230,125,300,305] 
ce ^
create xlabel form_label -title "field potentials" -bg pink
create xlabel synaptic_label -title "fp's generated by synaptic currents" -bg pink
create xtoggle Ia_Na -title "Ia_Na"
create xtoggle Ia_K -title "Ia_K"
create xtoggle supIb -title "supIb" 
create xtoggle deepIb -title "deepIb" 
create xtoggle soma -title "soma" 
create xtoggle III -title "III"
create xbutton display -title "display" -script "test_mode field_view 1 null"
create xlabel eeg -title "eeg display" -bg pink
create xbutton full_eeg -title "eeg" -script "test_mode field_view 2 null" 
create xbutton quit -title "cancel" -script "xhide /x_interface/field_pot"


// Make form containing info about spike count accross network

create xform /x_interface/spike [230,150,300,161]
ce ^
create xlabel form_label -title "spike count" -bg pink
create xlabel cell_type -title "cell type" -bg pink
create xbutton pyr -title "pyramidal" -script "test_mode spike_view 1 pyr"
create xbutton fb -title "feedback inhibitory" -script "test_mode spike_view 1 fb"
create xbutton ff -title "feedforward inhibitory" -script "test_mode spike_view 1 ff"
create xbutton cancel -title "cancel" -script "xhide /x_interface/spike"


// Make form containing info about playback viewing

create xform /x_interface/playback [150,400,300,280]
ce ^
create xlabel label -title "playback" -bg pink
create xdialog clock -title "clock (msec)" \
	-value {{getstat -time} + .1}
get_current_time
disable /x_interface/playback/clock
create xdialog step_size -title "playback step size (msec)" -value 2.0
create xdialog step_num -title "playback duration (msec)" -value "500" 
create xdialog dir -title "data directory" -value "default"
create xbutton reset -title "reset" -script "reset"
create xbutton run -title "run" -script run_playback
create xbutton step -title "step" -script "step"
create xbutton stop -title "stop" -script "stop"
create xbutton cancel -title "cancel" -script "xhide /x_interface/playback"


// Make form containing help window

create xform /x_interface/help [540,50,600,750]
ce ^
create xlabel label1 [1%,0%,98%,3%] -bg pink -title "help"
create xlabel label2 [1%,3%,98%,3%] -bg pink -title "Use the scroll bar on the left to move through the text."
create xtext text [1%,55,98%,83%] -bg white -filename piriform.help 
create xbutton legend [1%,92%,98%,4%] -title "legend" -script "xshow /menu/legend"
create xbutton cancel [1%,96%,98%,4%] -title "cancel" -script "xhide /x_interface/help"


// Make form containing quit options

create xform /x_interface/quit [400,600,300,120] 
ce ^
create xlabel label [1%,1%,98%,23%] -title "Do you wish to quit?" -bg pink
create xbutton yes [1%,25%,98%,23%] -title "yes" -script "quit"
create xbutton stay [1%,50%,98%,23%] -title "quit without leaving GENESIS"  \
       -script "deleteall -force"
create xbutton no [1%,75%,98%,23%] -title "no" -script "xhide /x_interface/quit"

