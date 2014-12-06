// genesis
//*************************************************************************
//Adam Franklin Strassberg
//March 15, 1992

//Simulation Messages
//*************************************************************************


//*************************************************************************
//Link Simulation Elements 

addmsg /patch /patch/Na VOLTAGE Vm
addmsg /patch /patch/K VOLTAGE Vm

addmsg /patch/Na /patch CHANNEL Gk Ek
addmsg /patch/K /patch CHANNEL Gk Ek

addmsg /pulse /patch INJECT output
//*************************************************************************


//*************************************************************************
//Link Graphic Elements 

addmsg /patch /graphs/Vm PLOT Vm *Vm *black

addmsg /pulse /graphs/inj PLOT output *Iclamp *black

addmsg /patch/Na /graphs/Gk PLOT Gk *Na *blue
addmsg /patch/K /graphs/Gk PLOT Gk *K *red

addmsg /patch/Na /graphs/Ik PLOTSCALE Ik *Na *blue -1 0
addmsg /patch/K /graphs/Ik PLOTSCALE Ik *K *red -1 0
//*************************************************************************

