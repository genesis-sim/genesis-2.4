//chemesis2.0
//cabufdiff.g
function cabuf (pool, Cainit, cabufinit, buftot, outerrad, len, Lengthunits,quantUnits, index, innerrad)
float outerrad, len,innerrad
float Cainit, cabufinit, buftot
float Lengthunits,quantUnits
str pool
int index

//units of 1 means concentration is in millimolar (SI units)

    float PI=3.14159
    float areaout=2*PI*outerrad*len
    float areain=2*PI*innerrad*len
    float areaside=PI*(outerrad*outerrad - innerrad*innerrad)
    float volume=areaside*len
//volume and length units are converted to meters using lengthunits

    /* create pools of molecules */
    create rxnpool {pool}Ca[{index}]
    setfield {pool}Ca[{index}] Cinit {Cainit} vol {volume} SAout {areaout} SAside {areaside} SAin {areain} radius {outerrad} len {len} units {quantUnits} Dunits {Lengthunits}
    create rxnpool {pool}Cabuf[{index}]
    setfield {pool}Cabuf[{index}] Cinit {cabufinit} vol {volume} SAout {areaout} SAside {areaside} SAin {areain} radius {outerrad} len {len} units {quantUnits} Dunits {Lengthunits}
    create conservepool {pool}buf[{index}]
    setfield {pool}buf[{index}] Ctot {buftot} volume {volume} Dunits {Lengthunits}

    /* create reaction */
//rate constants converted to sec and millimole units.
//this affinity is 7e-4 mM = 7e-1 uM = 0.7 uM = 700 nM
    create reaction {pool}cabufrxn[{index}]
    setfield {pool}cabufrxn[{index}] kf 0.028e6 kb 19.6

    /* messages from substrates and product to reaction */
    addmsg {pool}Ca[{index}] {pool}cabufrxn[{index}] SUBSTRATE Conc
    addmsg {pool}buf[{index}] {pool}cabufrxn[{index}] SUBSTRATE Conc
    addmsg {pool}Cabuf[{index}] {pool}cabufrxn[{index}]  PRODUCT Conc

    /* messages from reaction to substrates and products */
    addmsg {pool}cabufrxn[{index}] {pool}Ca[{index}]  RXN2 kbprod kfsubs
    addmsg {pool}cabufrxn[{index}] {pool}Cabuf[{index}] RXN2 kfsubs kbprod

    /* conserve pool messages */
    addmsg {pool}Cabuf[{index}] {pool}buf[{index}] CONC Conc

end

function Diffusion (pool1, pool2, diffconst, diffname, type, Lengthunits,quantUnits)
str pool1, pool2, diffname
float diffconst
float Lengthunits,quantUnits
int type

    create diffusion {diffname}
    setfield {diffname} D {diffconst} units {quantUnits} Dunits {lengthunits}
    if ({type} == 0)       //diffusion between shells
       addmsg {pool1} {diffname} POOL1 len SAin Conc
       addmsg {pool2} {diffname} POOL2 len SAout Conc
    elif ({type}==1)
       addmsg {pool1} {diffname} POOL1 len SAside Conc
       addmsg {pool2} {diffname} POOL2 len SAside Conc
    end
    addmsg {diffname} {pool1} RXN0MOLES difflux1
    addmsg {diffname} {pool2} RXN0MOLES difflux2
end
