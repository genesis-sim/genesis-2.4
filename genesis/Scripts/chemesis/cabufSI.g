//chemesis2.0
//cabufSI.g
function cabuf (pool, Cainit, cabufinit, buftot, rad, len, Lengthunits,quantUnits)
float rad, len
float Cainit, cabufinit, buftot
float Lengthunits,quantUnits
str pool

//units of 1 means concentration is in millimolar (SI units)

    float PI=3.14159
    float areaout=2*PI*rad*len
    float areaside=PI*rad*rad
    float volume=areaside*len
//volume and length units are converted to meters using lengthunits

    /* create pools of molecules */
    create rxnpool {pool}Ca
    setfield {pool}Ca Cinit {Cainit} vol {volume} SAout {areaout} SAside {areaside} radius {rad} len {len} units {quantUnits} Dunits {Lengthunits}
    create rxnpool {pool}Cabuf
    setfield {pool}Cabuf Cinit {cabufinit} vol {volume} SAout {areaout} SAside {areaside} radius {rad} len {len} units {quantUnits} Dunits {Lengthunits}
    create conservepool {pool}buf
    setfield {pool}buf Ctot {buftot} volume {volume} Dunits {Lengthunits}

    /* create reaction */
//rate constants converted to sec and millimole units.
//this affinity is 7e-4 mM = 7e-1 uM = 0.7 uM = 700 nM
    create reaction {pool}cabufrxn
    setfield {pool}cabufrxn kf 0.028e6 kb 19.6

    /* messages from substrates and product to reaction */
    addmsg {pool}Ca {pool}cabufrxn SUBSTRATE Conc
    addmsg {pool}buf {pool}cabufrxn SUBSTRATE Conc
    addmsg {pool}Cabuf {pool}cabufrxn PRODUCT Conc

    /* messages from reaction to substrates and products */
    addmsg {pool}cabufrxn {pool}Ca RXN2 kbprod kfsubs
    addmsg {pool}cabufrxn {pool}Cabuf RXN2 kfsubs kbprod

    /* conserve pool messages */
    addmsg {pool}Cabuf {pool}buf CONC Conc

end

