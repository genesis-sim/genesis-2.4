create xform /graph1 [10,20,600,400]
create xgraph /graph1/ca -title "cal" -hgeom 50% -wgeom 50%
setfield ^ XUnits Sec YUnits mM xmax 0.050 ymin 0.05e-3 ymax 4e-3
create xgraph /graph1/buf -title "buf" -hgeom 50% -wgeom 50%
setfield ^ XUnits Sec YUnits mM xmax  0.050 ymin 5e-3 ymax 0.14
create xgraph /graph1/xIP3 -title "xIP3" -xgeom 300 -ygeom 0 -hgeom 50% -wgeom 50%
setfield ^ XUnits Sec ymax 1.0 ymin 0 xmax 0.050
create xgraph /graph1/x2 -title "x-noIP3" -xgeom 300 -hgeom 50% -wgeom 50%
setfield ^ XUnits Sec  xmax 0.05 ymax 1.0 ymin 0

addmsg somaCa /graph1/ca PLOT Conc *somaca *blue 
addmsg somaCabuf /graph1/buf PLOT Conc *somaCabuf *blue 

addmsg dendCa /graph1/ca PLOT Conc *dendca *black
addmsg dendCabuf /graph1/buf PLOT Conc *dendCabuf *black

addmsg ERCa /graph1/buf PLOT Conc *ERca *red

addmsg ERCa/x100 /graph1/xIP3 PLOT fraction *x100 *black
addmsg ERCa/x110 /graph1/xIP3 PLOT fraction *x110 *blue
addmsg ERCa/x111 /graph1/xIP3 PLOT fraction *x111 *red
addmsg ERCa/x101 /graph1/xIP3 PLOT fraction *x101 *white

addmsg ERCa/x000 /graph1/x2 PLOT fraction *x000 *black
addmsg ERCa/x010 /graph1/x2 PLOT fraction *x010 *blue
addmsg ERCa/x011 /graph1/x2 PLOT fraction *x011 *red
addmsg ERCa/x001 /graph1/x2 PLOT fraction *x001 *white

xshow /graph1
useclock /graph1/# 1
