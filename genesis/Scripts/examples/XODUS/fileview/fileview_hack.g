//genesis
//
// This example demonstrates how xview can be used in combination
// with disk_in to read in data
//

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

create disk_in /diskin
	setfield /diskin \
		filename testfile \
		fileformat	1 \
		leave_open 1

	call /diskin RESET

nx = {getfield /diskin nx}
ny = {getfield /diskin ny}

pushe /diskin
k = 0
for (i = 0; i < nx; i = i + 1)
	for (j = 0; j < ny; j = j + 1)
		addmsg /diskin /form/draw/view VAL1 val[{i}][{j}]
		addmsg /diskin /form/draw/view COORDS \
			*{getfield xpts->table[{k}]} \
			*{getfield ypts->table[{k}]} \
			*0
		k = k + 1
	end
end
pope
reset
