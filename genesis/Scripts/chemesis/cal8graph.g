//chemesis2.0
//cal8graph.g
create xform /graph1 [10,20,600,400]
create xgraph /graph1/ca -title "calcium" -hgeom 50% -wgeom 50%
setfield ^ XUnits Sec YUnits uM xmax 0.2 ymin 0.05 ymax 0.15
create xgraph /graph1/buf -title "cabuf" -hgeom 50% -wgeom 50%
setfield ^ XUnits Sec YUnits uM xmax  0.2 ymin 10 ymax 15.0
create xgraph /graph1/vm -title "potential" -hgeom 50% -wgeom 50%  -xgeom 300 -ygeom 0
setfield ^ XUnits Sec YUnits mV xmax 0.2 ymin -100 ymax 0
create xgraph /graph1/current -title "current" -hgeom 50% -wgeom 50%  -xgeom 300 -ygeom 200
setfield ^ XUnits Sec YUnits nA xmax 0.2 ymin -0.01e-6 ymax 0.1e-6

addmsg somaCa[1] /graph1/ca PLOTSCALE Conc *somaca *blue 1e3 0
addmsg somaCabuf[1] /graph1/buf PLOTSCALE Conc *somaCabuf *blue 1e3 0

addmsg dendCa[1] /graph1/ca PLOTSCALE Conc *dendca *black 1e3 0
addmsg dendCabuf[1] /graph1/buf PLOTSCALE Conc *dendCabuf *black 1e3 0

addmsg soma /graph1/vm PLOTSCALE Vm *soma *red 1e3 0
addmsg dendrite /graph1/vm PLOTSCALE Vm *dend *blue 1e3 0

addmsg somaCa[1]/CaT /graph1/current PLOTSCALE Ik *CaT *blue 1e9 0
addmsg somaCa[1]/CaT_ghk /graph1/current PLOTSCALE Ik *CaT_ghk *red 1e3 0
addmsg somaCa[1]/CaT /graph1/current PLOTSCALE Gk *GCaT *black 1e9 0




xshow /graph1
useclock /graph1/# 1
