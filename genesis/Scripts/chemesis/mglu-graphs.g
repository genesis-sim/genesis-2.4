function graphs (pkc)
str pkc, cellpath
create xform /graph1 [10,20,600,400]

create xgraph /graph1/GPLC -title "PLC,IP3" -hgeom 50% -wgeom 50%
setfield ^ XUnits mSec YUnits mM xmax 2 ymin 0 ymax 5.1
create xgraph /graph1/mGlu -title "mGlu" -hgeom 50% -wgeom 50%
setfield ^ XUnits mSec YUnits nA xmax  1 ymin 0 ymax 2.5e6
create xgraph /graph1/IP3 -title "Gtot,PIP2tot" -xgeom 300 -ygeom 0 -hgeom 50% -wgeom 50%
setfield ^ XUnits mSec ymax 160 ymin 0 xmax 2
create xgraph /graph1/aG -title "aG and aPLC_aG" -xgeom 300 -hgeom 50% -wgeom 50%
setfield ^ XUnits mSec  xmax 2 ymax 0.06 ymin 0
xshow /graph1

addmsg {pkc}/IP3 /graph1/GPLC PLOT Conc *ip3 *black
addmsg {pkc}/PLCtot /graph1/GPLC PLOT Conc *plc *blue
addmsg {pkc}/pip2ip3 /graph1/GPLC PLOT complex_conc *aPLC_PIP2 *red

addmsg {pkc}/PIP2tot /graph1/IP3 PLOT Conc *pip2 *red
addmsg {pkc}/Gtot /graph1/IP3 PLOT Conc *G *black

addmsg {pkc}/Glu_mGluR /graph1/mGlu PLOT z *Glu_mGluR *blue

addmsg {pkc}/aG /graph1/aG PLOT Conc *aG *black
addmsg {pkc}/aPLC_aG /graph1/aG PLOT Conc *aPLC_aG *red

useclock /graph1/# 1
end
