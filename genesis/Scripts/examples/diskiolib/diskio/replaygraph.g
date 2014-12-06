//genesis

// To be used with generate_FMT1.g or generate_netcdf.g to produce "testfile


 int i,j, nx, ny

 create xform /f
 create xgraph /f/g
 setfield /f/g ymax 1.0 xmax 40.0

create xbutton /f/run -wgeom 50% -script "step 100"
create xbutton /f/quit -ygeom 0:g -xgeom 50% -wgeom 50% \
	-script quit
create xbutton /f/reset -script reset

 xshow /f

 create diskio /diskin -filename testfile -accessmode r 
	
// RESET needs to be called for the val array field to be allocated and filled 

 call /diskin RESET 

// Send PLOT message to the graph - In this case plot the output field of
// /manytabs/tab[99] && /manytabs/tab[65] only if there is data in the file 
// (from the generate.g script)

 ny = {getfield /diskin ny}

 if ({ny} > 99) 
	addmsg /diskin /f/g PLOT val[0][99] *val_99 *red
	addmsg /diskin /f/g PLOT val[0][65] *val_65 *blue
 end

 check

 reset
