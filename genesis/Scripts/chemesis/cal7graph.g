//chemesis2.0
//cal7graph.g
create xform /graph1 [10,20,600,400]
create xgraph /graph1/ca -title "calcium" -hgeom 50% -wgeom 50%
setfield ^ XUnits Sec YUnits uM xmax 0.2 ymin 0.05 ymax 0.15
create xgraph /graph1/buf -title "cabuf" -hgeom 50% -wgeom 50%
setfield ^ XUnits Sec YUnits uM xmax  0.2 ymin 10 ymax 15.0
create xgraph /graph1/vm -title "potential" -hgeom 50% -wgeom 50%  -xgeom 300 -ygeom 0
setfield ^ XUnits Sec YUnits mV xmax 0.2 ymin -100 ymax 0
create xgraph /graph1/current -title "current" -hgeom 50% -wgeom 50%  -xgeom 300 -ygeom 200
setfield ^ XUnits Sec YUnits nA xmax 0.2 ymin -0.01e-6 ymax 0.1e-6

if ({CHEMESIS})
   addmsg somaCa /graph1/ca PLOTSCALE Conc *somaca *blue 1e3 0
   addmsg somaCabuf /graph1/buf PLOTSCALE Conc *somaCabuf *blue 1e3 0

   addmsg dendCa /graph1/ca PLOTSCALE Conc *dendca *black 1e3 0
   addmsg dendCabuf /graph1/buf PLOTSCALE Conc *dendCabuf *black 1e3 0
else
   addmsg somaCa /graph1/ca PLOTSCALE C *somaca *blue 1e3 0
   addmsg somaCabuf /graph1/buf PLOTSCALE Bbound *somaCabuf *blue 1e3 0

   addmsg dendCa /graph1/ca PLOTSCALE C *dendca *black 1e3 0
   addmsg dendCabuf /graph1/buf PLOTSCALE Bbound *dendCabuf *black 1e3 0
end

addmsg soma /graph1/vm PLOTSCALE Vm *soma *red 1e3 0
addmsg dendrite /graph1/vm PLOTSCALE Vm *dend *blue 1e3 0

addmsg somaCa/CaT /graph1/current PLOTSCALE Ik *CaT *blue 1e9 0
addmsg somaCa/CaT_ghk /graph1/current PLOTSCALE Ik *CaT_ghk *red 1e3 0
addmsg somaCa/CaT /graph1/current PLOTSCALE Gk *GCaT *black 1e9 0

xshow /graph1
useclock /graph1/# 1
