//genesis

setclock 0 1e-5
setclock 1 1e-4
create compartment /compt
	setfield ^ Rm 1e9 Cm 10e-12 Ra 1e6 Em -0.07

create synchan /compt/syn
	setfield ^ Ek 0.05 tau1 0.01 tau2 0.02 gmax 2e-8
	addmsg /compt/syn /compt CHANNEL Gk Ek
	addmsg /compt /compt/syn VOLTAGE Vm

create table /compt/syn/table
	call /compt/syn/table TABCREATE 10 -0.1 0.1
	setfield /compt/syn/table table->table[0] 0 table->table[1] 0.02 \
	     table->table[2] 0.05 table->table[3] 0.1  \
	    table->table[4] 0.2 table->table[5] 0.5 table->table[6] 0.8  \
	    table->table[7] 0.9 table->table[8] 0.95  \
	    table->table[9] 0.98 table->table[10] 1
	call /compt/syn/table TABFILL 1000 0
	setfield /compt/syn/table x -0.07
	addmsg /compt/syn/table /compt/syn/table INPUT x
	addmsg /compt/syn/table /compt/syn ACTIVATION output


create xform /form [0,0,600,400]
create xgraph /form/graph [0,0,100%,100%] -range [0,-0.1,1.0,0.05]
useclock /form/graph 1
xshow /form

addmsg /compt /form/graph PLOT Vm *Vm *blue
reset
echo "Using a synchan with input from a table ..."
step 0.1 -time
setfield /compt/syn/table x -0.03
step 0.1 -time
setfield /compt/syn/table x 0.0
step 0.1 -time
setfield /compt/syn/table x 0.03
step 0.1 -time
setfield /compt/syn/table x 0.05
step 0.1 -time
setfield /compt/syn/table x 0.1
step 0.1 -time
setfield /compt/syn/table x -0.1
step 0.4 -time
setfield /compt/syn/table x -0.07

// now get rid of the snchan and do it with a ddsyn
delete /compt/syn
create ddsyn /compt/ddsyn
	setfield ^ Ek 0.05 tau1 0.01 tau2 0.02 gmax 2e-8
	addmsg /compt/ddsyn /compt CHANNEL Gk Ek
	addmsg /compt /compt/ddsyn VOLTAGE Vm

	call /compt/ddsyn TABCREATE 10 -0.1 0.1
	setfield /compt/ddsyn transf->table[0] 0 transf->table[1] 0.02  \
	    transf->table[2] 0.05 transf->table[3] 0.1  \
	    transf->table[4] 0.2 transf->table[5] 0.5  \
	    transf->table[6] 0.8 transf->table[7] 0.9  \
	    transf->table[8] 0.95 transf->table[9] 0.98  \
	    transf->table[10] 1
	call /compt/ddsyn TABFILL 1000 0
	setfield /compt/ddsyn x -0.07
	addmsg /compt/ddsyn /compt/ddsyn V_PRESYN x

setfield /form/graph overlay 1
reset
//change the plot color of the new plot
setfield /form/graph/Vm fg red
echo Now using a ddsyn ...
step 0.1 -time
setfield /compt/ddsyn x -0.03
step 0.1 -time
setfield /compt/ddsyn x 0.0
step 0.1 -time
setfield /compt/ddsyn x 0.03
step 0.1 -time
setfield /compt/ddsyn x 0.05
step 0.1 -time
setfield /compt/ddsyn x 0.1
step 0.1 -time
setfield /compt/ddsyn x -0.1
step 0.4 -time

