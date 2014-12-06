function make_comp(compname, length, radius)
str compname
float length, radius
//This isn't really SI units, still using cm for size
float area = length*PI*2*radius
float xarea = PI*radius*radius

create compartment {compname}
setfield {compname} Rm {RM/area} Cm {CM*area} Ra {RA*somalen/xarea} Em -80e-3 inject 0  initVm -80.0e-3

echo {compname} "xarea=" {xarea} "volume=" {xarea*length}

end
