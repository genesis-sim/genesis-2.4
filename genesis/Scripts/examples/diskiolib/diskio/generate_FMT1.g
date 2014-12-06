//genesis
//
// This example demonstrates how xview can use the fields
// 'path', 'field' and 'field2' to display a combination of message
// values and field values. Note how the
// color of the boxes (displaying msgin) in the view display is
// related to the size of the boxes (displaying the 'output' field
// of the tables) preceding them.
//

// Uses diskio to store data in FMT1 binary format 

int side = 10

xcolorscale rainbow2
create xform /form [0,0,500,550]
xshow /form
create xdraw /form/draw [0,0,100%,500]
	setfield /form/draw xmin -1 xmax 11 ymin -1 ymax 11
create xview /form/draw/view

create xbutton /form/run -wgeom 50% -script "step 100"
create xbutton /form/quit -ygeom 0:draw -xgeom 50% -wgeom 50% \
	-script quit

/* Set up the connectivity that the view will look at */
create table /tab
call /tab TABCREATE 1 0 1
setfield /tab \	
	stepsize 1 \
	step_mode 0 \
	table->table[0] 0 \
	table->table[1] 1

addobject tabobj /tab

createmap tabobj /manytabs {side} {side} -object

gen3dmsg /manytabs/tab[] /manytabs/tab[] 0.5 1 INPUT output -offset -1.1 0 0
gen3dmsg /manytabs/tab[] /manytabs/tab[] 0.5 1 INPUT output -offset 9 -1 0

call /manytabs/tab[99] TABCREATE 100 0 1 
setfield /manytabs/tab[99] \
	stepsize 0.01 \
	step_mode 1

int i
for(i = 0 ; i < 100 ; i = i + 1)
	setfield /manytabs/tab[99] table->table[{i}] {{rand 0 100 }/100.0}
end

setfield /form/draw/view \
	 path /manytabs/tab[] \
	 field msgin \
	 field2 output \
	 morph_val 2 \
	 relpath "" \
	 msgpath "" \
	 msgtype INPUT \
	 msgslotno 0

create diskio /dout -accessmode w 
	setfield ^ filename testfile \
		leave_open 1 append 1 flush 1

 addmsg /manytabs/tab[] /dout SAVE output
	
reset
