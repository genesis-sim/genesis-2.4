//chemesis2.0
//Cal8.g

include const.g
/* calcium specific parameters */
int difshell=0
int difcyl=1

// HOW IS DIFFUSION AFFECTED BY COMPARTMENT SIZE - change dendcomps
int dendcomps=6
int i
//HOW IS DIFFUSION AFFECTED BY HAVING SOMA SHELLS?
int somashells=6

//parameters for electrical aspects
//all units are SI 
float RA = 1  /* 100 ohm-cm, could be 200 */
float RM = 1    /* 1e4 ohms - cm^2; 5e-2 from Spruston et al */
float CM = 1e-2     /* 1e-6 Farads/cm^2; from Spruston et al. */
//currents through GHK need to be ~1e6x smaller than without GHK
float PCaT=20.0e-6

//function to create rxnpools of calcium, buffer and bimolecular reaction
include cabufdiff.g
//function to create compartment
include compSI.g
//function to create calcium channel
include icaTab.g

//----------------start the model here------------------------
//neutral element which acts as directory, to organize elements
create neutral cell
ce cell

//invoke function to create rxnpools and reaction
float height={somarad/somashells}
float outerrad, innerrad
outerrad=somarad
innerrad=somarad-height
for (i=1; i<=somashells; i=i+1)
    echo "i, outer, inner" {i} {outerrad} {innerrad}
    cabuf soma {CaInit} {CaBufInit} {BufTot} {outerrad} {somalen} {lengthunits} {quantUnits} {i} {innerrad} 
    outerrad=innerrad
    innerrad=innerrad-height
    if (i == {somashells-1})
        innerrad=0.0
    end
end 
float seglen={dendlen/dendcomps} 
for (i=1; i<=dendcomps; i=i+1)
    cabuf dend {CaInit} {CaBufInit} {BufTot} {dendrad} {seglen} {lengthunits} {quantUnits} {i} 0
end

//create diffusion element which allows movement of Ca between compartments.
// diffusion between shells uses different surface area than for dendrite
for (i=1; i<somashells; i=i+1)
    Diffusion somaCa[{i}] somaCa[{i+1}] {DiffConst} somadiff[{i}] {difshell} {lengthunits} {quantUnits}  
end

Diffusion somaCa[{somashells}] dendCa[1] {DiffConst} somadend {difcyl} {lengthunits} {quantUnits}  

for (i=1; i<dendcomps; i=i+1)
    Diffusion dendCa[{i}] dendCa[{i+1}] {DiffConst} denddiff[{i}] {difcyl} {lengthunits} {quantUnits}  
end

create conservepool caext
setfield caext Ctot 2 volume 0.001 Cinit 2

//*********create electrical cell compartment***********
make_comp soma {somalen} {somarad}
make_comp dendrite {dendlen} {dendrad}
addmsg dendrite soma RAXIAL Ra previous_state
addmsg soma dendrite AXIAL  previous_state

//add calcium channels, use SI units else ghk won't work
//consider making this channel in the library and copy into both compartments
   float Actvhalf=-20e-3
   float Actslope=-10e-3
   float Acttau=5.0e-3
   float Inactvhalf=-49e-3
   float Inactslope=5e-3
   float Inacttau=75e-3
   make_ica_ghk soma somaCa[1] caext "CaT" {PCaT} {Actvhalf} {Actslope} {Acttau} {Inactvhalf} {Inactslope} {Inacttau}
// Iunits of 1 indicates Amps, but -1 should convert from modeler to physiologists convention
   setfield somaCa[1] Iunits -1
    for (i=1; i<=dendcomps; i=i+1)
        setfield dendCa[{i}] Iunits -1
    end
//setup graphs to display results
include cal8graph.g

//again, use a larger dt for output
setclock 0 0.25e-6
setclock 1 0.1e-3

reset
step 100000
setfield soma inject 0.1e-9
step 10000
showmsg /graph1/ca
setfield soma inject 0
step 40000
showmsg /graph1/ca
step 50000
step 800000
setfield /graph1/# xmax 0.5
step 1000000
setfield /graph1/# xmax 2.5
step 2 -t

/*Notice that calcium and calcium bound buffer continues to increase
 in the soma.  There are no pumps in this model, thus nothing to restore
the total calcium to its original level.  
Also, though the channel inactivates, it is not completely inactivated.
No potassium currents restore membrane potential to rest.
Some calcium continues to flow in.*/
