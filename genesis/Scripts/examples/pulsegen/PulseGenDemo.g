//genesis - demonstration of pulsegen object

create pulsegen /pulse0
setfield ^ level1 50.0 width1 3.0 delay1 5.0 level2 -20.0 width2 5.0  \
    delay2 8.0 baselevel 10.0 trig_mode 0

create pulsegen /trig
setfield ^ level1 20.0 width1 1.0 delay1 5.0 width2 30.0

create pulsegen /pulse1
setfield ^ level1 50.0 width1 3.0 delay1 5.0 level2 -20.0 width2 5.0  \
    delay2 8.0 baselevel 10.0 trig_mode 1

addmsg /trig /pulse1 INPUT output

create pulsegen /gate
setfield ^ level1 20.0 width1 30.0 delay1 15.0 width2 30.0

create pulsegen /pulse2
setfield ^ level1 50.0 width1 3.0 delay1 5.0 level2 -20.0 width2 5.0  \
    delay2 8.0 baselevel 10.0 trig_mode 2

addmsg /gate /pulse2 INPUT output


/*
** graphics
*/
create xform /PGform [200,100,600,700] -title "Pulse Generator Demo"

create xgraph /PGform/graph0 [10,50,580,200] -title "free run mode"
setfield ^ ymin -100 ymax 100
addmsg /pulse0 /PGform/graph0 PLOT output *pulse *blue

create xgraph /PGform/graph1 [10,260,580,200] -title  \
    "triggered mode (with delay)"
setfield ^ ymin -100 ymax 100
addmsg /trig /PGform/graph1 PLOT output *trig *red
addmsg /pulse1 /PGform/graph1 PLOT output *pulse *blue

create xgraph /PGform/graph2 [10,470,580,200] -title  \
    "gated mode (leading edge triggers)"
setfield ^ ymin -100 ymax 100
addmsg /gate /PGform/graph2 PLOT output *gate *red
addmsg /pulse2 /PGform/graph2 PLOT output *pulse *blue

reset
setclock 0 0.5
step 200

xshow PGform
