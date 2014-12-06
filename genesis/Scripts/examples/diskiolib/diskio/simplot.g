//genesis


create xform /f
create xgraph /f/g
setfield /f/g ymax 1.0
create xbutton /f/QUIT -script quit
xshow /f


 xsimplot /f/g testfile -cell 45 -skip 20 -color blue -overlay 1
 xsimplot /f/g testfile -cell 99
 xsimplot /f/g testfile -cell 99 -skip 10 -color red -overlay 1 
 xsimplot /f/g testfile -cell 99 -skip 5 -color yellow -overlay 2 -offset 0.05
