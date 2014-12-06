//genesis
/*
** This example illustrates the use of the disk_in element when 
** reading ascii files (the element can also read FMT1 files).
** The two example files foo1 and foo2 are displayed in a 6x6x6 cube.
** Although they contain the same values, the effect of line breaks
** is illustrated by foo2. The purpose of this feature is to force
** alignment of input data. The table elements are there to transform
** the input data in any desired way, in this case they just send on
** the input data unchanged.
*/

function setfilename
	setfield /in filename {getfield /form/filename value}
end

echo starting
int i
int j
int k

create disk_in /in
	setfield /in nx 36 ny 6 loop 1 filename foo1 dt 1 leave_open 1
create table /proto/table
call /proto/table TABCREATE 100 0 100
for (i = 0; i <= 100; i = i + 1)
	setfield /proto/table table->table[{i}] {i}
end

create xform /form
setfield ^ wgeom 800 hgeom 700
create xdraw /form/draw
setfield ^ wgeom 800 hgeom 550 wx 10 wy 22 cx 3 cy 3 cz 6  \
    transform ortho3d vx 4 vy -19 vz 7
create xdialog /form/filename -script setfilename
setfield /form/filename value foo1
create xbutton /form/reset -script reset
create xbutton /form/step -script step
create xbutton /form/quit -script quit

for (i = 0; i < 6; i = i + 1)
	createmap /proto/table /transf{i} 6 6
	position transf{i} 0 0 {i*3}
	create xview /form/draw/v[{i}]
	setfield /form/draw/v[{i}] path /transf{i}/table[] field output  \
	    value_min 0 value_max 100 autoscale FALSE
	for (j = 0; j < 6; j = j + 1)
		for (k = 0; k < 6; k = k + 1)
			addmsg /in /transf{i}/table[{j*6 + k}] INPUT  \
			    val[{j*6 + k}][{i}]
		end
	end
end

reset

xshow /form
