//genesis
//
// This example demonstrates how xgraph can be used in combination
// with the variable object to plot data generated from generate.g
//

int ny

xcolorscale rainbow2
create xform /form [0,0,500,570]
xshow /form
create xgraph /form/graph
setfield /form/graph ymax 1.0

create xbutton /form/run -wgeom 50% -script "step 100"
create xbutton /form/quit -ygeom 0:graph -xgeom 50% -wgeom 50% \
	-script quit
create xbutton /form/reset -script reset


/* Interface to the file and open it read-only with diskio */
create diskio /diskin -filename testfile.nc 

/* PLOT the output field of the variables */

/* 
   Use the el function etc., to find how many variables are available 
   This is equal to the framesize ny. I just set it here directly. 
*/
ny = 100

if ( {ny} > 99 )
	addmsg /diskin/outputvalue[99] /form/graph PLOT output *output_99 *red
	addmsg /diskin/outputvalue[65] /form/graph PLOT output *output_65 *blue
end

reset

