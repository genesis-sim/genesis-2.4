// genesis
include graphics_funcs.g 

include squid_funcs.g 
include squid_graphs.g 
include squid_electronics.g 
include squid_forms.g 
include squid_sched.g 
include phsplot.g // added for Squid_tut

include tut_forms.g // added for Squid_tut
include setcolors.g // functions to change the default colors

// start up Xodus

xcolorscale hot

//-------
// SETUP 
//-------
squid_cmpt /axon 500 500
add_squid_graphs /axon
add_squid_electronics
connect_squid_electronics /axon
add_squid_forms
// added for Squid_tut
add_squid_phaseplot
// added for Squid_tut
add_squid_popups
//-----------------
// GRAPH MESSAGES
//-----------------
addmsg /axon /axon/graphs/Vm PLOT Vm *Vm *red
addmsg /Vclamp /axon/graphs/Vm PLOT output *command *blue

addmsg /Iclamp /axon/graphs/inj PLOT output *Iclamp *red
addmsg /PID /axon/graphs/inj PLOT output *Vclamp *blue


addmsg /axon/Na /axon/graphs/Gk PLOT Gk *Na *blue
addmsg /axon/K /axon/graphs/Gk PLOT Gk *K *red

addmsg /axon/Na /axon/graphs/Ik PLOTSCALE Ik *Na *blue -1 0
addmsg /axon/K /axon/graphs/Ik PLOTSCALE Ik *K *red -1 0



//-----------------
// INITIALIZATION
//-----------------
upicolors  // set the widget colors to something bright
xshow /forms/control
//added by Ed Vigmond
xshow /forms/exconcen
ce /forms/control
call tstep B1DOWN
call dt B1DOWN
call clamp_mode B1DOWN
call reset B1DOWN
