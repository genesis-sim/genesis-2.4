create xform /graph1 [10,20,600,400]
create xgraph /graph1/ca -title "cyt cal conc" -hgeom 50% -wgeom 50%
setfield ^ XUnits Sec YUnits mM xmax 0.80 ymin 0.05e-3 ymax 4e-3
create xgraph /graph1/buf -title "buffer & ER cal conc" -hgeom 50% -wgeom 50%
setfield ^ XUnits Sec YUnits mM xmax  0.80 ymin 5e-3 ymax 0.14
create xgraph /graph1/x1 -title "fraction open channels" -xgeom 300 -ygeom 0 -hgeom 50% -wgeom 50%
setfield ^ XUnits mSec ymax 1.0 ymin 0 xmax 0.8
create xgraph /graph1/flux -title "flux, in molecules" -xgeom 300 -hgeom 50% -wgeom 50%
setfield ^ XUnits mSec  xmax 0.8 ymax 2e10 ymin 0

addmsg somaCa /graph1/ca PLOT Conc *somaca *blue 
addmsg somaCabuf /graph1/buf PLOT Conc *somaCabuf *blue 

addmsg dendCa /graph1/ca PLOT Conc *dendca *black
addmsg dendCabuf /graph1/buf PLOT Conc *dendCabuf *black
addmsg ERCa /graph1/buf PLOT Conc *ERca *red

addmsg ERCa/x100 /graph1/x1 PLOT fraction *x100 *black
addmsg ERCa/x110 /graph1/x1 PLOT fraction *x110 *blue
addmsg ERCa/x111 /graph1/x1 PLOT fraction *x111 *red

create diffamp x01
setfield x01 gain 1.0 saturation 1.0
addmsg ERCa/x010 x01 PLUS fraction
addmsg ERCa/x011 x01 PLUS fraction

addmsg x01 /graph1/x1 PLOT output *x010+x011 *white

addmsg somaCa/leak /graph1/flux PLOT deltaflux1 *leak *black
addmsg somaCa/iicrflux /graph1/flux PLOT deltaflux1 *iicr *red
addmsg somaCa/serca /graph1/flux PLOT moles_in *-serca *blue
addmsg somadend /graph1/flux PLOT difflux1 *diffusion *white

xshow /graph1
useclock /graph1/# 1
