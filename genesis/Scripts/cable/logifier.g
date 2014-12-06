//genesis - logifier.g - used to plot log(Vm)
/*********************************************************************** \
    ****
 As the GENESIS xgraph object doesn't have a way to plot with a logarithmic
 scale, we create a "logifier" object which uses a lookup table to calculate
 a natural log.  The log of a quantity can then be plotted with messages
 like:
	sendmsg /cell/soma /logtable INPUT Vm
	sendmsg /logtable loggraph   PLOT output *soma *black
************************************************************************ \
    ***/

function make_logtable
int xdivs = 5000
int xmax = 500
int i
float dx
float x
float y
// We may later want to scale and shift the plot
float SCALE = 1.0
// if the range includes x = 0.
float OFFSET = 0.0

create table /logtable
call logtable TABCREATE {xdivs} 0 {xmax}
//call table TABCREATE xdivs xmin xmax (mV)
dx = {getfield logtable table->dx}
// this will be 0.0
x = {getfield logtable table->xmin}
// kludge to set log(0) = log(dx)
y = SCALE*{log {dx + OFFSET}}
setfield logtable table->table[{0}] {y}
x = x + dx

    for (i = 1; i <= (xdivs); i = i + 1)
	// OFFSET needed if x < 0
	y = SCALE*{log {x + OFFSET}}
	setfield logtable table->table[{i}] {y}
	x = x + dx
    end
end

// By default the table uses linear interpolation, which is what we want.
// do it
make_logtable
