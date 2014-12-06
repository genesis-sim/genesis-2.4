function makepump (cytpath, erpath, maxrate, expon, halfconc)
str cytpath, erpath
float maxrate, halfconc
int expon

    create mmpump2 {cytpath}/serca
    setfield {cytpath}/serca \ 
        power {expon} \ 
        half_conc {halfconc} \
        max_rate {maxrate} \
        units 1

    addmsg {cytpath} {cytpath}/serca CONC Conc
    addmsg {cytpath}/serca {cytpath} RXN0MOLES moles_out
    addmsg {cytpath}/serca {erpath} RXN0MOLES moles_in

end

/********************************************************************/

function makeleak(cytpath,erpath,maxrate,expon)
   str cytpath, erpath
   float maxrate
   int expon
   
   create cicrflux {cytpath}/leak
   setfield ^ \
      power {expon} \
      maxflux {maxrate} \
      units 1
      
addmsg {cytpath} {cytpath}/leak CONC1 Conc
addmsg {erpath} {cytpath}/leak CONC2 Conc
addmsg {cytpath} {cytpath}/leak IP3R 1
addmsg {cytpath}/leak {cytpath} RXN0MOLES deltaflux1
addmsg {cytpath}/leak {erpath} RXN0MOLES deltaflux2

end
/**************************************************************/

