//genesis
//
// This example demonstrates how xview can be used in combination
// with diskio to read in data - To be used in conjunction with 
// generate_FMT1.g or generate_netcdf.g to produce the file "testfile"

int nx, ny
int i,j,k

xcolorscale rainbow2
create xform /form [0,0,500,570]
xshow /form
create xdraw /form/draw [0,0,100%,500]
	setfield /form/draw xmin -1 xmax 11 ymin -1 ymax 11
create xview /form/draw/view

create xbutton /form/run -wgeom 50% -script "step 100"
create xbutton /form/quit -ygeom 0:draw -xgeom 50% -wgeom 50% \
	-script quit
create xbutton /form/reset -script reset

/* Set up the connectivity that the view will look at */

create diskio /diskin -filename testfile
setfield /diskin leave_open 1

/* 
RESET needs to be called here for allocating and filling the 
interpol arrays with data
*/

call /diskin RESET

nx = {getfield /diskin nx}
ny = {getfield /diskin ny}

echo "nx = " {nx}
echo "ny = " {ny}

pushe /diskin
k = 0
for (i = 0; i < nx; i = i + 1)
	for (j = 0; j < ny; j = j + 1)
		addmsg /diskin /form/draw/view VAL1 val[{i}][{j}]
		k = k + 1
	end
end
pope

reset

setfield /form/draw/view xpts /diskin/xpts
setfield /form/draw/view ypts /diskin/ypts
setfield /form/draw/view zpts /diskin/zpts
