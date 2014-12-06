//genesis

include X1compat
include xstartup 

include xout_funcs 
include xlib 
include xgeom 
include xgraph_funcs 
include xcompt_funcs 
include xchannel_funcs 
include xcell_funcs 
include xselect 
include xrun 
include xtitle 
include xicons 

do_title_bar
create_lib_infrastructure
create_lib compt_lib
str name
foreach name ({el /library/#})
	copy_to_lib compt_lib {name} 0
end

do_xselect_funcs
do_xcompt_funcs
do_xcell_funcs
do_xchannel_funcs
do_xcell_run_control

xshow /title_bar
