//genesis
/* Set up the connectivity that the tree will look at */
create table /tab
call /tab TABCREATE 1 0 1
setfield /tab \	
	stepsize 1 \
	step_mode 0 \
	table->table[0] 0 \
	table->table[1] 1

addobject tabobj /tab

createmap tabobj /manytabs {side} {side} -object
gen3dmsg /manytabs/tab[] /manytabs/tab[] 0.5 1 INPUT output -offset -1 0 0
gen3dmsg /manytabs/tab[] /manytabs/tab[] 0.5 1 INPUT output -offset 9 -1 0
call /manytabs/tab[99] TABCREATE 100 0 1 
setfield /manytabs/tab[99] \
	stepsize 0.01 \
	step_mode 1

int i
for( i = 0 ; i < 100 ; i = i + 1)
	setfield /manytabs/tab[99] table->table[{i}] {{rand 0 100}/100.0}
end
