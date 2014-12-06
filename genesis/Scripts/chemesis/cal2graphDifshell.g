//chemesis2.0
//cal2graphDifshell.g
float plotunits=1e3*ConcUnits
create xform /graph1 [10,20,600,400]
create xgraph /graph1/ca -title "cal" -hgeom 50% -wgeom 50%
setfield ^ XUnits Sec YUnits uM xmax 100e-3 ymin 0 ymax {0.001*{plotunits}}
create xgraph /graph1/buf -title "buf" -hgeom 50% -wgeom 50%
setfield ^ XUnits Sec YUnits uM xmax  100e-3 ymin {CaBufInit*{plotunits}*0.9} ymax {CaBufInit*{plotunits}*1.1}

addmsg somaCa /graph1/ca PLOTSCALE C *somaca *blue {plotunits} 0
addmsg somaCabuf /graph1/buf PLOTSCALE Bbound *somaCabuf *blue {plotunits} 0 

addmsg dendCa /graph1/ca PLOTSCALE C *dendca *black {plotunits} 0
addmsg dendCabuf /graph1/buf PLOTSCALE Bbound *dendCabuf *black {plotunits} 0

xshow /graph1
useclock /graph1/# 1
